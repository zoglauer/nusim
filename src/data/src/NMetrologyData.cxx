/*
 * NMetrologyData.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NMetrologyData
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NMetrologyData.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NMetrologyData)
#endif


////////////////////////////////////////////////////////////////////////////////


NMetrologyData::NMetrologyData()
{
  // Construct an instance of NMetrologyData

  m_Empty = false;
  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NMetrologyData::~NMetrologyData()
{
  // Delete this instance of NMetrologyData
}


////////////////////////////////////////////////////////////////////////////////


void NMetrologyData::Clear() 
{
  // Reset the data

  if (m_Empty == true) return;

  m_Metrology1.Clear();
  m_Metrology2.Clear();

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NMetrologyData::Stream(ofstream& S, int Version)
{
  //! Stream the content to an ASCII file 

  S<<"ME"<<endl;
  m_Metrology1.Stream(S, Version, "M1");
  m_Metrology2.Stream(S, Version, "M2");
  S<<"ME EN"<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NMetrologyData::Parse(TString& Line, int Version)
{
  //! Stream the content from a line of an ASCII file  

  bool NoError = true;

  if (Line.BeginsWith("M1") == true) {
    NoError = m_Metrology1.Parse(Line, Version);
  } else if (Line.BeginsWith("M2") == true) {
    NoError = m_Metrology2.Parse(Line, Version);
  } else if (Line.BeginsWith("ME EN") == true) {
    return true;
  } else {
    return false;
  }
  
  if (NoError == false) {
    cerr<<"Unable to parse line:"<<endl;
    cerr<<Line<<endl;
    return false;
  }

  m_Empty = false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NMetrologyData::Interpolate(NMetrologyData A, NMetrologyData B, NTime t)
{
  m_Metrology1.Interpolate(A.m_Metrology1, B.m_Metrology1, t);
  m_Metrology2.Interpolate(A.m_Metrology2, B.m_Metrology2, t);
}


// NMetrologyData.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
