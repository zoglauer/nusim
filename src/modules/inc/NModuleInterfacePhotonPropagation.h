/*
 * NModuleInterfacePhotonPropagation.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfacePhotonPropagation__
#define __NModuleInterfacePhotonPropagation__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MVector.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NPhoton.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface enables photon propagation for the ray-tracing modules
//! This module is a special decorator/interface for modules in the ray-tracing path.
//! It enables the propagation of photons.

class NModuleInterfacePhotonPropagation
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfacePhotonPropagation();
  //! Default destructor
  virtual ~NModuleInterfacePhotonPropagation();

  // protected methods:
 protected:
  //! Propagate a photon to a plane - return false if this is not possible (i.e. plane is not in flight direction) 
  bool PropagateToPlane(NPhoton& Photon, MVector Position, MVector Normal);

  //! Propagate a photon a certain distance d
  bool Propagate(NPhoton& Photon, double Distance);

  // private methods:
 private:



  // protected members:
 protected:

  // private members:
 private:




#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfacePhotonPropagation, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
