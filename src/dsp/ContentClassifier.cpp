#include "dsp/ContentClassifier.h"

namespace atmos {

ContentClassifier::ContentClassifier() {
    featureHistory_.fill(0.0f);
}

f32 ContentClassifier::calcCorrelation(const f32* L, const f32* R, i32 n) {
    f32 sumLR = 0.0f, sumL2 = 0.0f, sumR2 = 0.0f;
    for (i32 i = 0; i < n; ++i) {
        sumLR += L[i] * R[i];
        sumL2 += L[i] * L[i];
        sumR2 += R[i] * R[i];
    }
    f32 denom = std::sqrt(sumL2 * sumR2 + 1e-8f);
    return sumLR / denom;
}

f32 ContentClassifier::calcSideEnergy(const f32* L, const f32* R, i32 n) {
    f32 sum = 0.0f;
    for (i32 i = 0; i < n; ++i) {
        f32 s = 0.5f * (L[i] - R[i]);
        sum += s * s;
    }
    return std::sqrt(sum / n + 1e-8f);
}

f32 ContentClassifier::calcSpeechEnergy(const f32* L, const f32* R, i32 n) {
    f32 sum = 0.0f;
    for (i32 i = 0; i < n; ++i) {
        f32 m = 0.5f * (L[i] + R[i]);
        sum += m * m;
    }
    return std::sqrt(sum / n + 1e-8f);
}

f32 ContentClassifier::calcLowEnergy(const f32* L, const f32* R, i32 n) {
    f32 sum = 0.0f;
    for (i32 i = 0; i < n; ++i) {
        f32 m = 0.5f * (L[i] + R[i]);
        sum += m * m;
    }
    return std::sqrt(sum / n + 1e-8f);
}

f32 ContentClassifier::calcTransientDensity(const f32* L, const f32* R, i32 n) {
    i32 transients = 0;
    for (i32 i = 1; i < n; ++i) {
        f32 diff = std::abs(L[i] - L[i-1]) + std::abs(R[i] - R[i-1]);
        if (diff > 0.1f) transients++;
    }
    return static_cast<f32>(transients) / n;
}

f32 ContentClassifier::calcSpectralCentroid(const f32* L, const f32* R, i32 n) {
    (void)L; (void)R; (void)n;
    return 0.5f;
}

SoundMode ContentClassifier::classify(const f32* inL, const f32* inR, i32 numFrames, f32 sampleRate) {
    sampleRate_ = sampleRate;
    if (numFrames < 256) return SoundMode::Auto;

    f32 corr = calcCorrelation(inL, inR, numFrames);
    f32 side = calcSideEnergy(inL, inR, numFrames);
    f32 speech = calcSpeechEnergy(inL, inR, numFrames);
    f32 low = calcLowEnergy(inL, inR, numFrames);
    f32 trans = calcTransientDensity(inL, inR, numFrames);

    // Voice detection
    if (corr > 0.6f && speech > 0.05f && side < 0.15f) {
        return SoundMode::Voice;
    }

    // Game detection
    if (trans > 0.15f && side > 0.1f) {
        return SoundMode::Game;
    }

    // Movie detection
    if (low > 0.08f && side > 0.08f && trans > 0.05f) {
        return SoundMode::Movie;
    }

    // Music detection
    if (side > 0.06f && trans > 0.03f) {
        return SoundMode::Music;
    }

    return SoundMode::Auto;
}

void ContentClassifier::reset() {
    featureHistory_.fill(0.0f);
    historyIndex_ = 0;
    historyFilled_ = false;
}

}
