#include "TimerUtils.h"

Timer::Timer(std::size_t averageFrameCount) :
mAverageFrameCount(averageFrameCount), mInverseAverageFrameCount(1.0f / (float)averageFrameCount),
mTimes(averageFrameCount), mLastTime(std::chrono::steady_clock::now()) {

}

Timer::delta_time_t Timer::tick() {
    std::size_t nextIndex = (mCurrentIndex == mAverageFrameCount - 1) ? 0 : mCurrentIndex + 1;
    auto currentTime = std::chrono::steady_clock::now();

    mTotal = mTotal - mTimes[nextIndex];

    mTimes[mCurrentIndex] = currentTime - mLastTime;

    mTotal = mTotal + mTimes[mCurrentIndex];

    mLastTime = currentTime;
    mCurrentIndex = nextIndex;

    return mTotal * mInverseAverageFrameCount;
}
