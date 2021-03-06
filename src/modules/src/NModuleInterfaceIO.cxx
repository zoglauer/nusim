/*
 * NModuleInterfaceIO.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceIO
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceIO.h"

// Standard libs:

// ROOT libs:

// NuSIM libs:
#include "NModule.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceIO)
#endif

  
////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceIO::NModuleInterfaceIO() : m_ChosenType(NModule::c_Unknown)
{
  // Construct an instance of NModuleInterfaceIO

  m_ASCIIFileVersion = 2;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceIO::~NModuleInterfaceIO()
{
  // Delete this instance of NModuleInterfaceIO
}


// NModuleInterfaceIO.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
