/*
 * NGUIDiagnosticsDetectorEffectsEngine.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIDiagnosticsDetectorEffectsEngine__
#define __NGUIDiagnosticsDetectorEffectsEngine__


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
#include <TH2.h>

// Standard libs:
#include <map>
using namespace std;

// MEGAlib libs:
#include "NGlobal.h"
#include "MGUIERBList.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIDiagnostics.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIDiagnosticsDetectorEffectsEngine : public NGUIDiagnostics
{
  // public Session:
 public:
  //! Default constructor
  NGUIDiagnosticsDetectorEffectsEngine();
  //! Default destructor
  virtual ~NGUIDiagnosticsDetectorEffectsEngine();

  //! Set the detector parameters for improved display
  void SetDetectorParameters(const MVector& Center, const MVector& Si, const int NPixelsX, const int NPixelsY); 

  //! The creation part which gets overwritten
  virtual void Create();

  //! Update the frame
  virtual void Update();

  //! Add data to the initial direction histogram 
  void AddBefore(MVector Pos, double Energy);

  //! Add data to the backprojection histogram
  void AddAfter(MVector Pos, double Energy);

  // protected methods:
 protected:


  // protected members:
 protected:

  // private members:
 private:
  //! Initial image canvas
  TRootEmbeddedCanvas* m_PositionsBeforeCanvas;
  //! Initial histyogram
  TH2D* m_PositionsBefore;
  //! Backprojection canvas
  TRootEmbeddedCanvas* m_PositionsAfterCanvas;
  //! Backprojection histogram
  TH2D* m_PositionsAfter;

  //! Initial image canvas
  TRootEmbeddedCanvas* m_DepthBeforeCanvas;
  //! Initial histyogram
  TH1D* m_DepthBefore;
  //! Backprojection canvas
  TRootEmbeddedCanvas* m_DepthAfterCanvas;
  //! Backprojection histogram
  TH1D* m_DepthAfter;

  //! Initial image canvas
  TRootEmbeddedCanvas* m_EnergyBeforeCanvas;
  //! Initial histyogram
  TH1D* m_EnergyBefore;
  //! Backprojection canvas
  TRootEmbeddedCanvas* m_EnergyAfterCanvas;
  //! Backprojection histogram
  TH1D* m_EnergyAfter;


#ifdef ___CINT___
 public:
  ClassDef(NGUIDiagnosticsDetectorEffectsEngine, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
