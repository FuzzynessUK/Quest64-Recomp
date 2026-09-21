// q64music: converts music into Quest 64's sequence format, for the game's
// custom_music folder.
//
//   q64music <song.ootrs | song.seq | song.mid> [options]
//
// An .ootrs (a Darunia's Joy / OoT Randomizer music pack) is a zip holding
// the Ocarina of Time sequence bytecode and a .meta with the song's name,
// its soundfont and whether it is a bgm or a fanfare. The bytecode is run
// through an interpreter (oot_seq.cpp) and re-written as a compact
// sequence (cseq_writer.cpp); a bundled soundfont (.zbank/.zsound) cannot
// be carried across, since Quest 64 has only its own 29 instruments, so
// instruments are mapped - by a map file, or channel by channel with
// --prog - and that mapping is where a conversion is tuned by ear.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "miniz.h"

#include "cseq_writer.h"
#include "midi.h"
#include "oot_seq.h"

namespace fs = std::filesystem;

namespace {
    void usage() {
        std::puts(
            "q64music - convert music for Quest 64: Recompiled's custom_music folder\n"
            "\n"
            "  q64music <input>... [options]\n"
            "\n"
            "  input        .ootrs (Darunia's Joy / OoTR music pack), .seq/.zseq/.aseq\n"
            "               (OoT sequence bytecode), .mid (standard MIDI), or a folder:\n"
            "               every such file in it and its subfolders is converted\n"
            "  -o <file>    output .seq (default: the song's name, next to the input)\n"
            "  -d <folder>  output folder; for a folder input the default is the\n"
            "               custom_music folder beside this exe, when there is one\n"
            "  --prog c=p,... pin channel c (0-15) to Quest 64 program p (0-28; 9 = drums)\n"
            "  --inst f:i=p   map OoT soundfont f, instrument i, to program p\n"
            "  --map <file>   a map file: lines of \"font instrument program\" (hex or\n"
            "                 decimal); q64music.map beside the exe is read by default\n"
            "  --transpose n  shift every non-drum note by n semitones\n"
            "  --once         no loop, play through once\n"
            "  --list         show the channels and instruments, write nothing\n"
            "  --verbose      more about what was found\n"
            "\n"
            "Programs the game's own music uses: 21 lead, 23, 0, 19, 25, 18, 7 and 5\n"
            "for melody and bass, 9 for drums (General MIDI key numbers).\n");
    }

    std::vector<uint8_t> read_file(const fs::path& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) throw std::runtime_error("cannot open " + path.string());
        return std::vector<uint8_t>((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }

    std::string lower(std::string s) {
        for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }

    bool ends_with(const std::string& s, const std::string& suffix) {
        return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    std::string safe_name(std::string name) {
        for (char& c : name) {
            if (std::strchr("\\/:*?\"<>|", c) != nullptr || static_cast<unsigned char>(c) < 32) c = '-';
        }
        while (!name.empty() && (name.back() == ' ' || name.back() == '.')) name.pop_back();
        return name.empty() ? "song" : name;
    }

    int parse_int(const std::string& s) {
        return static_cast<int>(std::strtol(s.c_str(), nullptr, 0));
    }

    struct Pack {
        std::vector<uint8_t> sequence;
        std::string name;
        int font = 0;
        bool fanfare = false;
        bool has_soundfont = false;
    };

    Pack read_ootrs(const fs::path& path) {
        Pack pack;
        mz_zip_archive zip;
        std::memset(&zip, 0, sizeof zip);
        std::vector<uint8_t> bytes = read_file(path);
        if (!mz_zip_reader_init_mem(&zip, bytes.data(), bytes.size(), 0)) {
            throw std::runtime_error("not a zip file: " + path.string());
        }
        std::string meta;
        for (mz_uint i = 0; i < mz_zip_reader_get_num_files(&zip); i++) {
            mz_zip_archive_file_stat st;
            if (!mz_zip_reader_file_stat(&zip, i, &st) || st.m_is_directory) continue;
            std::string name = lower(st.m_filename);
            size_t size = 0;
            void* p = mz_zip_reader_extract_to_heap(&zip, i, &size, 0);
            if (p == nullptr) continue;
            std::vector<uint8_t> content(static_cast<uint8_t*>(p), static_cast<uint8_t*>(p) + size);
            mz_free(p);
            if (ends_with(name, ".seq") || ends_with(name, ".zseq") || ends_with(name, ".aseq")) {
                pack.sequence = std::move(content);
            }
            else if (ends_with(name, ".meta")) {
                meta.assign(content.begin(), content.end());
            }
            else if (ends_with(name, ".zbank") || ends_with(name, ".zsound") || ends_with(name, ".bankmeta")) {
                pack.has_soundfont = true;
            }
        }
        mz_zip_reader_end(&zip);
        if (pack.sequence.empty()) {
            throw std::runtime_error("no sequence (.seq/.zseq) inside " + path.string());
        }
        // .meta: name, soundfont (hex), type (bgm/fanfare), then categories.
        std::istringstream lines(meta);
        std::string line;
        int n = 0;
        while (std::getline(lines, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
            if (n == 0) pack.name = line;
            else if (n == 1) pack.font = parse_int(line);
            else if (n == 2) pack.fanfare = lower(line) == "fanfare";
            n++;
        }
        if (pack.name.empty()) pack.name = path.stem().string();
        return pack;
    }

    void load_map(const fs::path& path, std::map<int, int>& table, bool required) {
        std::ifstream in(path);
        if (!in) {
            if (required) throw std::runtime_error("cannot open map file " + path.string());
            return;
        }
        std::string line;
        while (std::getline(in, line)) {
            size_t hash = line.find('#');
            if (hash != std::string::npos) line.erase(hash);
            std::istringstream fields(line);
            std::string font, inst, prog;
            if (fields >> font >> inst >> prog) {
                table[(parse_int(font) << 8) | parse_int(inst)] = parse_int(prog);
            }
        }
    }

    // Walks the finished file the way the game's reader would, as a check
    // that every track parses to its end marker.
    std::string validate(const std::vector<uint8_t>& f) {
        if (f.size() < 68) return "file too short";
        auto u32 = [&f](size_t at) { return (static_cast<uint32_t>(f[at]) << 24) | (f[at + 1] << 16) | (f[at + 2] << 8) | f[at + 3]; };
        for (int t = 0; t < 16; t++) {
            uint32_t p = u32(t * 4);
            if (p == 0) continue;
            int status = 0;
            auto get = [&]() -> int {
                if (p >= f.size()) throw std::runtime_error("track " + std::to_string(t) + " runs off the file");
                uint8_t b = f[p++];
                if (b == 0xFE) {
                    if (p < f.size() && f[p] == 0xFE) { p++; return 0xFE; }
                    throw std::runtime_error("track " + std::to_string(t) + " has an unescaped 0xFE");
                }
                return b;
            };
            auto vlq = [&]() { uint32_t v = 0; for (int i = 0; i < 5; i++) { int b = get(); v = (v << 7) | (b & 0x7F); if (!(b & 0x80)) break; } return v; };
            try {
                while (true) {
                    vlq();
                    int b = get();
                    if (b == 0xFF) {
                        int type = get();
                        status = 0;
                        if (type == 0x2F) break;
                        if (type == 0x51) { get(); get(); get(); continue; }
                        if (type == 0x2E) { get(); get(); continue; }
                        if (type == 0x2D) { for (int i = 0; i < 6; i++) get(); continue; }
                        return "track " + std::to_string(t) + ": unknown meta";
                    }
                    if (b & 0x80) { status = b; b = get(); }
                    switch (status & 0xF0) {
                    case 0x90: get(); vlq(); break;
                    case 0xB0: get(); break;
                    case 0xC0: break;
                    case 0xE0: get(); break;
                    default: return "track " + std::to_string(t) + ": bad status";
                    }
                }
            }
            catch (std::exception& e) {
                return e.what();
            }
        }
        return "";
    }
}

namespace {
    struct Settings {
        fs::path output;      // -o: one file only
        fs::path out_dir;
        std::map<int, int> channel_program;
        std::map<int, int> map_table;
        bool once = false;
        bool list = false;
        bool verbose = false;
        int transpose = 0;
    };

    bool convertible(const fs::path& path) {
        std::string ext = lower(path.extension().string());
        return ext == ".ootrs" || ext == ".zip" || ext == ".seq" || ext == ".zseq" || ext == ".aseq" || ext == ".mid" || ext == ".midi";
    }

    // One input file. Returns 0 on success, 2 for a file that failed the
    // parse check, 3 for one over the game's size limit, 1 on any error.
    int convert_one(const fs::path& input, const Settings& s) {
        std::string ext = lower(input.extension().string());
        cseq::Song song;
        std::vector<std::string> warnings;
        std::string name = input.stem().string();
        oot::Report report;
        bool is_oot = false;

        if (ext == ".mid" || ext == ".midi") {
            midi::Options mo;
            mo.channel_program = s.channel_program;
            mo.one_shot = s.once;
            mo.transpose = s.transpose;
            midi::convert(read_file(input), mo, song, warnings);
        }
        else {
            is_oot = true;
            Pack pack;
            if (ext == ".ootrs" || ext == ".zip") {
                pack = read_ootrs(input);
                name = pack.name;
                if (pack.has_soundfont) {
                    warnings.push_back("the pack bundles its own soundfont, which cannot be used; instruments are mapped to the game's");
                }
            }
            else {
                pack.sequence = read_file(input);
            }
            oot::Options oo;
            oo.channel_program = s.channel_program;
            oo.instrument_program = s.map_table;
            oo.font = pack.font;
            oo.one_shot = s.once || pack.fanfare;
            oo.verbose = s.verbose;
            oot::convert(pack.sequence, oo, song, report);
            warnings.insert(warnings.end(), report.warnings.begin(), report.warnings.end());
            if (s.transpose != 0) {
                for (int t = 0; t < 15; t++) {
                    auto pinned = s.channel_program.find(t);
                    bool drums = pinned != s.channel_program.end() && pinned->second == 9;
                    for (cseq::Event& e : song.tracks[t].events) {
                        if ((e.bytes[0] & 0xF0) == 0x90 && !drums) {
                            e.bytes[1] = static_cast<uint8_t>(std::clamp(e.bytes[1] + s.transpose, 0, 127));
                        }
                    }
                }
            }
        }

        std::printf("%s\n", name.c_str());
        if (is_oot) {
            std::printf("  soundfont 0x%X, %s\n", report.channels.empty() ? 0 : report.channels[0].font,
                        song.loop_start >= 0 ? "loops" : "plays once");
            for (const oot::Report::ChannelUse& c : report.channels) {
                std::printf("  channel %2d: font 0x%02X instrument %3d -> program %2d  (%d notes)%s\n",
                            c.channel, c.font, c.instrument, c.program, c.notes,
                            s.channel_program.count(c.channel) ? "  [pinned]" : c.instrument == 0x7F ? "  [drums]" : "");
            }
            if (report.loop_start >= 0) {
                std::printf("  loop: ticks %lld to %lld (%.1f beats in, %.1f beats long)\n",
                            static_cast<long long>(report.loop_start), static_cast<long long>(report.length),
                            report.loop_start / 48.0, (report.length - report.loop_start) / 48.0);
            }
        }
        for (const std::string& w : warnings) {
            std::printf("  note: %s\n", w.c_str());
        }
        if (s.list) {
            return 0;
        }

        std::vector<uint8_t> file = cseq::write(song);
        std::string problem = validate(file);
        if (!problem.empty()) {
            std::printf("  ERROR: the written file does not parse: %s\n", problem.c_str());
            return 2;
        }
        fs::path output = s.output;
        if (output.empty()) {
            fs::path dir = s.out_dir.empty() ? input.parent_path() : s.out_dir;
            output = dir / fs::u8path(safe_name(name) + ".seq");
        }
        std::ofstream out(output, std::ios::binary);
        if (!out) throw std::runtime_error("cannot write " + output.string());
        out.write(reinterpret_cast<const char*>(file.data()), static_cast<std::streamsize>(file.size()));
        std::printf("  -> %s\n  %s\n", output.string().c_str(), cseq::summary(song, file).c_str());
        return file.size() > cseq::max_file_size ? 3 : 0;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        usage();
        return 1;
    }
    std::vector<fs::path> inputs;
    fs::path map_file;
    std::map<int, int> instrument_program;
    Settings s;
    auto parse_pairs = [](const std::string& text, std::map<int, int>& table, bool font_inst) {
        std::istringstream items(text);
        std::string item;
        while (std::getline(items, item, ',')) {
            size_t eq = item.find('=');
            if (eq == std::string::npos) throw std::runtime_error("bad mapping: " + item);
            std::string key = item.substr(0, eq);
            int value = parse_int(item.substr(eq + 1));
            if (font_inst) {
                size_t colon = key.find(':');
                if (colon == std::string::npos) throw std::runtime_error("bad mapping (want font:instrument=program): " + item);
                table[(parse_int(key.substr(0, colon)) << 8) | parse_int(key.substr(colon + 1))] = value;
            }
            else {
                table[parse_int(key)] = value;
            }
        }
    };
    try {
        for (int i = 1; i < argc; i++) {
            std::string a = argv[i];
            auto next = [&]() -> std::string {
                if (i + 1 >= argc) throw std::runtime_error(a + " needs a value");
                return argv[++i];
            };
            if (a == "-o") s.output = fs::u8path(next());
            else if (a == "-d") s.out_dir = fs::u8path(next());
            else if (a == "--prog") parse_pairs(next(), s.channel_program, false);
            else if (a == "--inst") parse_pairs(next(), instrument_program, true);
            else if (a == "--map") map_file = fs::u8path(next());
            else if (a == "--transpose") s.transpose = parse_int(next());
            else if (a == "--once") s.once = true;
            else if (a == "--list") s.list = true;
            else if (a == "--verbose") s.verbose = true;
            else if (a == "-h" || a == "--help") { usage(); return 0; }
            else if (!a.empty() && a[0] == '-') throw std::runtime_error("unknown option " + a);
            else inputs.push_back(fs::u8path(a));
        }
        if (inputs.empty()) {
            usage();
            return 1;
        }

        // The default map beside the exe, then the named one on top.
        fs::path exe_dir = fs::absolute(fs::u8path(argv[0])).parent_path();
        load_map(exe_dir / "q64music.map", s.map_table, false);
        if (!map_file.empty()) load_map(map_file, s.map_table, true);
        for (const auto& [k, v] : instrument_program) s.map_table[k] = v;

        // Folders are searched for anything convertible, subfolders too.
        std::vector<fs::path> files;
        bool any_folder = false;
        for (const fs::path& in : inputs) {
            std::error_code ec;
            if (fs::is_directory(in, ec)) {
                any_folder = true;
                for (const auto& entry : fs::recursive_directory_iterator(in, ec)) {
                    if (entry.is_regular_file() && convertible(entry.path())) {
                        files.push_back(entry.path());
                    }
                }
            }
            else {
                files.push_back(in);
            }
        }
        std::sort(files.begin(), files.end());
        if (files.empty()) {
            std::printf("nothing to convert (looking for .ootrs, .seq, .zseq, .mid)\n");
            return 1;
        }
        if (files.size() > 1 && !s.output.empty()) {
            throw std::runtime_error("-o names one file; use -d for a folder of outputs");
        }
        // With the exe next to the game, converted files go straight into
        // its custom_music folder unless told otherwise.
        if (s.out_dir.empty() && s.output.empty()) {
            std::error_code ec;
            if (any_folder && fs::is_directory(exe_dir / "custom_music", ec)) {
                s.out_dir = exe_dir / "custom_music";
            }
        }
        if (!s.out_dir.empty()) {
            std::error_code ec;
            fs::create_directories(s.out_dir, ec);
        }

        int ok = 0;
        int failed = 0;
        int worst = 0;
        for (const fs::path& file : files) {
            int result;
            try {
                result = convert_one(file, s);
            }
            catch (std::exception& e) {
                std::printf("%s\n  ERROR: %s\n", file.string().c_str(), e.what());
                result = 1;
            }
            if (result == 0 || result == 3) ok++; else failed++;
            worst = std::max(worst, result);
        }
        if (files.size() > 1) {
            std::printf("\n%d converted, %d failed%s\n", ok, failed,
                        s.out_dir.empty() ? "" : (", into " + s.out_dir.string()).c_str());
        }
        return worst;
    }
    catch (std::exception& e) {
        std::printf("error: %s\n", e.what());
        return 1;
    }
}
