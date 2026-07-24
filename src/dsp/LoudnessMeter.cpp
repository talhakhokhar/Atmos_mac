#include "dsp/LoudnessMeter.h"

namespace atmos {

LoudnessMeter::LoudnessMeter() {
    kFilterL_.setType(BiquadType::Peaking);
    kFilterR_.setType(BiquadType::Peaking);
    kFilterL_.setFrequency(1000.0f);
    kFilterR_.setFrequency(1000.0f);
    kFilterL_.setGainDb(0.0f);
    kFilterR_.setGainDb(0.0f);
    kFilterL_.setQ(1.0f);
    kFilterR_.setQ(1.0f);
    highPassL_.setType(BiquadType::HighPass);
    highPassR_.setType(BiquadType::HighPass);
    highPassL_.setFrequency(70.0f);
    highPassR_.setFrequency(70.0f);
    highPassL_.setQ(0.5f);
    highPassR_.setQ(0.5f);
}

void LoudnessMeter::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    kFilterL_.setSampleRate(sr);
    kFilterR_.setSampleRate(sr);
    highPassL_.setSampleRate(sr);
    highPassR_.setSampleRate(sr);
}

void LoudnessMeter::setBlockSize(i32 frames) {
    blockFrames_ = frames;
    frameCount_ = 0;
    squareSum_ = 0.0f;
}

f32 LoudnessMeter::process(const f32* inL, const f32* inR, i32 numFrames) {
    for (i32 n = 0; n < numFrames; ++n) {
        f32 l = highPassL_.process(kFilterL_.process(inL[n]));
        f32 r = highPassR_.process(kFilterR_.process(inR[n]));
        f32 sq = (l * l + r * r) * 0.5f;
        squareSum_ += sq;
        shortTermSum_ += sq;
        momentarySum_ += sq;
        frameCount_++;
        shortTermFrames_++;
        momentaryFrames_++;
        totalFrames_++;
        totalSquare_ += sq;

        if (frameCount_ >= blockFrames_) {
            f32 mean = squareSum_ / static_cast<f32>(blockFrames_);
            f32 rms = std::sqrt(mean);
            integratedLUFS_ = -0.691f + 10.0f * std::log10(mean + 1e-10f);
            squareSum_ = 0.0f;
            frameCount_ = 0;
        }

        if (shortTermFrames_ >= static_cast<i32>(sampleRate_ * 3.0f)) {
            f32 mean = shortTermSum_ / static_cast<f32>(shortTermFrames_);
            shortTermLUFS_ = -0.691f + 10.0f * std::log10(mean + 1e-10f);
            shortTermSum_ = 0.0f;
            shortTermFrames_ = 0;
        }

        if (momentaryFrames_ >= static_cast<i32>(sampleRate_ * 0.4f)) {
            f32 mean = momentarySum_ / static_cast<f32>(momentaryFrames_);
            momentaryLUFS_ = -0.691f + 10.0f * std::log10(mean + 1e-10f);
            momentarySum_ = 0.0f;
            momentaryFrames_ = 0;
        }
    }
    return integratedLUFS_;
}

void LoudnessMeter::reset() {
    kFilterL_.reset();
    kFilterR_.reset();
    highPassL_.reset();
    highPassR_.reset();
    squareSum_ = 0.0f;
    frameCount_ = 0;
    integratedLUFS_ = -70.0f;
    shortTermLUFS_ = -70.0f;
    momentaryLUFS_ = -70.0f;
    shortTermSum_ = 0.0f;
    shortTermFrames_ = 0;
    momentarySum_ = 0.0f;
    momentaryFrames_ = 0;
    totalFrames_ = 0.0f;
    totalSquare_ = 0.0f;
}

}
