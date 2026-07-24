#include "MasterEQ.h"

namespace atmos {

MasterEQ::MasterEQ() {
    filters_[0].setType(BiquadType::LowShelf);
    filters_[1].setType(BiquadType::HighShelf);
    filters_[2].setType(BiquadType::Peaking);
    filters_[3].setType(BiquadType::Peaking);
    filters_[4].setType(BiquadType::HighShelf);
    filters_[5].setType(BiquadType::Peaking);
    filters_[6].setType(BiquadType::LowShelf);
    filters_[7].setType(BiquadType::HighShelf);
}

void MasterEQ::setSampleRate(f32 sr) {
    sampleRate_ = sr;
    for (auto& f : filters_) f.setSampleRate(sr);
    updateFilters();
}

void MasterEQ::setBass(f32 db) { bass_ = db; updateFilters(); }
void MasterEQ::setBrightness(f32 db) { brightness_ = db; updateFilters(); }
void MasterEQ::setWarmth(f32 db) { warmth_ = db; updateFilters(); }
void MasterEQ::setPresence(f32 db) { presence_ = db; updateFilters(); }
void MasterEQ::setAir(f32 db) { air_ = db; updateFilters(); }

void MasterEQ::setProfile(HeadphoneProfile profile) {
    switch (profile) {
        case HeadphoneProfile::Flat:
            bass_ = 0; brightness_ = 0; warmth_ = 0; presence_ = 0; air_ = 0;
            break;
        case HeadphoneProfile::Premium:
            bass_ = 3.0f; brightness_ = 1.0f; warmth_ = -1.0f; presence_ = 2.0f; air_ = 1.5f;
            break;
        case HeadphoneProfile::Warm:
            bass_ = 4.0f; brightness_ = -2.0f; warmth_ = 3.0f; presence_ = 0; air_ = -1.0f;
            break;
        case HeadphoneProfile::Bright:
            bass_ = 1.0f; brightness_ = 3.0f; warmth_ = -2.0f; presence_ = 2.0f; air_ = 3.0f;
            break;
        case HeadphoneProfile::ClosedBack:
            bass_ = 2.0f; brightness_ = 0; warmth_ = -1.0f; presence_ = 1.0f; air_ = 0;
            break;
        case HeadphoneProfile::OpenBack:
            bass_ = 1.0f; brightness_ = 0; warmth_ = 0; presence_ = 0; air_ = 1.0f;
            break;
        case HeadphoneProfile::Earbuds:
            bass_ = 3.0f; brightness_ = -1.0f; warmth_ = 1.0f; presence_ = 1.0f; air_ = 0;
            break;
        case HeadphoneProfile::IEMBalanced:
            bass_ = 2.0f; brightness_ = -1.0f; warmth_ = 0; presence_ = 0; air_ = -1.0f;
            break;
        case HeadphoneProfile::IEMSafe:
            bass_ = 1.0f; brightness_ = -3.0f; warmth_ = 1.0f; presence_ = 0; air_ = -2.0f;
            break;
    }
    updateFilters();
}

void MasterEQ::updateFilters() {
    filters_[0].setFrequency(100.0f);
    filters_[0].setGainDb(bass_);
    filters_[0].setQ(0.7f);

    filters_[1].setFrequency(10000.0f);
    filters_[1].setGainDb(air_);
    filters_[1].setQ(0.7f);

    filters_[2].setFrequency(250.0f);
    filters_[2].setGainDb(warmth_);
    filters_[2].setQ(1.0f);

    filters_[3].setFrequency(2500.0f);
    filters_[3].setGainDb(presence_);
    filters_[3].setQ(1.5f);

    filters_[4].setFrequency(8000.0f);
    filters_[4].setGainDb(brightness_);
    filters_[4].setQ(0.8f);

    filters_[5].setFrequency(500.0f);
    filters_[5].setGainDb(warmth_ * 0.5f);
    filters_[5].setQ(1.2f);

    filters_[6].setFrequency(80.0f);
    filters_[6].setGainDb(bass_ * 0.3f);
    filters_[6].setQ(0.5f);

    filters_[7].setFrequency(12000.0f);
    filters_[7].setGainDb(air_ * 0.5f);
    filters_[7].setQ(0.6f);
}

void MasterEQ::process(f32& inL, f32& inR) {
    for (auto& f : filters_) {
        inL = f.process(inL);
        inR = f.process(inR);
    }
}

void MasterEQ::reset() {
    for (auto& f : filters_) f.reset();
}

}
