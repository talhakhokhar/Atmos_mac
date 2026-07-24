#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"
#include "../dsp/DelayLine.h"
#include "../dsp/ParamSmooth.h"

namespace atmos {

class CenterExtractor {
public:
    CenterExtractor();
    void setSampleRate(f32 sr);
    void setAlpha(f32 a);
    void setMode(f32 mode);
    void processBlock(const f32* inL, const f32* inR, f32* outC, f32* outLDirect, f32* outRDirect, i32 numFrames);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 mode_ = 0.0f;

    Biquad bandpassLow_;
    Biquad bandpassHigh_;
    AllpassDecorrelator decorrelator_;
    DelayLine shortDelay_;
    ParamSmooth smoothC_;
    ParamSmooth smoothAlpha_;

    f32 prevCorrelation_ = 0.0f;
    f32 prevCenterEnergy_ = 0.0f;
};

}
