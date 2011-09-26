/*
 * NGUIDiagnosticsOrbitEngine.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIDiagnosticsOrbitEngine__
#define __NGUIDiagnosticsOrbitEngine__


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
#include <list>
using namespace std;

// MEGAlib libs:
#include "NGlobal.h"
#include "MGUIERBList.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIDiagnostics.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIDiagnosticsOrbitEngine : public NGUIDiagnostics
{
  // public Session:
 public:
  //! Default constructor
  NGUIDiagnosticsOrbitEngine();
  //! Default destructor
  virtual ~NGUIDiagnosticsOrbitEngine();

  //! The creation part which gets overwritten
  virtual void Create();

  //! Update the frame
  virtual void Update();

  //! Add occultation and data
  void AddOrbitStatus(const NTime& Time, bool IsOcculted, bool IsNight);

  // protected methods:
 protected:


  // protected members:
 protected:

  // private members:
 private:
  //! The times at which the occultation time is stored
  list<NTime> m_Time;
  //! Is the object occulted
  list<bool> m_IsOcculted;
  //! Is it day or night
  list<bool> m_IsNight;
   
   //! Initial image canvas
  TRootEmbeddedCanvas* m_OccultationTimeCanvas;
  //! Initial histyogram
  TH1D* m_OccultationTime;

  //! Backprojection canvas
  TRootEmbeddedCanvas* m_NightTimeCanvas;
  //! Backprojection histogram
  TH1D* m_NightTime;



#ifdef ___CINT___
 public:
  ClassDef(NGUIDiagnosticsOrbitEngine, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
