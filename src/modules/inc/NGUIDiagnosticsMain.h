/*
 * NGUIDiagnosticsMain.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIDiagnosticsMain__
#define __NGUIDiagnosticsMain__


////////////////////////////////////////////////////////////////////////////////


// Standard libs
#include <vector>
using namespace std;

// ROOT libs
#include <TGMenu.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGFileDialog.h>
#include <TGIcon.h>
#include <TGPicture.h>
#include <TGTab.h>

// MEGAlib libs
#include "NGlobal.h"
#include "MGUIDialog.h"

// NuSTAR libs:
#include "NGUIDiagnostics.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIDiagnosticsMain : public MGUIDialog
{
  // Public members:
 public:
  //! Default constructor
  NGUIDiagnosticsMain();
  //! Default destructor
  virtual ~NGUIDiagnosticsMain();

  //! Set a new diagnostics tab
  void AddDiagnostics(NGUIDiagnostics* D) { m_DiagnosticTabs.push_back(D); }

  //! Create the GUI
  virtual void Create();
  //! Process all button, etc. messages
  virtual bool ProcessMessage(long Message, long Parameter1, long Parameter2);
 
  //! Update all tabs
  virtual void Update();

  //! Handle the close window event
  virtual void CloseWindow();

  //! Handle the apply button event
	virtual bool OnApply();
  //! Handle the cancel button event
	virtual bool OnCancel();

  // protected members:
 protected:



  // private members:
 private:
  //! All the diagnostics modules
  vector<NGUIDiagnostics*> m_DiagnosticTabs;
  //! The main tab...
  TGTab* m_MainTab;
  //! ... and its layout
  TGLayoutHints* m_MainTabLayout;


#ifdef ___CINT___
 public:
  ClassDef(NGUIDiagnosticsMain, 0) // main window of the Nuclearizer GUI
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
