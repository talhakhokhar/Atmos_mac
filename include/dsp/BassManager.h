#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"

namespace atmos {

class BassManager {
public:
    BassManager();
    void setSampleRate(f32 sr);
    void setCrossover(f32 freq);
    void setLFEBoost(f32 boostDb);
    void process(f32& inL, f32& inR, f32& outL, f32& outR);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 crossover_ = 80.0f;
    f32 lfeBoost_ = 0.0f;

    Biquad lpfL_, lpfR_;
    Biquad hpfL_, hpfR_;
    f32 lfeStateL_ = 0.0f;
    f32 lfeStateR_ = 0.0f;
};

}
