#pragma once
#include "VST3Plug.h"
#include <base/source/factory.h>
#include <pluginterfaces/base/funknown.h>

namespace atmos {

class VST3Factory {
public:
    static FUnknown* createInstance(void* context) {
        VST3Plug* plug = new VST3Plug();
        return reinterpret_cast<FUnknown*>(plug);
    }
};

} // namespace atmos