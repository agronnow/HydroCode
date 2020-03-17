#ifndef OUTPUT_H
#define OUTPUT_H
#include <auxillary.hpp>
#include <vector>
#include <fstream>
#include <Grid.hpp>

struct StepInfo
{
    int nstep;
    double t;
    double dt;
};

class Output
{
    public:
        Output(int n, double d, std::vector<std::pair<std::string, int>> v) : nout(n), dt(d), timeOfLastOutput(0), outputvars(v) {};
        virtual ~Output();
        virtual void OutputSnapshot(Grid&, StepInfo&) = 0;
        virtual std::string GetFormat() = 0;
        bool ShouldOutput(double time)
        {
            if (time - this->timeOfLastOutput >= this->dt)
            {
                this->timeOfLastOutput = time;
                return true;
            }
            return false;
        }
        void SetNout(int n) {this->nout = n;}
    protected:
        int nout;
        double dt;
        double timeOfLastOutput;
        std::vector<std::pair<std::string, int>> outputvars;
    private:
};

#endif // OUTPUT_H
