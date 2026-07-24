#pragma once
#include "../common/Types.h"
#include <array>

namespace atmos {

class ContentClassifier {
public:
    ContentClassifier();
    SoundMode classify(const f32* inL, const f32* inR, i32 numFrames, f32 sampleRate);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    std::array<f32, 8> featureHistory_;
    int historyIndex_ = 0;
    bool historyFilled_ = false;

    f32 calcCorrelation(const f32* L, const f32* R, i32 n);
    f32 calcSideEnergy(const f32* L, const f32* R, i32 n);
    f32 calcSpeechEnergy(const f32* L, const f32* R, i32 n);
    f32 calcLowEnergy(const f32* L, const f32* R, i32 n);
    f32 calcTransientDensity(const f32* L, const f32* R, i32 n);
    f32 calcSpectralCentroid(const f32* L, const f32* R, i32 n);
};

}
