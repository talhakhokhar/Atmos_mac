# Atmos Spatial Audio for macOS

A production-grade stereo Audio Unit (AUv2) plug-in that creates an immersive, Atmos-inspired spatial audio experience from stereo input. Designed for SoundSource and other AU hosts on macOS.

## Features

- **Stereo-in / Stereo-out** immersive virtualizer
- **7.1.4-like virtual bed** from stereo
- **Parametric HRTF** binaural rendering
- **Early reflections + FDN late reverb** room model
- **Bass management** with crossover and harmonic bass enhancement
- **Crossfeed** for headphone naturalness
- **Loudness normalization** (EBU R128 / ITU-R BS.1770)
- **True-peak limiting** with safety ceiling
- **Content-adaptive Auto mode**
- **12 factory presets** for headphones, IEMs, line out, and car
- **M1 Pro optimized** with no allocations in audio thread

## Build

```bash
mkdir build && cd build
cmake .. -G Xcode
cmake --build . --config Release
```

Or with Makefiles:
```bash
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## Install

Copy the built `.component` bundle to:
```bash
/Users/<username>/Library/Audio/Plug-Ins/Components/
```

Then restart your AU host (SoundSource, Logic, etc.).

## Usage

- Load as an **Audio Unit Effect** in SoundSource
- Select **Mode** (Movie, Music, Game, Voice, etc.)
- Select **Output Target** (Headphones, IEM, Line Aux, Car, Reference)
- Adjust **Width**, **Height**, **Surround** as desired
- Enable **Loudness Normalize** for consistent levels
- Use **Profile** to match your headphones/IEMs

## Output Targets

| Target | Behavior |
|---|---|
| Headphones | Full binaural HRTF + crossfeed |
| IEM | Strong externalization, gentler treble, safe limiting |
| Line / Aux | Mono-safe widening, no full HRTF |
| Car Aux | Vocal clarity, mono-safe, bass control |
| Reference | Flat, minimal processing, safety only |

## Sound Modes

| Mode | Purpose |
|---|---|
| Off | Bypass |
| Reference | Flat, accurate |
| Auto | Content-adaptive |
| Movie | Atmos-like immersive |
| Music | Natural stage |
| Game | Localization + low latency |
| Voice | Dialogue clarity |
| Night | Reduced dynamics |
| Bass+ | Bass enhancement |
| IEM Spatial | Externalization for IEMs |
| Aux / Line | Mono-safe line output |
| Car | Loudness + vocal clarity |

## Architecture

The plug-in implements a **bed-first, object-accent-second** architecture:

1. **Source Decomposition** — M/S, center extraction, surround/height synthesis
2. **Virtual Bed** — 7.1.4-like bed from stereo
3. **HRTF Renderer** — ITD, ILD, pinna cues, distance attenuation
4. **Room Model** — 6-tap early reflections + 8-line FDN
5. **Bass Management** — Linkwitz-Riley crossover + virtual LFE
6. **Master Chain** — EQ, dialogue enhance, crossfeed, loudness, limiter

## M1 Pro Optimization

- 32-bit float processing
- Preallocated buffers
- No allocations in audio thread
- SIMD-friendly layout
- Parameter smoothing per block
- Low-latency mode (< 8ms total)

## Disclaimer

This plug-in creates an immersive spatial audio experience inspired by object-based audio concepts. It is not Dolby Atmos and is not affiliated with Dolby Laboratories.

## License

Copyright 2024. All rights reserved.
