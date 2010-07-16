/*
 * NGUIDiagnosticsTemplate.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIDiagnosticsTemplate__
#define __NGUIDiagnosticsTemplate__


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

// NuSTAR libs
#include "NModule.h"
#include "NGUIDiagnostics.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIDiagnosticsTemplate : public NGUIDiagnostics
{
  // public Session:
 public:
  //! Default constructor
  NGUIDiagnosticsTemplate();
  //! Default destructor
  virtual ~NGUIDiagnosticsTemplate();

  //! The creation part which gets overwritten
  virtual void Create();

  //! Update the frame
  virtual void Update();

  // protected methods:
 protected:


  // protected members:
 protected:

  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NGUIDiagnosticsTemplate, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
