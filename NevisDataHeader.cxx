#include "NevisDataHeader.hh"
#include "sbnddaq-datatypes/Overlays/NevisTPCFragment.hh"

NevisDataHeader::NevisDataHeader(const sbnddaq::NevisTPCHeader *raw_header, double frame_to_dt) {
  event_number = raw_header->getEventNum();
  frame_number = raw_header->getFrameNum();
  checksum = raw_header->getChecksum();
  adc_word_count = raw_header->getADCWordCount();
  trig_frame_number = raw_header->getTrigFrame();
  frame_time = frame_number * frame_to_dt;
  trig_frame_time = trig_frame_number * frame_to_dt;
}

