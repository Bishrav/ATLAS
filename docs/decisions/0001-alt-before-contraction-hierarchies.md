# ADR 0001: Complete ALT evaluation before Contraction Hierarchies

## Status

Accepted — Contraction Hierarchies is deferred until representative benchmark
evidence justifies its preprocessing and maintenance cost.

## Context

ATLAS now has deterministic ALT landmark preprocessing, a revision-matched ALT
A* overload, correctness checks against Dijkstra, and a logical index-size
estimate. Contraction Hierarchies would add shortcut construction, ordering,
query, update, and shortcut-correctness contracts.

The current benchmark workload is intentionally small and deterministic. This
environment also does not have the C++ toolchain required to collect runtime
or preprocessing measurements locally. No performance advantage is assumed.

## Decision

Keep ALT as the active advanced-preprocessing path. Before implementing
Contraction Hierarchies, collect benchmark evidence across representative graph
sizes and query distributions, including:

- preprocessing time and logical index size;
- query runtime and nodes expanded;
- correctness against an independent shortest-path oracle; and
- behavior after graph revisions, where applicable.

Implement Contraction Hierarchies only if those measurements show a meaningful
benefit for ATLAS's intended workload and the additional update complexity is
acceptable.

## Consequences

- The current routing surface remains smaller and easier to verify.
- ALT can be evaluated without claiming an unmeasured speedup.
- Contraction Hierarchies remains a documented, conditional roadmap item.
- A future implementation must add shortcut-specific correctness and index-size
  tests before being presented as complete.
