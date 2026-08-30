#pragma once

#include "order.hpp"

#include <list>
#include <map>

struct PriceLevel {
    std::list<Order> orders;
    Quantity totalQty = 0;
};

class OrderBook {
public:
    std::map<Price, PriceLevel, std::greater<Price>> bids;
    std::map<Price, PriceLevel, std::less<Price>> asks;

    void insert(const Order& order) {
        PriceLevel& level = (order.side == Side::Buy)
            ? bids[order.price]
            : asks[order.price];
        level.orders.push_back(order);
        level.totalQty += order.remainingQty;
    }

    // linear scan for now, O(1) lookup is the next commit
    bool cancel(OrderId id) {
        return cancelFrom(bids, id) || cancelFrom(asks, id);
    }

private:
    template <typename BookSide>
    bool cancelFrom(BookSide& side, OrderId id) {
        for (auto levelIt = side.begin(); levelIt != side.end(); ++levelIt) {
            PriceLevel& level = levelIt->second;
            for (auto orderIt = level.orders.begin();
                 orderIt != level.orders.end(); ++orderIt) {
                if (orderIt->id == id) {
                    level.totalQty -= orderIt->remainingQty;
                    level.orders.erase(orderIt);
                    if (level.orders.empty()) {
                        side.erase(levelIt);
                    }
                    return true;
                }
            }
        }
        return false;
    }
};
