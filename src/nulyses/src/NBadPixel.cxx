/*
 * NBadPixel.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NBadPixel
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBadPixel.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NBadPixel)
#endif


////////////////////////////////////////////////////////////////////////////////


NBadPixel::NBadPixel()
{
  // Construct an instance of NBadPixel
}


////////////////////////////////////////////////////////////////////////////////


NBadPixel::~NBadPixel()
{
  // Delete this instance of NBadPixel
}


////////////////////////////////////////////////////////////////////////////////


void NBadPixel::Clean()
{
  NBase::Clean();

  m_DetectorID.clear();
  m_RawX.clear();  
  m_RawY.clear();  
  m_Start.clear();  
  m_Stop.clear();  
  m_CutGTI.clear();  
  
  m_UpTimes.resize(4*32*32);
}


////////////////////////////////////////////////////////////////////////////////


bool NBadPixel::Read(const TString& BadPixelFileName)
{
  //cout<<"Reading bad pixels from file "<<BadPixelFileName<<endl;
 
  if (MFile::Exists(BadPixelFileName) == false) {
    cout<<"The file \""<<BadPixelFileName<<"\" does not exists!"<<endl;
    return false;
  }
  
  // Section 1:
  // initialize the files
  int status = 0;
  char value[1000];
  char templt[100];

  int anynul;
  
  double nulldbl = 0;
  double valdbl = 0;
  
  int nullint = 0;
  int valint = 0;
  
  fitsfile* EventFile = 0;
  if (fits_open_file(&EventFile, BadPixelFileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open BadPixel file \""<<BadPixelFileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  //cout<<"Opened "<<BadPixelFileName<<endl;

  // Move to the second hdu
  for (int h = 2; h <= 5; ++h) {
    //cout<<"HUD: "<<h<<endl;
    fits_movabs_hdu(EventFile, h, NULL, &status);

    // Get the number of columns
    int NColumns = 0;
    map<TString, int> Columns;
    fits_get_num_cols(EventFile, &NColumns, &status);
    for (int c = 1; c <= NColumns; ++c) {
      int col;
      snprintf(templt, sizeof(templt), "%d", c);
      fits_get_colname(EventFile, CASEINSEN, templt, value, &col, &status);
      Columns[value] = c;
    }
    //cout<<"Columns: "<<NColumns<<endl;

    // Get the number of rows:
    long NRows = 0;
    fits_get_num_rows(EventFile, &NRows, &status);
    //cout<<"Rows: "<<NRows<<endl;

    // Loop over the events and all columns and generate MComptonEvent's
    for (long r = 1; r <= NRows; ++r) {

      if (fits_read_col_int(EventFile, Columns["RAWX"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
        cout<<"Column read (RAWX) failed!"<<endl;
        break;
      }
      int RawX = valint;

      if (fits_read_col_int(EventFile, Columns["RAWY"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
        cout<<"Column read (RAWY) failed!"<<endl;
        break;
      }
      int RawY = valint;

      if (fits_read_col_dbl(EventFile, Columns["TIME"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
        cout<<"Column read (START) failed!"<<endl;
        break;
      }
      double Start = valdbl;

      if (fits_read_col_dbl(EventFile, Columns["TIME_STOP"], r, 1, 1, nulldbl, &valdbl, &anynul, &status) ) {
        cout<<"Column read (STOP) failed!"<<endl;
        break;
      }
      int Stop = valdbl;
    
      Add(h-2, RawX, RawY, Start, Stop);
    }
  }
  
  //cout<<"Bad pixels:"<<endl;
  for (unsigned int  b = 0; b < m_DetectorID.size(); ++b) {
    //cout<<"D:"<<m_DetectorID[b]<<" x="<<m_RawX[b]<<" y="<<m_RawY[b]<<" Start="<<m_Start[b]<<" Stop="<<m_Stop[b]<<endl;
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBadPixel::CalculateUpTimes(NFilteredEvents& F)
{
  // For each pixel calculate the "up-time"
  
  double TotalUpTime = 0.0;
  for (unsigned int g = 0; g < F.m_GTIStart.size(); ++g) {
    TotalUpTime += F.m_GTIStop[g] - F.m_GTIStart[g];
    
    for (unsigned int b = 0; b < m_DetectorID.size(); ++b) {
      if (m_Start[b] <= F.m_GTIStart[g] && m_Stop[b] >= F.m_GTIStop[g]) {
        m_CutGTI[b] += F.m_GTIStop[g] - F.m_GTIStart[g];
        cout<<"Cut: "<<"D:"<<m_DetectorID[b]<<" x="<<m_RawX[b]<<" y="<<m_RawY[b]<<": "<<m_CutGTI[b]<<" by "<<F.m_GTIStop[g] - F.m_GTIStart[g]<<endl;
      } else if (m_Start[b] >= F.m_GTIStart[g] && m_Start[b] <= F.m_GTIStop[g] && m_Stop[b] >= F.m_GTIStop[g]) {
        m_CutGTI[b] += F.m_GTIStop[g] - m_Start[b];  
        cout<<"Cut: "<<"D:"<<m_DetectorID[b]<<" x="<<m_RawX[b]<<" y="<<m_RawY[b]<<": "<<m_CutGTI[b]<<" by "<<F.m_GTIStop[g] - F.m_GTIStart[g]<<endl;
      } else if (m_Start[b] <= F.m_GTIStart[g] && m_Stop[b] >= F.m_GTIStart[g] && m_Stop[b] <= F.m_GTIStop[g]) {
        m_CutGTI[b] += m_Stop[b] - F.m_GTIStart[g];  
        cout<<"Cut: "<<"D:"<<m_DetectorID[b]<<" x="<<m_RawX[b]<<" y="<<m_RawY[b]<<": "<<m_CutGTI[b]<<" by "<<F.m_GTIStop[g] - F.m_GTIStart[g]<<endl;
      } else if (m_Start[b] >= F.m_GTIStart[g] && m_Start[b] <= F.m_GTIStop[g] && m_Stop[b] >= F.m_GTIStart[g] && m_Stop[b] <= F.m_GTIStop[g]) {
        m_CutGTI[b] += m_Stop[b] - m_Start[b];  
        cout<<"Cut: "<<"D:"<<m_DetectorID[b]<<" x="<<m_RawX[b]<<" y="<<m_RawY[b]<<": "<<m_CutGTI[b]<<" by "<<F.m_GTIStop[g] - F.m_GTIStart[g]<<endl;
      }
    }
  }
  
  for (int d = 0; d < 4; ++d) {
    for (int x = 0; x < 32; ++x) {
      for (int y = 0; y < 32; ++y) {
        m_UpTimes[32*32*d + 32*y + x] = TotalUpTime;

        for (unsigned int b = 0; b < m_DetectorID.size(); ++b) {
          if (d == m_DetectorID[b] && x == m_RawX[d] && y == m_RawY[d]) {
            m_UpTimes[32*32*d + 32*y + x] -= m_CutGTI[d]; 
            if (m_UpTimes[32*32*d + 32*y + x] < 0) m_UpTimes[32*32*d + 32*y + x] = 0;
          }
        }
      }
    }
  }
  
  return true;
}

  
// NBadPixel.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
