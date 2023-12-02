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


class OrderBookEngine {
protected:
    OrderBookEngine() {}
public:

    static OrderBookEngine * get_engine();

    static void release_engine(OrderBookEngine *);

    virtual void add_data(const Order& order, bool block=true) {}

    virtual void add_data(Order && order, bool block=true) {}

    virtual std::string get_snapshot(std::string symbol, __uint64_t start, __uint64_t end,
                                const std::vector<std::string>& fields) {return "";}

    virtual std::unordered_map<std::string, std::string> get_snapshots(const std::vector<std::string>& symbols,
                 __uint64_t start, __uint64_t end, const std::vector<std::string>& fields) {return {};}

    virtual void load_data(std::string_view file_name) {}
};


#endif