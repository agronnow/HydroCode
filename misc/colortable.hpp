#ifndef COLORTABLE_H
#define COLORTABLE_H
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>

/*
   Return a RGB colour value given a scalar v in the range [vmin,vmax]
   In this case each colour component ranges from 0 (no contribution) to
   1 (fully saturated), modifications for other ranges is trivial.
   The colour is clipped at the end of the scales if v is outside
   the range [vmin,vmax]
*/

struct RGB
{
  double r,g,b;
};

enum ColorTableType {TabFile, Jet, GammaII};

class ColorTable
{
public:
  ColorTable(double vmi, double vma, ColorTableType typ) : vmin(vmi), vmax(vma), Type(typ) {};
  ColorTable(double vmi, double vma, const std::string& fname, bool rev);
  RGB GetColor(double);
private:
  const double vmin;
  const double vmax;
  ColorTableType Type;
  std::vector<double> table_r;
  std::vector<double> table_g;
  std::vector<double> table_b;
  bool Reverse;
};

#endif // COLORTABLE_H
