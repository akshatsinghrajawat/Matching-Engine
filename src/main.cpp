#include "order_book.hpp"
#include <iostream>

int main() {
    OrderBook book;
    Order sample(1, Side::Buy, 10000, 5);
    std::cout << "scaffold OK, order id=" << sample.id
              << ", bids=" << book.bids.size()
              << " asks=" << book.asks.size() << "\n";
    return 0;
}
