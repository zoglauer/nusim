/*
 * NEvent.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NEvent
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NEvent.h"

// Standard libs:
#include <iomanip>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NEvent)
#endif


////////////////////////////////////////////////////////////////////////////////


NEvent::NEvent()
{
  // Construct an instance of NEvent

  m_Empty = false;
  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NEvent::~NEvent()
{
  // Nothing needs to be done...
}


////////////////////////////////////////////////////////////////////////////////


const NEvent& NEvent::operator=(const NEvent& Event)
{
  // Assignment operator

  m_Empty = Event.m_Empty;

  m_Origin = Event.m_Origin;

  m_Ra = Event.m_Ra;
  m_Dec = Event.m_Dec;
  
  m_IsMeasured = Event.m_IsMeasured;
  
  m_Orientations = Event.m_Orientations;
  
  m_ID = Event.m_ID;
  m_Telescope = Event.m_Telescope;

  m_Time = Event.m_Time;
  m_DetectorLifeTime = Event.m_DetectorLifeTime;

  m_Blocked = Event.m_Blocked;
  m_LostInDeadTime = Event.m_LostInDeadTime;
  m_VetoLow = Event.m_VetoLow;
  m_VetoHigh = Event.m_VetoHigh;
  m_Trigger = Event.m_Trigger;
  m_PileUp = Event.m_PileUp;
  m_NPixelTriggers = Event.m_NPixelTriggers;
  m_EventCut = Event.m_EventCut;

  m_Interactions = Event.m_Interactions;
  m_PHEData = Event.m_PHEData;
  m_PixelHits = Event.m_PixelHits;
  m_ShieldHits = Event.m_ShieldHits;
  m_NinePixelHits = Event.m_NinePixelHits;
  m_Hits = Event.m_Hits;

  m_OriginalPhoton = Event.m_OriginalPhoton;
  m_InitialPhotonRelOM = Event.m_InitialPhotonRelOM;
  m_CurrentPhoton = Event.m_CurrentPhoton;

  return (*this);
}


////////////////////////////////////////////////////////////////////////////////


void NEvent::Clear()
{
  //! Reset all data

  if (m_Empty == true) return;

  m_Origin = c_OriginUnknown;

  m_ID = 0;
  m_Telescope = g_IntNotDefined;
  m_Time.Clear();
  m_DetectorLifeTime.Clear();
  m_IsMeasured = false;

  m_Blocked = false;
  m_LostInDeadTime = false;
  m_VetoLow = false;
  m_VetoHigh = false;
  m_Trigger = true;
  m_PileUp = false;
  m_NPixelTriggers = 0;
  m_EventCut = false;

  m_Ra = 0.0;
  m_Dec = 0.0;

  if (m_Orientations.IsEmpty() == false) m_Orientations.Clear();
  
  m_Interactions.clear();
  m_PHEData.Clear();
  m_PixelHits.clear();
  m_ShieldHits.clear();
  m_NinePixelHits.clear();
  m_Hits.clear();

  m_OriginalPhoton.Clear();
  m_CurrentPhoton.Clear();
  m_InitialPhotonRelOM.Clear();

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NEvent::Stream(ofstream& S, int Version, int WhatToStream)
{
  //! Stream the content to an ASCII file 

  S<<"SE"<<endl;
  S<<"TI "<<m_Time.GetString(9)<<endl;  // <-- TI MUST be the second one, since the supervisor must know the next event time!
  S<<"ID "<<m_ID<<endl;
  S<<"TE "<<m_Telescope<<endl;
  S<<"OG "<<m_Origin<<endl;
  if (WhatToStream < 2) {
    S<<"MS "<<((m_IsMeasured) ? 1 : 0)<<endl; 
    S<<"RD "<<m_Ra<<" "<<m_Dec<<endl;
    if (m_Orientations.IsEmpty() == false) m_Orientations.Stream(S, Version);
    if (m_OriginalPhoton.IsEmpty() == false) m_OriginalPhoton.Stream(S, Version, "OP");
    if (m_InitialPhotonRelOM.IsEmpty() == false) m_InitialPhotonRelOM.Stream(S, Version, "IP");
    if (m_CurrentPhoton.IsEmpty() == false) m_CurrentPhoton.Stream(S, Version, "CP");
    S<<"PU "<<((m_PileUp) ? 1 : 0)<<endl; 
    for (unsigned int i = 0; i < m_Interactions.size(); ++i) {
      m_Interactions[i].Stream(S, Version);
    }
    m_PHEData.Stream(S, Version);
    for (unsigned int i = 0; i < m_PixelHits.size(); ++i) {
      m_PixelHits[i].Stream(S, Version);
    }
    for (unsigned int i = 0; i < m_ShieldHits.size(); ++i) {
      m_ShieldHits[i].Stream(S, Version);
    }
    for (unsigned int i = 0; i < m_NinePixelHits.size(); ++i) {
      m_NinePixelHits[i].Stream(S, Version);
    }
  }
  for (unsigned int i = 0; i < m_Hits.size(); ++i) {
    m_Hits[i].Stream(S, Version);
  }
  S<<"LT "<<m_DetectorLifeTime.GetString(9)<<endl;


  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NEvent::Parse(TString& Line, int Version)
{
  //! Stream the content from a line of an ASCII file  

  bool Error = false;

  const char* Data = Line.Data();
  if (Data[0] == 'T' && Data[1] == 'I') {
    m_Time.Set(Data);
  } else if (Data[0] == 'I' && Data[1] == 'D') {
    if (sscanf(Data, "ID %lu", &m_ID) != 1) {
      Error = true;
    } 
  } else if (Data[0] == 'T' && Data[1] == 'E') {
    if (sscanf(Data, "TE %d", &m_Telescope) != 1) {
      Error = true;
    } 
  } else if (Data[0] == 'O' && Data[1] == 'G') {
    if (sscanf(Data, "OG %d", &m_Origin) != 1) {
      Error = true;
    } 
  } else if (Data[0] == 'M' && Data[1] == 'S') {
    int i;
    if (sscanf(Data, "MS %d", &i) != 1) {
      Error = true;
    }
    m_IsMeasured = ((i == 1) ? true : false); 
  } else if (Data[0] == 'P' && Data[1] == 'U') {
    int i;
    if (sscanf(Data, "PU %d", &i) != 1) {
      Error = true;
    }
    m_PileUp = ((i == 1) ? true : false); 
  } else if (Data[0] == 'R' && Data[1] == 'D') {
    if (sscanf(Data, "RD %lf %lf", &m_Ra, &m_Dec) != 2) {
      Error = true;
    } 
  } else if (Data[0] == 'O' && Data[1] == 'R') {
    if (m_Orientations.Parse(Line, Version) == false) {
      Error = true;
    }
  } else if (Data[0] == 'O' && Data[1] == 'P') {
    if (m_OriginalPhoton.Parse(Line, Version) == false) {
      Error = true;
    }
  } else if (Data[0] == 'I' && Data[1] == 'P') {
    if (m_InitialPhotonRelOM.Parse(Line, Version) == false) {
      Error = true;
    }
  } else if (Data[0] == 'C' && Data[1] == 'P') {
    if (m_CurrentPhoton.Parse(Line, Version) == false) {
      Error = true;
    }
  } else if (Data[0] == 'I' && Data[1] == 'A') {
    NInteraction IA;
    if (IA.Parse(Line, Version) == true) {
      m_Interactions.push_back(IA);
    } else {
      Error = true;
    }
  } else if (Data[0] == 'P' && Data[1] == 'E') {
    if (m_PHEData.Parse(Line, Version) == false) {
      Error = true;
    }
  } else if (Data[0] == 'P' && Data[1] == 'H') {
    NPixelHit PH;
    if (PH.Parse(Line, Version) == true) {
      m_PixelHits.push_back(PH);
    } else {
      Error = true;
    }
  } else if (Data[0] == 'S' && Data[1] == 'H') {
    NShieldHit SH;
    if (SH.Parse(Line, Version) == true) {
      m_ShieldHits.push_back(SH);
    } else {
      Error = true;
    }
  } else if (Data[0] == 'N' && Data[1] == 'H') {
    NNinePixelHit NH;
    if (NH.Parse(Line, Version) == true) {
      m_NinePixelHits.push_back(NH);
    } else {
      Error = true;
    }
  } else if (Data[0] == 'H' && Data[1] == 'T') {
    NHit HT;
    if (HT.Parse(Line, Version) == true) {
      m_Hits.push_back(HT);
    } else {
      Error = true;
    }
  } else if (Data[0] == 'L' && Data[1] == 'T') {
    m_DetectorLifeTime.Set(Data, "LT");
  } else {
    Error = true;
  }
  
  if (Error == true) {
    cerr<<"Unable to parse line:"<<endl;
    cerr<<Data<<endl;
    return false;
  }

  m_Empty = false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


TString NEvent::GetOriginString() const
{
  // Return the Origin of this event as string

  if (m_Origin == c_OriginSource) {
    return "Source";
  } else if (m_Origin == c_OriginBackground) {
    return "Background";
  }

  return "Unknown";
}


////////////////////////////////////////////////////////////////////////////////


void NEvent::SetOriginalPhoton(const NPhoton& Photon)
{
  // Set the original photon parameters

  m_Empty = false;

  m_OriginalPhoton = Photon;
  m_CurrentPhoton = Photon;
}


////////////////////////////////////////////////////////////////////////////////


void NEvent::SetCurrentPhoton(const NPhoton& Photon)
{
  // Set the original photon parameters

  m_Empty = false;

  m_CurrentPhoton = Photon;
}


////////////////////////////////////////////////////////////////////////////////


NInteraction NEvent::GetInteraction(unsigned int i) 
{ 
  //! Return interaction i
  
  if (i < m_Interactions.size()) {
    return m_Interactions[i]; 
  }

  merr<<"Interaction index out of bounds!"<<show;

  return NInteraction();
}


////////////////////////////////////////////////////////////////////////////////


NInteraction& NEvent::GetInteractionRef(unsigned int i) 
{ 
  //! Return interaction i
  
  if (i < m_Interactions.size()) {
    return m_Interactions[i]; 
  }

  merr<<"Interaction index out of bounds!"<<fatal;

  return m_Interactions[0];
}


////////////////////////////////////////////////////////////////////////////////


NPixelHit NEvent::GetPixelHit(unsigned int i) 
{ 
  //! Return pixel hit i
  
  if (i < m_PixelHits.size()) {
    return m_PixelHits[i]; 
  }

  merr<<"PixelHit index out of bounds ([0.."<<m_PixelHits.size()<<"[): "<<i<<fatal;

  return NPixelHit();
}


////////////////////////////////////////////////////////////////////////////////


NPixelHit& NEvent::GetPixelHitRef(unsigned int i) 
{ 
  //! Return pixel hit i
  
  if (i < m_PixelHits.size()) {
    return m_PixelHits[i]; 
  }

  merr<<"PixelHit index out of bounds ([0.."<<m_PixelHits.size()<<"[): "<<i<<fatal;

  return m_PixelHits[0];
}


////////////////////////////////////////////////////////////////////////////////


void NEvent::RemovePixelHit(unsigned int i)
{
  //! Remove a pixel hit
 
  if (i < m_PixelHits.size()) {
    m_PixelHits.erase(m_PixelHits.begin()+i);
  }
}


////////////////////////////////////////////////////////////////////////////////


NShieldHit NEvent::GetShieldHit(unsigned int i) 
{ 
  //! Return pixel hit i
  
  if (i < m_ShieldHits.size()) {
    return m_ShieldHits[i]; 
  }

  merr<<"ShieldHit index out of bounds ([0.."<<m_ShieldHits.size()<<"[): "<<i<<show;

  return NShieldHit();
}


////////////////////////////////////////////////////////////////////////////////


NShieldHit& NEvent::GetShieldHitRef(unsigned int i) 
{ 
  //! Return pixel hit i
  
  if (i < m_ShieldHits.size()) {
    return m_ShieldHits[i]; 
  }

  merr<<"ShieldHit index out of bounds ([0.."<<m_ShieldHits.size()<<"[): "<<i<<fatal;

  return m_ShieldHits[0];
}


////////////////////////////////////////////////////////////////////////////////


void NEvent::RemoveShieldHit(unsigned int i)
{
  //! Remove a pixel hit
 
  if (i < m_ShieldHits.size()) {
    m_ShieldHits.erase(m_ShieldHits.begin()+i);
  }
}


////////////////////////////////////////////////////////////////////////////////


NNinePixelHit NEvent::GetNinePixelHit(unsigned int i) 
{ 
  //! Return pixel hit i
  
  if (i < m_NinePixelHits.size()) {
    return m_NinePixelHits[i]; 
  }

  merr<<"NinePixelHit index out of bounds ([0.."<<m_NinePixelHits.size()<<"[): "<<i<<show;
  abort();

  return NNinePixelHit();
}


////////////////////////////////////////////////////////////////////////////////


NNinePixelHit& NEvent::GetNinePixelHitRef(unsigned int i) 
{ 
  //! Return pixel hit i
  
  if (i < m_NinePixelHits.size()) {
    return m_NinePixelHits[i]; 
  }

  merr<<"NinePixelHit index out of bounds ([0.."<<m_NinePixelHits.size()<<"[): "<<i<<fatal;
  abort();

  return m_NinePixelHits[0];
}


////////////////////////////////////////////////////////////////////////////////


void NEvent::RemoveNinePixelHit(unsigned int i)
{
  //! Remove a pixel hit
 
  if (i < m_NinePixelHits.size()) {
    m_NinePixelHits.erase(m_NinePixelHits.begin()+i);
  }
}


////////////////////////////////////////////////////////////////////////////////


NHit NEvent::GetHit(unsigned int i) 
{ 
  //! Return hit i
  
  if (i < m_Hits.size()) {
    return m_Hits[i]; 
  }

  merr<<"Hit index out of bounds ([0.."<<m_Hits.size()<<"[): "<<i<<show;

  return NHit();
}


////////////////////////////////////////////////////////////////////////////////


NHit& NEvent::GetHitRef(unsigned int i) 
{ 
  //! Return hit i
  
  if (i < m_Hits.size()) {
    return m_Hits[i]; 
  }

  merr<<"Hit index out of bounds ([0.."<<m_Hits.size()<<"[): "<<i<<fatal;

  return m_Hits[0];
}


// NEvent.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
