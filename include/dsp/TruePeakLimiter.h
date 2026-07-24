#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"

namespace atmos {

class TruePeakLimiter {
public:
    TruePeakLimiter();
    void setSampleRate(f32 sr);
    void setCeiling(f32 db);
    void setLookahead(f32 ms);
    void setRelease(f32 ms);
    void setMaxGainReduction(f32 db);
    f32 process(f32& inL, f32& inR);
    f32 getGainReduction() const { return gainReduction_; }
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 ceilingDb_ = -1.0f;
    f32 ceilingLin_ = 0.891f;
    f32 lookaheadMs_ = 5.0f;
    f32 releaseMs_ = 150.0f;
    f32 maxGainReductionDb_ = 6.0f;

    Biquad lowpass_;
    std::vector<f32> delayBuffer_;
    i32 delayWrite_ = 0;
    i32 delayRead_ = 0;
    i32 delaySize_ = 0;
    f32 gainReduction_ = 0.0f;
    f32 currentGain_ = 1.0f;
};

}
