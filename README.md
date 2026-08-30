# matching-engine

A limit order matching engine in C++, price-time priority.

**Status:** scaffold only (commit 1 of v0). No book or matching logic
yet — this commit exists to prove the build/CI/types are solid before
any logic gets written on top of them. Real content — the book, the
matching loop, tests, benchmarks, and the full writeup — lands as the
project progresses toward the `v0` tag.

See `DECISIONS.md` for the reasoning behind design choices as they're
made, starting with why prices are integer ticks rather than floats.
