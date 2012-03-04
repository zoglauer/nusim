/* 
 * Convert.cxx
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
#include <cmath> from 6.11 -> 6.11.1.
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
#include "MTokenizer.h"
#include "NExtractFitsImage.h"
#include "MFunction2D.h"


/******************************************************************************/

class Convert
{
public:
  /// Default constructor
  Convert();
  /// Default destructor
  ~Convert();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Analyze whatever needs to be analyzed...
  bool Analyze();
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }


private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;

};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
Convert::Convert() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
}


/******************************************************************************
 * Default destructor
 */
Convert::~Convert()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool Convert::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: Convert <options>"<<endl;
  Usage<<"    General options:"<<endl;
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
      //m_FitsFileName = argv[++i];
			//cout<<"Accepting file name: "<<m_FitsFileName<<endl;
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
bool Convert::Analyze()
{
  if (m_Interrupt == true) return false;

  ofstream off;
  off.open("Converted.spe");

  MTokenizer* T = 0;
  for (int i = -12000; i < 20000; ++i) {
    ostringstream out;
    out<<"obj"<<i<<".dat";
    
    TString FileName = out.str().c_str();
    if (MFile::Exists(FileName) == true) {
      MParser P;
      P.Open(FileName);
      if (P.GetNLines() == 0) {
        cout<<"File "<<FileName<<" has no content"<<endl;
        continue;
      }
      T = P.GetTokenizerAt(0); 

      ostringstream filename;
      filename<<gSystem->WorkingDirectory()<<"/Object_"<<T->GetTokenAt(0)<<".spe";

      double Flux = 0;
      ofstream spec;
      spec.open(filename.str().c_str());
      spec<<"IP LINLIN"<<endl;
      for (unsigned int l = 1; l < P.GetNLines(); ++l) {
        T = P.GetTokenizerAt(l); 
        if (T->GetNTokens() != 2) continue;
        if (T->GetTokenAtAsDouble(0) > 2 && T->GetTokenAtAsDouble(0) < 100) {
          spec<<"DP "<<T->GetTokenAtAsDouble(0)<<" "<<T->GetTokenAtAsDouble(1)<<endl;
          if (l+1 < P.GetNLines() && P.GetTokenizerAt(l+1)->GetNTokens() == 2) {
            // Input flux is in ph/cm2/s/keV * keV !!
            double Conversion = 0.5*(T->GetTokenAtAsDouble(1)+P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(1)); // the average flux
            // Conversion /= (0.5*(P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(0) + T->GetTokenAtAsDouble(0))); // --> ph/cm2/s/keV * keV
            // Conversion /= (0.5*(P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(0) + T->GetTokenAtAsDouble(0))); // --> ph/cm2/s/keV
            Conversion *= (P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(0) - T->GetTokenAtAsDouble(0)); // --> ph/cm2/s
            Flux += Conversion;
          }
        }
      }
      spec.close();
      
      // Sanity check:
      cout<<"Flux sanity check for object: "<<FileName<<endl;
      double SanityCheckFlux = 0.0;
      for (unsigned int l = 1; l < P.GetNLines(); ++l) {
        T = P.GetTokenizerAt(l); 
        if (T->GetNTokens() != 2) continue;
        if (T->GetTokenAtAsDouble(0) > 10 && T->GetTokenAtAsDouble(0) < 30) {
          // Input flux is in ph/cm2/s/keV * keV !!
          double Conversion = 0.5*(T->GetTokenAtAsDouble(1)+P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(1)); // the average flux
          // Conversion /= (0.5*(P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(0) + T->GetTokenAtAsDouble(0))); // --> ph/cm2/s/keV * keV
          // Conversion /= (0.5*(P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(0) + T->GetTokenAtAsDouble(0))); // --> ph/cm2/s/keV
          Conversion *= (P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(0) - T->GetTokenAtAsDouble(0)); // --> ph/cm2/s
          Conversion *= (0.5*(P.GetTokenizerAt(l+1)->GetTokenAtAsDouble(0) + T->GetTokenAtAsDouble(0))); // --> keV/cm2/s
          Conversion /= 624150974; // --> erg/cm2/s
           
          SanityCheckFlux += Conversion;
        }
      }
      cout<<"Flux [erg/cm2/s]: "<<SanityCheckFlux<<endl;
      cout<<"Flux on file [erg/cm2/s]: "<<P.GetTokenizerAt(0)->GetTokenAtAsDouble(4)<<endl;
      
            
      T = P.GetTokenizerAt(0); 
      off<<"Object_"<<T->GetTokenAt(0)<<" 1 "<<T->GetTokenAt(1)<<" "<<T->GetTokenAt(2)
        <<" 5 "<<filename.str()<<" "<<Flux<<endl;
    }
  }
  
  off.close();
      
  return true;
}


/******************************************************************************/

Convert* g_Prg = 0;

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


  TApplication ConvertApp("ConvertApp", 0, 0);

  g_Prg = new Convert();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  //ConvertApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
