/* 
 * NuSIM_SimTest.cxx
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

// MEGAlib
#include "MGlobal.h"
#include "MStreams.h"
#include "MDGeometryQuest.h"
#include "MDDetector.h"
#include "MFileEventsSim.h"
#include "MDVolumeSequence.h"
#include "MSimEvent.h"
#include "MSimHT.h"


/******************************************************************************/

class NuSIM_SimTest
{
public:
  /// Default constructor
  NuSIM_SimTest();
  /// Default destructor
  ~NuSIM_SimTest();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Analyze what eveer needs to be analyzed...
  bool Analyze();
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }

private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;
  /// The NuSIM file
  TString m_NuSIMFile;
  /// The Cosima file
  TString m_CosimaFile;
  /// The Cosima file
  TString m_GeometryFile;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
NuSIM_SimTest::NuSIM_SimTest() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
NuSIM_SimTest::~NuSIM_SimTest()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool NuSIM_SimTest::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: NuSIM_SimTest <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -c:   cosima file name"<<endl;
  Usage<<"         -n:   nusim file name"<<endl;
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
    if (Option == "-f") {
			if (!((argc > i+1) && 
            (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0))){
				cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
				cout<<Usage.str()<<endl;
				return false;
			}
		} 
		// Multiple arguments template
    /*
		else if (Option == "-??") {
			if (!((argc > i+2) && 
            (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) && 
            (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0))){
				cout<<"Error: Option "<<argv[i][1]<<" needs two arguments!"<<endl;
				cout<<Usage.str()<<endl;
				return false;
			}
		}
    */

		// Then fulfill the options:
    if (Option == "-n") {
      m_NuSIMFile = argv[++i];
			cout<<"Accepting nusim file name: "<<m_NuSIMFile<<endl;
    } else if (Option == "-c") {
      m_CosimaFile = argv[++i];
			cout<<"Accepting cosima file name: "<<m_CosimaFile<<endl;
    } else if (Option == "-g") {
      m_GeometryFile = argv[++i];
			cout<<"Accepting geometry file name: "<<m_GeometryFile<<endl;
		} else {
			cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
			cout<<Usage.str()<<endl;
			return false;
		}
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool NuSIM_SimTest::Analyze()
{
  if (m_Interrupt == true) return false;

  // Load geometry:
  MDGeometryQuest Geometry;

  if (Geometry.ScanSetupFile(m_GeometryFile) == true) {
    cout<<"Geometry "<<Geometry.GetName()<<" loaded!"<<endl;
    Geometry.ActivateNoising(false);
    Geometry.SetGlobalFailureRate(0.0);
  } else {
    cout<<"Loading of geometry "<<Geometry.GetName()<<" failed!!"<<endl;
    return false;
  }  

  MFileEventsSim SiReader(&Geometry);
  if (SiReader.Open(m_CosimaFile) == false) {
    cout<<"Unable to open sim file!"<<endl; 
    return false;
  }
  cout<<"Opened file "<<SiReader.GetFileName()<<" created with MEGAlib version: "<<SiReader.GetMEGAlibVersion()<<endl;
  SiReader.ShowProgress();

  
  //cout<<"Triggered events: "<<SiReader.GetNEvents(false)<<" --- Observation time: "<<SiReader.GetObservationTime()<<" sec  --  simulated events: "<<SiReader.GetSimulatedEvents()<<endl;

  TH2D* HistRadialCosima = new TH2D("HistRadialCosima", "Radial interaction distribution - Cosima", 181, -3, 3, 181, -3, 3);
  HistRadialCosima->SetXTitle("X [mm]");
  HistRadialCosima->SetYTitle("Y [mm]");
  TH1D* HistRadialDistanceCosima = new TH1D("HistRadialDistanceCosima", "Radial interaction distance distribution - Cosima", 5000, 0, 5);
  HistRadialDistanceCosima->SetXTitle("Depth [mm]");
  HistRadialDistanceCosima->SetYTitle("counts");
  TH1D* HistDepthCosima = new TH1D("HistDepthCosima", "Interaction depth distribution - Cosima", 300, -1, 1);
  HistDepthCosima->SetXTitle("Depth [mm]");
  HistDepthCosima->SetYTitle("counts");
  TH1D* HistEscapeCosima = new TH1D("HistEscapeCosima", "Escaping fluorescence photons - Cosima", 800, 0, 40);
  HistEscapeCosima->SetXTitle("Energy [keV]");
  HistEscapeCosima->SetYTitle("counts");
  TH1D* HistEscapeAngleCosima = new TH1D("HistEscapeAngleCosima", "HistEscapeAngleCosima", 90, 0, 90);
  TH1D* HistFluorescenceAngleCosima = new TH1D("HistFluorescenceAngleCosima", "HistFluorescenceAngleCosima", 90, 0, 180);
  
  int NCompton = 0;
  int NPhoto = 0;
  int NRayleigh = 0;
  int NFluorescence = 0;

  MSimEvent* SiEvent = 0;
  while ((SiEvent = SiReader.GetNextEvent(false)) != 0) {
    if (SiEvent->GetNIAs() > 0 && SiEvent->GetNHTs() > 0) {
      // Get start position
      MVector Zero = SiEvent->GetIAAt(0)->GetPosition();
      for (unsigned int h = 0; h < SiEvent->GetNHTs(); ++h) {
        MVector Dist = SiEvent->GetHTAt(h)->GetPosition() - Zero;
        HistRadialCosima->Fill(10*Dist.X(), 10*Dist.Y(), SiEvent->GetHTAt(h)->GetEnergy()/SiEvent->GetIAAt(0)->GetSecondaryEnergy());
        HistRadialDistanceCosima->Fill(10*sqrt(Dist.X()*Dist.X() + Dist.Y()*Dist.Y()), SiEvent->GetHTAt(h)->GetEnergy()/SiEvent->GetIAAt(0)->GetSecondaryEnergy());
        HistDepthCosima->Fill(10*SiEvent->GetHTAt(h)->GetPosition().Z(), SiEvent->GetHTAt(h)->GetEnergy()/SiEvent->GetIAAt(0)->GetSecondaryEnergy());
      }

      int iNFluorescence = 0;
      vector<int> ParentIDs;
      for (unsigned int i = 0; i < SiEvent->GetNIAs(); ++i) {
        if (SiEvent->GetIAAt(i)->GetProcess() == "COMP") {
          ++NCompton;
        } else if (SiEvent->GetIAAt(i)->GetProcess() == "RAYL") {
          ++NRayleigh;
        } else if (SiEvent->GetIAAt(i)->GetProcess() == "PHOT") {
          if (SiEvent->GetIAAt(i)->GetSecondaryParticleID() == 1) {
            ++iNFluorescence;
            bool FoundParent = false;
            for (unsigned int p = 0; p < ParentIDs.size(); ++p) {
              if (ParentIDs[p] == SiEvent->GetIAAt(i)->GetOriginID()) {
                FoundParent = true;
                break;
              }
            }
            if (FoundParent == false) {
              ++NFluorescence;
              ParentIDs.push_back(SiEvent->GetIAAt(i)->GetOriginID());
            }
            HistFluorescenceAngleCosima->Fill(SiEvent->GetIAAt(i)->GetSecondaryDirection().Angle(MVector(0.0, 0.0, -1.0))*c_Deg);
          }
        }

        if (SiEvent->GetIAAt(i)->GetProcess() == "ESCP" && SiEvent->GetIAAt(i)->GetMotherParticleID() == 1) {
          HistEscapeCosima->Fill(SiEvent->GetIAAt(i)->GetMotherEnergy());
          HistEscapeAngleCosima->Fill(SiEvent->GetIAAt(i)->GetMotherDirection().Angle(MVector(0.0, 0.0, 1.0))*c_Deg);
        }
      }
    }

    delete SiEvent;
  }

  cout<<"Photo:        "<<NPhoto<<endl;
  cout<<"Compton:      "<<NCompton<<endl;
  cout<<"Rayleigh:     "<<NRayleigh<<endl;
  cout<<"Fluorescence: "<<NFluorescence<<endl;

  TCanvas* CanvasRadialCosima = new TCanvas();
  CanvasRadialCosima->cd();
  HistRadialCosima->Draw("colz");
  CanvasRadialCosima->Update();

  TCanvas* CanvasRadialDistanceCosima = new TCanvas();
  CanvasRadialDistanceCosima->cd();
  HistRadialDistanceCosima->Draw();
  CanvasRadialDistanceCosima->Update();

  TCanvas* CanvasDepthCosima = new TCanvas();
  CanvasDepthCosima->cd();
  HistDepthCosima->Draw();
  CanvasDepthCosima->Update();

  TCanvas* CanvasEscapeCosima = new TCanvas();
  CanvasEscapeCosima->cd();
  HistEscapeCosima->Draw();
  CanvasEscapeCosima->Update();

//   TCanvas* CanvasEscapeAngleCosima = new TCanvas();
//   CanvasEscapeAngleCosima->cd();
//   HistEscapeAngleCosima->Draw();
//   CanvasEscapeAngleCosima->Update();

//   TCanvas* CanvasFluorescenceAngleCosima = new TCanvas();
//   CanvasFluorescenceAngleCosima->cd();
//   HistFluorescenceAngleCosima->Draw();
//   CanvasFluorescenceAngleCosima->Update();


  // NuSIM part:

  TH2D* HistRadialNuSIM = new TH2D("HistRadialNuSIM", "Radial interaction distribution - NuSIM", 181, -3, 3, 181, -3, 3);
  HistRadialNuSIM->SetXTitle("X [mm]");
  HistRadialNuSIM->SetYTitle("Y [mm]");
  TH1D* HistRadialDistanceNuSIM = new TH1D("HistRadialDistanceNuSIM", "Radial interaction distance distribution - NuSIM", 5000, 0, 5);
  HistRadialDistanceNuSIM->SetXTitle("Depth [mm]");
  HistRadialDistanceNuSIM->SetYTitle("counts");
  TH1D* HistDepthNuSIM = new TH1D("HistDepthNuSIM", "Interaction depth distribution - NuSIM", 300, -1, 1);
  HistDepthNuSIM->SetXTitle("Depth [mm]");
  HistDepthNuSIM->SetYTitle("counts");
  TH1D* HistEscapeNuSIM = new TH1D("HistEscapeNuSIM", "Escaping fluorescence photons - NuSIM", 800, 0, 40);
  HistEscapeNuSIM->SetXTitle("Energy [keV]");
  HistEscapeNuSIM->SetYTitle("counts");
  TH1D* HistEscapeAngleNuSIM = new TH1D("HistEscapeAngleNuSIM", "HistEscapeAngleNuSIM", 90, 0, 90);

  ifstream in;
  in.open(m_NuSIMFile);
  
  double InputEnergy = 30;
  double EscapedEnergy = 0;
  TString Line;
  while(!in.eof()) {
    Line.ReadLine(in);
    if (Line.BeginsWith("SE") == true) {
      // Restart; 
      InputEnergy = 30;
      EscapedEnergy = 0;    
    } 
    if (Line.BeginsWith("OP") == true) {
      if (sscanf(Line.Data(), 
                 "%*s %*f %*f %*f %*f %*f %*f %*f %*f %*f %lf", 
                 &InputEnergy) != 1) {
        cout<<"Unable to read OP"<<endl;
      }
    }
    if (Line.BeginsWith("CP") == true) {
      double x, y, z;
      if (sscanf(Line.Data(), 
                 "%*s %*f %*f %*f %lf %lf %lf %*f %*f %*f %lf", 
                 &x, &y, &z, &EscapedEnergy) != 4) {
        cout<<"Unable to read CP"<<endl;
      }
      if (EscapedEnergy > 0) {
        MVector Dir(x, y, z);
        HistEscapeNuSIM->Fill(EscapedEnergy);
        HistEscapeAngleNuSIM->Fill(Dir.Angle(MVector(0.0, 0.0, 1.0))*c_Deg);
      }
    }
    if (Line.BeginsWith("IA") == true) {
      double x, y, z, E;
      if (sscanf(Line.Data(), "IA d %*d %*d %lf %lf %lf %lf", 
                 &x, &y, &z, &E) != 4) {
        cout<<"Unable to read IA"<<endl;
      }
      HistRadialNuSIM->Fill(x, y, E/InputEnergy);
      HistRadialDistanceNuSIM->Fill(sqrt(x*x+y*y), E/InputEnergy);
      HistDepthNuSIM->Fill(z, E/InputEnergy);
    }
  }


  TCanvas* CanvasRadialNuSIM = new TCanvas();
  CanvasRadialNuSIM->cd();
  HistRadialNuSIM->Draw("colz");
  CanvasRadialNuSIM->Update();

  TCanvas* CanvasRadialDistanceNuSIM = new TCanvas();
  CanvasRadialDistanceNuSIM->cd();
  HistRadialDistanceNuSIM->Draw();
  CanvasRadialDistanceNuSIM->Update();

  TCanvas* CanvasDepthNuSIM = new TCanvas();
  CanvasDepthNuSIM->cd();
  HistDepthNuSIM->Draw();
  CanvasDepthNuSIM->Update();

  TCanvas* CanvasEscapeNuSIM = new TCanvas();
  CanvasEscapeNuSIM->cd();
  HistEscapeNuSIM->Draw();
  CanvasEscapeNuSIM->Update();

//   TCanvas* CanvasEscapeAngleNuSIM = new TCanvas();
//   CanvasEscapeAngleNuSIM->cd();
//   HistEscapeAngleNuSIM->Draw();
//   CanvasEscapeAngleNuSIM->Update();


  return true;
}


/******************************************************************************/

NuSIM_SimTest* g_Prg = 0;
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
  //void (*handler)(int);
	//handler = CatchSignal;
  //(void) signal(SIGINT, CatchSignal);

  // Initialize global MEGALIB variables, especially mgui, etc.
  MGlobal::Initialize();

  TApplication NuSIM_SimTestApp("NuSIM_SimTestApp", 0, 0);

  g_Prg = new NuSIM_SimTest();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  NuSIM_SimTestApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
