#include "HeightGen.h"

namespace atmos {

HeightGen::HeightGen() {
    hipass_.setType(BiquadType::HighPass);
    frontDelay_.setMaxDelay(static_cast<i32>(48000.0f * 0.035f));
    rearDelay_.setMaxDelay(static_cast<i32>(48000.0f * 0.035f));
}

void HeightGen::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    frontDelay_.setMaxDelay(static_cast<i32>(sr * 0.035f));
    rearDelay_.setMaxDelay(static_cast<i32>(sr * 0.035f));
    hipass_.setSampleRate(sr);
    smoothHf_.setSampleRate(sr);
    smoothHr_.setSampleRate(sr);
}

void HeightGen::setLevelDb(f32 db) { level_ = dbToLinear(db); }
void HeightGen::setElevation(f32 elev) { elevation_ = elev; hipass_.setFrequency(400.0f + elev * 10.0f); }

void HeightGen::processBlock(f32 mid, f32 side, f32& outHf, f32& outHr, i32 numFrames) {
    (void)numFrames;
    f32 frontSource = mid * 0.5f + side * 0.5f;
    f32 rearSource = mid * 0.3f - side * 0.7f;

    frontSource = hipass_.process(frontSource);
    rearSource = hipass_.process(rearSource);

    frontSource = frontDec_.process(frontSource);
    rearSource = rearDec_.process(rearSource);

    f32 frontDelay = 0.020f + 0.005f * (elevation_ / 45.0f);
    f32 rearDelay = 0.028f + 0.005f * (elevation_ / 45.0f);

    outHf = frontDelay_.readLinear(frontDelay * sampleRate_) * level_;
    outHr = rearDelay_.readLinear(rearDelay * sampleRate_) * level_ * 0.8f;
}

void HeightGen::reset() {
    frontDelay_.reset();
    rearDelay_.reset();
    frontDec_.reset();
    rearDec_.reset();
    hipass_.reset();
    smoothHf_.reset();
    smoothHr_.reset();
}

}
