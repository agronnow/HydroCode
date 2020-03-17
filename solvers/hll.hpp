#ifndef HLL_H
#define HLL_H

#include <Solver.hpp>


class hll : public Solver
{
    public:
        hll() {};
        virtual ~hll();
        double Riemann(State1D&, State1D&, Axis, std::unique_ptr<EOS>&) override final;
    protected:
    private:
};

#endif // HLL_H
