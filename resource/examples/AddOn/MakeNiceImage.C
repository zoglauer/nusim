

void MakeNiceImage() {
 
  // Initilize some global ROOT variables:
  gEnv->SetValue("Gui.BackgroundColor", "#e3dfdf");
  //gEnv->SetValue("X11.UseXft", "true");

  // Set a common ROOT style for all programs:
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);

  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.8);
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

  gStyle->SetPalette(1, 0);
  
  TCanvas* MainCanvas = new TCanvas("Image", "Image", 750, 750);
  MainCanvas->cd();
  //MainCanvas->SetLogz();
  
}
