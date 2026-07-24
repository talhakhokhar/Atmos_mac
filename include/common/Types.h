#pragma once
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <functional>
#include <array>
#include <vector>
#include <string>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#ifndef M_TAU
#define M_TAU 6.28318530717958647692f
#endif

namespace atmos {

using f32 = float;
using f64 = double;
using u32 = uint32_t;
using u64 = uint64_t;
using i32 = int32_t;
using i64 = int64_t;

static constexpr f32 kPi = M_PI;
static constexpr f32 kTau = M_TAU;
static constexpr f32 kSpeedOfSound = 343.0f;
static constexpr f32 kHeadRadius = 0.0875f;
static constexpr f32 kMaxITD = 0.0007f;
static constexpr f32 kEpsilon = 1e-8f;

constexpr f32 clamp(f32 v, f32 lo, f32 hi) noexcept {
    return std::max(lo, std::min(hi, v));
}

constexpr f32 lerp(f32 a, f32 b, f32 t) noexcept {
    return a + t * (b - a);
}

constexpr f32 dbToLinear(f32 db) noexcept {
    return std::pow(10.0f, db * 0.05f);
}

constexpr f32 linearToDb(f32 lin) noexcept {
    return 20.0f * std::log10(std::max(lin, 1e-10f));
}

struct AudioBus {
    f32* data = nullptr;
    i32 numFrames = 0;
    i32 numChannels = 0;
};

enum class OutputTarget : i32 {
    Auto = 0,
    Headphones = 1,
    IEM = 2,
    LineAux = 3,
    CarAux = 4,
    Reference = 5
};

enum class SoundMode : i32 {
    Off = 0,
    Reference = 1,
    Auto = 2,
    Movie = 3,
    Music = 4,
    Game = 5,
    Voice = 6,
    Night = 7,
    BassPlus = 8,
    IEMSpatial = 9,
    AuxLine = 10,
    Car = 11
};

enum class HeadphoneProfile : i32 {
    Flat = 0,
    Premium = 1,
    Warm = 2,
    Bright = 3,
    ClosedBack = 4,
    OpenBack = 5,
    Earbuds = 6,
    IEMBalanced = 7,
    IEMSafe = 8
};

}
