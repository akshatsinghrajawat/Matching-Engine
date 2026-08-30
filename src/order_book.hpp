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
};
