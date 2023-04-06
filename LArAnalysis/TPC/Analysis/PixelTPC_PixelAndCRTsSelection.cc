//**************************************************************************//
//**************************************************************************\\
//*******       Title:       PixelTPC_PixelAndCRTsSelection.cc       *******//
//*******                                                            *******\\
//*******       Author:       Ala Zglam                              *******//
//*******       Email:        aazglam1@sheffield.ac.uk               *******\\
//*******                     ala.zeglam10@gmail.com                 *******//
//*******                                                            *******\\
//*******       Description: A Code look at timestamp of CRTs and    *******//
//*******                    compare it to timestamp of the events   *******\\
//*******                    from the pixel. After that write the    *******//
//*******                    CRTs and Pixels evetns to seprate root  *******\\
//*******                    trees.                                  *******//
//*******                                                            *******\\
//*******       Last Updated: 26 July 2022                           *******//
//**************************************************************************\\
//**************************************************************************//

                             //#############################
                             //########   Note !!!  ########
                             //#############################
//############################################################################################################################
//#######                              To run this file please use something similar to this:                          #######
//####### $LArANAEXEDir/PixelTPC_PixelAndCRTsSelection -p PixelTPC_20220720_172733.root -c LArPMT20220720_171453.root  #######
//############################################################################################################################
//


//Decoder Includes For Pixels                                                                                                                        
#include "../DAQDecoder/Event.h"
#include "../DAQDecoder/ChannelInfo.h"
//Decoder Includes For CRTs
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/Event.h"
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/ChannelInfo.h"

//Root Includes    
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TSystem.h"

//C++ Includes 
#include <fstream> 
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <getopt.h>

int PixelAndCRTsSelection(std::vector<std::string>& pixelfiles, std::vector<std::string>& crtsfiles, int process_events);
int CheckIfFilesRoot(std::string& inputFilesTest);
void ROOTtoTXTfiles(std::vector<std::string>& txtFilename, std::vector<std::string>& rootFilesOutput);

int main(int argc, char* argv[]){

  std::vector<std::string> pixelinputfiles;
  std::vector<std::string> crtsinputfiles;
  //Generate Dictionary for crts 
  //gInterpreter->GenerateDictionary("","vector");

  int process_events = -1; 
  int current_eventnum = 0;

  //Get the options
  static struct option long_options[] = {
    {"pixelinputfile", required_argument, 0, 'p' },
    {"crtsinputfile",  required_argument, 0, 'c' },
    {"process_events", no_argument,       0, 'n' },
    {0,                0,                 0,  0  }
  };

  int opt = 0;
  int c = 0;
  while ((opt = getopt_long_only(argc, argv,"pcn:", long_options, &c )) != -1){
    switch (opt){
    case 'p' : 
      pixelinputfiles.push_back((argv[optind])); //pixelinputfiles.push_back(optarg);
      break;
    case 'c' : 
      crtsinputfiles.push_back((argv[optind])); //crtsinputfiles.push_back(optarg);
      break;
    case 'n' : 
      process_events = std::stoi(optarg);
      break;
    default: 
      std::cout << "Usage:  -c LArPMT.......root and -p PixelTPC.......root. -n for the number of events to be analysed. " << std::endl;
      return 1;
    }
  }

    if(pixelinputfiles.size() == 0 && crtsinputfiles.size() == 0){
    std::cout << "please give an inputfile with -p *.root and -c *.root for pixels and crts respctively" << std::endl;
    return 1;
  }

  //Check to see if the there is a one to one for tags and files
  if (argc - optind < 0) {
    std::cout << "Usage: " << argv[0] << " -c .root and -p .root " << std::endl;
    return 1;
  }

  // Look if the files are root, txt or unwanted formal.
  if(pixelinputfiles.size() > 1 || crtsinputfiles.size() > 1){
    std::cout<<" You give input files more than it is required; please give one root file for each or a txt file"<<std::endl;
    std::cout << "please give an input file with -p *.root and -c *.root for pixels and crts respctively"<<std::endl;
    return 1;
  }
  
  int crtsIProot   = CheckIfFilesRoot(crtsinputfiles[0]);
  int pixelsIProot = CheckIfFilesRoot(pixelinputfiles[0]);
  //
  std::cout<<" crtsIProot "<<crtsIProot<<" pixelsIProot "<<pixelsIProot<<std::endl;
  //
  if(crtsIProot == 1 && pixelsIProot == 1){
    int err = PixelAndCRTsSelection(pixelinputfiles,crtsinputfiles,process_events);
    //
    if(err != 0){std::cerr << "Error in analysis" << std::endl;}//close the if statement of err
    //
  }else if(crtsIProot == 1 && pixelsIProot == 2){
    std::vector<std::string> temp_inputFiles;
    ROOTtoTXTfiles(pixelinputfiles, temp_inputFiles);
    //Empty the pixelinputfiles holder to refill with root files
    pixelinputfiles.clear();
    //Refill the file with root files
    pixelinputfiles = temp_inputFiles;
    //
    int err = PixelAndCRTsSelection(pixelinputfiles,crtsinputfiles,process_events);
    //
    if(err != 0){std::cerr << "Error in analysis" << std::endl;}
    //
  }else if(crtsIProot == 2 && pixelsIProot == 1){
    std::vector<std::string> temp_inputFiles;
    ROOTtoTXTfiles(crtsinputfiles, temp_inputFiles);
    //Empty the crtsinputfiles holder to refill with root files
    crtsinputfiles.clear();
    //Refill the file with root files
    crtsinputfiles = temp_inputFiles;
    //
    int err = PixelAndCRTsSelection(pixelinputfiles,crtsinputfiles,process_events);
    //
    if(err != 0){std::cerr << "Error in analysis" << std::endl;}
    //
  }else{
    std::cout<<" One of the file is not in the correct form. Please check"<<std::endl;
    return 1;
  }
  //
  return 0;
}

  
int PixelAndCRTsSelection(std::vector<std::string>& pixelfiles, std::vector<std::string>& crtsfiles, int process_events){

  std::cout<< " process_events: " <<process_events<<std::endl;
  //if(current_eventnum > process_events && process_events > 0){return 0;}
  //###### Declaration and Initiate variables #######
  //Global variables
  int number_pixels_events = 0;
  //Places output holders for event information
  PixelData::TPC::Event* PixelsOutputEvents = new PixelData::TPC::Event();
  //Output file
  //Declaration for the output tree
  TFile* PixelOutputFile = new TFile(TString::Format("PixelTPC_miniCRTs_triggered_events_from_27072022_to_10082022.root"),"RECREATE");
  //Define output trees
  TTree* PixelOutputTree = new TTree("EventTree","Events");
  //
  PixelOutputTree->Branch("Events",&PixelsOutputEvents,16000,99);
  //
  //++current_eventnum;

  //##############################################################################################
  //################################### LOOP OVER CRTs Events ####################################
  //##############################################################################################
  //Loop over crts events and push the timestaps of events to a vector
  //Global variables for CRTs loop
  //
  std::vector<int> crts_timestamp;
  //
  {
    //File Inputs
    //Loop over crts files
    for(int i=0; i<crtsfiles.size(); ++i){
      //
      std::cout<<" LOOP OVER CRTS FILES "<<std::endl;
      std::cout<<" Number of CRTs files will be run: "<<crtsfiles.size()<<std::endl;
      std::string crtsfile = crtsfiles[i];
      std::cout<<" The file will be run now is: "<<crtsfile<<std::endl;
      //If the file is at the end with space continue so does not break the code
      if(crtsfile.size() < 1 ){continue;}
      //Access the files
      const char* crtsfilechar = crtsfile.c_str();
      TFile* crt_inputfile = new TFile(crtsfilechar);
      //Get the event tree
      TTree* crt_EventTree = (TTree*)crt_inputfile->Get("EventTree");
      //Places holders for event information
      PixelData::SubSystems::Event* crt_event = new PixelData::SubSystems::Event();
      //Get the tree event branch
      TBranch* crt_Branch  = crt_EventTree->GetBranch("Events");
      //
      crt_Branch->SetAddress(&crt_event);
      //loop over the crt events
      for(Long64_t eventNum=0;eventNum<crt_EventTree->GetEntries();eventNum++){
        //
        crt_Branch->GetEntry(eventNum);
        int eventNumber = crt_event->EventNumber();
        std::cout<<" CRT Event Number: "<<eventNumber<<std::endl;
        std::vector<PixelData::SubSystems::ChannelInfo> Channels = crt_event->GetChannels();
        //
        for(std::vector<PixelData::SubSystems::ChannelInfo>::iterator channel=Channels.begin();channel!=Channels.end(); ++channel){
          //Get the channels information
          //Declaration and Initialisation
          int ChannelNumber    = channel->channel;
          //
          std::pair<int, int> Timestamp = channel->timestamp;
          //Get the timestamp of ColdPMT
          if(ChannelNumber == 6 ){
            crts_timestamp.push_back((Timestamp.first));
          }//close the if statement
        }//close the loop over crts channels
      }//close the loop over crts events
    }//close the loop over crts files
    std::cout<<" crts_timestamp: "<<crts_timestamp.size()<<std::endl;
  }//close the crts block
  
  {//Pixle files block
    //File Inputs
    //Loop over pixels files
    for(int i=0; i<pixelfiles.size(); ++i){
      //
      std::cout<<" LOOP OVER Pixels FILES "<<std::endl;
      std::cout<<" Number of Pixels files will be run: "<<pixelfiles.size()<<std::endl;
      std::string pixelsfile = pixelfiles[i];
      std::cout<<" The file will be run now is: "<<pixelsfile<<std::endl;
      //If the file is at the end with space continue so does not break the code
      if(pixelsfile.size() < 1 ){continue;}
      //Access the file
      //int number_events_pixels = 0;
      const char* pixelfilechar = pixelsfile.c_str();
      TFile* pixel_inputFile = new TFile(pixelfilechar);
      TTree* pixelEventTree = (TTree*) pixel_inputFile->Get("EventTree");

      //Event Holder
      PixelData::TPC::Event* pixelEvent = new PixelData::TPC::Event();
      TBranch* pixelBranch  = pixelEventTree->GetBranch("Events");
      pixelBranch->SetAddress(&pixelEvent);
      for(Long64_t i=0;i<pixelEventTree->GetEntries();i++){
    
        //Get the Event Information
        pixelBranch->GetEntry(i);

        std::cout << " Event Number: " << pixelEvent->GetEventNumber()       <<std::endl;
        std::cout << " Pixel TimeStamp_s " <<pixelEvent->GetTimeStamp_s()    <<std::endl;
        std::cout << " Pixel TimeStamp_ns " << pixelEvent->GetTimeStamp_ns() <<std::endl;
        //
        //Get the timestamp for pixel events
        int NoEvents = pixelEvent->GetEventNumber();
        int pixel_timestamp_s  = pixelEvent->GetTimeStamp_s();
        int pixel_timestamp_ns = pixelEvent->GetTimeStamp_ns();
        //
    
        if(std::find(crts_timestamp.begin(), crts_timestamp.end(), pixel_timestamp_s) != crts_timestamp.end()){
          //Number of events that have been found
          ++number_pixels_events;
          std::cout<<" Pixel Event Number "<<NoEvents<<" Pixel timestamp: "<<pixel_timestamp_s<<std::endl;
          std::cout<<"################## The condition has been fulfilled: "<<number_pixels_events<<std::endl;
          PixelsOutputEvents->SetEventNumber(number_pixels_events);
          PixelsOutputEvents->SetTimestamp_s(pixel_timestamp_s);
          PixelsOutputEvents->SetTimestamp_ns(pixel_timestamp_ns);
          //Get Channels and loop over them to add to output tree
          //Get the channel Information
          const std::vector<PixelData::TPC::ChannelInfo> Channels = pixelEvent->GetChannels();
          for(std::vector<PixelData::TPC::ChannelInfo>::const_iterator channel=Channels.begin(); channel!=Channels.end(); ++channel){
            PixelData::TPC::ChannelInfo ChannelsToAdd = *channel;
            //Add the channel to output tree
            PixelsOutputEvents->AddChannel(ChannelsToAdd);
            //
          }//close the loop over channels
          //Fill the output tree
          PixelOutputTree->Fill();
          //
          //Clear variables
          PixelsOutputEvents->Clear();
          //
        }//Close the if statement 
      }//close the loop over pixel events
    }//Close the loop over pixel file 
    //Write the trees and close the files
    PixelOutputFile->cd();
    PixelOutputTree->Write();
    PixelOutputFile->Close();
    PixelOutputFile->Delete();
  }//Close pixel files block
  return 1;
}//close the Function PixelAndCRTsSelection

//################################################################################################
//#############################       Other Functions Required       #############################
//################################################################################################
//
//Function to loop over input files and check if they are root files then return a bool variables
//
int CheckIfFilesRoot(std::string& inputFilesTest){
  //Function Declaration and Initialisation
  //If the file is a root file it will retrun 1 or txt file it will return 2 otherwise will return 0
  int typeofInputFile = 0;
    
  //
  size_t length = strlen(inputFilesTest.c_str());
  //
  size_t num_space = std::count(inputFilesTest.begin(), inputFilesTest.end(),' ');
  if(num_space != 0){
    std::cerr << "Space found in input file. Please check" << std::endl;
    typeofInputFile = 0;  
  }
  //
  size_t suff = inputFilesTest.find(".");
  //
    /*if(inputFilesTest[i].substr(suff,length) != ".root"){
      std::cerr << "Input file is not a .root. Please check." << std::endl;
      //continue;
    }*/ 
  if(inputFilesTest.substr(suff,length) == ".root"){
    typeofInputFile = 1;
  }else if(inputFilesTest.substr(suff,length) == ".txt"){
    typeofInputFile = 2;
  }else{
    std::cerr << "Input file is not a .root nor .txt. Please check." << std::endl;
    typeofInputFile = 0; 
  }
  //
  return typeofInputFile;
}//close the input files check fun

//Loop over the files if the input file is txt file.
void ROOTtoTXTfiles(std::vector<std::string>& txtFilename, std::vector<std::string>& rootFilesOutput){  
  //###### Declaration and Initiate variables #######
  //Read the txt file to rewrite the events into two file based on their trigger
  std::ifstream myReadFile;
  std::string RootFileList = txtFilename[0];
  myReadFile.open(RootFileList);
  std::string InPutRoot;
  std::vector<std::string> InputRootFiles_vec;
  //
  //Loop over the input files
  if(myReadFile.is_open()){
    while(!myReadFile.eof()){
      //
      //Save the line in string
      std::getline(myReadFile, InPutRoot);
      if(InPutRoot.size() < 1){break;}
      //
      int IsItRoot =  CheckIfFilesRoot(InPutRoot);
      if(IsItRoot == 1){
        InputRootFiles_vec.push_back(InPutRoot);
      }//Close if statement
    }//close the while loop
  }//close the if statement
  myReadFile.close();
  //
  std::cout<<" Input files Number: "<<InputRootFiles_vec.size()<<std::endl;
  rootFilesOutput = InputRootFiles_vec;
  //
  return;
}//close the function to get root files from txt file


