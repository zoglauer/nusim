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
  Usage<<"         -s:   Start of Phase"<<endl;
  Usage<<"         -d:   Duration of Phase"<<endl;
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
  
  TH1D* Profile = new TH1D("Profile", "Profile", 100, 0, m_PhaseDuration.GetAsSeconds());
  
  NEvent Event;
  while (Loader.AnalyzeEvent(Event) == true) {
    if (Event.IsEmpty() == true) break; 
    NTime Time = Event.GetTime();
    long Cycle = static_cast<int>((Time-m_PhaseStart)/m_PhaseDuration);
    
    Profile->Fill(((Time-m_PhaseStart) - m_PhaseDuration*Cycle).GetAsSeconds());
  }
  
  Profile->Draw();

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
