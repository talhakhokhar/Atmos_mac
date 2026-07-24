#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"
#include "../dsp/ParamSmooth.h"

namespace atmos {

class DialogueEnhancer {
public:
    DialogueEnhancer();
    void setSampleRate(f32 sr);
    void setEnabled(bool en);
    void setAmount(f32 amt);
    void process(f32 center, f32& inL, f32& inR);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    bool enabled_ = false;
    f32 amount_ = 0.5f;

    Biquad bodyBoost_;
    Biquad presenceBoost_;
    Biquad deEss_;
    ParamSmooth smoothBody_, smoothPresence_, smoothDeess_;
    f32 env_ = 0.0f;
};

}
