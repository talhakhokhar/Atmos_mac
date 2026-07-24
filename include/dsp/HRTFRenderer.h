#pragma once
#include "../common/Types.h"
#include "../dsp/DelayLine.h"
#include "../dsp/Biquad.h"
#include "../dsp/ParamSmooth.h"

namespace atmos {

class AirAbsorption {
public:
    AirAbsorption();
    void setDistance(f32 dist);
    f32 process(f32 x);
    void reset();

private:
    Biquad lowpass_;
    f32 distance_ = 1.0f;
    f32 currentDist_ = 1.0f;
    ParamSmooth smooth_;
};

class HRTFRenderer {
public:
    HRTFRenderer();
    void setSampleRate(f32 sr);
    void setHeadSize(f32 size);
    void setHRTFStrength(f32 strength);
    void setMaxAzimuth(f32 az);
    void setOutputTarget(OutputTarget target);
    void renderObject(f32 x, f32 azimuthDeg, f32 elevationDeg, f32 distance, f32 size, f32& outL, f32& outR);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 headRadius_ = 0.0875f;
    f32 hrtfStrength_ = 1.0f;
    f32 maxAz_ = 90.0f;
    OutputTarget target_ = OutputTarget::Headphones;

    FractionalDelay delayL_, delayR_;
    AirAbsorption airL_, airR_;

    Biquad pinnaNotchL_, pinnaNotchR_;
    Biquad frontBackCueL_, frontBackCueR_;
    Biquad ildBandL_, ildBandR_;

    ParamSmooth smoothAzL_, smoothAzR_;
    ParamSmooth smoothElL_, smoothElR_;

    f32 microMotionPhase_ = 0.0f;
};

}
