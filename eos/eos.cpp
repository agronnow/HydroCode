#include "eos.hpp"
extern int g_NDim;


EOS::~EOS()
{
    //dtor
}

constoprim_status EOS::ConsToPrim(Array<double, 4>& prim, Array<double, 4>& cons)
{
    constoprim_status status = constoprim_status::ok;

    for (int i = 0 ; i < cons.extent(0) ; i++)
    {
        for (int j = 0 ; j < cons.extent(1) ; j++)
        {
            for (int k = 0 ; k < cons.extent(2) ; k++)
            {
                if (cons(i, j, k, (int)consvars::cdens) < 0.0)
                {
                  std::cout << "WARNING: ConsToPrim: Negative density rho=" << cons(i, j, k, (int)consvars::cdens) << " at i, j, k: " << i << ", " << j << ", " << k << std::endl;
                  cons(i, j, k, (int)consvars::cdens) = this->densityfloor;
                  status = constoprim_status::neg_density;
                }
                prim(i, j, k, (int)primvars::pdens) = cons(i, j, k, (int)consvars::cdens);

                prim(i, j, k, this->VelRange) = cons(i, j, k, this->MomRange)/cons(i, j, k, (int)consvars::cdens);

                if (cons(i, j, k, cenergy()) < 0.0)
                {
                    std::cout << "WARNING: CalcConsToPrim: Negative energy E=" << cons(i, j, k, cenergy()) << " at i, j, k: " << i << ", " << j << ", " << k << std::endl;
                    prim(i, j, k, (int)primvars::pprs) = this->pressurefloor;
                    cons(i, j, k, cenergy()) = this->GetEnergy(cons(i, j, k, Range::all()), prim(i, j, k, Range::all()));//TODO this->pressurefloor/this->eos.gamma_minus_one + Ekin;
                    status = constoprim_status::neg_energy;
                }
                else
                {
                    prim(i, j, k, (int)primvars::pprs) = this->GetPressure(cons(i, j, k, Range::all()), prim(i, j, k, Range::all()));//this->eos.gamma_minus_one*(this->conserved_vars[i][j][k][consvars::cenergy] - Ekin);
                    if (prim(i, j, k, (int)primvars::pprs) < 0.0)
                    {
                        std::cout << "WARNING: CalcConsToPrim: Negative pressure P=" << prim(i, j, k, (int)primvars::pprs) << " at i, j, k: " << i << ", " << j << ", " << k << std::endl;
                        prim(i, j, k, (int)primvars::pprs) =  this->pressurefloor;
                        cons(i, j, k, cenergy()) = this->GetEnergy(cons(i, j, k, Range::all()), prim(i, j, k, Range::all()));
                        status = constoprim_status::neg_pressure;
                    }
                }
                //std::cout << "Cons2Prim: Prim(" << i << "): PRS: " << prim(i, j, k, (int)primvars::pprs) << " vx: " << prim(i, j, k, VelRange) << std::endl;
            }
        }
    }
    return status;
}

constoprim_status EOS::ConsToPrim(Array<double, 2>& prim, Array<double, 2>& cons)
{
    constoprim_status status = constoprim_status::ok;
    double minrho = min(cons(Range::all(), (int)consvars::cdens));
	if (minrho < 0.0)
	{
	  std::cout << "WARNING: ConsToPrim: Negative density rho=" << minrho << std::endl;
	  cons(Range::all(), (int)consvars::cdens) = where(cons(Range::all(), (int)consvars::cdens) < 0.0,
			  this->densityfloor, cons(Range::all(), (int)consvars::cdens));
	  status = constoprim_status::neg_density;
	}
	prim(Range::all(), (int)primvars::pdens) = cons(Range::all(), (int)consvars::cdens);
	for (int ndim = 0 ; ndim < g_NDim ; ndim++)
	{
		prim(Range::all(), primvars::pvx1 + ndim) = cons(Range::all(), consvars::cmom_x1 + ndim)/cons(Range::all(), (int)consvars::cdens);
	}

	if (any(cons(Range::all(), cenergy())) < 0.0)
	{
		for (int i = 0; i < cons.extent(0); ++i)
		{
			if (cons(i, cenergy()) < 0.0)
			{
				std::cout << "WARNING: ConsToPrim: Negative energy E=" << cons(i, cenergy()) << " at i=" << i << std::endl;
				prim(i, (int)primvars::pprs) = this->pressurefloor;
				cons(i, cenergy()) = this->GetEnergy(cons(i, Range::all()), prim(i, Range::all()));//TODO this->pressurefloor/this->eos.gamma_minus_one + Ekin;
			}
		}
		status = constoprim_status::neg_energy;
	}
	else
	{
		this->GetPressure(cons, prim);//this->eos.gamma_minus_one*(this->conserved_vars[i][j][k][consvars::cenergy] - Ekin);
		if (any(prim(Range::all(), (int)primvars::pprs) < 0.0))
		{
			for (int i = 0; i < prim.extent(0); ++i)
			{
				if (prim(i, (int)primvars::pprs) < 0.0)
				{
					std::cout << "WARNING: ConsToPrim: Negative pressure P=" << prim(i, (int)primvars::pprs) << " at i=" << i << std::endl;
					prim(i, (int)primvars::pprs) =  this->pressurefloor;
					cons(i, cenergy()) = this->GetEnergy(cons(i, Range::all()), prim(i, Range::all()));
					status = constoprim_status::neg_pressure;
				}
			}
		}
	}
    return status;
}

void EOS::PrimToCons(Array<double, 4>& cons, Array<double, 4>& prim)
{
	cons(Range::all(), Range::all(), Range::all(), (int)consvars::cdens) = prim(Range::all(), Range::all(), Range::all(), (int)primvars::pdens);
	for (int ndim = 0 ; ndim < g_NDim ; ndim++)
	{
		cons(Range::all(), Range::all(), Range::all(), primvars::pvx1 + ndim) =
		 prim(Range::all(), Range::all(), Range::all(), consvars::cmom_x1 + ndim)*prim(Range::all(), Range::all(), Range::all(), (int)primvars::pdens);
	}
	this->GetEnergy(cons, prim);
}

void EOS::PrimToCons(Array<double, 2>& cons, Array<double, 2>& prim)
{
	cons(Range::all(), (int)consvars::cdens) = prim(Range::all(), (int)primvars::pdens);
	for (int ndim = 0 ; ndim < g_NDim ; ndim++)
	{
		cons(Range::all(), consvars::cmom_x1 + ndim) = prim(Range::all(), primvars::pvx1 + ndim)*prim(Range::all(), (int)primvars::pdens);
	}
	this->GetEnergy(cons, prim);
}

/*void EOS::PrimToChar(Array<double, 4>& charvars, Array<double, 4>& Leig, Array<double, 4>& prim)
{
    //TODO
    for (int i = 0 ; i < prim.extent(0) ; i++)
    {
        for (int j = 0 ; j < prim.extent(1) ; j++)
        {
            for (int k = 0 ; k < prim.extent(2) ; k++)
            {
                charvars(i, j, k, (int)consvars::cdens) = prim(i, j, k, (int)primvars::pdens);
                cons(i, j, k, MomRange) = prim(i, j, k, VelRange)*prim(i, j, k, (int)primvars::pdens);
                cons(i, j, k, cenergy()) = this->GetEnergy(cons(i, j, k, Range::all()), prim(i, j, k, Range::all()));
            }
        }
    }
}
*/
