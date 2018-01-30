#Makefile for gallery c++ programs.
#Note, being all-incllusive here: you can cut out libraries/includes you do not need
#you can also change the flags if you want too (Werror, pedantic, etc.)

CPPFLAGS=-I $(BOOST_INC) \
         -I $(CANVAS_INC) \
         -I $(CETLIB_INC) \
         -I $(CETLIB_EXCEPT_INC) \
         -I $(FHICLCPP_INC) \
         -I $(GALLERY_INC) \
         -I $(LARCOREOBJ_INC) \
         -I $(LARDATAOBJ_INC) \
         -I $(NUSIMDATA_INC) \
         -I $(ROOT_INC) \
	 -I $(TRACE_INC) \
	 -I $(ARTDAQ_CORE_INC) \
	 -I $(SBNDDAQ_DATATYPES_INC) \
	 -I $(HIREDIS_INC)

CXXFLAGS=-std=c++14 -Wall -Werror -pedantic
CXX=g++
LDFLAGS=$$(root-config --libs) \
        -L $(CANVAS_LIB) -l canvas \
        -L $(CETLIB_LIB) -l cetlib \
        -L $(CETLIB_EXCEPT_LIB) -l cetlib_except \
        -L $(GALLERY_LIB) -l gallery \
        -L $(NUSIMDATA_LIB) -l nusimdata_SimulationBase \
        -L $(LARCOREOBJ_LIB) -l larcoreobj_SummaryData \
        -L $(LARDATAOBJ_LIB) -l lardataobj_RecoBase -l lardataobj_MCBase -l lardataobj_RawData \
                             -l lardataobj_OpticalDetectorData -l lardataobj_AnalysisBase \
	-L $(SBNDDAQ_DATATYPES_LIB) -l sbnddaq-datatypes_Overlays -l sbnddaq-datatypes_NevisTPC \
	-L $(HIREDIS_LIB) -lhiredis

EXEC=analysis

lib: libNevisTPCEvent.so

NevisTPCEvent.cxx: NevisDataHeader.hh WaveformData.hh linkdef.h
	rootcint -f $@ -c $(CXXFLAGS) -p $^

libNevisTPCEvent.so: NevisTPCEvent.cxx
	@echo Building library
	$(CXX) -shared -fPIC -o$@ `root-config --ldflags` $(CXXFLAGS) -I$(ROOTSYS)/include $^

$(EXEC): Main.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $< NevisDataHeader.cxx

all: analysis lib

clean:
	$(RM) analysis NevisTPCEvent.cxx libNevisTPCEvent.so

