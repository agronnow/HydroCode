#ifndef EOS_H
#define EOS_H

#include <auxillary.hpp>

class EOS
{
    public:
        EOS(double g, Range m, Range v, std::size_t nd) : gamma(g), MomRange(m), VelRange(v), ndim(nd) {};
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
        inline int cenergy() const {return this->ndim + 2;}
        const double gamma;
        const Range MomRange;
        const Range VelRange;
        const std::size_t ndim;
    private:
        static constexpr double densityfloor = 1.e-9;
        static constexpr double pressurefloor = 1.e-10;
};

#endif // EOS_H
