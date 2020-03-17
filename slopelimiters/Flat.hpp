#ifndef FLAT_H
#define FLAT_H

#include <Slopelimiter.hpp>


class Flat : public Slopelimiter
{
    public:
	Flat();
        virtual ~Flat();
        void PrimLimitedStates(Grid&, const Array<double, 2>&, Array<double, 2>&, Array<double, 2>&, Axis) override final;
    protected:
    private:
};

#endif // FLAT_H
