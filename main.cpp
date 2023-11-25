#include <base.hpp>


int main() {
    typedef std::chrono::high_resolution_clock Clock;
    auto t1 = Clock::now();

    Engine engine;
    std::vector<Order> orders = FileLoader::load_order_file("../data/SCH.log");
    for (int i=0; i<10000; i++) {
        engine.add_data(orders[i]);
    }

    auto t2 = Clock::now();
    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    printf("time cost:\n");
    printf("load_data time cost: %ld ms \n", t);
    t1 = Clock::now();

    std::vector<std::string> fields {
        "epoch","symbol", "ask1p", "bid1p", "ask1q", "bid1q", "last trade price", "last trade quantity"
    };

    std::string result = engine.get_snapshot("SCH", 1609722944228022079, 1609722944233544470, fields);

    t2 = Clock::now();
    t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    printf("get_snapshot time cost: %ld ms \n\n", t);
    printf("result:\n");
    printf("%s", result.c_str());
    return 0;
}