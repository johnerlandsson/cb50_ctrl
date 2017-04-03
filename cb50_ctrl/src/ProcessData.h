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
