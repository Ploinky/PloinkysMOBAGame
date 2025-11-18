#include <common/util/frame-timer.h>

CFrameTimer::CFrameTimer(int nTickRate) {
    std::chrono::seconds oneSecond(1);
    std::chrono::nanoseconds oneSecondNS = std::chrono::duration_cast<std::chrono::nanoseconds>(oneSecond);
    llFrameTimeNS = oneSecondNS / nTickRate;
    llAccumulatedTimeNS = std::chrono::nanoseconds(0);
    fDelta = std::chrono::duration_cast<std::chrono::milliseconds>(llFrameTimeNS).count() / std::chrono::duration_cast<std::chrono::milliseconds>(oneSecond).count();
    tpLastFrame = std::chrono::high_resolution_clock::now();
}

bool CFrameTimer::Frame() {
    std::chrono::time_point tpNow = std::chrono::high_resolution_clock::now();
    std::chrono::duration durSinceLast = tpNow - tpLastFrame;
    tpLastFrame = tpNow;
    llAccumulatedTimeNS += durSinceLast;
    if(llAccumulatedTimeNS >= llFrameTimeNS) {
        llAccumulatedTimeNS -= llFrameTimeNS;
        return true;
    }

    return false;
}

float CFrameTimer::GetFrameDeltaTime() {
    return fDelta;
}