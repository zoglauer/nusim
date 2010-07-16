/*
 * NModuleInterfacePhotonPropagation.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfacePhotonPropagation
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfacePhotonPropagation.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfacePhotonPropagation)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfacePhotonPropagation::NModuleInterfacePhotonPropagation()
{
  // Construct an instance of NModuleInterfacePhotonPropagation
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfacePhotonPropagation::~NModuleInterfacePhotonPropagation()
{
  // Delete this instance of NModuleInterfacePhotonPropagation
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfacePhotonPropagation::PropagateToPlane(NPhoton& Photon, MVector Position, MVector Normal)
{
  //! Propagate a photon to a plane
  //! We simply solve the equations
  //! (1) N dot (p - PosPlane) = 0
  //! (2) p = PosPhot + u*DirPhot
  //!     --> u = N dot (PosPlane - PosPhot) / N dot DirPhot
  
  double u = Normal.Dot(Position - Photon.GetPosition()) / Normal.Dot(Photon.GetDirection());
  if (u < 0) return false;
  Photon.SetPosition(Photon.GetPosition() + u*Photon.GetDirection());
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfacePhotonPropagation::Propagate(NPhoton& Photon, double Distance)
{
  //! Propagate a photon a certain distance d

  Photon.SetPosition(Photon.GetPosition() + Distance*Photon.GetDirection());

  return true;
}


// NModuleInterfacePhotonPropagation.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
