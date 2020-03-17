#include "Grid.hpp"

int g_NDim = -1;

Grid::Grid(INI::File& settings)
{
    const std::size_t nghost = 2; //TODO calculate number of ghost cells based on stencil size of chosen numerical methods
	const double xbeg = settings.GetSection("domain")->GetValue("x").AsArray()[0].AsDouble();
	const double xend = settings.GetSection("domain")->GetValue("x").AsArray()[1].AsDouble();
	const double ybeg = settings.GetSection("domain")->GetValue("y").AsArray()[0].AsDouble();
	const double yend = settings.GetSection("domain")->GetValue("y").AsArray()[1].AsDouble();
	const double zbeg = settings.GetSection("domain")->GetValue("z").AsArray()[0].AsDouble();
	const double zend = settings.GetSection("domain")->GetValue("z").AsArray()[1].AsDouble();
	std::vector<Extents> Dom = { {xbeg, xend}, {ybeg, yend}, {zbeg, zend} };
	const std::size_t nx = settings.GetSection("domain")->GetValue("nx").AsInt();
	if (nx < 1)
	{
        std::ostringstream err;
        err << "SetDomain: nx must be a positive number!";
		throw std::runtime_error(err.str());
	}
	const std::size_t ny = settings.GetSection("domain")->GetValue("ny").AsInt();
	if (ny < 1)
	{
        std::ostringstream err;
        err << "SetDomain: nx must be a positive number!";
		throw std::runtime_error(err.str());
	}
	const std::size_t nz = settings.GetSection("domain")->GetValue("nz").AsInt();
	if (nz < 1)
	{
        std::ostringstream err;
        err << "SetDomain: nx must be a positive number!";
		throw std::runtime_error(err.str());
	}
	if ((xbeg >= xend) || (ybeg >= yend) || (zbeg >= zend))
	{
        std::ostringstream err;
        err << "SetDomain: invalid domain, each interval must be increasing!";
		throw std::runtime_error(err.str());
	}
	std::map<Side, std::string> sides;
	if (ny == 1) g_NDim = 1;
	else if (nz == 1) g_NDim = 2;
	else g_NDim = 3;
    this->MomRange = Range((int)consvars::cmom_x1, int(g_NDim+consvars::cmom_x1-1));
    this->VelRange = Range((int)primvars::pvx1, int(g_NDim+primvars::pvx1-1));

	const std::size_t nvar = 2 + g_NDim;
	this->NDim = g_NDim;
	this->NGhost = nghost;
	this->domain = Dom;
    this->Initialize(nx, ny, nz, nvar);
}

void Grid::Initialize(std::size_t sx, std::size_t sy, std::size_t sz, std::size_t nvar)
{
    this->conserved_vars.resize(sx+2*this->NGhost, sy+2*this->NGhost, sz+2*this->NGhost, nvar);
    this->prim_vars.resize(sx+2*this->NGhost, sy+2*this->NGhost, sz+2*this->NGhost, nvar);
    this->InnerRange[Axis::x1] = Range(this->NGhost, this->conserved_vars.extent(0) - 1 - this->NGhost);
    if (this->NDim > 1) this->InnerRange[Axis::x2] = Range(this->NGhost, this->conserved_vars.extent(1) - 1 - this->NGhost);
    else this->InnerRange[Axis::x2] = Range(0, 0);
    if (this->NDim > 2) this->InnerRange[Axis::x3] = Range(this->NGhost, this->conserved_vars.extent(2) - 1 - this->NGhost);
    else this->InnerRange[Axis::x3] = Range(0, 0);
    this->InnerOpenRange[Axis::x1] = Range(this->NGhost, this->conserved_vars.extent(0) - this->NGhost);
    if (this->NDim > 1) this->InnerOpenRange[Axis::x2] = Range(this->NGhost, this->conserved_vars.extent(1) - this->NGhost);
    else this->InnerOpenRange[Axis::x2] = Range(0, 0);
    if (this->NDim > 2) this->InnerOpenRange[Axis::x3] = Range(this->NGhost, this->conserved_vars.extent(2) - this->NGhost);
    else this->InnerOpenRange[Axis::x3] = Range(0, 0);
    for (int i = 0 ; i < this->NDim ; i++) this->dx.push_back((this->domain[i].axend - this->domain[i].axbeg)/(this->conserved_vars.extent(i)-2.0*this->NGhost));
    for (int i = this->NDim ; i < 3 ; i++) this->dx.push_back(this->domain[i].axend - this->domain[i].axbeg);
}

Grid::~Grid()
{
    //dtor
}

Array<double, 2> Grid::Get1DArray(Axis ax, std::size_t i, std::size_t j, meshextents ext = meshextents::inner_closed)
{
    if (ext == meshextents::full)
    {
        if (ax == Axis::x2) return this->conserved_vars(i, Range::all(), j, Range::all());
        if (ax == Axis::x3) return this->conserved_vars(i, j, Range::all(), Range::all());
        return this->conserved_vars(Range::all(), i, j, Range::all());
    }
    else if (ext == meshextents::inner_open)
    {
        if (ax == Axis::x2) return this->conserved_vars(i, this->InnerOpenRange[ax], j, Range::all());
        if (ax == Axis::x3) return this->conserved_vars(i, j, this->InnerOpenRange[ax], Range::all());
        return this->conserved_vars(this->InnerOpenRange[ax], i, j, Range::all());
    }
    if (ax == Axis::x2) return this->conserved_vars(i, this->InnerRange[ax], j, Range::all());
    if (ax == Axis::x3) return this->conserved_vars(i, j, this->InnerRange[ax], Range::all());
    return this->conserved_vars(this->InnerRange[ax], i, j, Range::all());
}

const Array<double, 2> Grid::GetConst1DArray(Axis ax, std::size_t i, std::size_t j, meshextents ext = meshextents::inner_closed)
{
    if (ext == meshextents::full)
    {
        if (ax == Axis::x2) return this->conserved_vars(i, Range::all(), j, Range::all());
        if (ax == Axis::x3) return this->conserved_vars(i, j, Range::all(), Range::all());
        return this->conserved_vars(Range::all(), i, j, Range::all());
    }
    else if (ext == meshextents::inner_open)
    {
        if (ax == Axis::x2) return this->conserved_vars(i, this->InnerOpenRange[ax], j, Range::all());
        if (ax == Axis::x3) return this->conserved_vars(i, j, this->InnerOpenRange[ax], Range::all());
        return this->conserved_vars(this->InnerOpenRange[ax], i, j, Range::all());
    }
    if (ax == Axis::x2) return this->conserved_vars(i, this->InnerRange[ax], j, Range::all());
    if (ax == Axis::x3) return this->conserved_vars(i, j, this->InnerRange[ax], Range::all());
    return this->conserved_vars(this->InnerRange[ax], i, j, Range::all());
}
Array<double, 2> Grid::Get1DPrimArray(Axis ax, std::size_t i, std::size_t j, meshextents ext = meshextents::inner_closed)
{
    if (ext == meshextents::full)
    {
        if (ax == Axis::x2) return this->prim_vars(i, Range::all(), j, Range::all());
        if (ax == Axis::x3) return this->prim_vars(i, j, Range::all(), Range::all());
        return this->prim_vars(Range::all(), i, j, Range::all());
    }
    else if (ext == meshextents::inner_open)
    {
        if (ax == Axis::x2) return this->prim_vars(i, this->InnerOpenRange[ax], j, Range::all());
        if (ax == Axis::x3) return this->prim_vars(i, j, this->InnerOpenRange[ax], Range::all());
        return this->prim_vars(this->InnerOpenRange[ax], i, j, Range::all());
    }
    if (ax == Axis::x2) return this->prim_vars(i, this->InnerRange[ax], j, Range::all());
    if (ax == Axis::x3) return this->prim_vars(i, j, this->InnerRange[ax], Range::all());
    return this->prim_vars(this->InnerRange[ax], i, j, Range::all());
}
const Array<double, 2> Grid::GetConst1DPrimArray(Axis ax, std::size_t i, std::size_t j, meshextents ext = meshextents::inner_closed)
{
    if (ext == meshextents::full)
    {
        if (ax == Axis::x2) return this->prim_vars(i, Range::all(), j, Range::all());
        if (ax == Axis::x3) return this->prim_vars(i, j, Range::all(), Range::all());
        return this->prim_vars(Range::all(), i, j, Range::all());
    }
    else if (ext == meshextents::inner_open)
    {
        if (ax == Axis::x2) return this->prim_vars(i, this->InnerOpenRange[ax], j, Range::all());
        if (ax == Axis::x3) return this->prim_vars(i, j, this->InnerOpenRange[ax], Range::all());
        return this->prim_vars(this->InnerOpenRange[ax], i, j, Range::all());
    }
    if (ax == Axis::x2) return this->prim_vars(i, this->InnerRange[ax], j, Range::all());
    if (ax == Axis::x3) return this->prim_vars(i, j, this->InnerRange[ax], Range::all());
    return this->prim_vars(this->InnerRange[ax], i, j, Range::all());
}



void Grid::CalcFlux(State1D& state, Axis ax)
{
    //std::cout << "Cons " << cons << std::endl;
	state.flux(Range::all(), (int)consvars::cdens) = state.cons(Range::all(), int(consvars::cmom_x1 + to_idx(ax)));
    for (int ndim = 0 ; ndim < g_NDim ; ndim++)
    {
    	state.flux(Range::all(), (int)consvars::cmom_x1 + ndim) = state.cons(Range::all(), (int)consvars::cmom_x1 + ndim) *
    			state.prim(Range::all(), int(primvars::pvx1 + to_idx(ax)));
    }
    state.flux(Range::all(), int(consvars::cmom_x1 + to_idx(ax))) += state.prim(Range::all(), (int)primvars::pprs);
    //std::cout << "Prim(" << prim.extent(0) -1 << "): PRS:" << prim(prim.extent(0)-1, (int)primvars::pprs) <<
    // " vx1: " << prim(prim.extent(0)-1, int(primvars::pvx1 + to_idx(ax))) <<  " ax=" << to_idx(ax) << std::endl;
    state.flux(Range::all(), cenergy()) = (state.cons(Range::all(), cenergy()) + state.prim(Range::all(), (int)primvars::pprs)) *
    		state.prim(Range::all(), int(primvars::pvx1 + to_idx(ax)));
}


Range Grid::GetRange(Axis ax, meshextents ext = meshextents::inner_closed)
{
    if (ext == meshextents::full) return Range::all();
    else if (ext == meshextents::inner_open) return this->InnerOpenRange[ax];
    else return this->InnerRange[ax];
}
