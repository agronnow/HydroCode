#include "Simulation.hpp"
extern int g_NDim;

void Simulation::ApplyInitialConditions()
{
    Array<double, 4> cons = this->grid.GetInnerConservedVars();
    for (int i = 0 ; i < cons.extent(0) ; i++)
    {
        for (int j = 0 ; j < cons.extent(1) ; j++)
        {
            for (int k = 0 ; k < cons.extent(2) ; k++)
            {
                //std::cout << "i, j, k, x1, x2, x3:" << i << " " << j << " " << k << ", " << grid.x1(i) << " " << grid.x2(j) << " " << grid.x3(k) << std::endl;
                if (i < cons.extent(0)/2)
                {
                    cons(i,j,k,(int)consvars::cdens) = 1.0;
                    for (int cdim = 0; cdim < g_NDim; cdim++) cons(i,j,k,int(consvars::cmom_x1 + cdim)) = 0.0;
                    cons(i,j,k,cenergy()) = 1.0/0.4;
                }
                else
                {
                    cons(i,j,k,(int)consvars::cdens) = 0.1;
                    for (int cdim = 0; cdim <  g_NDim; cdim++) cons(i,j,k,int(consvars::cmom_x1 + cdim)) = 0.0;
                    cons(i,j,k,cenergy()) = 0.1/0.4;
                }
                //std::cout << "i, j, k, rho:" << i << " " << j << " " << k << ", " << this->grid(i,j,k,consvars::cdens) << std::endl;
            }
        }
    }
    std::cout << "Memory usage: " << grid.GetGridMemUsage() << std::endl;
}
