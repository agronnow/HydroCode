#include "Euler.hpp"

Euler::Euler()
{
    //ctor
}

Euler::~Euler()
{
    //dtor
}

double Euler::AdvanceSoln(std::unique_ptr<Solver>& solver, std::unique_ptr<EOS>& eos, Grid& grid, Array<double, 2>& cons,
		State1D& state_l, State1D& state_r, double dt, Axis ax)
{
    double curmaxalpha = solver->Riemann(state_l, state_r, ax, eos);
    double dtdx = dt/grid.GetSpacing(ax);
    cons -= dtdx*(state_l.flux(Range(0,state_l.flux.extent(0)-2), Range::all()) - state_l.flux(Range(1,state_l.flux.extent(0)-1), Range::all()));
    std::cout << cons << std::endl;
    if (std::isnan(sum(cons)))
    {
        std::ostringstream err;
        err << "Euler: NaN in conserved variables array";// at index " << i;
        throw std::runtime_error(err.str());
    }
    return curmaxalpha;
}
