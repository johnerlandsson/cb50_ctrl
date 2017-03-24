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
