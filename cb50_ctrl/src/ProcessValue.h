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
