#ifndef IDEAL_H
#define IDEAL_H

#include <eos.hpp>
#include <blitz/tinyvec2.h>


class Ideal : public EOS
{
    public:
        Ideal(double g, Range m, Range v, std::size_t nd) : EOS(g, m, v, nd) {};
        virtual ~Ideal();
        void SoundSpeedSq(Array<double, 1>, const Array<double, 2>) override;
        inline double GetEnergy(const Array<double, 1>&, const Array<double, 1>&) const override;
        void GetEnergy(const Array<double, 2>&, const Array<double, 2>&) const override;
        void GetEnergy(const Array<double, 4>&, const Array<double, 4>&) const override;
        inline double GetPressure(const Array<double, 1>&, const Array<double, 1>&) const override;
        void GetPressure(const Array<double, 2>&, Array<double, 2>&) const override;
    protected:
    private:
};

#endif // IDEAL_H
