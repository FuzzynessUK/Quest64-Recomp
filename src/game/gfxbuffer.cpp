#include <atomic>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <string>

#include "gfxbuffer.h"
#include "zelda_config.h"
#include "recomp.h"

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif

// Display list room.
//
// func_800011DC builds a frame. Its argument is one of two 0xD150-byte frame
// blocks at 0x80301000 (D_80301000[2], alternating): the NNScTask at +0,
// segment 2 from +0x58 - the model matrices, n-th at +0x118 + n*0x40 - then
// the display list from +0x8148 up to +0xD148, where the message and the
// colour image pointer sit. It points gMasterGfxPos (0x8007B2FC) at +0x8148
// (0x80001204), every draw routine appends through it, and at the end it
// stores the list's start and size in the task (+0x40, +0x44; 0x800017C4,
// 0x800017E8). Nothing checks either area: 2560 commands and 512 matrices.
// Rock Shower in Blue Cave - the cave's geometry plus a model and a matrix
// for every rock - runs past the list and over the colour image pointer and
// the other block, which is the vanilla crash; several monsters attacking
// and casting at once in Real Time Combat does the same.
//
// Here the list is built in a 512 KB buffer per frame block instead, in RAM
// the game never touches (its last segment ends at 0x804FBC80; Leonardo's
// copy starts at 0x80700000): the hook after the store at 0x80001204 moves
// gMasterGfxPos there, and the one before the epilogue points the task at it.
// Everything that draws only ever goes through gMasterGfxPos, and the
// widescreen walker follows the task's pointer, so nothing else changes.
// The 0x5000 bytes the list leaves behind follow the matrices directly, so
// the matrices get 320 more (832) at the same segment addresses for free.
//
// The game's own high-water mark (0x8007B34C) and last size (0x8004C210) are
// kept up to date, and gfx.txt logs each new peak above the vanilla limits.
namespace {
    constexpr int32_t gMasterGfxPos = 0x8007B2FC;
    constexpr int32_t matrix_count = 0x8007B2F8;     // D_8007B2F8, reset each frame
    constexpr int32_t game_peak = 0x8007B34C;
    constexpr int32_t game_last = 0x8004C210;
    constexpr int32_t current_map = 0x80084EE4;
    constexpr int32_t current_submap = 0x80084EE8;

    constexpr int32_t frame_blocks = 0x80301000;
    constexpr int32_t frame_block_size = 0xD150;
    constexpr int vanilla_commands = (0xD148 - 0x8148) / 8;   // 2560
    constexpr int vanilla_matrices = (0x8148 - 0x118) / 0x40;  // 512
    constexpr int matrix_room = (0xD148 - 0x118) / 0x40;       // 832, list moved out

    constexpr int32_t lists = 0x80500000;
    constexpr int32_t list_size = 0x80000;
    constexpr int list_commands = list_size / 8;

    // Read by the crash handler.
    std::atomic<int> last_commands{ 0 };
    std::atomic<int> last_matrices{ 0 };
    std::atomic<int> peak_commands{ 0 };
    std::atomic<int> peak_matrices{ 0 };
    std::atomic<int> last_map{ -1 };
    std::atomic<int> last_submap{ -1 };
    int logged_commands = vanilla_commands * 3 / 4;
    int logged_matrices = vanilla_matrices * 3 / 4;

    int block_index(int32_t block) {
        const int32_t off = block - frame_blocks;
        if (off != 0 && off != frame_block_size) {
            return -1;
        }
        return off == 0 ? 0 : 1;
    }

    void log_line(const std::string& line) {
        FILE* f = std::fopen((zelda64::get_app_folder_path() / "gfx.txt").string().c_str(), "a");
        if (f) {
            std::fprintf(f, "%s\n", line.c_str());
            std::fclose(f);
        }
    }
}

// func_800011DC at 0x80001208, just after gMasterGfxPos = block + 0x8148;
// a0 = the frame block.
extern "C" void quest64_gfx_list_start(uint8_t* rdram, recomp_context* ctx) {
    const int index = block_index(static_cast<int32_t>(ctx->r4));
    if (index < 0) {
        return;
    }
    MEM_W(0, gMasterGfxPos) = lists + index * list_size;
}

// func_800011DC at 0x800017EC, before the epilogue: the list is finished and
// the task holds the vanilla start and size; a0 = the frame block.
extern "C" void quest64_gfx_list_end(uint8_t* rdram, recomp_context* ctx) {
    const int32_t block = static_cast<int32_t>(ctx->r4);
    const int index = block_index(block);
    if (index < 0) {
        return;
    }
    const int32_t start = lists + index * list_size;
    const int32_t bytes = MEM_W(0, gMasterGfxPos) - start;
    MEM_W(0x40, block) = start;
    MEM_W(0x44, block) = bytes;

    const int commands = bytes / 8;
    const int matrices = MEM_W(0, matrix_count);
    MEM_W(0, game_last) = commands;
    if (commands > MEM_W(0, game_peak)) {
        MEM_W(0, game_peak) = commands;
    }
    last_commands.store(commands);
    last_matrices.store(matrices);
    last_map.store(MEM_W(0, current_map));
    last_submap.store(MEM_W(0, current_submap));
    if (commands > peak_commands.load()) {
        peak_commands.store(commands);
    }
    if (matrices > peak_matrices.load()) {
        peak_matrices.store(matrices);
    }

    // A new peak worth knowing about: past three quarters of a vanilla limit,
    // then each further tenth.
    if (commands > logged_commands || matrices > logged_matrices) {
        if (commands > logged_commands) {
            logged_commands = commands + commands / 10;
        }
        if (matrices > logged_matrices) {
            logged_matrices = matrices + matrices / 10;
        }
        char line[256];
        std::snprintf(line, sizeof line,
            "map %d/%d: %d commands (vanilla room %d, now %d), %d matrices (vanilla room %d, now %d)%s%s",
            MEM_W(0, current_map), MEM_W(0, current_submap), commands, vanilla_commands, list_commands,
            matrices, vanilla_matrices, matrix_room,
            commands > list_commands ? " - LIST OVERFLOW" : "",
            matrices > matrix_room ? " - MATRIX OVERFLOW" : "");
        log_line(line);
    }
}

#ifdef _WIN32
namespace {
    wchar_t crash_txt[MAX_PATH];
    wchar_t crash_dmp[MAX_PATH];

    LONG WINAPI on_crash(EXCEPTION_POINTERS* info) {
        static std::atomic<bool> once{ false };
        if (once.exchange(true)) {
            return EXCEPTION_CONTINUE_SEARCH;
        }
        FILE* f = _wfopen(crash_txt, L"w");
        if (f) {
            const EXCEPTION_RECORD* rec = info->ExceptionRecord;
            std::time_t now = std::time(nullptr);
            char when[64];
            std::strftime(when, sizeof when, "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            std::fprintf(f, "Quest 64: Recompiled crashed at %s\n", when);
            std::fprintf(f, "exception %08lX at %p", rec->ExceptionCode, rec->ExceptionAddress);
            if (rec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && rec->NumberParameters >= 2) {
                std::fprintf(f, " (%s %p)", rec->ExceptionInformation[0] == 1 ? "writing" : "reading",
                             reinterpret_cast<void*>(rec->ExceptionInformation[1]));
            }
            std::fprintf(f, "\n");

            // Each frame of the faulting thread as module + offset.
            HANDLE process = GetCurrentProcess();
            HANDLE thread = GetCurrentThread();
            CONTEXT context = *info->ContextRecord;
            STACKFRAME64 frame{};
            frame.AddrPC.Offset = context.Rip;
            frame.AddrPC.Mode = AddrModeFlat;
            frame.AddrFrame.Offset = context.Rbp;
            frame.AddrFrame.Mode = AddrModeFlat;
            frame.AddrStack.Offset = context.Rsp;
            frame.AddrStack.Mode = AddrModeFlat;
            SymInitialize(process, nullptr, TRUE);
            std::fprintf(f, "stack:\n");
            for (int i = 0; i < 32; i++) {
                if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, &context, nullptr,
                                 SymFunctionTableAccess64, SymGetModuleBase64, nullptr) ||
                    frame.AddrPC.Offset == 0) {
                    break;
                }
                const DWORD64 pc = frame.AddrPC.Offset;
                HMODULE module = nullptr;
                char name[MAX_PATH] = "?";
                if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                       reinterpret_cast<LPCSTR>(pc), &module) && module) {
                    char full[MAX_PATH];
                    if (GetModuleFileNameA(module, full, MAX_PATH)) {
                        std::snprintf(name, sizeof name, "%s", std::filesystem::path(full).filename().string().c_str());
                    }
                }
                char symbol_buffer[sizeof(SYMBOL_INFO) + 256] = {};
                SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbol_buffer);
                symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
                symbol->MaxNameLen = 255;
                DWORD64 displacement = 0;
                const bool named = SymFromAddr(process, pc, &displacement, symbol) != FALSE;
                std::fprintf(f, "  %s+0x%llX%s%s\n", name,
                             static_cast<unsigned long long>(module ? pc - reinterpret_cast<DWORD64>(module) : pc),
                             named ? "  " : "", named ? symbol->Name : "");
            }

            std::fprintf(f, "last frame: map %d/%d, %d display list commands, %d matrices\n",
                         last_map.load(), last_submap.load(), last_commands.load(), last_matrices.load());
            std::fprintf(f, "peak this session: %d commands (room %d), %d matrices (room %d)\n",
                         peak_commands.load(), list_commands, peak_matrices.load(), matrix_room);
            std::fprintf(f, "a dump is in crash.dmp next to this file.\n");
            std::fclose(f);
        }

        HANDLE dump = CreateFileW(crash_dmp, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (dump != INVALID_HANDLE_VALUE) {
            MINIDUMP_EXCEPTION_INFORMATION exception{ GetCurrentThreadId(), info, FALSE };
            MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), dump,
                              static_cast<MINIDUMP_TYPE>(MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithThreadInfo),
                              &exception, nullptr, nullptr);
            CloseHandle(dump);
        }
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

void zelda64::gfxbuffer::install_crash_handler() {
    const std::filesystem::path folder = zelda64::get_app_folder_path();
    std::error_code ec;
    std::filesystem::create_directories(folder, ec);
    wcsncpy_s(crash_txt, (folder / "crash.txt").wstring().c_str(), _TRUNCATE);
    wcsncpy_s(crash_dmp, (folder / "crash.dmp").wstring().c_str(), _TRUNCATE);
    SetUnhandledExceptionFilter(on_crash);
}
#else
void zelda64::gfxbuffer::install_crash_handler() {
}
#endif
