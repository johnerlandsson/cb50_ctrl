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
