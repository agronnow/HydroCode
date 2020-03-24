#include "hll.hpp"

//extern Range MomRange;

hll::~hll()
{
    //dtor
}

double hll::Riemann(State1D& state_l, State1D& state_r, Axis CalcDir, std::unique_ptr<EOS>& eos)
{
    //std::cout << "Prim:" << prim << std::endl;
    Array<double, 1> c_sl(state_l.flux.extent(0));
    Array<double, 1> c_sr(state_r.flux.extent(0));
    eos->SoundSpeedSq(c_sl, state_l.prim);
    eos->SoundSpeedSq(c_sr, state_l.prim);
    c_sl = sqrt(c_sl);
    c_sr = sqrt(c_sr);
    double maxalpha = 0.0;
    //std::cout << "C_s: " << c_s << std::endl;
    //std::cout << "vminL: " << v_minL << " vmaxL: " << v_maxL << std::endl;
    for (int i = 0 ; i < state_l.flux.extent(0); i++)
    {
        double v_minL = state_l.prim(i, int(primvars::pvx1 + CalcDir)) - c_sl(i);
        double v_maxL = state_l.prim(i, int(primvars::pvx1 + CalcDir)) + c_sl(i);
        double v_minR = state_r.prim(i, int(primvars::pvx1 + CalcDir)) - c_sr(i);
        double v_maxR = state_r.prim(i, int(primvars::pvx1 + CalcDir)) + c_sr(i);
        double alphaL = std::min(v_minL, v_minR);
        double alphaR = std::max(v_maxL, v_maxR);
        maxalpha = std::max(maxalpha, std::max(std::abs(alphaL), std::abs(alphaR)));
        /*flux(i, (int)consvars::cdens) = (alphaR*flux(i, (int)consvars::cdens) + alphaL*flux(i+1, (int)consvars::cdens) -
         alphaL*alphaR*(cons(i+1, (int)consvars::cdens) - cons(i, (int)consvars::cdens)))/(alphaL + alphaR);
        flux(i, MomRange) = (alphaR*flux(i, MomRange) + alphaL*flux(i+1, MomRange) -
         alphaL*alphaR*(cons(i+1, MomRange) - cons(i, MomRange)))/(alphaL + alphaR);
        flux(i, (int)consvars::cenergy) = (alphaR*flux(i, (int)consvars::cenergy) + alphaL*flux(i+1, (int)consvars::cenergy) -
         alphaL*alphaR*(cons(i+1, (int)consvars::cenergy) - cons(i, (int)consvars::cenergy)))/(alphaL + alphaR);*/
        if (alphaL <= 0.0)
        {
            if (alphaR < 0.0) state_l.flux(i, Range::all()) = state_r.flux(i, Range::all());
            else
            {
                state_l.flux(i, Range::all()) = (alphaR*state_l.flux(i, Range::all()) - alphaL*state_r.flux(i, Range::all()) +
                 alphaL*alphaR*(state_r.cons(i, Range::all()) - state_l.cons(i, Range::all())))/(alphaR - alphaL);
            }
        }
        if (std::isnan(sum(state_l.flux(i, Range::all()))))
        {
            std::ostringstream err;
            err << "HLL: NaN in flux array at index " << i;
            throw std::runtime_error(err.str());
        }
    }
    return maxalpha;
}

/*double hll::Riemann(State1D& state_l, State1D& state_r, Axis CalcDir, EOS& eos)
{
    //std::cout << "Prim:" << prim << std::endl;
    Array<double, 1> c_sl(state_l.flux.extent(0));
    Array<double, 1> c_sr(state_r.flux.extent(0));
    eos.SoundSpeedSq(c_sl, state_l.prim);
    eos.SoundSpeedSq(c_sr, state_l.prim);
    c_sl = sqrt(c_sl);
    c_sr = sqrt(c_sr);
    double maxalpha = 0.0;
    //std::cout << "C_s: " << c_s << std::endl;
    double v_minL = prim(0, int(primvars::pvx1 + to_idx(CalcDir))) - c_s(0);
    double v_maxL = prim(0, int(primvars::pvx1 + to_idx(CalcDir))) + c_s(0);
    //std::cout << "vminL: " << v_minL << " vmaxL: " << v_maxL << std::endl;
    for (int i = 0 ; i < flux.extent(0) - 1; i++)
    {
        double v_minR = prim(i+1, int(primvars::pvx1 + to_idx(CalcDir))) - c_s(i+1);
        double v_maxR = prim(i+1, int(primvars::pvx1 + to_idx(CalcDir))) + c_s(i+1);
        double alphaL = std::min(v_minL, v_minR);
        double alphaR = std::max(v_maxL, v_maxR);
        maxalpha = std::max(maxalpha, std::max(std::fabs(alphaL), std::fabs(alphaR)));
        /*flux(i, (int)consvars::cdens) = (alphaR*flux(i, (int)consvars::cdens) + alphaL*flux(i+1, (int)consvars::cdens) -
         alphaL*alphaR*(cons(i+1, (int)consvars::cdens) - cons(i, (int)consvars::cdens)))/(alphaL + alphaR);
        flux(i, MomRange) = (alphaR*flux(i, MomRange) + alphaL*flux(i+1, MomRange) -
         alphaL*alphaR*(cons(i+1, MomRange) - cons(i, MomRange)))/(alphaL + alphaR);
        flux(i, (int)consvars::cenergy) = (alphaR*flux(i, (int)consvars::cenergy) + alphaL*flux(i+1, (int)consvars::cenergy) -
         alphaL*alphaR*(cons(i+1, (int)consvars::cenergy) - cons(i, (int)consvars::cenergy)))/(alphaL + alphaR);*/
        /*if (alphaL <= 0.0)
        {
            if (alphaR < 0.0) flux(i, Range::all()) = flux(i+1, Range::all());
            else
            {
                flux(i, Range::all()) = (alphaR*flux(i, Range::all()) - alphaL*flux(i+1, Range::all()) +
                 alphaL*alphaR*(cons(i+1, Range::all()) - cons(i, Range::all())))/(alphaR - alphaL);
            }
        }
        v_minL = v_minR;
        v_maxL = v_maxR;
        if (std::isnan(sum(flux(i, Range::all()))))
        {
            std::ostringstream err;
            err << "HLL: NaN in flux array at index " << i;
            throw std::runtime_error(err.str());
        }
    }
    return maxalpha;
}*/
