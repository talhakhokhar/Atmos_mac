#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"
#include "../dsp/DelayLine.h"
#include "../dsp/ParamSmooth.h"
#include <array>

namespace atmos {

class SpectralAccent {
public:
    SpectralAccent();
    void setSampleRate(f32 sr);
    void setStrength(f32 s);
    void setMaxAzimuth(f32 az);
    void setMaxElevation(f32 elev);
    void setSmoothing(f32 ms);
    void processBlock(const f32* inL, const f32* inR, f32* outL, f32* outR, i32 numFrames);
    void reset();

private:
    struct Band {
        f32 freq = 1000.0f;
        Biquad filterL, filterR;
        AllpassDecorrelator decL, decR;
        ParamSmooth smoothAz, smoothEl, smoothGain;
        f32 currentAz = 0.0f;
        f32 currentEl = 0.0f;
        f32 currentGain = 0.0f;
        f32 energy = 0.0f;
    };

    f32 sampleRate_ = 48000.0f;
    f32 strength_ = 0.35f;
    f32 maxAz_ = 45.0f;
    f32 maxEl_ = 40.0f;

    std::array<Band, 6> bands_;
    DelayLine objDelayL_, objDelayR_;
};

}
