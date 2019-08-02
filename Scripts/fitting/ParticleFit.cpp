#include "TH1.h"
#include "TF1.h"
#include "TFile.h"
#include "TMath.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
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
// Fix bug: If the file given is wrong/doesn't exist, 
// it will be created and read as "successful".
// else statement never reached. Is this due to ROOT?
class Element {
  public:
    std::string element_peak_1;
    std::string element_peak_2;
    std::string sim_infile;
    std::string sim_guess;
    std::string ADC_infile;
    std::string AQ4_infile;
    std::string front_ch_guess;
    std::string back_ch_guess;
    std::vector<std::string> var_name_vec;
    std::vector<std::string> var_value_vec;
    std::vector<std::string> headers;
    std::vector<double> min_bin;
    std::vector<double> max_bin;
    std::vector<double> mean_guess;
    std::vector<double> centroids;
    double front_width_guess = 0;
    double front_constant_guess = 0;
    double back_width_guess = 0;
    double back_constant_guess = 0;
    int peaks = 0;
    
    Element(void) { std::cout << "Constructor called for Element." << std::endl; }
    ~Element(void) { std::cout << "Destructor called for Element." << std::endl; }

    void read_setup_file(std::string infile) {
      std::ifstream input_file(infile);  // Open file containing the setup
      std::string line;
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
        this->element_peak_1 = var_value_vec.at(0);
        this->element_peak_2 = var_value_vec.at(1);
        this->peaks = std::stoi(var_value_vec.at(2));
        this->sim_infile = var_value_vec.at(3);
        this->sim_guess = var_value_vec.at(4);
        this->ADC_infile = var_value_vec.at(5);
        this->AQ4_infile = var_value_vec.at(6);
        this->front_ch_guess = var_value_vec.at(7);
        this->back_ch_guess = var_value_vec.at(8);
        this->front_width_guess = std::stod(var_value_vec.at(9));
        this->front_constant_guess = std::stod(var_value_vec.at(10));
        this->back_width_guess = std::stod(var_value_vec.at(11));
        this->back_constant_guess = std::stod(var_value_vec.at(12));

        std::cout << "Finished reading from input file. Closing file." << std::endl;
        
        input_file.close();
      } else {
        std::cerr << "Unable to open '" << infile << "'. Program terminated." 
                  << std::endl;
        exit(EXIT_FAILURE);
      }
    }

    void read_guesses(std::string infile) {
      std::ifstream input_file(infile);  // Open file containing the guesses
      std::string line;
      if (input_file.is_open()) {
        std::cout << "File containing guesses opened. Reading..." << std::endl;
        bool first_line = true;
        while (std::getline(input_file, line)) {
          std::stringstream line_stream(line);  // Used for breaking words
          std::string min;
          std::string max;
          std::string mean;
          if (line_stream >> min >> max >> mean) {
            if (first_line) {
              headers.push_back(min);
              headers.push_back(max);
              headers.push_back(mean);
              first_line = false;
            } else {
              this->min_bin.push_back(std::stod(min));
              this->max_bin.push_back(std::stod(max));
              this->mean_guess.push_back(std::stod(mean));
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

    void print_parameters(std::vector<double> vector) {
      for (unsigned long int i = 0; i < vector.size(); i++) {
        std::cout << vector.at(i) << std::endl;
      }
    }

    void print_centroids(void) {
      if (this->peaks == 1) {
        std::cout << std::setw(10) << this->element_peak_1 << std::endl;
        for (unsigned long int i = 0; i < this->centroids.size(); i++) {
          std::cout << std::setw(10) << centroids.at(i) << std::endl;
        }
      } else if (this->peaks == 2) {
          std::cout << std::setw(10) << this->element_peak_1 << std::setw(10) << this->element_peak_2 << std::endl;
          for (unsigned long int i = 0; i < this->centroids.size(); i+=2) {
            std::cout << std::setw(10) << this->centroids.at(i) 
                      << std::setw(10) << this->centroids.at(i+1) << std::endl;
          }
      }
    }
  private:
    ADC adc;  // Element gets access to ADC
};


double Gauss_lbg(double *x, double *parameter) 
{
  /*
      Gaussian function with linear background.

      This fitting function is a minor modification of
      Oleg Ivanov's fitting program function gaus_lbg(). 
      The original file can be found at:
      https://svn.physik.uni-muenchen.de/repos/miniball/trunk/miniball/cern-070917/FitPeaks.C

      parameter[0]   background constant
      parameter[1]   background slope
      parameter[2]   gauss width (sigma)
      parameter[3]   gauss0 constant (A) or amplitude
      parameter[4]   gauss0 mean (mu)
  */
  if (parameter[2] == 0) { parameter[2] = 1; }  // force widths /= 0
  double argument = (x[0] - parameter[4]) / parameter[2];
  double fitvalue = parameter[0] + x[0] * parameter[1];
  fitvalue += parameter[3] * TMath::Exp(- 0.5 * argument * argument);
  return fitvalue;
}


double FitPeak(TH1F *hist, std::string fit_name, double *parameter, double start_bin, double end_bin) {
  /*  
      The FitPeak function fits a Gaussian function with 
      linear background to the given histogram and draws it
      in the same plot. 

      This fitting function is a minor modification of
      Oleg Ivanov's fitting program functions
      FindSinglePeak() and FitSinglePeak(). 
      The original file can be found at:
      https://svn.physik.uni-muenchen.de/repos/miniball/trunk/miniball/cern-070917/FitPeaks.C
  */
  double centroid = 0;

  if (start_bin < end_bin) {
    int number_of_parameters = 5;
    // Creating the function of the form 'Gauss_lbg' defined above
    TF1 *fit_function = new TF1(fit_name.c_str(), Gauss_lbg, start_bin, end_bin, number_of_parameters);
    // Obtaining and specifying the start values for the fit
    double bg_constant = 0.00001;  // Assumed to be very small
    double bg_slope = 0.00001;     // Assumed to be very small
    double sigma = parameter[0];
    double gauss_constant = parameter[1];
    double mean = parameter[2];
    fit_function->SetParameters(bg_constant, bg_slope, sigma, gauss_constant, mean);
    fit_function->SetRange(start_bin, end_bin);
    fit_function->SetLineColor(kRed);
    fit_function->SetLineStyle(kDashed);
    // Fitting: 
    // 'Q': Quiet mode (minimum printing)
    // 'R': Use the range specified in the function range
    // '+': Add this new fitted function to the list of fitted functions (by default, the previous function is deleted and only the last one is kept)
    // 'L': Use log likelihood method (default is chi-square method). 
    //      To be used when the histogram represents counts.
    hist->Fit(fit_function, "QRL+", "SAME");
    // Extract the centroid
    centroid = fit_function->GetParameter(4);
    // Draw a line to check where the centroid is found
    TLine *line = new TLine(centroid, 0, centroid, hist->GetBinContent(centroid)*1.5);
    line->SetLineColor(kBlack);
    line->SetLineStyle(kDashed);
    line->Draw("SAME");
  } else if (start_bin == 0 && end_bin == 0) {
    std::cout << "Couldn't fit! Error: start_bin and end_bin set to zero!" << std::endl;
  } else {
    std::cout << "Couldn't fit! Error: the lower limit is larger than the upper limit!" << std::endl;
  }
  return centroid;
}


std::vector<double> Fit_4d_polynomial(TH1F *hist, double start_bin, double end_bin) {
  /*
      Fitting a 4th degree polynomial to the given histogram and draws it
      in the same plot. 
      Returns the x-value and y-value of the fitted peak.
  */
  std::vector<double> centroid {0, 0};  // x-value and y-value

  if (start_bin < end_bin) {
    //TF1 *fit_func = new TF1(fit_name.c_str(), "pol4", start_bin, end_bin);
    // Fitting: 
    // 'Q': Quiet mode (minimum printing)
    // 'R': Use the range specified in the function range
    // 'S': The result of the fit is returned in the TFitResultPtr
    TFitResultPtr fit_result = hist->Fit("pol4", "QRS", "SAME", start_bin, end_bin);
    TF1 *fit_function = hist->GetFunction("pol4");
    double *parameter = fit_function->GetParameters();
    // For some weird reason the '+' option is not available when using polynomials in the Fit() function.
    // This is a hack to draw the fitted function anyway.
    TGraph *graph = new TGraph(fit_function);
    graph->Draw("SAME");

    // Get the x- and y-value of the centroid
    double x_value = fit_function->GetMaximumX(start_bin, end_bin);
    double y_value = fit_function->GetMaximum(start_bin, end_bin);
    centroid.at(0) = x_value;
    centroid.at(1) = y_value;

    // Draw a line to check where the centroid is found
    TLine *line = new TLine(centroid.at(0), 0, centroid.at(0), centroid.at(1)*1.5);
    line->SetLineColor(kRed);
    line->SetLineStyle(kDashed);
    line->Draw("SAME");
  } else if (start_bin == 0 && end_bin == 0) {
    std::cout << "Couldn't fit! Error: start_bin and end_bin set to zero!" << std::endl;
  } else {
    std::cout << "Couldn't fit! Error: the lower limit is larger than the upper limit!" << std::endl;
  }
  return centroid;
}


void simulation_fit(std::string setup_file) {
  /*
      Fitting data simulated by kinsim3.
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);
  element->read_guesses(element->sim_guess);

  std::cout << "Looking at " << element->element_peak_1 << " and " << element->element_peak_1 
            << " from file: " << element->sim_infile << std::endl;

  std::cout << "--- Simulation fit of " << element->element_peak_2 << " started ---" << std::endl;

  // For plotting
  TFile *infile = new TFile(element->sim_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name = element->sim_infile;
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  canvas->Divide(4, 4);
  TH1F *histogram = nullptr;
  std::string histogram_name;
  float label_size = 0.07;
  float margin_size = 0.14;

  // For fitting
  std::string fit_name;
  double sigma = 0;
  double gauss_constant = 0;
  double mean = 0;
  double parameter[] = {0, 0, 0};
  double centroid = 0;
  double min_bin = 0;
  double max_bin = 0;
  double min_range = 0;
  double max_range = 0;
  std::string space;
  std::ofstream output_file(Form("output/%s_%s_energy_sim.txt", element->element_peak_1.c_str(), element->element_peak_2.c_str()));  // Open file containing the output data
  bool first_line = true;

  for (int ring = 0; ring < adc.rings; ring++) {
    canvas->cd(ring + 1);
    histogram_name = Form("cd_sim_%d", ring+1);
    histogram = (TH1F *)infile->Get(histogram_name.c_str());
    histogram->Draw();
    std::vector<double> centroid_peaks;
    for (int peak = 0; peak < element->peaks; peak++) { 
      min_bin = element->min_bin.at(peak*adc.rings + ring);
      max_bin = element->max_bin.at(peak*adc.rings + ring);
      fit_name = "fit_" + histogram_name;
      // Obtaining and specifying the start values for the fit
      sigma = 0.3 * (max_bin - min_bin);
      gauss_constant = histogram->Integral(histogram->FindBin(min_bin), histogram->FindBin(max_bin));
      mean = 0.5 * (min_bin + max_bin);
      parameter[0] = sigma;
      parameter[1] = gauss_constant; 
      parameter[2] = mean;
      centroid = FitPeak(histogram, fit_name, parameter, min_bin, max_bin);
      element->centroids.push_back(centroid);
      if (element->peaks == 2) { centroid_peaks.push_back(centroid); }
      // find x-axis range
      if (element->peaks == 2) {
        if (peak == 0) { min_range = min_bin; }
        if (peak == 1) { max_range = max_bin; }
      } else {
        if (ring == 0 || ring == 1) { min_range = 0; } 
        else { min_range = min_bin; }
        max_range = max_bin;
      }
    }
    histogram->SetAxisRange(0.95 * min_range, 1.05 * max_range, "X");
    histogram->SetStats(0);                        // Remove stats
    histogram->SetLabelSize(label_size, "xy");     // Label size for x- and y-axis
    histogram->SetTitleSize(label_size, "xy");     // Text  size for x- and y-axis
    histogram->GetYaxis()->SetTitleOffset(1.1);    // Move y-axis text a little closer
    histogram->SetTitle(Form("Ring %d", ring+1));  // Changing titles
    gStyle->SetTitleSize(label_size, "t");         // Title size
    gPad->SetLeftMargin(margin_size);
    gPad->SetRightMargin(margin_size);
    gPad->SetBottomMargin(margin_size);
    // Shorthand if-else statement: (condition) ? (if_true) : (if_false)
    (ring < 9) ? space = " " : space = "";
    // Write centroid to file
    if (element->peaks == 2) {
      if (first_line) {
        output_file << "Histogram " << std::setw(10) << element->element_peak_1
                    << std::setw(10) << element->element_peak_2 << std::endl;
        first_line = false;
      }
      output_file << histogram_name << ":" << space << std::setw(10) << centroid_peaks.at(0) 
                  << std::setw(10) << centroid_peaks.at(1) << std::endl;
    } else {
      if (first_line) {
        output_file << "Histogram " << std::setw(10) << element->element_peak_2 << std::endl;
        first_line = false;
      }
      output_file << histogram_name << ":" << space << std::setw(10) << centroid << std::endl;
    }
  }
  element->print_centroids();
  output_file.close();
  canvas->SaveAs(Form("../../Plots/fitting/%s_%s_sim.pdf", element->element_peak_1.c_str(), element->element_peak_2.c_str()));
  std::cout << "--- simulation fit of " << element->element_peak_2 << " ended ---" << std::endl;
}


void AQ4_fit(std::string setup_file, std::string detector_side, int ring_gate = 1, bool help_lines = false) {
  /*
      Fitting data sorted by AQ4Sort.
      Choose ("b")ack side or ("f")ront side of detector.
      Gating on ring is only for the back side.
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);

  std::cout << "Looking at " << element->element_peak_1 << " and " << element->element_peak_2 
            << " from file: " << element->AQ4_infile << std::endl;

  std::cout << "--- Fitting data started ---" << std::endl;

  // For plotting
  TFile *infile = new TFile(element->AQ4_infile.c_str(), "UPDATE");
  TCanvas *canvas[adc.quadrants];
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;

  // For fitting
  std::string detector_side_name;
  std::vector<double> centroid(2);
  double mean_guess = 0;
  double min_bin = 0;
  double max_bin = 0;
  double min_range = 0;
  double max_range = 0;
  int row = 0;
  int pos = 0;
  int ring_number = 0;
  std::string gate;
  std::string space;
  std::string extra_space;
  std::string extension;
  
  if (detector_side == "b") {
    detector_side_name = "back";
    element->read_guesses(element->back_ch_guess);
    gate = "_f" + std::to_string(ring_gate);
  } else if (detector_side == "f") {
    detector_side_name = "front";
    element->read_guesses(element->front_ch_guess);
  } else {
    std::cout << "Invalid option. Choose detector side (b)ack or (f)ront." << std::endl;
  }
  if ( !(ring_gate >= 1 && ring_gate <= 16) ) {
    std::cout << "Invalid option. Choose between ring 1-16." << std::endl;
  }
  std::ofstream output_file(Form("output/%s_%s_%s%s_ch.txt", element->element_peak_1.c_str(), element->element_peak_2.c_str(), detector_side_name.c_str(), gate.c_str()));  // Open file containing the output data
  bool first_line = true;
  
  // help lines
  TLine *line_min;
  TLine *line_mean;
  TLine *line_max;
  
  for (int quadrant = 0; quadrant < adc.quadrants; quadrant++) {
    canvas_name = Form("Quadrant %d, %s detector", quadrant+1, detector_side_name.c_str());
    canvas[quadrant] = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
    // Shorthand if-else statement: (condition) ? (if_true) : (if_false)
    (detector_side == "b") ? canvas[quadrant]->Divide(4, 3) : canvas[quadrant]->Divide(4, 4);
    for (int ring = 0; ring < adc.rings; ring++) {   
      if (detector_side == "b" && ring >= adc.strips) { break; }
      if (detector_side == "b") { 
        ring_number = ring_gate-1;
        extension = "_b" + std::to_string(ring+1);
      } else if (detector_side == "f") { 
        ring_number = ring;
      }
      canvas[quadrant]->cd(ring+1);
      histogram_name = Form("%sE_Q%d_f%d%s", detector_side.c_str(), quadrant+1, ring_number+1, extension.c_str());
      histogram = (TH1F *)infile->Get(histogram_name.c_str());
      histogram->Draw();
      histogram->GetYaxis()->SetLabelSize(0.05);
      histogram->GetXaxis()->SetLabelSize(0.05);
      std::vector<double> centroid_peaks;
      for (int peak = 0; peak < element->peaks; peak++) {
        if (peak == 0 ) { row = 0; }
        else if (peak == 1 ) { row = adc.quadrants*adc.rings; }
        pos = quadrant*adc.rings + ring_number;
        min_bin = element->min_bin.at(row + pos);
        max_bin = element->max_bin.at(row + pos);
        mean_guess = element->mean_guess.at(row + pos);
        centroid = Fit_4d_polynomial(histogram, min_bin, max_bin);

        if (help_lines) {
          line_min = new TLine(min_bin, 0, min_bin, centroid.at(1)*1.5);
          line_min->SetLineColor(kGreen);
          line_min->Draw("SAME");
          line_mean = new TLine(mean_guess, 0, mean_guess, centroid.at(1)*1.5);
          line_mean->SetLineColor(kGreen);
          line_mean->Draw("SAME");
          line_max = new TLine(max_bin, 0, max_bin, centroid.at(1)*1.5);
          line_max->SetLineColor(kGreen);
          line_max->Draw("SAME"); 
        }
        element->centroids.push_back(centroid.at(0));
        if (element->peaks == 2) { centroid_peaks.push_back(centroid.at(0)); }
        // find x-axis range
        if (element->peaks == 2) {
          if (peak == 0) { min_range = min_bin; }
          if (peak == 1) { max_range = max_bin; }
        } else {
          min_range = min_bin;
          max_range = max_bin;
        }
        
        std::cout << "Q = " << quadrant+1    << "   R = "  << std::setw(2) << ring+1 
                            << std::setw(7)  << min_bin    << std::setw(7) << max_bin 
                            << std::setw(10) << mean_guess 
                            << std::setw(10) << centroid.at(0) << std::endl;
      }
      if (!(min_bin == 0 && max_bin == 0)) {
        histogram->SetAxisRange(0.95 * min_range, 1.05 * max_range, "X");
      }
      // Shorthand if-else statement: (condition) ? (if_true) : (if_false)
      (ring < 9) ? space = " " : space = "";
      // Write centroid to file
      if (element->peaks == 2) {
        if (first_line) {
          // Shorthand if-else statement: (condition) ? (if_true) : (if_false)
          (detector_side == "b") ? extra_space = "   " : extra_space = "";
          (ring_gate > 9) ? extra_space += " " : extra_space += "";
          output_file << "Histogram " << extra_space << std::setw(10) << element->element_peak_1
                      << std::setw(10) << element->element_peak_2 << std::endl;
          first_line = false;
        }
        output_file << histogram_name << ":" << space << std::setw(10) << centroid_peaks.at(0) 
                    << std::setw(10) << centroid_peaks.at(1) << std::endl;
      } else {
        if (first_line) {
          // Shorthand if-else statement: (condition) ? (if_true) : (if_false)
          (detector_side == "b") ? extra_space = "   " : extra_space = "";
          output_file << "Histogram " << extra_space << std::setw(10) << element->element_peak_2 << std::endl;
          first_line = false;
        }
        output_file << histogram_name << ":" << space << std::setw(10) << centroid.at(0) << std::endl;
      }   
    }
    canvas[quadrant]->SaveAs(Form("../../Plots/fitting/%s_%s_Q%d_%s%s_fit.pdf", element->element_peak_1.c_str(), element->element_peak_2.c_str(), quadrant+1, detector_side_name.c_str(), gate.c_str()));
  }
  element->print_centroids();
  output_file.close();
  std::cout << "--- Fitting data ended ---" << std::endl;
}


void AQ4_single_fit(std::string setup_file, std::string detector_side, int quadrant, 
                    int ring_gate = 1, int strip_gate = 1, bool help_lines = false) {
  /*
      Fitting data sorted by AQ4Sort.
      Choose ("b")ack side or ("f")ront side of detector.
      Gating on ring is only for the back side.

      Choices: ("f")ront or ("b")ack side, quadrant 1-4, ring gate 1-16, back strip 1-12.
      Help lines plot lines for the minimum and maximum range, in addition to the centroid.
      Back strip only valid for looking at the back side of the detector.
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);

  std::cout << "Looking at " << element->element_peak_1 << " and " << element->element_peak_2 
            << " from file: " << element->AQ4_infile << std::endl;

  std::cout << "--- Fitting data started ---" << std::endl;

  // For plotting
  TFile *infile = new TFile(element->AQ4_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;

  // For fitting
  std::string detector_side_name;
  std::vector<double> centroid(2);
  double mean_guess = 0;
  double min_bin = 0;
  double max_bin = 0;
  double min_range = 0;
  double max_range = 0;
  int row = 0;
  int pos = 0;
  int ring_number = 0;
  std::string gate = "_f" + std::to_string(ring_gate);;
  std::string space;

  if (detector_side == "b") {
    detector_side_name = "back";
    element->read_guesses(element->back_ch_guess);
  } else if (detector_side == "f") {
    detector_side_name = "front";
    element->read_guesses(element->front_ch_guess);
  } else {
    std::cout << "Invalid option. Choose detector side (b)ack or (f)ront." << std::endl;
  }
  if ( !(ring_gate >= 1 && ring_gate <= 16) ) {
    std::cout << "Invalid option. Choose between ring 1-16." << std::endl;
  }

  std::string extension;
  if (detector_side == "b") {
    extension = "_b" + std::to_string(strip_gate);
  } 
  
  // help lines
  TLine *line_min;
  TLine *line_mean;
  TLine *line_max;

  canvas_name = Form("Quadrant %d, %s detector", quadrant, detector_side_name.c_str());
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);
  histogram_name = Form("%sE_Q%d_f%d%s", detector_side.c_str(), quadrant, ring_gate, extension.c_str());
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();
  histogram->GetYaxis()->SetLabelSize(0.05);
  histogram->GetXaxis()->SetLabelSize(0.05);
  std::vector<double> centroid_peaks;
  for (int peak = 0; peak < element->peaks; peak++) {
    if (peak == 0 ) { row = 0; }
    else if (peak == 1 ) { row = adc.quadrants*adc.rings; }
    pos = (quadrant-1)*adc.rings + ring_gate-1;
    min_bin = element->min_bin.at(row + pos);
    max_bin = element->max_bin.at(row + pos);
    mean_guess = element->mean_guess.at(row + pos);
    centroid = Fit_4d_polynomial(histogram, min_bin, max_bin);

    if (help_lines) {
      line_min = new TLine(min_bin, 0, min_bin, centroid.at(1)*1.5);
      line_min->SetLineColor(kGreen);
      line_min->Draw("SAME");
      line_mean = new TLine(mean_guess, 0, mean_guess, centroid.at(1)*1.5);
      line_mean->SetLineColor(kGreen);
      line_mean->Draw("SAME");
      line_max = new TLine(max_bin, 0, max_bin, centroid.at(1)*1.5);
      line_max->SetLineColor(kGreen);
      line_max->Draw("SAME"); 
    }
    element->centroids.push_back(centroid.at(0));
    if (element->peaks == 2) { centroid_peaks.push_back(centroid.at(0)); }
    // find x-axis range
    if (element->peaks == 2) {
      if (peak == 0) { min_range = min_bin; }
      if (peak == 1) { max_range = max_bin; }
    } else {
      min_range = min_bin;
      max_range = max_bin;
    }
    
    std::cout << "Q = "   << quadrant      
              << ", R = " << std::setw(2) << ring_gate 
              << std::setw(7)  << ", min = "      << min_bin    
              << std::setw(7)  << ", max = "      << max_bin
              << std::setw(10) << ", mean = "     << mean_guess 
              << std::setw(10) << ", centroid = " << centroid.at(0) << std::endl;
    
    if (!(min_bin == 0 && max_bin == 0)) {
      histogram->SetAxisRange(0.95 * min_range, 1.05 * max_range, "X");
    }

  canvas->SaveAs(Form("../../Plots/fitting/%s_%s_%s_fit.pdf", element->element_peak_1.c_str(), element->element_peak_2.c_str(), histogram_name.c_str()));
  }
  element->print_centroids();
  std::cout << "--- Fitting data ended ---" << std::endl;
}


void AQ4_back_strip_fit(std::string setup_file, int quadrant, int ring_gate, 
                        int back_strip, double min_bin, double max_bin, 
                        double centroid_guess) {
  /*
      Fitting data sorted by AQ4Sort.
      Choose the quadrant, front ring to gate on, the back strip you want to look at
      and the range + guess of the centroid you want to fit.
      Choices: Quadrant 1-4, ring gate 1-16, back strip 1-12.
  */
  ADC adc;
  Element *element = new Element();
  element->read_setup_file(setup_file);

  std::cout << "Looking at " << element->element_peak_1 << " and " << element->element_peak_2 
            << " from file: " << element->AQ4_infile << std::endl;

  std::cout << "--- Manual fitting data started ---" << std::endl;

  if ( !(quadrant >= 1 && quadrant <= 4) ) {
    std::cout << "Invalid option. Choose between quadrant 1-4." << std::endl;
  }
  if ( !(ring_gate >= 1 && ring_gate <= 16) ) {
    std::cout << "Invalid option. Choose between front ring 1-16." << std::endl;
  }
  if ( !(back_strip >= 1 && back_strip <= 12) ) {
    std::cout << "Invalid option. Choose between back strip 1-12." << std::endl;
  }

  // For plotting
  TFile *infile = new TFile(element->AQ4_infile.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;

  // For fitting
  std::string fit_name;
  double width_guess = element->back_width_guess;
  double constant_guess = element->back_constant_guess;
  double parameter[] = {width_guess, constant_guess, centroid_guess};
  double centroid = 0;
  double min_range = 0;
  double max_range = 0;

  // ---  REMOVE THIS
  //element->print_parameters(element->min_bin);
  //element->print_parameters(element->max_bin);
  //element->print_parameters(element->mean_guess);
  TLine *line_min;
  TLine *line_mean;
  TLine *line_max;
  // --- END

  canvas_name = Form("Quadrant %d, front ring %d, back strip %d", quadrant, ring_gate, back_strip);
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);

  histogram_name = Form("bE_Q%d_f%d_b%d", quadrant, ring_gate, back_strip);
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->Draw();
  histogram->GetYaxis()->SetLabelSize(0.06);
  histogram->GetXaxis()->SetLabelSize(0.06);

  fit_name = "fit_" + histogram_name;
  centroid = FitPeak(histogram, fit_name, parameter, min_bin, max_bin);

  // --- REMOVE THIS
  //std::cout << "min = " << min_bin << std::endl;
  //std::cout << "max = " << max_bin << std::endl;
  //std::cout << "mean = " << mean_guess << std::endl;
  line_min = new TLine(min_bin, 0, min_bin, histogram->GetBinContent(centroid)*1.5);
  line_min->SetLineColor(kGreen);
  //line_min->SetLineStyle(kDashed);
  line_min->Draw("SAME");
  line_mean = new TLine(centroid_guess, 0, centroid_guess, histogram->GetBinContent(centroid)*1.5);
  line_mean->SetLineColor(kGreen);
  //line_mean->SetLineStyle(kDashed);
  line_mean->Draw("SAME");
  line_max = new TLine(max_bin, 0, max_bin, histogram->GetBinContent(centroid)*1.5);
  line_max->SetLineColor(kGreen);
  //line_max->SetLineStyle(kDashed);
  line_max->Draw("SAME");  
  // --- END

  //histogram->SetAxisRange(0.6 * min_bin, 1.4 * max_bin, "X");

  std::cout << std::setw(8)  << "Quadrant"   << std::setw(12) << "Front_ring" 
            << std::setw(12) << "Back_strip" << std::setw(9) << "min_bin" 
            << std::setw(9)  << "max_bin"    << std::setw(12) << "mean_guess"
            << std::setw(10) << "centroid"   << std::endl;
  
  std::cout << std::setw(8)  << quadrant     << std::setw(12) << ring_gate
            << std::setw(12) << back_strip   << std::setw(9)  << min_bin
            << std::setw(9)  << max_bin      << std::setw(12) << centroid_guess
            << std::setw(10) << centroid     << std::endl;

  canvas->SaveAs(Form("../../Plots/fitting/Manual_Pb_Sm_back_Q%d_f%d_b%d_fit.pdf", quadrant, ring_gate, back_strip));
  
  std::cout << "--- Manual fitting data ended ---" << std::endl;
}



void test() {
  std::string infile_ = "../../Sorted_data/Sm_user-AQ4Sort-2019-03-23.root";
  TFile *infile = new TFile(infile_.c_str(), "UPDATE");
  TCanvas *canvas = nullptr;
  std::string canvas_name;
  TH1F *histogram = nullptr;
  std::string histogram_name;

  std::vector<double> centroid(2);
  std::vector<double> centroid2(2);

  int start_bin = 395;
  int end_bin = 660;

  int start_bin2 = 1000;
  int end_bin2 = 1300;

  canvas_name = Form("Test");
  canvas = new TCanvas(canvas_name.c_str(), canvas_name.c_str(), 1280, 800);

  histogram_name = Form("fE_Q1_f1");
  histogram = (TH1F *)infile->Get(histogram_name.c_str());
  histogram->SetAxisRange(315, 1500, "X");
  histogram->Draw();

  centroid = Fit_4d_polynomial(histogram, start_bin, end_bin);
  centroid2 = Fit_4d_polynomial(histogram, start_bin2, end_bin2);

  //TF1 *fit_func = new TF1(fit_name.c_str(), "pol4", start_bin, end_bin, 5);
  //fit_func->SetParameters(parameter[0], parameter[1], parameter[2], parameter[3], parameter[4]);
  ////fit_func->SetRange(start_bin, end_bin);
  //fit_func->SetLineColor(kRed);
  //fit_func->SetLineStyle(kDashed);
  //fit_func->Draw("SAME");

  std::cout << "Did it work?" << std::endl;
  std::cout << "x-val = " << centroid.at(0) << ", y-val = " << centroid.at(1) << std::endl;

  std::cout << "Did it work for 2?" << std::endl;
  std::cout << "x-val = " << centroid2.at(0) << ", y-val = " << centroid2.at(1) << std::endl;

}
