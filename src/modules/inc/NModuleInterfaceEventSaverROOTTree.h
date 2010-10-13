/*
 * NModuleInterfaceEventSaverROOTTree.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceEventSaverROOTTree__
#define __NModuleInterfaceEventSaverROOTTree__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
// #include <fstream>
// using namespace std;

// HEAsoft lib:
// #include "fitsio.h"

// ROOT libs:
#include "TFile.h"
#include "TTree.h"
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

class NModuleInterfaceEventSaverROOTTree
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceEventSaverROOTTree(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleInterfaceEventSaverROOTTree();

  //! Load and initialize the file
  virtual bool OpenROOTFile(TString FileName);

  //! Return true if the ASCII file is open
  virtual bool IsROOTFileOpen() { if (m_File != 0) return true; else return false; }

  //! Main data analysis routine, which updates the event to a new level
  //! WhatToStream: see NEvent::Stream
  virtual bool SaveEventTree(NEvent& Event);

  //! Close the file
  virtual bool CloseROOTFile();


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

  float Reference_Ra;
  float Reference_Dec;
  float Pixsize;

  TTree* m_EventTree;

  double   m_Time;
  int      m_Grade;
  double   m_PrimaryEnergy;
  TVector3 m_PrimaryPosition, m_PrimaryDirection;
  double   m_RA, m_Dec;
  double   m_XPix, m_YPix;
  int      m_Column, m_Row;
  int      m_NTrigs;
  double   m_Energy, m_MinusEnergy, m_ReconstructedEnergy, m_Energies[9];

#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceEventSaverROOTTree, 0) // no descriptEventSaverLevel2Fitsn
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
