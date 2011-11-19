/*
 * MFunction3D.cxx
 *
 *
 * Copyright (C) 2008-2010 by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// MFunction3D
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MFunction3D.h"

// Standard libs:
#include <algorithm>
#include <limits>
using namespace std;

// ROOT libs:
#include "TH3.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TSystem.h"

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MFunction3D)
#endif


////////////////////////////////////////////////////////////////////////////////


const unsigned int MFunction3D::c_InterpolationUnknown  = 0;
const unsigned int MFunction3D::c_InterpolationConstant = 1;
const unsigned int MFunction3D::c_InterpolationLinear   = 2;


////////////////////////////////////////////////////////////////////////////////


MFunction3D::MFunction3D() 
  : m_InterpolationType(c_InterpolationUnknown)
{
  // Construct an instance of MFunction3D
}


////////////////////////////////////////////////////////////////////////////////


MFunction3D::MFunction3D(const MFunction3D& F)
{
  // Copy-construct an instance of MFunction3D

  m_InterpolationType = F.m_InterpolationType;

  m_X = F.m_X;
  m_Y = F.m_Y;
  m_Z = F.m_Z;
  m_V = F.m_V;
  m_Maximum = F.m_Maximum;
}


////////////////////////////////////////////////////////////////////////////////


MFunction3D::~MFunction3D()
{
  // Delete this instance of MFunction3D
}


////////////////////////////////////////////////////////////////////////////////


const MFunction3D& MFunction3D::operator=(const MFunction3D& F)
{
  // Copy-construct an instance of MFunction3D

  m_InterpolationType = F.m_InterpolationType;

  m_X = F.m_X;
  m_Y = F.m_Y;
  m_Z = F.m_Z;
  m_V = F.m_V;
  m_Maximum = F.m_Maximum;

  return *this;
}


////////////////////////////////////////////////////////////////////////////////


bool MFunction3D::Set(const TString FileName, const TString KeyWord,
                      const unsigned int InterpolationType)
{
  // Set the basic data, load the file and parse it

  m_InterpolationType = InterpolationType;

  MParser Parser;

  if (Parser.Open(FileName, MFile::c_Read) == false) {
    mout<<"Unable to open file "<<FileName<<endl;
    return false;
  }

  // Round one: Look for axis and interpolation type:
  for (unsigned int i = 0; i < Parser.GetNLines(); ++i) {
    if (Parser.GetTokenizerAt(i)->GetNTokens() == 0) continue;
    if (Parser.GetTokenizerAt(i)->IsTokenAt(0, "XA") == true) {
      if (Parser.GetTokenizerAt(i)->GetNTokens() >= 2) {
        m_X = Parser.GetTokenizerAt(i)->GetTokenAtAsDoubleVector(1);
      } else {
        mout<<"In the function defined by: "<<FileName<<endl;
        mout<<"XA keyword has not enough parameters!"<<endl;
        return false;
      }
    }
    if (Parser.GetTokenizerAt(i)->IsTokenAt(0, "YA") == true) {
      if (Parser.GetTokenizerAt(i)->GetNTokens() >= 2) {
        m_Y = Parser.GetTokenizerAt(i)->GetTokenAtAsDoubleVector(1);
      } else {
        mout<<"In the function defined by: "<<FileName<<endl;
        mout<<"YA keyword has not enough parameters!"<<endl;
        return false;
      }
    }
    if (Parser.GetTokenizerAt(i)->IsTokenAt(0, "ZA") == true) {
      if (Parser.GetTokenizerAt(i)->GetNTokens() >= 2) {
        m_Z = Parser.GetTokenizerAt(i)->GetTokenAtAsDoubleVector(1);
      } else {
        mout<<"In the function defined by: "<<FileName<<endl;
        mout<<"YA keyword has not enough parameters!"<<endl;
        return false;
      }
    }
    if (Parser.GetTokenizerAt(i)->IsTokenAt(0, "IP") == true) {
      if (Parser.GetTokenizerAt(i)->GetNTokens() == 2) {
        if (Parser.GetTokenizerAt(i)->GetTokenAt(1) == "LIN") {
          m_InterpolationType = c_InterpolationLinear;
        } else {
          mout<<"In the function defined by: "<<FileName<<endl;
          mout<<"Unknown interpolation type!"<<endl;
          return false;
        }
      } else {
        mout<<"In the function defined by: "<<FileName<<endl;
        mout<<"IP keyword incorrect!"<<endl;
        return false;
      }
    }
  }
  m_V.clear();
  m_V.resize(m_X.size()*m_Y.size()*m_Z.size());


  // Sanity checks:

  // We need at least two bins in x and y direction:
  if (m_X.size() < 2) {
    mout<<"In the function defined by: "<<FileName<<endl;
    mout<<"You need at least 2 x-bins!"<<endl;
    return false;
  }
  if (m_Y.size() < 2) {
    mout<<"In the function defined by: "<<FileName<<endl;
    mout<<"You need at least 2 y-bins!"<<endl;
    return false;
  }
  if (m_Z.size() < 2) {
    mout<<"In the function defined by: "<<FileName<<endl;
    mout<<"You need at least 2 z-bins!"<<endl;
    return false;
  }

  // Are m_X in increasing order?
  for (unsigned int i = 0; i < m_X.size()-1; ++i) {
    if (m_X[i] >= m_X[i+1]) {
      mout<<"In the function defined by: "<<FileName<<endl;
      mout<<"x values are not in increasing order!"<<endl;
      return false;
    }
  }
  // Are m_Y in increasing order?
  for (unsigned int i = 0; i < m_Y.size()-1; ++i) {
    if (m_Y[i] >= m_Y[i+1]) {
      mout<<"In the function defined by: "<<FileName<<endl;
      mout<<"y values are not in increasing order!"<<endl;
      return false;
    }
  }
  // Are m_Z in increasing order?
  for (unsigned int i = 0; i < m_Z.size()-1; ++i) {
    if (m_Z[i] >= m_Z[i+1]) {
      mout<<"In the function defined by: "<<FileName<<endl;
      mout<<"z values are not in increasing order!"<<endl;
      return false;
    }
  }

  // Round two: Parse the actual data
  for (unsigned int i = 0; i < Parser.GetNLines(); ++i) {
    if (Parser.GetTokenizerAt(i)->GetNTokens() == 0) continue;
    if (Parser.GetTokenizerAt(i)->IsTokenAt(0, KeyWord) == true) {
      if (Parser.GetTokenizerAt(i)->GetNTokens() != 5) {
        mout<<"In the function defined by: "<<FileName<<endl;
        mout<<"Wrong number of arguments!"<<endl;
        return false;
      } else {
//         int x = Parser.GetTokenizerAt(i)->GetTokenAtAsInt(1);
//         int y = Parser.GetTokenizerAt(i)->GetTokenAtAsInt(2);
//         int z = Parser.GetTokenizerAt(i)->GetTokenAtAsInt(3);
//         if (x < 0 || x > (int) m_X.size()) {
//           mout<<"In the function defined by: "<<FileName<<endl;
//           mout<<"X-axis out of bounds!"<<endl;
//           return false;          
//         }
//         if (y < 0 || y > (int) m_Y.size()) {
//           mout<<"In the function defined by: "<<FileName<<endl;
//           mout<<"Y-axis out of bounds!"<<endl;
//           return false;                    
//         }
//         if (z < 0 || z > (int) m_Z.size()) {
//           mout<<"In the function defined by: "<<FileName<<endl;
//           mout<<"Z-axis out of bounds!"<<endl;
//           return false;                    
//         }

        double x = Parser.GetTokenizerAt(i)->GetTokenAtAsDouble(1);
        double y = Parser.GetTokenizerAt(i)->GetTokenAtAsDouble(2);
        double z = Parser.GetTokenizerAt(i)->GetTokenAtAsDouble(3);

        int xPosition = -1; 
        for (unsigned int j = 0; j < m_X.size(); ++j) {
          if (m_X[j] > x) {
            break;
          } 
          xPosition = (int) j;
        }
        if (xPosition < 0) xPosition = 0;
        if (xPosition > (int) m_X.size()-1) xPosition = (int) (m_X.size()-1);

        int yPosition = -1; 
        for (unsigned int j = 0; j < m_Y.size(); ++j) {
          if (m_Y[j] > y) {
            break;
          } 
          yPosition = (int) j;
        }
        if (yPosition < 0) yPosition = 0;
        if (yPosition > (int) m_Y.size()-1) yPosition = (int) (m_Y.size()-1);

        int zPosition = -1; 
        for (unsigned int j = 0; j < m_Z.size(); ++j) {
          if (m_Z[j] > z) {
            break;
          } 
          zPosition = (int) j;
        }
        if (zPosition < 0) zPosition = 0;
        if (zPosition > (int) m_Z.size()-1) zPosition = (int) (m_Z.size()-1);


        if (xPosition < 0 || xPosition > (int) m_X.size()) {
          mout<<"In the function defined by: "<<FileName<<endl;
          mout<<"X-axis out of bounds!"<<endl;
          return false;          
        }
        if (yPosition < 0 || yPosition > (int) m_Y.size()) {
          mout<<"In the function defined by: "<<FileName<<endl;
          mout<<"Y-axis out of bounds!"<<endl;
          return false;                    
        }
        if (zPosition < 0 || zPosition > (int) m_Z.size()) {
          mout<<"In the function defined by: "<<FileName<<endl;
          mout<<"Z-axis out of bounds!"<<endl;
          return false;                    
        }

        m_V[xPosition + m_X.size()*yPosition + m_X.size()*m_Y.size()*zPosition] = Parser.GetTokenizerAt(i)->GetTokenAtAsDouble(4);
      }
    }
  }

  // Determine interapolation type:
  if (m_X.size() > 1 && m_InterpolationType == c_InterpolationConstant) {
    m_InterpolationType = c_InterpolationLinear;
  } 

  if (m_X.size() == 1) {
    m_InterpolationType = c_InterpolationConstant;
  } 
  
  if (m_X.size() == 0) {
    m_InterpolationType = c_InterpolationConstant;
    m_X.push_back(0);
    m_Y.push_back(0);
    m_Z.push_back(0);
  } 

  // Clean up:
  m_Maximum = g_DoubleNotDefined;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool MFunction3D::Set(const vector<double>& X, const vector<double>& Y, const vector<double>& Z, const vector<double>& V, unsigned int InterpolationType)
{
  //! Set the basic data from a 1D ResponseMatrix

  m_X = X;
  m_Y = Y;
  m_Z = Z;
  
  m_V = V;
  
  m_InterpolationType = InterpolationType;

  // Clean up:
  m_Maximum = g_DoubleNotDefined;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::ScaleX(double Scaler)
{
  // Multiple the x-axis by some value

   for (unsigned int i = 0; i < m_X.size(); ++i) {
    m_X[i] *= Scaler;
  }  

  // We clear the cumulative function:
  m_Maximum = g_DoubleNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::RescaleX(double XMin, double XMax)
{
  // Scale the x-axis to it's new min and max

  double Zero = m_X[0];
  double Scale = (XMax - XMin)/(m_X.back() - m_X.front());
  
  m_X[0] = XMin;
  for (unsigned int i = 1; i < m_X.size(); ++i) {
    m_X[i] = m_X[0] + Scale*(m_X[i]-Zero);
  }  

  // We clear the cumulative function:
  m_Maximum = g_DoubleNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::ScaleY(double Scaler)
{
  // Multiple the y-axis by some value

   for (unsigned int i = 0; i < m_Y.size(); ++i) {
    m_Y[i] *= Scaler;
  }  

  // We clear the cumulative function:
  m_Maximum = g_DoubleNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::RescaleY(double YMin, double YMax)
{
  // Scale the y-axis to it's new min and max

  double Zero = m_Y[0];
  double Scale = (YMax - YMin)/(m_Y.back() - m_Y.front());
  
  m_Y[0] = YMin;
  for (unsigned int i = 1; i < m_Y.size(); ++i) {
    m_Y[i] = m_Y[0] + Scale*(m_Y[i]-Zero);
  }  

  // We clear the cumulative function:
  m_Maximum = g_DoubleNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::InvertZ()
{
  // Invert the z-content

  vector<double> Temp = m_V;

  for (unsigned int x = 0; x < m_X.size(); ++x) {
    for (unsigned int y = 0; y < m_Y.size(); ++y) {
      for (unsigned int z = 0; z < m_Z.size(); ++z) {
        unsigned int z_inv = m_Z.size() - z - 1;
        m_V[x + m_X.size()*y + m_X.size()*m_Y.size()*z] = Temp[x + m_X.size()*y + m_X.size()*m_Y.size()*z_inv];
      }
    }
  }


  // Clean up:
  m_Maximum = g_DoubleNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::ScaleZ(double Scaler)
{
  // Scale the z-axis by some value

  if (Scaler <= 0) {
    merr<<"Scaler needs to bne positive"<<show;
    return;
  }

  for (unsigned int i = 0; i < m_Z.size(); ++i) {
    m_Z[i] *= Scaler;
  } 

  // Clean up:
  m_Maximum = g_DoubleNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::RescaleZ(double ZMin, double ZMax)
{
  // Scale the z-axis to it's new min and max

  double Zero = m_Z[0];
  double Scale = (ZMax - ZMin)/(m_Z.back() - m_Z.front());
  
  m_Z[0] = ZMin;
  for (unsigned int i = 1; i < m_Z.size(); ++i) {
    m_Z[i] = m_Z[0] + Scale*(m_Z[i]-Zero);
  }  

  // We clear the cumulative function:
  m_Maximum = g_DoubleNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::ScaleV(double Scaler)
{
  // Scale the value-content by some value

  for (unsigned int i = 0; i < m_V.size(); ++i) {
    m_V[i] *= Scaler;
  } 

  // Clean up:
  m_Maximum = g_DoubleNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::Eval(double x, double y, double z) const
{
  // Evalute the function

  if (m_InterpolationType == c_InterpolationUnknown) {
    merr<<"Unknown fit type: "<<c_InterpolationUnknown<<endl;
    return 0;
  }

  if (m_InterpolationType == c_InterpolationConstant) {
    return m_Z[0];
  } else if (m_InterpolationType == c_InterpolationLinear) {

    // Get Position:
    int xPosition = -1; 
    for (unsigned int i = 0; i < m_X.size(); ++i) {
      if (m_X[i] > x) {
        break;
      } 
      xPosition = (int) i;
    }

    int yPosition = -1; 
    for (unsigned int i = 0; i < m_Y.size(); ++i) {
      if (m_Y[i] > y) {
        break;
      } 
      yPosition = (int) i;
    }

    int zPosition = -1; 
    for (unsigned int i = 0; i < m_Z.size(); ++i) {
      if (m_Z[i] > z) {
        break;
      } 
      zPosition = (int) i;
    }

    //cout<<x<<" --> "<<xPosition<<"|"<<y<<" --> "<<yPosition<<"|"<<z<<" --> "<<zPosition<<": "<<m_V[xPosition + m_X.size()*yPosition + m_X.size()*m_Y.size()*zPosition]<<endl;

    if (xPosition < 0) xPosition = 0;
    if (yPosition < 0) yPosition = 0;
    if (zPosition < 0) zPosition = 0;
    if (xPosition >= (int) m_X.size()-1) xPosition = (int) (m_X.size()-2);
    if (yPosition >= (int) m_Y.size()-1) yPosition = (int) (m_Y.size()-2);
    if (zPosition >= (int) m_Z.size()-1) zPosition = (int) (m_Z.size()-2);

//     return m_V[xPosition + m_X.size()*yPosition + m_X.size()*m_Y.size()*zPosition];

    double m, t;

    // Interpolate at ymin/zmin
    m = (m_V[(xPosition+1) + m_X.size()*(yPosition) + m_X.size()*m_Y.size()*(zPosition)] - m_V[(xPosition) + m_X.size()*(yPosition) + m_X.size()*m_Y.size()*(zPosition)]) 
      / (m_X[xPosition+1] - m_X[xPosition]);
    t = m_V[(xPosition) + m_X.size()*(yPosition) + m_X.size()*m_Y.size()*(zPosition)] - m*m_X[xPosition];
    double V_yminzmin = m*x + t;

    // Interpolate at ymax/zmin
    m = (m_V[(xPosition+1) + m_X.size()*(yPosition+1) + m_X.size()*m_Y.size()*(zPosition)] - m_V[(xPosition) + m_X.size()*(yPosition+1) + m_X.size()*m_Y.size()*(zPosition)]) 
      / (m_X[xPosition+1] - m_X[xPosition]);
    t = m_V[(xPosition) + m_X.size()*(yPosition+1) + m_X.size()*m_Y.size()*(zPosition)] - m*m_X[xPosition];
    double V_ymaxzmin = m*x + t;

    // Interpolate at ymin/zmax
    m = (m_V[(xPosition+1) + m_X.size()*(yPosition) + m_X.size()*m_Y.size()*(zPosition+1)] - m_V[(xPosition) + m_X.size()*(yPosition) + m_X.size()*m_Y.size()*(zPosition+1)]) 
      / (m_X[xPosition+1] - m_X[xPosition]);
    t = m_V[(xPosition) + m_X.size()*(yPosition) + m_X.size()*m_Y.size()*(zPosition+1)] - m*m_X[xPosition];
    double V_yminzmax = m*x + t;

    // Interpolate at ymax/zmax
    m = (m_V[(xPosition+1) + m_X.size()*(yPosition+1) + m_X.size()*m_Y.size()*(zPosition+1)] - m_V[(xPosition) + m_X.size()*(yPosition+1) + m_X.size()*m_Y.size()*(zPosition+1)]) 
      / (m_X[xPosition+1] - m_X[xPosition]);
    t = m_V[(xPosition) + m_X.size()*(yPosition+1) + m_X.size()*m_Y.size()*(zPosition+1)] - m*m_X[xPosition];
    double V_ymaxzmax = m*x + t;


    // Interpolate at zmin
    m = (V_ymaxzmin - V_yminzmin) / (m_Y[yPosition+1] - m_Y[yPosition]);
    t = V_yminzmin - m*m_Y[yPosition];
    double V_zmin = m*y + t;

    // Interpolate at zmax
    m = (V_ymaxzmax - V_yminzmax) / (m_Y[yPosition+1] - m_Y[yPosition]);
    t = V_yminzmax - m*m_Y[yPosition];
    double V_zmax = m*y + t;


    // Final interpolation:
    m = (V_zmax - V_zmin) / (m_Z[zPosition+1] - m_Z[zPosition]);
    t = V_zmin - m*m_Z[zPosition];
    return m*z + t;

  } 

  return 0.0;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::GetRandom(double& x, double& y, double& z)
{
  // Return a random number distributed as the underlying function
  // The follwoing is accurate and safe version but rather slow...

  if (m_Maximum == g_DoubleNotDefined) {
    m_Maximum = GetVMax();
  }

  double v = 0;
  do {
    x = gRandom->Rndm()*(GetXMax() - GetXMin()) + GetXMin();
    y = gRandom->Rndm()*(GetYMax() - GetYMin()) + GetYMin();
    z = gRandom->Rndm()*(GetZMax() - GetZMin()) + GetZMin();

    v = Eval(x, y, z);
  } while (m_Maximum*gRandom->Rndm() > v);
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::GetXMin() const
{
  //! Get the minimum x-value

  return m_X.front();
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::GetXMax() const
{
  //! Get the maximum x-value

  return m_X.back();
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::GetYMin() const
{
  //! Get the minimum y-value

  return m_Y.front();
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::GetYMax() const
{
  //! Get the maximum y-value

  return m_Y.back();
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::GetZMax() const
{
  //! Get the maximum z-value

  return m_Z.back();
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::GetZMin() const
{
  //! Get the minimum z-value

  return m_Z.front();
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::GetVMin() const
{
  //! Get the minimum z-value

  double Min = numeric_limits<double>::max();
  for (unsigned int i = 0; i < m_V.size(); ++i) {
    if (m_V[i] < Min) Min = m_V[i];
  }

  return Min;
}


////////////////////////////////////////////////////////////////////////////////


double MFunction3D::GetVMax() const
{
  //! Get the maximum z-value

  double Max = -numeric_limits<double>::max();
  for (unsigned int i = 0; i < m_V.size(); ++i) {
    if (m_V[i] > Max) Max = m_V[i];
  }

  return Max;
}


////////////////////////////////////////////////////////////////////////////////


void MFunction3D::Plot()
{
  // Plot the function in a Canvas (diagnostics only)
  
  if (m_X.size() >= 2 && m_Y.size() >= 2 && m_Z.size() >= 2) {

    TH3D* Hist = new TH3D("Diagnostics", "Diagnostics", 50, m_X.front(), m_X.back(), 50, m_Y.front(), m_Y.back(), 50, m_Z.front(), m_Z.back());
    for (int bx = 1; bx <= Hist->GetXaxis()->GetNbins(); ++bx) {
      for (int by = 1; by <= Hist->GetYaxis()->GetNbins(); ++by) {
        for (int bz = 1; bz <= Hist->GetZaxis()->GetNbins(); ++bz) {
          Hist->SetBinContent(bx, by, bz, Eval(Hist->GetXaxis()->GetBinCenter(bx), Hist->GetYaxis()->GetBinCenter(by), Hist->GetZaxis()->GetBinCenter(bz)));
        }
      }
    }

    TCanvas* Canvas = new TCanvas("DiagnosticsCanvas", "DiagnosticsCanvas");
    Canvas->cd();
    Hist->Draw("colz");
    Canvas->Update();

    gSystem->ProcessEvents();

    for (unsigned int i = 0; i < 10; ++i) {
      gSystem->ProcessEvents();
      gSystem->Sleep(10);
    }

  } else {
    mout<<"Not enough data points for diagnostics: x="<<m_X.size()<<" y="<<m_Y.size()<<" z="<<m_Z.size()<<endl;
  }
}


// MFunction3D.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
