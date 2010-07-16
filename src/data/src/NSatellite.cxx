/*
 * NSatellite.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NSatellite
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NSatellite.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NSatellite)
#endif


////////////////////////////////////////////////////////////////////////////////


NSatellite::NSatellite()
{
  // Construct an instance of NSatellite

  m_Pointing = 0;
  m_Orbit = 0;
  m_Orientations = 0;
  m_Time = 0;

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NSatellite::~NSatellite()
{
  // Delete this instance of NSatellite
}


////////////////////////////////////////////////////////////////////////////////


bool NSatellite::Initialize()
{
  //! Initialize for simulating a new run

  if (m_Pointing == 0) {
    mgui<<"You have no pointing module defined for your satellite!"<<error;
    return false;
  }
  //if (m_Pointing->Initialize() == false) return false;
  if (m_Orbit == 0) {
    mgui<<"You have no orbit module defined for your satellite!"<<error;
    return false;
  }
  //if (m_Orbit->Initialize() == false) return false;
  if (m_Orientations == 0) {
    mgui<<"You have no orientations module defined for your satellite!"<<error;
    return false;
  }
  //if (m_Orientations->Initialize() == false) return false;
  if (m_Time == 0) {
    mgui<<"You have no time module defined for your satellite!"<<error;
    return false;
  }
  //if (m_Time->Initialize() == false) return false;

  return true;
}


// NSatellite.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
