/*
 * NGUIOptionsSourceDistributionFarFieldPointSource.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptionsSourceDistributionFarFieldPointSource__
#define __NGUIOptionsSourceDistributionFarFieldPointSource__


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
#include "MGUIEEntryList.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIOptions.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsSourceDistributionFarFieldPointSource : public NGUIOptions
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptionsSourceDistributionFarFieldPointSource(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptionsSourceDistributionFarFieldPointSource();

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

  // private members:
 private:
  //! The data list
  MGUIEEntryList* m_Start;


#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsSourceDistributionFarFieldPointSource, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
