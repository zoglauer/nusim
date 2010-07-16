/*
 * MSystem.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MSystem__
#define __MSystem__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TROOT.h>
#include <TString.h>
#include <TTime.h>

// MEGAlib libs:
#include "MGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MSystem : public TObject
{
  // Public Interface:
 public:
  MSystem();
  ~MSystem();

  bool FreeMemory(int &Free);
  int GetRAM();
  int GetFreeRAM();
  int GetSwap();
  int GetFreeSwap();

  static bool GetTime(long int& Seconds, long int& NanoSeconds);
  static void BusyWait(int musec);

  int GetProcessMemory();

  bool FileExist(const TString Filename);
  bool GetFileDirectory(TString Filename, TString* Directory);
  bool GetFileSuffix(TString Filename, TString* Suffix);
  bool GetFileWithoutSuffix(TString Filename, TString* NewFilename);
  

  // protected methods:
 protected:
  bool GetMemory();
  bool GetProcessInfo(int ProcessID);
  void Reset();


  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  int m_RAM;        // Installed RAM
  int m_FreeRAM;    // Free RAM
  int m_Swap;       // Installed Swap-space
  int m_FreeSwap;   // Free Swap-Space

  TTime m_LastCheck;      // Time of last check
  TTime m_CheckInterval;  // Minimum time gap between two checks

  int m_ProcessMemory;


#ifdef ___CINT___
 public:
  ClassDef(MSystem, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
