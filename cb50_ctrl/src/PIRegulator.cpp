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

