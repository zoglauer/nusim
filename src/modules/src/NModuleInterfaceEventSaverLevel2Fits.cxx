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
  int tfield = 19;
  char* ttype[] = {"X","Y","PI","E","Time","grade","phottype","reject","Qx","Qy","Qz","Qr","Tx","Ty","Tz","QSx","QSy","QSz","QSr"};
  char* tform[] = {"1E","1E","1J","1E","1D","1I","1I","1I","1E","1E","1E","1E","1E","1E","1E","1E","1E","1E","1E"};
  char* tunit[] = {"pixel","pixel","channel","keV","s","grade","photon type","rejection type","unit","unit","unit","unit","mm","mm","mm","unit","unit","unit","unit"};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating extension: "<<ExtensionName<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  m_Ra.clear();
  m_Dec.clear();
  m_Energy.clear();
  m_Time.clear();
  m_Origin.clear();
  m_Reject.clear();
  m_Grade.clear();
  m_Qfbob.clear();
  m_Tfbob.clear();
  m_Qstar.clear();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::SaveEventLevel2Fits(NEvent& Event)
{
  //! Main data analysis routine, which updates the event to a new level 
  
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    m_Ra.push_back(Event.GetHit(i).GetObservatoryData().GetRa());
    m_Dec.push_back(Event.GetHit(i).GetObservatoryData().GetDec());
    m_Energy.push_back(Event.GetHit(i).GetEnergy());
    m_Time.push_back(Event.GetTime());
	m_Origin.push_back(Event.GetOrigin());
	if (Event.GetHit(i).GetDepthCut()==false) m_Reject.push_back(0);
    if (Event.GetHit(i).GetDepthCut()==true) m_Reject.push_back(1);
    m_Grade.push_back(Event.GetNinePixelHit(i).GetTriggerGrade());
    m_Qfbob.push_back(Event.GetHit(i).GetObservatoryData().GetOrientationFocalPlaneToOB().GetRotationQuaternion());
    m_Tfbob.push_back(Event.GetHit(i).GetObservatoryData().GetOrientationFocalPlaneToOB().GetTranslation());
	  m_Qstar.push_back(Event.GetHit(i).GetObservatoryData().GetOrientationOBToIS().GetRotationQuaternion());
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
  double PixelSize = 6.0*arcsec;
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
      m_Time.erase(m_Time.begin() + Diff);
      m_Origin.erase(m_Origin.begin() + Diff);
    } else {
      ++I;
    }
  }



  //! Write WCS header keywords   
  char degree[10], tctyp1[10], tctyp2[10],radesys[10],object[10];
  char instrume[10], mission[10], telescope[10], timeunit[10];
  float tcrvl1=RaMax/deg;
  float tcrvl2=DecMin/deg;
  float tcdlt1=-PixelSize/deg;
  float tcdlt2=-tcdlt1;
  float tcrpx1=0.0, tcrpx2=0.0;
  long MDJREFI = 55197;
  float MDJREFF =7.6601852000000E-04; 
  ostringstream out1;
  
  strcpy(timeunit, "s");
  strcpy(degree,"deg");
  strcpy(tctyp1,"RA---TAN");
  strcpy(tctyp2,"DEC--TAN");
  strcpy(radesys,"FK5");
  strcpy(object,"MyObj");
  strcpy(instrume,"FPM");
  strcpy(mission,"NuSim");
  strcpy(telescope,"NuSim");
  
  double tstart = m_Satellite.GetEpochObservationStartTime().GetAsSeconds();
  double tend = m_Satellite.GetEpochObservationEndTime().GetAsSeconds();
  NTime Start = m_Satellite.GetAbsoluteObservationStartTime();
  NTime End = m_Satellite.GetAbsoluteObservationEndTime();

  
  fits_write_key(m_File, TSTRING, "INSTRUME", instrume, "Detector", &Status); 	  
  fits_write_key(m_File, TSTRING, "MISSION", mission, " ", &Status); 	  
  fits_write_key(m_File, TSTRING, "TELESCOP", telescope, " ", &Status); 
  fits_write_key(m_File, TLONG, "MJDREFI", &MDJREFI, "MJD reference day 01 Jan 2010 00:00:00 UTC", &Status);
  fits_write_key(m_File, TFLOAT, "MDJREFF", &MDJREFF , "MJD reference", &Status);
  fits_write_key(m_File, TSTRING, "TIMEUNIT", timeunit, " ", &Status); 
  fits_write_key(m_File, TDOUBLE, "TSTART", &tstart, "start time", &Status); 	  
  fits_write_key(m_File, TDOUBLE, "TSTOP", &tend, "end time", &Status); 	  
  out1<<Start.GetYears()<<"-"<<Start.GetMonths()<<"-"<<Start.GetDays()<<"T"<<Start.GetHours()<<":"<<Start.GetMinutes()<<":"<<Start.GetSeconds();
  char* DateObs = (char*) out1.str().c_str();
  fits_write_key(m_File, TSTRING, "DATE-OBS", DateObs, " ", &Status);
  out1.str("");
  out1<<End.GetYears()<<"-"<<End.GetMonths()<<"-"<<End.GetDays()<<"T"<<End.GetHours()<<":"<<End.GetMinutes()<<":"<<End.GetSeconds();
  char* DateEnd = (char*) out1.str().c_str();
  fits_write_key(m_File, TSTRING, "DATE-END", DateEnd, " ", &Status);	  
  fits_write_key(m_File, TFLOAT, "TCDLT1", &tcdlt1, "Platescale", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCDLT2", &tcdlt2, "Platescale", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCRVL1", &tcrvl1, "Transform to celestrial coords", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCRVL2", &tcrvl2, "Transform to celestrial coords", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCRPX1", &tcrpx1, "Pixel reference point", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TCRPX2", &tcrpx2, "Pixel reference point", &Status); 	  
  fits_write_key(m_File, TSTRING, "TCRUNI1", degree," ", &Status);
  fits_write_key(m_File, TSTRING, "TCRUNI2", degree," ", &Status);
  fits_write_key(m_File, TSTRING, "TCTYP1", tctyp1," ", &Status);
  fits_write_key(m_File, TSTRING, "TCTYP2", tctyp2," ", &Status); 
  fits_write_key(m_File, TSTRING, "RADESYS", radesys, " ", &Status);
  fits_write_key(m_File, TSTRING, "OBJECT", object, "NuSim object", &Status);

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

  // We have to use pointers here to prevent a stack overflow for large data sets!
  float* fRa     = new float[m_Ra.size()];
  float* fDec    = new float[m_Ra.size()];
  float* fEnergy = new float[m_Ra.size()];
  double* dTime   = new double[m_Ra.size()];
  short* fGrade  = new short[m_Ra.size()];
  short* fOrigin = new short[m_Ra.size()];
  short* fReject = new short[m_Ra.size()];
  int*   fPI     = new int  [m_Ra.size()];
  float* fQx     = new float[m_Ra.size()];
  float* fQy     = new float[m_Ra.size()];
  float* fQz     = new float[m_Ra.size()];
  float* fQr     = new float[m_Ra.size()];
  float* fTx     = new float[m_Ra.size()];
  float* fTy     = new float[m_Ra.size()];
  float* fTz     = new float[m_Ra.size()];
  float* fQSx     = new float[m_Ra.size()];
  float* fQSy     = new float[m_Ra.size()];
  float* fQSz     = new float[m_Ra.size()];
  float* fQSr     = new float[m_Ra.size()];

  //! save the data before closing  
  for (unsigned int i = 0; i < m_Ra.size(); ++i) {
    float Ra = RaMax+(m_Ra[i]-RaMax)*cos(DecAvg/rad);
    fRa    [i] = (RaMax-Ra)/PixelSize;
    fDec   [i] = (m_Dec[i]-DecMin)/PixelSize;
    fPI    [i] = (int)((m_Energy[i]-3.0)*10 + 0.5);  // conversion to PI channels
    fEnergy[i] = m_Energy[i];
    dTime  [i] = m_Satellite.ConvertToTimeSinceEpoch(m_Time[i]).GetAsSeconds();
    fOrigin[i] = (short) m_Origin[i];
    fReject[i] = (short) m_Reject[i];
    fGrade [i] = (short) m_Grade[i];
	fQx    [i] = (float) m_Qfbob[i].m_V[0];
	fQy    [i] = (float) m_Qfbob[i].m_V[1];
	fQz    [i] = (float) m_Qfbob[i].m_V[2];
	fQr    [i] = (float) m_Qfbob[i].m_R;
	fTx    [i] = (float) m_Tfbob[i][0];
	fTy    [i] = (float) m_Tfbob[i][1];
	fTz    [i] = (float) m_Tfbob[1][2];
	fQSx   [i] = (float) m_Qstar[i].m_V[0];
	fQSy   [i] = (float) m_Qstar[i].m_V[1];
	fQSz   [i] = (float) m_Qstar[i].m_V[2];
	fQSr   [i] = (float) m_Qstar[i].m_R;
  }
  
  char Words[30];

  fits_write_col(m_File, TFLOAT, 1, 1, 1, m_Ra.size(), fRa, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('RA') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 2, 1, 1, m_Ra.size(), fDec, &Status); 
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('DEC') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TINT, 3, 1, 1, m_Ra.size(), fPI, &Status); 
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('PI') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TFLOAT, 4, 1, 1, m_Ra.size(), fEnergy, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Energy') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TDOUBLE, 5, 1, 1, m_Ra.size(), dTime, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error L2: fits_write_col('Time') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TSHORT, 6, 1, 1, m_Ra.size(), fGrade, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Grade') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TSHORT, 7, 1, 1, m_Ra.size(), fOrigin, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('PhotType') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  fits_write_col(m_File, TSHORT, 8, 1, 1, m_Ra.size(), fReject, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Reject') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 9, 1, 1, m_Ra.size(), fQx, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qx') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 10, 1, 1, m_Ra.size(), fQy, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qy') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 11, 1, 1, m_Ra.size(), fQz, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qz') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 12, 1, 1, m_Ra.size(), fQr, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qr') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
   fits_write_col(m_File, TFLOAT, 13, 1, 1, m_Ra.size(), fTx, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Tx') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 14, 1, 1, m_Ra.size(), fTy, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Ty') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 15, 1, 1, m_Ra.size(), fTz, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Tz') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 16, 1, 1, m_Ra.size(), fQSx, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qx') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 17, 1, 1, m_Ra.size(), fQSy, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qy') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 18, 1, 1, m_Ra.size(), fQSz, &Status);
  if (Status != 0) {
    fits_get_errstatus(Status, Words);
    cerr << "Error: fits_write_col('Qz') failed (" << Words << ")" << endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
 fits_write_col(m_File, TFLOAT, 19, 1, 1, m_Ra.size(), fQSr, &Status);
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
  
  delete [] fRa;
  delete [] fDec;
  delete [] fEnergy;
  delete [] dTime;
  delete [] fOrigin;
  delete [] fPI;
  delete [] fGrade;
  delete [] fQx;
  delete [] fQy;
  delete [] fQz;
  delete [] fQr;
  delete [] fTx;
  delete [] fTy;
  delete [] fTz;
  delete [] fQSx;
  delete [] fQSy;
  delete [] fQSz;
  delete [] fQSr;
  
  float tlmin1 = 0;
  float tlmin2 = 0;
  float tlmin3 = 0;
  float tlmin5 = 0;

  float tlmax1 = (RaMax-RaMin)*cos(DecAvg/rad)/PixelSize;
  float tlmax2 = (DecMax-DecMin)/PixelSize;
  float tlmax3 = 819;
  float tlmax5 = 3;

  fits_write_key(m_File, TFLOAT, "TLMIN1", &tlmin1, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN2", &tlmin2, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN3", &tlmin3, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN5", &tlmin5, "Min value", &Status); 	   
  fits_write_key(m_File, TFLOAT, "TLMAX1", &tlmax1, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX2", &tlmax2, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX3", &tlmax3, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX5", &tlmax5, "Max value", &Status); 	   
   
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
    
  fits_write_key(m_File, TSTRING, "OBJECT", object, "NuSim object", &Status);
  fits_write_key(m_File, TSTRING, "INSTRUME", instrume, "Detector", &Status); 	  
  fits_write_key(m_File, TSTRING, "MISSION", mission, " ", &Status); 	  
  fits_write_key(m_File, TSTRING, "TELESCOP", telescope, " ", &Status); 	
  fits_write_key(m_File, TDOUBLE, "TSTART", &tstart, "start time", &Status); 	  
  fits_write_key(m_File, TDOUBLE, "TSTOP", &tend, "end time", &Status); 	  	  
  
  fits_write_col(m_File, TDOUBLE, 1, 1, 1, 1, &tstart, &Status);
  fits_write_col(m_File, TDOUBLE, 2, 1, 1, 1, &tend, &Status);
   
  fits_close_file(m_File, &Status);
  
  m_File = 0;
  
  
  // Save also the exposure map
  

  m_ExposureMap.SetImageParameters(tcrvl1, tcdlt1, tlmax1, tcrvl2, tcdlt2, tlmax2);
  for (unsigned int i = 0; i < m_Ra.size(); ++i) {
    m_ExposureMap.AddObservatoryData(m_Qfbob[i], m_Tfbob[i], m_Qstar[i], m_Time[i]);
  }
  m_ExposureMap.ExposeSky(1);
  
  
  return true;
}


// NModuleInterfaceEventSaverLevel2Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
