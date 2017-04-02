#ifndef PROCESSDATAH
#define PROCESSDATAH

#include <mutex>
#include <crow/json.h>

class ProcessData {
    public:
        ProcessData();
        virtual ~ProcessData();
        double getSv() const;
        double getPv() const;
        double getOutput() const;
        bool getPump() const;
        bool getMixer() const;
        bool getRunRecipe() const;
        crow::json::wvalue toWvalue() const;
        void setSv(const double v);
        void setPv(const double v);
        void setOutput(const double v);
        void setPump(const bool v);
        void setMixer(const bool v);
        void setRunRecipe(const bool v);

    private:
        double _sv;
        double _pv;
        double _output;
        bool _pump;
        bool _mixer;
        bool _run_recipe;
        mutable std::mutex _m;

};

#endif
