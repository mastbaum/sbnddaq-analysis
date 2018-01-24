# Basic DAQ Analysis Code #

Right now the code just makes some TTree's with a few of the variables
defined in the NevisTPCHeader, with the option to also save histograms
of those variables.

**To Use**

First, you need to change `SBNDDAQ_DATATYPES_LOCATION` in `setup.sh` to
the location of your local repository containing `sbnddaq-datatypes` and
`artdaq`.

Then, running `setup.sh` will setup the required libraries, and then calling
`make` will build the executable. 

To run the program, call `./analysis [input_files]` where
`[input_files]` is a list of space separated root files from the output
of one of the drivers from `sbnddaq-readout` (e.g. `test_driver`). 

Other configuration options--the name of the output root file, whether
to save histograms in the output, and the conversion from frame number
to time--are all currently set in `Main.cc`, but they will soon be
configurable as input arguments.
