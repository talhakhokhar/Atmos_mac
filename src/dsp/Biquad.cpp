#include "Biquad.h"

namespace atmos {

Biquad::Biquad() = default;

void Biquad::setType(BiquadType type) {
    type_ = type;
    calculateCoeffs();
}

void Biquad::setFrequency(f32 freq) {
    freq_ = clamp(freq, 20.0f, 20000.0f);
    calculateCoeffs();
}

void Biquad::setQ(f32 q) {
    q_ = std::max(q, 0.1f);
    calculateCoeffs();
}

void Biquad::setGainDb(f32 gainDb) {
    gainDb_ = gainDb;
    calculateCoeffs();
}

void Biquad::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    calculateCoeffs();
}

void Biquad::calculateCoeffs() {
    const f32 w0 = kTau * freq_ / sampleRate_;
    const f32 cosW0 = std::cos(w0);
    const f32 sinW0 = std::sin(w0);
    const f32 A = dbToLinear(gainDb_ * 0.5f);
    const f32 alpha = sinW0 * std::sinh(kPi * 0.5f * q_);
    f32 b0 = 0.0f, b1 = 0.0f, b2 = 0.0f, a0 = 1.0f, a1 = 0.0f, a2 = 0.0f;

    switch (type_) {
        case BiquadType::LowShelf:
            b0 = A * ((A + 1) - (A - 1) * cosW0 + 2 * std::sqrt(A) * alpha);
            b1 = 2 * A * ((A - 1) - (A + 1) * cosW0);
            b2 = A * ((A + 1) - (A - 1) * cosW0 - 2 * std::sqrt(A) * alpha);
            a0 = (A + 1) + (A - 1) * cosW0 + 2 * std::sqrt(A) * alpha;
            a1 = -2 * ((A - 1) + (A + 1) * cosW0);
            a2 = (A + 1) + (A - 1) * cosW0 - 2 * std::sqrt(A) * alpha;
            break;
        case BiquadType::HighShelf:
            b0 = A * ((A + 1) + (A - 1) * cosW0 + 2 * std::sqrt(A) * alpha);
            b1 = -2 * A * ((A - 1) + (A + 1) * cosW0);
            b2 = A * ((A + 1) + (A - 1) * cosW0 - 2 * std::sqrt(A) * alpha);
            a0 = (A + 1) - (A - 1) * cosW0 + 2 * std::sqrt(A) * alpha;
            a1 = 2 * ((-A + 1) - (A + 1) * cosW0);
            a2 = (A + 1) - (A - 1) * cosW0 - 2 * std::sqrt(A) * alpha;
            break;
        case BiquadType::Peaking: {
            f32 sqrtA = std::sqrt(A);
            b0 = 1.0f + alpha * sqrtA;
            b1 = -2.0f * cosW0;
            b2 = 1.0f - alpha * sqrtA;
            a0 = 1.0f + alpha / sqrtA;
            a1 = -2.0f * cosW0;
            a2 = 1.0f - alpha / sqrtA;
            break;
        }
        case BiquadType::LowPass:
            b0 = (1.0f - cosW0) * 0.5f;
            b1 = 1.0f - cosW0;
            b2 = (1.0f - cosW0) * 0.5f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * cosW0;
            a2 = 1.0f - alpha;
            break;
        case BiquadType::HighPass:
            b0 = (1.0f + cosW0) * 0.5f;
            b1 = -(1.0f + cosW0);
            b2 = (1.0f + cosW0) * 0.5f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * cosW0;
            a2 = 1.0f - alpha;
            break;
        case BiquadType::LinkwitzRileyLP:
            b0 = (1.0f - cosW0) * 0.5f;
            b1 = 1.0f - cosW0;
            b2 = (1.0f - cosW0) * 0.5f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * cosW0;
            a2 = 1.0f - alpha;
            break;
        case BiquadType::LinkwitzRileyHP:
            b0 = (1.0f + cosW0) * 0.5f;
            b1 = -(1.0f + cosW0);
            b2 = (1.0f + cosW0) * 0.5f;
            a0 = 1.0f + alpha;
            a1 = -2.0f * cosW0;
            a2 = 1.0f - alpha;
            break;
        default:
            break;
    }

    c_.b0 = b0 / a0; c_.b1 = b1 / a0; c_.b2 = b2 / a0;
    c_.a1 = a1 / a0; c_.a2 = a2 / a0;
}

f32 Biquad::process(f32 x) {
    f32 y = c_.b0 * x + c_.b1 * x1_ + c_.b2 * x2_ - c_.a1 * y1_ - c_.a2 * y2_;
    x2_ = x1_; x1_ = x;
    y2_ = y1_; y1_ = y;
    return y;
}

void Biquad::processBlock(f32* buffer, i32 numFrames) {
    for (i32 n = 0; n < numFrames; ++n) {
        buffer[n] = process(buffer[n]);
    }
}

void Biquad::reset() {
    x1_ = x2_ = y1_ = y2_ = 0.0f;
}

}
