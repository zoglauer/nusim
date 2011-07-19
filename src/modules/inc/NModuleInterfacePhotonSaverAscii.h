/*
 * NModuleInterfacePhotonSaverAscii.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfacePhotonSaverAscii__
#define __NModuleInterfacePhotonSaverAscii__


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

// Forward declaratPhotonSaverAsciins:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which provide to save the photon as ASCII
//! This interface represents a module interface representing modules with the ASCII photon saver
//! capability. 

class NModuleInterfacePhotonSaverAscii
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfacePhotonSaverAscii();
  //! Default destructor
  virtual ~NModuleInterfacePhotonSaverAscii();
  
  //! Load and initialize the file
  virtual bool OpenAsciiFile(TString FileName);
  
  //! Return true if the ASCII file is open
  virtual bool IsAsciiFileOpen() { return m_Out.is_open(); }

  //! Main data analysis routine, which updates the event to a new level
  //! WhatToStream: see NEvent::Stream
  virtual bool SavePhotonAscii(NPhoton& Photon);
 
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
  ClassDef(NModuleInterfacePhotonSaverAscii, 0) // no descriptPhotonSaverAsciin
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
