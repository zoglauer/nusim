/*
 * NHousekeeping.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NHousekeeping
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NHousekeeping.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NHousekeeping)
#endif


////////////////////////////////////////////////////////////////////////////////


NHousekeeping::NHousekeeping()
{
  // Construct an instance of NHousekeeping
}


////////////////////////////////////////////////////////////////////////////////


NHousekeeping::~NHousekeeping()
{
  // Delete this instance of NHousekeeping
}


////////////////////////////////////////////////////////////////////////////////


void NHousekeeping::Clean()
{
  NBase::Clean();

  m_ShieldRateLow.clear();
  m_ShieldRateHigh.clear();
  m_SWSAA.clear();
  m_SWTentacled.clear();
  m_SoftSAA.clear();
  m_SoftSAAStrictHSR.clear();
  m_SoftSAAOptimizedHSRFoM.clear();
  m_SoftSAAStrictLSR.clear();
  m_SoftSAAOptimizedLSRRMS.clear();
  m_SoftTentacled.clear();
  m_SoftTentacledFoM.clear();
  m_SoftTentacledRMS.clear();
  m_LiveTime.clear();
  m_NAcceptedEvents.clear();
  m_NRejectedEvents.clear();  
}


////////////////////////////////////////////////////////////////////////////////


bool NHousekeeping::Read(const TString& FileName)
{
  cout<<"Reading housekeeping from file "<<FileName<<endl;
 
  if (MFile::Exists(FileName) == false) {
    cout<<"The file \""<<FileName<<"\" does not exists!"<<endl;
    return false;
  }
  
  // Section 1:
  // initialize the files
  int status = 0;
  char value[1000];
  char templt[100];

  int anynul;
  
  double nulldbl = 0.0;
  double valdbl = 0;
  
  int nullint = 0;
  int valint = 0;
  
  fitsfile* File = 0;
  if (fits_open_file(&File, FileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open housekeeping file \""<<FileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  //cout<<"Opened "<<FileName<<endl;

  // Move to the second hdu
  fits_movabs_hdu(File, 2, NULL, &status);

  // Get the number of columns
  int NColumns = 0;
  map<TString, int> Columns;
  fits_get_num_cols(File, &NColumns, &status);
  for (int c = 1; c <= NColumns; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(File, CASEINSEN, templt, value, &col, &status);
    Columns[value] = c;
    //cout<<value<<endl;
  }
  //cout<<"N columns: "<<NColumns<<endl;

  // Get the number of rows:
  long NRows = 0;
  fits_get_num_rows(File, &NRows, &status);
  //cout<<"Rows: "<<NRows<<endl;

  // Loop over the events and all columns and generate MComptonEvent's
  for (long r = 1; r <= NRows; ++r) {

    if (fits_read_col_dbl(File, Columns["TIME"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      fits_get_errstatus(status, value);
      cout<<"Column read (time, r="<<r<<", c="<<Columns["TIME"]<<") failed: "<<value<<endl;
      break;
    }
    double Time = valdbl;
    
    if (fits_read_col_int(File, Columns["SHLDLO"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SHLDLO) failed!"<<endl;
      continue;
    }
    int ShieldLow = valint;
    if (fits_read_col_int(File, Columns["SHLDHI"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SHLDHI) failed!"<<endl;
      continue;
    }
    int ShieldHigh = valint;
    
    if (fits_read_col_int(File, Columns["NACCEPT"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (NACCEPT) failed!"<<endl;
      continue;
    }
    int NAcceptedEvents = valint;
    
    if (fits_read_col_int(File, Columns["NREJECT"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (NREJECT) failed!"<<endl;
      continue;
    }
    int NRejectedEvents = valint;
    
    if (fits_read_col_dbl(File, Columns["LIVETIME"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (LiveTime) failed!"<<endl;
      continue;
    }
    double LiveTime = valdbl;
    
    int SW_SAA = 0;
    if (!fits_read_col_int(File, Columns["SW_SAA"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      SW_SAA = valint;
    } else {
      status = 0; 
    }
    int SW_Tentacle = 0;
    if (!fits_read_col_int(File, Columns["SW_TENTACLE"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      SW_Tentacle = valint;
    } else {
      status = 0; 
    }
    
    Add(Time, ShieldLow, ShieldHigh, LiveTime, NAcceptedEvents, NRejectedEvents, SW_SAA, SW_Tentacle);
  }

  return true;
}




// NHousekeeping.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
