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

  std::cout << "Histogram: " << histogram_name << std::endl;
    
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw("colz");
  histogram->SetTitle("");                      // Removing title
  histogram->SetStats(0);                       // Remove stats
  histogram->SetLabelSize(label_size, "xyz");   // Label size for x-, y- and z-axis
  histogram->SetTitleSize(label_size, "xy");    // Text size for x- and y-axis
  histogram->GetYaxis()->SetTitleOffset(1.0);   // Move y-axis text a little closer
  histogram->SetAxisRange(20, 58, "X");
  //gStyle->SetTitleSize(label_size, "t");        // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetRightMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogz();                              // Log-scale on z-axis
}






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
