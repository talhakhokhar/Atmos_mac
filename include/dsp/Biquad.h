#pragma once
#include "../common/Types.h"
#include <array>

namespace atmos {

enum class BiquadType : i32 {
    LowShelf = 0,
    HighShelf = 1,
    Peaking = 2,
    LowPass = 3,
    HighPass = 4,
    BandPass = 5,
    AllPass = 6,
    LinkwitzRileyLP = 7,
    LinkwitzRileyHP = 8
};

struct BiquadCoeffs {
    f32 b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    f32 a1 = 0.0f, a2 = 0.0f;
};

class Biquad {
public:
    Biquad();
    void setType(BiquadType type);
    void setFrequency(f32 freq);
    void setQ(f32 q);
    void setGainDb(f32 gainDb);
    void setSampleRate(f32 sr);
    void calculateCoeffs();
    BiquadCoeffs getCoeffs() const { return c_; }
    f32 process(f32 x);
    void processBlock(f32* buffer, i32 numFrames);
    void reset();

private:
    BiquadType type_ = BiquadType::Peaking;
    f32 freq_ = 1000.0f;
    f32 q_ = 1.0f;
    f32 gainDb_ = 0.0f;
    f32 sampleRate_ = 48000.0f;
    BiquadCoeffs c_;
    f32 x1_ = 0.0f, x2_ = 0.0f;
    f32 y1_ = 0.0f, y2_ = 0.0f;
};

}
