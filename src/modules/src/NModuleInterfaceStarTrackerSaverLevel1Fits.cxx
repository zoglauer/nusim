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


NModuleInterfaceStarTrackerSaverLevel1Fits::NModuleInterfaceStarTrackerSaverLevel1Fits(NSatellite& Satellite):m_Sat(Satellite) 
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
  m_counter = 1;
  
  fits_create_file(&m_File, FileName, &Status);
  if (Status != 0) {
    mgui<<"Error in creating file: "<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }
  //! create binary table extension
  char ExtensionName[] = "ATTITUDE";
  long nrow = 0;
  int tfield = 2;
  char* ttype[] = {"TIME","QPARAM"};
  char* tform[] = {"1D","4D"};
  char* tunit[] = {"s",""};
  
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

  Time.push_back(double(m_Sat.ConvertToTimeSinceEpoch(Data.GetTime()).GetAsSeconds()));
  m_X.push_back(Q.m_V[0]);
  m_Y.push_back(Q.m_V[1]);
  m_Z.push_back(Q.m_V[2]);
  m_R.push_back(Q.m_R);
   
  //! If there are 100000 metrology data points then save the chunk to file 
  // and reset buffers
  if (Time.size() == 1000) {
    SaveData();
  }
	  
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool NModuleInterfaceStarTrackerSaverLevel1Fits::SaveData()
{

  int Status = 0;
  
   // We have to use pointers here to prevent a stack overflow for large data sets!
  double* dTime = new double[Time.size()];
  double* dQ = new double[Time.size()*4];
  
   //! save the data before closing  
  for (unsigned int i = 0; i < Time.size(); ++i) {
    dTime  [i] = Time[i];
    dQ     [i*4] = m_X[i];
    dQ     [i*4+1] = m_Y[i];
    dQ     [i*4+2] = m_Z[i];
    dQ     [i*4+3] = m_R[i];
   }


  char Words[30];

  fits_write_col(m_File, TDOUBLE, 1, m_counter, 1, Time.size(), dTime, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error L1CHU4: fits_write_col('Time') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TDOUBLE, 2, m_counter, 1, Time.size()*4, dQ, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('X') failed (" << Words << ")" << endl;
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
  m_counter += 1000;

  delete [] dTime;
  delete [] dQ;
  
  Time.clear();
  m_X.clear();
  m_Y.clear();
  m_Z.clear();
  m_R.clear();
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceStarTrackerSaverLevel1Fits::CloseLevel1FitsFile()
{
  // Close the file    

  int Status = 0;
  int hdutype;
  
  char version[10];
  char creator[10],telescop[10];
  strcpy(version,g_Version);
  strcpy(creator,"NuSIM");
  strcpy(telescop,"NuSTAR");

  fits_write_key(m_File, TSTRING, "TELESCOP", telescop, " ", &Status);  
  fits_write_key(m_File, TSTRING, "CREATOR", creator, " ", &Status);  
  fits_write_key(m_File, TSTRING, "NuSimVER", version, "NuSim version number", &Status);
  fits_write_key(m_File, TLONG, "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);

  SaveData();
  
  WriteHDR();   
    //! Move to primary hdr and write hdr again
  fits_movabs_hdu(m_File, 1, &hdutype, &Status);
  WriteHDR();
   
  fits_close_file(m_File, &Status);
  
  m_File = 0;
  
  return true;
}


/////////////////////////////////////////////////////////////////////////////////////

bool NModuleInterfaceStarTrackerSaverLevel1Fits::WriteHDR()
{
  // Close the file    

  int Status = 0;
   
  //! Write NuSim header keywords
  char version[10], obs_id[10], object[20];;
  char creator[10],telescop[10], TT[10], timeunit[10];
  strcpy(version,g_Version);
  strcpy(creator,"NuSIM");
  strcpy(telescop,"NuSTAR");
  strcpy(TT, "TT");
  strcpy(timeunit, "s");  
  strcpy(obs_id,m_ObservationID);
  strcpy(object,m_TargetName);
  long targ_id = 0;
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

///////////////////////////////////////////////////

// NModuleInterfaceStarTrackerSaverLevel1Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
