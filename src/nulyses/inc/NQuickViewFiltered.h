/*
 * NQuickViewFiltered.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NQuickViewFiltered__
#define __NQuickViewFiltered__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NQuickViewFiltered : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NQuickViewFiltered();
  //! Default destructor
  virtual ~NQuickViewFiltered();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze();

  // protected methods:
 protected:
  //NQuickViewFiltered() {};
  //NQuickViewFiltered(const NQuickViewFiltered& NCTHit) {};

  bool CreateGTIFile(vector<double>& GTIStart, vector<double>& GTIStop, NHousekeeping& H);

  bool Show(NFilteredEvents& FE, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
            int SourcePosX, int SourcePosY, double DistanceCutOff);
  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
   //! Write a GTI file
  bool m_WriteGTI;


#ifdef ___CINT___
 public:
  ClassDef(NQuickViewFiltered, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
