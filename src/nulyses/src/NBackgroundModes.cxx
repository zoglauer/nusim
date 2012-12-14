/*
 * NBackgroundModes.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NBackgroundModes
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBackgroundModes.h"

// Standard libs:
#include <fstream>
#include <limits>
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include <TFile.h>
#include <TKey.h>
#include <TIterator.h>
#include <TList.h>
#include <TClass.h>
#include <TROOT.h>

// MEGAlib libs:

// NuSTAR libs:
#include "NPhaFile.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NBackgroundModes)
#endif


////////////////////////////////////////////////////////////////////////////////


NBackgroundModes::NBackgroundModes()
{
  // Construct an instance of NBackgroundModes
  
  m_MinFitRange = 95;
  m_MaxFitRange = 120;
}


////////////////////////////////////////////////////////////////////////////////


NBackgroundModes::~NBackgroundModes()
{
  // Delete this instance of NBackgroundModes
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundModes::ParseCommandLine(int argc, char** argv)
{
  if (NBaseTool::ParseCommandLine(argc, argv) == false) return false;
  
  // Now parse the command line options:
  string Option;
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    
    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "-dps" || Option == "--dps" ) {
      if (!((argc > i+8) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) && 
        (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0) && 
        (argv[i+3][0] != '-' || isalpha(argv[i+3][1]) == 0) && 
        (argv[i+4][0] != '-' || isalpha(argv[i+4][1]) == 0) && 
        (argv[i+5][0] != '-' || isalpha(argv[i+5][1]) == 0) && 
        (argv[i+6][0] != '-' || isalpha(argv[i+6][1]) == 0) && 
        (argv[i+7][0] != '-' || isalpha(argv[i+7][1]) == 0) && 
        (argv[i+8][0] != '-' || isalpha(argv[i+8][1]) == 0))){
        cout<<"Error: Option "<<argv[i]<<" needs nine arguments!"<<endl;
        return false;
      }
    } else if (Option == "-dpsr" || Option == "--dpsr") {
      if (!((argc > i+10) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) && 
        (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0) && 
        (argv[i+3][0] != '-' || isalpha(argv[i+3][1]) == 0) && 
        (argv[i+4][0] != '-' || isalpha(argv[i+4][1]) == 0) && 
        (argv[i+5][0] != '-' || isalpha(argv[i+5][1]) == 0) && 
        (argv[i+6][0] != '-' || isalpha(argv[i+6][1]) == 0) && 
        (argv[i+7][0] != '-' || isalpha(argv[i+7][1]) == 0) && 
        (argv[i+8][0] != '-' || isalpha(argv[i+8][1]) == 0) && 
        (argv[i+9][0] != '-' || isalpha(argv[i+9][1]) == 0) && 
        (argv[i+10][0] != '-' || isalpha(argv[i+10][1]) == 0))){
        cout<<"Error: Option "<<argv[i]<<" needs eleven arguments!"<<endl;
        return false;
      }
    }
    
    // Then fulfill the options:
    if (Option == "-dps" || Option == "--dps") {
      m_Directories.push_back(argv[++i]);
      m_DetPosXA.push_back(atoi(argv[++i]));
      m_DetPosYA.push_back(atoi(argv[++i]));
      m_DetSizeA.push_back(atoi(argv[++i]));
      m_PhaA.push_back(argv[++i]);
      m_RegA.push_back("");
      m_DetPosXB.push_back(atoi(argv[++i]));
      m_DetPosYB.push_back(atoi(argv[++i]));
      m_DetSizeB.push_back(atoi(argv[++i]));
      m_PhaB.push_back(argv[++i]);
      m_RegB.push_back("");
      cout<<"Accepting directory file name: "<<m_Directories.back()
          <<" - (A: x="<<m_DetPosXA.back()<<", y="<<m_DetPosYA.back()<<", s="<<m_DetSizeA.back()<<", pha="<<m_PhaA.back()<<", reg="<<m_RegA.back()
          <<" -- B: x="<<m_DetPosXB.back()<<", y="<<m_DetPosYB.back()<<", s="<<m_DetSizeB.back()<<", pha="<<m_PhaB.back()<<", reg="<<m_RegB.back()<<")"<<endl;
    } else if (Option == "-dpsr" || Option == "--dpsr") {
      m_Directories.push_back(argv[++i]);
      m_DetPosXA.push_back(atoi(argv[++i]));
      m_DetPosYA.push_back(atoi(argv[++i]));
      m_DetSizeA.push_back(atoi(argv[++i]));
      m_PhaA.push_back(argv[++i]);
      m_RegA.push_back(argv[++i]);
      m_DetSizeA.push_back(atoi(argv[++i]));
      m_DetPosXB.push_back(atoi(argv[++i]));
      m_DetPosYB.push_back(atoi(argv[++i]));
      m_DetSizeB.push_back(atoi(argv[++i]));
      m_PhaB.push_back(argv[++i]);
      m_RegB.push_back(argv[++i]);
      cout<<"Accepting directory file name: "<<m_Directories.back()
          <<" - (A: x="<<m_DetPosXA.back()<<", y="<<m_DetPosYA.back()<<", s="<<m_DetSizeA.back()<<", pha="<<m_PhaA.back()<<", reg="<<m_RegA.back()
          <<" -- B: x="<<m_DetPosXB.back()<<", y="<<m_DetPosYB.back()<<", s="<<m_DetSizeB.back()<<", pha="<<m_PhaA.back()<<", reg="<<m_RegA.back()<<")"<<endl;
    } else if (Option == "--fit-range") {
      m_MinFitRange = atof(argv[++i]);
      m_MaxFitRange = atof(argv[++i]);
      cout<<"Accepting fit range from "<<m_MinFitRange<<" "<<m_MaxFitRange<<endl;
    }
  }
    
  return true;
}


// NBackgroundModes.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
