/*
 * NGUIDiagnostics.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIDiagnostics__
#define __NGUIDiagnostics__


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
#include "MGUIERBList.h"
#include "MTimer.h"

// NuSTAR lib:
#include "NGlobal.h"

// Forward declarations:
class NModule;

////////////////////////////////////////////////////////////////////////////////


class NGUIDiagnostics : public TGCompositeFrame
{
  // public Session:
 public:
  //! Default constructor
  NGUIDiagnostics();
  //! Default destructor
  virtual ~NGUIDiagnostics();

  //! Close the window
  void CloseWindow();
  //! Process all button, etc. messages
  virtual bool ProcessMessage(long Message, long Parameter1, long Parameter2);
  
  //! The creation part which gets overwritten
  virtual void Create() {};

  //! Update the frame
  virtual void Update() { };

  //! Get the title
  TString GetTabTitle() { return m_TabTitle; }

  //! Return true if we need an update
  bool NeedsUpdate() { return m_NeedsUpdate; }
  
  // protected methods:
 protected:


  // protected members:
 protected:
  //! Tab Title of the GUI element
  TString m_TabTitle;
  
  //! Flag to indicate we need an update
  bool m_NeedsUpdate;
  
  // private members:
 private:

#ifdef ___CINT___
 public:
  ClassDef(NGUIDiagnostics, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
