# include <base.h>
# include <chrono>


static int idx = 0;
static int test_len = 1002; // 10^6
template <typename T, typename ...Args>
void run(T func, Args... args) {
    idx++;
    typedef std::chrono::high_resolution_clock Clock;
    auto t1 = Clock::now();
    if (!func(args...)) {printf("test %d passed. ", idx);}
    else {printf("test %d failed. ", idx);}
    auto t2 = Clock::now();//计时结束
    __int64_t t = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    printf("time cost: %ld ms on N=%d\n", t, test_len);
}

bool test_DiskSeachArray() {
    DiskSearchArray<int, 7> arr;
    for (int i=0; i<=test_len; i+=2) {
        arr.add_data(i);
    }
    for (int i=0; i<=test_len+100; i++) {
        int idx = arr.get_idx(i);
        if (i<=test_len && idx != i/2)return 1;
        if (idx>test_len && idx != test_len/2)return 1;
    }
    return 0;
}

bool test_DiskArray() {
    DiskSearchArray<int, 7> arr;
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
        arr.add_data(i);
    }

    for (int i=4; i<test_len; i*=3) {
        int idx = arr.get_idx(i);
        Order order;
        fm.fetchRecord(&order, idx);
        if ((int)order.epoch==2*idx && order.quantity==(2*idx)%5);
        else
            return true;
        i/=2;
    }
    return false;
}

bool test_Engine() {
    OrderBookEngine * engine = OrderBookEngine::get_engine();
    for (int i=0; i<test_len; i++) {
        Order order;
        order.epoch = i;
        order.price = i%20+1.5;
        order.quantity = i%10 +5;
        order.side = 'B';
        std::strcpy(order.symbol, i%2?"TSLA": "SCSA");
        order.category = 'N';
        order.order_id = i+2;
        engine->add_data(std::move(order));
    }

    sleep(1);
    //verify:
    double max_val = 0;
    for (int i=0; i<test_len+100; i++) {
        if (i<20) max_val = (i)%20+1.5;
        else max_val = 20.5 - !(i%2);
        std::string_view symbol = (i%2?"TSLA":"SCSA");
        auto nested_result = engine->get_snapshots({"TSLA","SCSA"}, i, i, {"symbol", "ask1q", "bid1p", "bid1q"});
        std::string_view tmp = nested_result.at(std::string(symbol));
        int j =  tmp.find('\n', 1); j++;
        if (tmp.substr(j, 4)!= symbol)
            return 1;
        j+=5;
        int k = tmp.find(',',j);
        if (tmp.substr(j, k-j) != "0")
            return 1;
        j = k+1; k = tmp.find(',', j);
        double r = std::atof(tmp.substr(j, k-j).data());
        if (r != max_val)
            return 1;
    }
    OrderBookEngine::release_engine(engine);
    return false;






    return false;
}


int main() {

    auto all_test = {
        test_DiskSeachArray, test_DiskArray, test_Engine
    };

    std::vector<int> test_size {50, 100, 1000};
    for (int i: test_size) {
        printf("----------> new test round:\n");
        test_len = i;
        idx = 0;
        for (auto test: all_test) {
            run(test);
        }
    }
    return 0;
}