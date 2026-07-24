#include "dsp/DelayLine.h"

namespace atmos {

DelayLine::DelayLine() : buffer_(1), maxDelay_(1), writePos_(0), readPos_(0) {}

DelayLine::DelayLine(i32 maxDelay) {
    setMaxDelay(maxDelay);
}

void DelayLine::setMaxDelay(i32 maxDelay) {
    maxDelay_ = std::max(1, maxDelay);
    buffer_.assign(maxDelay_, 0.0f);
    writePos_ = 0;
    readPos_ = 0;
}

void DelayLine::reset() {
    std::fill(buffer_.begin(), buffer_.end(), 0.0f);
    writePos_ = 0;
    readPos_ = 0;
}

void DelayLine::write(f32 x) {
    buffer_[writePos_] = x;
    writePos_ = (writePos_ + 1) % maxDelay_;
}

f32 DelayLine::read(f32 delaySamples) const {
    i32 delay = static_cast<i32>(delaySamples);
    delay = clamp(delay, 0, maxDelay_ - 1);
    i32 pos = writePos_ - delay;
    if (pos < 0) pos += maxDelay_;
    return buffer_[pos];
}

f32 DelayLine::readLinear(f32 delaySamples) const {
    i32 delayInt = static_cast<i32>(delaySamples);
    f32 frac = delaySamples - delayInt;
    delayInt = clamp(delayInt, 0, maxDelay_ - 1);
    i32 pos1 = writePos_ - delayInt;
    if (pos1 < 0) pos1 += maxDelay_;
    i32 pos2 = writePos_ - delayInt - 1;
    if (pos2 < 0) pos2 += maxDelay_;
    return lerp(buffer_[pos1], buffer_[pos2], frac);
}

f32 DelayLine::readHermite(f32 delaySamples) const {
    i32 delayInt = static_cast<i32>(delaySamples);
    f32 frac = delaySamples - delayInt;
    delayInt = clamp(delayInt, 0, maxDelay_ - 1);
    i32 pos0 = writePos_ - delayInt;
    if (pos0 < 0) pos0 += maxDelay_;
    i32 pos1 = writePos_ - delayInt - 1;
    if (pos1 < 0) pos1 += maxDelay_;
    i32 pos2 = writePos_ - delayInt - 2;
    if (pos2 < 0) pos2 += maxDelay_;
    i32 pos3 = writePos_ - delayInt - 3;
    if (pos3 < 0) pos3 += maxDelay_;

    f32 p = (buffer_[pos1] - buffer_[pos2]) - (buffer_[pos0] - buffer_[pos3]); (void)p;
    f32 t = frac;
    f32 t2 = t * t;
    f32 t3 = t2 * t;
    return 0.5f * ((2.0f * buffer_[pos1]) + (-buffer_[pos0] + buffer_[pos2]) * t +
                   (2.0f * buffer_[pos0] - 5.0f * buffer_[pos1] + 4.0f * buffer_[pos2] - buffer_[pos3]) * t2 +
                   (-buffer_[pos0] + 3.0f * buffer_[pos1] - 3.0f * buffer_[pos2] + buffer_[pos3]) * t3);
}

FractionalDelay::FractionalDelay() : delay_(2048) {}

void FractionalDelay::setDelay(f32 samples) {
    delaySamples_ = clamp(samples, 0.0f, static_cast<f32>(delay_.getMaxDelay() - 1));
}

f32 FractionalDelay::process(f32 x) {
    frac_ = std::fmod(delaySamples_, 1.0f);
    f32 out = delay_.readHermite(delaySamples_);
    delay_.write(x);
    return out;
}

void FractionalDelay::reset() {
    delay_.reset();
    frac_ = 0.0f;
    x1_ = x2_ = x3_ = y1_ = y2_ = y3_ = 0.0f;
}

AllpassDecorrelator::AllpassDecorrelator() : delay_(4096) {}

void AllpassDecorrelator::setDelay(f32 samples) {
    delaySamples_ = clamp(samples, 1.0f, static_cast<f32>(delay_.getMaxDelay() - 1));
}

void AllpassDecorrelator::setGain(f32 gain) {
    gain_ = clamp(gain, -0.95f, 0.95f);
}

f32 AllpassDecorrelator::process(f32 x) {
    f32 delayed = delay_.readLinear(delaySamples_);
    f32 out = delayed - gain_ * x;
    f32 toWrite = x + gain_ * delayed;
    delay_.write(toWrite);
    return out;
}

void AllpassDecorrelator::reset() {
    delay_.reset();
    history_ = 0.0f;
}

}
