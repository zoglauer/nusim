/*
 * NOrbits.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NOrbits__
#define __NOrbits__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBase.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NOrbits : public NBase
{ 
public:
  NOrbits();
  virtual ~NOrbits();
  
  //! Clean - empty the arrays
  virtual void Clean();

  void Add(double Time, double TimeAfterSAA, int SAAFlag, int SAAAFlag, int SAABFlag, int SlewFlag, int DayFlag, int OccultedFlag, double GeomagneticCutOff, double Longitude, double Latitude, double Altitude) {
    m_Time.push_back(Time);
    m_TimeAfterSAA.push_back(TimeAfterSAA);
    m_SAAFlag.push_back(SAAFlag);
    m_SAAAFlag.push_back(SAAAFlag);
    m_SAABFlag.push_back(SAABFlag);
    m_SlewFlag.push_back(SlewFlag);
    m_DayFlag.push_back(DayFlag);
    m_OccultedFlag.push_back(OccultedFlag);
    m_GeomagneticCutOff.push_back(GeomagneticCutOff);
    m_Longitude.push_back(Longitude);
    m_Latitude.push_back(Latitude);
    m_Altitude.push_back(Altitude);
    m_SafelyOcculted.push_back(0);
    m_SafelyOnSource.push_back(0);
  }
  
  bool IsGoodOrbit(unsigned int i) {
    if (i >= m_OccultedFlag.size()) return false;
    if (m_OccultedFlag[i] != 0 || m_SAAAFlag[i] != 0 || m_SAABFlag[i] != 0 || m_SlewFlag[i] != 0) return false;
    return true;
  }
  
  bool IsGoodOrbit(double Time) {
    return IsGoodOrbit(FindOrbitIndex(Time));
  }
  
  unsigned int FindOrbitIndex(double Time);
  
  double GetMinTime() { return m_Time[0]; }
  double GetMaxTime() { return m_Time.back(); }
  
  double GetMinGeomagneticCutOff();
  
  double GetMaxGeomagneticCutOff();

  //! Read the Orbits fits file
  bool Read(const TString& OrbitsFileName);

  
  vector<double> m_TimeAfterSAA;
  vector<int> m_OccultedFlag;
  vector<int> m_SafelyOcculted;
  vector<int> m_SafelyOnSource;
  vector<int> m_SAAFlag;
  vector<int> m_SAAAFlag;
  vector<int> m_SAABFlag;
  vector<int> m_SlewFlag;
  vector<int> m_DayFlag;
  vector<double> m_GeomagneticCutOff;
  vector<double> m_Longitude;
  vector<double> m_Latitude;
  vector<double> m_Altitude;
  
  vector<double> m_OccultationSteps;
  vector<double> m_OccultationValues;
  vector<double> m_SAASteps;
  vector<double> m_SAAValues;
  vector<double> m_DaySteps;
  vector<double> m_DayValues;
  vector<double> m_SlewSteps;
  vector<double> m_SlewValues;
};

#endif


////////////////////////////////////////////////////////////////////////////////
