/* 
 * LightCurveViewer.cxx
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
#include <TMath.h>
#include <TCanvas.h>
#include <TMarker.h>
#include <TString.h>

// NuSIM
#include "NExtractFitsImage.h"
#include "MFunction2D.h"
#include "NModuleEventLoader.h"
#include "NModuleEventSaver.h"
#include "NModuleTimeEngine.h"
#include "NTime.h"
#include "NSatellite.h"


/******************************************************************************/

class LightCurveViewer
{
public:
  /// Default constructor
  LightCurveViewer();
  /// Default destructor
  ~LightCurveViewer();
  
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
    
  //! number of bins
  int m_NBins;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
LightCurveViewer::LightCurveViewer() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
    
  m_NBins = 100;
}


/******************************************************************************
 * Default destructor
 */
LightCurveViewer::~LightCurveViewer()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool LightCurveViewer::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: LightCurveViewer <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -i:   Input file name"<<endl;
  Usage<<"         -b:   Bins (default: 100)"<<endl;
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
    } else if (Option == "-b") {
      m_NBins = atoi(argv[++i]);
      cout<<"Accepting number of bins: "<<m_NBins<<endl;
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

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool LightCurveViewer::Analyze()
{
  NSatellite Sat;
  Sat.SetTimeModule(new NModuleTimeEngine(Sat));
  
  NModuleEventLoader Loader(Sat);
  Loader.SetFileName(m_InputFileName);
  if (Loader.Initialize() == false) return false;
  
  TH1D* Profile = new TH1D("Light curve", "LightCurve", m_NBins, 
                           0, 
                           Loader.GetAbsoluteObservationEndTime().GetAsSeconds() - Loader.GetAbsoluteObservationStartTime().GetAsSeconds());
  Profile->SetXTitle("Obs. Time [sec]");
  Profile->SetYTitle("cts/sec/mod");
  
  NEvent Event;
  while (Loader.AnalyzeEvent(Event) == true) {
    if (Event.IsEmpty() == true) break; 
    Profile->Fill(Event.GetTime().GetAsSeconds());
  }
  
  // Normalize:
  for (int b= 1; b <= Profile->GetXaxis()->GetNbins(); ++b) {
    Profile->SetBinContent(b, Profile->GetBinContent(b)/2/Profile->GetBinWidth(b));
  }
  
  
  Profile->Draw();

  return true;
}


/******************************************************************************/

LightCurveViewer* g_Prg = 0;

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

  TApplication LightCurveViewerApp("LightCurveViewerApp", 0, 0);

  g_Prg = new LightCurveViewer();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  LightCurveViewerApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
