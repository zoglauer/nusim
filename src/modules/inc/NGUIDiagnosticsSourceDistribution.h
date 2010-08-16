/*
 * NGUIDiagnosticsSourceDistribution.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIDiagnosticsSourceDistribution__
#define __NGUIDiagnosticsSourceDistribution__


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


class NGUIDiagnosticsSourceDistribution : public NGUIDiagnostics
{
  // public Session:
 public:
  //! Default constructor
  NGUIDiagnosticsSourceDistribution();
  //! Default destructor
  virtual ~NGUIDiagnosticsSourceDistribution();

  //! The creation part which gets overwritten
  virtual void Create();

  //! Update the frame
  virtual void Update();

  //! Set the initial pointing 
  void SetInitialPointing(double Ra, double Dec);

  //! Add data to the initial direction histogram 
  void AddOrigin(double Ra, double Dec);
  //! Add data to the energy histogram
  void AddEnergy(double Energy);

  // protected methods:
 protected:


  // protected members:
 protected:

  // private members:
 private:
  //! Initial image canvas
  TRootEmbeddedCanvas* m_OriginCanvas;
  //! Initial histogram
  TH2D* m_Origin;

  //! Initial pointing RA
  double m_InitialRa;
  //! Initial pointing DEC
  double m_InitialDec;
  
  //! Energy canvas
  TRootEmbeddedCanvas* m_EnergyCanvas;
  //! Energy histogram
  TH1D* m_Energy;



#ifdef ___CINT___
 public:
  ClassDef(NGUIDiagnosticsSourceDistribution, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
