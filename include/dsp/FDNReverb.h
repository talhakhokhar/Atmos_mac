#pragma once
#include "../common/Types.h"
#include "../dsp/DelayLine.h"
#include "../dsp/Biquad.h"
#include <array>

namespace atmos {

class FDNReverb {
public:
    FDNReverb();
    void setSampleRate(f32 sr);
    void setRoomSize(f32 size);
    void setDecay(f32 decay);
    void setModulation(f32 modRate, f32 modDepth);
    void process(f32 inL, f32 inR, f32& outL, f32& outR);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 roomSize_ = 0.5f;
    f32 decay_ = 0.4f;
    f32 modRate_ = 0.2f;
    f32 modDepth_ = 4.0f;

    std::array<i32, 8> delayLengths_;
    std::array<DelayLine, 8> delays_;
    std::array<Biquad, 8> dampingFilters_;
    std::array<f32, 8> feedbackGains_;
    std::array<f32, 8> outputGainsL_;
    std::array<f32, 8> outputGainsR_;

    Biquad inputHp_;
    Biquad outputLp_;
    f32 modPhase_ = 0.0f;
};

}
