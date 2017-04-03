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

#include "ProcessValue.h"

ProcessValue::ProcessValue() {}

ProcessValue::~ProcessValue() {}

double ProcessValue::getValue() const {
#ifndef DRY_RUN
    read();
#endif
    return _v;
}

#ifdef DRY_RUN
void ProcessValue::calculate(double start_temp, double ambient_temp, chrono::milliseconds t) {
    // TODO calculate pv
    _v = 100;
}
#endif

#ifndef DRY_RUN
void ProcessValue::read() const {
    std::lock_guard<std::mutex> lock(_m);
    //TODO Fetch data from hw
}
#endif
