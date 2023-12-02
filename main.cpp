#include <base.h>


int main() {
    typedef std::chrono::high_resolution_clock Clock;
    auto t1 = Clock::now();

    OrderBookEngine * engine = OrderBookEngine::get_engine();
    std::vector<Order> orders = FileLoader::load_order_file("../data/SCH.log");
    for (int i=0; i<50000; i++) {
        engine->add_data(orders[i]);
    }

    auto t2 = Clock::now();
    auto t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    printf("time cost:\n");
    printf("load_data time cost: %ld ms \n", t);
    t1 = Clock::now();

    std::vector<std::string> fields {
        "epoch","symbol", "ask1p", "bid1p", "ask1q", "bid1q",
        "ask2p", "bid2p", "ask2q", "bid2q",
        "last trade price", "last trade quantity"
    };

    std::string result = engine->get_snapshot("SCH", 0, 2909722950019872114, fields);

    t2 = Clock::now();
    t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    printf("get_snapshot time cost: %ld ms \n", t);
    int count  = 0;
    for (char i: result) {count+=i=='\n';}
    printf("result length: %d\n", count);

    OrderBookEngine::release_engine(engine);

    // printf("%s", result.c_str());
    return 0;
}