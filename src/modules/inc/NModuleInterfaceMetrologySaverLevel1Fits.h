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
  NModuleInterfaceMetrologySaverLevel1Fits(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleInterfaceMetrologySaverLevel1Fits();
  
  //! Load and initialize the file
  virtual bool OpenLevel1FitsFile(TString FileName);
  
  //! Return true if the ASCII file is open
  virtual bool IsLevel1FitsFileOpen() { if (m_File != 0) return true; else return false; }
  
  //! Main data analysis routine, which updates the event to a new level
  //! WhatToStream: see NEvent::Stream
  virtual bool SaveAsLevel1Fits(NMetrologyData& Data);
  //! Save a chunk of data
  virtual bool SaveData();
  //! Write Header
  virtual bool WriteHDR();
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
  NSatellite& m_Sat;

  // private members:
 private:
  vector<double> Time; 
  vector<double> m_xpsd0;
  vector<double> m_ypsd0;
  vector<double> m_xpsd1;
  vector<double> m_ypsd1;
  
  long m_counter;


#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceMetrologySaverLevel1Fits, 0) // no descriptMetrologySaverLevel1Fitsn
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
