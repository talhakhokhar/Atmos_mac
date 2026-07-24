#include "VST3Plug.h"
#include "../dsp/SpatialEngine.h"
#include <audioeffect.h>
#include <iaudioeffect.h>
#include <ivstaudiobuffer.h>
#include <vsttypes.h>
#include <cstring>

using namespace atmos;

class VST3Plugin : public IAudioEffect, public IComponent {
public:
    VST3Plugin() : mEngine() {}

    // IAudioEffect methods
    virtual FUnknown* queryInterface(REFIID inIID) final {
        static const REFIID kIAudioEffectID = IID_IAudioEffect;
        static const REFIID kIComponentID = IID_IComponent;
        
        FUnknown** ppInterface = nullptr;
        if (inIID == kIAudioEffectID) return reinterpret_cast<FUnknown*>(this);
        if (inIID == kIComponentID) return reinterpret_cast<FUnknown*>(this);

        return nullptr;
    }

    virtual uint32 addRef() final { return ++mRefCount;
    } virtual uint32 release() final {
        if (--mRefCount == 0) {
            delete this;
        }
        return mRefCount;
    }

    virtual int32 getAttributeCount(const char* attributes) final {
        return 1;
    }

    virtual int32 getAttribute(const char* attrName, void* value) final {
        if (strcmp(attrName, "VSTAudio processing") == 0) {
            *(int32_t*)value = mNumChannels;
        }
        return kErrNotFound;
    }

    // IComponent methods
    virtual int32 initialize(FComponent* newComponent) final {
        mNumChannels = 2;
        mEngine.setSampleRate(mSampleRate);
        return 0;
    }
    virtual int32 termComponent() final { mEngine.reset(); return 0; }

    virtual int32 processEvent(FComponent, int eventID, IEvent* data) final {
        (void) eventID; (void) data; return 0;
    }
    virtual int32 getComponentInfo(int infoID, void* info) final {
        (void) infoID; (void) info; return 0;
    }

    // Processing methods
    virtual int32 audioProcess(int32 section, AudioSampleBuffer& input, AudioSampleBuffer& output, int32 sampleCount) final {
        if (!mEngine.isInitialized()) return 0;

        for (int ch = 0; ch < mNumChannels; ch++) {
            float* in = input.getManySamples(ch, sampleCount);
            float* out = output.getManySamples(ch, sampleCount);
            mEngine.processSingle(input.getElements(ch), output.getElements(ch), sampleCount);
        }
        return sampleCount;
    }

private:
    SpatialEngine mEngine;
    int32_t mNumChannels = 0;
    int32_t mRefCount = 0;
    double mSampleRate = 48000.0;
};

// VST3 registration
extern "C" IAudioEffect* createEffect() { return new VST3Plugin(); }