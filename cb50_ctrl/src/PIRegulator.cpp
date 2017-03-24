#include "PIRegulator.h"
#include <cstring>

PIRegulator::PIRegulator() {
    memset(&_p, 0, sizeof(parameters_t));
}

PIRegulator::PIRegulator(const parameters_t p) : _p(p) {}

PIRegulator::~PIRegulator() {}

float PIRegulator::regulate(double sv, double pv) {
    auto err = sv - pv;

    // Calculate proportional term
    auto pterm = err * _p.kp;

    // Accumulate error
    _istate += err;
    if (_istate > _p.max_istate)
        _istate = _p.max_istate;
    else if (_istate < _p.min_istate)
        _istate = _p.min_istate;

    // Calculate integral term
    auto iterm = _istate * _p.ki;

    // Calculate output
    double ret = pterm + iterm;
    if (ret > _p.max_output)
        return _p.max_output;
    else if (ret < _p.min_output)
        return _p.min_output;

    return ret;
}

