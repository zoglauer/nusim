/*
 * NEngineering.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NEngineering
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NEngineering.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NEngineering)
#endif


////////////////////////////////////////////////////////////////////////////////


NEngineering::NEngineering()
{
  // Construct an instance of NEngineering
}


////////////////////////////////////////////////////////////////////////////////


NEngineering::~NEngineering()
{
  // Delete this instance of NEngineering
}


////////////////////////////////////////////////////////////////////////////////


void NEngineering::Clean()
{
  NBase::Clean();

  m_EarthAngle.clear();
  m_LimbAngle.clear();  
}


////////////////////////////////////////////////////////////////////////////////


bool NEngineering::Read(const TString& EngineeringFileName)
{
  cout<<"Reading engineering data from file "<<EngineeringFileName<<endl;
 
  if (MFile::Exists(EngineeringFileName) == false) {
    cout<<"The file \""<<EngineeringFileName<<"\" does not exists!"<<endl;
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
  
  fitsfile* EventFile = 0;
  if (fits_open_file(&EventFile, EngineeringFileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open engineering file \""<<EngineeringFileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  //cout<<"Opened "<<OrbitFileName<<endl;

  // Move to the right hdu
  fits_movabs_hdu(EventFile, 12, NULL, &status);

  // Get the number of columns
  int NColumns = 0;
  map<TString, int> Columns;
  fits_get_num_cols(EventFile, &NColumns, &status);
  for (int c = 1; c <= NColumns; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(EventFile, CASEINSEN, templt, value, &col, &status);
    Columns[value] = c;
    //cout<<value<<endl;
  }
  //cout<<"N columns: "<<NColumns<<endl;

  // Get the number of rows:
  long NRows = 0;
  fits_get_num_rows(EventFile, &NRows, &status);
  //cout<<"Rows: "<<NRows<<endl;

  // Loop over the events and all columns and generate MComptonEvent's
  for (long r = 1; r <= NRows; ++r) {

    if (fits_read_col_dbl(EventFile, Columns["TIME"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (time) failed!"<<endl;
      break;
    }
    double Time = valdbl;
    
    if (fits_read_col_dbl(EventFile, Columns["chu04_to_earth_angle"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (chu04_to_earth_angle) failed!"<<endl;
      break;
    }
    double EarthAngle = valdbl;
 
    if (fits_read_col_dbl(EventFile, Columns["chu04_to_limb_angle"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (chu04_to_limb_angle) failed!"<<endl;
      break;
    }
    double LimbAngle = valdbl;
 
    Add(Time, EarthAngle, LimbAngle);
  }
  
  return true;
}


// NEngineering.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
