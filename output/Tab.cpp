#include "Tab.hpp"

Tab::~Tab()
{
    //dtor
}

void Tab::OutputSnapshot(Grid& grid, StepInfo& info)
{
    Array<double, 4> vars = grid.GetPrimitiveVars();
    std::ostringstream fname;
    fname << "vars." << std::setfill('0') << std::setw(4) << this->nout << ".tab";
    std::ofstream Tabout(fname.str());
    if (!Tabout.is_open())
    {
        std::cout << "Failed to open file " << fname.str() << " for output." << std::endl;
    }
    else
    {
        Tabout << "#noutput  nstep     sim_time        sim_dt          " << std::endl << "#" <<
         boost::format("%10d %10d %16.8e %16.8e") % this->nout % info.nstep % info.t % info.dt <<
         std::endl << "#i        x1             ";
        for (auto& outputvar : this->outputvars) Tabout << std::setw(16) << outputvar.first;
        Tabout << std::endl;

        for (int i = grid.GetNGhost(Axis_x1) ; i < vars.extent(0) - grid.GetNGhost(Axis_x1) ; i++)
        {
            Tabout << boost::format("%10d %16.8e") % (i - grid.GetNGhost(Axis_x1)) % grid.GetCoord(Axis_x1, i, true);
            for (auto& outputvar : this->outputvars) Tabout << boost::format(" %16.8e") % vars(i, 0, 0, (int)outputvar.second);
            Tabout << std::endl;
        }
        Tabout.close();
    }
    this->nout++;
}
