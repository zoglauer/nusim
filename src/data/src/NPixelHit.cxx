/*
 * NPixelHit.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *

 */


////////////////////////////////////////////////////////////////////////////////
//
// NPixelHit
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NPixelHit.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NPixelHit)
#endif


////////////////////////////////////////////////////////////////////////////////


NPixelHit::NPixelHit()
{
  // Construct an instance of NPixelHit

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NPixelHit::~NPixelHit()
{
  // Delete this instance of NPixelHit
}


////////////////////////////////////////////////////////////////////////////////


void NPixelHit::Clear()
{
  // Reset all data

  m_Telescope = g_UnsignedIntNotDefined;
  m_Detector = g_UnsignedIntNotDefined;
  m_XPixel = g_UnsignedIntNotDefined;
  m_YPixel = g_UnsignedIntNotDefined;
  m_PreTriggerSampleSum = g_DoubleNotDefined;
  m_PostTriggerSampleSum = g_DoubleNotDefined;
  
  m_IdealAverageDepth = g_DoubleNotDefined;
  m_NoisedAverageDepth = g_DoubleNotDefined;
  m_IdealEnergy = g_DoubleNotDefined;
  m_NoisedEnergy = g_DoubleNotDefined;
  

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPixelHit::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  S<<"PH ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    S<<m_Telescope<<" "<<m_Detector<<" "<<m_XPixel<<" "<<m_YPixel<<" "<<m_PreTriggerSampleSum<<" "<<m_PostTriggerSampleSum<<" "<<m_IdealAverageDepth<<" "<<m_NoisedAverageDepth<<" "<<m_IdealEnergy<<" "<<m_NoisedEnergy<<endl;
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPixelHit::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file

  if (Line == "PH -") {
    Clear();
    return true;
  }

  if (sscanf(Line.Data(), 
             "PH %d %d %d %d %lf %lf %lf %lf %lf %lf", 
             &m_Telescope, &m_Detector,
             &m_XPixel, &m_YPixel,
             &m_PreTriggerSampleSum, &m_PostTriggerSampleSum,
             &m_IdealAverageDepth, &m_NoisedAverageDepth, 
             &m_IdealEnergy, &m_NoisedEnergy) != 10) {
    return false;
  }

  m_Empty = false;

  return true;
}



// NPixelHit.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
