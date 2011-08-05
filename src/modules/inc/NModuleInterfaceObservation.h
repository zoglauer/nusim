/*
 * NModuleInterfaceObservation.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceObservation__
#define __NModuleInterfaceObservation__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which need access to basic observation infos


class NModuleInterfaceObservation
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceObservation();
  //! Default destructor
  virtual ~NModuleInterfaceObservation();

  //! Set the file name
  void SetTargetName(TString TargetName) { m_TargetName = TargetName; }
  //! Get the file name
  TString GetTargetName() const { return m_TargetName; }

  //! Set the base file name
  void SetObservationID(TString ObservationID) { m_ObservationID = ObservationID; }
  //! Get the base file name
  TString GetObservationID() { return m_ObservationID; }


  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! The output file name
  TString m_TargetName;
  //! The base file name
  TString m_ObservationID;


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceObservation, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
