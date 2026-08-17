# ATLAS Operations and Verification

ATLAS is currently a local C++20 research prototype. The supported operating
model is a deterministic build, test, CLI demonstration, and benchmark run on a
machine with CMake and a C++20 compiler.

## Build and test

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

## Demonstration

```powershell
.\build\Release\atlas.exe --version
.\build\Release\atlas.exe --help
.\build\Release\atlas.exe demo-route --algorithm dijkstra
.\build\Release\atlas.exe demo-route --algorithm a-star
.\build\Release\atlas.exe demo-route --algorithm bidirectional
```

The demonstration uses the fixed benchmark graph. It is not a network service
and does not ingest live traffic or map data.

## Benchmarking

```powershell
.\build\Release\atlas_benchmark.exe
```

Benchmark output is machine- and workload-dependent. It reports correctness,
search metrics, estimated memory values, and local elapsed time. No baseline
performance number is committed as a product guarantee.

## Current limitations

- This checkout requires an external CMake/C++20 toolchain for executable
  verification.
- The routing service is in-process; HTTP and gRPC adapters are not included.
- Search budgets limit expanded nodes deterministically; they are not wall-clock
  cancellation.
- External traffic, map ingestion, databases, Prometheus, and a web UI are not
  implemented.
- VRP functionality is baseline optimization, not a proof of optimality.

## Release evidence rule

Only claims backed by repository code, tests, CI output, or recorded benchmark
artifacts should be presented as completed evidence.
