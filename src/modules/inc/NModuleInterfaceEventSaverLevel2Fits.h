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

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NEvent.h"
#include "NModuleInterfaceIO.h"

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
  NModuleInterfaceEventSaverLevel2Fits();
  //! Default destructor
  virtual ~NModuleInterfaceEventSaverLevel2Fits();
  
  //! Load and initialize the file
  virtual bool OpenLevel2FitsFile(TString FileName);
  
  //! Return true if the Level2Fits file is open
  virtual bool IsLevel2FitsFileOpen() { return m_Out.is_open(); }

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
  ofstream m_Out;

  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceEventSaverLevel2Fits, 0) // no descriptEventSaverLevel2Fitsn
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
