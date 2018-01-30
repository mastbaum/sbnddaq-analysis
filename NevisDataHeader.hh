#ifndef _sbnddaq_analysis_NevisDataHeader
#define _sbnddaq_analysis_NevisDataHeader

#include <cstdint>

// TODO: This should probably have a better name

namespace sbnddaq {
  class NevisTPCHeader;
}

class NevisDataHeader {
public:
  uint32_t event_number;
  uint32_t frame_number;
  uint32_t checksum;
  uint32_t adc_word_count;
  uint32_t trig_frame_number;
  double frame_time;
  double trig_frame_time;

  NevisDataHeader(const sbnddaq::NevisTPCHeader *raw_header, double frame_to_dt);
  NevisDataHeader() {}

};


#endif
