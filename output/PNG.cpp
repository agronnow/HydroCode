#include "PNG.hpp"

PNG::~PNG()
{
    //dtor
}

void PNG::OutputSnapshot(Grid& grid, StepInfo& info)
{
    Array<double, 4> vars = grid.GetPrimitiveVars();
    std::size_t nx, ny;
    if (this->ax == Axis_x1) {nx = vars.extent(1);ny = vars.extent(2);}
    else if (this->ax == Axis_x2) {nx = vars.extent(0);ny = vars.extent(2);}
    else {nx = vars.extent(0);ny = vars.extent(1);}
    for (auto& outputvar : this->outputvars)
    {
        Array<double, 3> curvar = vars(Range::all(), Range::all(), Range::all(), outputvar.second);
        std::ostringstream fname;
        fname << outputvar.first << "." << std::setfill('0') << std::setw(4) << this->nout << ".dbl";

        int cbarwidth = 0;
        if (this->colorbar) cbarwidth = 100;
        pngwriter png(nx-1 + cbarwidth, ny-1, 0, fname.str().c_str());

        Array<double, 2> proj;
        if (this->projection)
        {
            if (this->ax == Axis_x1) proj = mean(curvar(tensor::j, tensor::k, tensor::i), tensor::k);
            else if (this->ax == Axis_x2) proj = mean(curvar(tensor::i, tensor::k, tensor::j), tensor::k);
            else if (this->ax == Axis_x3) proj = mean(curvar, tensor::k);

            if (autominmax)
            {
                this->minval = min(curvar);
                this->maxval = max(curvar);
            }
        }
        double logrange = 0.0;
        if (this->logplot)
        {
            if (this->minval < 1.e-14) this->minval = 1.e-14;
            logrange = log10(this->minval/this->maxval);
        }
        std::cout << "Minval: " << this->minval << std::endl;
        std::cout << "Maxval: " << this->maxval << std::endl;
        std::cout << "logrange: " << logrange << std::endl;

        double maxcol = 0.0;
        for (int i=0;i < nx;i++)
        {
            for (int j=0;j < ny;j++)
            {
                double curval = 0;
                if (this->projection) curval = proj(i, j);
                else if (this->ax == Axis_x1) curval = curvar(curvar.extent(0)/2, i, j, (int)outputvar.second);
                else if (this->ax == Axis_x2) curval = curvar(i, curvar.extent(1)/2, j, (int)outputvar.second);
                else if (this->ax == Axis_x3) curval = curvar(i, j, curvar.extent(2)/2, (int)outputvar.second);
                double col;
                if (curval < this->minval) curval = this->minval;
                else if (curval > this->maxval) curval = this->maxval;
                if (this->logplot)
                {
                    col = log10(this->minval/curval)/logrange;
                }
                else col = (curval-this->minval)/(this->maxval-this->minval);
                if (col > maxcol) maxcol = col;
                RGB rgb = CTab.GetColor(col);
                png.plot(i, j, rgb.r, rgb.g, rgb.b);
            }
        }

        if (this->colorbar)
        {
            //Draw colorbar on right
            png.filledsquare(nx-1, 0, nx-1+200, ny-1, 1.0, 1.0, 1.0);
            png.square(nx+15, 15, nx-15+48, ny-15, 0.0, 0.0, 0.0);
            int delta_label = (ny-16)/10;
            char fontpath[255] = "/usr/share/fonts/liberation/LiberationSans-Regular.ttf";
            for (std::size_t y = 16; y <= ny - 16; y++)
            {
                double col = (y-16)/(ny-32.0);
                RGB rgb = CTab.GetColor(col);
                png.line(nx+16, y, nx-16+48, y, rgb.r, rgb.g, rgb.b);
                if (y % delta_label == 0)
                {
                    double label;
                    if (this->logplot) label = (log10(maxval)-log10(minval))*(y-16)/(ny-32.0) + log10(minval);
                    else label = (maxval-minval)*(y-16)/(ny-32.0) + minval;
                    char clabel[15];
                    sprintf(clabel, "%4.2f", label);
                    png.plot_text(fontpath, 12, nx-16+50, y, 0.0, clabel, 0.0, 0.0, 0.0);
                }
            }
        }
        png.close();
        std::cout << "Maxcol: " << maxcol << std::endl;
    }
    this->nout++;
}

