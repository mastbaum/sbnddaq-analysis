#ifndef _sbnddaq_analysis_NevisDataHeader
#define _sbnddaq_analysis_NevisDataHeader

#include "sbnddaq-datatypes/Overlays/NevisTPCFragment.hh"

// TODO: This should probably have a better name

struct NevisDataHeader {
  public:
  uint32_t event_number;
  uint32_t frame_number;
  uint32_t checksum;
  uint32_t adc_word_count;
  uint32_t trig_frame_number;
  double frame_time;
  double trig_frame_time;

  NevisDataHeader(const sbnddaq::NevisTPCHeader *raw_header, double frame_to_dt) {
    event_number = raw_header->getEventNum();
    frame_number = raw_header->getFrameNum();
    checksum = raw_header->getChecksum();
    adc_word_count = raw_header->getADCWordCount();
    trig_frame_number = raw_header->getTrigFrame();

    frame_time = frame_number * frame_to_dt;
    trig_frame_time = trig_frame_number * frame_to_dt;
  }
  NevisDataHeader() {}

};


#endif
