#ifndef MONOTONIZEDCENTRAL_H
#define MONOTONIZEDCENTRAL_H

#include <Slopelimiter.hpp>


class MonotonizedCentral : public Slopelimiter
{
    public:
	MonotonizedCentral();
        virtual ~MonotonizedCentral();
        void PrimLimitedStates(Grid&, const Array<double, 2>&, Array<double, 2>&, Array<double, 2>&, Axis) override final;
    protected:
    private:
};

#endif // MONOTONIZEDCENTRAL_H
