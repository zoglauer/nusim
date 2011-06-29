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

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceStarTrackerSaverLevel1Fits::SaveAsLevel1Fits(NStarTrackerData& Data)
{
  // Copy relevant data to the fits file
  
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceStarTrackerSaverLevel1Fits::CloseLevel1FitsFile()
{
  // Close the file    

  int Status = 0;
   
  fits_close_file(m_File, &Status);
  
  m_File = 0;
  
  return true;
}


// NModuleInterfaceStarTrackerSaverLevel1Fits.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
