# DelayWithEQ

Delay plugin built with **JUCE** that applies a 3-band EQ **only to the delayed (wet) signal**.

## Features

- Classic delay with:
  - Delay Time (1 – 2000 ms)
  - Feedback (0 – 95 %)
  - Mix (Dry/Wet)
- **EQ applied only on the delayed signal**:
  - Low Shelf (Freq + Gain)
  - Peak / Bell (Freq + Gain + Q)
  - High Shelf (Freq + Gain)
- Clean separation of Dry and Wet paths
- Parameter smoothing to avoid clicks
- Stereo support

## How the signal flows

```
Input
  │
  ├─── Dry ──────────────────────────────┐
  │                                      │
  └─── Wet → DelayLine → Feedback        │
                │                        │
                └──→ 3-Band EQ ──────────┼──→ Mix → Output
```

## Building

### Requirements
- CMake ≥ 3.22
- C++17 compiler
- JUCE (recommended as git submodule)

### Steps

1. Clone this repository:
   ```bash
   git clone https://github.com/YOUR_USERNAME/DelayWithEQ.git
   cd DelayWithEQ
   ```

2. Add JUCE as submodule (recommended):
   ```bash
   git submodule add https://github.com/juce-framework/JUCE.git JUCE
   git submodule update --init --recursive
   ```

3. Configure and build:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```

The plugin will be generated in the `build` folder (VST3 / AU / Standalone depending on the platform).

## Project structure

```
DelayWithEQ/
├── CMakeLists.txt
├── README.md
├── Source/
│   ├── PluginProcessor.h
│   ├── PluginProcessor.cpp
│   ├── PluginEditor.h
│   └── PluginEditor.cpp
└── JUCE/          ← (add as git submodule)
```

## Parameters

| Parameter       | Range          | Default | Description                    |
|-----------------|----------------|---------|--------------------------------|
| Delay Time      | 1 – 2000 ms    | 350 ms  | Delay time                     |
| Feedback        | 0 – 0.95       | 0.40    | Feedback amount                |
| Mix             | 0 – 1          | 0.35    | Dry/Wet mix                    |
| Low Shelf Freq  | 30 – 500 Hz    | 120 Hz  | Low shelf frequency            |
| Low Shelf Gain  | -12 – +12 dB   | 0 dB    | Low shelf gain                 |
| Peak Freq       | 200 – 5000 Hz  | 1000 Hz | Peak frequency                 |
| Peak Gain       | -12 – +12 dB   | 0 dB    | Peak gain                      |
| Peak Q          | 0.1 – 10       | 0.707   | Peak resonance                 |
| High Shelf Freq | 2k – 16k Hz    | 8 kHz   | High shelf frequency           |
| High Shelf Gain | -12 – +12 dB   | 0 dB    | High shelf gain                |

## License

Feel free to use and modify this code for personal or commercial projects.
