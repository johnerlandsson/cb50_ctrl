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

#ifndef TRENDDATA_H
#define TRENDDATA_H
#include <crow/json.h>
#include <mutex>

class TrendData {
   public:
    TrendData();
    virtual ~TrendData();
    void append(double sv, double pv, double output);
    crow::json::wvalue getData() const;

   private:
    size_t _sd;
    crow::json::wvalue _d;
    mutable std::mutex _m;
};

#endif
