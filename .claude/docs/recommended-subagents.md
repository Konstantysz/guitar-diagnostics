# Recommended Sub-Agents for Guitar-Diagnostics

This document outlines specialized sub-agents that would be valuable for the guitar-diagnostics C++20 audio DSP project.

## Overview: When to Use Sub-Agents

Sub-agents are specialized autonomous agents that can handle complex, multi-step tasks independently. Use them when:

- ✅ Task requires multiple exploration steps (searching, reading, analyzing)
- ✅ Task has independent work that can be parallelized
- ✅ You want focused analysis without cluttering main conversation context
- ✅ Task needs specialized expertise (DSP, threading, performance)

## Project-Specific Sub-Agents

### 1. Real-Time Safety Auditor

**Purpose**: Deep audit of code for real-time audio thread safety violations

**When to Use**:
- Before merging audio callback changes
- When debugging audio glitches/dropouts
- During code review of AudioProcessingLayer or similar

**Example Usage**:
```
"Launch a Real-Time Safety Auditor agent to audit src/App/AudioProcessingLayer.cpp

Specifically check for:
1. Any allocations (new, delete, malloc, vector::push_back, string operations)
2. Mutex usage (std::mutex, lock_guard, unique_lock)
3. I/O operations (cout, spdlog, file operations)
4. Unbounded loops or recursion
5. System calls that might block

For each violation:
- Report file:line location
- Explain why it violates RT constraints
- Suggest lock-free alternative

Also verify:
- Pre-allocation strategy
- Ring buffer usage is lock-free
- Atomic operations are memory_order correct"
```

**Value**: Catches subtle RT violations that manual review might miss

---

### 2. DSP Algorithm Validator

**Purpose**: Verify mathematical correctness of DSP implementations

**When to Use**:
- After implementing pitch detection, FFT analysis, harmonic tracking
- When results seem inaccurate (wrong pitch, incorrect Hz values)
- Before publishing or production deployment

**Example Usage**:
```
"Launch a DSP Algorithm Validator agent to validate src/Analysis/Intonation/IntonationAnalyzer.cpp

Verify YIN pitch detection implementation:
1. Check autocorrelation calculation against YIN paper formula
2. Verify difference function computation
3. Validate CMNDF (cumulative mean normalized difference)
4. Check parabolic interpolation for sub-bin accuracy
5. Verify sample-rate-to-Hz conversions (are constants correct for 48kHz?)
6. Check boundary conditions (min/max frequency constraints)
7. Validate against known test cases (440Hz sine wave should return 440Hz ±1 cent)

Reference: http://audition.ens.fr/adc/pdf/2002_JASA_YIN.pdf
Compare implementation against lib-guitar-dsp if available"
```

**Value**: Mathematical correctness is critical for DSP; subtle errors compound

---

### 3. Thread Safety Analyzer

**Purpose**: Deep analysis of multi-threaded interactions and data races

**When to Use**:
- When implementing cross-thread communication
- Debugging data races or corrupted data
- Verifying lock-free data structures

**Example Usage**:
```
"Launch a Thread Safety Analyzer agent to analyze AnalysisEngine thread interactions

Focus on:
1. Map all thread boundaries:
   - Real-time audio thread (AudioProcessingLayer)
   - Worker thread (AnalysisEngine)
   - UI thread (DiagnosticVisualizationLayer)

2. For each shared data structure:
   - LockFreeRingBuffer: Verify SPSC guarantees
   - Atomic shared_ptr for results: Check memory ordering
   - Any other shared state

3. Check for:
   - Data races (use ThreadSanitizer patterns)
   - ABA problems in lock-free code
   - Memory ordering issues (sequential consistency assumptions)
   - Lifetime issues (use-after-free across threads)

4. Verify synchronization:
   - Ring buffer read/write coordination
   - Result publication atomicity
   - Startup/shutdown sequence safety

Generate thread interaction diagram showing all sync points"
```

**Value**: Thread bugs are notoriously hard to debug; proactive analysis prevents them

---

### 4. Performance Profiler Agent

**Purpose**: Identify performance bottlenecks and optimization opportunities

**When to Use**:
- When CPU usage exceeds 10% target
- When latency exceeds 20ms target
- Before optimizing (measure first!)

**Example Usage**:
```
"Launch a Performance Profiler Agent to analyze src/Analysis/FretBuzzDetector.cpp

Profile the Analyze() method:
1. Identify hot loops (most time spent)
2. Check for:
   - Hidden allocations (vector resize, string concat)
   - Cache misses (poor data locality)
   - Unnecessary copies (large objects passed by value)
   - Branch mispredictions (unpredictable conditionals)
   - Virtual function calls in tight loops
   - Division operations (expensive on some CPUs)

3. Analyze FFT performance:
   - Is FFT size optimal? (power of 2?)
   - Are input/output buffers aligned?
   - Is PFFFT being used correctly?

4. Suggest optimizations ranked by:
   - Expected speedup
   - Implementation complexity
   - Risk of introducing bugs

Target: <2% CPU for this component"
```

**Value**: Data-driven optimization; avoid premature optimization

---

### 5. Architecture Documenter

**Purpose**: Generate comprehensive architecture documentation

**When to Use**:
- After completing a major feature
- When onboarding new developers
- For technical design reviews

**Example Usage**:
```
"Launch an Architecture Documenter agent to document the AnalysisEngine subsystem

Generate documentation including:
1. ASCII component diagram showing:
   - AnalysisEngine
   - Three analyzers (FretBuzz, Intonation, StringHealth)
   - Data flow from ring buffer to results

2. Thread model:
   - Which thread runs AnalysisEngine::Run()
   - How it reads from ring buffer (blocking/non-blocking?)
   - How it publishes results (atomic swap?)

3. Lifecycle:
   - Initialization sequence
   - Steady-state operation
   - Shutdown sequence

4. API documentation:
   - Public methods with purpose
   - Configuration parameters
   - Expected usage patterns

5. Design decisions:
   - Why separate worker thread?
   - Why not process in audio callback?
   - Why atomic shared_ptr for results?

Output: Markdown document ready for .claude/docs/"
```

**Value**: Living documentation that stays synchronized with code

---

### 6. Test Coverage Analyzer

**Purpose**: Identify missing test coverage and suggest test cases

**When to Use**:
- Before marking feature complete
- During code review
- When bugs slip through testing

**Example Usage**:
```
"Launch a Test Coverage Analyzer agent for IntonationAnalyzer

Analyze src/Analysis/Intonation/IntonationAnalyzer.cpp and tests/Analysis/TestIntonationAnalyzer.cpp

1. Map all code paths in IntonationAnalyzer:
   - State machine transitions (Idle → OpenString → ... → Complete)
   - Error handling paths
   - Edge cases

2. Check test coverage:
   - Which state transitions are tested?
   - Are error cases tested?
   - Are edge cases tested?

3. Identify missing tests:
   - Untested state transitions
   - Invalid input handling
   - Boundary conditions (0Hz, Nyquist frequency)
   - Concurrent state changes

4. Suggest specific test cases:
   - Use AAA (Arrange, Act, Assert) pattern
   - Use Google Test framework
   - Prioritize by risk (high-risk paths first)

Generate: List of test cases to add, in priority order"
```

**Value**: Ensures robust test coverage before bugs reach production

---

### 7. Refactoring Advisor

**Purpose**: Safe refactoring guidance with impact analysis

**When to Use**:
- Planning code restructuring
- Paying down technical debt
- Improving code maintainability

**Example Usage**:
```
"Launch a Refactoring Advisor agent for src/Analysis/AnalysisEngine.cpp

Current issue: AnalysisEngine::Run() is 150 lines (exceeds 100-line limit)

Task:
1. Analyze function structure:
   - Identify logical sections
   - Find repeated patterns
   - Spot extract-method opportunities

2. Propose refactoring:
   - How to split into smaller functions?
   - Suggest names for extracted functions
   - Show before/after structure

3. Impact analysis:
   - What tests need updating?
   - Are there coupling concerns?
   - What's the risk level?

4. Migration plan:
   - Step-by-step refactoring sequence
   - How to ensure no behavior change?
   - What to test after each step?

Constraints:
- Maintain coding standards (Allman braces, initializer lists)
- Keep real-time safety guarantees
- All tests must pass after refactoring"
```

**Value**: Reduces refactoring risk through systematic analysis

---

### 8. Dependency Analyzer

**Purpose**: Analyze dependencies and suggest improvements

**When to Use**:
- Adding new third-party libraries
- Investigating build issues
- Reducing coupling

**Example Usage**:
```
"Launch a Dependency Analyzer agent for the project

Analyze dependencies:
1. Map dependency tree:
   - kappa-core dependencies
   - lib-guitar-io dependencies
   - lib-guitar-dsp dependencies
   - System dependencies (OpenGL, GLFW, etc.)

2. Check for issues:
   - Circular dependencies
   - Unnecessary transitive dependencies
   - Version conflicts
   - License incompatibilities

3. Analyze coupling:
   - Which components depend on which?
   - Are there tight coupling issues?
   - Should we use dependency injection?

4. Suggest improvements:
   - Dependencies to remove/replace
   - Interfaces to introduce
   - Inversion of control opportunities

Focus on: Keeping audio processing decoupled from UI"
```

**Value**: Maintains clean architecture, prevents dependency hell

---

## Parallel Sub-Agent Workflows

For maximum efficiency, launch multiple agents in parallel for independent tasks.

### Workflow 1: Pre-Merge Safety Check

Launch 3 agents in parallel before merging:

```
"Launch 3 agents in parallel to audit AudioProcessingLayer changes:

Agent 1: Real-Time Safety Auditor
- Audit src/App/AudioProcessingLayer.cpp for RT violations

Agent 2: Thread Safety Analyzer
- Verify ring buffer interactions are race-free

Agent 3: Test Coverage Analyzer
- Check tests/App/TestAudioProcessingLayer.cpp coverage

After all complete, synthesize findings into go/no-go decision"
```

### Workflow 2: New Feature Implementation

Launch 2 agents in sequence with quality gate:

```
"Phase 1: Launch Architecture Documenter agent
- Document current AnalysisEngine architecture
- Identify extension points for new analyzer

[Wait for completion, review output]

Phase 2: Launch Test Coverage Analyzer agent
- Based on architecture, suggest test cases for new analyzer
- Ensure existing tests won't break

Then: Implement with test-driven development"
```

### Workflow 3: Performance Investigation

Launch agents sequentially, each informing the next:

```
"Step 1: Launch Performance Profiler Agent
- Identify top 3 bottlenecks in AnalysisEngine

[Review results]

Step 2: Launch DSP Algorithm Validator
- For top bottleneck, verify algorithm correctness
- Ensure optimization won't break correctness

[Review results]

Step 3: Launch Refactoring Advisor
- Plan optimization refactoring
- Ensure maintainability preserved"
```

## Sub-Agent Best Practices

### 1. Provide Specific Context

❌ **Vague**: "Check the code for issues"

✅ **Specific**:
```
"Launch Real-Time Safety Auditor for src/App/AudioProcessingLayer.cpp

Focus on OnAudioInput() callback (lines 45-120)
Recently changed: Added envelope detection
Concern: Might have introduced allocations"
```

### 2. Define Success Criteria

Always specify what the agent should produce:

```
"Expected output:
1. List of violations with file:line references
2. Severity rating (Critical/High/Medium/Low)
3. Suggested fixes for each violation
4. Overall pass/fail decision"
```

### 3. Set Boundaries

Guide the agent's scope:

```
"Constraints:
- Focus on src/Analysis/ directory only
- Ignore test code (separate analysis)
- Prioritize real-time thread issues
- Maximum 30 minute investigation"
```

### 4. Request Actionable Results

```
"Deliverable format:
- Markdown report with findings
- Code snippets showing violations
- Suggested fixes as diff/patch
- Priority-ranked action items"
```

## Integration with Existing Workflow

### With Slash Commands

Slash commands trigger quick checks; sub-agents do deep analysis:

```
# Quick check
/rt-audit src/App/AudioProcessingLayer.cpp

# If issues found, deep dive
"Launch Real-Time Safety Auditor agent for comprehensive analysis..."
```

### With Skills

Skills handle straightforward tasks; sub-agents handle complex ones:

```
# Skill handles simple documentation
[Invoke architecture-explainer skill]

# Sub-agent handles comprehensive audit
"Launch Architecture Documenter agent for full subsystem documentation..."
```

### With Templates

Use templates to structure sub-agent prompts:

```
# Start with comparative analysis template
[Use comparative-analysis.md template]

# Launch agent for each option
"Launch 3 Refactoring Advisor agents in parallel, one for each approach..."
```

## When NOT to Use Sub-Agents

❌ **Don't use for**:
- Simple one-file reads (use Read tool)
- Quick grep searches (use Grep tool)
- Single-step tasks (use slash commands)
- Tasks needing user input mid-way (use main conversation)

✅ **DO use for**:
- Multi-file analysis requiring context building
- Complex investigations with multiple steps
- Independent parallel tasks
- Specialized deep dives (DSP math, thread safety, performance)

## Creating Custom Sub-Agents

You can create project-specific sub-agents in `.claude/agents/`:

```markdown
---
name: harmonic-analyzer
description: Analyze harmonic content in audio processing code
allowed-tools: Read,Grep,mcp__serena
---

# Harmonic Analysis Agent

## Purpose
Verify harmonic tracking implementations for correctness

## Workflow
1. Find fundamental frequency detection code
2. Trace harmonic calculation (2f0, 3f0, ..., 10f0)
3. Verify bin-to-frequency conversions
4. Check for aliasing above Nyquist
5. Validate against test signals
```

## Summary: Recommended Priorities

**High Priority** (Use frequently):
1. Real-Time Safety Auditor - Critical for audio quality
2. DSP Algorithm Validator - Critical for accuracy
3. Thread Safety Analyzer - Prevents nasty bugs

**Medium Priority** (Use periodically):
4. Performance Profiler Agent - For optimization cycles
5. Test Coverage Analyzer - Before major releases
6. Architecture Documenter - After major features

**Low Priority** (Use as needed):
7. Refactoring Advisor - When paying down tech debt
8. Dependency Analyzer - During architecture reviews

---

**Next Steps**:
1. Try launching a Real-Time Safety Auditor for AudioProcessingLayer
2. Experiment with parallel agent workflows
3. Create custom agents for recurring project-specific tasks

Would you like me to demonstrate launching one of these agents?
