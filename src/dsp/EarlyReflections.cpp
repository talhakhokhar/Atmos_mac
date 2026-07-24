#include "dsp/EarlyReflections.h"

namespace atmos {

EarlyReflections::EarlyReflections() {
    f32 baseDelays[6] = {6.0f, 10.0f, 14.0f, 20.0f, 27.0f, 35.0f};
    f32 baseAzimuth[6] = {-30.0f, 20.0f, -15.0f, 40.0f, -50.0f, 25.0f};
    f32 baseElev[6] = {10.0f, 35.0f, 5.0f, 15.0f, 20.0f, 40.0f};

    for (int i = 0; i < 6; ++i) {
        reflections_[i].delayMs = baseDelays[i];
        reflections_[i].azimuth = baseAzimuth[i];
        reflections_[i].elevation = baseElev[i];
        reflections_[i].gain = dbToLinear(-12.0f - i * 2.0f);
        reflections_[i].hp.setType(BiquadType::HighPass);
        reflections_[i].hp.setFrequency(200.0f);
    }
    inputHp_.setType(BiquadType::HighPass);
    inputHp_.setFrequency(150.0f);
}

void EarlyReflections::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    for (int i = 0; i < 6; ++i) {
        i32 maxD = static_cast<i32>(sr * 0.05f);
        reflections_[i].delay.setMaxDelay(maxD);
        reflections_[i].hp.setSampleRate(sr);
    }
    inputHp_.setSampleRate(sr);
    smoothLevel_.setSampleRate(sr);
}

void EarlyReflections::setRoomSize(f32 size) {
    roomSize_ = size;
    for (int i = 0; i < 6; ++i) {
        f32 d = reflections_[i].delayMs * (0.7f + 0.6f * size);
        reflections_[i].delay.setMaxDelay(static_cast<i32>(sampleRate_ * d * 0.001f));
    }
}

void EarlyReflections::setLevel(f32 level) {
    level_ = level;
    smoothLevel_.setTarget(level);
}

void EarlyReflections::process(f32 inL, f32 inR, f32& outL, f32& outR) {
    f32 level = smoothLevel_.process();
    inL = inputHp_.process(inL);
    inR = inputHp_.process(inR);

    outL = 0.0f;
    outR = 0.0f;

    for (int i = 0; i < 6; ++i) {
        Reflection& ref = reflections_[i];
        f32 pan = ref.azimuth / 90.0f;
        pan = clamp(pan, -1.0f, 1.0f);
        f32 gL = std::cos((pan + 1.0f) * kPi * 0.25f);
        f32 gR = std::sin((pan + 1.0f) * kPi * 0.25f);

        f32 delayedL = ref.delay.readLinear(ref.delayMs * sampleRate_ * 0.001f);
        f32 delayedR = ref.delay.readLinear(ref.delayMs * sampleRate_ * 0.001f + 0.001f * sampleRate_);

        f32 filteredL = ref.hp.process(delayedL) * ref.gain;
        f32 filteredR = ref.hp.process(delayedR) * ref.gain;

        ref.delay.write(inL * 0.5f + inR * 0.5f);

        outL += filteredL * gL * level;
        outR += filteredR * gR * level;
    }
}

void EarlyReflections::reset() {
    for (int i = 0; i < 6; ++i) {
        reflections_[i].delay.reset();
        reflections_[i].hp.reset();
    }
    inputHp_.reset();
    smoothLevel_.reset();
}

}
