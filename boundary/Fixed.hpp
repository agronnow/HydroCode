#ifndef FIXED_H
#define FIXED_H

#include "Boundary.hpp"


class Fixed : public Boundary
{
    public:
        Fixed(const Side s) : Boundary(s) {};
        virtual ~Fixed();
        void ApplyBoundary(Grid&) override;
};

#endif // FIXED_H
