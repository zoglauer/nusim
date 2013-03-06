/*
 * NBadPixel.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBadPixel__
#define __NBadPixel__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBase.h"
#include "NFilteredEvents.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBadPixel : public NBase
{ 
public:
  NBadPixel();
  ~NBadPixel();
  
  //! Clean - empty the arrays
  virtual void Clean();

  //! Read the fits file
  bool Read(const TString& BadPixelFileName);
  
  void Add(int DetectorID, int RawX, int RawY, double Start, double Stop) {
    m_DetectorID.push_back(DetectorID);
    m_RawX.push_back(RawX);
    m_RawY.push_back(RawY);
    m_Start.push_back(Start);
    m_Stop.push_back(Stop);
    m_CutGTI.push_back(0);
  }
  
  bool CalculateUpTimes(NFilteredEvents& F);

  double GetUpTime(int DetectorID, int RawX, int RawY) { return m_UpTimes[32*32*DetectorID + 32*RawY + RawX]; }
  
  vector<int> m_DetectorID;
  vector<int> m_RawX;
  vector<int> m_RawY;
  vector<double> m_Start;
  vector<double> m_Stop;
  vector<double> m_CutGTI;
  
  vector<double> m_UpTimes; // Different int - loops over all pixels via: 32*32*DetectorID + 32*RawY + RawX
};


#endif


////////////////////////////////////////////////////////////////////////////////
