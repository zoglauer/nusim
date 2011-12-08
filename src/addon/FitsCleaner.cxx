/* 
 * FitsCleaner.cxx
 *
 *
 * Copyright (C) 1998-2010 by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */

// Standard
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <csignal>
#include <map>
using namespace std;

// ROOT
#include <TApplication.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TMarker.h>
#include <TString.h>

// HEAsoft
#include "fitsio.h"

// NuSIM
#include "NExtractFitsImage.h"
#include "MFunction2D.h"
#include "MStreams.h"
#include "MVector.h"


/******************************************************************************/

class FitsCleaner
{
public:
  /// Default constructor
  FitsCleaner();
  /// Default destructor
  ~FitsCleaner();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Analyze whatever needs to be analyzed...
  bool Analyze();
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }


private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;

  /// Simulation file name
  TString m_FitsFileName;
  
  //! Minimum average counts
  double m_MinAverageCounts;
  
  //! RA of cutout pulsar in deg
  double m_CutRA;
  
  //! DEC of cutout pulsar in deg
  double m_CutDEC;
  
  //! Size of cutout pulsar in deg
  double m_CutSize;
  
};

/******************************************************************************/


/******************************************************************************
 * Default constructor
 */
FitsCleaner::FitsCleaner() : m_Interrupt(false)
{
  gStyle->SetPalette(1, 0);
  m_CutRA = 0.0;
  m_CutDEC = 0.0;
  m_CutSize = 0.0;
}


/******************************************************************************
 * Default destructor
 */
FitsCleaner::~FitsCleaner()
{
  // Intentionally left blanck
}


/******************************************************************************
 * Parse the command line
 */
bool FitsCleaner::ParseCommandLine(int argc, char** argv)
{
  ostringstream Usage;
  Usage<<endl;
  Usage<<"  Usage: FitsCleaner <options>"<<endl;
  Usage<<"    General options:"<<endl;
  Usage<<"         -f:   fits file name"<<endl;
  Usage<<"         -a:   minimum average counts"<<endl;
  Usage<<"         -c:   cut out pulsar (RA, DEC, Size in deg)"<<endl;
  Usage<<"         -h:   print this help"<<endl;
  Usage<<endl;

  string Option;

  // Check for help
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    if (Option == "-h" || Option == "--help" || Option == "?" || Option == "-?") {
      cout<<Usage.str()<<endl;
      return false;
    }
  }

  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];

    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "-f" || Option == "-o") {
      if (!((argc > i+1) && argv[i+1][0] != '-')){
        cout<<"Error: Option "<<argv[i][1]<<" needs a second argument!"<<endl;
        cout<<Usage.str()<<endl;
        return false;
      }
    } 
    // Multiple arguments_
    //else if (Option == "-??") {
    //  if (!((argc > i+2) && argv[i+1][0] != '-' && argv[i+2][0] != '-')){
    //    cout<<"Error: Option "<<argv[i][1]<<" needs two arguments!"<<endl;
    //    cout<<Usage.str()<<endl;
    //    return false;
    //  }
    //}

    // Then fulfill the options:
    if (Option == "-f") {
      m_FitsFileName = argv[++i];
      cout<<"Accepting file name: "<<m_FitsFileName<<endl;
    } else if (Option == "-a") {
      m_MinAverageCounts = atof(argv[++i]);
      cout<<"Accepting minimum average counts: "<<m_MinAverageCounts<<endl;
    } else if (Option == "-c") {
      m_CutRA = atof(argv[++i]);
      m_CutDEC = atof(argv[++i]);
      m_CutSize = atof(argv[++i]);
      cout<<"Accepting cut: RA="<<m_CutRA<<" deg, DEC="<<m_CutDEC<<" deg, Size="<<m_CutSize<<" deg"<<endl;
    } else {
      cout<<"Error: Unknown option \""<<Option<<"\"!"<<endl;
      cout<<Usage.str()<<endl;
      return false;
    }
  }

  if (m_FitsFileName == "") {
    cout<<"Error: Empty file name!"<<endl;
    return false;
  }
  if (MFile::Exists(m_FitsFileName) == false) {
    cout<<"Error: File does not exist: "<<m_FitsFileName<<endl;
    return false;
  }

  return true;
}


/******************************************************************************
 * Do whatever analysis is necessary
 */
bool FitsCleaner::Analyze()
{
  if (m_Interrupt == true) return false;
  
  // Plot
  MFunction2D F;
  NExtractFitsImage Extractor;
  Extractor.Extract(m_FitsFileName, F);
  
  F.Plot();
  
  // Convert to a nicer image
  vector<double> X = F.GetXAxis();
  vector<double> Y = F.GetYAxis();
  
  double XBinWidth = (X.back()-X[0])/(X.size()-1);
  double YBinWidth = (Y.back()-Y[0])/(Y.size()-1);
  
  TH2D* OriginalImage = new TH2D("OriginalImage", "Original image", 
                         X.size(), X[0]-XBinWidth/2, X.back()+XBinWidth/2, 
                         Y.size(), Y[0]-YBinWidth/2, Y.back()+YBinWidth/2);
  for (unsigned int x = 0; x < X.size(); ++x) {
    for (unsigned int y = 0; y < Y.size(); ++y) {
      OriginalImage->Fill(X[x], Y[y], F.Eval(X[x], Y[y]));
    }
  }

  
  TCanvas* OriginalImageCanvas = new TCanvas("OriginalImage", "OriginalImage");
  OriginalImageCanvas->cd();
  OriginalImage->DrawCopy("colz");
  OriginalImageCanvas->Update();  
  
  
  int Sourrounding = 3;
  TH2D* CleanedImage = new TH2D("CleanedImage", "Cleaned image", 
                                 X.size(), X[0]-XBinWidth/2, X.back()+XBinWidth/2, 
                                 Y.size(), Y[0]-YBinWidth/2, Y.back()+YBinWidth/2);
  for (int bx = 1; bx <= OriginalImage->GetXaxis()->GetNbins(); ++bx) {
    for (int by = 1; by <= OriginalImage->GetYaxis()->GetNbins(); ++by) {
      int MinXRange = bx-Sourrounding;
      if (MinXRange < 1) MinXRange = 1;
      int MaxXRange = bx+Sourrounding;
      if (MaxXRange > OriginalImage->GetXaxis()->GetNbins()) MaxXRange = OriginalImage->GetXaxis()->GetNbins();
      int MinYRange = by-Sourrounding;
      if (MinYRange < 1) MinYRange = 1;
      int MaxYRange = by+Sourrounding;
      if (MaxYRange > OriginalImage->GetYaxis()->GetNbins()) MaxYRange = OriginalImage->GetYaxis()->GetNbins();
      
      //cout<<"Ranges: "<<MinXRange<<"-"<<MaxXRange<<" "<<MinYRange<<"-"<<MaxYRange<<endl;
      int Entries = 0;
      double Sum = 0.0;
      for (int ix = MinXRange; ix <= MaxXRange; ++ix) {
        for (int iy = MinYRange; iy <= MaxYRange; ++iy) {
          ++Entries;
          Sum += OriginalImage->GetBinContent(ix, iy);
          //cout<<OriginalImage->GetBinContent(ix, iy)<<endl;
        }
      } 
      //cout<<"Sum: "<<Sum<<" Entries: "<<Entries<<endl;
      if (Entries > 0 && Sum/Entries > m_MinAverageCounts) {
        //cout<<"Filling: "<<bx<<":"<<by<<endl;
        CleanedImage->SetBinContent(bx, by, OriginalImage->GetBinContent(bx, by));
      }
    }
  }
  
  
  double TotalCounts = 0.0;
  double CutCounts = 0.0;
  
  MVector CutDirection;
  CutDirection.SetMagThetaPhi(1.0, c_Pi/2 - m_CutDEC*c_Rad, m_CutRA*c_Rad);
  for (int bx = 1; bx <= CleanedImage->GetXaxis()->GetNbins(); ++bx) {
    for (int by = 1; by <= CleanedImage->GetYaxis()->GetNbins(); ++by) {
      TotalCounts += CleanedImage->GetBinContent(bx, by);
      MVector PixelDir;
      PixelDir.SetMagThetaPhi(1.0, 
                              c_Pi/2 - CleanedImage->GetYaxis()->GetBinCenter(by)*c_Rad, 
                              CleanedImage->GetXaxis()->GetBinCenter(bx)*c_Rad);
      if (PixelDir.Angle(CutDirection) <= m_CutSize*c_Rad) {
        CutCounts += CleanedImage->GetBinContent(bx, by);
        CleanedImage->SetBinContent(bx, by, 0.0);
      }
    }
  }
   
  cout<<"Total counts: "<<TotalCounts<<":"<<CleanedImage->Integral()<<endl;
  cout<<"Cut counts:   "<<CutCounts<<endl;
   
  
  TCanvas* CleanedImageCanvas = new TCanvas("CleanedImage", "CleanedImage");
  CleanedImageCanvas->cd();
  CleanedImage->DrawCopy("colz");
  CleanedImageCanvas->Update();  

  
  
  
  // Write everything back to fits:
  TString NewFitsFileName = m_FitsFileName;
  NewFitsFileName.ReplaceAll(".fits", ".cleaned.fits");
  gSystem->CopyFile(m_FitsFileName, NewFitsFileName);
  
  int Status = 0;
  
  fitsfile* File = 0; 
  fits_open_file(&File, NewFitsFileName, READWRITE, &Status);
  if (Status != 0) {
    mgui<<"Unable to open file: "<<endl;
    mgui<<NewFitsFileName<<show;
    return false;
  }

  // Switch to the right extension:
  int HDUNumber = 1;
  fits_movabs_hdu(File, HDUNumber, NULL, &Status);
  if (Status != 0) {
    mgui<<"HDU number "<<HDUNumber<<" not found in fits file!"<<show;
    return false;
  }
  
  // Check if we have an image
  char Type[20];
  char Key[20];
  strcpy(Key, "XTENSION");
  fits_read_keyword(File, Key, Type, NULL, &Status);
  if ((Status == 0 && strstr(Type , "IMAGE") != NULL) || Status == 202) {
    Status = 0;
  } else {
    mgui<<"No image found in fits file!"<<show;
    return false;
  }

  // Retrieve the dimensions and data type of the image:
  int NDimensions;
  int DataType;
  long AxisDimension[9];  
  fits_get_img_param(File, 9, &DataType, &NDimensions, AxisDimension, &Status);
  if (Status != 0 || NDimensions == 0) {
    mgui<<"The image in the fits file has no dimensions..."<<show;
    return false;
  }
  
  
  // Create some dummy arrays
  //unsigned int tfSize[9];
  long StartPixel[9];
  long NPixel = 1;
  for (int dim = 0; dim < NDimensions; ++dim) {
    NPixel *= AxisDimension[dim];
    StartPixel[dim] = 1;
    //tfSize[dim] = AxisDimension[NDimensions - dim - 1];
  }

  
  // Write the data to the fits image
  if (DataType == LONG_IMG) {
    int* IntArray = new int[NPixel];
    for (unsigned int i = 0; i < NPixel; ++i) {
      IntArray[i] = 0;  
    }
    cout<<"NPixels: "<<NPixel<<endl;
    for (int bx = 1; bx <= CleanedImage->GetXaxis()->GetNbins(); ++bx) {
      for (int by = 1; by <= CleanedImage->GetYaxis()->GetNbins(); ++by) {
        //int Entry = (bx+StartPixel[0]) + (by+StartPixel[1])*(AxisDimension[0]-StartPixel[0]+1);
        //int Entry = (bx+StartPixel[0]) + (by+StartPixel[1])*AxisDimension[0];
        int Entry = (bx-1) + (by-1)*AxisDimension[0];
        if (Entry < 0 || Entry >= NPixel) {
          cout<<"Index out of bounds: "<<Entry<<" "<<bx<<":"<<by<<endl;
          continue;
        }
        //cout<<"x="<<1 + (CleanedImage->GetXaxis()->GetNbins()-bx)<<" - "<<by<<endl;
        if (IntArray[Entry] == 1) cout<<Entry<<" "<<bx<<" "<<by<<endl;
        IntArray[Entry] = CleanedImage->GetBinContent(1 + (CleanedImage->GetXaxis()->GetNbins()-bx), by);
      }
    }
   
    fits_write_pix(File, TINT, StartPixel, NPixel, IntArray, &Status);
    delete [] IntArray;
    if (Status != 0) {
      mgui<<"Failed to read data from fits file: "<<Status<<show;
      return false;
    }
    
  } else {
    mgui<<"The fits image contains an unhandled data type: "<<DataType<<" vs. "<<FLOAT_IMG<<show;
    return false;
  }
  
  cout<<"Finished writting new fits file!"<<endl;
  
  
  return true;
}


/******************************************************************************/

FitsCleaner* g_Prg = 0;

/******************************************************************************/


/******************************************************************************
 * Called when an interrupt signal is flagged
 * All catched signals lead to a well defined exit of the program
 */
void CatchSignal(int a)
{
  cout<<"Catched signal Ctrl-C (ID="<<a<<"):"<<endl;
  
  if (g_Prg != 0) {
    g_Prg->Interrupt();
  }
}


/******************************************************************************
 * Main program
 */
int main(int argc, char** argv)
{
  //void (*handler)(int);
  //handler = CatchSignal;
  //(void) signal(SIGINT, CatchSignal);


  TApplication FitsCleanerApp("FitsCleanerApp", 0, 0);

  g_Prg = new FitsCleaner();

  if (g_Prg->ParseCommandLine(argc, argv) == false) {
    cerr<<"Error during parsing of command line!"<<endl;
    return -1;
  } 
  if (g_Prg->Analyze() == false) {
    cerr<<"Error during analysis!"<<endl;
    return -2;
  } 

  FitsCleanerApp.Run();

  cout<<"Program exited normally!"<<endl;

  return 0;
}

/*
 * Cosima: the end...
 ******************************************************************************/
