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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <crow.h>
#include <mutex>
#include <string>
#include <json.hpp>
#include "PIRegulator.h"

class Parameters {
   public:
    Parameters();
    virtual ~Parameters();
    crow::json::wvalue to_wvalue() const;
    bool from_str(const std::string& p);
    bool from_json(const nlohmann::json& p);
    bool from_wvalue(const crow::json::wvalue& p);
    bool load_file();
    PIRegulator::parameters_t getRegulatorParameters() const;

   private:
    mutable std::mutex _m;
    PIRegulator::parameters_t _rp;
};

#endif
