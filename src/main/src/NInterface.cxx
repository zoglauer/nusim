/*
 * NInterface.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NInterface
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NInterface.h"

// Standard libs:
#include <iostream>
#include <sstream>
#include <limits>
#include <vector>
using namespace std;

// ROOT libs:
#include "TROOT.h"
#include "TString.h"
#include "TCanvas.h"
#include "TView.h"
#include "TGMsgBox.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TRandom.h"

// MEGAlib libs:
#include "NGlobal.h"
#include "MAssert.h"
#include "MStreams.h"
#include "MFile.h"

// NuSTAR libs:
#include "NGUIMain.h"
#include "NGUIDiagnostics.h"
#include "NGUIDiagnosticsMain.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceEntry.h"
#include "NSupervisor.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NInterface)
#endif


////////////////////////////////////////////////////////////////////////////////


NInterface::NInterface()
{
  // standard constructor

  gRandom->SetSeed(0);

  m_Gui = 0;
  m_UseGui = true;

  m_Supervisor = new NSupervisor();

  m_Interrupt = false;
}



////////////////////////////////////////////////////////////////////////////////


NInterface::~NInterface()
{
  // standard destructor

  delete m_Supervisor;
}


////////////////////////////////////////////////////////////////////////////////


bool NInterface::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: Nuclearizer <options>"<<endl;
  Usage<<endl;
  Usage<<"      -c --configuration <filename>.cfg:"<<endl;
  Usage<<"             Use this file as configuration file."<<endl;
  Usage<<"             If no configuration file is give ~/.nuclearizer.xml.cfg is used"<<endl;
  Usage<<"      -a --auto:"<<endl;
  Usage<<"             Automatically start analysis without GUI"<<endl;
  Usage<<"      -s --seed <integer>:"<<endl;
  Usage<<"             Set the seed for the random number generator (zero results in a random seed)"<<endl;
  Usage<<"         --start-stop-time <double> <double>:"<<endl;
  Usage<<"             Set an observation start and stop time within the limits of the given observation time"<<endl;
  Usage<<"             This is mainly used for parallel simulations with pnusim!"<<endl;
  Usage<<"      -r --mode-restricted <filename>.cfg:"<<endl;
  Usage<<"             Use a restricted mode which only enables certain modules from the file given with -c and the others from the file given with -r"<<endl;
  Usage<<"      -p --mode-astrophysics:"<<endl;
  Usage<<"             Use a restricted mode which only enables modules relevant for astrophysics"<<endl;
  Usage<<"      -h --help:"<<endl;
  Usage<<"             You know the answer..."<<endl;
  Usage<<endl;

  // Dump command line
  cout<<"Command-line parser: Options: ";
  for (int i = 0; i < argc; i++) {
    cout<<argv[i]<<" ";
  }
  cout<<endl;
  
  
  // Store some options temporarily:
  TString Option;

  // Check for help
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "-h" || Option == "--help" || Option == "?" || Option == "-?") {
      cout<<Usage.str()<<endl;
      return false;
    }
  }

  // First check if all options are ok:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];

    // Single argument
    if (Option == "-c" || Option == "--configuration" ||
        Option == "-s" || Option == "--seed") {
      if (!((argc > i+1) && argv[i+1][0] != '-')){
        cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    } 
    // Double argument
    if (Option == "--start-stop-time") {
      if (!((argc > i+2) && argv[i+1][0] != '-' && argv[i+2][0] != '-')){
         cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
         cout<<Usage.str()<<endl;
         return false;
       }
    }
  }
    
  // First parse all high level options
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "--mode-restricted" || Option == "-r") {
      TString FileName = argv[++i];
      m_Supervisor->SetRestrictedMode(true, FileName);
      cout<<"Command-line parser: Enabling restricted mode with file: "<<FileName<<endl;
    } else if (Option == "--mode-astrophysics" || Option == "-p") {
      m_Supervisor->SetRestrictedMode(true);
      cout<<"Command-line parser: Enabling astrophysics mode"<<endl;
    }
  }
    
  // Now parse all low level options
  bool ConfigurationLoaded = false;
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "--configuration" || Option == "-c") {
      ConfigurationLoaded = m_Supervisor->Load(argv[++i]);
      cout<<"Command-line parser: Use configuration file "<<argv[i]<<endl;
    } else if (Option == "--seed" || Option == "-s") {
      gRandom->SetSeed(atoi(argv[++i]));
      cout<<"Command-line parser: Setting seed to "<<argv[i]<<endl;
    } else if (Option == "--start-stop-time") {
      double Start = atof(argv[++i]);
      double Stop = atof(argv[++i]);
      m_Supervisor->SetObservationStartStopTime(Start, Stop);
      cout<<"Command-line parser: Enabling observation start and stop time for parallel simulations"<<endl;
    } else if (Option == "--auto" || Option == "-a") {
      // Parse later
    }
  }
  if (ConfigurationLoaded == false) {
    if (MFile::Exists(gSystem->ConcatFileName(gSystem->HomeDirectory(), ".nusim.cfg")) == true) {
      m_Supervisor->Load(gSystem->ConcatFileName(gSystem->HomeDirectory(), ".nusim.cfg"));
    } else {
      m_Supervisor->Load("resource/configurations/Ideal.cfg");
    }
  }

    // Now parse all high level options
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "--auto" || Option == "-a") {
      m_UseGui = false;
      gROOT->SetBatch(true);
      Analyze();
      Exit();
      return false;
    }
  }

  if (m_UseGui == true) {
    m_Gui = new NGUIMain(this, m_Supervisor);
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NInterface::Analyze()
{
  // Analyze the data

  return m_Supervisor->Run();
}


////////////////////////////////////////////////////////////////////////////////


void NInterface::Save()
{
  // Exit the application

  m_Supervisor->Save(gSystem->ConcatFileName(gSystem->HomeDirectory(), ".nusim.cfg"));
}


////////////////////////////////////////////////////////////////////////////////


void NInterface::Exit()
{
  // Exit the application

  Save();
  gApplication->Terminate(0);
}


// NInterface: the end...
////////////////////////////////////////////////////////////////////////////////
