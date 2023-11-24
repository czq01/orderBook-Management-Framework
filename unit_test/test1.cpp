# include <base.hpp>
# include <chrono>


static int idx = 0;
static int test_len = 1000000; // 10^6
template <typename T, typename ...Args>
void run(T func, Args... args) {
    idx++;
    typedef std::chrono::high_resolution_clock Clock;
    auto t1 = Clock::now();
    if (func(args...)) {printf("test %d passed. ", idx);}
    else {printf("test %d failed. ", idx);}
    auto t2 = Clock::now();//计时结束
    __int64_t t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    printf("time cost: %ld ms\n", t);
}


bool test_DiskSeachArray() {
    DiskSearchArray<int, 5> arr;
    for (int i=0; i<test_len; i++) {
        arr.add_data(i);
    }
    for (int i=1; i<test_len; i*=2) {
        int idx = arr.get_idx(i);
        if (idx != i/2) return 1;
        // printf("%d got: %d\n",i, idx);
    }
    return 0;
}

bool test_DiskArray() {
    DiskArray<Order> fm("TSLA");
    for (int i=0; i<test_len; i+=2) {
        Order order;
        order.epoch = i;
        order.price = i+1.0;
        order.quantity = i%5;
        order.side = 'B';
        std::strcpy(order.symbol, "TSLA");
        order.category = 'N';
        order.order_id = i+2;
        fm.insertRecord(order);
    }

    for (int i=1; i<test_len; i*=2) {
        Order order;
        fm.fetchRecord(static_cast<void*>(&order), i);
        if ((int)order.epoch==i && order.quantity==i%5) continue;
        else return true;
    }
    return false;
}




int main() {
    Engine engine;
    // engine.load(file)
    // engine.load(piece of order data)
    // engine.query(time, time)

    auto all_test = {test_DiskSeachArray, test_DiskArray};

    for (auto test: all_test) {
        run(test);
    }

}