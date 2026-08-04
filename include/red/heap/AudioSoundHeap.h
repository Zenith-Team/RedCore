#pragma once

namespace red {

class AudioSoundHeap {
public:
    static void saveState();
    static void loadState();

    static int getState() { return sState; }

private:
    AudioSoundHeap();

    static int sState;
};

} // namespace red
