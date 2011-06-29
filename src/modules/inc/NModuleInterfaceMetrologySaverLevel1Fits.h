/*
 * NModuleInterfaceMetrologySaverLevel1Fits.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceMetrologySaverLevel1Fits__
#define __NModuleInterfaceMetrologySaverLevel1Fits__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// HEAsoft lib:
#include "fitsio.h"

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NMetrologyData.h"
#include "NSatellite.h"
#include "NModule.h"

// Forward declaratMetrologySaverLevel1Fitsns:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which provide to save the event as level 1 fits file
//! This interface represents a module interface representing modules with the 
//! capability to save to a level 1 fits file.

class NModuleInterfaceMetrologySaverLevel1Fits
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceMetrologySaverLevel1Fits();
  //! Default destructor
  virtual ~NModuleInterfaceMetrologySaverLevel1Fits();
  
  //! Load and initialize the file
  virtual bool OpenLevel1FitsFile(TString FileName);
  
  //! Return true if the ASCII file is open
  virtual bool IsLevel1FitsFileOpen() { if (m_File != 0) return true; else return false; }
  
  //! Main data analysis routine, which updates the event to a new level
  //! WhatToStream: see NEvent::Stream
  virtual bool SaveAsLevel1Fits(NMetrologyData& Data);
 
  //! Close the file
  virtual bool CloseLevel1FitsFile();


  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! The output stream
  fitsfile* m_File; 

  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceMetrologySaverLevel1Fits, 0) // no descriptMetrologySaverLevel1Fitsn
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////