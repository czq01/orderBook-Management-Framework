#ifndef __CZQ01_ORDER_STRUCTURE__
#define __CZQ01_ORDER_STRUCTURE__
#include "../base.hpp"


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
public:
    __uint64_t epoch;    // unix timestamps
    char symbol[8]{0};    // symbol of the order book
    int last_price;
    int last_qunatity;
    float ask1p, ask2p, ask3p, ask4p, ask5p;
    float bid1p, bid2p, bid3p, bid4p, bid5p;
    int ask1q, ask2q, ask3q, ask4q, ask5q;
    int bid1q, bid2q, bid3q, bid4q, bid5q;
    int top_order_quantity[5][6]{0};

    OrderBook():
         epoch(0),last_price(0),last_qunatity(0) ,
         ask1p(0.0f), ask2p(0.0f), ask3p(0.0f), ask4p(0.0f), ask5p(0.0f),
         bid1p(0.0f), bid2p(0.0f), bid3p(0.0f), bid4p(0.0f), bid5p(0.0f),
         ask1q(0), ask2q(0), ask3q(0), ask4q(0), ask5q(0),
         bid1q(0), bid2q(0), bid3q(0), bid4q(0), bid5q(0) {}

};

#endif