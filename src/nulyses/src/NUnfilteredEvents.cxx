/*
 * NUnfilteredEvents.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NUnfilteredEvents
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NUnfilteredEvents.h"

// Standard libs:
#include <algorithm>
using namespace std;

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NUnfilteredEvents)
#endif


////////////////////////////////////////////////////////////////////////////////


NUnfilteredEvents::NUnfilteredEvents()
{
  // Construct an instance of NUnfilteredEvents
}


////////////////////////////////////////////////////////////////////////////////


NUnfilteredEvents::~NUnfilteredEvents()
{
  // Delete this instance of NUnfilteredEvents
}


////////////////////////////////////////////////////////////////////////////////


void NUnfilteredEvents::Clean()
{
  NBase::Clean();

  m_Energy.clear();
  m_SurrEnergy.clear();
  m_Prior.clear();
  m_Reset.clear();
  m_Grade.clear();
  m_DetectorID.clear();
  m_RawX.clear();
  m_RawY.clear();
  m_ShieldVeto.clear();
  m_BaseLine.clear();
  m_PrePhase.clear();
  m_PostPhase.clear();
  m_Status.clear();  
}


////////////////////////////////////////////////////////////////////////////////


int NUnfilteredEvents::FindShieldVeto(double Time) {
  vector<double>::iterator I = find(m_Time.begin(), m_Time.end(), Time);
  if (I != m_Time.end()) {
    return m_ShieldVeto[I-m_Time.begin()];
  }
  
  cout<<"FindShieldVeto: Time not found: "<<Time<<endl;
  return -1;
}


////////////////////////////////////////////////////////////////////////////////


bool NUnfilteredEvents::Read(const TString& FileName)
{
  cout<<"Reading unfiltered events from "<<FileName<<endl;
 
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
  
  unsigned short valusht = 0;

  
  fitsfile* File = 0;
  if (fits_open_file(&File, FileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open unfiltered events file \""<<FileName<<"\": "<<status<<":"<<value<<endl;
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

  // Loop over the events and all columns
  for (long r = 1; r <= NRows; ++r) {

    if (fits_read_col_dbl(File, Columns["TIME"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (time) failed!"<<endl;
      break;
    }
    double Time = valdbl;
    
    if (fits_read_col_dbl(File, Columns["PI"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (PI) failed!"<<endl;
      break;
    }
    double Energy = 0.04*valdbl+1.6; //valdbl/10.0 + 3.0;

    if (fits_read_col_int(File, Columns["SURRPI"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SURRPI) failed!"<<endl;
      break;
    }
    //int SurrPI = valint;
    double SurrEnergy = 0.04*double(valint)+1.6;
    
    if (fits_read_col_int(File, Columns["GRADE"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (GRADE) failed!"<<endl;
      break;
    }
    int Grade = valint;
    
    if (fits_read_col_int(File, Columns["DET_ID"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (DET_ID) failed!"<<endl;
      break;
    }
    int DetectorID = valint;
    
    if (fits_read_col_int(File, Columns["RAWX"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (RAWX) failed!"<<endl;
      break;
    }
    int RawX = valint;
    
    if (fits_read_col_int(File, Columns["RAWY"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (RAWY) failed!"<<endl;
      break;
    }
    int RawY = valint;
    
    if (fits_read_col_dbl(File, Columns["PRIOR"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (PRIOR) failed!"<<endl;
      break;
    }
    double Prior = valdbl;
    
    if (fits_read_col_dbl(File, Columns["RESET"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
      cout<<"Column read (RESET) failed!"<<endl;
      break;
    }
    double Reset = valdbl;
    
    if (fits_read_col_int(File, Columns["SHIELD"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (SHIELD) failed!"<<endl;
      break;
    }
    int ShieldVeto = valint;
    
    if (fits_read_col_int(File, Columns["PREPHAS"], r, 5, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (PREPHAS) failed!"<<endl;
      break;
    }
    int Baseline = valint;
    
    vector<int> PrePhase(9);
    for (unsigned int i = 0; i < PrePhase.size(); ++i) {
      if (fits_read_col_int(File, Columns["PREPHAS"], r, i+1, 1, nullint, &valint, &anynul, &status) ) {
        cout<<"Column read (PREPHAS) failed!"<<endl;
        break;
      } 
      PrePhase[i] = valint;
    }
    
    vector<int> PostPhase(9);
    for (unsigned int i = 0; i < PrePhase.size(); ++i) {
      if (fits_read_col_int(File, Columns["POSTPHAS"], r, i+1, 1, nullint, &valint, &anynul, &status) ) {
        cout<<"Column read (PREPHAS) failed!"<<endl;
        break;
      } 
      PostPhase[i] = valint;
    }

    if (fits_read_col_bit_usht(File, Columns["STATUS"], r, 1, 1, 16, &valusht, &status) ) {
      fits_get_errstatus(status, value);
      cout<<"Column read (STATUS) failed: "<<status<<":"<<value<<endl;
      break;
    }
    unsigned long Status = valusht;
    
    Add(Time, Energy, SurrEnergy, Grade, DetectorID, RawX, RawY, ShieldVeto, Baseline, PrePhase, PostPhase, Prior, Reset, Status);
  }

  return true;
}

// NUnfilteredEvents.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
