#pragma once
#include "../common/Types.h"
#include <array>

namespace atmos {

class ParamSmooth {
public:
    ParamSmooth() = default;
    void setSampleRate(f32 sr) { sampleRate_ = sr; }
    void setTimeConstant(f32 tcSec) { alpha_ = 1.0f - std::exp(-1.0f / (tcSec * sampleRate_)); }
    void setTarget(f32 t) { target_ = t; if (!initialized_) { current_ = t; initialized_ = true; } }
    f32 process() { current_ += alpha_ * (target_ - current_); return current_; }
    f32 getCurrent() const { return current_; }
    void reset() { current_ = target_; initialized_ = true; }
private:
    f32 sampleRate_ = 48000.0f;
    f32 alpha_ = 0.1f;
    f32 target_ = 0.0f;
    f32 current_ = 0.0f;
    bool initialized_ = false;
};

}
