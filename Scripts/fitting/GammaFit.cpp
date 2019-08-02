#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TString.h"
#include "TLine.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>


std::string infile_ = "../../Sorted_data/test_analysis_wcut.root";


void dcB_x_ProjY(bool is_beam) {
  TFile *infile = new TFile(infile_.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH2F *histogram = nullptr;
  std::string histogram_name;

  float label_size = 0.06;
  float margin_size = 0.13;

  canvas_name = Form("Test");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);

  if (is_beam) {
    histogram_name = Form("B_dcB_x");
  } else {
    histogram_name = Form("T_dcB_x");
  }
  histogram = (TH2F *)infile->Get(histogram_name.c_str());

  std::string angle[16] = {"22", "26", "29.1", "32.2", "35.2", "37.9", "40.4", "42.8", 
                           "45.0", "47.1", "49.0", "50.7", "52.4", "53.9", "55.3", "56.7"};

  for (int i = 1; i < 16; i++) {
    TH1D *Y_projection = histogram->ProjectionY(" ", i+1, i+1);
    Y_projection->SetTitle(Form("Y projection [%s-%s deg]", angle[i-1].c_str(), angle[i].c_str())); // Changing titles
    Y_projection->Draw();
    gPad->SetLogy();
    canvas->SaveAs(Form("../../Plots/plotting/%s-ProjY-%d.pdf", histogram_name.c_str(), i+1));
  }
  
  //histogram->SetAxisRange(315, 1500, "X");
  
  //histogram->Draw("colz");
  //gPad->SetLogz();
  //histogram->SetAxisRange(22, 57, "X");

  
  //histogram->SetStats(0);                       // Remove stats
  //histogram->SetLabelSize(label_size, "xyz");   // Label size for x-, y- and z-axis
  //histogram->SetTitleSize(label_size, "xy");    // Text size for x- and y-axis
  //histogram->GetYaxis()->SetTitleOffset(1.1);   // Move y-axis text a little closer
  //gStyle->SetTitleSize(label_size, "t");        // Title size
  //gPad->SetLeftMargin(margin_size);
  //gPad->SetRightMargin(margin_size);
  //gPad->SetBottomMargin(margin_size);

}
