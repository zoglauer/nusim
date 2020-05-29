/*
 * NPointSourceFitter.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NPointSourceFitter
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NPointSourceFitter.h"

// Standard libs:
#include <fstream>
#include <limits>
#include <iomanip>
#include <sstream>
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TF2.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TMarker.h"
#include "TFitResult.h"

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NPointSourceFitter)
#endif




////////////////////////////////////////////////////////////////////////////////


double Gauss2D(double* x, double* par)
{
  // 0: Scaler
  // 1: Mean1
  // 2: Sigma1
  // 3: Mean2
  // 4: Sigma3
  
  double arg1 = 0;
  double arg2 = 0;
  if (par[2] != 0) arg1 += (x[0] - par[1])/par[2];
  if (par[4] != 0) arg2 += (x[1] - par[3])/par[4];
  
  return par[0]*exp(-0.5*(arg1*arg1 + arg2*arg2));
}


////////////////////////////////////////////////////////////////////////////////


double TwoGauss2D(double* x, double* par)
{
  double Scaler = par[0];
  double Mean1X = par[1];
  double Mean1Y = par[2];
  double Mean2X = par[3];
  double Mean2Y = par[4];
  double Sigma = par[5];
  
  
  double arg1 = 0;
  double arg2 = 0;
  if (Sigma != 0) {
    arg1 += ((x[0] - Mean1X)*(x[0] - Mean1X) + (x[1] - Mean1Y)*(x[1] - Mean1Y))/Sigma/Sigma;
    arg2 += ((x[0] - Mean2X)*(x[0] - Mean2X) + (x[1] - Mean2Y)*(x[1] - Mean2Y))/Sigma/Sigma;
  }
  
  return Scaler*(exp(-0.5*(arg1 + arg1)) + exp(-0.5*(arg2 + arg2)));
}


////////////////////////////////////////////////////////////////////////////////


double TwoDoubleGauss2D(double* x, double* par)
{
  double Scaler = par[0];
  double Mean1X = par[1];
  double Mean1Y = par[2];
  double Mean2X = par[3];
  double Mean2Y = par[4];
  double SigmaA = par[5];
  double SigmaB = par[6];
  double RelativeScaler = par[7];
  
  double arg1A = 0;
  double arg2A = 0;
  if (SigmaA != 0) {
    arg1A += ((x[0] - Mean1X)*(x[0] - Mean1X) + (x[1] - Mean1Y)*(x[1] - Mean1Y))/SigmaA/SigmaA;
    arg2A += ((x[0] - Mean2X)*(x[0] - Mean2X) + (x[1] - Mean2Y)*(x[1] - Mean2Y))/SigmaA/SigmaA;
  }
  
  double arg1B = 0;
  double arg2B = 0;
  if (SigmaA != 0) {
    arg1B += ((x[0] - Mean1X)*(x[0] - Mean1X) + (x[1] - Mean1Y)*(x[1] - Mean1Y))/SigmaB/SigmaB;
    arg2B += ((x[0] - Mean2X)*(x[0] - Mean2X) + (x[1] - Mean2Y)*(x[1] - Mean2Y))/SigmaB/SigmaB;
  }
  
  return Scaler*(RelativeScaler*exp(-0.5*(arg1A + arg1A)) + RelativeScaler*exp(-0.5*(arg2A + arg2A)) + (1-RelativeScaler)*exp(-0.5*(arg1B + arg1B)) + (1-RelativeScaler)*exp(-0.5*(arg2B + arg2B)));
}


////////////////////////////////////////////////////////////////////////////////


double Gauss2DRot(double* x, double* par)
{
  // 0: Scaler
  // 1: Mean1
  // 2: Sigma1
  // 3: Mean2
  // 4: Sigma3
  // 5: Rotation
  
  
  double a = 0;
  if (par[2] != 0) a += cos(par[5])*cos(par[5])/(2*par[2]*par[2]);
  if (par[4] != 0) a += sin(par[5])*sin(par[5])/(2*par[4]*par[4]);
  
  double b = 0;
  if (par[2] != 0) b += sin(2*par[5])/(2*par[2]*par[2]);
  if (par[4] != 0) b -= sin(2*par[5])/(2*par[4]*par[4]);
  
  double c = 0;
  if (par[2] != 0) c += sin(par[5])*sin(par[5])/(2*par[2]*par[2]);
  if (par[4] != 0) c += cos(par[5])*cos(par[5])/(2*par[4]*par[4]);
  
  double Result = par[0]*exp( - a*(x[0] - par[1])*(x[0] - par[1])  -  b*(x[0] - par[1])*(x[1] - par[3])  -  c*(x[1] - par[3])*(x[1] - par[3]));
  
  //cout<<Result<<endl;
  
  return Result;
}


////////////////////////////////////////////////////////////////////////////////


double DoubleGaussians2DRotated(double* x, double* par)
{
  double MeanX = par[0];
  double MeanY = par[1];
  double Scaler1 = par[2];
  double Scaler2 = par[3];
  double SigmaX1 = par[4];
  double SigmaX2 = par[5];
  double SigmaY1 = par[6];
  double SigmaY2 = par[7];
  double Rotation = par[8];
  
  
  double a1 = 0;
  if (SigmaX1 != 0) a1 += cos(Rotation)*cos(Rotation)/(2*SigmaX1*SigmaX1);
  if (SigmaY1 != 0) a1 += sin(Rotation)*sin(Rotation)/(2*SigmaY1*SigmaY1);
  
  double b1 = 0;
  if (SigmaX1 != 0) b1 += sin(2*Rotation)/(2*SigmaX1*SigmaX1);
  if (SigmaY1 != 0) b1 -= sin(2*Rotation)/(2*SigmaY1*SigmaY1);
  
  double c1 = 0;
  if (SigmaX1 != 0) c1 += sin(Rotation)*sin(Rotation)/(2*SigmaX1*SigmaX1);
  if (SigmaY1 != 0) c1 += cos(Rotation)*cos(Rotation)/(2*SigmaY1*SigmaY1);
  
  double Result = Scaler1*exp( - a1*(x[0] - MeanX)*(x[0] - MeanX)  -  b1*(x[0] - MeanX)*(x[1] - MeanY)  -  c1*(x[1] - MeanY)*(x[1] - MeanY));
  
  
  double a2 = 0;
  if (SigmaX2 != 0) a2 += cos(Rotation)*cos(Rotation)/(2*SigmaX2*SigmaX2);
  if (SigmaY2 != 0) a2 += sin(Rotation)*sin(Rotation)/(2*SigmaY2*SigmaY2);
  
  double b2 = 0;
  if (SigmaX2 != 0) b2 += sin(2*Rotation)/(2*SigmaX2*SigmaX2);
  if (SigmaY2 != 0) b2 -= sin(2*Rotation)/(2*SigmaY2*SigmaY2);
  
  double c2 = 0;
  if (SigmaX2 != 0) c2 += sin(Rotation)*sin(Rotation)/(2*SigmaX2*SigmaX2);
  if (SigmaY2 != 0) c2 += cos(Rotation)*cos(Rotation)/(2*SigmaY2*SigmaY2);
  
  Result += Scaler2*exp( - a2*(x[0] - MeanX)*(x[0] - MeanX)  -  b2*(x[0] - MeanX)*(x[1] - MeanY)  -  c2*(x[1] - MeanY)*(x[1] - MeanY));
  
  //cout<<Result<<endl;
  
  return Result;
}


////////////////////////////////////////////////////////////////////////////////


NPointSourceFitter::NPointSourceFitter()
{
  // Construct an instance of NPointSourceFitter
  
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;
  
  m_OutputFile = "";
  
  gStyle->SetPalette(kViridis);
}


////////////////////////////////////////////////////////////////////////////////


NPointSourceFitter::~NPointSourceFitter()
{
  // Delete this instance of NPointSourceFitter
}


////////////////////////////////////////////////////////////////////////////////


bool NPointSourceFitter::ParseCommandLine(int argc, char** argv)
{
  if (NBaseTool::ParseCommandLine(argc, argv) == false) return false;
  
  string Option;
  
  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    
    // Then fulfill the options:
    if (Option == "--outputfile") {
      m_OutputFile = argv[++i];
      cout<<"Using output file "<<m_OutputFile<<endl;
    }
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPointSourceFitter::Analyze() 
{
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    TString Directory = m_Directories[d];
    
    if (Directory.EndsWith("/")) {
      Directory = Directory.Remove(Directory.Last('/'), 1); 
    }    
    
    // Check if we are in the main dir or the event directory
    TString EventDir = "event_cl_normal";
    if (gSystem->AccessPathName(Directory + "/hk") == true) { // == it is not accessible
      // Remove last directory
      EventDir = Directory;
      EventDir.Remove(0, EventDir.Last('/')+1);  
      Directory = Directory.Remove(Directory.Last('/'), Directory.Length() - Directory.Last('/'));  
    }
    
    TString Tag = Directory;
    if (Tag.Last('/') != kNPOS) {
      Tag = Tag.Remove(0, Tag.Last('/')+1);      
    }
    
    TString FIFileNameA01 = Directory + "/" + EventDir + "/nu" + Tag + "A01_cl.evt";
    TString FIFileNameB01 = Directory + "/" + EventDir + "/nu" + Tag + "B01_cl.evt";
    
    NFilteredEvents NormalA;
    if (NormalA.Read(FIFileNameA01) == false) return false;
    NormalA.m_Module = 0;
    NormalA.m_ID = Tag;
    
    NFilteredEvents NormalB;
    if (NormalB.Read(FIFileNameB01) == false) return false;
    NormalB.m_Module = 1;
    NormalB.m_ID = Tag;
    
    // Check if we are in the main dir or the event directory
    EventDir = "event_cl_fixed";
    if (gSystem->AccessPathName(Directory + "/hk") == true) { // == it is not accessible
      // Remove last directory
      EventDir = Directory;
      EventDir.Remove(0, EventDir.Last('/')+1);  
      Directory = Directory.Remove(Directory.Last('/'), Directory.Length() - Directory.Last('/'));  
    }
    
    Tag = Directory;
    if (Tag.Last('/') != kNPOS) {
      Tag = Tag.Remove(0, Tag.Last('/')+1);      
    }
    
    FIFileNameA01 = Directory + "/" + EventDir + "/nu" + Tag + "A01_cl.evt";
    FIFileNameB01 = Directory + "/" + EventDir + "/nu" + Tag + "B01_cl.evt";
    
    NFilteredEvents FixedA;
    if (FixedA.Read(FIFileNameA01) == false) return false;
    FixedA.m_Module = 0;
    FixedA.m_ID = Tag;
    
    NFilteredEvents FixedB;
    if (FixedB.Read(FIFileNameB01) == false) return false;
    FixedB.m_Module = 1;
    FixedB.m_ID = Tag;
    
    
    // Analysis
    
    double SigmaMaxNA = 0;
    double AssymetryNA = 0;
    double RAPeakNA = 0;
    double DECPeakNA = 0;
    TH2D* HistNA = Show("Normal A", NormalA, SigmaMaxNA, AssymetryNA, RAPeakNA, DECPeakNA);
    TVector3 PeakNA = Vectorize(RAPeakNA*c_Rad, DECPeakNA*c_Rad);
    ostringstream TitleNA;
    TitleNA<<"#splitline{Normal A:}{"<<"#sigma_{max} = "<<std::setprecision(3)<<SigmaMaxNA*60*60<<"\", #sigma_{max} / #sigma_{min} = "<<std::setprecision(4)<<AssymetryNA<<"}";
    HistNA->SetTitle(TitleNA.str().c_str());
    
    
    double SigmaMaxFA = 0;
    double AssymetryFA = 0;
    double RAPeakFA = 0;
    double DECPeakFA = 0;
    TH2D* HistFA = Show("Neural net A", FixedA, SigmaMaxFA, AssymetryFA, RAPeakFA, DECPeakFA);
    TVector3 PeakFA = Vectorize(RAPeakFA*c_Rad, DECPeakFA*c_Rad);
    double DistanceNAFA = PeakFA.Angle(PeakNA)*60*60;
    
    ostringstream TitleFA;
    TitleFA<<"#splitline{Neural net A:}{"<<"#splitline{#sigma_{max} = "<<std::setprecision(3)<<SigmaMaxFA*60*60<<"\", #sigma_{max} / #sigma_{min} = "<<std::setprecision(4)<<AssymetryFA<<"}{Peak shift: "<<DistanceNAFA<<"\"}}";
    HistFA->SetTitle(TitleFA.str().c_str());
    
    
    double SigmaMaxNB = 0;
    double AssymetryNB = 0;
    double RAPeakNB = 0;
    double DECPeakNB = 0;
    TH2D* HistNB = Show("Normal B", NormalB, SigmaMaxNB, AssymetryNB, RAPeakNB, DECPeakNB);
    TVector3 PeakNB = Vectorize(RAPeakNB*c_Rad, DECPeakNB*c_Rad);
    ostringstream TitleNB;
    TitleNB<<"#splitline{Normal B:}{"<<"#sigma_{max} = "<<std::setprecision(3)<<SigmaMaxNB*60*60<<"\", #sigma_{max} / #sigma_{min} = "<<std::setprecision(4)<<AssymetryNB<<"}";
    HistNB->SetTitle(TitleNB.str().c_str());
    
    
    double SigmaMaxFB = 0;
    double AssymetryFB = 0;
    double RAPeakFB = 0;
    double DECPeakFB = 0;
    TH2D* HistFB = Show("Neural net B", FixedB, SigmaMaxFB, AssymetryFB, RAPeakFB, DECPeakFB);
    TVector3 PeakFB = Vectorize(RAPeakFB*c_Rad, DECPeakFB*c_Rad);
    double DistanceNBFB = PeakFB.Angle(PeakNB)*60*60;
    ostringstream TitleFB;
    TitleFB<<"#splitline{Neural net B:}{"<<"#splitline{#sigma_{max} = "<<std::setprecision(3)<<SigmaMaxFB*60*60<<"\", #sigma_{max} / #sigma_{min} = "<<std::setprecision(4)<<AssymetryFB<<"}{Peak shift: "<<DistanceNBFB<<"\"}}";
    HistFB->SetTitle(TitleFB.str().c_str());
    
    
    TCanvas* Canvas = new TCanvas("Summary", "Summary", 2000, 2000);
    Canvas->SetMargin(0.0, 0.0, 0.0, 0.0);
    
    TPaveText* Title = new TPaveText(0.1,0.96,0.9,0.99);
    Title->AddText(TString("Observation: ") + NormalA.m_ID);
    Title->SetFillColor(0);
    Title->Draw("NB");
    
    TPad* GraphPad = new TPad("Graphs","Graphs",0.01,0.01,0.99,0.95);
    GraphPad->Draw();
    GraphPad->cd();
    GraphPad->Divide(2,2);
    //GraphPad->SetMargin(0.02, 0.02, 0.02, 0.52);
    
    GraphPad->cd(1);
    GraphPad->GetPad(1)->SetMargin(0.15, 0.15, 0.125, 0.175);
    HistNA->Draw("colz");
    TMarker* NAPeakMarker = new TMarker(RAPeakNA, DECPeakNA, 2);
    NAPeakMarker->Draw("SAME");
    
    GraphPad->cd(2);
    GraphPad->GetPad(2)->SetMargin(0.15, 0.15, 0.125, 0.175);
    HistFA->Draw("colz");
    TMarker* PeakFAMarker = new TMarker(RAPeakFA, DECPeakFA, 2);
    PeakFAMarker->Draw("SAME");

    GraphPad->cd(3);
    GraphPad->GetPad(3)->SetMargin(0.15, 0.15, 0.125, 0.175);
    HistNB->Draw("colz");
    TMarker* PeakNBMarker = new TMarker(RAPeakNB, DECPeakNB, 2);
    PeakNBMarker->Draw("SAME");

    GraphPad->cd(4);
    GraphPad->GetPad(4)->SetMargin(0.15, 0.15, 0.125, 0.175);
    HistFB->Draw("colz");
    TMarker* PeakFBMarker = new TMarker(RAPeakFB, DECPeakFB, 2);
    PeakFBMarker->Draw("SAME");

    Canvas->Update();
    
    if (m_ShowHistograms.Contains("f") == true) Canvas->SaveAs(TString("Obs_") + NormalA.m_ID + "_NNPSD.pdf");
    
    ofstream out;
    out.open(m_OutputFile, std::ofstream::out | std::ofstream::app);
    if (out.is_open() == true) {
      out<<"O "<<NormalA.m_ID<<" "<<NormalA.m_ObservationDate<<" A-Std "<<std::setprecision(5)<<SigmaMaxNA*60*60<<" "<<std::setprecision(4)<<AssymetryNA<<" A-NN "<<std::setprecision(5)<<SigmaMaxFA*60*60<<" "<<std::setprecision(4)<<AssymetryFA<<" "<<DistanceNAFA<<" B-Std "<<std::setprecision(5)<<SigmaMaxNB*60*60<<" "<<std::setprecision(4)<<AssymetryNB<<" B-NN "<<std::setprecision(5)<<SigmaMaxFB*60*60<<" "<<std::setprecision(4)<<AssymetryFB<<" "<<DistanceNBFB<<endl;
      out<<flush;
    } else {
      cout<<"Unable to open output file "<<m_OutputFile<<endl; 
    }
    
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


TH2D* NPointSourceFitter::Show(TString Title, NFilteredEvents& E, double& SigmaMax, double& Assymetry, double& RAFitPeak, double& DECFitPeak)
{
  cout<<"Quick view of the data..."<<endl;
  
  // Section A: Create all histograms:
  double RA, DEC;
  
  double MinRA = +numeric_limits<double>::max();
  double MaxRA = -numeric_limits<double>::max();
  double MinDEC = +numeric_limits<double>::max();
  double MaxDEC = -numeric_limits<double>::max();
  
  vector<double> RAs;
  vector<double> DECs;
  for (unsigned int d = 0; d < E.m_X.size(); ++d) {
    if (E.m_X[d] == -1 || E.m_Y[d] == -1) continue;
    //if (E.m_Energy[d] < m_EnergyMin || E.m_Energy[d] > m_EnergyMax) continue;
    //if (E.m_Time[d] < m_TimeMin || E.m_Time[d] > m_TimeMax) continue;
    
    
    
    //cout<<E.m_ReferencePixelX<<":"<<E.m_X[d]<<":"<<E.m_ReferencePixelValueX<<":"<<E.m_ReferencePixelDeltaX<<endl;
    RA = E.m_ReferencePixelValueX + (E.m_X[d] - E.m_ReferencePixelX)*E.m_ReferencePixelDeltaX;
    DEC = E.m_ReferencePixelValueY + (E.m_Y[d] - E.m_ReferencePixelY)*E.m_ReferencePixelDeltaY;
    //cout<<RA<<":"<<DEC<<endl;
    
    
    // Randomize in pixel
    RA += E.m_ReferencePixelDeltaX*(gRandom->Rndm()-0.5);
    DEC += E.m_ReferencePixelDeltaY*(gRandom->Rndm()-0.5);
    
    
    if (RA < MinRA) MinRA = RA;
    if (RA > MaxRA) MaxRA = RA;
    if (DEC < MinDEC) MinDEC = DEC;
    if (DEC > MaxDEC) MaxDEC = DEC;
    
    //cout<<MinRA<<":"<<MaxRA<<endl;
    
    RAs.push_back(RA);
    DECs.push_back(DEC);
    //Energies[ObsID][ModuleID].push_back(E.m_Energy[d]);
  }
  
  
  cout<<"(1) DEC: "<<MinDEC<<"-"<<MaxDEC<<"    RA: "<<MinRA<<"-"<<MaxRA<<endl;
  
  
  // Add two arcmin for response reasons
  double RAScaler = 1.0/cos(0.5*(MaxDEC+MinDEC)*c_Rad);
  double Safety = 0*arcmin;
  MinDEC -= Safety;
  MaxDEC += Safety;
  MinRA -= Safety*RAScaler;
  MaxRA += Safety*RAScaler;
  
  cout<<"(2) DEC: "<<MinDEC<<"-"<<MaxDEC<<"    RA: "<<MinRA<<"-"<<MaxRA<<endl;
  
  // Now create the image:
  
  /*
   *  // First make sure the dimensions are identical
   *  double DimDEC = MaxDEC - MinDEC;
   *  double DimRA = MaxRA - MinRA;
   *  RAScaler = 1.0/cos(0.5*(MaxDEC+MinDEC)*c_Rad);
   *  double DimDiff = DimDEC - DimRA/RAScaler;
   *  if (DimDiff > 0.0) {
   *    // Increase RA
   *    MaxRA += 0.5*DimDiff*RAScaler;
   *    MinRA -= 0.5*DimDiff*RAScaler;
   *    if (MinRA < 0.0) MinRA = 0.0;
   *    if (MaxRA > 360.0) MaxRA = 360.0;
} else {
  // Increase DEC
  MaxDEC += 0.5*fabs(DimDiff/RAScaler);
  MinDEC -= 0.5*fabs(DimDiff/RAScaler);
  if (MaxDEC > 90) MaxDEC = 90;
  if (MinDEC < -90) MinDEC = -90;
  RAScaler = 1.0/cos(0.5*(MaxDEC+MinDEC)*c_Rad);
}
*/
  
  cout<<"(3) DEC: "<<MinDEC<<"-"<<MaxDEC<<"    RA: "<<MinRA<<"-"<<MaxRA<<endl;
  
  /*
   * if (m_ZoomUse == true) {
   *  MinDEC = m_ZoomY - m_ZoomWidth;
   *  MaxDEC = m_ZoomY + m_ZoomWidth;
   *  RAScaler = 1.0/cos(0.5*(MaxDEC+MinDEC)*c_Rad);
   *  MinRA = m_ZoomX - m_ZoomWidth*RAScaler;
   *  MaxRA = m_ZoomX + m_ZoomWidth*RAScaler;  
}
*/
  cout<<"(4) DEC: "<<MinDEC<<"-"<<MaxDEC<<"    RA: "<<MinRA<<"-"<<MaxRA<<endl;
  
  
  //cout<<"After:"<<endl;
  //cout<<MinRA<<":"<<MaxRA<<endl;
  //cout<<MinDEC<<":"<<MaxDEC<<endl;
  
  double BinSizeDEC = 2.0/60/60;
  int MaxBins = 1000;
  
  RAScaler = 1.0/cos(0.5*(MaxDEC+MinDEC)*c_Rad);
  int BinsDEC = ceil((MaxDEC - MinDEC)/BinSizeDEC);
  int BinsRA = ceil((MaxRA - MinRA)/BinSizeDEC*RAScaler);
  
  
  if (MinRA >= MaxRA) {
    cout<<"RA-axis problem : Min (="<<MinRA<<") >= Max (="<<MaxRA<<")"<<endl;
  }
  if (MinRA >= MaxRA) {
    cout<<"DEC-axis problem : Min (="<<MinDEC<<") >= Max (="<<MaxDEC<<")"<<endl;
  }
  
  // Limit the number of bins, or we spend for ever updating the histogram...
  if (BinsDEC > MaxBins) {
    cout<<"Max. number of DEC bins reached: "<<MaxBins<<endl;
    BinsDEC = MaxBins;
  }
  if (BinsRA > MaxBins) {
    cout<<"Max. number of RA bins reached: "<<MaxBins<<endl;
    BinsRA = MaxBins;
  }
  
  cout<<"Image dimensions: RA="<<BinsRA<<" ("<<MinRA<<" - "<<MaxRA<<") DEC="<<BinsDEC<<" ("<<MinDEC<<" - "<<MaxDEC<<endl;
  
  // Set up data
  
  TH2D* Hist = new TH2D("Hist", "Hist", BinsRA, MinRA, MaxRA, BinsDEC, MinDEC, MaxDEC);
  Hist->GetXaxis()->CenterTitle();
  Hist->GetYaxis()->CenterTitle();
  Hist->GetYaxis()->SetTitleOffset(2.1);
  Hist->GetXaxis()->SetNdivisions(506);
  Hist->GetYaxis()->SetNdivisions(506);
  Hist->SetXTitle("RA [deg]");
  Hist->SetYTitle("DEC [deg]");
  //Hist->SetZTitle("counts");

  for (unsigned int r = 0; r < RAs.size(); ++r) {
    Hist->Fill(RAs[r], DECs[r], 1);
  }
  
  // Floor
  double PeakRatio = 0.7;
  TH2D* PeakHist = new TH2D(*Hist);
  double MaxValue = Hist->GetMaximum();
  for (int bx = 1; bx <= PeakHist->GetXaxis()->GetNbins(); ++bx) {
    for (int by = 1; by <= PeakHist->GetYaxis()->GetNbins(); ++by) {
      if (Hist->GetBinContent(bx, by) > PeakRatio * MaxValue) {
        PeakHist->SetBinContent(bx, by, Hist->GetBinContent(bx, by) - PeakRatio*MaxValue); 
      } else {
        PeakHist->SetBinContent(bx, by, 0);
      }
    }
  }
  
  int MaxBin = Hist->GetMaximumBin();
  int x,y,z;
  Hist->GetBinXYZ(MaxBin, x, y, z);
  
  double RAPeak = Hist->GetXaxis()->GetBinCenter(x);
  double DECPeak = Hist->GetYaxis()->GetBinCenter(y);
  
  cout<<"Peak RA: "<<RAPeak<<"  DEC: "<<DECPeak<<endl;
  
  double FitZoom = 0.075;
  
  // Fit a 2-d Gaussian
  TF1* FitS = nullptr;
  FitS = new TF2("Gauss2DRot", Gauss2DRot, RAPeak - FitZoom*(MaxRA - MinRA), RAPeak + FitZoom*(MaxRA - MinRA), DECPeak - FitZoom*(MaxDEC - MinDEC), DECPeak + FitZoom*(MaxDEC - MinDEC), 6);
  FitS->SetBit(kCanDelete);
  FitS->SetParNames("Scaler", "Mean X", "Sigma 1", "Mean Y", "Sigma 2", "Rotation");
  FitS->SetParameters(100, RAPeak, 0.003, DECPeak, 0.003, 0.0);
  FitS->SetParLimits(0, 0.1, 10000);
  //Fit->FixParameter(1, RAPeak);
  FitS->SetParLimits(1, RAPeak-0.01, RAPeak+0.01);
  FitS->SetParLimits(2, 0.0001, 1);
  //Fit->FixParameter(3, DECPeak);
  FitS->SetParLimits(3, DECPeak-0.01, DECPeak+0.01);
  FitS->SetParLimits(4, 0.0001, 1);
  //Fit->FixParameter(5, 0.0);
  FitS->SetParLimits(5, -2*c_Pi, 2*c_Pi);
  
  TFitResultPtr Result = Hist->Fit(FitS, "WRN E S");
  
  if (Result.Get()->Status() != 0) {
    FitS->SetParameters(100, RAPeak, 0.003, DECPeak, 0.003, 0.0);
    TFitResultPtr Result2 = Hist->Fit(FitS, "RN IM S");
  }
  
  SigmaMax = FitS->GetParameter(2) > FitS->GetParameter(4) ? FitS->GetParameter(2) : FitS->GetParameter(4);
  double SigmaMin = FitS->GetParameter(2) < FitS->GetParameter(4) ? FitS->GetParameter(2) : FitS->GetParameter(4);
  
  Assymetry = SigmaMax / SigmaMin;
  cout<<"Assymetry: "<<Assymetry<<endl;
  
  /*
   *  Fit = new TF2("TwoGauss2D", TwoGauss2D, 1.01*MinRA, 0.99*MaxRA, 1.01*MinDEC, 0.99*MaxDEC, 6);
   *  Fit->SetBit(kCanDelete);
   *  Fit->SetParNames("Scaler", "Mean X1", "Mean Y1", "Mean X2", "Mean Y2", "Sigma");
   *  Fit->SetParameters(100, RAPeak-0.005, DECPeak-0.005, RAPeak+0.005, DECPeak+0.005, 0.01);
   *  Fit->SetParLimits(0, 0.1, 10000);
   *  Fit->SetParLimits(1, RAPeak-0.01, RAPeak+0.01);
   *  Fit->SetParLimits(2, DECPeak-0.01, DECPeak+0.01);
   *  Fit->SetParLimits(3, RAPeak-0.01, RAPeak+0.01);
   *  Fit->SetParLimits(4, DECPeak-0.01, DECPeak+0.01);
   *  Fit->SetParLimits(5, 0.0001, 1);
   */
  
  
  /*
   *  Fit = new TF2("DoubleGaussians2DRotated", DoubleGaussians2DRotated, 1.01*MinRA, 0.99*MaxRA, 1.01*MinDEC, 0.99*MaxDEC, 9);
   *  Fit->SetBit(kCanDelete);
   *  Fit->SetParNames("Mean X", "Mean Y", "Scaler 1", "Scaler 2", "Sigma X1", "Sigma X2", "Sigma Y1", "Sigma Y2", "Rotation");
   *  Fit->SetParameters(100, RAPeak-0.005, DECPeak-0.005, RAPeak+0.005, DECPeak+0.005, 0.01);
   *  Fit->SetParLimits(0, RAPeak-0.01, RAPeak+0.01);
   *  Fit->SetParLimits(1, DECPeak-0.01, DECPeak+0.01);
   *  Fit->SetParLimits(2, 0.1, 10000);
   *  Fit->SetParLimits(3, 0.1, 10000);
   *  Fit->SetParLimits(4, 0.0001, 1);
   *  Fit->SetParLimits(5, 0.0001, 1);
   *  Fit->SetParLimits(6, 0.0001, 1);
   *  Fit->SetParLimits(7, 0.0001, 1);
   *  Fit->SetParLimits(8, -c_Pi, c_Pi);
   */
  
  /*
   *  TF2* Fit = nullptr;
   *  Fit = new TF2("TwoDoubleGauss2D", TwoDoubleGauss2D, RAPeak - FitZoom*(MaxRA - MinRA), RAPeak + FitZoom*(MaxRA - MinRA), DECPeak - FitZoom*(MaxDEC - MinDEC), DECPeak + FitZoom*(MaxDEC - MinDEC), 8);
   *  Fit->SetBit(kCanDelete);
   *  Fit->SetParNames("Scaler", "Mean X1", "Mean Y1", "Mean X2", "Mean Y2", "Sigma A", "Sigma B", "Component Scaler");
   *  Fit->SetParameters(100, RAPeak-0.005, DECPeak-0.005, RAPeak+0.005, DECPeak+0.005, 0.01, 0.01, 0.5);
   *  Fit->SetParLimits(0, 0.1, 10000);
   *  Fit->SetParLimits(1, RAPeak-0.01, RAPeak+0.01);
   *  Fit->SetParLimits(2, DECPeak-0.01, DECPeak+0.01);
   *  Fit->SetParLimits(3, RAPeak-0.01, RAPeak+0.01);
   *  Fit->SetParLimits(4, DECPeak-0.01, DECPeak+0.01);
   *  Fit->SetParLimits(5, 0.0001, 1);
   *  Fit->SetParLimits(6, 0.0001, 1);
   *  Fit->SetParLimits(7, 0.0001, 0.99999);
   *  
   * 
   *  Hist->Fit(Fit, "WRN LE");
   *  
   *  TVector3 Peak1 = Vectorize(Fit->GetParameter(1)*c_Rad, Fit->GetParameter(2)*c_Rad);
   *  TVector3 Peak2 = Vectorize(Fit->GetParameter(3)*c_Rad, Fit->GetParameter(4)*c_Rad);
   *  double Distance = Peak1.Angle(Peak2)*60*60;
   *  cout<<"Distance: "<<Distance<<" arcsec"<<endl;
   */
  
  //ostringstream out;
  //out<<Title<<": D-Metric = "<<std::setprecision(3)<<Distance<<"  A-Metric = "<<std::setprecision(3)<<Assymetry;
  //out<<Title<<": #sigma_{A} / #sigma_{B} = "<<std::setprecision(3)<<Assymetry<<", 2-Gauss dist. = "<<std::setprecision(3)<<Distance;
  //out<<Title<<": #sigma_{max} = "<<std::setprecision(3)<<SigmaMax*60*60<<"\"  #sigma_{max} / #sigma_{min} = "<<std::setprecision(4)<<Assymetry;
  
  //Hist->SetTitle(out.str().c_str());
  Hist->GetXaxis()->SetRangeUser(RAPeak - FitZoom*(MaxRA - MinRA), RAPeak + FitZoom*(MaxRA - MinRA));
  Hist->GetYaxis()->SetRangeUser(DECPeak - FitZoom*(MaxDEC - MinDEC), DECPeak + FitZoom*(MaxDEC - MinDEC));
  

  // Fit the peak:
  TF2* FitPeak = new TF2("Gauss2D", Gauss2D, 1.01*MinRA, 0.99*MaxRA, 1.01*MinDEC, 0.99*MaxDEC, 5);
  FitPeak->SetBit(kCanDelete);
  FitPeak->SetParNames("Scaler", "Mean X", "Sigma X", "Mean Y", "Sigma Y");
  FitPeak->SetParameters(100, RAPeak-0.005, 0.001, DECPeak+0.005, 0.01);
  FitPeak->SetParLimits(0, 0.1, 10000);
  FitPeak->SetParLimits(1, RAPeak-0.01, RAPeak+0.01);
  FitPeak->SetParLimits(2, 0.0001, 1);
  FitPeak->SetParLimits(3, DECPeak-0.01, DECPeak+0.01);
  FitPeak->SetParLimits(4, 0.0001, 1);
  
  
  PeakHist->Fit(FitPeak, "L");
  
  RAFitPeak = FitPeak->GetParameter(1);
  DECFitPeak = FitPeak->GetParameter(3);
  
  PeakHist->GetXaxis()->SetRangeUser(RAPeak - FitZoom*(MaxRA - MinRA), RAPeak + FitZoom*(MaxRA - MinRA));
  PeakHist->GetYaxis()->SetRangeUser(DECPeak - FitZoom*(MaxDEC - MinDEC), DECPeak + FitZoom*(MaxDEC - MinDEC));
  
  /*
  TCanvas* C = new TCanvas();
  C->cd();
  PeakHist->Draw("colz");
  FitPeak->Draw("SAME CONT1");
  C->Update();
  */
  
  cout<<"Peak: "<<FitPeak->GetParameter(1)<<", "<<FitPeak->GetParameter(3)<<endl;
  
  return Hist;
}


// NPointSourceFitter.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
