#include "TrendData.h"
#include <chrono>
#include <sstream>

#include "date.h"

TrendData::TrendData() : _sd{0} {}
TrendData::~TrendData() {}

void TrendData::append(double sv, double pv, double output) {
    using namespace date;
    std::stringstream ss;
    ss << std::chrono::system_clock::now();

    std::lock_guard<std::mutex> lock(_m);
    //_d["labels"][_sd] = std::ctime(&now);
    _d["labels"][_sd] = ss.str();
    _d["data"]["sv"][_sd] = sv;
    _d["data"]["pv"][_sd] = pv;
    _d["data"]["output"][_sd] = output;
    _sd++;
}

crow::json::wvalue TrendData::getData() const {
    std::lock_guard<std::mutex> lock(_m);
    return crow::json::load(crow::json::dump(_d));
}
