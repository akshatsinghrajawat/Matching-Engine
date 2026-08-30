#pragma once

#include "types.hpp"

// A single resting or incoming limit order.
//
// v0 scope only: no time-in-force, no order type beyond plain limit --
// those are v1 additions (see DECISIONS.md and the project roadmap).
// Keeping this struct minimal now means every field added later is a
// deliberate, documented decision, not something inherited by accident
// from a template.
struct Order {
    OrderId id;
    Side side;
    Price price;

    // Original submitted quantity. Kept alongside remainingQty so that
    // fill amount (submitted - remaining) is always recoverable for
    // quantity-reconciliation checks (submitted == filled + resting),
    // which the v0 invariant tests and fuzzer both rely on.
    Quantity submittedQty;
    Quantity remainingQty;

    Order(OrderId id_, Side side_, Price price_, Quantity qty_)
        : id(id_), side(side_), price(price_), submittedQty(qty_),
          remainingQty(qty_) {}
};
