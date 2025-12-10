Create a new analyzer following the project pattern:

1. Inherits from `Analysis::Analyzer` base class
2. Constructor initializer list (NO in-class initialization)
3. Separate .h (in src/Analysis/) and .cpp files
4. PascalCase namespace, camelCase variables
5. Include corresponding Google Test file
6. Follow TDD: write failing test first

Constraints:

- Methods MUST be in .cpp (never inline in header)
- Use #pragma once
- Allman brace style
- Functions ≤100 lines
