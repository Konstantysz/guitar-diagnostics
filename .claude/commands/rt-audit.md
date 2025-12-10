Audit the specified file for real-time audio thread violations:

Search for FORBIDDEN patterns:
- Allocations: new, delete, malloc, free, std::vector::push_back
- Containers that might allocate: std::string +=, std::vector without reserve
- Mutexes: std::mutex, std::lock_guard
- I/O: std::cout, spdlog, std::ofstream, printf

Report line numbers with violations.
Suggest lock-free alternatives (ring buffer, atomics, pre-allocation).
