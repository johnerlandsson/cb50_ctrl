/* 
 * This file is part of the cb50_ctrl distribution (https://github.com/johnerlandsson/cb50_ctrl).
 * Copyright (c) 2017 John Erlandsson
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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
