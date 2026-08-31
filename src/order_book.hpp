#pragma once

#include "order.hpp"

#include <list>
#include <map>
#include <unordered_map>
#include <utility>

struct PriceLevel {
    std::list<Order> orders;
    Quantity totalQty = 0;
};

class OrderBook {
public:
    std::map<Price, PriceLevel, std::greater<Price>> bids;
    std::map<Price, PriceLevel, std::less<Price>> asks;

    void insert(const Order& order) {
        if (order.side == Side::Buy) {
            insertInto(bids, bidLookup, order);
        } else {
            insertInto(asks, askLookup, order);
        }
    }

    // O(1) amortized: the lookup gives us the map iterator directly,
    // no re-searching the tree.
    bool cancel(OrderId id) {
        return cancelFrom(bids, bidLookup, id) || cancelFrom(asks, askLookup, id);
    }

private:
    using BidLevels = std::map<Price, PriceLevel, std::greater<Price>>;
    using AskLevels = std::map<Price, PriceLevel, std::less<Price>>;
    using OrderIt = std::list<Order>::iterator;

    std::unordered_map<OrderId, std::pair<BidLevels::iterator, OrderIt>> bidLookup;
    std::unordered_map<OrderId, std::pair<AskLevels::iterator, OrderIt>> askLookup;

    template <typename Levels, typename Lookup>
    void insertInto(Levels& levels, Lookup& lookup, const Order& order) {
        auto levelIt = levels.try_emplace(order.price).first;
        levelIt->second.orders.push_back(order);
        levelIt->second.totalQty += order.remainingQty;
        lookup[order.id] = {levelIt, std::prev(levelIt->second.orders.end())};
    }

    template <typename Levels, typename Lookup>
    bool cancelFrom(Levels& levels, Lookup& lookup, OrderId id) {
        auto found = lookup.find(id);
        if (found == lookup.end()) return false;
        auto [levelIt, orderIt] = found->second;
        PriceLevel& level = levelIt->second;
        level.totalQty -= orderIt->remainingQty;
        level.orders.erase(orderIt);
        if (level.orders.empty()) {
            levels.erase(levelIt);
        }
        lookup.erase(found);
        return true;
    }
};
