/*
 * NUnfilteredEvents.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NUnfilteredEvents__
#define __NUnfilteredEvents__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBase.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NUnfilteredEvents : public NBase
{ 
public:
  NUnfilteredEvents();
  virtual ~NUnfilteredEvents();
  
  //! Clean - empty the arrays
  virtual void Clean();

  //! Read the fits file
  bool Read(const TString& FileName);
  
  void Add(double Time, double Energy, double SurrEnergy, int Grade, int DetectorID, int RawX, int RawY, int ShieldVeto, int BaseLine, vector<int> PrePhase, vector<int> PostPhase, double Prior, double Reset, unsigned long Status) {
    m_Time.push_back(Time);
    m_Energy.push_back(Energy);
    m_SurrEnergy.push_back(SurrEnergy);
    m_Grade.push_back(Grade);
    m_DetectorID.push_back(DetectorID);
    m_RawX.push_back(RawX);
    m_RawY.push_back(RawY);
    m_ShieldVeto.push_back(ShieldVeto);
    m_BaseLine.push_back(BaseLine);
    m_PrePhase.push_back(PrePhase);
    m_PostPhase.push_back(PostPhase);
    m_Prior.push_back(Prior);
    m_Reset.push_back(Reset);
    m_Status.push_back(Status);
  }
  
  double GetMinTime() { return m_Time[0]; }
  double GetMaxTime() { return m_Time.back(); }
  
  int FindShieldVeto(double Time);
  
  vector<double> m_Energy;
  vector<double> m_SurrEnergy;
  vector<double> m_Prior;
  vector<double> m_Reset;
  vector<int> m_Grade;
  vector<int> m_DetectorID;
  vector<int> m_RawX;
  vector<int> m_RawY;
  vector<int> m_ShieldVeto;
  vector<int> m_BaseLine;
  vector<vector<int> > m_PrePhase;
  vector<vector<int> > m_PostPhase;
  vector<unsigned int> m_Status;
};

#endif


////////////////////////////////////////////////////////////////////////////////
