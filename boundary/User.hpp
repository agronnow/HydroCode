#ifndef USER_H
#define USER_H

#include "Boundary.hpp"


class User : public Boundary
{
    public:
		User(const Side s) : Boundary(s) {};
        virtual ~User();
        void ApplyBoundary(Grid&) override;
    protected:
    private:
};

#endif // USER_H
