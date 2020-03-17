#ifndef PERIODIC_H
#define PERIODIC_H

#include "Boundary.hpp"


class Periodic : public Boundary
{
    public:
        Periodic(const Side s) : Boundary(s) {};
        virtual ~Periodic();
        void ApplyBoundary(Grid&) override;
    protected:
    private:
};

#endif // PERIODIC_H
