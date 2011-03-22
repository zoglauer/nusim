/*
 * NGUIOptionsSourceDistribution.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptionsSourceDistribution__
#define __NGUIOptionsSourceDistribution__


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
#include "MGUIEEntryList.h"

// NuSTAR libs
#include "NModule.h"
#include "NGUIOptions.h"
#include "NGUIOptionsSource.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsSourceDistribution : public NGUIOptions
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptionsSourceDistribution(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptionsSourceDistribution();

  //! Process all button, etc. messages
  virtual bool ProcessMessage(long Message, long Parameter1, long Parameter2);

  //! The creation part which gets overwritten
  virtual void Create();

  static const int c_Add      = 1001;
  static const int c_List     = 1002;
  static const int c_Remove   = 1003;
  static const int c_Pointing = 1004;

  // protected methods:
 protected:

  //! Actions after the Apply or OK button has been pressed
	virtual bool OnApply();
  //! Actions after the Add button has been pressed
  virtual bool OnAdd();
  //! Actions after the List button has been pressed
  virtual bool OnList();
  //! Actions after the Remove button has been pressed
	virtual bool OnRemove();
  //! Actions after the Pointing button has been pressed
	virtual bool OnPointing();

  //! Create the tab
  void CreateTab(NSource* Source);

  // protected members:
 protected:

  // private members:
 private:
  //! The main Tab
  TGTab* m_Tab;
  //! List of all tab frames
  vector<TGCompositeFrame*> m_TabFrames;
  //! List of all source frames
  vector<NGUIOptionsSource*> m_SourceFrames;

#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsSourceDistribution, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
