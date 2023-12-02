# OrderBook System

Name: OrderBook System
Author: czq01

project dependency: `g++>=9`, `Cmake>=3.9.0`

## Structure Design

We have three part of component:
- Main Engine
- Book Processor
- Disk Data Manager

### Main Engine

`Main Engine` responsible for interprete queries and execution, as well as new data loading.

it handles two main requests:
- adding new orders.  by calling `add_data(Order, block=true)` function. Only in online case, we use `block=false` to that real time order will be append to system in a non-block way. I use a relatively slow implementation to maintain accuracy. It process about 2500 records in a second.
- grab snapshots.  by calling `get_snapshot()` function. We can grab pre-stored data during the timeframe.

### Book Processor

`Book Processor` provides apis for new data processing and query data fetching. All functions from `Engine` are submitting tasks to it, and it submit/fetch data using Disk Structure. It is the core part of the framwork.

the main function in this part:
- `get_snapshot(Args ...args)` responsible for query all the snapshots, formatting using fields provided, and form a long csv string contains multiple lines;
- `processsing_order(order)` responsible for new order processing and generate a new snapshot to store in disk.

Each Book Processor only responsible for one symbol. So I maintain an unordered_map in `Engine` to store and fetch data. Since the orderbook are seperate for each symbol, I believe this design will work well.

### Disk Data Manager

`DiskManager` is responsible for Data storage on the disk. It store data in several seperate file. Contains `DiskSearchArray` and `DiskArray` structure. Can easily query data by epoch.

`DiskSearchArray` is use to store epoch and do binary search. get store index of the epoch and OrderBook. I design this structure inorder to faster find OrderBook index at a specific epoch.

`DiskArray` use to store OrderBooks on disk. All OrderBook objects are store in FILE, and can be fetch directly by index given (can be get from `DiskSearchArray`).

## How to use

### Compile & Run

- In this repo, compile using Cmake with g++>=9. There will be two executives: test1(for test), order_book(demo app).

```bash
cd ${project_base_folder}
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cd ..
# run test using:
cd bin/
./test1
# run framework using:
cd bin/
./order_book_test
```

- Using in other project: just compile the this project, simply copy the `/include` and `/lib` folder to your own project. Remember to add compile option `-std=c++17` or above.


### API usage

just as code appear in file [main.cpp](main.cpp), we can easily use like this:

```cpp
OrderBookEngine *engine = OrderBookEngine::get_engine();
//load data
engine->load_file(file_name);
//add new order
Order order;
...
// add to engine
engine->add_data(order);
// get snapshot
string sr = engine->get_snapeshot(args...);
//release the engine
OrderBookEngine::release_engine(engine);
```

use `load_file`(for file) and `add_data`(for Order) API on the main engine to load and preprocess new order data.

use `get_snapshot` API on the main engine to fetch snapshot.


