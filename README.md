# Basic DAQ Analysis Code #

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

**Waveform Plot**

You can make a waveform plot with the output of `analysis` by running
the script `waveform.py`. See the source for arguments setting
input/output file names and channel/entry to be plotted.

