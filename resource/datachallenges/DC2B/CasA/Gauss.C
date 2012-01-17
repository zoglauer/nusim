void Gauss() {
  double LineEnergy = 67.9; // keV
  double LineWidth = 0.85; // keV
  TH1D* EnergyHist = new TH1D("Energy", "Energy", 200, LineEnergy-7*LineWidth, LineEnergy+7*LineWidth);
  
  double x, y, z;
  double Theta, Phi;
  double ProjectedVelocity;
  TVector3 Dir;
  for (unsigned int i = 0; i < 4000000; ++i) {
    EnergyHist->Fill(gRandom->Gaus(LineEnergy, LineWidth));
  }
  
  TCanvas* EnergyCanvas = new TCanvas();
  EnergyCanvas->cd();
  EnergyHist->Draw();
  EnergyCanvas->Update();

  // Now save for NuSIM
  ostringstream Title;
  Title<<"SNLineProfile_Line_"<<LineEnergy<<"pm"<<LineWidth<<"keV.spe";
  
  ofstream out;
  out.open(Title.str().c_str());
  out<<"# Line energy: "<<LineEnergy<<"keV"<<endl;
  out<<"# Line width (1 sigma): "<<LineWidth<<"keV"<<endl;
  out<<endl;
  out<<"IP LIN"<<endl;
  out<<endl;
  for (unsigned int b = 1; b <= EnergyHist->GetXaxis()->GetNbins(); ++b) { 
    out<<"DP "<<EnergyHist->GetBinCenter(b)<<" "<<EnergyHist->GetBinContent(b)<<endl;
  }
  out<<"EN"<<endl;
  
  out.close();

  return;
}
