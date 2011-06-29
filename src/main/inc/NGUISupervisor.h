/*
 * NGUISupervisor.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUISupervisor__
#define __NGUISupervisor__


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

// NuSTAR lib:
#include "NSupervisor.h"
#include "NModule.h"

// Forward declarations:
class NSupervisor;

////////////////////////////////////////////////////////////////////////////////


//! \brief The supervisor GUI window
//! The class represents the supervisor GUI window

class NGUISupervisor : public TGTransientFrame
{
  // public methods:
 public:
  //! Default constructor
  NGUISupervisor(NSupervisor* S);
  //! Default destructor
  virtual ~NGUISupervisor();

  //! Close this window
  void CloseWindow();
  //! Process all button, etc. messages
  virtual bool ProcessMessage(long Message, long Parameter1, long Parameter2);


  // protected methods:
 protected:
  //! Create the Gui
  void Create();

  //! Actions when the OK button has been pressed
	virtual bool OnOk();
  //! Actions when the Cancel button has been pressed
	virtual bool OnCancel();
  //! Actions when the Apply button has been pressed
	virtual bool OnApply();


  // protected members:
 protected:

	enum BasicButtonIDs { e_Ok = 1, e_Cancel, e_Apply };

  // private members:
 private:
  //! The supervisor, were we store the data
  NSupervisor* m_Supervisor;

  //! Observation time widget
  MGUIEEntry* m_ObservationTime;
  
  //! Update frequency of the diagnostics window 
  MGUIEEntry* m_UpdateInterval;

  //! Set a basic file name
  MGUIEFileSelector* m_BaseFileName;

#ifdef ___CINT___
 public:
  ClassDef(NGUISupervisor, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
