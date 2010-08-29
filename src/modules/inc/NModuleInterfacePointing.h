/*
 * NModuleInterfacePointing.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfacePointing__
#define __NModuleInterfacePointing__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"
#include "NPointing.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface provides the base functionality for retrieving pointing data for the satellite modules
//! This is a special decorator for satellite modules. It provides functions to retrieve pointing data.

class NModuleInterfacePointing
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfacePointing();
  //! Default destructor
  virtual ~NModuleInterfacePointing();

  //! Set the satellite pointing at time t
  virtual NPointing GetPointing(NTime t) { return m_Pointing; }

  //! Set the observation time
  virtual void SetObservationTime(NTime O) { m_ObservationTime = O; }

  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! The current pointing
  NPointing m_Pointing;

  //! The total observation time, required to scale the pointing time, if we have relative timing
  NTime m_ObservationTime;
  
  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfacePointing, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
