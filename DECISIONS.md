# Decisions

Why, not what -- the README documents behavior, this documents reasoning.

## Integer ticks for Price, not floating point

`Price` is `int64_t`, representing integer ticks rather than a decimal
value. Floating-point prices are the classic matching-engine bug class:
IEEE 754 doesn't represent most decimal fractions exactly (`0.1 + 0.2 !=
0.3`), which is fatal for a book that has to compare prices for exact
equality (does this order's price match a resting level?) and ordering
(does it cross the spread?). Real exchanges quote and match in integer
tick units for exactly this reason -- the tick size is a market-defined
constant, and converting to/from a human-readable decimal price is a
display-layer concern, not something the book or matching logic ever
touches.

`Quantity` and `OrderId` are unsigned 64-bit for the same category of
reason: quantities are never meaningfully negative, and 64 bits gives
enough headroom that overflow isn't a concern at any benchmark scale
this project will run.

## Unsigned Quantity and underflow

Unsigned is still the right choice, but it has a sharp edge: a matching
bug that computes `remainingQty -= filledQty` with `filledQty >
remainingQty` doesn't throw -- it wraps around to a number near 2^64,
and a level with a "huge" resting quantity silently eats every order
that touches it afterward instead of crashing loudly. Handled by
guarding, not by switching to signed:

- an explicit `assert(filledQty <= remainingQty)` (or equivalent check)
  before every such subtraction, active in debug builds -- lands with
  the matching loop itself (commit 3+), not here, since there's no
  subtraction yet
- a dedicated fuzz invariant once the fuzzer exists: `remainingQty`
  must never exceed `submittedQty` for any order, at any point --
  underflow is exactly the failure mode that check catches

## Time priority: list position, not a timestamp

`Order` deliberately has no timestamp or sequence field. FIFO ordering
within a price level comes from position in the `std::list` (front =
oldest = matches first), not from comparing clock values. Two reasons
this is staying, and it's worth remembering when v1 (modify, multiple
instruments) makes a sequence number tempting to bolt on for other
reasons: first, it's simpler -- no clock dependency to get right, no
tie-breaking logic if two timestamps collide. Second, and more
important: it makes v0's matching deterministic and testable -- given
the same sequence of add/cancel calls, the book's behavior has no
wall-clock dependency at all. If v2's sequencer later needs a real
sequence number for total ordering across threads, that's a distinct,
deliberate addition for a distinct reason (cross-thread ordering, not
intra-level FIFO) -- not a replacement for this.
