#include "dsp/AirAbsorption.h"

namespace atmos {

AirAbsorption::AirAbsorption() {
    lowpass_.setType(BiquadType::LowPass);
}

void AirAbsorption::setDistance(f32 dist) {
    distance_ = clamp(dist, 0.5f, 20.0f);
    smooth_.setTarget(distance_);
}

f32 AirAbsorption::process(f32 x) {
    currentDist_ = smooth_.process();
    f32 freq = 20000.0f / (1.0f + 0.12f * currentDist_);
    lowpass_.setFrequency(freq);
    return lowpass_.process(x);
}

void AirAbsorption::reset() {
    lowpass_.reset();
    smooth_.reset();
    currentDist_ = distance_;
}

}
