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
   
  long firstelem;
  long firstrow;
  //! fields for fits file. Saving in chunks of 1000 photons
  vector <float> c1; 
  vector <float> c2;
  vector <float> c3;
  vector <float> c4;
  vector <float> c5;
  vector <float> c6;
  vector <float> c7;
  vector <float> c8;
  vector <float> c9;
  vector <float> c10;
  vector <float> c11;

    
  unsigned int counter;
  float Reference_Ra;
  float Reference_Dec;
  float Pixsize;
  float maxRa;
  float minRa;
  float maxDec;
  float minDec;


#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceEventSaverLevel2Fits, 0) // no descriptEventSaverLevel2Fitsn
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
