#include <iostream>
#include "Simulation.hpp"

#define UNIT_TESTING

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
		REQUIRE(cons(0,cenergy()) == Approx(2.5));
	}
	Array<double, 2> prim(1, 3);
	Array<double, 2> flux(1, 3);
	Range MomRange((int)consvars::cmom_x1, int(g_NDim+consvars::cmom_x1-1));
	Range VelRange((int)primvars::pvx1, int(g_NDim+primvars::pvx1-1));
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
		grid.CalcFlux(state, Axis::x1);
		REQUIRE(flux(0,(int)consvars::cdens) == Approx(3.0));
		REQUIRE(flux(0,(int)consvars::cmom_x1) == Approx(4.666667));
		REQUIRE(flux(0,cenergy()) == Approx(4.0));
	}
}

#endif
