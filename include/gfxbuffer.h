#ifndef __GFXBUFFER_H__
#define __GFXBUFFER_H__

// The game builds each frame's display list in a fixed 0x5000-byte area
// (2560 commands) with no check on running past it; Rock Shower in Blue
// Cave, or a busy Real Time Combat battle, did. The list is now built in a
// 512 KB buffer of its own; src/game/gfxbuffer.cpp explains. Also the crash
// log, crash.txt (+ crash.dmp) in the app folder.
namespace zelda64::gfxbuffer {
    // Main thread, as early as possible: writes crash.txt / crash.dmp on an
    // unhandled exception.
    void install_crash_handler();
}

#endif
