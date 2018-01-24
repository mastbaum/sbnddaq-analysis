//some standard C++ includes
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

//some ROOT includes
#include "TInterpreter.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
#include "TStyle.h"
#include "TSystem.h"

//"art" includes (canvas, and gallery)
#include "canvas/Utilities/InputTag.h"
#include "gallery/Event.h"
#include "gallery/ValidHandle.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindOne.h"

#include "sbnddaq-datatypes/Overlays/NevisTPCFragment.hh"
#include "sbnddaq-datatypes/NevisTPC/NevisTPCTypes.hh"
#include "artdaq-core/Data/Fragment.hh"

#include "NevisDataHeader.hh"
//This way you can be lazy
using namespace art;

int main(int argv, char** argc) {

  art::InputTag daqTag("daq","NEVISTPC");

  // TODO: make these configurable at command line
  double frame_to_dt = 1.6e-3; // units of seconds
  TFile* output = new TFile("output.root","RECREATE");
  bool save_histograms = false;

  output->cd();

  TH1D *h_event_numbers;
  TH1D *h_frame_numbers;
  TH1D *h_checksums;
  TH1D *h_adc_word_counts;
  TH1D *h_trig_frame_numbers;

  if (save_histograms) {
    h_event_numbers = new TH1D("event_numbers", "", 100, -0.5, 99.5);
    h_frame_numbers = new TH1D("frame_numbers", "", 100, -0.5, 99.5);
    h_checksums = new TH1D("checksums", "", 100, 2455. - 50.5, 2455. + 49.5);
    h_adc_word_counts = new TH1D("adc_word_counts", "", 100, -0.5, 99.5);
    h_trig_frame_numbers = new TH1D("trig_frame_numbers", "", 100, -0.5, 99.5);
  }

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
 
  //We have passed the input file as an argument to the function 
  std::vector<std::string> filename;
  for (int i = 1; i < argv; ++i) { 
    std::cout << "FILE : " << argc[i] << std::endl; 
    filename.push_back(std::string(argc[i]));
  }

  for (gallery::Event ev(filename) ; !ev.atEnd(); ev.next()) {

    auto const& daq_handle = ev.getValidHandle<std::vector<artdaq::Fragment>>(daqTag);

    for(auto const& rawfrag : *daq_handle){
      sbnddaq::NevisTPCFragment fragment(rawfrag);
      auto fragment_header = fragment.header(); 

      const sbnddaq::NevisTPCFragmentMetadata* meta = fragment.metadata();
      const uint32_t nsamples = meta->NChannels() * meta->SamplesPerChannel();

      const sbnddaq::NevisTPC_ADC_t* data = fragment.data();
      for (size_t i=0; i<nsamples; i++) {
        std::cout << std::hex << data[i] << " ";
      }
      std::cout << std::endl;

      data_header = NevisDataHeader(fragment_header, frame_to_dt); 
      output->cd();
      header.Fill();

      if (save_histograms) {
	h_event_numbers->Fill((double)data_header.event_number);
	std::cout << "EVENT NUMBER: "  << data_header.event_number << std::endl;
	
	h_frame_numbers->Fill((double)data_header.frame_number);
	std::cout << "FRAME NUMBER: "  << data_header.frame_number << std::endl;
	
	h_checksums->Fill((double)data_header.checksum);
	std::cout << "CHECKSUM: "  << data_header.checksum << std::endl;
	
	h_adc_word_counts->Fill((double)data_header.adc_word_count);
	std::cout << "ADC WORD COUNT: "  << data_header.adc_word_count << std::endl;
	
	h_trig_frame_numbers->Fill((double)data_header.trig_frame_number); 
	std::cout << "TRIG FRAME NUMBER: "  << data_header.trig_frame_number << std::endl;
      }
    }// Iterate through fragments
  }// Iterate through events


  output->cd();
  if (save_histograms) {
    h_event_numbers->Write();
    h_frame_numbers->Write();
    h_checksums->Write();
    h_adc_word_counts->Write();
    h_trig_frame_numbers->Write();
  }
  header.Write();

  output->Close();
  return 0;
}
