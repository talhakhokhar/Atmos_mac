#include "dsp/SurroundGen.h"

namespace atmos {

SurroundGen::SurroundGen() {
    lsDelay_.setMaxDelay(static_cast<i32>(48000.0f * 0.025f));
    rsDelay_.setMaxDelay(static_cast<i32>(48000.0f * 0.025f));
}

void SurroundGen::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    lsDelay_.setMaxDelay(static_cast<i32>(sr * 0.025f));
    rsDelay_.setMaxDelay(static_cast<i32>(sr * 0.025f));
    smoothLs_.setSampleRate(sr);
    smoothRs_.setSampleRate(sr);
}

void SurroundGen::setLevelDb(f32 db) { level_ = dbToLinear(db); }
void SurroundGen::setWidth(f32 w) { width_ = w; }

void SurroundGen::processBlock(f32 sideL, f32 sideR, f32& outLs, f32& outRs, i32 numFrames, bool centerPresent) {
    (void)numFrames;
    f32 lsLevel = level_ * (1.0f - 0.3f * width_);
    f32 rsLevel = level_ * (1.0f - 0.3f * width_);

    if (centerPresent) {
        lsLevel *= 0.4f;
        rsLevel *= 0.4f;
    }

    f32 ls = sideL * lsLevel;
    f32 rs = sideR * rsLevel;

    ls = lsDec_.process(ls);
    rs = rsDec_.process(rs);

    f32 lsDelay = 0.015f + 0.005f * width_;
    f32 rsDelay = 0.018f + 0.005f * width_;

    outLs = lsDelay_.readLinear(lsDelay * sampleRate_);
    outRs = rsDelay_.readLinear(rsDelay * sampleRate_);
}

void SurroundGen::reset() {
    lsDelay_.reset();
    rsDelay_.reset();
    lsDec_.reset();
    rsDec_.reset();
    smoothLs_.reset();
    smoothRs_.reset();
}

}
