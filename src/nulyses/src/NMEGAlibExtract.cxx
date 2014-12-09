/*
 * NMEGAlibExtract.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NMEGAlibExtract
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NMEGAlibExtract.h"

// Standard libs:
#include <sstream>
#include <iomanip>
#include <fstream>
#include <limits>
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NTime.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NMEGAlibExtract)
#endif


////////////////////////////////////////////////////////////////////////////////


NMEGAlibExtract::NMEGAlibExtract()
{
  // Construct an instance of NMEGAlibExtract
  
  m_ReadFiltered02 = false;
  
  m_SpectrumMin = 2.0;
  m_SpectrumMax = 150.0;
  
  m_BatchMode = true;
}


////////////////////////////////////////////////////////////////////////////////


NMEGAlibExtract::~NMEGAlibExtract()
{
  // Delete this instance of NMEGAlibExtract
}


////////////////////////////////////////////////////////////////////////////////


bool NMEGAlibExtract::ParseCommandLine(int argc, char** argv)
{
  if (NBaseTool::ParseCommandLine(argc, argv) == false) return false;
  
  string Option;
  
  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NMEGAlibExtract::Analyze() 
{
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    if (Load(m_Directories[d]) == false) continue;
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA, m_UnfilteredEventsA, m_HousekeepingA, m_Orbits, m_Engineering, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB, m_UnfilteredEventsB, m_HousekeepingB, m_Orbits, m_Engineering, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NMEGAlibExtract::Show(NFilteredEvents& F, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Quick view of the data..."<<endl;
  
  // Section A: Create all histograms:

  if (F.m_Time.size() == 0) return false;

  NTime NUSTAREpoch;
  NUSTAREpoch.Set(2010, 1, 1, 0, 0, 0);

  NTime Time(F.m_Time[0]);
  Time += NUSTAREpoch;
  TString TimeString = Time.ToString();
  TimeString = TimeString.ReplaceAll("sec", "");

  ofstream Outs;
  ostringstream Name;
  Name<<F.m_ID<<"_"<<F.m_Module<<"_"<<int(Time.GetAsSeconds())<<".roa";
  Outs.open(Name.str().c_str());
  
  Outs<<"TY roa"<<endl;
  Outs<<"UF single adc"<<endl;
  Outs<<endl;
  
  // Section B: Fill (and normalize) histograms
  double LiveTime = 0;
  
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < F.m_Time.size(); ++e) {

    if (WithinSpecialGTI(F.m_Time[e]) == false) continue;
    if (WithinSpecialBTI(F.m_Time[e]) == true) continue;

    
    int HKIndex = H.FindClosestIndex(F.m_Time[e]);
    if (HKIndex == -1) {
      cout<<"Housekeeping: Index not found for time "<<F.m_Time[e]<<"..."<<endl;
      continue;      
    }
 
    int OrbitIndex = O.FindClosestIndex(F.m_Time[e]);
    if (OrbitIndex == -1) {
      cout<<"Orbit: Index not found for time "<<F.m_Time[e]<<"..."<<endl;
      continue;      
    }
    int i = U.FindIndex(F.m_Time[e]);
    if (i == -1) {
      cout<<"Unfiltered events: Index not found..."<<endl;
      continue;
    }    
    //if (IsGoodEventByExternalDepthFilter(U.m_Status[i]) == false) continue;
    
    int DetectorID = F.m_DetectorID[e];

    Time.Set(F.m_Time[e]);
    Time += NUSTAREpoch;
    TimeString = Time.ToString();
    TimeString = TimeString.ReplaceAll("sec", "");

    Outs<<"SE"<<endl;
    Outs<<"TI "<<TimeString<<endl;
    Outs<<"UH "<<DetectorID<<" "<<F.m_PI[e]<<endl; //0.04*double(valint)+1.6
  }
  
  Outs.close();
  
  return true;
}


// NMEGAlibExtract.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
