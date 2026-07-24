#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"

namespace atmos {

class Crossfeed {
public:
    Crossfeed();
    void setSampleRate(f32 sr);
    void setLevel(f32 level);
    void setLowpass(f32 freq);
    void setTarget(OutputTarget target);
    void process(f32& inL, f32& inR);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 level_ = 0.5f;
    f32 lowpass_ = 700.0f;
    OutputTarget target_ = OutputTarget::Headphones;

    Biquad lpfL_, lpfR_;
    f32 delayL_ = 0.0f, delayR_ = 0.0f;
    f32 x1L_ = 0.0f, x1R_ = 0.0f;
};

}
