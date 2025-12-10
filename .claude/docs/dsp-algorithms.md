# DSP Algorithms

## Fret Buzz Detection

**Algorithm**: Transient + Spectral Anomaly + Inharmonicity

1. **Onset Detection**: RMS energy + spectral flux
2. **Transient Analysis**: Attack time, zero-crossing rate
3. **Spectral Anomalies**: High-frequency energy (4-8 kHz)
4. **Inharmonicity**: Deviation from ideal harmonics
5. **Scoring**: `0.3*transient + 0.4*highFreqNoise + 0.3*inharmonicity`

## Intonation Analysis

**Algorithm**: YIN Pitch Tracking + State Machine

1. **State Machine**: Idle → OpenString → WaitFor12thFret → FrettedString → Complete
2. **Pitch Tracking**: YIN algorithm with 2048 sample window
3. **Averaging**: Median filter + 500ms stable accumulation
4. **Deviation**: `cents = 1200 * log2(measured / expected)`
5. **Tolerance**: ±5 cents

## String Health Analysis

**Algorithm**: Harmonic Decay + Spectral Brightness + Inharmonicity

1. **Harmonic Extraction**: Track f₀, 2f₀, ..., 10f₀
2. **Decay Analysis**: Exponential fit, measure dB/s
3. **Spectral Features**: Centroid, rolloff, HF energy
4. **Inharmonicity**: `β = Σ|f'_n - n*f₀| / (n*f₀)`
5. **Health Score**: `0.3*decay + 0.3*spectral + 0.4*inharmonicity`

## References

- [YIN algorithm](http://audition.ens.fr/adc/pdf/2002_JASA_YIN.pdf) - Pitch detection paper
- [lib-guitar-dsp](https://github.com/Konstantysz/lib-guitar-dsp) - DSP algorithms
- [precision-guitar-tuner](https://github.com/Konstantysz/precision-guitar-tuner) - Reference implementation
