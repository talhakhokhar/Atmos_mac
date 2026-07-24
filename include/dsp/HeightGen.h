#pragma once
#include "../common/Types.h"
#include "../dsp/DelayLine.h"
#include "../dsp/Biquad.h"
#include "../dsp/ParamSmooth.h"

namespace atmos {

class HeightGen {
public:
    HeightGen();
    void setSampleRate(f32 sr);
    void setLevelDb(f32 db);
    void setElevation(f32 elev);
    void processBlock(f32 mid, f32 side, f32& outHf, f32& outHr, i32 numFrames);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 level_ = 0.3f;
    f32 elevation_ = 35.0f;

    DelayLine frontDelay_, rearDelay_;
    AllpassDecorrelator frontDec_, rearDec_;
    Biquad hipass_;
    ParamSmooth smoothHf_, smoothHr_;
};

}
