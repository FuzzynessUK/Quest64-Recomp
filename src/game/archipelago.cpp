#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <thread>
#include <span>
#include <vector>

#include "archipelago.h"
#include "enhancements.h"
#include "randomizer.h"
#include "notify.h"
#include "randomizer/chest_data.h"
#include "randomizer/merrow_data.h"
#include "zelda_config.h"
#include "recomp.h"
#include "librecomp/game.hpp"
#include "json/json.hpp"

#ifdef _WIN32
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  define SECURITY_WIN32
#  include <security.h>
#  include <schannel.h>
#  pragma comment(lib, "ws2_32.lib")
#  pragma comment(lib, "secur32.lib")
using socket_t = SOCKET;
constexpr socket_t invalid_socket = INVALID_SOCKET;
#else
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <sys/socket.h>
#  include <unistd.h>
using socket_t = int;
constexpr socket_t invalid_socket = -1;
#endif

// The Archipelago connector. See include/archipelago.h for what the game
// side may call; everything below runs on one worker thread.
//
// There is no networking library in this tree, so the WebSocket is done here:
// a TCP connection, the HTTP upgrade handshake (which needs SHA-1 and
// base64, both small enough to write out), and then RFC 6455 frames. Only
// what an Archipelago session uses is implemented - text frames, ping/pong,
// close - and a client always masks what it sends.
//
// The protocol itself is Archipelago's: the server opens with RoomInfo, we
// answer with Connect, and it replies Connected or ConnectionRefused. After
// that it sends ReceivedItems and we send LocationChecks.

namespace {
    using nlohmann::json;

    constexpr const char* game_name = "Quest 64 Recompiled";
    // The Archipelago version this speaks, which is the one the apworld is
    // built for. The server refuses a client older than it needs.
    constexpr int ap_major = 0, ap_minor = 6, ap_build = 7;
    // Receive our own items from other worlds, our own items from our own
    // world, and anything the slot starts with.
    constexpr int items_handling = 0b111;

    std::filesystem::path options_path() {
        return zelda64::get_app_folder_path() / "archipelago.json";
    }

    std::filesystem::path log_path() {
        return zelda64::get_app_folder_path() / "archipelago.txt";
    }

    std::mutex log_mutex;
    void log_line(const std::string& text) {
        std::lock_guard lock{ log_mutex };
        std::ofstream out(log_path(), std::ios::app);
        out << text << "\n";
    }

    // ---- SHA-1 and base64, for the handshake only ------------------------

    std::array<uint32_t, 5> sha1(const std::string& input) {
        std::array<uint32_t, 5> h{ 0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u };
        std::string message = input;
        uint64_t bits = static_cast<uint64_t>(message.size()) * 8;
        message.push_back(static_cast<char>(0x80));
        while (message.size() % 64 != 56) {
            message.push_back('\0');
        }
        for (int i = 7; i >= 0; i--) {
            message.push_back(static_cast<char>((bits >> (i * 8)) & 0xFF));
        }
        for (size_t at = 0; at < message.size(); at += 64) {
            uint32_t w[80];
            for (int i = 0; i < 16; i++) {
                w[i] = (static_cast<uint8_t>(message[at + i * 4]) << 24) |
                       (static_cast<uint8_t>(message[at + i * 4 + 1]) << 16) |
                       (static_cast<uint8_t>(message[at + i * 4 + 2]) << 8) |
                       static_cast<uint8_t>(message[at + i * 4 + 3]);
            }
            auto rotl = [](uint32_t v, int by) { return (v << by) | (v >> (32 - by)); };
            for (int i = 16; i < 80; i++) {
                w[i] = rotl(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
            }
            uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
            for (int i = 0; i < 80; i++) {
                uint32_t f, k;
                if (i < 20)      { f = (b & c) | (~b & d);          k = 0x5A827999u; }
                else if (i < 40) { f = b ^ c ^ d;                   k = 0x6ED9EBA1u; }
                else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDCu; }
                else             { f = b ^ c ^ d;                   k = 0xCA62C1D6u; }
                uint32_t t = rotl(a, 5) + f + e + k + w[i];
                e = d; d = c; c = rotl(b, 30); b = a; a = t;
            }
            h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
        }
        return h;
    }

    std::string base64(const uint8_t* data, size_t length) {
        static const char* set = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string out;
        for (size_t i = 0; i < length; i += 3) {
            uint32_t block = static_cast<uint32_t>(data[i]) << 16;
            if (i + 1 < length) block |= static_cast<uint32_t>(data[i + 1]) << 8;
            if (i + 2 < length) block |= data[i + 2];
            out.push_back(set[(block >> 18) & 0x3F]);
            out.push_back(set[(block >> 12) & 0x3F]);
            out.push_back(i + 1 < length ? set[(block >> 6) & 0x3F] : '=');
            out.push_back(i + 2 < length ? set[block & 0x3F] : '=');
        }
        return out;
    }

    // ---- the socket -------------------------------------------------------

    struct Socket {
        socket_t handle = invalid_socket;

        ~Socket() { close(); }

        void close() {
            if (handle != invalid_socket) {
#ifdef _WIN32
                closesocket(handle);
#else
                ::close(handle);
#endif
                handle = invalid_socket;
            }
        }

        bool open(const std::string& host, const std::string& port, std::string& error) {
            addrinfo hints{};
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            addrinfo* found = nullptr;
            if (getaddrinfo(host.c_str(), port.c_str(), &hints, &found) != 0 || found == nullptr) {
                error = "cannot find " + host;
                return false;
            }
            for (addrinfo* a = found; a != nullptr; a = a->ai_next) {
                handle = ::socket(a->ai_family, a->ai_socktype, a->ai_protocol);
                if (handle == invalid_socket) {
                    continue;
                }
                if (::connect(handle, a->ai_addr, static_cast<int>(a->ai_addrlen)) == 0) {
                    break;
                }
                close();
            }
            freeaddrinfo(found);
            if (handle == invalid_socket) {
                error = "nothing listening on " + host + ":" + port;
                return false;
            }
            int one = 1;
            setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&one), sizeof one);
            // A short read timeout, so the worker can also flush whatever the
            // game has queued rather than sitting in recv.
#ifdef _WIN32
            DWORD timeout = 100;
            setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof timeout);
#else
            timeval timeout{ 0, 100 * 1000 };
            setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
#endif
            return true;
        }

        bool send_all(const uint8_t* data, size_t length) {
            size_t sent = 0;
            while (sent < length) {
                int n = ::send(handle, reinterpret_cast<const char*>(data + sent),
                               static_cast<int>(length - sent), 0);
                if (n <= 0) {
                    return false;
                }
                sent += static_cast<size_t>(n);
            }
            return true;
        }

        // -1 closed or failed, 0 nothing waiting, else bytes read.
        int read_some(uint8_t* into, size_t room) {
            int n = ::recv(handle, reinterpret_cast<char*>(into), static_cast<int>(room), 0);
            if (n > 0) {
                return n;
            }
            if (n == 0) {
                return -1;
            }
#ifdef _WIN32
            int err = WSAGetLastError();
            return (err == WSAETIMEDOUT || err == WSAEWOULDBLOCK) ? 0 : -1;
#else
            return (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) ? 0 : -1;
#endif
        }
    };

    // ---- TLS ---------------------------------------------------------------
    //
    // A room hosted on archipelago.gg is wss:// only: a plaintext upgrade to
    // one gets no reply at all, which is what "the server hung up during the
    // handshake" in the log meant. A room hosted from the Archipelago
    // launcher is the other way round - plain ws:// and no certificate - and
    // the two look identical as an address, so which one to speak is decided
    // by trying (see split_server and run_session).
    //
    // There is no TLS library in this tree and no package manager to add one
    // with, so this uses what Windows already has: Schannel, through SSPI, in
    // the same spirit as the SHA-1 and base64 above. Only what one session
    // needs is here - a client handshake, then encrypt and decrypt - and
    // Windows checks the certificate chain and the host name itself, so
    // nothing here has to be trusted to get that right.
#ifdef _WIN32
    std::string tls_error(SECURITY_STATUS status) {
        switch (status) {
            case SEC_E_UNTRUSTED_ROOT:     return "the server's certificate is not trusted";
            case SEC_E_CERT_EXPIRED:       return "the server's certificate has expired";
            case SEC_E_WRONG_PRINCIPAL:    return "the server's certificate is for another host";
            case SEC_E_ILLEGAL_MESSAGE:    return "the server did not answer in TLS";
            case SEC_E_ALGORITHM_MISMATCH: return "no TLS version in common with the server";
            default: {
                char note[64];
                std::snprintf(note, sizeof note, "TLS failed (0x%08lX)",
                              static_cast<unsigned long>(status));
                return note;
            }
        }
    }

    struct Tls {
        CredHandle credentials{};
        CtxtHandle context{};
        bool have_credentials = false;
        bool have_context = false;
        SecPkgContext_StreamSizes sizes{};
        std::vector<uint8_t> cipher;   // read off the socket, not yet decrypted
        std::vector<uint8_t> plain;    // decrypted, not yet handed to the caller

        ~Tls() {
            if (have_context) {
                DeleteSecurityContext(&context);
            }
            if (have_credentials) {
                FreeCredentialsHandle(&credentials);
            }
        }

        bool handshake(Socket& socket, const std::string& host, std::string& error) {
            // SCHANNEL_CRED rather than the newer SCH_CREDENTIALS: the
            // latter is only declared when SCHANNEL_USE_BLACKLISTS is
            // defined, which also drags in the NT headers for
            // UNICODE_STRING. The difference is TLS 1.3, and the server
            // takes 1.2 (checked against archipelago.gg), so it is not
            // worth the extra headers. Leaving grbitEnabledProtocols at 0
            // lets Windows pick the best it has.
            SCHANNEL_CRED credential{};
            credential.dwVersion = SCHANNEL_CRED_VERSION;
            credential.dwFlags = SCH_USE_STRONG_CRYPTO | SCH_CRED_AUTO_CRED_VALIDATION |
                                 SCH_CRED_NO_DEFAULT_CREDS;
            if (AcquireCredentialsHandleA(nullptr, const_cast<char*>(UNISP_NAME_A),
                                          SECPKG_CRED_OUTBOUND, nullptr, &credential, nullptr,
                                          nullptr, &credentials, nullptr) != SEC_E_OK) {
                error = "Windows would not open a TLS credential";
                return false;
            }
            have_credentials = true;

            // The host name goes in as the target: that is both what SNI
            // carries and what the certificate is checked against.
            std::string target = host;
            const DWORD request = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
                                  ISC_REQ_CONFIDENTIALITY | ISC_REQ_ALLOCATE_MEMORY |
                                  ISC_REQ_STREAM;
            DWORD attributes = 0;

            SecBuffer greeting{};
            greeting.BufferType = SECBUFFER_TOKEN;
            SecBufferDesc greeting_desc{ SECBUFFER_VERSION, 1, &greeting };
            SECURITY_STATUS status = InitializeSecurityContextA(
                &credentials, nullptr, const_cast<char*>(target.c_str()), request, 0, 0,
                nullptr, 0, &context, &greeting_desc, &attributes, nullptr);
            if (status != SEC_I_CONTINUE_NEEDED) {
                error = tls_error(status);
                return false;
            }
            have_context = true;
            if (greeting.cbBuffer != 0 && greeting.pvBuffer != nullptr) {
                bool sent = socket.send_all(static_cast<uint8_t*>(greeting.pvBuffer),
                                            greeting.cbBuffer);
                FreeContextBuffer(greeting.pvBuffer);
                if (!sent) {
                    error = "the server hung up during the TLS handshake";
                    return false;
                }
            }

            // The socket's own 100ms read timeout paces this; a server that
            // says nothing at all is given five seconds before giving up,
            // which is also how quickly a plaintext server is found out when
            // TLS is tried against it first.
            std::vector<uint8_t> incoming;
            uint8_t buffer[4096];
            int idle = 0;
            for (;;) {
                int n = socket.read_some(buffer, sizeof buffer);
                if (n < 0) {
                    error = "the server hung up during the TLS handshake";
                    return false;
                }
                if (n == 0) {
                    if (++idle > 50) {
                        error = "the TLS handshake timed out";
                        return false;
                    }
                    continue;
                }
                idle = 0;
                incoming.insert(incoming.end(), buffer, buffer + n);

                SecBuffer in_buffers[2]{};
                in_buffers[0].BufferType = SECBUFFER_TOKEN;
                in_buffers[0].pvBuffer = incoming.data();
                in_buffers[0].cbBuffer = static_cast<unsigned long>(incoming.size());
                in_buffers[1].BufferType = SECBUFFER_EMPTY;
                SecBufferDesc in_desc{ SECBUFFER_VERSION, 2, in_buffers };

                SecBuffer out_buffer{};
                out_buffer.BufferType = SECBUFFER_TOKEN;
                SecBufferDesc out_desc{ SECBUFFER_VERSION, 1, &out_buffer };

                status = InitializeSecurityContextA(
                    &credentials, &context, const_cast<char*>(target.c_str()), request, 0, 0,
                    &in_desc, 0, nullptr, &out_desc, &attributes, nullptr);

                if (out_buffer.cbBuffer != 0 && out_buffer.pvBuffer != nullptr) {
                    bool sent = socket.send_all(static_cast<uint8_t*>(out_buffer.pvBuffer),
                                                out_buffer.cbBuffer);
                    FreeContextBuffer(out_buffer.pvBuffer);
                    if (!sent) {
                        error = "the server hung up during the TLS handshake";
                        return false;
                    }
                }

                if (status == SEC_E_INCOMPLETE_MESSAGE) {
                    continue;   // a record still arriving; keep it and read on
                }
                if (status == SEC_I_CONTINUE_NEEDED || status == SEC_E_OK) {
                    // Whatever was not part of the handshake belongs to
                    // whatever comes next.
                    if (in_buffers[1].BufferType == SECBUFFER_EXTRA) {
                        incoming.erase(incoming.begin(),
                                       incoming.end() - in_buffers[1].cbBuffer);
                    }
                    else {
                        incoming.clear();
                    }
                    if (status == SEC_E_OK) {
                        cipher = std::move(incoming);
                        break;
                    }
                    continue;
                }
                error = tls_error(status);
                return false;
            }

            if (QueryContextAttributes(&context, SECPKG_ATTR_STREAM_SIZES, &sizes) != SEC_E_OK) {
                error = "Windows would not describe the TLS stream";
                return false;
            }
            return true;
        }

        bool send_all(Socket& socket, const uint8_t* data, size_t length) {
            std::vector<uint8_t> record;
            while (length > 0) {
                unsigned long chunk =
                    static_cast<unsigned long>(std::min<size_t>(length, sizes.cbMaximumMessage));
                record.assign(sizes.cbHeader + chunk + sizes.cbTrailer, 0);
                std::memcpy(record.data() + sizes.cbHeader, data, chunk);

                SecBuffer buffers[3]{};
                buffers[0] = { sizes.cbHeader, SECBUFFER_STREAM_HEADER, record.data() };
                buffers[1] = { chunk, SECBUFFER_DATA, record.data() + sizes.cbHeader };
                buffers[2] = { sizes.cbTrailer, SECBUFFER_STREAM_TRAILER,
                               record.data() + sizes.cbHeader + chunk };
                SecBufferDesc message{ SECBUFFER_VERSION, 3, buffers };
                if (EncryptMessage(&context, 0, &message, 0) != SEC_E_OK) {
                    return false;
                }
                size_t total = static_cast<size_t>(buffers[0].cbBuffer) + buffers[1].cbBuffer +
                               buffers[2].cbBuffer;
                if (!socket.send_all(record.data(), total)) {
                    return false;
                }
                data += chunk;
                length -= chunk;
            }
            return true;
        }

        // The same contract as Socket::read_some: -1 gone, 0 nothing waiting.
        int read_some(Socket& socket, uint8_t* into, size_t room) {
            for (;;) {
                if (!plain.empty()) {
                    size_t taking = std::min(room, plain.size());
                    std::memcpy(into, plain.data(), taking);
                    plain.erase(plain.begin(), plain.begin() + static_cast<long>(taking));
                    return static_cast<int>(taking);
                }
                if (!cipher.empty()) {
                    SecBuffer buffers[4]{};
                    buffers[0] = { static_cast<unsigned long>(cipher.size()), SECBUFFER_DATA,
                                   cipher.data() };
                    buffers[1].BufferType = SECBUFFER_EMPTY;
                    buffers[2].BufferType = SECBUFFER_EMPTY;
                    buffers[3].BufferType = SECBUFFER_EMPTY;
                    SecBufferDesc message{ SECBUFFER_VERSION, 4, buffers };
                    SECURITY_STATUS status = DecryptMessage(&context, &message, 0, nullptr);
                    if (status == SEC_E_OK) {
                        // Both of these point into `cipher`, so they are
                        // copied out before it is replaced.
                        std::vector<uint8_t> extra;
                        for (const SecBuffer& part : buffers) {
                            const uint8_t* at = static_cast<const uint8_t*>(part.pvBuffer);
                            if (part.BufferType == SECBUFFER_DATA && part.cbBuffer != 0) {
                                plain.insert(plain.end(), at, at + part.cbBuffer);
                            }
                            else if (part.BufferType == SECBUFFER_EXTRA && part.cbBuffer != 0) {
                                extra.assign(at, at + part.cbBuffer);
                            }
                        }
                        cipher = std::move(extra);
                        continue;
                    }
                    if (status != SEC_E_INCOMPLETE_MESSAGE) {
                        return -1;   // expired, renegotiating, or simply broken
                    }
                }
                uint8_t buffer[4096];
                int n = socket.read_some(buffer, sizeof buffer);
                if (n <= 0) {
                    return n;
                }
                cipher.insert(cipher.end(), buffer, buffer + n);
            }
        }
    };
#endif

    // What the WebSocket talks through: a plain socket, or one with TLS over
    // it. Both answer send_all and read_some the same way, so nothing below
    // has to know which it got.
    struct Stream {
        Socket socket;
#ifdef _WIN32
        std::unique_ptr<Tls> tls;
#endif

        void close() {
#ifdef _WIN32
            tls.reset();
#endif
            socket.close();
        }

        bool open(const std::string& host, const std::string& port, bool secure,
                  std::string& error) {
            close();
            if (!socket.open(host, port, error)) {
                return false;
            }
            if (!secure) {
                return true;
            }
#ifdef _WIN32
            tls = std::make_unique<Tls>();
            if (!tls->handshake(socket, host, error)) {
                close();
                return false;
            }
            return true;
#else
            error = "this build cannot speak TLS";
            close();
            return false;
#endif
        }

        bool send_all(const uint8_t* data, size_t length) {
#ifdef _WIN32
            if (tls) {
                return tls->send_all(socket, data, length);
            }
#endif
            return socket.send_all(data, length);
        }

        int read_some(uint8_t* into, size_t room) {
#ifdef _WIN32
            if (tls) {
                return tls->read_some(socket, into, room);
            }
#endif
            return socket.read_some(into, room);
        }
    };

    // ---- WebSocket --------------------------------------------------------

    struct WebSocket {
        Stream stream;
        std::vector<uint8_t> incoming;   // raw bytes not yet parsed into frames
        std::string partial;             // a message still arriving in pieces
        std::mt19937 rng{ std::random_device{}() };

        // Back to how it started, so the other scheme can be tried on the
        // same object - nothing here is safe to copy or move.
        void reset() {
            stream.close();
            incoming.clear();
            partial.clear();
        }

        bool handshake(const std::string& host, const std::string& port, std::string& error) {
            uint8_t nonce[16];
            for (uint8_t& b : nonce) {
                b = static_cast<uint8_t>(rng() & 0xFF);
            }
            std::string key = base64(nonce, sizeof nonce);

            std::string request =
                "GET / HTTP/1.1\r\n"
                "Host: " + host + ":" + port + "\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Key: " + key + "\r\n"
                "Sec-WebSocket-Version: 13\r\n"
                "\r\n";
            if (!stream.send_all(reinterpret_cast<const uint8_t*>(request.data()), request.size())) {
                error = "the server hung up during the handshake";
                return false;
            }

            std::string response;
            uint8_t buffer[1024];
            while (response.find("\r\n\r\n") == std::string::npos) {
                int n = stream.read_some(buffer, sizeof buffer);
                if (n < 0) {
                    error = "the server hung up during the handshake";
                    return false;
                }
                if (n == 0) {
                    if (response.size() > 8192) {
                        error = "the handshake reply made no sense";
                        return false;
                    }
                    continue;
                }
                response.append(reinterpret_cast<char*>(buffer), static_cast<size_t>(n));
            }
            if (response.compare(0, 12, "HTTP/1.1 101") != 0) {
                error = "not a WebSocket server (" + response.substr(0, response.find("\r\n")) + ")";
                return false;
            }

            // The accept header proves the server saw our key.
            std::array<uint32_t, 5> digest = sha1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
            uint8_t raw[20];
            for (int i = 0; i < 5; i++) {
                raw[i * 4 + 0] = static_cast<uint8_t>(digest[i] >> 24);
                raw[i * 4 + 1] = static_cast<uint8_t>(digest[i] >> 16);
                raw[i * 4 + 2] = static_cast<uint8_t>(digest[i] >> 8);
                raw[i * 4 + 3] = static_cast<uint8_t>(digest[i]);
            }
            std::string want = base64(raw, sizeof raw);
            std::string lowered = response;
            std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (lowered.find("sec-websocket-accept: " + [&] {
                    std::string w = want;
                    std::transform(w.begin(), w.end(), w.begin(),
                                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                    return w;
                }()) == std::string::npos) {
                error = "the server's handshake reply did not match";
                return false;
            }

            // Anything after the headers is already frame data.
            size_t body = response.find("\r\n\r\n") + 4;
            incoming.assign(response.begin() + static_cast<long>(body), response.end());
            return true;
        }

        bool send_text(const std::string& text) {
            std::vector<uint8_t> frame;
            frame.push_back(0x81);   // FIN, text
            size_t length = text.size();
            if (length < 126) {
                frame.push_back(static_cast<uint8_t>(0x80 | length));
            }
            else if (length <= 0xFFFF) {
                frame.push_back(0x80 | 126);
                frame.push_back(static_cast<uint8_t>(length >> 8));
                frame.push_back(static_cast<uint8_t>(length));
            }
            else {
                frame.push_back(0x80 | 127);
                for (int i = 7; i >= 0; i--) {
                    frame.push_back(static_cast<uint8_t>((static_cast<uint64_t>(length) >> (i * 8)) & 0xFF));
                }
            }
            uint8_t mask[4];
            for (uint8_t& b : mask) {
                b = static_cast<uint8_t>(rng() & 0xFF);
            }
            frame.insert(frame.end(), mask, mask + 4);
            for (size_t i = 0; i < length; i++) {
                frame.push_back(static_cast<uint8_t>(text[i]) ^ mask[i % 4]);
            }
            return stream.send_all(frame.data(), frame.size());
        }

        void send_control(uint8_t opcode, const std::vector<uint8_t>& payload) {
            std::vector<uint8_t> frame;
            frame.push_back(static_cast<uint8_t>(0x80 | opcode));
            frame.push_back(static_cast<uint8_t>(0x80 | payload.size()));
            uint8_t mask[4];
            for (uint8_t& b : mask) {
                b = static_cast<uint8_t>(rng() & 0xFF);
            }
            frame.insert(frame.end(), mask, mask + 4);
            for (size_t i = 0; i < payload.size(); i++) {
                frame.push_back(payload[i] ^ mask[i % 4]);
            }
            stream.send_all(frame.data(), frame.size());
        }

        // Reads whatever has arrived and appends any complete messages.
        // False means the connection is gone.
        bool poll(std::vector<std::string>& messages) {
            uint8_t buffer[4096];
            int n = stream.read_some(buffer, sizeof buffer);
            if (n < 0) {
                return false;
            }
            if (n > 0) {
                incoming.insert(incoming.end(), buffer, buffer + n);
            }

            for (;;) {
                if (incoming.size() < 2) {
                    return true;
                }
                bool fin = (incoming[0] & 0x80) != 0;
                uint8_t opcode = incoming[0] & 0x0F;
                bool masked = (incoming[1] & 0x80) != 0;
                uint64_t length = incoming[1] & 0x7F;
                size_t at = 2;
                if (length == 126) {
                    if (incoming.size() < at + 2) return true;
                    length = (static_cast<uint64_t>(incoming[at]) << 8) | incoming[at + 1];
                    at += 2;
                }
                else if (length == 127) {
                    if (incoming.size() < at + 8) return true;
                    length = 0;
                    for (int i = 0; i < 8; i++) {
                        length = (length << 8) | incoming[at + i];
                    }
                    at += 8;
                }
                uint8_t mask[4] = {};
                if (masked) {
                    if (incoming.size() < at + 4) return true;
                    std::memcpy(mask, incoming.data() + at, 4);
                    at += 4;
                }
                if (incoming.size() < at + length) {
                    return true;
                }
                std::string payload(reinterpret_cast<char*>(incoming.data() + at), length);
                if (masked) {
                    for (size_t i = 0; i < payload.size(); i++) {
                        payload[i] = static_cast<char>(static_cast<uint8_t>(payload[i]) ^ mask[i % 4]);
                    }
                }
                incoming.erase(incoming.begin(), incoming.begin() + static_cast<long>(at + length));

                switch (opcode) {
                    case 0x0:   // continuation
                    case 0x1:   // text
                        partial += payload;
                        if (fin) {
                            messages.push_back(partial);
                            partial.clear();
                        }
                        break;
                    case 0x8:   // close
                        return false;
                    case 0x9:   // ping
                        send_control(0xA, std::vector<uint8_t>(payload.begin(), payload.end()));
                        break;
                    default:
                        break;   // pong, or a binary frame we have no use for
                }
            }
        }
    };

    // ---- state shared with the game --------------------------------------

    std::mutex state_mutex;
    zelda64::archipelago::Options live_options;
    std::atomic<zelda64::archipelago::Status> current_status{ zelda64::archipelago::Status::Off };
    std::string status_detail = "Off";
    std::string slot_connected;

    std::mutex queue_mutex;
    // Archipelago mode is on, which is not the same as being connected: the
    // game is watched whenever it is on, and what it notices waits in the
    // queue until there is a server to tell. Playing on through a dropped
    // connection, or before logging in, then costs nothing.
    std::atomic<bool> ap_enabled{ false };
    // Whether the game plays by the seed's rules right now: empty chests, no
    // spirit screen, gifts taken back, Souls and the portal gate. Only while
    // the server has accepted us - enabled but not connected (or dropped)
    // plays exactly as the original game, and only the watching goes on.
    bool playing_seed() {
        return ap_enabled.load() && current_status.load() == zelda64::archipelago::Status::Connected;
    }
    // The gift NPCs, the portal door and the boss-item locks are found in
    // the ROM once, the first time Archipelago mode is on - but never before
    // the game has booted: the menu applies the saved settings at start-up,
    // before the ROM is loaded, and a search of an empty ROM finds nothing
    // and would still count as done.
    bool located = false;
    bool rom_ready = false;
    void locate_once();
    // Defined with the game side, at the end of the file.
    void forget_sent_checks();
    // Defined with the seed settings, near goal_reached.
    void note_seed_settings();
    // How an item is written in a notice. Defined with the boss names.
    std::string pretty_item(int64_t item);
    // Works the held Souls out from the whole of server_items. Unlike the
    // rest, a Soul is a state rather than something handed over once, so it
    // is read from the entire list every time that list changes and takes no
    // notice of how far the save has been given. Called with server_mutex
    // already held.
    void refresh_souls();

    std::deque<int64_t> outgoing_checks;    // locations the game has reached
    bool goal_pending = false;

    // What the server says this slot has already checked, and what the yaml
    // asked for. Both arrive with Connected, so both survive everything the
    // game itself forgets - a reload, a crash, a session played offline.
    // The bosses and the monsters have no save flag to read back (unlike the
    // chests and spirits), so for a gate that counts them this is the only
    // honest source.
    std::mutex server_mutex;
    // Everything the server has sent this slot, in order, and how far down
    // that list the save in play has been given. Archipelago re-sends the
    // whole list whenever a client connects, so it is a list and a mark
    // rather than a queue: reconnecting rebuilds the list, the mark does not
    // move, and nothing is handed over twice. Loading a save sets the mark
    // to whatever that save had, so an older file is caught up again and a
    // newer one is left alone.
    std::vector<int64_t> server_items;
    int items_applied = 0;
    std::set<int64_t> checked_locations;
    // Every location this slot has, done or not: checked_locations plus the
    // Connected packet's missing_locations. A gift NPC whose location is not
    // in it (giftsanity off) is left to give its own item.
    std::set<int64_t> slot_locations;
    // The yaml's settings for the game itself (slot_data "settings") and the
    // seed its randomizer rolls with, as last received. Null until a
    // Connected packet carries them.
    json seed_settings_live;
    int64_t rando_seed_live = 0;

    // Whether a location is part of this seed. Chests, spirits and gift NPCs
    // are only changed while theirs is: with chestsanity or spiritsanity off
    // in the yaml, a chest opens and gives its own item as usual and a
    // spirit brings up the level-up screen.
    bool seed_has(int64_t group, int index) {
        std::lock_guard lock{ server_mutex };
        return slot_locations.count(zelda64::archipelago::id_base + group + index) != 0;
    }
    int mammon_portal = 0;   // 0 vanilla, 1 bosses, 2 monsters, 3 both
    // 0 off, 1 the seven before Mammon, 2 with Mammon as well.
    std::atomic<int> boss_souls{ 0 };
    // Bit n (1-8) set once that boss's Soul has arrived. Read from the boss
    // spawn hook, which runs on the game thread, so it is an atomic rather
    // than something the mutex guards. The server sends every item again on
    // every connect, so this rebuilds itself and never needs saving.
    std::atomic<uint32_t> souls_held{ 0 };
    // Who else is in the room, so an arriving item can say where it came
    // from, and which slot is us, so our own finds do not say "from".
    std::map<int, std::string> player_names;
    int our_slot = -1;
    // Which game each slot is playing, and every game's item ids by name.
    // An item found here for somebody else belongs to their game, not this
    // one, so naming it takes the server's data package - all the room
    // sends in the message itself is numbers ("1 found their 1365511683").
    std::map<int, std::string> slot_game;
    std::map<std::string, std::map<int64_t, std::string>> item_names_by_game;

    // What to call an item belonging to `slot`. Called with server_mutex held.
    std::string item_name_for(int slot, int64_t item) {
        auto game = slot_game.find(slot);
        if (game != slot_game.end()) {
            auto names = item_names_by_game.find(game->second);
            if (names != item_names_by_game.end()) {
                auto found = names->second.find(item);
                if (found != names->second.end()) {
                    return found->second;
                }
            }
        }
        // Whose game it is was not known, or the id was not in it. Ids are
        // handed out per game and the room only holds a few, so looking
        // through the rest still beats showing a number at somebody.
        for (const auto& [name, names] : item_names_by_game) {
            auto found = names.find(item);
            if (found != names.end()) {
                return found->second;
            }
        }
        return "item " + std::to_string(item);
    }

    std::thread worker;
    std::atomic<bool> worker_should_run{ false };
    std::atomic<uint32_t> generation{ 0 };   // bumped to make the worker restart

    void set_status(zelda64::archipelago::Status status, const std::string& detail) {
        current_status.store(status);
        std::lock_guard lock{ state_mutex };
        status_detail = detail;
        log_line(detail);
    }

    // Which of the two an address asks for. A bare host:port says nothing
    // either way, and the two kinds of room are told apart by trying.
    enum class Scheme { Either, Plain, Secure };

    struct Address {
        std::string host;
        std::string port = "38281";
        Scheme scheme = Scheme::Either;
    };

    Address split_server(const std::string& server) {
        // Accepts host, host:port, and a ws:// or wss:// prefix.
        Address address;
        std::string rest = server;
        if (rest.compare(0, 6, "wss://") == 0) {
            rest = rest.substr(6);
            address.scheme = Scheme::Secure;
        }
        else if (rest.compare(0, 5, "ws://") == 0) {
            rest = rest.substr(5);
            address.scheme = Scheme::Plain;
        }
        size_t colon = rest.rfind(':');
        if (colon == std::string::npos) {
            address.host = rest;
            return address;
        }
        address.host = rest.substr(0, colon);
        address.port = rest.substr(colon + 1);
        return address;
    }

    // This machine or this network, where a room is the Archipelago
    // launcher's and speaks plain. Only used to decide what to try first.
    bool local_host(const std::string& host) {
        static const char* local[] = { "localhost", "127.", "::1", "10.", "192.168." };
        for (const char* prefix : local) {
            if (host.compare(0, std::strlen(prefix), prefix) == 0) {
                return true;
            }
        }
        return false;
    }

    // One connection, from dial to hang-up. Returns when it drops.
    void run_session(const zelda64::archipelago::Options& options, uint32_t my_generation) {
        auto stale = [&] { return !worker_should_run.load() || generation.load() != my_generation; };

        auto [host, port, scheme] = split_server(options.server);
        set_status(zelda64::archipelago::Status::Connecting, "Connecting to " + host + ":" + port + "...");

        // A room on archipelago.gg takes TLS and nothing else; one hosted
        // from the Archipelago launcher takes plain and nothing else; and
        // "host:port" is how both are written. So both are always tried,
        // and all the address decides is which to try first: a ws:// or
        // wss:// prefix if there is one, otherwise plain for a room on this
        // machine or this network and TLS for anything else.
        //
        // A prefix picks the order rather than ruling the other one out on
        // purpose. Nobody should have to know which kind of room they were
        // given to be able to join it, and the wrong one is refused in a
        // moment rather than waited on.
        bool secure_first = scheme == Scheme::Secure ? true
                          : scheme == Scheme::Plain  ? false
                          :                            !local_host(host);
        const bool attempts[2] = { secure_first, !secure_first };

        WebSocket ws;
        std::string error, first_error;
        bool ready = false;
        for (bool secure : attempts) {
            if (stale()) {
                return;
            }
            ws.reset();
            log_line(secure ? "trying TLS" : "trying without TLS");
            if (ws.stream.open(host, port, secure, error) && ws.handshake(host, port, error)) {
                log_line(secure ? "connected over TLS" : "connected without TLS");
                ready = true;
                break;
            }
            log_line("  " + error);
            // The kind that was tried first is the one worth reporting: the
            // other was only the long shot, and its complaint is usually
            // about not speaking the right thing rather than about the room.
            if (first_error.empty()) {
                first_error = error;
            }
        }
        if (!ready) {
            set_status(zelda64::archipelago::Status::Failed, "Could not connect: " + first_error);
            return;
        }

        bool authenticated = false;
        std::vector<std::string> messages;

        while (!stale()) {
            messages.clear();
            if (!ws.poll(messages)) {
                if (!stale()) {
                    set_status(zelda64::archipelago::Status::Failed,
                               authenticated ? "Disconnected from the server" : "The server closed the connection");
                }
                return;
            }

            for (const std::string& raw : messages) {
                json parsed;
                try {
                    parsed = json::parse(raw);
                }
                catch (const json::exception&) {
                    continue;
                }
                if (!parsed.is_array()) {
                    continue;
                }
                for (const json& packet : parsed) {
                    std::string cmd = packet.value("cmd", "");
                    if (cmd == "RoomInfo") {
                        json connect = {
                            { "cmd", "Connect" },
                            { "password", options.password },
                            { "game", game_name },
                            { "name", options.slot },
                            { "uuid", "quest64-recomp" },
                            { "version", { { "major", ap_major }, { "minor", ap_minor },
                                           { "build", ap_build }, { "class", "Version" } } },
                            { "items_handling", items_handling },
                            { "tags", json::array() },
                            { "slot_data", true },
                        };
                        ws.send_text(json::array({ connect }).dump());

                        // And the names for everything in the room. Only the
                        // games actually being played - asking for none at
                        // all fetches every game Archipelago knows about.
                        json games = json::array();
                        if (packet.contains("games") && packet["games"].is_array()) {
                            games = packet["games"];
                        }
                        json wanted = { { "cmd", "GetDataPackage" }, { "games", games } };
                        ws.send_text(json::array({ wanted }).dump());
                    }
                    else if (cmd == "DataPackage") {
                        if (packet.contains("data") && packet["data"].contains("games") &&
                            packet["data"]["games"].is_object()) {
                            std::lock_guard lock{ server_mutex };
                            const json& games = packet["data"]["games"];
                            for (auto game = games.begin(); game != games.end(); ++game) {
                                if (!game.value().contains("item_name_to_id")) {
                                    continue;
                                }
                                const json& ids = game.value()["item_name_to_id"];
                                std::map<int64_t, std::string>& names = item_names_by_game[game.key()];
                                for (auto it = ids.begin(); it != ids.end(); ++it) {
                                    if (it.value().is_number_integer()) {
                                        names[it.value().get<int64_t>()] = it.key();
                                    }
                                }
                            }
                            log_line("item names for " + std::to_string(item_names_by_game.size()) +
                                     " game(s)");
                        }
                    }
                    else if (cmd == "Connected") {
                        authenticated = true;
                        {
                            std::lock_guard lock{ state_mutex };
                            slot_connected = options.slot;
                        }
                        // What this slot has already done, and what the yaml
                        // asked for. A gate that counts bosses or monsters
                        // has nothing else to go on: neither leaves a flag in
                        // the save, so after a reload the server's list is
                        // the only record that they were ever beaten.
                        {
                            std::lock_guard lock{ server_mutex };
                            slot_locations.clear();
                            if (packet.contains("checked_locations") &&
                                packet["checked_locations"].is_array()) {
                                for (const json& id : packet["checked_locations"]) {
                                    if (id.is_number_integer()) {
                                        checked_locations.insert(id.get<int64_t>());
                                        slot_locations.insert(id.get<int64_t>());
                                    }
                                }
                            }
                            if (packet.contains("missing_locations") &&
                                packet["missing_locations"].is_array()) {
                                for (const json& id : packet["missing_locations"]) {
                                    if (id.is_number_integer()) {
                                        slot_locations.insert(id.get<int64_t>());
                                    }
                                }
                            }
                            // Who is who, so an item can say who found it.
                            if (packet.contains("slot")) {
                                our_slot = packet["slot"].get<int>();
                            }
                            if (packet.contains("players") && packet["players"].is_array()) {
                                player_names.clear();
                                for (const json& who : packet["players"]) {
                                    if (!who.contains("slot")) {
                                        continue;
                                    }
                                    std::string name = who.value("alias", "");
                                    if (name.empty()) {
                                        name = who.value("name", "");
                                    }
                                    if (!name.empty()) {
                                        player_names[who["slot"].get<int>()] = name;
                                    }
                                }
                            }
                            // Which game each slot plays, so an item sent to
                            // one of them can be looked up in the right game.
                            if (packet.contains("slot_info") && packet["slot_info"].is_object()) {
                                slot_game.clear();
                                for (auto it = packet["slot_info"].begin();
                                     it != packet["slot_info"].end(); ++it) {
                                    std::string game = it.value().value("game", "");
                                    if (!game.empty()) {
                                        slot_game[std::atoi(it.key().c_str())] = game;
                                    }
                                }
                            }
                            if (packet.contains("slot_data") && packet["slot_data"].is_object()) {
                                const json& slot = packet["slot_data"];
                                if (slot.contains("mammon_portal") &&
                                    slot["mammon_portal"].is_number_integer()) {
                                    mammon_portal = slot["mammon_portal"].get<int>();
                                }
                                if (slot.contains("boss_souls") &&
                                    slot["boss_souls"].is_number_integer()) {
                                    boss_souls.store(slot["boss_souls"].get<int>());
                                }
                                if (slot.contains("settings") && slot["settings"].is_object()) {
                                    seed_settings_live = slot["settings"];
                                    rando_seed_live = slot.value("rando_seed", int64_t{ 0 });
                                }
                            }
                            log_line("the server has " + std::to_string(checked_locations.size()) +
                                     " check(s) for this slot; mammon_portal = " +
                                     std::to_string(mammon_portal) + ", boss_souls = " +
                                     std::to_string(boss_souls.load()));
                        }
                        set_status(zelda64::archipelago::Status::Connected,
                                   "Connected as " + options.slot);
                        note_seed_settings();
                    }
                    else if (cmd == "RoomUpdate") {
                        // Sent when anything the room knows changes, including
                        // this slot's own list growing.
                        if (packet.contains("checked_locations") &&
                            packet["checked_locations"].is_array()) {
                            std::lock_guard lock{ server_mutex };
                            for (const json& id : packet["checked_locations"]) {
                                if (id.is_number_integer()) {
                                    checked_locations.insert(id.get<int64_t>());
                                }
                            }
                        }
                    }
                    else if (cmd == "ConnectionRefused") {
                        std::string why;
                        if (packet.contains("errors") && packet["errors"].is_array()) {
                            for (const json& e : packet["errors"]) {
                                if (!why.empty()) why += ", ";
                                why += e.get<std::string>();
                            }
                        }
                        if (why.empty()) {
                            why = "the server refused the connection";
                        }
                        set_status(zelda64::archipelago::Status::Failed, "Refused: " + why);
                        return;
                    }
                    else if (cmd == "ReceivedItems") {
                        if (packet.contains("items") && packet["items"].is_array()) {
                            // index 0 is the server listing everything this
                            // slot has ever been sent, which it does on every
                            // connect. That is worth applying but not worth
                            // announcing - it would be a wall of notices for
                            // things found hours ago. Anything else is new.
                            bool fresh = packet.value("index", 0) != 0;
                            {
                                // index says where these belong in the list,
                                // so a resync (0) rebuilds it and a delta
                                // adds to the end. The mark is left alone.
                                std::lock_guard lock{ server_mutex };
                                size_t at = packet.value("index", 0);
                                if (at > server_items.size()) {
                                    at = server_items.size();
                                }
                                server_items.resize(at);
                                for (const json& item : packet["items"]) {
                                    if (item.contains("item")) {
                                        server_items.push_back(item["item"].get<int64_t>());
                                    }
                                }
                                refresh_souls();
                            }
                            if (fresh) {
                                for (const json& item : packet["items"]) {
                                    if (!item.contains("item")) {
                                        continue;
                                    }
                                    std::string line = "Received " +
                                        pretty_item(item["item"].get<int64_t>());
                                    int finder = item.value("player", -1);
                                    std::lock_guard lock{ server_mutex };
                                    auto who = player_names.find(finder);
                                    if (finder != our_slot && who != player_names.end()) {
                                        line += " from " + who->second;
                                    }
                                    zelda64::notify::post(line, zelda64::notify::Kind::ApReceived);
                                    log_line(line);
                                }
                            }
                        }
                    }
                    else if (cmd == "PrintJSON") {
                        std::string line;
                        if (packet.contains("data") && packet["data"].is_array()) {
                            for (const json& part : packet["data"]) {
                                line += part.value("text", "");
                            }
                        }
                        if (!line.empty()) {
                            log_line("  " + line);
                        }
                        // The other direction: a check found here holding
                        // something that belongs to another player. The
                        // message itself is only numbers, so the name comes
                        // from the data package and the player from the room
                        // - our own receipts are announced from ReceivedItems
                        // instead, where the name is this game's own.
                        if (packet.value("type", "") == "ItemSend" &&
                            packet.contains("item") && packet["item"].is_object()) {
                            int finder = packet["item"].value("player", -1);
                            int receiver = packet.value("receiving", -1);
                            int64_t what = packet["item"].value("item", static_cast<int64_t>(0));
                            std::lock_guard lock{ server_mutex };
                            auto name_of = [&](int slot) {
                                auto who = player_names.find(slot);
                                return who != player_names.end() ? who->second
                                                                 : "player " + std::to_string(slot);
                            };
                            if (finder == our_slot && receiver != our_slot) {
                                zelda64::notify::post(
                                    "You have sent \"" + item_name_for(receiver, what) +
                                        "\" to \"" + name_of(receiver) + "\"",
                                    zelda64::notify::Kind::ApSent);
                            }
                            else if (finder != our_slot && receiver != our_slot) {
                                // The rest of the room's traffic, for the
                                // "all items in the room" setting. What
                                // arrives here is announced from
                                // ReceivedItems, so it is not repeated.
                                std::string item = item_name_for(receiver, what);
                                zelda64::notify::post(
                                    finder == receiver
                                        ? name_of(finder) + " found their \"" + item + "\""
                                        : name_of(finder) + " sent \"" + item + "\" to \"" +
                                              name_of(receiver) + "\"",
                                    zelda64::notify::Kind::ApRoom);
                            }
                        }
                    }
                }
            }

            if (!authenticated) {
                continue;
            }

            // Anything the game has queued up.
            std::vector<int64_t> to_send;
            bool send_goal = false;
            {
                std::lock_guard lock{ queue_mutex };
                while (!outgoing_checks.empty()) {
                    to_send.push_back(outgoing_checks.front());
                    outgoing_checks.pop_front();
                }
                send_goal = goal_pending;
                goal_pending = false;
            }
            if (!to_send.empty()) {
                json checks = { { "cmd", "LocationChecks" }, { "locations", to_send } };
                if (!ws.send_text(json::array({ checks }).dump())) {
                    set_status(zelda64::archipelago::Status::Failed, "Disconnected from the server");
                    return;
                }
                log_line("sent " + std::to_string(to_send.size()) + " check(s)");
            }
            if (send_goal) {
                json done = { { "cmd", "StatusUpdate" }, { "status", 30 } };   // CLIENT_GOAL
                ws.send_text(json::array({ done }).dump());
                log_line("told the server the goal is done");
            }
        }
    }

    void worker_main() {
        while (worker_should_run.load()) {
            uint32_t my_generation = generation.load();
            zelda64::archipelago::Options options;
            {
                std::lock_guard lock{ state_mutex };
                options = live_options;
            }
            if (!options.enabled || options.slot.empty()) {
                set_status(zelda64::archipelago::Status::Off,
                           options.enabled ? "Waiting for a slot name" : "Off");
                // Nothing to do until the options change.
                while (worker_should_run.load() && generation.load() == my_generation) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));
                }
                continue;
            }

            run_session(options, my_generation);

            // A dropped connection waits a moment and tries again, unless the
            // options changed underneath us or the server refused us outright.
            for (int i = 0; i < 20 && worker_should_run.load() && generation.load() == my_generation; i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
        }
        set_status(zelda64::archipelago::Status::Off, "Off");
    }

    // Declared after `worker`, so it is destroyed before it: a joinable
    // std::thread whose destructor runs would call terminate() on the way
    // out. This is what actually stops the connector when the game closes.
    struct WorkerGuard {
        ~WorkerGuard() {
            worker_should_run.store(false);
            generation.fetch_add(1);
            if (worker.joinable()) {
                worker.join();
            }
        }
    };
    WorkerGuard worker_guard;

    void start_worker() {
        if (worker.joinable()) {
            return;
        }
#ifdef _WIN32
        WSADATA wsa{};
        WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
        worker_should_run.store(true);
        worker = std::thread(worker_main);
    }
}

// ---------------------------------------------------------------- options

zelda64::archipelago::Options zelda64::archipelago::load_options() {
    Options o;
    std::ifstream in(options_path());
    if (!in.good()) {
        return o;
    }
    nlohmann::json j;
    try {
        in >> j;
    }
    catch (const nlohmann::json::parse_error&) {
        return o;
    }
    if (!j.is_object()) {
        return o;
    }
    auto get = [&](const char* name, auto& field) {
        if (j.contains(name)) {
            try {
                field = j[name].get<std::decay_t<decltype(field)>>();
            }
            catch (const nlohmann::json::exception&) {
            }
        }
    };
    get("enabled", o.enabled);
    get("server", o.server);
    get("slot", o.slot);
    get("password", o.password);
    return o;
}

void zelda64::archipelago::save_options(const Options& options) {
    nlohmann::json j;
    j["enabled"] = options.enabled;
    j["server"] = options.server;
    j["slot"] = options.slot;
    j["password"] = options.password;
    std::ofstream out(options_path());
    out << j.dump(4);
}

const zelda64::archipelago::Options& zelda64::archipelago::active_options() {
    static Options snapshot;
    std::lock_guard lock{ state_mutex };
    snapshot = live_options;
    return snapshot;
}

void zelda64::archipelago::apply_options(const Options& options) {
    save_options(options);
    if (options.enabled) {
        locate_once();
    }
    ap_enabled.store(options.enabled);
    {
        std::lock_guard lock{ state_mutex };
        live_options = options;
    }
    {
        // A new connection starts from a clean slate.
        std::lock_guard lock{ queue_mutex };
        outgoing_checks.clear();
        goal_pending = false;
    }
    {
        // The item list is the server's to state, and it states all of it
        // again on connecting, so it goes rather than being carried into
        // what might be a different slot altogether. The mark does not: how
        // much this save has had is the save's business, not the
        // connection's.
        //
        // The Souls are left alone as well. refresh_souls works them out
        // afresh from whatever list arrives, so clearing them here would
        // buy nothing and would make every Soul already held look new -
        // announcing the lot of them again on every reconnect.
        std::lock_guard lock{ server_mutex };
        server_items.clear();
    }
    // Emptying that queue would lose anything noticed while there was no
    // server to tell, so forget what has been sent as well: the chests and
    // spirits are read back out of the save flags on the next frame, which
    // is the same re-sync a resumed save gets.
    forget_sent_checks();
    start_worker();
    generation.fetch_add(1);
}

// ---------------------------------------------------------------- status

zelda64::archipelago::Status zelda64::archipelago::status() {
    return current_status.load();
}

std::string zelda64::archipelago::status_line() {
    std::lock_guard lock{ state_mutex };
    return status_detail;
}

std::string zelda64::archipelago::connected_slot() {
    std::lock_guard lock{ state_mutex };
    return slot_connected;
}

// ---------------------------------------------------------------- game side

void zelda64::archipelago::send_check(int64_t location) {
    if (!ap_enabled.load()) {
        return;
    }
    {
        // Counted straight away rather than waiting for the server to say so,
        // so a gate that needs this check opens the moment it happens - and
        // so it still counts while playing offline.
        std::lock_guard lock{ server_mutex };
        checked_locations.insert(location);
    }
    std::lock_guard lock{ queue_mutex };
    outgoing_checks.push_back(location);
}

// How far down the server's list each save has been given, filed under the
// run timer's key for that save - a hash of the bytes themselves, so a save
// is recognised by its contents rather than by which slot it sits in. See
// the timer's own note in speedrun.cpp; both are called from its Controller
// Pak hooks.
//
// A save nobody has a record for starts at 0, which is right for a new file
// in an ongoing seed: everything the server has sent so far is handed over
// as the game gets going.
namespace {
    std::filesystem::path progress_path() {
        return zelda64::get_app_folder_path() / "archipelago_progress.json";
    }

    std::map<std::string, int> save_progress_by_key;
    bool progress_loaded = false;

    void load_progress_file() {
        if (progress_loaded) {
            return;
        }
        progress_loaded = true;
        std::ifstream in(progress_path());
        if (!in.is_open()) {
            return;
        }
        nlohmann::json j;
        try {
            in >> j;
        }
        catch (const nlohmann::json::parse_error&) {
            return;
        }
        if (!j.is_object()) {
            return;
        }
        for (auto it = j.begin(); it != j.end(); ++it) {
            if (it->is_number_integer()) {
                save_progress_by_key[it.key()] = it->get<int>();
            }
        }
    }
}

// The two halves are defined at the end of the file, with the level-up
// counter they have to reset.

// ---------------------------------------------------------------- seed settings

namespace {
    std::atomic<bool> settled{ false };
    std::atomic<bool> in_effect{ false };
    json seed_settings_boot;
    int64_t rando_seed_boot = 0;

    int setting(const char* name) {
        if (!seed_settings_boot.is_object() || !seed_settings_boot.contains(name)) {
            return 0;
        }
        const json& v = seed_settings_boot[name];
        if (v.is_boolean()) {
            return v.get<bool>() ? 1 : 0;
        }
        return v.is_number_integer() ? v.get<int>() : 0;
    }
}

// Called by the worker once a Connected packet has been read. A launch that
// is already running cannot take the settings any more - the ROM was
// patched at boot - so it says so rather than quietly playing without them.
namespace {
void note_seed_settings() {
    if (!settled.load()) {
        return;   // the boot has not happened yet and will take them itself
    }
    json live;
    int64_t seed = 0;
    {
        std::lock_guard lock{ server_mutex };
        live = seed_settings_live;
        seed = rando_seed_live;
    }
    if (live.is_null()) {
        return;
    }
    if (!in_effect.load()) {
        log_line("the seed's settings arrived after the game had started: restart the game to use them");
        zelda64::notify::post("Archipelago: restart the game to play with this seed's settings");
    }
    else if (live != seed_settings_boot || seed != rando_seed_boot) {
        log_line("this slot's settings differ from the ones the game started with: restart the game");
        zelda64::notify::post("Archipelago: a different seed's settings - restart the game");
    }
}
}

void zelda64::archipelago::settle_seed_settings() {
    if (settled.exchange(true)) {
        return;
    }
    if (!load_options().enabled) {
        return;
    }
    // Connecting from the launcher is the way to do it; this only covers a
    // connection that is still on its way when Start is pressed.
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (current_status.load() == Status::Connecting && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if (current_status.load() != Status::Connected) {
        log_line("not connected as the game started: it plays with the menus' settings");
        return;
    }
    {
        std::lock_guard lock{ server_mutex };
        seed_settings_boot = seed_settings_live;
        rando_seed_boot = rando_seed_live;
    }
    if (seed_settings_boot.is_null()) {
        log_line("connected, but the slot sent no settings (an older apworld): the menus' settings are used");
        return;
    }
    in_effect.store(true);
    log_line("playing with the seed's settings: " + seed_settings_boot.dump() +
             ", randomizer seed " + std::to_string(rando_seed_boot));
}

bool zelda64::archipelago::seed_settings_in_effect() {
    return in_effect.load();
}

bool zelda64::archipelago::settings_locked() {
    return in_effect.load() || current_status.load() == Status::Connected;
}

// The randomizer the seed plays with: vanilla except for what the yaml
// switched on. The item lists stay off - what is where is the server's to
// say - and so do the spirit and chest movers, Lost Keys and the door
// options, which the connector handles its own way (Boss Souls).
void zelda64::archipelago::apply_seed_settings(zelda64::randomizer::Options& o) {
    using zelda64::randomizer::ListMode;
    o = zelda64::randomizer::Options{};
    o.mode = zelda64::randomizer::Mode::Randomizer;
    o.seed = std::to_string(rando_seed_boot);
    o.chests = ListMode::Off;
    o.drops = ListMode::Off;
    o.gifts = ListMode::Off;
    o.wingsmiths = ListMode::Off;
    o.boss_rewards_shuffler = false;

    bool spells = setting("shuffle_spells") != 0;
    o.spell_shuffle = spells;
    o.hinted_spell_names = spells;
    o.spell_rebalance = spells;
    o.invalidity = spells;
    o.spell_overrides = spells;
    o.early_healing = setting("early_healing") != 0;
    o.enemy_randomizer = setting("enemy_randomizer") != 0;
    o.boss_order = setting("shuffle_boss_order") != 0;
    o.boss_element = setting("random_guilty_element") != 0;
    bool fast = setting("faster_areas") != 0;
    o.fast_monastery = fast;
    o.fast_blue_cave = fast;
    o.fast_shamwood = fast;
    o.fast_mammon = fast;
    o.wings_never_expire = setting("wings_never_expire") != 0;
    o.drop_limit_disabled = setting("no_enemy_drop_limit") != 0;
    o.element_uncap = setting("element_cap_99") != 0;
    o.text_improvements = setting("text_improvements") != 0;
    o.text_palette = setting("text_palette") != 0 ? 1 : 0;   // 1 is random
    o.staff_palette = setting("staff_palette") != 0;
    o.cloak_palette = setting("cloak_colour") != 0;
    o.brian_palette = setting("brian_clothes") != 0;
    o.spell_palette = setting("spell_palettes") != 0;
}

// The enhancements: the six the yaml decides, and the three that would
// change the game underneath the seed (One Hit KO, Hard Mode, Easy Mode) off.
// Everything else - the timer, the notices, N64 mode, the HUD layout - is
// the player's own and stays as the menus have it.
void zelda64::archipelago::apply_seed_settings(zelda64::enhancements::Options& o) {
    o.jp_healing = setting("jp_healing") != 0;
    o.longer_magic_barrier = setting("jp_magic_barrier") != 0;
    o.stat_up_effect = setting("jp_stat_up_effect") != 0;
    o.exit_from_anywhere = setting("exit_from_anywhere") != 0;
    o.faster_walk = setting("fast_walking") != 0;
    // Always, in Archipelago: a multiworld hands out a lot of the same item.
    o.stack_items = true;
    o.one_hit_ko = false;
    o.hard_mode = false;
    o.easier_quest = false;
}

void zelda64::archipelago::goal_reached() {
    std::lock_guard lock{ queue_mutex };
    goal_pending = true;
}

void zelda64::archipelago::shutdown() {
    worker_should_run.store(false);
    generation.fetch_add(1);
    if (worker.joinable()) {
        worker.join();
    }
#ifdef _WIN32
    WSACleanup();
#endif
}

// ---------------------------------------------------------------- the game
//
// What the game has reached, and what the server has sent back.
//
// Checks are read out of the game's own save flags rather than by hooking
// the routines that grant things. Both groups are plain bitfields, bit
// `id & 7` of byte `base + (id >> 3)`:
//
//   chests opened  0x800869D8, ids 0-87   (func_80012170 tests, 0x800121B0 sets)
//   spirits taken  0x80086AE8, ids 0-97   (func_80012700 tests, 0x80012740 sets)
//
// and a chest's or spirit's id is exactly the index the world file numbers
// its location by, so a set bit is a location with no translation needed.
//
// Reading the state rather than catching the event is what makes resuming
// work: everything already done in a loaded save is seen on the first frame
// and sent, so a game continued after a crash, or one played offline for a
// while, re-syncs by itself. Sending a location twice is harmless - the
// server ignores a repeat - so the only thing that has to be right is never
// missing one.
namespace {
    // The menu mask: a bit a menu, and setting a bit opens it. Bit 3 is the
    // spirit screen, where an element is chosen and levelled. That is both
    // how a Level Up from the server is handed over and, with the bit
    // cleared again as quickly as a spirit sets it, how a spirit is made to
    // simply vanish instead.
    constexpr int32_t menu_mask = 0x8007B2E4;
    constexpr uint32_t menu_spirit = 0x8;
    // A battle is running when bit 0 is set - func_8001C5F4 tests exactly
    // that. gGameMode is not a battle indicator.
    constexpr int32_t gBattleState = 0x8008C592;

    // The game's monster list: 0-66 ordinary, 67-74 the bosses in story
    // order (Solvaring first, Mammon last). gBossData is in the same order,
    // so a boss's number is his index there plus one, and that is the number
    // his check and his Soul both use.
    constexpr int boss_first = 67;
    constexpr int boss_count = 8;
    const char* const boss_names[boss_count] = {
        "Solvaring", "Zelse", "Nepty", "Shilf", "Fargo", "Guilty", "Beigis", "Mammon",
    };
    // Where each one waits, so a Soul arriving can say where to go. A Soul
    // is no use without this: the item says nothing about which corner of
    // the world just became worth walking back to.
    const char* const boss_arenas[boss_count] = {
        "Connor Forest", "Windward Forest", "Blue Cave", "Baragoon Tunnel",
        "Boil Hole", "Brannoch Castle", "Brannoch Castle roof", "Mammon's World",
    };

    // The Souls the server has sent, worked out afresh from the whole list.
    // Called with server_mutex held, from the thread that changed the list.
    void refresh_souls() {
        uint32_t held = 0;
        for (int64_t item : server_items) {
            int64_t soul = item - zelda64::archipelago::item_boss_soul;
            if (soul >= 1 && soul <= boss_count) {
                held |= 1u << soul;
            }
        }
        uint32_t had = souls_held.exchange(held);
        // Only what is new is worth a line: a reconnect re-reads the same
        // list and should not announce all of it again. Nothing visible
        // happens when a Soul lands, so say who it was and where he is.
        for (int order = 1; order <= boss_count; order++) {
            if ((held & (1u << order)) && !(had & (1u << order))) {
                zelda64::notify::post(std::string(boss_names[order - 1]) + " is waiting in " +
                                      boss_arenas[order - 1],
                                      zelda64::notify::Kind::ApReceived);
                log_line(std::string(boss_names[order - 1]) + "'s Soul is held");
            }
        }
    }

    // How an item is written in a notice. Everything the server sends this
    // slot is one of this game's own items, so every name is here: the ROM's
    // list for the ordinary ones, and our own two kinds on top of it. The
    // ROM shouts its names and writes an apostrophe as a backtick, so
    // "DRAGON`S POTION" is turned back into "Dragon's Potion".
    std::string pretty_item(int64_t item) {
        if (item == zelda64::archipelago::item_level_up) {
            return "Level Up";
        }
        int64_t soul = item - zelda64::archipelago::item_boss_soul;
        if (soul >= 1 && soul <= boss_count) {
            return std::string(boss_names[soul - 1]) + "'s Soul";
        }
        int64_t id = item - zelda64::archipelago::id_base - zelda64::archipelago::group_item;
        if (id < 0 || static_cast<size_t>(id) * 3 >= merrow::data::items.size()) {
            return "something unknown (" + std::to_string(item) + ")";
        }
        std::string name = merrow::data::items[static_cast<size_t>(id) * 3];
        bool word_start = true;
        for (char& c : name) {
            if (c == '`') {
                c = '\'';
                word_start = false;
            }
            else if (c == ' ') {
                word_start = true;
            }
            else if (word_start) {
                word_start = false;
            }
            else {
                c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
        }
        return name;
    }

    // The chest being opened, while one is: func_80002F60 keeps it here and
    // clears it once the chest is done with.
    constexpr int32_t current_chest = 0x8007BA7C;

    constexpr int32_t chest_flags = 0x800869D8;
    constexpr int chest_count = 88;
    constexpr int32_t spirit_flags = 0x80086AE8;
    constexpr int spirit_count = 98;

    constexpr int32_t gInventory = 0x8008CF78;
    constexpr int inventory_slots = 150;
    constexpr int32_t inventory_empty = 0xFF;

    // ---- the way into Mammon's World -------------------------------------
    //
    // The last door of the final staircase, in Baragoon Moor's submap 7. Its
    // exit record is at ROM 0x3C780C and every exit record carries its lock
    // in the four bytes at +0x14: a flags byte at +0x15 where 0x10 means
    // "an item opens this", and the item's id as a halfword at +0x16.
    // Vanilla is flags 0x16, item 24 (Eletale's Book), and beyond it lie the
    // two rooms that lead to Mammon's World itself.
    //
    // (Every other locked door in the game is one of the four gems, and the
    // one inside Mammon's World that wants the Dark Gaol Key. Found by
    // walking the map table's submap headers: map entry +0x18 gives the
    // 24-byte submap headers, +4 of each is the exit array and +8 its count,
    // and a record is 0x24 bytes with its destination at +0x1E.)
    constexpr uint32_t portal_rom = 0x3C780C;
    constexpr int portal_map = 11;
    constexpr int32_t gCurrentMap = 0x80084EEC;
    // -1 until a file is loaded, so this is how "a game is actually in
    // progress" is told from the title and the file-select screen.
    constexpr int32_t gNextMap = 0x80084EE4;
    constexpr int portal_item = 24;        // Eletale's Book, the vanilla key
    // Ids run 0-25, so nothing in the game is 26 and nothing ever matches:
    // the door stays shut however full the bag is. The id is only ever
    // compared against what is in the bag, never used to index anything.
    constexpr int portal_no_key = 26;
    int32_t portal_ram = 0;                // worked out at boot
    uint8_t portal_flags = 0;              // what the ROM says, to check RAM against
    uint32_t portal_tail = 0;
    bool portal_visit = false;             // said something this visit to the moor

    // Bosses 1-7. Mammon is the eighth and he is behind this door, so
    // counting him would be asking for the impossible.
    constexpr int bosses_before_mammon = 7;
    constexpr int monster_kinds = 67;

    std::vector<bool> chest_sent(chest_count, false);
    std::vector<bool> spirit_sent(spirit_count, false);
    // Enemies and bosses have no flag to watch, so unlike the two above
    // these are only remembered for this session; the server is what
    // actually holds them.
    std::vector<bool> enemy_sent(boss_first, false);
    std::vector<bool> boss_sent(boss_count, false);
    std::atomic<int> level_ups_waiting{ 0 };

    bool flag_set(uint8_t* rdram, int32_t base, int id) {
        return (MEM_BU(0, base + (id >> 3)) & (1u << (id & 7))) != 0;
    }

    // How much of what the portal is waiting for is done. Counted from the
    // server's list rather than from anything in the save, because a boss or
    // a monster leaves no flag behind; see checked_locations.
    struct PortalProgress {
        int bosses = 0, bosses_needed = 0;
        int monsters = 0, monsters_needed = 0;
        bool open() const { return bosses >= bosses_needed && monsters >= monsters_needed; }
    };

    PortalProgress portal_progress() {
        PortalProgress p;
        std::lock_guard lock{ server_mutex };
        if (mammon_portal & 1) {
            p.bosses_needed = bosses_before_mammon;
            for (int order = 1; order <= bosses_before_mammon; order++) {
                if (checked_locations.count(zelda64::archipelago::id_base +
                                            zelda64::archipelago::group_boss + order)) {
                    p.bosses++;
                }
            }
        }
        if (mammon_portal & 2) {
            p.monsters_needed = monster_kinds;
            for (int id = 0; id < monster_kinds; id++) {
                if (checked_locations.count(zelda64::archipelago::id_base +
                                            zelda64::archipelago::group_enemy + id)) {
                    p.monsters++;
                }
            }
        }
        return p;
    }

    // Holds the last door of the final staircase shut until the yaml's
    // condition is met, by giving it a key that does not exist. The door is
    // the game's own, so refusing it is the game's own refusal - no
    // transition is begun and nothing has to be unwound.
    //
    // Once the condition is met the door is simply open: the condition takes
    // the Eletale's Book's place rather than adding to it, so the Shannon
    // who hands the Book over is just another gift check. Open is Merrow's
    // unlocked value for this very record (unlockedDoorData entry 16): kind
    // 01, flags 01 (no item wanted), item 0.
    constexpr uint8_t portal_open_flags = 0x01;
    void guard_portal(uint8_t* rdram) {
        if (portal_ram == 0 || mammon_portal == 0) {
            return;
        }
        if (MEM_W(0, gCurrentMap) != portal_map) {
            portal_visit = false;
            return;
        }
        // Make sure that really is the record before writing into it. The
        // map is only just being DMA'd in on the frame it becomes current,
        // and a map file that ever moves would otherwise have this scribbling
        // on whatever is there instead. The flags byte and the destination
        // are ours to compare because neither is ever written here.
        uint8_t flags_now = static_cast<uint8_t>(MEM_BU(0, portal_ram + 0x15));
        if ((flags_now != portal_flags && flags_now != portal_open_flags) ||
            static_cast<uint32_t>(MEM_W(0, portal_ram + 0x20)) != portal_tail) {
            static bool said = false;
            if (!said) {
                said = true;
                char note[144];
                std::snprintf(note, sizeof note,
                              "the Mammon's World door does not look right in RAM "
                              "(flags %02X tail %08X, wanted %02X %08X); not touching it",
                              static_cast<uint8_t>(MEM_BU(0, portal_ram + 0x15)),
                              static_cast<uint32_t>(MEM_W(0, portal_ram + 0x20)),
                              portal_flags, portal_tail);
                log_line(note);
            }
            return;
        }
        PortalProgress progress = portal_progress();
        if (progress.open()) {
            MEM_B(0, portal_ram + 0x15) = static_cast<int8_t>(portal_open_flags);
            MEM_H(0, portal_ram + 0x16) = 0;
        }
        else {
            MEM_B(0, portal_ram + 0x15) = static_cast<int8_t>(portal_flags);
            MEM_H(0, portal_ram + 0x16) = static_cast<int16_t>(portal_no_key);
        }

        // Say so when it changes, and once each time the moor is walked into,
        // so the reason the door will not open is never a mystery. The notice
        // stack is the port's own, top-left.
        static int last_said = -1;
        int state = progress.open() ? -2 : progress.bosses * 100 + progress.monsters;
        if (state == last_said && portal_visit) {
            return;
        }
        last_said = state;
        portal_visit = true;
        if (progress.open()) {
            zelda64::notify::post("The way to Mammon's World is open");
            return;
        }
        std::string what;
        if (progress.bosses_needed) {
            what = std::to_string(progress.bosses) + "/" +
                   std::to_string(progress.bosses_needed) + " bosses";
        }
        if (progress.monsters_needed) {
            if (!what.empty()) {
                what += ", ";
            }
            what += std::to_string(progress.monsters) + "/" +
                    std::to_string(progress.monsters_needed) + " monsters";
        }
        zelda64::notify::post("Mammon's World is sealed: " + what);
    }

    // Puts an item in the bag. func_8000FFE8 searches the whole list rather
    // than indexing it, so any free slot will do; a full bag drops the item,
    // which is the same thing the game does.
    bool give_item(uint8_t* rdram, int item) {
        for (int slot = 0; slot < inventory_slots; slot++) {
            if (MEM_BU(0, gInventory + slot) == inventory_empty) {
                MEM_B(0, gInventory + slot) = static_cast<int8_t>(item);
                return true;
            }
        }
        return false;
    }

    void scan_flags(uint8_t* rdram, int32_t base, int count, std::vector<bool>& sent,
                    int64_t group, const char* what) {
        for (int id = 0; id < count; id++) {
            if (sent[id] || !flag_set(rdram, base, id)) {
                continue;
            }
            sent[id] = true;
            zelda64::archipelago::send_check(zelda64::archipelago::id_base + group + id);
            log_line(std::string(what) + " " + std::to_string(id) + " checked");
        }
    }
}

namespace {
    // Defined with the rest of the gift-NPC watch, at the end of the file.
    void locate_givers(const std::vector<uint8_t>& rom);
    void locate_portal(const std::vector<uint8_t>& rom);
    void locate_boss_locks(const std::vector<uint8_t>& rom);
    void open_boss_locks(uint8_t* rdram);
}

void zelda64::archipelago::on_frame(uint8_t* rdram) {
    if (!ap_enabled.load()) {
        return;
    }
    scan_flags(rdram, chest_flags, chest_count, chest_sent, group_chest, "chest");
    scan_flags(rdram, spirit_flags, spirit_count, spirit_sent, group_spirit, "spirit");
    if (!playing_seed()) {
        // Not connected: the original game. The door is the game's own, a
        // gift NPC gives as usual and a spirit opens its screen.
        return;
    }
    guard_portal(rdram);
    open_boss_locks(rdram);

    // Hand over whatever the server has sent that this save has not had.
    // Nothing happens until a file is loaded: items put in the bag before
    // that are written over by the save coming in, and the mark would have
    // moved past them for nothing. Nor while an item list is on screen: they
    // wait for it to close, so the list never changes under the cursor.
    if (MEM_W(0, gNextMap) != -1 && !zelda64::enhancements::item_menu_open(rdram)) {
        for (;;) {
            int64_t item = 0;
            {
                std::lock_guard lock{ server_mutex };
                if (items_applied >= static_cast<int>(server_items.size())) {
                    break;
                }
                item = server_items[static_cast<size_t>(items_applied)];
                items_applied++;
            }
            if (item == item_level_up) {
                int waiting = level_ups_waiting.fetch_add(1) + 1;
                // Nothing said here: the arrival already shows as "Received
                // Level Up", and the screen announces itself when it opens.
                log_line("a Level Up to hand over; " + std::to_string(waiting) + " waiting");
                continue;
            }
            // A Soul is not handed over at all - refresh_souls works those
            // out from the whole list, because they are a state rather than
            // a thing given once.
            int64_t soul = item - item_boss_soul;
            if (soul >= 1 && soul <= boss_count) {
                continue;
            }
            int64_t vanilla = item - id_base - group_item;
            if (vanilla <= 0 || vanilla > 0xFE) {
                log_line("item " + std::to_string(item) + " is not one of ours");
                continue;
            }
            bool taken = give_item(rdram, static_cast<int>(vanilla));
            log_line("gave item " + std::to_string(vanilla) + (taken ? "" : " but the bag is full"));
        }
    }

    // The spirit screen. A spirit no longer opens it (that is stopped at the
    // grab, quest64_archipelago_spirit_grab), but it is also the game's own
    // level-up screen: EXP from monsters opens it through the same bit
    // (func_800074A0, 0x8000799C). So it is never taken down here - doing
    // that turned ordinary level-ups off - and while it is up, whoever
    // opened it, a Level Up from the server waits its turn.
    uint32_t mask = static_cast<uint32_t>(MEM_W(0, menu_mask));
    if (mask & menu_spirit) {
        return;
    }
    // Hand over a Level Up when there is somewhere to put it: not in a
    // battle, and not while any other menu is up, which covers the pause
    // screen and the item menu without having to know their bits.
    if (level_ups_waiting.load() <= 0) {
        return;
    }
    // Every element at its cap: the element-choice screen would have nothing
    // to raise and could not be left, so a Level Up has nowhere to go. The
    // cap never comes down again, so they are let go rather than kept.
    if (zelda64::enhancements::elements_all_maxed(rdram)) {
        int dropped = level_ups_waiting.exchange(0);
        log_line(std::to_string(dropped) + " Level Up(s) dropped: every element is at its maximum");
        zelda64::notify::post("Every element is at its maximum: Level Up not needed",
                              zelda64::notify::Kind::ApReceived);
        return;
    }
    if (mask != 0 || (MEM_HU(0, gBattleState) & 1)) {
        // Whether the mask is ever 0 in ordinary play is the one guess here,
        // so say what is holding a Level Up up - once per value, or this
        // would be a line a frame.
        static uint32_t last_blocked = 0xFFFFFFFFu;
        uint32_t blocked = mask | ((MEM_HU(0, gBattleState) & 1) ? 0x80000000u : 0u);
        if (blocked != last_blocked) {
            last_blocked = blocked;
            char note[96];
            std::snprintf(note, sizeof note, "a Level Up is waiting; menu mask 0x%08X%s",
                          mask, (blocked & 0x80000000u) ? ", in a battle" : "");
            log_line(note);
        }
        return;
    }
    level_ups_waiting.fetch_sub(1);
    MEM_W(0, menu_mask) = static_cast<int32_t>(mask | menu_spirit);
    zelda64::notify::post("Level Up: choose an element", zelda64::notify::Kind::ApReceived);
    log_line("opened the spirit screen for a Level Up; " +
             std::to_string(level_ups_waiting.load()) + " still waiting");
}

// Boot. Nothing in the ROM changes any more: a chest is emptied as it is
// opened, and only while connected (quest64_archipelago_empty_chest), so an
// unconnected game is the original one.
void zelda64::archipelago::apply_at_boot(uint8_t* rdram) {
    (void)rdram;
    ap_enabled.store(load_options().enabled);
    rom_ready = true;
    if (ap_enabled.load()) {
        locate_once();
    }
}

namespace {
    void locate_once() {
        if (located || !rom_ready) {
            return;
        }
        std::span<const uint8_t> rom = recomp::get_rom();
        if (rom.empty()) {
            return;
        }
        located = true;
        std::vector<uint8_t> copy(rom.begin(), rom.end());
        locate_givers(copy);
        locate_portal(copy);
        locate_boss_locks(copy);
    }
}

// func_80002F60's chest branch, at 0x80003444: `lh $t2, 0x50($t9)` reads the
// item out of the chest being opened (t9) for the "you got ..." box and the
// hand-over. While the seed is in play the chest is emptied right there: 255
// is the game's own "nothing", the value Merrow's shuffle uses for an empty
// chest. The check still fires, because that comes from the chest's opened
// flag rather than from what was inside.
extern "C" void quest64_archipelago_empty_chest(uint8_t* rdram, recomp_context* ctx) {
    if (!playing_seed()) {
        return;
    }
    int32_t chest = static_cast<int32_t>(ctx->r25);
    if (chest != 0 && seed_has(zelda64::archipelago::group_chest, MEM_HU(0x62, chest))) {
        MEM_H(0x50, chest) = 0xFF;
    }
}

int zelda64::archipelago::pending_level_ups() {
    return level_ups_waiting.load();
}

// func_80009818 is the rewards routine, run once per monster that died, and
// at 0x800098D4 it reads that monster's experience out of its stat table
// entry: `lw $t7, 0x10($t6)`, where t6 came from `lw $t6, 0x64($s0)` - the
// defeated monster's entry. The hook sits after that load (0x800098D8,
// shared with Hard Mode's own), so t6 still holds the entry.
//
// The entry does not say which monster it is in any global sense: each of
// the six monster files numbers its own entries from 0, so reading an id out
// of it made every area's first monster "monster 0" and never reached a
// boss. The one list the locations use is Merrow's 75 (0-66 the ordinary
// monsters, 67-74 the bosses), and the ordinary ones are simply the six
// files one after another. So an ordinary monster is
//
//     first id of the file that is loaded + the entry's place in its table
//
// where the loaded file is the row of the file table (0x80054160, 20 bytes a
// row: ROM start, ROM end, table end, table start, ...) that 0x8007D0BC
// points at - the same row the Enemy Randomizer rescales on load.
//
// A boss's entry is in his own file, outside that table. Which boss is here
// is what func_8000B530 leaves at 0x8007D1A0 as the map loads: his index in
// gBossData plus one, 0 when there is none. gBossData is in story order and
// the boss-order shuffle only moves where each entry stands, so that number
// is the boss himself (Solvaring 1 ... Mammon 8).
//
// A repeat is harmless - the server ignores a location it already has - but
// the sent list keeps the traffic down, since an enemy type is checked once
// however many of them are killed.
namespace {
    constexpr int32_t monster_file_table = 0x80054160;
    constexpr int32_t monster_file_row = 0x8007D0BC;
    constexpr int32_t monster_file_row_size = 20;
    constexpr int32_t monster_entry_size = 0x38;
    constexpr int32_t boss_here = 0x8007D1A0;
    // Merrow's id of each file's first entry; the files hold 12, 15, 11,
    // 13, 12 and 4 entries, 67 in all.
    constexpr std::array<int, 6> file_first_id = { 0, 12, 27, 38, 51, 63 };
    constexpr std::array<int, 6> file_entries = { 12, 15, 11, 13, 12, 4 };

    // Merrow's id of the monster whose stat entry this is, or -1.
    int global_monster_id(uint8_t* rdram, int32_t entry) {
        int32_t row = MEM_W(0, monster_file_row);
        int file = (row - monster_file_table) / monster_file_row_size;
        if (row < monster_file_table || file < 0 || file >= static_cast<int>(file_first_id.size()) ||
            (row - monster_file_table) % monster_file_row_size != 0) {
            return -1;
        }
        int32_t table = MEM_W(0xC, row);
        int32_t end = MEM_W(0x8, row);
        if (entry < table || entry >= end || (entry - table) % monster_entry_size != 0) {
            return -1;
        }
        int local = (entry - table) / monster_entry_size;
        if (local >= file_entries[file]) {
            return -1;
        }
        return file_first_id[file] + local;
    }
}

extern "C" void quest64_archipelago_kill(uint8_t* rdram, recomp_context* ctx) {
    if (!ap_enabled.load()) {
        return;
    }
    int32_t entry = static_cast<int32_t>(ctx->r14);
    if (entry == 0) {
        return;
    }
    int index = global_monster_id(rdram, entry);
    if (index >= 0) {
        if (enemy_sent[index]) {
            return;
        }
        enemy_sent[index] = true;
        zelda64::archipelago::send_check(zelda64::archipelago::id_base +
                                         zelda64::archipelago::group_enemy + index);
        log_line("defeated monster " + std::to_string(index) + ", checked");
        return;
    }
    int order = MEM_W(0, boss_here);
    if (order < 1 || order > boss_count) {
        char note[128];
        std::snprintf(note, sizeof note,
                      "defeated something at 0x%08X that is neither in the monster file (row 0x%08X) "
                      "nor a boss; not checked", static_cast<uint32_t>(entry),
                      static_cast<uint32_t>(MEM_W(0, monster_file_row)));
        log_line(note);
        return;
    }
    if (boss_sent[order - 1]) {
        return;
    }
    boss_sent[order - 1] = true;
    zelda64::archipelago::send_check(zelda64::archipelago::id_base +
                                     zelda64::archipelago::group_boss + order);
    log_line(std::string("defeated boss ") + std::to_string(order) + " (" + boss_names[order - 1] + "), checked");
    // Mammon is the last of them, and the run is over.
    if (order == boss_count) {
        zelda64::archipelago::goal_reached();
        log_line("Mammon is down: the goal is reached");
    }
}

// Boss Souls: a boss whose Soul has not arrived is simply not there.
//
// func_8000B530 runs as a map loads and is the whole of how a boss gets into
// the world. It walks gBossData (0x803A9AD0, eight entries of map, submap,
// file index and position, in story order), and for the entry matching the
// map being loaded it calls func_8000B9D8 with the boss's index - the test
// for "this one has already been beaten", bit `index` of the byte at
// 0x8007D19C. Only if that comes back zero does it DMA the boss's file to
// gBossVAddr and place him.
//
// So saying "already beaten" is exactly saying "not here", and it is a state
// the game deals with every time you walk back into a cleared arena: nothing
// is loaded, nothing is placed, and there is nothing left behind to walk into.
// It also needs no undoing - the Soul arriving simply lets the next load
// through, and the save's own flags are never touched.
//
// The hook sits at 0x8000B58C, the instruction that tests what came back,
// rather than on func_8000B9D8 itself: the same routine answers for
// func_800086E4 too, and that one has nothing to do with spawning. At this
// point v0 is the answer and s0 is still the boss's index.
extern "C" void quest64_archipelago_boss_soul(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    if (!playing_seed()) {
        return;
    }
    int mode = boss_souls.load();
    if (mode == 0) {
        return;
    }
    int order = static_cast<int>(ctx->r16) + 1;
    if (order < 1 || order > boss_count) {
        return;
    }
    // Mammon only has a Soul when the yaml asked for one.
    if (order == boss_count && mode < 2) {
        return;
    }
    if (souls_held.load() & (1u << order)) {
        return;
    }
    ctx->r2 = 1;   // "already beaten": do not load him, do not place him
    static uint32_t said = 0;
    if (!(said & (1u << order))) {
        said |= 1u << order;
        log_line(std::string(boss_names[order - 1]) + " is not in the game: no Soul yet");
    }
}

// ---- the gift NPCs
//
// The sixteen are found through their records: Merrow's itemgranters holds
// the ROM address of each one's item byte, which is +7 of a type-2 record in
// its map's NPC table { u16 id, u16 type, ..., u8 item at +7, u16 message
// when not held +8, u16 message when held +0xA }. The map table turns that
// into the RAM address the record has while its map is loaded.
//
// The game keeps no "given already" state: func_800086E4, talking to an
// NPC, asks func_80021240 whether the bag holds the record's item, and gives
// it (func_800212A0) only if not. So in vanilla a giver hands over again
// whenever you no longer have the thing, and never while you do. In
// Archipelago mode the bag is not asked at all: see
// quest64_archipelago_giver_talk below.
namespace {
    constexpr int giver_count = 16;

    struct Giver {
        int map = -1;
        int32_t item_ram = 0;
        uint8_t item = 0;
    };
    std::array<Giver, giver_count> givers{};
    std::array<bool, giver_count> giver_sent{};

    // The map table, in the boot segment: 0x44 bytes a map, 36 maps, +4 the
    // file's ROM start, +8 its end, +0xC where it is loaded in RAM.
    constexpr uint32_t map_table_rom = 0x80054F10 - 0x80000450 + 0x1050;
    constexpr int map_table_maps = 36;
    constexpr uint32_t map_table_stride = 0x44;

    uint32_t be32(const std::vector<uint8_t>& rom, size_t at) {
        return (static_cast<uint32_t>(rom[at]) << 24) | (static_cast<uint32_t>(rom[at + 1]) << 16) |
            (static_cast<uint32_t>(rom[at + 2]) << 8) | rom[at + 3];
    }

    void locate_givers(const std::vector<uint8_t>& rom) {
        for (int i = 0; i < giver_count; i++) {
            if (static_cast<size_t>(i) * 2 + 1 >= merrow::data::itemgranters.size()) {
                break;
            }
            uint32_t address = static_cast<uint32_t>(merrow::data::itemgranters[i * 2]);
            givers[i].item = static_cast<uint8_t>(merrow::data::itemgranters[i * 2 + 1]);
            for (int m = 0; m < map_table_maps; m++) {
                size_t row = map_table_rom + static_cast<size_t>(m) * map_table_stride;
                if (row + 0x10 > rom.size()) {
                    break;
                }
                uint32_t start = be32(rom, row + 4);
                uint32_t end = be32(rom, row + 8);
                uint32_t dest = be32(rom, row + 0xC);
                if (address >= start && address < end) {
                    givers[i].map = m;
                    givers[i].item_ram = static_cast<int32_t>(address - start + dest);
                    break;
                }
            }
            char note[128];
            std::snprintf(note, sizeof note, "giver %2d: item %3d, ROM 0x%06X, map %d, RAM 0x%08X",
                          i, givers[i].item, address, givers[i].map,
                          static_cast<uint32_t>(givers[i].item_ram));
            log_line(note);
        }
    }

    // Where the portal's exit record lands in RAM. Every map file is DMA'd to
    // the same address, so this is only the right place while Baragoon Moor
    // is the map that is loaded - which is exactly when the door is looked
    // at. Worked out the same way a giver's item byte is, and checked
    // against what the ROM says is there: if the record has moved, the guard
    // stays out of it rather than writing over whatever is there now.
    void locate_portal(const std::vector<uint8_t>& rom) {
        portal_ram = 0;
        size_t row = map_table_rom + static_cast<size_t>(portal_map) * map_table_stride;
        if (row + 0x10 > rom.size()) {
            return;
        }
        uint32_t start = be32(rom, row + 4);
        uint32_t end = be32(rom, row + 8);
        uint32_t dest = be32(rom, row + 0xC);
        if (portal_rom < start || portal_rom >= end) {
            log_line("the Mammon's World door is not in map 11; the portal gate is off");
            return;
        }
        uint8_t flags = rom[portal_rom + 0x15];
        uint16_t item = static_cast<uint16_t>((rom[portal_rom + 0x16] << 8) | rom[portal_rom + 0x17]);
        if (!(flags & 0x10) || item != portal_item) {
            char note[128];
            std::snprintf(note, sizeof note,
                          "the Mammon's World door reads flags %02X item %d, not a locked "
                          "Eletale's Book door; the portal gate is off", flags, item);
            log_line(note);
            return;
        }
        portal_ram = static_cast<int32_t>(portal_rom - start + dest);
        portal_flags = flags;
        portal_tail = be32(rom, portal_rom + 0x20);
        char note[112];
        std::snprintf(note, sizeof note,
                      "Mammon's World door: ROM 0x%06X, RAM 0x%08X, flags %02X, wants item %d",
                      portal_rom, static_cast<uint32_t>(portal_ram), flags, item);
        log_line(note);
    }

    // ---- the boss-item locks
    //
    // With Boss Souls a boss may not be there to beat, so the four items the
    // bosses drop - Earth Orb 0x14, Wind Jewel 0x15, Water Jewel 0x16, Fire
    // Ruby 0x17 - can no longer be what the way forward waits on. Each lock
    // is an exit record in a map file (the door, boat or teleporter), and
    // its word at +0x14 is { u8 kind, u8 flags, u16 item }: flag 0x10 is
    // "needs the item", 0x20 "blocked once you have it" (Epona's teleporter
    // back to the boat, which the Water Jewel turns off).
    //
    // What they are opened to is exactly Merrow's "Unlock progression locks"
    // (merrow::data::unlockedDoorData, entries 0-15). Entries 16 and 17, the
    // Eletale Book's door and the Dark Gaol Key's, are not boss items and are
    // left to the Mammon's World gate above. Epona's hut follows Merrow too:
    // its two overlapping teleporters both go to Colleen's side.
    //
    // Merrow writes these into the ROM at boot; here they have to wait for
    // the server to say Boss Souls is on, so they are written into the map
    // as it sits in RAM instead, like the portal door. A map file is DMA'd
    // afresh on every load, so this is done on every frame the map is
    // current and undoes itself the moment it stops: disconnect, or play
    // without Boss Souls, and the next load is the game's own again.
    struct BossLock {
        int map = -1;
        int32_t ram = 0;
        std::vector<uint8_t> original;   // what the ROM holds, from +0x10
        std::vector<uint8_t> opened;     // what goes at +0x14
    };
    std::vector<BossLock> boss_locks;
    constexpr int boss_lock_count = 16;

    std::vector<uint8_t> from_hex(const std::string& hex) {
        std::vector<uint8_t> out;
        for (size_t i = 0; i + 1 < hex.size(); i += 2) {
            out.push_back(static_cast<uint8_t>(std::stoul(hex.substr(i, 2), nullptr, 16)));
        }
        return out;
    }

    void locate_boss_locks(const std::vector<uint8_t>& rom) {
        boss_locks.clear();
        for (int i = 0; i < boss_lock_count; i++) {
            if (static_cast<size_t>(i) * 2 + 1 >= merrow::data::unlockedDoorData.size()) {
                break;
            }
            uint32_t address = static_cast<uint32_t>(
                std::stoul(merrow::data::unlockedDoorData[i * 2], nullptr, 16));
            BossLock lock;
            lock.opened = from_hex(merrow::data::unlockedDoorData[i * 2 + 1]);
            // The record from +0x10 (four bytes before the patch) through the
            // end of the patch, so the check that it is really there covers
            // more than the bytes being changed.
            uint32_t from = address - 4;
            size_t length = 4 + lock.opened.size();
            for (int m = 0; m < map_table_maps; m++) {
                size_t row = map_table_rom + static_cast<size_t>(m) * map_table_stride;
                if (row + 0x10 > rom.size()) {
                    break;
                }
                uint32_t start = be32(rom, row + 4);
                uint32_t end = be32(rom, row + 8);
                uint32_t dest = be32(rom, row + 0xC);
                if (from >= start && address + lock.opened.size() <= end) {
                    lock.map = m;
                    lock.ram = static_cast<int32_t>(from - start + dest);
                    break;
                }
            }
            if (lock.map < 0 || from + length > rom.size()) {
                log_line("boss lock " + std::to_string(i) + " is not in any map file; left alone");
                continue;
            }
            lock.original.assign(rom.begin() + from, rom.begin() + from + length);
            if (std::equal(lock.opened.begin(), lock.opened.end(), lock.original.begin() + 4)) {
                // Already open in the ROM (Unlock progression locks, Lost Keys).
                continue;
            }
            boss_locks.push_back(std::move(lock));
        }
        log_line("boss-item locks: " + std::to_string(boss_locks.size()) + " to open when Boss Souls is on");
    }

    void open_boss_locks(uint8_t* rdram) {
        if (boss_souls.load() == 0) {
            return;
        }
        int map = static_cast<int32_t>(MEM_W(0, gCurrentMap));
        for (const BossLock& lock : boss_locks) {
            if (lock.map != map) {
                continue;
            }
            // Written only over the record the ROM says is there: the map is
            // still arriving on the frame it becomes current.
            bool vanilla = true;
            for (size_t i = 0; i < lock.original.size() && vanilla; i++) {
                vanilla = static_cast<uint8_t>(MEM_BU(static_cast<int32_t>(i), lock.ram)) == lock.original[i];
            }
            if (!vanilla) {
                continue;
            }
            for (size_t i = 0; i < lock.opened.size(); i++) {
                MEM_B(static_cast<int32_t>(4 + i), lock.ram) = static_cast<int8_t>(lock.opened[i]);
            }
            char note[96];
            std::snprintf(note, sizeof note, "opened a boss-item lock in map %d (RAM 0x%08X)",
                          map, static_cast<uint32_t>(lock.ram + 4));
            log_line(note);
        }
    }

}

// func_800086E4 at 0x80008814, just after `jal func_80021240` asked whether
// the bag holds the item of the NPC being talked to: v0 is the answer, s0 the
// NPC and [s0+0x80] its record. Non-zero means "held": the NPC says the
// line at +0xA and gives nothing; zero means the line at +8 and the item.
//
// While the seed is in play a giver's answer ignores the bag. The first time,
// it is "not held": the NPC hands over, the check goes, and the item itself
// is kept out of the bag (quest64_archipelago_skip_gift_item). From then on,
// once this session has sent the check or the server says it has it, it is
// "held", so the NPC never gives twice - not even after a reload, since the
// server's list arrives with every connection.
namespace {
    bool skip_next_add = false;
}

extern "C" void quest64_archipelago_giver_talk(uint8_t* rdram, recomp_context* ctx) {
    if (!playing_seed()) {
        return;
    }
    int32_t npc = static_cast<int32_t>(ctx->r16);
    int32_t record = npc != 0 ? static_cast<int32_t>(MEM_W(0x80, npc)) : 0;
    if (record == 0) {
        return;
    }
    int map = static_cast<int32_t>(MEM_W(0, gCurrentMap));
    for (int i = 0; i < giver_count; i++) {
        const Giver& giver = givers[i];
        if (giver.map != map || giver.item_ram == 0 || giver.item_ram - 7 != record) {
            continue;
        }
        int64_t location = zelda64::archipelago::id_base + zelda64::archipelago::group_giver + i;
        bool done = giver_sent[i];
        {
            std::lock_guard lock{ server_mutex };
            if (slot_locations.count(location) == 0) {
                return;   // not a check in this seed: the NPC gives as usual
            }
            done = done || checked_locations.count(location) != 0;
        }
        if (done) {
            giver_sent[i] = true;
            ctx->r2 = 1;
            return;
        }
        giver_sent[i] = true;
        ctx->r2 = 0;
        skip_next_add = true;
        zelda64::archipelago::send_check(location);
        log_line("giver " + std::to_string(i) + " checked on map " + std::to_string(map) +
                 " (item " + std::to_string(MEM_BU(7, record)) + " kept out of the bag)");
        return;
    }
}

// func_800212A0, at its first instruction: put item a0 in the bag. A leaf
// that never moves the stack, so returning here skips it cleanly. Only the
// one add that follows a giver's check is skipped; the flag is set at
// 0x80008814 and the add is at 0x80008850, on the same path.
extern "C" int quest64_archipelago_skip_gift_item(uint8_t* rdram, recomp_context* ctx) {
    (void)rdram;
    (void)ctx;
    if (!skip_next_add) {
        return 0;
    }
    skip_next_add = false;
    return 1;
}


namespace {
    // A (re)connect starts the outgoing queue empty, so whatever was noticed
    // while there was nowhere to send it would be lost. The chests and
    // spirits are not, because their flags are still set in the save:
    // forgetting that they were sent makes the next frame's scan find them
    // all again. Enemies, bosses and gifts have no flag, so those are the
    // ones a long spell offline can cost.
    void forget_sent_checks() {
        std::fill(chest_sent.begin(), chest_sent.end(), false);
        std::fill(spirit_sent.begin(), spirit_sent.end(), false);
    }
}

// func_80006604, at its first instruction: the chest's "you got ..." box.
// It is reached from one place only, the chest branch of func_80002F60, so
// refusing the call is the same as taking that call out - and doing it at
// the callee is what the recomp allows, since a hook cannot skip a jal and
// returning out of func_80002F60 itself would drop the work after it.
//
// With the chests emptied there is nothing to announce anyway: the box would
// show item 255. The chest has been opened and its flag set before this is
// reached, so the check still fires - only the box goes.
//
// Which chest it was is known here too: 0x8007BA7C holds the one being
// opened and its flag id sits at +0x62, the same number that indexes the
// flag array and names the location.
// func_80002F60's spirit grab. At 0x800032D8 it ORs the spirit screen's bit
// into the menu mask and stores it (in a branch delay slot, so not a place a
// hook can go); every path that stores it then falls into 0x80003318. Taking
// the bit straight back out there means the screen never opens at all -
// clearing it from on_frame instead left it on screen for a few frames. The
// spirit is still taken and its flag set just after (0x80003320), which is
// what sends the check.
extern "C" void quest64_archipelago_spirit_grab(uint8_t* rdram) {
    if (!playing_seed()) {
        return;
    }
    // The spirit being taken: 0x8007BA78 points at it, and its id at +0x14
    // is what 0x80003320 hands to the flag routine just after.
    constexpr int32_t current_spirit = 0x8007BA78;
    int32_t spirit = static_cast<int32_t>(MEM_W(0, current_spirit));
    if (spirit == 0 || !seed_has(zelda64::archipelago::group_spirit, MEM_BU(0x14, spirit))) {
        return;
    }
    MEM_W(0, menu_mask) = static_cast<int32_t>(static_cast<uint32_t>(MEM_W(0, menu_mask)) & ~menu_spirit);
}

extern "C" int quest64_archipelago_hide_chest_text(uint8_t* rdram) {
    if (!playing_seed()) {
        return 0;
    }
    int32_t chest = static_cast<int32_t>(MEM_W(0, current_chest));
    if (chest == 0 || !seed_has(zelda64::archipelago::group_chest, MEM_HU(0x62, chest))) {
        return 0;   // not a check in this seed: the game's own box
    }
    {
        log_line("chest " + std::to_string(MEM_HU(0, chest + 0x62)) + " opened, no box shown");
    }
    return 1;
}

void zelda64::archipelago::save_progress(const std::string& save_key) {
    if (!ap_enabled.load()) {
        return;
    }
    load_progress_file();
    int applied = 0;
    {
        std::lock_guard lock{ server_mutex };
        applied = items_applied;
    }
    save_progress_by_key[save_key] = applied;
    nlohmann::json j = nlohmann::json::object();
    for (const auto& [key, count] : save_progress_by_key) {
        j[key] = count;
    }
    std::ofstream out(progress_path());
    if (out.is_open()) {
        out << j.dump(2);
    }
    log_line("saved: " + save_key + " has had " + std::to_string(applied) + " item(s)");
}

void zelda64::archipelago::load_progress(const std::string& save_key) {
    if (!ap_enabled.load()) {
        return;
    }
    load_progress_file();
    auto it = save_progress_by_key.find(save_key);
    int applied = it == save_progress_by_key.end() ? 0 : it->second;
    {
        std::lock_guard lock{ server_mutex };
        items_applied = applied;
    }
    // A Level Up the file was saved without spending is not owed twice
    // either: what this save is due is the mark, and nothing else.
    level_ups_waiting.store(0);
    log_line("loaded: " + save_key + " has had " + std::to_string(applied) + " item(s)");
}
