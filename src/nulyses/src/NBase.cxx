/*
* NBase.cxx
*
* Copyright (C) by the NuSTAR team.
* All rights reserved.
*
*/


////////////////////////////////////////////////////////////////////////////////
//
// NBase
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBase.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NBase)
#endif


////////////////////////////////////////////////////////////////////////////////


NBase::NBase()
{
  // Construct an instance of NBase
  
  Clean();
}


////////////////////////////////////////////////////////////////////////////////


NBase::~NBase()
{
  // Delete this instance of NBase
}


////////////////////////////////////////////////////////////////////////////////


void NBase::Clean()
{
  m_LastClostestIndex = -1;
  m_LastFirstLargerIndex = -1;
  m_LastFoundIndex = -1;
  
  m_Time.clear();
  m_Module = 0;
  m_ID = "";
}


////////////////////////////////////////////////////////////////////////////////


int NBase::FindFirstIndexLarger(double Time, bool Silent) {
  
  unsigned int Start = 0;
  if (m_LastFirstLargerIndex > 0 && m_Time[m_LastFirstLargerIndex-1] < Time) {
    Start = m_LastFirstLargerIndex;
  }
  for (unsigned int i = Start; i < m_Time.size(); ++i) {
    if (m_Time[i] > Time) {
      m_LastFirstLargerIndex = i;
      return i;
    }
  }
  
  if (Silent == false) {
    cout<<"FindFirstIndexLarger: Time not found: "<<Time<<endl;
  }
  m_LastFirstLargerIndex = -1;
  return -1;
}  


////////////////////////////////////////////////////////////////////////////////


int NBase::FindFirstIndexBefore(double Time) {
  for (unsigned int i = 0; i < m_Time.size(); ++i) {
    if (m_Time[i] > Time) {
      if (i == 0) return -1;
      return i-1;
    }
  }
  
  cout<<"FindFirstIndexBefore: Time not found: "<<Time<<endl;
  return -1;
}


////////////////////////////////////////////////////////////////////////////////


int NBase::FindIndex(double Time) {

    unsigned int Start = 0;
    if (m_LastFoundIndex > 0 && m_Time[m_LastFoundIndex-1] < Time) {
      Start = m_LastFoundIndex;
    }
    
    for (unsigned int i = Start; i < m_Time.size(); ++i) {
      if (m_Time[i] == Time) {
        m_LastFoundIndex = i;
        return i;
      }
    }
    cout<<"FindIndex: Time not found: "<<Time<<endl;
    m_LastFoundIndex = -1;
    return -1;
}


////////////////////////////////////////////////////////////////////////////////


int NBase::FindClosestIndex(double Time) {
  
  unsigned int Size = m_Time.size();
  
  if (Size == 0) {
    cout<<"Find closest: The size of this data set is zero!"<<endl;
    return -1;
  }
  if (Time < m_Time[0]) return 0;
  if (Time > m_Time.back()) return Size - 1; 
  
  if (m_LastClostestIndex > 0 && Time > m_Time[m_LastClostestIndex-1] && Time - m_Time[m_LastClostestIndex-1] < 20) {
    int ClosestSlow = -1;
    for (unsigned int i = m_LastClostestIndex-1; i < Size; ++i) {
      if (m_Time[i] > Time) {
        ClosestSlow = i;
        break;
      }
    }
    if (m_Time[ClosestSlow] - Time < Time - m_Time[ClosestSlow-1]) {
      m_LastClostestIndex = ClosestSlow;
      return ClosestSlow; 
    } else {
      m_LastClostestIndex = ClosestSlow - 1;      
      return ClosestSlow - 1; 
    }
  }
  
  //int ClosestSlow = -1;
  int ClosestFast = -1;
  
  /*
  *    int FirstAbove = -1;
  *    for (unsigned int i = 1; i < Size; ++i) {
  *      if (m_Time[i] > Time) {
  *        ClosestSlow = i;
  *        break;
  }
  }
  */
  
  // Binary search:
  unsigned int upper = Size;
  unsigned int center = 1;
  unsigned int lower = 0;
  
  while (upper-lower > 1) {
    center = (upper+lower) >> 1;
    if (Time == m_Time[center]) {
      ClosestFast = int(center+1);
      break;
    }
    if (Time < m_Time[center]) {
      upper = center;
    } else {
      lower = center;
    }
  }
  if (ClosestFast == -1) {
    ClosestFast = int(lower+1);
  }
  
  /*
  *    if (ClosestFast != ClosestSlow) {
  *      cerr<<"Closest fast "<<ClosestFast<<" vs. Closest slow "<<ClosestSlow<<endl;
  *      cout<<Time<<" vs fast: "<<m_Time[ClosestFast-1]<<" - "<<m_Time[ClosestFast]<<"  slow: "<<m_Time[ClosestSlow-1]<<" - "<<m_Time[ClosestSlow]<<endl;
  }
  */
  
  if (m_Time[ClosestFast] - Time < Time - m_Time[ClosestFast-1]) {
    m_LastClostestIndex = ClosestFast;
    return ClosestFast; 
  } else {
    m_LastClostestIndex = ClosestFast - 1;      
    return ClosestFast - 1; 
  }
}  
  
  
// NBase.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
  