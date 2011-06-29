/*
 * NModuleInterfaceEventSaverROOTEnergyResponse.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceEventSaverROOTEnergyResponse__
#define __NModuleInterfaceEventSaverROOTEnergyResponse__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
// #include <fstream>
// using namespace std;

// HEAsoft lib:
// #include "fitsio.h"

// ROOT libs:
#include "TFile.h"
#include "TTree.h"
#include "TH2.h"
#include "TVector3.h"

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NEvent.h"
#include "NSatellite.h"
#include "NModuleInterfaceIO.h"
#include "NModule.h"

// Forward declaratEventSaverLevel2Fitsns:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which provide to save the event as ROOT Tree.
//! This interface represents a module interface representing modules with the
//! capability to save to a ROOT Tree file.

class NModuleInterfaceEventSaverROOTEnergyResponse
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceEventSaverROOTEnergyResponse(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleInterfaceEventSaverROOTEnergyResponse();

  //! Load and initialize the file
  virtual bool OpenEnergyResponseROOTFile(TString FileName);

  //! Return true if the ASCII file is open
  virtual bool IsEnergyResponseROOTFileOpen() { if (m_File != 0) return true; else return false; }

  virtual bool SaveEnergyResponse(NEvent& Event);

  //! Close the file
  virtual bool CloseEnergyResponseROOTFile();


  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! The output stream
  TFile* m_File;

  // private members:
 private:
  //! Reference to the satellite module
  NSatellite& m_Satellite;

  //! Detector response in ROOT TH2 class
  TH2D*  m_Response;
  

#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceEventSaverROOTEnergyResponse, 0) // no descriptEventSaverLevel2Fitsn
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
