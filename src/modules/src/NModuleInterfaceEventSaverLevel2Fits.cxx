/*
 * NModuleInterfaceEventSaverLevel2Fits.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceEventSaverLevel2Fits
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceEventSaverLevel2Fits.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

// NuSIM libs:
#include "NModule.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceEventSaverLevel2Fits)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverLevel2Fits::NModuleInterfaceEventSaverLevel2Fits()
{
  // Construct an instance of NModuleInterfaceEventSaverLevel2Fits
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverLevel2Fits::~NModuleInterfaceEventSaverLevel2Fits()
{
  // Delete this instance of NModuleInterfaceEventSaverLevel2Fits
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::OpenLevel2FitsFile(TString FileName)
{  
  //! Load and initialize the file

  cout<<"OpenFits"<<endl;

  MFile::ExpandFileName(FileName);
  m_Out.open(FileName);
  if (m_Out.is_open() == false) {
    mgui<<"Unable to open file: "<<FileName<<error;
    return false;
  }

  m_Out<<"# NuSIM event file"<<endl;
  m_Out<<endl;
  m_Out<<"# NuSIM version:"<<endl;
  m_Out<<"VE "<<g_Version<<endl;
  m_Out<<"# NuSIM revision:"<<endl;
  m_Out<<"RV "<<g_SVNRevision<<endl;
  m_Out<<endl;
  m_Out<<"# Start data..."<<endl;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::SaveEventLevel2Fits(NEvent& Event)
{
  //! Main data analysis routine, which updates the event to a new level 

  cout<<"SaveFits"<<endl;

  if (m_Out.is_open() == false) {
    merr<<"Level2Fits file not open for saving"<<show;
    return false;
  }
  
  Event.Stream(m_Out, 2);
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::CloseLevel2FitsFile()
{
  //! Close the file    

  cout<<"CloseFits"<<endl;

  if (m_Out.is_open() == true) {
    m_Out<<"EN"<<endl;
    m_Out.close();
  }
  
  return true;
}


// NModuleInterfaceEventSaverLevel2Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
