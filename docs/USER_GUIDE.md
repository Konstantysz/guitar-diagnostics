# Guitar Diagnostic Analyzer - User Guide

Welcome to the **Guitar Diagnostic Analyzer**! This professional tool helps you assess the condition of your guitar's setup, including fret buzz, intonation, and string health, using advanced audio analysis.

## Table of Contents

1. [Installation](#installation)
2. [Getting Started](#getting-started)
3. [Audio Setup](#audio-setup)
4. [Using the Modules](#using-the-modules)
   - [Fret Buzz Detector](#fret-buzz-detector)
   - [Intonation Analyzer](#intonation-analyzer)
   - [String Health Analyzer](#string-health-analyzer)
5. [Interpreting Results](#interpreting-results)

---

## Installation

### Windows

1. Download the latest installer (`GuitarDiagnostics-Setup-v1.0.0.exe`) from the [Releases page](https://github.com/Konstantysz/guitar-diagnostics/releases).
2. Run the installer and follow the on-screen instructions.
3. Launch **Guitar Diagnostic Analyzer** from the Start Menu or Desktop shortcut.

### macOS & Linux

*Support for macOS and Linux installers is coming in v1.1.0. Currently, you will need to build from source.*

---

## Getting Started

When you launch the application, you will be presented with the main interface containing four tabs:

1. **Audio Monitor**: For setting up your signal.
2. **Fret Buzz**: For detecting fret noise.
3. **Intonation**: For checking pitch accuracy up the neck.
4. **String Health**: For analyzing string brilliance and decay.

**First Step**: Connect your electric guitar to your computer using a USB Audio Interface (e.g., Focusrite Scarlett, Behringer U-Phoria) or a standard 1/4" to USB cable.

---

## Audio Setup

Before analyzing, ensure your audio signal is clean and detected.

1. Navigate to the **Audio Monitor** tab.
2. Select your **Input Device** (your audio interface) from the dropdown menu.
3. Select your **Output Device** (usually your speakers or headphones) if you want to monitor the signal.
4. Play your guitar. You should see the waveform react in the real-time display.
5. Adjust the **Gain** knob on your audio interface so the signal is strong but not "clipping" (hitting the top/bottom edges of the graph).

> **Tip**: For best results, use the bridge pickup of your guitar and turn the tone knob to maximum.

---

## Using the Modules

### Fret Buzz Detector

This tool identifies unwanted buzzing noises caused by strings hitting frets due to low action or uneven fretwork.

1. Select the **Fret Buzz** tab.
2. Choose the string you are testing (e.g., "Low E").
3. Pluck the string firmly.
4. The analyzer will detect the initial attack (transient) and listen for high-frequency buzzing that follows.
5. **Results**:
   - **Buzz Score**: A value from 0.0 to 1.0. Lower is better.
   - **Status**: Will indicate "Clean", "Slight Buzz", or "Severe Buzz".

### Intonation Analyzer

This tool checks if your guitar plays in tune across the entire neck.

1. Select the **Intonation** tab.
2. Tune your open string perfectly using the built-in tuner display.
3. Play the harmonic at the 12th fret (lightly touch the string over the fret wire and pluck).
4. Play the fretted note at the 12th fret (press down normally).
5. The analyzer compares the pitch difference.
6. **Interpretation**:
   - **Sharp (> 0 cents)**: The saddle needs to be moved **backward** (away from the neck).
   - **Flat (< 0 cents)**: The saddle needs to be moved **forward** (towards the neck).

### String Health Analyzer

Strings lose their high-frequency content (brightness) and sustain as they age. This tool quantifies that loss.

1. Select the **String Health** tab.
2. Pluck an open string cleanly and let it ring out.
3. The system measures:
   - **Spectral Brightness**: Presence of high harmonics.
   - **Decay Rate**: How fast the sound dies out.
   - **Inharmonicity**: How "wobbly" the pitch is.
4. **Health Score**: A percentage (0-100%).
   - **90-100%**: Fresh strings.
   - **50-89%**: Usable.
   - **< 50%**: Consider changing strings.

---

## Interpreting Results

| Metric | Good Range | Action Required |
| :--- | :--- | :--- |
| **Buzz Score** | < 0.2 | If > 0.5, raise action or check fret leveling. |
| **Intonation** | +/- 3 cents | Adjust bridge saddles to center the needle. |
| **String Health** | > 80% | If < 50%, replace strings for better tone and tuning stability. |
