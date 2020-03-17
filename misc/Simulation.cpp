#include "Simulation.hpp"
extern int g_NDim;

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
	this->SetTimestep(settings);
	this->SetOutput(settings);
	this->SetPhysics(settings);
	this->SetSchemes(settings);
}

void Simulation::SetBoundaries(INI::File& settings)
{
	std::map<Side, std::string> sides;
	switch(g_NDim)
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
	auto boundarytypes = settings.GetSection("domain")->GetValue("boundary").AsMap();
	for (auto& side : sides) this->boundaries.insert(std::pair<Side, std::unique_ptr<Boundary>>(side.first,std::move(CreateBoundary(boundarytypes[side.second].AsString(), side.first))));
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
	this->dt = dt_init;
	this->tstop = t_end;
}

void Simulation::SetOutput(INI::File& settings)
{
    std::vector<std::pair<std::string, int>> vars = {{"rho", 0}, {"prs", 1}, {"vx1", 2}};
	if (g_NDim > 1) vars.emplace_back("vx2", 3);
	if (g_NDim == 3)
	{
		vars.emplace_back("vx2", 3);
		vars.emplace_back("vx3", 4);
	}
	const int nout = 0; //TODO set accordingly if restarting
	double dtout = settings.GetSection("output")->GetSubSection("dbl")->GetValue("dt_output",-1.0).AsDouble();
    std::vector<std::unique_ptr<Output>> output;
    if (dtout > 0.0) outputs.push_back(std::move(std::make_unique<DBL>(DBL(nout, dtout, vars))));
	dtout = settings.GetSection("output")->GetSubSection("tab")->GetValue("dt_output",-1.0).AsDouble();
    if (dtout > 0.0) outputs.push_back(std::move(std::make_unique<Tab>(Tab(nout, dtout, vars))));
	dtout = settings.GetSection("output")->GetSubSection("png")->GetValue("dt_output",-1.0).AsDouble();
    if (dtout > 0.0) outputs.push_back(std::move(std::make_unique<PNG>(PNG(nout, dtout, vars, Axis::x3, true, true, "cubehelix.cmap", true))));
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
	    this->eos = std::make_unique<Ideal>(gamma, this->grid.GetMomRange(), this->grid.GetVelRange());
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
    //std::cout << "Vars:" << std::endl << this->grid.GetConservedVars() << std::endl;
        this->t += this->dt;
        std::cout << "Time: " << this->t << std::endl;
        this->Step();
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
        //std::cout << "Vars:" << std::endl << this->grid.GetConservedVars() << std::endl;
    }
    auto walltime_end = chrono::steady_clock::now();
    auto diff = walltime_end - walltime_start;
    std::cout << "Done" << std::endl << "Wall time used: " << chrono::duration <double, milli> (diff).count() << " ms" << std::endl;
}

void Simulation::Step()
{
    for (auto const& boundary : this->boundaries) boundary.second->ApplyBoundary(this->grid);
    this->eos->ConsToPrim(this->grid.GetPrimitiveVars(), this->grid.GetConservedVars());
    Axis ax1;
    Axis ax2;
    double maxdt = 1.e20;
    try
    {
        for (int ax = to_idx(Axis::x1) ; ax <= g_NDim-1 ; ax++)
        {
            if (ax == to_idx(Axis::x1))
            {
                ax1 = Axis::x2;
                ax2 = Axis::x3;
            }
            else if (ax == to_idx(Axis::x2))
            {
                ax1 = Axis::x1;
                ax2 = Axis::x3;
            }
            else
            {
                ax1 = Axis::x1;
                ax2 = Axis::x2;
            }
            Array<double, 2> prim_l(grid.GetInnerExtent(Axes[ax])+1, grid.GetNumVars());
            Array<double, 2> prim_r(grid.GetInnerExtent(Axes[ax])+1, grid.GetNumVars());
            Array<double, 2> cons_l(grid.GetInnerExtent(Axes[ax])+1, grid.GetNumVars());
            Array<double, 2> cons_r(grid.GetInnerExtent(Axes[ax])+1, grid.GetNumVars());
            for (size_t i = this->grid.GetNGhost(ax1) ; i < grid.GetFullExtent(ax1)-this->grid.GetNGhost(ax1) ; i++)
            {
                for (size_t j = this->grid.GetNGhost(ax2) ; j < grid.GetFullExtent(ax2)-this->grid.GetNGhost(ax2) ; j++)
                {
                    Array<double, 2> cons = this->grid.Get1DArray(Axes[ax], i, j, meshextents::inner_closed);
                    Array<double, 2> prim = this->grid.Get1DPrimArray(Axes[ax], i, j, meshextents::full);
                    this->slopelimiter->PrimLimitedStates(this->grid, prim, prim_l, prim_r, Axes[ax]);
                    this->eos->PrimToCons(cons_l, prim_l);
                    this->eos->PrimToCons(cons_r, prim_r);
                    Array<double, 2> flux_l(this->grid.GetInnerExtent(Axes[ax])+1, this->grid.GetNumVars());
                    Array<double, 2> flux_r(this->grid.GetInnerExtent(Axes[ax])+1, this->grid.GetNumVars());
                    State1D state_l(cons_l, prim_l, flux_l);
                    State1D state_r(cons_r, prim_r, flux_r);
                    //std::cout << grid.GetExtent(Axes[ax]) << " " << this->grid.GetNGhost(Axes[ax]) << " " << grid.GetExtent(Axes[ax])-2*this->grid.GetNGhost(Axes[ax])+1 << std::endl;
                    this->grid.CalcFlux(state_l, Axes[ax]);
                    this->grid.CalcFlux(state_r, Axes[ax]);
//                    std::cout << "flux: " << state_l.flux(201,(int)consvars::cmom_x1) << " " << state_r.flux(201,(int)consvars::cmom_x1) << std::endl;
/*                    Array<double, 2> prim = this->grid.Get1DPrimArray(Axes[ax], i, j, meshextents::full);
                    Array<double, 2> dp(grid.GetExtent(Axes[ax])-2*this->grid.GetNGhost(Axes[ax])+1);
                    Array<double, 2> dm(grid.GetExtent(Axes[ax])-2*this->grid.GetNGhost(Axes[ax])+1);
                    for (int l = this->grid.GetNGhost(Axes[ax])-1; l < grid.GetExtent(Axes[ax])-this->grid.GetNGhost(Axes[ax]); l++)
                    {
                        dp(l) = prim(l+1, Range::all()) - prim(l, Range::all());
                        dm(l) = prim(l, Range::all()) - prim(l-1, Range::all());
                    }
                    Array<double, 2> d0 = 0.5*(dp + dm);
                    this->slopelim(prim, dp, dm, d0);*/
                    //std::cout << ax << ": " << i << " " << j << " Arr:" << std::endl << this->grid.GetConservedVars() << std::endl;
                    //std::cout << "Flux:" << std::endl << flux << std::endl;
                    //std::cout << "Riemann:" << std::endl << flux << std::endl;
                    //std::cout << "Cons:" << std::endl << cons << std::endl;
                    double curmaxalpha = this->integrator->AdvanceSoln(this->solver, this->eos, this->grid, cons, state_l, state_r, this->dt, Axes[ax]);
                    //std::cout << "Cons:" << std::endl << cons << std::endl;
                    //std::cout << this->grid.GetSpacing(Axes[ax]) << " " << curmaxalpha << std::endl;
                    maxdt = std::min(maxdt, this->grid.GetSpacing(Axes[ax])/curmaxalpha);
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
}

