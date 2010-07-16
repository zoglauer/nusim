/*
 * NPHEFitsData.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *

 */


////////////////////////////////////////////////////////////////////////////////
//
// NPHEFitsData
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NPHEFitsData.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NPHEFitsData)
#endif


////////////////////////////////////////////////////////////////////////////////


NPHEFitsData::NPHEFitsData()
{
  // Construct an instance of NPHEFitsData

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NPHEFitsData::~NPHEFitsData()
{
  // Delete this instance of NPHEFitsData
}


////////////////////////////////////////////////////////////////////////////////


void NPHEFitsData::Clear()
{
  // Reset all data
  
  m_ResetTime = 0;
  m_Frame = 0;
  m_Flag = 0;
  m_SubFrame = 0;
  m_RawX = 0;
  m_RawY = 0;
  
  m_StartCap = 0;
  m_CZTID = 0;
  m_RiseTime = 0;
  m_ElapsedTime = 0;
  m_NHardwareTriggers = 0;
  
  m_NSoftwareTriggers = 0;
  m_PHA = 0;
  m_Energy = 0;
  m_CZTTemperature = 0;
  m_EventTime = 0;
  
  m_PreTriggerSampleSum.clear();
  m_PostTriggerSampleSum.clear();
  m_TriggerMap.clear();
  for (unsigned int i = 0; i < 9; ++i) {
    m_PreTriggerSampleSum.push_back(0);
    m_PostTriggerSampleSum.push_back(0);
    m_TriggerMap.push_back(0);
  }

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


void NPHEFitsData::SetPreTriggerSampleSum(unsigned int PixelID, int PreTriggerSampleSum) 
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


int NPHEFitsData::GetPreTriggerSampleSum(unsigned int PixelID) const 
{ 
  //! Return the pre-trigger pulse height

  if (PixelID == 0 || PixelID > 9) {
    cerr<<"GetPreTriggerSampleSum: Pixel ID must be between 1..9"<<endl;
    return 0.0;
  }

  return m_PreTriggerSampleSum[PixelID - 1]; 
}


////////////////////////////////////////////////////////////////////////////////


void NPHEFitsData::SetPostTriggerSampleSum(unsigned int PixelID, int PostTriggerSampleSum) 
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


int NPHEFitsData::GetPostTriggerSampleSum(unsigned int PixelID) const 
{ 
  //! Return the post-trigger pulse height

  if (PixelID == 0 || PixelID > 9) {
    cerr<<"GetPostTriggerSampleSum: Pixel ID must be between 1..9"<<endl;
    return 0.0;
  }

  return m_PostTriggerSampleSum[PixelID - 1]; 
}


////////////////////////////////////////////////////////////////////////////////


void NPHEFitsData::SetTriggerMap(unsigned int PixelID, int TriggerMap) 
{ 
  //! Set the trigger map value
  
  if (PixelID == 0 || PixelID > 9) {
    cerr<<"SetTrigger: Pixel ID must be between 1..9"<<endl;
    return;
  }

  m_Empty = false; 
  m_TriggerMap[PixelID - 1] = TriggerMap; 
}
 

////////////////////////////////////////////////////////////////////////////////


int NPHEFitsData::GetTriggerMap(unsigned int PixelID) const 
{ 
  //! Return the trigger mapo value

  if (PixelID == 0 || PixelID > 9) {
    cerr<<"GetTrigger: Pixel ID must be between 1..9"<<endl;
    return 0.0;
  }

  return m_TriggerMap[PixelID - 1]; 
}


////////////////////////////////////////////////////////////////////////////////


bool NPHEFitsData::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  S<<"PE ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    merr<<"To be written when fits file is finalized!"<<show;
    S<<"-"<<endl;
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPHEFitsData::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file
  
  if (Line == "PE -") {
    Clear();
    return true;
  }
  
  merr<<"To be written when fits file is finalized!"<<show;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


std::ostream& operator<<(std::ostream& os, const NPHEFitsData& PHE)
{
  os<<"PHE fits data: ";
  os<<" ResetTime: "<<PHE.GetResetTime();
  os<<" Frame: "<<PHE.GetFrame();
  os<<" Flag: "<<PHE.GetFlag();
  os<<" SubFrame: "<<PHE.GetSubFrame();
  os<<" RawX: "<<PHE.GetRawX();
  os<<" RawY: "<<PHE.GetRawY();
  os<<" StartCap: "<<PHE.GetStartCap();
  os<<" CZTID: "<<PHE.GetCZTID();
  os<<" RiseTime: "<<PHE.GetRiseTime();
  os<<" ElapsedTime: "<<PHE.GetElapsedTime();
  os<<" NHardwareTriggers: "<<PHE.GetNHardwareTriggers();
  os<<" PreTriggerSampleSum: ";
  for (unsigned int i = 1; i <= 9; ++i) {
    cout<<PHE.GetPreTriggerSampleSum(i)<<" ";
  }
  os<<" PostTriggerSampleSum: ";
  for (unsigned int i = 1; i <= 9; ++i) {
    cout<<PHE.GetPostTriggerSampleSum(i)<<" ";
  }
  os<<" TriggerMap: ";
  for (unsigned int i = 1; i <= 9; ++i) {
    cout<<PHE.GetTriggerMap(i)<<" ";
  }
  os<<" NSoftwareTriggers: "<<PHE.GetNSoftwareTriggers();
  os<<" PHA: "<<PHE.GetPHA();
  os<<" Energy: "<<PHE.GetEnergy();
  os<<" CZTTemperature: "<<PHE.GetCZTTemperature();
  os<<" EventTime: "<<PHE.GetEventTime();
  return os;
}



// NPHEFitsData.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
