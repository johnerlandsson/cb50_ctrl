#ifndef PIREGULATOR_H
#define PIREGULATOR_H

class PIRegulator {
   public:
    typedef struct {
        double kp;
        double ki;
        double max_istate;
        double min_istate;
        double min_output;
        double max_output;
    } parameters_t;

    PIRegulator();
    PIRegulator(const parameters_t p);
    ~PIRegulator();
    float regulate(double sv, double pv);

   private:
    parameters_t _p;
    double _istate;
};

#endif
