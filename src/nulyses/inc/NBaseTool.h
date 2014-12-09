/*
 * NBaseTool.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NBaseTool__
#define __NBaseTool__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NAttitude.h"
#include "NOrbits.h"
#include "NEngineering.h"
#include "NFilteredEvents.h"
#include "NUnfilteredEvents.h"
#include "NHousekeeping.h"
#include "NBadPixel.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NBaseTool
{
  // public interface:
 public:
  //! Standard constructor
  NBaseTool();
  //! Default destructor
  virtual ~NBaseTool();
  
  /// Parse the module specific parts of the command line
  virtual bool ParseCommandLine(int argc, char** argv);
  /// Analyze what ever needs to be analyzed...
  virtual bool Analyze() { return false; }

  virtual bool IsBatchMode() { return m_BatchMode; }
  
  // protected methods:
 protected:
  //NBaseTool() {};
  //NBaseTool(const NBaseTool& NCTHit) {};

  //! Read a GTI file
  bool ReadGTI(const TString& GTIFileName);
  bool WithinSpecialGTI(double Time) {
    if (m_SpecialGTIStart.size() == 0) return true;
    for (unsigned int i = 0; i < m_SpecialGTIStart.size(); ++i) {
      if (Time >= m_SpecialGTIStart[i] && Time <= m_SpecialGTIStop[i]) return true;
    }
    return false;
  }
  bool WithinSpecialBTI(double Time) {
    if (m_SpecialBTIStart.size() == 0) return false;
    for (unsigned int i = 0; i < m_SpecialBTIStart.size(); ++i) {
      if (Time >= m_SpecialBTIStart[i] && Time <= m_SpecialBTIStop[i]) return true;
    }
    return false;
  }
  
  //! Load all the data
  virtual bool Load(TString Directory, const TString& LookAt = "ab");

  //! Find the SAA cut
  bool FindSAAs(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show = false);
  //! Find the SAA passage
  bool FindSAATentacle(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, bool Show = false);
  
  static const int c_SAACutNone                  = 0;
  static const int c_SAACutStrict                = 1;
  static const int c_SAACutOptimized             = 2;
  static const int c_SAACutOptimizedByLifeTime   = 3;
  
  bool IsGoodEventByInternalDetectorEffectsFilter(int ModuleID, double Energy, int Grade, int Veto, 
                                                  double Baseline, double Reset, double Prior);
  bool IsGoodEventByInternalDepthFilter(int ModuleID, int DetectorID, int RawX, int RawY,
                                        double Energy, double SurrEnergy, int Grade);
  
  bool IsGoodEventByExternalDetectorEffectsFilter(double Energy, int Grade, int Veto, int Status);
  bool IsGoodEventByExternalDepthFilter(int Status);

  void ConvertRawPos(int RawX, int RawY, int Detector, double& PosX, double& PosY);
  
  // private methods:
 private:



  // protected members:
 protected:
  /// The master directories
  vector<TString> m_Directories;

  vector<int> m_DetPosXA;
  vector<int> m_DetPosYA;
  vector<double> m_DetSizeA;

  vector<int> m_DetPosXB;
  vector<int> m_DetPosYB;
  vector<double> m_DetSizeB;

  NOrbits m_Orbits;
  NAttitude m_Attitude;
  NEngineering m_Engineering;
  
  NHousekeeping m_HousekeepingA;
  NUnfilteredEvents m_UnfilteredEventsA;
  NFilteredEvents m_FilteredEventsA01;
  NFilteredEvents m_FilteredEventsA02;
  NFilteredEvents m_FilteredEventsA;
  NBadPixel m_BadPixelA;
  
  NHousekeeping m_HousekeepingB;
  NUnfilteredEvents m_UnfilteredEventsB;
  NFilteredEvents m_FilteredEventsB01;
  NFilteredEvents m_FilteredEventsB02;
  NFilteredEvents m_FilteredEventsB;
  NBadPixel m_BadPixelB;
  
  bool m_MergeFilteredEvents;
  
  double m_ModuleDetectorArea;
  
  vector<double> m_SpecialGTIStart;
  vector<double> m_SpecialGTIStop;
  
  vector<double> m_SpecialBTIStart;
  vector<double> m_SpecialBTIStop;
  
  TString m_LookAtModule;
  TString m_ShowHistograms;

  double m_SpectrumMin;
  double m_SpectrumMax;
  int m_SpectrumBins;
  
  bool m_ReadUnfiltered;
  bool m_ReadFiltered01;
  bool m_ReadFiltered02;
  bool m_ReadEngineering;
  
  TString m_FileType;
  
  bool m_ApplySAAFilters;
  
  bool m_BatchMode;
  
  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NBaseTool, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
