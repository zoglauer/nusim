/*
 * NMetrologyDataSet.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NMetrologyDataSet
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NMetrologyDataSet.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NMetrologyDataSet)
#endif


////////////////////////////////////////////////////////////////////////////////


NMetrologyDataSet::NMetrologyDataSet()
{
  // Construct an instance of NMetrologyDataSet

  m_Empty = false;
  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NMetrologyDataSet::~NMetrologyDataSet()
{
  // Delete this instance of NMetrologyDataSet
}


////////////////////////////////////////////////////////////////////////////////


void NMetrologyDataSet::Clear() 
{
  // Reset the data

  if (m_Empty == true) return;

  m_Time.Clear();
  
  m_OriginalLaserHit = g_VectorNotDefined;

  m_CurrentNorth = g_DoubleNotDefined;
  m_CurrentSouth = g_DoubleNotDefined;
  m_CurrentEast = g_DoubleNotDefined;
  m_CurrentWest = g_DoubleNotDefined;

  m_CalibratedLaserHit = g_VectorNotDefined;

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NMetrologyDataSet::Stream(ofstream& S, TString Keyword)
{
  //! Stream the content to an ASCII file 

  if (m_Empty == true) {
    S<<Keyword<<" -"<<endl;
  } else {
    S<<Keyword<<" "<<m_Time.GetAsSeconds()<<" "<<m_OriginalLaserHit.X()<<" "<<m_OriginalLaserHit.Y()<<" "<<m_OriginalLaserHit.Z()<<" "
     <<m_CurrentNorth<<" "<<m_CurrentSouth<<" "<<m_CurrentEast<<" "<<m_CurrentWest<<" "
     <<m_CalibratedLaserHit.X()<<" "<<m_CalibratedLaserHit.Y()<<" "<<m_CalibratedLaserHit.Z()<<endl;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NMetrologyDataSet::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  

  if (Line.Length() == 4 && Line[4] == '-') {
    Clear();
    return true;
  }

  char Key[2];
  double S, xP, yP, zP, xO, yO, zO;
  if (sscanf(Line.Data(), 
             "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
             Key, &S, &xP, &yP, &zP, &m_CurrentNorth, &m_CurrentSouth, &m_CurrentEast, &m_CurrentWest, &xO, &yO, &zO) != 12) {
    return false;
  }
  m_Time.Set(S);
  m_OriginalLaserHit.SetXYZ(xP, yP, zP);
  m_CalibratedLaserHit.SetXYZ(xO, yO, zO);

  m_Empty = false;

  return true;
}

void NMetrologyDataSet::Interpolate(NMetrologyDataSet A, NMetrologyDataSet B, NTime t)
{
  MVector p1=A.GetCalibratedLaserHit();
  MVector p2=B.GetCalibratedLaserHit();
  MVector pNew;
  
  pNew[0] = p1[0] + (p2[0] - p1[0])*(t.GetAsSeconds() - A.GetTime().GetAsSeconds())/(B.GetTime().GetAsSeconds() - A.GetTime().GetAsSeconds());
  pNew[1] = p1[1] + (p2[1] - p1[1])*(t.GetAsSeconds() - A.GetTime().GetAsSeconds())/(B.GetTime().GetAsSeconds() - A.GetTime().GetAsSeconds());
  pNew[2] = p1[2];
  
  m_CalibratedLaserHit = pNew;
}

// NMetrologyDataSet.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
