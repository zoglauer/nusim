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
#include <sstream>
#include <iostream>
#include <string>
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


NModuleInterfaceMetrologySaverLevel1Fits::NModuleInterfaceMetrologySaverLevel1Fits(NSatellite& Satellite):m_Sat(Satellite) 
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
  
  MVector m1 = Data.GetMetrologyDataSet1().GetCalibratedLaserHit();
  MVector m2 = Data.GetMetrologyDataSet2().GetCalibratedLaserHit();
  
  Time.push_back(double(m_Sat.ConvertToTimeSinceEpoch(Data.GetTime()).GetAsSeconds()));
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


bool NModuleInterfaceMetrologySaverLevel1Fits::WriteHDR()
{
  // Close the file    

  int Status = 0;
   
  //! Write NuSim header keywords
  char version[10], obs_id[10], object[20];
  char creator[10],telescop[10], TT[10], timeunit[10];
  strcpy(version,g_Version);
  strcpy(creator,"NuSIM");
  strcpy(telescop,"NuSTAR");
  strcpy(TT, "TT");
  strcpy(timeunit, "s");
  strcpy(obs_id,m_ObservationID);
  strcpy(object,m_TargetName);
  long targ_id = 0;
  float timepixr = 0.0;
  long MDJREFI = 55197;
  float MDJREFF =7.6601852000000E-04;
  double tstart = m_Sat.GetEpochObservationStartTime().GetAsSeconds();
  double tend = m_Sat.GetEpochObservationEndTime().GetAsSeconds();
  ostringstream out;
  
  fits_write_key(m_File, TSTRING, "OBS_ID", obs_id, " ", &Status);  
  fits_write_key(m_File, TLONG, "TARG_ID", &targ_id, " ", &Status); 
  fits_write_key(m_File, TSTRING, "OBJECT", object, " ", &Status); 
  fits_write_key(m_File, TSTRING, "CREATOR", creator, " ", &Status);  
  fits_write_key(m_File, TSTRING, "NuSimVER", version, "NuSim version number", &Status);
  fits_write_key(m_File, TLONG, "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);
  fits_write_key(m_File, TSTRING, "TELESCOP", telescop, " ", &Status);
  fits_write_key(m_File, TFLOAT, "TIMEPIXR", &timepixr, "Bin time beginning=0 middle=0.5 end=1", &Status);
  fits_write_key(m_File, TSTRING, "TIEMSYS", TT, "Terrestrial Time", &Status);
  fits_write_key(m_File, TLONG, "MJDREFI", &MDJREFI, "MJD reference day 01 Jan 2010 00:00:00 UTC", &Status);
  fits_write_key(m_File, TFLOAT, "MDJREFF", &MDJREFF , "MJD reference", &Status); 
  fits_write_key(m_File, TSTRING, "TIMEUNIT", timeunit, " ", &Status);
  fits_write_key(m_File, TDOUBLE, "TSTART", &tstart, " ", &Status);
  fits_write_key(m_File, TDOUBLE, "TSTOP", &tend, " ", &Status);
  out<<m_Sat.GetAbsoluteObservationStartTime().GetDateInString();
  char* DateObs = (char*) out.str().c_str();
  fits_write_key(m_File, TSTRING, "DATE-OBS", DateObs, " ", &Status);
  out.str("");
  out<<m_Sat.GetAbsoluteObservationEndTime().GetDateInString();
  char* DateEnd = (char*) out.str().c_str();
  fits_write_key(m_File, TSTRING, "DATE-END",DateEnd, " ", &Status);
  return true;

}

////////////////////////////////////////////////////////////////////////////////

bool NModuleInterfaceMetrologySaverLevel1Fits::CloseLevel1FitsFile()
{
  // Close the file    

  int Status = 0;
  int hdutype;
   
  //! Write NuSim header keywords
  WriteHDR();  

  SaveData();

   //! Move to primary hdr and write hdr again
  fits_movabs_hdu(m_File, 1, &hdutype, &Status);
  WriteHDR();

  fits_close_file(m_File, &Status);
  
  m_File = 0;

  return true;
}


// NModuleInterfaceMetrologySaverLevel1Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
