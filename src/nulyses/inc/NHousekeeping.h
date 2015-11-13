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
    m_HardSAA.push_back(false);
    m_SoftSAA.push_back(false);
    m_SoftSAAStrictHSR.push_back(false);
    m_SoftSAAOptimizedHSRFoM.push_back(false);
    m_SoftSAAStrictLSR.push_back(false);
    m_SoftSAAOptimizedLSRRMS.push_back(false);
    m_SoftTentacled.push_back(false);
    m_SoftTentacledFoM.push_back(false);
    m_SoftTentacledRMS.push_back(false);
  }
  
  double GetMinTime() { return m_Time[0]; }
  double GetMaxTime() { return m_Time.back(); }
  
  vector<double> m_ShieldRateLow;
  vector<double> m_ShieldRateHigh;
  vector<int> m_SWSAA;
  vector<int> m_SWTentacled;
  vector<bool> m_HardSAA;
  vector<bool> m_SoftSAA;
  vector<bool> m_SoftSAAStrictHSR;
  vector<bool> m_SoftSAAOptimizedHSRFoM;
  vector<bool> m_SoftSAAStrictLSR;
  vector<bool> m_SoftSAAOptimizedLSRRMS;
  vector<bool> m_SoftTentacled;
  vector<bool> m_SoftTentacledFoM;
  vector<bool> m_SoftTentacledRMS;
  vector<double> m_LiveTime;
  vector<int> m_NAcceptedEvents;
  vector<int> m_NRejectedEvents;
};

#endif


////////////////////////////////////////////////////////////////////////////////
