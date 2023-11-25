#ifndef __CZQ01_ORDER_BASE__
#define __CZQ01_ORDER_BASE__
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <list>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <thread>
#include <queue>
#include <mutex>

#include "utils/structures.hpp"

#include "utils/storage.hpp"

#include "utils/dataloader.hpp"

struct _OrderVolumeNode{
    u_int64_t order_id;
    int volume;
};

struct _PriceArrayNode {
    float price;
    int volume;
    std::list<_OrderVolumeNode> records;
};

class BookBuildProcessor{
    DiskBookManager m_storage;
    std::list<_PriceArrayNode> m_ask;
    std::list<_PriceArrayNode> m_bid;
    char symbol[8] {0};
    float m_last_price;
    int m_last_volume;
    __uint64_t m_last_epoch;


    // generate orderbook and store in disk;
    void _update_order_book() {
        OrderBook book;
        char count = '1';
        auto ask_iter = m_ask.cbegin();
        // update ask prices and quantity
        std::string name = "ask__";
        while (count<='5' && ask_iter != m_ask.cend()) {
            // set ask prices
            name[3] = count;
            name[4] = 'p';
            UnionType p = bookMemberPosition.at(name);
            book.*(p.flV) = ask_iter->price;

            // set ask quantity
            name[4] = 'q';
            UnionType q = bookMemberPosition.at(name);
            book.*(q.intV) = ask_iter->volume;
            count++; ask_iter++;
        }

        // update ask prices and quantity
        auto bid_iter = m_bid.cend();
        name = "bid__";
        count = '1';
        while (count<='5' && bid_iter != m_bid.cbegin()) {
            bid_iter--;
            // set ask prices
            name[3] = count;
            name[4] = 'p';
            UnionType p = bookMemberPosition.at(name);
            book.*(p.flV) = bid_iter->price;

            // set ask quantity
            name[4] = 'q';
            UnionType q = bookMemberPosition.at(name);
            book.*(q.intV) = bid_iter->volume;
            count++;
        }

        // update other info
        book.epoch = m_last_epoch;
        book.last_price = m_last_price;
        book.last_qunatity = m_last_volume;
        std::strcpy(book.symbol, this->symbol);

        // store order book;
        m_storage.add_new_data(book);
    }

    void _add_new_order(const Order& order) {
        std::list<_PriceArrayNode> * ls;
        switch(order.side) {
            case 'B': ls = &m_bid; break;
            case 'S': ls = &m_ask; break;
        }
        auto i = ls->begin();
        while (i != ls->end() && i->price<order.price) {i++;}
        if (i==ls->end() || i->price>order.price) {
            _PriceArrayNode p;
            p.price = order.price;
            p.volume = order.quantity;
            p.records.emplace_back(_OrderVolumeNode{order.order_id, order.quantity});
            ls->insert(i, p);
        } else {
            i->volume += order.quantity;
            i->records.emplace_back(_OrderVolumeNode{order.order_id, order.quantity});
        }
    }

    void _trade_order(const Order& order) {
        std::list<_PriceArrayNode> * ls;
        switch(order.side) {
            case 'B': ls = &m_bid; break;
            case 'S': ls = &m_ask; break;
        }
        auto i = ls->begin();
        while (i != ls->end() && i->price<order.price) {i++;}
        if (i->price != order.price) {
            printf("WARNING: trade order failed. No price order record.\n");
        }
        auto j = i->records.begin();
        while (j != i->records.end() && j->order_id != order.order_id) {j++;}
        if (j == i->records.end()) {
            printf("WARNING: trade order failed. No order id record.\n");
        };
        i->volume -= order.quantity;
        if (j->volume == order.quantity) {
            i->records.erase(j);
        } else {
            j->volume -= order.quantity;
        }
        if (!i->records.size()) {ls->erase(i);}
        m_last_price = order.price;
        m_last_volume = order.quantity;
    }

    void _cancel_order(const Order& order) {
        std::list<_PriceArrayNode> * ls;
        switch(order.side) {
            case 'B': ls = &m_bid; break;
            case 'S': ls = &m_ask; break;
        }
        auto i = ls->begin();
        while (i != ls->end() && i->price<order.price) {i++;}
        if (i->price != order.price) {
            printf("WARNING: trade order failed. No order on price recorded.\n");}

        auto j = i->records.begin();
        while (j != i->records.end() && j->order_id != order.order_id) {j++;}

        if (j == i->records.end()) {
            printf("WARNING: trade order failed. No order id recorded.\n");};
        i->volume -= j->volume;
        i->records.erase(j);
        if (!i->records.size()) {ls->erase(i);}
    }

    // for orders whose category not in 'N'new 'C'cancel 'T'trade
    void _not_implemented_error() {
        printf("Error: order category not support.\n");
    }

    // generate csv line based on give field.
    std::string get_field_of_order_book(const OrderBook& book, const std::string& field) {
        UnionType p = bookMemberPosition.at(field);
        switch(p._type) {
            case 0: return std::to_string(book.*(p.int64V));
            case 1: return std::to_string(book.*(p.intV));
            case 2: return std::to_string(book.*(p.flV));
            case 3: return std::string(book.*(p.strV));
            default: throw std::invalid_argument(
                        "[get_field_of_order_book] Unsupported _type appeared");
        }
    }


public:
    BookBuildProcessor(const char * symbol):
        m_storage(symbol), m_last_price(0), m_last_volume(0) {
        std::strcpy(this->symbol, symbol);
    }

    // processing and update previous orders
    void processing_order(const Order& order) {
        // TODO can't handle unknown trade with id=0 currently
        if (order.side == 'U') return;
        switch (order.category) {
            case 'N': _add_new_order(order); break;
            case 'C': _cancel_order(order); break;
            case 'T': _trade_order(order); break;
            default: _not_implemented_error(); break;
        }
        m_last_epoch = order.epoch;
        _update_order_book();
    }

    std::string get_snapshot(std::string_view symbol, __uint64_t start, __uint64_t end,
                                    const std::vector<std::string>& fields) {
        const std::vector<OrderBook> books = m_storage.get_snapshot(start, end);
        std::string result;
        result.reserve(10*fields.size()*books.size());
        for (const std::string& field: fields) {
            if (!bookMemberPosition.count(field)) {return "Error: Got Unexpected field - " + field;}
            result.append(field);
            result.push_back(',');
        }
        result.pop_back();
        result.push_back('\n');
        for (auto book: books) {
            for (const std::string& field: fields) {
                result.append(get_field_of_order_book(book, field));
                result.push_back(',');
            }
            result.pop_back();
            result.push_back('\n');
        }
        return result;
    }

};

class Engine {
    std::unordered_map<std::string, BookBuildProcessor> m_proc;
    std::queue<Order> m_data_queue;  // queue of new data to be processed
    std::thread m_th_data;  // thread for processsing new data
    std::mutex m_lock;
    bool signal;

    void th_data_processing() {
        while (signal) {
            if (m_data_queue.empty()) {sleep(1); continue;}
            Order& p = m_data_queue.front();
            m_proc.at(p.symbol).processing_order(p);
            m_lock.lock();
            m_data_queue.pop();
            m_lock.unlock();
        }
    }

public:
    Engine(): m_th_data(&Engine::th_data_processing, this), signal(true) {
        m_th_data.detach();
    }

    Engine(Engine &&) = delete;

    Engine(const Engine&) = delete;

    void add_data(const Order& order, bool block=true) {
        if (!m_proc.count(order.symbol)) m_proc.emplace(order.symbol, order.symbol);
        if (block) {
            m_proc.at(order.symbol).processing_order(order);
        } else {
            std::unique_lock<std::mutex> ul(m_lock);
            m_data_queue.push(order);
        }
    }

    void add_data(Order && order, bool block=true) {
        if (!m_proc.count(order.symbol)) m_proc.emplace(order.symbol, order.symbol);
        std::unique_lock<std::mutex> ul(m_lock);
        if (block) m_proc.at(order.symbol).processing_order(order);
        else m_data_queue.push(std::forward<Order>(order));
    }

    // get orderbook snapshot between time start and end (inclusive) for one symbol
    std::string get_snapshot(std::string symbol, __uint64_t start, __uint64_t end,
                                const std::vector<std::string>& fields) {
        return m_proc.at(symbol).get_snapshot(symbol, start, end, fields);
    }

    std::unordered_map<std::string, std::string> get_snapshots(const std::vector<std::string>& symbols, __uint64_t start,
                                __uint64_t end, const std::vector<std::string>& fields) {
        std::unordered_map<std::string, std::string> result;
        for (const std::string& symbol: symbols) {
            if (!m_proc.count(symbol)) {continue;}
            result.emplace(symbol, get_snapshot(symbol, start, end, fields));
        }
        return result;
    }


    void load_data(std::string_view file_name) {
    auto data = FileLoader::load_order_file("../data/SCH.log");
    for (const Order& order: data) {
        add_data(order);
    }
    }


    ~Engine() {signal = false;}

};


#endif