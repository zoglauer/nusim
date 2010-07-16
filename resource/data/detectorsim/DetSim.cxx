/* 
 * NuSimSimulator.cxx
 *
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
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
#include <map>
using namespace std;

// ROOT
#include <TApplication.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TString.h>

// MEGAlib
#include "MStreams.h"
#include "MDGeometryQuest.h"
#include "MDDetector.h"
#include "MFileEventsSim.h"
#include "MDVolumeSequence.h"
#include "MSimEvent.h"
#include "MSimHT.h"

/******************************************************************************/

class NuSimSimulator
{
public:
  /// Default constructor
  NuSimSimulator();
  /// Default destructor
  ~NuSimSimulator();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Analyze whatever needs to be analyzed...
  bool Analyze();
  /// Analyze the event, return true if it has to be writen to file
  bool AnalyzeEvent(MSimEvent& Event);
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }

  /// Show spectra per detector
  bool Spectra();

private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;

  /// Simulation file name
  TString m_FileName;
  /// Output file name
  TString m_OutputFileName;
  /// Geometry file name
  TString m_GeoFileName;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
NuSimSimulator::NuSimSimulator() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
NuSimSimulator::~NuSimSimulator()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool NuSimSimulator::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: NuSimSimulator <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -f:   simulation file name"<<endl;
  Usage<<"         -o:   output simulation file name"<<endl;
  Usage<<"         -g:   geometry file name"<<endl;
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
    if (Option == "-f" || Option == "-o") {
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
    if (Option == "-f") {
      m_FileName = argv[++i];
			cout<<"Accepting file name: "<<m_FileName<<endl;
    } else if (Option == "-o") {
      m_OutputFileName = argv[++i];
			cout<<"Accepting output file name: "<<m_OutputFileName<<endl;
    } else if (Option == "-g") {
      m_GeoFileName = argv[++i];
			cout<<"Accepting file name: "<<m_GeoFileName<<endl;
		} else {
			cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
			cout<<Usage.str()<<endl;
			return false;
		}
  }

  if (m_FileName == "") {
    cout<<"Error: Need a simulation file name!"<<endl;
    cout<<Usage.str()<<endl;
    return false;
  }

  if (m_GeoFileName == "") {
    cout<<"Error: Need a geometry file name!"<<endl;
    cout<<Usage.str()<<endl;
    return false;
  }

  if (m_FileName.EndsWith(".sim") == true) {
    if (m_OutputFileName == "") {
      m_OutputFileName = m_FileName;
      m_OutputFileName.Replace(m_FileName.Length()-4, 4, ".mod.sim");
      cout<<"Accepting output file name: "<<m_OutputFileName<<endl;
    }    
  } else {
    cout<<"Error: Need a simulation file name, not a "<<m_FileName<<" file "<<endl;
    cout<<Usage.str()<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool NuSimSimulator::Analyze()
{
  if (m_Interrupt == true) return false;

  // Load geometry:
  MDGeometryQuest Geometry;

  if (Geometry.ScanSetupFile(m_GeoFileName) == true) {
    cout<<"Geometry "<<Geometry.GetName()<<" loaded!"<<endl;
    Geometry.ActivateNoising(false);
    Geometry.SetGlobalFailureRate(0.0);
  } else {
    cout<<"Loading of geometry "<<Geometry.GetName()<<" failed!!"<<endl;
    return false;
  }  

  MFileEventsSim SiReader(&Geometry);
  if (SiReader.Open(m_FileName) == false) {
    cout<<"Unable to open sim file!"<<endl; 
    return false;
  }
  cout<<"Opened file "<<SiReader.GetFileName()<<" created with MEGAlib version: "<<SiReader.GetMEGAlibVersion()<<endl;
  SiReader.ShowProgress();

  int InputBins = 100;
  double InputMin = 0;
  double InputMax = 100;

  int RayleighBins = 36;

  TH2D* FluoresenceEnergy = new TH2D("FluorescenceEnergy", "FluorescenceEnergy", InputBins, InputMin, InputMax, 350, 0.0, 35.0);
  TH2D* FluoresenceMultiplicity = new TH2D("FluorescenceMultiplicity", "FluorescenceMultiplicity", InputBins, InputMin, InputMax, 6, -0.5, 5.5);
  TH2D* RayleighAngle = new TH2D("RayleighAngle", "RayleighAngle", InputBins/4, InputMin, InputMax, RayleighBins, 0, 180);

  int NPhoto = 0;
  MSimEvent* SiEvent = 0;
  while ((SiEvent = SiReader.GetNextEvent(false)) != 0) {
    if (SiEvent->GetNIAs() >= 2) {
      // Just look at the first IA
      int i = 1;
      if (SiEvent->GetIAAt(i)->GetProcess() == "PHOT") {
        ++NPhoto;

        // Loop until we have found all particles with the same origin and store the number of photons as well as the largest photon energy
        int OriginID = SiEvent->GetIAAt(i)->GetOriginID();
        int NPhotons = 0;
        double LargestEnergy = -1.0;
        for (unsigned int j = i; j < SiEvent->GetNIAs(); ++j) {
          if (SiEvent->GetIAAt(j)->GetOriginID() == OriginID) {
            if (SiEvent->GetIAAt(j)->GetSecondaryParticleID() == 1) {
              NPhotons++;
              if (SiEvent->GetIAAt(j)->GetSecondaryEnergy() > LargestEnergy) {
                LargestEnergy = SiEvent->GetIAAt(j)->GetSecondaryEnergy();
              }
            }
          }
        }
        if (LargestEnergy > 0.0) {
          FluoresenceEnergy->Fill(SiEvent->GetIAAt(0)->GetSecondaryEnergy(), LargestEnergy);
        }
        FluoresenceMultiplicity->Fill(SiEvent->GetIAAt(0)->GetSecondaryEnergy(), NPhotons);
      } else if (SiEvent->GetIAAt(i)->GetProcess() == "RAYL") {
        RayleighAngle->Fill(SiEvent->GetIAAt(i)->GetMotherEnergy(), c_Deg*SiEvent->GetIAById(SiEvent->GetIAAt(i)->GetOriginID())->GetSecondaryDirection().Angle(SiEvent->GetIAAt(i)->GetMotherDirection()));
      }
    }
    delete SiEvent;
  }
  SiReader.Close();
  


  // Create and save a MFunction for each energy bin:
  vector<double> X;
  vector<double> Y;
  for (int by = 1; by <= FluoresenceEnergy->GetNbinsY(); ++by) {
    X.push_back(FluoresenceEnergy->GetYaxis()->GetBinCenter(by));
  }
  for (int bx = 1; bx <= FluoresenceEnergy->GetNbinsX(); ++bx) {
    TString Name = "PEE_Fluorescence_Energy.";
    Name += FluoresenceEnergy->GetXaxis()->GetBinCenter(bx);
    Name += ".dat";
    Name.ReplaceAll(" ", "");

    Y.clear();
    for (int by = 1; by <= FluoresenceEnergy->GetNbinsY(); ++by) {
      Y.push_back(FluoresenceEnergy->GetBinContent(bx, by));
    }
    MFunction F;
    F.Set(X, Y, MFunction::c_InterpolationNone);
    F.Save(Name);
  }
  X.clear();
  for (int by = 1; by <= FluoresenceMultiplicity->GetNbinsY(); ++by) {
    X.push_back(FluoresenceMultiplicity->GetYaxis()->GetBinCenter(by));
  }
  for (int bx = 1; bx <= FluoresenceMultiplicity->GetNbinsX(); ++bx) {
    TString Name = "PEE_Fluorescence_Multiplicity.";
    Name += FluoresenceMultiplicity->GetXaxis()->GetBinCenter(bx);
    Name += ".dat";
    Name.ReplaceAll(" ", "");

    Y.clear();
    for (int by = 1; by <= FluoresenceMultiplicity->GetNbinsY(); ++by) {
      Y.push_back(FluoresenceMultiplicity->GetBinContent(bx, by));
    }
    MFunction F;
    F.Set(X, Y, MFunction::c_InterpolationNone);
    F.Save(Name);
  }
  X.clear();
  for (int by = 1; by <= RayleighAngle->GetNbinsY(); ++by) {
    X.push_back(RayleighAngle->GetYaxis()->GetBinCenter(by));
  }
  for (int bx = 1; bx <= RayleighAngle->GetNbinsX(); ++bx) {
    TString Name = "RA_Angle.";
    Name += RayleighAngle->GetXaxis()->GetBinCenter(bx);
    Name += ".dat";
    Name.ReplaceAll(" ", "");

    Y.clear();
    for (int by = 1; by <= RayleighAngle->GetNbinsY(); ++by) {
      Y.push_back(RayleighAngle->GetBinContent(bx, by));
    }
    MFunction F;
    F.Set(X, Y, MFunction::c_InterpolationNone);
    F.Save(Name);
  }


  TCanvas* FluoresenceEnergyCanvas = new TCanvas();
  FluoresenceEnergyCanvas->cd();
  FluoresenceEnergy->Draw("colz");
  FluoresenceEnergyCanvas->Update();

  TCanvas* FluoresenceMultiplicityCanvas = new TCanvas();
  FluoresenceMultiplicityCanvas->cd();
  FluoresenceMultiplicity->Draw("colz");
  FluoresenceMultiplicityCanvas->Update();

  TCanvas* RayleighAngleCanvas = new TCanvas();
  RayleighAngleCanvas->cd();
  RayleighAngle->Draw("colz");
  RayleighAngleCanvas->Update();
  
  return true;
}


/******************************************************************************
 * Analyze the event, return true if it has to be writen to file
 */
bool NuSimSimulator::AnalyzeEvent(MSimEvent& Event)
{
  // Add your code here
  // Return true if the event should be written to file

  // Example:
  // if (Event.GetVeto() == true) return false;

  return true;
}


/******************************************************************************/

NuSimSimulator* g_Prg = 0;

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

  TApplication NuSimSimulatorApp("NuSimSimulatorApp", 0, 0);

  g_Prg = new NuSimSimulator();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  NuSimSimulatorApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
