#ifndef MUSCL_H
#define MUSCL_H

#include <Integrator.hpp>


class Muscl : public Integrator
{
    public:
        Muscl();
        virtual ~Muscl();
        double AdvanceSoln(std::unique_ptr<Solver>&, std::unique_ptr<EOS>&, Grid&, Array<double, 2>&, State1D&, State1D&, double, Axis) override final;
    protected:
    private:
};

#endif // MUSCL_H
