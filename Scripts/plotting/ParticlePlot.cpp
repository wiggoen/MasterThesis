#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TArrow.h"
#include "TLatex.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>


class ADC {
  public:
    const int rings = 16;
    const int strips = 12;
    const int quadrants = 4;
    ADC(void) { std::cout << "Constructor called for ADC." << std::endl; }
    ~ADC(void) { std::cout << "Destructor called for ADC." << std::endl; }
};


// TODO:
// Make to other functions. This is to get a specific plot from the root file.
// This function should save the spectra as pdf or something in the plots folder.
// Should also show the fitting based on values I have used. 
// !! Check that the centroid is the same !!
// e.g. get_adc_spectra(f10, Q4)
// e.g. get_aq4_spectra(f10)


// TODO:
// Fix bug: If the file given is wrong/doesn't exist, 
// it will be created and read as "successful".
// else statement never reached. Is this due to ROOT?
class Element {
  public:
    std::string element_name;
    std::string sim_infile;
    std::string ADC_infile;
    std::string AQ4_infile;
    std::string threshold_infile;
    int start_bin = 0;
    int end_bin = 0;
    std::vector<double> parameters;

    Element(void) { std::cout << "Constructor called for Element." << std::endl; }
    ~Element(void) { std::cout << "Destructor called for Element." << std::endl; }

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
        this->element_name = var_value_vec.at(0);
        this->sim_infile = var_value_vec.at(1);
        this->start_bin = std::stoi(var_value_vec.at(2));
        this->end_bin = std::stoi(var_value_vec.at(3));
        this->ADC_infile = var_value_vec.at(4);
        this->AQ4_infile = var_value_vec.at(5);
        this->threshold_infile = var_value_vec.at(6);

        std::cout << "Finished reading from input file. Closing file." << std::endl;
        input_file.close();
      } else {
        std::cerr << "Unable to open '" << infile << "'. Program terminated." 
                  << std::endl;
        exit(EXIT_FAILURE);
      }
    }

    void read_parameters(std::string infile) {
      std::ifstream input_file(infile);  // Open file containing the values
      std::string line;
      std::vector<std::string> var_name_vec;
      if (input_file.is_open()) {
        std::cout << "File containing guesses opened. Reading..." << std::endl;
        bool first_line = true;
        while (std::getline(input_file, line)) {
          std::stringstream line_stream(line);  // Used for breaking words
          std::string name;
          std::string parameter;
          if (line_stream >> name >> parameter) {
            if (first_line) {
              first_line = false;  // skip first line
            } else {
              var_name_vec.push_back(name);
              this->parameters.push_back(std::stod(parameter));
            }
          } 
        }
        std::cout << "Finished reading file containing guesses. Closing file." << std::endl;

        input_file.close();
      } else {
        std::cerr << "Unable to open '" << infile << "'. Program terminated." 
                  << std::endl;
        exit(EXIT_FAILURE);
      }
    }
};


void simulation_plot(std::string setup_file, bool single_plots) {
  /*
      Plotting data and saving histograms simulated by kinsim3.
      The single_plots option is for storing either single plots, 
      or a all-in-one plot of the simulated rings.
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);

  std::cout << "Looking at " << element->element_name 
            << " from file: " << element->sim_infile << std::endl;

  TFile *infile = new TFile(element->sim_infile.c_str(), "UPDATE");
  
  TCanvas *canvas[adc.rings+1];
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;
  std::string zero;
  
  float label_size = 0.05;
  float label_size_AllInOne = 0.07;
  float margin_size = 0.13;
  float margin_size_AllInOne = 0.14;

  if (!single_plots) {
    canvas_name = element->sim_infile;
    canvas[0] = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
    canvas[0]->Divide(4, 4);
  } else {
    // Kinematics
    canvas_name = "kin_140Sm_208Pb";
    canvas[0] = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
    histogram_name = Form("cd_sim");
    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw();
    histogram->SetStats(0);                      // Remove stats
    histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
    histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
    histogram->GetYaxis()->SetTitleOffset(1.1);  // Move y-axis text a little closer
    histogram->SetTitle(Form("LAB frame"));      // Change title of histogram
    gStyle->SetTitleSize(label_size, "t");       // Title size
    gPad->SetLeftMargin(margin_size);
    gPad->SetRightMargin(margin_size);
    gPad->SetBottomMargin(margin_size);
    canvas[0]->SaveAs(Form("../../Plots/simulation/kin_140Sm_208Pb.png"));
  }

  for (int ring = 0; ring < adc.rings; ring++) {
    if (!single_plots) {
      canvas[0]->cd(ring+1);
    } else {
      canvas[ring+1] = new TCanvas(histogram_name.c_str(), histogram_name.c_str(), 1280, 800); 
    }
    histogram_name = Form("cd_sim_%d", ring+1);

    //std::cout << "Histogram: " << histogram_name << std::endl;

    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw();
    histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
    histogram->GetYaxis()->SetTitleOffset(1.1);  // Move y-axis text a little closer
    if (!single_plots) {
      histogram->SetAxisRange(0.95*element->start_bin, 1.05*element->end_bin, "X");
      histogram->SetStats(0);                              // Remove stats
      histogram->SetTitle(Form("Ring %d", ring+1));        // Changing titles
      histogram->SetLabelSize(label_size_AllInOne, "xy");  // Label size for x- and y-axis
      histogram->SetTitleSize(label_size_AllInOne, "xy");  // Text  size for x- and y-axis
      gStyle->SetTitleSize(label_size_AllInOne, "t");      // Title size
      gPad->SetLeftMargin(margin_size_AllInOne);
      gPad->SetRightMargin(margin_size_AllInOne);
      gPad->SetBottomMargin(0.15);
      if (ring == adc.rings-1) {
        canvas[0]->SaveAs(Form("../../Plots/simulation/cd_sim_all.pdf"));
      } 
    } else {
      // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
      (ring < 9) ? zero = "0" : zero = "";
      histogram->SetStats(0);                     // Remove stats
      histogram->SetLabelSize(label_size, "xy");  // Label size for x- and y-axis
      histogram->SetTitleSize(label_size, "xy");  // Text  size for x- and y-axis
      gStyle->SetTitleSize(label_size, "t");      // Title size
      gPad->SetLeftMargin(margin_size);
      gPad->SetRightMargin(margin_size);
      gPad->SetBottomMargin(margin_size);
      canvas[ring+1]->SaveAs(Form("../../Plots/simulation/cd_sim_%s%d.png", zero.c_str(), ring+1));
    }
  }
}


void ADC_plot(std::string setup_file, bool use_calibrated) {
  /*
      Plotting data sorted by TreeBuilder. Energy in MeV.
      This function plots an overview of the four quadrants
      with all front strips and back strips separately.
      Choose to use calibrated spectra or not.
      Use true, 1 or any number for calibrated spectra.
      Use false or 0 for uncalibrated spectra.
      Counting on front side: f1 (innermost ring) to f16 (outermost ring).
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);

  std::cout << "Looking at " << element->element_name 
            << " from file: " << element->ADC_infile << std::endl;
  
  std::string calibrated;
  // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
  use_calibrated ? calibrated = "_cal" : calibrated = "";
  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");
  TCanvas *canvas[2*adc.quadrants];
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;
  std::string detector_side;
  int channel = 0;
  int quadrant_number = 0;
  float label_size = 0.07;
  float margin_size = 0.15;
  
  for (int quadrant = 0; quadrant < 2*adc.quadrants; quadrant++) {   
    if (quadrant < adc.quadrants) {
      detector_side = "Front";
      quadrant_number = quadrant;
    } else {
      detector_side = "Back";
      quadrant_number = quadrant-adc.quadrants;
    }
    canvas_name = Form("%s, quadrant %d", detector_side.c_str(), quadrant_number+1);
    canvas[quadrant] = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
    // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
    (quadrant < adc.quadrants) ? canvas[quadrant]->Divide(4, 4) : canvas[quadrant]->Divide(4, 3);
    for (int ring = 0; ring < adc.rings; ring++) {   
      if (quadrant >= adc.quadrants && ring >= adc.strips) { break; }
      // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
      (quadrant < adc.quadrants) ? channel = adc.rings-ring-1 : channel = adc.rings + ring;
      canvas[quadrant]->cd(ring+1);
      histogram_name = Form("adc_spec/adc_%d_%d%s", quadrant_number, channel, calibrated.c_str());
      
      //std::cout << "Histogram: " << histogram_name << std::endl;

      histogram = (TH1F *)infile->Get(histogram_name.c_str());
      histogram->Draw();
      histogram->SetStats(0);                      // Remove stats
      histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
      histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
      histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
      histogram->GetYaxis()->SetTitleOffset(1.0);  // Move y-axis text a little closer
      histogram->GetYaxis()->SetMaxDigits(3);      // Force scientific notation if number is large
      histogram->SetTitle(Form("Q%d %s %d %s", quadrant_number+1, detector_side.c_str(), ring+1, calibrated.c_str()));  // Changing titles
      // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
      (use_calibrated) ? histogram->SetAxisRange(40, 750, "X") : histogram->SetAxisRange(100, 3000, "X");
      gStyle->SetTitleSize(label_size, "t");       // Title size
      gPad->SetLeftMargin(margin_size);
      gPad->SetBottomMargin(margin_size);
    }
  }
}


void AQ4_plot(std::string setup_file, std::string gate, bool use_calibrated) {
  /*
      Plotting data sorted by AQ4Sort. Energy in keV.
      This function plots an overview of the four quadrants
      with the gated front strips or back strips separately.
      Choose which side and front ring or back strip to gate on. 
      E.g.: "f10" or "b3".
      Counting on front side: f1 (outermost ring) to f16 (innermost ring).
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);

  std::cout << "Looking at " << element->element_name 
            << " from file: " << element->AQ4_infile << std::endl;

  std::string gate_side = gate.substr(0, 1);  // first letter of gate
  std::string gate_number = gate.substr(1);   // everything except the first letter
  int strip_gate = std::stoi(gate_number);    // convert string to int
  if ( !(strip_gate >= 1 && ((gate_side == "b" && strip_gate <= 12) || (gate_side == "f" && strip_gate <= 16))) ) {
    std::cout << "Invalid option. Choose either (b)ack or (f)ront and between strip 1-12 for back or ring 1-16 for front." << std::endl;
  }
  std::string calibrated;
  // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
  use_calibrated ? calibrated = "_cal" : calibrated = "";
  TFile *infile = new TFile(element->AQ4_infile.c_str(), "UPDATE");
  TCanvas *canvas[adc.quadrants];
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;
  std::string detector_side;
  std::string detector_gate;
  std::string vs;
  int front_ring = 0;
  int back_strip = 0;
  float label_size = 0.07;
  float margin_size = 0.15;
  if (gate_side == "b") {
    detector_side = "f";
    detector_gate = "back";
    vs = "front rings 1-16";
  } else if (gate_side == "f") {
    detector_side = "b";
    detector_gate = "front";
    vs = "back strips 1-12";
  }

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas_name = Form("Quadrant %d, %s gate %s%d vs %s", quadrant+1, detector_gate.c_str(), gate_side.c_str(), strip_gate, vs.c_str());
    canvas[quadrant] = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
    // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
    (gate_side == "f") ? canvas[quadrant]->Divide(4, 3) : canvas[quadrant]->Divide(4, 4);
    for (int ring = 0; ring < adc.rings; ring++) {   
      if (gate_side == "f" && ring >= adc.strips) { break; }
      if (gate_side == "f") {
        front_ring = strip_gate;
        back_strip = ring+1;
      } else if (gate_side == "b") {
        front_ring = ring+1;
        back_strip = strip_gate;
      }
      canvas[quadrant]->cd(ring+1);
      histogram_name = Form("%sE_Q%d_f%d_b%d%s", detector_side.c_str(), quadrant+1, front_ring, back_strip, calibrated.c_str());
      
      //std::cout << "Histogram: " << histogram_name << std::endl;

      histogram = (TH1F *)infile->Get(histogram_name.c_str());
      histogram->Draw();
      histogram->SetStats(0);                      // Remove stats
      histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
      histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
      histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
      histogram->GetYaxis()->SetTitleOffset(1.1);  // Move y-axis text a little closer
      histogram->GetYaxis()->SetMaxDigits(3);      // Force scientific notation if number is large
      // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
      (use_calibrated) ? histogram->SetAxisRange(40000, 750000, "X") : histogram->SetAxisRange(100, 3000, "X");
      if (use_calibrated) {
        histogram->GetXaxis()->SetMaxDigits(3);    // Force scientific notation if number is large
      }
      gStyle->SetTitleSize(label_size, "t");       // Title size
      gPad->SetLeftMargin(margin_size);
      gPad->SetRightMargin(margin_size);
      gPad->SetBottomMargin(margin_size);
    }
  }
}


void plot_side(std::string setup_file, std::string detector_side, bool use_calibrated) {
  /*
      Plotting data sorted by AQ4Sort. Energy in keV.
      Choose ("b")ack side or ("f")ront side of detector.
      Choose to use calibrated spectra or not.
      Use true, 1 or any number for calibrated spectra.
      Use false or 0 for uncalibrated spectra.
      Counting on front side: f1 (outermost ring) to f16 (innermost ring).
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);
  if ( !(detector_side == "b" || detector_side == "f")) {
    std::cout << "Invalid option. Choose detector side (b)ack or (f)ront." << std::endl;
  }
  std::string calibrated;
  // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
  use_calibrated ? calibrated = "_cal" : calibrated = "";

  std::cout << "Looking at " << element->element_name 
            << " from file: " << element->AQ4_infile << std::endl;
  
  TFile *infile = new TFile(element->AQ4_infile.c_str(), "UPDATE");
  TCanvas *canvas[adc.quadrants];
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;
  std::string detector_gate;
  if      (detector_side == "b") { detector_gate = "back"; } 
  else if (detector_side == "f") { detector_gate = "front"; }
  // Colors: kRed, kBlue, kGreen, kPink, kAzure, kSpring, kMagenta, kCyan, kYellow, kViolet, kTeal, kOrange
  int colors[] = { 632, 600, 416, 900, 860, 820, 616, 432, 400, 880, 840, 800 };
  float label_size = 0.07;
  float margin_size = 0.15;

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas_name = Form("Quadrant %d, %s energy", quadrant+1, detector_gate.c_str());
    canvas[quadrant] = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
    canvas[quadrant]->Divide(4, 4);
    for (int ring = 0; ring < adc.rings; ring++) {   
      canvas[quadrant]->cd(ring+1);
      if (detector_side == "b") {
        for (int back_strip = 1; back_strip <= adc.strips; back_strip++) {
          histogram_name = Form("%sE_Q%d_f%d_b%d%s", detector_side.c_str(), quadrant+1, ring+1, back_strip, calibrated.c_str());         
          //std::cout << histogram_name << std::endl;
          histogram = (TH1F *)infile->Get(histogram_name.c_str());                    
          histogram->SetLineColor(colors[back_strip - 1]);
          if (use_calibrated) { histogram->GetXaxis()->SetRange(0, 700); }
          histogram->Draw("SAME");
          histogram->SetStats(0);                      // Remove stats
          histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
          histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
          histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
          histogram->GetYaxis()->SetTitleOffset(1.0);  // Move y-axis text a little closer
          histogram->GetYaxis()->SetMaxDigits(3);      // Force scientific notation if number is large
          // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
          (use_calibrated) ? histogram->SetAxisRange(40000, 750000, "X") : histogram->SetAxisRange(100, 3000, "X");
          if (use_calibrated) {
            histogram->GetXaxis()->SetMaxDigits(3);    // Force scientific notation if number is large
          }
          gStyle->SetTitleSize(label_size, "t");       // Title size
          gPad->SetLeftMargin(margin_size);
          gPad->SetRightMargin(margin_size);
          gPad->SetBottomMargin(margin_size);
        }
      } else if (detector_side == "f") {
        histogram_name = Form("%sE_Q%d_f%d%s", detector_side.c_str(), quadrant+1, ring+1, calibrated.c_str());
        
        //std::cout << "Histogram: " << histogram_name << std::endl;
        
        histogram = (TH1F *)infile->Get(histogram_name.c_str());
        if (use_calibrated) { histogram->GetXaxis()->SetRange(0, 1000); }
        histogram->Draw();
        histogram->SetStats(0);                      // Remove stats
        histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
        histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
        histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
        histogram->GetYaxis()->SetTitleOffset(1.0);  // Move y-axis text a little closer
        histogram->GetYaxis()->SetMaxDigits(3);      // Force scientific notation if number is large
        // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
        (use_calibrated) ? histogram->SetAxisRange(40000, 750000, "X") : histogram->SetAxisRange(100, 3000, "X");
        if (use_calibrated) {
          histogram->GetXaxis()->SetMaxDigits(3);    // Force scientific notation if number is large
        }
        gStyle->SetTitleSize(label_size, "t");       // Title size
        gPad->SetLeftMargin(margin_size);
        gPad->SetRightMargin(margin_size);
        gPad->SetBottomMargin(margin_size);
      }
    }
  }
}


void plot_quadrants(std::string setup_file, std::string detector_side, 
                    int ring_gate, bool use_calibrated, int back_strip = 1) {
  /*
      Fitting data sorted by AQ4Sort. Energy in keV.
      Choose the detector side, quadrant, front ring to gate on, the back strip you 
      want to look at.
      Choices: ("f")ront or ("b")ack side, quadrant 1-4, ring gate 1-16, if you want the 
      calibrated spectra or not (0 or false, 1 or true), back strip 1-12.
      Back strip only valid for looking at the back side of the detector.
      Counting on front side: f1 (outermost ring) to f16 (innermost ring).
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);
  if ( !(detector_side == "b" || detector_side == "f")) {
    std::cout << "Invalid option. Choose detector side (b)ack or (f)ront." << std::endl;
  }
  if ( !(ring_gate >= 1 && ring_gate <= 16) ) {
    std::cout << "Invalid option. Choose between front ring 1-16." << std::endl;
  }
  if ( !(back_strip >= 1 && back_strip <= 12) ) {
    std::cout << "Invalid option. Choose between back strip 1-12." << std::endl;
  }
  std::string calibrated;
  // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
  use_calibrated ? calibrated = "_cal" : calibrated = "";

  std::cout << "Looking at " << element->element_name 
            << " from file: " << element->AQ4_infile << std::endl;

  std::string detector_side_name;
  std::string extension;
  std::string title_extension;
  if (detector_side == "b") {
    detector_side_name = "Back";
    extension = "_b" + std::to_string(back_strip);
    title_extension = ", back strip " + std::to_string(back_strip);
  } else if (detector_side == "f") {
    detector_side_name = "Front";
  }

  TFile *infile = new TFile(element->AQ4_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  canvas_name = Form("%s detector, quadrant 1-4, front ring %d%s", detector_side_name.c_str(), ring_gate, title_extension.c_str());
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  canvas->Divide(2, 2);
  TH1F *histogram = nullptr;
  std::string histogram_name;
  float label_size = 0.07;
  float margin_size = 0.14;

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    histogram_name = Form("%sE_Q%d_f%d%s%s", detector_side.c_str(), quadrant+1, ring_gate, extension.c_str(), calibrated.c_str());
    
    //std::cout << "Histogram: " << histogram_name << std::endl;

    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw("SAME");
    histogram->SetStats(0);                      // Remove stats
    histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
    histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
    histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
    histogram->GetYaxis()->SetTitleOffset(0.9);  // Move y-axis text a little closer
    histogram->GetYaxis()->SetMaxDigits(3);      // Force scientific notation if number is large
    if (use_calibrated) {
      histogram->SetAxisRange(40000, 750000, "X");
    } else {
      histogram->SetAxisRange(100, 3000, "X");
      histogram->GetXaxis()->SetTitle("Channel");   // Change x-axis title
    }
    gStyle->SetTitleSize(label_size, "t");       // Title size
    gPad->SetLeftMargin(margin_size);
    gPad->SetRightMargin(margin_size);
    gPad->SetBottomMargin(margin_size);
  }
}


void plot_back_quadrants(std::string setup_file, int front_ring, bool use_calibrated) {
  /*
      Fitting data sorted by AQ4Sort. Energy in keV.
      Shows all back strips in each separated quadrant.
      Choose which front ring number to gate on. Choice: 1-16.
      Choose to use calibrated spectra or not.
      Use true, 1 or any number for calibrated spectra.
      Use false or 0 for uncalibrated spectra.
      Counting on front side: f1 (outermost ring) to f16 (innermost ring).
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);
  if ( !(front_ring >= 1 && front_ring <= 16) ) {
    std::cout << "Invalid option. Choose between ring 1-16." << std::endl;
  }
  std::string calibrated;
  // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
  use_calibrated ? calibrated = "_cal" : calibrated = "";

  std::cout << "Looking at " << element->element_name 
            << " from file: " << element->AQ4_infile << std::endl;

  TFile *infile = new TFile(element->AQ4_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  canvas_name = Form("Quadrant Q1-4, front gate f%d vs back strips 1-12", front_ring);
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  canvas->Divide(2, 2);
  TH1F *histogram = nullptr;
  std::string histogram_name;
  // Colors: kRed, kBlue, kGreen, kPink, kAzure, kSpring, kMagenta, kCyan, kYellow, kViolet, kTeal, kOrange
  int colors[] = { 632, 600, 416, 900, 860, 820, 616, 432, 400, 880, 840, 800 };
  float label_size = 0.07;
  float margin_size = 0.14;

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    for (int strip = 0; strip < adc.strips; strip++) {
      histogram_name = Form("bE_Q%d_f%d_b%d%s", quadrant+1, front_ring, strip+1, calibrated.c_str());
      
      //std::cout << "Histogram: " << histogram_name << std::endl;
      
      histogram = (TH1F *)infile->Get(histogram_name.c_str());
      histogram->SetLineColor(colors[strip]);
      histogram->Draw("SAME");
      histogram->SetStats(0);                      // Remove stats
      histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
      histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
      histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
      histogram->GetYaxis()->SetTitleOffset(0.9);  // Move y-axis text a little closer
      histogram->GetYaxis()->SetMaxDigits(3);      // Force scientific notation if number is large
      if (use_calibrated) {
        histogram->SetAxisRange(40000, 750000, "X");
      } else {
        histogram->SetAxisRange(100, 3000, "X");
        histogram->GetXaxis()->SetTitle("Channel");  // Change x-axis title
      }
      gStyle->SetTitleSize(label_size, "t");         // Title size
      gPad->SetLeftMargin(margin_size);
      gPad->SetRightMargin(margin_size);
      gPad->SetBottomMargin(margin_size);
    }
  }
}


void plot_front_back_energy(std::string setup_file, std::string name_addition = "") {
  /*
      Plots data sorted from TreeBuilder. Energy in MeV.
      Plots front vs back energy for the four quadrants.
      Option: name_addition is an addition to the file name, 
      so it's easier to find the correct one. 
  */
  ADC adc;
  Element *element = new Element();

  element->read_setup_file(setup_file);

  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  canvas_name = Form("Front vs back energy");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  canvas->Divide(2, 2);
  TH1F *histogram = nullptr;
  std::string histogram_name;

  std::string title[4] = {"Q1", "Q2", "Q3", "Q4"};
  float label_size = 0.06;
  float margin_size = 0.13;

  if (!name_addition.empty()) {
    name_addition = "-" + name_addition;
  }

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    histogram_name = Form("CD_spec/E_f_b_%d", quadrant);

    //std::cout << "Histogram: " << histogram_name << std::endl;
    
    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw("colz");
    histogram->SetTitle(title[quadrant].c_str()); // Changing titles
    //std::cout << title[quadrant] << std::endl;
    histogram->SetStats(0);                       // Remove stats
    histogram->SetLabelSize(label_size, "xyz");   // Label size for x-, y- and z-axis
    histogram->SetTitleSize(label_size, "xy");    // Text size for x- and y-axis
    histogram->GetYaxis()->SetTitleOffset(1.0);   // Move y-axis text a little closer
    gStyle->SetTitleSize(label_size, "t");        // Title size
    gPad->SetLeftMargin(margin_size);
    gPad->SetRightMargin(margin_size);
    gPad->SetBottomMargin(margin_size);
  }
  canvas->SaveAs(Form("../../Plots/plotting/E_f_b_Q1-4%s.png", name_addition.c_str()));
}


void check_pedestal(std::string setup_file, std::string detector_side,
                            int quadrant, int strip, int x_max = 100) {
  /*
      Plots data sorted from TreeBuilder. Uncalibrated (counts vs. channels).
      Plots the pedestal for a given quadrant and ring/strip.
      Choices: ("f")ront or ("b")ack side, quadrant 1-4, 
      strip 1-16 for front side or strip 1-12 for back side.
      x_max chooses the maximum channel.
      Counting on front side: f1 (innermost ring) to f16 (outermost ring).
  */
  ADC adc;
  Element *element = new Element();

  element->read_setup_file(setup_file);
  element->read_parameters(element->threshold_infile);

  if ( !(detector_side == "b" || detector_side == "f")) {
    std::cout << "Invalid option. Choose detector side (b)ack or (f)ront." << std::endl;
  }
  if ( detector_side == "f" && !(strip >= 1 && strip <= 16) ) {
    std::cout << "Invalid option. Choose between front ring 1-16." << std::endl;
  }
  if ( detector_side == "b" && !(strip >= 1 && strip <= 12) ) {
    std::cout << "Invalid option. Choose between back strip 1-12." << std::endl;
  }

  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;
  std::string detector_side_character;
  int channel = 0;
  int strip_number = 0;
  float label_size = 0.05;
  float margin_size = 0.13;

  if (detector_side == "f") {
      detector_side_character = "F";
      channel = strip-1;
      strip_number = adc.rings - strip;  // For correct counting of front side plots
  } else if (detector_side == "b") {
      detector_side_character = "B";
      channel = adc.rings + strip-1;
      strip_number = channel;
  }

  canvas_name = Form("Pedestal Q%d_%s%d", quadrant, detector_side.c_str(), strip);
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  histogram_name = Form("adc_spec/adc_%d_%d", quadrant-1, strip_number);

  std::cout << "Histogram: " << histogram_name << std::endl;

  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();
  histogram->SetAxisRange(0, x_max, "X");
  histogram->SetStats(0);                      // Remove stats
  histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
  histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
  histogram->GetXaxis()->SetTitle("Channel");  // Change x-axis title
  histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
  histogram->GetYaxis()->SetTitleOffset(1.1);  // Move y-axis text a little closer
  histogram->SetTitle(Form("Q%d %s%d", quadrant, detector_side_character.c_str(), strip));  // Change title of histogram
  gStyle->SetTitleSize(label_size, "t");       // Title size
  //gStyle->SetStatFontSize(0.04);               // Stats font size
  gPad->SetLeftMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
 
  canvas->SaveAs(Form("../../Plots/plotting/Pedestal_Q%d_%s%d.png", quadrant, detector_side.c_str(), strip));
}


void check_all_threshold(std::string setup_file) {
  /*
      Plots data sorted from TreeBuilder. Energy in MeV.
      Plots the threshold for each quadrant and ring/strip.
      Counting on front side: f0 (outermost ring) to f15 (innermost ring).
  */
  ADC adc;
  Element *element = new Element();

  element->read_setup_file(setup_file);
  element->read_parameters(element->threshold_infile);

  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");
  TCanvas *canvas[2*adc.quadrants];
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;
  std::string detector_side;
  TLine *line = nullptr;

  TLine *y_line = nullptr;

  int quadrant_number = 0;
  int channel = 0;
  int pos = 0;
  double x_value = 0;
  int x_max = 0;
  int y_max = 0;

  float label_size = 0.07;
  float margin_size = 0.15;

  for (int quadrant = 0; quadrant < 2*adc.quadrants; quadrant++) {
    if (quadrant < adc.quadrants) {
      detector_side = "Front";
      quadrant_number = quadrant;
    } else {
      detector_side = "Back";
      quadrant_number = quadrant-adc.quadrants;
    }
    canvas_name = Form("Quadrant %d, %s threshold", quadrant_number+1, detector_side.c_str());
    canvas[quadrant] = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
    // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
    (quadrant < adc.quadrants) ? canvas[quadrant]->Divide(4, 4) : canvas[quadrant]->Divide(4, 3);
    for (int ring = 0; ring < adc.rings; ring++) {
      if (quadrant >= adc.quadrants && ring >= adc.strips) { break; }
      // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
      (quadrant < adc.quadrants) ? channel = ring : channel = adc.rings + ring;  // front: outermost ring first (default)
      //(quadrant < adc.quadrants) ? channel = adc.rings-ring-1 : channel = adc.rings + ring;  // front: innermost ring first
      canvas[quadrant]->cd(ring+1);
      histogram_name = Form("adc_spec/adc_%d_%d", quadrant_number, channel);

      //std::cout << "Histogram: " << histogram_name << std::endl;

      histogram = (TH1F *)infile->Get(histogram_name.c_str());
      histogram->Draw();

      if (quadrant < adc.quadrants) {
        if (channel < 4) {
          x_max = 2000;
          y_max = 4000;
        } else if (channel >= 4 && channel < 8) {
          x_max = 2500;
          y_max = 3000;
        }  else if (channel >= 8 && channel < 12) {
          x_max = 3000;
          y_max = 3000;
        } else {
          x_max = 3500;
          y_max = 12000;
        }
      } else {
          x_max = 3000;
          y_max = 2500;
      } 
      histogram->SetAxisRange(0, x_max, "X");
      histogram->SetAxisRange(0, y_max, "Y");

      histogram->SetStats(0);                      // Remove stats
      histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
      histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
      histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
      histogram->GetYaxis()->SetTitleOffset(1.1);  // Move y-axis text a little closer
      if (quadrant >= adc.quadrants) {
        histogram->GetYaxis()->SetMaxDigits(3);    // Force scientific notation if number is large
      }
      gStyle->SetTitleSize(label_size, "t");       // Title size
      gPad->SetLeftMargin(margin_size);
      gPad->SetBottomMargin(margin_size);

      pos = quadrant_number*(adc.rings+adc.strips) + channel;
      x_value = element->parameters.at(pos);
      line = new TLine(x_value, 0, x_value, histogram->GetBinContent(x_value)*2);
      line->SetLineColor(kRed);
      line->SetLineStyle(kDashed);
      line->Draw("SAME");
    }
    //canvas[quadrant]->SaveAs(Form("../../Plots/plotting/Threshold_Q%d_%s.pdf", quadrant_number+1, detector_side.c_str()));
  }
}


void check_single_threshold(std::string setup_file, std::string detector_side,
                            int quadrant, int strip, int x_max = 3500, int y_max = 12000) {
  /*
      Plots data sorted from TreeBuilder. Uncalibrated (counts vs. channels).
      Plots the threshold for a given quadrant and ring/strip.
      Choices: ("f")ront or ("b")ack side, quadrant 1-4, 
      strip 1-16 for front side or strip 1-12 for back side.
      x_max chooses the maximum channel, y_max chooses the maximum count.
      Counting on front side: f1 (innermost ring) to f16 (outermost ring).
  */
  ADC adc;
  Element *element = new Element();

  element->read_setup_file(setup_file);
  element->read_parameters(element->threshold_infile);

  if ( !(detector_side == "b" || detector_side == "f")) {
    std::cout << "Invalid option. Choose detector side (b)ack or (f)ront." << std::endl;
  }
  if ( detector_side == "f" && !(strip >= 1 && strip <= 16) ) {
    std::cout << "Invalid option. Choose between front ring 1-16." << std::endl;
  }
  if ( detector_side == "b" && !(strip >= 1 && strip <= 12) ) {
    std::cout << "Invalid option. Choose between back strip 1-12." << std::endl;
  }

  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;
  TLine *line = nullptr;

  std::string detector_side_name;
  int strip_number = 0;
  int channel = 0;
  int pos = 0;
  double x_value = 0;
  float label_size = 0.05;
  float margin_size = 0.13;

  if (detector_side == "f") {
      detector_side_name = "Front";
      channel = strip-1;
      strip_number = adc.rings - strip;  // For correct counting of front side plots
  } else if (detector_side == "b") {
      detector_side_name = "Back";
      channel = adc.rings + strip-1;
      strip_number = channel;
  }

  canvas_name = Form("Quadrant %d, %s threshold", quadrant, detector_side_name.c_str());
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);

  histogram_name = Form("adc_spec/adc_%d_%d", quadrant-1, strip_number);

  std::cout << "Histogram: " << histogram_name << std::endl;

  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();
  histogram->SetAxisRange(0, x_max, "X");
  histogram->SetAxisRange(0, y_max, "Y");
  histogram->SetStats(0);                      // Remove stats
  histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
  histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
  histogram->GetXaxis()->SetTitle("Channel");  // Change x-axis title
  histogram->GetYaxis()->SetTitle("Counts");   // Change y-axis title
  histogram->GetYaxis()->SetTitleOffset(1.2);  // Move y-axis text a little closer
  histogram->SetTitle(Form("Q%d %s%d", quadrant, detector_side_name.substr(0,1).c_str(), strip));  // Change title of histogram
  gStyle->SetTitleSize(label_size, "t");       // Title size
  //gStyle->SetStatFontSize(0.04);               // Stats font size
  gPad->SetLeftMargin(margin_size);
  gPad->SetBottomMargin(margin_size);

  pos = (quadrant-1)*(adc.rings+adc.strips) + channel;
  x_value = element->parameters.at(pos);
  line = new TLine(x_value, 0, x_value, histogram->GetBinContent(x_value)*2);
  line->SetLineColor(kRed);
  line->SetLineStyle(kDashed);
  line->Draw("SAME");
 
  canvas->SaveAs(Form("../../Plots/plotting/Threshold_Q%d_%s%d.png", quadrant, detector_side.c_str(), strip));
}


void check_ADC_time_offsets(std::string setup_file, std::string name_addition = "") {
  /*
      Plots data sorted from TreeBuilder.
      Plots ADC time offsets for the 4 CD quadrants.
  */
  ADC adc;
  Element *element = new Element();

  element->read_setup_file(setup_file);

  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  canvas_name = Form("Time offsets");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  canvas->Divide(2, 2);
  TH1F *histogram = nullptr;
  std::string histogram_name;
  float label_size = 0.06;
  float margin_size = 0.13;

  if (!name_addition.empty()) {
    name_addition = "-" + name_addition;
  }

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    histogram_name = Form("tdiff_gp_%d", quadrant);

    std::cout << "Histogram: " << histogram_name << std::endl;

    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw();
    histogram->SetAxisRange(-10, 10, "X");
    histogram->SetStats(0);                        // Remove stats
    histogram->SetLabelSize(label_size, "xy");     // Label size for x- and y-axis
    histogram->SetTitleSize(label_size, "xy");     // Text  size for x- and y-axis
    histogram->GetXaxis()->SetTitle("Time difference (particle - #gamma) [25 ns ticks]");  // Change x-axis title
    histogram->GetYaxis()->SetTitle("Counts");     // Change y-axis title
    histogram->GetYaxis()->SetTitleOffset(1.0);    // Move y-axis text a little closer
    histogram->SetTitle(Form("Q%d", quadrant+1));  // Change title of histogram
    gStyle->SetTitleSize(label_size, "t");         // Title size
    gPad->SetLeftMargin(margin_size);
    gPad->SetBottomMargin(margin_size);
  }
  canvas->SaveAs(Form("../../Plots/plotting/tdiff_gp_0-3%s.png", name_addition.c_str()));
}


void check_cd_debug(std::string setup_file, std::string name_addition = "") {
  /*
      Plots data sorted from TreeBuilder.
      Plots CD debugging.
  */
  ADC adc;
  Element *element = new Element();

  element->read_setup_file(setup_file);

  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  canvas_name = Form("CD debugging");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH1F *histogram = nullptr;
  std::string histogram_name;
  float label_size = 0.05;
  float margin_size = 0.13;

  if (!name_addition.empty()) {
    name_addition = "-" + name_addition;
  }

  histogram_name = Form("cd_debug");

  std::cout << "Histogram: " << histogram_name << std::endl;

  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();
  histogram->SetAxisRange(0, 25, "X");
  histogram->SetStats(0);                       // Remove stats
  histogram->SetLabelSize(label_size, "xy");    // Label size for x- and y-axis
  histogram->SetTitleSize(label_size, "xy");    // Text  size for x- and y-axis
  histogram->GetXaxis()->SetTitle("Debug ID");  // Change x-axis title
  histogram->GetYaxis()->SetTitle("Counts");    // Change y-axis title
  histogram->GetYaxis()->SetTitleOffset(0.7);   // Move y-axis text a little closer
  //histogram->SetTitle(Form(""));                // Remove title of histogram
  gStyle->SetTitleSize(label_size, "t");        // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetBottomMargin(margin_size);

  canvas->SaveAs(Form("../../Plots/plotting/cd_debug%s.png", name_addition.c_str()));
}


void energy_vs_ring(std::string setup_file) {
  /*
      Plots data sorted from TreeBuilder. Energy in MeV.
      Plots energy vs ring number for CD.
      Counting on front side: f1 (innermost ring) to f16 (outermost ring).
  */
  ADC adc;
  Element *element = new Element();

  element->read_setup_file(setup_file);

  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");

  TCanvas *canvas = nullptr;
  std::string canvas_name = Form("Energy vs ring number");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  TH1F *histogram = nullptr;
  std::string histogram_name;
  float label_size = 0.05;
  float margin_size = 0.13;

  histogram_name = Form("part");

  std::cout << "Histogram: " << histogram_name << std::endl;

  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw("colz");
  histogram->SetAxisRange(-0.5, 15, "X");
  histogram->SetStats(0);                      // Remove stats
  histogram->SetLabelSize(label_size, "xy");   // Label size for x- and y-axis
  histogram->SetTitleSize(label_size, "xy");   // Text  size for x- and y-axis
  histogram->GetXaxis()->SetTitle("Ring number");             // Change x-axis title
  histogram->GetYaxis()->SetTitle("Energy particle [MeV]");  // Change y-axis title
  histogram->GetYaxis()->SetTitleOffset(1.0);  // Move y-axis text a little closer
  histogram->SetTitle(Form(""));               // Remove title of histogram
  gStyle->SetTitleSize(label_size, "t");       // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  gPad->SetLogz();                             // Log-scale on z-axis

  // Change label on x-axis for correct counting of front side plots
  histogram->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"16");
  histogram->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"14");
  histogram->GetXaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"12");
  histogram->GetXaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"10");
  histogram->GetXaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"8");
  histogram->GetXaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"6");
  histogram->GetXaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"4");
  histogram->GetXaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"2");

  canvas->SaveAs(Form("../../Plots/plotting/E_vs_ring_all.png")); 
}


void CD_energy(std::string setup_file, std::string detector_side) {
  /*
      Plots data sorted from TreeBuilder. Energy in MeV.
      Plots energy vs ring number for the 4 quadrants of the CD.
      Choices: ("f")ront or ("b")ack side.
      Counting on front side: f1 (innermost ring) to f16 (outermost ring).
  */
  ADC adc;
  Element *element = new Element();

  element->read_setup_file(setup_file);

  if ( !(detector_side == "b" || detector_side == "f")) {
    std::cout << "Invalid option. Choose detector side (b)ack or (f)ront." << std::endl;
  }

  TFile *infile = new TFile(element->ADC_infile.c_str(), "UPDATE");

  TCanvas *canvas = nullptr;
  std::string canvas_name = Form("Energy vs strip number");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  canvas->Divide(2, 2);
  TH1F *histogram = nullptr;
  std::string histogram_name;
  std::string detector_side_name;
  std::string title[4] = {"Q1", "Q2", "Q3", "Q4"};
  float label_size = 0.06;
  float margin_size = 0.13;
  int x_max = 0;

  if (detector_side == "f") {
      detector_side_name = "front";
      x_max = 15;
  } else if (detector_side == "b") {
      detector_side_name = "back";
      x_max = 11;
  }

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    histogram_name = Form("CD_spec/CD_%s_energy_%d", detector_side_name.c_str(), quadrant);

    //std::cout << "Histogram: " << histogram_name << std::endl;

    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw("colz");
    histogram->SetAxisRange(-0.5, x_max, "X");
    histogram->SetStats(0);                        // Remove stats
    histogram->SetLabelSize(label_size, "xy");     // Label size for x- and y-axis
    histogram->SetTitleSize(label_size, "xy");     // Text  size for x- and y-axis
    histogram->GetYaxis()->SetTitleOffset(1.0);    // Move y-axis text a little closer
    histogram->SetTitle(title[quadrant].c_str());  // Changing titles
    gStyle->SetTitleSize(label_size, "t");         // Title size
    gPad->SetLeftMargin(margin_size);
    gPad->SetBottomMargin(margin_size);

    if (detector_side == "f") {
      gPad->SetLogz();
      //histogram->GetXaxis()->SetTitle("Ring number");  // Change x-axis title
      // Change label on x-axis for correct counting of front side plots
      histogram->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"16");
      histogram->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"14");
      histogram->GetXaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"12");
      histogram->GetXaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"10");
      histogram->GetXaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"8");
      histogram->GetXaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"6");
      histogram->GetXaxis()->ChangeLabel(7,-1,-1,-1,-1,-1,"4");
      histogram->GetXaxis()->ChangeLabel(8,-1,-1,-1,-1,-1,"2");
    } else if (detector_side == "b") {
      // Change label on x-axis for correct counting of back side plots
      histogram->GetXaxis()->ChangeLabel(1,-1,-1,-1,-1,-1,"1");
      histogram->GetXaxis()->ChangeLabel(2,-1,-1,-1,-1,-1,"3");
      histogram->GetXaxis()->ChangeLabel(3,-1,-1,-1,-1,-1,"5");
      histogram->GetXaxis()->ChangeLabel(4,-1,-1,-1,-1,-1,"7");
      histogram->GetXaxis()->ChangeLabel(5,-1,-1,-1,-1,-1,"9");
      histogram->GetXaxis()->ChangeLabel(6,-1,-1,-1,-1,-1,"11");
    }
  }
  canvas->SaveAs(Form("../../Plots/plotting/E_vs_%s-strip_all_Q.png", detector_side.c_str()));
}


void get_single_plot(std::string setup_file, std::string sorter, std::string detector_side, 
                     bool use_calibrated, int quadrant, int ring, int strip = 1,
                     int x_min = 0, int x_max = 3000, int y_max = 12000) {
  /*
      Plotting data sorted by TreeBuilder with energy in MeV or
      data sorted by AQ4Sort with energy in keV.
      This function gets a single plot of one strip on either 
      the front side or the back side.
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);
  std::string input_file;
  std::string calibrated;
  // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
  use_calibrated ? calibrated = "_cal" : calibrated = "";
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;
  std::string detector_side_name;
  std::string savefile_name;
  int quadrant_number = 0;
  int channel = 0;
  float label_size = 0.05;
  float margin_size = 0.13;

  if ( !(detector_side == "f" || detector_side == "b") ) {
    std::cout << "Invalid option. Choose between (f)ront side or (b)ack side." << std::endl;
  }
  if (sorter == "tb") {
    input_file = element->ADC_infile;
    if (detector_side == "f") {
      channel = adc.rings-ring;
      canvas_name = Form("Q%d F%d", quadrant, ring);
      savefile_name = Form("TB_Q%d_F%d%s", quadrant, ring, calibrated.c_str());
    } else if (detector_side == "b") {
      channel = adc.rings + strip - 1;
      canvas_name = Form("Q%d B%d", quadrant, strip);
      savefile_name = Form("TB_Q%d_B%d%s", quadrant, strip, calibrated.c_str());
    }
    histogram_name = Form("adc_spec/adc_%d_%d%s", quadrant-1, channel, calibrated.c_str());
  } else if (sorter == "q4") {
    input_file = element->AQ4_infile;
    channel = adc.rings-ring+1;
    canvas_name = Form("%sE Q%d F%d B%d", detector_side.c_str(), quadrant, ring, strip);
    histogram_name = Form("%sE_Q%d_f%d_b%d%s", detector_side.c_str(), quadrant, channel, strip, calibrated.c_str());
    savefile_name = Form("%sE_Q%d_f%d_b%d%s", detector_side.c_str(), quadrant, ring, strip, calibrated.c_str());
  } else {
    std::cout << "Invalid option. Choose either ''tb'' for TreeBuilder or ''q4'' for AQ4Sort." << std::endl;
  }
  if ( !(quadrant >= 1 && quadrant <= 4) ) {
    std::cout << "Invalid option. Choose between quadrant 1-4." << std::endl;
  }
  if ( !(ring >= 1 && ring <= 16) ) {
    std::cout << "Invalid option. Choose between front ring 1-16." << std::endl;
  }
  if ( !(strip >= 1 && strip <= 12) ) {
    std::cout << "Invalid option. Choose between back strip 1-12." << std::endl;
  }

  std::cout << "Looking at " << element->element_name 
            << " from file: " << input_file << std::endl;

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);

  std::cout << "Histogram: " << histogram_name << std::endl;

  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();  
  histogram->SetStats(0);                        // Remove stats
  histogram->SetLabelSize(label_size, "xy");     // Label size for x- and y-axis
  histogram->SetTitleSize(label_size, "xy");     // Text  size for x- and y-axis
  if (!use_calibrated) {
    histogram->GetXaxis()->SetTitle("Channel");  // Change x-axis title
  }
  histogram->GetYaxis()->SetTitle("Counts");     // Change y-axis title
  if (sorter == "tb") {
    histogram->GetYaxis()->SetTitleOffset(1.2);  // Move y-axis text a little closer
  } else if (sorter == "q4") {
    // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
    (use_calibrated) ? x_max *= 1000 : x_max *= 1;
    histogram->GetYaxis()->SetTitleOffset(1.2);  // Move y-axis text a little closer
  }
  histogram->SetTitle(canvas_name.c_str());      // Changing titles
  histogram->SetAxisRange(x_min, x_max, "X");
  histogram->SetAxisRange(0, y_max, "Y");
  gStyle->SetTitleSize(label_size, "t");         // Title size
  gPad->SetLeftMargin(margin_size);
  gPad->SetBottomMargin(margin_size);
  /*
  // -- Q4 F9 ----------
  // Write on plot
  TLatex latex;
  latex.SetTextSize(0.05);
  // Coordinates: x, y
  latex.DrawLatex(750, 900, "Contaminant");
  latex.DrawLatex(950, 600, "^{208}Pb");
  latex.DrawLatex(1600, 950, "^{140}Sm");
  // Draw arrow on plot
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *arrow = new TArrow(730, 925, 520, 925, 0.015, "|>");
  arrow->SetAngle(40);     // Angle of the arrow tip
  arrow->SetLineWidth(2);
  arrow->Draw();
  // -------------------
  */
  /*
  // -- bE Q4 F16 B2 ---
  // Write on plot
  TLatex latex;
  latex.SetTextSize(0.05);
  // Coordinates: x, y
  latex.DrawLatex(450, 275, "Contaminant");
  latex.DrawLatex(370, 110, "^{208}Pb");
  latex.DrawLatex(1030, 40, "^{140}Sm");
  // Draw arrow on plot
  // Coordinates: x-min, y-min, x-max, y-max
  TArrow *arrow = new TArrow(440, 282, 320, 282, 0.015, "|>");
  arrow->SetAngle(40);     // Angle of the arrow tip
  arrow->SetLineWidth(2);
  arrow->Draw();
  // -------------------
  */
  canvas->SaveAs(Form("../../Plots/plotting/%s.png", savefile_name.c_str()));
}
