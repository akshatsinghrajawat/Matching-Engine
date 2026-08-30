#pragma once

#include <cstdint>

// Prices are integer ticks, not floating point.
//
// Floating point prices are the classic matching-engine bug: 0.1 + 0.2 !=
// 0.3 in IEEE 754, and a book that compares float prices for equality or
// crossing can silently misbehave. Real exchanges quote and match in
// integer tick units (e.g. cents, or exchange-defined tick size) for
// exactly this reason. Converting to/from a human-readable decimal price
// is a display-layer concern, not a book-internals concern -- it happens
// at the I/O boundary, never inside the matching logic itself.
using Price = std::int64_t;

// Order quantity. Unsigned: a negative quantity is never meaningful.
// 64-bit to avoid overflow when summing quantity across many orders at
// a single price level (PriceLevel::totalQty).
using Quantity = std::uint64_t;

// Unique identifier for an order, assigned by the engine (or the
// simulator, in v0) at submission time. 64-bit gives effectively
// unlimited headroom for any benchmark scale we'll actually run.
using OrderId = std::uint64_t;

enum class Side : std::uint8_t {
    Buy,
    Sell,
};
