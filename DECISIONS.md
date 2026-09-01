# Decisions

Notes on why, since I won't remember in three months.

## Prices are integer ticks, not floats

0.1 + 0.2 != 0.3 in floating point. That's a real bug in a system that
compares prices for equality and for crossing, not a theoretical one.
Real exchanges match on ticks for the same reason. Price is int64_t.
Converting to a display price happens at the edges, never inside the
book or matching logic.

Quantity and OrderId are unsigned 64-bit -- never negative, and 64 bits
is way more than I'll need at any scale I'll actually benchmark.

## Unsigned quantity means I have to watch for underflow

remainingQty -= filledQty with a bug in the matching logic won't throw,
it'll wrap to something near 2^64 and quietly eat every order that
touches that level afterward. Fix is an assert(filledQty <= remainingQty)
once the matching loop exists, plus a fuzz check that remainingQty never
exceeds submittedQty for any order. Not switching to signed just to
dodge this -- the guard is enough.

## FIFO comes from list position, not a timestamp

No timestamp field on Order. Position in the list is the tiebreak.
Simpler -- no clock, no tie-break logic if two timestamps collide -- and
it means matching is fully deterministic given the same sequence of
calls, which is what the tests and fuzzer actually need. If v2's
sequencer needs a real sequence number later, that's for ordering
across threads. Different problem, not a replacement for this.

## Two lookup maps instead of one, for O(1) cancel

Cancel now stores the map iterator itself (not just price), so erasing
doesn't need to re-search the tree -- erase(iterator) on std::map is
O(1) amortized, so cancel is O(1) amortized now, not O(log P).

Bids and asks are different map types (different Compare), so their
iterators are different types too. Could unify with std::variant, but
that's a lot of machinery (visit, if constexpr) for something I'd
rather keep readable -- two separate lookup maps (bidLookup,
askLookup) with a small templated insert/cancel is simpler to read and
just as fast.

## Cancel needs O(1) lookup, not a scan

insert/cancel started with a linear scan across both sides to find an
order by id -- fine for tests, wrong for anything real. Added an
unordered_map<OrderId, Location> (side + price + list iterator) that
insert populates and cancel erases. list iterators stay valid when
other elements are erased, so the map can hold them directly as long
as the map entry is cleaned up in the same call that erases from the
list.

match() will need the same lookup once fills start touching resting
orders by id, so this had to happen before matching logic, not after.
