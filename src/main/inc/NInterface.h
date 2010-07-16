/*
 * NInterface.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NInterface__
#define __NInterface__


////////////////////////////////////////////////////////////////////////////////


// standard libs
#include <iostream>
using namespace std;

// ROOT libs
#include "MVector.h"

// MEGAlib libs:
#include "NGlobal.h"
#include "NSupervisor.h"

class NGUIMain;

////////////////////////////////////////////////////////////////////////////////


//! \brief The interface between command line, GUI and supervisor
//! The class represents the intreface between the command line via ParseCommandLine, the GUI, and the supervisor

class NInterface
{
  // Public Interface:
 public:
  //! Default constructor
  NInterface();
  //! Default destructor
  ~NInterface();

  //! Each interface must be able to parse a command line - 
  //! this function is called by main()
  bool ParseCommandLine(int argc, char** argv);

  //! Analyze the data
  bool Analyze();

  //! Save the GUI/supervisor data
  void Save();

  //! Exit the application
  void Exit();

  //! Cancel the run
  void Interrupt() { m_Supervisor->Interrupt(); }

  //! Toggle the diagnostocs window
  void ToggleDiagnosticsWindow() { m_Supervisor->ToggleDiagnosticsWindow(); }

  // protected methods:
 protected:
 

  // private methods:
 private:


  // protected members:
 protected:


  // private members:
 private:
  //! True if the GUI is used
  bool m_UseGui;
  //! The graphical user interface
  NGUIMain* m_Gui;

  //! The store for all user data of the GUI:
  NSupervisor* m_Supervisor;

  //! Flag indicating that the run shall be interrupted
  bool m_Interrupt;

#ifdef ___CINT___
 public:
  ClassDef(NInterface, 0) // image reconstruction management class 
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
