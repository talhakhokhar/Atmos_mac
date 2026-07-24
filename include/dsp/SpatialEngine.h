#pragma once
#include "../common/Types.h"
#include "../dsp/CenterExtractor.h"
#include "../dsp/SurroundGen.h"
#include "../dsp/HeightGen.h"
#include "../dsp/SpectralAccent.h"
#include "../dsp/HRTFRenderer.h"
#include "../dsp/EarlyReflections.h"
#include "../dsp/FDNReverb.h"
#include "../dsp/BassManager.h"
#include "../dsp/HarmonicBass.h"
#include "../dsp/MasterEQ.h"
#include "../dsp/DialogueEnhancer.h"
#include "../dsp/Crossfeed.h"
#include "../dsp/LoudnessNormalizer.h"
#include "../dsp/TruePeakLimiter.h"
#include <memory>

namespace atmos {

class SpatialEngine {
public:
    SpatialEngine();
    void setSampleRate(f32 sr);
    void setMode(SoundMode mode);
    void setOutputTarget(OutputTarget target);
    void setWidth(f32 w);
    void setHeight(f32 h);
    void setSurround(f32 s);
    void setObjectSize(f32 sz);
    void setHRTFStrength(f32 str);
    void setRoomSize(f32 sz);
    void setReverbAmount(f32 amt);
    void setEarlyReflectionLevel(f32 lvl);
    void setBass(f32 db);
    void setBrightness(f32 db);
    void setDialogue(f32 db);
    void setCrossfeed(f32 cf);
    void setLoudnessTarget(f32 lufs);
    void setLimiterCeiling(f32 db);
    void setProfile(HeadphoneProfile profile);
    void setLowLatency(bool ll);
    void process(const f32* inL, const f32* inR, f32* outL, f32* outR, i32 numFrames);
    void reset();

private:
    f32 sampleRate_ = 48000.0f;
    SoundMode mode_ = SoundMode::Movie;
    OutputTarget target_ = OutputTarget::Headphones;

    // Source decomposition
    CenterExtractor centerExtract_;
    SurroundGen surroundGen_;
    HeightGen heightGen_;
    SpectralAccent spectralAccent_;

    // Spatial rendering
    HRTFRenderer hrtf_;

    // Room
    EarlyReflections earlyRef_;
    FDNReverb fdnReverb_;

    // Bass
    BassManager bassMgr_;
    HarmonicBass harmonicBass_;

    // Master chain
    MasterEQ masterEQ_;
    DialogueEnhancer dialogueEnh_;
    Crossfeed crossfeed_;
    LoudnessNormalizer loudnessNorm_;
    TruePeakLimiter limiter_;

    // Parameters
    f32 width_ = 0.75f;
    f32 height_ = 0.45f;
    f32 surround_ = 0.4f;
    f32 objectSize_ = 0.35f;
    f32 hrtfStrength_ = 1.0f;
    f32 roomSize_ = 0.5f;
    f32 reverbAmount_ = 0.3f;
    f32 earlyLevel_ = 0.25f;
    f32 bass_ = 2.0f;
    f32 brightness_ = 1.0f;
    f32 dialogue_ = 3.0f;
    f32 crossfeedLevel_ = 0.5f;
    f32 loudnessTarget_ = -16.0f;
    f32 limiterCeiling_ = -1.0f;
    bool lowLatency_ = false;
    
    // Pre-allocated buffers for processBed to avoid per-block allocations
    std::vector<f32> cBuf_;
    std::vector<f32> ldBuf_;
    std::vector<f32> rdBuf_;
    std::vector<f32> lsBuf_;
    std::vector<f32> rsBuf_;
    std::vector<f32> hfBuf_;
    std::vector<f32> hrBuf_;
    std::vector<f32> accL_;
    std::vector<f32> accR_;

    void applyModeDefaults(SoundMode m);
    void processBed(const f32* inL, const f32* inR, f32* outL, f32* outR, i32 numFrames);
};

}
