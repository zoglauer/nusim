/*
 * NModuleInterfaceStopCriterion.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceStopCriterion__
#define __NModuleInterfaceStopCriterion__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! The class is an StopCriterion decorator for a module.
//! It provides a function to check if the stop criterion is fullfilled

class NModuleInterfaceStopCriterion
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceStopCriterion();
  //! Default destructor
  virtual ~NModuleInterfaceStopCriterion();

  //! Check if stop criterion is fullfilled
  virtual bool StopCriterionFullFilled() { return false; }


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
  ClassDef(NModuleInterfaceStopCriterion, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
