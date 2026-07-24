#pragma once
#include "../common/Types.h"
#include <vector>
#include <memory>

namespace atmos {

class DelayLine {
public:
    DelayLine();
    explicit DelayLine(i32 maxDelay);
    void setMaxDelay(i32 maxDelay);
    void reset();
    void write(f32 x);
    f32 read(f32 delaySamples) const;
    f32 readLinear(f32 delaySamples) const;
    f32 readHermite(f32 delaySamples) const;
    i32 getMaxDelay() const { return maxDelay_; }
    i32 getWritePos() const { return writePos_; }

private:
    std::vector<f32> buffer_;
    i32 maxDelay_ = 0;
    i32 writePos_ = 0;
    mutable i32 readPos_ = 0;
};

class FractionalDelay {
public:
    FractionalDelay();
    void setDelay(f32 samples);
    f32 process(f32 x);
    void reset();

private:
    DelayLine delay_;
    f32 delaySamples_ = 0.0f;
    f32 frac_ = 0.0f;
    f32 x1_ = 0.0f, x2_ = 0.0f, x3_ = 0.0f;
    f32 y1_ = 0.0f, y2_ = 0.0f, y3_ = 0.0f;
};

class AllpassDecorrelator {
public:
    AllpassDecorrelator();
    void setDelay(f32 samples);
    void setGain(f32 gain);
    f32 process(f32 x);
    void reset();

private:
    DelayLine delay_;
    f32 gain_ = 0.5f;
    f32 delaySamples_ = 0.0f;
    f32 history_ = 0.0f;
};

}
