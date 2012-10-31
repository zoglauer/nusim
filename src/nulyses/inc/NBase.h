/*
 * NBase.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBase__
#define __NBase__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
#include <iostream>
#include <map>
using namespace std;

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "MFile.h"

// FITS libs:
#include "fitsio.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBase
{
  public:
  NBase();
  virtual ~NBase();
  
  //! Clean - empty the arrays
  virtual void Clean();
  
  int FindFirstIndexLarger(double Time, bool Silent = false);
  
  int FindFirstIndexBefore(double Time);
 
  int FindIndex(double Time);
   
  int FindClosestIndex(double Time);
  
  
public:
  vector<double> m_Time;
  int m_Module;
  TString m_ID;
  
  private:
    int m_LastClostestIndex;
    int m_LastFirstLargerIndex;
    int m_LastFoundIndex;
};

#endif


////////////////////////////////////////////////////////////////////////////////
