//####################################################################\\
//####### Title:        CRTReading_Rewritting.C                #######\\
//#######                                                      #######\\
//####### Author:       Ala Zglam                              #######\\
//####### Email:        aazglam1@sheffield.ac.uk               #######\\
//#######               ala.zeglam10@gmail.com                 #######\\
//#######                                                      #######\\
//####### Description: Read the txt file to rewrite the events #######\\
//#######              into two file based on their trigger;   #######\\
//#######              events triggered by Side CRTs or        #######\\
//#######              vertical CRTs.                          #######\\
//#######                                                      #######\\
//####### Last Updated: 16 September 2022                      #######\\
//####################################################################\\


//##################################################################################
//
//Decoder Includes                                                           
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/ChannelInfo.h"
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/Event.h"
//
//C++ Includes                                                           
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <cassert>

//Root Includes                                                          
#include "TMath.h"
#include "TApplication.h"
#include "TLine.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

void CRTReading_Rewritting(){
  //
  //Read the txt file to rewrite the events into two file based on their trigger
  std::ifstream myReadFile;
  myReadFile.open("RootFileList.txt");
  std::string InPutRoot;
  std::vector<std::string> InputRootFiles_vec;
  //
  //Loop over the input files
  if(myReadFile.is_open()){
    while(!myReadFile.eof()){
      //
      //Save the line in string
      std::getline(myReadFile, InPutRoot);
      InputRootFiles_vec.push_back(InPutRoot);
      //
    }//close the while loop
  }//close the if statement
  myReadFile.close();
  //
  std::cout<<" Input files Number: "<<InputRootFiles_vec.size()<<std::endl;
  //###### Declaration and Initiate variables #######
  //Global variables
  int number_events_crt34 = 0;
  int number_events_crt78 = 0;
  //Places output holders for event information
  PixelData::SubSystems::Event* OutPutEvents = new PixelData::SubSystems::Event();
  //
  //Declaration for the output tree
  TFile* outputfile_crt34 = new TFile(TString::Format("LArPMT_FAR_and_BENCH_sides_events.root"),"RECREATE");
  TFile* outputfile_crt78 = new TFile(TString::Format("LArPMT_TOP_and_BOTTOM_sides_events.root"),"RECREATE");
  //Define output trees
  TTree* outputtree_crt34 = new TTree("EventTree","Events");
  TTree* outputtree_crt78 = new TTree("EventTree","Events");
  //Define branches
  outputtree_crt34->Branch("Events",&OutPutEvents,16000,2);
  outputtree_crt78->Branch("Events",&OutPutEvents,16000,2);
  //
  int endloop = 0;
  //
  for(auto InputFile : InputRootFiles_vec){
    //
    std::cout<<" input file : "<<InputFile<<std::endl;
    //
    ++endloop;
    if(endloop == InputRootFiles_vec.size()){break;}
    //File Inputs
    const char* fInputFile = InputFile.c_str();
    //
    TFile* inputfile = new TFile(fInputFile);
    TTree* EventTree = (TTree*)inputfile->Get("pulsetree");
    //Places holders for event information
    PixelData::SubSystems::Event* event = new PixelData::SubSystems::Event();
    TBranch* branch  = EventTree->GetBranch("event");
    branch->SetAddress(&event);
    for(Long64_t eventNum=0;eventNum<EventTree->GetEntries();eventNum++){
      //Declaration Event variables
      branch->GetEntry(eventNum);
      int eventNumber = event->EventNumber();
      //A map of channels number and peak height in ADC
      std::map<int, float> ChannelsToPH_crt34;
      std::map<int, float> ChannelsToPH_crt78;
      std::map<int, PixelData::SubSystems::ChannelInfo > ChannelIDToChannelInfo;
      //
      std::cout<<" Event Number: "<<eventNumber<<std::endl;
      //Get the Channels as CRTs and PMTs
      std::vector<PixelData::SubSystems::ChannelInfo> Channels = event->GetChannels();
      //
      for(std::vector<PixelData::SubSystems::ChannelInfo>::iterator channel=Channels.begin();channel!=Channels.end(); ++channel){
        //Declaration and Initialisation
        int ChannelNumber    = channel->channel;
        //Add to ChannelIDToChannelInfo map
        ChannelIDToChannelInfo[ChannelNumber] = *channel;
        float MaxADC         = channel->maxAdc;
        float BaseAdc        = channel->baseAdc;
        float PeakHeightADC     = TMath::Abs(MaxADC - BaseAdc);
        float PeaktimeSec       = ((int)(channel->peaktimeSec));
        //
        if(PeakHeightADC > 10){
          std::cout<<" Event Number: "<<eventNumber<<" Channel Number: "<<ChannelNumber<<" Peak Height: "<<PeakHeightADC<<std::endl;
          //<<" Peak time Sec: "<<PeaktimeSec<<" time first: "<<Timestamp.first<<" time second "<<Timestamp.second<<std::endl;
          //<<" MaxADC: "<<MaxADC<<" Integral: "<<Integral<<" IntegralCharge: "<<IntegralCharge<<" BaseRmsAdc: "<<BaseRmsAdc
          //<<" BaseAdc: "<<BaseAdc<<" PeaktimeTdc: "<<PeaktimeTdc<<" waveform size: "<<waveform.size()<<std::endl;
          //
          //Look if the channels number with peak height more than 10 ADC is 3, 4, 5, 10
          if(ChannelNumber == 3 || ChannelNumber == 4){
            ChannelsToPH_crt34[ChannelNumber] = PeakHeightADC;
          }else if(ChannelNumber == 7 || ChannelNumber == 8){
            ChannelsToPH_crt78[ChannelNumber] = PeakHeightADC;
          }//close if statement 
          //
        }//close if statement
        //
      }//Close the loop over the channels
      //
      //Write every event to its' own tree file
      if(ChannelsToPH_crt34.size() > 1){
        //
        std::cout<<" ChannelsToPH_crt34 size: "<<ChannelsToPH_crt34.size()<<std::endl;
        // increase the number of events before adding to tree
        ++number_events_crt34;
        //Open the output file
        //Add the event number to the OutPutEvents
        OutPutEvents->SetEventNumber(number_events_crt34);
        //Add the channels to the OutPutEvents
        std::cout<<" Channel Test: "<<ChannelIDToChannelInfo[6].channel<<std::endl;
        OutPutEvents->AddChannel(ChannelIDToChannelInfo[3]);
        OutPutEvents->AddChannel(ChannelIDToChannelInfo[4]);
        OutPutEvents->AddChannel(ChannelIDToChannelInfo[5]);
        OutPutEvents->AddChannel(ChannelIDToChannelInfo[6]);
        //Fill the output tree
        outputtree_crt34->Fill();
        //Clear variables
        ChannelsToPH_crt34.clear();
        OutPutEvents->Clear();
      }
      //
      //Write every event to its' own tree file
      if(ChannelsToPH_crt78.size() > 1){
        //
        std::cout<<" ChannelsToPH_crt78 size: "<<ChannelsToPH_crt78.size()<<std::endl;
        // increase the number of events before adding to tree
        ++number_events_crt78;
        //Open the output file
        //Add the event number to the OutPutEvents
        OutPutEvents->SetEventNumber(number_events_crt78);
        //Add the channels to the OutPutEvents
        OutPutEvents->AddChannel(ChannelIDToChannelInfo[5]);
        OutPutEvents->AddChannel(ChannelIDToChannelInfo[6]);
        OutPutEvents->AddChannel(ChannelIDToChannelInfo[7]);
        OutPutEvents->AddChannel(ChannelIDToChannelInfo[8]);
        //Fill the output tree
        outputtree_crt78->Fill();
        //Clear variables
        ChannelsToPH_crt78.clear();
        OutPutEvents->Clear();
      }
      //
    }//close the loop over the events
  }//Close the loop over the files
  //
  //Write the trees and close the files
  //For Far and branch sides
  outputfile_crt34->cd();
  outputtree_crt34->Write();
  outputfile_crt34->Close();
  outputfile_crt34->Delete();
  //For Top and Bottom sides
  outputfile_crt78->cd();
  outputtree_crt78->Write();
  outputfile_crt78->Close();
  outputfile_crt78->Delete();
}//close the CRTReading_Test.C
