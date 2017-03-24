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

#include "Parameters.h"
#include <fstream>
#include <stdexcept>

#ifdef NDEBUG
#define PARAMETER_FILE "/etc/cb50_ctrl_params.json"
#else
#define PARAMETER_FILE "./cb50_ctrl_params.json"
#endif

Parameters::Parameters() {}
Parameters::~Parameters() {}

crow::json::wvalue Parameters::to_wvalue() const {
    std::lock_guard<std::mutex> lock(_m);
    crow::json::wvalue ret;

    ret["regulator"]["kp"] = _rp.kp;
    ret["regulator"]["ki"] = _rp.ki;
    ret["regulator"]["min_istate"] = _rp.min_istate;
    ret["regulator"]["max_istate"] = _rp.max_istate;
    ret["regulator"]["min_output"] = _rp.min_output;
    ret["regulator"]["max_output"] = _rp.max_output;

    return ret;
}

bool Parameters::from_json(const nlohmann::json& p) {
    std::lock_guard<std::mutex> lock(_m);
    _rp.ki = (double)p["regulator"]["ki"];
    _rp.kp = (double)p["regulator"]["kp"];
    _rp.min_istate = (double)p["regulator"]["min_istate"];
    _rp.max_istate = (double)p["regulator"]["max_istate"];
    _rp.min_output = (double)p["regulator"]["min_output"];
    _rp.max_output = (double)p["regulator"]["max_output"];

    std::ofstream f;
    try {
        f.open(PARAMETER_FILE, std::ofstream::trunc | std::ofstream::out);
        f << p.dump(2);
        f.close();
    } catch (std::ios_base::failure&) {
        return false;
    }

    return true;
}

bool Parameters::from_str(const std::string& p) {
    return from_json(nlohmann::json::parse(p));
}

bool Parameters::from_wvalue(const crow::json::wvalue& p) {
    return from_str(crow::json::dump(p));
}

bool Parameters::load_file() {
    bool ret = false;
    std::ifstream f{PARAMETER_FILE};
    if (f.is_open()) {
        ret = from_json(nlohmann::json::parse(f));
        f.close();
    }

    return ret;
}

PIRegulator::parameters_t Parameters::getRegulatorParameters() const {
    std::lock_guard<std::mutex> lock(_m);
    return _rp;
}
