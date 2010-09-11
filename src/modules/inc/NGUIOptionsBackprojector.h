/*
 * NGUIOptionsBackprojector.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptionsBackprojector__
#define __NGUIOptionsBackprojector__


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


class NGUIOptionsBackprojector : public NGUIOptions
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptionsBackprojector(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptionsBackprojector();

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
  MGUIEEntry* m_PixelSize;



#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsBackprojector, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
