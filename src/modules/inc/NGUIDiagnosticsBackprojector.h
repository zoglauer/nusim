/*
 * NGUIDiagnosticsBackprojector.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIDiagnosticsBackprojector__
#define __NGUIDiagnosticsBackprojector__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TROOT.h>
#include <TVirtualX.h>
#include <TGWindow.h>
#include <TObjArray.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TString.h>
#include <TGClient.h>
#include <TRootEmbeddedCanvas.h>
#include <TH1.h>
#include <TH2.h>

// MEGAlib libs:
#include "NGlobal.h"
#include "MGUIERBList.h"

// NuSTAR libs
#include "NModule.h"
#include "NTime.h"
#include "NGUIDiagnostics.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIDiagnosticsBackprojector : public NGUIDiagnostics
{
  // public Session:
 public:
  //! Default constructor
  NGUIDiagnosticsBackprojector(double PixelSize);
  //! Default destructor
  virtual ~NGUIDiagnosticsBackprojector();

  //! Set the detector parameters for improved display
  void SetDetectorParameters(const MVector& Center, const MVector& Si, const int NPixelsX, const int NPixelsY); 

  //! Set the time
  void SetTime(const NTime Time) { m_Time = Time; }

  //! Set the initial pointing 
  void SetInitialPointing(double Ra, double Dec);

  //! The creation part which gets overwritten
  virtual void Create();

  //! Update the frame
  virtual void Update();

  //! Add data to the backprojection histogram
  void AddBackprojection(double RA, double DEC);

  //! Add data to the energy histogram
  void AddEnergy(double Energy);

  // protected methods:
 protected:


  // protected members:
 protected:

  // private members:
 private:
  //! Backprojection canvas
  TRootEmbeddedCanvas* m_BackprojectionCanvas;
  //! Backprojection histogram
  TH2D* m_Backprojection;
  //! Energy canvas
  TRootEmbeddedCanvas* m_EnergyCanvas;
  //! Energy histogram
  TH1D* m_Energy;
  //! Normalized energy histogram
  TH1D* m_NormalizedEnergy;
    
  //! Initial pointing RA
  double m_InitialRa;
  //! Initial pointing DEC
  double m_InitialDec;

  //! Store all RA values
  vector<double> m_Ra;
  //! Store all DEC value;
  vector<double> m_Dec;
  
  //! The energy range
  double m_EnergyWidth;

  //! The detection area
  double m_Area;
  
  //! The observation time
  NTime m_Time;

  //! True if the window is fixed
  bool m_FixedSize;
  //! True if a resize is required
  bool m_NeedsResize;
  
  //! lower limit on Ra
  double m_MinRa;
  //! upper limit on Ra;
  double m_MaxRa;
  //! The default bin size in Ra
  double m_BinSizeRa;
  //! lower limit on Dec
  double m_MinDec;
  //! upper limit on Dec
  double m_MaxDec;
  //! The default bin size in Dec
  double m_BinSizeDec;

  //! The maximum number of allowed bins per direction
  double m_MaxBins;
  
#ifdef ___CINT___
 public:
  ClassDef(NGUIDiagnosticsBackprojector, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
