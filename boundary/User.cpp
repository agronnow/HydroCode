#include "../boundary/User.hpp"

User::~User()
{
    //dtor
}

void User::ApplyBoundary(Grid& grid)
{
    Array<double, 4> consvars = grid.GetConservedVars();
	switch (this->side)
	{
		case Side::x1b:
		{
			for (std::size_t i = 0; i < grid.GetNGhost(Axis_x1); i++)
			{
				//Set boundary condition for consvars at -x boundary
			}
			break;
		}
		case Side::x1e:
		{
			for (std::size_t i = consvars.extent(0)-grid.GetNGhost(Axis_x1); i < consvars.extent(0); i++)
			{
				//Set boundary condition for consvars at +x boundary
			}
			break;
		}
		case Side::x2b:
		{
			for (std::size_t i = 0; i < grid.GetNGhost(Axis_x2); i++)
			{
				//Set boundary condition for consvars at -y boundary
			}
			break;
		}
		case Side::x2e:
		{
			for (std::size_t i = consvars.extent(0)-grid.GetNGhost(Axis_x1); i < consvars.extent(0); i++)
			{
				//Set boundary condition for consvars at +y boundary
			}
			break;
		}
		case Side::x3b:
		{
			for (std::size_t i = 0; i < grid.GetNGhost(Axis_x3); i++)
			{
				//Set boundary condition for consvars at -z boundary
			}
			break;
		}
		case Side::x3e:
		{
			for (std::size_t i = consvars.extent(0)-grid.GetNGhost(Axis_x3); i < consvars.extent(0); i++)
			{
				//Set boundary condition for consvars at +z boundary
			}
			break;
		}
	}
}
