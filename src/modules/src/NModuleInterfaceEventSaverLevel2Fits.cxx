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
ClassImp(NModuleInterfaceEventSaverLevel2Fits)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceEventSaverLevel2Fits::NModuleInterfaceEventSaverLevel2Fits(NSatellite& Satellite) : m_Satellite(Satellite)
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
  // Deactivating the direction and origin of photon to save space since the precollimator has been scrapped.
  //int tfield = 11;
  //char *ttype[] = {"X","Y","PHA","Time","grade","opticX","opticY","OpticZ","opticVx","opticVy","opticVz"};
  //char *tform[] = {"1E","1E","1E","1E","1I","1E","1E","1E","1E","1E","1E"};
  //char *tunit[] = {"pixel","pixel","keV","s","grade","mm","mm","mm","unit","unit","unit"};
  int tfield = 5;
  char* ttype[] = {"X","Y","PHA","Time","grade"};
  char* tform[] = {"1E","1E","1E","1E","1I"};
  char* tunit[] = {"pixel","pixel","keV","s","grade"};
  
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
    m_Time.push_back(float(Event.GetTime().GetSeconds()));
	  m_Origin.push_back(Event.GetOrigin());
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
  char instrume[10], mission[10], telescope[10];
  float tcrvl1=RaMax/deg;
  float tcrvl2=DecMin/deg;
  float tcdlt1=-PixelSize/deg;
  float tcdlt2=-tcdlt1;
  float tcrpx1=0.0, tcrpx2=0.0;
  
  strcpy(degree,"deg");
  strcpy(tctyp1,"RA---TAN");
  strcpy(tctyp2,"DEC--TAN");
  strcpy(radesys,"FK5");
  strcpy(object,"MyObj");
  strcpy(instrume,"FPM");
  strcpy(mission,"NuSim");
  strcpy(telescope,"NuSim");
  
  float Time_Start[] = { 0.0 };
  float Time_End[] = { 0.0 };

  if (m_Time.size() > 0) {
    Time_Start[0] = m_Time[0];
    Time_End[0] = m_Time[m_Ra.size()-1];
  }
  
  fits_write_key(m_File, TSTRING, "INSTRUME", instrume, "Detector", &Status); 	  
  fits_write_key(m_File, TSTRING, "MISSION", mission, " ", &Status); 	  
  fits_write_key(m_File, TSTRING, "TELESCOP", telescope, " ", &Status); 
  fits_write_key(m_File, TFLOAT, "TSTART", Time_Start, "start time", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TSTOP", Time_End, "end time", &Status); 	  	  
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
  float* fRa = new float[m_Ra.size()];
  float* fDec = new float[m_Ra.size()];
  float* fEnergy = new float[m_Ra.size()];
  float* fTime = new float[m_Ra.size()];
  float* fOrigin = new float[m_Ra.size()];

  //! save the data before closing  
  for (unsigned int i = 0; i < m_Ra.size(); ++i) {
    float Ra = RaMax+(m_Ra[i]-RaMax)*cos(DecMin/rad);
    fRa[i] = (RaMax-Ra)/PixelSize;
    fDec[i] = (m_Dec[i]-DecMin)/PixelSize;
    fEnergy[i] = m_Energy[i];
    fTime[i] = m_Time[i];
    fOrigin[i] = m_Origin[i];
  }
  
  fits_write_col(m_File, TFLOAT, 1, 1, 1, m_Ra.size(), fRa, &Status);
  fits_write_col(m_File, TFLOAT, 2, 1, 1, m_Ra.size(), fDec, &Status); 
  fits_write_col(m_File, TFLOAT, 3, 1, 1, m_Ra.size(), fEnergy, &Status);
  fits_write_col(m_File, TFLOAT, 4, 1, 1, m_Ra.size(), fTime, &Status);
  fits_write_col(m_File, TFLOAT, 5, 1, 1, m_Ra.size(), fOrigin, &Status);

  if (Status != 0) {
    mgui<<"Error writing event table!"<<endl;
    fits_close_file(m_File, &Status);
    m_File = 0;
    return false;
  }
  
  delete [] fRa;
  delete [] fDec;
  delete [] fEnergy;
  delete [] fTime;
  delete [] fOrigin;


  float tlmin1 = 0;
  float tlmin2 = 0;
  float tlmin3 = 0;
  float tlmin5 = 1;

  float tlmax1 = (RaMax-RaMin)*cos(DecAvg/rad)/PixelSize;
  float tlmax2 = (DecMax-DecMin)/PixelSize;
  float tlmax3 = 100;
  float tlmax5 = 3;

  fits_write_key(m_File, TFLOAT, "TLMIN1", &tlmin1, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN2", &tlmin2, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN3", &tlmin3, "Min value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMIN4", &tlmin5, "Min value", &Status); 	   
  fits_write_key(m_File, TFLOAT, "TLMAX1", &tlmax1, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX2", &tlmax2, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX3", &tlmax3, "Max value", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TLMAX4", &tlmax5, "Max value", &Status); 	   
   
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
  char* tform[] = {"1E","1E"};
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
  fits_write_key(m_File, TFLOAT, "TSTART", Time_Start, "start time", &Status); 	  
  fits_write_key(m_File, TFLOAT, "TSTOP", Time_End, "end time", &Status); 	  	  
  
  fits_write_col(m_File, TFLOAT, 1, 1, 1, 1, Time_Start, &Status);
  fits_write_col(m_File, TFLOAT, 2, 1, 1, 1, Time_End, &Status);
   
  fits_close_file(m_File, &Status);
  
  m_File = 0;
  
  return true;
}


// NModuleInterfaceEventSaverLevel2Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
