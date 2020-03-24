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
			for (std::size_t i = 0; i < grid.GetNGhost(Axis_x1); i++)
			{
				consvars(i, Range::all(), Range::all(), Range::all()) =
						consvars(consvars.extent(0)-grid.GetNGhost(Axis_x1)-1, Range::all(), Range::all(), Range::all());
			}
			break;
		}
		case Side::x1e:
		{
			for (std::size_t i = consvars.extent(0)-grid.GetNGhost(Axis_x1); i < consvars.extent(0); i++)
			{
				consvars(i, Range::all(), Range::all(), Range::all()) =
						consvars(grid.GetNGhost(Axis_x1), Range::all(), Range::all(), Range::all());
			}
			break;
		}
		case Side::x2b:
		{
			for (std::size_t i = 0; i < grid.GetInnerBeg(Axis_x2); i++)
			{
				consvars(Range::all(), i, Range::all(), Range::all()) =
						consvars(Range::all(), grid.GetInnerEnd(Axis_x2), Range::all(), Range::all());
			}
			break;
		}
		case Side::x2e:
		{
			for (std::size_t i = grid.GetInnerEnd(Axis_x2)+1; i < consvars.extent(0); i++)
			{
				consvars(Range::all(), i, Range::all(), Range::all()) =
						consvars(Range::all(), grid.GetInnerBeg(Axis_x2), Range::all(), Range::all());
			}
			break;
		}
		case Side::x3b:
		{
			for (std::size_t i = 0; i < grid.GetNGhost(Axis_x3); i++)
			{
				consvars(Range::all(), Range::all(), i, Range::all()) =
						consvars(Range::all(), Range::all(), consvars.extent(2)-grid.GetNGhost(Axis_x3)-1, Range::all());
			}
			break;
		}
		case Side::x3e:
		{
			for (std::size_t i = consvars.extent(0)-grid.GetNGhost(Axis_x3); i < consvars.extent(0); i++)
			{
				consvars(Range::all(), Range::all(), i, Range::all()) =
						consvars(Range::all(), Range::all(), grid.GetNGhost(Axis_x3), Range::all());
			}
			break;
		}
	}
}
