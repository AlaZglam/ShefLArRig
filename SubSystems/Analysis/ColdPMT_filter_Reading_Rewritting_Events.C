//###################################################################\\
//####### Title: ColdPMT_filter_Reading_Rewritting_Events.cc  #######\\
//#######                                                     #######\\
//####### Author:       Ala Zglam                             #######\\
//####### Email:        aazglam1@sheffield.ac.uk              #######\\
//#######               ala.zeglam10@gmail.com                #######\\
//#######                                                     #######\\
//####### Description:  This module compare the time between  #######\\
//#######               all Subsystmes CRTs, and classified   #######\\
//#######               events into events triggered by Side  #######\\
//#######               CRTs or vertical CRTs.                #######\\
//#######                                                     #######\\
//####### Last Updated: 16 September 2022                     #######\\
//###################################################################\\


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

void ColdPMT_filter_Reading_Rewritting_Events(){
  //
  //###### Declaration and Initiate variables #######
  //Global variables
  int number_events_coldPMT = 0;
  //Places output holders for event information
  PixelData::SubSystems::Event* OutPutEvents = new PixelData::SubSystems::Event();
  //
  //Declaration for the output tree
  TFile* outputfile_coldPMT = new TFile(TString::Format("LArPMT_ColdPMT_Events_With_Trigger_Far_and_Bench_Sides_Events_with_threshold_10ADC.root"),"RECREATE");
  //Define output trees
  TTree* outputtree_coldPMT = new TTree("EventTree","Events");
  //Define branches
  outputtree_coldPMT->Branch("Events",&OutPutEvents,16000,2);
  //
  //File Inputs
  TFile* inputfile = new TFile("LArPMT_FAR_and_BENCH_sides_events.root");
  TTree* EventTree = (TTree*)inputfile->Get("EventTree");
  //Places holders for event information
  PixelData::SubSystems::Event* event = new PixelData::SubSystems::Event();
  TBranch* branch  = EventTree->GetBranch("Events");  
  branch->SetAddress(&event);  
  for(Long64_t eventNum=0;eventNum<EventTree->GetEntries();eventNum++){
    //Declaration Event variables
    branch->GetEntry(eventNum);
    int eventNumber = event->EventNumber();
    //A map of channels number and peak height in ADC
    std::map<int, float> ChannelsToPH_coldPMT;
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
      float PeakHeightADC  = TMath::Abs(MaxADC - BaseAdc);  
      float PeaktimeSec    = ((int)(channel->peaktimeSec));
      //
      if(PeakHeightADC > 10){
        std::cout<<" Event Number: "<<eventNumber<<" Channel Number: "<<ChannelNumber<<" Peak Height: "<<PeakHeightADC<<std::endl;
        //Look if the channels number with peak height more than 10 ADC is 3, 4, 5, 10
        if(ChannelNumber == 6 ){
          ChannelsToPH_coldPMT[ChannelNumber] = PeakHeightADC;
        }//close if statement 
      }//close if statement  
    }//Close the loop over the channels
    //
    //Write every event to its' own tree file  
    if(ChannelsToPH_coldPMT.size() > 0){
      //
      std::cout<<" ChannelsToPH_coldPMT size: "<<ChannelsToPH_coldPMT.size()<<std::endl;
      // increase the number of events before adding to tree
      ++number_events_coldPMT;  
      //Open the output file
      //Add the event number to the OutPutEvents
      OutPutEvents->SetEventNumber(number_events_coldPMT);  
      //Add the channels to the OutPutEvents
      std::cout<<" Channel Test: "<<ChannelIDToChannelInfo[6].channel<<std::endl;
      OutPutEvents->AddChannel(ChannelIDToChannelInfo[5]);  
      OutPutEvents->AddChannel(ChannelIDToChannelInfo[6]);
      //Fill the output tree
      outputtree_coldPMT->Fill();
      //Clear variables
      ChannelsToPH_coldPMT.clear();
      OutPutEvents->Clear();  
    }
    //
  }//close the loop over the events
  //
  //Write the trees and close the files
  //For Far and branch sides
  outputfile_coldPMT->cd();
  outputtree_coldPMT->Write();
  outputfile_coldPMT->Close();
  outputfile_coldPMT->Delete();
}//close the ColdPMT_filter_Reading_Rewritting_Events.C
