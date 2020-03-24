#include "Flat.hpp"

Flat::Flat()
{
    //ctor
}

Flat::~Flat()
{
    //dtor
}


void Flat::PrimLimitedStates(Grid& grid, const Array<double, 2>& prim, Array<double, 2>& prim_l, Array<double, 2>& prim_r, Axis ax)
{
    prim_l = prim(Range(grid.GetInnerBeg(ax), grid.GetInnerEnd(ax)), Range::all());
    prim_r = prim(Range(grid.GetInnerBeg(ax)+1, grid.GetInnerEnd(ax)+1), Range::all());
    //std::cout << prim << std::endl;
    //std::cout << prim_l << std::endl;
    //std::cout << prim_r << std::endl;
}
