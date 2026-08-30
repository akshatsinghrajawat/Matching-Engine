#include "src/order_book.hpp"

#include <iostream>

static int failures = 0;

void check(bool condition, const char* what) {
    if (!condition) {
        std::cerr << "FAILED: " << what << "\n";
        failures++;
    }
}

void insertPutsOrderInRightLevel() {
    OrderBook book;
    book.insert(Order(1, Side::Buy, 100, 10));
    check(book.bids.count(100) == 1, "bid level at 100 exists after insert");
    check(book.bids[100].orders.size() == 1, "one order in the level");
    check(book.bids[100].totalQty == 10, "totalQty matches inserted qty");
    check(book.asks.empty(), "asks untouched by a buy insert");
}

void fifoOrderPreservedOnInsert() {
    OrderBook book;
    book.insert(Order(1, Side::Buy, 100, 10));
    book.insert(Order(2, Side::Buy, 100, 5));
    auto& orders = book.bids[100].orders;
    check(orders.front().id == 1, "first order in stays first");
    check(orders.back().id == 2, "second order in stays second");
}

void cancelRemovesOrderAndUpdatesTotal() {
    OrderBook book;
    book.insert(Order(1, Side::Buy, 100, 10));
    book.insert(Order(2, Side::Buy, 100, 5));
    bool cancelled = book.cancel(1);
    check(cancelled, "cancel reports success for an id that exists");
    check(book.bids[100].orders.size() == 1, "one order left after cancel");
    check(book.bids[100].totalQty == 5, "totalQty drops by the cancelled qty");
    check(book.bids[100].orders.front().id == 2, "the remaining order is #2");
}

void cancelOnlyOrderRemovesTheLevel() {
    OrderBook book;
    book.insert(Order(1, Side::Buy, 100, 10));
    book.cancel(1);
    check(book.bids.count(100) == 0, "empty level disappears from the map");
}

void cancelMiddleOrderPreservesFifoForTheRest() {
    OrderBook book;
    book.insert(Order(1, Side::Buy, 100, 10));
    book.insert(Order(2, Side::Buy, 100, 5));
    book.insert(Order(3, Side::Buy, 100, 7));
    book.cancel(2);
    auto& orders = book.bids[100].orders;
    check(orders.size() == 2, "two orders left");
    check(orders.front().id == 1, "#1 still first");
    check(orders.back().id == 3, "#3 still second, #2 is just gone");
}

void cancelUnknownIdReturnsFalse() {
    OrderBook book;
    book.insert(Order(1, Side::Buy, 100, 10));
    check(!book.cancel(999), "cancelling a nonexistent id returns false");
    check(book.bids[100].orders.size() == 1, "and doesn't touch the book");
}

void bidsAndAsksAreOrderedTowardTheSpread() {
    OrderBook book;
    book.insert(Order(1, Side::Buy, 100, 10));
    book.insert(Order(2, Side::Buy, 105, 10));
    book.insert(Order(3, Side::Sell, 110, 10));
    book.insert(Order(4, Side::Sell, 108, 10));
    check(book.bids.begin()->first == 105, "best bid is the highest price");
    check(book.asks.begin()->first == 108, "best ask is the lowest price");
}

int main() {
    insertPutsOrderInRightLevel();
    fifoOrderPreservedOnInsert();
    cancelRemovesOrderAndUpdatesTotal();
    cancelOnlyOrderRemovesTheLevel();
    cancelMiddleOrderPreservesFifoForTheRest();
    cancelUnknownIdReturnsFalse();
    bidsAndAsksAreOrderedTowardTheSpread();

    if (failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }
    std::cerr << failures << " test(s) failed.\n";
    return 1;
}
