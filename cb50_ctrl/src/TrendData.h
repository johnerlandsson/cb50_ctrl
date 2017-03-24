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
