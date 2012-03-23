/*
 * NGUIOptionsGeometryAndDetectorProperties.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGUIOptionsGeometryAndDetectorProperties__
#define __NGUIOptionsGeometryAndDetectorProperties__


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

// NuSTAR libs
#include "NModule.h"
#include "NGUIOptions.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsGeometryAndDetectorProperties : public NGUIOptions
{
  // public Session:
 public:
  //! Default constructor
  NGUIOptionsGeometryAndDetectorProperties(NModule* Module);
  //! Default destructor
  virtual ~NGUIOptionsGeometryAndDetectorProperties();

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
  TGCheckButton* m_EnableDetectorGaps;

  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsGeometryAndDetectorProperties, 1) // basic class for dialog windows
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
