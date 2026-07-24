#pragma once
#include "../common/Types.h"
#include "../dsp/DelayLine.h"
#include "../dsp/Biquad.h"
#include "../dsp/ParamSmooth.h"

namespace atmos {

class EarlyReflections {
public:
    EarlyReflections();
    void setSampleRate(f32 sr);
    void setRoomSize(f32 size);
    void setLevel(f32 level);
    void process(f32 inL, f32 inR, f32& outL, f32& outR);
    void reset();

private:
    struct Reflection {
        DelayLine delay;
        f32 delayMs = 10.0f;
        f32 gain = 0.5f;
        f32 azimuth = 0.0f;
        f32 elevation = 0.0f;
        Biquad hp;
    };

    f32 sampleRate_ = 48000.0f;
    f32 roomSize_ = 0.5f;
    f32 level_ = 0.25f;

    std::array<Reflection, 6> reflections_;
    Biquad inputHp_;
    ParamSmooth smoothLevel_;
};

}
