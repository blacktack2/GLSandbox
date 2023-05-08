#pragma once
#include <chrono>
#include <vector>

class Timer {
public:
    typedef std::chrono::duration<float> delta_time_t;

    explicit Timer(std::size_t averageFrameCount);
    ~Timer() = default;

    [[nodiscard]] delta_time_t tick();
private:
    std::chrono::time_point<std::chrono::steady_clock> mLastTime;
    std::vector<delta_time_t> mTimes;
    delta_time_t mTotal{0};

    std::size_t mCurrentIndex = 1;

    const std::size_t mAverageFrameCount;
    const float mInverseAverageFrameCount;
};

