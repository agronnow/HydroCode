#include "Minmod.hpp"

Minmod::Minmod()
{
    //ctor
}

Minmod::~Minmod()
{
    //dtor
}


void Minmod::PrimLimitedStates(Grid& grid, const Array<double, 2>& prim, Array<double, 2>& prim_l, Array<double, 2>& prim_r, Axis ax)
{
	Array<double, 2> dp(grid.GetInnerExtent(ax), grid.GetNumVars());
	Array<double, 2> dm(grid.GetInnerExtent(ax), grid.GetNumVars());
	Array<double, 2> dlim(grid.GetInnerExtent(ax), grid.GetNumVars());
	dp = prim(Range(grid.GetInnerBeg(ax)+1, grid.GetInnerEnd(ax)+1), Range::all()) -
			prim(Range(grid.GetInnerBeg(ax), grid.GetInnerEnd(ax)), Range::all());
	dm = prim(Range(grid.GetInnerBeg(ax), grid.GetInnerEnd(ax)), Range::all()) -
			prim(Range(grid.GetInnerBeg(ax)-1, grid.GetInnerEnd(ax)-1), Range::all());
    dlim = where(dp*dm > 0.0, absmin(dp, dm), 0.0);
    prim_l = prim(Range(grid.GetInnerBeg(ax), grid.GetInnerEnd(ax)+1), Range::all()) - dlim*0.5;
    prim_r = prim(Range(grid.GetInnerBeg(ax)-1, grid.GetInnerEnd(ax)), Range::all()) + dlim*0.5;
}
