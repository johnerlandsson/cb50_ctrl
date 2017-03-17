#include "Parameters.h"
#include <fstream>
#include <stdexcept>

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
    std::ifstream f{"./cb50_ctrl_params.json"};
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