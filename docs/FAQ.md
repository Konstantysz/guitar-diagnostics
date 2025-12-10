# Frequently Asked Questions

## General

**Q: Is the Guitar Diagnostic Analyzer free?**
A: Yes, this is an open-source project. You can download and use it freely.

**Q: What platforms are supported?**
A: Currently, **Windows** is the primary supported platform for v1.0.0.

- **macOS** and **Linux** support is available if you build from source, with official installers planned for v1.1.0.

**Q: Can I use this for bass guitar?**
A: Yes! The frequency detection algorithms cover the bass range (Low E is ~41 Hz). However, fret buzz detection thresholds might need mental adjustment as bass strings oscillate more.

## Hardware

**Q: Do I need a special audio interface?**
A: You need a way to get your guitar signal into the computer. A dedicated USB Audio Interface (Focusrite, PreSonus, Behringer) is highly recommended for low latency and clear signal. A simple "Rocksmith" USB cable can also work.

**Q: Can I use my computer microphone?**
A: Technically yes, but it is **not recommended**. Acoustic coupling introduces room noise and reverb that confuses the precision analyzers. Always plug in directly for diagnostic work.

## Technical Terms

**Q: What is a "Cent"?**
A: A cent is a unit of pitch. There are 100 cents in a semitone. For intonation, we want the error to be close to 0 cents.

**Q: What is "Inharmonicity"?**
A: Ideally, a string's headers (overtones) are perfect multiples of the fundamental frequency. "Inharmonicity" measures how much they deviate. Old or stiff strings have higher inharmonicity, making them sound "out of tune" even when the open string is tuned perfectly.

**Q: What is a "Transient"?**
A: The transient is the initial, loud burst of sound when you pick the string. The Fret Buzz detector analyzes the sound *after* this transient to separate pick noise from fret buzz.

## Analyzers

**Q: My "String Health" is 60%. Should I change strings?**
A: It depends. For recording, yes. For practice, they are still fine. We recommend changing when the score drops below 50% or if you notice tuning instability.

**Q: The Fret Buzz detector says "Severe Buzz" but I don't hear it.**
A: The analyzer is very sensitive and detects high-frequency rattling that might be masked by the amp tone. If it doesn't bother you through your amp, you might not needs to fix it. However, for a perfect setup, aim for "Clean".
