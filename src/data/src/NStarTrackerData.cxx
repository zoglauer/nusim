/*
 * NStarTrackerData.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NStarTrackerData
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NStarTrackerData.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NStarTrackerData)
#endif


////////////////////////////////////////////////////////////////////////////////


NStarTrackerData::NStarTrackerData()
{
  // Construct an instance of NStarTrackerData

  m_Empty = false;
  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NStarTrackerData::~NStarTrackerData()
{
  // Delete this instance of NStarTrackerData
}


////////////////////////////////////////////////////////////////////////////////


void NStarTrackerData::Clear() 
{
  // Reset the star tracker data to not defined values

  if (m_Empty == true) return;

  m_StarTracker4.Clear();

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NStarTrackerData::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  m_StarTracker4.Stream(S, "S4");

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NStarTrackerData::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file

  if (Line.BeginsWith("S4") == true) {
    if (m_StarTracker4.Parse(Line) == false) {
      return false;
    }
  } else {
    return false;
  }

  m_Empty = false;

  return true;
}


// NStarTrackerData.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
