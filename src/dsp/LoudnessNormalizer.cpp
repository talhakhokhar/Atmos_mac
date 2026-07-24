#include "LoudnessNormalizer.h"

namespace atmos {

LoudnessNormalizer::LoudnessNormalizer() {
    meter_.setSampleRate(48000.0f);
    meter_.setBlockSize(19200);
    smoothGain_.setSampleRate(48000.0f);
    smoothGain_.setTimeConstant(1.5f);
    smoothTarget_.setSampleRate(48000.0f);
    smoothTarget_.setTimeConstant(0.5f);
}

void LoudnessNormalizer::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    meter_.setSampleRate(sr);
    meter_.setBlockSize(static_cast<i32>(sr * 0.4f));
    smoothGain_.setSampleRate(sr);
    smoothTarget_.setSampleRate(sr);
}

void LoudnessNormalizer::setTargetLUFS(f32 target) { smoothTarget_.setTarget(target); }
void LoudnessNormalizer::setEnabled(bool en) { enabled_ = en; }
void LoudnessNormalizer::setAttack(f32 ms) { smoothGain_.setTimeConstant(ms * 0.001f); }
void LoudnessNormalizer::setRelease(f32 ms) { /* simplified */ }

f32 LoudnessNormalizer::process(const f32* inL, const f32* inR, f32* outL, f32* outR, i32 numFrames) {
    if (!enabled_) {
        std::memcpy(outL, inL, numFrames * sizeof(f32));
        std::memcpy(outR, inR, numFrames * sizeof(f32));
        return 0.0f;
    }

    f32 target = smoothTarget_.process();
    f32 currentLUFS = meter_.process(inL, inR, numFrames);

    f32 neededDb = target - currentLUFS;
    f32 targetGain = dbToLinear(neededDb);
    targetGain = clamp(targetGain, 0.1f, 10.0f);

    currentGain_ = smoothGain_.process();
    smoothGain_.setTarget(targetGain);

    gainReductionDb_ = linearToDb(currentGain_);

    for (i32 n = 0; n < numFrames; ++n) {
        outL[n] = inL[n] * currentGain_;
        outR[n] = inR[n] * currentGain_;
    }

    return gainReductionDb_;
}

void LoudnessNormalizer::reset() {
    meter_.reset();
    smoothGain_.reset();
    smoothTarget_.reset();
    currentGain_ = 1.0f;
    gainReductionDb_ = 0.0f;
    prevLoudness_ = -70.0f;
}

}
