#include "../boundary/Periodic.hpp"

Periodic::~Periodic()
{
    //dtor
}

void Periodic::ApplyBoundary(Grid& grid)
{
    Array<double, 4> consvars = grid.GetConservedVars();
	switch (this->side)
	{
		case Side::x1b:
		{
			consvars(0, Range::all(), Range::all(), Range::all()) = consvars(consvars.extent(0)-1, Range::all(), Range::all(), Range::all());
			break;
		}
		case Side::x1e:
		{
			consvars(consvars.extent(0)-1, Range::all(), Range::all(), Range::all()) = consvars(0, Range::all(), Range::all(), Range::all());
			break;
		}
		case Side::x2b:
		{
			consvars(Range::all(), 0, Range::all(), Range::all()) = consvars(Range::all(), consvars.extent(1)-1, Range::all(), Range::all());
			break;
		}
		case Side::x2e:
		{
			consvars(Range::all(), consvars.extent(1)-1, Range::all(), Range::all()) = consvars(Range::all(), 0, Range::all(), Range::all());
			break;
		}
		case Side::x3b:
		{
			consvars(Range::all(), Range::all(), 0, Range::all()) = consvars(Range::all(), Range::all(), consvars.extent(2)-1, Range::all());
			break;
		}
		case Side::x3e:
		{
			consvars(Range::all(), Range::all(), consvars.extent(2)-1, Range::all()) = consvars(Range::all(), Range::all(), 0, Range::all());
			break;
		}
	}
}
