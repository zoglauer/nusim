/*
 * NBackgroundMode4DataBase.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBackgroundMode4DataBase__
#define __NBackgroundMode4DataBase__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBackgroundMode4DataBase : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NBackgroundMode4DataBase();
  //! Default destructor
  virtual ~NBackgroundMode4DataBase();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NBackgroundMode4DataBase() {};
  //NBackgroundMode4DataBase(const NBackgroundMode4DataBase& NCTHit) {};

  bool Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  TString m_DataBaseName;
   
  bool m_DBInitialized;
  TH1D* m_DBInternalSpectrum;
  double m_DBInternalSpectrumLifetime;
  
  TH1D* m_DBInternalRates;
  TH1D* m_DBInternalRatesLifetime;
   
  TH3D* m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID;
  TH2D* m_DBInternalGeomagneticCutOffVsSpectrum;
  TH1D* m_DBInternalGeomagneticCutOffLifetime;


#ifdef ___CINT___
 public:
  ClassDef(NBackgroundMode4DataBase, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
