/* 
 * ExtractSpectrum.cxx
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
#include "NTime.h"
#include "NSatellite.h"


/******************************************************************************/

class ExtractSpectrum
{
public:
  /// Default constructor
  ExtractSpectrum();
  /// Default destructor
  ~ExtractSpectrum();
  
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
  /// Output file names
  TString m_OutputFileName;
 
  //! RA
  vector<double> m_RAs; //deg
  //! DEC
  vector<double> m_DECs; // deg
  //! Radius
  vector<double> m_Radii; // deg
  //! Direction
  vector<MVector> m_Directions; // deg
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
ExtractSpectrum::ExtractSpectrum() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
ExtractSpectrum::~ExtractSpectrum()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool ExtractSpectrum::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: ExtractSpectrum <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -i:   Input file name"<<endl;
  Usage<<"         -p:   Position: RA, DEC, Radius (all in deg)"<<endl;
  Usage<<"         -o:   Write a spectrum to this file (don't add a suffix)"<<endl;
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
    if (Option == "-i") {
			if (!((argc > i+1) && argv[i+1][0] != '-')){
				cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
				cout<<Usage.str()<<endl;
				return false;
			}
		} 
		/*
		// Multiple arguments_
		else if (Option == "-p") {
			if (!((argc > i+3) && argv[i+1][0] != '-' && argv[i+2][0] != '-' && argv[i+3][0] != '-')) {
				cout<<"Error: Option "<<argv[i][1]<<" needs three arguments!"<<endl;
				cout<<Usage.str()<<endl;
				return false;
			}
		}
    */
    
		// Then fulfill the options:
    if (Option == "-i") {
      m_InputFileName = argv[++i];
      cout<<"Accepting input file name: "<<m_InputFileName<<endl;
    } else if (Option == "-o") {
      m_OutputFileName = argv[++i];
      cout<<"Accepting output file name: "<<m_OutputFileName<<endl;
    } else if (Option == "-p") {
      m_RAs.push_back(atof(argv[++i]));
      m_DECs.push_back(atof(argv[++i]));
      MVector Dir;
      Dir.SetMagThetaPhi(1.0, m_RAs.back()*c_Rad, m_DECs.back()*c_Rad+c_Pi/2);
      m_Directions.push_back(Dir);
      m_Radii.push_back(atof(argv[++i]));
			cout<<"Accepting position: "<<m_RAs.back()<<", "<<m_DECs.back()<<", "<<m_Radii.back()<<endl;
    } else {
			cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
			cout<<Usage.str()<<endl;
			return false;
		}
  }


  if (m_InputFileName == "" || MFile::Exists(m_InputFileName) == false) {
	  cout<<"Error: You need to give a valid input file names!"<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool ExtractSpectrum::Analyze()
{
  TH1D* GeneralEnergyHist = new TH1D("GeneralEnergyHist", "GeneralEnergyHist", 200, 0, 100);
  GeneralEnergyHist->SetXTitle("keV");
  GeneralEnergyHist->SetYTitle("cts/keV/sec");
  
  vector<TH1D*> SelectedHists;
  for (unsigned int i = 0; i < m_RAs.size(); ++i) {
    ostringstream Name;
    Name<<"Sel ("<<m_RAs[i]<<", "<<m_DECs[i]<<", "<<m_Radii[i]<<")";
    TH1D* Hist = new TH1D(Name.str().c_str(), Name.str().c_str(), 200, 0, 100);
    Hist->SetMinimum(0);
    Hist->SetXTitle("keV");
    Hist->SetYTitle("cts/keV/sec");

    SelectedHists.push_back(Hist);
  }
  
  NSatellite Sat;
  
  NModuleEventLoader Loader(Sat);
  Loader.SetFileName(m_InputFileName);
  if (Loader.Initialize() == false) return false;
    
  NEvent Event;

  NTime LastTime;
  while (Loader.AnalyzeEvent(Event) == true) {
    if (Event.IsEmpty() == true) break; 
      
    for (unsigned int h = 0; h < Event.GetNHits(); ++h) {
      NHit& Hit = Event.GetHitRef(h);
      LastTime = Event.GetTime();
      GeneralEnergyHist->Fill(Hit.GetEnergy());
      for (unsigned int i = 0; i < m_RAs.size(); ++i) {
        MVector DirPhoton;
        DirPhoton.SetMagThetaPhi(1.0, Hit.GetObservatoryData().GetRa()/rad, Hit.GetObservatoryData().GetDec()/rad + c_Pi/2);
        cout<<Hit.GetObservatoryData().GetRa()/deg<<":"<<Hit.GetObservatoryData().GetDec()/deg<<endl;
        if (DirPhoton.Angle(m_Directions[i]) <= m_Radii[i]*c_Rad) {
          SelectedHists[i]->Fill(Hit.GetEnergy());
        }
      }
    }
  }

  for (int b = 1; b <= GeneralEnergyHist->GetXaxis()->GetNbins(); ++b) {
    GeneralEnergyHist->SetBinContent(b, GeneralEnergyHist->GetBinContent(b)/LastTime.GetSeconds()/GeneralEnergyHist->GetXaxis()->GetBinWidth(b));
  }

  TCanvas* GeneralEnergyCanvas = new TCanvas();
  GeneralEnergyCanvas->cd();
  GeneralEnergyHist->Draw();
  GeneralEnergyCanvas->Update();

  
  
  for (unsigned int i = 0; i < m_RAs.size(); ++i) {
    for (int b = 1; b <= GeneralEnergyHist->GetXaxis()->GetNbins(); ++b) {
      SelectedHists[i]->SetBinContent(b, SelectedHists[i]->GetBinContent(b)/LastTime.GetSeconds()/GeneralEnergyHist->GetXaxis()->GetBinWidth(b));
    }
    TCanvas* Canvas = new TCanvas();
    Canvas->cd();
    SelectedHists[i]->Draw();
    Canvas->Update();
    
    if (m_OutputFileName != "") {
      TString Name = m_OutputFileName;
      Name += ".RA=";
      Name += m_RAs[i];
      Name += ".DEC=";
      Name += m_DECs[i];
      Name += ".csv";
      ofstream out;
      out.open(Name.Data());
      for (int b = 1; b <= SelectedHists[i]->GetXaxis()->GetNbins(); ++b) {
        out<<SelectedHists[i]->GetXaxis()->GetBinCenter(b)<<" "<<SelectedHists[i]->GetBinContent(b)<<endl;
      }
      out.close();
    }
  }
  
  return true;
}


/******************************************************************************/

ExtractSpectrum* g_Prg = 0;

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


  TApplication ExtractSpectrumApp("ExtractSpectrumApp", 0, 0);

  g_Prg = new ExtractSpectrum();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  ExtractSpectrumApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
