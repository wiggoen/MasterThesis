#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
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


void simulation_plot(std::string setup_file, bool single_plot) {
  /*
      Plotting data and saving histograms simulated by kinsim3.
      The single_plot option is for storing either single plots, 
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
  
  if (!single_plot) {
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
    canvas[0]->SaveAs(Form("../../Plots/simulation/kin_140Sm_208Pb.pdf"));
  }

  for (int ring = 0; ring < adc.rings; ring++) {
    if (!single_plot) {
      canvas[0]->cd(ring+1);
    } else {
      canvas[ring+1] = new TCanvas(histogram_name.c_str(), histogram_name.c_str(), 1280, 800); 
    }
    histogram_name = Form("cd_sim_%d", ring+1);
    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw();
    if (!single_plot) {
      histogram->SetAxisRange(0.95*element->start_bin, 1.05*element->end_bin, "X");
      histogram->GetYaxis()->SetLabelSize(0.06);
      histogram->GetXaxis()->SetLabelSize(0.06);
      if (ring == adc.rings-1) {
        canvas[0]->SaveAs(Form("../../Plots/simulation/cd_sim_all.pdf"));
      } 
    } else {
      // Shorthand if-else statement: (condition) ? (if_true) : (if_false);
      (ring < 9) ? zero = "0" : zero = "";
      canvas[ring+1]->SaveAs(Form("../../Plots/simulation/cd_sim_%s%d.pdf", zero.c_str(), ring+1));
    }
  }
}


void ADC_plot(std::string setup_file, bool use_calibrated) {
  /*
      Plotting data sorted by TreeBuilder. 
      Choose to use calibrated spectra or not.
      Use true, 1 or any number for calibrated spectra.
      Use false or 0 for uncalibrated spectra.
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
      histogram = (TH1F *)infile->Get(histogram_name.c_str());
      histogram->Draw();
      //histogram->SetAxisRange(0, 500, "X");
      histogram->GetYaxis()->SetLabelSize(0.06);
      histogram->GetXaxis()->SetLabelSize(0.06);
    }
  }
}


void AQ4_plot(std::string setup_file, std::string gate, bool use_calibrated) {
  /*
      Plotting data sorted by AQ4Sort.
      Choose which side and front ring or back strip to gate on. 
      E.g.: "f10" or "b3".
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
      histogram = (TH1F *)infile->Get(histogram_name.c_str());
      histogram->Draw();
      //histogram->SetAxisRange(0, 200, "X");
      histogram->GetYaxis()->SetLabelSize(0.06);
      histogram->GetXaxis()->SetLabelSize(0.05);  
    }
  }
}


void plot_side(std::string setup_file, std::string detector_side, bool use_calibrated) {
  /*
      Plotting data sorted by AQ4Sort.
      Choose ("b")ack side or ("f")ront side of detector.
      Choose to use calibrated spectra or not.
      Use true, 1 or any number for calibrated spectra.
      Use false or 0 for uncalibrated spectra.
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
        }
      } else if (detector_side == "f") {
        histogram_name = Form("%sE_Q%d_f%d%s", detector_side.c_str(), quadrant+1, ring+1, calibrated.c_str());
        //std::cout << histogram_name << std::endl;
        histogram = (TH1F *)infile->Get(histogram_name.c_str());
        if (use_calibrated) { histogram->GetXaxis()->SetRange(0, 1000); }
        histogram->Draw();   
      }
    }
  }
}


void plot_quadrants(std::string setup_file, std::string detector_side, 
                    int ring_gate, bool use_calibrated, int back_strip = 1) {
  /*
      Fitting data sorted by AQ4Sort.
      Choose the detector side, quadrant, front ring to gate on, the back strip you 
      want to look at.
      Choices: ("f")ront or ("b")ack side, quadrant 1-4, ring gate 1-16, if you want the 
      calibrated spectra or not (0 or false, 1 or true), back strip 1-12.
      Back strip only valid for looking at the back side of the detector.
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

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    histogram_name = Form("%sE_Q%d_f%d%s%s", detector_side.c_str(), quadrant+1, ring_gate, extension.c_str(), calibrated.c_str());
    //std::cout << histogram_name << std::endl;
    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw("SAME");
    if (use_calibrated) {
      histogram->SetAxisRange(0, 1E6, "X");
    } 
    histogram->GetYaxis()->SetLabelSize(0.06);
    histogram->GetXaxis()->SetLabelSize(0.06);
  }
}


void plot_back_quadrants(std::string setup_file, int front_ring, bool use_calibrated) {
  /*
      Fitting data sorted by AQ4Sort. Shows all back strips in each separated quadrant.
      Choose which front ring number to gate on. Choice: 1-16.
      Choose to use calibrated spectra or not.
      Use true, 1 or any number for calibrated spectra.
      Use false or 0 for uncalibrated spectra.
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

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    for (int strip = 0; strip < adc.strips; strip++) {
      histogram_name = Form("bE_Q%d_f%d_b%d%s", quadrant+1, front_ring, strip+1, calibrated.c_str());
      //std::cout << histogram_name << std::endl;
      histogram = (TH1F *)infile->Get(histogram_name.c_str());
      histogram->SetLineColor(colors[strip]);
      histogram->Draw("SAME");
      //histogram->SetAxisRange(0, 3000, "X");
      histogram->GetYaxis()->SetLabelSize(0.06);
      histogram->GetXaxis()->SetLabelSize(0.06);
    }
  }
}


void plot_front_back_energy(std::string setup_file, std::string name_addition = "") {
  /*
      Plots data sorted from TreeBuilder.
      Plots front vs back energy for the four quadrants.
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

  if (!name_addition.empty()) {
    name_addition = "-" + name_addition;
  }

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    histogram_name = Form("CD_spec/E_f_b_%d", quadrant);
    //std::cout << histogram_name << std::endl;
    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw("colz");
    histogram->GetYaxis()->SetLabelSize(0.06);
    histogram->GetXaxis()->SetLabelSize(0.06);
  }
  canvas->SaveAs(Form("../../Plots/plotting/E_f_b_Q1-4%s.pdf", name_addition.c_str()));
}


void check_all_threshold(std::string setup_file) {
  /*
      Plots data sorted from TreeBuilder.
      Plots the threshold for each quadrant and ring/strip.
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
      //(quadrant < adc.quadrants) ? channel = adc.rings-ring-1 : channel = adc.rings + ring;  // front first, then back
      (quadrant < adc.quadrants) ? channel = ring : channel = adc.rings + ring;
      canvas[quadrant]->cd(ring+1);
      histogram_name = Form("adc_spec/adc_%d_%d", quadrant_number, channel);
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
      histogram->GetYaxis()->SetLabelSize(0.06);
      histogram->GetXaxis()->SetLabelSize(0.05);

      pos = quadrant_number*(adc.rings+adc.strips) + channel;
      x_value = element->parameters.at(pos);
      line = new TLine(x_value, 0, x_value, histogram->GetBinContent(x_value)*2);
      line->SetLineColor(kRed);
      line->SetLineStyle(kDashed);
      line->Draw("SAME");

      //y_line = new TLine(0, 300, 1000, 300);
      //y_line->SetLineColor(kRed);
      //y_line->SetLineStyle(kDashed);
      //y_line->Draw("SAME");

    }
    //canvas[quadrant]->SaveAs(Form("../../Plots/plotting/Threshold_Q%d_%s.pdf", quadrant_number+1, detector_side.c_str()));
  }
}


void check_single_threshold(std::string setup_file, std::string detector_side,
                            int quadrant, int strip) {
  /*
      Plots data sorted from TreeBuilder.
      Plots the threshold for a given quadrant and ring/strip.
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
  int channel = 0;
  int pos = 0;
  double x_value = 0;
  int x_max = 0;
  int y_max = 0;  

  if (detector_side == "f") {
      detector_side_name = "Front";
      channel = strip-1;
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
  } else if (detector_side == "b") {
      detector_side_name = "Back";
      channel = adc.rings + strip-1;
      x_max = 3000;
      y_max = 2500;
  }

  canvas_name = Form("Quadrant %d, %s threshold", quadrant, detector_side_name.c_str());
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);

  histogram_name = Form("adc_spec/adc_%d_%d", quadrant-1, channel);
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();
  histogram->SetAxisRange(0, x_max, "X");
  histogram->SetAxisRange(0, y_max, "Y");
  //histogram->GetYaxis()->SetLabelSize(0.06);
  //histogram->GetXaxis()->SetLabelSize(0.06);

  pos = (quadrant-1)*(adc.rings+adc.strips) + channel;
  x_value = element->parameters.at(pos);
  line = new TLine(x_value, 0, x_value, histogram->GetBinContent(x_value)*2);
  line->SetLineColor(kRed);
  line->SetLineStyle(kDashed);
  line->Draw("SAME");

  //y_line = new TLine(0, 300, 1000, 300);
  //y_line->SetLineColor(kRed);
  //y_line->SetLineStyle(kDashed);
  //y_line->Draw("SAME");  
  canvas->SaveAs(Form("../../Plots/plotting/Threshold_Q%d_%s%d.pdf", quadrant, detector_side.c_str(), strip));
}


void check_ADC_time_offsets(std::string setup_file, std::string name_addition = "") {
  /*
      Plots data sorted from TreeBuilder.
      Plots ADC time offsets for the 4 ADC quadrants.
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

  if (!name_addition.empty()) {
    name_addition = "-" + name_addition;
  }

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    histogram_name = Form("tdiff_gp_%d", quadrant);
    //std::cout << histogram_name << std::endl;
    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw();
    histogram->SetAxisRange(-10, 10, "X");
    histogram->GetYaxis()->SetLabelSize(0.06);
    histogram->GetXaxis()->SetLabelSize(0.06);
  }
  canvas->SaveAs(Form("../../Plots/plotting/tdiff_gp_0-3%s.pdf", name_addition.c_str()));
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
  //canvas->Divide(2, 2);
  TH1F *histogram = nullptr;
  std::string histogram_name;

  if (!name_addition.empty()) {
    name_addition = "-" + name_addition;
  }

  histogram_name = Form("cd_debug");
  //std::cout << histogram_name << std::endl;
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();
  histogram->SetAxisRange(0, 25, "X");
  histogram->GetYaxis()->SetLabelSize(0.06);
  histogram->GetXaxis()->SetLabelSize(0.06);
  canvas->SaveAs(Form("../../Plots/plotting/cd_debug%s.pdf", name_addition.c_str()));
}


void check_pedestal(std::string setup_file, std::string detector_side,
                            int quadrant, int strip) {
  /*
      Plots data sorted from TreeBuilder.
      Plots the pedestal for a given quadrant and ring/strip.
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
  int channel = 0;
 
  if (detector_side == "f") {
      channel = strip-1;
  } else if (detector_side == "b") {
      channel = adc.rings + strip-1;
  }

  canvas_name = Form("Pedestal Q%d_%s%d", quadrant, detector_side.c_str(), strip);
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  histogram_name = Form("adc_spec/adc_%d_%d", quadrant-1, channel);
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();
  histogram->SetAxisRange(0, 100, "X");
  //histogram->GetYaxis()->SetLabelSize(0.06);
  //histogram->GetXaxis()->SetLabelSize(0.06); 
  canvas->SaveAs(Form("../../Plots/plotting/Pedestal_Q%d_%s%d.pdf", quadrant, detector_side.c_str(), strip));
}


void energy_vs_angle() {
  /*

  */
  ADC adc;

  std::string input_file = "../../Sorted_data/test_analysis.root";

  TFile *infile = new TFile(input_file.c_str(), "UPDATE");

  TCanvas *canvas = nullptr;
  std::string canvas_name = Form("Energy vs angles");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  canvas->Divide(2, 2);
  TH1F *histogram = nullptr;
  std::string histogram_name;

  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas->cd(quadrant+1);
    histogram_name = Form("partQ%d", quadrant+1);
    //std::cout << histogram_name << std::endl;
    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw("colz");
    gPad->SetLogz();
    histogram->SetAxisRange(22, 57, "X");
    //histogram->GetYaxis()->SetLabelSize(0.06);
    //histogram->GetXaxis()->SetLabelSize(0.06);
  }
  //canvas->SaveAs(Form("../../Plots/plotting/E_vs_theta_all_Q.pdf")); 
}
