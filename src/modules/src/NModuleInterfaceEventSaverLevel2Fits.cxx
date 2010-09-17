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

  cout<<"OpenFits"<<endl;

  MFile::ExpandFileName(FileName);
  
  FileName = "!"+FileName; // To overwrite file
  
  int Status = 0;
  m_File = 0;
  
  Reference_Ra = m_Satellite.GetPointing(0).GetRa();
  Reference_Dec = m_Satellite.GetPointing(0).GetRa();
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
  int tfield = 3;
  long nrow = 0;
  char *ttype[] = {"X", "Y", "PHA"};
  char *tform[] = {"1E","1E","1E"};
  char *tunit[] = {"pixel","pixel","keV"};
  
  fits_create_tbl(m_File, BINARY_TBL, nrow, tfield, 
    ttype, tform, tunit, ExtensionName, &Status); 
  cout<<Status<<endl;
    
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

  cout<<"SaveFits"<<endl;
  
  int Status = 0;
  
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
  
    double Ra = (Event.GetHit(i).GetObservatoryData().GetRa()-Reference_Ra*60.)*60./Pixsize;
    double Dec = (Event.GetHit(i).GetObservatoryData().GetDec()-Reference_Dec*60.)*60/Pixsize;
    double Energy = Event.GetHit(i).GetEnergy();

    c1[counter]= Ra;
    c2[counter]= Dec;
    c3[counter]= Energy;

    //! Save 1000 events at a time
    if (counter == 999) {
      fits_write_col(m_File, TFLOAT, 1, firstrow, firstelem, 1000, c1, &Status);
      fits_write_col(m_File, TFLOAT, 2, firstrow, firstelem, 1000, c2, &Status);
      fits_write_col(m_File, TFLOAT, 3, firstrow, firstelem, 1000, c3, &Status);
      firstrow = firstrow + 999;
      
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
    for (int i = 0; i < counter; ++i) {
      rc1[i] = c1[i];
      rc2[i] = c2[i];
      rc3[i] = c3[i];
    }
    fits_write_col(m_File, TFLOAT, 1, firstrow, firstelem, counter, rc1,
                   &Status);
    fits_write_col(m_File, TFLOAT, 2, firstrow, firstelem, counter, rc2,
                   &Status);
    fits_write_col(m_File, TFLOAT, 3, firstrow, firstelem, counter, rc3,
                   &Status);   
  }
   
  fits_close_file(m_File, &Status);
  
  m_File = 0;

  return true;
}


// NModuleInterfaceEventSaverLevel2Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
