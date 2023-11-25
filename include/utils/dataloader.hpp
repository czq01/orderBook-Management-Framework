#ifndef __CZQ01_DATALOADER__
#define __CZQ01_DATALOADER__
#include "../base.hpp"
// #include <iostream>
#include <sstream>
#include <fstream>

class FileLoader {
    static Order load_order_str(std::string_view record) {
        Order order;
        std::string tmp;
        std::istringstream ss(record.data());
        // epoch, order id, symbol, order side, order category, price, quantity
        ss >> order.epoch;
        ss >> order.order_id;
        ss >> order.symbol;
        ss >> tmp; order.side = tmp[0];
        ss >> tmp; order.category = tmp[0];
        ss >> order.price;
        ss >> order.quantity;
        return order;
    }

public:
    static const std::vector<Order> load_order_file(std::string_view s) {
        std::vector<Order> result;
        std::ifstream file;
        file.open(s.data());
        char szbuff[255] = {0};
        while(!file.eof())  {
        	file.getline(szbuff, 255);
            result.emplace_back(load_order_str(szbuff));
        }
        return result;
    }
};

#endif