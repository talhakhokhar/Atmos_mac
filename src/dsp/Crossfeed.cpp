#include "Crossfeed.h"

namespace atmos {

Crossfeed::Crossfeed() {
    lpfL_.setType(BiquadType::LowPass);
    lpfR_.setType(BiquadType::LowPass);
}

void Crossfeed::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    lpfL_.setSampleRate(sr);
    lpfR_.setSampleRate(sr);
}

void Crossfeed::setLevel(f32 level) { level_ = level; }
void Crossfeed::setLowpass(f32 freq) { lowpass_ = freq; lpfL_.setFrequency(freq); lpfR_.setFrequency(freq); }
void Crossfeed::setTarget(OutputTarget target) {
    target_ = target;
    switch (target) {
        case OutputTarget::Headphones:
            level_ = 0.5f; lowpass_ = 700.0f; break;
        case OutputTarget::IEM:
            level_ = 0.3f; lowpass_ = 600.0f; break;
        case OutputTarget::LineAux:
            level_ = 0.0f; break;
        case OutputTarget::CarAux:
            level_ = 0.2f; lowpass_ = 500.0f; break;
        case OutputTarget::Reference:
            level_ = 0.0f; break;
        default:
            level_ = 0.5f; lowpass_ = 700.0f; break;
    }
    lpfL_.setFrequency(lowpass_);
    lpfR_.setFrequency(lowpass_);
}

void Crossfeed::process(f32& inL, f32& inR) {
    if (level_ < 0.001f) return;

    f32 cfL = lpfL_.process(inR) * level_;
    f32 cfR = lpfR_.process(inL) * level_;

    inL = inL * (1.0f - level_) + cfL;
    inR = inR * (1.0f - level_) + cfR;
}

void Crossfeed::reset() {
    lpfL_.reset();
    lpfR_.reset();
    delayL_ = delayR_ = 0.0f;
    x1L_ = x1R_ = 0.0f;
}

}
