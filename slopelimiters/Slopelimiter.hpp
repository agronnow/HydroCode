#ifndef SLOPELIMITERS_H
#define SLOPELIMITERS_H

#include <auxillary.hpp>
#include <Grid.hpp>

class Slopelimiter
{
    public:
		Slopelimiter();
        virtual ~Slopelimiter();
        virtual void PrimLimitedStates(Grid&, const Array<double, 2>&, Array<double, 2>&, Array<double, 2>&, Axis) = 0;
    protected:
    private:
};

#endif //SLOPELIMITERS_H
