void Velocities() 
{
  double LineEnergy = 67.9; // keV
  double Velocity = 5000; // km/s
  double VelocityOneSigma = 1; // km/s
  double LightSpeed = 300000; // km/s

  double SphereRadiusMax = 1.1;
  double SphereRadiusMin = 0.9;

  int NEventsToSimulate = 1000000;
  
  double Sigmas = 3;
  double EMin = LineEnergy*(-Sigmas*Velocity/LightSpeed + 1);
  double EMax = LineEnergy*(Sigmas*Velocity/LightSpeed + 1);
  TH1D* EnergyHist = new TH1D("Energy", "Energy", 100, EMin, EMax);
  TH1D* ThetaHist = new TH1D("Theta", "Theta", 90, 0.0, 180.0);
  TH1D* VelocityHist = new TH1D("Velocity", "Velocity", 100, -5000, 5000);
  TH1D* ProjVelocityHist = new TH1D("ProjVelocity", "ProjVelocity", 100, -5000, 5000);
  
  double x, y, z;
  double Theta, Phi;
  double RandomizedVelocity;
  double ProjectedVelocity;
  TVector3 Dir;
  for (unsigned int i = 0; i < NEventsToSimulate; ++i) {
    do { 
      x = (2*gRandom->Rndm()-1)*SphereRadiusMax;
      y = (2*gRandom->Rndm()-1)*SphereRadiusMax;
      z = (2*gRandom->Rndm()-1)*SphereRadiusMax;
    } while (sqrt(x*x+y*y+z*z) > SphereRadiusMax || sqrt(x*x+y*y+z*z) < SphereRadiusMin);
    Dir.SetXYZ(x, y, z);
    Theta = Dir.Angle(TVector3(0.0, 0.0, 1.0));
    
    RandomizedVelocity = gRandom->Uniform(Velocity-VelocityOneSigma, Velocity+VelocityOneSigma); //gRandom->Gaus(Velocity, VelocityOneSigma);
    ProjectedVelocity = Dir.Mag()*RandomizedVelocity*cos(Theta);
    
    VelocityHist->Fill(RandomizedVelocity);
    ProjVelocityHist->Fill(ProjectedVelocity);
    EnergyHist->Fill(LineEnergy*(ProjectedVelocity/LightSpeed+1));
    ThetaHist->Fill(Theta*TMath::RadToDeg());
  }
  
  TCanvas* VelocityCanvas = new TCanvas();
  VelocityCanvas->cd();
  VelocityHist->SetMinimum(0);
  VelocityHist->Draw();
  VelocityCanvas->Update();
  
  TCanvas* ProjVelocityCanvas = new TCanvas();
  ProjVelocityCanvas->cd();
  ProjVelocityHist->SetMinimum(0);
  ProjVelocityHist->Draw();
  ProjVelocityCanvas->Update();
  
  TCanvas* ThetaCanvas = new TCanvas();
  ThetaCanvas->cd();
  ThetaHist->SetMinimum(0);
  ThetaHist->Draw();
  ThetaCanvas->Update();
  
  TCanvas* EnergyCanvas = new TCanvas();
  EnergyCanvas->cd();
  EnergyHist->Draw();
  EnergyCanvas->Update();

  // Now save for NuSIM
  ostringstream Title;
  Title<<"SNLineProfile_Line_"<<LineEnergy<<"keV_Velocity_"<<Velocity<<"pm"<<VelocityOneSigma<<"kmps_Shell_"<<SphereRadiusMax<<"to"<<SphereRadiusMin<<".spe";
  
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

  // Create a Gaussian approximation
  TH1D* GaussianHist = new TH1D("Gaussian", "Gaussian", 100, EMin, EMax);

  GaussianHist->Reset();
  
  double LineSigma = EnergyHist->GetRMS();
  for (unsigned int i = 0; i < NEventsToSimulate; ++i) {
    GaussianHist->Fill(gRandom->Gaus(LineEnergy, LineSigma));
  }
  
  TCanvas* GaussianCanvas = new TCanvas();
  GaussianCanvas->cd();
  GaussianHist->Draw();
  GaussianCanvas->Update();
  
  // Now save for NuSIM
  ostringstream TitleG;
  TitleG<<"SNGaussianLineProfile_Line_"<<LineEnergy<<"keV_Velocity_"<<Velocity<<"pm"<<VelocityOneSigma<<"kmps_Shell_"<<SphereRadiusMax<<"to"<<SphereRadiusMin<<".spe";
  
  ofstream outG;
  outG.open(TitleG.str().c_str());
  outG<<"# Line energy: "<<LineEnergy<<"keV"<<endl;
  outG<<"# Velocity: ("<<Velocity<<"+-"<<VelocityOneSigma<<") km/s"<<endl;
  outG<<"# Shell: "<<SphereRadiusMax<<" - "<<SphereRadiusMin<<endl;
  outG<<"# Gaussian approximation: "<<LineSigma<<endl;
  outG<<endl;
  outG<<"IP LIN"<<endl;
  outG<<endl;
  for (unsigned int b = 1; b <= EnergyHist->GetXaxis()->GetNbins(); ++b) { 
    outG<<"DP "<<EnergyHist->GetBinCenter(b)<<" "<<EnergyHist->GetBinContent(b)<<endl;
  }
  outG<<"EN"<<endl;
  
  outG.close();

  return;
}


















