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

#ifndef PROCESSVALUEH
#define PROCESSVALUEH

#ifdef DRY_RUN
#include <chrono>
#endif
#include <mutex>

class ProcessValue {
   public:
    ProcessValue();
    virtual ~ProcessValue();
    double getValue() const;
#ifdef DRY_RUN
    void calculate(double start_temp, double ambient_temp,
                   chrono::milliseconds t);
#endif

   private:
#ifndef DRY_RUN
    void read() const;
#endif
    double _v;
    mutable std::mutex _m;
};

#endif
