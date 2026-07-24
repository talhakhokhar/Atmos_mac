#include "VST3Plug.h"
#include "../dsp/SpatialEngine.h"
#include <audioeffect.h>
#include <iaudioeffect.h>
#include <ivstaudiobuffer.h>
#include <vsttypes.h>
#include <cstring>

using namespace atmos;

VST3Plug::VST3Plug() : engine_() {}

VST3Plug::~VST3Plug() = default;

int32_t VST3Plug::queryInterface(uint32_t iid, void** outIface) {
    static const GUID kAudioEffect = {0x3A8B3C6D, 0x5E2F, 0x4A8B, {0x9D, 0x6E, 0x5C, 0x8B, 0x2A, 0x1F, 0x4E, 0x9D}};
    static const GUID kIComponent = {0x3A8B3C6D, 0x5E2F, 0x4A8B, {0x9D, 0x6E, 0x5C, 0x8B, 0x2A, 0x1F, 0x4E, 0x9D}};
    static const GUID kIAudioEffect = {0x3A8B3C6D, 0x5E2F, 0x4A8B, {0x9D, 0x6E, 0x5C, 0x8B, 0x2A, 0x1F, 0x4E, 0x9D}};
    static const GUID kIVstComponent = {0x3A8B3C6D, 0x5E2F, 0x4A8B, {0x9D, 0x6E, 0x5C, 0x8B, 0x2A, 0x1F, 0x4E, 0x9D}};

    *outIface = nullptr;

    if (iid == 0 || iid == kAudioEffect || iid == kIComponent || iid == kIAudioEffect || iid == kIVstComponent) {
        *outIface = this;
        return kResult_OK;
    }

    return kNoInterface;
}

uint32_t VST3Plug::addRef() { return ++refCount_; }

uint32_t VST3Plug::release() {
    uint32_t count = --refCount_;
    if (count == 0) delete this;
    return count;
}

int32_t VST3Plug::initialize(void* context) {
    (void)context;
    active_ = true;
    return kResult_OK;
}

int32_t VST3Plug::terminate() {
    active_ = false;
    return kResult_OK;
}

int32_t VST3Plug::getBusCount(int32_t direction, int32_t busCount) {
    if (direction == kAudioBus) {
        busCount = 1;
        return kResult_OK;
    }
    return kInvalidArgument;
}

int32_t VST3Plug::getBusInfo(int32_t direction, int32_t busIndex, void* busInfo) {
    (void)busIndex;
    if (direction != kAudioBus) return kInvalidArgument;

    StAudioBusInfo* info = static_cast<StAudioBusInfo*>(busInfo);
    info->mediaType = kAudio;
    info->channelCount = 2;
    info->sampleRate = 48000.0;
    info->flags = kAudioBusCanMonitor;
    return kResult_OK;
}

int32_t VST3Plug::getLatency(int32_t* latency) {
    *latency = 0;
    return kResult_OK;
}

int32_t VST3Plug::activateBuses(bool state) {
    active_ = state;
    return kResult_OK;
}

int32_t VST3Plug::setSampleRate(double sampleRate) {
    sampleRate_ = static_cast<f32>(sampleRate);
    engine_.setSampleRate(sampleRate_);
    return kResult_OK;
}

int32_t VST3Plug::setBlockSize(int32_t blockSize) {
    (void)blockSize;
    return kResult_OK;
}

int32_t VST3Plug::setupProcessing(void* setup) {
    (void)setup;
    return kResult_OK;
}

int32_t VST3Plug::teardownProcessing() {
    return kResult_OK;
}

int32_t VST3Plug::canProcessSampleSize(int32_t symbolicSampleSize) {
    if (symbolicSampleSize == kSample32 || symbolicSampleSize == kSample64) {
        return kResult_OK;
    }
    return kInvalidArgument;
}

int32_t VST3Plug::process(void* data) {
    if (!active_) return kResult_OK;

    StProcessData* processData = static_cast<StProcessData*>(data);

    if (processData->numInputs > 0 && processData->inputs[0].numBuffers > 0) {
        float* inL = static_cast<float*>(processData->inputs[0].channelBuffers32[0]);
        float* inR = static_cast<float*>(processData->inputs[0].channelBuffers32[1]);

        for (int32_t i = 0; i < processData->numInputs; ++i) {
            if (processData->inputs[i].numBuffers >= 2) {
                float* outL = static_cast<float*>(processData->outputs[0].channelBuffers32[0]);
                float* outR = static_cast<float*>(processData->outputs[0].channelBuffers32[1]);

                engine_.process(inL, inR, outL, outR, processData->numSamples);
            }
        }
    }

    return kResult_OK;
}

int32_t VST3Plug::setState(void* state) {
    engine_.reset();
    return kResult_OK;
}

int32_t VST3Plug::getState(void* state) {
    return kResult_OK;
}

int32_t VST3Plug::setComponentState(void* state) {
    return kResult_OK;
}

int32_t VST3Plug::getComponentState(void* state) {
    return kResult_OK;
}

void VST3Plug::reset() {
    engine_.reset();
}