/*
 * NFilteredEvents.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NFilteredEvents__
#define __NFilteredEvents__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NBase.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NFilteredEvents : public NBase
{ 
public:
  NFilteredEvents();
  virtual ~NFilteredEvents();
  
  //! Clean - empty the arrays
  virtual void Clean();
  
  //! Read the fits file
  bool Read(const TString& FileName);

  
  void Add(double Time, int PI, double Energy, double SurrEnergy, int Grade, int DetectorID, int RawX, int RawY, int Det1X, int Det1Y, int X, int Y) {
    m_Time.push_back(Time);
    m_PI.push_back(PI);
    m_Energy.push_back(Energy);
    m_SurrEnergy.push_back(SurrEnergy);
    m_Grade.push_back(Grade);
    m_DetectorID.push_back(DetectorID);
    m_RawX.push_back(RawX);
    m_RawY.push_back(RawY);
    m_Det1X.push_back(Det1X);
    m_Det1Y.push_back(Det1Y);
    m_X.push_back(X);
    m_Y.push_back(Y);
  }
  void AddGTI(double Start, double Stop, bool Good) {
    m_GTIStart.push_back(Start); 
    m_GTIStop.push_back(Stop);
    m_GoodGTI.push_back(Good);
  }
  
  bool IsGTI(double Time, bool TestForGood = false) {
    for (unsigned int i = 0; i < m_GTIStart.size(); ++i) {
      if (Time >= m_GTIStart[i] && Time <= m_GTIStop[i]) {
        if (TestForGood == true) {
          return m_GoodGTI[i];
        } else {
          return true;
        }
      }
    }
    return false;
  }
  
  void Merge(NFilteredEvents M) {
    if (m_Time.size() == 0) {
      cout<<"Copying filtered events..."<<endl;
      m_Time = M.m_Time;
      m_ID = M.m_ID;
      m_Module = M.m_Module;
      m_LiveTime = M.m_LiveTime;
      m_StartTime = M.m_StartTime;
      m_ObservationDate = M.m_ObservationDate;
      m_Object = M.m_Object;
      m_DetectorArea = M.m_DetectorArea;
      m_PI = M.m_PI;
      m_Energy = M.m_Energy;
      m_SurrEnergy = M.m_SurrEnergy;
      m_Grade = M.m_Grade;
      m_DetectorID = M.m_DetectorID;
      m_RawX = M.m_RawX;
      m_RawY = M.m_RawY;
      m_Det1X = M.m_Det1X;
      m_Det1Y = M.m_Det1Y;
      m_X = M.m_X;
      m_Y = M.m_Y;
      m_GTIStart = M.m_GTIStart;
      m_GTIStop = M.m_GTIStop;
      m_GoodGTI = M.m_GoodGTI;
    } else {
      cout<<"Merging filtered events..."<<endl;
      m_LiveTime += M.m_LiveTime;
      for (unsigned int m = 0; m < M.m_Time.size(); ++m) {
        int Index = FindFirstIndexLarger(M.m_Time[m]);
        // Find the number of indices to insert:
        unsigned int m_max = m;
        for (m_max = m+1; m_max < M.m_Time.size(); ++m_max) {
          if (Index != FindFirstIndexLarger(M.m_Time[m_max], true)) break;
        }
        --m_max;
        if (Index == -1) {
          m_Time.insert(m_Time.end(), M.m_Time.begin()+m, M.m_Time.begin()+m_max);
          m_PI.insert(m_PI.end(), M.m_PI.begin()+m, M.m_PI.begin()+m_max);
          m_Energy.insert(m_Energy.end(), M.m_Energy.begin()+m, M.m_Energy.begin()+m_max);
          m_SurrEnergy.insert(m_SurrEnergy.end(), M.m_SurrEnergy.begin()+m, M.m_SurrEnergy.begin()+m_max);
          m_Grade.insert(m_Grade.end(), M.m_Grade.begin()+m, M.m_Grade.begin()+m_max);
          m_DetectorID.insert(m_DetectorID.end(), M.m_DetectorID.begin()+m, M.m_DetectorID.begin()+m_max);
          m_RawX.insert(m_RawX.end(), M.m_RawX.begin()+m, M.m_RawX.begin()+m_max);
          m_RawY.insert(m_RawY.end(), M.m_RawY.begin()+m, M.m_RawY.begin()+m_max);
          m_Det1X.insert(m_Det1X.end(), M.m_Det1X.begin()+m, M.m_Det1X.begin()+m_max);
          m_Det1Y.insert(m_Det1Y.end(), M.m_Det1Y.begin()+m, M.m_Det1Y.begin()+m_max);
          m_X.insert(m_X.end(), M.m_X.begin()+m, M.m_X.begin()+m_max);
          m_Y.insert(m_Y.end(), M.m_Y.begin()+m, M.m_Y.begin()+m_max);
        } else {
          m_Time.insert(m_Time.begin()+Index, M.m_Time.begin()+m, M.m_Time.begin()+m_max);
          m_PI.insert(m_PI.begin()+Index, M.m_PI.begin()+m, M.m_PI.begin()+m_max);
          m_Energy.insert(m_Energy.begin()+Index, M.m_Energy.begin()+m, M.m_Energy.begin()+m_max);
          m_SurrEnergy.insert(m_SurrEnergy.begin()+Index, M.m_SurrEnergy.begin()+m, M.m_SurrEnergy.begin()+m_max);
          m_Grade.insert(m_Grade.begin()+Index, M.m_Grade.begin()+m, M.m_Grade.begin()+m_max);
          m_DetectorID.insert(m_DetectorID.begin()+Index, M.m_DetectorID.begin()+m, M.m_DetectorID.begin()+m_max);
          m_RawX.insert(m_RawX.begin()+Index, M.m_RawX.begin()+m, M.m_RawX.begin()+m_max);
          m_RawY.insert(m_RawY.begin()+Index, M.m_RawY.begin()+m, M.m_RawY.begin()+m_max);
          m_Det1X.insert(m_Det1X.begin()+Index, M.m_Det1X.begin()+m, M.m_Det1X.begin()+m_max);
          m_Det1Y.insert(m_Det1Y.begin()+Index, M.m_Det1Y.begin()+m, M.m_Det1Y.begin()+m_max);
          m_X.insert(m_X.begin()+Index, M.m_X.begin()+m, M.m_X.begin()+m_max);
          m_Y.insert(m_Y.begin()+Index, M.m_Y.begin()+m, M.m_Y.begin()+m_max);
        }
        m = m_max;
      }
      // Merge GTI's
      for (unsigned int g = 0; g < M.m_GTIStart.size(); ++g) {
        AddGTI(M.m_GTIStart[g], M.m_GTIStop[g], m_GoodGTI[g]);
      }
    }
  }
  
  double GetMinTime() { return m_Time[0]; }
  double GetMaxTime() { return m_Time.back(); }
  
  int m_CleanLevel;
  
  double m_LiveTime;
  double m_StartTime;
  TString m_ObservationDate;
  TString m_Object;
  double m_DetectorArea;
  
  int m_MinX;
  int m_MaxX;
  double m_ReferencePixelX;
  double m_ReferencePixelValueX;
  double m_ReferencePixelDeltaX;
  int m_MinY;
  int m_MaxY;
  double m_ReferencePixelY;
  double m_ReferencePixelValueY;
  double m_ReferencePixelDeltaY;
  
  vector<int> m_PI;
  vector<double> m_Energy;
  vector<double> m_SurrEnergy;
  vector<int> m_Grade;
  vector<int> m_DetectorID;
  vector<int> m_RawX;
  vector<int> m_RawY;
  vector<int> m_Det1X;
  vector<int> m_Det1Y;
  vector<int> m_X;
  vector<int> m_Y;
  
  vector<double> m_GTIStart;
  vector<double> m_GTIStop;
  vector<double> m_GoodGTI;
};

#endif


////////////////////////////////////////////////////////////////////////////////
