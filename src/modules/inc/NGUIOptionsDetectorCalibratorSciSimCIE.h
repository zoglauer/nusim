/*
 * NGUIOptionsDetectorCalibratorSciSimCIE.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptionsDetectorCalibratorSciSimCIE__
#define __NGUIOptionsDetectorCalibratorSciSimCIE__


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

// MEGAlib libs:
#include "NGlobal.h"
#include "MGUIEEntry.h"
#include "MGUIEFileSelector.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIOptions.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsDetectorCalibratorSciSimCIE : public NGUIOptions
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptionsDetectorCalibratorSciSimCIE(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptionsDetectorCalibratorSciSimCIE();

  //! Process all button, etc. messages
  virtual bool ProcessMessage(long Message, long Parameter1, long Parameter2);

  //! The creation part which gets overwritten
  virtual void Create();

  // protected methods:
 protected:

  //! Actions after the Apply or OK button has been pressed
  virtual bool OnApply();


  // protected members:
 protected:

  //! Gain for single-trigger events
  MGUIEEntry* m_GainSingleTrigger;
  //! Offset for single-trigger events
  MGUIEEntry* m_OffsetSingleTrigger;

  //! Gain for double-trigger events
  MGUIEEntry* m_GainDoubleTrigger;
  //! Offset for double-trigger events
  MGUIEEntry* m_OffsetDoubleTrigger;

  //! Gain for triple-trigger events
  MGUIEEntry* m_GainTripleTrigger;
  //! Offset for triple-trigger events
  MGUIEEntry* m_OffsetTripleTrigger;

  //! Gain for quadruple-trigger events
  MGUIEEntry* m_GainQuadrupleTrigger;
  //! Offset for quadruple-trigger events
  MGUIEEntry* m_OffsetQuadrupleTrigger;

  //! ROOT file name for depth cut
  MGUIEFileSelector* m_DepthCutFile;

  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsDetectorCalibratorSciSimCIE, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
