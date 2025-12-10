# Sub-Agent Library for Guitar-Diagnostics

This directory contains specialized sub-agent definitions optimized for C++20 audio DSP development.

## Available Agents

| Agent | Purpose | Priority | When to Use |
|-------|---------|----------|-------------|
| [realtime-safety-auditor](realtime-safety-auditor.md) | Audit real-time thread safety | 🔴 Critical | Before merging audio callback code |
| [dsp-algorithm-validator](dsp-algorithm-validator.md) | Verify DSP mathematical correctness | 🔴 Critical | After DSP implementation |
| [thread-safety-analyzer](thread-safety-analyzer.md) | Detect data races, memory ordering issues | 🔴 Critical | When implementing cross-thread communication |
| [performance-profiler](performance-profiler.md) | Identify bottlenecks | 🟡 High | During optimization cycles |
| [test-coverage-analyzer](test-coverage-analyzer.md) | Find missing test coverage | 🟡 High | Before releases |

## Quick Usage Guide

### Launching a Single Agent

```
Launch a Real-Time Safety Auditor agent to audit src/App/AudioProcessingLayer.cpp

Check the OnAudioInput() callback for:
- Allocations (new, malloc, push_back)
- Mutexes (std::mutex, lock_guard)
- I/O operations (cout, spdlog)

Report violations with file:line and suggested fixes.
```

### Launching Multiple Agents in Parallel

```
Launch 3 agents in parallel to audit AudioProcessingLayer:

Agent 1: Real-Time Safety Auditor
- Audit src/App/AudioProcessingLayer.cpp

Agent 2: Thread Safety Analyzer
- Verify ring buffer write is lock-free SPSC

Agent 3: Performance Profiler
- Profile for allocation hotspots

After all complete, synthesize into go/no-go decision.
```

### Sequential Agents with Quality Gates

```
Step 1: Launch Performance Profiler
- Find top 3 bottlenecks in AnalysisEngine

[Review results]

Step 2: Launch DSP Algorithm Validator
- Verify optimization won't break correctness

[Review results]

Step 3: Implement optimizations with TDD
```

## Agent Descriptions

### 1. Real-Time Safety Auditor

**Purpose**: Comprehensive audit for real-time audio thread violations

**Checks**:
- ✅ FORBIDDEN: Allocations (new, malloc, push_back)
- ✅ FORBIDDEN: Mutexes (std::mutex, lock_guard)
- ✅ FORBIDDEN: I/O (cout, spdlog, file operations)
- ✅ FORBIDDEN: Blocking system calls
- ✅ Dangerous: Unbounded loops, recursion, virtual calls

**Output**: Severity-ranked violations with fixes

**Example**:
```
VIOLATION: [CRITICAL]
File: src/App/AudioProcessingLayer.cpp:78
Code: std::vector<float> temp(size);
Issue: Allocation in audio callback
Fix: Pre-allocate in constructor:
  class AudioProcessingLayer {
      std::vector<float> temp;
  public:
      AudioProcessingLayer(size_t size) : temp(size) {}
  };
```

---

### 2. DSP Algorithm Validator

**Purpose**: Verify mathematical correctness of DSP implementations

**Checks**:
- ✅ Formulas match reference papers
- ✅ Sample-rate conversions correct (Hz ↔ samples ↔ bins)
- ✅ Nyquist frequency constraints
- ✅ Numerical stability (divide-by-zero, overflow)
- ✅ Window function application
- ✅ Edge case handling (silence, DC, full-scale)

**Algorithms Supported**:
- YIN pitch detection
- FFT-based analysis
- Autocorrelation
- Harmonic tracking
- Spectral analysis

**Output**: Validation report with test cases

**Example**:
```
ISSUE: Incorrect bin-to-frequency conversion
File: src/Analysis/FretBuzzDetector.cpp:145
Current: freq = bin * (fftSize / sampleRate)  // WRONG!
Correct: freq = bin * (sampleRate / fftSize)

Test case suggestion:
  bin=100, sampleRate=48000, fftSize=2048
  Expected: 100 * (48000/2048) = 2343.75 Hz
  Actual: 100 * (2048/48000) = 4.27 Hz  ❌
```

---

### 3. Thread Safety Analyzer

**Purpose**: Deep analysis of multi-threaded interactions

**Checks**:
- ✅ Thread boundary mapping
- ✅ Shared data inventory
- ✅ Data race detection
- ✅ Memory ordering correctness (acquire/release)
- ✅ Lock-free guarantees (SPSC verification)
- ✅ Atomic operations analysis

**Output**: Thread interaction diagram + race reports

**Example**:
```
DATA RACE DETECTED: [CRITICAL]
Variable: m_latestPitch (float)
Writers: Worker thread (AnalysisEngine::Run)
Readers: UI thread (DiagnosticVisualizationLayer::Update)
Synchronization: NONE ❌

Fix: Use atomic or atomic shared_ptr:
  std::atomic<float> m_latestPitch;
  // Writer (worker thread)
  m_latestPitch.store(pitch, std::memory_order_release);
  // Reader (UI thread)
  float pitch = m_latestPitch.load(std::memory_order_acquire);
```

---

### 4. Performance Profiler

**Purpose**: Static analysis for performance bottlenecks

**Checks**:
- ✅ Hidden allocations
- ✅ Unnecessary copies (pass-by-value)
- ✅ Cache-unfriendly patterns
- ✅ Loop inefficiencies (invariants not hoisted)
- ✅ Branch mispredictions
- ✅ Virtual function overhead
- ✅ Expensive math operations (pow, exp, sin)

**Output**: Ranked optimizations with speedup estimates

**Example**:
```
BOTTLENECK: [HIGH IMPACT]
Location: src/Analysis/AnalysisEngine.cpp:89-102
Issue: Loop-invariant calculation inside loop

Current (SLOW):
  for (int i = 0; i < size; ++i) {
      float gain = CalculateGain();  // Calculated size times!
      output[i] = input[i] * gain;
  }

Optimized (FAST):
  float gain = CalculateGain();  // Calculate once
  for (int i = 0; i < size; ++i) {
      output[i] = input[i] * gain;
  }

Expected speedup: 5-10x
Difficulty: Easy (1-line change)
Risk: Low
```

---

### 5. Test Coverage Analyzer

**Purpose**: Identify missing test coverage

**Checks**:
- ✅ Untested functions
- ✅ Untested error paths
- ✅ Missing edge cases
- ✅ Untested state transitions
- ✅ Missing boundary tests
- ✅ Integration test gaps

**Output**: Prioritized test case suggestions

**Example**:
```
MISSING TEST: [HIGH PRIORITY]
Component: IntonationAnalyzer
Method: ProcessFrettedNote()
Issue: No test for invalid state transition

Suggested test:
TEST_F(IntonationAnalyzerTest, ProcessFrettedNote_CalledInIdleState_ThrowsException)
{
    // Arrange: Analyzer starts in Idle state
    EXPECT_EQ(analyzer->GetState(), State::Idle);

    // Act & Assert: Calling ProcessFrettedNote in Idle should throw
    EXPECT_THROW(
        analyzer->ProcessFrettedNote(signal),
        std::runtime_error
    );
}

Rationale: Prevents incorrect usage, avoids undefined behavior
Risk if untested: User could crash app by calling methods out of order
```

## Workflow Examples

### Pre-Merge Checklist (Parallel Agents)

Before merging changes to AudioProcessingLayer:

```
Launch 3 agents in parallel:

1. Real-Time Safety Auditor: Audit src/App/AudioProcessingLayer.cpp
2. Thread Safety Analyzer: Verify ring buffer interactions
3. Test Coverage Analyzer: Check tests/App/TestAudioProcessingLayer.cpp

Synthesize: PASS = merge, FAIL = block, WARNINGS = review required
```

### New Feature Implementation (Sequential with Gates)

When adding new analyzer:

```
Phase 1: Launch Test Coverage Analyzer
- Analyze similar analyzer (e.g., FretBuzzDetector)
- Learn test patterns for this project

[Review output, create test plan]

Phase 2: TDD Implementation
- Write tests first
- Implement analyzer

Phase 3: Launch DSP Algorithm Validator
- Verify mathematical correctness

[Fix any issues]

Phase 4: Launch Performance Profiler
- Ensure <2% CPU target met

Phase 5: Launch Real-Time Safety Auditor
- Verify integration with audio pipeline

[All pass → Merge]
```

### Performance Investigation (Sequential, Each Informs Next)

When CPU usage exceeds 10% target:

```
Step 1: Launch Performance Profiler
- Identify top 3 bottlenecks

[Review: Bottleneck is in FFT computation]

Step 2: Launch DSP Algorithm Validator
- Verify FFT implementation correctness
- Ensure optimization won't break algorithm

[Review: Algorithm correct, can optimize]

Step 3: Implement optimization with TDD
- Write performance test
- Optimize FFT size or algorithm

Step 4: Launch Test Coverage Analyzer
- Ensure optimization didn't reduce coverage

[All pass → Deploy]
```

### Bug Investigation (Parallel Discovery + Sequential Fix)

When audio glitches reported:

```
Discovery Phase (Parallel):
  Agent 1: Real-Time Safety Auditor → Check for RT violations
  Agent 2: Thread Safety Analyzer → Check for data races
  Agent 3: Performance Profiler → Check for CPU spikes

[Agents identify: Allocation in audio callback]

Fix Phase (Sequential):
  1. Remove allocation (TDD)
  2. Launch Real-Time Safety Auditor → Verify fix
  3. Launch Test Coverage Analyzer → Add regression test
  4. Merge
```

## Agent Invocation Best Practices

### ✅ Good Invocation (Specific)

```
Launch DSP Algorithm Validator for src/Analysis/Intonation/IntonationAnalyzer.cpp

Validate YIN pitch detection in DetectPitch() method (lines 89-145):
1. Verify difference function matches YIN paper Eq. 6
2. Check CMNDF calculation (Eq. 8)
3. Verify pitch = sampleRate / tau (not tau / sampleRate!)
4. Test edge cases: 0Hz, Nyquist, silence

Reference: http://audition.ens.fr/adc/pdf/2002_JASA_YIN.pdf

Report any formula discrepancies with suggested fixes.
```

### ❌ Bad Invocation (Vague)

```
Check the code for problems
```

### Key Elements of Good Invocation

1. **Agent name**: Which agent to use
2. **Target file(s)**: Specific paths
3. **Focus area**: Which functions/lines
4. **Specific checks**: What to verify
5. **Context**: Why this matters, what changed
6. **Expected output**: What format you want

## Integration with Project Workflow

### With Slash Commands

Slash commands = quick checks, Agents = deep analysis

```bash
# Quick RT audit
/rt-audit src/App/AudioProcessingLayer.cpp

# If issues found, deep dive
"Launch Real-Time Safety Auditor agent for comprehensive analysis..."
```

### With CI/CD (Future)

```yaml
# .github/workflows/pre-merge.yml
- name: Run Agent Audits
  run: |
    claude-agent run realtime-safety-auditor src/App/
    claude-agent run thread-safety-analyzer src/Analysis/
    claude-agent run test-coverage-analyzer tests/
```

### With Code Review

Reviewers can request agent analysis:

```
PR Comment: "@claude Launch Real-Time Safety Auditor for changes in this PR"
```

## Creating Custom Agents

Add new agent by creating `[name].md` in this directory:

```markdown
# [Agent Name]

[Brief description]

## Instructions

You are a [role] expert specializing in [domain].

### Analysis Checklist

[What to check]

### Reporting Format

[Expected output structure]

### Example Invocation

[How to use this agent]
```

## Tips for Effective Agent Usage

1. **Be specific**: Provide file paths, line numbers, function names
2. **Provide context**: What changed? Why are you investigating?
3. **Set constraints**: Time limits, focus areas, priority
4. **Request actionable output**: File:line references, code examples, diffs
5. **Use parallel agents**: For independent audits
6. **Use sequential agents**: When one informs the next
7. **Synthesize results**: Combine findings into decisions

## Performance Targets Reference

Quick reference for agents:

- **CPU**: <10% on i5/i7
- **Memory**: <100MB
- **Latency**: <20ms total (input → display)
- **Audio Buffer**: 512 samples @ 48kHz (10.7ms)
- **Real-Time Thread**: Zero allocations, zero mutexes, zero I/O
- **Sample Rate**: 48000 Hz (Nyquist = 24000 Hz)
- **Test Coverage**: >80% target, >90% excellent

## Related Documentation

- [QUICK-REFERENCE.md](../QUICK-REFERENCE.md) - Slash commands, skills, templates
- [recommended-subagents.md](../docs/recommended-subagents.md) - Detailed guide to all agents
- [architecture.md](../docs/architecture.md) - System architecture
- [real-time-constraints.md](../docs/real-time-constraints.md) - RT safety rules

---

**Next Steps**:
1. Try launching Real-Time Safety Auditor on AudioProcessingLayer
2. Experiment with parallel agent workflows
3. Create custom agents for project-specific needs

**Feedback**: If an agent produces unclear output or misses issues, update its .md file with better instructions!
