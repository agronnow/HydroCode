#include "colortable.hpp"

ColorTable::ColorTable(double vmi, double vma, const std::string& fname, bool rev) : vmin(vmi), vmax(vma), Type(TabFile), Reverse(rev)
{
    std::ifstream cmapfile(fname);
    if (cmapfile.is_open())
    {
      while(!cmapfile.eof())
      {
        double r, g, b;
        cmapfile >> r >> g >> b;
        this->table_r.push_back(r);
        this->table_g.push_back(g);
        this->table_b.push_back(b);
      }
    }
    else
    {
      std::cout << "Failed to open color table file: " << fname << std::endl;
    }
}

RGB ColorTable::GetColor(double v)
{
   RGB c = {1.0,1.0,1.0}; // white

   if (v < vmin) v = vmin;
   if (v > vmax) v = vmax;
   double dv = vmax - vmin;

   switch(this->Type)
   {
       case TabFile:
       {
         int vidx = std::round((this->table_r.size()-1.0)*(v - vmin)/(vmax-vmin));
         if (this->Reverse) vidx = this->table_r.size() -  vidx - 1;
         c.r = table_r[vidx];
         c.g = table_g[vidx];
         c.b = table_b[vidx];
         break;
       }
       case Jet:
       {
         if (v < (vmin + 0.25 * dv)) {
           c.r = 0;
           c.g = 4 * (v - vmin) / dv;
         } else if (v < (vmin + 0.5 * dv)) {
           c.r = 0;
           c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
         } else if (v < (vmin + 0.75 * dv)) {
           c.r = 4 * (v - vmin - 0.5 * dv) / dv;
           c.b = 0;
         } else {
           c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
           c.b = 0;
         }
         break;
       }
       case GammaII:
       {
         double vidx = 255.0*(v - vmin)/(vmax-vmin);
         if (vidx < 48) c.r = 0;
         else if (vidx < 65) c.r = 4.8*vidx-226;
         else if (vidx < 79) c.r = -0.12*vidx+89;
         else if (vidx < 98) c.r = 5.0*vidx-316;
              else if (vidx < 109) c.r = 5.28*vidx-343;
         else if (vidx < 110) c.r = 22.0*vidx-2165;
         else if (vidx < 162) c.r = 255.0;
         else if (vidx < 174) c.r = -7.7*vidx+1503;
         else if (vidx < 192) c.r = 5.1*vidx-725;
         else c.r = 255.0;

         if (vidx < 113) c.g = 0;
         else if (vidx < 145) c.g = 5.1*vidx-575;
         else if (vidx < 176) c.g = 163.0;
         else if (vidx < 191) c.g = 6.15*vidx-920;
         else c.g = 255.0;

         if (vidx < 4) c.b = 5.0*vidx;
         else if (vidx < 49) c.b = 5.2*vidx;
         else if (vidx < 95) c.b = -5.4*vidx+519;
         else if (vidx < 128) c.b = 0;
         else if (vidx < 145) c.b = 4.9*vidx-628;
         else if (vidx < 159) c.b = -6.0*vidx+953;
         else if (vidx < 176) c.b = 0;
         else c.b = 3.2*vidx-563;

         c.r /= 255;
         c.g /= 255;
         c.b /= 255;
       }
   }
   return c;
}

