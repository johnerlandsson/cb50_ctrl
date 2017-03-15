#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <crow.h>
#include <mutex>
#include <string>

class Parameters {
   public:
    Parameters();
    virtual ~Parameters();
    crow::json::wvalue to_json() const;
    void from_json(const crow::json::wvalue& p);
    void load_file();

   private:
    std::mutex _m;
};

#endif
