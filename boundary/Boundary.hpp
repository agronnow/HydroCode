#ifndef BOUNDARY_H
#define BOUNDARY_H
#include <Grid.hpp>

class Boundary
{
    public:
        Boundary(const Side);
        virtual ~Boundary();
        virtual void ApplyBoundary(Grid&) = 0;
    protected:
        const Side side;
};

#endif // BOUNDARY_H
