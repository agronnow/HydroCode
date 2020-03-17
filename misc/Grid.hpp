#ifndef GRID_H
#define GRID_H
#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <auxillary.hpp>
#include <iniparser.hpp>

class Grid
{
    public:
        Grid(INI::File&);
        virtual ~Grid();
        Grid(const Grid&) = delete; // non construction-copyable
        std::unique_ptr<Grid>& operator=(const Grid&) = delete; // non copyable
        void Initialize(std::size_t, std::size_t, std::size_t, std::size_t);
        Array<double, 4>& GetConservedVars() {return this->conserved_vars;}
        Array<double, 4>& GetPrimitiveVars() {return this->prim_vars;}
        Array<double, 4> GetInnerConservedVars() {return this->conserved_vars(this->InnerRange[Axis::x1], this->InnerRange[Axis::x2], this->InnerRange[Axis::x3], Range::all());}
        Array<double, 2> Get1DArray(Axis, std::size_t, std::size_t, meshextents);
        const Array<double, 2> GetConst1DArray(Axis, std::size_t, std::size_t, meshextents);
        Array<double, 2> Get1DPrimArray(Axis, std::size_t, std::size_t, meshextents);
        const Array<double, 2> GetConst1DPrimArray(Axis, std::size_t, std::size_t, meshextents);
        double GetCoord(Axis ax, int idx, bool subtr_ghost=false) {return this->dx[to_idx(ax)]*(idx - this->NGhost*(int)subtr_ghost + 0.5) + this->domain[to_idx(ax)].axbeg;}
        double GetCoordLeft(Axis ax, int idx, bool subtr_ghost=false) {return this->dx[to_idx(ax)]*(idx - this->NGhost*(int)subtr_ghost) + this->domain[to_idx(ax)].axbeg;}
        double GetCoordRight(Axis ax, int idx, bool subtr_ghost=false) {return this->dx[to_idx(ax)]*(idx + 1 - this->NGhost*(int)subtr_ghost) + this->domain[to_idx(ax)].axbeg;}
        double x1(int idx) {return this->dx[0]*(idx + 0.5) + this->domain[0].axbeg;}
        double x2(int idx) {return this->dx[1]*(idx + 0.5) + this->domain[1].axbeg;}
        double x3(int idx) {return this->dx[2]*(idx + 0.5) + this->domain[2].axbeg;}
        double GetSpacing(Axis ax) {return this->dx[to_idx(ax)];}
        std::size_t GetFullExtent(Axis ax) {return this->conserved_vars.extent(to_idx(ax));}
        std::size_t GetNumVars() {return this->conserved_vars.extent((int)VarIdx::var);}
        std::size_t GetFullExtent(int ax) {return this->conserved_vars.extent(ax);}
        std::size_t GetInnerExtent(Axis ax) {return this->conserved_vars.extent(to_idx(ax)) - 2*this->GetNGhost(ax);}
        std::size_t GetInnerExtent(int ax) {return this->conserved_vars.extent(ax) - 2*this->GetNGhost(Axes[ax]);}
        std::size_t GetInnerBeg(Axis ax) {return this->GetNGhost(ax);}
        std::size_t GetInnerEnd(Axis ax) {return this->conserved_vars.extent(to_idx(ax)) - this->GetNGhost(ax) - 1;}
        std::size_t GetGridMemUsage() {return sizeof(double)*this->conserved_vars.size();}
        void CalcFlux(State1D&, Axis);
        inline std::size_t GetNGhost(Axis ax)
        {
            if (to_idx(ax) > this->NDim - 1) return 0;
            return this->NGhost;
        }
        Range GetRange(Axis, meshextents);
        Range GetMomRange() {return this->MomRange;}
        Range GetVelRange() {return this->VelRange;}

        //double& operator()(std::size_t i, std::size_t j, std::size_t k, std::size_t nv) {return conserved_vars(i, j, k, nv);}
    protected:
    private:
        std::size_t NDim;
        Array<double, 4> conserved_vars;
        Array<double, 4> prim_vars;
        std::vector<Extents> domain;
        std::vector<double> dx;
        std::size_t NGhost;
        std::map<Axis, Range> InnerRange;
        std::map<Axis, Range> InnerOpenRange;
        Range MomRange;
        Range VelRange;
};

#endif // GRID_H
