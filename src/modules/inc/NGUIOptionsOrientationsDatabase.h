/*
 * NGUIOptionsOrientationsDatabase.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptionsOrientationsDatabase__
#define __NGUIOptionsOrientationsDatabase__


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
#include "MGUIEFileSelector.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIOptions.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsOrientationsDatabase : public NGUIOptions
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptionsOrientationsDatabase(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptionsOrientationsDatabase();

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
  MGUIEFileSelector* m_CalibratedMetrologyDB;
  MGUIEFileSelector* m_PerturbedMetrologyDB;

  MGUIEFileSelector* m_CalibratedOpticsDB;
  MGUIEFileSelector* m_PerturbedOpticsDB;
  
  MGUIEFileSelector* m_CalibratedAlignmentsDB;
  MGUIEFileSelector* m_PerturbedAlignmentsDB;

  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsOrientationsDatabase, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
