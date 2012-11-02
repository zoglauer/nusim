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
#include "NBackgroundMode1.h"
#include "NBackgroundMode23.h"
#include "NBackgroundMode4.h"
#include "NBackgroundMode4DataBase.h"

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
  Usage<<"          background1: Background rates for mode 1"<<endl;
  Usage<<"          background23: Background rates for modes 2 & 3"<<endl;
  Usage<<"          background4: Background rates for mode 4"<<endl;
  Usage<<"          background4database: Create data base for mode 4"<<endl;
  Usage<<"          checkrates: Check SAA rates"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"      -d:  directory containing all files"<<endl;
  Usage<<"      -dp: directory containing all files plus exclusion region ([data directory] [x center of exclusion region on A in detector pixels] [y center of exclusion region on A in detector pixels] [radius of exclusion region on A] [x center of exclusion region on B in detector pixels] [y center of exclusion region on B in detector pixels]  [radius of exclusion region on B]"<<endl;
  Usage<<"      -g:  good time interval min max time"<<endl;
  Usage<<"      -b:  min max time"<<endl;
  Usage<<"      -h:  print this help"<<endl;
  Usage<<"    Tool specific options:"<<endl;
  Usage<<"      checkrates:"<<endl;
  Usage<<"        --write-gti:  dump a fits based gti file"<<endl;
  Usage<<"      background123 & background 4:"<<endl; 
  Usage<<"        -dps: directory containing all files plus exclusion region plus pha file ([data directory] [x center of exclusion region on A in detector pixels] [y center of exclusion region on A in detector pixels] [radius of exclusion region on A] [source pha file for A] [x center of exclusion region on B in detector pixels] [y center of exclusion region on B in detector pixels] [radius of exclusion region on B] [source pha file for B]"<<endl;
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
      if (Tool == "checkrates") {
        NCheckRates R;
        if (R.ParseCommandLine(argc, argv) == false) return false;
        if (R.Analyze() == false) return false; 
        return true;
      } else if (Tool == "quickview" || Tool == "quicklook") {
        NQuickView Q;
        if (Q.ParseCommandLine(argc, argv) == false) return false;
        if (Q.Analyze() == false) return false; 
        return true;
      } else if (Tool == "quickviewfiltered" || Tool == "quicklookfiltered") {
        NQuickViewFiltered Q;
        if (Q.ParseCommandLine(argc, argv) == false) return false;
        if (Q.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundmode1" || Tool == "backgroundmode123") {
        NBackgroundMode1 B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundmode23") {
        NBackgroundMode23 B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundmode4") {
        NBackgroundMode4 B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else if (Tool == "backgroundmode4database") {
        NBackgroundMode4DataBase B;
        if (B.ParseCommandLine(argc, argv) == false) return false;
        if (B.Analyze() == false) return false; 
        return true;
      } else {
        cout<<"Error: Unknown tool: "<<argv[i]<<"!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    }
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

  NulysesApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
