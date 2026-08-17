# ATLAS roadmap

ATLAS is an **In Development** C++20 research prototype for routing and
optimization over weighted graphs. This roadmap follows the project
specification and keeps algorithm correctness ahead of infrastructure breadth.

## Phase 0 — Discovery and foundation

- Establish C++20/CMake build
- Define graph, route, and error contracts
- Add deterministic fixtures and baseline tests
- Add local build documentation and CI

## Phase 1 — Graph model and parser

- Implement validated node and edge models
- Build adjacency-list representation
- Load a documented deterministic graph format
- Add invalid-input diagnostics

## Phase 2 — Correctness-first routing

- BFS and DFS traversal
- Dijkstra with binary heap
- Path reconstruction and unreachable handling
- Correctness oracle and randomized fixed-seed tests

## Phase 3 — Heuristic and bidirectional routing

- Geographic metadata and admissible heuristic
- A* search
- Bidirectional Dijkstra
- Controlled algorithm comparison

## Phase 4 — Benchmarking and evaluation

- Versioned graph snapshots
- Short, medium, and long query buckets
- Runtime, nodes expanded, heap operations, and memory reporting
- Regression checks only after baselines are recorded

## Phase 5 — Dynamic routing and caching

- Edge-weight and closure updates
- Graph revisions
- Route cache and invalidation
- Replayable update tests

## Phase 6 — Advanced preprocessing

- ALT/landmarks, ALT A* integration, and correctness benchmarking (implemented)
- Logical ALT index-size estimation (implemented)
- Contraction Hierarchies deferred pending representative benchmark evidence
  (see [ADR 0001](decisions/0001-alt-before-contraction-hierarchies.md))

## Phase 7 — Vehicle routing optimization

- Depot, delivery, capacity, and time-window contracts
- Greedy and nearest-neighbor baselines
- 2-opt local search
- Multi-vehicle objectives and quality comparison (implemented); runtime
  benchmarking remains planned

## Phase 8 — Backend and demonstration quality

- Deterministic CLI demonstration, structured errors, and service boundary
  (implemented)
- Deterministic search budgets and in-process service metrics (implemented)
- Transport-neutral JSON response serialization (implemented)
- In-process REST-style route adapter (implemented)
- Thin REST or gRPC API (planned)
- Optional spatial storage and cache integrations (planned)
- Demonstration UI that exposes, but does not own, algorithm logic (planned)

## Status rule

Only implemented and verified behavior may be presented as complete. Benchmark
numbers, optimization quality, production deployment, and market validation
must not be claimed until measured or collected.
