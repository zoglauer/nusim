/* 
 * SphericalPattern.cxx
 *
 *
 * Copyright (C) 1998-2010 by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */

// Standard
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <csignal>
#include <map>
using namespace std;

// ROOT
#include <TApplication.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TMarker.h>
#include <TString.h>

// NuSIM
#include "NHousekeeping.h"
#include "NGTI.h"


/******************************************************************************/

class SphericalPattern
{
public:
  /// Default constructor
  SphericalPattern();
  /// Default destructor
  ~SphericalPattern();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Analyze whatever needs to be analyzed...
  bool Analyze();
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }


private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;

  /// Housekeeping file name
  TString m_HKFileName;
  /// Good time intervals file name
  TString m_GTIFileName;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
SphericalPattern::SphericalPattern() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
SphericalPattern::~SphericalPattern()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool SphericalPattern::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: SAAComparer <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -k:   hk file generated with nucalcsaa"<<endl;
  Usage<<"         -g:   good time intervals file generated with nulyses"<<endl;
  Usage<<"         -b:   batch mode"<<endl;
  Usage<<"         -h:   print this help"<<endl;
  Usage<<endl;

  string Option;

  // Check for help
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "-h" || Option == "--help" || Option == "?" || Option == "-?") {
      cout<<Usage.str()<<endl;
      return false;
    }
  }

  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];

    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "-k" || Option == "-g") {
      if (!((argc > i+1) && argv[i+1][0] != '-')){
        cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    } 
    // Multiple arguments_
    //else if (Option == "-??") {
    //  if (!((argc > i+2) && argv[i+1][0] != '-' && argv[i+2][0] != '-')){
    //    cout<<"Error: Option "<<argv[i][1]<<" needs two arguments!"<<endl;
    //    cout<<Usage.str()<<endl;
    //    return false;
    //  }
    //}

    // Then fulfill the options:
    if (Option == "-b") {
      gROOT->SetBatch(true);
    } else if (Option == "-k") {
      m_HKFileName = argv[++i];
      cout<<"Accepting HK file name: "<<m_HKFileName<<endl;
    } else if (Option == "-g") {
      m_GTIFileName = argv[++i];
      cout<<"Accepting GTI file name: "<<m_GTIFileName<<endl;
    } else {
      cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
      cout<<Usage.str()<<endl;
      return false;
    }
  }

  if (m_HKFileName == "") {
    cout<<"Error: Empty file name!"<<endl;
    return false;
  }
  if (MFile::Exists(m_HKFileName) == false) {
    cout<<"Error: File does not exist: "<<m_HKFileName<<endl;
    return false;
  }
  if (m_GTIFileName == "") {
    cout<<"Error: Empty file name!"<<endl;
    return false;
  }
  if (MFile::Exists(m_GTIFileName) == false) {
    cout<<"Error: File does not exist: "<<m_GTIFileName<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool SphericalPattern::Analyze()
{
  // Read the housekeeping file:
  bool Return = true; 
  
  NHousekeeping HK;
  HK.Read(m_HKFileName);
  
  NGTI GTI;
  GTI.Read(m_GTIFileName);
  
   
  // Now create two histograms
  int OneSecBins = HK.GetMaxTime() - HK.GetMinTime() + 1;
  double MinTime = HK.GetMinTime()-0.5;
  double MaxTime = HK.GetMaxTime()+0.5;
  
  TH1D* HKHisto = new TH1D("HK", "HK", OneSecBins, MinTime, MaxTime);
  for (unsigned int i = 0; i < HK.m_Time.size(); ++i) {
    //if (HK.m_SWSAA[i] > 0) cout<<"Found one: "<<setprecision(10)<<HK.m_Time[i]<<":"<<HK.m_SWSAA[i]<<endl;
    int Status = 0;
    if (HK.m_SWSAA[i] == 1 || HK.m_SWTentacled[i] == 1) Status = 1;
    HKHisto->Fill(HK.m_Time[i], Status);
  }
  
  TH1D* GTIHisto = new TH1D("GTI", "GTI", OneSecBins, MinTime, MaxTime);
  GTIHisto->SetLineColor(kRed);
  for (unsigned int i = 0; i < HK.m_Time.size(); ++i) {
    if (GTI.IsGTI(HK.m_Time[i])) {
      GTIHisto->Fill(HK.m_Time[i], 0.8);
    }
  }
  
  
  TCanvas* C = new TCanvas();
  C->cd();
  HKHisto->Draw("HIST");
  GTIHisto->Draw("HIST SAME");
  C->Update();
  
  // Compare
  for (int b = 1; b <= HKHisto->GetNbinsX(); ++b) {
    if ((HKHisto->GetBinContent(b) > 0 && GTIHisto->GetBinContent(b) == 0) ||
        (HKHisto->GetBinContent(b) == 0 && GTIHisto->GetBinContent(b) > 0)) {
      cout<<"Not identical at bin "<<b<<"  (Time: "<<setprecision(10)<<HKHisto->GetBinCenter(b)<<" HK: "<<HKHisto->GetBinContent(b)<<" GTI: "<<GTIHisto->GetBinContent(b)<<")"<<endl;
      Return = false;
    }
  }
  
  if (Return == true) {
    cout<<endl<<"Everything identical :)"<<endl;
  } else {
    cout<<endl<<"Unfortunately there are differences in the files :("<<endl;
  }
  
  return Return;
}


/******************************************************************************/

SphericalPattern* g_Prg = 0;

/******************************************************************************/


/******************************************************************************
 * Called when an interrupt signal is flagged
 * All catched signals lead to a well defined exit of the program
 */
void CatchSignal(int a)
{
  cout<<"Catched signal Ctrl-C (ID="<<a<<"):"<<endl;
  
  if (g_Prg != 0) {
    g_Prg->Interrupt();
  }
}


/******************************************************************************
 * Main program
 */
int main(int argc, char** argv)
{
  //void (*handler)(int);
  //handler = CatchSignal;
  //(void) signal(SIGINT, CatchSignal);


  TApplication* SphericalPatternApp = new TApplication("SphericalPatternApp", 0, 0);

  g_Prg = new SphericalPattern();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  bool Identical = g_Prg->Analyze();

  if (gROOT->IsBatch() == false) {
    SphericalPatternApp->Run();
  } 

  return (Identical == true) ? 0 : 1;
}

/*
 * Cosima: the end...
 ******************************************************************************/
