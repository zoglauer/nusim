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
#include "TH1.h"

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
  //! Find the SAA - modes based on the high shield rate
  bool FindSAAsHighThresholdShieldRateBased(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, bool Show = false);
  //! Find the SAA - modes based on the low shield rate
  bool FindSAAsLowThresholdShieldRateBased(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show = false);
  
  //! Find the SAA passage
  bool FindSAATentacle(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show = false);
  //! Find the SAA passage - old figure-of-merit approach 
  bool FindSAATentacleFoM(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, bool Show = false);
  //! Find the SAA passage - new rms-based approach
  bool FindSAATentacleRMS(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show = false);
  
  //! Return the pixels where the source is most likely on
  vector<int> MostlyEliminateSource(NFilteredEvents& F, NOrbits& O, bool Show = false);
  
  //! Return true if the position is in the RMS calculation region 
  bool InRMSCalculationRegion(double Latitude, double Longitude, int Mode) const;

  //! Return true if the position is in the tentacle region 
  bool InTentacleRegion(double Latitude, double Longitude, int Mode) const;
  
  //! Return an interpolated longitude, -1 on error
  double InterpolateLong(double Lat1, double Long1, double Lat2, double Long2, double LatRef) const;
 
  static const int c_SAACutNone                 = 100;
  static const int c_SAACutStrictHSR            = 101;
  static const int c_SAACutOptimizedHSRFoM      = 102;
  static const int c_SAACutStrictLSR            = 103;
  static const int c_SAACutOptimizedLSRRMS_v1   = 104;
  static const int c_SAACutOptimizedLSRRMS_v2   = 105;
  
  static const int c_TentacleCutNone            = 200;
  static const int c_TentacleCutFoM             = 201;
  static const int c_TentacleCutRMS_v1          = 202;
  static const int c_TentacleCutRMS_v2          = 203;
  
  bool IsGoodEventByInternalDetectorEffectsFilter(int ModuleID, double Energy, int Grade, int Veto, 
                                                  double Baseline, double Reset, double Prior);
  bool IsGoodEventByInternalDepthFilter(int ModuleID, int DetectorID, int RawX, int RawY,
                                        double Energy, double SurrEnergy, int Grade);
  
  bool IsGoodEventByExternalDetectorEffectsFilter(double Energy, int Grade, int Veto, int Status);
  bool IsGoodEventByExternalDepthFilter(int Status);

  void ConvertRawPos(int RawX, int RawY, int Detector, double& PosX, double& PosY);
  void ConvertPosRaw(double PosX, double PosY, int& DetectorID, int& RawX, int& RawY);
  
  // protected methods:
 protected:
  void DebugOutput(TH1* Hist, TString FileName); 
  void DebugOutput(vector<bool>& V, TString FileName); 
  
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
  
  bool m_DoLifeTimeCorrection;
  
  double m_SAACutRMSThreshold;
  bool m_SAACutRMSSourceElimination;
  bool m_SAACutRMSSanityChecks;
  
  double m_TentacleCutRMSThreshold;
  bool m_TentacleCutRMSSourceElimination;
  bool m_TentacleCutRMSRegionRestriction;
  bool m_TentacleCutRMSSanityChecks;

  double m_SourceCutRMSThreshold;
  
  
  vector<double> m_RMSRegionTopLeftLat; 
  vector<double> m_RMSRegionTopLeftLong; 

  vector<double> m_RMSRegionBottomLeftLat; 
  vector<double> m_RMSRegionBottomLeftLong; 
  
  vector<double> m_RMSRegionTopRightLat; 
  vector<double> m_RMSRegionTopRightLong; 

  vector<double> m_RMSRegionBottomRightLat; 
  vector<double> m_RMSRegionBottomRightLong; 
  
  
  vector<double> m_TentacleRegionTopLeftLat; 
  vector<double> m_TentacleRegionTopLeftLong; 

  vector<double> m_TentacleRegionBottomLeftLat; 
  vector<double> m_TentacleRegionBottomLeftLong; 
  
  vector<double> m_TentacleRegionTopRightLat; 
  vector<double> m_TentacleRegionTopRightLong; 

  vector<double> m_TentacleRegionBottomRightLat; 
  vector<double> m_TentacleRegionBottomRightLong; 

  
  bool m_Debug;
  
  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NBaseTool, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
