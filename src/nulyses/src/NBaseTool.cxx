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

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

// MEGAlib libs:

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
    if (Option == "-f" || Option == "-m" || Option == "--gti" || Option == "-d") {
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
    else if (Option == "-s") {
      if (!((argc > i+2) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) && 
        (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0) && 
        (argv[i+3][0] != '-' || isalpha(argv[i+3][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs t arguments!"<<endl;
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
    if (Option == "-d") {
      m_Directories.push_back(argv[++i]);
      m_DetPosXA.push_back(0);
      m_DetPosYA.push_back(0);
      m_DetSizeA.push_back(0);
      m_DetPosXB.push_back(0);
      m_DetPosYB.push_back(0);
      m_DetSizeB.push_back(0);
      cout<<"Accepting directory file name: "<<m_Directories.back()<<endl;
    } else if (Option == "-dp") {
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
    } else if (Option == "-g") {
      m_SpecialGTIStart.push_back(atof(argv[++i]));
      m_SpecialGTIStop.push_back(atof(argv[++i]));
      cout<<"Accepting special good time interval: "<<m_SpecialGTIStart.back()<<" to "<<m_SpecialGTIStop.back()<<endl;
    } else if (Option == "-b") {
      m_SpecialBTIStart.push_back(atof(argv[++i]));
      m_SpecialBTIStop.push_back(atof(argv[++i]));
      cout<<"Accepting special bad time window: "<<m_SpecialBTIStart.back()<<" to "<<m_SpecialBTIStop.back()<<endl;
    } else if (Option == "--gti") {
      if (ReadGTI(argv[++i]) == false) return false;
      cout<<"Accepting GTI!"<<endl;
    }
  }
  
  if (m_LookAtModule == "") {
    cout<<"Analyzing both modules!"<<endl;
    m_LookAtModule = "ab";
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
  cout<<"N columns: "<<NColumns<<endl;

  // Get the number of rows:
  long NRows = 0;
  fits_get_num_rows(EventFile, &NRows, &status);
  cout<<"Rows: "<<NRows<<endl;

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
    cout<<"Added GTI: "<<Start<<":"<<Stop<<endl;
  }
  
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::Load(const TString& Directory, const TString& LookAtModule)
{
  TString Tag = Directory;
  if (Tag.EndsWith("/")) {
    Tag = Tag.Remove(Tag.Last('/'), 1); 
  }
  if (Tag.Last('/') != kNPOS) {
    Tag = Tag.Remove(0, Tag.Last('/')+1);      
  }
  
  TString OrbitsFileName = Directory + "/auxil/nu" + Tag + "_orb.fits";
  TString AttitudeFileName = Directory + "/auxil/nu" + Tag + "_att.fits";
  TString EngineeringFileName = Directory + "/hk/nu" + Tag + "_eng.hk";
  TString HKFileNameA = Directory + "/hk/nu" + Tag + "A_fpm.hk";
  TString HKFileNameB = Directory + "/hk/nu" + Tag + "B_fpm.hk";
  TString UFFileNameA = Directory + "/event_cl/nu" + Tag + "A_uf.evt";
  TString UFFileNameB = Directory + "/event_cl/nu" + Tag + "B_uf.evt";
  TString FIFileNameA01 = Directory + "/event_cl/nu" + Tag + "A01_cl.evt";
  TString FIFileNameB01 = Directory + "/event_cl/nu" + Tag + "B01_cl.evt";
  TString FIFileNameA02 = Directory + "/event_cl/nu" + Tag + "A02_cl.evt";
  TString FIFileNameB02 = Directory + "/event_cl/nu" + Tag + "B02_cl.evt";
  
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
  if (LookAtModule.Contains("a")) if (m_HousekeepingA.Read(HKFileNameA) == false) return false;
  m_HousekeepingA.m_Module = 0;
  m_HousekeepingA.m_ID = Tag;
  
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
  if (LookAtModule.Contains("b")) if (m_HousekeepingB.Read(HKFileNameB) == false) return false;
  m_HousekeepingB.m_Module = 1;
  m_HousekeepingB.m_ID = Tag;
  
  m_UnfilteredEventsB.Clean();
  if (m_ReadUnfiltered == true && LookAtModule.Contains("b")) if (m_UnfilteredEventsB.Read(UFFileNameB) == false) return false;
  m_UnfilteredEventsB.m_Module = 1;
  m_UnfilteredEventsB.m_ID = Tag;
  
  m_FilteredEventsB01.Clean();
  if (m_ReadFiltered01 == true && LookAtModule.Contains("b")) if (m_FilteredEventsB01.Read(FIFileNameB01) == false) return false;
  m_FilteredEventsB01.m_Module = 1;
  m_FilteredEventsB01.m_ID = Tag;
  
  m_FilteredEventsB02.Clean();
  if (m_ReadFiltered02 == true && LookAtModule.Contains("b")) if (m_FilteredEventsB01.Read(FIFileNameB01) == false) return false;
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
  
  if (m_MergeFilteredEvents == false) {
    if (LookAtModule.Contains("a")) FindSAAs(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, c_SAACutStrict);
    if (LookAtModule.Contains("b")) FindSAAs(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, c_SAACutStrict);
  } else {
    if (LookAtModule.Contains("a")) FindSAAs(m_FilteredEventsA, m_HousekeepingA, m_Orbits, c_SAACutStrict);
    if (LookAtModule.Contains("b")) FindSAAs(m_FilteredEventsB, m_HousekeepingB, m_Orbits, c_SAACutStrict);
  }
  
  if (LookAtModule.Contains("a")) FindSAATentacle(m_FilteredEventsA, m_HousekeepingA, m_Orbits);
  if (LookAtModule.Contains("b")) FindSAATentacle(m_FilteredEventsB, m_HousekeepingB, m_Orbits);
  
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
  cout<<"Finding SAA ..."<<endl;
  
  if (F.m_Time.size() == 0) {
    cout<<"No filtered events = no SAA cut"<<endl;  
    return true;
  }
  
  // Start with no cuts (clean in case we run it):
  for (unsigned int i = 0; i < H.m_Time.size(); ++i) {
    H.m_SoftSAA[i] = false;
  }
  
  int RejectionOn  = 0;
  int RejectionOff = 1;
  
  int Bins = H.GetMaxTime() - H.GetMinTime() + 1;
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;
  
  TString iID = TString("_ID_") + H.m_ID + "_M_" + ((H.m_Module == 0) ? "A" : "B");
  TString ID = TString(" (ID: ") + H.m_ID + ((H.m_Module == 0) ? "A" : "B") + ")";

  TH1D* LifeTimes = new TH1D(TString("LifeTimes") + iID, TString("LifeTimes") + ID, Bins, MinTime, MaxTime);
  TH1D* ShieldRateHigh = new TH1D(TString("SAAShieldRateHigh") + iID, TString("SAAShieldRateHigh") + ID, Bins, MinTime, MaxTime);
  ShieldRateHigh->SetXTitle("Time [sec since 1/1/2010]");
  ShieldRateHigh->SetYTitle("cts/sec");
  TH1D* ShieldRateHighAverage = new TH1D(TString("SAAShieldRateHighAverage") + iID, TString("SAAShieldRateHighAverage") + ID, Bins, MinTime, MaxTime);
  ShieldRateHighAverage->SetLineColor(kGreen);
  TH1D* ShieldRateHighCut = new TH1D(TString("SAAShieldRateHighCut") + iID, TString("SAAShieldRateHighCut") + ID, Bins, MinTime, MaxTime);
  ShieldRateHighCut->SetLineColor(kRed);
  TH1D* NAcceptedEvents = new TH1D(TString("SAANAcceptedEvents") + iID, TString("SAANAcceptedEvents") + ID, Bins, MinTime, MaxTime);
  TH1I* OnOff = new TH1I(TString("SAAOnOff") + iID, TString("SAAOnOff") + ID, Bins, MinTime, MaxTime);
  TH1D* NAcceptedEventsShieldHighCut = new TH1D(TString("SAANAcceptedEventsShieldHighCut") + iID, TString("SAANAcceptedEventsShieldHighCut") + ID, Bins, MinTime, MaxTime);
  NAcceptedEventsShieldHighCut->SetLineColor(kRed);
  
  for (unsigned int i = 0; i < H.m_Time.size(); ++i) {
    int OIndex = O.FindClosestIndex(ShieldRateHigh->GetBinCenter(i));
    int SAAFlag = O.m_SAAFlag[OIndex];
    if (SAAFlag == 0 && H.m_ShieldRateHigh[i] > 0) { // Some rare events happen even during known SAA passages (super-high energy protons?) - skip those
      ShieldRateHigh->Fill(H.m_Time[i], H.m_ShieldRateHigh[i]);
      ShieldRateHighCut->Fill(H.m_Time[i], H.m_ShieldRateHigh[i]);
      OnOff->Fill(H.m_Time[i], RejectionOff);
    } else {
      OnOff->Fill(H.m_Time[i], RejectionOn);      
    }
    NAcceptedEvents->Fill(H.m_Time[i], H.m_NAcceptedEvents[i]); 
    NAcceptedEventsShieldHighCut->Fill(H.m_Time[i], H.m_NAcceptedEvents[i]);
    LifeTimes->Fill(H.m_Time[i], H.m_LiveTime[i]);
  }
  
  // Step 1: Calculate averages

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
  
  // Step 2: Decide which regions to skip:
  
  /*
  // Do a simple cut, whenever the rate is above MaxRateCut
  double MaxRateCut = 100.0;
  for (int i = 1; i <= ShieldRateHighAverage->GetNbinsX(); ++i) {
    if (ShieldRateHighAverage->GetBinContent(i) > MaxRateCut || ShieldRateHigh->GetBinContent(i) > MaxRateCut) {
      OnOff->SetBinContent(i, RejectionOn);
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
      OnOff->SetBinContent(i, RejectionOn);
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
  

  // Clean the periods immediately after SAA passages and in SAA passages - since we are still ramping up the voltage and don't get all vetoes
  int SAARampingTime = 60;
  
  bool InSAAorFreakEvent = false;
  for (int b = 1; b <= ShieldRateHigh->GetNbinsX(); ++b) {
    int OIndex = O.FindClosestIndex(ShieldRateHigh->GetBinCenter(b));
    int SAAFlag = O.m_SAAFlag[OIndex];
    if (SAAFlag == 1) {
      InSAAorFreakEvent = true;
      OnOff->SetBinContent(b, RejectionOn); 
    } else {
      if (InSAAorFreakEvent == true) {
        // Flag the next "SAARampingTime" seconds as still bad, since the high voltage is ramping up slowly
        for (int bb = b; bb <= ShieldRateHigh->GetNbinsX() && bb-b < SAARampingTime; ++bb) {
          OnOff->SetBinContent(bb, RejectionOn);
        }
      }
      InSAAorFreakEvent = false;
    }
  }
  
  
  // Clean up - all SAA passages are confinend between MinLong and MaxLong
  // We might replace this one day with a real outer SAA contour...
  double MinLong = 270;
  double MaxLong = 350;
  for (int i = 1; i <= OnOff->GetNbinsX(); ++i) {
    int OIndex = O.FindClosestIndex(OnOff->GetBinCenter(i));
    if (fabs(O.m_Time[OIndex] - OnOff->GetBinCenter(i)) < 1.0) {
      if (O.m_Longitude[i] < MinLong || O.m_Longitude[i] > MaxLong) {
        OnOff->SetBinContent(i, RejectionOff);
      }
    } else {
      cerr<<"Hmmm, no Orbits information found... Ignoring..."<<endl;
    }
  }


  // Clean up - remove off periods < X*HalfAverageLength

  int CutOffCount = HalfAverageLength; // good: 2x
  
  int OffCount = 0;
  for (int i = 1; i <= ShieldRateHigh->GetNbinsX(); ++i) {
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

  
  // Remove on periods < X*HalfAverageLength
  int OnCount = 0;
  for (int i = 1; i <= ShieldRateHigh->GetNbinsX(); ++i) {
    //cout<<i<<": "<<OnCount<<" - "<<OnOff->GetBinContent(i)<<endl;
    if (OnOff->GetBinContent(i) == 0) {
      if (OnCount > 0 && OnCount < CutOffCount) {
        for (int b = i-1; b >= i-OnCount; --b) {
          if (OnOff->GetBinContent(b) == 1) {
            OnOff->SetBinContent(b, RejectionOn);
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
  
  for (int b = 1; b <= OnOff->GetNbinsX(); ++b) {
    int Index = H.FindClosestIndex(OnOff->GetBinCenter(b));
    if (OnOff->GetBinCenter(b) - H.m_Time[Index] < 1.0) {
      if (OnOff->GetBinContent(b) == RejectionOn) {
        if (Mode == c_SAACutStrict) H.m_SoftSAA[Index] = true;
        H.m_SoftSAAStrict[Index] = true;
      } else {
        if (Mode == c_SAACutStrict) H.m_SoftSAA[Index] = false;
        H.m_SoftSAAStrict[Index] = false;
      }
    } else {
      //cerr<<"Something is wrong with the times as I cannot find housekeeping data... "<<OnOff->GetBinCenter(b)<<endl; 
    }
  }
  
  
  // Final stage: back track - make sure to only exclude regions where we actually see an increase in the final count rate
  TH1D* FilteredRate = new TH1D(TString("SAAFilteredRate") + iID, TString("SAAFilteredRate") + ID, Bins, MinTime, MaxTime);
  FilteredRate->SetXTitle("Time [sec since 1/1/2010]");
  FilteredRate->SetYTitle("cts/sec");
  TH1D* CleanRate = new TH1D(TString("SAACleanRate") + iID, TString("SAACleanRate") + ID, Bins, MinTime, MaxTime);
  CleanRate->SetXTitle("Time [sec since 1/1/2010]");
  CleanRate->SetYTitle("cts/sec");
  TH1D* FilteredRateCutOriginal = new TH1D(TString("SAAFilteredRateCut") + iID, TString("SAAFilteredRateCut") + ID, Bins, MinTime, MaxTime);
  FilteredRateCutOriginal->SetLineColor(kMagenta);
  TH1D* FilteredRateCut = new TH1D(TString("SAAFilteredRateCut") + iID, TString("SAAFilteredRateCut") + ID, Bins, MinTime, MaxTime);
  FilteredRateCut->SetLineColor(kRed);
  TH1D* FilteredRateAverageLarge = new TH1D(TString("SAAFilteredRateAverageLarge") + iID, TString("SAAFilteredRateHighLarge") + ID, Bins, MinTime, MaxTime);
  FilteredRateAverageLarge->SetLineColor(kGreen);
  TH1D* FilteredRateAverageSmall = new TH1D(TString("SAAFilteredRateAverageSmall") + iID, TString("SAAFilteredRateHighSmall") + ID, Bins, MinTime, MaxTime);
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
    if (OnOff->GetBinContent(b) == RejectionOff) {
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


  // Track from the left and right
  double FilteredDecisionThresholdLeft = 1.25;
  double FilteredDecisionThresholdRight = 1.25;
  double SingleRateDecisionThreshold = 2.25;
  bool Previous = RejectionOff;
  for (int b = 1; b <= OnOff->GetNbinsX(); ++b) {
    // Switch off -> on
    if (OnOff->GetBinContent(b) == RejectionOn && Previous == RejectionOff) {
      Previous = RejectionOn;
      int Min = b-FilteredEventsHalfAverageSmall;
      if (Min < 1) Min = 1;
      int Dist = b-Min;
      while (FilteredRateAverageSmall->GetBinContent(b) < FilteredDecisionThresholdLeft*FilteredRateAverageLarge->GetBinContent(Min) &&
             FilteredRate->GetBinContent(b) < SingleRateDecisionThreshold*FilteredRateAverageLarge->GetBinContent(Min) + 3*sqrt(FilteredRateAverageLarge->GetBinContent(Min)) ) {
        for (int a = b-Dist; a <= b; ++a) OnOff->SetBinContent(a, RejectionOff);
        ++b;
        if (b > OnOff->GetNbinsX()) break;
        if (OnOff->GetBinContent(b) == RejectionOff) break;
      }
    }
    // Switch on -> off
    else if (OnOff->GetBinContent(b) == RejectionOff && Previous == RejectionOn) {
      Previous = RejectionOff;
    }
  }
  // Track from right to left
  Previous = RejectionOff;
  for (int b = OnOff->GetNbinsX(); b >= 1; --b) {
    // Switch off -> on
    if (OnOff->GetBinContent(b) == RejectionOn && Previous == RejectionOff) {
      //cout<<"Found new rejection on at t="<<setprecision(10)<<OnOff->GetBinCenter(b)<<endl;
      Previous = RejectionOn;
      int Max = b+FilteredEventsHalfAverageSmall;
      if (Max > OnOff->GetNbinsX()) Max = OnOff->GetNbinsX();
      int Dist = Max-b;
      while (FilteredRateAverageSmall->GetBinContent(b) < FilteredDecisionThresholdRight*FilteredRateAverageLarge->GetBinContent(Max) &&
             FilteredRate->GetBinContent(b) < SingleRateDecisionThreshold*FilteredRateAverageLarge->GetBinContent(Max) + 3*sqrt(FilteredRateAverageLarge->GetBinContent(Max)) ) {
       //cout<<"At t="<<setprecision(10)<<OnOff->GetBinCenter(b)<<" value: "<<FilteredRateAverageSmall->GetBinContent(b)<<"  threshold: "<<FilteredDecisionThresholdRight*FilteredRateAverageLarge->GetBinContent(Max)<<endl;
        for (int a = b; a <= b+Dist; ++a) OnOff->SetBinContent(a, RejectionOff);
        --b;
        if (b < 1) break;
        if (OnOff->GetBinContent(b) == RejectionOff) break;
      }
    }
    // Switch on -> off
    else if (OnOff->GetBinContent(b) == RejectionOff && Previous == RejectionOn) {
      Previous = RejectionOff;
    }
  }

  
  for (int b = 1; b <= NAcceptedEventsShieldHighCut->GetNbinsX(); ++b) {
    if (OnOff->GetBinContent(b) == RejectionOn) NAcceptedEventsShieldHighCut->SetBinContent(b, 0);
    if (OnOff->GetBinContent(b) == RejectionOff) ShieldRateHighCut->SetBinContent(b, 0);
    if (OnOff->GetBinContent(b) == RejectionOff) {
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
  
  for (int b = 1; b <= OnOff->GetNbinsX(); ++b) {
    int Index = H.FindClosestIndex(OnOff->GetBinCenter(b));
    if (OnOff->GetBinCenter(b) - H.m_Time[Index] < 1.0) {
      if (OnOff->GetBinContent(b) == RejectionOn) {
        if (Mode == c_SAACutOptimized) H.m_SoftSAA[Index] = true;
        H.m_SoftSAAOptimized[Index] = true;
      } else {
        if (Mode == c_SAACutOptimized) H.m_SoftSAA[Index] = false;
        H.m_SoftSAAOptimized[Index] = false;
      }
    } else {
      //cerr<<"Something is wrong with the times as I cannot find housekeeping data... "<<OnOff->GetBinCenter(b)<<endl; 
    }
  }
  
  cout<<"Live times: "<<endl;
  cout<<"No cut: "<<LiveTimeNoCuts<<endl;
  cout<<"Strict cut: "<<LifeTimestrictCuts<<"(Loss: "<<100.0*(LiveTimeNoCuts-LifeTimestrictCuts)/LiveTimeNoCuts<<"%)"<<endl;
  cout<<"Optimized cut: "<<LiveTimeOptimizedCuts<<"(Loss: "<<100.0*(LiveTimeNoCuts-LiveTimeOptimizedCuts)/LiveTimeNoCuts<<"%)"<<endl;
  
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBaseTool::FindSAATentacle(NFilteredEvents& FE, NHousekeeping& HK, NOrbits& O, bool Show) 
{
  // For each Orbits check for each module the count rate in the tentacle region - 
  // if there is an increase flag the region

  cout<<"Finding Tentacle ..."<<endl;
  
  if (HK.m_Time.size() == 0) return true;
  
  int RejectionOn  = 0;
  int RejectionOff = 1;
  
  // First define the time intervals in the tentacle region:
  int Bins = HK.GetMaxTime() - HK.GetMinTime() + 1;
  double MinTime = HK.GetMinTime()-0.5;
  double MaxTime = HK.GetMaxTime()+0.5;
  
  TString iID = TString("_ID_") + HK.m_ID + "_M_" + ((HK.m_Module == 0) ? "A" : "B");
  TString ID = TString(" (ID: ") + HK.m_ID + ((HK.m_Module == 0) ? "A" : "B") + ")";
    
  TH1D* LifeTimes = new TH1D(TString("TentaclesLifeTimes") + iID, TString("TentaclesLifeTimes") + ID, Bins, MinTime, MaxTime);

  TH1D* FilteredRate = new TH1D(TString("TentaclesFilteredRate") + iID, TString("TentaclesFilteredRate") + ID, Bins, MinTime, MaxTime);
  FilteredRate->SetXTitle("Time [sec since 1/1/2010]");
  FilteredRate->SetYTitle("cts/sec");
  
  TH1D* CleanRate = new TH1D(TString("TentaclesCleanRate") + iID, TString("TentaclesCleanRate") + ID, Bins, MinTime, MaxTime);
  CleanRate->SetXTitle("Time [sec since 1/1/2010]");
  CleanRate->SetYTitle("cts/sec");

  TH1D* FilteredRateCut = new TH1D(TString("TentaclesFilteredRateCut") + iID, TString("TentaclesFilteredRateCut") + ID, Bins, MinTime, MaxTime);
  FilteredRateCut->SetLineColor(kRed);
  
  TH1D* FilteredRateAverageLarge = new TH1D(TString("TentaclesFilteredRateAverageLarge") + iID, TString("TentaclesFilteredRateHighLarge") + ID, Bins, MinTime, MaxTime);
  FilteredRateAverageLarge->SetLineColor(kGreen);
  TH1D* FilteredRateAverageSmall = new TH1D(TString("TentaclesFilteredRateAverageSmall") + iID, TString("TentaclesFilteredRateHighSmall") + ID, Bins, MinTime, MaxTime);
  FilteredRateAverageSmall->SetLineColor(kYellow);

  TH1I* OnOff = new TH1I(TString("TentaclesOnOff") + iID, TString("TentaclesOnOff") + ID, Bins, MinTime, MaxTime);

  
  double LiveTimeNoCuts = 0;
  double LiveTimeWithCuts = 0;
  
  
  // Fill:
  for (unsigned int i = 0; i < FE.m_Time.size(); ++i) {
    FilteredRate->Fill(FE.m_Time[i]);
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

    if (LifeTimes->GetBinContent(b) > 0 && HK.m_SoftSAA[HKIndex] == false) {
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
      //cout<<"Case A"<<":"<<FilteredRateAverageLarge->GetBinContent(i)<<":"<<DecisionThresholdLeft*FilteredRateAverageLarge->GetBinContent(DecisionPointLeft)<<endl;
    }
    // ... we have an increase above our decision threshold from right to left
    if (FilteredRateAverageLarge->GetBinContent(DecisionPointRight) > 1 &&
        FilteredRateAverageLarge->GetBinContent(i) > DecisionThresholdRight*FilteredRateAverageLarge->GetBinContent(DecisionPointRight)) {
      Cut = true;
      //cout<<"Case B"<<endl;
    }
    
    // Next we take care of the case if the increase is no longer above the decision threshold,
    // but we are still above the initial increase value:
    
    // If we have both a left and a right value we cut if we are still above the average values
    if (EnteringValueLeft > 0 && EnteringValueRight  > 0) {
      if (FilteredRateAverageLarge->GetBinContent(i) > 0.5*(EnteringValueLeft+EnteringValueRight)) {
        Cut = true;
        //cout<<"Case C"<<endl;
      }
    } 
    // If we have just one value, we only cut on it
    else {
      if ((FilteredRateAverageLarge->GetBinContent(i) > EnteringValueLeft && EnteringValueLeft > 0) ||
          (FilteredRateAverageLarge->GetBinContent(i) > EnteringValueRight && EnteringValueRight  > 0)) {
        Cut = true;
        //cout<<"Case E"<<endl;
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
  
  // (A) If it start or ends outside the tantacle region filter out:
  double LongitudeMin = 250;
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
  int CutOffCount = 3*FilteredEventsHalfAverageLarge; // good: 2x
  
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
        HK.m_SoftTentacled[Index] = true;
      } else {
        HK.m_SoftTentacled[Index] = false;        
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
    cout<<"Live time before cuts: "<<LiveTimeNoCuts<<endl;
    cout<<"Live time after cuts:  "<<LiveTimeWithCuts<<" ("<<100.0*(LiveTimeNoCuts-LiveTimeWithCuts)/LiveTimeNoCuts<<"%)"<<endl;
 
  
  if (Show == true) {
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
  
  cout<<"Done with tentacle"<<endl;
  
  return true;
}


// NBaseTool.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
