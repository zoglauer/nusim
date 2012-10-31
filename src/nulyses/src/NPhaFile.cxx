/*
 * NPhaFile.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NPhaFile
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NPhaFile.h"

// Standard libs:
#include <iostream>
#include <map>
#include <fstream>
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TCanvas.h"

// MEGAlib libs:
#include "MFile.h"

// FITS libs:
#include "fitsio.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NPhaFile)
#endif


////////////////////////////////////////////////////////////////////////////////


NPhaFile::NPhaFile() : m_Spectrum(0)
{
  // Construct an instance of NPhaFile
}


////////////////////////////////////////////////////////////////////////////////


NPhaFile::~NPhaFile()
{
  // Delete this instance of NPhaFile
}


////////////////////////////////////////////////////////////////////////////////


double OffsetTripplePeakPlaw(double* x, double* par)
{
  //   L->SetParameters(1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
  //   L->SetParLimits(1, 0, 1E10);
  //   L->SetParLimits(2, 0, 1E10);
  //   L->SetParLimits(3, 0, 1E10);
  //   L->SetParLimits(4, 0, 1E10);
  //   L->SetParLimits(5, 0, 1E10);
  //   L->SetParLimits(6, 0, 1E10);
  //   L->SetParNames("Offset", "Sigma1", "Mean1", "Height1", 
  //                  "Sigma2", "Mean2", "Height2",
  //                  "Sigma3", "Mean3", "Height3");
  
  double val = par[0];
  double arg = 0;
  
  if (par[1] != 0) arg = (x[0]-par[2])/par[1];
  val += par[3]*TMath::Exp(-0.5*arg*arg);
  val += par[12]*TMath::Landau(-x[0] + par[5], 0, par[13]);
  
  arg = 0;
  if (par[4] != 0) arg = (x[0]-par[5])/par[4];
  val += par[6]*TMath::Exp(-0.5*arg*arg);
  val += par[14]*TMath::Landau(-x[0] + par[5], 0, par[15]);
  
  
  arg = 0;
  if (par[7] != 0) arg = (x[0]-par[8])/par[7];
  val += par[9]*TMath::Exp(-0.5*arg*arg);
  
  val += par[10]*TMath::Exp(-x[0]*par[11]);
  
  return val;
}


////////////////////////////////////////////////////////////////////////////////


bool NPhaFile::Load(TString FileName) 
{
  m_FileName = FileName;
  
  cout<<"Reading PHA from file "<<FileName<<endl;
 
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
  
  int nullint = 0;
  int valint = 0;
  
  
  fitsfile* File = 0;
  if (fits_open_file(&File, FileName, READONLY, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open PHA file \""<<FileName<<"\": "<<status<<":"<<value<<endl;
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
  }
  
  // Get the number of rows:
  long NRows = 0;
  fits_get_num_rows(File, &NRows, &status);
  
  // Loop over the events and all columns
  for (long r = 1; r <= NRows; ++r) {
    if (fits_read_col_int(File, Columns["COUNTS"], r, 1, 1, nullint, &valint, &anynul, &status) ) {
      cout<<"Column read (COUNTS) failed!"<<endl;
      break;
    }
    m_Counts.push_back(valint);
  }
  
  TString HistName = "Pha for ";
  HistName += FileName;
  // Conversion: 0.04*valdbl+1.6
  m_Spectrum = new TH1D(HistName, HistName, m_Counts.size(), 1.6-0.02, 0.04*m_Counts.size()+1.6+0.02);
  for (unsigned int b = 0; b < m_Counts.size(); ++b) {
    m_Spectrum->SetBinContent(b+1, m_Counts[b]);
  }
  m_Spectrum->SetXTitle("Energy [keV]");
  m_Spectrum->SetYTitle("cts");
  
  
  /*
  TF1* Fit = 0;
  Fit = new TF1("OffsetTripplePeakPlaw", OffsetTripplePeakPlaw, 20, 40, 16);
  Fit->SetBit(kCanDelete);
  Fit->SetParNames("Offset", 
                   "Paek1Width", "Peak1Mean", "Peak1Scale", 
                   "Paek2Width", "Peak2Mean", "Peak2Scale", 
                   "Paek3Width", "Peak3Mean", "Peak3Scale");
  Fit->SetParameters(0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
  Fit->SetParameter(11, 1);
  
  Fit->SetParLimits(0, 0, 10000);
  
  Fit->SetParLimits(1, 0.1, 0.75);
  Fit->SetParLimits(2, 28, 29);
  Fit->SetParLimits(3, 0, 10000);
  Fit->SetParLimits(12, 0, 10000);
  Fit->SetParLimits(13, 0.1, 3);
  
  Fit->SetParLimits(4, 0.1, 0.75);
  Fit->SetParLimits(5, 30.5, 31.5);
  Fit->SetParLimits(6, 0, 10000);
  Fit->SetParLimits(14, 0, 10000);
  Fit->SetParLimits(15, 0.1, 3);
  
  
  Fit->SetParLimits(7, 0.25, 1.5);
  Fit->SetParLimits(8, 24, 26);
  Fit->SetParLimits(9, 0, 10000);
  
  Fit->SetParLimits(10, 0, 10000);
  Fit->SetParLimits(11, 0.05, 0.5);
  
  TCanvas* Canvas = new TCanvas(HistName, HistName);
  Canvas->cd();
  if (Fit != 0) m_Spectrum->Fit(Fit, "RI");
                                m_Spectrum->Draw("HIST");
  if (Fit != 0) Fit->Draw("SAME");
                          Canvas->Modified();
  Canvas->Update();
  */
  
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPhaFile::SaveBackground(TH1D* ScaledSpectrum, double Scaler, TString Mode) 
{
  /*
  // Copy template
  TString BkgFileName = m_FileName;
  BkgFileName.ReplaceAll("_sr.pha", "");
  BkgFileName += "_bk_";
  BkgFileName += Mode;
  BkgFileName += ".pha";
  
  ifstream src(m_FileName);
  ofstream dst(BkgFileName);
  dst << src.rdbuf();
  src.close();
  dst.close();
  
  TString SourceFileName = m_FileName;
  SourceFileName.ReplaceAll("_sr.pha", "");
  SourceFileName += "_sr_";
  SourceFileName += Mode;
  SourceFileName += ".pha";
  
  ifstream src2(m_FileName);
  ofstream dst2(SourceFileName);
  dst2 << src2.rdbuf();
  src2.close();
  dst2.close();
  
  // Section 1:
  // initialize the files
  int status = 0;
  char value[1000];
  char templt[100];
  
  fitsfile* sFile = 0;
  if (fits_open_file(&sFile, SourceFileName, READWRITE, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open source file \""<<SourceFileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  // Move to the second hdu
  fits_movabs_hdu(sFile, 2, NULL, &status);
  TString ShortBkgFileName = BkgFileName;
  if (ShortBkgFileName[ShortBkgFileName.Length() - 1] == '/') {
    ShortBkgFileName = ShortBkgFileName.Remove(ShortBkgFileName.Last('/'), ShortBkgFileName.Length()); 
  }
  ShortBkgFileName = ShortBkgFileName.Remove(0, ShortBkgFileName.Last('/')+1);
  fits_update_key(sFile, TSTRING, "BACKFILE", const_cast<char*>(ShortBkgFileName.Data()), const_cast<char*>("Updated background file"), &status);
  //fits_update_key(sFile, TDOUBLE, "BACKSCAL", &V, const_cast<char*>("Updated background scaler"), &status);    
  fits_close_file(sFile, &status);
  
  
  fitsfile* File = 0;
  if (fits_open_file(&File, BkgFileName, READWRITE, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open background file \""<<BkgFileName<<"\": "<<status<<":"<<value<<endl;
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
  
  for (int s = 1; s <= ScaledSpectrum->GetNbinsX(); ++s) {
    int Value = ScaledSpectrum->GetBinContent(s);
    fits_write_col_int(File, Columns["COUNTS"], s, 1, 1, &Value, &status);
    if (status != 0) {
      fits_get_errstatus(status, value);
      cerr << "Error: Writing COUNTS failed (" << value << ")" << endl;
      fits_close_file(File, &status);
      File = 0;
      return false;
    }
  }
  
  //Scaler = 1.0/Scaler;
  fits_update_key(File, TDOUBLE, "BACKSCAL", &Scaler, const_cast<char*>("Updated background scaler"), &status);    
  
  
  fits_close_file(File, &status);
  
  return true;
  */
      // Copy template
    TString BkgFileName = m_FileName;
    BkgFileName.ReplaceAll("_sr.pha", "");
    BkgFileName += "_bk_";
    BkgFileName += Mode;
    BkgFileName += ".pha";
    
    ifstream src(m_FileName);
    ofstream dst(BkgFileName);
    dst << src.rdbuf();
    src.close();
    dst.close();
  
    TString SourceFileName = m_FileName;
    SourceFileName.ReplaceAll("_sr.pha", "");
    SourceFileName += "_sr_";
    SourceFileName += Mode;
    SourceFileName += ".pha";
  
    ifstream src2(m_FileName);
    ofstream dst2(SourceFileName);
    dst2 << src2.rdbuf();
    src2.close();
    dst2.close();
  
    // Section 1:
    // initialize the files
    int status = 0;
    char value[1000];
    char templt[100];

    fitsfile* sFile = 0;
    if (fits_open_file(&sFile, SourceFileName, READWRITE, &status)) {
      fits_get_errstatus(status, value);
      cout<<"Unable to open source file \""<<SourceFileName<<"\": "<<status<<":"<<value<<endl;
      return false;
    }
    // Move to the second hdu
    fits_movabs_hdu(sFile, 2, NULL, &status);
    double V = 1.0;
    TString ShortBkgFileName = BkgFileName;
    if (ShortBkgFileName[ShortBkgFileName.Length() - 1] == '/') {
      ShortBkgFileName = ShortBkgFileName.Remove(ShortBkgFileName.Last('/'), ShortBkgFileName.Length()); 
    }
    ShortBkgFileName = ShortBkgFileName.Remove(0, ShortBkgFileName.Last('/')+1);
    fits_update_key(sFile, TSTRING, "BACKFILE", const_cast<char*>(ShortBkgFileName.Data()), const_cast<char*>("Updated background file"), &status);
    fits_update_key(sFile, TDOUBLE, "BACKSCAL", &V, const_cast<char*>("Updated background scaler"), &status);    
    fits_close_file(sFile, &status);

    
    fitsfile* File = 0;
    if (fits_open_file(&File, BkgFileName, READWRITE, &status)) {
      fits_get_errstatus(status, value);
      cout<<"Unable to open background file \""<<BkgFileName<<"\": "<<status<<":"<<value<<endl;
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

    for (int s = 1; s <= ScaledSpectrum->GetNbinsX(); ++s) {
      int Value = ScaledSpectrum->GetBinContent(s);
      fits_write_col_int(File, Columns["COUNTS"], s, 1, 1, &Value, &status);
      if (status != 0) {
        fits_get_errstatus(status, value);
        cerr << "Error: Writing COUNTS failed (" << value << ")" << endl;
        fits_close_file(File, &status);
        File = 0;
        return false;
      }
    }
    
    Scaler = 1.0/Scaler;
    fits_update_key(File, TDOUBLE, "BACKSCAL", &Scaler, const_cast<char*>("Updated background scaler"), &status);    

  
    fits_close_file(File, &status);
    
    return true;

}


////////////////////////////////////////////////////////////////////////////////


bool NPhaFile::SaveBackground(TH1D* ScaledSpectrum, double Scaler, TH1D* SpectrumSource, TString Mode) 
{
  // Copy template
  TString BkgFileName = m_FileName;
  BkgFileName.ReplaceAll("_sr.pha", "");
  BkgFileName += "_bk_";
  BkgFileName += Mode;
  BkgFileName += ".pha";
  
  ifstream src(m_FileName);
  ofstream dst(BkgFileName);
  dst << src.rdbuf();
  src.close();
  dst.close();
  
  TString SourceFileName = m_FileName;
  SourceFileName.ReplaceAll("_sr.pha", "");
  SourceFileName += "_sr_";
  SourceFileName += Mode;
  SourceFileName += ".pha";
  
  ifstream src2(m_FileName);
  ofstream dst2(SourceFileName);
  dst2 << src2.rdbuf();
  src2.close();
  dst2.close();
  
  // Section 1:
  // initialize the files
  int status = 0;
  char value[1000];
  char templt[100];
  
  fitsfile* sFile = 0;
  if (fits_open_file(&sFile, SourceFileName, READWRITE, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open source file \""<<SourceFileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  // Move to the second hdu
  fits_movabs_hdu(sFile, 2, NULL, &status);
  double V = 1.0;
  TString ShortBkgFileName = BkgFileName;
  if (ShortBkgFileName[ShortBkgFileName.Length() - 1] == '/') {
    ShortBkgFileName = ShortBkgFileName.Remove(ShortBkgFileName.Last('/'), ShortBkgFileName.Length()); 
  }
  ShortBkgFileName = ShortBkgFileName.Remove(0, ShortBkgFileName.Last('/')+1);
  fits_update_key(sFile, TSTRING, "BACKFILE", const_cast<char*>(ShortBkgFileName.Data()), const_cast<char*>("Updated background file"), &status);
  //fits_update_key(sFile, TDOUBLE, "BACKSCAL", &V, const_cast<char*>("Updated background scaler"), &status);    
  
  // Get the number of columns
  int sNColumns = 0;
  map<TString, int> sColumns;
  fits_get_num_cols(sFile, &sNColumns, &status);
  for (int c = 1; c <= sNColumns; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(sFile, CASEINSEN, templt, value, &col, &status);
    sColumns[value] = c;
    //cout<<value<<endl;
  }

  for (int s = 1; s <= SpectrumSource->GetNbinsX(); ++s) {
    int Value = SpectrumSource->GetBinContent(s);
    fits_write_col_int(sFile, sColumns["COUNTS"], s, 1, 1, &Value, &status);
    if (status != 0) {
      fits_get_errstatus(status, value);
      cerr << "Error: Writing COUNTS failed (" << value << ")" << endl;
      fits_close_file(sFile, &status);
      sFile = 0;
      return false;
    }
  }

  fits_close_file(sFile, &status);
  
  
  fitsfile* File = 0;
  if (fits_open_file(&File, BkgFileName, READWRITE, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open background file \""<<BkgFileName<<"\": "<<status<<":"<<value<<endl;
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
  
  for (int s = 1; s <= ScaledSpectrum->GetNbinsX(); ++s) {
    int Value = ScaledSpectrum->GetBinContent(s)*Scaler;
    fits_write_col_int(File, Columns["COUNTS"], s, 1, 1, &Value, &status);
    if (status != 0) {
      fits_get_errstatus(status, value);
      cerr << "Error: Writing COUNTS failed (" << value << ")" << endl;
      fits_close_file(File, &status);
      File = 0;
      return false;
    }
  }
  
  //Scaler = 1.0/Scaler;
  //fits_update_key(File, TDOUBLE, "BACKSCAL", &Scaler, const_cast<char*>("Updated background scaler"), &status);    
  
  
  fits_close_file(File, &status);
  
  return true;
}


// NPhaFile.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
