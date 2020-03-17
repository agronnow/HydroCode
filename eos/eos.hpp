#ifndef EOS_H
#define EOS_H

#include <auxillary.hpp>

class EOS
{
    public:
        EOS(double g, Range m, Range v) : gamma(g), MomRange(m), VelRange(v) {};
        virtual ~EOS();
        virtual void SoundSpeedSq(Array<double, 1>, const Array<double, 2>) = 0;
        virtual double GetEnergy(const Array<double, 1>&, const Array<double, 1>&) const = 0;
        virtual void GetEnergy(const Array<double, 2>&, const Array<double, 2>&) const = 0;
        virtual void GetEnergy(const Array<double, 4>&, const Array<double, 4>&) const = 0;
        virtual double GetPressure(const Array<double, 1>&, const Array<double, 1>&) const = 0;
        virtual void GetPressure(const Array<double, 2>&, Array<double, 2>&) const = 0;
        constoprim_status ConsToPrim(Array<double, 4>&, Array<double, 4>&);
        constoprim_status ConsToPrim(Array<double, 2>&, Array<double, 2>&);
        void PrimToCons(Array<double, 4>&, Array<double, 4>&);
        void PrimToCons(Array<double, 2>&, Array<double, 2>&);
        void PrimToChar(Array<double, 4>&, Array<double, 4>&, Array<double, 4>&);
    protected:
        double gamma;
        Range MomRange;
        Range VelRange;
    private:
        static constexpr double densityfloor = 1.e-9;
        static constexpr double pressurefloor = 1.e-10;
};

#endif // EOS_H
