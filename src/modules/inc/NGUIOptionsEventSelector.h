/*
 * NGUIOptionsEventSelector.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptionsEventSelector__
#define __NGUIOptionsEventSelector__


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
#include "MGUIEMinMaxEntry.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIOptions.h"
#include "NGUIOptionsSource.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsEventSelector : public NGUIOptions
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptionsEventSelector(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptionsEventSelector();

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
  //! Choose the energy window
  MGUIEMinMaxEntry* m_Energies;



#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsEventSelector, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
