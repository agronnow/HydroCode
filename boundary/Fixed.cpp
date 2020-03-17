#include "../boundary/Fixed.hpp"

Fixed::~Fixed()
{
    //dtor
}

void Fixed::ApplyBoundary(Grid& grid)
{
	Array<double, 4> consvars = grid.GetConservedVars();
	switch (this->side)
	{
		case Side::x1b:
		{
			for (int i = 0; i < grid.GetNGhost(Axis::x1); i++)
			{
				consvars(i, Range::all(), Range::all(), Range::all()) =
				 consvars(grid.GetNGhost(Axis::x1), Range::all(), Range::all(), Range::all());
			}
			break;
		}
		case Side::x1e:
		{
			for (int i = consvars.extent(0)-grid.GetNGhost(Axis::x1); i < consvars.extent(0); i++)
			{
				consvars(i, Range::all(), Range::all(), Range::all()) =
				 consvars(consvars.extent(0)-grid.GetNGhost(Axis::x1)-1, Range::all(), Range::all(), Range::all());
			}
			break;
		}
		case Side::x2b:
		{
			for (int i = 0; i < grid.GetNGhost(Axis::x2); i++)
			{
				consvars(Range::all(), i, Range::all(), Range::all()) =
				 consvars(Range::all(), grid.GetNGhost(Axis::x2), Range::all(), Range::all());
			}
			break;
		}
		case Side::x2e:
		{
			for (int i = consvars.extent(1)-grid.GetNGhost(Axis::x2); i < consvars.extent(1); i++)
			{
				consvars(Range::all(), i, Range::all(), Range::all()) =
				 consvars(Range::all(), consvars.extent(1)-grid.GetNGhost(Axis::x2)-1, Range::all(), Range::all());
			}
			break;
		}
		case Side::x3b:
		{
			for (int i = 0; i < grid.GetNGhost(Axis::x3); i++)
			{
				consvars(Range::all(), Range::all(), i, Range::all()) =
				 consvars(Range::all(), Range::all(), grid.GetNGhost(Axis::x3), Range::all());
			}
			break;
		}
		case Side::x3e:
		{
			for (int i = consvars.extent(2)-grid.GetNGhost(Axis::x3); i < consvars.extent(2); i++)
			{
				consvars(Range::all(), Range::all(), i, Range::all()) =
				 consvars(Range::all(), Range::all(), consvars.extent(2)-grid.GetNGhost(Axis::x3)-1, Range::all());
			}
			break;
		}
	}
}
