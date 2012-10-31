/*
 * NAttitude.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NAttitude__
#define __NAttitude__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBase.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NAttitude : public NBase
{ 
public:
  NAttitude();
  ~NAttitude();
  
  //! Clean - empty the arrays
  virtual void Clean();

  //! Read the fits file
  bool Read(const TString& AttitudeFileName);
  
  void Add(double Time, double RA, double DEC) {
    m_Time.push_back(Time);
    m_RA.push_back(RA);
    m_DEC.push_back(DEC);
  }

  vector<double> m_RA;
  vector<double> m_DEC;
};


#endif


////////////////////////////////////////////////////////////////////////////////
