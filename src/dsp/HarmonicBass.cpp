#include "HarmonicBass.h"

namespace atmos {

HarmonicBass::HarmonicBass() {
    lp2_.setType(BiquadType::LowPass);
    lp3_.setType(BiquadType::LowPass);
    inputBp_.setType(BiquadType::BandPass);
}

void HarmonicBass::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    lp2_.setSampleRate(sr);
    lp3_.setSampleRate(sr);
    inputBp_.setSampleRate(sr);
}

void HarmonicBass::setEnabled(bool en) { enabled_ = en; }
void HarmonicBass::setAmount(f32 amt) { amount_ = amt; }
void HarmonicBass::setFrequency(f32 freq) {
    freq_ = freq;
    lp2_.setFrequency(freq * 2.0f);
    lp3_.setFrequency(freq * 3.0f);
    inputBp_.setFrequency(freq);
    inputBp_.setQ(1.0f);
}

f32 HarmonicBass::process(f32 x) {
    if (!enabled_) return 0.0f;

    f32 fundamental = inputBp_.process(x);
    f32 h2 = lp2_.process(fundamental * fundamental);
    f32 h3 = lp3_.process(fundamental * fundamental * fundamental);

    h2State_ = 0.8f * h2State_ + 0.2f * h2;
    h3State_ = 0.8f * h3State_ + 0.2f * h3;

    return (h2State_ * 0.5f + h3State_ * 0.25f) * amount_;
}

void HarmonicBass::reset() {
    lp2_.reset();
    lp3_.reset();
    inputBp_.reset();
    h2State_ = h3State_ = 0.0f;
}

}
