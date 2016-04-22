/*
 * NGTI.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NGTI__
#define __NGTI__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <iomanip>
using namespace std;

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBase.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGTI : public NBase
{ 
public:
  NGTI();
  virtual ~NGTI();
  
  //! Clean - empty the arrays
  virtual void Clean();
  
  //! Read the fits file
  bool Read(const TString& FileName);

  void Add(double Start, double Stop) {
    m_Start.push_back(Start);
    m_Stop.push_back(Stop);
  }
  
  bool IsGTI(double Time) {
    for (unsigned int i = 0; i < m_Start.size(); ++i) {
      if (Time >= m_Start[i] && Time < m_Stop[i]) {
        return true;
      }
    }
    return false;
  } 
  vector<double> m_Start;
  vector<double> m_Stop;
};

#endif


////////////////////////////////////////////////////////////////////////////////
