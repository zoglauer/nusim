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
  if (MFile::Exists(gSystem->ConcatFileName(gSystem->HomeDirectory(), ".nusim.cfg")) == true) {
    m_Supervisor->Load(gSystem->ConcatFileName(gSystem->HomeDirectory(), ".nusim.cfg"));
  } else {
    m_Supervisor->Load("resource/configurations/Ideal.cfg");
  }

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
  Usage<<"      -h --help:"<<endl;
  Usage<<"             You know the answer..."<<endl;
  Usage<<endl;

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
    if (Option == "-c" || Option == "--configuration") {
      if (!((argc > i+1) && argv[i+1][0] != '-')){
        cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    }		
  }
    
  // Now parse all low level options
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "--configuration" || Option == "-c") {
      m_Supervisor->Load(argv[++i]);
      cout<<"Command-line parser: Use configuration file "<<argv[i]<<endl;
    } else if (Option == "--seed" || Option == "-s") {
      gRandom->SetSeed(atoi(argv[++i]));
      cout<<"Command-line parser: Setting seed to "<<argv[i]<<endl;
    } else if (Option == "--auto" || Option == "-a") {
      // Parse later
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
