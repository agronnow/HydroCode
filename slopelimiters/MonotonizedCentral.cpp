#include "MonotonizedCentral.hpp"

MonotonizedCentral::MonotonizedCentral()
{
    //ctor
}

MonotonizedCentral::~MonotonizedCentral()
{
    //dtor
}


void MonotonizedCentral::PrimLimitedStates(Grid& grid, const Array<double, 2>& prim, Array<double, 2>& prim_l, Array<double, 2>& prim_r, Axis ax)
{
	Array<double, 2> dp(grid.GetInnerExtent(ax)+1, grid.GetNumVars());
	Array<double, 2> dm(grid.GetInnerExtent(ax)+1, grid.GetNumVars());
	Array<double, 2> dlim(grid.GetInnerExtent(ax)+1, grid.GetNumVars());
	dp = prim(Range(grid.GetInnerBeg(ax), grid.GetInnerEnd(ax)+1), Range::all()) -
			prim(Range(grid.GetInnerBeg(ax)-1, grid.GetInnerEnd(ax)), Range::all());
	dm = prim(Range(grid.GetInnerBeg(ax)-1, grid.GetInnerEnd(ax)), Range::all()) -
			prim(Range(grid.GetInnerBeg(ax)-2, grid.GetInnerEnd(ax)-1), Range::all());
    dlim = where(dp*dm > 0.0, absmin(0.5*(dp + dm), 2.0*absmin(dm, dp)), 0.0);
    prim_l = prim(Range(grid.GetInnerBeg(ax), grid.GetInnerEnd(ax)+1), Range::all()) - dlim*0.5;
    prim_r = prim(Range(grid.GetInnerBeg(ax)-1, grid.GetInnerEnd(ax)), Range::all()) + dlim*0.5;
}
