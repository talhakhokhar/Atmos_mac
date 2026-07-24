#include "SpectralAccent.h"

namespace atmos {

SpectralAccent::SpectralAccent() {
    f32 freqs[6] = {200.0f, 500.0f, 1200.0f, 3000.0f, 6000.0f, 10000.0f};
    for (int i = 0; i < 6; ++i) {
        bands_[i].freq = freqs[i];
        bands_[i].filterL.setType(BiquadType::BandPass);
        bands_[i].filterR.setType(BiquadType::BandPass);
        bands_[i].decL.setGain(0.5f);
        bands_[i].decR.setGain(0.5f);
    }
    objDelayL_.setMaxDelay(static_cast<i32>(48000.0f * 0.002f));
    objDelayR_.setMaxDelay(static_cast<i32>(48000.0f * 0.002f));
}

void SpectralAccent::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    for (int i = 0; i < 6; ++i) {
        bands_[i].filterL.setSampleRate(sr);
        bands_[i].filterR.setSampleRate(sr);
        bands_[i].smoothAz.setSampleRate(sr);
        bands_[i].smoothEl.setSampleRate(sr);
        bands_[i].smoothGain.setSampleRate(sr);
    }
    objDelayL_.setMaxDelay(static_cast<i32>(sr * 0.002f));
    objDelayR_.setMaxDelay(static_cast<i32>(sr * 0.002f));
}

void SpectralAccent::setStrength(f32 s) { strength_ = s; }
void SpectralAccent::setMaxAzimuth(f32 az) { maxAz_ = az; }
void SpectralAccent::setMaxElevation(f32 elev) { maxEl_ = elev; }
void SpectralAccent::setSmoothing(f32 ms) {
    for (int i = 0; i < 6; ++i) {
        bands_[i].smoothAz.setTimeConstant(ms * 0.001f);
        bands_[i].smoothEl.setTimeConstant(ms * 0.001f);
        bands_[i].smoothGain.setTimeConstant(ms * 0.001f);
    }
}

void SpectralAccent::processBlock(const f32* inL, const f32* inR, f32* outL, f32* outR, i32 numFrames) {
    for (i32 n = 0; n < numFrames; ++n) {
        f32 accL = 0.0f, accR = 0.0f;
        for (int i = 0; i < 6; ++i) {
            Band& b = bands_[i];
            f32 bandL = b.filterL.process(inL[n]);
            f32 bandR = b.filterR.process(inR[n]);

            f32 energy = std::abs(bandL) + std::abs(bandR);
            b.energy = lerp(b.energy, energy, 0.02f);

            f32 balance = 0.0f;
            f32 sum = bandL + bandR;
            if (std::abs(sum) > 1e-8f) {
                balance = (bandL - bandR) / (sum + 1e-8f);
            }

            f32 az = balance * maxAz_;
            f32 el = (i < 3) ? maxEl_ * 0.3f : maxEl_ * 0.7f;
            f32 gain = b.energy * strength_ * 0.15f;

            b.smoothAz.setTarget(az);
            b.smoothEl.setTarget(el);
            b.smoothGain.setTarget(gain);

            f32 curAz = b.smoothAz.process();
            f32 curGain = b.smoothGain.process();

            f32 theta = curAz * kPi / 180.0f;
            f32 ild = 1.5f * std::sin(theta);
            f32 gL = dbToLinear(-ild * 0.5f);
            f32 gR = dbToLinear(ild * 0.5f);
            if (theta > 0.0f) std::swap(gL, gR);

            accL += bandL * curGain * gL;
            accR += bandR * curGain * gR;
        }
        outL[n] = accL;
        outR[n] = accR;
    }
}

void SpectralAccent::reset() {
    for (int i = 0; i < 6; ++i) {
        bands_[i].filterL.reset();
        bands_[i].filterR.reset();
        bands_[i].decL.reset();
        bands_[i].decR.reset();
        bands_[i].smoothAz.reset();
        bands_[i].smoothEl.reset();
        bands_[i].smoothGain.reset();
        bands_[i].energy = 0.0f;
    }
    objDelayL_.reset();
    objDelayR_.reset();
}

}
