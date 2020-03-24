#include <iostream>
#include "Simulation.hpp"

#ifndef UNIT_TESTING
int main()
{
	INI::File settings;
	std::ifstream inifile("settings.ini",std::ios::in);
	if (inifile.is_open()) inifile >> settings;
	else
	{
        std::ostringstream err;
        err << "SetParams: File settings.ini not found";
		throw std::runtime_error(err.str());
	}
	inifile.close();
    Simulation Sim("test", settings);
    Sim.MainLoop();
    return 0;
}

#else
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Grid")
{
	INI::File settings;
	std::ifstream inifile("settings.ini",std::ios::in);
	if (inifile.is_open()) inifile >> settings;
	else
	{
		std::ostringstream err;
		err << "SetParams: File settings.ini not found";
		throw std::runtime_error(err.str());
	}
	inifile.close();

	Grid grid(settings);

	double data[] = {2.0, 3.0, 2.5};
	Array<double, 2> cons(data, shape(1, 3), neverDeleteData);
	SECTION("conservative array initialization")
	{
		REQUIRE(cons(0,(int)consvars::cdens) == Approx(2.0));
		REQUIRE(cons(0,(int)consvars::cmom_x1) == Approx(3.0));
		REQUIRE(cons(0,this->grid.cenergy()) == Approx(2.5));
	}
	Array<double, 2> prim(1, 3);
	Array<double, 2> flux(1, 3);
	Range MomRange((int)consvars::cmom_x1, int(grid.GetNDim()+consvars::cmom_x1-1));
	Range VelRange((int)primvars::pvx1, int(grid.GetNDim()+primvars::pvx1-1));
	double gamma = 5.0/3.0;
	auto ideal_eos(Ideal(gamma, MomRange, VelRange));
	ideal_eos.ConsToPrim(prim, cons);
	SECTION("conservative to primitive state conversion")
	{
		REQUIRE(prim(0,(int)primvars::pdens) == Approx(2.0));
		REQUIRE(prim(0,(int)primvars::pvx1) == Approx(1.5));
		REQUIRE(prim(0,(int)primvars::pprs) == Approx((gamma-1.0)*0.25));
	}
	SECTION("conservative flux calculation")
	{
		State1D state(cons, prim, flux);
		grid.CalcFlux(state, Axis_x1);
		REQUIRE(flux(0,(int)consvars::cdens) == Approx(3.0));
		REQUIRE(flux(0,(int)consvars::cmom_x1) == Approx(4.666667));
		REQUIRE(flux(0,this->grid.cenergy()) == Approx(4.0));
	}
	SECTION("boundaries")
	{
		Array<double, 2> ray = grid.GetConst1DArray(Axis_x1, 0, 0, meshextents::full);
		ray((int)grid.GetNGhost(Axis_x1), (int)consvars::cdens) = 1.0;
		ray((int)grid.GetNGhost(Axis_x1), (int)consvars::cmom_x1) = 2.0;
		ray((int)grid.GetNGhost(Axis_x1), this->grid.cenergy()) = 3.0;
		ray(int(ray.extent(0)-grid.GetNGhost(Axis_x1)-1), (int)consvars::cdens) = 4.0;
		ray(int(ray.extent(0)-grid.GetNGhost(Axis_x1)-1), (int)consvars::cmom_x1) = 5.0;
		ray(int(ray.extent(0)-grid.GetNGhost(Axis_x1)-1), this->grid.cenergy()) = 6.0;
		SECTION("outflow")
		{
			std::unique_ptr<Boundary> BoundaryL(std::move(CreateBoundary("outflow", Side::x1b)));
			BoundaryL->ApplyBoundary(grid);
			for (int i = 0; i < grid.GetNGhost(Axis_x1); i++)
			{
				REQUIRE(ray(i,(int)consvars::cdens) == 1.0);
				REQUIRE(ray(i,(int)consvars::cmom_x1) == 2.0);
				REQUIRE(ray(i,this->grid.cenergy()) == 3.0);
			}
			std::unique_ptr<Boundary> BoundaryR(std::move(CreateBoundary("outflow", Side::x1e)));
			BoundaryR->ApplyBoundary(grid);
			for (int i = ray.extent(0) - grid.GetNGhost(Axis_x1); i < ray.extent(0); i++)
			{
				REQUIRE(ray(i,(int)consvars::cdens) == 4.0);
				REQUIRE(ray(i,(int)consvars::cmom_x1) == 5.0);
				REQUIRE(ray(i,this->grid.cenergy()) == 6.0);
			}
		}
		SECTION("user")
		{
			std::unique_ptr<Boundary> BoundaryL(std::move(CreateBoundary("user", Side::x1b)));
			BoundaryL->ApplyBoundary(grid);
			for (int i = 0; i < grid.GetNGhost(Axis_x1); i++)
			{
				REQUIRE(ray(i,(int)consvars::cdens) == 1.0);
				REQUIRE(ray(i,(int)consvars::cmom_x1) == 2.0);
				REQUIRE(ray(i,this->grid.cenergy()) == 3.0);
			}
			std::unique_ptr<Boundary> BoundaryR(std::move(CreateBoundary("user", Side::x1e)));
			BoundaryR->ApplyBoundary(grid);
			for (int i = ray.extent(0) - grid.GetNGhost(Axis_x1); i < ray.extent(0); i++)
			{
				REQUIRE(ray(i,(int)consvars::cdens) == 4.0);
				REQUIRE(ray(i,(int)consvars::cmom_x1) == 5.0);
				REQUIRE(ray(i,this->grid.cenergy()) == 6.0);
			}
		}
		SECTION("periodic")
		{
			std::unique_ptr<Boundary> BoundaryL(std::move(CreateBoundary("periodic", Side::x1b)));
			BoundaryL->ApplyBoundary(grid);
			for (int i = 0; i < grid.GetNGhost(Axis_x1); i++)
			{
				REQUIRE(ray(i,(int)consvars::cdens) == 4.0);
				REQUIRE(ray(i,(int)consvars::cmom_x1) == 5.0);
				REQUIRE(ray(i,this->grid.cenergy()) == 6.0);
			}
			std::unique_ptr<Boundary> BoundaryR(std::move(CreateBoundary("periodic", Side::x1e)));
			BoundaryR->ApplyBoundary(grid);
			for (int i = ray.extent(0) - grid.GetNGhost(Axis_x1); i < ray.extent(0); i++)
			{
				REQUIRE(ray(i,(int)consvars::cdens) == 1.0);
				REQUIRE(ray(i,(int)consvars::cmom_x1) == 2.0);
				REQUIRE(ray(i,this->grid.cenergy()) == 3.0);
			}
		}
	}
}

#endif
