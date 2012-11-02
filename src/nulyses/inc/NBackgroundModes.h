/*
 * NBackgroundModes.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBackgroundModes__
#define __NBackgroundModes__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBaseTool.h"
#include "NPhaFile.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBackgroundModes : public NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NBackgroundModes();
  //! Default destructor
  virtual ~NBackgroundModes();

  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);

  // protected methods:
 protected:
  //NBackgroundModes() {};
  //NBackgroundModes(const NBackgroundModes& NCTHit) {};
  
  // private methods:
 private:



  // protected members:
 protected:

  vector<TString> m_PhaA;
  vector<TString> m_PhaB;
  
  vector<TString> m_RegA;
  vector<TString> m_RegB;

  double m_MinFitRange;
  double m_MaxFitRange;
  
  // private members:
 private:

   
#ifdef ___CINT___
 public:
  ClassDef(NBackgroundModes, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
