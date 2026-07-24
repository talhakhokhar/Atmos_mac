#pragma once
#include "../common/Types.h"
#include "../dsp/SpatialEngine.h"
#include <cstdint>
#include <cstring>

namespace atmos {

class VST3Plug {
public:
    VST3Plug();
    ~VST3Plug();

    int32_t queryInterface(uint32_t iid, void** outIface);
    uint32_t addRef();
    uint32_t release();

    // Initialize/terminate
    int32_t initialize(void* context);
    int32_t terminate();

    // Processing
    int32_t getBusCount(int32_t direction, int32_t busCount);
    int32_t getBusInfo(int32_t direction, int32_t busIndex, void* busInfo);
    int32_t getLatency(int32_t* latency);
    int32_t activateBuses(bool state);

    int32_t setSampleRate(double sampleRate);
    int32_t setBlockSize(int32_t blockSize);
    int32_t setupProcessing(void* setup);
    int32_t teardownProcessing();
    int32_t canProcessSampleSize(int32_t symbolicSampleSize);
    int32_t process(void* data);

    // Preset/pass-through
    int32_t setState(void* state);
    int32_t getState(void* state);
    int32_t setComponentState(void* state);
    int32_t getComponentState(void* state);

    void reset();

private:
    SpatialEngine engine_;
    f32 sampleRate_ = 48000.0f;
    bool active_ = false;
    uint32_t refCount_ = 1;
};

}
