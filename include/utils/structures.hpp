#ifndef __CZQ01_ORDER_STRUCTURE__
#define __CZQ01_ORDER_STRUCTURE__
#include "../base.h"

struct Order {
    __uint64_t epoch;    // unix timestamps in ns
    __uint64_t order_id; // order_id (not unique)
    char symbol[8];     // symbol for trade
    char side;          // 'B' for buy, 'S' for sell
    char category;      // 'N' new, 'C' cancell, 'M' modify, ... (etc.)
    float price;        // order price
    int quantity;       // order volume
};


struct OrderBook {
    __uint64_t epoch;    // unix timestamps
    char symbol[8]{0};    // symbol of the order book
    int last_price;
    int last_qunatity;
    float ask1p, ask2p, ask3p, ask4p, ask5p;
    float bid1p, bid2p, bid3p, bid4p, bid5p;
    int ask1q, ask2q, ask3q, ask4q, ask5q;
    int bid1q, bid2q, bid3q, bid4q, bid5q;

    OrderBook():
         epoch(0),last_price(0),last_qunatity(0) ,
         ask1p(0.0f), ask2p(0.0f), ask3p(0.0f), ask4p(0.0f), ask5p(0.0f),
         bid1p(0.0f), bid2p(0.0f), bid3p(0.0f), bid4p(0.0f), bid5p(0.0f),
         ask1q(0), ask2q(0), ask3q(0), ask4q(0), ask5q(0),
         bid1q(0), bid2q(0), bid3q(0), bid4q(0), bid5q(0) {}
};


// UnionType for mapping use.
template <typename T>
struct UnionType {
    union {
        __uint64_t T::* int64V;
        int T::* intV;
        float T::* flV;
        char (T::* strV) [8];
    };
    const int _type;

    UnionType(__uint64_t T::* val): int64V(val), _type(0) {}
    UnionType(int T::*val): intV(val), _type(1) {}
    UnionType(float T::*val): flV(val), _type(2) {}
    UnionType(char (T::*val) [8]): strV(val), _type(3) {}
};


// field mapping for easier fetching selected fields.
const std::unordered_map<std::string, UnionType<OrderBook>> bookMemberPosition {
    {"epoch", &OrderBook::epoch},{"symbol", &OrderBook::symbol},
    {"last trade price", &OrderBook::last_price}, {"last trade quantity", &OrderBook::last_qunatity},
    {"ask1p", &OrderBook::ask1p}, {"ask2p", &OrderBook::ask2p}, {"ask3p", &OrderBook::ask3p},
    {"ask4p", &OrderBook::ask4p}, {"ask5p", &OrderBook::ask4p}, {"ask1q", &OrderBook::ask1q},
    {"ask2q", &OrderBook::ask2q}, {"ask3q", &OrderBook::ask3q}, {"ask4q", &OrderBook::ask4q},
    {"ask5q", &OrderBook::ask5q}, {"bid1p", &OrderBook::bid1p}, {"bid2p", &OrderBook::bid2p},
    {"bid3p", &OrderBook::bid3p}, {"bid4p", &OrderBook::bid4p}, {"bid5p", &OrderBook::bid5p},
    {"bid1q", &OrderBook::bid1q}, {"bid2q", &OrderBook::bid2q}, {"bid3q", &OrderBook::bid3q},
    {"bid4q", &OrderBook::bid4q}, {"bid5q", &OrderBook::bid5q}
};


#endif