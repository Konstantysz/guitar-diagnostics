# Role-Play Scenario Templates

Use role-play to get deeper architectural analysis through debate and multiple perspectives.

## Template 1: Design Review Debate

```
Let's review the [component/design] design through debate.

Roles:
- You (Claude): [Role 1 with perspective]
- Me: [Role 2 with different perspective]
- Observer: [Role 3 learning from debate]

Rules:
1. [Role 1] presents design + rationale
2. [Role 2] challenges with 3 critical questions
3. [Role 1] defends or concedes each point
4. Observer synthesizes: what approach makes most sense?

Focus areas:
- [Focus 1, e.g., Performance]
- [Focus 2, e.g., Maintainability]
- [Focus 3, e.g., Testability]
```

### Example: State Machine Debate

```
Let's review the IntonationAnalyzer state machine design.

Roles:
- You (Claude): Senior C++ audio engineer skeptical of state machines, prefer simple if/else
- Me: Architect defending state-driven approach for clarity
- Observer: Junior dev learning design patterns

Rules:
1. I present state machine design + benefits
2. You challenge with 3 specific concerns about complexity/overhead
3. I defend each or propose compromises
4. Observer: which approach is best for THIS project?

Focus areas:
- Code clarity vs complexity trade-off
- Runtime overhead in worker thread
- Ease of adding new states later
```

## Template 2: Code Review Panel

```
Code review panel for: [component/feature]

Panel members (all Claude):
1. **Performance Expert**: Focus on CPU, memory, latency
2. **Safety Advocate**: Focus on thread safety, undefined behavior
3. **Maintainability Guardian**: Focus on readability, testability
4. **Domain Specialist**: Focus on audio/DSP correctness

Each reviewer provides:
- ✅ **Strengths**: 2-3 things done well
- ⚠️ **Concerns**: 2-3 potential issues (with severity: Low/Med/High)
- 💡 **Suggestions**: 1-2 specific improvements

Final consensus: Approve / Request Changes / Block
```

### Example: FretBuzzDetector Review

```
Code review panel for: FretBuzzDetector::Analyze() implementation

Panel members:
1. **Performance Expert**: Check for allocations, cache misses, hot loops
2. **Safety Advocate**: Verify no UB, proper bounds checking
3. **Maintainability Guardian**: Assess function length, naming, comments
4. **DSP Specialist**: Validate transient detection algorithm correctness

Review this implementation: [paste code or file path]
```

## Template 3: Architecture Decision Record (ADR) Dialogue

```
Create Architecture Decision Record for: [decision]

Characters:
- **Tech Lead (you)**: Facilitate discussion, document decision
- **Performance Engineer (you)**: Prioritize speed/efficiency
- **Reliability Engineer (you)**: Prioritize stability/safety
- **Product Owner (me)**: Provide requirements/constraints

Process:
1. Product Owner states requirements
2. Performance Engineer proposes optimal approach
3. Reliability Engineer raises risks
4. Tech Lead synthesizes compromise
5. Document as ADR with: Context, Decision, Consequences

Decision to make: [describe decision]
```

### Example: Audio Buffer Threading

```
Create ADR for: How should audio buffers move between threads?

Characters:
- Tech Lead: Balance all concerns, document final decision
- Performance Engineer: Minimize latency, maximize throughput
- Reliability Engineer: Ensure no data races, corruption
- Product Owner (me): Requirement - audio must NEVER glitch, < 20ms latency

Process:
1. I state: "Users cannot tolerate any audio glitches or latency > 20ms"
2. Performance Engineer: Propose lock-free ring buffer approach
3. Reliability Engineer: Raise concerns about SPSC vs MPSC guarantees
4. Tech Lead: Document decision with rationale

Generate ADR in this format:
# ADR-XXX: [Title]
## Status: [Proposed/Accepted/Deprecated]
## Context: [Background]
## Decision: [What we decided]
## Consequences: [Benefits and trade-offs]
```

## Template 4: Pair Programming Session

```
Pair programming session: [task description]

Roles:
- **Driver (you)**: Write code, explain thinking aloud
- **Navigator (me)**: Review, ask questions, suggest improvements

Rules:
1. Driver explains intent before writing each function
2. Navigator asks: "Why this approach?" or "What about edge case X?"
3. Switch on my cue or every 20 minutes
4. Driver must justify decisions or accept Navigator's suggestion

Task: [specific coding task]

Start by explaining your implementation approach at high level.
```

### Example: Add Harmonic Tracking

```
Pair programming: Add harmonic tracking to StringHealthAnalyzer

Driver (Claude): Write tracking code
Navigator (me): Review and challenge

Task: Track 10 harmonics (f0, 2f0, ... 10f0) over 3 seconds

Start by outlining your approach:
1. How to detect fundamental frequency?
2. How to track harmonics over time?
3. Where to store results?
```

## Template 5: Socratic Method (Learning)

```
Teach me about [concept] using Socratic method.

Process:
1. You ask leading question about concept
2. I answer based on my understanding
3. You probe deeper with follow-up question
4. Continue until I discover key insight
5. You summarize what I learned

Concept to explore: [technical concept]

Start with your first question.
```

### Example: Lock-Free Programming

```
Teach me about lock-free programming for audio using Socratic method.

Topic: Why can't we use std::mutex in audio callback?

Start with: "What happens when a thread calls lock() on a mutex that's already locked?"

Guide me to discover:
- Priority inversion
- Unbounded waiting time
- Why this violates real-time constraints
```

## Template 6: Devil's Advocate

```
Play devil's advocate on: [proposal/design]

My proposal:
[Describe what you want to do]

Your role: Challenge this proposal with:
1. **Technical concerns**: What could go wrong technically?
2. **Performance concerns**: Where are the bottlenecks?
3. **Maintenance concerns**: How does this age?
4. **Alternative proposals**: What's better?

For each concern, rate severity: 🟢 Low | 🟡 Medium | 🔴 High

Then: Despite concerns, would you approve this? Why/why not?
```

### Example: Replace YIN with ML Model

```
Play devil's advocate on: Replace YIN pitch detection with TensorFlow Lite ML model

My proposal:
- Train neural network on guitar samples
- Deploy TFLite model for pitch detection
- Claim: Better accuracy, handles complex timbres

Challenge this with technical, performance, and maintenance concerns.
```

## Template 7: Explain Like I'm... (ELI5/ELI15)

```
Explain [technical concept] at two levels:

**ELI15 (high school student learning programming)**:
- Use analogies to everyday experiences
- Avoid jargon, or define when necessary
- Focus on "why it matters"

**ELI-Expert (senior dev from different domain)**:
- Assume CS fundamentals
- Use precise technical terms
- Focus on "how it's different from X"

Concept: [what to explain]
```

### Example: Lock-Free Ring Buffer

```
Explain lock-free ring buffer at two levels:

ELI15: Explain without assuming knowledge of threads
ELI-Expert: Explain assuming knowledge of multithreading but not audio programming

Focus on: Why we need this for real-time audio
```

## Template 8: Pre-Mortem Analysis

```
Pre-mortem: It's 6 months from now, and [component/feature] has failed catastrophically.

Roles:
- **Forensic Analyst (you)**: Investigate what went wrong
- **Team Lead (me)**: Answer questions about decisions

Process:
1. Describe the failure scenario (be specific)
2. List 5-7 plausible causes (from most to least likely)
3. For top 3 causes: What warning signs did we miss?
4. Preventive measures: What should we do NOW?

Component/Feature: [what to analyze]
```

### Example: Production Audio Glitches

```
Pre-mortem: 6 months from now, users report frequent audio glitches/dropouts.

Investigate:
1. What likely caused this? (technical root causes)
2. What warning signs did we ignore during development?
3. What metrics should we monitor to catch this early?
4. What should we do differently NOW?
```

## Usage Tips

### When to Use Each Template

- **Design Review Debate**: When considering architectural changes
- **Code Review Panel**: Before merging significant features
- **ADR Dialogue**: For decisions with long-term consequences
- **Pair Programming**: When implementing complex algorithms
- **Socratic Method**: When you need to deeply understand a concept
- **Devil's Advocate**: To stress-test your ideas before committing
- **ELI5/ELI-Expert**: When documenting for different audiences
- **Pre-Mortem**: Before starting risky/complex features

### How to Get Best Results

1. **Set clear roles**: Define what perspective each role represents
2. **Use constraints**: Time limits, focus areas, severity scales
3. **Request specific format**: ADR, markdown table, bullet points
4. **Follow the process**: Don't skip steps in multi-step dialogues
5. **Iterate depth**: Start broad, drill into top concerns
6. **Synthesize**: Always end with actionable conclusion

### Follow-Up Patterns

After role-play, ask:
```
Based on this discussion:
1. What should I implement first?
2. What risks should I document?
3. What tests should I write?
4. What documentation should I update?
```

Or request artifacts:
```
Generate:
- ADR document (Markdown)
- Risk assessment matrix
- Test plan outline
- Updated design diagram
```
