/*
 * NModuleInterfaceMetrologySaverLevel1Fits.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceMetrologySaverLevel1Fits
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceMetrologySaverLevel1Fits.h"

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
ClassImp(NModuleInterfaceMetrologySaverLevel1Fits)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceMetrologySaverLevel1Fits::NModuleInterfaceMetrologySaverLevel1Fits()
{
  // Construct an instance of NModuleInterfaceMetrologySaverLevel1Fits
  
  m_File = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceMetrologySaverLevel1Fits::~NModuleInterfaceMetrologySaverLevel1Fits()
{
  // Delete this instance of NModuleInterfaceMetrologySaverLevel1Fits
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceMetrologySaverLevel1Fits::OpenLevel1FitsFile(TString FileName)
{  
  //! Load and initialize the file

  // cout<<"OpenFits"<<endl;

  MFile::ExpandFileName(FileName);
  
  FileName = "!"+FileName; // To overwrite file
  
  int Status = 0;
  m_File = 0;
  m_counter = 1;
  
  fits_create_file(&m_File, FileName, &Status);
  if (Status != 0) {
    mgui<<"Error in creating file: "<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }
  
  //! create binary table extension
  char ExtensionName[] = "PSDPOS";
  long nrow = 0;
    int tfield = 5;
  char* ttype[] = {"TIME","X_PSD0","Y_PSD0","X_PSD1","Y_PSD1"};
  char* tform[] = {"1D","1E","1E","1E","1E"};
  char* tunit[] = {"s","mm","mm","mm","mm"};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating extension: "<<ExtensionName<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  Time.clear();
  m_xpsd0.clear();
  m_ypsd0.clear();
  m_xpsd1.clear();
  m_ypsd1.clear();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceMetrologySaverLevel1Fits::SaveAsLevel1Fits(NMetrologyData& Data)
{
  // Copy relevant data to the fits file
  
  NTime nt = Data.GetTime();
  MVector m1 = Data.GetMetrologyDataSet1().GetCalibratedLaserHit();
  MVector m2 = Data.GetMetrologyDataSet2().GetCalibratedLaserHit();
  
  Time.push_back(float(nt.GetAsSeconds()));
  m_xpsd0.push_back(m1[0]);
  m_ypsd0.push_back(m1[1]);
  m_xpsd1.push_back(m2[0]);
  m_ypsd1.push_back(m2[1]);
  
  //! If there are 100000 metrology data points then save the chunk to file 
  // and reset buffers
  if (Time.size() == 100000) {
    SaveData();
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool NModuleInterfaceMetrologySaverLevel1Fits::SaveData()
{
   // save a chunk of data
   
   int Status=0;
   // We have to use pointers here to prevent a stack overflow for large data sets!
    float* fxpsd0    = new float[Time.size()];
    float* fypsd0    = new float[Time.size()];
    float* fxpsd1    = new float[Time.size()];
    float* fypsd1    = new float[Time.size()];
    double* dTime   = new double[Time.size()];
  
    //! save the data before closing  
    for (unsigned int i = 0; i < Time.size(); ++i) {
      dTime  [i] = Time[i];
	  fxpsd0 [i] = m_xpsd0[i];
	  fypsd0 [i] = m_ypsd0[i];
	  fxpsd1 [i] = m_xpsd1[i];
	  fypsd1 [i] = m_ypsd1[i];
    }
  
    char Words[30];

    fits_write_col(m_File, TDOUBLE, 1, m_counter, 1, Time.size(), dTime, &Status);
    if (Status != 0) {
      fits_get_errstatus(Status, Words);
      cerr << "Error L1MET: fits_write_col('Time') failed (" << Words << ")" << endl;
      fits_close_file(m_File, &Status);
      m_File = 0;
      return false;
    }
    fits_write_col(m_File, TFLOAT, 2, m_counter, 1, Time.size(), fxpsd0, &Status);
    if (Status != 0) {
      fits_get_errstatus(Status, Words);
      cerr << "Error: fits_write_col('Time') failed (" << Words << ")" << endl;
      fits_close_file(m_File, &Status);
      m_File = 0;
      return false;
    }
    fits_write_col(m_File, TFLOAT, 3, m_counter, 1, Time.size(), fypsd0, &Status);
    if (Status != 0) {
      fits_get_errstatus(Status, Words);
      cerr << "Error: fits_write_col('Time') failed (" << Words << ")" << endl;
      fits_close_file(m_File, &Status);
      m_File = 0;
      return false;
    }
    fits_write_col(m_File, TFLOAT, 4, m_counter, 1, Time.size(), fxpsd1, &Status);
    if (Status != 0) {
      fits_get_errstatus(Status, Words);
      cerr << "Error: fits_write_col('Time') failed (" << Words << ")" << endl;
      fits_close_file(m_File, &Status);
      m_File = 0;
      return false;
    }
    fits_write_col(m_File, TFLOAT, 5, m_counter, 1, Time.size(), fypsd1, &Status);
    if (Status != 0) {
      fits_get_errstatus(Status, Words);
      cerr << "Error: fits_write_col('Time') failed (" << Words << ")" << endl;
      fits_close_file(m_File, &Status);
      m_File = 0;
      return false;
    }

   m_counter += 100000;	
	
   delete [] dTime;
   delete [] fxpsd0;
   delete [] fypsd0;
   delete [] fxpsd1;
   delete [] fypsd1;
 
   Time.clear();
   m_xpsd0.clear();
   m_ypsd0.clear();
   m_xpsd1.clear();
   m_ypsd1.clear();
   
   return true;

}

////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceMetrologySaverLevel1Fits::CloseLevel1FitsFile()
{
  // Close the file    

  int Status = 0;
   
  //! Write NuSim header keywords
  char version[10];
  char creator[10],telescop[10];
  strcpy(version,g_Version);
  strcpy(creator,"NuSIM");
  strcpy(telescop,"NuSTAR");

  fits_write_key(m_File, TSTRING, "CREATOR", creator, " ", &Status);  
  fits_write_key(m_File, TSTRING, "NuSimVER", version, "NuSim version number", &Status);
  fits_write_key(m_File, TLONG, "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);
  fits_write_key(m_File, TSTRING, "TELESCOP", telescop, " ", &Status);

  SaveData();
 
  fits_close_file(m_File, &Status);
  
  m_File = 0;

  return true;
}


// NModuleInterfaceMetrologySaverLevel1Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
