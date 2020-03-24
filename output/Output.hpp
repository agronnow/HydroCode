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
        Output(std::size_t n, double d, std::size_t nd, std::vector<std::pair<std::string, std::size_t>> v) :
        	nout(n), dt(d), ndim(nd), timeOfLastOutput(0), outputvars(v) {};
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
        std::size_t nout;
        const double dt;
        const std::size_t ndim;
        double timeOfLastOutput;
        const std::vector<std::pair<std::string, std::size_t>> outputvars;
    private:
};

#endif // OUTPUT_H
