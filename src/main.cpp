#include "order.hpp"

#include <iostream>

// Placeholder entry point. No book, no matching yet -- this exists so
// the scaffold compiles and CI has something to build against. Real
// logic starts with the skeleton book (next commit).
int main() {
    Order sample(/*id=*/1, Side::Buy, /*price=*/10000, /*qty=*/5);
    std::cout << "matching-engine scaffold OK, sample order id="
              << sample.id << "\n";
    return 0;
}
