/*
 * NModuleInterfaceStarTrackerSaverLevel1Fits.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceStarTrackerSaverLevel1Fits
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceStarTrackerSaverLevel1Fits.h"

// Standard libs:
#include <cmath>
#include <cstdio>
using namespace std;

// HEAsoft
#include "fitsio.h"

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

// NuSIM libs:
#include "NModule.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceStarTrackerSaverLevel1Fits)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceStarTrackerSaverLevel1Fits::NModuleInterfaceStarTrackerSaverLevel1Fits()
{
  // Construct an instance of NModuleInterfaceStarTrackerSaverLevel1Fits
  
  m_File = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceStarTrackerSaverLevel1Fits::~NModuleInterfaceStarTrackerSaverLevel1Fits()
{
  // Delete this instance of NModuleInterfaceStarTrackerSaverLevel1Fits
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceStarTrackerSaverLevel1Fits::OpenLevel1FitsFile(TString FileName)
{  
  //! Load and initialize the file

  // cout<<"OpenFits"<<endl;

  MFile::ExpandFileName(FileName);
  
  FileName = "!"+FileName; // To overwrite file
  
  int Status = 0;
  m_File = 0;
  
  fits_create_file(&m_File, FileName, &Status);
  if (Status != 0) {
    mgui<<"Error in creating file: "<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }
  //! create binary table extension
  char ExtensionName[] = "CHU4";
  long nrow = 0;
  int tfield = 5;
  char* ttype[] = {"time","X","Y","Z","R"};
  char* tform[] = {"1E","1E","1E","1E","1E"};
  char* tunit[] = {"s","x","y","z","r"};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating extension: "<<ExtensionName<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  Time.clear();
  m_X.clear();
  m_Y.clear();
  m_Z.clear();
  m_R.clear();
  

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceStarTrackerSaverLevel1Fits::SaveAsLevel1Fits(NStarTrackerData& Data)
{
  // Copy relevant data to the fits file

  NQuaternion Q = Data.GetStarTrackerDataSet4().GetMeasuredTransformation();

  Time.push_back(float(Data.GetTime().GetSeconds()));
  m_X.push_back(Q.m_V[0]);
  m_Y.push_back(Q.m_V[1]);
  m_Z.push_back(Q.m_V[2]);
  m_R.push_back(Q.m_R);
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceStarTrackerSaverLevel1Fits::CloseLevel1FitsFile()
{
  // Close the file    

  int Status = 0;
  
  char version[10];
  char creator[10],telescop[10];
  strcpy(version,g_Version);

  fits_write_key(m_File, TSTRING, "TELESCOP", telescop, " ", &Status);  
  fits_write_key(m_File, TSTRING, "CREATOR", creator, " ", &Status);  
  fits_write_key(m_File, TSTRING, "NuSimVER", version, "NuSim version number", &Status);
  fits_write_key(m_File, TLONG, "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);

  // We have to use pointers here to prevent a stack overflow for large data sets!
  double* dTime = new double[Time.size()];
  float* fX = new float[Time.size()];
  float* fY = new float[Time.size()];
  float* fZ = new float[Time.size()];
  float* fR = new float[Time.size()];

   //! save the data before closing  
  for (unsigned int i = 0; i < Time.size(); ++i) {
    dTime  [i] = Time[i];
    fX     [i] = m_X[i];
    fY     [i] = m_Y[i];
    fZ     [i] = m_Z[i];
    fR     [i] = m_R[i];
  }


  char Words[30];

  fits_write_col(m_File, TDOUBLE, 1, 1, 1, Time.size(), dTime, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Time') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 2, 1, 1, Time.size(), fX, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('X') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 3, 1, 1, Time.size(), fY, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Y') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 4, 1, 1, Time.size(), fZ, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Z') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 5, 1, 1, Time.size(), fR, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('R') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }

 if (Status != 0) {
    mgui<<"Error writing event table!"<<endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  
   
  fits_close_file(m_File, &Status);
  
  m_File = 0;
  
  delete [] dTime;
  delete [] fX;
  delete [] fY;
  delete [] fZ;
  delete [] fR;
  
  return true;
}


// NModuleInterfaceStarTrackerSaverLevel1Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
