#pragma once
#include "../common/Types.h"
#include "../dsp/Biquad.h"
#include <array>

namespace atmos {

class MasterEQ {
public:
    MasterEQ();
    void setSampleRate(f32 sr);
    void setBass(f32 db);
    void setBrightness(f32 db);
    void setWarmth(f32 db);
    void setPresence(f32 db);
    void setAir(f32 db);
    void setProfile(HeadphoneProfile profile);
    void process(f32& inL, f32& inR);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    f32 bass_ = 0.0f;
    f32 brightness_ = 0.0f;
    f32 warmth_ = 0.0f;
    f32 presence_ = 0.0f;
    f32 air_ = 0.0f;

    std::array<Biquad, 8> filters_;
    void updateFilters();
};

}
