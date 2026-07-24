#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"
#include <array>

namespace atmos {

class LoudnessMeter {
public:
    LoudnessMeter();
    void setSampleRate(f32 sr);
    void setBlockSize(i32 frames);
    f32 process(const f32* inL, const f32* inR, i32 numFrames);
    f32 getIntegratedLoudness() const { return integratedLUFS_; }
    f32 getShortTermLoudness() const { return shortTermLUFS_; }
    f32 getMomentaryLoudness() const { return momentaryLUFS_; }
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 blockSizeSec_ = 0.4f;
    f32 integratedLUFS_ = -70.0f;
    f32 shortTermLUFS_ = -70.0f;
    f32 momentaryLUFS_ = -70.0f;

    Biquad kFilterL_, kFilterR_;
    Biquad highPassL_, highPassR_;
    f32 squareSum_ = 0.0f;
    i32 blockFrames_ = 19200;
    i32 frameCount_ = 0;
    f32 shortTermSum_ = 0.0f;
    i32 shortTermFrames_ = 0;
    f32 momentarySum_ = 0.0f;
    i32 momentaryFrames_ = 0;
    f32 totalFrames_ = 0.0f;
    f32 totalSquare_ = 0.0f;
};

}
