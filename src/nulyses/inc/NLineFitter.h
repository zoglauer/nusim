/*
 * NLineFitter.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NLineFitter__
#define __NLineFitter__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TH1.h"
#include "TH2.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NLineFitter : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NLineFitter();
  //! Default destructor
  virtual ~NLineFitter();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  bool Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            int SourcePosX, int SourcePosY, double DistanceCutOff, TH1D* LifeTime, TH2D* Spectrum,
            TH1D* GeomagneticCutOff, TH1D* GeomagneticCutOffLifeTime);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:

#ifdef ___CINT___
 public:
  ClassDef(NLineFitter, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
