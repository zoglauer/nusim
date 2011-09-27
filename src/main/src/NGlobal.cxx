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


const TString g_MajorVersion = "0.10";
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

const double deg = 60.0;
const double arcmin = 1.0;  // default unit for angles
const double arcsec = 1.0/60.0;
const double rad = 180.0/c_Pi * 60.0;

const double km = 1000000.0;
const double m  = 1000.0;
const double cm = 10.0;
const double mm = 1.0; // default unit for length

const double cm2 = 100.0;
const double mm2 = 1.0;

const double ns       = 0.000000001;
const double us       = 0.000001;
const double ms       = 0.001;
const double s        = 1.0; // default unit for time
const double minute   = 60.0;
const double hour     = 3600.0;
const double day      = 24.0*hour;
const double year     = 365.24*day;

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

#ifndef ___MACOSX___
  //gEnv->SetValue("X11.UseXft", "true");
#endif
  
  // Set a common ROOT style for all programs:
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);

  gStyle->SetTitleAlign(13);
  gStyle->SetTitleX(0.15);
  gStyle->SetTitleW(0.7);
  gStyle->SetTitleY(0.95);
  gStyle->SetTitleFont(72, "");

  gStyle->SetTitleFont(42, "XYZ");
  gStyle->SetTitleOffset(1.2, "X");
  gStyle->SetTitleOffset(1.8, "Y");
  gStyle->SetTitleOffset(1.2, "Z");
  
  gStyle->SetLabelFont(42, "XYZ");
  gStyle->SetLabelSize(0.03, "XYZ");
  
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);

  double Margin = 0.15;
  gStyle->SetPadLeftMargin(Margin);
  gStyle->SetPadRightMargin(Margin);
  gStyle->SetPadTopMargin(Margin);
  gStyle->SetPadBottomMargin(Margin);

  
  gStyle->SetOptStat(0);

  gStyle->SetPalette(1, 0);

  
  
  // Ignore ROOT messages up to kError
  gErrorIgnoreLevel = kError;
  // Abort on errors
  //gErrorAbortLevel = kError;

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
