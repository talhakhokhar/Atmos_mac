#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"
#include "../dsp/ParamSmooth.h"
#include "../dsp/LoudnessMeter.h"

namespace atmos {

class LoudnessNormalizer {
public:
    LoudnessNormalizer();
    void setSampleRate(f32 sr);
    void setTargetLUFS(f32 target);
    void setEnabled(bool en);
    void setAttack(f32 ms);
    void setRelease(f32 ms);
    f32 process(const f32* inL, const f32* inR, f32* outL, f32* outR, i32 numFrames);
    f32 getGainReduction() const { return gainReductionDb_; }
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 targetLUFS_ = -16.0f;
    bool enabled_ = true;
    f32 gainReductionDb_ = 0.0f;
    f32 currentGain_ = 1.0f;

    LoudnessMeter meter_;
    ParamSmooth smoothGain_;
    ParamSmooth smoothTarget_;
    f32 prevLoudness_ = -70.0f;
};

}
