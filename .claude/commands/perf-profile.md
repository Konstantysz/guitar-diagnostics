Analyze the specified code for performance bottlenecks:

1. Identify allocations (even hidden ones in STL)
2. Check cache-friendliness (data layout, access patterns)
3. Look for unnecessary copies (pass by value vs const ref)
4. Review loop efficiency (invariant hoisting, vectorization potential)
5. Check for virtual function overhead in hot paths
6. Suggest pre-allocation strategies

Target: <10% CPU on modern i5/i7, <20ms total latency.
