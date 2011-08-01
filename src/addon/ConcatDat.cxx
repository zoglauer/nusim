/* 
 * ConcatDat.cxx
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

class ConcatDat
{
public:
  /// Default constructor
  ConcatDat();
  /// Default destructor
  ~ConcatDat();
  
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
  vector<TString> m_InputFileNames;
 
  /// Output file name
  TString m_OutputFileName;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
ConcatDat::ConcatDat() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
ConcatDat::~ConcatDat()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool ConcatDat::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: ConcatDat <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -i:   Input file name"<<endl;
  Usage<<"         -o:   Output file name"<<endl;
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
		//	if (!((argc > i+2) && argv[i+1][0] != '-' && argv[i+2][0] != '-')){
		//		cout<<"Error: Option "<<argv[i][1]<<" needs two arguments!"<<endl;
		//		cout<<Usage.str()<<endl;
		//		return false;
		//	}
		//}

		// Then fulfill the options:
    if (Option == "-i") {
      m_InputFileNames.push_back(argv[++i]);
			cout<<"Accepting input file name: "<<m_InputFileNames.back()<<endl;
    } else if (Option == "-o") {
      m_OutputFileName = argv[++i];
			cout<<"Accepting output file name: "<<m_OutputFileName<<endl;
    } else {
			cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
			cout<<Usage.str()<<endl;
			return false;
		}
  }

  if (m_OutputFileName == "") {
	  cout<<"Error: Empty output file name!"<<endl;
    return false;
  }
  if (m_InputFileNames.size() == 0) {
	  cout<<"Error: You need to give some input file names!"<<endl;
    return false;
  }
  for (unsigned int f = 0; f < m_InputFileNames.size(); ++f) {
    if (MFile::Exists(m_InputFileNames[f]) == false) {
	    cout<<"Error: File does not exist: "<<m_InputFileNames[f]<<endl;
      return false;
    }
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool ConcatDat::Analyze()
{
  NSatellite Sat;
  Sat.SetTimeModule(new NModuleTimeEngine(Sat));
  
  // Retrieve the observation start and end times:
  NModuleEventLoader LoaderStart(Sat);
  LoaderStart.SetFileName(m_InputFileNames[0]);
  if (LoaderStart.Initialize() == false) return false;
  Sat.SetAbsoluteObservationStartTime(LoaderStart.GetAbsoluteObservationStartTime());
  int ChosenType = LoaderStart.GetChosenModuleType();
  LoaderStart.Finalize();
  
  NModuleEventLoader LoaderEnd(Sat);
  LoaderEnd.SetFileName(m_InputFileNames.back());
  if (LoaderEnd.Initialize() == false) return false;
  Sat.SetAbsoluteObservationEndTime(LoaderEnd.GetAbsoluteObservationEndTime());
  LoaderEnd.Finalize();
  
  // Set up the saver
  NModuleEventSaver Saver(Sat);
  Saver.SetFileName(m_OutputFileName);
  Saver.SetChosenModuleType(ChosenType);
  if (Saver.Initialize() == false) return false;

  // Loop over all files, extrcat and save the events into the new file
  unsigned long IDOffset = 0;
  for (unsigned int f = 0; f < m_InputFileNames.size(); ++f) {
    NModuleEventLoader Loader(Sat);
    Loader.SetFileName(m_InputFileNames[f]);
    if (Loader.Initialize() == false) return false;
    
    NEvent Event;
    unsigned long LatestID = 0;
    while (Loader.AnalyzeEvent(Event) == true) {
      if (Event.IsEmpty() == true) break; 
     
      LatestID = Event.GetID();
      Event.SetID(Event.GetID() + IDOffset);
      Saver.AnalyzeEvent(Event);
    }
    IDOffset += LatestID; 
    Loader.Finalize();
  }
  Saver.Finalize();

  return true;
}


/******************************************************************************/

ConcatDat* g_Prg = 0;

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

  TApplication ConcatDatApp("ConcatDatApp", 0, 0);

  g_Prg = new ConcatDat();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  //ConcatDatApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
