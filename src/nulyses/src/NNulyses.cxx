/* 
 * Nulyses.cxx
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
#include <sstream>
#include <csignal>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <limits>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <vector>
#include <bitset>
using namespace std;

// ROOT
#include <TROOT.h>
#include <TEnv.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TKey.h>
#include <TIterator.h>
#include <TList.h>
#include <TClass.h>
#include <TF1.h>
#include <TMath.h>
#include <TVirtualFFT.h>

// Nulyses
#include "NGlobal.h"
#include "NAttitude.h"
#include "NOrbits.h"
#include "NEngineering.h"
#include "NFilteredEvents.h"
#include "NUnfilteredEvents.h"
#include "NHousekeeping.h"
#include "NPhaFile.h"
#include "NNulyses.h"
#include "NCheckRates.h"
#include "NQuickView.h"
#include "NQuickViewFiltered.h"
#include "NBackgroundVariations.h"
#include "NBackgroundMode1.h"
#include "NBackgroundMode23.h"
#include "NBackgroundMode4.h"
#include "NBackgroundMode4DataBase.h"
#include "NLineFitter.h"
#include "NDetectorHealth.h"
#include "NApertureTester.h"

// Special
#include "fitsio.h"



/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
Nulyses::Nulyses() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
  gStyle->SetOptStat(0);
}


/******************************************************************************
 * Default destructor
 */
Nulyses::~Nulyses()
{
  // Intentionally left blank
}


/******************************************************************************
 * Parse the command line
 */
bool Nulyses::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: Nulyses <options>"<<endl;
  Usage<<"    Tools:"<<endl;
  Usage<<"      --tool [name of tool]: One of ..."<<endl;
  Usage<<"          quicklook: show a quick look of the data (position, spectra, rates) of the unfiltered data"<<endl;
  Usage<<"          quicklookfiltered: show a quick look of the data (position, spectra, rates) of the unfiltered data"<<endl;
  Usage<<"          checkrates: Check SAA rates"<<endl;
  Usage<<"          backgroundmode1: Background rates for mode 1"<<endl;
  Usage<<"          backgroundmode23: Background rates for modes 2 & 3"<<endl;
  Usage<<"          backgroundmode4: Background rates for mode 4"<<endl;
  Usage<<"          backgroundmode4database: Create data base for mode 4"<<endl;
  Usage<<"          linefitter: Check SAA rates"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"      -d:  directory containing all files"<<endl;
  Usage<<"      -dp: directory containing all files plus exclusion region ([data directory] [x center of exclusion region on A in detector pixels] [y center of exclusion region on A in detector pixels] [radius of exclusion region on A] [x center of exclusion region on B in detector pixels] [y center of exclusion region on B in detector pixels]  [radius of exclusion region on B]"<<endl;
  Usage<<"      -g:  good (mandatory) time interval min max time"<<endl;
  Usage<<"      -b:  min max time"<<endl;
  Usage<<"      -s:  show the histograms: s on screen (default), f to file, can be combined, -s \" \" will don't show anything"<<endl;
  Usage<<"      -h:  print this help"<<endl;
  Usage<<"    Tool specific options:"<<endl;
  Usage<<"      checkrates:"<<endl;
  Usage<<"        --write-gti: Optional - dump a fits based gti file"<<endl;
  Usage<<"      backgroundmodeX:"<<endl; 
  Usage<<"        -dps: directory containing all files plus exclusion region plus pha file ([data directory] [x center of exclusion region on A in detector pixels] [y center of exclusion region on A in detector pixels] [radius of exclusion region on A] [source pha file for A] [x center of exclusion region on B in detector pixels] [y center of exclusion region on B in detector pixels] [radius of exclusion region on B] [source pha file for B]"<<endl;
  Usage<<"        --fit-range [min] [max]: The minimum and maximum energy used for the fit"<<endl;
  Usage<<"      backgroundmode4:"<<endl; 
  Usage<<"        --names [DB_A.root] [CB_B.root]: Mandatory - Give the names of the two database files"<<endl;
  Usage<<"        --dump-one-spectrum-per-pixel: Optional - Dumps one spectrum per pixel and detector as fits file"<<endl;
  Usage<<endl;

  string Option;

  // Check for help
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "--help" || Option == "?" || Option == "-?") {
      cout<<Usage.str()<<endl;
      return false;
    }
  }
  
  // Check for a tool
  bool FoundTool = false;
  for (int i = 1; i < argc; i++) {
    Option = argv[i];

    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "--tool") {
      if (!((argc > i+1) && 
            (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    } 

    // Then fulfill the options:
    if (Option == "--tool") {
      string Tool = argv[++i];
      FoundTool = true;
      if (Tool == "checkrates") {
        cout<<"Found tool: "<<Tool<<endl;
        NCheckRates R;
        if (R.ParseCommandLine(argc, argv) == false) return false;
        if (R.Analyze() == false) return false; 
        return true;
      } else if (Tool == "detectorhealth") {
        cout<<"Found tool: "<<Tool<<endl;
        NDetectorHealth D;
        if (D.ParseCommandLine(argc, argv) == false) return false;
        if (D.Analyze() == false) return false; 
        return true;
      } else if (Tool == "quickview" || Tool == "quicklook") {
        cout<<"Found tool: "<<Tool<<endl;
        NQuickView Q;
        if (Q.ParseCommandLine(argc, argv) == false) return false;
        if (Q.Analyze() == false) return false; 
        return true;
      } else if (Tool == "quickviewfiltered" || Tool == "quicklookfiltered") {
        cout<<"Found tool: "<<Tool<<endl;
        NQuickViewFiltered Q;
        if (Q.ParseCommandLine(argc, argv) == false) return false;
        if (Q.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundmode1" || Tool == "backgroundmode123") {
        cout<<"Found tool: "<<Tool<<endl;
        NBackgroundMode1 B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundmode23") {
        cout<<"Found tool: "<<Tool<<endl;
        NBackgroundMode23 B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundmode4") {
        cout<<"Found tool: "<<Tool<<endl;
        NBackgroundMode4 B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundmode4database") {
        cout<<"Found tool: "<<Tool<<endl;
        NBackgroundMode4DataBase B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundvariations") {
        cout<<"Found tool: "<<Tool<<endl;
        NBackgroundVariations B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else if (Tool == "linefitter") {
        cout<<"Found tool: "<<Tool<<endl;
        NLineFitter F;
        if (F.ParseCommandLine(argc, argv) == false) return false;
        if (F.Analyze() == false) return false; 
        return true;
      } else if (Tool == "aperturetester") {
        cout<<"Found tool: "<<Tool<<endl;
        NApertureTester T;
        if (T.ParseCommandLine(argc, argv) == false) return false;
        if (T.Analyze() == false) return false; 
        return true;
      } else {
        cout<<"Error: Unknown tool: "<<Tool<<"!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    }
  }
  
  if (FoundTool == false) {
    cout<<"Error: No tool found!"<<endl;
    cout<<Usage.str()<<endl;
    return false;
  }
    
  
  return true;
}
  

/******************************************************************************/

Nulyses* g_Prg = 0;
int g_NInterruptCatches = 1;

/******************************************************************************/


/******************************************************************************
 * Called when an interrupt signal is flagged
 * All catched signals lead to a well defined exit of the program
 */
void CatchSignal(int a)
{
  if (g_Prg != 0 && g_NInterruptCatches-- > 0) {
    cout<<"Catched signal Ctrl-C (ID="<<a<<"):"<<endl;
    g_Prg->Interrupt();
  } else {
    abort();
  }
}


/******************************************************************************
 * Main program
 */
int main(int argc, char** argv)
{
  // Catch a user interupt for graceful shutdown
  // signal(SIGINT, CatchSignal);

  // Initialize global MEGALIB variables, especially mgui, etc.
  NGlobal::Initialize();

  TApplication NulysesApp("NulysesApp", 0, 0);

  g_Prg = new Nulyses();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 

  if (gROOT->IsBatch() == false) {
    NulysesApp.Run();
  }

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
