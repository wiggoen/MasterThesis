#include "TCanvas.h"
#include "TGraph.h"
#include "TFile.h"
#include "TLine.h"
#include "TH1.h"
#include "TF1.h"

#include <iostream>
#include <fstream>
#include <string>


TCanvas *canvas[16];
//TFile *AQ4_infile = new TFile("../Raw_data/Sm/140Sm_208Pb_pos6_025_OnBeam.root", "UPDATE");
TFile *AQ4_infile = new TFile("../../Sorted_data/Sm_user-AQ4Sort-2019-06-24.root", "UPDATE");


void test() {
  std::string canvas_plot = Form("Quadrant %d front strip f%d vs back strips 1-12", 1, 1);
  TCanvas *c1 = new TCanvas(canvas_plot.c_str(), canvas_plot.c_str(), 1280, 800);
  //canvas[0]->Divide(2, 2);
  //canvas[0]->cd(1);
  
  std::string histogram_fE = Form("fE_Q%d_f%d", 1, 1);
  //std::string histogram_bE = Form("bE_Q%d_f%d_b%d_cal", 1, 1, 1);
  
  cout << histogram_fE << endl;

  TH1F *histogram_front;
  histogram_front = (TH1F *) AQ4_infile->Get(histogram_fE.c_str());
  
  //TH1F *histogram_back;
  //histogram_back = (TH1F *) AQ4_infile->Get(histogram_bE.c_str());
  
  histogram_front->Draw();
  //histogram_back->Draw("SAME");
  cout << "test?!" << endl;
}


void particle_comparison() {
  // General setup
  int max_back_strip = 12;  // radial strips
  int max_front_strip = 16; // rings / annular strips
  int rows = 4;
  int columns = 4;
  float centroid;
  TH1F *histogram_back = nullptr;
  TH1F *histogram_front = nullptr;
  std::string front_strip;
  std::string histogram_name;
  std::string canvas_plot;
  
  // Colors: kRed, kBlue, kGreen, kPink, kAzure, kSpring, kMagenta, kCyan, kYellow, kViolet, kTeal, kOrange
  int colors[] = { 632, 600, 416, 900, 860, 820, 616, 432, 400, 880, 840, 800 };

  for (int quadrant = 0; quadrant < 4; quadrant++) {
    canvas_plot = Form("Quadrant %d front strip f1-16 vs back strips b1-12", quadrant+1);
    canvas[quadrant] = new TCanvas(canvas_plot.c_str(), canvas_plot.c_str(), 1280, 800);
    canvas[quadrant]->Divide(columns, rows);
    
    for (int front_strip = 0; front_strip < max_front_strip; front_strip++) {
      canvas[quadrant]->cd(front_strip + 1);

      // Front detector
      histogram_name = Form("fE_Q%d_f%d_cal", quadrant+1, front_strip+1);

      histogram_front = (TH1F *) AQ4_infile->Get(histogram_name.c_str());
      histogram_front->SetLineColor(kRed);
      histogram_front->SetLineStyle(kDashed);

      for (int back_strip = 0; back_strip < max_back_strip; back_strip++) {
        // Back detector
        histogram_name = Form("bE_Q%d_f%d_b%d_cal", quadrant+1, front_strip+1, back_strip+1);
    
        // Back detectors: Gated on selected front detector ring
        histogram_back = (TH1F *) AQ4_infile->Get(histogram_name.c_str());
        histogram_back->SetAxisRange(5E3, 650E3, "X");
        histogram_back->SetAxisRange(0, 4E3, "Y");
        histogram_back->SetLineColor(colors[back_strip]);
        histogram_back->Draw("SAME");
      }
      histogram_front->Draw("SAME");
    }
    canvas[quadrant]->SaveAs(Form("../../Plots/comparing/compare_Q%d.pdf", quadrant+1));
  }
}


void gamma_comparison() {
  TFile *infile = new TFile("../../Raw_data/Gamma/gamma_080919.root", "UPDATE");
  // General setup
  int max_cluster = 8;
  int max_crystals = 3;
  int max_segments = 6;
  
  std::string canvas_plot = "Gamma comparison";
  TCanvas *one_canvas = new TCanvas(canvas_plot.c_str(), canvas_plot.c_str(), 1280, 800);
  TH1F *histogram;
  std::string histogram_name;

  float label_size = 0.05;
  float margin_size = 0.13;

  // Colors: kRed, kBlue, kGreen, kPink, kAzure, kSpring, kMagenta, kCyan, kViolet, kTeal, kOrange
  int colors[] = { 632, 600, 416, 900, 860, 820, 616, 432, 880, 840, 800 };

  int count = 0;
  for (int cluster = 0; cluster < max_cluster; cluster++) {
    for (int crystal = 0; crystal < max_crystals; crystal++) {
      for (int segment = 0; segment < max_segments; segment++) {
        // Calibrated plots
        histogram_name = Form("E_gam_seg/E_gam_%d_%d_%d_cal", cluster, crystal, segment);
        histogram = (TH1F *) infile->Get(histogram_name.c_str());
        if (cluster <= max_cluster && crystal == 0 && segment == 0) {
          count += 1;
        }
        histogram->SetLineColor(colors[count]);
        histogram->Draw("SAME");

        //std::cout << histogram_name << std::endl;

        histogram->SetTitle("");                        // Removing title
        histogram->SetStats(0);                         // Remove stats
        histogram->SetLabelSize(label_size, "xy");      // Label size for x- and y-axis
        histogram->SetTitleSize(label_size, "xy");      // Text size for x- and y-axis
        histogram->GetYaxis()->SetTitle("Counts");      // Change y-axis title
        histogram->GetYaxis()->SetTitleOffset(1.0);     // Move y-axis text a little closer
        histogram->SetAxisRange(950, 1500, "X");
        //histogram->SetAxisRange(0, 1000, "X");
        //histogram->SetAxisRange(0, 1000, "Y");
        //gStyle->SetTitleSize(label_size, "t");          // Title size
        gPad->SetLeftMargin(margin_size);
        gPad->SetRightMargin(margin_size);
        gPad->SetBottomMargin(margin_size);
      }
    }
  }
  //std::cout << "count = " << count << std::endl;
  one_canvas->SaveAs(Form("../../Plots/comparing/gamma_comparison_full.png"));
}