# ShefLArRig
The repository contains the codes used for Sheffield LArRig data analysis.

This the directory for the analysis code for the Purity study of the liquid argon rig. This is discribed below.
Make sure you in a runx directory to run this code.
For questions email: aazglam1@sheffield.ac.uk
##########################################################################################
Here is a description of the code in the directory:
CreateAvg.sh (run with sh CreateAvg.sh): This creates the root files containing the waveform and header information and averages and performs a fits. Each step only occurs if the file required exist and the file has not already been created.
RUN_DAQ.sh (run with sh RUN_DAQ.sh): This runs the daq acquisition with the config described in params.txt. It repeatly up to a certain time. One can add there email addresss to get updates fromt the DAQ progress using email_DAQ.py. Individual runs can be run via $HOME/AgMD2_GPM/runGPM
email_DAQ.py: Used in RUN_DAQ.sh to send a email updating you on the status of the DAQ.
waveform.cc (run with ./waveform 0 1 2 3 4 5 6 7 8 (Set each channel to 0 which you are not using (6 is the raw pmt)): Creates a root file contain the peak time, peak height and the waveforms for each event. To build use: "make clean; make"
params.txt: Holds the config file for the DAQ.
dataAveraging.cc (run with ./dataAveraging waveforms_in.root fileout.root): Averages over all the events in the file and provides multiple fits. See Lohans Documentation for more detail.
##########################################################################################
This is the directory that holds the data for the liquid argon PMT study.
This holds the data for the R&D runs that were performed July-August 2021. The PMT was ran at 1600V for all the runs.
The measurements during purification. For more detail on the Rig design please see: https://arxiv.org/abs/1602.01884 . 
Below is a quick discription of the experiment and then the configurations of the runs.
The Liquid argon rig was filled to ~90% full of argon, maintain a pressure of ~20 psig. The argon is held ina 20 litre vacuum jacket which holds a Hamaatsu R11065 PMT: http://lartpc-docdb.fnal.gov/cgi-bin/RetrieveFile?docid=441&filename=R11065_data_sheet_0903-1.pdf&version=1. When purification system is not running the argon is allowed to vent out at ~2-2.5% per hour. 
It takes ~30-40 hours to completely evaporate. 
When the purification system is running the argon is past through a 3X molecular sieve 8-14 mesh beads and 3 kg of GetterMax-133 copper catalyst 3 × 3 mm tablets.
The argon then condensors through a liquid nitrogen condensor and falls back down into the dewar.
The DAQ code that can be found in ./RunDir directory uses code created for the keysight DAQ for the GPM experiment.
