# Guitar Diagnostic Analyzer - Development Guide

## Project Overview

**Guitar Diagnostic Analyzer** is a professional C++20 application for real-time guitar diagnostics using advanced DSP algorithms. It provides three core analysis modules: Fret Buzz Detection, String Intonation Analysis, and String Health Assessment.

### Key Features

- **Real-time Audio Processing**: Low-latency audio input via ASIO/CoreAudio/ALSA
- **Three Diagnostic Modules**:
  - Fret Buzz Detector (transient + spectral analysis)
  - Intonation Analyzer (YIN pitch tracking + state machine)
  - String Health Analyzer (harmonic decay + spectral brightness)
- **Tab-Based UI**: Four panels (Fret Buzz, Intonation, String Health, Audio Monitor)
- **Thread-Safe Architecture**: Separate real-time audio, analysis worker, and UI threads

## Current Phase

**Phase 4 Complete** - All UI panels implemented and functional

**Focus**: Refinement, optimization, and bug fixes

## Quick Reference

**For quick command/agent/template lookup**: @.claude/QUICK-REFERENCE.md

## Documentation Structure

Detailed documentation is organized into focused modules:

@.claude/docs/architecture.md
@.claude/docs/cpp-coding-standards.md
@.claude/docs/real-time-constraints.md
@.claude/docs/dsp-algorithms.md
@.claude/docs/build-and-test.md
@.claude/docs/git-workflow.md

## Available Tools

**Slash Commands**: See `.claude/commands/` - Quick checks and scaffolding

- `/add-analyzer` - Scaffold new analyzer module
- `/rt-audit` - Check real-time safety
- `/review-commit` - Pre-commit checklist
- `/dsp-validate` - Verify DSP correctness
- `/perf-profile` - Performance analysis

**Sub-Agents**: See `.claude/agents/README.md` - Deep autonomous analysis

- **realtime-safety-auditor** - Audit RT thread violations (Critical)
- **dsp-algorithm-validator** - Verify DSP math (Critical)
- **thread-safety-analyzer** - Detect data races (Critical)
- **performance-profiler** - Find bottlenecks (High priority)
- **test-coverage-analyzer** - Find test gaps (High priority)

**Skills**: See `.claude/skills/` - Specialized workflows

- **architecture-explainer** - Generate architecture docs with diagrams

**Templates**: See `.claude/templates/` - Prompt patterns

- **comparative-analysis.md** - Structured decision-making (6 templates)
- **role-play-scenarios.md** - Deep analysis via debate (8 scenarios)
