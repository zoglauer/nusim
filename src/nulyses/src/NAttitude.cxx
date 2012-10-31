/*
 * NAttitude.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NAttitude
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NAttitude.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NAttitude)
#endif


////////////////////////////////////////////////////////////////////////////////


NAttitude::NAttitude()
{
  // Construct an instance of NAttitude
}


////////////////////////////////////////////////////////////////////////////////


NAttitude::~NAttitude()
{
  // Delete this instance of NAttitude
}


////////////////////////////////////////////////////////////////////////////////


void NAttitude::Clean()
{
  NBase::Clean();

  m_RA.clear();
  m_DEC.clear();  
}


////////////////////////////////////////////////////////////////////////////////


bool NAttitude::Read(const TString& AttitudeFileName)
{
  cout<<"Reading attitude from file "<<AttitudeFileName<<endl;
 
  if (MFile::Exists(AttitudeFileName) == false) {
    cout<<"The file \""<<AttitudeFileName<<"\" does not exists!"<<endl;
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
  if (fits_open_file(&EventFile, AttitudeFileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open Attitude file \""<<AttitudeFileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  //cout<<"Opened "<<AttitudeFileName<<endl;

  // Move to the second hdu
  fits_movabs_hdu(EventFile, 2, NULL, &status);

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
        
    if (fits_read_col_dbl(EventFile, Columns["POINTING"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (POINTING) failed!"<<endl;
      break;
    }
    double Pointing1 = valdbl;
    
    if (fits_read_col_dbl(EventFile, Columns["POINTING"], r, 2, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (POINTING) failed!"<<endl;
      break;
    }
    double Pointing2 = valdbl;
    
    Add(Time, Pointing1, Pointing2);
  }
  
  /*
  cout<<"Occs:"<<endl;
  for (unsigned int o = 0; o < m_OccultationSteps.size(); ++o) {
    cout<<setprecision(10)<<m_OccultationSteps[o]<<": "<<m_OccultationValues[o]<<endl;
    if (m_OccultationValues[o] == 0 && o > 0) {
      cout<<"Occultation duration: "<<m_OccultationSteps[o] - m_OccultationSteps[o-1]<<endl; 
    }
  }
  */
  
  return true;
}


// NAttitude.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
