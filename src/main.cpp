#include "order_book.hpp"
#include <iostream>

int main() {
    OrderBook book;
    book.insert(Order(1, Side::Buy, 10000, 5));
    book.insert(Order(2, Side::Buy, 10000, 3));
    std::cout << "scaffold OK, bid levels=" << book.bids.size()
              << ", resting qty at 10000=" << book.bids[10000].totalQty
              << "\n";
    return 0;
}
