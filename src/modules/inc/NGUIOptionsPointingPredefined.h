/*
 * NGUIOptionsPointingPredefined.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptionsPointingPredefined__
#define __NGUIOptionsPointingPredefined__


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
#include <TGTab.h>
#include <TGCanvas.h>

// MEGAlib libs:
#include "NGlobal.h"
#include "MGUIEEntry.h"
#include "MGUIEFileSelector.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIOptions.h"
#include "NGUIOptionsSource.h"
#include "NGUIOptionsSinglePointing.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsPointingPredefined : public NGUIOptions
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptionsPointingPredefined(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptionsPointingPredefined();

  //! Process all button, etc. messages
  virtual bool ProcessMessage(long Message, long Parameter1, long Parameter2);

  //! The creation part which gets overwritten
  virtual void Create();

  static const int c_Import;
  static const int c_Absolute;

  
  // protected methods:
 protected:

  //! Actions after the Apply or OK button has been pressed
	virtual bool OnApply();

  //! Import a pointing list
  bool OnImport();
  
  //! Update the poitning viewer
  void UpdateViewer();

  // protected members:
 protected:

  // private members:
 private:
  //! The pointing viewer
  TGCanvas* m_PointingViewer;
   
  //! The single pointing GUIs
  vector<NGUIOptionsSinglePointing*> m_SinglePointings;

  //! Are the times absolute or relative
  TGCheckButton* m_AbsoluteTime;
  
  //! Pointing jitter DB selector
  MGUIEFileSelector* m_PointingJitterDB;





#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsPointingPredefined, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
