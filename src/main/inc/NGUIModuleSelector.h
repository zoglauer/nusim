/*
 * NGUIModuleSelector.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIModuleSelector__
#define __NGUIModuleSelector__


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
#include "NSupervisor.h"
#include "NModule.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief The module selector GUI window
//! The class represents the module selector GUI window

class NGUIModuleSelector : public TGTransientFrame
{
  // public methods:
 public:
  //! Default constructor
  NGUIModuleSelector(vector<NModule*> Modules, NModule* PreSelected = 0);
  //! Default destructor
  virtual ~NGUIModuleSelector();

  //! Return true if the OK button was pressed successfully
  bool OKPressed() { return m_OKPressed; }

  //! Return the selected module (zero if "No module" was selected)
  NModule* GetSelectedModule() { return m_SelectedModule; }

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
  //! List of possible modules
  vector<NModule*> m_Modules;
  //! The initially selected module
  NModule* m_PreSelected;
  //! True if the OK button was pressed
  bool m_OKPressed;
  //! The selected module
  NModule* m_SelectedModule;

  //! The list
  MGUIERBList* m_List;

#ifdef ___CINT___
 public:
  ClassDef(NGUIModuleSelector, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
