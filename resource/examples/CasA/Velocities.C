void Velocities() {
  double LineEnergy = 67.9; // keV
  double Velocity = 5000; // km/s
  double VelocityOneSigma = 500; // km/s
  double LightSpeed = 300000; // km/s

  double SphereRadiusMax = 1.0;
  double SphereRadiusMin = 0.0;

  
  double Sigmas = 3;
  double EMin = LineEnergy*(-Sigmas*Velocity/LightSpeed + 1);
  double EMax = LineEnergy*(Sigmas*Velocity/LightSpeed + 1);
  TH1D* EnergyHist = new TH1D("Energy", "Energy", 100, EMin, EMax);
  TH1D* ThetaHist = new TH1D("Theta", "Theta", 90, 0.0, 180.0);
  TH1D* VelocityHist = new TH1D("Velocity", "Velocity", 100, -5000, 5000);
  
  double x, y, z;
  double Theta, Phi;
  double ProjectedVelocity;
  TVector3 Dir;
  for (unsigned int i = 0; i < 1000000; ++i) {
    do { 
      x = (2*gRandom->Rndm()-1)*SphereRadiusMax;
      y = (2*gRandom->Rndm()-1)*SphereRadiusMax;
      z = (2*gRandom->Rndm()-1)*SphereRadiusMax;
    } while (sqrt(x*x+y*y+z*z) > SphereRadiusMax || sqrt(x*x+y*y+z*z) < SphereRadiusMin);
    Dir.SetXYZ(x, y, z);
    Theta = Dir.Angle(TVector3(0.0, 0.0, 1.0));
    
    ProjectedVelocity = Dir.Mag()*gRandom->Gaus(Velocity, VelocityOneSigma)*cos(Theta);
    
    VelocityHist->Fill(ProjectedVelocity);
    EnergyHist->Fill(LineEnergy*(ProjectedVelocity/LightSpeed+1));
    ThetaHist->Fill(Theta*TMath::RadToDeg());
  }
  
  TCanvas* EnergyCanvas = new TCanvas();
  EnergyCanvas->cd();
  EnergyHist->Draw();
  EnergyCanvas->Update();
  
  TCanvas* VelocityCanvas = new TCanvas();
  VelocityCanvas->cd();
  VelocityHist->SetMinimum(0);
  VelocityHist->Draw();
  VelocityCanvas->Update();
  
  TCanvas* ThetaCanvas = new TCanvas();
  ThetaCanvas->cd();
  ThetaHist->SetMinimum(0);
  ThetaHist->Draw();
  ThetaCanvas->Update();

  // Now save for NuSIM
  ostringstream Title;
  Title<<"SNLineProfile_Line_"<<LineEnergy<<"keV_Velocity_"<<Velocity<<"pm"<<VelocityOneSigma<<"kmps_Shell_"<<SphereRadiusMax<<"to"<<SphereRadiusMin<<".txt";
  
  ofstream out;
  out.open(Title.str().c_str());
  out<<"# Line energy: "<<LineEnergy<<"keV"<<endl;
  out<<"# Velocity: ("<<Velocity<<"+-"<<VelocityOneSigma<<") km/s"<<endl;
  out<<"# Shell: "<<SphereRadiusMax<<" - "<<SphereRadiusMin<<endl;
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
