/*
 * NPhaFile.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NPhaFile__
#define __NPhaFile__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TH1.h"

// NuSTAR libs:
#include "NGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NPhaFile
{ 
public:
  NPhaFile();
  ~NPhaFile();

  bool Load(TString FileName);
  
  bool SaveBackground(TH1D* ScaledSpectrum, double Scaler, TString Mode);
  bool SaveBackground(TH1D* ScaledSpectrum, double Scaler, TH1D* SpectrumSource, TString Mode);
  
  vector<double> m_Counts;
  TH1D* m_Spectrum;
  TString m_FileName;
};


#endif


////////////////////////////////////////////////////////////////////////////////
