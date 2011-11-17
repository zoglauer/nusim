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
#include "NModuleInterfaceObservation.h"
#include "NModule.h"
#include "NExposureMap.h"

// Forward declaratEventSaverLevel2Fitsns:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which provide to save the event as level 2 fits file
//! This interface represents a module interface representing modules with the 
//! capability to save to a level 2 fits file. 

class NModuleInterfaceEventSaverLevel2Fits : public NModuleInterfaceObservation
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceEventSaverLevel2Fits(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleInterfaceEventSaverLevel2Fits();
  
  //! Load and initialize the file
  virtual bool OpenLevel2FitsFile(TString FileName);
  
  //! Set the pixel size
  void SetPixelSize(const double PixelSize) { m_PixelSize = PixelSize; }
  //! Get the pixel size
  double GetPixelSize() const { return m_PixelSize; }
  
  //! Set the pixel size
  void SetCreateExposureMap(const bool CreateExposureMap) { m_CreateExposureMap = CreateExposureMap; }
  //! Get the pixel size
  bool GetCreateExposureMap() const { return m_CreateExposureMap; }
  
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
  
  //! The pixel size when saving as fits file
  double m_PixelSize;

  //! Create the exposure map
  bool m_CreateExposureMap;

  // private members:
 private:
  //! Reference to the satellite module 
  NSatellite& m_Satellite; 
  
  //! The exposure map
  NExposureMap m_ExposureMap;
  
  //! fields for fits file.
  vector<double>  m_Ra; 
  vector<double>  m_Dec;
  vector<float>  m_Energy;
  vector<double> m_Life;
  vector<double> m_Time;
  vector<int>    m_Module;
  vector<short>  m_Origin;
  vector<short>  m_Reject;
  vector<int>    m_PI;
  vector<short>  m_Grade;
  
  vector<float> m_QfbobX;
  vector<float> m_QfbobY;
  vector<float> m_QfbobZ;
  vector<float> m_QfbobR;

  vector<float> m_TfbobX;
  vector<float> m_TfbobY;
  vector<float> m_TfbobZ;
  
  vector<float> m_QstarX;
  vector<float> m_QstarY;
  vector<float> m_QstarZ;
  vector<float> m_QstarR;

  //! Fully saved entries
  unsigned int m_NEntries;
  
#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceEventSaverLevel2Fits, 0) // no descriptEventSaverLevel2Fitsn
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
