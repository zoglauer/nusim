/*
 * MNuclearizerMain.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// MNuclearizerMain.cxx
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NMain.h"

// Standard libs:
#include <iostream>
using namespace std;

// ROOT libs:
#include <TROOT.h>
#include <TEnv.h>
#include <TApplication.h>
#include <TString.h>
#include <TSystem.h>

// NuSIM libs:
#include "NGlobal.h"
#include "NInterface.h"


//////////////////////////////////////////////////////////////////////////////////


int main(int argc, char** argv)
{
  // Main function... the beginning...

  // Initialize global NuSIM variables, especially mgui, etc.
  NGlobal::Initialize();

	TApplication* AppNuclearizer = new TApplication("Nuclearizer", 0, 0);

	NInterface Nuclearizer;
  if (Nuclearizer.ParseCommandLine(argc, argv) == false) {
    return 0;
  } else {
    AppNuclearizer->Run();
  }  

  return 0;
}


// MNuclearizerMain: the end...
//////////////////////////////////////////////////////////////////////////////////
