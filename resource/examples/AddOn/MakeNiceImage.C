

void MakeNiceImage() {
 
  // Initilize some global ROOT variables:
  gEnv->SetValue("Gui.BackgroundColor", "#e3dfdf");
  //gEnv->SetValue("X11.UseXft", "true");

  // Set a common ROOT style for all programs:
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);

  gStyle->SetTitleAlign(13);
  gStyle->SetTitleX(0.15);
  gStyle->SetTitleW(0.7);
  gStyle->SetTitleY(0.95);
  gStyle->SetTitleFont(72, "");

  gStyle->SetTitleFont(42, "XYZ");
  gStyle->SetTitleOffset(1.2, "X");
  gStyle->SetTitleOffset(1.8, "Y");
  gStyle->SetTitleOffset(1.2, "Z");
  
  gStyle->SetLabelFont(42, "XYZ");
  gStyle->SetLabelSize(0.03, "XYZ");
  
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(0);

  double Margin = 0.15;
  gStyle->SetPadLeftMargin(Margin);
  gStyle->SetPadRightMargin(Margin);
  gStyle->SetPadTopMargin(Margin);
  gStyle->SetPadBottomMargin(Margin);

  
  gStyle->SetOptStat(0);

  //gStyle->SetPalette(1, 0);
  const unsigned int Number = 8;
  double Red[Number]   = { 0.0000, 0.2500, 0.5000, 1.0000, 1.0000, 1.0000, 1.0000, 1.0000 };
  double Green[Number] = { 0.0000, 0.0000, 0.0000, 0.0000, 0.5000, 1.0000, 1.0000, 1.0000 };
  double Blue[Number]  = { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.5000, 0.7500 };
  double Stops[Number] = { 0.0000, 0.0000, 0.1000, 0.3000, 0.5000,  0.700, 0.9000, 1.0000 };
    
  // Arithmetic-mean for nice black'n'white prints:
  double Min = (Red[0]+Green[0]+Blue[0])/3.0;
  double Max = (Red[Number-1]+Green[Number-1]+Blue[Number-1])/3.0;
  Stops[0] = 0.0;
  Stops[Number-1] = 1.0;
  for (unsigned int i = 1; i < Number-1; ++i) {
    double Value = ((Red[i]+Green[i]+Blue[i])/3.0-Min)/(Max-Min);
    Stops[i] = Value; 
  }
  TColor::CreateGradientColorTable(Number, Stops, Red, Green, Blue, 100);
  
  TCanvas* MainCanvas = new TCanvas("Image", "Image", 750, 750);
  MainCanvas->cd();
  //MainCanvas->SetLogz();
  
}
