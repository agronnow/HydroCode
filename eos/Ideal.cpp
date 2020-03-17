#include "Ideal.hpp"
#include <iostream>

Ideal::~Ideal()
{
    //dtor
}

void Ideal::SoundSpeedSq(Array<double, 1> C_s, const Array<double, 2> prim)
{
    C_s = this->gamma * prim(Range::all(), (int)primvars::pprs)/prim(Range::all(), (int)primvars::pdens);
}

inline double Ideal::GetEnergy(const Array<double, 1>& cons, const Array<double, 1>& prim) const
 {return prim((int)primvars::pprs)/(this->gamma - 1.0) + 0.5*sum(pow2(cons(this->MomRange)))/cons((int)consvars::cdens);}

void Ideal::GetEnergy(const Array<double, 2>& cons, const Array<double, 2>& prim) const
{
	cons(Range::all(), cenergy()) = prim(Range::all(), (int)primvars::pprs)/(this->gamma - 1.0) +
		 0.5*sum(sqr(cons(Range::all(), this->MomRange)))/cons(Range::all(), (int)consvars::cdens);
}

void Ideal::GetEnergy(const Array<double, 4>& cons, const Array<double, 4>& prim) const
{
	cons(Range::all(), Range::all(), Range::all(), cenergy()) = prim(Range::all(), Range::all(), Range::all(), (int)primvars::pprs)/(this->gamma - 1.0) +
		0.5*sum(sqr(cons(Range::all(), Range::all(), Range::all(), this->MomRange)))/
		cons(Range::all(), Range::all(), Range::all(), (int)consvars::cdens);
}

inline double Ideal::GetPressure(const Array<double, 1>& cons, const Array<double, 1>& prim) const
 {return (this->gamma - 1.0)*(cons(cenergy()) - 0.5*sum(pow2(cons(this->MomRange)))/cons((int)consvars::cdens));}

void Ideal::GetPressure(const Array<double, 2>& cons, Array<double, 2>& prim) const
{
	prim(Range::all(), (int)primvars::pprs) = (this->gamma - 1.0)*(cons(Range::all(), cenergy()) - 0.5*sum(sqr(cons(Range::all(), this->MomRange)))/
			cons(Range::all(), (int)consvars::cdens));
}
