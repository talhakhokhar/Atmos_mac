#include "dsp/TruePeakLimiter.h"

namespace atmos {

TruePeakLimiter::TruePeakLimiter() {
    lowpass_.setType(BiquadType::LowPass);
    lowpass_.setFrequency(22000.0f);
}

void TruePeakLimiter::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    lowpass_.setSampleRate(sr);
    delaySize_ = static_cast<i32>(sr * 0.01f) + 1;
    delayBuffer_.assign(delaySize_, 0.0f);
    delayWrite_ = 0;
    delayRead_ = 0;
}

void TruePeakLimiter::setCeiling(f32 db) {
    ceilingDb_ = db;
    ceilingLin_ = dbToLinear(db);
}

void TruePeakLimiter::setLookahead(f32 ms) {
    lookaheadMs_ = ms;
    delayRead_ = static_cast<i32>(ms * 0.001f * sampleRate_);
    delayRead_ = std::min(delayRead_, delaySize_ - 1);
}

void TruePeakLimiter::setRelease(f32 ms) { releaseMs_ = ms; }
void TruePeakLimiter::setMaxGainReduction(f32 db) { maxGainReductionDb_ = db; }

f32 TruePeakLimiter::process(f32& inL, f32& inR) {
    // Write input to delay buffer for lookahead
    delayBuffer_[delayWrite_] = inL;
    delayBuffer_[(delayWrite_ + 1) % delaySize_] = inR;
    
    // Calculate read position with lookahead offset
    i32 lookaheadReadPos = (delayWrite_ - delayRead_ + delaySize_) % delaySize_;
    f32 delayedL = delayBuffer_[lookaheadReadPos];
    f32 delayedR = delayBuffer_[(lookaheadReadPos + 1) % delaySize_];
    
    delayWrite_ = (delayWrite_ + 1) % delaySize_;

    f32 peak = std::max(std::abs(delayedL), std::abs(delayedR));
    f32 maxGain = 1.0f;
    if (peak > ceilingLin_) {
        maxGain = ceilingLin_ / peak;
        maxGain = clamp(maxGain, 0.0f, 1.0f);
    }

    f32 targetGain = maxGain;
    f32 attackCoeff = 1.0f - std::exp(-1.0f / (0.0005f * sampleRate_));
    f32 releaseCoeff = 1.0f - std::exp(-1.0f / (releaseMs_ * 0.001f * sampleRate_));

    if (targetGain < currentGain_) {
        currentGain_ = lerp(currentGain_, targetGain, attackCoeff);
    } else {
        currentGain_ = lerp(currentGain_, targetGain, releaseCoeff);
    }

    gainReduction_ = linearToDb(currentGain_);
    gainReduction_ = std::max(gainReduction_, -maxGainReductionDb_);

    f32 gain = dbToLinear(gainReduction_);
    inL = delayedL * gain;
    inR = delayedR * gain;

    return gainReduction_;
}

void TruePeakLimiter::reset() {
    lowpass_.reset();
    std::fill(delayBuffer_.begin(), delayBuffer_.end(), 0.0f);
    delayWrite_ = 0;
    delayRead_ = 0;
    gainReduction_ = 0.0f;
    currentGain_ = 1.0f;
}

}
