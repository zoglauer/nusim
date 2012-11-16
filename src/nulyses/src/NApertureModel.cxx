/*
* NApertureModel.cxx
*
* Copyright (C) by the NuSTAR team.
* All rights reserved.
*
*/


////////////////////////////////////////////////////////////////////////////////
//
// NApertureModel
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NApertureModel.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NApertureModel)
#endif


////////////////////////////////////////////////////////////////////////////////


NApertureModel::NApertureModel()
{
  // Construct an instance of NApertureModel
}


////////////////////////////////////////////////////////////////////////////////


NApertureModel::~NApertureModel()
{
  // Delete this instance of NApertureModel
}


////////////////////////////////////////////////////////////////////////////////


bool NApertureModel::Initialize()
{
  if (m_SpectrumA0.Load("$(NUSIM)/resource/nulyses/aperbgdA_det0.pha") == false) return false;
  if (m_SpectrumA1.Load("$(NUSIM)/resource/nulyses/aperbgdA_det1.pha") == false) return false;
  if (m_SpectrumA2.Load("$(NUSIM)/resource/nulyses/aperbgdA_det2.pha") == false) return false;
  if (m_SpectrumA3.Load("$(NUSIM)/resource/nulyses/aperbgdA_det3.pha") == false) return false;
  if (m_SpectrumB0.Load("$(NUSIM)/resource/nulyses/aperbgdB_det0.pha") == false) return false;
  if (m_SpectrumB1.Load("$(NUSIM)/resource/nulyses/aperbgdB_det1.pha") == false) return false;
  if (m_SpectrumB2.Load("$(NUSIM)/resource/nulyses/aperbgdB_det2.pha") == false) return false;
  if (m_SpectrumB3.Load("$(NUSIM)/resource/nulyses/aperbgdB_det3.pha") == false) return false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


double NApertureModel::GetApertureShape(int detx, int dety, int Module)
{
  double p[] = { 1.0, 1.0, 0.0, 0.0, 1.28, -1.23, 4.55, 0.70 };
  //double p[] = { 1.8247618,1.8456988,6.5860895,6.1775471,-0.094825531,-0.26157041,26.617317,2.3922457 };

  // 70x70 mm polynomial fit, z-distance reduced to use optic backside
  double acoeff[6][6] = { 
    { 374459.,    -1396.94,     12.8261,    -2.60828,   0.0499416,-0.000233391 }, 
    {-968.233,     63.0611,    -31.1783,     1.33731,  -0.0210902, 0.000113852 }, 
    {-61.1686,    -19.9369,     2.79334,   -0.120849,  0.00194248,-1.03889e-05 }, 
    {1.84454,    0.559458,  -0.0719477,  0.00305963,-4.88927e-05, 2.58232e-07 },
    {-0.00628727, -0.00408784, 0.000572117,-2.29168e-05, 3.50316e-07,-1.74875e-09 },
    {-6.81786e-05,-1.16308e-07,-3.66437e-07, 4.21674e-11, 1.87926e-10,-2.18813e-12 }
  };


  double bcoeff[6][6] = {
    {     145375.,    -2845.75,    -203.809,     13.3728,   -0.202717, 0.000997269 }, 
    {    -6128.16,    -1170.23,     84.8699,    -2.24046,   0.0267836,-0.000120917 }, 
    {     112.675,     97.2683,    -8.47097,    0.255571, -0.00329352, 1.54470e-05 }, 
    {     6.11565,    -2.86178,    0.276812, -0.00896906, 0.000120047,-5.74844e-07 }, 
    {   -0.136972,   0.0356496, -0.00367424, 0.000123755,-1.68958e-06, 8.17812e-09 }, 
    { 0.000771634,-0.000159447, 1.72740e-05,-5.96966e-07, 8.25131e-09,-4.02023e-11 }
  };

  double coeff[6][6];
  double xp = 0;
  double yp = 0;
  double scl = 0;
  double off = 0;
  if (Module == 0) {
    for (int x = 0; x < 6; ++x) for (int y = 0; y < 6; ++y) coeff[x][y] = acoeff[x][y];
    xp=p[4];
    yp=p[5];
    scl=p[0];
    off=p[2];
  } else if (Module == 1) {
    for (int x = 0; x < 6; ++x) for (int y = 0; y < 6; ++y) coeff[x][y] = bcoeff[x][y];
    xp=p[6];
    yp=p[7];
    scl=p[1];
    off=p[3];   
  }
  
  double dx = 0.12096; // mm/(det1 pixel)
  double val = 0;
  for (int i= 0; i <= 5; ++i) { 
    for (int j = 0; j <= 5; ++j) {
      if (Module == 0) { 
        val += coeff[j][i]*pow(dety*dx+13+xp, i)*pow(detx*dx+13+yp, j);
      } else {
        val += coeff[j][i]*pow(dety*dx+13+xp, i)*pow(detx*dx+13+yp, j);
      }
    }
  }
      
  return (val*scl*3.1865e-5 + off)*0.01; // *dx*dx
}
 
  
// NApertureModel.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
  