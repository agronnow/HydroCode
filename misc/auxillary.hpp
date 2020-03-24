#ifndef AUXILLARY_H
#define AUXILLARY_H
#include <blitz/array.h>
#include <blitz/tinyvec2.h>
#include <memory>
#include <array>
#include <exception>
#include <boost/format.hpp>

#if NDIM == 1
 #define DIM_EXPAND(a,b,c) a
 #define DIM_SELECT(a,b,c) a
#endif

#if NDIM == 2
 #define DIM_EXPAND(a,b,c) a b
 #define DIM_SELECT(a,b,c) b
#endif

#if NDIM == 3
 #define DIM_EXPAND(a,b,c) a b c
 #define DIM_SELECT(a,b,c) c
#endif

using namespace blitz;

template<typename T>
inline T square(T x) {return x*x;}

inline double signum(double x) {return (x > 0.0) ? 1.0 : ((x < 0.0) ? -1 : 0.0);}
BZ_DECLARE_FUNCTION(signum)
inline double absmin(double x, double y) {return (std::abs(x) < std::abs(y) ? x:y);}
BZ_DECLARE_FUNCTION2(absmin)

template <typename E>
constexpr typename std::underlying_type<E>::type to_idx(E e) {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

using Axis = std::size_t;
enum : std::size_t
{
    Axis_x1 = 0, Axis_x2, Axis_x3, Axis_x = 0, Axis_y, Axis_z
};
//const std::array<Axis, 3> Axes = {Axis_x1, Axis_x2, Axis_x3};

enum class Side : unsigned int
{
    x1b = 0, x1e = 1, x2b = 2, x2e = 3, x3b = 4, x3e = 5
};
//const std::array<Side, 6> Sides = {Side::x1b, Side::x1e, Side::x2b, Side::x2e, Side::x3b, Side::x3e};

enum class VarIdx : unsigned int
{
    x1 = 0, x2 = 1, x3 = 2, var = 3
};

enum class meshextents : unsigned int
{
    inner_closed = 0, inner_open = 1, full = 2
};

enum class Dir : unsigned int
{
    center, left, right, up, down
};

enum consvars
{
    cdens = 0, cmom_x1, cmom_x2, cmom_x3, energy3d
};

enum primvars
{
    pdens = 0, pprs, pvx1, pvx2, pvx3
};

enum class constoprim_status {ok, neg_energy, neg_pressure, neg_density};

struct Extents
{
    double axbeg;
    double axend;
};

struct State1D
{
    State1D(Array<double, 2>& c, Array<double, 2>& p, Array<double, 2>& f) : cons(c), prim(p), flux(f) {};
    Array<double, 2>& cons;
    Array<double, 2>& prim;
    Array<double, 2>& flux;
};

#endif //AUXILLARY_H
