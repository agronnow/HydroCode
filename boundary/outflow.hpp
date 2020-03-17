#ifndef OUTFLOW_H
#define OUTFLOW_H

#include "Boundary.hpp"


class Outflow : public Boundary
{
    public:
        Outflow(const Side s) : Boundary(s) {};
        virtual ~Outflow();
        void ApplyBoundary(Grid&) override;
    protected:
    private:
};

#endif // OUTFLOW_H

