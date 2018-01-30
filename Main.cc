//some standard C++ includes
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <numeric>
#include <getopt.h>

//some ROOT includes
#include "TInterpreter.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TGraph.h"
#include "TFFTReal.h"

//"art" includes (canvas, and gallery)
#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindOne.h"

#include "sbnddaq-datatypes/Overlays/NevisTPCFragment.hh"
#include "sbnddaq-datatypes/NevisTPC/NevisTPCTypes.hh"
#include "sbnddaq-datatypes/NevisTPC/NevisTPCUtilities.hh"

#include "artdaq-core/Data/Fragment.hh"

#include "NevisDataHeader.hh"
#include "WaveformData.hh"
//This way you can be lazy
using namespace art;

int main(int argv, char** argc) {
  if (argv == 0) {
    std::cout << "Pass in some imput files" << std::endl;
    return 1;
  }
  //We have passed the input file as an argument to the function 
  std::vector<std::string> filename;
  for (int i = 1; i < argv; ++i) { 
    std::cout << "FILE : " << argc[i] << std::endl; 
    filename.push_back(std::string(argc[i]));
  }

  art::InputTag daqTag("daq","NEVISTPC");

  // TODO: make these configurable at command line
  double frame_to_dt = 1.6e-3; // units of seconds
  TFile* output = new TFile("output.root","RECREATE");
  bool save_waveforms = true;
  bool verbose = true;
  int n_events = 10;
  unsigned n_baseline_samples = 20;

  // TODO: how to detect this?
  // currently only the first 16 channels have anything interesting
  size_t n_channels = 16;

  output->cd();

  TTree header("nevis_header", "nevis_header");
  NevisDataHeader data_header;
  TBranch *b_event_number = header.Branch("event_number", &data_header.event_number);
  TBranch *b_frame_number = header.Branch("frame_number", &data_header.frame_number);
  TBranch *b_checksum = header.Branch("checksum", &data_header.checksum);
  TBranch *b_adc_word_count = header.Branch("adc_word_count", &data_header.adc_word_count);
  TBranch *b_trig_frame_number = header.Branch("trig_frame_number", &data_header.trig_frame_number);
  TBranch *b_frame_time = header.Branch("frame_time", &data_header.frame_time);
  TBranch *b_trig_frame_time = header.Branch("trig_frame_time", &data_header.trig_frame_time);

  (void)b_event_number;
  (void)b_frame_number;
  (void)b_checksum;
  (void)b_adc_word_count;
  (void)b_trig_frame_number;
  (void)b_frame_time;
  (void)b_trig_frame_time;

  TTree t_raw_adc_digits("raw_adc_digits", "raw_adc_digits");

  // NOTE: The way things are currently set up, if you push to per_channel_adc_digits 
  // after this loop, the code will SEGFAULT. So don't do that.
  std::vector<std::vector<int>*> per_channel_adc_digits(n_channels);
  for (size_t i = 0; i < n_channels; i++) {
    per_channel_adc_digits.push_back(new std::vector<int>);
    char branch_name[30];
    sprintf(branch_name, "channel_%lu_adc_words", i);
    (void) t_raw_adc_digits.Branch(branch_name, &per_channel_adc_digits[i]);
  }

  TTree t_waveform_data("waveform_data", "waveform_data");
  std::vector<WaveformData> per_channel_waveform_data(n_channels);
  for (size_t i = 0; i < n_channels; i++) {
    per_channel_waveform_data.push_back(WaveformData());
    char branch_name[30];
    sprintf(branch_name, "channel_%lu_baseline", i);
    (void) t_waveform_data.Branch(branch_name, &per_channel_waveform_data[i].baseline);
    sprintf(branch_name, "channel_%lu_peak", i);
    (void) t_waveform_data.Branch(branch_name, &per_channel_waveform_data[i].peak);
  }

  int event_ind = 0;
  for (gallery::Event ev(filename) ; !ev.atEnd() && event_ind < n_events; ev.next()) {
    event_ind ++;

    auto const& daq_handle = ev.getValidHandle<std::vector<artdaq::Fragment>>(daqTag);

    // NOTE: Currently the code assumes there is only one frament per event. 
    // It does not try to do any maching of fragment id -> channel id.
    // This will have to be implemented later.
    for(auto const& rawfrag : *daq_handle){
      sbnddaq::NevisTPCFragment fragment(rawfrag);
      auto fragment_header = fragment.header(); 
      
      data_header = NevisDataHeader(fragment_header, frame_to_dt); 
      output->cd();

      if (verbose) {
	std::cout << "EVENT NUMBER: "  << data_header.event_number << std::endl;
	std::cout << "FRAME NUMBER: "  << data_header.frame_number << std::endl;
	std::cout << "CHECKSUM: "  << data_header.checksum << std::endl;
	std::cout << "ADC WORD COUNT: "  << data_header.adc_word_count << std::endl;
	std::cout << "TRIG FRAME NUMBER: "  << data_header.trig_frame_number << std::endl;
      }

      std::unordered_map<uint16_t,sbnddaq::NevisTPC_Data_t> waveform_map;
      size_t n_waveforms = fragment.decode_data(waveform_map);
      std::cout << "Decoded data. Found " << n_waveforms << " waveforms." << std::endl;
      for (auto waveform: waveform_map) {
        if (save_waveforms && waveform.first < n_channels) {
          unsigned peak = 0;
          std::vector<double> waveform_samples;

          for (auto adc: waveform.second) {
            if (adc > peak) peak = adc;

            waveform_samples.push_back((double) adc);
            per_channel_adc_digits[waveform.first]->push_back((int) adc);
          }
          /*
          // calculate FFT
          TFFTReal fft(waveform_samples.size());
          fft.SetPoints(&waveform_samples[0]);
          fft.Transform();
          // for now make separate array to store output, maybe take out later in optimization?
          std::vector<double> dft_waveform(waveform_samples.size());
          fft.GetPoints(&dft_waveform[0]);
          // TODO: store
          */

          // Baseline calculation assumes baseline is constant and that the first
          // `n_baseline_samples` of the adc values represent a baseline
          per_channel_waveform_data[waveform.first].baseline = 
            std::accumulate(waveform_samples.begin(), waveform_samples.begin() + n_baseline_samples, 0.0) / n_baseline_samples;

          per_channel_waveform_data[waveform.first].peak = peak;
        }
      }  // iterate over fragments
      t_waveform_data.Fill();
      t_raw_adc_digits.Fill();
      header.Fill();

      // clear out adc count containers for next iter
      for (unsigned i = 0; i < per_channel_adc_digits.size(); i++) {
        per_channel_adc_digits[i]->clear();
      }
    }// Iterate through fragments
  }// Iterate through events

  std::cout << "Saving..." << std::endl;
  output->cd();
  header.Write();
  t_raw_adc_digits.Write();
  t_waveform_data.Write();

  output->Close();
  return 0;
}
