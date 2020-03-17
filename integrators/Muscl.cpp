#include "Muscl.hpp"

Muscl::Muscl()
{
    //ctor
}

Muscl::~Muscl()
{
    //dtor
}

double Muscl::AdvanceSoln(std::unique_ptr<Solver>& solver, std::unique_ptr<EOS>& eos, Grid& grid, Array<double, 2>& cons,
		State1D& state_l, State1D& state_r, double dt, Axis ax)
{
    double dtdx = dt/grid.GetSpacing(ax);
    state_l.cons -= 0.5*dtdx*(state_l.flux - state_r.flux);
	state_r.cons += 0.5*dtdx*(state_l.flux - state_r.flux);
    eos->ConsToPrim(state_l.prim, state_l.cons);
    eos->ConsToPrim(state_r.prim, state_r.cons);
    grid.CalcFlux(state_l, ax);
    grid.CalcFlux(state_r, ax);
    double curmaxalpha = solver->Riemann(state_l, state_r, ax, eos);
    cons -= dtdx*(state_l.flux(Range(1,toEnd), Range::all()) - state_l.flux(Range(1,toEnd)-1, Range::all()));
    if (std::isnan(sum(cons)))
    {
        std::ostringstream err;
        err << "Muscl: NaN in conserved variables array";// at index " << i;
        throw std::runtime_error(err.str());
    }
    return curmaxalpha;
}
