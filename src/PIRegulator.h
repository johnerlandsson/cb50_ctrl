#ifndef PIREGULATOR_H
#define PIREGULATOR_H

#include "Parameters.h"

class PIRegulator {
   public:
    PIRegulator();
    ~PIRegulator();
    float regulate(double sv, double pv);

    typedef struct {
        double kp;
        double ki;
        double max_istate;
        double min_istate;
        double min_output;
        double max_output;
    } parameters_t;

   private:
    parameters_t _p;
    double _istate;
};

#endif
