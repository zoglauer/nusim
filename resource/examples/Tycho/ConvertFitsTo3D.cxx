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
#include "NGlobal.h"
#include "NExtractFitsImage.h"
#include "MFunction2D.h"
#include "MVector.h"
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
  double EnergyMin = 2;
  double EnergyMax = 82;
  unsigned int EnergyBins = (unsigned int) 2*(EnergyMax - EnergyMin);
  TF1* Flux = new TF1("Convert", "0.001", EnergyMin, EnergyMax);

  
  vector<double> X = F.GetXAxis();
  vector<double> Y = F.GetYAxis();
      
  F.Plot();
      
  vector<double> E;
  for (unsigned int e = 0; e <= EnergyBins; ++e) {
    E.push_back(EnergyMin + e*(EnergyMax - EnergyMin)/EnergyBins);
  }
  
  vector<double> V(X.size()*Y.size()*E.size(), 0);
  
  MVector Pos;

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
   cout<<"Processing x-Pixel: "<<x<<endl;
   for (unsigned int y = 0; y < Y.size(); ++y) {
      // Convert to theta, for the area calculation to work!
      double Area = (cos((Y[y]+90)*c_Rad-yPixelHalfSize) - cos((Y[y]+90)*c_Rad + yPixelHalfSize))* (2*xPixelHalfSize);
      TotalArea += Area;

      for (unsigned int e = 0; e < E.size(); ++e) {
        V[x + X.size()*y + X.size()*Y.size()*e] = F.Eval(X[x], Y[y])/Sum*Flux->Eval(E[e])/Area;
        TotalFlux += F.Eval(X[x], Y[y])/Sum*Flux->Eval(E[e])/Area;
      }

    }
  }  
  
  cout<<"Shrinking arrays..."<<endl;

  unsigned int xMin = 0;
  unsigned int xMax = X.size()-1;  
  unsigned int yMin = 0;
  unsigned int yMax = Y.size()-1;

  // Test x-min:
  for (unsigned int x = 0; x < X.size(); ++x) {
    bool Found = false;
    for (unsigned int y = 0; y < Y.size(); ++y) {
      for (unsigned int e = 0; e < E.size(); ++e) {
        if (V[x + X.size()*y + X.size()*Y.size()*e] != 0) {
          cout<<"xMIn: Found: x="<<x<<" - "<<V[x + X.size()*y + X.size()*Y.size()*e]<<endl;
          Found = true;
          break;
        }
      }
      if (Found == true) break;
    }
    if (Found == false) {
      xMin = x;
    } else {
      break;
    }
  }  
  // Test y-min:
  for (unsigned int y = 0; y < Y.size(); ++y) {
    bool Found = false;
    for (unsigned int x = 0; x < X.size(); ++x) {
      for (unsigned int e = 0; e < E.size(); ++e) {
        if (V[x + X.size()*y + X.size()*Y.size()*e] != 0) {
          Found = true;
          break;
        }
      }
    }
    if (Found == false) {
      yMin = y;
    } else {
      break;
    }
  }
  // Test x-max:
  for (unsigned int x = X.size()-1; x < X.size(); --x) {
    bool Found = false;
    for (unsigned int y = 0; y < Y.size(); ++y) {
      for (unsigned int e = 0; e < E.size(); ++e) {
        if (V[x + X.size()*y + X.size()*Y.size()*e] != 0) {
          Found = true;
          break;
        }
      }
    }
    if (Found == false) {
      xMax = x;
    } else {
      break;
    }
  }
  // Test y-max
  for (unsigned int y = Y.size()-1; y < Y.size(); --y) {
    bool Found = false;
    for (unsigned int x = 0; x < X.size(); ++x) {
      for (unsigned int e = 0; e < E.size(); ++e) {
        if (V[x + X.size()*y + X.size()*Y.size()*e] != 0) {
          Found = true;
          break;
        }
      }
    }
    if (Found == false) {
      yMax = y;
    } else {
      break;
    }
  }

  cout<<"Shrinking result: "<<xMin<<":"<<xMax<<":"<<yMin<<":"<<yMax<<endl;

  MFunction3DSpherical S;
  
  cout<<"Creating new arrays..."<<endl;

  // Create new axes:
  vector<double> newX;
  for (unsigned int x = xMin; x <= xMax; ++x) newX.push_back(X[x]);
  vector<double> newY;
  for (unsigned int y = yMin; y <= yMax; ++y) newY.push_back(Y[y]);
  vector<double> newV(newX.size()*newY.size()*E.size());
  for (unsigned int x = 0; x < newX.size(); ++x) {
    for (unsigned int y = 0; y < newY.size(); ++y) {
      for (unsigned int e = 0; e < E.size(); ++e) {
        newV[x + newX.size()*y + newX.size()*newY.size()*e] = V[(x+xMin) + X.size()*(y+yMin) + X.size()*Y.size()*e];
      }
    }
  }  
  
  cout<<"Setting new arrays..."<<endl;

  S.Set(newX, newY, E, newV);
 
  cout<<"Integral: "<<S.Integrate()<<endl;
   
  cout<<"Saving..."<<endl;

  S.Save("Tycho.v3.3Ddat");
  
  //S.Plot(true);

       
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
