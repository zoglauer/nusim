/*
 * NModuleInterfaceTimeJump.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceTimeJump__
#define __NModuleInterfaceTimeJump__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for modules which are can time jump
//! This interface represents an time jump with in the simulations.


class NModuleInterfaceTimeJump 
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceTimeJump();
  //! Default destructor
  virtual ~NModuleInterfaceTimeJump();

  //! Perform a time jump:
  virtual void PerformTimeJump(const NTime& TimeJump) { } 

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
  ClassDef(NModuleInterfaceTimeJump, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
