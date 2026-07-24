#include "CenterExtractor.h"

namespace atmos {

CenterExtractor::CenterExtractor() {
    bandpassLow_.setType(BiquadType::BandPass);
    bandpassHigh_.setType(BiquadType::BandPass);
}

void CenterExtractor::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    bandpassLow_.setSampleRate(sr);
    bandpassHigh_.setSampleRate(sr);
    shortDelay_.setMaxDelay(static_cast<i32>(sr * 0.01f));
    smoothC_.setSampleRate(sr);
    smoothAlpha_.setSampleRate(sr);
}

void CenterExtractor::setAlpha(f32 a) { smoothAlpha_.setTarget(a); }
void CenterExtractor::setMode(f32 m) { mode_ = m; }

void CenterExtractor::processBlock(const f32* inL, const f32* inR, f32* outC, f32* outLDirect, f32* outRDirect, i32 numFrames) {
    f32 alpha = smoothAlpha_.process();

    for (i32 n = 0; n < numFrames; ++n) {
        f32 L = inL[n];
        f32 R = inR[n];
        f32 M = 0.5f * (L + R);

        // Bandpass around speech frequencies
        f32 lowBand = bandpassLow_.process(M);
        f32 highBand = bandpassHigh_.process(M);
        f32 centerRaw = lowBand * 0.7f + highBand * 0.3f;

        // Correlation-based center mask
        f32 corr = 0.0f;
        if (std::abs(L) > 1e-6f || std::abs(R) > 1e-6f) {
            corr = (L * R) / (L * L + R * R + 1e-8f);
        }
        prevCorrelation_ = lerp(prevCorrelation_, corr, 0.1f);

        f32 centerMask = 0.5f + 0.5f * prevCorrelation_;
        centerMask = clamp(centerMask, 0.2f, 1.0f);

        f32 c = centerRaw * centerMask;
        f32 env = std::abs(c);
        prevCenterEnergy_ = lerp(prevCenterEnergy_, env, 0.05f);
        f32 gain = prevCenterEnergy_ > 0.001f ? 1.0f : 0.0f;
        c *= gain;

        outC[n] = c;
        outLDirect[n] = L - alpha * c;
        outRDirect[n] = R - alpha * c;
    }
}

void CenterExtractor::reset() {
    bandpassLow_.reset();
    bandpassHigh_.reset();
    decorrelator_.reset();
    shortDelay_.reset();
    smoothC_.reset();
    smoothAlpha_.reset();
    prevCorrelation_ = 0.0f;
    prevCenterEnergy_ = 0.0f;
}

}
