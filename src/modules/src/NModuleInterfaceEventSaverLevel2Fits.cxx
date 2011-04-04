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
#include "math.h"

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
  
  Reference_Ra = m_Satellite.GetPointing(0).GetRa();
  Reference_Dec = m_Satellite.GetPointing(0).GetDec();
  Pixsize = 6.;
  
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
    mgui<<"Error in creating extension: "<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  firstelem = 1;
  firstrow = 1; 
  counter = 0;
  minRa = maxRa = Reference_Ra;  // arcmin
  minDec = maxDec = Reference_Dec;  // arcmin

 
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::SaveEventLevel2Fits(NEvent& Event)
{
  //! Main data analysis routine, which updates the event to a new level 

  // cout<<"SaveFits"<<endl;
  
  // int Status = 0;
  
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
  
	double Dec = Event.GetHit(i).GetObservatoryData().GetDec();
   	double Ra = Event.GetHit(i).GetObservatoryData().GetRa();

	if (Dec > maxDec) maxDec=Dec;
	if (Ra > maxRa) maxRa=Ra;
	if (Dec < minDec) minDec=Dec;
	if (Ra < minRa) minRa=Ra;
	
	double Energy = Event.GetHit(i).GetEnergy();
    NTime Time = Event.GetTime();
   
	NPhoton OriginalPhoton = Event.GetInitialPhotonRelOM();	
   
	c1.push_back(Ra);
    c2.push_back(Dec);
    c3.push_back(Energy);
    c4.push_back(float(Time.GetSeconds()));
	c5.push_back(Event.GetOrigin());
	// Deactivating the origin and direction of photon
	/*c6.push_back(OriginalPhoton.GetPosition()[0]);
	c7.push_back(OriginalPhoton.GetPosition()[1]);
	c8.push_back(OriginalPhoton.GetPosition()[2]);
	c9.push_back(OriginalPhoton.GetDirection()[0]);
	c10.push_back(OriginalPhoton.GetDirection()[1]);
	c11.push_back(OriginalPhoton.GetDirection()[2]);*/
   
    counter++;

     }
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::CloseLevel2FitsFile()
{
  //! Close the file    

  int Status = 0;
  
  //! Write WCS header keywords   
  char deg[10], tctyp1[10], tctyp2[10],radesys[10],object[10];
  char instrume[10], mission[10], telescope[10];
  float tcrvl1=maxRa/60.;
  float tcrvl2=minDec/60.;
  float tcdlt1=-Pixsize/3600.;
  float tcdlt2=-tcdlt1;
  float tcrpx1=0.0, tcrpx2=0.0;
  
  strcpy(deg,"deg");
  strcpy(tctyp1,"RA---TAN");
  strcpy(tctyp2,"DEC--TAN");
  strcpy(radesys,"FK5");
  strcpy(object,"MyObj");
  strcpy(instrume,"FPM");
  strcpy(mission,"NuSim");
  strcpy(telescope,"NuSim");
  
  float Time_Start[] = { 0.0 };
  float Time_End[] = { 0.0 };

  if (c4.size() > 0) {
    Time_Start[0] = c4[0];
    Time_End[0] = c4[counter-1];
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
  fits_write_key(m_File, TSTRING, "TCRUNI1", deg," ", &Status);
  fits_write_key(m_File, TSTRING, "TCRUNI2", deg," ", &Status);
  fits_write_key(m_File, TSTRING, "TCTYP1", tctyp1," ", &Status);
  fits_write_key(m_File, TSTRING, "TCTYP2", tctyp2," ", &Status); 
  fits_write_key(m_File, TSTRING, "RADESYS", radesys, " ", &Status);
  fits_write_key(m_File, TSTRING, "OBJECT", object, "NuSim object", &Status);
  
  
   //! Write NuSim header keywords
  char version[10];
  strcpy(version,g_Version);
  
  fits_write_key(m_File, TSTRING, "NuSimVER", version, "NuSim version number", &Status);
  fits_write_key(m_File, TLONG, "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);

 
  float rc1[counter],rc2[counter],
        rc3[counter],rc4[counter],
        rc5[counter];
		/*,rc6[counter],
		rc7[counter],rc8[counter],
		rc9[counter],rc10[counter],
		rc11[counter];*/

  //! save the data before closing  
  for (unsigned int i = 0; i < counter; ++i) {
    float Ra = maxRa+(c1[i]-maxRa)*cos(minDec/c_Deg/60.);
    rc1[i] = (maxRa-Ra)*60./Pixsize;
    rc2[i] = (c2[i]-minDec)*60/Pixsize;
    rc3[i] = c3[i];
    rc4[i] = c4[i];
    rc5[i] = c5[i];
    /*
    rc6[i] = c6[i];
    rc7[i] = c7[i];
    rc8[i] = c8[i];
    rc9[i] = c9[i];
    rc10[i] = c10[i];
    rc11[i] = c11[i];
    */
  }
  
  fits_write_col(m_File, TFLOAT, 1, firstrow, firstelem, counter, rc1, &Status);
  fits_write_col(m_File, TFLOAT, 2, firstrow, firstelem, counter, rc2, &Status); 
  fits_write_col(m_File, TFLOAT, 3, firstrow, firstelem, counter, rc3, &Status);
  fits_write_col(m_File, TFLOAT, 4, firstrow, firstelem, counter, rc4, &Status);
  fits_write_col(m_File, TFLOAT, 5, firstrow, firstelem, counter, rc5, &Status);
  /*fits_write_col(m_File, TFLOAT, 6, firstrow, firstelem, counter, rc6, &Status);
  fits_write_col(m_File, TFLOAT, 7, firstrow, firstelem, counter, rc7, &Status);
  fits_write_col(m_File, TFLOAT, 8, firstrow, firstelem, counter, rc8, &Status);
  fits_write_col(m_File, TFLOAT, 9, firstrow, firstelem, counter, rc9, &Status);
  fits_write_col(m_File, TFLOAT, 10, firstrow, firstelem, counter, rc10, &Status);
  fits_write_col(m_File, TFLOAT, 11, firstrow, firstelem, counter, rc11, &Status);*/
  
  float tlmin1 = 0;
  float tlmin2 = 0;
  float tlmin3 = 0;
  float tlmin5 = 1;
  float tlmax1 = 0;
  if ( (maxRa-minRa) > (maxDec-minDec)) {
    tlmax1 = (maxRa-minRa)*60./Pixsize;
  } else {
    tlmax1 = (maxDec-minDec)*60./Pixsize;
  }
  float tlmax2 = tlmax1;
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
   
  //! create GTI table extension
  char ExtensionName[] = "GTI";
  int tfield = 2;
  long nrow = 0;
  char *ttype[] = {"Start","Stop"};
  char *tform[] = {"1E","1E"};
  char *tunit[] = {"s","s"};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating extension: "<<endl;
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
