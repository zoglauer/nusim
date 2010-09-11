/*
 * NModuleInterfaceEventSaverAscii.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceEventSaverAscii__
#define __NModuleInterfaceEventSaverAscii__


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

// Forward declaratEventSaverAsciins:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which provide to save the event as ASCII
//! This interface represents a module interface representing modules with the ASCII event saver
//! capability. 

class NModuleInterfaceEventSaverAscii
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceEventSaverAscii();
  //! Default destructor
  virtual ~NModuleInterfaceEventSaverAscii();
  
  //! Load and initialize the file
  virtual bool OpenAsciiFile(TString FileName, int ModuleType);
  
  //! Return true if the ASCII file is open
  virtual bool IsAsciiFileOpen() { return m_Out.is_open(); }

  //! Main data analysis routine, which updates the event to a new level
  //! WhatToStream: see NEvent::Stream
  virtual bool SaveEventAscii(NEvent& Event, int WhatToStream = 0);
 
  //! Close the file
  virtual bool CloseAsciiFile();


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
  ClassDef(NModuleInterfaceEventSaverAscii, 0) // no descriptEventSaverAsciin
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
