/*
 * NHotPixel.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NHotPixel
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NHotPixel.h"

// Standard libs:

// ROOT libs:
#include "TRandom.h"

// MEGAlib libs:
#include "MStreams.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NHotPixel)
#endif


////////////////////////////////////////////////////////////////////////////////


NHotPixel::NHotPixel()
{
  // Construct an instance of NHotPixel
  
  m_Boundaries = g_VectorNotDefined;
}


////////////////////////////////////////////////////////////////////////////////


NHotPixel::~NHotPixel()
{  
  // Delete this instance of NHotPixel
}


////////////////////////////////////////////////////////////////////////////////


const NHotPixel& NHotPixel::operator=(const NHotPixel& HotPixel)
{
  // Assignment operator

  m_Empty = HotPixel.m_Empty;

  m_Boundaries = HotPixel.m_Boundaries;
  
  m_Time = HotPixel.m_Time;
  
  m_Telescope = HotPixel.m_Telescope;
  m_Detector = HotPixel.m_Detector;

  m_Position = HotPixel.m_Position;
  m_RandomPosition = HotPixel.m_RandomPosition;

  m_Start = HotPixel.m_Start;
  m_Stop = HotPixel.m_Stop;
  
  m_OnDuration = HotPixel.m_OnDuration;
  m_OnDurationUncertainty = HotPixel.m_OnDurationUncertainty;
  
  m_OffDuration = HotPixel.m_OffDuration;
  m_OffDurationUncertainty = HotPixel.m_OffDurationUncertainty;
  
  m_CurrentFireFrequency = HotPixel.m_FireFrequency;
  
  m_FireFrequency = HotPixel.m_FireFrequency;
  m_FireFrequencyUncertainty = HotPixel.m_FireFrequencyUncertainty;

  m_Energy = HotPixel.m_Energy;
  m_EnergyUncertainty = HotPixel.m_EnergyUncertainty;
  
  
  return (*this);
}


////////////////////////////////////////////////////////////////////////////////


void NHotPixel::SetDetectorBoundaries(const MVector& Boundaries)
{
  //! Set detector boundaries

  m_Boundaries = Boundaries;
}
  
  
////////////////////////////////////////////////////////////////////////////////


NInteraction NHotPixel::GetNextInteraction()
{
  //! Create an interaction

  NInteraction I;
  I.SetIsDetector(true);
  I.SetTelescope(m_Telescope);
  I.SetDetector(m_Detector);
  I.SetPosition(m_Position);
  I.SetEnergy(gRandom->Gaus(m_Energy, m_EnergyUncertainty));
  
  Advance();
  
  return I;
}
  

////////////////////////////////////////////////////////////////////////////////


void NHotPixel::PerformTimeJump(const NTime& TimeJump) 
{ 
  //! Perform a time jump:

  m_Time += TimeJump;
  m_Start += TimeJump;
  m_Stop += TimeJump;
} 


////////////////////////////////////////////////////////////////////////////////


void NHotPixel::Advance()
{
  //! Advance the internal data to the next hot pixel event
  
  
  if (m_CurrentFireFrequency > 0) {
    m_Time += gRandom->Exp(1/m_CurrentFireFrequency);
    //cout<<"Really new time: "<<m_Time<<endl;
  } else {
    m_Time = m_Stop;
  }
  if (m_Time >= m_Stop) {
    //cout<<"Stop condition fullfilled!"<<endl;
    if (m_OffDuration == numeric_limits<double>::max()) {
      m_Start.SetMax();
      m_Time.SetMax();
    } else {
      //cout<<"Old time: "<<m_Time<<" vs. stop="<<m_Stop<<endl;
      double OffTime = -1;
      while (OffTime < 0) OffTime = gRandom->Gaus(m_OffDuration, m_OffDurationUncertainty);
      m_Start = m_Stop + OffTime;
      m_Time = m_Start;
      m_Stop = m_Start;
      m_CurrentFireFrequency = 0.0;
      while (m_CurrentFireFrequency <= 0) m_CurrentFireFrequency = gRandom->Gaus(m_FireFrequency, m_FireFrequencyUncertainty);
      if (m_OnDuration > 0) {
        double OnTime = -1;
        while (OnTime < 0) OnTime = gRandom->Gaus(m_OnDuration, m_OnDurationUncertainty); 
        m_Stop += OnTime; 
      }
      if (m_RandomPosition == true) {
        m_Telescope = gRandom->Integer(2)+1;
        m_Detector = gRandom->Integer(4)+1;
        m_Position.SetX((gRandom->Rndm()*2-1)*m_Boundaries.X());
        m_Position.SetY((gRandom->Rndm()*2-1)*m_Boundaries.Y());
        m_Position.SetZ((gRandom->Rndm()*2-1)*m_Boundaries.Z());
      }
      //cout<<"New time: "<<m_Time<<" on until: "<<m_Stop<<endl;
    }
  }
  
}


////////////////////////////////////////////////////////////////////////////////


bool NHotPixel::Parse(TString& Line, int Version)
{
  //! Stream the content from a line of an ASCII file  

  if (m_Boundaries == g_VectorNotDefined) {
    cerr<<"Hot pixel data: "<<Line<<endl;
    cerr<<"Please set the detector boundaries first!"<<endl;
    return false;
  }
  
  MTokenizer T(' ', false);
  T.Analyze(Line);
  if (T.GetNTokens() != 16) {
    cerr<<"Hot pixel data: "<<Line<<endl;
    cerr<<"Not enought tokens: 16 expected, "<<T.GetNTokens()<<" found!"<<endl;
    return false;
  }
  
  unsigned int Pos = 1;
  if (T.GetTokenAt(Pos) == "-") {
    m_Telescope = gRandom->Integer(2)+1;
  } else {
    m_Telescope = T.GetTokenAtAsUnsignedInt(Pos);
    if (m_Telescope < 1 || m_Telescope > 2) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"The telescope ID must be 1 or 2 and not "<<m_Telescope<<")!"<<endl;
      return false;
    }
  }
  
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_Detector = gRandom->Integer(4)+1;
  } else {
    m_Detector = T.GetTokenAtAsUnsignedInt(Pos);
    if (m_Detector < 1 || m_Detector > 3) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"The detector ID must be between 1 and 4 and not "<<m_Detector<<")!"<<endl;
      return false;
    }
  }
  
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_Position.SetX((gRandom->Rndm()*2-1)*m_Boundaries.X());
  } else {
    m_Position.SetX(T.GetTokenAtAsDouble(Pos));
    if (fabs(m_Position.GetX()) > m_Boundaries.X()) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"x-position (entry "<<Pos+1<<": "<<m_Position.GetX()<<") is outside of the detector (max: +-"<<m_Boundaries.X()<<")!"<<endl;
      return false;
    }
  }
   
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_Position.SetY((gRandom->Rndm()*2-1)*m_Boundaries.Y());
  } else {
    m_Position.SetY(T.GetTokenAtAsDouble(Pos));
    if (fabs(m_Position.GetY()) > m_Boundaries.Y()) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"y-position (entry "<<Pos+1<<": "<<m_Position.GetY()<<") is outside of the detector (max: +-"<<m_Boundaries.Y()<<")!"<<endl;
      return false;
    }
  }
   
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_Position.SetZ((gRandom->Rndm()*2-1)*m_Boundaries.Z());
  } else {
    m_Position.SetZ(T.GetTokenAtAsDouble(Pos));
    if (fabs(m_Position.GetZ()) > m_Boundaries.Z()) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"z-position (entry "<<Pos+1<<": "<<m_Position.GetZ()<<") is outside of the detector (max: +-"<<m_Boundaries.Z()<<")!"<<endl;
      return false;
    }
  }
   
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_Start.SetMax(); // Determine later
  } else {
    m_Start.Set(T.GetTokenAtAsDouble(Pos));
  }
  
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    cerr<<"Hot pixel data: "<<Line<<endl;
    cerr<<"On duration (entry "<<Pos+1<<") is mandatory!"<<endl;
    return false;
  } else {
    m_OnDuration = T.GetTokenAtAsDouble(Pos);
    if (m_OnDuration < 0) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"On duration (entry "<<Pos+1<<") must be non-negative!"<<endl;
      return false;      
    }
  }
   
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_OnDurationUncertainty = 0;
  } else {
    m_OnDurationUncertainty = T.GetTokenAtAsDouble(Pos);
    if (m_OnDurationUncertainty < 0) m_OnDurationUncertainty = 0;
  }
  
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_OffDuration = numeric_limits<double>::max();
  } else {
    m_OffDuration = T.GetTokenAtAsDouble(Pos);
    if (m_OffDuration <= 0) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"Off duration (entry "<<Pos+1<<") must be positive!"<<endl;
      return false;      
    }
  }
   
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_OffDurationUncertainty = 0;
  } else {
    m_OffDurationUncertainty = T.GetTokenAtAsDouble(Pos);
    if (m_OffDurationUncertainty < 0) m_OffDurationUncertainty = 0;
  }
   
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    cerr<<"Hot pixel data: "<<Line<<endl;
    cerr<<"\"Same pixel after off\" (entry "<<Pos+1<<") is mandatory!"<<endl;
    return false;
  } else {
    m_RandomPosition = T.GetTokenAtAsBoolean(Pos);
  }
  
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    cerr<<"Hot pixel data: "<<Line<<endl;
    cerr<<"Fire frequency (entry "<<Pos+1<<") is mandatory!"<<endl;
    return false;
  } else {
    m_FireFrequency = T.GetTokenAtAsDouble(Pos);
    if (m_FireFrequency <= 0) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"Fire frequency (entry "<<Pos+1<<") must be positive!"<<endl;
      return false;      
    }
  }
   
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_FireFrequencyUncertainty = 0;
  } else {
    m_FireFrequencyUncertainty = T.GetTokenAtAsDouble(Pos);
  }
  
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    cerr<<"Hot pixel data: "<<Line<<endl;
    cerr<<"Energy (entry "<<Pos+1<<") is mandatory!"<<endl;
    return false;
  } else {
    m_Energy = T.GetTokenAtAsDouble(Pos);
    if (m_Energy <= 0) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"Energy (entry "<<Pos+1<<") must be positive!"<<endl;
      return false;
    }
  }
   
  ++Pos;
  if (T.GetTokenAt(Pos) == "-") {
    m_EnergyUncertainty = 0;
  } else {
    m_EnergyUncertainty = T.GetTokenAtAsDouble(Pos);
    if (m_EnergyUncertainty < 0) m_EnergyUncertainty = 0;
  }

  
  if (m_Start.IsMax() == true) {
    if (m_OffDuration == numeric_limits<double>::max()) {
      cerr<<"Hot pixel data: "<<Line<<endl;
      cerr<<"Cannot determine start time without on off duration!"<<endl;
      return false;
    }
    double OffTime = -1;
    while (OffTime < 0) OffTime = gRandom->Gaus(m_OffDuration, m_OffDurationUncertainty);
    m_Start.Set(OffTime);
  }
  m_Time = m_Start;
  m_Stop = m_Start;
  if (m_OnDuration > 0) {
    double OnTime = -1;
    while (OnTime < 0) OnTime = gRandom->Gaus(m_OnDuration, m_OnDurationUncertainty);
    m_Stop += OnTime;
  }    
 
  m_CurrentFireFrequency = 0.0;
  while (m_CurrentFireFrequency <= 0.0) m_CurrentFireFrequency = gRandom->Gaus(m_FireFrequency, m_FireFrequencyUncertainty);

  cout<<"After init: "<<m_Time<<endl;
  
  m_Empty = false;

  return true;
}


// NHotPixel.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
