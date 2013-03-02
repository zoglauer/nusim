/*
 * NHousekeeping.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NHousekeeping__
#define __NHousekeeping__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBase.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NHousekeeping : public NBase
{ 
public:
  NHousekeeping();
  virtual ~NHousekeeping();
  
  //! Clean - empty the arrays
  virtual void Clean();
  
  //! Read the fits file
  bool Read(const TString& FileName);

  
  void Add(double Time, double ShieldRateLow, double ShieldRateHigh, double LiveTime, int NAcceptedEvents, int NRejectedEvents, bool SWSAA, bool SWTentacle) {
    m_Time.push_back(Time);
    m_ShieldRateLow.push_back(ShieldRateLow);
    m_ShieldRateHigh.push_back(ShieldRateHigh);
    m_LiveTime.push_back(LiveTime);
    m_NAcceptedEvents.push_back(NAcceptedEvents);
    m_NRejectedEvents.push_back(NAcceptedEvents);
    m_SWSAA.push_back(SWSAA);
    m_SWTentacled.push_back(SWTentacle);
    m_SoftSAA.push_back(false);
    m_SoftSAAStrict.push_back(false);
    m_SoftSAAOptimized.push_back(false);
    m_SoftSAAOptimizedByLifeTime.push_back(false);
    m_SoftTentacled.push_back(false);
  }
  
  double GetMinTime() { return m_Time[0]; }
  double GetMaxTime() { return m_Time.back(); }
  
  vector<double> m_ShieldRateLow;
  vector<double> m_ShieldRateHigh;
  vector<int> m_SWSAA;
  vector<int> m_SWTentacled;
  vector<bool> m_SoftSAA;
  vector<bool> m_SoftSAAStrict;
  vector<bool> m_SoftSAAOptimized;
  vector<bool> m_SoftSAAOptimizedByLifeTime;
  vector<bool> m_SoftTentacled;
  vector<double> m_LiveTime;
  vector<int> m_NAcceptedEvents;
  vector<int> m_NRejectedEvents;
};

#endif


////////////////////////////////////////////////////////////////////////////////
