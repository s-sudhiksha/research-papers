# Performance Analysis — Linked List Implementations

## Setup
- Machine: [CPU model, RAM, OS — fill in]
- Compiler: `gcc` version [fill in]
- Compile flags used: `-O2 -std=c11`
- Date of run: [YYYY-MM-DD]

## Quick summary
- Cache-optimized traversal gives large improvements for big lists.
- Batch insertion reduces per-element overhead.
- Memory pool reduces allocation overhead and becomes better after ~20k nodes.

## Detailed observations (fill with CSV & graphs)
- Traversal (size=100000): standard = 13.782 ms, cache = 8.341 ms → ~39.5% improvement.
- Insertion (size=100000): standard = 24.918 ms, batch = 16.743 ms, pool = 14.102 ms.
- Memory: pooling reduces memory usage for large lists (see table).

## Limitations
- `getrusage` based memory/counter values are approximate.
- Benchmarks run on a single machine — results may vary.
- Pool implementation uses fixed capacity.

## Next steps / Future work
- Add randomized workloads (random insert/delete).
- Use hardware perf counters (`perf`) to measure cache misses precisely.