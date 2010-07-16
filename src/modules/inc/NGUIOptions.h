/*
 * NGUIOptions.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptions__
#define __NGUIOptions__


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
#include "MGUIERBList.h"

// NuSTAR lib:
#include "NModule.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptions : public TGTransientFrame
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptions(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptions();

  //! Close the window
  void CloseWindow();
  //! Process all button, etc. messages
  virtual bool ProcessMessage(long Message, long Parameter1, long Parameter2);
  
  //! The creation part which gets overwritten
  virtual void Create();


  // protected methods:
 protected:
  //! Create the Gui -- initial step
  void PreCreate();
  //! Create the Gui -- final step
  void PostCreate();

  //! Actions after the OK button has been pressed
	virtual bool OnOk();
  //! Actions after the Cancel button has been pressed
	virtual bool OnCancel();
  //! Actions after the Apply or OK button has been pressed
	virtual bool OnApply();


  // protected members:
 protected:
  //! The module
  NModule* m_Module;

	enum BasicButtonIDs { e_Ok = 1, e_Cancel, e_Apply };

  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NGUIOptions, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
