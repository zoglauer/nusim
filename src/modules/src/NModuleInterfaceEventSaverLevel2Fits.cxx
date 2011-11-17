/*
 * NModuleInterfaceEventSaverLevel2Fits.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceEventSaverLevel2Fits
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceEventSaverLevel2Fits.h"

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
#include "NExposureMap.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceEventSaverLevel2Fits)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverLevel2Fits::NModuleInterfaceEventSaverLevel2Fits(NSatellite& Satellite) : m_Satellite(Satellite), m_ExposureMap(Satellite)
{
  // Construct an instance of NModuleInterfaceEventSaverLevel2Fits
  
  m_File = 0;
  m_PixelSize = (12.36/5.)*arcsec;
  m_CreateExposureMap = true;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverLevel2Fits::~NModuleInterfaceEventSaverLevel2Fits()
{
  // Delete this instance of NModuleInterfaceEventSaverLevel2Fits
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::OpenLevel2FitsFile(TString FileName)
{  
  //! Load and initialize the file

  // cout<<"OpenFits"<<endl;

  MFile::ExpandFileName(FileName);

  TString ExposureFileNamePrefix = FileName;
  ExposureFileNamePrefix.ReplaceAll(".events.fits", "");
  m_ExposureMap.SetFileNamePrefix(ExposureFileNamePrefix);
  
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
  char ExtensionName[] = "EVENTS";
  long nrow = 0;
  int tfield = 21;
  char* ttype[] = {"Time","LifeTime","Module","X","Y","PI","E","grade","phottype","reject","Qx","Qy","Qz","Qr","Tx","Ty","Tz","QSx","QSy","QSz","QSr"};
  char* tform[] = {"1D","1D","1J","1E","1E","1J","1E","1I","1I","1I","1E","1E","1E","1E","1E","1E","1E","1E","1E","1E","1E"};
  char* tunit[] = {"s","s","unit","pixel","pixel","channel","keV","grade","photon type","rejection type","unit","unit","unit","unit","mm","mm","mm","unit","unit","unit","unit"};
  
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating extension: "<<ExtensionName<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  m_Ra.clear();
  m_Dec.clear();
  m_Module.clear();
  m_Energy.clear();
  m_Time.clear();
  m_Life.clear();
  m_Origin.clear();
  m_Reject.clear();
  m_Grade.clear();
  m_QfbobX.clear();
  m_QfbobY.clear();
  m_QfbobZ.clear();
  m_QfbobR.clear();
  m_TfbobX.clear();
  m_TfbobY.clear();
  m_TfbobZ.clear();
  m_QstarX.clear();
  m_QstarY.clear();
  m_QstarZ.clear();
  m_QstarR.clear();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::SaveEventLevel2Fits(NEvent& Event)
{
  //! Main data analysis routine, which updates the event to a new level 
  
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    m_Ra.push_back(Event.GetHit(i).GetObservatoryData().GetRa());
    m_Dec.push_back(Event.GetHit(i).GetObservatoryData().GetDec());
    m_Module.push_back(Event.GetTelescope());
    m_Energy.push_back(Event.GetHit(i).GetEnergy());
    m_Time.push_back(m_Satellite.ConvertToTimeSinceEpoch(Event.GetTime()).GetAsSeconds());
    m_Life.push_back(Event.GetDetectorLifeTime().GetAsSeconds());
    m_Origin.push_back(Event.GetOrigin());
    if (Event.GetHit(i).GetDepthCut() == false) {
      m_Reject.push_back(0);
    } else {
      m_Reject.push_back(1);
    }
    m_Grade.push_back(Event.GetHit(i).GetTriggerGrade());
    NQuaternion Qfbob = Event.GetHit(i).GetObservatoryData().GetOrientationFocalPlaneToOB().GetRotationQuaternion();
    m_QfbobX.push_back(Qfbob.m_V[0]);
    m_QfbobY.push_back(Qfbob.m_V[1]);
    m_QfbobZ.push_back(Qfbob.m_V[2]);
    m_QfbobR.push_back(Qfbob.m_R);
    MVector Tfbob = Event.GetHit(i).GetObservatoryData().GetOrientationFocalPlaneToOB().GetTranslation();
    m_TfbobX.push_back(Tfbob.GetX());
    m_TfbobY.push_back(Tfbob.GetY());
    m_TfbobZ.push_back(Tfbob.GetZ());
    NQuaternion Qstar = Event.GetHit(i).GetObservatoryData().GetOrientationOBToIS().GetRotationQuaternion();
    m_QstarX.push_back(Qstar.m_V[0]);
    m_QstarY.push_back(Qstar.m_V[1]);
    m_QstarZ.push_back(Qstar.m_V[2]);
    m_QstarR.push_back(Qstar.m_R);
  }
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::CloseLevel2FitsFile()
{
  // Close the file    

  int Status = 0;

  if (m_Ra.size() == 0) {
    fits_close_file(m_File, &Status);
    m_File = 0;
    return true;
  }

  // The default image pixel size:
  double PixelSize = m_PixelSize;
  double PixelLimit = 2048.0; // +- center, so the real limits is double this size

  // Calculate the minimum, maximum and average RA & DEC

  double DecMin = numeric_limits<double>::max();
  double DecMax = -numeric_limits<double>::max();
  double DecAvg = 0;
  for (unsigned int i = 0; i < m_Dec.size(); ++i) {
    if (m_Dec[i] < DecMin) {
      DecMin = m_Dec[i];
      //cout<<"New dec min: "<<DecMin/deg<<endl;
    }
    if (m_Dec[i] > DecMax) {
      DecMax = m_Dec[i];
      //cout<<"New dec max: "<<DecMax/deg<<endl;
    }
    DecAvg += m_Dec[i];
  }
  DecAvg /= m_Dec.size();

  //cout<<"Intermediate min, avg, max: "<<DecMin/deg<<":"<<DecAvg/deg<<":"<<DecMax/deg<<endl;

  // Make sure we are with the maximum allowed image pixel size
  if ((DecAvg - DecMin)/PixelSize > PixelLimit) {
    DecMin = DecAvg - PixelLimit*PixelSize;
  }
  if ((DecMax - DecAvg)/PixelSize > PixelLimit) {
    DecMax = DecAvg + PixelLimit*PixelSize;
  }

  // We want to "point" at RaAvg, thus we have to make a symmetric field around RaAvg
  if (DecAvg - DecMin > DecMax - DecAvg) {
    DecMax = 2*DecAvg - DecMin;
  } else {
    DecMin = 2*DecAvg - DecMax;
  } 

  //cout<<"Final min, avg, max: "<<DecMin/deg<<":"<<DecAvg/deg<<":"<<DecMax/deg<<endl;

  double RaMin = numeric_limits<double>::max();
  double RaMax = -numeric_limits<double>::max();
  double RaAvg = 0;
  for (unsigned int i = 0; i < m_Ra.size(); ++i) {
    if (m_Ra[i] < RaMin) {
      RaMin = m_Ra[i];
      //cout<<"New RA min: "<<RaMin/deg<<endl;
    }
    if (m_Ra[i] > RaMax) {
      RaMax = m_Ra[i];
      //cout<<"New RA max: "<<RaMax/deg<<endl;
    }
    RaAvg += m_Ra[i];
  }
  RaAvg /= m_Ra.size();

  //cout<<"Intermediate min, avg, max: "<<RaMin/deg<<":"<<RaAvg/deg<<":"<<RaMax/deg<<endl;

  // Make sure we are with the maximum allowed image pixel size
  if ((RaAvg - RaMin)/PixelSize*cos(DecAvg/rad) > PixelLimit) {
    RaMin = RaAvg - PixelLimit*PixelSize / cos(DecAvg/rad);
  }
  if ((RaMax - RaAvg)/PixelSize > PixelLimit) {
    RaMax = RaAvg + PixelLimit*PixelSize / cos(DecAvg/rad);
  }

  // We want to "point" at RaAvg, thus we have to make a symmetric field around RaAvg
  if (RaAvg - RaMin > RaMax - RaAvg) {
    RaMax = 2*RaAvg - RaMin;
  } else {
    RaMin = 2*RaAvg - RaMax;
  } 

  //cout<<"Final min, avg, max: "<<RaMin/deg<<":"<<RaAvg/deg<<":"<<RaMax/deg<<endl;

  // Throw out everything beyond the borders
  for (vector<double>::iterator I = m_Ra.begin(); I != m_Ra.end(); ) {
    if ((*I) < RaMin || (*I) > RaMax || m_Dec[int(I-m_Ra.begin())] < DecMin || m_Dec[int(I-m_Ra.begin())] > DecMax) {
      unsigned int Diff = int(I-m_Ra.begin());
      cout<<"Erasing events at time "<<m_Time[Diff]<<" since the position is out of bounds!"<<endl;
      I = m_Ra.erase(I);
      m_Dec.erase(m_Dec.begin() + Diff);
      m_Energy.erase(m_Energy.begin() + Diff);
      m_Life.erase(m_Life.begin() + Diff);
      m_Time.erase(m_Time.begin() + Diff);
      m_Module.erase(m_Module.begin() + Diff);
      m_Origin.erase(m_Origin.begin() + Diff);
      m_Reject.erase(m_Reject.begin() + Diff);
      m_Grade.erase(m_Grade.begin() + Diff);
      m_QfbobX.erase(m_QfbobX.begin() + Diff);
      m_QfbobY.erase(m_QfbobY.begin() + Diff);
      m_QfbobZ.erase(m_QfbobZ.begin() + Diff);
      m_QfbobR.erase(m_QfbobR.begin() + Diff);
      m_TfbobX.erase(m_TfbobX.begin() + Diff);
      m_TfbobY.erase(m_TfbobY.begin() + Diff);
      m_TfbobZ.erase(m_TfbobZ.begin() + Diff);
      m_QstarX.erase(m_QstarX.begin() + Diff);
      m_QstarY.erase(m_QstarY.begin() + Diff);
      m_QstarZ.erase(m_QstarZ.begin() + Diff);
      m_QstarR.erase(m_QstarR.begin() + Diff);
    } else {
      ++I;
    }
  }
  
  //! Write WCS header keywords   
  float tcrvl1=RaMax/deg;
  float tcrvl2=DecMin/deg;
  float tcdlt1=-PixelSize/deg;
  float tcdlt2=-tcdlt1;
  float tcrpx1=0.0, tcrpx2=0.0;
  long MDJREFI = 55197;
  float MDJREFF =7.6601852000000E-04;
  long targ_id = 0; 
  ostringstream out1;
  
  double tstart = m_Satellite.GetEpochObservationStartTime().GetAsSeconds();
  double tend = m_Satellite.GetEpochObservationEndTime().GetAsSeconds();
  NTime Start = m_Satellite.GetAbsoluteObservationStartTime();
  NTime End = m_Satellite.GetAbsoluteObservationEndTime();

  fits_write_key(m_File, TSTRING, "INSTRUME", const_cast<char*>("FPM"), "Detector", &Status); 	  
  fits_write_key(m_File, TSTRING, "MISSION", const_cast<char*>("NuSIM"), " ", &Status); 	  
  fits_write_key(m_File, TSTRING, "TELESCOP", const_cast<char*>("NuSIM"), " ", &Status); 
  fits_write_key(m_File, TSTRING, "CREATOR", const_cast<char*>("NuSIM"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "NuSimVER", const_cast<char*>(g_Version.Data()), "NuSim version number", &Status);
  fits_write_key(m_File, TLONG,   "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);
  fits_write_key(m_File, TSTRING, "OBS_ID",   const_cast<char*>(m_ObservationID.Data()), " ", &Status);  
  fits_write_key(m_File, TLONG,   "TARG_ID",  &targ_id, " ", &Status);  
  fits_write_key(m_File, TSTRING, "OBJECT",   const_cast<char*>(m_TargetName.Data()), " ", &Status); 
  TString Name = m_Satellite.GetCalibratedAlignmentsDBFileName();
  fits_write_key(m_File, TSTRING, "ALIGNDB",   const_cast<char*>(Name.Remove(0, Name.Last('/')+1).Data()), " ", &Status); 
  Name = m_Satellite.GetCalibratedAlignmentsDBFileName();
  fits_write_key(m_File, TSTRING, "PERTURDB",   const_cast<char*>(Name.Remove(0, Name.Last('/')+1).Data()), " ", &Status); 
  fits_write_key(m_File, TLONG, "MJDREFI", &MDJREFI, "MJD reference day 01 Jan 2010 00:00:00 UTC", &Status);
  fits_write_key(m_File, TFLOAT, "MDJREFF", &MDJREFF , "MJD reference", &Status);
  fits_write_key(m_File, TSTRING, "TIMEUNIT", const_cast<char*>("s"), " ", &Status); 
  fits_write_key(m_File, TDOUBLE, "TSTART", &tstart, "start time", &Status); 	  
  fits_write_key(m_File, TDOUBLE, "TSTOP", &tend, "end time", &Status); 	
  fits_write_key(m_File, TSTRING, "DATE-OBS", const_cast<char*>(m_Satellite.GetAbsoluteObservationStartTime().GetDateInString().Data()), " ", &Status);
  fits_write_key(m_File, TSTRING, "DATE-END", const_cast<char*>(m_Satellite.GetAbsoluteObservationEndTime().GetDateInString().Data()), " ", &Status);  
  fits_write_key(m_File, TFLOAT, "TCDLT4", &tcdlt1, "Platescale", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCDLT5", &tcdlt2, "Platescale", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCRVL4", &tcrvl1, "Transform to celestrial coords", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCRVL5", &tcrvl2, "Transform to celestrial coords", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCRPX4", &tcrpx1, "Pixel reference point", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCRPX5", &tcrpx2, "Pixel reference point", &Status); 	  
  fits_write_key(m_File, TSTRING, "TCRUNI4", const_cast<char*>("deg")," ", &Status);
  fits_write_key(m_File, TSTRING, "TCRUNI5", const_cast<char*>("deg")," ", &Status);
  fits_write_key(m_File, TSTRING, "TCTYP4", const_cast<char*>("RA---TAN")," ", &Status);
  fits_write_key(m_File, TSTRING, "TCTYP5", const_cast<char*>("DEC--TAN")," ", &Status); 
  fits_write_key(m_File, TSTRING, "RADESYS", const_cast<char*>("FK5"), " ", &Status);

  if (Status != 0) {
    mgui<<"Error in creating header for events table "<<endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  
   //! Write NuSim header keywords
  char version[10];
  strcpy(version,g_Version);
  
  fits_write_key(m_File, TSTRING, "NuSimVER", version, "NuSim version number", &Status);
  fits_write_key(m_File, TLONG, "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);
  
  vector<float> NormalizedRA(m_Ra.size());
  vector<float> NormalizedDEC(m_Ra.size());
  vector<int> PI(m_Ra.size());
  
  //! save the data before closing  
  for (unsigned int i = 0; i < m_Ra.size(); ++i) {
    NormalizedRA[i] = (RaMax-m_Ra[i])*cos(DecAvg/rad)/PixelSize;
    NormalizedDEC[i] = (m_Dec[i]-DecMin)/PixelSize;
    PI[i] = (int)((m_Energy[i]-3.0)*10 + 0.5);  // conversion to PI channels
  }
  
  char Words[30];

  fits_write_col(m_File, TDOUBLE, 1, 1, 1, m_Ra.size(), &m_Time[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error L2: fits_write_col('Time') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TDOUBLE, 2, 1, 1, m_Ra.size(), &m_Life[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error L2: fits_write_col('Life') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TINT, 3, 1, 1, m_Ra.size(), &m_Module[0], &Status); 
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Module') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 4, 1, 1, m_Ra.size(), &NormalizedRA[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('RA') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 5, 1, 1, m_Ra.size(), &NormalizedDEC[0], &Status); 
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('DEC') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TINT, 6, 1, 1, m_Ra.size(), &PI[0], &Status); 
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('PI') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 7, 1, 1, m_Ra.size(), &m_Energy[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Energy') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TSHORT, 8, 1, 1, m_Ra.size(), &m_Grade[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Grade') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TSHORT, 9, 1, 1, m_Ra.size(), &m_Origin[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('PhotType') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TSHORT, 10, 1, 1, m_Ra.size(), &m_Reject[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Reject') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 11, 1, 1, m_Ra.size(), &m_QfbobX[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qx') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 12, 1, 1, m_Ra.size(), &m_QfbobY[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qy') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 13, 1, 1, m_Ra.size(), &m_QfbobZ[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qz') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 14, 1, 1, m_Ra.size(), &m_QfbobR[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qr') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
   fits_write_col(m_File, TFLOAT, 15, 1, 1, m_Ra.size(), &m_TfbobX[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Tx') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 16, 1, 1, m_Ra.size(), &m_TfbobY[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Ty') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 17, 1, 1, m_Ra.size(), &m_TfbobZ[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Tz') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 18, 1, 1, m_Ra.size(), &m_QstarX[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qx') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 19, 1, 1, m_Ra.size(), &m_QstarY[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qy') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 20, 1, 1, m_Ra.size(), &m_QstarZ[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qz') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 21, 1, 1, m_Ra.size(), &m_QstarR[0], &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qr') failed (" << Words << ")" << endl;
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
  
  
  float tlmin1 = 0;
  float tlmin2 = 0;
  float tlmin3 = 0;
  float tlmin5 = 0;

  float tlmax1 = ceil((RaMax-RaMin)*cos(DecAvg/rad)/PixelSize);
  float tlmax2 = ceil((DecMax-DecMin)/PixelSize);
  float tlmax3 = 819;
  float tlmax5 = 3;

  fits_write_key(m_File, TFLOAT, "TLMIN4", &tlmin1, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN5", &tlmin2, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN6", &tlmin3, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN9", &tlmin5, "Min value", &Status); 	   
  fits_write_key(m_File, TFLOAT, "TLMAX4", &tlmax1, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX5", &tlmax2, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX6", &tlmax3, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX9", &tlmax5, "Max value", &Status); 	   
   
  if (Status != 0) {
    mgui<<"Error writing event table header: TLInfo!"<<endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }

  //! create GTI table extension
  char ExtensionName[] = "GTI";
  int tfield = 2;
  long nrow = 0;
  char* ttype[] = {"Start","Stop"};
  char* tform[] = {"1D","1D"};
  char* tunit[] = {"s","s"};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating table: "<<ExtensionName<<endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
    
  fits_write_key(m_File, TSTRING, "INSTRUME", const_cast<char*>("FPM"), "Detector", &Status); 	  
  fits_write_key(m_File, TSTRING, "MISSION", const_cast<char*>("NuSTAR"), " ", &Status); 	  
  fits_write_key(m_File, TSTRING, "TELESCOP", const_cast<char*>("NuSIM"), " ", &Status); 
  fits_write_key(m_File, TSTRING, "CREATOR", const_cast<char*>("NuSIM"), " ", &Status);  
  fits_write_key(m_File, TSTRING, "NuSimVER", const_cast<char*>(g_Version.Data()), "NuSim version number", &Status);
  fits_write_key(m_File, TLONG,   "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);
  fits_write_key(m_File, TSTRING, "OBS_ID",   const_cast<char*>(m_ObservationID.Data()), " ", &Status);  
  fits_write_key(m_File, TLONG,   "TARG_ID",  &targ_id, " ", &Status);  
  fits_write_key(m_File, TSTRING, "OBJECT",   const_cast<char*>(m_TargetName.Data()), " ", &Status); 
  fits_write_key(m_File, TDOUBLE, "TSTART", &tstart, "start time", &Status); 	  
  fits_write_key(m_File, TDOUBLE, "TSTOP", &tend, "end time", &Status); 	  	  
  
  fits_write_col(m_File, TDOUBLE, 1, 1, 1, 1, &tstart, &Status);
  fits_write_col(m_File, TDOUBLE, 2, 1, 1, 1, &tend, &Status);
   
  fits_close_file(m_File, &Status);
  
  m_File = 0;
  
  
  // Save also the exposure map
  if (m_CreateExposureMap == true) {
    m_ExposureMap.SetImageParameters(tcrvl1, tcdlt1, tlmax1, tcrvl2, tcdlt2, tlmax2);
    NQuaternion Qfbob;
    MVector Tfbob;
    NQuaternion Qstar;
    NTime Time;
    for (unsigned int i = 0; i < m_Ra.size(); ++i) {
      Qfbob.SetRXYZ(m_QfbobR[i], m_QfbobX[i], m_QfbobY[i], m_QfbobZ[i]);
      Tfbob.SetXYZ(m_TfbobX[i], m_TfbobY[i], m_TfbobZ[i]);
      Qstar.SetRXYZ(m_QstarR[i], m_QstarX[i], m_QstarY[i], m_QstarZ[i]);
      Time = m_Satellite.ConvertFromTimeSinceEpoch(NTime(m_Time[i]));
      m_ExposureMap.AddObservatoryData(Qfbob, Tfbob, Qstar, Time);
    }
    m_ExposureMap.ExposeSky(1);
  }
  
  return true;
}


// NModuleInterfaceEventSaverLevel2Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
