/*
 * NEngineering.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NEngineering__
#define __NEngineering__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBase.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NEngineering : public NBase
{ 
public:
  NEngineering();
  virtual ~NEngineering();
  
  //! Clean - empty the arrays
  virtual void Clean();

  //! Read the fits file
  bool Read(const TString& EngineeringFileName);

  void Add(double Time, double EarthAngle, double LimbAngle) {
    m_Time.push_back(Time);
    m_EarthAngle.push_back(EarthAngle);
    m_LimbAngle.push_back(LimbAngle);
  }
  
  vector<double> m_EarthAngle;
  vector<double> m_LimbAngle;
};

#endif


////////////////////////////////////////////////////////////////////////////////
