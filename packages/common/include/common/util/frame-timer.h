#pragma once

#include <chrono>

/**
 * Timer that can used to time frames. Can be set to either total frame time,
 * or to a specific tick rate.
 */
class CFrameTimer {
public:
    /**
     * @param nTickRate The tick rate of the time, e.g. 60 for 60HZ
     */
    CFrameTimer(int nTickRate);

    /**
     * Accumulates the passed time since the last call to `Frame()`.
     * Indicates whether enough time has passed to constitute a tick.
     * Also immediately starts counting towards the next tick,
     * carrying over any excess time from the last tick.
     * 
     * @return `true` if enough time has passed for a new tick
     */
    bool Frame();

    /**
     * Get the float delta time for one frame, e.g. 16.6666 for 60HZ.
     * @return A float indicating the delta time that one frame takes in this timer.
     */
    float GetFrameDeltaTime();

private:
    // Time for one tick in nanoseconds
    std::chrono::nanoseconds llFrameTimeNS;
    // Time point of last frame tick
    std::chrono::time_point<std::chrono::high_resolution_clock> tpLastFrame;
    // Time that has passed in total since the last tick
    std::chrono::nanoseconds llAccumulatedTimeNS;
    // Delta time that one frame takes in this timer
    float fDelta;
};