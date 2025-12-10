# Comparative Analysis Prompt Templates

Use these templates to get structured comparison of multiple approaches from Claude.

## Template 1: Algorithm Selection

```
We need to [describe problem]. Give me 3-4 solutions:

1. [Approach 1 name/description]
2. [Approach 2 name/description]
3. [Approach 3 name/description]
4. [Approach 4 name/description] (optional)

Compare in a markdown table with these columns:
| Approach | [Criterion 1] | [Criterion 2] | [Criterion 3] | [Criterion 4] | Recommendation |

Then provide:
- **Best for production**: [Which and why]
- **Best for prototyping**: [Which and why]
- **Best for performance**: [Which and why]
```

### Example: Thread Communication

```
We need to pass audio buffers between threads. Give me 3 solutions:

1. Lock-free ring buffer (current approach)
2. Triple-buffering with atomics
3. Lock-free queue (boost::lockfree)

Compare in a table:
| Approach | Latency | Memory Overhead | Complexity | Real-Time Safe | Portability | Recommendation |
```

## Template 2: Architecture Decision

```
Architectural decision: [describe decision to make]

Context:
- [Constraint 1]
- [Constraint 2]
- [Performance target]

Provide 3 approaches with:
1. **Design sketch**: Brief description + ASCII diagram
2. **Pros**: 3-4 key advantages
3. **Cons**: 3-4 key disadvantages
4. **Complexity**: Scale 1-5 (1=simple, 5=complex)
5. **Risk**: Scale 1-5 (1=low risk, 5=high risk)

Then rank by:
- Maintainability
- Performance
- Time to implement
```

### Example: State Management

```
Architectural decision: How should IntonationAnalyzer manage state transitions?

Context:
- 5 states: Idle → OpenString → WaitFor12thFret → FrettedString → Complete
- Must handle invalid inputs gracefully
- Called from worker thread (not real-time)

Provide 3 approaches:
1. Enum-based state machine with switch statements
2. State pattern (polymorphic state objects)
3. Table-driven state machine
```

## Template 3: DSP Algorithm Comparison

```
DSP problem: [describe what needs to be detected/analyzed]

Input constraints:
- Sample rate: [e.g., 48kHz]
- Buffer size: [e.g., 512 samples]
- Frequency range: [e.g., 80Hz - 1kHz]

Compare 3-4 algorithms:
| Algorithm | Accuracy | Latency | CPU Cost | Memory | Complexity | Best For |

For each algorithm, specify:
- **Mathematical basis**: [Brief description]
- **Implementation complexity**: [1-5 scale]
- **Reference**: [Paper/library if applicable]
```

### Example: Pitch Detection

```
DSP problem: Detect guitar string pitch accurately in range 80Hz - 1.2kHz

Input constraints:
- Sample rate: 48kHz
- Buffer size: 2048 samples
- Must work with harmonic-rich signals

Compare algorithms:
| Algorithm | Accuracy (cents) | Latency (ms) | CPU (%) | Memory (KB) | Complexity | Best For |
|- YIN
|- Autocorrelation
|- FFT + HPS (Harmonic Product Spectrum)
|- ASDF (Average Squared Difference Function)
```

## Template 4: Performance Trade-off Analysis

```
Performance optimization for: [component/function name]

Current metrics:
- CPU: [current %]
- Latency: [current ms]
- Memory: [current MB]

Target metrics:
- CPU: [target %]
- Latency: [target ms]
- Memory: [target MB]

Analyze 3 optimization strategies:
| Strategy | CPU Impact | Latency Impact | Memory Impact | Complexity | Risk Level | Estimated Gain |

For top 2 strategies, provide:
- **Implementation approach**: [Brief steps]
- **Potential pitfalls**: [What could go wrong]
- **Testing strategy**: [How to verify improvement]
```

## Template 5: Library/Dependency Selection

```
Need library for: [functionality]

Requirements:
- [Requirement 1]
- [Requirement 2]
- [Performance constraint]
- [Licensing constraint]

Compare 3-4 options:
| Library | Features | Performance | License | Maturity | Integration Effort | Documentation | Recommendation |

For each library:
- **Pros**: [List advantages]
- **Cons**: [List disadvantages]
- **Community**: [Active? GitHub stars? Last update?]
```

## Template 6: Refactoring Decision

```
Considering refactoring: [component/code section]

Current issues:
- [Issue 1]
- [Issue 2]
- [Technical debt concern]

Propose 3 refactoring approaches:
1. [Minimal change approach]
2. [Moderate restructuring]
3. [Complete redesign]

Compare:
| Approach | Improves X | Improves Y | Risk | Time | Breaking Changes | Recommendation |

For recommended approach:
- **Migration path**: [How to transition]
- **Testing strategy**: [How to ensure no regressions]
- **Rollback plan**: [If something goes wrong]
```

## Usage Tips

1. **Be specific with criteria**: Choose comparison dimensions relevant to your project
2. **Include context**: Constraints, targets, current state
3. **Ask for ranking**: Request explicit recommendation with justification
4. **Scale appropriately**: Use 1-5 scales for subjective measures
5. **Request examples**: Ask for code snippets for top 2 approaches
6. **Iterate**: Start with 3 options, drill deeper into top 2

## Follow-up Questions

After getting comparison table:

```
For the recommended approach ([approach name]):
1. Show example implementation (pseudocode or C++20)
2. What are the top 3 implementation risks?
3. How would you test this?
4. What documentation should we update?
```

Or for deeper analysis:

```
Debate: [Approach A] vs [Approach B]

Roles:
- You (Claude): Senior engineer advocating for [Approach A]
- Me: Architect preferring [Approach B]
- Observer: Junior dev learning design patterns

Provide arguments FOR and AGAINST each, then synthesize best hybrid approach.
```
