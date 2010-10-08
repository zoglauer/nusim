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
  
  Reference_Ra = m_Satellite.GetPointing(0).GetRa()/60.0;
  Reference_Dec = m_Satellite.GetPointing(0).GetDec()/60.0;
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
  int tfield = 11;
  long nrow = 0;
  char *ttype[] = {"X","Y","PHA","Time","grade","opticX","opticY","OpticZ","opticVx","opticVy","opticVz"};
  char *tform[] = {"1E","1E","1E","1E","1I","1E","1E","1E","1E","1E","1E"};
  char *tunit[] = {"pixel","pixel","keV","seconds","grade","mm","mm","mm","unit","unit","unit"};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    mgui<<"Error in creating extension: "<<endl;
    mgui<<FileName<<show;
    m_File = 0;
    return false;
  }

  //! Write WCS header keywords   
  char deg[10], tctyp1[10], tctyp2[10],radesys[10];
  float tcrvl1=Reference_Ra;
  float tcrvl2=Reference_Dec;
  float tcdlt1=Pixsize/3600.;
  float tcdlt2=tcdlt1;
  float tcrpx1=0.0, tcrpx2=0.0;
  
  strcpy(deg,"deg");
  strcpy(tctyp1,"RA---TAN");
  strcpy(tctyp2,"DEC--TAN");
  strcpy(radesys,"FK5");
  
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
 
   //! Write NuSim header keywords
  char version[10];
  strcpy(version,g_Version);
  
  fits_write_key(m_File, TSTRING, "NuSimVER", version, "NuSim version number", &Status);
  fits_write_key(m_File, TLONG, "NuSimSVN", &g_SVNRevision, "NuSim SVN reversion number", &Status);

  firstelem = 1;
  firstrow = 1; 
  counter = 0;
 
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::SaveEventLevel2Fits(NEvent& Event)
{
  //! Main data analysis routine, which updates the event to a new level 

  // cout<<"SaveFits"<<endl;
  
  int Status = 0;
  
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
  
	double Dec = Event.GetHit(i).GetObservatoryData().GetDec();
   	double Ra = Event.GetHit(i).GetObservatoryData().GetRa();
	Ra = Reference_Ra*60.+(Ra-Reference_Ra*60.)*cos(Reference_Dec/c_Deg);
	double XPix = (Ra-Reference_Ra*60.)*60./Pixsize;
	double YPix = (Dec-Reference_Dec*60.)*60/Pixsize;
	double Energy = Event.GetHit(i).GetEnergy();
    NTime Time = Event.GetTime();
   
	NPhoton OriginalPhoton = Event.GetInitialPhotonRelOM();	
   
	c1[counter]= XPix;
    c2[counter]= YPix;
    c3[counter]= Energy;
    c4[counter]= float(Time.GetSeconds());
	c5[counter]= Event.GetOrigin();
	c6[counter]= OriginalPhoton.GetPosition()[0];
	c7[counter]= OriginalPhoton.GetPosition()[1];
	c8[counter]= OriginalPhoton.GetPosition()[2];
	c9[counter]= OriginalPhoton.GetDirection()[0];
	c10[counter]= OriginalPhoton.GetDirection()[1];
	c11[counter]= OriginalPhoton.GetDirection()[2];

    //! Save a chunk of 1000 events at a time

    if (counter == 999) {

      fits_write_col(m_File, TFLOAT, 1, firstrow, firstelem, 1000, c1, &Status);
      fits_write_col(m_File, TFLOAT, 2, firstrow, firstelem, 1000, c2, &Status);
      fits_write_col(m_File, TFLOAT, 3, firstrow, firstelem, 1000, c3, &Status);
	  fits_write_col(m_File, TFLOAT, 4, firstrow, firstelem, 1000, c4, &Status);
	  fits_write_col(m_File, TFLOAT, 5, firstrow, firstelem, 1000, c5, &Status);
      fits_write_col(m_File, TFLOAT, 6, firstrow, firstelem, 1000, c6, &Status);
      fits_write_col(m_File, TFLOAT, 7, firstrow, firstelem, 1000, c7, &Status);
      fits_write_col(m_File, TFLOAT, 8, firstrow, firstelem, 1000, c8, &Status);
      fits_write_col(m_File, TFLOAT, 9, firstrow, firstelem, 1000, c9, &Status);
	  fits_write_col(m_File, TFLOAT, 10, firstrow, firstelem, 1000, c10, &Status);
      fits_write_col(m_File, TFLOAT, 11, firstrow, firstelem, 1000, c11, &Status);
	  firstrow = firstrow + 1000;

	  counter = 0;
    } else {
      ++counter;
    }
  }
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceEventSaverLevel2Fits::CloseLevel2FitsFile()
{
  //! Close the file    

  cout<<"CloseFits"<<endl;
  
  int Status = 0;

  //! save the remaining data before closing 
  if (counter != 0) {
	float rc1[counter];
	float rc2[counter];
	float rc3[counter];
	float rc4[counter];
	float rc5[counter];
	float rc6[counter];
	float rc7[counter];
	float rc8[counter];
	float rc9[counter];
    float rc10[counter];
	float rc11[counter];
	for (int i=0; i < counter; i++) {
	  rc1[i] = c1[i];
	  rc2[i] = c2[i];
	  rc3[i] = c3[i];
	  rc4[i] = c4[i];
	  rc5[i] = c5[i];
	  rc6[i] = c6[i];
	  rc7[i] = c7[i];
	  rc8[i] = c8[i];
	  rc9[i] = c9[i];
	  rc10[i] = c10[i];
	  rc11[i] = c11[i];
	}
	fits_write_col(m_File, TFLOAT, 1, firstrow, firstelem, counter, rc1, &Status);
    fits_write_col(m_File, TFLOAT, 2, firstrow, firstelem, counter, rc2, &Status);
    fits_write_col(m_File, TFLOAT, 3, firstrow, firstelem, counter, rc3, &Status);
	fits_write_col(m_File, TFLOAT, 4, firstrow, firstelem, counter, rc4, &Status);
	fits_write_col(m_File, TFLOAT, 5, firstrow, firstelem, counter, rc5, &Status);
    fits_write_col(m_File, TFLOAT, 6, firstrow, firstelem, counter, rc6, &Status);
    fits_write_col(m_File, TFLOAT, 7, firstrow, firstelem, counter, rc7, &Status);
    fits_write_col(m_File, TFLOAT, 8, firstrow, firstelem, counter, rc8, &Status);
    fits_write_col(m_File, TFLOAT, 9, firstrow, firstelem, counter, rc9, &Status);
	fits_write_col(m_File, TFLOAT, 10, firstrow, firstelem, counter, rc10, &Status);
	fits_write_col(m_File, TFLOAT, 11, firstrow, firstelem, counter, rc11, &Status);
  }
   
  fits_close_file(m_File, &Status);
  
  m_File = 0;

  return true;
}


// NModuleInterfaceEventSaverLevel2Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
