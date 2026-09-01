#pragma once

#include "order.hpp"

#include <list>
#include <map>
#include <unordered_map>

struct PriceLevel {
    std::list<Order> orders;
    Quantity totalQty = 0;
};

class OrderBook {
public:
    std::map<Price, PriceLevel, std::greater<Price>> bids;
    std::map<Price, PriceLevel, std::less<Price>> asks;

    void insert(const Order& order) {
        bool isBuy = (order.side == Side::Buy);
        PriceLevel& level = isBuy ? bids[order.price] : asks[order.price];
        level.orders.push_back(order);
        auto it = std::prev(level.orders.end());
        level.totalQty += order.remainingQty;
        locations_[order.id] = Location{order.side, order.price, it};
    }

    // O(1) lookup via locations_, replacing the old linear scan.
    bool cancel(OrderId id) {
        auto locIt = locations_.find(id);
        if (locIt == locations_.end()) {
            return false;
        }
        const Location& loc = locIt->second;
        if (loc.side == Side::Buy) {
            cancelAt(bids, loc.price, loc.it);
        } else {
            cancelAt(asks, loc.price, loc.it);
        }
        locations_.erase(locIt);
        return true;
    }

private:
    struct Location {
        Side side;
        Price price;
        std::list<Order>::iterator it;
    };

    std::unordered_map<OrderId, Location> locations_;

    template <typename BookSide>
    void cancelAt(BookSide& side, Price price, std::list<Order>::iterator orderIt) {
        auto levelIt = side.find(price);
        PriceLevel& level = levelIt->second;
        level.totalQty -= orderIt->remainingQty;
        level.orders.erase(orderIt);
        if (level.orders.empty()) {
            side.erase(levelIt);
        }
    }
};
