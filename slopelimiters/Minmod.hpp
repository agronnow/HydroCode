#ifndef MINMOD_H
#define MINMOD_H

#include <Slopelimiter.hpp>


class Minmod : public Slopelimiter
{
    public:
	Minmod();
        virtual ~Minmod();
        void PrimLimitedStates(Grid&, const Array<double, 2>&, Array<double, 2>&, Array<double, 2>&, Axis) override final;
    protected:
    private:
};

#endif // MINMOD_H
