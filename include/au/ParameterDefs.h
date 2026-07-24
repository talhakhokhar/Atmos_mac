#pragma once
#include "../common/Types.h"
#include <string>

namespace atmos {

struct ParameterRanges {
    f32 minValue;
    f32 maxValue;
    f32 defaultValue;
    f32 step;
};

struct ParameterInfo {
    std::string name;
    std::string identifier;
    ParameterRanges ranges;
    bool isBypass = false;
};

enum class ParamID : i32 {
    Bypass = 0,
    Mode = 1,
    OutputTarget = 2,
    Width = 3,
    Height = 4,
    Surround = 5,
    ObjectSize = 6,
    HRTFStrength = 7,
    RoomSize = 8,
    ReverbAmount = 9,
    EarlyReflectionLevel = 10,
    Bass = 11,
    Brightness = 12,
    Dialogue = 13,
    Crossfeed = 14,
    LoudnessNormalize = 15,
    TargetLUFS = 16,
    LimiterCeiling = 17,
    HeadSize = 18,
    Profile = 19,
    LowLatency = 20,
    OutputGain = 21,
    NumParams = 22
};

inline const char* getParamName(ParamID id) {
    switch (id) {
        case ParamID::Bypass: return "Bypass";
        case ParamID::Mode: return "Mode";
        case ParamID::OutputTarget: return "Output Target";
        case ParamID::Width: return "Width";
        case ParamID::Height: return "Height";
        case ParamID::Surround: return "Surround";
        case ParamID::ObjectSize: return "Object Size";
        case ParamID::HRTFStrength: return "HRTF Strength";
        case ParamID::RoomSize: return "Room Size";
        case ParamID::ReverbAmount: return "Reverb Amount";
        case ParamID::EarlyReflectionLevel: return "Early Reflections";
        case ParamID::Bass: return "Bass";
        case ParamID::Brightness: return "Brightness";
        case ParamID::Dialogue: return "Dialogue";
        case ParamID::Crossfeed: return "Crossfeed";
        case ParamID::LoudnessNormalize: return "Loudness Normalize";
        case ParamID::TargetLUFS: return "Target LUFS";
        case ParamID::LimiterCeiling: return "Limiter Ceiling";
        case ParamID::HeadSize: return "Head Size";
        case ParamID::Profile: return "Profile";
        case ParamID::LowLatency: return "Low Latency";
        case ParamID::OutputGain: return "Output Gain";
        default: return "Unknown";
    }
}

inline ParameterRanges getParamRanges(ParamID id) {
    switch (id) {
        case ParamID::Bypass: return {0.0f, 1.0f, 0.0f, 1.0f};
        case ParamID::Mode: return {0.0f, 11.0f, 2.0f, 1.0f};
        case ParamID::OutputTarget: return {0.0f, 5.0f, 1.0f, 1.0f};
        case ParamID::Width: return {0.0f, 1.0f, 0.75f, 0.01f};
        case ParamID::Height: return {0.0f, 1.0f, 0.45f, 0.01f};
        case ParamID::Surround: return {0.0f, 1.0f, 0.4f, 0.01f};
        case ParamID::ObjectSize: return {0.0f, 1.0f, 0.35f, 0.01f};
        case ParamID::HRTFStrength: return {0.0f, 1.0f, 1.0f, 0.01f};
        case ParamID::RoomSize: return {0.0f, 1.0f, 0.5f, 0.01f};
        case ParamID::ReverbAmount: return {0.0f, 1.0f, 0.3f, 0.01f};
        case ParamID::EarlyReflectionLevel: return {0.0f, 1.0f, 0.25f, 0.01f};
        case ParamID::Bass: return {-6.0f, 6.0f, 2.0f, 0.1f};
        case ParamID::Brightness: return {-6.0f, 6.0f, 1.0f, 0.1f};
        case ParamID::Dialogue: return {0.0f, 6.0f, 3.0f, 0.1f};
        case ParamID::Crossfeed: return {0.0f, 1.0f, 0.5f, 0.01f};
        case ParamID::LoudnessNormalize: return {0.0f, 1.0f, 1.0f, 1.0f};
        case ParamID::TargetLUFS: return {-24.0f, -10.0f, -16.0f, 1.0f};
        case ParamID::LimiterCeiling: return {-6.0f, 0.0f, -1.0f, 0.1f};
        case ParamID::HeadSize: return {0.0f, 1.0f, 0.5f, 0.01f};
        case ParamID::Profile: return {0.0f, 8.0f, 1.0f, 1.0f};
        case ParamID::LowLatency: return {0.0f, 1.0f, 0.0f, 1.0f};
        case ParamID::OutputGain: return {-12.0f, 12.0f, 0.0f, 0.1f};
        default: return {0.0f, 1.0f, 0.5f, 0.01f};
    }
}

}
