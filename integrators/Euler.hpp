#ifndef EULER_H
#define EULER_H

#include <Integrator.hpp>


class Euler : public Integrator
{
    public:
        Euler();
        virtual ~Euler();
        double AdvanceSoln(std::unique_ptr<Solver>&, std::unique_ptr<EOS>&, Grid&, Array<double, 2>&, State1D&, State1D&, double, Axis) override final;
    protected:
    private:
};

#endif // EULER_H
