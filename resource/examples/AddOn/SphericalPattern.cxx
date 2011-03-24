/* 
 * SphericalPattern.cxx
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
#include <TMath.h>
#include <TCanvas.h>
#include <TMarker.h>
#include <TString.h>

// NuSIM
#include "NExtractFitsImage.h"
#include "MFunction2D.h"


/******************************************************************************/

class SphericalPattern
{
public:
  /// Default constructor
  SphericalPattern();
  /// Default destructor
  ~SphericalPattern();
  
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
  /// Output file name
  TString m_OutputFileName;
  /// RA in degree
  double m_Ra;
  /// DEC in degree
  double m_Dec;
  /// Extend in degree
  double m_Extend;
  /// Distance between pointings in degree 
  double m_Gap;
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
SphericalPattern::SphericalPattern() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
SphericalPattern::~SphericalPattern()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool SphericalPattern::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: SphericalPattern <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -f:   fits file name"<<endl;
  Usage<<"         -o:   output file name"<<endl;
  Usage<<"         -r:   RA in deg"<<endl;
  Usage<<"         -d:   DEC in deg"<<endl;
  Usage<<"         -e:   Extend in deg"<<endl;
  Usage<<"         -g:   Gap between pointing in deg"<<endl;
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
    } else if (Option == "-o") {
      m_OutputFileName = argv[++i];
			cout<<"Accepting output file name: "<<m_OutputFileName<<endl;
    } else if (Option == "-r") {
      m_Ra = atof(argv[++i]);
      cout<<"Accepting RA: "<<m_Ra<<endl;
    } else if (Option == "-d") {
      m_Dec = atof(argv[++i]);
      cout<<"Accepting DEC: "<<m_Dec<<endl;
    } else if (Option == "-e") {
      m_Extend = atof(argv[++i]);
      cout<<"Accepting extend: "<<m_Extend<<endl;
    } else if (Option == "-g") {
      m_Gap = atof(argv[++i]);
      cout<<"Accepting gap: "<<m_Gap<<endl;
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
bool SphericalPattern::Analyze()
{
  if (m_Interrupt == true) return false;

  double PointingDistanceDec = m_Gap; // arcmin
  double PointingDistanceRa = PointingDistanceDec/cos(m_Dec*TMath::DegToRad()); // arcmin

  cout<<PointingDistanceDec<<":"<<PointingDistanceRa<<endl;
  
  double ExtendDec = m_Extend;
  double ExtendRa = m_Extend/cos(m_Dec*TMath::DegToRad());
  
  double F1Ra = sqrt(ExtendRa*ExtendRa - ExtendDec*ExtendDec);
  double F2Ra = -F1Ra;
  
  vector<double> RaPointings;
  vector<double> DecPointings;
  vector<double> RollPointings;

  double Roll = 0;
  double RollDist = 10.0;
  
  unsigned int Points = m_Extend/m_Gap;
  
  double MinRa = m_Ra - Points*PointingDistanceRa;
  double MinDec = m_Dec - Points*PointingDistanceDec;
  
  for (unsigned int d = 0; d <= 2*Points; ++d) {
    for (unsigned int r = 0; r <= 2*Points; ++r) {
      double Ra = MinRa + r*PointingDistanceRa;
      double RaDiff = Ra - m_Ra;
      double Dec = MinDec + d*PointingDistanceDec;
      double DecDiff = Dec - m_Dec;
      
      double Spot1Dist = sqrt((F1Ra-RaDiff)*(F1Ra-RaDiff) + DecDiff*DecDiff);
      double Spot2Dist = sqrt((F2Ra-RaDiff)*(F2Ra-RaDiff) + DecDiff*DecDiff);
      
      //cout<<Ra<<":"<<Dec<<" - ";
      if (Spot1Dist + Spot2Dist < 2*ExtendRa) {
        //cout<<"yes";
        RaPointings.push_back(Ra);
        DecPointings.push_back(Dec);
        RollPointings.push_back(Roll);
        Roll += RollDist;
        if (Roll >= 60.0) Roll = 0;
      }
      //cout<<endl;
    }
  }

  // Plot
  MFunction2D F;
  NExtractFitsImage Extractor;
  Extractor.Extract(m_FitsFileName, F);
  
  F.Plot();
  for (unsigned int i = 0; i < RaPointings.size(); ++i) {
    TMarker* M = new TMarker(RaPointings[i], DecPointings[i], 20);
    M->Draw("SAME");
  }

  // Dump
  ofstream out;
  out.open(m_OutputFileName);
  out<<"# RA center: "<<m_Ra<<endl;
  out<<"# DEC center: "<<m_Dec<<endl;
  out<<"# Gap center: "<<m_Gap<<endl;
  out<<"# Extend center: "<<m_Extend<<endl;
  for (unsigned int i = 0; i < RaPointings.size(); ++i) {
    out<<"RD "<<RaPointings[i]<<" "<<DecPointings[i]<<" "<<RollPointings[i]<<" 10"<<endl;
  }
  out.close();
      
  return true;
}


/******************************************************************************/

SphericalPattern* g_Prg = 0;

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


  TApplication SphericalPatternApp("SphericalPatternApp", 0, 0);

  g_Prg = new SphericalPattern();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  SphericalPatternApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
