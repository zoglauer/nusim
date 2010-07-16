/*
 * NModuleDetectorEffectsEngineBackgroundSims.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleDetectorEffectsEngineBackgroundSims__
#define __NModuleDetectorEffectsEngineBackgroundSims__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MResponseMatrixO3.h"
#include "MFunction.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NModuleDetectorEffectsEngineSciSim.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleDetectorEffectsEngineBackgroundSims : public NModuleDetectorEffectsEngineSciSim
{
  // public interface:
 public:
  //! Default constructor
  NModuleDetectorEffectsEngineBackgroundSims(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleDetectorEffectsEngineBackgroundSims();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NModuleDetectorEffectsEngineBackgroundSims, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
