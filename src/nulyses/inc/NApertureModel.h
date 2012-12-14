/*
 * NApertureModel.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NApertureModel__
#define __NApertureModel__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
#include <iostream>
#include <map>
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "MFile.h"
#include "NPhaFile.h"

// FITS libs:
#include "fitsio.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NApertureModel
{
public:
  NApertureModel();
  virtual ~NApertureModel();

  bool Initialize(TString ObsID, int Module);

  void Add(double Det1X, double Det1Y, double Energy);
  
  bool Analyze();
  
  TH1D* GetSubtractedSpectrum() { return m_SubtractedSpectrumAll; }

  
  static TH2D* s_Aperture;
  static double s_Scaler0;
  static double s_Scaler1;
  static double s_Scaler2;
  static double s_Scaler3;
  static bool s_CombinedFit;
  
public:
  double GetApertureShape(int detx, int dety, int Module);
  
private:
  int m_Module;
  TString m_ObsID;
  TString m_iID;
  TString m_ID;
  
  double m_SpectrumMin;
  double m_SpectrumMax;
  int m_SpectrumBins;
  
  double m_ApertureSpectrumMin;
  double m_ApertureSpectrumMax;
  int m_ApertureSpectrumBins;
  
  double m_PosMin;
  double m_PosMax;
  double m_ExcludeMin;
  double m_ExcludeMax;
  double m_Divider;
  int m_NPositionBins;
  int m_NPositionDataBins;

  TH2D* m_Positions;
  TH2D* m_SubtractedPositions;
  TH1D* m_SpectrumAll; 
  TH1D* m_SubtractedSpectrumAll; 
  TH1D* m_Spectrum0; 
  TH1D* m_Spectrum1; 
  TH1D* m_Spectrum2; 
  TH1D* m_Spectrum3; 
  
  NPhaFile m_SpectrumA0;
  NPhaFile m_SpectrumA1;
  NPhaFile m_SpectrumA2;
  NPhaFile m_SpectrumA3;
  NPhaFile m_SpectrumB0;
  NPhaFile m_SpectrumB1;
  NPhaFile m_SpectrumB2;
  NPhaFile m_SpectrumB3;
  
};

#endif


////////////////////////////////////////////////////////////////////////////////
