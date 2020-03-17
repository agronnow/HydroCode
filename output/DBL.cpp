#include "DBL.hpp"

DBL::~DBL()
{
    //dtor
}

void DBL::OutputSnapshot(Grid& grid, StepInfo& info)
{
    Array<double, 4> vars = grid.GetPrimitiveVars();
    std::size_t varsize = (vars.extent(0)*vars.extent(1)*vars.extent(2))*sizeof(double);
    for (auto& outputvar : this->outputvars)
    {
        std::size_t offset = varsize*outputvar.second;
        std::ostringstream fname;
        fname << outputvar.first << "." << std::setfill('0') << std::setw(4) << this->nout << ".dbl";
        std::ofstream DBLout(fname.str(), std::ios::binary);
        if (!DBLout.is_open())
        {
            std::cout << "Failed to open file " << fname.str() << " for output." << std::endl;
        }
        else
        {
            DBLout.write(reinterpret_cast<char*>(vars.data()+offset), varsize);
            DBLout.close();
        }
    }
    this->nout++;
}
