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

// MEGAlib libs:
#include "NGlobal.h"
#include "MGUIEEntry.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIOptions.h"
#include "NGUIOptionsSource.h"

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

  // protected methods:
 protected:

  //! Actions after the Apply or OK button has been pressed
	virtual bool OnApply();

  //! Create the tab
  void CreateTab(NSource* Source);

  // protected members:
 protected:

  // private members:
 private:
  //! Right ascension GUI
  MGUIEEntry* m_Ra;
  //! Declination GUI
  MGUIEEntry* m_Dec;
  //! Selection motion pattern
  TGComboBox* m_MotionPatterns;



#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsPointingPredefined, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
