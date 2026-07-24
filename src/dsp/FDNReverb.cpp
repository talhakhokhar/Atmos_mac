#include "dsp/FDNReverb.h"

namespace atmos {

FDNReverb::FDNReverb() {
    delayLengths_ = {1409, 1783, 2003, 2269, 2591, 2939, 3259, 3517};
    for (int i = 0; i < 8; ++i) {
        delays_[i].setMaxDelay(delayLengths_[i] + 64);
        dampingFilters_[i].setType(BiquadType::LowPass);
        dampingFilters_[i].setFrequency(8000.0f);
        dampingFilters_[i].setQ(0.7f);
        feedbackGains_[i] = 0.6f;
        outputGainsL_[i] = 0.125f;
        outputGainsR_[i] = 0.125f;
    }
    inputHp_.setType(BiquadType::HighPass);
    inputHp_.setFrequency(200.0f);
    outputLp_.setType(BiquadType::LowPass);
    outputLp_.setFrequency(10000.0f);
}

void FDNReverb::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    for (int i = 0; i < 8; ++i) {
        delays_[i].setMaxDelay(delayLengths_[i] + 64);
        dampingFilters_[i].setSampleRate(sr);
    }
    inputHp_.setSampleRate(sr);
    outputLp_.setSampleRate(sr);
}

void FDNReverb::setRoomSize(f32 size) {
    roomSize_ = size;
    f32 rt60Scale = 0.5f + size * 0.5f;
    for (int i = 0; i < 8; ++i) {
        feedbackGains_[i] = std::pow(0.001f, rt60Scale * static_cast<f32>(delayLengths_[i]) / (sampleRate_ * 3.0f));
    }
}

void FDNReverb::setDecay(f32 decay) {
    decay_ = decay;
    for (int i = 0; i < 8; ++i) {
        feedbackGains_[i] = std::pow(0.001f, decay * static_cast<f32>(delayLengths_[i]) / (sampleRate_ * 3.0f));
    }
}

void FDNReverb::setModulation(f32 modRate, f32 modDepth) {
    modRate_ = modRate;
    modDepth_ = modDepth;
}

void FDNReverb::process(f32 inL, f32 inR, f32& outL, f32& outR) {
    f32 input = (inL + inR) * 0.5f;
    input = inputHp_.process(input);

    // Modulate delay lengths
    modPhase_ += modRate_ / sampleRate_;
    if (modPhase_ > kTau) modPhase_ -= kTau;
    std::array<f32, 8> modOffsets;
    for (int i = 0; i < 8; ++i) {
        modOffsets[i] = std::sin(modPhase_ + i * 0.8f) * modDepth_;
    }

    std::array<f32, 8> v;
    for (int i = 0; i < 8; ++i) {
        v[i] = delays_[i].readLinear(delayLengths_[i] + modOffsets[i]);
    }

    // Householder feedback matrix
    f32 sum = 0.0f;
    for (int i = 0; i < 8; ++i) sum += v[i];
    f32 avg = sum * 0.125f;

    std::array<f32, 8> y;
    for (int i = 0; i < 8; ++i) {
        y[i] = v[i] + input * 0.1f - avg + (v[i] - avg) * decay_;
        y[i] = dampingFilters_[i].process(y[i]);
        y[i] *= feedbackGains_[i];
        delays_[i].write(y[i]);
    }

    outL = 0.0f;
    outR = 0.0f;
    for (int i = 0; i < 8; ++i) {
        outL += y[i] * outputGainsL_[i];
        outR += y[i] * outputGainsR_[i];
    }

    outL = outputLp_.process(outL);
    outR = outputLp_.process(outR);
}

void FDNReverb::reset() {
    for (int i = 0; i < 8; ++i) {
        delays_[i].reset();
        dampingFilters_[i].reset();
    }
    inputHp_.reset();
    outputLp_.reset();
    modPhase_ = 0.0f;
}

}
