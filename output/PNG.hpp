#ifndef HC_PNG_H
#define HC_PNG_H

#include <pngwriter.h>
#include <Output.hpp>
#include "../misc/colortable.hpp"

class PNG : public Output
{
    public:
        PNG(std::size_t n, double d, std::size_t nd, std::vector<std::pair<std::string, std::size_t>> v, Axis a, bool proj, bool lp,
            const std::string& cname, bool rev=false, bool aut=true, double minv=0.0, double maxv=0.0, bool cbar=true) :
            Output(n, d, nd, v), ax(a), projection(proj), logplot(lp), autominmax(aut),
            minval(minv), maxval(maxv), CTab(ColorTable(0.0, 1.0, cname, rev)), colorbar(cbar)
        {
            assert(("PNG output not available in 1D!", this->ndim > 1));
            if (this->ndim == 2)
            {
                this->projection = false;
                this->ax = Axis_x3;
            }
        };
        PNG(std::size_t n, double d, std::size_t nd, std::vector<std::pair<std::string, std::size_t>> v, Axis a, bool proj, bool lp,
            ColorTableType ctyp, bool aut=true, double minv=0.0, double maxv=0.0, bool cbar=true) :
            Output(n, d, nd, v), ax(a), projection(proj), logplot(lp), autominmax(aut),
            minval(minv), maxval(maxv), CTab(ColorTable(0.0, 1.0, ctyp)), colorbar(cbar)
        {
            assert(("PNG output not available in 1D!", this->ndim > 1));
            if (this->ndim == 2)
            {
                this->projection = false;
                this->ax = Axis_x3;
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

