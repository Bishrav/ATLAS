# Benchmarking

ATLAS benchmarks Dijkstra, A*, and bidirectional Dijkstra on a fixed graph and
short, medium, and long query buckets. Each row reports measured runtime,
successful queries, correctness against Dijkstra, nodes expanded, priority
queue pushes, and estimated graph-storage bytes.

The benchmark output is a local observation. It is not a production capacity
claim.

The memory value is an estimate of graph adjacency and coordinate container
storage. It is not process RSS, allocator overhead, executable memory, or total
application memory.

## Regression comparison

The `compare_benchmarks` API compares current rows with a baseline supplied by
the caller. It matches rows by algorithm and bucket, rejects correctness
failures and query-count changes, and applies a configurable runtime slowdown
threshold. The repository does not check in baseline timings because they are
machine- and workload-dependent.

Future regression reports should record the graph snapshot, query set, compiler
version, hardware, build flags, and raw output alongside the threshold used.
ATLAS now emits compiler family, `__cplusplus` language-standard value, and
platform metadata at benchmark startup; hardware and build flags still need to
be recorded by the benchmark runner.
