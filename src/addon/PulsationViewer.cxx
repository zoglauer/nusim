/* 
 * PulsationViewer.cxx
 *
 *
 * Copyright (C) by Andreas Zoglauer.
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
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TMarker.h>
#include <TString.h>

// NuSIM
#include "NExtractFitsImage.h"
#include "MFunction.h"
#include "NModuleEventLoader.h"
#include "NModuleEventSaver.h"
#include "NModuleTimeEngine.h"
#include "NTime.h"
#include "NSatellite.h"


/******************************************************************************/

/// Input file names
MFunction g_ComparisonLightCurve;

double LightCurveFitFunction(double* x, double* par)
{
  // 0: Height
  // 1: Function scaler

  return par[0] + par[1]*g_ComparisonLightCurve.Eval(x[0]);
}


/******************************************************************************/

class PulsationViewer
{
public:
  /// Default constructor
  PulsationViewer();
  /// Default destructor
  ~PulsationViewer();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Analyze whatever needs to be analyzed...
  bool Analyze();
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }


private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;

  /// Input file names
  TString m_InputFileName;
  
  /// Start of a typical phase
  NTime m_PhaseStart;
  /// Duration of a typical phase
  NTime m_PhaseDuration;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
PulsationViewer::PulsationViewer() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
  
  m_PhaseStart.SetMax();
  m_PhaseDuration.SetMax();
}


/******************************************************************************
 * Default destructor
 */
PulsationViewer::~PulsationViewer()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool PulsationViewer::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: PulsationViewer <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -i:   Input file name"<<endl;
  Usage<<"         -d:   Duration of Phase (in sec)"<<endl;
  Usage<<"         -s:   Start of Phase (default: 0 sec)"<<endl;
  Usage<<"         -c:   File for comparison (if skipped: no comparison)"<<endl;
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
    if (Option == "-i" || Option == "-o") {
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
    if (Option == "-i") {
      m_InputFileName = argv[++i];
      cout<<"Accepting input file name: "<<m_InputFileName<<endl;
    } else if (Option == "-c") {
      TString ComparisonFileName = argv[++i];
      cout<<"Accepting input file name: "<<ComparisonFileName<<endl;
      if (MFile::Exists(ComparisonFileName) == true) {
        if (g_ComparisonLightCurve.Set(ComparisonFileName, "DP") == false) {
          mout<<"LightCurveFile: Unable to load light curve!"<<endl;
          return false;
        }
      } else {
        return false;
      }
    } else if (Option == "-s") {
      m_PhaseStart.Set(atof(argv[++i]));
      cout<<"Accepting start of phase: "<<m_PhaseStart<<endl;
    } else if (Option == "-d") {
      m_PhaseDuration.Set(atof(argv[++i]));
      cout<<"Accepting duration of phase: "<<m_PhaseDuration<<endl;
    } else {
      cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
      cout<<Usage.str()<<endl;
      return false;
    }
  }

  if (MFile::Exists(m_InputFileName) == false) {
    cout<<"Error: Input file name does not exist!"<<endl;
    return false;
  }

  if (m_PhaseStart.IsMax() == true) {
    cout<<"Error: You need to give a phase start!"<<endl;
    return false;
  }

  if (g_ComparisonLightCurve.GetSize() > 0) {
    cout<<"Aligning the phase start with the phase start in the comparison file..."<<endl;
    m_PhaseStart.Set(g_ComparisonLightCurve.GetXMin());
  }
  
  if (m_PhaseDuration.IsMax() == true) {
    cout<<"Error: You need to give a phase duration!"<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool PulsationViewer::Analyze()
{
  NSatellite Sat;
  Sat.SetTimeModule(new NModuleTimeEngine(Sat));
  
  NModuleEventLoader Loader(Sat);
  Loader.SetFileName(m_InputFileName);
  if (Loader.Initialize() == false) return false;
  NTime ObsTime = Sat.GetEffectiveObservationTime();
  
  double DeadTime = 0.0025;
  cout<<"Attention: Using a hard-coded dead time of "<<DeadTime<<" sec."<<endl;
  
  // Step 1: Create the initial uncorrected pulse and dead time profiles by loading them from file
  
  // The bin size shuld be >= 0.1*DeadTime to get a correct dead-time correction,
  // without having to account for half filled pixels.
  int NBins = m_PhaseDuration.GetAsSeconds()/(0.1*DeadTime);
  double BinWidth = m_PhaseDuration.GetAsSeconds()/NBins;
  
  TH1D* Profile1 = new TH1D("Profile1", "Pulse profile (telescope 1 only)", NBins, 0, m_PhaseDuration.GetAsSeconds());
  Profile1->SetXTitle("phase time [sec]");
  Profile1->SetYTitle("Counts");
  TH1D* Profile2 = new TH1D("Profile2", "Pulse profile (telescope 2 only)", NBins, 0, m_PhaseDuration.GetAsSeconds());
  Profile2->SetXTitle("phase time [sec]");
  Profile2->SetYTitle("Counts");
  
  TH1D* Profile1DeadTime = new TH1D("Profile1DeadTime", "Profile of dead time (telescope 1 only)", NBins, 0, m_PhaseDuration.GetAsSeconds());
  TH1D* Profile2DeadTime = new TH1D("Profile2DeadTime", "Profile of dead time (telescope 2 only)", NBins, 0, m_PhaseDuration.GetAsSeconds());
  
  NEvent Event;
  while (Loader.AnalyzeEvent(Event) == true) {
    if (Event.IsEmpty() == true) break;
    NTime Time = Event.GetTime();
    long Cycle = static_cast<long>((Time-m_PhaseStart)/m_PhaseDuration);
    
    // Fill the event at the correct phase time and for the correct pixels
    double PhaseTime = ((Time-m_PhaseStart) - m_PhaseDuration*Cycle).GetAsSeconds();
    if (Event.GetTelescope() == 1) {
      Profile1->Fill(PhaseTime);
    } else {
      Profile2->Fill(PhaseTime);     
    }
    
    // Now fill the dead time histogram:
    // Start at the current bin and fill each bin with the value 1 until we have exceeded the dead-time
    double CurrentTime = PhaseTime;
    while (CurrentTime < PhaseTime + DeadTime) {
      double FillTime = CurrentTime;
      while (FillTime > m_PhaseDuration.GetAsSeconds()) FillTime -= m_PhaseDuration.GetAsSeconds();
      if (Event.GetTelescope() == 1) {
        Profile1DeadTime->Fill(FillTime);
      } else {
        Profile2DeadTime->Fill(FillTime);       
      }
      CurrentTime += BinWidth;
    }
  }
  
  //TCanvas* ProfileCanvas = new TCanvas();
  //ProfileCanvas->cd();
  //Profile1->Draw();
  //ProfileCanvas->Update();
  
  //TCanvas* ProfileDeadTimeCanvas = new TCanvas();
  //ProfileDeadTimeCanvas->cd();
  //Profile1DeadTime->Draw();
  //ProfileDeadTimeCanvas->Update();

  // Step 2: Correct for dead time:
  
  // We know how many phases we have within our observation time, 
  // and we know in how many phases we had a dead time in the given bin:
  // Thus we can easily calculate the lifetime as
  // (# Cycles - # Dead time counts) / # Cycles
  
  double Cycles = ObsTime.GetAsSeconds()/m_PhaseDuration.GetAsSeconds();
  TH1D* Profile1LifeTimeRatio = new TH1D("Profile1LifeTimeRatio", "Life time (telescope 1 only)", NBins, 0, m_PhaseDuration.GetAsSeconds());
  Profile1LifeTimeRatio->SetXTitle("phase time [sec]");
  Profile1LifeTimeRatio->SetYTitle("Life time (100% = 1)");
  TH1D* Profile2LifeTimeRatio = new TH1D("Profile2LifeTimeRatio", "Life time (telescope 2 only)", NBins, 0, m_PhaseDuration.GetAsSeconds());
  Profile2LifeTimeRatio->SetXTitle("phase time [sec]");
  Profile2LifeTimeRatio->SetYTitle("Life time (100% = 1)");
  for (int bx = 1; bx <= NBins; ++bx) {
    Profile1LifeTimeRatio->SetBinContent(bx, (Cycles-Profile1DeadTime->GetBinContent(bx))/Cycles);
    Profile2LifeTimeRatio->SetBinContent(bx, (Cycles-Profile2DeadTime->GetBinContent(bx))/Cycles);
  }
  //TCanvas* ProfileLifeTimeRatioCanvas = new TCanvas();
  //ProfileLifeTimeRatioCanvas->cd();
  //ProfileLifeTimeRatio->Draw();
  //ProfileLifeTimeRatioCanvas->Update();

  // Now correct the profile with the life-time and add up both histograms
  
  TH1D* ProfileLifeTimeCorrected = new TH1D("ProfileLifeTimeCorrected", "Life-time corrected pulse profile (telescope 1 & 2)", NBins, 0, m_PhaseDuration.GetAsSeconds());
  ProfileLifeTimeCorrected->SetXTitle("phase time [sec]");
  ProfileLifeTimeCorrected->SetYTitle("Life-time corrected counts");
  for (int bx = 1; bx <= NBins; ++bx) {
    ProfileLifeTimeCorrected->SetBinContent(bx, Profile1->GetBinContent(bx)/Profile1LifeTimeRatio->GetBinContent(bx) + Profile2->GetBinContent(bx)/Profile2LifeTimeRatio->GetBinContent(bx));
  }
  TCanvas* ProfileLifeTimeCorrectedCanvas = new TCanvas();
  ProfileLifeTimeCorrectedCanvas->cd();
  ProfileLifeTimeCorrected->Draw();
  ProfileLifeTimeCorrectedCanvas->Update();

  // Step 3: (optional) Fit the original light curve
  
  if (g_ComparisonLightCurve.GetSize() > 0) {
    TF1* Fit = new TF1("LightCurveFit", LightCurveFitFunction, 0, m_PhaseDuration.GetAsSeconds(), 2);

    Fit->SetParNames("Offset", "Scaler");
    Fit->SetParameters(1, 1);

    ProfileLifeTimeCorrectedCanvas->cd();
    ProfileLifeTimeCorrected->Fit(Fit, "RQI");
    Fit->Draw("SAME");
    ProfileLifeTimeCorrectedCanvas->Modified();
    ProfileLifeTimeCorrectedCanvas->Update();
  }
  
  return true;
}


/******************************************************************************/

PulsationViewer* g_Prg = 0;

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

  // Initialize global NuSIM variables, especially mgui, etc.
  NGlobal::Initialize();

  TApplication PulsationViewerApp("PulsationViewerApp", 0, 0);

  g_Prg = new PulsationViewer();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  PulsationViewerApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
