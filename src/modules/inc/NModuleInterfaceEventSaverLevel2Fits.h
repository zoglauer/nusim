/*
 * NModuleInterfaceEventSaverLevel2Fits.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceEventSaverLevel2Fits__
#define __NModuleInterfaceEventSaverLevel2Fits__


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
#include "NEvent.h"
#include "NSatellite.h"
#include "NModuleInterfaceIO.h"
#include "NModule.h"

// Forward declaratEventSaverLevel2Fitsns:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which provide to save the event as level 2 fits file
//! This interface represents a module interface representing modules with the 
//! capability to save to a level 2 fits file. 

class NModuleInterfaceEventSaverLevel2Fits
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceEventSaverLevel2Fits(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleInterfaceEventSaverLevel2Fits();
  
  //! Load and initialize the file
  virtual bool OpenLevel2FitsFile(TString FileName);
  
  //! Return true if the ASCII file is open
  virtual bool IsLevel2FitsFileOpen() { if (m_File != 0) return true; else return false; }
  
  //! Main data analysis routine, which updates the event to a new level
  //! WhatToStream: see NEvent::Stream
  virtual bool SaveEventLevel2Fits(NEvent& Event);
 
  //! Close the file
  virtual bool CloseLevel2FitsFile();


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
  //! Reference to the satellite module 
  NSatellite& m_Satellite; 
   
  //! fields for fits file.
  vector<double> m_Ra; 
  vector<double> m_Dec;
  vector<double> m_Energy;
  vector<double> m_Time;
  vector<int>    m_Origin;
  vector<int>    m_PI;
  vector<int>    m_Grade;
  vector<NQuaternion> m_Qfbob;
  vector<MVector> m_Tfbob;

#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceEventSaverLevel2Fits, 0) // no descriptEventSaverLevel2Fitsn
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
