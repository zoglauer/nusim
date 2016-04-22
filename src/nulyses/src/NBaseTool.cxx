/*
 * NBaseTool.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NBaseTool
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBaseTool.h"

// Standard libs:
#include <iomanip>
#include <limits>
#include <algorithm>
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TSystem.h"

// MEGAlib libs:
#include "MBinnerBayesianBlocks.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NBaseTool)
#endif


////////////////////////////////////////////////////////////////////////////////


NBaseTool::NBaseTool()
{
  // Construct an instance of NBaseTool
  
  m_MergeFilteredEvents = true;
  
  m_SpectrumMin = 0.0;
  m_SpectrumMax = 100.0;
  m_SpectrumBins = 200;
    
  m_ReadUnfiltered = true;
  m_ReadFiltered01 = true;
  m_ReadFiltered02 = true;
  m_ReadEngineering = true;
  
  m_ShowHistograms = "s";
  m_FileType = ".pdf";
  
  m_ApplySAAFilters = true;
  
  m_BatchMode = false;
  
  m_DoLifeTimeCorrection = true;
  
  m_SAACutRMSThreshold = 4.5;
  m_SAACutRMSSourceElimination = true;
  m_SAACutRMSSanityChecks = true;
  
  m_TentacleCutRMSThreshold = 4.5;
  m_TentacleCutRMSSourceElimination = true;
  m_TentacleCutRMSRegionRestriction = true;
  m_TentacleCutRMSSanityChecks = true;
  
  m_SourceCutRMSThreshold = 4.0;
  
  m_Debug = false;
}


////////////////////////////////////////////////////////////////////////////////


NBaseTool::~NBaseTool()
{
  // Delete this instance of NBaseTool
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::ParseCommandLine(int argc, char** argv)
{
  string Option;
  
  
  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    
    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "-f" || Option == "-m" || Option == "--gti" || Option == "-d" || Option == "-s") {
      if (!((argc > i+1) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
        return false;
      }
    } 
    // Multiple arguments template
    else if (Option == "-g" || Option == "-b") {
      if (!((argc > i+2) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) && 
        (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs two arguments!"<<endl;
        return false;
      }
    }
    else if (Option == "-dp") {
      if (!((argc > i+6) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) && 
        (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0) && 
        (argv[i+3][0] != '-' || isalpha(argv[i+3][1]) == 0) && 
        (argv[i+4][0] != '-' || isalpha(argv[i+4][1]) == 0) && 
        (argv[i+5][0] != '-' || isalpha(argv[i+5][1]) == 0) && 
        (argv[i+6][0] != '-' || isalpha(argv[i+6][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs six arguments!"<<endl;
        return false;
      }
    }
    
    // Then fulfill the options:
    if (Option == "-d" || Option == "--d") {
      m_Directories.push_back(argv[++i]);
      m_DetPosXA.push_back(0);
      m_DetPosYA.push_back(0);
      m_DetSizeA.push_back(0);
      m_DetPosXB.push_back(0);
      m_DetPosYB.push_back(0);
      m_DetSizeB.push_back(0);
      cout<<"Accepting directory file name: "<<m_Directories.back()<<endl;
    } else if (Option == "-dp" || Option == "--dp") {
      m_Directories.push_back(argv[++i]);
      m_DetPosXA.push_back(atoi(argv[++i]));
      m_DetPosYA.push_back(atoi(argv[++i]));
      m_DetSizeA.push_back(atoi(argv[++i]));
      m_DetPosXB.push_back(atoi(argv[++i]));
      m_DetPosYB.push_back(atoi(argv[++i]));
      m_DetSizeB.push_back(atoi(argv[++i]));
      cout<<"Accepting directory file name: "<<m_Directories.back()
          <<" - (A: x="<<m_DetPosXA.back()<<", y="<<m_DetPosYA.back()<<", s="<<m_DetSizeA.back()
          <<" -- B: x="<<m_DetPosXB.back()<<", y="<<m_DetPosYB.back()<<", s="<<m_DetSizeB.back()<<")"<<endl;
    } else if (Option == "-m") {
      m_LookAtModule = argv[++i];
      cout<<"Accepting to look at the following module(s): "<<m_LookAtModule<<endl;
    } else if (Option == "-s") {
      m_ShowHistograms = argv[++i];
      cout<<"Accepting the following show options: "<<m_ShowHistograms<<endl;
    } else if (Option == "-g") {
      m_SpecialGTIStart.push_back(atof(argv[++i]));
      m_SpecialGTIStop.push_back(atof(argv[++i]));
      if (m_SpecialGTIStart.back() >= m_SpecialGTIStop.back()) {
        cout<<"Bad GTI: Start time is larger than stop time!"<<endl;
        return false;
      }
      cout<<"Accepting special good time interval: "<<m_SpecialGTIStart.back()<<" to "<<m_SpecialGTIStop.back()<<endl;
    } else if (Option == "-e") {
      m_SpectrumMin = atof(argv[++i]);
      m_SpectrumMax = atof(argv[++i]);
      cout<<"Accepting spectral range: "<<m_SpectrumMin<<" to "<<m_SpectrumMax<<endl;
    } else if (Option == "-b") {
      m_SpecialBTIStart.push_back(atof(argv[++i]));
      m_SpecialBTIStop.push_back(atof(argv[++i]));
      if (m_SpecialBTIStart.back() >= m_SpecialBTIStop.back()) {
        cout<<"Bad BTI: Start time is larger than stop time!"<<endl;
        return false;
      }
      cout<<"Accepting special bad time window: "<<m_SpecialBTIStart.back()<<" to "<<m_SpecialBTIStop.back()<<endl;
    } else if (Option == "--gti") {
      if (ReadGTI(argv[++i]) == false) return false;
      cout<<"Accepting GTI!"<<endl;
    } else if (Option == "--saa-rms-threshold") {
      m_SAACutRMSThreshold = atof(argv[++i]);
      cout<<"Accepting RMS threshold for the SAA cut \"Optimized by RMS\": "<<m_SAACutRMSThreshold<<endl;
    } else if (Option == "--saa-rms-source-elimination") {
      m_SAACutRMSSourceElimination = bool(atoi(argv[++i]));
      cout<<"Accepting source elimination for the SAA cut \"Optimized by RMS\": "<<(m_SAACutRMSSourceElimination == true ? "on" : "off")<<endl;
    } else if (Option == "--saa-rms-sanity-checks") {
      m_SAACutRMSSanityChecks = bool(atoi(argv[++i]));
      cout<<"Accepting sanity checks for the SAA cut \"Optimized by RMS\": "<<(m_SAACutRMSSanityChecks == true ? "on" : "off")<<endl;
    } else if (Option == "--tentacle-rms-threshold") {
      m_TentacleCutRMSThreshold = atof(argv[++i]);
      cout<<"Accepting RMS threshold for the Tentacle cut \"by RMS\": "<<m_TentacleCutRMSThreshold<<endl;
    } else if (Option == "--tentacle-rms-source-elimination") {
      m_TentacleCutRMSSourceElimination = bool(atoi(argv[++i]));
      cout<<"Accepting source elimination for the Tentacle cut \"by RMS\": "<<(m_TentacleCutRMSSourceElimination == true ? "on" : "off")<<endl;
    } else if (Option == "--tentacle-rms-sanity-checks") {
      m_TentacleCutRMSSanityChecks = bool(atoi(argv[++i]));
      cout<<"Accepting sanity checks for the Tentacle cut \"by RMS\": "<<(m_TentacleCutRMSSanityChecks == true ? "on" : "off")<<endl;
    } else if (Option == "--tentacle-rms-region-restriction") {
      m_TentacleCutRMSRegionRestriction = bool(atoi(argv[++i]));
      cout<<"Accepting region restriction for the Tentacle cut \"by RMS\": "<<(m_TentacleCutRMSRegionRestriction == true ? "on" : "off")<<endl;
    } else if (Option == "--sourcecut-rms-threshold") {
      m_SourceCutRMSThreshold = atof(argv[++i]);
      cout<<"Accepting RMS threshold for the source cut \"by RMS\": "<<m_SourceCutRMSThreshold<<endl;
    } else if (Option == "--debug") {
      m_Debug = true;
      cout<<"Activating debug mode"<<endl;
    }
  }
  
  if (m_LookAtModule == "") {
    cout<<"Analyzing both modules!"<<endl;
    m_LookAtModule = "ab";
  }
  
  if (m_TentacleCutRMSSourceElimination != m_SAACutRMSSourceElimination) {
    cout<<"You have to do either the source elimination on both or in none, tentacle and SAA cut by RMS"<<endl;
    return false;
  }
  
  if (m_ShowHistograms.Contains("s") == false) {
    gROOT->SetBatch(true); 
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::ReadGTI(const TString& GTIFileName)
{
  cout<<"Reading GTI "<<GTIFileName<<endl;
 
  // Section 1:
  // initialize the files
  int status = 0;
  char value[1000];
  char templt[100];

  int anynul;
  
  double nulldbl = 0.0;
  double valdbl = 0;
  
  fitsfile* EventFile = 0;
  if (fits_open_file(&EventFile, GTIFileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open GTI file \""<<GTIFileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  //cout<<"Opened "<<AttitudeFileName<<endl;

  // Move to the second hdu
  fits_movabs_hdu(EventFile, 2, NULL, &status);

  // Get the number of columns
  int NColumns = 0;
  map<TString, int> Columns;
  fits_get_num_cols(EventFile, &NColumns, &status);
  for (int c = 1; c <= NColumns; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(EventFile, CASEINSEN, templt, value, &col, &status);
    Columns[value] = c;
    //cout<<value<<endl;
  }
  //cout<<"N columns: "<<NColumns<<endl;

  // Get the number of rows:
  long NRows = 0;
  fits_get_num_rows(EventFile, &NRows, &status);
  //cout<<"Rows: "<<NRows<<endl;

  // Loop over the events and all columns and generate MComptonEvent's
  for (long r = 1; r <= NRows; ++r) {

    if (fits_read_col_dbl(EventFile, Columns["START"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (START) failed!"<<endl;
      break;
    }
    double Start = valdbl;
        
    if (fits_read_col_dbl(EventFile, Columns["STOP"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (STOP) failed!"<<endl;
      break;
    }
    double Stop = valdbl;
    
    m_SpecialGTIStart.push_back(Start);
    m_SpecialGTIStop.push_back(Stop);
    //cout<<"Added GTI: "<<Start<<":"<<Stop<<endl;
  }
  
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::Load(TString Directory, const TString& LookAtModule)
{
  if (Directory.EndsWith("/")) {
    Directory = Directory.Remove(Directory.Last('/'), 1); 
  }    
  
  // Check if we are in the main dir or the event directory
  TString EventDir = "event_cl";
  if (gSystem->AccessPathName(Directory + "/hk") == true) { // == it is not accessible
    // Remove last directory
    EventDir = Directory;
    EventDir.Remove(0, EventDir.Last('/')+1);  
    Directory = Directory.Remove(Directory.Last('/'), Directory.Length() - Directory.Last('/'));  
  }
  
  TString Tag = Directory;
  if (Tag.Last('/') != kNPOS) {
    Tag = Tag.Remove(0, Tag.Last('/')+1);      
  }
  
  cout<<Directory<<":"<<EventDir<<":"<<Tag<<endl;
  
  TString OrbitsFileName = Directory + "/auxil/nu" + Tag + "_orb.fits";
  TString AttitudeFileName = Directory + "/auxil/nu" + Tag + "_att.fits";
  TString EngineeringFileName = Directory + "/hk/nu" + Tag + "_eng.hk";
  TString HKFileNameA = Directory + "/hk/nu" + Tag + "A_fpm.hk";
  TString HKFileNameB = Directory + "/hk/nu" + Tag + "B_fpm.hk";
  TString BPFileNameA = Directory + "/" + EventDir + "/nu" + Tag + "A_bp.fits";
  TString BPFileNameB = Directory + "/" + EventDir + "/nu" + Tag + "B_bp.fits";
  TString UFFileNameA = Directory + "/" + EventDir + "/nu" + Tag + "A_uf.evt";
  TString UFFileNameB = Directory + "/" + EventDir + "/nu" + Tag + "B_uf.evt";
  TString FIFileNameA01 = Directory + "/" + EventDir + "/nu" + Tag + "A01_cl.evt";
  TString FIFileNameB01 = Directory + "/" + EventDir + "/nu" + Tag + "B01_cl.evt";
  TString FIFileNameA02 = Directory + "/" + EventDir + "/nu" + Tag + "A02_cl.evt";
  TString FIFileNameB02 = Directory + "/" + EventDir + "/nu" + Tag + "B02_cl.evt";
  
  m_Orbits.Clean();
  if (m_Orbits.Read(OrbitsFileName) == false) return false;
     
  m_Attitude.Clean();
  if (m_Attitude.Read(AttitudeFileName) == false) return false;
                     
  m_Engineering.Clean();
  if (m_Engineering.Read(EngineeringFileName) == false || m_Engineering.m_Time.size() == 0) {
    cout<<"WARNING **** NO ENGINEERING DATA **** WARNING"<<endl;
    for (unsigned int i = 0; i < m_Orbits.m_Time.size(); ++i) {
      m_Orbits.m_SafelyOnSource[i] = (m_Orbits.m_OccultedFlag[i] == 0) ? true : false; 
      m_Orbits.m_SafelyOcculted[i] = (m_Orbits.m_OccultedFlag[i] == 1) ? true : false;  
    }
  } else {
    bool FoundSafelyOcculted = false;
    for (unsigned int i = 0; i < m_Orbits.m_Time.size(); ++i) {
      int EngIndex = m_Engineering.FindClosestIndex(m_Orbits.m_Time[i]);
      if (EngIndex == -1) {
        cout<<"Engineering: Index not found..."<<endl;
        continue;      
      }
      if (m_Engineering.m_LimbAngle[EngIndex] > 6) {
        m_Orbits.m_SafelyOnSource[i] = true; 
      } else {
        m_Orbits.m_SafelyOnSource[i] = false;         
      }
      if (m_Engineering.m_LimbAngle[EngIndex] < -2) {
        m_Orbits.m_SafelyOcculted[i] = true; 
        FoundSafelyOcculted = true;
      } else {
        m_Orbits.m_SafelyOcculted[i] = false;         
      }
    }
    if (FoundSafelyOcculted == false) {
      cout<<"Info: Due to the orbit parameters we do not have safely occulted data!"<<endl; 
    }
  }
  
  m_HousekeepingA.Clean();
  if (LookAtModule.Contains("a")) {
    if (m_HousekeepingA.Read(HKFileNameA) == false) return false;
    // Add hard SAA
    for (unsigned int h = 0; h < m_HousekeepingA.m_Time.size(); ++h) {
      int OrbitIndex = m_Orbits.FindClosestIndex(m_HousekeepingA.m_Time[h]);
      if (OrbitIndex == -1) {
        cout<<"Orbit: Index not found for time "<<m_HousekeepingA.m_Time[h]<<"..."<<endl;
        continue;      
      }
      m_HousekeepingA.m_HardSAA[h] = m_Orbits.m_SAAFlag[OrbitIndex] == 1 ? true : false;
    }
  }
  m_HousekeepingA.m_Module = 0;
  m_HousekeepingA.m_ID = Tag;
  
  m_BadPixelA.Clean();
  if (LookAtModule.Contains("a")) if (m_BadPixelA.Read(BPFileNameA) == false) return false;
  m_BadPixelA.m_Module = 0;
  m_BadPixelA.m_ID = Tag;
  
  m_UnfilteredEventsA.Clean();
  if (m_ReadUnfiltered == true && LookAtModule.Contains("a")) if (m_UnfilteredEventsA.Read(UFFileNameA) == false) return false;
  m_UnfilteredEventsA.m_Module = 0;
  m_UnfilteredEventsA.m_ID = Tag;
  
  m_FilteredEventsA01.Clean();
  if (m_ReadFiltered01 == true && LookAtModule.Contains("a")) if (m_FilteredEventsA01.Read(FIFileNameA01) == false) return false;
  m_FilteredEventsA01.m_Module = 0;
  m_FilteredEventsA01.m_ID = Tag;
  
  m_FilteredEventsA02.Clean();
  if (m_ReadFiltered02 == true && LookAtModule.Contains("a")) if (m_FilteredEventsA02.Read(FIFileNameA02) == false) return false;
  m_FilteredEventsA02.m_Module = 0;
  m_FilteredEventsA02.m_ID = Tag;
  
  m_HousekeepingB.Clean();
  if (LookAtModule.Contains("b")) {
    if (m_HousekeepingB.Read(HKFileNameB) == false) return false;
    // Add hard SAA
    for (unsigned int h = 0; h < m_HousekeepingB.m_Time.size(); ++h) {
      int OrbitIndex = m_Orbits.FindClosestIndex(m_HousekeepingB.m_Time[h]);
      if (OrbitIndex == -1) {
        cout<<"Orbit: Index not found for time "<<m_HousekeepingB.m_Time[h]<<"..."<<endl;
        continue;      
      }
      m_HousekeepingB.m_HardSAA[h] = m_Orbits.m_SAAFlag[OrbitIndex] == 1 ? true : false;
    }    
  }
  m_HousekeepingB.m_Module = 1;
  m_HousekeepingB.m_ID = Tag;
  
  m_BadPixelB.Clean();
  if (LookAtModule.Contains("b")) if (m_BadPixelB.Read(BPFileNameB) == false) return false;
  m_BadPixelB.m_Module = 1;
  m_BadPixelB.m_ID = Tag;
  
  m_UnfilteredEventsB.Clean();
  if (m_ReadUnfiltered == true && LookAtModule.Contains("b")) if (m_UnfilteredEventsB.Read(UFFileNameB) == false) return false;
  m_UnfilteredEventsB.m_Module = 1;
  m_UnfilteredEventsB.m_ID = Tag;
  
  m_FilteredEventsB01.Clean();
  if (m_ReadFiltered01 == true && LookAtModule.Contains("b")) if (m_FilteredEventsB01.Read(FIFileNameB01) == false) return false;
  m_FilteredEventsB01.m_Module = 1;
  m_FilteredEventsB01.m_ID = Tag;
  
  m_FilteredEventsB02.Clean();
  if (m_ReadFiltered02 == true && LookAtModule.Contains("b")) if (m_FilteredEventsB02.Read(FIFileNameB01) == false) return false;
  m_FilteredEventsB02.m_Module = 1;
  m_FilteredEventsB02.m_ID = Tag;
  
  m_FilteredEventsA.Clean();
  m_FilteredEventsB.Clean();
  if (m_MergeFilteredEvents == true) {
    if (LookAtModule.Contains("a")) {
      m_FilteredEventsA.Merge(m_FilteredEventsA01);
      m_FilteredEventsA.Merge(m_FilteredEventsA02);
      m_FilteredEventsA.m_CleanLevel = 12;
    }
    
    if (LookAtModule.Contains("b")) {
      m_FilteredEventsB.Merge(m_FilteredEventsB01);
      m_FilteredEventsB.Merge(m_FilteredEventsB02);
      m_FilteredEventsB.m_CleanLevel = 12;
    }
  }
  
  if (m_ApplySAAFilters == true) {
    bool Show = false;
    if (m_ShowHistograms.Contains("s")) Show = true;
    
    if (m_MergeFilteredEvents == false) {
      if (LookAtModule.Contains("a")) {
        if (FindSAAs(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, c_SAACutNone, Show) == false) return false;
      }
      if (LookAtModule.Contains("b")) {
        if (FindSAAs(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, c_SAACutNone, Show) == false) return false;
      }
    } else {
      if (LookAtModule.Contains("a")) {
        if (FindSAAs(m_FilteredEventsA, m_HousekeepingA, m_Orbits, c_SAACutNone, Show) == false) return false;
      }
      if (LookAtModule.Contains("b")) {
        if (FindSAAs(m_FilteredEventsB, m_HousekeepingB, m_Orbits, c_SAACutNone, Show) == false) return false;
      }
    }
  
    if (LookAtModule.Contains("a")) {
      if (FindSAATentacle(m_FilteredEventsA, m_HousekeepingA, m_Orbits, c_TentacleCutNone, Show) == false) return false;
    }
    if (LookAtModule.Contains("b")) {
      if (FindSAATentacle(m_FilteredEventsB, m_HousekeepingB, m_Orbits, c_TentacleCutNone, Show) == false) return false;
    }
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::IsGoodEventByInternalDetectorEffectsFilter(int ModuleID, double Energy, int Grade, int Veto, 
                                                         double Baseline, double Reset, double Prior)
{    
  bool Deselected = false;

  if (Energy < 2.5 || Energy > 100) {
    Deselected = true;
    //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Out of energy range (3<"<<Energy<<"<100)"<<endl;
  }
  if (Veto == 1) {
    Deselected = true;
    //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Shield veto on!"<<endl;        
  }
  if (Grade > 26) {
    Deselected = true;
    //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Grade out above 26: "<<Grade<<endl;        
  }
      
  if (ModuleID == 0) {
    if (Baseline < 1900 && Energy < 25) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Within baseline rejection window: "<<BaseLine<<" < 1900 && "<<Energy<<"< 25 keV"<<endl;                  
    }  
    if (Baseline > 13500) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Baseline too large: "<<BaseLine<<" > 13500"<<endl;                  
    }
    if (Prior < 3E-5) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Prior too small: "<<Prior<<" < 3E-5"<<endl;                            
    }
    if (Prior > 0.00086 && Prior < 0.00091 && Energy < 26.5) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Within prior-energy=window: (0.00086<"<<Prior<<" < 0.00091 && "<<Energy<<" < 26.5 keV)"<<endl;                  
    }
    if (Reset < 0.0000273) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Reset too small: "<<Reset<<" < 0.0000273"<<endl;
    }
    if (Reset > 0.000196 && Reset < 0.000215 && Energy > 23 && Energy < 26.5) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Within reset-energy-window (0.000196 < "<<Reset<<" < 0.000215 && 23 keV < "<<Energy<<" < 26.5 keV)"<<endl;                  
    }
  } else if (ModuleID == 1) {
    if (Baseline < 2100 && Energy < 25) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Within baseline rejection window: "<<BaseLine<<" < 2100 && "<<Energy<<"< 25 keV"<<endl;                  
    }  
    if (Baseline > 13500) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Baseline too large: "<<BaseLine<<" > 13500"<<endl;                  
    }
    if (Prior < 1E-4) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Prior too small: "<<Prior<<" < 1E-5"<<endl;                            
    }
    if (Reset < 0.0000273) {
      Deselected = true;
      //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Reset too small: "<<Reset<<" < 0.0000273"<<endl;
    }
  }
        
  return !Deselected;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::IsGoodEventByInternalDepthFilter(int ModuleID, int DetectorID, int RawX, int RawY,
                                               double Energy, double SurrEnergy, int Grade)
{    
/*
  // Depth cut:
  if (m_DepthCutDBs.size() > 0) {
    bool Found = false;
    for (unsigned int d = 0; d < m_DepthCutDBs.size(); ++d) {
      if (m_DepthCutDBs[d].DoCut(ModuleID, DetectorID, Grade, RawX, RawY, Energy, SurrEnergy) == true) {
        Found = true;
        break;
      }
    }
    if (Found == true) {
      return false;
    }
  }
  */
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::IsGoodEventByExternalDetectorEffectsFilter(double Energy, int Grade, int Veto, int Status)
{    
  bool Deselected = false;

  if (Energy < 2.5 || Energy > 150) {
    Deselected = true;
    //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Out of energy range (3<"<<Energy<<"<100)"<<endl;
  }
  
  
  if (Veto == 1) {
    Deselected = true;
    //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Shield veto on!"<<endl;        
  }
  if (Grade > 26) {
    Deselected = true;
    //cout<<"Reject event at t="<<setprecision(12)<<Time<<" (id="<<i+1<<"): Grade out above 26: "<<Grade<<endl;        
  }
    
  // Baseline cut:
  if (((Status & (1 << 8)) >> 8) == 1) {
    Deselected = true;
  }
  
  // Prior-reset cut:
  if (((Status & (1 << 9)) >> 9) == 1) {
    Deselected = true;
  }
  
  // Prior cut:
  if (((Status & (1 << 10)) >> 10) == 1) {
    Deselected = true;
  }
  
  // Reset cut:
  if (((Status & (1 << 11)) >> 11) == 1) {
    Deselected = true;
  }
    
  return !Deselected;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::IsGoodEventByExternalDepthFilter(int Status)
{    
  // Depth cut:
  if (((Status & (1 << 7)) >> 7) == 1) {
    //cout<<"External depth filter"<<endl;
    return false;
  }  
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::FindSAAs(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show)
{
  FindSAAsHighThresholdShieldRateBased(F, H, O, Mode, Show);
  FindSAAsLowThresholdShieldRateBased(F, H, O, Mode, Show);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::FindSAATentacle(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show)
{
  FindSAATentacleFoM(F, H, O, Show);
  FindSAATentacleRMS(F, H, O, Show);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::FindSAAsHighThresholdShieldRateBased(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show)
{
  cout<<endl;
  cout<<"Finding SAA - high shield threshold based approaches..."<<endl; 

  double SAARegionMin = 270;
  double SAARegionMax = 360;

  if (F.m_Time.size() == 0) {
    cout<<"No filtered events = no SAA cut"<<endl;  
    return true;
  }
  
  // Start with no cuts (clean in case we run it):
  for (unsigned int i = 0; i < H.m_Time.size(); ++i) {
    H.m_SoftSAA[i] = false;
  }
  
  // The rejection flags
  int RejectionOn  = 0;
  int RejectionOff = 1;
  
  // Default one second binning
  int OneSecBins = H.GetMaxTime() - H.GetMinTime() + 1;
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;
  
  double LowShieldRateBinning = 60; // sec
  
  TString iID = TString("_ID_") + H.m_ID + "_M_" + ((H.m_Module == 0) ? "A" : "B");
  TString ID = TString(" (ID: ") + H.m_ID + ((H.m_Module == 0) ? "A" : "B") + ")";

  
  // Step 1: Create a safe "strict" cut which eliminates all regions (close to the SAA) 
  //         where the the high-shield rate is above its normal (= during this obs.) values 
  
  TH1D* LifeTimes = new TH1D(TString("LifeTimes") + iID, TString("LifeTimes") + ID, OneSecBins, MinTime, MaxTime);
  TH1D* ShieldRateLow = new TH1D(TString("SAAShieldRateLow") + iID, TString("SAAShieldRateLow") + ID, OneSecBins/LowShieldRateBinning, MinTime, MaxTime);
  ShieldRateLow->SetXTitle("Time [sec since 1/1/2010]");
  ShieldRateLow->SetYTitle("cts/sec");
  TH1D* ShieldRateHigh = new TH1D(TString("SAAShieldRateHigh") + iID, TString("SAAShieldRateHigh") + ID, OneSecBins, MinTime, MaxTime);
  ShieldRateHigh->SetXTitle("Time [sec since 1/1/2010]");
  ShieldRateHigh->SetYTitle("cts/sec");
  TH1D* ShieldRateHighAverage = new TH1D(TString("SAAShieldRateHighAverage") + iID, TString("SAAShieldRateHighAverage") + ID, OneSecBins, MinTime, MaxTime);
  ShieldRateHighAverage->SetLineColor(kGreen);
  TH1D* ShieldRateHighCut = new TH1D(TString("SAAShieldRateHighCut") + iID, TString("SAAShieldRateHighCut") + ID, OneSecBins, MinTime, MaxTime);
  ShieldRateHighCut->SetLineColor(kRed);
  TH1D* NAcceptedEvents = new TH1D(TString("SAANAcceptedEvents") + iID, TString("SAANAcceptedEvents") + ID, OneSecBins, MinTime, MaxTime);
  TH1I* OnOffInternalSAA = new TH1I(TString("OnOffInternalSAA") + iID, TString("OnOffInternalSAA") + ID, OneSecBins, MinTime, MaxTime);
  TH1I* OnOffStrictSAA = new TH1I(TString("OnOffStrictSAA") + iID, TString("OnOffStrictSAA") + ID, OneSecBins, MinTime, MaxTime);
  TH1D* NAcceptedEventsShieldHighCut = new TH1D(TString("SAANAcceptedEventsShieldHighCut") + iID, TString("SAANAcceptedEventsShieldHighCut") + ID, OneSecBins, MinTime, MaxTime);
  NAcceptedEventsShieldHighCut->SetLineColor(kRed);
  
  int ExposureTimeInternalSAA = 0;
  for (unsigned int i = 0; i < H.m_Time.size(); ++i) {
    int OIndex = O.FindClosestIndex(ShieldRateHigh->GetBinCenter(i));
    int SAAFlag = O.m_SAAFlag[OIndex];
    if (SAAFlag == 0 && H.m_ShieldRateHigh[i] > 0) { // Some rare events happen even during known SAA passages (super-high energy protons?) - skip those
      ShieldRateLow->Fill(H.m_Time[i], H.m_ShieldRateLow[i]);
      ShieldRateHigh->Fill(H.m_Time[i], H.m_ShieldRateHigh[i]);
      ShieldRateHighCut->Fill(H.m_Time[i], H.m_ShieldRateHigh[i]);
      OnOffInternalSAA->Fill(H.m_Time[i], RejectionOff);
      ++ExposureTimeInternalSAA;
    } else {
      OnOffInternalSAA->Fill(H.m_Time[i], RejectionOn);      
    }
    NAcceptedEvents->Fill(H.m_Time[i], H.m_NAcceptedEvents[i]); 
    NAcceptedEventsShieldHighCut->Fill(H.m_Time[i], H.m_NAcceptedEvents[i]);
    LifeTimes->Fill(H.m_Time[i], H.m_LiveTime[i]);
  }
  
  
  
  ////////////////////////
  // Strict  
  
  OnOffStrictSAA->Add(OnOffInternalSAA);
  
  // Step 1.1: Calculate averages

  // Half length of the interval over which we average:
  int HalfAverageLength = 60; // good values: 30-60
  
  int NBins = 0;
  double Average = 0;
  for (int i = 1; i <= ShieldRateHigh->GetNbinsX(); ++i) {
    Average = 0;
    NBins = 0;
    
    // Determine the interval over which we average dynamically to take care of the edges
    unsigned int Min = i-HalfAverageLength;
    if (Min < 0) Min = 0;
    unsigned int Max = i+HalfAverageLength;
    if (Max > H.m_Time.size()) Max = H.m_Time.size();
    
    for (unsigned int a = Min; a < Max; ++a) {
      // We need to jump over bins where we don't have shield or HK data!!! Why do we have gaps in HK data??
      if (ShieldRateHigh->GetBinContent(a) > 1) { 
        ++NBins;
        Average += ShieldRateHigh->GetBinContent(a);
      }
    }
    Average /= NBins;

    if (ShieldRateHigh->GetBinContent(i) > 1) { // we have some stray "1" during SAA passages... 
      ShieldRateHighAverage->SetBinContent(i, Average);
    }
  }
  
  // Step 1.2: Decide which regions to skip:
  
  /*
  // Do a simple cut, whenever the rate is above MaxRateCut
  double MaxRateCut = 100.0;
  for (int i = 1; i <= ShieldRateHighAverage->GetNbinsX(); ++i) {
    if (ShieldRateHighAverage->GetBinContent(i) > MaxRateCut || ShieldRateHigh->GetBinContent(i) > MaxRateCut) {
      OnOffStrictSAA->SetBinContent(i, RejectionOn);
    }
  }
  */
  
  // We decide based on a few key numbers of the average high veto rate:
  
  // This is the distance between two data points on which we determine that we have a significant increase 
  int DecisionDistance = 4*HalfAverageLength; // 2 for 30 and 60
  // This is the threshold of increase we require for starting to skip
  // There is a slight difference between left and right to account for delayed activation
  double DecisionThresholdLeft = 1.35; // good: 30+1.15, 60+1.35
  double DecisionThresholdRight = 0.95*DecisionThresholdLeft; // good: 30+1.15+0.95, 60+1.3+0.95
  
  
  // We also store the shield-rate values when we started skipping from the left (lower time values) and the right (higher time values) 
  double EnteringValueLeft = 0;
  double EnteringValueRight = 0;
  for (int i = 1; i <= ShieldRateHighAverage->GetNbinsX(); ++i) {
  //for (int i = ShieldRateHigh->GetNbinsX(); i >= 1; --i) {
    // We always make a decision from the left and from the rights
    
    // Here we take care of the boarders of the distribution
    int DecisionPointLeft = i-DecisionDistance;
    if (DecisionPointLeft < 0) DecisionPointLeft = 1;
    int DecisionPointRight = i+DecisionDistance;
    if (DecisionPointRight >= int(H.m_Time.size())) DecisionPointRight = ShieldRateHigh->GetNbinsX();
    
    // We cut this bin if:
    
    bool Cut = false;
    // ... we have an increase above our decision threshold from left to right
    if (ShieldRateHighAverage->GetBinContent(DecisionPointLeft) > 1 && 
        ShieldRateHighAverage->GetBinContent(i) > DecisionThresholdLeft*ShieldRateHighAverage->GetBinContent(DecisionPointLeft)) {
      Cut = true;
      //cout<<"Case A"<<":"<<ShieldRateHighAverage->GetBinContent(i)<<":"<<DecisionThresholdLeft*ShieldRateHighAverage->GetBinContent(DecisionPointLeft)<<endl;
    }
    // ... we have an increase above our decision threshold from right to left
    if (ShieldRateHighAverage->GetBinContent(DecisionPointRight) > 1 &&
        ShieldRateHighAverage->GetBinContent(i) > DecisionThresholdRight*ShieldRateHighAverage->GetBinContent(DecisionPointRight)) {
      Cut = true;
      //cout<<"Case B"<<endl;
    }
    
    // Next we take care of the case if the increase is no longer above the decision threshold,
    // but we are still above the initial increase value:
    
    // If we have both a left and a right value we cut if we are still above the higher values
    if (EnteringValueLeft > 0 && EnteringValueRight  > 0) {
      if (EnteringValueLeft > EnteringValueRight) {
        if (ShieldRateHighAverage->GetBinContent(i) > EnteringValueLeft) {
          Cut = true;
          //cout<<"Case C"<<endl;
        }
      } else {
        if (ShieldRateHighAverage->GetBinContent(i) > EnteringValueRight) {
          Cut = true;
          //cout<<"Case D"<<endl;
        }
      }
    } 
    // If we have just one value, we only cut on it
    else {
      if ((ShieldRateHighAverage->GetBinContent(i) > EnteringValueLeft && EnteringValueLeft > 0) ||
          (ShieldRateHighAverage->GetBinContent(i) > EnteringValueRight && EnteringValueRight  > 0)) {
        Cut = true;
        //cout<<"Case E"<<endl;
      }
    }
    
    
    if (Cut == true) {
      OnOffStrictSAA->SetBinContent(i, RejectionOn);
      // Update the entering values
      if (ShieldRateHighAverage->GetBinContent(i) > DecisionThresholdLeft*ShieldRateHighAverage->GetBinContent(DecisionPointLeft) &&
          (EnteringValueLeft == 0 || ShieldRateHighAverage->GetBinContent(i) < EnteringValueLeft)) { 
        EnteringValueLeft = ShieldRateHighAverage->GetBinContent(i);
      }
      if (ShieldRateHighAverage->GetBinContent(i) > DecisionThresholdRight*ShieldRateHighAverage->GetBinContent(DecisionPointRight) &&
         (EnteringValueRight == 0 || ShieldRateHighAverage->GetBinContent(i) < EnteringValueRight)) {
        EnteringValueRight = ShieldRateHighAverage->GetBinContent(i);
      }
    } else {
      EnteringValueLeft = 0;
      EnteringValueRight = 0;
    }
  }

  // Step 1.3: Cleanup:
  

  // Clean the periods immediately after SAA passages and in SAA passages - since we are still ramping up the voltage and don't get all vetoes
  int SAARampingTime = 60;
  
  bool InSAAorFreakEvent = false;
  for (int b = 1; b <= ShieldRateHigh->GetNbinsX(); ++b) {
    int OIndex = O.FindClosestIndex(ShieldRateHigh->GetBinCenter(b));
    int SAAFlag = O.m_SAAFlag[OIndex];
    if (SAAFlag == 1) {
      InSAAorFreakEvent = true;
      OnOffStrictSAA->SetBinContent(b, RejectionOn); 
    } else {
      if (InSAAorFreakEvent == true) {
        // Flag the next "SAARampingTime" seconds as still bad, since the high voltage is ramping up slowly
        for (int bb = b; bb <= ShieldRateHigh->GetNbinsX() && bb-b < SAARampingTime; ++bb) {
          OnOffStrictSAA->SetBinContent(bb, RejectionOn);
        }
      }
      InSAAorFreakEvent = false;
    }
  }
  
  
  // Clean up - all SAA passages are confinend between MinLong and MaxLong
  // We might replace this one day with a real outer SAA contour...
  double MinLong = SAARegionMin;
  double MaxLong = SAARegionMax;
  for (int i = 1; i <= OnOffStrictSAA->GetNbinsX(); ++i) {
    int OIndex = O.FindClosestIndex(OnOffStrictSAA->GetBinCenter(i));
    if (fabs(O.m_Time[OIndex] - OnOffStrictSAA->GetBinCenter(i)) < 1.0) {
      if (O.m_Longitude[i] < MinLong || O.m_Longitude[i] > MaxLong) {
        OnOffStrictSAA->SetBinContent(i, RejectionOff);
      }
    } else {
      cerr<<"Hmmm, no Orbits information found... Ignoring..."<<endl;
    }
  }


  // Clean up - remove off periods < X*HalfAverageLength

  int CutOffCount = HalfAverageLength; // good: 2x
  
  int OffCount = 0;
  for (int i = 1; i <= ShieldRateHigh->GetNbinsX(); ++i) {
    if (OnOffStrictSAA->GetBinContent(i) == 1) {
      if (OffCount > 0 && OffCount < CutOffCount) {
        for (int b = i-1; b >= i-OffCount; --b) {
          if (OnOffStrictSAA->GetBinContent(b) == 0) {
            OnOffStrictSAA->SetBinContent(b, RejectionOff);
          } else {
            cerr<<"On remove error..."<<endl;
          }
        }
      }
      OffCount = 0;
    } else {
      OffCount++;
    }
  }

  
  // Remove periods < X*HalfAverageLength
  int OnCount = 0;
  for (int i = 1; i <= ShieldRateHigh->GetNbinsX(); ++i) {
    //cout<<i<<": "<<OnCount<<" - "<<OnOffStrictSAA->GetBinContent(i)<<endl;
    if (OnOffStrictSAA->GetBinContent(i) == 0) {
      if (OnCount > 0 && OnCount < CutOffCount) {
        for (int b = i-1; b >= i-OnCount; --b) {
          if (OnOffStrictSAA->GetBinContent(b) == 1) {
            OnOffStrictSAA->SetBinContent(b, RejectionOn);
          } else {
            cerr<<"On remove error..."<<endl;
          }
        }
      }
      OnCount = 0;
    } else {
      OnCount++;
    }
  }
  
  int LTStrictSAA = 0;
  for (int b = 1; b <= OnOffStrictSAA->GetNbinsX(); ++b) {
    int Index = H.FindClosestIndex(OnOffStrictSAA->GetBinCenter(b));
    if (OnOffStrictSAA->GetBinCenter(b) - H.m_Time[Index] < 1.0) {
      if (OnOffStrictSAA->GetBinContent(b) == RejectionOn || OnOffInternalSAA->GetBinContent(b) == RejectionOn) {
        if (Mode == c_SAACutStrictHSR) H.m_SoftSAA[Index] = true;
        H.m_SoftSAAStrictHSR[Index] = true;
      } else {
        if (Mode == c_SAACutStrictHSR) H.m_SoftSAA[Index] = false;
        H.m_SoftSAAStrictHSR[Index] = false;
        ++LTStrictSAA;
      }
    } else {
      //cerr<<"Something is wrong with the times as I cannot find housekeeping data... "<<OnOffStrictSAA->GetBinCenter(b)<<endl; 
    }
  }
  cout<<"Strict: "<<100 * double(LTStrictSAA)/ExposureTimeInternalSAA<<"%"<<endl;
  
  // END step 1: End of strict cut:

  
  
  
  ////////////////////////
  // Optimized  
  
  // Step 2: Do an optimized cut by backtracking until the good, on source count rate goes above a certain level
  //         backtrack - make sure to only exclude regions where we actually see an increase in the final count rate

  TH1I* OnOffOptimizedSAA = new TH1I(TString("OnOffOptimizedSAA") + iID, TString("OnOffOptimizedSAA") + ID, OneSecBins, MinTime, MaxTime);
  OnOffOptimizedSAA->Add(OnOffStrictSAA); // We start with strict SAA cuts!
  TH1D* FilteredRate = new TH1D(TString("SAAFilteredRate") + iID, TString("SAAFilteredRate") + ID, OneSecBins, MinTime, MaxTime);
  FilteredRate->SetXTitle("Time [sec since 1/1/2010]");
  FilteredRate->SetYTitle("cts/sec");
  TH1D* CleanRate = new TH1D(TString("SAACleanRate") + iID, TString("SAACleanRate") + ID, OneSecBins, MinTime, MaxTime);
  CleanRate->SetXTitle("Time [sec since 1/1/2010]");
  CleanRate->SetYTitle("cts/sec");
  TH1D* FilteredRateCutOriginal = new TH1D(TString("SAAFilteredRateCut") + iID, TString("SAAFilteredRateCut") + ID, OneSecBins, MinTime, MaxTime);
  FilteredRateCutOriginal->SetLineColor(kMagenta);
  TH1D* FilteredRateCut = new TH1D(TString("SAAFilteredRateCut") + iID, TString("SAAFilteredRateCut") + ID, OneSecBins, MinTime, MaxTime);
  FilteredRateCut->SetLineColor(kRed);
  TH1D* FilteredRateAverageLarge = new TH1D(TString("SAAFilteredRateAverageLarge") + iID, TString("SAAFilteredRateHighLarge") + ID, OneSecBins, MinTime, MaxTime);
  FilteredRateAverageLarge->SetLineColor(kGreen);
  TH1D* FilteredRateAverageSmall = new TH1D(TString("SAAFilteredRateAverageSmall") + iID, TString("SAAFilteredRateHighSmall") + ID, OneSecBins, MinTime, MaxTime);
  FilteredRateAverageSmall->SetLineColor(kYellow);

  double LiveTimeNoCuts = 0;
  double LifeTimestrictCuts = 0;
  double LiveTimeOptimizedCuts = 0;
  
  
  // Fill:
  for (unsigned int i = 0; i < F.m_Time.size(); ++i) {
    FilteredRate->Fill(F.m_Time[i]);
  }
  for (int b = 1; b <= FilteredRate->GetNbinsX(); ++b) {
    if (LifeTimes->GetBinContent(b) > 0) {
      // No live time correction! FilteredRate->SetBinContent(b, FilteredRate->GetBinContent(b)/LifeTimes->GetBinContent(b)); 
      FilteredRateCutOriginal->SetBinContent(b, FilteredRate->GetBinContent(b)); 
      FilteredRateCut->SetBinContent(b, FilteredRate->GetBinContent(b)); 
    } else {
      FilteredRate->SetBinContent(b, 0);       
      FilteredRateCutOriginal->SetBinContent(b, FilteredRate->GetBinContent(b)); 
      FilteredRateCut->SetBinContent(b, FilteredRate->GetBinContent(b)); 
    }
    if (F.IsGTI(FilteredRate->GetBinCenter(b)) == true) LiveTimeNoCuts += LifeTimes->GetBinContent(b);
  }
  
  for (int b = 1; b <= FilteredRateCutOriginal->GetNbinsX(); ++b) {
    if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOff) {
      FilteredRateCutOriginal->SetBinContent(b, 0);
      if (F.IsGTI(FilteredRateCutOriginal->GetBinCenter(b)) == true)  {
        LifeTimestrictCuts += LifeTimes->GetBinContent(b); 
      }
    }
  }

  
  // Create average again:
  int FilteredEventsHalfAverageLarge = 120;
  for (int i = 1; i <= FilteredRate->GetNbinsX(); ++i) {
    Average = 0;
    NBins = 0;
    
    // Determine the interval over which we average dynamically to take care of the edges
    int Min = i-FilteredEventsHalfAverageLarge;
    if (Min < 1) Min = 1;
    int Max = i+FilteredEventsHalfAverageLarge;
    if (Max > FilteredRate->GetNbinsX()) Max = FilteredRate->GetNbinsX();
    
    for (int a = Min; a <= Max; ++a) {
      // We need to jump over bins where we don't have HK data
      if (FilteredRate->GetBinContent(a) > 0) { 
        ++NBins;
        Average += FilteredRate->GetBinContent(a);
      }
    }
    if (NBins == 0) continue;

    Average /= NBins;
    FilteredRateAverageLarge->SetBinContent(i, Average);
  }
  int FilteredEventsHalfAverageSmall = 40;
  for (int i = 1; i <= FilteredRate->GetNbinsX(); ++i) {
    Average = 0;
    NBins = 0;
    
    // Determine the interval over which we average dynamically to take care of the edges
    int Min = i-FilteredEventsHalfAverageSmall;
    if (Min < 1) Min = 1;
    int Max = i+FilteredEventsHalfAverageSmall;
    if (Max > FilteredRate->GetNbinsX()) Max = FilteredRate->GetNbinsX();
    
    for (int a = Min; a <= Max; ++a) {
      // We need to jump over bins where we don't have data 
      if (FilteredRate->GetBinContent(a) > 0) { 
        ++NBins;
        Average += FilteredRate->GetBinContent(a);
      }
    }
    if (NBins == 0) continue;
    
    Average /= NBins;
    FilteredRateAverageSmall->SetBinContent(i, Average);
  }


  // Track from the left and right
  double FilteredDecisionThresholdLeft = 1.25;
  double FilteredDecisionThresholdRight = 1.25;
  double SingleRateDecisionThreshold = 2.25;
  bool Previous = RejectionOff;
  for (int b = 1; b <= OnOffOptimizedSAA->GetNbinsX(); ++b) {
    // Switch off -> on
    if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOn && Previous == RejectionOff) {
      Previous = RejectionOn;
      int Min = b-FilteredEventsHalfAverageSmall;
      if (Min < 1) Min = 1;
      int Dist = b-Min;
      while (FilteredRateAverageSmall->GetBinContent(b) < FilteredDecisionThresholdLeft*FilteredRateAverageLarge->GetBinContent(Min) &&
             FilteredRate->GetBinContent(b) < SingleRateDecisionThreshold*FilteredRateAverageLarge->GetBinContent(Min) + 3*sqrt(FilteredRateAverageLarge->GetBinContent(Min)) ) {
        for (int a = b-Dist; a <= b; ++a) OnOffOptimizedSAA->SetBinContent(a, RejectionOff);
        ++b;
        if (b > OnOffOptimizedSAA->GetNbinsX()) break;
        if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOff) break;
      }
    }
    // Switch on -> off
    else if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOff && Previous == RejectionOn) {
      Previous = RejectionOff;
    }
  }
  // Track from right to left
  Previous = RejectionOff;
  for (int b = OnOffOptimizedSAA->GetNbinsX(); b >= 1; --b) {
    // Switch off -> on
    if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOn && Previous == RejectionOff) {
      //cout<<"Found new rejection on at t="<<setprecision(10)<<OnOffOptimizedSAA->GetBinCenter(b)<<endl;
      Previous = RejectionOn;
      int Max = b+FilteredEventsHalfAverageSmall;
      if (Max > OnOffOptimizedSAA->GetNbinsX()) Max = OnOffOptimizedSAA->GetNbinsX();
      int Dist = Max-b;
      while (FilteredRateAverageSmall->GetBinContent(b) < FilteredDecisionThresholdRight*FilteredRateAverageLarge->GetBinContent(Max) &&
             FilteredRate->GetBinContent(b) < SingleRateDecisionThreshold*FilteredRateAverageLarge->GetBinContent(Max) + 3*sqrt(FilteredRateAverageLarge->GetBinContent(Max)) ) {
       //cout<<"At t="<<setprecision(10)<<OnOffOptimizedSAA->GetBinCenter(b)<<" value: "<<FilteredRateAverageSmall->GetBinContent(b)<<"  threshold: "<<FilteredDecisionThresholdRight*FilteredRateAverageLarge->GetBinContent(Max)<<endl;
        for (int a = b; a <= b+Dist; ++a) OnOffOptimizedSAA->SetBinContent(a, RejectionOff);
        --b;
        if (b < 1) break;
        if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOff) break;
      }
    }
    // Switch on -> off
    else if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOff && Previous == RejectionOn) {
      Previous = RejectionOff;
    }
  }

  
  for (int b = 1; b <= NAcceptedEventsShieldHighCut->GetNbinsX(); ++b) {
    if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOn) NAcceptedEventsShieldHighCut->SetBinContent(b, 0);
    if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOff) ShieldRateHighCut->SetBinContent(b, 0);
    if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOff) {
      CleanRate->SetBinContent(b, FilteredRate->GetBinContent(b));
      FilteredRateCut->SetBinContent(b, 0);
      if (F.IsGTI(FilteredRateCutOriginal->GetBinCenter(b)) == true) {
        LiveTimeOptimizedCuts += LifeTimes->GetBinContent(b); 
      }
    }
  }
    
  /*  
  TCanvas* ShieldRateHighCanvas = new TCanvas();
  ShieldRateHighCanvas->cd();
  ShieldRateHigh->Draw();
  ShieldRateHighCut->Draw("SAME");
  ShieldRateHighAverage->Draw("SAME");
  ShieldRateHighCanvas->Update();
  */  
  /*
  TCanvas* NAcceptedEventsCanvas = new TCanvas();
  NAcceptedEventsCanvas->cd();
  NAcceptedEvents->Draw();
  NAcceptedEventsShieldHighCut->Draw("SAME");
  NAcceptedEventsCanvas->Update();
  */
  
  if (Show == true) {
    TCanvas* FilteredRateCanvas = new TCanvas();
    FilteredRateCanvas->cd();
    FilteredRate->Draw();
    FilteredRateCutOriginal->Draw("SAME");
    FilteredRateCut->Draw("SAME");
    FilteredRateAverageLarge->Draw("SAME");
    FilteredRateAverageSmall->Draw("SAME");
    FilteredRateCanvas->Update();

    TCanvas* CleanRateCanvas = new TCanvas();
    CleanRateCanvas->cd();
    CleanRate->Draw();
    CleanRateCanvas->Update();
  }
  
  int LTOptimizedSAA = 0;
  for (int b = 1; b <= OnOffOptimizedSAA->GetNbinsX(); ++b) {
    int Index = H.FindClosestIndex(OnOffOptimizedSAA->GetBinCenter(b));
    if (OnOffOptimizedSAA->GetBinCenter(b) - H.m_Time[Index] < 1.0) {
      if (OnOffOptimizedSAA->GetBinContent(b) == RejectionOn || OnOffInternalSAA->GetBinContent(b) == RejectionOn) {
        if (Mode == c_SAACutOptimizedHSRFoM) H.m_SoftSAA[Index] = true;
        H.m_SoftSAAOptimizedHSRFoM[Index] = true;
      } else {
        if (Mode == c_SAACutOptimizedHSRFoM) H.m_SoftSAA[Index] = false;
        H.m_SoftSAAOptimizedHSRFoM[Index] = false;
        ++LTOptimizedSAA;
      }
    } else {
      //cerr<<"Something is wrong with the times as I cannot find housekeeping data... "<<OnOffOptimizedSAA->GetBinCenter(b)<<endl; 
    }
  }
  cout<<"Optimized: "<<100 * double(LTOptimizedSAA)/ExposureTimeInternalSAA<<"%"<<endl;

  return true;
}  


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::FindSAAsLowThresholdShieldRateBased(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show)
{
  // Basic strategy
  // (1) Create a mask of all regions where the low shield rate peaks
  //     This is done by using a Bayesian block peak identification approach
  //     (The same one used for peak identification in the line calibrator of MEGAlib)
  // (2) In this region filter out everything where the detector rate is above a certain RMS 
  //     threshold
  
  double MaxAllowedRMS = m_SAACutRMSThreshold;          // --> user adjustable
  bool ElimiateSource = m_SAACutRMSSourceElimination;   // --> user adjustable
  bool PerformSanityChecks = m_SAACutRMSSanityChecks;   // --> user adjustable
  
  
  cout<<endl;
  cout<<"Finding SAA - low threshold shield rate mode..."<<endl;

  if (F.m_Time.size() == 0) {
    cout<<"No filtered events = no SAA cut"<<endl;  
    return true;
  }
  
  // Start with no cuts (clean in case we run it):
  for (unsigned int i = 0; i < H.m_Time.size(); ++i) {
    H.m_SoftSAA[i] = false;
    H.m_SoftSAAStrictLSR[i] = false;
    H.m_SoftSAAOptimizedLSRRMS[i] = false;
  }
  
  // Define rejection flags
  int RejectionOn  = 0;
  int RejectionOff = 1;
  
  // One second binning
  int OneSecBins = H.GetMaxTime() - H.GetMinTime() + 1;
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;
  
  // 60-second binning -- good compromise for having enough statistics per bin, and small enough bins for not cutting out too much 
  unsigned int SuperStrictOffTimeInterval = 60;    // into caldb 
  int SuperStrictBins = OneSecBins/SuperStrictOffTimeInterval;

  TString iID = TString("_ID_") + H.m_ID + "_M_" + ((H.m_Module == 0) ? "A" : "B");
  TString ID = TString(" (ID: ") + H.m_ID + ((H.m_Module == 0) ? "A" : "B") + ")";

  
  // Create a list of pixels with high source count for elimination
  vector<int> ExcludedDetRawXY;
  if (ElimiateSource == true) ExcludedDetRawXY = MostlyEliminateSource(F, O, Show);  
  
  
  // Step One: Create a mask of the regions where the low-threshold shield rate is high, utilizing a Bayesian Block peak finder  
  
  cout<<"Timings: "<<SuperStrictBins<<":"<<setprecision(10)<<MinTime<<":"<<setprecision(10)<<MaxTime<<endl;
  
  // (A) Define histograms
  TH1D* ShieldRateLow = new TH1D(TString("ShieldRateLow") + iID, TString("SAA/RMS: ShieldRateLow") + ID, SuperStrictBins, MinTime, MaxTime);
  ShieldRateLow->SetXTitle("Time [sec since 1/1/2010]");
  ShieldRateLow->SetYTitle("cts/sec");
  
  TH1I* OnOffInternalSAA = new TH1I(TString("OnOffInternalSAA") + iID, TString("SAA/RMS: OnOffInternalSAA") + ID, OneSecBins, MinTime, MaxTime);

  TH1D* RawRate = new TH1D(TString("RawRate") + iID, TString("RawRate") + ID, OneSecBins, MinTime, MaxTime);
  RawRate->SetXTitle("Time [sec since 1/1/2010]");
  RawRate->SetYTitle("cts/sec");

  TH1D* LifeTime = new TH1D(TString("LifeTime") + iID, TString("LifeTime") + ID, OneSecBins, MinTime, MaxTime);
  LifeTime->SetXTitle("Time [sec since 1/1/2010]");
  LifeTime->SetYTitle("cts/sec");
     
  // (B) Fill the histograms
  int ExposureTimeInternalSAA = 0;
  for (unsigned int i = 0; i < H.m_Time.size(); ++i) {
    LifeTime->Fill(H.m_Time[i], H.m_LiveTime[i]);  
    if (WithinSpecialGTI(H.m_Time[i]) == false) continue;
    if (H.m_HardSAA[i] == false) {
      ShieldRateLow->Fill(H.m_Time[i], H.m_ShieldRateLow[i]);
      OnOffInternalSAA->Fill(H.m_Time[i], RejectionOff);
      ++ExposureTimeInternalSAA;
    } else {
      OnOffInternalSAA->Fill(H.m_Time[i], RejectionOn);      
    }
  }
  DebugOutput(LifeTime, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_LiveTime");
  DebugOutput(ShieldRateLow, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_ShieldRateLow");
  DebugOutput(OnOffInternalSAA, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_OnOffInternalSAA"); 
 
  for (unsigned int i = 0; i < F.m_Time.size(); ++i) {
    if (WithinSpecialGTI(F.m_Time[i]) == false) continue;
    if (F.m_Energy[i] < m_SpectrumMin || F.m_Energy[i] > m_SpectrumMax) continue;    
    //if (F.IsGTI(F.m_Time[i]) == false) continue;

    int DetectorID = F.m_DetectorID[i];
    int RawX = F.m_RawX[i];
    int RawY = F.m_RawY[i];
    int ID = 10000*DetectorID + 100*RawX + RawY;
    vector<int>::iterator I = lower_bound(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end(), 10000*DetectorID + 100*RawX + RawY);
    if (I != ExcludedDetRawXY.end() && (*I) == ID) continue;

    RawRate->Fill(F.m_Time[i]);
  }

  if (RawRate->Integral() == 0) {
    cout<<"Something went wrong! No data!"<<endl;
    return false;
  }
  
  // (C) Create a Bayesian Block binned histogram of the shield data
  
  const int Prior = 200;      // into caldb 
  
  // Step 1: Create a histogram with Bayesian block binning
  MBinnerBayesianBlocks Binner;
  //cout<<"Min/Max/Diff/Bins: "<<ShieldRateLow->GetBinCenter(1)<<":"<<ShieldRateLow->GetBinCenter(ShieldRateLow->GetNbinsX())<<":"<<ShieldRateLow->GetBinCenter(ShieldRateLow->GetNbinsX()) - ShieldRateLow->GetBinCenter(1)<<":"<<ShieldRateLow->GetNbinsX()<<endl;
  //Binner.SetMinMax(ShieldRateLow->GetBinCenter(1), ShieldRateLow->GetBinCenter(ShieldRateLow->GetNbinsX()));
  Binner.SetMinMax(MinTime, MaxTime);
  Binner.SetMinimumBinWidth(SuperStrictOffTimeInterval);
  Binner.SetPrior(Prior); 
  ShieldRateLow->Smooth(1);     // into caldb 
  DebugOutput(ShieldRateLow, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_ShieldRateLowSmoothed");
  
  for (int b = 1; b <= ShieldRateLow->GetNbinsX(); ++b) {
    Binner.Add(ShieldRateLow->GetBinCenter(b), ShieldRateLow->GetBinContent(b));
  }
  TH1D* ShieldRateLowBB = Binner.GetNormalizedHistogram("BB-binned low count rate", "Time since epoch", "cts/sec");
  
  DebugOutput(ShieldRateLowBB, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_ShieldRateLowBB");
  
  
  // (D) Calculate a simple (slightly) shifted, *normalized* gradient histogram
  TH1D* GradientHistogram = new TH1D(*ShieldRateLowBB);
  GradientHistogram->SetTitle(TString("Gradient histogram"));
  GradientHistogram->Reset();
  for (int b = 2; b <= GradientHistogram->GetNbinsX(); ++b) {
    double Average = 0.5*(ShieldRateLowBB->GetBinContent(b)+ShieldRateLowBB->GetBinContent(b-1));
    GradientHistogram->SetBinContent(b, (ShieldRateLowBB->GetBinContent(b)-ShieldRateLowBB->GetBinContent(b-1))/
                                        (ShieldRateLowBB->GetBinCenter(b)-ShieldRateLowBB->GetBinCenter(b-1))/Average);
  }
  
  DebugOutput(GradientHistogram, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_Gradient");

  
  // (E) Find zero passages + --> - and regions around zero passages for SAA exclusion
  //     i.e. find the peaks
  
  //double PeakBin;
  double PeakLowEdge;
  double PeakHighEdge;
  
  TH1I* OnOffStrictLSR = new TH1I(TString("OnOffStrictLSR") + iID, TString("OnOffStrictLSR") + ID, OneSecBins, MinTime, MaxTime);
  OnOffStrictLSR->Add(OnOffInternalSAA);
  
  for (int b = 1; b < GradientHistogram->GetNbinsX(); ++b) { // Shifted one down 
    if (GradientHistogram->GetBinContent(b) > 0 && GradientHistogram->GetBinContent(b+1) < 0) {  
      //PeakBin = GradientHistogram->GetBinLowEdge(b+1);
      //cout<<PeakBin<<" - investigating..."<<endl;
      
      // Find edges:
      int MaximumBin = b;
      for (int bb = MaximumBin-1; bb >= 1; --bb) {
        if (GradientHistogram->GetBinContent(bb) > GradientHistogram->GetBinContent(MaximumBin)) {
          MaximumBin = bb;
        } else {
          break;
        }
      }
        
      int MinimumBin = b+1;
      for (int bb = MinimumBin+1; bb <= GradientHistogram->GetNbinsX(); ++bb) {
        if (GradientHistogram->GetBinContent(bb) < GradientHistogram->GetBinContent(MinimumBin)) {
          MinimumBin = bb;
        } else {
          break;
        }
      }
      
      double Minimum = GradientHistogram->GetBinContent(MinimumBin);
      double Maximum = GradientHistogram->GetBinContent(MaximumBin);
      double Volatility = Maximum - Minimum;
      
      double MinAllowedVolatility = 0.002;    // in caldb
      double MinMaximum = +0.00075;           // in caldb
      double MaxMinimum = +0.00025;           // in caldb
      
      if (!(Volatility > MinAllowedVolatility && 
          ((Maximum > +MinMaximum && Minimum < -MaxMinimum) ||
           (Minimum < -MinMaximum && Maximum > +MaxMinimum)))) {
        //cout<<PeakBin<<" - Rejected: Derivative peak not strong enough: max:"<<GradientHistogram->GetBinContent(MaximumBin)<<" min: "<<GradientHistogram->GetBinContent(MinimumBin)<<"  vol:"<<Volatility<<endl;
        continue;
      }
     
      
      // Go further, until the variation from zero is below XYZ (rights lower for activation):
      double MinVariationLeft = 0.0005;           // in caldb
      double MinVariationRight = 0.00035;          // in caldb
      while (MaximumBin > 1 && fabs(GradientHistogram->GetBinContent(MaximumBin-1)) > MinVariationLeft) {
        --MaximumBin;
      }
      while (MinimumBin < GradientHistogram->GetNbinsX() && fabs(GradientHistogram->GetBinContent(MinimumBin+1)) > MinVariationRight) { // Relaxed for delayed decay
        ++MinimumBin; 
      }
      // Go one further in the minimum direction to
      
      PeakLowEdge = GradientHistogram->GetBinLowEdge(MaximumBin);
      PeakHighEdge = GradientHistogram->GetBinLowEdge(MinimumBin)+GradientHistogram->GetBinWidth(MinimumBin);
      
      //cout<<PeakBin<<" - Region: "<<setprecision(10)<<PeakLowEdge<<" - "<<setprecision(10)<<PeakHighEdge<<endl;
      
      // Add to the on-off-histo:
      for (int bx = OnOffStrictLSR->FindBin(PeakLowEdge); bx <= OnOffStrictLSR->FindBin(PeakHighEdge); ++bx) {
        //cout<<"Rejection at "<<OnOffStrictLSR->GetBinCenter(bx)<<endl;
        OnOffStrictLSR->SetBinContent(bx, RejectionOn);
      }
      
    }
  }
  
  // (F) Add the internal/hardware SAA rejection regions
  for (int b = 1; b <= OnOffStrictLSR->GetNbinsX(); ++b) {
    if (OnOffInternalSAA->GetBinContent(b) == RejectionOn) {
      OnOffStrictLSR->SetBinContent(b, RejectionOn);
    }
  }

  DebugOutput(OnOffStrictLSR, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_OnOffStrictLSRFinal"); 
  
  
  // (G) Store the preliminary cut
  int ExposureTimeStrictLSR = 0;
  for (int b = 1; b <= OnOffStrictLSR->GetNbinsX(); ++b) {
    int Index = H.FindClosestIndex(OnOffStrictLSR->GetBinCenter(b));
    if (OnOffStrictLSR->GetBinCenter(b) - H.m_Time[Index] < 1.0) {
      if (OnOffStrictLSR->GetBinContent(b) == RejectionOn) { 
        if (Mode == c_SAACutStrictLSR) H.m_SoftSAA[Index] = true;
        H.m_SoftSAAStrictLSR[Index] = true;
      } else {
        if (Mode == c_SAACutStrictLSR) H.m_SoftSAA[Index] = false;
        H.m_SoftSAAStrictLSR[Index] = false;
        ++ExposureTimeStrictLSR;
      }
    } else {
      //cerr<<"Something is wrong with the times as I cannot find housekeeping data... "<<OnOffStrictLSR->GetBinCenter(b)<<endl; 
    }
  }
  cout<<"Exposure time strict: "<<100 * double(ExposureTimeStrictLSR)/ExposureTimeInternalSAA<<"%"<<endl;
  
  
  // (H) Show the result if desired
  if (Show == true) {
    TH1D* Rate = new TH1D(TString("Rate") + iID, TString("SAA/RMS - LSR only part: Rate") + ID, OneSecBins/60, MinTime, MaxTime);
    Rate->SetXTitle("Time [sec since 1/1/2010]");
    Rate->SetYTitle("cts/sec");
    
    cout<<"Events: "<<F.m_Time.size()<<endl;
    for (unsigned int i = 0; i < F.m_Time.size(); ++i) {
      if (WithinSpecialGTI(F.m_Time[i]) == false) continue;
      if (F.IsGTI(F.m_Time[i]) == false) continue;
      if (F.m_Energy[i] < m_SpectrumMin || F.m_Energy[i] > m_SpectrumMax) continue;
      
      int HKIndex = H.FindClosestIndex(F.m_Time[i]);
      if (HKIndex == -1) {
        cout<<"Housekeeping: Index not found for time "<<F.m_Time[i]<<"..."<<endl;
        continue;      
      }
      if (H.m_SoftSAAStrictLSR[HKIndex] == true) continue;

      int DetectorID = F.m_DetectorID[i];
      int RawX = F.m_RawX[i];
      int RawY = F.m_RawY[i];
      int ID = 10000*DetectorID + 100*RawX + RawY;
      vector<int>::iterator I = lower_bound(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end(), 10000*DetectorID + 100*RawX + RawY);
      if (I != ExcludedDetRawXY.end() && (*I) == ID) continue;
      
      Rate->Fill(F.m_Time[i]);
    }
    
    TCanvas* ShieldRateLowBBCanvas = new TCanvas();
    ShieldRateLowBBCanvas->cd();
    ShieldRateLowBB->Draw();
    TH1I* OnOffStrictLSRAsDrawn = new TH1I(*OnOffStrictLSR);
    OnOffStrictLSRAsDrawn->Scale(ShieldRateLowBB->GetMaximum());
    OnOffStrictLSRAsDrawn->SetLineColor(kYellow);
    OnOffStrictLSRAsDrawn->SetFillColor(kYellow);
    OnOffStrictLSRAsDrawn->Draw("SAME");
    ShieldRateLowBB->Draw();
    ShieldRateLowBBCanvas->Update();
    
    
    TCanvas* GradientHistogramCanvas = new TCanvas();
    GradientHistogramCanvas->cd();
    GradientHistogram->Draw();
    GradientHistogramCanvas->Update();
    
    TCanvas* RateCanvas = new TCanvas();
    RateCanvas->cd();
    Rate->Draw();
    TH1I* OnOffStrictLSRAsDrawn2 = new TH1I(*OnOffStrictLSR);
    OnOffStrictLSRAsDrawn2->Scale(Rate->GetMaximum());
    OnOffStrictLSRAsDrawn2->SetLineColor(kYellow);
    OnOffStrictLSRAsDrawn2->SetFillColor(kYellow);
    OnOffStrictLSRAsDrawn2->Draw("SAME");
    Rate->Draw("SAME");
    RateCanvas->Update();  
  }
  
  
  
  // Step 2: Check for rate increases and only cut those regions
  
  // (A) Create and fill all required histograms
  
  // We have to choose a region close to the SAA to have a similar average count rate
  // i.e. everything between 0-180 is too low
  double NonSAAMinLong = 180;           // in caldb
  double NonSAAMaxLong = 250;           // in caldb
  
  // This histogram shows the rate in the GTIs in the SAA free region  
  TH1D* NonSAARate = new TH1D(TString("NonSAARate") + iID, TString("SAA/RMS: NonSAARate") + ID, SuperStrictBins, MinTime, MaxTime);
  NonSAARate->SetXTitle("Time [sec since 1/1/2010]");
  NonSAARate->SetYTitle("cts/sec");

  TH1D* EvaluationRate = new TH1D(TString("EvaluationRate") + iID, TString("SAA/RMS: EvaluationRate") + ID, SuperStrictBins, MinTime, MaxTime);
  EvaluationRate->SetXTitle("Time [sec since 1/1/2010]");
  EvaluationRate->SetYTitle("cts/sec");

  TH1D* EvaluationRateLifeTimes = new TH1D(TString("EvaluationRateLifeTimes") + iID, TString("SAA/RMS: EvaluationRateLifeTimes") + ID, SuperStrictBins, MinTime, MaxTime);
  EvaluationRateLifeTimes->SetXTitle("Time [sec since 1/1/2010]");
  EvaluationRateLifeTimes->SetYTitle("cts/sec");

  ofstream deb;
  if (m_Debug == true) deb.open(TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_EventLog");
  
  for (unsigned int i = 0; i < F.m_Time.size(); ++i) {
    if (WithinSpecialGTI(F.m_Time[i]) == false) {
      if (m_Debug == true) deb<<"ID "<<i<<" --> special GTI reject"<<endl;
      continue;
    }
    //if (F.IsGTI(F.m_Time[i]) == false) continue;
    if (F.m_Energy[i] < m_SpectrumMin || F.m_Energy[i] > m_SpectrumMax) {
      if (m_Debug == true) deb<<"ID "<<i<<" --> energy reject"<<endl;
      continue;
    }
    if (O.IsGoodOrbit(F.m_Time[i]) == false) {
      if (m_Debug == true) deb<<"ID "<<i<<" --> orbit reject, index: "<<O.FindOrbitIndex(F.m_Time[i])<<endl;
      continue;
    }
    
    // Exclude source region
    int DetectorID = F.m_DetectorID[i];
    int RawX = F.m_RawX[i];
    int RawY = F.m_RawY[i];
    int ID = 10000*DetectorID + 100*RawX + RawY;
    vector<int>::iterator I = lower_bound(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end(), 10000*DetectorID + 100*RawX + RawY);
    if (I != ExcludedDetRawXY.end() && (*I) == ID) continue;
    
    int HKIndex = H.FindClosestIndex(F.m_Time[i]);
    if (HKIndex < 0) continue;
    
    double LifeTime = H.m_LiveTime[HKIndex];
    if (m_DoLifeTimeCorrection == false) LifeTime = 1.0;
    if (LifeTime > 0) {
      EvaluationRate->Fill(F.m_Time[i], 1.0/LifeTime);
    }
    if (m_Debug == true) deb<<"ID "<<i<<" time: "<<setprecision(10)<<F.m_Time[i]<<" bin: "<<EvaluationRate->FindBin(F.m_Time[i])-1<<" content "<<EvaluationRate->GetBinContent(EvaluationRate->FindBin(F.m_Time[i]))<<" --> Good"<<endl;
    int Index = O.FindClosestIndex(F.m_Time[i]);
    if (O.m_Longitude[Index] >= NonSAAMinLong && O.m_Longitude[Index] <= NonSAAMaxLong) {
      if (LifeTime > 0) {
        NonSAARate->Fill(F.m_Time[i], 1.0/LifeTime);
      }
    }
  }
  if (m_Debug == true) deb.close();

  
  DebugOutput(NonSAARate, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_NonSAARate"); 
  DebugOutput(EvaluationRate, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_EvaluationRate"); 
  
  
  // (B) Determine the mean and rms detector count rate in the regions away from the SAA as well as the threshold
   
  // (a) Create a rate histogram
  int NonSAARateHistogramNumberOfEntries = 0;
  TH1D* NonSAARateHistogram = new TH1D(TString("NonSAARateHistogram") + iID, TString("NonSAARateHistogram") + ID, 1000, 0, NonSAARate->GetMaximum() + 1);
  for (int b = 2; b < NonSAARate->GetNbinsX(); ++b) {
    if (NonSAARate->GetBinContent(b) > 0 &&
        NonSAARate->GetBinContent(b-1) > 0 && NonSAARate->GetBinContent(b+1) > 0) { // Ignore half filled edge pixels
      NonSAARateHistogram->Fill(NonSAARate->GetBinContent(b));
      ++NonSAARateHistogramNumberOfEntries;
    }
  }

  DebugOutput(NonSAARateHistogram, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_NonSAARateHistogram");
  
  // (b) Calculate Average and RMS
  double NonSAARateMax = NonSAARate->GetMaximum();
  double NonSAARateMean = NonSAARateHistogram->GetMean();
  double NonSAARateRMS = NonSAARateHistogram->GetRMS();
  double NonSAARateSigma = sqrt(NonSAARateMean);
  
  double Threshold = MaxAllowedRMS*NonSAARateRMS + NonSAARateMean;
  
  // Take care of the rare case when we have observations with strong outbursts
  // Normal outburst should increase the RMS thus we are automatically in a higher threshold regime
  // Then we set the threshold to the maximum + 1 rms
  // There is only one case where this ever happened "RapidBurster"
  
  // If the rms is more than 3 times the expected 1-sigma value, then we have a burster 
  // Thus set the threshold to maximum + MaxAllowedRMS * rms
  double TimeOneSigma = 3.0;
  if (NonSAARateRMS > TimeOneSigma * NonSAARateSigma) {
    Threshold = NonSAARateMax + MaxAllowedRMS*NonSAARateRMS;
    cout<<"--> Bursts detected! Threshold is max + "<<MaxAllowedRMS<<"*rms"<<endl;
  }
  
  // When we have no data in the NonSAA region our threshold will be zero,
  // Set it ti the maximum
  if (Threshold == 0) {
    cout<<"Threshold would be zero since we have no data, setting it to max"<<endl;
    Threshold = numeric_limits<double>::max() / 100;
  }

  cout<<"Threshold calculation diagnostics based on non-SAA regions: "<<endl;
  cout<<"Average rate in a "<<SuperStrictOffTimeInterval<<"-sec interval: "<<NonSAARateMean<<endl;
  cout<<"Maximum rate: "<<NonSAARateMax<<endl;
  cout<<"RMS: "<<NonSAARateRMS<<" (vs. 1-sigma: "<<sqrt(NonSAARateMean)<<")"<<endl;
  cout<<"--> Threshold rate: "<<Threshold<<endl;

  
  
  
  
  // (C) Determine the regions to reject
  
  // Approach:
  // Assuming we have a threshold of 5 RMS
  // Then we look for single bins with an increase of 5 RMS, two-bins with an increase of 5/sqrt(2), 
  // three bins with an increase of 5/sqrt(3), etc. until we reach our max search distance MaxSearchDistance
  // Since this approach has a tail (e.g. we have one gigantic increase and the two following bins normal, 
  // then a search distance of 3 would catch all three although we do not have an increase in the latter 2),
  // We do it once from the left and once from the right and then "AND" those two together --> no more tails
  // Finally "AND" / mask it with the previously dtermined regions of low-shield rate increases
  
  
  // The maximum amount of adjacent bins we serach for a significant rate increase
  int MaxSearchDistance = 3;   // in caldb
  
  // (a) left to right
  TH1I* OnOffOptimizedRMSLeftToRight = new TH1I(TString("OnOffOptimizedRMSLeftToRight") + iID, TString("OnOffOptimizedRMSLeftToRight") + ID, SuperStrictBins, MinTime, MaxTime);
  for (int b = 1; b <= SuperStrictBins; ++b) OnOffOptimizedRMSLeftToRight->SetBinContent(b, RejectionOff);

  // First seach for one-bin X-rms increases, then two-bin X/sqrt(2) increases, etc.
  for (int s = 1; s <= MaxSearchDistance; ++s) {
    for (int b = 1; b <= OnOffOptimizedRMSLeftToRight->GetNbinsX(); ++b) {
      int ContentBins = 0;
      double Content = 0.0;
      for (int bb = b; bb < b+s && b <= OnOffOptimizedRMSLeftToRight->GetNbinsX(); ++bb) {
        if (EvaluationRate->GetBinContent(bb) > NonSAARateMean) { // stop immediately if we are below mean
          Content += EvaluationRate->GetBinContent(bb);
          ContentBins++;
        } else {
          break; 
        }
      }
      if (ContentBins > 0 && NonSAARateMean > 0) { // NonSAARateMean > 0 means we do have threshold values, i.e. data i the non-SAA region
        if ( Content/ContentBins > NonSAARateRMS * MaxAllowedRMS/sqrt(ContentBins) + NonSAARateMean) {
          // We are above our threshold -> reject
          for (int bb = b; bb < b+ContentBins; ++bb) {
            OnOffOptimizedRMSLeftToRight->SetBinContent(bb, RejectionOn); 
          }
        }
      }
    }   
  }

 
  // (b) Search right to left
  TH1I* OnOffOptimizedRMSRightToLeft = new TH1I(TString("OnOffOptimizedRMSRightToLeft") + iID, TString("OnOffOptimizedRMSRightToLeft") + ID, SuperStrictBins, MinTime, MaxTime);
  for (int b = 1; b <= SuperStrictBins; ++b) OnOffOptimizedRMSRightToLeft->SetBinContent(b, RejectionOff);
  
  for (int s = 1; s <= MaxSearchDistance; ++s) {
    for (int b = OnOffOptimizedRMSRightToLeft->GetNbinsX(); b >= 1; --b) {
      int ContentBins = 0;
      double Content = 0.0;
      for (int bb = b; bb > b-s && b >= 1; --bb) {
        if (EvaluationRate->GetBinContent(bb) > NonSAARateMean) {
          Content += EvaluationRate->GetBinContent(bb);
          ContentBins++;
        } else {
          break; 
        }
      }
      if (ContentBins > 0 && NonSAARateMean > 0) { // NonSAARateMean > 0 means we do have threshold values, i.e. data i the non-SAA region
        if ( Content/ContentBins > NonSAARateRMS * MaxAllowedRMS/sqrt(ContentBins) + NonSAARateMean) {
          for (int bb = b; bb > b-ContentBins; --bb) {
            OnOffOptimizedRMSRightToLeft->SetBinContent(bb, RejectionOn); 
          }
        }
      }
    }   
  }
  
  // (c) "AND" them together: Only when both are ON set the combined to ON
  TH1I* OnOffOptimizedRMS = new TH1I(TString("OnOffOptimizedRMS") + iID, TString("OnOffOptimizedRMS") + ID, SuperStrictBins, MinTime, MaxTime);
  
  for (int b = 1; b <= SuperStrictBins; ++b) {
    if (OnOffOptimizedRMSRightToLeft->GetBinContent(b) == RejectionOn && 
        OnOffOptimizedRMSLeftToRight->GetBinContent(b) == RejectionOn) {
      OnOffOptimizedRMS->SetBinContent(b, RejectionOn);
    } else {
      OnOffOptimizedRMS->SetBinContent(b, RejectionOff);
    }
  }

  DebugOutput(OnOffOptimizedRMS, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_OnOffOptimizedRMS"); 
  DebugOutput(OnOffOptimizedRMSRightToLeft, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_OnOffOptimizedRMSRightToLeft"); 
  DebugOutput(OnOffOptimizedRMSLeftToRight, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_OnOffOptimizedRMSLeftToRight"); 
  
 
  // (d) Make sure we include edge bins in the rejection:
  for (int b = 3; b < OnOffOptimizedRMS->GetNbinsX()-1; ++b) {
    if (OnOffOptimizedRMS->GetBinContent(b) == RejectionOn) {
      if (EvaluationRate->GetBinContent(b-2) == 0) OnOffOptimizedRMS->SetBinContent(b-1, RejectionOn);
      if (EvaluationRate->GetBinContent(b+2) == 0) OnOffOptimizedRMS->SetBinContent(b+1, RejectionOn);
    }
  }
  
  DebugOutput(OnOffOptimizedRMS, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_OnOffOptimizedRMS_NextStep"); 
  
  // (e) Mask it with the low-shield rate cut
  for (int b = 1; b <= OnOffStrictLSR->GetNbinsX(); ++b) {
    if (OnOffStrictLSR->GetBinContent(b) == RejectionOff) {
      OnOffOptimizedRMS->SetBinContent(OnOffOptimizedRMS->FindBin(OnOffStrictLSR->GetBinCenter(b)), RejectionOff);
    }
  }
  
  DebugOutput(OnOffOptimizedRMS, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_OnOffOptimizedRMS_BeforeSanityChecks"); 
  
  // (D) Sanity checks:
  
  // A normal SAA passage should give us regions which are way above the threshold.
  // Thus if the average rate in our region is overall below the threshold, 
  // and we either never exceed the threshold or we have a slightly variable source
  // then we reject the SAA detection
  
  if (PerformSanityChecks == true) {
    double MaxFlux = 0;
    double AverageFlux = 0;
    int NAverageFluxes = 0;
    
    bool Started = false;
    for (int i = 1; i <= OnOffOptimizedRMS->GetNbinsX(); ++i) {
      if (OnOffOptimizedRMS->GetBinContent(i) == RejectionOn) {
        Started = true;
        if (EvaluationRate->GetBinContent(i) > 0) {
          AverageFlux += EvaluationRate->GetBinContent(i);
          NAverageFluxes++;
          if (EvaluationRate->GetBinContent(i) > MaxFlux) {
            MaxFlux = EvaluationRate->GetBinContent(i);
          }
        }
      } else {
        if (Started == true) {
          Started = false;
        }
      }
    }
    if (NAverageFluxes > 0) {
      AverageFlux /= NAverageFluxes;
      
      int Suspiciousness = 0;
      
      // Usually the average flux is way high 
      double HintOfVariabilityCutOff = 1.4;                        // <-- into caldb 
      if (AverageFlux < Threshold && MaxFlux <= (MaxAllowedRMS+2.0)*NonSAARateRMS + NonSAARateMean) {
        cout<<"Suspicious: The average flux value ("<<AverageFlux<<" vs. max: "<<MaxFlux<<") is smaller than the threshold ("<<Threshold<<")"<<endl;
        Suspiciousness++;
        if (MaxFlux < Threshold) {
          cout<<"            and the maximum flux value ("<<MaxFlux<<") is smaller than the threshold ("<<Threshold<<")"<<endl;
          Suspiciousness++;
        }
        if (NonSAARateRMS > HintOfVariabilityCutOff*NonSAARateSigma) {
          cout<<"            and we have a hint of a variable source"<<endl;
          ++Suspiciousness;
        }
      }
      
      // More than 2 suspicousness points? No SAA!
      if (Suspiciousness >= 2) {
        cout<<"Attention: The characteristics of the given SAA region is not in agreement with a typical one."<<endl;
        cout<<"           I ignore them all!"<<endl;
        for (int b = 1; b <= SuperStrictBins; ++b) OnOffOptimizedRMS->SetBinContent(b, RejectionOff);
      }
    }
  }
  
  DebugOutput(OnOffOptimizedRMS, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_OnOffOptimizedRMSFinal"); 
  
  // (E) Now set it (slow...):
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    int Bin = OnOffOptimizedRMS->FindBin(H.m_Time[h]);

    if (Bin == 0 || Bin > OnOffOptimizedRMS->GetNbinsX()) {
      cerr<<"Something is wrong with the times as I cannot find a rejection value for time "<<H.m_Time[h]<<endl; 
      if (Mode == c_SAACutOptimizedLSRRMS) H.m_SoftSAA[h] = true;
      H.m_SoftSAAOptimizedLSRRMS[h] = false;        
    } else {
      if (OnOffOptimizedRMS->GetBinContent(Bin) == RejectionOn) {
        if (Mode == c_SAACutOptimizedLSRRMS) H.m_SoftSAA[h] = true;
        H.m_SoftSAAOptimizedLSRRMS[h] = true;
      } else {
        if (Mode == c_SAACutOptimizedLSRRMS) H.m_SoftSAA[h] = true;
        H.m_SoftSAAOptimizedLSRRMS[h] = false;        
      }    
    }
  }
  
  if (m_Debug == true) {
    ofstream out;
    out.open(TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_SoftSAA_OPTIMIZED.txt");
    for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
      out<<"Time["<<h<<"] "<<setprecision(10)<<H.m_Time[h]<<"   SoftSAA["<<h<<"] "<<H.m_SoftSAAOptimizedLSRRMS[h]<<endl;
    }
    out.close();
  }

  // (F) Show the result if desired
  if (Show == true) {
    TH1D* FinalOptimizedRate = new TH1D(TString("FinalOptimizedRate") + iID, TString("SAA/RMS: FinalOptimizedRate") + ID, SuperStrictBins, MinTime, MaxTime);
    FinalOptimizedRate->SetXTitle("Time [sec since 1/1/2010]");
    FinalOptimizedRate->SetYTitle("cts/sec");
    
    TH1D* FinalSAAOnOff = new TH1D(TString("FinalSAAOnOff") + iID, TString("SAA/RMS: FinalSAAOnOff") + ID, OneSecBins, MinTime, MaxTime);
    FinalSAAOnOff->SetXTitle("Time [sec since 1/1/2010]");
    FinalSAAOnOff->SetYTitle("on = 1");
    
    for (unsigned int i = 0; i < F.m_Time.size(); ++i) {
      if (F.IsGTI(F.m_Time[i]) == false) continue;
      if (F.m_Energy[i] < m_SpectrumMin || F.m_Energy[i] > m_SpectrumMax) continue;
      
      int HKIndex = H.FindClosestIndex(F.m_Time[i]);
      if (HKIndex == -1) {
        cout<<"Housekeeping: Index not found for time "<<F.m_Time[i]<<"..."<<endl;
        continue;      
      }
      if (H.m_SoftSAAOptimizedLSRRMS[HKIndex] == true) {
        FinalSAAOnOff->SetBinContent(FinalSAAOnOff->FindBin(F.m_Time[i]), 1.0);
        continue;
      }
      
      int DetectorID = F.m_DetectorID[i];
      int RawX = F.m_RawX[i];
      int RawY = F.m_RawY[i];
      int ID = 10000*DetectorID + 100*RawX + RawY;
      vector<int>::iterator I = lower_bound(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end(), 10000*DetectorID + 100*RawX + RawY);
      if (I != ExcludedDetRawXY.end() && (*I) == ID) continue;
      
      FinalOptimizedRate->Fill(F.m_Time[i]);
    }
    for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
      if (H.m_SoftSAAOptimizedLSRRMS[h] == true) {
        FinalSAAOnOff->SetBinContent(h, 1.0);
      }      
    }
    
    DebugOutput(FinalSAAOnOff, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_FinalSAAOnOff"); 
 
    TCanvas* FinalSAAOnOffCanvas = new TCanvas();
    FinalSAAOnOffCanvas->cd();
    FinalSAAOnOff->Draw();
    FinalSAAOnOffCanvas->Update();
 
    TCanvas* NonSAARateCanvas = new TCanvas();
    NonSAARateCanvas->cd();
    NonSAARate->Draw();
    NonSAARateCanvas->Update();

    TCanvas* NonSAARateHistogramCanvas = new TCanvas();
    NonSAARateHistogramCanvas->cd();
    NonSAARateHistogram->Draw();
    NonSAARateHistogramCanvas->Update();   
    
 
    TCanvas* EvaluationRateCanvas = new TCanvas();
    EvaluationRateCanvas->cd();
    EvaluationRate->Draw();
    OnOffOptimizedRMS->Scale(EvaluationRate->GetMaximum());
    OnOffOptimizedRMS->SetLineColor(kYellow);
    OnOffOptimizedRMS->SetFillColor(kYellow);
    OnOffOptimizedRMS->Draw("SAME");
    EvaluationRate->Draw("SAME");
    EvaluationRateCanvas->Update();

    TCanvas* FinalOptimizedRateCanvas = new TCanvas();
    FinalOptimizedRateCanvas->cd();
    FinalOptimizedRate->Draw();
    FinalOptimizedRateCanvas->Update();
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::FindSAATentacleFoM(NFilteredEvents& FE, NHousekeeping& HK, NOrbits& O, int Mode, bool Show) 
{
  // For each Orbits check for each module the count rate in the tentacle region - 
  // if there is an increase flag the region
  
  cout<<endl<<"Finding Tentacle --- figure-of-merit based approach..."<<endl;
  
  if (HK.m_Time.size() == 0) {
    cout<<"Something went wrong! No data!"<<endl;
    return true;
  }
  
  int RejectionOn  = 0;
  int RejectionOff = 1;
  
  // First define the time intervals in the tentacle region:
  int Bins = HK.GetMaxTime() - HK.GetMinTime() + 1;
  double MinTime = HK.GetMinTime()-0.5;
  double MaxTime = HK.GetMaxTime()+0.5;
  
  TString iID = TString("_ID_") + HK.m_ID + "_M_" + ((HK.m_Module == 0) ? "A" : "B");
  TString ID = TString(" (ID: ") + HK.m_ID + ((HK.m_Module == 0) ? "A" : "B") + ")";
  
  TH1D* LifeTimes = new TH1D(TString("TentaclesFoMLifeTimes") + iID, TString("TentaclesFoMLifeTimes") + ID, Bins, MinTime, MaxTime);
  
  TH1D* FilteredRate = new TH1D(TString("TentaclesFoMFilteredRate") + iID, TString("TentaclesFoMFilteredRate") + ID, Bins, MinTime, MaxTime);
  FilteredRate->SetXTitle("Time [sec since 1/1/2010]");
  FilteredRate->SetYTitle("cts/sec");
  
  TH1D* CleanRate = new TH1D(TString("TentaclesFoMCleanRate") + iID, TString("TentaclesFoMCleanRate") + ID, Bins, MinTime, MaxTime);
  CleanRate->SetXTitle("Time [sec since 1/1/2010]");
  CleanRate->SetYTitle("cts/sec");
  
  TH1D* FilteredRateCut = new TH1D(TString("TentaclesFoMFilteredRateCut") + iID, TString("TentaclesFoMFilteredRateCut") + ID, Bins, MinTime, MaxTime);
  FilteredRateCut->SetLineColor(kRed);
  
  TH1D* FilteredRateAverageLarge = new TH1D(TString("TentaclesFoMFilteredRateAverageLarge") + iID, TString("TentaclesFoMFilteredRateAverageLarge") + ID, Bins, MinTime, MaxTime);
  FilteredRateAverageLarge->SetLineColor(kGreen);
  TH1D* FilteredRateAverageSmall = new TH1D(TString("TentaclesFoMFilteredRateAverageSmall") + iID, TString("TentaclesFoMFilteredRateAverageSmall") + ID, Bins, MinTime, MaxTime);
  FilteredRateAverageSmall->SetLineColor(kYellow);
  
  TH1I* OnOff = new TH1I(TString("TentaclesFoMOnOff") + iID, TString("TentaclesFoMOnOff") + ID, Bins, MinTime, MaxTime);
  
  
  double LiveTimeNoCuts = 0;
  double LiveTimeWithCuts = 0;
  
  
  // Fill:
  for (unsigned int i = 0; i < FE.m_Time.size(); ++i) {
    FilteredRate->Fill(FE.m_Time[i]);
  }
  if (FilteredRate->Integral() == 0) {
    cout<<"Something went wrong! No data!"<<endl;
    return true;
  }
    
  
  for (unsigned int i = 0; i < HK.m_Time.size(); ++i) {
    LifeTimes->Fill(HK.m_Time[i], HK.m_LiveTime[i]);
  }
  
  for (int b = 1; b <= FilteredRate->GetNbinsX(); ++b) {
    OnOff->SetBinContent(b, RejectionOff);
    // Find the housekeeping position
    double Time = FilteredRate->GetBinCenter(b);
    int HKIndex = HK.FindClosestIndex(Time);
    if (HKIndex == -1) {
      cerr<<"No suiting housekeeping found!"<<endl;  
      continue;
    }
    if (fabs(HK.m_Time[HKIndex] - Time) > 1) {
      cerr<<"Housekeeping is "<<fabs(HK.m_Time[HKIndex] - Time)<<" seconds from data"<<endl;
      continue;
    }
    
    if (LifeTimes->GetBinContent(b) > 0 /* && HK.m_SoftSAA[HKIndex] == false */) {
      // No live time correction! FilteredRate->SetBinContent(b, FilteredRate->GetBinContent(b)/LifeTimes->GetBinContent(b)); 
      FilteredRateCut->SetBinContent(b, FilteredRate->GetBinContent(b)); 
    } else {
      FilteredRate->SetBinContent(b, 0);       
      FilteredRateCut->SetBinContent(b, FilteredRate->GetBinContent(b)); 
    }
    if (FE.IsGTI(FilteredRate->GetBinCenter(b)) == true) LiveTimeNoCuts += LifeTimes->GetBinContent(b);
  } 
  
  int NBins = 0;
  double GlobalAverage = 0.0;
  for (int b = 1; b <= FilteredRate->GetNbinsX(); ++b) {
    if (FilteredRate->GetBinContent(b) > 0) {
      ++NBins;
      GlobalAverage += FilteredRate->GetBinContent(b);
    }
  }
  if (NBins > 0) GlobalAverage /= NBins;
  
  
  // Create average again:
  int FilteredEventsHalfAverageLarge = 60;
  
  double Average;
  for (int i = 1; i <= FilteredRate->GetNbinsX(); ++i) {
    Average = 0;
    NBins = 0;
    
    // Determine the interval over which we average dynamically to take care of the edges
    int Min = i-FilteredEventsHalfAverageLarge;
    if (Min < 1) Min = 1;
    int Max = i+FilteredEventsHalfAverageLarge;
    if (Max > FilteredRate->GetNbinsX()) Max = FilteredRate->GetNbinsX();
    
    for (int a = Min; a <= Max; ++a) {
      // We need to jump over bins where we don't have HK data
      if (FilteredRate->GetBinContent(a) > 0) { 
        ++NBins;
        Average += FilteredRate->GetBinContent(a);
      }
    }
    Average /= NBins;
    
    FilteredRateAverageLarge->SetBinContent(i, Average);
  }
  int FilteredEventsHalfAverageSmall = 40;
  for (int i = 1; i <= FilteredRate->GetNbinsX(); ++i) {
    Average = 0;
    NBins = 0;
    
    // Determine the interval over which we average dynamically to take care of the edges
    int Min = i-FilteredEventsHalfAverageSmall;
    if (Min < 1) Min = 1;
    int Max = i+FilteredEventsHalfAverageSmall;
    if (Max > FilteredRate->GetNbinsX()) Max = FilteredRate->GetNbinsX();
    
    for (int a = Min; a <= Max; ++a) {
      // We need to jump over bins where we don't have data 
      if (FilteredRate->GetBinContent(a) > 0) { 
        ++NBins;
        Average += FilteredRate->GetBinContent(a);
      }
    }
    Average /= NBins;
    
    FilteredRateAverageSmall->SetBinContent(i, Average);
  }
  
  
  // This is the distance between two data points on which we determine that we have a significant increase 
  int DecisionDistance = 2*FilteredEventsHalfAverageLarge; // 2 for 30 and 60
  // This is the threshold of increase we require for starting to skip
  // There is a slight difference between left and right to account for delayed activation
  double DecisionThresholdLeft = 1.25; // good: 30+1.15, 60+1.35
  double DecisionThresholdRight = 0.95*DecisionThresholdLeft; // good: 30+1.15+0.95, 60+1.3+0.95
  
  
  // We also store the shield-rate values when we started skipping from the left (lower time values) and the right (higher time values) 
  double EnteringValueLeft = 0;
  double EnteringValueRight = 0;
  for (int i = 1; i <= FilteredRateAverageLarge->GetNbinsX(); ++i) {
    // We always make a decision from the left and from the rights
    
    // Here we take care of the boarders of the distribution
    int DecisionPointLeft = i-DecisionDistance;
    if (DecisionPointLeft < 0) DecisionPointLeft = 1;
    int DecisionPointRight = i+DecisionDistance;
    if (DecisionPointRight >= int(HK.m_Time.size())) DecisionPointRight = FilteredRateAverageLarge->GetNbinsX();
    
    // We cut this bin if:
    
    bool Cut = false;
    // ... we have an increase above our decision threshold from left to right
    if (FilteredRateAverageLarge->GetBinContent(DecisionPointLeft) > 1 && 
      FilteredRateAverageLarge->GetBinContent(i) > DecisionThresholdLeft*FilteredRateAverageLarge->GetBinContent(DecisionPointLeft)) {
      Cut = true;
    }
    // ... we have an increase above our decision threshold from right to left
    if (FilteredRateAverageLarge->GetBinContent(DecisionPointRight) > 1 &&
      FilteredRateAverageLarge->GetBinContent(i) > DecisionThresholdRight*FilteredRateAverageLarge->GetBinContent(DecisionPointRight)) {
      Cut = true;
    }
        
    // Next we take care of the case if the increase is no longer above the decision threshold,
    // but we are still above the initial increase value:
        
    // If we have both a left and a right value we cut if we are still above the average values
    if (EnteringValueLeft > 0 && EnteringValueRight  > 0) {
      if (FilteredRateAverageLarge->GetBinContent(i) > 0.5*(EnteringValueLeft+EnteringValueRight)) {
        Cut = true;
      }
    } 
    // If we have just one value, we only cut on it
    else {
      if ((FilteredRateAverageLarge->GetBinContent(i) > EnteringValueLeft && EnteringValueLeft > 0) ||
          (FilteredRateAverageLarge->GetBinContent(i) > EnteringValueRight && EnteringValueRight  > 0)) {
        Cut = true;
      }
    }
        
        
    if (Cut == true) {
      OnOff->SetBinContent(i, RejectionOn);
      // Update the entering values
      if (FilteredRateAverageLarge->GetBinContent(i) > DecisionThresholdLeft*FilteredRateAverageLarge->GetBinContent(DecisionPointLeft) &&
          (EnteringValueLeft == 0 || FilteredRateAverageLarge->GetBinContent(i) < EnteringValueLeft)) { 
        EnteringValueLeft = FilteredRateAverageLarge->GetBinContent(i);
      }
      if (FilteredRateAverageLarge->GetBinContent(i) > DecisionThresholdRight*FilteredRateAverageLarge->GetBinContent(DecisionPointRight) &&
          (EnteringValueRight == 0 || FilteredRateAverageLarge->GetBinContent(i) < EnteringValueRight)) {
        EnteringValueRight = FilteredRateAverageLarge->GetBinContent(i);
      }
    } else {
      EnteringValueLeft = 0;
      EnteringValueRight = 0;
    }
  }

  
  // Filters:
  bool Previous = RejectionOff;  
  
  // (A) If it starts or ends outside the tentacle region filter out:
  double LongitudeMin = 230;
  double LongitudeMax = 320;
  Previous = RejectionOff;
  for (int b = 1; b <= OnOff->GetNbinsX(); ++b) {
    if (OnOff->GetBinContent(b) == RejectionOn) {
      // Find the Orbits position
      int OrbitsIndex = O.FindClosestIndex(OnOff->GetBinCenter(b));
      if (OrbitsIndex == -1) {
        cerr<<"No suiting Orbits found!"<<endl;  
        continue;
      }
      if (fabs(O.m_Time[OrbitsIndex] - OnOff->GetBinCenter(b)) > 1) {
        cerr<<"Orbits is "<<fabs(O.m_Time[OrbitsIndex] - OnOff->GetBinCenter(b))<<" seconds from data"<<endl;
        continue;
      }
      if (O.m_Longitude[OrbitsIndex] < LongitudeMin || O.m_Longitude[OrbitsIndex] > LongitudeMax) {
        OnOff->SetBinContent(b, RejectionOff);
      }
    }
  }
  
  // (B) The average off must be at least X percent above average value: 
  double AboveGlobalAverageThreshold = 1.25;
  Previous = RejectionOff;
  
  for (int b = 1; b <= OnOff->GetNbinsX(); ++b) {
    // Switch on -> off
    if (OnOff->GetBinContent(b) == RejectionOn && Previous == RejectionOff) {
      Previous = RejectionOn;
      double LocalAverage = 0.0;
      NBins = 0;
      int l = b;
      do {
        if (FilteredRate->GetBinContent(l) > 0) {
          LocalAverage += FilteredRate->GetBinContent(l);
          NBins++;
        }
        ++l;
      } while (l < OnOff->GetNbinsX() && OnOff->GetBinContent(l) == RejectionOn);
      if (NBins > 0) LocalAverage /= NBins;
      if (LocalAverage < AboveGlobalAverageThreshold*GlobalAverage) {
        do {
          OnOff->SetBinContent(b, RejectionOff);
          ++b;
        } while (b < OnOff->GetNbinsX() && OnOff->GetBinContent(b) == RejectionOn);
      }
    }
    // Switch on -> off
    else if (OnOff->GetBinContent(b) == RejectionOff && Previous == RejectionOn) {
      Previous = RejectionOff;
    }
  }
  
  // (C) Cutoffs require a certain length to be reasonable 
  int CutOffCount = 3*FilteredEventsHalfAverageLarge; // good: 2x, 3x
  
  int OffCount = 0;
  for (int i = 1; i <= FilteredRate->GetNbinsX(); ++i) {
    if (OnOff->GetBinContent(i) == 1) {
      if (OffCount > 0 && OffCount < CutOffCount) {
        for (int b = i-1; b >= i-OffCount; --b) {
          if (OnOff->GetBinContent(b) == 0) {
            OnOff->SetBinContent(b, RejectionOff);
          } else {
            cerr<<"On remove error..."<<endl;
          }
        }
      }
      OffCount = 0;
    } else {
      OffCount++;
    }
  }
  
  
  for (int b = 1; b <= OnOff->GetNbinsX(); ++b) {
    if (OnOff->GetBinContent(b) == RejectionOff) {
      CleanRate->SetBinContent(b, FilteredRate->GetBinContent(b));
      FilteredRateCut->SetBinContent(b, 0);
    }
  }
  
  if (Show == true) {
    TCanvas* FilteredRateCanvas = new TCanvas();
    FilteredRateCanvas->cd();
    FilteredRate->Draw();
    FilteredRateCut->Draw("SAME");
    FilteredRateAverageLarge->Draw("SAME");
    FilteredRateAverageSmall->Draw("SAME");
    FilteredRateCanvas->Update();
    
    TCanvas* CleanRateCanvas = new TCanvas();
    CleanRateCanvas->cd();
    CleanRate->Draw();
    CleanRateCanvas->Update();
  }
  
  for (int b = 1; b <= OnOff->GetNbinsX(); ++b) {
    int Index = HK.FindClosestIndex(OnOff->GetBinCenter(b));
    if (Index == -1) continue; 
    if (OnOff->GetBinCenter(b) - HK.m_Time[Index] < 1.0) {
      if (OnOff->GetBinContent(b) == RejectionOn) {
        if (Mode == c_TentacleCutFoM) HK.m_SoftTentacled[Index] = true;
        HK.m_SoftTentacledFoM[Index] = true;
      } else {
        if (Mode == c_TentacleCutFoM) HK.m_SoftTentacled[Index] = false;        
        HK.m_SoftTentacledFoM[Index] = false;        
      }
    } else {
      //cerr<<"Something is wrong with the times as I cannot find housekeeping data... "<<OnOff->GetBinCenter(b)<<endl; 
    }
  }
  
  
  // Just as sanity check make an Orbits plot of the rates
  int LongitudeBins = 48;
  int LatitudeBins = 12;
  double MinLatitude = -6.06;
  double MaxLatitude = +6.06;
  
  double LatMin = 100;
  double LatMax = -100;
  
  TH2D* OrbitsNormalizer = new TH2D(TString("TentaclesOrbitsNormalizer") + iID, 
                                    TString("TentaclesOrbitsNormalizer") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  
  TH2D* Orbits = new TH2D(TString("TentaclesOrbits") + iID, 
                          TString("TentaclesOrbits") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  Orbits->SetXTitle("Longitude [deg]");
  Orbits->SetYTitle("Latitude [deg]");
  Orbits->SetZTitle("Events [cts/sec]");
  
  
  TH1D* RateNormalizer = new TH1D(TString("RatesTentacleNormalizer") + iID, TString("RatesTentacleNormalizer") + ID, Bins, MinTime, MaxTime);
  TH1D* Rates = new TH1D(TString("RatesTentacle") + iID, TString("RatesTentacle") + ID, Bins, MinTime, MaxTime);
  Rates->SetXTitle("Time [s]");
  Rates->SetYTitle("cts/sec");
  
  
  if (Show == true) {
    for (unsigned int u = 0; u < FE.m_Time.size(); ++u) {
      double Time = FE.m_Time[u];
      if (WithinSpecialGTI(Time) == false) continue;
      
      // Find the Orbits position
      int OrbitsIndex = O.FindClosestIndex(Time);
      if (OrbitsIndex == -1) {
        cerr<<"No suiting Orbits found!"<<endl;  
        continue;
      }
      if (fabs(O.m_Time[OrbitsIndex] - Time) > 1) {
        cerr<<"Orbits is "<<fabs(O.m_Time[OrbitsIndex] - Time)<<" seconds from data"<<endl;
        continue;
      }
      // Find the housekeeping position
      int HKIndex = HK.FindClosestIndex(Time);
      if (HKIndex == -1) {
        cerr<<"No suiting housekeeping found!"<<endl;  
        continue;
      }
      if (fabs(HK.m_Time[HKIndex] - Time) > 1) {
        cerr<<"Housekeeping is "<<fabs(HK.m_Time[HKIndex] - Time)<<" seconds from data"<<endl;
        continue;
      }
      if (HK.m_SoftSAA[HKIndex] == true) continue;
      //if (HK.m_SoftTentacled[HKIndex] == true) continue;
      
      Orbits->Fill(O.m_Longitude[OrbitsIndex], O.m_Latitude[OrbitsIndex]);
      Rates->Fill(FE.m_Time[u]);
      
      if (O.m_Latitude[OrbitsIndex] > LatMax) LatMax = O.m_Latitude[OrbitsIndex];
      if (O.m_Latitude[OrbitsIndex] < LatMin) LatMin = O.m_Latitude[OrbitsIndex];
    }
    
    LiveTimeWithCuts = 0;
    LiveTimeNoCuts = 0;
    for (unsigned int h = 0; h < HK.m_Time.size(); ++h) {
      if (WithinSpecialGTI(HK.m_Time[h]) == false) continue;
      
      if (FE.IsGTI(HK.m_Time[h]) == true && HK.m_SoftSAA[h] == false) {
        int OrbitsIndex = O.FindClosestIndex(HK.m_Time[h]);
        if (OrbitsIndex == -1) {
          cerr<<"No suiting Orbits found!"<<endl;  
          continue;
        }
        if (fabs(O.m_Time[OrbitsIndex] - HK.m_Time[h]) > 1) {
          cerr<<"Orbits is "<<fabs(O.m_Time[OrbitsIndex] - HK.m_Time[h])<<" seconds from data"<<endl;
          continue;
        }
        //if (HK.m_SoftTentacled[h] == false) {
        LiveTimeWithCuts += HK.m_LiveTime[h];
        OrbitsNormalizer->Fill(O.m_Longitude[OrbitsIndex], O.m_Latitude[OrbitsIndex], HK.m_LiveTime[h]);
        RateNormalizer->Fill(HK.m_Time[h], HK.m_LiveTime[h]);
        //}
        LiveTimeNoCuts += HK.m_LiveTime[h];          
      }
    }
    //cout<<"Live time before cuts: "<<LiveTimeNoCuts<<endl;
    //cout<<"Live time after cuts:  "<<LiveTimeWithCuts<<" ("<<100.0*(LiveTimeNoCuts-LiveTimeWithCuts)/LiveTimeNoCuts<<"%)"<<endl;
    
    
    TCanvas* OrbitsCanvas = new TCanvas();
    OrbitsCanvas->cd();
    for (int bx = 1; bx <= Orbits->GetNbinsX(); ++bx) {
      for (int by = 1; by <= Orbits->GetNbinsY(); ++by) {
        if (OrbitsNormalizer->GetBinContent(bx, by) > 0) {
          Orbits->SetBinContent(bx, by, Orbits->GetBinContent(bx, by)/OrbitsNormalizer->GetBinContent(bx, by));
        }
      }
    }
    Orbits->Draw("colz");
    OrbitsCanvas->Update();
    
    TCanvas* RatesCanvas = new TCanvas();
    RatesCanvas->cd();
    for (int bx = 1; bx <= Rates->GetNbinsX(); ++bx) {
      if (RateNormalizer->GetBinContent(bx) > 0) {
        Rates->SetBinContent(bx, Rates->GetBinContent(bx)/RateNormalizer->GetBinContent(bx));
      }
    }
    Rates->Draw();
    RatesCanvas->Update();
  }
  
  cout<<"Done with tentacle flag identification - figure-of-merit-based approach"<<endl;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::FindSAATentacleRMS(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, int Mode, bool Show) 
{
  // For each Orbits check for each module the count rate in the tentacle region - 
  // if there is an increase flag the region
  
  cout<<endl;
  cout<<"Finding Tentacle via RMS ..."<<endl;
  
  if (H.m_Time.size() == 0) {
    cout<<"Something went wrong! No data!"<<endl;
    return true;
  }
  
  // (A) Initializations...
 
  double MaxAllowedRMS = m_TentacleCutRMSThreshold;               // --> user adjustable
  bool DoSourceElimination = m_TentacleCutRMSSourceElimination;   // --> user adjustable
  bool DoRegionRestriction = m_TentacleCutRMSRegionRestriction;   // --> user adjustable
  bool DoSanityChecks = m_TentacleCutRMSSanityChecks;             // --> user adjustable

  // The rejection flags
  int RejectionOn  = 0;
  int RejectionOff = 1;
  
  // The region where we determine the base count rate (mean & rms)
  double NonSAAMinLong = 180;               // <-- into caldb 
  double NonSAAMaxLong = 250;               // <-- into caldb 
  
  // First define the time intervals in the tentacle region:
  int OneSecondBins = H.GetMaxTime() - H.GetMinTime() + 1;
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;
  
  // 60-second binning -- good compromise for having enough statistics per bin, and small enough bins for not cutting out too much 
  int TimeInterval = 60;                 // <-- into caldb (everything was optimized towards 60 seconds, thus better not change it tto much) 
  int EvaluationBins = OneSecondBins/TimeInterval;  // Almost 60 second bins (but not perfectly)
  
  // Create a list of pixels with high source count for elimination
  vector<int> ExcludedDetRawXY;
  if (DoSourceElimination == true) ExcludedDetRawXY = MostlyEliminateSource(F, O, Show);  

  TString iID = TString("_ID_") + H.m_ID + "_M_" + ((H.m_Module == 0) ? "A" : "B");
  TString ID = TString(" (ID: ") + H.m_ID + ((H.m_Module == 0) ? "A" : "B") + ")";


  
  // (B) Create and fill the histograms
  
  TH1D* EvaluationRate = new TH1D(TString("EvaluationRate") + iID, TString("Tentacle - EvaluationRate") + ID, EvaluationBins, MinTime, MaxTime);
  EvaluationRate->SetXTitle("Time [sec since 1/1/2010]");
  EvaluationRate->SetYTitle("cts/sec");
  
  TH1D* NonSAAComparisonRate = new TH1D(TString("NonSAAComparisonRate") + iID, TString("Tentacle - NonSAAComparisonRate") + ID, EvaluationBins, MinTime, MaxTime);
  NonSAAComparisonRate->SetXTitle("Time [sec since 1/1/2010]");
  NonSAAComparisonRate->SetYTitle("cts/sec");
  
  TH1I* OnOffInternalSAA = new TH1I(TString("OnOffInternalSAA") + iID, TString("Tentacle - OnOffInternalSAA") + ID, EvaluationBins, MinTime, MaxTime);
  for (int b = 1; b <= EvaluationBins; ++b) OnOffInternalSAA->SetBinContent(b, RejectionOff);
  
  ofstream deb;
  if (m_Debug == true) deb.open(TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_TentacleEventLog");

  for (unsigned int i = 0; i < F.m_Time.size(); ++i) {
    if (WithinSpecialGTI(F.m_Time[i]) == false) {
      if (m_Debug == true) deb<<"ID "<<i<<" --> special GTI reject"<<endl;
      continue;
    }
    if (F.IsGTI(F.m_Time[i]) == false) {
      if (m_Debug == true) deb<<"ID "<<i<<" --> GTI reject"<<endl;
      continue;
    }
    if (F.m_Energy[i] < m_SpectrumMin || F.m_Energy[i] > m_SpectrumMax) {
      if (m_Debug == true) deb<<"ID "<<i<<" --> energy reject"<<endl;
      continue;
    }
    if (O.IsGoodOrbit(F.m_Time[i]) == false) {
      if (m_Debug == true) deb<<"ID "<<i<<" --> orbit reject, index: "<<O.FindOrbitIndex(F.m_Time[i])<<endl;
      continue;
    }
    
    int DetectorID = F.m_DetectorID[i];
    int RawX = F.m_RawX[i];
    int RawY = F.m_RawY[i];
    int ID = 10000*DetectorID + 100*RawX + RawY;
    vector<int>::iterator I = lower_bound(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end(), 10000*DetectorID + 100*RawX + RawY);
    if (I != ExcludedDetRawXY.end() && (*I) == ID) {
      deb<<"ID "<<i<<" --> source pos reject"<<endl;
      continue;
    }
    
    int HKIndex = H.FindClosestIndex(F.m_Time[i]);
    if (HKIndex < 0) continue;
    
    double LifeTime = H.m_LiveTime[HKIndex];
    if (m_DoLifeTimeCorrection == false) LifeTime = 1.0;
    if (LifeTime > 0) {
      EvaluationRate->Fill(F.m_Time[i], 1.0/LifeTime);
    }
    if (m_Debug == true) deb<<"ID "<<i<<" time: "<<setprecision(10)<<F.m_Time[i]<<" bin: "<<EvaluationRate->FindBin(F.m_Time[i])-1<<" content "<<EvaluationRate->GetBinContent(EvaluationRate->FindBin(F.m_Time[i]))<<" --> Good"<<endl;
    
    int Index = O.FindClosestIndex(F.m_Time[i]);
    if (O.m_Longitude[Index] >= NonSAAMinLong && O.m_Longitude[Index] <= NonSAAMaxLong) {
      if (LifeTime > 0) {
        NonSAAComparisonRate->Fill(F.m_Time[i], 1.0/LifeTime);
      }
    }    
  }

  if (m_Debug == true) deb.close();
  DebugOutput(EvaluationRate, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_TentacleEvaluationRate"); 
  
  
  for (unsigned int i = 0; i < H.m_Time.size(); ++i) {
    if (H.m_HardSAA[i] == true) {
      OnOffInternalSAA->SetBinContent(OnOffInternalSAA->FindBin(H.m_Time[i]), RejectionOn);
    }
  }

  
  // (C) Create a rate histogram in order to be able to calculate mean and the rms of the rate
  
  // (a) Create a rate histogram
  int NonSAARateHistogramNumberOfEntries = 0;
  TH1D* NonSAARateHistogram = new TH1D(TString("NonSAARateHistogram") + iID, TString("NonSAARateHistogram") + ID, 1000, 0, NonSAAComparisonRate->GetMaximum() + 1);
  for (int b = 2; b < NonSAAComparisonRate->GetNbinsX(); ++b) {
    // Avoid partially filled edges:
    if (NonSAAComparisonRate->GetBinContent(b-1) > 0 &&
        NonSAAComparisonRate->GetBinContent(b) > 0 &&
        NonSAAComparisonRate->GetBinContent(b+1) > 0) {
      NonSAARateHistogram->Fill(NonSAAComparisonRate->GetBinContent(b));
      ++NonSAARateHistogramNumberOfEntries;
    }
  }
  
  // (b) Calculate Average and RMS
  double NonSAARateMax = NonSAAComparisonRate->GetMaximum();
  double NonSAARateMean = NonSAARateHistogram->GetMean();
  double NonSAARateRMS = NonSAARateHistogram->GetRMS();
  double NonSAARateSigma = sqrt(NonSAARateMean);
  
  double Threshold = MaxAllowedRMS*NonSAARateRMS + NonSAARateMean;

  
  // Take care of the rare case when we have observations with strong outbursts
  // Normal outburst should increase the RMS thus we are automatically in a higher threshold regime
  // Then we set the threshold to the maximum + 1 rms
  // There is only one case where this ever happened "RapidBurster"
  
  // If the rms is more than 3 times the expected 1-sigma value, then we have a burster 
  // Thus set the threshold to maximum + MaxAllowedRMS * rms
  double TimeOneSigma = 3.0;
  if (NonSAARateRMS > TimeOneSigma * NonSAARateSigma) {
    Threshold = NonSAARateMax + MaxAllowedRMS*NonSAARateRMS;
    cout<<"--> Bursts detected! Threshold is max + "<<MaxAllowedRMS<<"*rms"<<endl;
  }
  
  // When we have no data in the NonSAA region our threshold will be zero,
  // Set it ti the maximum
  if (Threshold == 0) {
    cout<<"Threshold would be zero since we have no data, setting it to max"<<endl;
    Threshold = numeric_limits<double>::max() / 100;
  }
  
  cout<<"Threshold calculation diagnostics based on non-SAA regions: "<<endl;
  cout<<"Average rate in a "<<TimeInterval<<"-sec interval: "<<NonSAARateMean<<endl;
  cout<<"Maximum rate: "<<NonSAARateMax<<endl;
  cout<<"RMS: "<<NonSAARateRMS<<" (vs. 1-sigma: "<<sqrt(NonSAARateMean)<<")"<<endl;
  cout<<"--> Threshold rate: "<<Threshold<<endl;
  
  
  // (D) Determine the regions to reject
  
  // Approach:
  // Assuming we have a threshold of 5 RMS
  // Then we look for single bins with an increase of 5 RMS, two-bins with an increase of 5/sqrt(2), 
  // three bins with an increase of 5/sqrt(3), etc. until we reach our max search distance MaxSearchDistance
  // Since this approach has a tail (e.g. we have one gigantic increase and the two following bins normal, 
  // then a search distance of 3 would catch all three although we do not have an increase in the latter 2),
  // We do it once from the left and once from the right and then "AND" those two together --> no more tails
  // Finally "AND" / mask it with the previously dtermined regions of low-shield rate increases
  
  
  // The maximum amount of adjacent bins we serach for a significant rate increase
  int MaxSearchDistance = 3;   // in caldb
  
  // (a) left to right
  TH1I* OnOffLeftToRight = new TH1I(TString("OnOffLeftToRight") + iID, TString("OnOffLeftToRight") + ID, EvaluationBins, MinTime, MaxTime);
  for (int b = 1; b <= EvaluationBins; ++b) OnOffLeftToRight->SetBinContent(b, RejectionOff);

  // First seach for one-bin X-rms increases, then two-bin X/sqrt(2) increases, etc.
  for (int s = 1; s <= MaxSearchDistance; ++s) {
    for (int b = 1; b <= OnOffLeftToRight->GetNbinsX(); ++b) {
      int ContentBins = 0;
      double Content = 0.0;
      for (int bb = b; bb < b+s && b <= OnOffLeftToRight->GetNbinsX(); ++bb) {
        if (EvaluationRate->GetBinContent(bb) > NonSAARateMean) { // stop immediately if we are below mean
          Content += EvaluationRate->GetBinContent(bb);
          ContentBins++;
        } else {
          break; 
        }
      }
      if (ContentBins > 0 && NonSAARateMean > 0) { // NonSAARateMean > 0 means we do have threshold values, i.e. data i the non-SAA region
        if ( Content/ContentBins > NonSAARateRMS * MaxAllowedRMS/sqrt(ContentBins) + NonSAARateMean) {
          // We are above our threshold -> reject
          for (int bb = b; bb < b+ContentBins; ++bb) {
            OnOffLeftToRight->SetBinContent(bb, RejectionOn); 
          }
        }
      }
    }   
  }

 
  // (b) Search right to left
  TH1I* OnOffRightToLeft = new TH1I(TString("OnOffRightToLeft") + iID, TString("Tentacle - OnOffRightToLeft") + ID, EvaluationBins, MinTime, MaxTime);
  for (int b = 1; b <= EvaluationBins; ++b) OnOffRightToLeft->SetBinContent(b, RejectionOff);
  
  for (int s = 1; s <= MaxSearchDistance; ++s) {
    for (int b = OnOffRightToLeft->GetNbinsX(); b >= 1; --b) {
      int ContentBins = 0;
      double Content = 0.0;
      for (int bb = b; bb > b-s && b >= 1; --bb) {
        if (EvaluationRate->GetBinContent(bb) > NonSAARateMean) {
          Content += EvaluationRate->GetBinContent(bb);
          ContentBins++;
        } else {
          break; 
        }
      }
      if (ContentBins > 0 && NonSAARateMean > 0) { // NonSAARateMean > 0 means we do have threshold values, i.e. data i the non-SAA region
        if ( Content/ContentBins > NonSAARateRMS * MaxAllowedRMS/sqrt(ContentBins) + NonSAARateMean) {
          for (int bb = b; bb > b-ContentBins; --bb) {
            OnOffRightToLeft->SetBinContent(bb, RejectionOn); 
          }
        }
      }
    }   
  }
  
  // (c) "AND" them together: Only when both are ON set the combined to ON
  TH1I* OnOff = new TH1I(TString("OnOff") + iID, TString("Tentacle - OnOff") + ID, EvaluationBins, MinTime, MaxTime);
  
  for (int b = 1; b <= EvaluationBins; ++b) {
    if (OnOffRightToLeft->GetBinContent(b) == RejectionOn && 
        OnOffLeftToRight->GetBinContent(b) == RejectionOn) {
      OnOff->SetBinContent(b, RejectionOn);
    } else {
      OnOff->SetBinContent(b, RejectionOff);
    }
  }

 
  // (d) Make sure we include edge bins in the rejection:
  for (int b = 3; b < OnOff->GetNbinsX()-1; ++b) {
    if (OnOff->GetBinContent(b) == RejectionOn) {
      if (EvaluationRate->GetBinContent(b-2) == 0) OnOff->SetBinContent(b-1, RejectionOn);
      if (EvaluationRate->GetBinContent(b+2) == 0) OnOff->SetBinContent(b+1, RejectionOn);
    }
  }
  
  
  DebugOutput(OnOff, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_TentacleOnOffRMS"); 
  
  
  // (E) Restriction to tentacle region
  
  if (DoRegionRestriction == true) {
    double LongitudeMin = 240;               // <-- into caldb 
    double LongitudeMax = 320;               // <-- into caldb 
    for (int b = 1; b <= OnOff->GetNbinsX(); ++b) {
      if (OnOff->GetBinContent(b) == RejectionOn) {
        // Find the Orbits position
        int OrbitsIndex = O.FindClosestIndex(OnOff->GetBinCenter(b));
        if (OrbitsIndex == -1) {
          cerr<<"No suiting orbit data found!"<<endl;  
          continue;
        }
        if (fabs(O.m_Time[OrbitsIndex] - OnOff->GetBinCenter(b)) > 60) {
          cerr<<"Orbits is "<<fabs(O.m_Time[OrbitsIndex] - OnOff->GetBinCenter(b))<<" seconds from data"<<endl;
          continue;
        }
        if (O.m_Longitude[OrbitsIndex] < LongitudeMin || O.m_Longitude[OrbitsIndex] > LongitudeMax) {
          OnOff->SetBinContent(b, RejectionOff);
        }
      }
    }
  }
  
  
  // (F) Sanity checks
  
  if (DoRegionRestriction == true && DoSanityChecks == true) {
    
    // A tentacle has some 
    
    double OrbitDuration = 6240; //sec  --- no need at all to be perfect here
    double LongTentacleDuration = 7; // x 60 sec  --> into caldb
    
    double NTentacles = 0;
    double NLongTentacles = 0;
    
    bool LastAdjacentCounted = false;
    int LastAdjacentBin = 0; 
    double NAdjacentTentacles = 0;
    double LastDuration = 0;
    double DurationOfTentacles = 0;
    double MaxFlux = 0;
    double AverageFlux = 0;
    int NAverageFluxes = 0;
     
    bool Started = false;
    for (int i = 1; i <= OnOff->GetNbinsX(); ++i) {
      if (OnOff->GetBinContent(i) == RejectionOn) {
        Started = true;
        LastDuration++;
        if (EvaluationRate->GetBinContent(i) > 0) {
          AverageFlux += EvaluationRate->GetBinContent(i);
          if (EvaluationRate->GetBinContent(i) > MaxFlux) MaxFlux = EvaluationRate->GetBinContent(i);
          NAverageFluxes++;
        }
        if (OnOffInternalSAA->GetBinContent(i) == RejectionOn) { // Skip those here since we have an SAA and not a tentacle
          while (OnOff->GetBinContent(i) == RejectionOn) ++i;
          Started = false;
          LastDuration = 0;
        }
      } else {
        if (Started == true) {
          cout<<"Tentacle "<<i-LastDuration<<": Duration: "<<LastDuration<<" Orbit: "<<(int) ((OnOff->GetBinCenter(i)-OnOff->GetBinCenter(1))/OrbitDuration + 0.5)<<endl;
          DurationOfTentacles += LastDuration;
          NTentacles++;
          if (LastDuration >= LongTentacleDuration) NLongTentacles++;
          Started = false;
          LastDuration = 0;
          if (LastAdjacentBin != 0) {
            double TimeGap = OnOff->GetBinCenter(i) - OnOff->GetBinCenter(LastAdjacentBin);
            if (int((TimeGap + OrbitDuration/2)/OrbitDuration) == 1) {
              ++NAdjacentTentacles;
              if (LastAdjacentCounted == false) {
                ++NAdjacentTentacles;
                LastAdjacentCounted = true; 
              }
            } else {
              LastAdjacentCounted = false; 
            }
          } 
          LastAdjacentBin = i;
        }
      }
    }
    if (NTentacles > 0) {
      DurationOfTentacles /= NTentacles;
      AverageFlux /= NAverageFluxes;
      
      // Determine the number of orbits crossing the tentacle region while observing:
      // The line we are looking at is lat > -2 & long = 275
      
      double TentacleMinLatitude = -6;                // <-- into caldb 
      double TentacleAvgLongitude = 280;              // <-- into caldb 
      
      int NCrossings = 0;
      for (int b = 1; b < EvaluationRate->GetNbinsX(); ++b) {
        if (EvaluationRate->GetBinContent(b) > 0 && EvaluationRate->GetBinContent(b+1) > 0) {
          int Index1 = O.FindClosestIndex(EvaluationRate->GetBinCenter(b));
          int Index2 = O.FindClosestIndex(EvaluationRate->GetBinCenter(b+1));
          if (O.m_Latitude[Index1] >= TentacleMinLatitude && O.m_Latitude[Index2] >= TentacleMinLatitude &&
            O.m_Longitude[Index1] <= TentacleAvgLongitude && O.m_Longitude[Index2] > TentacleAvgLongitude) {
            NCrossings++;
          }
        }
      }
      
      cout<<"Number of tentacles: "<<NTentacles<<" (vs. "<<NCrossings<<" orbits) with average duration: "<<DurationOfTentacles<<endl;
      cout<<"Number of adjacent tentacles: "<<NAdjacentTentacles<<endl;
      cout<<"Average flux: "<<AverageFlux<<" vs. "<<NonSAARateMean<<endl;
      
      int Suspiciousness = 0;
      
      
      // Sanity check 1:
      // If we really have a tentacle most of the tentacle region crossings should produce a signal 
      double TentacleVsRegionCrossingsBadCutoff = 0.5;             // <-- into caldb 
      double TentacleVsRegionCrossingsReallyBadCutoff = 0.15;      // <-- into caldb 
      double AdjacentOrNotCutOff = 0.5;                            // <-- into caldb  
      if (NTentacles < TentacleVsRegionCrossingsBadCutoff * NCrossings) {
        cout<<"Suspicious: Only "<<NTentacles<<" tentacles in "<<NCrossings<<" crossings of the tentacle zone"<<endl;
        ++Suspiciousness;
        if (NTentacles < TentacleVsRegionCrossingsReallyBadCutoff * NCrossings) {
          ++Suspiciousness;
        }
      }
      
      // Sanity check 2:
      // The average tentacle cut duration is 4-5, i.e. 240-300 seconds
      double TentacleDurationCutOff = 3;                    // = 150 sec<-- into caldb (60 is from default Timeintervall)
      if (DurationOfTentacles <= TentacleDurationCutOff) {
        cout<<"Suspicious: The average duration of a tentacle passing is only "<<DurationOfTentacles<<" instead of ~5"<<endl;
        ++Suspiciousness;
      }
      
      // Sanity check 3:
      // Tentacle follows after tentacle follows after tentacle follows...
      if (NTentacles > 2 && NAdjacentTentacles < AdjacentOrNotCutOff * NTentacles) {
        cout<<"Suspicious: Only "<<NAdjacentTentacles<<" out of "<<NTentacles<<" are in sequential (or same) orbit"<<endl;
        ++Suspiciousness;
      }
      
      // Sanity check 4:
      // If the average flux in the tentacles is below the threshold then we have mostly likely just random flux increases
      // It is even worse, if there is some slight source variability
      double HintOfVariabilityCutOff = 1.4;                        // <-- into caldb 
      if (AverageFlux < Threshold && MaxFlux <= 2.0*MaxAllowedRMS*NonSAARateRMS + NonSAARateMean) {
        cout<<"Suspicious: The average OVERALL tentacle flux is below the threshold: "<<AverageFlux<<" vs. "<<Threshold<<endl;
        ++Suspiciousness;
        if (NonSAARateRMS > HintOfVariabilityCutOff * NonSAARateSigma) {
          cout<<"            and we have a hint of a variable source"<<endl;
          ++Suspiciousness;
        }
      }
      
      // Sanity check 5:
      // If we have a few tentacles with above average length, then we likely have a tentacle
      unsigned int LongTentacleThreshold = 2;
      if (NLongTentacles > LongTentacleThreshold) {
        cout<<"Positive: We have a few long duration tentacles: "<<NLongTentacles<<" out of "<<NTentacles<<endl;    
        --Suspiciousness;
      }
      
      // Sanity check 6:
      // If we have a tentacle with at least 5 times the average flux, then we likely have a tentacle 
      unsigned int TentacleFluxThreshold = 5;
      if (MaxFlux > TentacleFluxThreshold*AverageFlux) {
        cout<<"Positive: We have at least one strong tentacles: "<<MaxFlux<<" vs. average "<<AverageFlux<<endl;    
        --Suspiciousness;
      }
      
      
      // More than 2 suspicousness points? No Tentacle!
      int SuspiciousnessThreshold = 2;                              // <-- into caldb 
      if (Suspiciousness >= SuspiciousnessThreshold) {
        cout<<"Attention: The characteristics of the given tentacle regions is not in agreement with a typical tentacle."<<endl;
        cout<<"           I ignore them all!"<<endl;
        for (int b = 1; b <= EvaluationBins; ++b) OnOff->SetBinContent(b, RejectionOff);
      }
    //} else {
    //  // If all tentacles end up in the internal SAA cut, then we do not have any...
    //  for (int b = 1; b <= EvaluationBins; ++b) OnOff->SetBinContent(b, RejectionOff);
    }
  }
 
  
  // (G) Now set it:
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    int Bin = OnOff->FindBin(H.m_Time[h]);
    if (Bin == 0 || Bin > OnOff->GetNbinsX()) {
      cerr<<"Something is wrong with the times as I cannot find a rejection value for time "<<H.m_Time[h]<<endl; 
      if (Mode == c_TentacleCutRMS) H.m_SoftTentacled[h] = true;
      H.m_SoftTentacledRMS[h] = false;        
    } else {
      if (OnOff->GetBinContent(Bin) == RejectionOn) {
        if (Mode == c_TentacleCutRMS) H.m_SoftTentacled[h] = true;
        H.m_SoftTentacledRMS[h] = true;
      } else {
        if (Mode == c_TentacleCutRMS) H.m_SoftTentacled[h] = true;
        H.m_SoftTentacledRMS[h] = false;        
      }    
    }
  }
    
  DebugOutput(H.m_SoftTentacledRMS, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_TentacleOnOffRMSFinalAll"); 
  DebugOutput(OnOff, TString(H.m_ID) + "_" + ((H.m_Module == 0) ? "A" : "B") + "_TentacleOnOffRMSFinal"); 
  
  
  // (H) Show everything
  
  if (Show == true) {  
    // Just as sanity check make an Orbits plot of the rates
    int LongitudeBins = 48;
    int LatitudeBins = 12;
    double MinLatitude = -6.06;
    double MaxLatitude = +6.06;
    
    double LatMin = 100;
    double LatMax = -100;
    
    TH2D* OrbitsNormalizer = new TH2D(TString("TentaclesOrbitsNormalizer") + iID, 
                                      TString("Tentacle - TentaclesOrbitsNormalizer") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
    
    TH2D* Orbits = new TH2D(TString("TentaclesOrbits") + iID, 
                            TString("TentaclesOrbits") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
    Orbits->SetXTitle("Longitude [deg]");
    Orbits->SetYTitle("Latitude [deg]");
    Orbits->SetZTitle("Events [cts/sec]");
    
    
    TH1D* RateNormalizer = new TH1D(TString("RatesTentacleNormalizer") + iID, TString("Tentacle - RatesTentacleNormalizer") + ID, OneSecondBins, MinTime, MaxTime);
    TH1D* Rates = new TH1D(TString("RatesTentacle") + iID, TString("RatesTentacle") + ID, OneSecondBins, MinTime, MaxTime);
    Rates->SetXTitle("Time [s]");
    Rates->SetYTitle("cts/sec");
    
    
    for (unsigned int u = 0; u < F.m_Time.size(); ++u) {
      double Time = F.m_Time[u];
      if (WithinSpecialGTI(Time) == false) continue;
      
      // Find the Orbits position
      int OrbitsIndex = O.FindClosestIndex(Time);
      if (OrbitsIndex == -1) {
        cerr<<"No suiting Orbits found!"<<endl;  
        continue;
      }
      if (fabs(O.m_Time[OrbitsIndex] - Time) > 1) {
        cerr<<"Orbits is "<<fabs(O.m_Time[OrbitsIndex] - Time)<<" seconds from data"<<endl;
        continue;
      }
      // Find the housekeeping position
      int HKIndex = H.FindClosestIndex(Time);
      if (HKIndex == -1) {
        cerr<<"No suiting housekeeping found!"<<endl;  
        continue;
      }
      if (fabs(H.m_Time[HKIndex] - Time) > 1) {
        cerr<<"Housekeeping is "<<fabs(H.m_Time[HKIndex] - Time)<<" seconds from data"<<endl;
        continue;
      }
      if (H.m_SoftSAA[HKIndex] == true) continue;
      //if (H.m_SoftTentacled[HKIndex] == true) continue;
      
      int DetectorID = F.m_DetectorID[u];
      int RawX = F.m_RawX[u];
      int RawY = F.m_RawY[u];
      int ID = 10000*DetectorID + 100*RawX + RawY;
      vector<int>::iterator I = lower_bound(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end(), 10000*DetectorID + 100*RawX + RawY);
      if (I != ExcludedDetRawXY.end() && (*I) == ID) continue;
      
      Orbits->Fill(O.m_Longitude[OrbitsIndex], O.m_Latitude[OrbitsIndex]);
      Rates->Fill(F.m_Time[u]);
      
      if (O.m_Latitude[OrbitsIndex] > LatMax) LatMax = O.m_Latitude[OrbitsIndex];
      if (O.m_Latitude[OrbitsIndex] < LatMin) LatMin = O.m_Latitude[OrbitsIndex];
    }
    
    double LiveTimeWithCuts = 0;
    double LiveTimeNoCuts = 0;
    for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
      if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
      
      if (F.IsGTI(H.m_Time[h]) == true && H.m_SoftSAA[h] == false) {
        int OrbitsIndex = O.FindClosestIndex(H.m_Time[h]);
        if (OrbitsIndex == -1) {
          cerr<<"No suiting Orbits found!"<<endl;  
          continue;
        }
        if (fabs(O.m_Time[OrbitsIndex] - H.m_Time[h]) > 1) {
          cerr<<"Orbits is "<<fabs(O.m_Time[OrbitsIndex] - H.m_Time[h])<<" seconds from data"<<endl;
          continue;
        }
        if (H.m_SoftTentacled[h] == false) {
          LiveTimeWithCuts += H.m_LiveTime[h];
          OrbitsNormalizer->Fill(O.m_Longitude[OrbitsIndex], O.m_Latitude[OrbitsIndex], H.m_LiveTime[h]);
          RateNormalizer->Fill(H.m_Time[h], H.m_LiveTime[h]);
        }
        LiveTimeNoCuts += H.m_LiveTime[h];          
      }
    }
    cout<<"Live time before cuts: "<<LiveTimeNoCuts<<endl;
    cout<<"Live time after cuts:  "<<LiveTimeWithCuts<<" ("<<100.0*(LiveTimeNoCuts-LiveTimeWithCuts)/LiveTimeNoCuts<<"%)"<<endl;
    
    
    TCanvas* EvaluationRateCanvas = new TCanvas();
    EvaluationRateCanvas->cd();
    EvaluationRate->Draw();
    OnOff->Scale(EvaluationRate->GetMaximum());
    OnOff->Draw("SAME");
    EvaluationRateCanvas->Update();
    
    TCanvas* NonSAARateHistogramCanvas = new TCanvas();
    NonSAARateHistogramCanvas->cd();
    NonSAARateHistogram->Draw();
    NonSAARateHistogramCanvas->Update();
    
    TCanvas* OrbitsCanvas = new TCanvas();
    OrbitsCanvas->cd();
    for (int bx = 1; bx <= Orbits->GetNbinsX(); ++bx) {
      for (int by = 1; by <= Orbits->GetNbinsY(); ++by) {
        if (OrbitsNormalizer->GetBinContent(bx, by) > 0) {
          Orbits->SetBinContent(bx, by, Orbits->GetBinContent(bx, by)/OrbitsNormalizer->GetBinContent(bx, by));
        }
      }
    }
    Orbits->Draw("colz");
    OrbitsCanvas->Update();
    
    TCanvas* RatesCanvas = new TCanvas();
    RatesCanvas->cd();
    for (int bx = 1; bx <= Rates->GetNbinsX(); ++bx) {
      if (RateNormalizer->GetBinContent(bx) > 0) {
        Rates->SetBinContent(bx, Rates->GetBinContent(bx)/RateNormalizer->GetBinContent(bx));
      }
    }
    Rates->Draw();
    RatesCanvas->Update();
  }
  
  cout<<"Done with tentacle flag identification - RMS mode"<<endl;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


vector<int> NBaseTool::MostlyEliminateSource(NFilteredEvents& F, NOrbits& O, bool Show)
{
  //! Return the pixels where the source is most likely on

  int NPositionBins = 64;
  TH2D* PositionsOnSource = new TH2D("PositionsOnSource", "PositionsOnSource", NPositionBins, -32, 32, NPositionBins, -32, 32);
  PositionsOnSource->SetXTitle("1, 2 <-- Raw X [pixel] --> 3, 0");
  PositionsOnSource->SetYTitle("2, 3 <-- Raw Y [pixel] --> 1, 0");
  PositionsOnSource->SetZTitle("cts");  
  
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < F.m_Time.size(); ++e) {
    
    //if (WithinSpecialGTI(F.m_Time[e]) == false) continue;
    if (F.m_Energy[e] < m_SpectrumMin || F.m_Energy[e] > m_SpectrumMax) continue;
    if (F.IsGTI(F.m_Time[e]) == false) continue;
    if (O.IsGoodOrbit(F.m_Time[e]) == false) continue;
    
    int DetectorID = F.m_DetectorID[e];
    int RawX = F.m_RawX[e];
    int RawY = F.m_RawY[e];
    
    double PosX = 0;
    double PosY = 0;
    ConvertRawPos(RawX, RawY, DetectorID, PosX, PosY);
    
    if (RawX >= 0 && RawX <= 31 && RawY >= 0 && RawY <= 31) {
      PositionsOnSource->Fill(PosX, PosY);
    }
  }
  
  int LogBinningBins = 100;
  double LogBinningMin = numeric_limits<double>::max();
  double LogBinningMax = 0;
  
  for (int bx = 1; bx <= NPositionBins; ++bx) {
    for (int by = 1; by <= NPositionBins; ++by) {
      if (PositionsOnSource->GetBinContent(bx, by) > 1) {
        if (PositionsOnSource->GetBinContent(bx, by) > LogBinningMax) LogBinningMax = PositionsOnSource->GetBinContent(bx, by);
        if (PositionsOnSource->GetBinContent(bx, by) < LogBinningMin) LogBinningMin = PositionsOnSource->GetBinContent(bx, by);
      }
    }
  }
  
  LogBinningMax += 1;  // Make it 1 count larger, to ensure everything is contained
  
  vector<double> LogBinning(LogBinningBins);
  LogBinningMin = log(LogBinningMin);
  LogBinningMax = log(LogBinningMax); 
  double LogDist = (LogBinningMax-LogBinningMin)/(LogBinningBins - 1); // one smaller, since one is the edge
  
  int RealLogBinningBins = 0;
  LogBinning[0] = int(exp(LogBinningMin) + 0.5) - 0.5; // Always >= 0.5
  for (int i = 1; i < LogBinningBins; ++i) {
    double Candidate = int(exp(LogBinningMin+i*LogDist) + 0.5) - 0.5;
    if (Candidate == LogBinning[RealLogBinningBins]) continue; // if the difference is smaller than 1, do not do anything
    LogBinning[++RealLogBinningBins] = Candidate;
  }   
  
  //cout<<"Bins: "<<endl;
  //for (int i = 0; i < LogBinningBins; ++i) {
  //  cout<<i<<" "<<LogBinning[i]<<endl; 
  //}
  TH1D* PositionRates = new TH1D("PositionRates", "PositionRates", RealLogBinningBins, &LogBinning[0]);
  
  double GoodPixels = 0;
  for (int bx = 1; bx <= NPositionBins; ++bx) {
    for (int by = 1; by <= NPositionBins; ++by) {
      if (PositionsOnSource->GetBinContent(bx, by) > 0) {
        PositionRates->Fill(PositionsOnSource->GetBinContent(bx, by));
        GoodPixels++;
      }
    }
  }
  cout<<"Source elimiation: Good Pixels: "<<GoodPixels<<endl;
  cout<<"Real log binning bins: "<<RealLogBinningBins<<endl;
  for (int b = 1; b <= RealLogBinningBins; ++b) {
    PositionRates->SetBinContent(b, PositionRates->GetBinContent(b)/PositionRates->GetBinWidth(b));
  }
  
  DebugOutput(PositionRates, "00000000000_X_PositionRatesBeforeSmooth"); 
  
  // Smooth the hell out of it:
  PositionRates->Smooth(100);
  
  DebugOutput(PositionRates, "00000000000_X_PositionRatesAfterSmooth"); 

  // Find the maximum
  double PositionRatesMaximum = PositionRates->GetBinCenter(PositionRates->GetMaximumBin());
  cout<<"PositionRatesMaximum: "<<PositionRatesMaximum<<endl;
  
  // It is definitely non-Gaussian, but let's just use the 5-sigma upper limit as our cut-off Count
  double PositionRatesCutoff = PositionRatesMaximum + m_SourceCutRMSThreshold*sqrt(PositionRatesMaximum);
  cout<<"PositionRatesCutoff: "<<PositionRatesCutoff<<endl;
  
  // This task should just eliminate the worst offenders, we do not want to cut more than ~25% of the data
  double NotEliminatedPixels = 0;
  for (int b = 1; b <= RealLogBinningBins; ++b) {
    if (PositionRates->GetBinCenter(b) < PositionRatesCutoff) {
      NotEliminatedPixels += PositionRates->GetBinContent(b) * PositionRates->GetBinWidth(b);
    } else {
      /*
      if (NotEliminatedPixels < 0.75 * GoodPixels) {
        PositionRatesCutoff = PositionRates->GetBinCenter(b);
        NotEliminatedPixels += PositionRates->GetBinContent(b) * PositionRates->GetBinWidth(b);
      }
      */
    }
    //printf("Not elim: %d: %f (%f * %f)\n", b-1, NotEliminatedPixels, PositionRates->GetBinContent(b), PositionRates->GetBinWidth(b));
  }
  cout<<"Source elimiation: Cut off: "<<PositionRatesCutoff<<" with "<<NotEliminatedPixels<<" of "<<GoodPixels<<" used pixels"<<endl;
  
  
  // Create a map of excluded bins: Notation 1000*Det + 100*RawX + RawY (for faster lookup later)
  vector<int> ExcludedDetRawXY;
  for (int bx = 1; bx <= NPositionBins; ++bx) {
    for (int by = 1; by <= NPositionBins; ++by) {
      if (PositionsOnSource->GetBinContent(bx, by) > PositionRatesCutoff) {
        int Det = 0;
        int RawX = 0;
        int RawY = 0;
        ConvertPosRaw(PositionsOnSource->GetXaxis()->GetBinCenter(bx), PositionsOnSource->GetXaxis()->GetBinCenter(by), Det, RawX, RawY);
        
        int DXY = 10000*Det + 100*RawX + RawY;
        ExcludedDetRawXY.push_back(DXY);
      }
    }
  }
  sort(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end());
  
  cout<<"Source elimiation: Eliminated pixels: "<<100.0 * ExcludedDetRawXY.size() / (64*64)<<" %"<<endl;
  
  // Redo to check it has worked!
  PositionsOnSource->Reset();
  for (unsigned int e = 0; e < F.m_Time.size(); ++e) {
    
    //if (WithinSpecialGTI(F.m_Time[e]) == false) continue;
    if (F.m_Energy[e] < m_SpectrumMin || F.m_Energy[e] > m_SpectrumMax) continue;
    if (F.IsGTI(F.m_Time[e]) == false) continue;
  
    int DetectorID = F.m_DetectorID[e];
    int RawX = F.m_RawX[e];
    int RawY = F.m_RawY[e];
    int ID = 10000*DetectorID + 100*RawX + RawY;
    vector<int>::iterator I = lower_bound(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end(), 10000*DetectorID + 100*RawX + RawY);
    if (I != ExcludedDetRawXY.end() && (*I) == ID) continue;

    double PosX = 0;
    double PosY = 0;
    ConvertRawPos(RawX, RawY, DetectorID, PosX, PosY);    
    PositionsOnSource->Fill(PosX, PosY);
  }
  
  if (Show == true) {
    TCanvas* PositionsOnSourceCanvas = new TCanvas();
    PositionsOnSourceCanvas->cd();
    PositionsOnSource->Draw("colz");
    PositionsOnSourceCanvas->Update();  
 
    TCanvas* PositionRatesCanvas = new TCanvas();
    PositionRatesCanvas->cd();
    PositionRates->Draw("colz");
    PositionRatesCanvas->Update();
  }
  
  return ExcludedDetRawXY; 
}
  
  
////////////////////////////////////////////////////////////////////////////////


void NBaseTool::ConvertRawPos(int RawX, int RawY, int DetectorID, double& PosX, double& PosY)
{ 
  if (DetectorID == 0) {
    PosX = +RawY+0.5; 
    PosY = +RawX+0.5;
  } else if (DetectorID == 1) {
    PosX = -RawX-0.5; 
    PosY = +RawY+0.5;
  } else if (DetectorID == 2) {
    PosX = -RawY-0.5;
    PosY = -RawX-0.5;
  } else if (DetectorID == 3) {
    PosX = +RawX+0.5;
    PosY = -RawY-0.5;
  }
}


////////////////////////////////////////////////////////////////////////////////


void NBaseTool::ConvertPosRaw(double PosX, double PosY, int& DetectorID, int& RawX, int& RawY)
{ 
  if (PosX > 0 && PosY > 0) {
    DetectorID = 0;
    RawX = int(PosY);
    RawY = int(PosX);
  } else if (PosX < 0 && PosY > 0) {
    DetectorID = 1;
    RawX = int(-PosX);
    RawY = int(PosY);
  } else if (PosX < 0 && PosY < 0) {
    DetectorID = 2;
    RawX = int(-PosY);
    RawY = int(-PosX);
  } else if (PosX > 0 && PosY < 0) {
    DetectorID = 3;
    RawX = int(PosX);
    RawY = int(-PosY);
  }
}


////////////////////////////////////////////////////////////////////////////////


void NBaseTool::DebugOutput(TH1* Hist, TString FileName)
{
  if (m_Debug == false) return; 

  /*
  TString HistFileName = TString("Nulyses_") + FileName + ".png";
  TCanvas* C = new TCanvas("C", "C", 4000, 3000);
  C->cd();
  Hist->Draw();
  C->Update();
  C->SaveAs(HistFileName);
  */
    
  TString TextFileName = FileName + ".txt";
  ofstream out;
  out.open(TextFileName);
  if (out.is_open() == false) {
    cout<<"Error: Unable to open "<<TextFileName<<endl;
    return;
  }
  /*
  out<<"# Bin min, Bin max, Value"<<endl;
  for (int b = 1; b <= Hist->GetXaxis()->GetNbins(); ++b) {
    out<<setprecision(9)<<Hist->GetXaxis()->GetBinLowEdge(b)<<"  "<<Hist->GetXaxis()->GetBinUpEdge(b)<<"  "<<Hist->GetBinContent(b)<<endl; 
  }
  */
  
  if (FileName.Contains("_")) {
    FileName = FileName(14, FileName.Length() - 14);
  }
  for (int b = 1; b <= Hist->GetXaxis()->GetNbins(); ++b) {
    out<<FileName<<" "<<b-1<<" "<<fixed<<setprecision(6)<<Hist->GetBinContent(b)<<endl; 
  }
  
  out.close();
}


////////////////////////////////////////////////////////////////////////////////


void NBaseTool::DebugOutput(vector<bool>& V, TString FileName)
{
  if (m_Debug == false) return; 
  
  TString TextFileName = FileName + ".txt";
  ofstream out;
  out.open(TextFileName);
  if (out.is_open() == false) {
    cout<<"Error: Unable to open "<<TextFileName<<endl;
    return;
  }

  
  if (FileName.Contains("_")) {
    FileName = FileName(14, FileName.Length() - 14);
  }
  for (unsigned int b = 0; b < V.size(); ++b) {
    out<<FileName<<" "<<b<<" "<<fixed<<setprecision(6)<<V[b]<<endl; 
  }
  
  out.close();
}


// NBaseTool.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
