#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <Solver.hpp>
#include <Grid.hpp>


class Integrator
{
    public:
        Integrator();
        virtual ~Integrator();
        virtual double AdvanceSoln(std::unique_ptr<Solver>&, std::unique_ptr<EOS>&, Grid&, Array<double, 2>&, State1D&, State1D&, double, Axis) = 0;
    protected:
    private:
};

#endif // INTEGRATOR_H
