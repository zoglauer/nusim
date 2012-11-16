/*
 * NApertureTester.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NApertureTester__
#define __NApertureTester__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TH2.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"
#include "NBackgroundModes.h"
#include "NPhaFile.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NApertureTester : public NBackgroundModes
{
  // public interface:
 public:
  //! Standard constructor
  NApertureTester();
  //! Default destructor
  virtual ~NApertureTester();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  static TH2D* s_Aperture;
  static double s_Scaler0;
  static double s_Scaler1;
  static double s_Scaler2;
  static double s_Scaler3;
  
  // protected methods:
 protected:
  //NApertureTester() {};
  //NApertureTester(const NApertureTester& NCTHit) {};

  bool Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  // private methods:
 private:




  // protected members:
 protected:


  // private members:
 private:

  bool m_InclusionMode;
  

#ifdef ___CINT___
 public:
  ClassDef(NApertureTester, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
