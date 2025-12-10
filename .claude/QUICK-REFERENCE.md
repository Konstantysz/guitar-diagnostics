# Claude Code Quick Reference for Guitar-Diagnostics

Quick access to commands, templates, and best practices.

## Slash Commands

| Command | Purpose | When to Use |
|---------|---------|-------------|
| `/add-analyzer` | Scaffold new analyzer module | Adding new diagnostic feature |
| `/rt-audit` | Check real-time safety | Before committing audio callback code |
| `/review-commit` | Pre-commit checklist | Before every commit |
| `/dsp-validate` | Verify DSP algorithm correctness | After DSP implementation |
| `/perf-profile` | Performance analysis | Investigating slowdowns |

### Quick Usage

```bash
# Example: Audit AudioProcessingLayer for real-time violations
/rt-audit src/App/AudioProcessingLayer.cpp

# Example: Review before committing FretBuzzDetector changes
/review-commit
```

## Sub-Agents

Specialized agents for deep, autonomous analysis. See [.claude/agents/README.md](.claude/agents/README.md) for details.

| Agent | Purpose | Priority | When to Use |
|-------|---------|----------|-------------|
| **realtime-safety-auditor** | Audit RT thread safety | 🔴 Critical | Before merging audio callback code |
| **dsp-algorithm-validator** | Verify DSP math correctness | 🔴 Critical | After DSP implementation |
| **thread-safety-analyzer** | Detect data races | 🔴 Critical | Cross-thread communication |
| **performance-profiler** | Find bottlenecks | 🟡 High | Optimization cycles |
| **test-coverage-analyzer** | Find missing tests | 🟡 High | Before releases |

### Quick Launch Example

```
Launch a Real-Time Safety Auditor agent to audit src/App/AudioProcessingLayer.cpp

Check OnAudioInput() callback for:
- Allocations (new, malloc, vector::push_back)
- Mutexes (std::mutex, lock_guard)
- I/O operations (cout, spdlog)

Report violations with file:line and fixes.
```

### Parallel Agents Example

```
Launch 3 agents in parallel for pre-merge audit:

Agent 1: Real-Time Safety Auditor → src/App/AudioProcessingLayer.cpp
Agent 2: Thread Safety Analyzer → Verify ring buffer SPSC
Agent 3: Test Coverage Analyzer → tests/App/TestAudioProcessingLayer.cpp

Synthesize: PASS/FAIL decision
```

## Agent Skills

### User Skills (~/.claude/skills/)

**cpp20-refactor**: Modernize C++ code to C++20 patterns
- Trigger: "refactor this to C++20", "modernize this code"
- Preserves coding standards, verifies real-time safety

**test-generator**: Generate Google Test files
- Trigger: "write tests for", "need test coverage"
- Follows TDD workflow, AAA pattern

### Project Skills (.claude/skills/)

**architecture-explainer**: Document architecture with diagrams
- Trigger: "how does X work", "explain the architecture"
- Generates ASCII diagrams, explains thread model
- Located: `.claude/skills/architecture-explainer/`

## Prompt Templates

### Comparative Analysis (.claude/templates/comparative-analysis.md)

Use when choosing between multiple approaches:

**Quick Pattern**:
```
We need to [problem]. Give me 3 solutions and compare in a table:
| Approach | [Criterion 1] | [Criterion 2] | ... | Recommendation |
```

**6 Template Types**:
1. Algorithm Selection (e.g., pitch detection methods)
2. Architecture Decision (e.g., state management)
3. DSP Algorithm Comparison
4. Performance Trade-off Analysis
5. Library/Dependency Selection
6. Refactoring Decision

**Example**:
```
We need to pass audio buffers between threads. Give me 3 solutions:
1. Lock-free ring buffer
2. Triple-buffering with atomics
3. Lock-free queue

Compare: | Latency | Memory | Complexity | Real-Time Safe | Recommendation |
```

### Role-Play Scenarios (.claude/templates/role-play-scenarios.md)

Use for deeper architectural analysis:

**8 Scenario Types**:
1. **Design Review Debate**: Multiple perspectives on design
2. **Code Review Panel**: Multi-expert code review
3. **ADR Dialogue**: Document architectural decisions
4. **Pair Programming**: Collaborative implementation
5. **Socratic Method**: Learn concepts deeply
6. **Devil's Advocate**: Stress-test proposals
7. **ELI5/ELI-Expert**: Multi-level explanations
8. **Pre-Mortem**: Identify risks before they happen

**Example - Design Debate**:
```
Let's review IntonationAnalyzer state machine design.

Roles:
- You (Claude): Senior C++ engineer skeptical of state machines
- Me: Architect defending state-driven approach
- Observer: Junior dev learning patterns

Debate: State machine vs simple if/else logic
```

## Context Management

### Monitor Context Usage

```bash
# Check context size frequently
/context

# Trim if >150K tokens
/compact
```

### Provide Specific Search Hints

❌ **Vague**: "Look at the codebase"

✅ **Specific**:
```
Analyze IntonationAnalyzer state machine. Focus on:
- src/Analysis/Intonation/IntonationAnalyzer.h for state enum
- Use Grep tool to find all state transitions
- Check tests/Analysis/TestIntonationAnalyzer.cpp for state tests
```

## Workflow Patterns

### TDD Cycle

1. Write failing test first
2. Verify test fails (don't run, mention)
3. Write minimal implementation
4. User runs: `ctest -R TestName`
5. Refactor while keeping tests green

### Commit Workflow

1. Run `/review-commit`
2. Check all 7 verification points
3. Get suggested commit message
4. User commits with message (imperative mood)

### Architecture Documentation

1. Invoke `architecture-explainer` skill
2. Get ASCII diagram + thread analysis
3. Review design rationale
4. Document in `.claude/docs/` if needed

## Common Tasks Quick Reference

### Adding New Analyzer

```bash
/add-analyzer

# Follow prompts for:
# - Analyzer name
# - Base class: Analysis::Analyzer
# - Location: src/Analysis/[Name]/
# - TDD workflow
```

### Performance Investigation

```bash
/perf-profile src/Analysis/AnalysisEngine.cpp

# Reviews:
# - Allocations
# - Cache-friendliness
# - Copy vs const ref
# - Loop efficiency
# - Virtual function overhead
```

### DSP Validation

```bash
/dsp-validate src/Analysis/Intonation/IntonationAnalyzer.cpp

# Checks:
# - Mathematical correctness
# - Sample rate conversions
# - Nyquist constraints
# - Numerical stability
```

### Real-Time Safety Audit

```bash
/rt-audit src/App/AudioProcessingLayer.cpp

# Searches for FORBIDDEN:
# - Allocations (new, malloc, push_back)
# - Mutexes (std::mutex, lock_guard)
# - I/O (cout, spdlog, ofstream)
```

## Best Practices Summary

### Context Engineering

✅ **DO**:
- Use `/context` after reading 5+ files
- Provide file/tool hints in requests
- Use modular CLAUDE.md imports
- Leverage Serena for symbol navigation

❌ **DON'T**:
- Let context exceed 150K tokens
- Use vague exploration requests
- Read entire files unnecessarily

### Prompt Engineering

✅ **DO**:
- Use comparative analysis for decisions
- Apply role-play for deep analysis
- Break complex tasks into steps
- Request specific formats (tables, ADRs)

❌ **DON'T**:
- Accept first solution without comparison
- Skip architectural justification
- Rush to implementation without planning

### Code Quality

✅ **DO**:
- Run `/review-commit` before every commit
- Use `/rt-audit` for audio callback code
- Follow TDD workflow (test first)
- Keep functions ≤100 lines

❌ **DON'T**:
- Initialize members in headers
- Implement methods inline in headers
- Allocate in audio callbacks
- Use `using namespace` (except tests)

## Project-Specific Reminders

### Thread Safety Model

- **Real-time Audio**: NO allocations, NO mutexes, NO I/O
- **Worker Thread**: Can allocate, use mutexes, log
- **UI Thread**: Read-only access via atomic shared_ptr

### Performance Targets

- CPU: <10% on i5/i7
- Memory: <100MB
- Latency: <20ms (input → display)
- Audio buffer: 512 samples @ 48kHz

### Coding Standards

- Headers: `#pragma once`, NO in-class init
- Namespaces: PascalCase, NO `using namespace`
- Variables: camelCase (including members)
- Constructors: ALWAYS use initializer lists
- Formatting: Allman braces, 120 char line limit

## Documentation Structure

```
.claude/
├── docs/                      # Modular documentation
│   ├── architecture.md        # System design, components
│   ├── cpp-coding-standards.md # Naming, formatting
│   ├── real-time-constraints.md # Audio safety rules
│   ├── dsp-algorithms.md      # Algorithm specs
│   ├── build-and-test.md      # CMake, TDD, VS Code
│   └── git-workflow.md        # Commit guidelines
├── commands/                  # Slash commands
│   ├── add-analyzer.md
│   ├── rt-audit.md
│   ├── review-commit.md
│   ├── dsp-validate.md
│   └── perf-profile.md
├── skills/                    # Project skills
│   └── architecture-explainer/
├── templates/                 # Prompt templates
│   ├── comparative-analysis.md
│   └── role-play-scenarios.md
└── QUICK-REFERENCE.md         # This file
```

## Getting Help

### Built-in Help

```bash
/help                  # Claude Code help
/context               # Check context usage
/compact               # Trim context
```

### Template Access

```bash
# Read comparative analysis templates
Read: .claude/templates/comparative-analysis.md

# Read role-play scenario templates
Read: .claude/templates/role-play-scenarios.md
```

### Skill Documentation

```bash
# View architecture-explainer skill docs
Read: .claude/skills/architecture-explainer/SKILL.md

# View test-generator skill docs
Read: ~/.claude/skills/test-generator/SKILL.md
```

## Quick Start Examples

### Example 1: Adding Harmonic Tracking Feature

```
# 1. Use comparative analysis
"We need to track 10 harmonics over time. Give me 3 approaches:
1. Store in circular buffer
2. Use std::deque with time stamps
3. Rolling window with pre-allocated array

Compare: | Memory | Performance | Complexity | Real-Time Safe |"

# 2. After deciding, scaffold with command
/add-analyzer

# 3. Follow TDD workflow
"Write failing test for HarmonicTracker::Track(fundamental, timestamp)"

# 4. Implement and review
/review-commit
```

### Example 2: Investigating Performance Issue

```
# 1. Profile the code
/perf-profile src/Analysis/AnalysisEngine.cpp

# 2. Comparative analysis of solutions
"CPU usage is 15% (target <10%). Give me 3 optimization strategies:
1. Reduce FFT size
2. Skip frames when idle
3. Use SIMD for buffer operations

Compare: | CPU Impact | Latency Impact | Complexity | Risk |"

# 3. Use role-play for validation
"Pre-mortem: Optimization broke real-time guarantees. What went wrong?"
```

### Example 3: Architectural Decision

```
# 1. Use ADR role-play template
"Create ADR for: How to notify UI of analysis results?

Roles:
- Tech Lead (you): Document decision
- Performance Engineer (you): Minimize overhead
- Reliability Engineer (you): Ensure thread safety
- Product Owner (me): Need 60 FPS UI updates

Generate ADR with decision rationale."

# 2. Document with architecture-explainer
[Invoke architecture-explainer skill]
"Explain the chosen notification mechanism with thread diagram"

# 3. Add to documentation
"Update .claude/docs/architecture.md with this decision"
```

## Kaizen Retrospective Template

Run every 5 tasks or weekly:

```
Let's do a Kaizen retrospective:

1. What went well in our last few tasks?
2. What was inefficient or repetitive?
3. Which patterns should become:
   - CLAUDE.md updates
   - Slash commands
   - Agent skills
4. What mistakes did we make repeatedly?

Suggest 1-2 concrete improvements to our workflow.
```

---

**Last Updated**: 2025-12-09
**For**: Guitar Diagnostics C++20 Audio DSP Project
**Claude Code Version**: Latest
