/*
 * NGlobal.cxx
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


////////////////////////////////////////////////////////////////////////////////
//
// NGlobal
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGlobal.h"

// Standard libs:
#include <limits>
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

// ROOT libs:
#include <TEnv.h>
#include <TStyle.h>
#include <TError.h>

// MEGAlib libs:
#include "MFile.h"
#include "MStreams.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGlobal)
#endif


////////////////////////////////////////////////////////////////////////////////


const TString g_MajorVersion = "0.9";
const TString g_MinorVersion = ".0";
const TString g_Version = g_MajorVersion + g_MinorVersion;
const TString g_CopyrightYear = "2010"; 
const TString g_Homepage = "TBD"; 
int g_SVNRevision = 0;

const TString g_MEGAlibPath = "$(NUSIM)";

const TString g_StringNotDefined = "___NotDefined___"; 
const MVector g_VectorNotDefined = MVector(numeric_limits<double>::max()/11, 
                                           numeric_limits<double>::max()/11, 
                                           numeric_limits<double>::max()/11);
const int g_IntNotDefined = numeric_limits<int>::max()-11;
const unsigned int g_UnsignedIntNotDefined = numeric_limits<unsigned int>::max()-11;
const double g_DoubleNotDefined = numeric_limits<double>::max()/11;
const float g_FloatNotDefined = numeric_limits<float>::max()/11;
const NQuaternion g_QuaternionNotDefined = NQuaternion(numeric_limits<double>::max()/11, 
                                                       numeric_limits<double>::max()/11, 
                                                       numeric_limits<double>::max()/11, 
                                                       numeric_limits<double>::max()/11);

int g_DebugLevel = 0;

const double c_Pi = 3.14159265358979311600;
const double c_Rad = c_Pi / 180.0;
const double c_Deg = 180.0 / c_Pi;
const double c_Sqrt2Pi = 2.5066283;
const double c_SpeedOfLight = 29.9792458E+9; // cm/s
const double c_E0 = 510.999; // keV
const double c_FarAway = 1E30; // cm
const double c_LargestEnergy = 0.999*numeric_limits<float>::max();
const MVector c_NullVector(0.0, 0.0, 0.0);

NGlobal g_Global;


////////////////////////////////////////////////////////////////////////////////


bool NGlobal::Initialize()
{
#ifndef NDEBUG
  g_DebugLevel = 1;
#endif

  // Initialize the GUI stream
  mgui.DumpToGui(true);

  // Initilize some global ROOT variables:
  gEnv->SetValue("Gui.BackgroundColor", "#e3dfdf");
  //gEnv->SetValue("X11.UseXft", "true");

  // Set a common ROOT style for all programs:
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);

  gStyle->SetTitleX(float(0.1));
  gStyle->SetTitleW(float(0.8));
  gStyle->SetTitleFont(72, "");

  gStyle->SetTitleFont(42, "XYZ");

  gStyle->SetLabelFont(42, "XYZ");

  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);

  gStyle->SetOptStat(0);

  gStyle->SetPalette(1, 0);

  // Ignore ROOT messages up to kError
  gErrorIgnoreLevel = kError;
  // Abort on errors
  // gErrorAbortLevel = kError;

  // Set the revision:
  TString RevFile = "$(NUSIM)/config/revision.txt";
  MFile::ExpandFileName(RevFile);
  if (MFile::Exists(RevFile) == false) {
    cout<<"Error: Cannot find NuSim revision file..."<<endl;
  }

  ifstream in;
  in.open(RevFile);
  //TString Line;
  //Line.ReadLine(in);
  //cout<<Line<<endl;
  //g_SVNRevision = atoi(Line);
  in >> g_SVNRevision;
  in.close();
  
  cout<<"Found NuSim revision "<<g_SVNRevision<<endl;

  return true;
}


// NGlobal.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
