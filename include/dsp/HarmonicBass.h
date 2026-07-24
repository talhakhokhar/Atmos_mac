#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"

namespace atmos {

class HarmonicBass {
public:
    HarmonicBass();
    void setSampleRate(f32 sr);
    void setEnabled(bool en);
    void setAmount(f32 amt);
    void setFrequency(f32 freq);
    f32 process(f32 x);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    bool enabled_ = false;
    f32 amount_ = 0.3f;
    f32 freq_ = 60.0f;

    Biquad lp2_;
    Biquad lp3_;
    Biquad inputBp_;
    f32 h2State_ = 0.0f;
    f32 h3State_ = 0.0f;
};

}
