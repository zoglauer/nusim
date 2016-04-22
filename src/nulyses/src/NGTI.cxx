/*
 * NGTI.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGTI
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGTI.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGTI)
#endif


////////////////////////////////////////////////////////////////////////////////


NGTI::NGTI()
{
  // Construct an instance of NGTI
}


////////////////////////////////////////////////////////////////////////////////


NGTI::~NGTI()
{
  // Delete this instance of NGTI
}


////////////////////////////////////////////////////////////////////////////////


void NGTI::Clean()
{
  NBase::Clean();

  m_Start.clear();
  m_Stop.clear(); 
}


////////////////////////////////////////////////////////////////////////////////


bool NGTI::Read(const TString& FileName)
{
  cout<<"Reading GTI from file "<<FileName<<endl;
 
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
  
  fitsfile* File = 0;
  if (fits_open_file(&File, FileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open GTI file \""<<FileName<<"\": "<<status<<":"<<value<<endl;
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

  // Loop over the events and all columns and generate 
  for (long r = 1; r <= NRows; ++r) {

    if (fits_read_col_dbl(File, Columns["START"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      fits_get_errstatus(status, value);
      cout<<"Column read (time, r="<<r<<", c="<<Columns["START"]<<") failed: "<<value<<endl;
      break;
    }
    double Start = valdbl;
    if (fits_read_col_dbl(File, Columns["STOP"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      fits_get_errstatus(status, value);
      cout<<"Column read (time, r="<<r<<", c="<<Columns["STOP"]<<") failed: "<<value<<endl;
      break;
    }
    double Stop = valdbl;

    //cout<<"Adding: "<<Start<<"-"<<Stop<<endl;
    Add(Start, Stop);
  }

  return true;
}




// NGTI.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
