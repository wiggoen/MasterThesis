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
#include "TPaveText.h"
#include "TArrow.h"
#include "TLatex.h"
#include "TLegend.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>


class Path {
  public:
    std::string default_infile;
    std::string singles_infile;
    std::string addback_infile;
    std::string reject_infile;

    Path(void) { std::cout << "Constructor called for Path." << std::endl; }
    ~Path(void) { std::cout << "Destructor called for Path." << std::endl; }

    void read_setup_file(std::string infile) {
      std::ifstream input_file(infile);  // Open file containing the setup
      std::string line;
      std::vector<std::string> var_name_vec;
      std::vector<std::string> var_value_vec;

      if (input_file.is_open()) {
        std::cout << "Input file opened. Reading..." << std::endl;

        while (std::getline(input_file, line)) {
          std::stringstream line_stream(line);  // Used for breaking words
          std::string name;
          std::string parameter;
          if (line_stream >> name >> parameter) {
            var_name_vec.push_back(name);
            var_value_vec.push_back(parameter);
          } 
        }
        this->default_infile = var_value_vec.at(0);
        this->singles_infile = var_value_vec.at(1);
        this->addback_infile = var_value_vec.at(2);
        this->reject_infile = var_value_vec.at(3);


        std::cout << "Finished reading from input file. Closing file." << std::endl;
        input_file.close();
      } else {
        std::cerr << "Unable to open '" << infile << "'. Program terminated." 
                  << std::endl;
        exit(EXIT_FAILURE);
      }
    }
};


std::string get_input_file(std::string setup_file, std::string sorting) {
  /*
      A function used to provide the correct input file to the plotting 
      functions.
  */
  
  Path *path = new Path();
  path->read_setup_file(setup_file);

  std::string sort_name;
  std::string infile;

  if (sorting == "d") {
    sort_name = "default";
    infile = path->default_infile;
  } else if (sorting == "s") {
    sort_name = "singles";
    infile = path->singles_infile;
  } else if (sorting == "a") {
    sort_name = "add back";
    infile = path->addback_infile;
  } else if (sorting == "r") {
    sort_name = "reject";
    infile = path->reject_infile;
  } else {
    std::cout << "Invalid file option. Choose either (d)efault, (s)ingles, (a)dd back or (r)eject." << std::endl;
  }

  std::cout << "Looking at " << sort_name 
            << " from file: " << infile << std::endl;

  return infile;
}


void energy_vs_angles(std::string setup_file, std::string sorting) {
  /*
      Plotting data sorted by CLXAna.
      Choose which sorting type to use: 
      "d" for default, "s" for singles, "a" for add back, "r" for reject.
  */
  std::string input_file = get_input_file(setup_file, sorting);

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  canvas_name = Form("Energy vs. angles");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH1F *histogram = nullptr;
  std::string histogram_name = Form("part");
  float label_size = 0.05;
  float margin_size = 0.13;

  //std::cout << "Histogram: " << histogram_name << std::endl;
    
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw("colz");
  histogram->SetTitle("");                      // Removing title
  histogram->SetStats(0);                       // Remove stats
  histogram->SetLabelSize(label_size, "xyz");   // Label size for x-, y- and z-axis
  histogram->SetTitleSize(label_size, "xy");    // Text size for x- and y-axis
  histogram->GetYaxis()->SetTitleOffset(1.1);   // Move y-axis text a little closer
  histogram->SetAxisRange(22.2, 56.6, "X");
  //gStyle->SetTitleSize(label_size, "t");        // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetRightMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogz();                              // Log-scale on z-axis

  canvas->SaveAs(Form("../../Plots/plotting/particle-events-%s.png", sorting.c_str()));
}


void particle_events_with_cut() {
  /*
      Plots detected particle events with cut. 
      Input file made with 'hadd'.
  */
  std::string input_file = "../../Sorted_data/part_wcut.root";

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  canvas_name = Form("Energy vs. angles with cut");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH1F *histogram = nullptr;
  TH1F *Bcut = nullptr;
  TH1F *Tcut = nullptr;
  std::string histogram_name = Form("part");
  std::string Bcut_histogram = Form("Bcut");
  std::string Tcut_histogram = Form("Tcut");
  float label_size = 0.05;
  float margin_size = 0.13;
  // Initialize legend: x-min, y-min, x-max, y-max
  auto legend = new TLegend(0.75, 0.7, 0.87, 0.9);

  //std::cout << "Histogram: " << histogram_name << std::endl;
    
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw("colz");
  histogram->SetTitle("");                      // Removing title
  histogram->SetStats(0);                       // Remove stats
  histogram->SetLabelSize(label_size, "xyz");   // Label size for x-, y- and z-axis
  histogram->SetTitleSize(label_size, "xy");    // Text size for x- and y-axis
  histogram->GetYaxis()->SetTitleOffset(1.1);   // Move y-axis text a little closer
  histogram->SetAxisRange(22.2, 56.6, "X");
  //gStyle->SetTitleSize(label_size, "t");        // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetRightMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogz();                              // Log-scale on z-axis

  // Beam cut
  Bcut = (TH1F *)infile->Get(Bcut_histogram.c_str());
  Bcut->SetLineWidth(4);
  Bcut->Draw("SAME");
  
  // Target cut
  Tcut = (TH1F *)infile->Get(Tcut_histogram.c_str());
  Tcut->SetLineWidth(4);
  Tcut->SetLineStyle(7);
  Tcut->Draw("SAME");

  // Legend
  legend->AddEntry(Bcut, Form("^{140}Sm"), "l");
  legend->AddEntry(Tcut, Form("^{208}Pb"), "l");
  gStyle->SetLegendTextSize(0.04);
  legend->Draw();

  canvas->SaveAs(Form("../../Plots/plotting/particle-events-wcut.png"));
}


void beam_gated_dcB_gammas_core(std::string setup_file, std::string sorting) {
  /*
      Plotting data sorted by CLXAna.
      Collects the histogram of beam gated prompt, Doppler corrected gamma-rays.

      Choose which sorting type to use: 
      "d" for default, "s" for singles, "a" for add back, "r" for reject.
  */
  std::string input_file = get_input_file(setup_file, sorting);

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name = Form("Beam gated prompt, Doppler corrected gamma-rays");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH2F *histogram = nullptr;
  float label_size = 0.05;
  float margin_size = 0.13;

  std::string histogram_name = Form("B_dcB_cid");
  histogram = (TH2F *)infile->Get(histogram_name.c_str());

  histogram->Draw("colz");
  histogram->SetTitle("");                      // Removing title
  histogram->SetStats(0);                       // Remove stats
  histogram->SetLabelSize(label_size, "xyz");   // Label size for x-, y- and z-axis
  histogram->SetTitleSize(label_size, "xy");    // Text size for x- and y-axis
  histogram->GetYaxis()->SetTitleOffset(1.1);   // Move y-axis text a little closer
  histogram->SetAxisRange(0, 1000, "Y");
  //gStyle->SetTitleSize(label_size, "t");        // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetRightMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogz();                              // Log-scale on z-axis

  canvas->SaveAs(Form("../../Plots/plotting/B_dcB_cid.png")); 
}


void dcB_x_ProjY(std::string setup_file, std::string sorting, bool is_beam) {
  /*
      Plotting data sorted by CLXAna.
      Collects the histogram of beam/target gated by lab angle, background 
      subtracted gamma-rays, Doppler corrected for scattered projectile, 
      and performs a projection of y-axis.

      Choose which sorting type to use: 
      "d" for default, "s" for singles, "a" for add back, "r" for reject.
      Choose to use beam gated or target gated.
      Use true, 1 or any number for beam gated spectra.
      Use false or 0 for target gated spectra.
  */
  std::string input_file = get_input_file(setup_file, sorting);

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH2F *histogram = nullptr;
  std::string histogram_name;

  float label_size = 0.05;
  float margin_size = 0.13;

  canvas_name = Form("dcB_x");
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

    Y_projection->SetStats(0);                      // Remove stats
    Y_projection->SetLabelSize(label_size, "xyz");  // Label size for x-, y- and z-axis
    Y_projection->SetTitleSize(label_size, "xy");   // Text size for x- and y-axis
    Y_projection->GetYaxis()->SetTitle("Counts");   // Change y-axis title
    Y_projection->GetYaxis()->SetTitleOffset(1.1);  // Move y-axis text a little closer
    gStyle->SetTitleSize(label_size, "t");          // Title size
    gStyle->SetStatFontSize(0.04);                  // Stats font size
    gPad->SetLeftMargin(margin_size);
    gPad->SetRightMargin(margin_size);
    gPad->SetBottomMargin(margin_size);
    gPad->SetLogy();                              // Log-scale on z-axis

    canvas->SaveAs(Form("../../Plots/plotting/Projections-single/%s-ProjY-%d.png", histogram_name.c_str(), i+1));
  }
}


void dcB_x_ProjY_combine(std::string setup_file, std::string sorting, bool is_beam, int bin1, int bin2) {
  /*
      Plotting data sorted by CLXAna.
      Collects the histogram of beam/target gated by lab angle, background 
      subtracted gamma-rays, Doppler corrected for scattered projectile, 
      and performs a projection of y-axis.

      Choose which sorting type to use: 
      "d" for default, "s" for singles, "a" for add back, "r" for reject.
      Choose to use beam gated or target gated.
      Use true, 1 or any number for beam gated spectra.
      Use false or 0 for target gated spectra.

      Bins start counting from 2.
  */
  if (bin1 > bin2) {
    std::cout << "First bin is larger than second bin." << std::endl;
  }

  if ( !(bin1 >= 2 && bin1 <= 16) && !(bin2 >= 2 && bin2 <= 16) ) {
    std::cout << "Invalid option. Choose between bin 2-16." << std::endl;
  }

  std::string input_file = get_input_file(setup_file, sorting);

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH2F *histogram = nullptr;
  std::string histogram_name;
  int beam_or_target = 0;

  float label_size = 0.05;
  float margin_size = 0.13;

  canvas_name = Form("dcB_x");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);

  if (is_beam) {
    histogram_name = Form("B_dcB_x");
  } else {
    histogram_name = Form("T_dcB_x");
    beam_or_target = 1;
  }
  histogram = (TH2F *)infile->Get(histogram_name.c_str());

  std::string angle[16] = {"22", "26", "29.1", "32.2", "35.2", "37.9", "40.4", "42.8", 
                           "45.0", "47.1", "49.0", "50.7", "52.4", "53.9", "55.3", "56.7"};

  std::string CM_angle[2][16] = 
  {
    {"36.6", "43.2", "48.2", "53.3", "58.1", "62.4", "66.3", "70.1", "73.5", "76.7", "79.6", "82.1", "84.7", "86.9", "88.9", "91.0"},
    {"136.0", "128.0", "121.8", "115.6", "109.6", "104.2", "99.2", "94.4", "90.0", "85.8", "82.0", "78.6", "75.2", "72.2", "69.4", "66.6"}
  };

  TH1D *Y_projection = histogram->ProjectionY(" ", bin1, bin2);
  Y_projection->SetTitle(Form("LAB: %s[%s-%s deg], CM: B[%s-%s deg]", histogram_name.substr(0, 1).c_str(), angle[bin1-2].c_str(), angle[bin2-1].c_str(),
                              CM_angle[beam_or_target][bin2-1].c_str(), CM_angle[beam_or_target][bin1-2].c_str())); // Changing titles
  Y_projection->Draw();

  Y_projection->SetStats(0);                      // Remove stats
  Y_projection->SetLabelSize(label_size, "xyz");  // Label size for x-, y- and z-axis
  Y_projection->SetTitleSize(label_size, "xy");   // Text size for x- and y-axis
  Y_projection->GetYaxis()->SetTitle("Counts");   // Change y-axis title
  Y_projection->GetYaxis()->SetTitleOffset(0.9);  // Move y-axis text a little closer
  gStyle->SetTitleSize(label_size, "t");          // Title size
  //gStyle->SetStatFontSize(0.04);                  // Stats font size
  gPad->SetLeftMargin(margin_size);
  gPad->SetRightMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogy();                              // Log-scale on z-axis

  canvas->SaveAs(Form("../../Plots/plotting/Projections-combined/%s-ProjY_%d-%d_combined.png", histogram_name.c_str(), bin1, bin2));
}


void gg_matrix_dcB(std::string setup_file, std::string sorting) {
  /*
      Plotting data sorted by CLXAna.
      Collects the histogram of gamma-gamma matrix, Doppler corrected for beam.

      Choose which sorting type to use: 
      "d" for default, "s" for singles, "a" for add back, "r" for reject.
  */
  std::string input_file = get_input_file(setup_file, sorting);

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name = Form("Gamma-gamma matrix, Doppler corrected for beam");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH2F *histogram = nullptr;
  float label_size = 0.05;
  float margin_size = 0.13;

  std::string histogram_name = Form("gg_dcB");
  histogram = (TH2F *)infile->Get(histogram_name.c_str());

  histogram->Draw("colz");
  histogram->SetTitle("");                      // Removing title
  histogram->SetStats(0);                       // Remove stats
  histogram->SetLabelSize(label_size, "xyz");   // Label size for x-, y- and z-axis
  histogram->SetTitleSize(label_size, "xy");    // Text size for x- and y-axis
  histogram->GetYaxis()->SetTitleOffset(1.1);   // Move y-axis text a little closer
  histogram->GetZaxis()->SetTitle("");          // Removing z-axis title
  histogram->SetAxisRange(0, 1000, "X");
  histogram->SetAxisRange(0, 1000, "Y");
  //gStyle->SetTitleSize(label_size, "t");        // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetRightMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogz();                              // Log-scale on z-axis

  canvas->SaveAs(Form("../../Plots/plotting/gg_dcB.png")); 
}


void total_statistics(std::string setup_file, std::string sorting) {
  /*
      Plotting data sorted by CLXAna.
      Collects the histogram of total statistics for gamma rays, 
      background subtracted, Doppler corrected for scattered projectile.

      Choose which sorting type to use: 
      "d" for default, "s" for singles, "a" for add back, "r" for reject.
  */
  std::string input_file = get_input_file(setup_file, sorting);

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name = Form("Total statistics, Doppler corrected");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH2F *histogram = nullptr;
  float label_size = 0.05;
  float margin_size = 0.13;

  std::string histogram_name = Form("gam_dcB");
  histogram = (TH2F *)infile->Get(histogram_name.c_str());

  histogram->Draw("colz");
  histogram->SetTitle("");                        // Removing title
  histogram->SetStats(0);                         // Remove stats
  histogram->SetLabelSize(label_size, "xy");      // Label size for x- and y-axis
  histogram->SetTitleSize(label_size, "xy");      // Text size for x- and y-axis
  histogram->GetYaxis()->SetTitle("Counts/keV");  // Change y-axis title
  histogram->GetYaxis()->SetTitleOffset(1.0);     // Move y-axis text a little closer
  //histogram->SetAxisRange(0, 1000, "X");
  //histogram->SetAxisRange(0, 1000, "Y");
  //gStyle->SetTitleSize(label_size, "t");          // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetRightMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogy();                              // Log-scale on y-axis

  // Write and draw arrows on plot
  // Coordinates: x, y
  TLatex Lxray(90, 50, "^{208}Pb x-rays");
  Lxray.SetTextSize(0.03);
  Lxray.SetTextAngle(90);
  Lxray.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *Axray = new TArrow(76, 1000, 76, 6000, 0.015, "|>");
  Axray->SetAngle(40);     // Angle of the arrow tip
  Axray->SetLineWidth(2);
  Axray->Draw();
  
  // Compton
  TLine *Compton_min = new TLine(98, 1000, 98, 6000);
  Compton_min->SetLineColor(kRed);
  Compton_min->SetLineStyle(kDashed);
  Compton_min->SetLineWidth(2);
  Compton_min->Draw(); 
  TLine *Compton_max = new TLine(370, 1000, 370, 3300);
  Compton_max->SetLineColor(kRed);
  Compton_max->SetLineStyle(kDashed);
  Compton_max->SetLineWidth(2);
  Compton_max->Draw();
  TLatex latex;
  latex.SetTextSize(0.03);
  latex.SetTextColor(kRed);
  latex.DrawLatex(120, 2500, "Compton");
  latex.DrawLatex(120, 1400, "continuum");

  // Coordinates: x, y
  TLatex LG460(470, 50, "2_{2}^{+} #rightarrow 2_{1}^{+}");          // 460
  LG460.SetTextSize(0.03);
  LG460.SetTextAngle(90);
  LG460.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G460 = new TArrow(457, 300, 457, 1780, 0.015, "|>");
  G460->SetAngle(40);     // Angle of the arrow tip
  G460->SetLineWidth(2);
  G460->Draw();

  // Coordinates: x, y
  TLatex LG531(541, 50, "2_{1}^{+} #rightarrow 0_{1}^{+}");          // 531 (2+)
  LG531.SetTextSize(0.03);
  LG531.SetTextAngle(90);
  LG531.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G531 = new TArrow(528, 300, 528, 1780, 0.015, "|>");
  G531->SetAngle(40);     // Angle of the arrow tip
  G531->SetLineWidth(2);
  G531->Draw();

  // Coordinates: x, y
  TLatex LG715(725, 5, "4_{1}^{+} #rightarrow 2_{1}^{+}");           // 715
  LG715.SetTextSize(0.03);
  LG715.SetTextAngle(90);
  LG715.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G715 = new TArrow(712, 30, 712, 300, 0.015, "|>");
  G715->SetAngle(40);     // Angle of the arrow tip
  G715->SetLineWidth(2);
  G715->Draw();

  // Coordinates: x, y
  TLatex LG768(780, 10000, "5 #rightarrow 4_{1}^{+}");               // 769
  LG768.SetTextSize(0.03);
  LG768.SetTextAngle(90);
  LG768.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G769 = new TArrow(768, 8000, 768, 400, 0.015, "|>");
  G769->SetAngle(40);        // Angle of the arrow tip
  G769->SetLineWidth(2);
  G769->Draw();

  // Coordinates: x, y
  TLatex LG836(849, 5, "6_{1}^{+} #rightarrow 4_{1}^{+}");           // 836
  LG836.SetTextSize(0.03);
  LG836.SetTextAngle(90);
  LG836.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G836 = new TArrow(836, 30, 836, 100, 0.015, "|>");
  G836->SetAngle(40);        // Angle of the arrow tip
  G836->SetLineWidth(2);
  G836->Draw();

  // Coordinates: x, y
  TLatex LG888(899, 10000, "8_{1}^{+} #rightarrow 6_{1}^{+}");       // 888
  LG888.SetTextSize(0.03);
  LG888.SetTextAngle(90);
  LG888.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G888 = new TArrow(887, 8000, 887, 400, 0.015, "|>");
  G888->SetAngle(40);        // Angle of the arrow tip
  G888->SetLineWidth(2);
  G888->Draw();

  // Coordinates: x, y
  TLatex LG990(1001, 600, "2_{2}^{+}");                              // 990 (2+)
  LG990.SetTextSize(0.03);
  LG990.SetTextAngle(90);
  LG990.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G990 = new TArrow(988, 500, 988, 150, 0.015, "|>");
  G990->SetAngle(40);        // Angle of the arrow tip
  G990->SetLineWidth(2);
  G990->SetLineColor(kGreen+2);
  G990->SetFillColor(kGreen+2);
  G990->Draw();

  // Coordinates: x, y
  TLatex LG1068(1080, 10000, "0_{2}^{+} #rightarrow 2_{1}^{+}");     // 1068
  LG1068.SetTextSize(0.03);
  LG1068.SetTextAngle(90);
  LG1068.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1068 = new TArrow(1068, 8000, 1068, 150, 0.015, "|>");
  G1068->SetAngle(40);        // Angle of the arrow tip
  G1068->SetLineWidth(2);
  G1068->Draw();

  // Coordinates: x, y
  TLatex LG1098(1117, 600, "0_{3}^{(+)} #rightarrow 2_{1}^{+}");     // 1098
  LG1098.SetTextSize(0.03);
  LG1098.SetTextAngle(90);
  LG1098.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1098 = new TArrow(1096, 500, 1096, 150, 0.015, "|>");
  G1098->SetAngle(40);     // Angle of the arrow tip
  G1098->SetLineWidth(2);
  G1098->Draw();

  // Coordinates: x, y
  TLatex LG1246(1257, 600, "4_{1}^{+}");                             // 1246 (4+)
  LG1246.SetTextSize(0.03);
  LG1246.SetTextAngle(90);
  LG1246.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1246 = new TArrow(1246, 500, 1246, 150, 0.015, "|>");
  G1246->SetAngle(40);     // Angle of the arrow tip
  G1246->SetLineWidth(2);
  G1246->SetLineColor(kGreen+2);
  G1246->SetFillColor(kGreen+2);
  G1246->Draw();

  // Coordinates: x, y
  TLatex LG1294(1307, 10000, "2_{3}^{+} #rightarrow 2_{2}^{+}");     // 1294
  LG1294.SetTextSize(0.03);
  LG1294.SetTextAngle(90);
  LG1294.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1294 = new TArrow(1294, 8000, 1294, 70, 0.015, "|>");
  G1294->SetAngle(40);     // Angle of the arrow tip
  G1294->SetLineWidth(2);
  G1294->Draw();

  // Coordinates: x, y
  TLatex LG1299(1332, 600, "(1,2)_{2} #rightarrow 2_{2}^{+}");       // 1299
  LG1299.SetTextSize(0.03);
  LG1299.SetTextAngle(90);
  LG1299.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1299 = new TArrow(1302, 500, 1302, 70, 0.015, "|>");
  G1299->SetAngle(40);     // Angle of the arrow tip
  G1299->SetLineWidth(2);
  G1299->Draw();

  // Coordinates: x, y
  TLatex LG1420(1423, 600, "(1,2)_{1} #rightarrow 0_{1}^{+}");       // 1420
  LG1420.SetTextSize(0.03);
  LG1420.SetTextAngle(90);
  LG1420.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1420 = new TArrow(1412, 500, 1412, 70, 0.015, "|>");
  G1420->SetAngle(40);     // Angle of the arrow tip
  G1420->SetLineWidth(2);
  G1420->Draw();

  // Coordinates: x, y
  TLatex LG1599(1612, 10000, "0_{2}^{+}");                           // 1599 (0+)
  LG1599.SetTextSize(0.03);
  LG1599.SetTextAngle(90);
  LG1599.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1599 = new TArrow(1602, 8000, 1602, 60, 0.015, "|>");
  G1599->SetAngle(40);     // Angle of the arrow tip
  G1599->SetLineWidth(2);
  G1599->SetLineColor(kGreen+2);
  G1599->SetFillColor(kGreen+2);
  G1599->Draw();

  // Coordinates: x, y
  TLatex LG1628(1649, 600, "0_{3}^{(+)}");                           // 1628 (0+)
  LG1628.SetTextSize(0.03);
  LG1628.SetTextAngle(90);
  LG1628.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1628 = new TArrow(1625, 500, 1625, 60, 0.015, "|>");
  G1628->SetAngle(40);     // Angle of the arrow tip
  G1628->SetLineWidth(2);
  G1628->SetLineColor(kGreen+2);
  G1628->SetFillColor(kGreen+2);
  G1628->Draw();

  // Coordinates: x, y
  TLatex LG1753(1766, 10000, "2_{3}^{+} #rightarrow 2_{1}^{+}");     // 1753
  LG1753.SetTextSize(0.03);
  LG1753.SetTextAngle(90);
  LG1753.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1753 = new TArrow(1753, 8000, 1753, 60, 0.015, "|>");
  G1753->SetAngle(40);     // Angle of the arrow tip
  G1753->SetLineWidth(2);
  G1753->Draw();

  // Coordinates: x, y
  TLatex LG1759(1791, 600, "(1,2)_{2} #rightarrow 2_{1}^{+}");       // 1759
  LG1759.SetTextSize(0.03);
  LG1759.SetTextAngle(90);
  LG1759.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1759 = new TArrow(1759, 500, 1759, 60, 0.015, "|>");
  G1759->SetAngle(40);     // Angle of the arrow tip
  G1759->SetLineWidth(2);
  G1759->Draw();

  // Coordinates: x, y
  TLatex LG1952(1963, 600, "(1,2)^{+} #rightarrow 2_{1}^{+}");       // 1952
  LG1952.SetTextSize(0.03);
  LG1952.SetTextAngle(90);
  LG1952.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1952 = new TArrow(1950, 500, 1950, 60, 0.015, "|>");
  G1952->SetAngle(40);     // Angle of the arrow tip
  G1952->SetLineWidth(2);
  G1952->Draw();

  canvas->SaveAs(Form("../../Plots/plotting/%s.png", histogram_name.c_str())); 
}


void total_statistics_s_and_a() {
  /*
      Plotting data sorted by CLXAna. Singles and addback.
      Collects the histogram of total statistics for gamma rays, 
      background subtracted, Doppler corrected for scattered projectile.

      Choose which sorting type to use: 
      "d" for default, "s" for singles, "a" for add back, "r" for reject.
  */
  std::string input_file = "../../Sorted_data/gdcB_tot.root";

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name = Form("Total statistics, Doppler corrected");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH2F *histogram = nullptr;
  TH2F *histogram_addback = nullptr;
  float label_size = 0.05;
  float margin_size = 0.13;

  histogram = (TH2F *)infile->Get(Form("gam_dcB_d"));  // singles
  histogram_addback = (TH2F *)infile->Get(Form("gam_dcB_a")); 

  histogram_addback->Draw();
  histogram_addback->SetLineColor(kRed);
  
  histogram->Draw("SAME");  // Singles
  histogram->SetStats(0);   // Remove stats
  
  histogram_addback->SetTitle("");                        // Removing title
  histogram_addback->SetStats(0);                         // Remove stats
  histogram_addback->SetLabelSize(label_size, "xy");      // Label size for x- and y-axis
  histogram_addback->SetTitleSize(label_size, "xy");      // Text size for x- and y-axis
  histogram_addback->GetYaxis()->SetTitle("Counts/keV");  // Change y-axis title
  histogram_addback->GetYaxis()->SetTitleOffset(1.0);     // Move y-axis text a little closer
  //histogram->SetAxisRange(0, 1000, "X");
  //histogram->SetAxisRange(0, 1000, "Y");
  //gStyle->SetTitleSize(label_size, "t");          // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetRightMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogy();                              // Log-scale on y-axis


  // Write and draw arrows on plot
  // Coordinates: x, y
  TLatex Lxray(90, 50, "^{208}Pb x-rays");
  Lxray.SetTextSize(0.03);
  Lxray.SetTextAngle(90);
  Lxray.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *Axray = new TArrow(76, 1000, 76, 6000, 0.015, "|>");
  Axray->SetAngle(40);     // Angle of the arrow tip
  Axray->SetLineWidth(2);
  Axray->Draw();
  
  // Compton
  TLine *Compton_min = new TLine(98, 1000, 98, 6000);
  Compton_min->SetLineColor(kRed);
  Compton_min->SetLineStyle(kDashed);
  Compton_min->SetLineWidth(2);
  Compton_min->Draw(); 
  TLine *Compton_max = new TLine(370, 1000, 370, 3300);
  Compton_max->SetLineColor(kRed);
  Compton_max->SetLineStyle(kDashed);
  Compton_max->SetLineWidth(2);
  Compton_max->Draw();
  TLatex latex;
  latex.SetTextSize(0.03);
  latex.SetTextColor(kRed);
  latex.DrawLatex(120, 2500, "Compton");
  latex.DrawLatex(120, 1400, "continuum");

  // Coordinates: x, y
  TLatex LG460(470, 50, "2_{2}^{+} #rightarrow 2_{1}^{+}");          // 460
  LG460.SetTextSize(0.03);
  LG460.SetTextAngle(90);
  LG460.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G460 = new TArrow(457, 300, 457, 1780, 0.015, "|>");
  G460->SetAngle(40);     // Angle of the arrow tip
  G460->SetLineWidth(2);
  G460->Draw();

  // Coordinates: x, y
  TLatex LG531(541, 50, "2_{1}^{+} #rightarrow 0_{1}^{+}");          // 531 (2+)
  LG531.SetTextSize(0.03);
  LG531.SetTextAngle(90);
  LG531.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G531 = new TArrow(528, 300, 528, 1780, 0.015, "|>");
  G531->SetAngle(40);     // Angle of the arrow tip
  G531->SetLineWidth(2);
  G531->Draw();

  // Coordinates: x, y
  TLatex LG715(725, 5, "4_{1}^{+} #rightarrow 2_{1}^{+}");           // 715
  LG715.SetTextSize(0.03);
  LG715.SetTextAngle(90);
  LG715.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G715 = new TArrow(712, 30, 712, 300, 0.015, "|>");
  G715->SetAngle(40);     // Angle of the arrow tip
  G715->SetLineWidth(2);
  G715->Draw();

  // Coordinates: x, y
  TLatex LG768(780, 10000, "5 #rightarrow 4_{1}^{+}");               // 769
  LG768.SetTextSize(0.03);
  LG768.SetTextAngle(90);
  LG768.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G769 = new TArrow(768, 8000, 768, 500, 0.015, "|>");
  G769->SetAngle(40);        // Angle of the arrow tip
  G769->SetLineWidth(2);
  G769->Draw();

  // Coordinates: x, y
  TLatex LG836(849, 5, "6_{1}^{+} #rightarrow 4_{1}^{+}");           // 836
  LG836.SetTextSize(0.03);
  LG836.SetTextAngle(90);
  LG836.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G836 = new TArrow(836, 30, 836, 100, 0.015, "|>");
  G836->SetAngle(40);        // Angle of the arrow tip
  G836->SetLineWidth(2);
  G836->Draw();

  // Coordinates: x, y
  TLatex LG888(899, 10000, "8_{1}^{+} #rightarrow 6_{1}^{+}");       // 888
  LG888.SetTextSize(0.03);
  LG888.SetTextAngle(90);
  LG888.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G888 = new TArrow(887, 8000, 887, 500, 0.015, "|>");
  G888->SetAngle(40);        // Angle of the arrow tip
  G888->SetLineWidth(2);
  G888->Draw();

  // Coordinates: x, y
  TLatex LG990(1001, 600, "2_{2}^{+}");                              // 990 (2+)
  LG990.SetTextSize(0.03);
  LG990.SetTextAngle(90);
  LG990.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G990 = new TArrow(988, 500, 988, 250, 0.015, "|>");
  G990->SetAngle(40);        // Angle of the arrow tip
  G990->SetLineWidth(2);
  G990->SetLineColor(kGreen+2);
  G990->SetFillColor(kGreen+2);
  G990->Draw();

  // Coordinates: x, y
  TLatex LG1068(1080, 10000, "0_{2}^{+} #rightarrow 2_{1}^{+}");     // 1068
  LG1068.SetTextSize(0.03);
  LG1068.SetTextAngle(90);
  LG1068.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1068 = new TArrow(1068, 8000, 1068, 200, 0.015, "|>");
  G1068->SetAngle(40);        // Angle of the arrow tip
  G1068->SetLineWidth(2);
  G1068->Draw();

  // Coordinates: x, y
  TLatex LG1098(1117, 600, "0_{3}^{(+)} #rightarrow 2_{1}^{+}");     // 1098
  LG1098.SetTextSize(0.03);
  LG1098.SetTextAngle(90);
  LG1098.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1098 = new TArrow(1096, 500, 1096, 200, 0.015, "|>");
  G1098->SetAngle(40);     // Angle of the arrow tip
  G1098->SetLineWidth(2);
  G1098->Draw();

  // Coordinates: x, y
  TLatex LG1246(1257, 600, "4_{1}^{+}");                             // 1246 (4+)
  LG1246.SetTextSize(0.03);
  LG1246.SetTextAngle(90);
  LG1246.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1246 = new TArrow(1246, 500, 1246, 200, 0.015, "|>");
  G1246->SetAngle(40);     // Angle of the arrow tip
  G1246->SetLineWidth(2);
  G1246->SetLineColor(kGreen+2);
  G1246->SetFillColor(kGreen+2);
  G1246->Draw();

  // Coordinates: x, y
  TLatex LG1294(1307, 10000, "2_{3}^{+} #rightarrow 2_{2}^{+}");     // 1294
  LG1294.SetTextSize(0.03);
  LG1294.SetTextAngle(90);
  LG1294.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1294 = new TArrow(1294, 8000, 1294, 100, 0.015, "|>");
  G1294->SetAngle(40);     // Angle of the arrow tip
  G1294->SetLineWidth(2);
  G1294->Draw();

  // Coordinates: x, y
  TLatex LG1299(1332, 600, "(1,2)_{2} #rightarrow 2_{2}^{+}");       // 1299
  LG1299.SetTextSize(0.03);
  LG1299.SetTextAngle(90);
  LG1299.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1299 = new TArrow(1302, 500, 1302, 100, 0.015, "|>");
  G1299->SetAngle(40);     // Angle of the arrow tip
  G1299->SetLineWidth(2);
  G1299->Draw();

  // Coordinates: x, y
  TLatex LG1420(1423, 600, "(1,2)_{1} #rightarrow 0_{1}^{+}");       // 1420
  LG1420.SetTextSize(0.03);
  LG1420.SetTextAngle(90);
  LG1420.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1420 = new TArrow(1412, 500, 1412, 80, 0.015, "|>");
  G1420->SetAngle(40);     // Angle of the arrow tip
  G1420->SetLineWidth(2);
  G1420->Draw();

  // Coordinates: x, y
  TLatex LG1599(1612, 10000, "0_{2}^{+}");                           // 1599 (0+)
  LG1599.SetTextSize(0.03);
  LG1599.SetTextAngle(90);
  LG1599.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1599 = new TArrow(1602, 8000, 1602, 60, 0.015, "|>");
  G1599->SetAngle(40);     // Angle of the arrow tip
  G1599->SetLineWidth(2);
  G1599->SetLineColor(kGreen+2);
  G1599->SetFillColor(kGreen+2);
  G1599->Draw();

  // Coordinates: x, y
  TLatex LG1628(1649, 600, "0_{3}^{(+)}");                           // 1628 (0+)
  LG1628.SetTextSize(0.03);
  LG1628.SetTextAngle(90);
  LG1628.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1628 = new TArrow(1625, 500, 1625, 60, 0.015, "|>");
  G1628->SetAngle(40);     // Angle of the arrow tip
  G1628->SetLineWidth(2);
  G1628->SetLineColor(kGreen+2);
  G1628->SetFillColor(kGreen+2);
  G1628->Draw();

  // Coordinates: x, y
  TLatex LG1753(1766, 10000, "2_{3}^{+} #rightarrow 2_{1}^{+}");     // 1753
  LG1753.SetTextSize(0.03);
  LG1753.SetTextAngle(90);
  LG1753.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1753 = new TArrow(1753, 8000, 1753, 60, 0.015, "|>");
  G1753->SetAngle(40);     // Angle of the arrow tip
  G1753->SetLineWidth(2);
  G1753->Draw();

  // Coordinates: x, y
  TLatex LG1759(1791, 600, "(1,2)_{2} #rightarrow 2_{1}^{+}");       // 1759
  LG1759.SetTextSize(0.03);
  LG1759.SetTextAngle(90);
  LG1759.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1759 = new TArrow(1759, 500, 1759, 60, 0.015, "|>");
  G1759->SetAngle(40);     // Angle of the arrow tip
  G1759->SetLineWidth(2);
  G1759->Draw();

  // Coordinates: x, y
  TLatex LG1952(1963, 600, "(1,2)^{+} #rightarrow 2_{1}^{+}");       // 1952
  LG1952.SetTextSize(0.03);
  LG1952.SetTextAngle(90);
  LG1952.Draw();
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *G1952 = new TArrow(1950, 500, 1950, 60, 0.015, "|>");
  G1952->SetAngle(40);     // Angle of the arrow tip
  G1952->SetLineWidth(2);
  G1952->Draw();

  canvas->SaveAs(Form("../../Plots/plotting/gam_dcB_s_and_a.png")); 
}