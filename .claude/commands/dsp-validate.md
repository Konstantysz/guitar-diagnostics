For the specified DSP code (FFT, YIN pitch detection, harmonic analysis):

1. Check mathematical correctness against algorithm papers
2. Verify sample rate conversions (samples ↔ Hz ↔ bins)
3. Check window function application
4. Validate frequency-to-bin and bin-to-frequency formulas
5. Ensure Nyquist frequency constraints
6. Review for numerical stability (division by zero, log(0))

Reference lib-guitar-dsp documentation if needed.
