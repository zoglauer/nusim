/*
 * NModuleInterfaceIO.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceIO__
#define __NModuleInterfaceIO__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief This interface is used for all modules which provide IO capability
//! This interface represents a module interface representing modules with IO
//! capability. It just stores the actual module position (=type) as those 
//! modules can have multiple positions/types.

class NModuleInterfaceIO
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceIO();
  //! Default destructor
  virtual ~NModuleInterfaceIO();

  //! Set the chosen module type:
  void SetChosenModuleType(int Type) { m_ChosenType = Type; }
  //! Retrieve the chosen module type
  int GetChosenModuleType() const { return m_ChosenType; }

  //! Set the file name
  void SetFileName(TString FileName) { m_FileName = FileName; }
  //! Get the file name
  TString GetFileName() const { return m_FileName; }


  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! The output file name
  TString m_FileName;
  //! The module position as the module type
  int m_ChosenType;

  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceIO, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
