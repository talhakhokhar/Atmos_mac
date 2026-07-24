#pragma once
#include "../common/Types.h"
#include "ParameterDefs.h"
#include <string>
#include <array>

namespace atmos {

struct PresetData {
    std::string name;
    std::array<f32, static_cast<size_t>(ParamID::NumParams)> values;
};

PresetData getFactoryPreset(int index);
int getNumFactoryPresets();

}
