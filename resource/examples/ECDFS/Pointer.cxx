/* 
 * Pointer.cxx
 *
 *
 * Copyright (C) 1998-2010 by Andreas Zoglauer.
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
#include <TH2.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TMarker.h>
#include <TLine.h>
#include <TString.h>
#include <TVector3.h>

// NuSIM
#include "NExtractFitsImage.h"
#include "MFunction2D.h"
#include "NModuleEventLoader.h"
#include "NModuleEventSaver.h"
#include "NModuleTimeEngine.h"
#include "NTime.h"
#include "NSatellite.h"
#include "NSource.h"

/******************************************************************************/

class Pointer
{
public:
  /// Default constructor
  Pointer();
  /// Default destructor
  ~Pointer();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Analyze whatever needs to be analyzed...
  bool Analyze();
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }


private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;

  /// Simulation file name
  TString m_FileName;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
Pointer::Pointer() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
Pointer::~Pointer()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool Pointer::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: Pointer <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -f:   file name"<<endl;
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
    } else {
			cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
			cout<<Usage.str()<<endl;
			return false;
		}
  }

  if (m_FileName == "") {
	  cout<<"Error: Empty file name!"<<endl;
    return false;
  }
  if (MFile::Exists(m_FileName) == false) {
	  cout<<"Error: File does not exist: "<<m_FileName<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool Pointer::Analyze()
{
  if (m_Interrupt == true) return false;

  ifstream in;
  in.open(m_FileName);
  if (in.is_open() == false) {
    mgui<<"Unable to open file: "<<m_FileName<<show;
    return false;
  }

  
  NSatellite Sat;
  Sat.SetTimeModule(new NModuleTimeEngine(Sat));
  Sat.SetGeometryAndDetectorPropertiesModule(new NModuleInterfaceGeometryAndDetectorProperties(Sat));

  TString Line;
  vector<NSource*> Sources;
  while (Line.ReadLine(in)) {
    if (Line.BeginsWith("#") == true || Line.BeginsWith(" ") == true || Line == "") continue;
    NSource* Source = new NSource(Sat);
    if (Source->ParseLine(Line, 1) == true) {
      Sources.push_back(Source);
    } else {
      cout<<"Unable to parse line: "<<Line<<endl;
      delete Source; 
    }
  }
  in.close();

  vector<double> RAs;
  vector<double> DECs;
  
  double m_MinRA = numeric_limits<double>::max();
  double m_MaxRA = -numeric_limits<double>::max();
  double m_MinDEC = numeric_limits<double>::max();
  double m_MaxDEC = -numeric_limits<double>::max();
  
  for (unsigned int s = 0; s < Sources.size(); ++s) {
    if (Sources[s]->GetBeamType() == NSource::c_FarFieldPoint) {
      //cout<<Sources[s]->GetPositionParameter1()/deg<<" "<<Sources[s]->GetPositionParameter2()/deg<<endl;
      RAs.push_back(Sources[s]->GetPositionParameter2()/deg);
      if (RAs.back() < m_MinRA) m_MinRA = RAs.back();
      if (RAs.back() > m_MaxRA) m_MaxRA = RAs.back();
      DECs.push_back(Sources[s]->GetPositionParameter1()/deg);
      if (DECs.back() < m_MinDEC) m_MinDEC = DECs.back();
      if (DECs.back() > m_MaxDEC) m_MaxDEC = DECs.back();
    }
  }
  
  double Tol = 0.1;
  TH2D* Hist = new TH2D("Randoms", "Randoms", 200, m_MinRA-Tol, m_MaxRA+Tol, 200, m_MinDEC-Tol, m_MaxDEC+Tol);
  for (unsigned int s = 0; s < RAs.size(); ++s) {
    Hist->Fill(RAs[s], DECs[s]);  
  }

  vector<double> pRAs;
  vector<double> pDECs;
  
  /* Best for dual pointing
  double FrameSize = 6.7/60;
  double Angle = 40;
  
  double RefDEC = -27.8;
  double RefRA = 53.02;
  */
  
  double FrameSize = 6.6/60;
  double Angle = 40;
  double AngleOffsetForNuSIM = 27.5;
  
  double RefDEC = -27.82;
  double RefRA = 52.93;
  
  vector<int> sRAs;
  vector<int> sDECs;
  
  for (int r = 0; r <= 2; ++r) {  
    for (int d = 0; d <= 2; ++d) {  
      sRAs.push_back(r); sDECs.push_back(d);
    }
  }  
      
  TVector3 DirRef;
  double DEC = RefDEC*c_Rad+c_Pi/2;
  double RA = RefRA*c_Rad;
  DirRef.SetMagThetaPhi(1.0, DEC, RA);  
  
  for (unsigned int s = 0; s < sRAs.size(); ++s) {
    TVector3 Dir;
    DEC = (RefDEC + sDECs[s]*FrameSize)*c_Rad+c_Pi/2;
    RA = (RefRA + (sRAs[s]*FrameSize)/cos((RefDEC + sDECs[s]*FrameSize)*c_Rad))*c_Rad;
    Dir.SetMagThetaPhi(1.0, DEC, RA);
    Dir.Rotate(Angle*c_Rad, DirRef);
    pRAs.push_back(Dir.Phi()*c_Deg);
    pDECs.push_back((Dir.Theta()-c_Pi/2)*c_Deg);
  }
  
  ofstream fout;
  fout.open("Pointings.pat");
  for (unsigned int s = 0; s < pRAs.size(); ++s) {
    fout<<"RD "<<pRAs[s]<<" "<<pDECs[s]<<" "<<Angle-AngleOffsetForNuSIM<<" 10"<<endl;
  }  
  fout.close();

  TCanvas* Canvas = new TCanvas();
  Canvas->cd();
  Hist->Draw("colz");
  Canvas->Update();


  for (unsigned int s = 0; s < pRAs.size(); ++s) {
    cout<<pRAs[s]<<":"<<pDECs[s]<<endl;
    Hist->Fill(pRAs[s], pDECs[s], 5);

    TVector3 Dir;
    Dir.SetMagThetaPhi(1.0, pDECs[s]*c_Rad+c_Pi/2, pRAs[s]*c_Rad);

    TVector3 pdzrDir;
    pdzrDir.SetMagThetaPhi(1.0, (pDECs[s]+FrameSize)*c_Rad+c_Pi/2, pRAs[s]*c_Rad);
    pdzrDir.Rotate(Angle*c_Rad, Dir);
    Hist->Fill(pdzrDir.Phi()*c_Deg, (pdzrDir.Theta()-c_Pi/2)*c_Deg, 3);

    TVector3 mdzrDir;
    mdzrDir.SetMagThetaPhi(1.0, (pDECs[s]-FrameSize)*c_Rad+c_Pi/2, pRAs[s]*c_Rad);
    mdzrDir.Rotate(Angle*c_Rad, Dir);
    Hist->Fill(mdzrDir.Phi()*c_Deg, (mdzrDir.Theta()-c_Pi/2)*c_Deg, 3);

    TVector3 zdprDir;
    zdprDir.SetMagThetaPhi(1.0, pDECs[s]*c_Rad+c_Pi/2, (pRAs[s] + FrameSize/cos(pDECs[s]*c_Rad))*c_Rad);
    zdprDir.Rotate(Angle*c_Rad, Dir);
    Hist->Fill(zdprDir.Phi()*c_Deg, (zdprDir.Theta()-c_Pi/2)*c_Deg, 3);

    TVector3 zdmrDir;
    zdmrDir.SetMagThetaPhi(1.0, pDECs[s]*c_Rad+c_Pi/2, (pRAs[s] - FrameSize/cos(pDECs[s]*c_Rad))*c_Rad);
    zdmrDir.Rotate(Angle*c_Rad, Dir);
    Hist->Fill(zdmrDir.Phi()*c_Deg, (zdmrDir.Theta()-c_Pi/2)*c_Deg, 3);
 
    TVector3 pdmrDir;
    pdmrDir.SetMagThetaPhi(1.0, (pDECs[s]+FrameSize)*c_Rad+c_Pi/2, (pRAs[s] - FrameSize/cos(pDECs[s]*c_Rad))*c_Rad);
    pdmrDir.Rotate(Angle*c_Rad, Dir);
    Hist->Fill(pdmrDir.Phi()*c_Deg, (pdmrDir.Theta()-c_Pi/2)*c_Deg, 3);
  
    TVector3 pdprDir;
    pdprDir.SetMagThetaPhi(1.0, (pDECs[s]+FrameSize)*c_Rad+c_Pi/2, (pRAs[s] + FrameSize/cos(pDECs[s]*c_Rad))*c_Rad);
    pdprDir.Rotate(Angle*c_Rad, Dir);
    Hist->Fill(pdprDir.Phi()*c_Deg, (pdprDir.Theta()-c_Pi/2)*c_Deg, 3);
 
    TVector3 mdmrDir;
    mdmrDir.SetMagThetaPhi(1.0, (pDECs[s]-FrameSize)*c_Rad+c_Pi/2, (pRAs[s] - FrameSize/cos(pDECs[s]*c_Rad))*c_Rad);
    mdmrDir.Rotate(Angle*c_Rad, Dir);
    Hist->Fill(mdmrDir.Phi()*c_Deg, (mdmrDir.Theta()-c_Pi/2)*c_Deg, 3);
  
    TVector3 mdprDir;
    mdprDir.SetMagThetaPhi(1.0, (pDECs[s]-FrameSize)*c_Rad+c_Pi/2, (pRAs[s] + FrameSize/cos(pDECs[s]*c_Rad))*c_Rad);
    mdprDir.Rotate(Angle*c_Rad, Dir);
    Hist->Fill(mdprDir.Phi()*c_Deg, (mdprDir.Theta()-c_Pi/2)*c_Deg, 3);
   
    TLine* mD = new TLine(mdmrDir.Phi()*c_Deg, (mdmrDir.Theta()-c_Pi/2)*c_Deg, mdprDir.Phi()*c_Deg, (mdprDir.Theta()-c_Pi/2)*c_Deg);
    mD->Draw();
    
    TLine* pD = new TLine(pdmrDir.Phi()*c_Deg, (pdmrDir.Theta()-c_Pi/2)*c_Deg, pdprDir.Phi()*c_Deg, (pdprDir.Theta()-c_Pi/2)*c_Deg);
    pD->Draw();
    
    TLine* mR = new TLine(pdmrDir.Phi()*c_Deg, (pdmrDir.Theta()-c_Pi/2)*c_Deg, mdmrDir.Phi()*c_Deg, (mdmrDir.Theta()-c_Pi/2)*c_Deg);
    mR->Draw();
    
    TLine* pR = new TLine(mdprDir.Phi()*c_Deg, (mdprDir.Theta()-c_Pi/2)*c_Deg, pdprDir.Phi()*c_Deg, (pdprDir.Theta()-c_Pi/2)*c_Deg);
    pR->Draw();
  }
  Canvas->Modified();
  Canvas->Update();

  return true;
}


/******************************************************************************/

Pointer* g_Prg = 0;

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


  TApplication PointerApp("PointerApp", 0, 0);

  g_Prg = new Pointer();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  PointerApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
