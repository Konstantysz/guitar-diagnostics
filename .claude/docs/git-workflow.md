# Git Workflow

## Commit Guidelines

**Single-line commit messages only** (imperative mood):

```bash
# ✅ CORRECT
git commit -m "Add FretBuzzDetector with onset detection"
git commit -m "Fix buffer overflow in ring buffer write"
git commit -m "Implement IntonationAnalyzer state machine"
git commit -m "Refactor AnalysisEngine thread management"

# ❌ WRONG
git commit -m "Added FretBuzzDetector"           # Not imperative
git commit -m "I fixed the bug"                  # Not imperative
git commit -m "Add feature\n\nLong description"  # Multi-line
```

## References

- [kappa-core](https://github.com/Konstantysz/kappa-core) - Application framework
- [lib-guitar-io](https://github.com/Konstantysz/lib-guitar-io) - Audio I/O wrapper
- [lib-guitar-dsp](https://github.com/Konstantysz/lib-guitar-dsp) - DSP algorithms
- [precision-guitar-tuner](https://github.com/Konstantysz/precision-guitar-tuner) - Reference implementation
- [RtAudio](https://www.music.mcgill.ca/~gary/rtaudio/) - Cross-platform audio I/O

## Contact

For questions or contributions, please open an issue on GitHub.
