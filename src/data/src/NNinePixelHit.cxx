/*
 * NNinePixelHit.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *

 */


////////////////////////////////////////////////////////////////////////////////
//
// NNinePixelHit
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NNinePixelHit.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NNinePixelHit)
#endif


////////////////////////////////////////////////////////////////////////////////


NNinePixelHit::NNinePixelHit()
{
  // Construct an instance of NNinePixelHit

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NNinePixelHit::~NNinePixelHit()
{
  // Delete this instance of NNinePixelHit
}


////////////////////////////////////////////////////////////////////////////////


void NNinePixelHit::Clear()
{
  // Reset all data

  m_Telescope = g_UnsignedIntNotDefined;
  m_Detector = g_UnsignedIntNotDefined;
  m_XPixel = g_UnsignedIntNotDefined;
  m_YPixel = g_UnsignedIntNotDefined;
  
  m_IdealAverageDepth = g_DoubleNotDefined;
  m_NoisedAverageDepth = g_DoubleNotDefined;
  m_IdealEnergy = g_DoubleNotDefined;
  m_NoisedEnergy = g_DoubleNotDefined;
  
  m_PreTriggerSampleSum.clear();
  m_PostTriggerSampleSum.clear();
  m_Trigger.clear();
  for (unsigned int i = 0; i < 9; ++i) {
    m_PreTriggerSampleSum.push_back(0);
    m_PostTriggerSampleSum.push_back(0);
    m_Trigger.push_back(0);
  }
  m_NTriggers = g_UnsignedIntNotDefined;
  m_TriggerGrade = g_UnsignedIntNotDefined;

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


void NNinePixelHit::SetPreTriggerSampleSum(unsigned int PixelID, double PreTriggerSampleSum) 
{ 
  //! Set the pre-trigger pulse height

  if (PixelID == 0 || PixelID > 9) {
    cerr<<"SetPreTriggerSampleSum: Pixel ID must be between 1..9"<<endl;
    return;
  }
  
  m_Empty = false; 
  m_PreTriggerSampleSum[PixelID-1] = PreTriggerSampleSum; 
}
 

////////////////////////////////////////////////////////////////////////////////


double NNinePixelHit::GetPreTriggerSampleSum(unsigned int PixelID) const 
{ 
  //! Return the pre-trigger pulse height

  if (PixelID == 0 || PixelID > 9) {
    cerr<<"GetPreTriggerSampleSum: Pixel ID must be between 1..9"<<endl;
    return 0.0;
  }

  return m_PreTriggerSampleSum[PixelID - 1]; 
}


////////////////////////////////////////////////////////////////////////////////


void NNinePixelHit::SetPostTriggerSampleSum(unsigned int PixelID, double PostTriggerSampleSum) 
{ 
  //! Set the post-trigger pulse height
  
  if (PixelID == 0 || PixelID > 9) {
    cerr<<"SetPostTriggerSampleSum: Pixel ID must be between 1..9"<<endl;
    return;
  }

  m_Empty = false; 
  m_PostTriggerSampleSum[PixelID - 1] = PostTriggerSampleSum; 
}
 

////////////////////////////////////////////////////////////////////////////////


double NNinePixelHit::GetPostTriggerSampleSum(unsigned int PixelID) const 
{ 
  //! Return the post-trigger pulse height

  if (PixelID == 0 || PixelID > 9) {
    cerr<<"GetPostTriggerSampleSum: Pixel ID must be between 1..9"<<endl;
    return 0.0;
  }

  return m_PostTriggerSampleSum[PixelID - 1]; 
}


////////////////////////////////////////////////////////////////////////////////


void NNinePixelHit::SetTrigger(unsigned int PixelID, bool Trigger) 
{ 
  //! Set the post-trigger pulse height
  
  if (PixelID == 0 || PixelID > 9) {
    cerr<<"SetTrigger: Pixel ID must be between 1..9"<<endl;
    return;
  }

  m_Empty = false; 
  m_Trigger[PixelID - 1] = Trigger; 
}
 

////////////////////////////////////////////////////////////////////////////////


bool NNinePixelHit::GetTrigger(unsigned int PixelID) const 
{ 
  //! Return the post-trigger pulse height

  if (PixelID == 0 || PixelID > 9) {
    cerr<<"GetTrigger: Pixel ID must be between 1..9"<<endl;
    return 0.0;
  }

  return m_Trigger[PixelID - 1]; 
}


////////////////////////////////////////////////////////////////////////////////


bool NNinePixelHit::Stream(ofstream& S, int Version)
{
  //! Stream the content to an ASCII file 

  S<<"NH ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    S<<m_Telescope<<" "<<m_Detector<<" "<<m_XPixel<<" "<<m_YPixel<<" ";
    for (unsigned int i = 0; i < 9; ++i) {
      S<<m_PreTriggerSampleSum[i]<<" "<<m_PostTriggerSampleSum[i]<<" "<<m_Trigger[i]<<" ";
    }
    S<<m_IdealAverageDepth<<" "<<m_NoisedAverageDepth<<" "<<m_IdealEnergy<<" "<<m_NoisedEnergy;
    S<<endl;
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NNinePixelHit::Parse(TString& Line, int Version)
{
  //! Stream the content from a line of an ASCII file

  if (Line == "NH -") {
    Clear();
    return true;
  }

  vector<int> Trigger(9, 0);
  if (sscanf(Line.Data(), 
             "NH %d %d %d %d %lf %lf %d %lf %lf %d %lf %lf %d %lf %lf %d %lf %lf %d %lf %lf %d %lf %lf %d %lf %lf %d %lf %lf %d %lf %lf %lf %lf", 
             &m_Telescope, &m_Detector,
             &m_XPixel, &m_YPixel,
             &m_PreTriggerSampleSum[0], &m_PostTriggerSampleSum[0], &Trigger[0],  
             &m_PreTriggerSampleSum[1], &m_PostTriggerSampleSum[1], &Trigger[1],
             &m_PreTriggerSampleSum[2], &m_PostTriggerSampleSum[2], &Trigger[2],
             &m_PreTriggerSampleSum[3], &m_PostTriggerSampleSum[3], &Trigger[3],
             &m_PreTriggerSampleSum[4], &m_PostTriggerSampleSum[4], &Trigger[4],
             &m_PreTriggerSampleSum[5], &m_PostTriggerSampleSum[5], &Trigger[5], 
             &m_PreTriggerSampleSum[6], &m_PostTriggerSampleSum[6], &Trigger[6],
             &m_PreTriggerSampleSum[7], &m_PostTriggerSampleSum[7], &Trigger[7],
             &m_PreTriggerSampleSum[8], &m_PostTriggerSampleSum[8], &Trigger[8],
             &m_IdealAverageDepth, &m_NoisedAverageDepth, 
             &m_IdealEnergy, &m_NoisedEnergy) != 35) {
    return false;
  }

  m_Empty = false;

  for (unsigned int i = 0; i < 9; ++i) {
    if (Trigger[i] == 0) m_Trigger[i] = false; else m_Trigger[i] = true;
  }
  
  return true;
}



// NNinePixelHit.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
