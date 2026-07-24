#include "BassManager.h"

namespace atmos {

BassManager::BassManager() {
    lpfL_.setType(BiquadType::LowPass);
    lpfR_.setType(BiquadType::LowPass);
    hpfL_.setType(BiquadType::HighPass);
    hpfR_.setType(BiquadType::HighPass);
}

void BassManager::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    lpfL_.setSampleRate(sr);
    lpfR_.setSampleRate(sr);
    hpfL_.setSampleRate(sr);
    hpfR_.setSampleRate(sr);
}

void BassManager::setCrossover(f32 freq) {
    crossover_ = clamp(freq, 40.0f, 160.0f);
    lpfL_.setFrequency(crossover_);
    lpfR_.setFrequency(crossover_);
    hpfL_.setFrequency(crossover_);
    hpfR_.setFrequency(crossover_);
}

void BassManager::setLFEBoost(f32 boostDb) {
    lfeBoost_ = boostDb;
}

void BassManager::process(f32& inL, f32& inR, f32& outL, f32& outR) {
    f32 lfeRaw = lpfL_.process(inL) + lpfR_.process(inR);
    lfeRaw *= 0.5f;
    f32 lfe = lfeRaw * dbToLinear(lfeBoost_);

    f32 midHigh = hpfL_.process(inL);
    f32 sideHigh = hpfR_.process(inR);

    outL = midHigh + lfe;
    outR = sideHigh + lfe;
}

void BassManager::reset() {
    lpfL_.reset();
    lpfR_.reset();
    hpfL_.reset();
    hpfR_.reset();
    lfeStateL_ = lfeStateR_ = 0.0f;
}

}
