#include "dsp/HRTFRenderer.h"

namespace atmos {

HRTFRenderer::HRTFRenderer() {
    pinnaNotchL_.setType(BiquadType::Peaking);
    pinnaNotchR_.setType(BiquadType::Peaking);
    frontBackCueL_.setType(BiquadType::Peaking);
    frontBackCueR_.setType(BiquadType::Peaking);
    ildBandL_.setType(BiquadType::HighShelf);
    ildBandR_.setType(BiquadType::HighShelf);
}

void HRTFRenderer::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    delayL_.setDelay(0.0f);
    delayR_.setDelay(0.0f);
    pinnaNotchL_.setSampleRate(sr);
    pinnaNotchR_.setSampleRate(sr);
    frontBackCueL_.setSampleRate(sr);
    frontBackCueR_.setSampleRate(sr);
    ildBandL_.setSampleRate(sr);
    ildBandR_.setSampleRate(sr);
    airL_.setDistance(1.5f);
    airR_.setDistance(1.5f);
}

void HRTFRenderer::setHeadSize(f32 size) {
    headRadius_ = 0.075f + size * 0.025f;
}

void HRTFRenderer::setMaxAzimuth(f32 az) {
    maxAz_ = clamp(az, 10.0f, 180.0f);
}

void HRTFRenderer::setHRTFStrength(f32 strength) {
    hrtfStrength_ = clamp(strength, 0.0f, 1.0f);
}

void HRTFRenderer::setOutputTarget(OutputTarget target) {
    target_ = target;
}

void HRTFRenderer::renderObject(f32 x, f32 azimuthDeg, f32 elevationDeg, f32 distance, f32 size, f32& outL, f32& outR) {
    if (target_ == OutputTarget::LineAux || target_ == OutputTarget::CarAux || target_ == OutputTarget::Reference) {
        // Simple stereo pan for non-binaural
        f32 pan = azimuthDeg / 90.0f;
        pan = clamp(pan, -1.0f, 1.0f);
        f32 gL = std::cos((pan + 1.0f) * kPi * 0.25f);
        f32 gR = std::sin((pan + 1.0f) * kPi * 0.25f);
        f32 distGain = 1.0f / (1.0f + distance / 1.5f);
        x *= distGain;
        outL += x * gL;
        outR += x * gR;
        return;
    }

    // Distance attenuation
    f32 distGain = 1.0f / (1.0f + distance / 1.5f);
    x *= distGain;

    // Air absorption
    x = airL_.process(x);

    // ITD calculation
    f32 theta = azimuthDeg * kPi / 180.0f;
    theta = clamp(theta, -kPi * 0.5f, kPi * 0.5f);
    f32 itd = (headRadius_ / kSpeedOfSound) * (theta + std::sin(theta));
    f32 itdSamples = itd * sampleRate_;

    // ILD
    f32 ildDb = 18.0f * std::sin(theta) * std::sin(theta);
    f32 gainL = dbToLinear(-ildDb * 0.5f);
    f32 gainR = dbToLinear(ildDb * 0.5f);
    if (theta > 0.0f) std::swap(gainL, gainR);

    // Micro motion for cone-of-confusion
    microMotionPhase_ += 0.0001f;
    f32 microAz = azimuthDeg + 1.5f * std::sin(microMotionPhase_);
    f32 microEl = elevationDeg + 1.0f * std::cos(microMotionPhase_ * 1.3f);

    // Pinna cues
    f32 notchFreq = 6000.0f + microEl * 40.0f;
    pinnaNotchL_.setFrequency(notchFreq);
    pinnaNotchR_.setFrequency(notchFreq);
    pinnaNotchL_.setGainDb(-4.0f - 0.05f * std::abs(microEl));
    pinnaNotchR_.setGainDb(-4.0f - 0.05f * std::abs(microEl));
    pinnaNotchL_.setQ(10.0f);
    pinnaNotchR_.setQ(10.0f);

    // Front/back cue
    f32 fbGain = (azimuthDeg > 45.0f || azimuthDeg < -45.0f) ? -3.0f : -1.0f;
    frontBackCueL_.setFrequency(10000.0f);
    frontBackCueR_.setFrequency(10000.0f);
    frontBackCueL_.setGainDb(fbGain);
    frontBackCueR_.setGainDb(fbGain);
    frontBackCueL_.setQ(6.0f);
    frontBackCueR_.setQ(6.0f);

    // ILD frequency dependency
    ildBandL_.setFrequency(1500.0f);
    ildBandR_.setFrequency(1500.0f);
    ildBandL_.setGainDb(-ildDb * 0.3f * hrtfStrength_);
    ildBandR_.setGainDb(ildDb * 0.3f * hrtfStrength_);

    // Object spread / size
    f32 spreadL = 0.0f, spreadR = 0.0f;
    if (size > 0.01f) {
        f32 spreadAz = maxAz_ * size * 0.5f;
        // Simple spread by offsetting azimuth
        f32 spreadGain = size * 0.3f;
        spreadL = x * spreadGain * std::cos((clamp(azimuthDeg - spreadAz, -90.0f, 90.0f)) * kPi / 180.0f);
        spreadR = x * spreadGain * std::cos((clamp(azimuthDeg + spreadAz, -90.0f, 90.0f)) * kPi / 180.0f);
    }

    // Render with HRTF
    f32 directL = x * gainL * hrtfStrength_;
    f32 directR = x * gainR * hrtfStrength_;

    f32 processedL = pinnaNotchL_.process(frontBackCueL_.process(ildBandL_.process(directL)));
    f32 processedR = pinnaNotchR_.process(frontBackCueR_.process(ildBandR_.process(directR)));

    f32 delayedL = delayL_.process(processedL);
    f32 delayedR = delayR_.process(processedR);

    if (theta > 0.0f) {
        delayedL = delayL_.process(processedL + itdSamples * 0.5f);
        delayedR = delayR_.process(processedR);
    } else {
        delayedL = delayL_.process(processedL);
        delayedR = delayR_.process(processedR - itdSamples * 0.5f);
    }

    outL += delayedL + spreadL;
    outR += delayedR + spreadR;
}

void HRTFRenderer::reset() {
    delayL_.reset();
    delayR_.reset();
    pinnaNotchL_.reset();
    pinnaNotchR_.reset();
    frontBackCueL_.reset();
    frontBackCueR_.reset();
    ildBandL_.reset();
    ildBandR_.reset();
    airL_.reset();
    airR_.reset();
    microMotionPhase_ = 0.0f;
}

}
