---
name: architecture-explainer
description: Explain guitar-diagnostics architectural decisions with ASCII diagrams and thread flow analysis.
allowed-tools: Read,Grep,mcp__serena
---

# Architecture Documentation Generator

## Description
Generate clear explanations of guitar-diagnostics architecture with ASCII diagrams, thread flow analysis, and design rationale.

## Trigger
User asks "how does X work", "explain the architecture", "document this design", or "why is this designed this way"

## Workflow

1. **Identify Scope**: Determine what architectural element to explain:
   - Full system architecture
   - Specific component (AnalysisEngine, AudioProcessingLayer, etc.)
   - Thread interaction model
   - Data flow patterns

2. **Gather Information**: Use Serena tools to understand:
   - Component structure (get_symbols_overview)
   - Component relationships (find_referencing_symbols)
   - Thread boundaries (search for mutex, atomic, ring buffer)
   - Data flow (search for Write, Read, Publish patterns)

3. **Create Visual Representation**: Generate ASCII diagram showing:
   - Component hierarchy
   - Thread boundaries (Real-time vs Worker vs UI)
   - Data flow (ring buffer, atomic shared_ptr)
   - Communication patterns

4. **Explain Design Decisions** in terms of:
   - **Responsibility**: What is each component's single responsibility?
   - **Thread Safety**: What guarantees does the design provide?
   - **Performance**: What are the latency/throughput implications?
   - **Trade-offs**: What alternatives were rejected and why?

5. **Reference Documentation**: Link to relevant sections in CLAUDE.md:
   - @.claude/docs/architecture.md for system design
   - @.claude/docs/real-time-constraints.md for thread safety
   - @.claude/docs/cpp-coding-standards.md for conventions

6. **Use Role-Play for Deep Analysis** (when appropriate):
   - "I'm a new team member reviewing this design"
   - "I'm a senior engineer skeptical of this approach"
   - "I'm explaining this to someone unfamiliar with real-time audio"

## Example Output Format

```
# [Component Name] Architecture

## Overview
[1-2 sentence summary]

## Component Diagram
[ASCII art showing structure]

## Thread Model
[Which thread(s), priority, constraints]

## Data Flow
[How data moves through the component]

## Design Rationale
- **Why this approach?** [Explain choice]
- **What alternatives?** [Mention rejected options]
- **Trade-offs?** [Benefits vs costs]

## Key Interfaces
[List main public methods with purpose]

## Dependencies
[What this component depends on]

## Thread Safety Guarantees
[Locking strategy, wait-free guarantees, etc.]
```

## Special Considerations

- **Real-time components**: Emphasize lock-free design, pre-allocation
- **Worker threads**: Highlight where allocations/mutexes are allowed
- **UI components**: Focus on read-only access, atomic updates
- **DSP algorithms**: Reference papers, explain mathematical basis
- **State machines**: Show state transitions, event handling

## Performance Context

Always mention performance targets:
- CPU: <10% on modern i5/i7
- Memory: <100MB RAM
- Latency: <20ms total (input → display)
- Audio buffer: 512 samples @ 48kHz (10.7ms)
