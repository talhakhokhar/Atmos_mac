#include "dsp/SpatialEngine.h"

namespace atmos {

SpatialEngine::SpatialEngine() = default;

void SpatialEngine::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    centerExtract_.setSampleRate(sr);
    surroundGen_.setSampleRate(sr);
    heightGen_.setSampleRate(sr);
    spectralAccent_.setSampleRate(sr);
    hrtf_.setSampleRate(sr);
    earlyRef_.setSampleRate(sr);
    fdnReverb_.setSampleRate(sr);
    bassMgr_.setSampleRate(sr);
    harmonicBass_.setSampleRate(sr);
    masterEQ_.setSampleRate(sr);
    dialogueEnh_.setSampleRate(sr);
    crossfeed_.setSampleRate(sr);
    loudnessNorm_.setSampleRate(sr);
    limiter_.setSampleRate(sr);
}

void SpatialEngine::setMode(SoundMode m) {
    mode_ = m;
    applyModeDefaults(m);
}

void SpatialEngine::setOutputTarget(OutputTarget target) {
    target_ = target;
    hrtf_.setOutputTarget(target);
    crossfeed_.setTarget(target);
}

void SpatialEngine::setWidth(f32 w) { width_ = w; }
void SpatialEngine::setHeight(f32 h) { height_ = h; }
void SpatialEngine::setSurround(f32 s) { surround_ = s; }
void SpatialEngine::setObjectSize(f32 sz) { objectSize_ = sz; }
void SpatialEngine::setHRTFStrength(f32 str) { hrtfStrength_ = str; }
void SpatialEngine::setRoomSize(f32 sz) { roomSize_ = sz; earlyRef_.setRoomSize(sz); fdnReverb_.setRoomSize(sz); }
void SpatialEngine::setReverbAmount(f32 amt) { reverbAmount_ = amt; }
void SpatialEngine::setEarlyReflectionLevel(f32 lvl) { earlyLevel_ = lvl; earlyRef_.setLevel(lvl); }
void SpatialEngine::setBass(f32 db) { bass_ = db; masterEQ_.setBass(db); }
void SpatialEngine::setBrightness(f32 db) { brightness_ = db; masterEQ_.setBrightness(db); }
void SpatialEngine::setDialogue(f32 db) { dialogue_ = db; dialogueEnh_.setAmount(db * 0.2f); }
void SpatialEngine::setCrossfeed(f32 cf) { crossfeedLevel_ = cf; crossfeed_.setLevel(cf); }
void SpatialEngine::setLoudnessTarget(f32 lufs) { loudnessTarget_ = lufs; loudnessNorm_.setTargetLUFS(lufs); }
void SpatialEngine::setLimiterCeiling(f32 db) { limiterCeiling_ = db; limiter_.setCeiling(db); }
void SpatialEngine::setProfile(HeadphoneProfile profile) { masterEQ_.setProfile(profile); }
void SpatialEngine::setLowLatency(bool ll) { lowLatency_ = ll; limiter_.setLookahead(ll ? 0.5f : 5.0f); }

void SpatialEngine::applyModeDefaults(SoundMode m) {
    switch (m) {
        case SoundMode::Off:
            width_ = 0.0f; height_ = 0.0f; surround_ = 0.0f; objectSize_ = 0.0f;
            reverbAmount_ = 0.0f; earlyLevel_ = 0.0f; bass_ = 0.0f;
            brightness_ = 0.0f; dialogue_ = 0.0f; crossfeedLevel_ = 0.0f;
            loudnessTarget_ = 0.0f; hrtfStrength_ = 0.0f;
            loudnessNorm_.setEnabled(false);
            break;
        case SoundMode::Reference:
            width_ = 0.2f; height_ = 0.0f; surround_ = 0.0f; objectSize_ = 0.0f;
            reverbAmount_ = 0.0f; earlyLevel_ = 0.0f; bass_ = 0.0f;
            brightness_ = 0.0f; dialogue_ = 0.0f; crossfeedLevel_ = 0.0f;
            loudnessTarget_ = -16.0f; hrtfStrength_ = 0.2f;
            loudnessNorm_.setEnabled(false);
            break;
        case SoundMode::Movie:
            width_ = 0.8f; height_ = 0.5f; surround_ = 0.45f; objectSize_ = 0.4f;
            reverbAmount_ = 0.35f; earlyLevel_ = 0.3f; bass_ = 2.5f;
            brightness_ = 1.0f; dialogue_ = 4.0f; crossfeedLevel_ = 0.5f;
            loudnessTarget_ = -16.0f; hrtfStrength_ = 1.0f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::Music:
            width_ = 0.55f; height_ = 0.15f; surround_ = 0.2f; objectSize_ = 0.3f;
            reverbAmount_ = 0.15f; earlyLevel_ = 0.15f; bass_ = 1.0f;
            brightness_ = 0.5f; dialogue_ = 0.0f; crossfeedLevel_ = 0.4f;
            loudnessTarget_ = -14.0f; hrtfStrength_ = 0.8f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::Game:
            width_ = 0.65f; height_ = 0.35f; surround_ = 0.5f; objectSize_ = 0.25f;
            reverbAmount_ = 0.15f; earlyLevel_ = 0.2f; bass_ = 1.5f;
            brightness_ = 1.5f; dialogue_ = 1.0f; crossfeedLevel_ = 0.6f;
            loudnessTarget_ = -18.0f; hrtfStrength_ = 0.9f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::Voice:
            width_ = 0.3f; height_ = 0.1f; surround_ = 0.1f; objectSize_ = 0.1f;
            reverbAmount_ = 0.1f; earlyLevel_ = 0.1f; bass_ = 0.0f;
            brightness_ = 0.0f; dialogue_ = 5.0f; crossfeedLevel_ = 0.3f;
            loudnessTarget_ = -16.0f; hrtfStrength_ = 0.6f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::Night:
            width_ = 0.5f; height_ = 0.2f; surround_ = 0.2f; objectSize_ = 0.2f;
            reverbAmount_ = 0.15f; earlyLevel_ = 0.15f; bass_ = -2.0f;
            brightness_ = -1.0f; dialogue_ = 2.0f; crossfeedLevel_ = 0.3f;
            loudnessTarget_ = -20.0f; hrtfStrength_ = 0.7f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::BassPlus:
            width_ = 0.6f; height_ = 0.3f; surround_ = 0.3f; objectSize_ = 0.3f;
            reverbAmount_ = 0.2f; earlyLevel_ = 0.2f; bass_ = 4.0f;
            brightness_ = 0.5f; dialogue_ = 2.0f; crossfeedLevel_ = 0.4f;
            loudnessTarget_ = -14.0f; hrtfStrength_ = 0.8f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::IEMSpatial:
            width_ = 0.7f; height_ = 0.4f; surround_ = 0.35f; objectSize_ = 0.35f;
            reverbAmount_ = 0.3f; earlyLevel_ = 0.25f; bass_ = 1.0f;
            brightness_ = -1.0f; dialogue_ = 3.0f; crossfeedLevel_ = 0.7f;
            loudnessTarget_ = -18.0f; hrtfStrength_ = 1.0f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::AuxLine:
            width_ = 0.4f; height_ = 0.1f; surround_ = 0.15f; objectSize_ = 0.2f;
            reverbAmount_ = 0.1f; earlyLevel_ = 0.1f; bass_ = 1.0f;
            brightness_ = 0.0f; dialogue_ = 2.0f; crossfeedLevel_ = 0.2f;
            loudnessTarget_ = -16.0f; hrtfStrength_ = 0.3f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::Car:
            width_ = 0.35f; height_ = 0.1f; surround_ = 0.15f; objectSize_ = 0.15f;
            reverbAmount_ = 0.08f; earlyLevel_ = 0.1f; bass_ = 1.5f;
            brightness_ = 0.0f; dialogue_ = 3.5f; crossfeedLevel_ = 0.3f;
            loudnessTarget_ = -16.0f; hrtfStrength_ = 0.2f;
            loudnessNorm_.setEnabled(true);
            break;
        case SoundMode::Auto:
        default:
            width_ = 0.7f; height_ = 0.35f; surround_ = 0.35f; objectSize_ = 0.35f;
            reverbAmount_ = 0.25f; earlyLevel_ = 0.2f; bass_ = 1.5f;
            brightness_ = 0.5f; dialogue_ = 2.5f; crossfeedLevel_ = 0.4f;
            loudnessTarget_ = -16.0f; hrtfStrength_ = 0.8f;
            loudnessNorm_.setEnabled(true);
            break;
    }
}

void SpatialEngine::reset() {
    centerExtract_.reset();
    surroundGen_.reset();
    heightGen_.reset();
    spectralAccent_.reset();
    hrtf_.reset();
    earlyRef_.reset();
    fdnReverb_.reset();
    bassMgr_.reset();
    harmonicBass_.reset();
    masterEQ_.reset();
    dialogueEnh_.reset();
    crossfeed_.reset();
    loudnessNorm_.reset();
    limiter_.reset();
}

void SpatialEngine::processBed(const f32* inL, const f32* inR, f32* outL, f32* outR, i32 numFrames) {
    // Allocate temporary buffers
    std::vector<f32> cBuf(numFrames);
    std::vector<f32> ldBuf(numFrames);
    std::vector<f32> rdBuf(numFrames);

    // Extract center
    centerExtract_.processBlock(inL, inR, cBuf.data(), ldBuf.data(), rdBuf.data(), numFrames);

    // Generate surrounds
    std::vector<f32> lsBuf(numFrames), rsBuf(numFrames);
    std::vector<f32> hfBuf(numFrames), hrBuf(numFrames);
    std::vector<f32> accL(numFrames), accR(numFrames);

    // Simple single-sample version for each frame
    for (i32 n = 0; n < numFrames; ++n) {
        f32 L = inL[n];
        f32 R = inR[n];
        f32 M = 0.5f * (L + R);
        f32 S = 0.5f * (L - R);

        f32 ls = 0.0f, rs = 0.0f;
        surroundGen_.processBlock(S, -S, ls, rs, 1, std::abs(cBuf[n]) > 0.01f);

        f32 hf = 0.0f, hr = 0.0f;
        heightGen_.processBlock(M, S, hf, hr, 1);

        outL[n] = 0.0f;
        outR[n] = 0.0f;

        // Render bed channels
        hrtf_.renderObject(ldBuf[n] * width_, -30.0f, 0.0f, 1.2f, objectSize_, outL[n], outR[n]);
        hrtf_.renderObject(rdBuf[n] * width_, 30.0f, 0.0f, 1.2f, objectSize_, outL[n], outR[n]);
        hrtf_.renderObject(cBuf[n], 0.0f, 0.0f, 1.0f, objectSize_ * 0.5f, outL[n], outR[n]);
        hrtf_.renderObject(ls * surround_, -100.0f, 0.0f, 2.0f, objectSize_, outL[n], outR[n]);
        hrtf_.renderObject(rs * surround_, 100.0f, 0.0f, 2.0f, objectSize_, outL[n], outR[n]);
        hrtf_.renderObject(hf * height_, -45.0f, 35.0f, 2.5f, objectSize_ * 0.8f, outL[n], outR[n]);
        hrtf_.renderObject(hr * height_, -135.0f, 30.0f, 3.0f, objectSize_ * 0.8f, outL[n], outR[n]);

        // Early reflections
        f32 earlyL = 0.0f, earlyR = 0.0f;
        earlyRef_.process(outL[n], outR[n], earlyL, earlyR);
        outL[n] += earlyL * earlyLevel_;
        outR[n] += earlyR * earlyLevel_;

        // Late reverb
        f32 wetL = 0.0f, wetR = 0.0f;
        fdnReverb_.process(outL[n] * 0.5f, outR[n] * 0.5f, wetL, wetR);
        outL[n] += wetL * reverbAmount_;
        outR[n] += wetR * reverbAmount_;

        // Bass management
        bassMgr_.process(outL[n], outR[n], outL[n], outR[n]);

        // Harmonic bass
        f32 bassL = harmonicBass_.process(outL[n]);
        f32 bassR = harmonicBass_.process(outR[n]);
        outL[n] += bassL;
        outR[n] += bassR;

        // Spectral accents
        f32 accL = 0.0f, accR = 0.0f;
        spectralAccent_.processBlock(&inL[n], &inR[n], &accL, &accR, 1);
        outL[n] += accL;
        outR[n] += accR;
    }
}

void SpatialEngine::process(const f32* inL, const f32* inR, f32* outL, f32* outR, i32 numFrames) {
    // Copy input to output initially
    for (i32 n = 0; n < numFrames; ++n) {
        outL[n] = inL[n];
        outR[n] = inR[n];
    }

    if (mode_ == SoundMode::Off) {
        limiter_.process(outL[0], outR[0]);
        return;
    }

    // Process spatial engine
    processBed(inL, inR, outL, outR, numFrames);

    // Master EQ
    for (i32 n = 0; n < numFrames; ++n) {
        masterEQ_.process(outL[n], outR[n]);
    }

    // Dialogue enhancement
    std::vector<f32> cBuf(numFrames);
    std::vector<f32> ldBuf(numFrames), rdBuf(numFrames);
    centerExtract_.processBlock(inL, inR, cBuf.data(), ldBuf.data(), rdBuf.data(), numFrames);
    for (i32 n = 0; n < numFrames; ++n) {
        dialogueEnh_.process(cBuf[n], outL[n], outR[n]);
    }

    // Crossfeed
    for (i32 n = 0; n < numFrames; ++n) {
        crossfeed_.process(outL[n], outR[n]);
    }

    // Loudness normalization
    loudnessNorm_.process(outL, outR, outL, outR, numFrames);

    // Limiter
    for (i32 n = 0; n < numFrames; ++n) {
        limiter_.process(outL[n], outR[n]);
    }
}

}
