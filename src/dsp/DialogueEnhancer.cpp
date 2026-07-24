#include "DialogueEnhancer.h"

namespace atmos {

DialogueEnhancer::DialogueEnhancer() {
    bodyBoost_.setType(BiquadType::Peaking);
    presenceBoost_.setType(BiquadType::Peaking);
    deEss_.setType(BiquadType::Peaking);
}

void DialogueEnhancer::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    bodyBoost_.setSampleRate(sr);
    presenceBoost_.setSampleRate(sr);
    deEss_.setSampleRate(sr);
    smoothBody_.setSampleRate(sr);
    smoothPresence_.setSampleRate(sr);
    smoothDeess_.setSampleRate(sr);
}

void DialogueEnhancer::setEnabled(bool en) { enabled_ = en; }
void DialogueEnhancer::setAmount(f32 amt) {
    amount_ = amt;
    smoothBody_.setTarget(amt * 4.0f);
    smoothPresence_.setTarget(amt * 3.0f);
    smoothDeess_.setTarget(amt * 4.0f);
}

void DialogueEnhancer::process(f32 center, f32& inL, f32& inR) {
    if (!enabled_) return;

    f32 cEnv = std::abs(center);
    env_ = lerp(env_, cEnv, 0.02f);

    f32 bodyGain = smoothBody_.process() * (env_ > 0.001f ? 1.0f : 0.0f);
    f32 presGain = smoothPresence_.process() * (env_ > 0.001f ? 1.0f : 0.0f);
    f32 deessGain = smoothDeess_.process();

    bodyBoost_.setFrequency(250.0f);
    bodyBoost_.setGainDb(bodyGain);
    bodyBoost_.setQ(1.0f);

    presenceBoost_.setFrequency(2500.0f);
    presenceBoost_.setGainDb(presGain);
    presenceBoost_.setQ(2.0f);

    deEss_.setFrequency(5500.0f);
    deEss_.setGainDb(-deessGain);
    deEss_.setQ(3.0f);

    f32 duck = 1.0f - clamp(env_ * 8.0f, 0.0f, 0.4f) * amount_;
    inL = inL * duck + bodyBoost_.process(inL) * 0.3f;
    inR = inR * duck + bodyBoost_.process(inR) * 0.3f;

    inL = presenceBoost_.process(inL);
    inR = presenceBoost_.process(inR);

    inL = deEss_.process(inL);
    inR = deEss_.process(inR);
}

void DialogueEnhancer::reset() {
    bodyBoost_.reset();
    presenceBoost_.reset();
    deEss_.reset();
    smoothBody_.reset();
    smoothPresence_.reset();
    smoothDeess_.reset();
    env_ = 0.0f;
}

}
