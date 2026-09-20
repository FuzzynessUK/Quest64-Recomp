#ifndef __SPEEDRUN_H__
#define __SPEEDRUN_H__

#include <string>

// Run timer for the Enhancements tab. Starts when Start Game is chosen and
// stops the moment Mammon's defeat message is shown, so it measures the run
// rather than wall-clock time with the game open.
namespace zelda64::speedrun {
    // Arms the timer when the game is launched from the boot menu.
    void arm();
    // Starts the run. Called once the title screen has handed over.
    void start();
    // Run once per frame; starts the run on the frame the title ends.
    void update(bool title_running);
    // Called from the hook on the Mammon defeat message.
    void stop();
    void reset();

    bool running();
    bool finished();

    // The elapsed time, formatted per the configured detail level. Keeps its
    // final value once the run has finished.
    std::string display();
}

#endif
