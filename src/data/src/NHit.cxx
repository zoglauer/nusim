/*
 * NHit.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NHit
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NHit.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NHit)
#endif


////////////////////////////////////////////////////////////////////////////////


NHit::NHit()
{
  // Construct an instance of NHit

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NHit::~NHit()
{
  // Delete this instance of NHit
}


////////////////////////////////////////////////////////////////////////////////


const NHit& NHit::operator=(const NHit& Hit)
{
  // Assignment operator

  m_Empty = Hit.m_Empty;

  m_Telescope = Hit.m_Telescope;
  m_Detector = Hit.m_Detector;

  m_Position = Hit.m_Position;
  m_PositionResolution = Hit.m_PositionResolution;
  
  m_Energy = Hit.m_Energy;
  m_EnergyResolution = Hit.m_EnergyResolution;

  m_ObservatoryData = Hit.m_ObservatoryData;

  return (*this);
}


////////////////////////////////////////////////////////////////////////////////


void NHit::Clear()
{
  // Reset all data

  m_Empty = true;

  m_Telescope = g_UnsignedIntNotDefined;
  m_Detector = g_UnsignedIntNotDefined;

  m_Position = g_VectorNotDefined;
  m_Energy = g_DoubleNotDefined;
  m_PositionResolution = g_VectorNotDefined;
  m_EnergyResolution = g_DoubleNotDefined;
  
  m_BadDepthCalibration = false;

  m_ObservatoryData.Clear();
}


////////////////////////////////////////////////////////////////////////////////


bool NHit::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  S<<"HT ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    S<<m_Telescope<<" "<<m_Detector<<" "
     <<m_Position[0]<<" "<<m_Position[1]<<" "<<m_Position[2]<<" "
     <<m_PositionResolution[0]<<" "<<m_PositionResolution[1]<<" "<<m_PositionResolution[2]<<" "
     <<m_Energy<<" "<<m_EnergyResolution<<" ";
     m_ObservatoryData.Stream(S, true);
  } 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NHit::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  

  if (Line == "HT -") {
    Clear();
    return true;
  }

  /*
  double xP, yP, zP, xD, yD, zD;
  char Text[1024];
  if (sscanf(Line.Data(), "HT %d %d %lf %lf %lf %lf %lf %lf %lf %lf %s", 
             &m_Telescope, &m_Detector,
             &xP, &yP, &zP, &xD, &yD, &zD, 
             &m_Energy, &m_EnergyResolution, Text) != 11) {
    merr<<"Unable to parse hit"<<show;
    return false;
  }
  m_Position.SetXYZ(xP, yP, zP);
  m_PositionResolution.SetXYZ(xD, yD, zD);
  */
  char* p;
  m_Telescope = strtol(Line.Data()+3, &p, 10);
  m_Detector = strtol(p, &p, 10);
  m_Position[0] = strtod(p, &p);
  m_Position[1] = strtod(p, &p);
  m_Position[2] = strtod(p, &p);
  m_PositionResolution[0] = strtod(p, &p);
  m_PositionResolution[1] = strtod(p, &p);
  m_PositionResolution[2] = strtod(p, &p);
  m_Energy = strtod(p, &p);
  m_EnergyResolution = strtod(p, &p);
  
  TString ObsString = p;
  m_ObservatoryData.Parse(ObsString, true);
  
  m_Empty = false;

  return true;
}



// NHit.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
