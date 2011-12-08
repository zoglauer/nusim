/*
 * NModuleInterfaceEventSaverAscii.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceEventSaverAscii
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceEventSaverAscii.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

// NuSIM libs:
#include "NModule.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceEventSaverAscii)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverAscii::NModuleInterfaceEventSaverAscii(NSatellite& Satellite) : NModuleInterfaceObservation(), m_Satellite(Satellite)
{
  // Construct an instance of NModuleInterfaceEventSaverAscii
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverAscii::~NModuleInterfaceEventSaverAscii()
{
  // Delete this instance of NModuleInterfaceEventSaverAscii
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverAscii::OpenAsciiFile(TString FileName, int ASCIIFileVersion, int ModuleType)
{  
  //! Load and initialize the file

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
  m_Out<<"# ASCII file version:"<<endl;
  m_Out<<"VF "<<ASCIIFileVersion<<endl;
  m_Out<<endl;
  m_Out<<"# Stored after the following module type: "<<endl;
  m_Out<<"CM "<<ModuleType<<endl;
  m_Out<<endl;
  m_Out<<"# Start data..."<<endl;
  m_Out<<endl;
  m_Out<<"TARGETNAME "<<m_TargetName<<endl;
  m_Out<<endl;
  m_Out<<"OBSID "<<m_ObservationID<<endl;
  m_Out<<endl;
  NTime Start = m_Satellite.GetAbsoluteObservationStartTime();
  m_Out<<"OBSSTART "<<Start.GetASCIIFileString()<<endl;
  m_Out<<"DATE-OBS "<<Start.GetDateInString()<<endl;
  m_Out<<"TSTART "<<fixed<<m_Satellite.GetEpochObservationStartTime().GetAsSeconds()<<endl;
  m_Out<<endl;
  
  m_TotalLifeTime1.Set(0.0);
  m_TotalLifeTime2.Set(0.0);
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverAscii::SaveEventAscii(NEvent& Event, int ASCIIFileVersion, int WhatToStream)
{
  //! Main data analysis routine, which updates the event to a new level 

  if (m_Out.is_open() == false) {
    merr<<"Ascii file not open for saving"<<show;
    return false;
  }

  Event.Stream(m_Out, ASCIIFileVersion, WhatToStream);
  
  if (Event.GetTelescope() == 1) m_TotalLifeTime1 += Event.GetDetectorLifeTime();
  if (Event.GetTelescope() == 2) m_TotalLifeTime2 += Event.GetDetectorLifeTime();
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverAscii::CloseAsciiFile()
{
  //! Close the file    

  if (m_Out.is_open() == true) {
    m_Out<<"EN"<<endl;
    m_Out<<endl;
    NTime End = m_Satellite.GetAbsoluteObservationEndTime();
    m_Out<<"OBSEND "<<End.GetASCIIFileString()<<endl;
    m_Out<<endl;
    m_Out<<"TOTLIFE1 "<<m_TotalLifeTime1<<endl;
    m_Out<<"TOTLIFE2 "<<m_TotalLifeTime2<<endl;
    m_Out<<endl;
    m_Out<<"EFFOBS "<<m_Satellite.GetEffectiveObservationTime()<<endl;
    m_Out<<endl;
    
    m_Out.close();
  }
  
  return true;
}


// NModuleInterfaceEventSaverAscii.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
