#pragma once
#include "../common/Types.h"
#include "../dsp/DelayLine.h"
#include "../dsp/ParamSmooth.h"
#include <array>

namespace atmos {

class SurroundGen {
public:
    SurroundGen();
    void setSampleRate(f32 sr);
    void setLevelDb(f32 db);
    void setWidth(f32 w);
    void processBlock(f32 sideL, f32 sideR, f32& outLs, f32& outRs, i32 numFrames, bool centerPresent);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 level_ = 0.5f;
    f32 width_ = 0.7f;

    DelayLine lsDelay_, rsDelay_;
    AllpassDecorrelator lsDec_, rsDec_;
    ParamSmooth smoothLs_, smoothRs_;
};

}
