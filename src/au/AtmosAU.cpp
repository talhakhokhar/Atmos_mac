#include "au/AtmosAU.h"
#include "au/PresetManager.h"

#ifdef __APPLE__
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AudioUnitProperties.h>
#include <AudioToolbox/AudioComponent.h>
#include <CoreFoundation/CoreFoundation.h>
#else
// Stub types for non-macOS builds (testing only)
typedef int32_t OSStatus;
typedef uint32_t UInt32;
typedef uint64_t UInt64;
typedef int32_t SInt16;
typedef uint32_t AudioUnitPropertyID;
typedef uint32_t AudioUnitScope;
typedef uint32_t AudioUnitElement;
typedef uint32_t Boolean;
typedef uint32_t AudioUnitParameterID;
typedef float AudioUnitParameterValue;
typedef float Float64;
typedef void* AudioUnitPropertyListenerProc;
typedef void* AURenderCallback;

// Stub AudioBuffer structure
struct AudioBufferStub {
    UInt32 mNumberChannels;
    UInt32 mDataByteSize;
    void* mData;
};

// Stub AudioBufferList structure  
struct AudioBufferListStub {
    UInt32 mNumberBuffers;
    AudioBufferStub mBuffers[1];
};

// Stub AudioStreamBasicDescription
struct AudioStreamBasicDescriptionStub {
    Float64 mSampleRate;
    UInt32 mFormatID;
    UInt32 mFormatFlags;
    UInt32 mBytesPerPacket;
    UInt32 mFramesPerPacket;
    UInt32 mBytesPerFrame;
    UInt32 mChannelsPerFrame;
    UInt32 mBitsPerChannel;
};

// Stub AudioTimeStamp
struct AudioTimeStampStub {
    Float64 mSampleTime;
    UInt64 mHostTime;
    Float64 mRateScalar;
    UInt64 mWordClockTime;
    AudioStreamBasicDescriptionStub mSMPTETime;
    UInt32 mFlags;
    UInt32 mReserved;
};

// Stub AudioComponentPlugInInterface
struct AudioComponentPlugInInterfaceStub {
    OSStatus (*Open)(void* self, void* mInstance);
    OSStatus (*Close)(void* self);
    void* (*Lookup)(SInt16 selector);
    void* reserved;
};

typedef AudioBufferListStub AudioBufferList;
typedef AudioStreamBasicDescriptionStub AudioStreamBasicDescription;
typedef AudioTimeStampStub AudioTimeStamp;
typedef AudioComponentPlugInInterfaceStub AudioComponentPlugInInterface;
typedef void* AudioComponentInstance;
typedef void* AudioComponentDescription;
typedef void* AudioUnitRenderActionFlags;
typedef void* AudioUnitParameterEvent;
typedef void* AudioComponentMethod;

// Stub constants for non-macOS builds
#define kAudio_ParamError -10000
#define noErr 0
#define kAudioUnitErr_InvalidProperty -10870
#define kAudioUnitErr_InvalidParameter -10851
#define kAudioUnitInitializeSelect 1
#define kAudioUnitUninitializeSelect 2
#define kAudioUnitGetPropertyInfoSelect 3
#define kAudioUnitGetPropertySelect 4
#define kAudioUnitSetPropertySelect 5
#define kAudioUnitGetParameterSelect 6
#define kAudioUnitSetParameterSelect 7
#define kAudioUnitRenderSelect 11
#define kAudioUnitResetSelect 12
#define kAudioUnitAddPropertyListenerSelect 13
#define kAudioUnitRemovePropertyListenerSelect 14
#define kAudioUnitRemovePropertyListenerWithUserDataSelect 15
#define kAudioUnitAddRenderNotifySelect 16
#define kAudioUnitRemoveRenderNotifySelect 17
#define kAudioUnitScheduleParametersSelect 19
#define kAudioUnitProcessSelect 8
#define kAudioUnitProcessMultipleSelect 9
#define kAudioUnitComplexRenderSelect 10
#define kAudioUnitScope_Global 0
#define kAudioUnitScope_Input 1
#define kAudioUnitScope_Output 2
#define kAudioUnitProperty_Latency 22
#define kAudioUnitProperty_TailTime 23
#define kAudioUnitProperty_BusCount 33
#define kAudioUnitProperty_MaximumFramesPerSlice 34
#define kAudioUnitProperty_StreamFormat 8
#define kAudioUnitProperty_SupportedNumChannels 27
#define kAudioUnitProperty_ParameterInfo 18
#define kAudioUnitProperty_ParameterList 17
#define kAudioUnitProperty_FactoryPresets 20
#define kAudioUnitProperty_PresentPreset 21
#define kAudioUnitProperty_BypassEffect 28
#define kAudioFormatLinearPCM 1819704624  // 'lpcm' as integer
#define kAudioFormatFlagIsFloat (1 << 1)
#define kAudioFormatFlagIsPacked (1 << 3)
#endif

#include <mutex>
#include <cstring>
#include <algorithm>

using namespace atmos;

// =============================================================================
// Per-instance storage
// =============================================================================
struct AtmospherePlugInInstance {
    AudioComponentPlugInInterface mPlugInInterface;
    AtmosAU* mInstance;
};

static std::recursive_mutex gAUMutex;

static AtmosAU* GetInstance(void* self) {
    auto* plugInstance = static_cast<AtmospherePlugInInstance*>(self);
    return plugInstance ? plugInstance->mInstance : nullptr;
}

// =============================================================================
// AudioUnit Dispatch Wrappers
// =============================================================================

static OSStatus AtmosAU_Initialize(void* self) {
    AtmosAU* au = GetInstance(self);
    return au ? au->Initialize() : kAudio_ParamError;
}

static OSStatus AtmosAU_Uninitialize(void* self) {
    AtmosAU* au = GetInstance(self);
    return au ? au->Uninitialize() : kAudio_ParamError;
}

static OSStatus AtmosAU_GetPropertyInfo(void* self, AudioUnitPropertyID prop, AudioUnitScope scope,
                                        AudioUnitElement elem, UInt32* outDataSize, Boolean* outWritable) {
    AtmosAU* au = GetInstance(self);
    if (!au) return kAudio_ParamError;
    UInt32 size = 0;
    Boolean writable = false;
    OSStatus result = au->GetPropertyInfo(prop, scope, elem, size, writable);
    if (outDataSize) *outDataSize = size;
    if (outWritable) *outWritable = writable;
    return result;
}

static OSStatus AtmosAU_GetProperty(void* self, AudioUnitPropertyID inID, AudioUnitScope inScope,
                                    AudioUnitElement inElement, void* outData, UInt32* ioDataSize) {
    AtmosAU* au = GetInstance(self);
    if (!au) return kAudio_ParamError;
    if (ioDataSize == nullptr) return kAudio_ParamError;
    UInt32 size = *ioDataSize;
    OSStatus result = au->GetProperty(inID, inScope, inElement, size, outData);
    *ioDataSize = size;
    return result;
}

static OSStatus AtmosAU_SetProperty(void* self, AudioUnitPropertyID inID, AudioUnitScope inScope,
                                    AudioUnitElement inElement, const void* inData, UInt32 inDataSize) {
    AtmosAU* au = GetInstance(self);
    return au ? au->SetProperty(inID, inScope, inElement, inDataSize, inData) : kAudio_ParamError;
}

static OSStatus AtmosAU_GetParameter(void* self, AudioUnitParameterID param, AudioUnitScope scope,
                                     AudioUnitElement elem, AudioUnitParameterValue* value) {
    AtmosAU* au = GetInstance(self);
    if (!au) return kAudio_ParamError;
    if (value == nullptr) return kAudio_ParamError;
    return au->GetParameter(param, scope, elem, *value);
}

static OSStatus AtmosAU_SetParameter(void* self, AudioUnitParameterID param, AudioUnitScope scope,
                                     AudioUnitElement elem, AudioUnitParameterValue value, UInt32 bufferOffset) {
    AtmosAU* au = GetInstance(self);
    return au ? au->SetParameter(param, scope, elem, value, bufferOffset) : kAudio_ParamError;
}

static OSStatus AtmosAU_Render(void* self, AudioUnitRenderActionFlags* ioActionFlags,
                               const AudioTimeStamp* inTimeStamp, UInt32 inOutputBusNumber,
                               UInt32 inNumberFrames, AudioBufferList* ioData) {
    AtmosAU* au = GetInstance(self);
    return au ? au->Render(ioActionFlags, inTimeStamp, inNumberFrames, ioData) : kAudio_ParamError;
}

static OSStatus AtmosAU_Reset(void* self, AudioUnitScope inScope, AudioUnitElement inElement) {
    (void)self; (void)inScope; (void)inElement;
    return noErr;
}

static OSStatus AtmosAU_AddPropertyListener(void* self, AudioUnitPropertyID prop,
                                            AudioUnitPropertyListenerProc proc, void* userData) {
    (void)self; (void)prop; (void)proc; (void)userData;
    return noErr;
}

static OSStatus AtmosAU_RemovePropertyListener(void* self, AudioUnitPropertyID prop,
                                               AudioUnitPropertyListenerProc proc) {
    (void)self; (void)prop; (void)proc;
    return noErr;
}

static OSStatus AtmosAU_RemovePropertyListenerWithUserData(void* self, AudioUnitPropertyID prop,
                                                           AudioUnitPropertyListenerProc proc,
                                                           void* userData) {
    (void)self; (void)prop; (void)proc; (void)userData;
    return noErr;
}

static OSStatus AtmosAU_AddRenderNotify(void* self, AURenderCallback proc, void* userData) {
    (void)self; (void)proc; (void)userData;
    return noErr;
}

static OSStatus AtmosAU_RemoveRenderNotify(void* self, AURenderCallback proc, void* userData) {
    (void)self; (void)proc; (void)userData;
    return noErr;
}

static OSStatus AtmosAU_ScheduleParameters(void* self, const AudioUnitParameterEvent* events, UInt32 numEvents) {
    (void)self; (void)events; (void)numEvents;
    return noErr;
}

// =============================================================================
// Vtable Lookup
// =============================================================================

static AudioComponentMethod AtmosAULookup(SInt16 selector) {
    switch (selector) {
        case kAudioUnitInitializeSelect:
            return (AudioComponentMethod)AtmosAU_Initialize;
        case kAudioUnitUninitializeSelect:
            return (AudioComponentMethod)AtmosAU_Uninitialize;
        case kAudioUnitGetPropertyInfoSelect:
            return (AudioComponentMethod)AtmosAU_GetPropertyInfo;
        case kAudioUnitGetPropertySelect:
            return (AudioComponentMethod)AtmosAU_GetProperty;
        case kAudioUnitSetPropertySelect:
            return (AudioComponentMethod)AtmosAU_SetProperty;
        case kAudioUnitGetParameterSelect:
            return (AudioComponentMethod)AtmosAU_GetParameter;
        case kAudioUnitSetParameterSelect:
            return (AudioComponentMethod)AtmosAU_SetParameter;
        case kAudioUnitRenderSelect:
            return (AudioComponentMethod)AtmosAU_Render;
        case kAudioUnitResetSelect:
            return (AudioComponentMethod)AtmosAU_Reset;
        case kAudioUnitAddPropertyListenerSelect:
            return (AudioComponentMethod)AtmosAU_AddPropertyListener;
        case kAudioUnitRemovePropertyListenerSelect:
            return (AudioComponentMethod)AtmosAU_RemovePropertyListener;
        case kAudioUnitRemovePropertyListenerWithUserDataSelect:
            return (AudioComponentMethod)AtmosAU_RemovePropertyListenerWithUserData;
        case kAudioUnitAddRenderNotifySelect:
            return (AudioComponentMethod)AtmosAU_AddRenderNotify;
        case kAudioUnitRemoveRenderNotifySelect:
            return (AudioComponentMethod)AtmosAU_RemoveRenderNotify;
        case kAudioUnitScheduleParametersSelect:
            return (AudioComponentMethod)AtmosAU_ScheduleParameters;
        case kAudioUnitProcessSelect:
        case kAudioUnitProcessMultipleSelect:
        case kAudioUnitComplexRenderSelect:
            return nullptr;
        default:
            break;
    }
    return nullptr;
}

// =============================================================================
// Open / Close
// =============================================================================

static OSStatus AtmosAUOpen(void* self, AudioComponentInstance mInstance) {
    (void)mInstance;
    std::lock_guard<std::recursive_mutex> lock(gAUMutex);

    auto* plugInstance = static_cast<AtmospherePlugInInstance*>(self);
    AtmosAU* au = new AtmosAU();
    au->Initialize();
    plugInstance->mInstance = au;

    return noErr;
}

static OSStatus AtmosAUClose(void* self) {
    std::lock_guard<std::recursive_mutex> lock(gAUMutex);

    auto* plugInstance = static_cast<AtmospherePlugInInstance*>(self);
    if (plugInstance->mInstance) {
        delete plugInstance->mInstance;
        plugInstance->mInstance = nullptr;
    }
    free(plugInstance);
    return noErr;
}

// =============================================================================
// Factory Function
// =============================================================================

extern "C" {
    __attribute__((visibility("default")))
    AudioComponentPlugInInterface* AtmosAUFactory(const AudioComponentDescription* inDesc) {
        (void)inDesc;
        auto* plugInstance = static_cast<AtmospherePlugInInstance*>(
            malloc(sizeof(AtmospherePlugInInstance)));
        plugInstance->mPlugInInterface.Open = AtmosAUOpen;
        plugInstance->mPlugInInterface.Close = AtmosAUClose;
        plugInstance->mPlugInInterface.Lookup = AtmosAULookup;
        plugInstance->mPlugInInterface.reserved = nullptr;
        plugInstance->mInstance = nullptr;
        return &plugInstance->mPlugInInterface;
    }
}

// =============================================================================
// AtmosAU Implementation
// =============================================================================

AtmosAU::AtmosAU() {
    params_.fill(0.0f);
    params_[static_cast<size_t>(ParamID::Mode)] = 2.0f;
    params_[static_cast<size_t>(ParamID::OutputTarget)] = 1.0f;
    params_[static_cast<size_t>(ParamID::Width)] = 0.75f;
    params_[static_cast<size_t>(ParamID::Height)] = 0.45f;
    params_[static_cast<size_t>(ParamID::Surround)] = 0.4f;
    params_[static_cast<size_t>(ParamID::ObjectSize)] = 0.35f;
    params_[static_cast<size_t>(ParamID::HRTFStrength)] = 1.0f;
    params_[static_cast<size_t>(ParamID::RoomSize)] = 0.5f;
    params_[static_cast<size_t>(ParamID::ReverbAmount)] = 0.3f;
    params_[static_cast<size_t>(ParamID::EarlyReflectionLevel)] = 0.25f;
    params_[static_cast<size_t>(ParamID::Bass)] = 0.33f;
    params_[static_cast<size_t>(ParamID::Brightness)] = 0.17f;
    params_[static_cast<size_t>(ParamID::Dialogue)] = 0.5f;
    params_[static_cast<size_t>(ParamID::Crossfeed)] = 0.5f;
    params_[static_cast<size_t>(ParamID::TargetLUFS)] = 0.5f;
    params_[static_cast<size_t>(ParamID::LimiterCeiling)] = 0.9f;
    params_[static_cast<size_t>(ParamID::Profile)] = 1.0f;
    params_[static_cast<size_t>(ParamID::OutputGain)] = 0.5f;

    inputFormat_.mSampleRate = 48000.0;
    inputFormat_.mFormatID = kAudioFormatLinearPCM;
    inputFormat_.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
    inputFormat_.mBytesPerPacket = 8;
    inputFormat_.mFramesPerPacket = 1;
    inputFormat_.mBytesPerFrame = 8;
    inputFormat_.mChannelsPerFrame = 2;
    inputFormat_.mBitsPerChannel = 32;

    outputFormat_ = inputFormat_;

    engine_ = std::make_unique<SpatialEngine>();
    for (auto& s : paramSmoothers_) {
        s.setSampleRate(48000.0f);
        s.setTimeConstant(0.02f);
    }
}

AtmosAU::~AtmosAU() = default;

OSStatus AtmosAU::Initialize() {
    if (initialized_) return noErr;
    sampleRate_ = static_cast<f32>(inputFormat_.mSampleRate);
    engine_->setSampleRate(sampleRate_);
    syncEngineFromParams();
    initialized_ = true;
    return noErr;
}

OSStatus AtmosAU::Uninitialize() {
    initialized_ = false;
    return noErr;
}

OSStatus AtmosAU::GetPropertyInfo(AudioUnitPropertyID propID, AudioUnitScope scope, AudioUnitElement element,
                                  UInt32& outDataSize, Boolean& outWritable) {
    (void)element;
    outWritable = false;

    switch (propID) {
        case kAudioUnitProperty_Latency:
            if (scope == kAudioUnitScope_Global) {
                outDataSize = sizeof(Float64);
                return noErr;
            }
            break;
        case kAudioUnitProperty_TailTime:
            if (scope == kAudioUnitScope_Global) {
                outDataSize = sizeof(Float64);
                return noErr;
            }
            break;
        case kAudioUnitProperty_BusCount:
            if (scope == kAudioUnitScope_Input || scope == kAudioUnitScope_Output) {
                outDataSize = sizeof(UInt32);
                outWritable = false;
                return noErr;
            }
            break;
        case kAudioUnitProperty_MaximumFramesPerSlice:
            if (scope == kAudioUnitScope_Global) {
                outDataSize = sizeof(UInt32);
                outWritable = true;
                return noErr;
            }
            break;
        case kAudioUnitProperty_StreamFormat:
            if ((scope == kAudioUnitScope_Input && element < inputBusCount_) ||
                (scope == kAudioUnitScope_Output && element < outputBusCount_)) {
                outDataSize = sizeof(AudioStreamBasicDescription);
                outWritable = true;
                return noErr;
            }
            break;
        case kAudioUnitProperty_SupportedNumChannels:
            if (scope == kAudioUnitScope_Global) {
                outDataSize = 4;
                outWritable = false;
                return noErr;
            }
            break;
        case kAudioUnitProperty_ParameterInfo:
            if (scope == kAudioUnitScope_Global && element < static_cast<AudioUnitElement>(ParamID::NumParams)) {
                outDataSize = sizeof(AudioUnitParameterInfo);
                outWritable = false;
                return noErr;
            }
            break;
        case kAudioUnitProperty_ParameterList:
        case kAudioUnitProperty_FactoryPresets:
        case kAudioUnitProperty_PresentPreset:
            if (scope == kAudioUnitScope_Global) {
                outDataSize = (propID == kAudioUnitProperty_ParameterList)
                    ? sizeof(AudioUnitParameterID) * static_cast<UInt32>(ParamID::NumParams)
                    : sizeof(AUPreset);
                return noErr;
            }
            break;
        case kAudioUnitProperty_BypassEffect:
            if (scope == kAudioUnitScope_Global) {
                outDataSize = sizeof(UInt32);
                outWritable = true;
                return noErr;
            }
            break;
        default:
            break;
    }
    return kAudioUnitErr_InvalidProperty;
}

OSStatus AtmosAU::GetProperty(AudioUnitPropertyID propID, AudioUnitScope scope, AudioUnitElement element,
                              UInt32& ioDataSize, void* outData) {
    (void)element;

    switch (propID) {
        case kAudioUnitProperty_Latency: {
            if (outData == nullptr) {
                ioDataSize = sizeof(Float64);
                return noErr;
            }
            if (ioDataSize < sizeof(Float64)) return kAudioUnitErr_InvalidParameter;
            *static_cast<Float64*>(outData) = 0.008;
            ioDataSize = sizeof(Float64);
            return noErr;
        }
        case kAudioUnitProperty_TailTime: {
            if (outData == nullptr) {
                ioDataSize = sizeof(Float64);
                return noErr;
            }
            if (ioDataSize < sizeof(Float64)) return kAudioUnitErr_InvalidParameter;
            *static_cast<Float64*>(outData) = 2.0;
            ioDataSize = sizeof(Float64);
            return noErr;
        }
        case kAudioUnitProperty_BusCount: {
            if (scope == kAudioUnitScope_Input || scope == kAudioUnitScope_Output || scope == kAudioUnitScope_Global) {
                if (ioDataSize < sizeof(UInt32)) return kAudioUnitErr_InvalidParameter;
                UInt32 count = 1;
                if (scope == kAudioUnitScope_Input) count = inputBusCount_;
                else if (scope == kAudioUnitScope_Output) count = outputBusCount_;
                *static_cast<UInt32*>(outData) = count;
                ioDataSize = sizeof(UInt32);
                return noErr;
            }
            break;
        }
        case kAudioUnitProperty_MaximumFramesPerSlice: {
            if (scope == kAudioUnitScope_Global) {
                if (ioDataSize < sizeof(UInt32)) return kAudioUnitErr_InvalidParameter;
                *static_cast<UInt32*>(outData) = 4096;
                ioDataSize = sizeof(UInt32);
                return noErr;
            }
            break;
        }
        case kAudioUnitProperty_StreamFormat: {
            if (scope == kAudioUnitScope_Input && element < inputBusCount_) {
                if (ioDataSize < sizeof(AudioStreamBasicDescription)) return kAudioUnitErr_InvalidParameter;
                *static_cast<AudioStreamBasicDescription*>(outData) = inputFormat_;
                ioDataSize = sizeof(AudioStreamBasicDescription);
                return noErr;
            }
            if (scope == kAudioUnitScope_Output && element < outputBusCount_) {
                if (ioDataSize < sizeof(AudioStreamBasicDescription)) return kAudioUnitErr_InvalidParameter;
                *static_cast<AudioStreamBasicDescription*>(outData) = outputFormat_;
                ioDataSize = sizeof(AudioStreamBasicDescription);
                return noErr;
            }
            break;
        }
        case kAudioUnitProperty_ParameterList: {
            if (scope == kAudioUnitScope_Global) {
                auto* list = static_cast<AudioUnitParameterID*>(outData);
                for (int i = 0; i < static_cast<int>(ParamID::NumParams); ++i) {
                    list[i] = static_cast<AudioUnitParameterID>(i);
                }
                ioDataSize = sizeof(AudioUnitParameterID) * static_cast<UInt32>(ParamID::NumParams);
                return noErr;
            }
            break;
        }
        case kAudioUnitProperty_FactoryPresets:
        case kAudioUnitProperty_PresentPreset: {
            if (scope == kAudioUnitScope_Global) {
                AUPreset preset = { 0, CFSTR("Default") };
                if (ioDataSize < sizeof(AUPreset)) return kAudioUnitErr_InvalidParameter;
                *static_cast<AUPreset*>(outData) = preset;
                ioDataSize = sizeof(AUPreset);
                return noErr;
            }
            break;
        }
        case kAudioUnitProperty_SupportedNumChannels:
            if (scope == kAudioUnitScope_Global) {
                outDataSize = 4;
                outWritable = false;
                return noErr;
            }
            break;
        case kAudioUnitProperty_ParameterInfo: {
            if (scope == kAudioUnitScope_Global && element < static_cast<AudioUnitElement>(ParamID::NumParams)) {
                auto param = static_cast<ParamID>(static_cast<int>(element));
                AudioUnitParameterInfo info = {};
                const char* paramName = getParamName(param);
                strncpy(info.name, paramName, sizeof(info.name) - 1);
                info.cfNameString = CFStringCreateWithCString(nullptr, paramName, kCFStringEncodingUTF8);
                info.minValue = getParamRanges(param).minValue;
                info.maxValue = getParamRanges(param).maxValue;
                info.defaultValue = getParamRanges(param).defaultValue;
                info.unit = kAudioUnitParameterUnit_Generic;
                info.flags = kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_IsReadable;
                if (param == ParamID::Bypass) {
                    info.flags |= kAudioUnitParameterFlag_IsElementMeta;
                    info.unit = kAudioUnitParameterUnit_Boolean;
                }
                *static_cast<AudioUnitParameterInfo*>(outData) = info;
                ioDataSize = sizeof(AudioUnitParameterInfo);
                return noErr;
            }
            break;
        }
        case kAudioUnitProperty_ClassInfo: {
            if (scope == kAudioUnitScope_Global) {
                if (ioDataSize < sizeof(CFStringRef)) return kAudioUnitErr_InvalidParameter;
                *static_cast<CFStringRef*>(outData) = CFSTR("com.atmos.spatial-audio");
                ioDataSize = sizeof(CFStringRef);
                return noErr;
            }
            break;
        }
        case kAudioUnitProperty_BypassEffect: {
            if (scope == kAudioUnitScope_Global) {
                if (ioDataSize < sizeof(UInt32)) return kAudioUnitErr_InvalidParameter;
                *static_cast<UInt32*>(outData) = params_[static_cast<size_t>(ParamID::Bypass)] > 0.5f ? 1 : 0;
                ioDataSize = sizeof(UInt32);
                return noErr;
            }
            break;
        }
        default:
            return kAudioUnitErr_InvalidProperty;
    }
    return kAudioUnitErr_InvalidProperty;
}

OSStatus AtmosAU::SetProperty(AudioUnitPropertyID propID, AudioUnitScope scope, AudioUnitElement element,
                              UInt32 inDataSize, const void* inData) {
    switch (propID) {
        case kAudioUnitProperty_StreamFormat: {
            if (inData == nullptr || inDataSize < sizeof(AudioStreamBasicDescription)) {
                return kAudio_ParamError;
            }
            const AudioStreamBasicDescription* fmt = static_cast<const AudioStreamBasicDescription*>(inData);
            if (scope == kAudioUnitScope_Input && element < inputBusCount_) {
                inputFormat_ = *fmt;
                return noErr;
            }
            if (scope == kAudioUnitScope_Output && element < outputBusCount_) {
                outputFormat_ = *fmt;
                return noErr;
            }
            return kAudio_ParamError;
        }
        case kAudioUnitProperty_MaximumFramesPerSlice: {
            if (scope == kAudioUnitScope_Global && inDataSize >= sizeof(UInt32)) {
                notifyPropertyListeners(kAudioUnitProperty_MaximumFramesPerSlice);
                return noErr;
            }
            return kAudio_ParamError;
        }
        case kAudioUnitProperty_BypassEffect: {
            if (scope == kAudioUnitScope_Global && inDataSize >= sizeof(UInt32)) {
                UInt32 bypass = *static_cast<const UInt32*>(inData);
                setParameterValue(ParamID::Bypass, bypass > 0 ? 1.0f : 0.0f);
                return noErr;
            }
            return kAudio_ParamError;
        }
        default:
            break;
    }
    return noErr;
}

OSStatus AtmosAU::GetParameter(AudioUnitParameterID paramID, AudioUnitScope scope, AudioUnitElement element,
                               AudioUnitParameterValue& outValue) {
    (void)scope; (void)element;
    if (paramID < static_cast<UInt32>(ParamID::NumParams)) {
        outValue = params_[paramID];
        return noErr;
    }
    return kAudioUnitErr_InvalidParameter;
}

OSStatus AtmosAU::SetParameter(AudioUnitParameterID paramID, AudioUnitScope scope, AudioUnitElement element,
                               AudioUnitParameterValue inValue, UInt32 inBufferOffsetInFrames) {
    (void)scope; (void)element; (void)inBufferOffsetInFrames;
    if (paramID < static_cast<UInt32>(ParamID::NumParams)) {
        paramSmoothers_[paramID].setTarget(inValue);
        params_[paramID] = inValue;
        if (paramID == static_cast<UInt32>(ParamID::Bypass)) {
            notifyPropertyListeners(kAudioUnitProperty_BypassEffect);
        }
        return noErr;
    }
    return kAudioUnitErr_InvalidParameter;
}

OSStatus AtmosAU::Render(AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp,
                        UInt32 nFrames, AudioBufferList* ioData) {
    (void)inTimeStamp;
    (void)ioActionFlags;

    if (!initialized_ || !engine_) {
        return noErr;
    }

    if (ioData == nullptr || ioData->mNumberBuffers < 2) {
        return kAudio_ParamError;
    }

    f32* inL = static_cast<f32*>(ioData->mBuffers[0].mData);
    f32* inR = static_cast<f32*>(ioData->mBuffers[1].mData);
    f32* outL = inL;
    f32* outR = inR;

    if (ioData->mNumberBuffers >= 4) {
        outL = static_cast<f32*>(ioData->mBuffers[2].mData);
        outR = static_cast<f32*>(ioData->mBuffers[3].mData);
    }

    bool bypassed = params_[static_cast<size_t>(ParamID::Bypass)] > 0.5f;

    if (bypassed) {
        if (outL != inL || outR != inR) {
            memcpy(outL, inL, nFrames * sizeof(f32));
            memcpy(outR, inR, nFrames * sizeof(f32));
        }
        return noErr;
    }

    for (size_t i = 0; i < static_cast<size_t>(ParamID::NumParams); ++i) {
        paramSmoothers_[i].process();
    }
    syncEngineFromParams();

    engine_->process(inL, inR, outL, outR, static_cast<i32>(nFrames));

    f32 gain = dbToLinear((params_[static_cast<size_t>(ParamID::OutputGain)] - 0.5f) * 24.0f);
    for (UInt32 n = 0; n < nFrames; ++n) {
        outL[n] *= gain;
        outR[n] *= gain;
    }

    return noErr;
}

void AtmosAU::setParameterValue(ParamID id, f32 value) {
    size_t idx = static_cast<size_t>(id);
    if (idx < static_cast<size_t>(ParamID::NumParams)) {
        params_[idx] = value;
        paramSmoothers_[idx].setTarget(value);
    }
}

f32 AtmosAU::getParameterValue(ParamID id) const {
    size_t idx = static_cast<size_t>(id);
    if (idx < static_cast<size_t>(ParamID::NumParams)) {
        return params_[idx];
    }
    return 0.0f;
}

void AtmosAU::loadPreset(const PresetData& preset) {
    for (size_t i = 0; i < static_cast<size_t>(ParamID::NumParams); ++i) {
        setParameterValue(static_cast<ParamID>(i), preset.values[i]);
    }
}

void AtmosAU::syncEngineFromParams() {
    engine_->setMode(static_cast<SoundMode>(static_cast<i32>(params_[static_cast<size_t>(ParamID::Mode)])));
    engine_->setOutputTarget(static_cast<OutputTarget>(static_cast<i32>(params_[static_cast<size_t>(ParamID::OutputTarget)])));
    engine_->setWidth(params_[static_cast<size_t>(ParamID::Width)]);
    engine_->setHeight(params_[static_cast<size_t>(ParamID::Height)]);
    engine_->setSurround(params_[static_cast<size_t>(ParamID::Surround)]);
    engine_->setObjectSize(params_[static_cast<size_t>(ParamID::ObjectSize)]);
    engine_->setHRTFStrength(params_[static_cast<size_t>(ParamID::HRTFStrength)]);
    engine_->setRoomSize(params_[static_cast<size_t>(ParamID::RoomSize)]);
    engine_->setReverbAmount(params_[static_cast<size_t>(ParamID::ReverbAmount)]);
    engine_->setEarlyReflectionLevel(params_[static_cast<size_t>(ParamID::EarlyReflectionLevel)]);
    engine_->setBass(params_[static_cast<size_t>(ParamID::Bass)]);
    engine_->setBrightness(params_[static_cast<size_t>(ParamID::Brightness)]);
    engine_->setDialogue(params_[static_cast<size_t>(ParamID::Dialogue)]);
    engine_->setCrossfeed(params_[static_cast<size_t>(ParamID::Crossfeed)]);
    engine_->setLoudnessTarget(params_[static_cast<size_t>(ParamID::TargetLUFS)]);
    engine_->setLimiterCeiling(params_[static_cast<size_t>(ParamID::LimiterCeiling)]);
    engine_->setProfile(static_cast<HeadphoneProfile>(static_cast<i32>(params_[static_cast<size_t>(ParamID::Profile)])));
    engine_->setLowLatency(params_[static_cast<size_t>(ParamID::LowLatency)] > 0.5f);
}

void AtmosAU::notifyPropertyListeners(AudioUnitPropertyID prop) {
    std::lock_guard<std::mutex> lock(listenerMutex_);
    for (auto& listener : propertyListeners_) {
        // Stub implementation - no-op on non-macOS
        (void)prop;
        (void)listener;
    }
}

OSStatus AtmosAU::AddPropertyListener(AudioUnitPropertyID prop, AudioUnitPropertyListenerProc proc, void* userData) {
    std::lock_guard<std::mutex> lock(listenerMutex_);
    propertyListeners_.emplace_back(proc, userData);
    return noErr;
}

OSStatus AtmosAU::RemovePropertyListener(AudioUnitPropertyID prop, AudioUnitPropertyListenerProc proc) {
    std::lock_guard<std::mutex> lock(listenerMutex_);
    propertyListeners_.erase(
        std::remove_if(propertyListeners_.begin(), propertyListeners_.end(),
            [proc](const auto& pair) { return pair.first == proc; }),
        propertyListeners_.end());
    return noErr;
}

OSStatus AtmosAU::RemovePropertyListenerWithUserData(AudioUnitPropertyID prop, AudioUnitPropertyListenerProc proc, void* userData) {
    std::lock_guard<std::mutex> lock(listenerMutex_);
    propertyListeners_.erase(
        std::remove_if(propertyListeners_.begin(), propertyListeners_.end(),
            [proc, userData](const auto& pair) { return pair.first == proc && pair.second == userData; }),
        propertyListeners_.end());
    return noErr;
}
