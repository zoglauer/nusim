/*
 * NModuleInterfacePhotonSaverAscii.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfacePhotonSaverAscii
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfacePhotonSaverAscii.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

// NuSIM libs:
#include "NModule.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfacePhotonSaverAscii)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfacePhotonSaverAscii::NModuleInterfacePhotonSaverAscii()
{
  // Construct an instance of NModuleInterfacePhotonSaverAscii
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfacePhotonSaverAscii::~NModuleInterfacePhotonSaverAscii()
{
  // Delete this instance of NModuleInterfacePhotonSaverAscii
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfacePhotonSaverAscii::OpenAsciiFile(TString FileName, int Version)
{  
  //! Load and initialize the file

  MFile::ExpandFileName(FileName);
  m_Out.open(FileName);
  if (m_Out.is_open() == false) {
    mgui<<"Unable to open file: "<<FileName<<error;
    return false;
  }

  m_Out<<"# NuSIM photon file"<<endl;
  m_Out<<endl;
  m_Out<<"# NuSIM version:"<<endl;
  m_Out<<"VE "<<g_Version<<endl;
  m_Out<<"# NuSIM revision:"<<endl;
  m_Out<<"RV "<<g_SVNRevision<<endl;
  m_Out<<"# ASCII file version:"<<endl;
  m_Out<<"VF "<<Version<<endl;
  m_Out<<endl;
  m_Out<<"# Start data..."<<endl;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfacePhotonSaverAscii::SavePhotonAscii(NPhoton& Photon, int Version)
{
  //! Main data analysis routine, which updates the event to a new level 

  if (m_Out.is_open() == false) {
    merr<<"Ascii file not open for saving"<<show;
    return false;
  }

  Photon.Stream(m_Out, Version);
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfacePhotonSaverAscii::CloseAsciiFile()
{
  //! Close the file    

  if (m_Out.is_open() == true) {
    m_Out<<"EN"<<endl;
    m_Out.close();
  }
  
  return true;
}


// NModuleInterfacePhotonSaverAscii.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
