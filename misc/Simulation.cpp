#include "Simulation.hpp"

Simulation::Simulation(const std::string& n, INI::File& s) : name(n), t(0), grid(s)
{
    this->SetParams(s);
    this->ApplyInitialConditions();
}

Simulation::~Simulation()
{
    //dtor
}

void Simulation::SetParams(INI::File& settings)
{
	this->SetBoundaries(settings);
	this->SetTimestep(settings);
	this->SetOutput(settings);
	this->SetPhysics(settings);
	this->SetSchemes(settings);
}

void Simulation::SetBoundaries(INI::File& settings)
{
	std::map<Side, std::string> sides;
	switch(this->grid.GetNDim())
	{
	case 1:
		sides = {{Side::x1b, "xb"}, {Side::x1e, "xe"}};
		break;
	case 2:
		sides = {{Side::x1b, "xb"}, {Side::x1e, "xe"}, {Side::x2b, "yb"}, {Side::x2e, "ye"}};
		break;
	default:
		sides = {{Side::x1b, "xb"}, {Side::x1e, "xe"}, {Side::x2b, "yb"}, {Side::x2e, "ye"}, {Side::x3b, "zb"}, {Side::x3e, "ze"}};
		break;
	}

	std::map<std::string, std::vector<Side>> BoundarySides;
	auto boundarytypes = settings.GetSection("domain")->GetValue("boundaries").AsMap();
	for (auto& side : sides) this->boundaries.insert(std::pair<Side, std::unique_ptr<Boundary>>
			(side.first,std::move(CreateBoundary(boundarytypes[side.second].AsString(), side.first))));
}

void Simulation::SetTimestep(INI::File& settings)
{
    const double dt_init = settings.GetSection("timestep")->GetValue("dt_init").AsDouble();
	if (dt_init <= 0.0)
	{
        std::ostringstream err;
        err << "SetTimestep: dt_init must be a positive number!";
		throw std::runtime_error(err.str());
	}
    const double t_end = settings.GetSection("timestep")->GetValue("t_end").AsDouble();
	if (t_end <= 0.0)
	{
        std::ostringstream err;
        err << "SetTimestep: t_end must be a positive number!";
		throw std::runtime_error(err.str());
	}
    const double C = settings.GetSection("timestep")->GetValue("courant").AsDouble();
	if (C <= 0.0 || C >= 1.0)
	{
        std::ostringstream err;
        err << "SetTimestep: courant must be between 0 and 1!";
		throw std::runtime_error(err.str());
	}
	this->dt = dt_init;
	this->tstop = t_end;
	this->courant = C;
}

void Simulation::SetOutput(INI::File& settings)
{
    std::vector<std::pair<std::string, std::size_t>> vars = {{"rho", 0}, {"prs", 1}, {"vx1", 2}};
	if (this->grid.GetNDim() > 1) vars.emplace_back("vx2", 3);
	if (this->grid.GetNDim() == 3)
	{
		vars.emplace_back("vx2", 3);
		vars.emplace_back("vx3", 4);
	}
	const int nout = 0; //TODO set accordingly if restarting
	double dtout = settings.GetSection("output")->GetSubSection("dbl")->GetValue("dt_output",-1.0).AsDouble();
    std::vector<std::unique_ptr<Output>> output;
    if (dtout > 0.0) outputs.push_back(std::move(std::make_unique<DBL>(DBL(nout, dtout, this->grid.GetNDim(), vars))));
	dtout = settings.GetSection("output")->GetSubSection("tab")->GetValue("dt_output",-1.0).AsDouble();
    if (dtout > 0.0) outputs.push_back(std::move(std::make_unique<Tab>(Tab(nout, dtout, this->grid.GetNDim(), vars))));
	dtout = settings.GetSection("output")->GetSubSection("png")->GetValue("dt_output",-1.0).AsDouble();
    if (dtout > 0.0) outputs.push_back(std::move(std::make_unique<PNG>(PNG(nout, dtout, this->grid.GetNDim(), vars, Axis_x3, true, true, "cubehelix.cmap", true))));
    this->outputs = std::move(output);
}

void Simulation::SetPhysics(INI::File& settings)
{
	const std::string eos = settings.GetSection("physics")->GetValue("eos").AsString();
	if (eos == "ideal")
	{
	    const double gamma = settings.GetSection("physics")->GetValue("gamma").AsDouble();
		if (gamma <= 0.0)
		{
	        std::ostringstream err;
	        err << "SetPhysics: gamma must be a positive number!";
			throw std::runtime_error(err.str());
		}
	    this->eos = std::make_unique<Ideal>(gamma, this->grid.GetMomRange(), this->grid.GetVelRange(), this->grid.GetNDim());
	}
}

void Simulation::SetSchemes(INI::File& settings)
{
	const std::string riemann = settings.GetSection("schemes")->GetValue("riemann").AsString();
	if (riemann == "hll") this->solver = std::make_unique<hll>();
	const std::string integrator = settings.GetSection("schemes")->GetValue("integrator").AsString();
	if (integrator == "euler") this->integrator = std::make_unique<Euler>();
	else this->integrator = std::make_unique<Muscl>();
	const std::string slopelim = settings.GetSection("schemes")->GetValue("slopelimiter").AsString();
	if (slopelim == "flat") this->slopelimiter = std::make_unique<Flat>();
	else if (slopelim == "minmod") this->slopelimiter = std::make_unique<Minmod>();
	else this->slopelimiter = std::make_unique<MonotonizedCentral>();
}

void Simulation::MainLoop()
{
    auto walltime_start = chrono::steady_clock::now();
    StepInfo info;
    info.nstep = 0;
    while (this->t < this->tstop)
    {
        this->t += this->dt;
        std::cout << "Time: " << this->t << std::endl;
        this->dt = this->Step();
        for (auto& output : outputs)
        {
            if (output->ShouldOutput(this->t))
            {
                info.dt = this->dt;
                info.t = this->t;
                output->OutputSnapshot(this->grid, info);
                std::cout << output->GetFormat() << " output written." << std::endl;
            }
        }
        info.nstep++;
    }
    auto walltime_end = chrono::steady_clock::now();
    auto diff = walltime_end - walltime_start;
    std::cout << "Done" << std::endl << "Wall time used: " << chrono::duration <double, milli> (diff).count() << " ms" << std::endl;
}

double Simulation::Step()
{
    for (auto const& boundary : this->boundaries) boundary.second->ApplyBoundary(this->grid);
    this->eos->ConsToPrim(this->grid.GetPrimitiveVars(), this->grid.GetConservedVars());
    Axis ax1;
    Axis ax2;
    double maxdt = 1.e20;
    try
    {
        for (std::size_t ax = Axis_x1 ; ax <= this->grid.GetNDim()-1 ; ax++)
        {
            if (ax == Axis_x1)
            {
                ax1 = Axis_x2;
                ax2 = Axis_x3;
            }
            else if (ax == Axis_x2)
            {
                ax1 = Axis_x1;
                ax2 = Axis_x3;
            }
            else
            {
                ax1 = Axis_x1;
                ax2 = Axis_x2;
            }
            Array<double, 2> prim_l(grid.GetInnerExtent(ax)+1, grid.GetNumVars());
            Array<double, 2> prim_r(grid.GetInnerExtent(ax)+1, grid.GetNumVars());
            Array<double, 2> cons_l(grid.GetInnerExtent(ax)+1, grid.GetNumVars());
            Array<double, 2> cons_r(grid.GetInnerExtent(ax)+1, grid.GetNumVars());
            for (std::size_t i = this->grid.GetNGhost(ax1) ; i < grid.GetFullExtent(ax1)-this->grid.GetNGhost(ax1) ; i++)
            {
                for (std::size_t j = this->grid.GetNGhost(ax2) ; j < grid.GetFullExtent(ax2)-this->grid.GetNGhost(ax2) ; j++)
                {
                    Array<double, 2> cons = this->grid.Get1DArray(ax, i, j, meshextents::inner_closed);
                    Array<double, 2> prim = this->grid.Get1DPrimArray(ax, i, j, meshextents::full);
                    this->slopelimiter->PrimLimitedStates(this->grid, prim, prim_l, prim_r, ax);
                    this->eos->PrimToCons(cons_l, prim_l);
                    this->eos->PrimToCons(cons_r, prim_r);
                    Array<double, 2> flux_l(this->grid.GetInnerExtent(ax)+1, this->grid.GetNumVars());
                    Array<double, 2> flux_r(this->grid.GetInnerExtent(ax)+1, this->grid.GetNumVars());
                    State1D state_l(cons_l, prim_l, flux_l);
                    State1D state_r(cons_r, prim_r, flux_r);
                    this->grid.CalcFlux(state_l, ax);
                    this->grid.CalcFlux(state_r, ax);
                    double curmaxalpha = this->integrator->AdvanceSoln(this->solver, this->eos, this->grid, cons, state_l, state_r, this->dt, ax);
                    maxdt = std::min(maxdt, this->courant*this->grid.GetSpacing(ax)/curmaxalpha);
                }
            }
        }
    }
    catch (std::exception const& e)
    {
        std::cout << "FATAL ERROR: " << e.what() << std::endl;
        exit(1);
    }
    std::cout << "Max dt: " << maxdt << std::endl;
    return maxdt;
}

