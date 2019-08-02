#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TF2.h"
#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLine.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>


std::string input_file = "../../Sorted_data/test_analysis_wcut_newEx.root";


void B_dcB_cid() {
  /*

  */
  TFile *infile = new TFile(input_file.c_str(), "UPDATE");

  TCanvas *canvas = nullptr;
  std::string canvas_name = Form("Beam gated prompt, Doppler corrected gamma-rays");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH2F *histogram = nullptr;
  std::string histogram_name;
  histogram_name = Form("B_dcB_cid");
  histogram = (TH2F *)infile->Get(histogram_name.c_str());
  histogram->Draw("colz");
  //histogram->SetAxisRange(22, 57, "X");
  //histogram->GetYaxis()->SetLabelSize(0.06);
  //histogram->GetXaxis()->SetLabelSize(0.06);
  gPad->SetLogz();
  //canvas->SaveAs(Form("../../Plots/plotting/E_vs_theta_all_Q.pdf")); 
}
