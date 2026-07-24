#pragma once
#include "../common/Types.h"
#include "../dsp/SpatialEngine.h"
#include "../au/ParameterDefs.h"
#include "../au/PresetManager.h"

#ifdef __APPLE__
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioComponent.h>
#else
// Stub types for non-macOS builds (testing only)
typedef int32_t OSStatus;
typedef uint32_t UInt32;
typedef uint32_t AudioUnitPropertyID;
typedef uint32_t AudioUnitScope;
typedef uint32_t AudioUnitElement;
typedef uint32_t Boolean;
typedef float AudioUnitParameterValue;
typedef void* AudioTimeStamp;
typedef void* AudioBufferList;
typedef void* AudioUnitRenderActionFlags;
typedef void* AudioStreamBasicDescription;
typedef void* AudioComponentPlugInInterface;
typedef void* AudioComponentDescription;
typedef void* AudioUnitPropertyListenerProc;
#endif

#include <iostream>
#include <vector>
#include <memory>
#include <mutex>

namespace atmos {

class AtmosAU {
public:
    AtmosAU();
    ~AtmosAU();

    OSStatus Initialize();
    OSStatus Uninitialize();
    
    OSStatus GetPropertyInfo(AudioUnitPropertyID propID, AudioUnitScope scope, AudioUnitElement element,
                            UInt32& outDataSize, Boolean& outWritable);
    OSStatus GetProperty(AudioUnitPropertyID propID, AudioUnitScope scope, AudioUnitElement element,
                        UInt32& ioDataList, void* outData);
    OSStatus SetProperty(AudioUnitPropertyID propID, AudioUnitScope scope, AudioUnitElement element,
                        UInt32 inDataSize, const void* inData);
    OSStatus GetParameter(AudioUnitParameterID paramID, AudioUnitScope scope, AudioUnitElement element,
                         AudioUnitParameterValue& outValue);
    OSStatus SetParameter(AudioUnitParameterID paramID, AudioUnitScope scope, AudioUnitElement element,
                         AudioUnitParameterValue inValue, UInt32 inBufferOffsetInFrames);
    OSStatus Render(AudioUnitRenderActionFlags* ioActionFlags, const AudioTimeStamp* inTimeStamp,
                   UInt32 nFrames, AudioBufferList* ioData);

    void setParameterValue(ParamID id, f32 value);
    f32 getParameterValue(ParamID id) const;
    void loadPreset(const PresetData& preset);

    OSStatus AddPropertyListener(AudioUnitPropertyID prop, AudioUnitPropertyListenerProc proc, void* userData);
    OSStatus RemovePropertyListener(AudioUnitPropertyID prop, AudioUnitPropertyListenerProc proc);
    OSStatus RemovePropertyListenerWithUserData(AudioUnitPropertyID prop, AudioUnitPropertyListenerProc proc, void* userData);

private:
    std::unique_ptr<SpatialEngine> engine_;
    std::array<f32, static_cast<size_t>(ParamID::NumParams)> params_;
    std::array<ParamSmooth, static_cast<size_t>(ParamID::NumParams)> paramSmoothers_;
    bool initialized_ = false;
    f32 sampleRate_ = 48000.0f;
    
    UInt32 inputBusCount_ = 1;
    UInt32 outputBusCount_ = 1;
    AudioStreamBasicDescription inputFormat_ = {};
    AudioStreamBasicDescription outputFormat_ = {};

    void syncEngineFromParams();

    std::mutex listenerMutex_;
    std::vector<std::pair<AudioUnitPropertyListenerProc, void*>> propertyListeners_;
    void notifyPropertyListeners(AudioUnitPropertyID prop);
};

} // namespace atmos

extern "C" {
    AudioComponentPlugInInterface* AtmosAUFactory(const AudioComponentDescription* inDesc);
}
