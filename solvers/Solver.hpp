#ifndef SOLVER_H
#define SOLVER_H

#include <eos.hpp>

class Solver
{
    public:
        Solver() {};
        virtual ~Solver();
        virtual double Riemann(State1D&, State1D&, Axis, std::unique_ptr<EOS>&) = 0;
    protected:
    private:
};

#endif // SOLVER_H
