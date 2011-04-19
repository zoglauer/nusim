/* 
 * ConvertFits.cxx
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
#include <TF1.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TMarker.h>
#include <TString.h>

// NuSIM
#include "NExtractFitsImage.h"
#include "MFunction2D.h"
#include "MFunction3DSpherical.h"


/******************************************************************************/

class ConvertFits
{
public:
  /// Default constructor
  ConvertFits();
  /// Default destructor
  ~ConvertFits();
  
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
  TString m_FitsFileName;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
ConvertFits::ConvertFits() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
ConvertFits::~ConvertFits()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool ConvertFits::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: ConvertFits <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -f:   fits file name"<<endl;
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
      m_FitsFileName = argv[++i];
			cout<<"Accepting file name: "<<m_FitsFileName<<endl;
    } else {
			cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
			cout<<Usage.str()<<endl;
			return false;
		}
  }

  if (m_FitsFileName == "") {
	  cout<<"Error: Empty file name!"<<endl;
    return false;
  }
  if (MFile::Exists(m_FitsFileName) == false) {
	  cout<<"Error: File does not exist: "<<m_FitsFileName<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool ConvertFits::Analyze()
{
  if (m_Interrupt == true) return false;

  // Plot
  MFunction2D F;
  NExtractFitsImage Extractor;
  Extractor.Extract(m_FitsFileName, F);

  
  F.Plot();
  
  // Now convert to an MFunction3DSpherical
  
  // Spectrum:
  double EnergyMin = 10;
  double EnergyMax = 80;
  unsigned int EnergyBins = (unsigned int) (EnergyMax - EnergyMin);
  TF1* Flux = new TF1("Convert", "2.0e-4*pow(x/10, -3.2)", 10, 80);
  
  vector<double> X = F.GetXAxis();
  vector<double> Y = F.GetYAxis();
      
  vector<double> E;
  for (unsigned int e = 0; e <= EnergyBins; ++e) {
    E.push_back(EnergyMin + e*(EnergyMax - EnergyMin)/EnergyBins);
  }
  
  vector<double> V(X.size()*Y.size()*E.size(), 0);
  
  double Sum = 0;
  for (unsigned int x = 0; x < X.size(); ++x) {
    for (unsigned int y = 0; y < Y.size(); ++y) {
      Sum += F.Eval(X[x], Y[y]);
    }
  }
  
  double TotalArea = 0.0;
  double TotalFlux = 0.0;
  double xPixelHalfSize = (X.back() - X[0])/(X.size()-1)/2 * c_Rad;
  double yPixelHalfSize = (Y.back() - Y[0])/(Y.size()-1)/2 * c_Rad;
  for (unsigned int x = 0; x < X.size(); ++x) {
    for (unsigned int y = 0; y < Y.size(); ++y) {
      double Area = (cos(Y[y]*c_Rad-yPixelHalfSize) - cos(Y[y]*c_Rad + yPixelHalfSize))* (2*xPixelHalfSize);
      TotalArea += Area;
      //cout<<"Area: "<<Area<<":"<<xPixelHalfSize<<":"<<yPixelHalfSize<<endl;
      for (unsigned int e = 0; e < E.size(); ++e) {
        //cout<<"x:"<<X[x]<<"  y:"<<Y[y]<<"  e:"<<E[e]<<"  F:"<<F.Eval(X[x], Y[y])<<"  S:"<<Sum<<"  Flux:"<<Flux->Eval(E[e])<<endl;
        V[x + X.size()*y + X.size()*Y.size()*e] = F.Eval(X[x], Y[y])/Sum*Flux->Eval(E[e])/Area;
        TotalFlux += F.Eval(X[x], Y[y])/Sum*Flux->Eval(E[e])/Area;
      }
    }
  }  
  
  MFunction3DSpherical S;
  S.Set(X, Y, E, V);
 
  cout<<"Integral: "<<S.Integrate()<<endl;
 
  S.Save("Tycho.3Ddat");
  
  //S.Plot(true);
    
  MFunction3DSpherical T;
  T.Set("Tycho.3Ddat", "AP");
  cout<<"Integral: "<<T.Integrate()<<endl;
  T.Save("Tycho.v2.3Ddat");

       
  return true;
}


/******************************************************************************/

ConvertFits* g_Prg = 0;

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


  TApplication ConvertFitsApp("ConvertFitsApp", 0, 0);

  g_Prg = new ConvertFits();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  ConvertFitsApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
