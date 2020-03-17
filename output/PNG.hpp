#ifndef HC_PNG_H
#define HC_PNG_H

#include <pngwriter.h>
#include <Output.hpp>
#include "../misc/colortable.hpp"
extern int g_NDim;

class PNG : public Output
{
    public:
        PNG(int n, double d, std::vector<std::pair<std::string, int>> v, Axis a, bool proj, bool lp,
            const std::string& cname, bool rev=false, bool aut=true, double minv=0.0, double maxv=0.0, bool cbar=true) :
            Output(n, d, v), ax(a), projection(proj), logplot(lp), autominmax(aut),
            minval(minv), maxval(maxv), CTab(ColorTable(0.0, 1.0, cname, rev)), colorbar(cbar)
        {
            assert(("PNG output not available in 1D!", g_NDim > 1));
            if (g_NDim == 2)
            {
                this->projection = false;
                this->ax = Axis::x3;
            }
        };
        PNG(int n, double d, std::vector<std::pair<std::string, int>> v, Axis a, bool proj, bool lp,
            ColorTableType ctyp, bool aut=true, double minv=0.0, double maxv=0.0, bool cbar=true) :
            Output(n, d, v), ax(a), projection(proj), logplot(lp), autominmax(aut),
            minval(minv), maxval(maxv), CTab(ColorTable(0.0, 1.0, ctyp)), colorbar(cbar)
        {
            assert(("PNG output not available in 1D!", g_NDim > 1));
            if (g_NDim == 2)
            {
                this->projection = false;
                this->ax = Axis::x3;
            }
        };
        virtual ~PNG();
        void OutputSnapshot(Grid&, StepInfo&) override;
        std::string GetFormat() override {return "PNG";}
    protected:
    private:
        Axis ax;
        bool projection;
        bool logplot;
        bool autominmax;
        double minval;
        double maxval;
        ColorTable CTab;
        bool colorbar;
};

#endif // HC_PNG_H

