# ATLAS

ATLAS is an **In Development** C++20 routing and optimization engine for
weighted graph algorithms, dynamic routing, and vehicle-routing experiments.

## Current status

Phase 0 foundation is in progress. The repository currently contains a CMake
build, a versioned core library, a validated in-memory graph contract, a deterministic graph parser, a minimal CLI, and a baseline test. Routing
algorithms, graph ingestion, benchmarks, dynamic updates, and VRP optimization
are planned and are not yet implemented.

## Build and test

Requirements:

- CMake 3.20 or newer
- A compiler with C++20 support

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
.\build\Release\atlas.exe --version
```

On single-configuration generators, the executable may be at `build/atlas`
instead.

## Architecture direction

```text
Graph data -> parser -> validated adjacency graph -> path engine
                                             |              |
                                             v              v
                                      dynamic updates   benchmarks
                                             |
                                             v
                                      route optimizer
```

The full staged plan is in [docs/roadmap.md](docs/roadmap.md). The first
implementation will prioritize deterministic graph facts and shortest-path
correctness before adding external databases, caches, APIs, or UI.

The initial graph format uses one record per line: `node <id>` or
`edge <from> <to> <weight>`. Blank lines and `#` comments are accepted. Edge
endpoints must be declared before use, and weights must be finite and
non-negative.

## Repository structure

```text
include/atlas/   Public core interfaces
src/atlas/       Core implementation
apps/            Command-line entry points
tests/           CTest-based checks
docs/            Roadmap and design documentation
```
