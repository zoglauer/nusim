/*
 * NModuleInterfaceExit.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceExit__
#define __NModuleInterfaceExit__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which are exit points
//! This interface represents a required exit point from the pipeline
//! Usually there is no need for a well defined exit point. The only exceptin are the data savers

class NModuleInterfaceExit
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceExit();
  //! Default destructor
  virtual ~NModuleInterfaceExit();

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
  ClassDef(NModuleInterfaceExit, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
