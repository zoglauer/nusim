/*
 * NStarTrackerDataSet.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NStarTrackerDataSet
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NStarTrackerDataSet.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NStarTrackerDataSet)
#endif


////////////////////////////////////////////////////////////////////////////////


NStarTrackerDataSet::NStarTrackerDataSet()
{
  // Construct an instance of NStarTrackerDataSet

  m_Empty = false;
  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NStarTrackerDataSet::~NStarTrackerDataSet()
{
  // Delete this instance of NStarTrackerDataSet
}


////////////////////////////////////////////////////////////////////////////////


void NStarTrackerDataSet::Clear() 
{
  // Reset the star tracker data to not defined values

  if (m_Empty == true) return;

  m_Time.Clear();
  m_OriginalPointing.Clear();
  m_ReconstructedPointing.Clear();
  m_Transformation = g_QuaternionNotDefined;

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NStarTrackerDataSet::Stream(ofstream& S, TString Keyword)
{
  //! Stream the content to an ASCII file 

  S<<Keyword<<" ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    S<<m_Time.GetSeconds()<<" "<<m_OriginalPointing.GetRa()<<" "<<m_OriginalPointing.GetDec()<<" "<<m_OriginalPointing.GetRoll()<<endl;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NStarTrackerDataSet::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  

  if (Line.Length() == 4 && Line[4] == '-') {
    Clear();
    return true;
  }

  char Key[2];
  double T, Ra, Dec, Roll;
  Roll = 180.0*60;
  if (sscanf(Line.Data(), "%s %lf %lf %lf %lf", Key, &T, &Ra, &Dec, &Roll) != 5) {
    if (sscanf(Line.Data(), "%s %lf %lf %lf", Key, &T, &Ra, &Dec) != 4) {
      return false;
    }
  }
  m_Time.SetSeconds(T);
  m_OriginalPointing.SetRaDecRoll(Ra, Dec, Roll);

  m_Empty = false;

  return true;
}



// NStarTrackerDataSet.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
