#pragma once
#include "../common/Types.h"
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

}
