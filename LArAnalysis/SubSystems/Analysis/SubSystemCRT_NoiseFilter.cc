//###################################################################\\
//####### Title:        SubSystemCRT_NoiseFilter.cc           #######\\
//#######                                                     #######\\
//####### Author:       Ala Zglam                             #######\\
//####### Email:        aazglam1@sheffield.ac.uk              #######\\
//#######               ala.zeglam10@gmail.com                #######\\
//#######                                                     #######\\
//####### Description: This filter is based on threshold cut  #######\\
//#######              and smoothing function to the waveform #######\\
//#######              to reduce the noise.                   #######\\
//#######                                                     #######\\
//####### Last Updated: 26 July 2022                          #######\\
//###################################################################\\

                                  //#############################//
                                  //######## WARNING !!! ########//
                                  //#############################//
//#######################################################################################
//#######      To run this file please use something similar to this:             #######
//#######      root -l  'SubSystemCRTs_NoiseFilter.cc("YourROOTFileName.root")'   #######
//#######################################################################################
 

//Decoder Includes                                                                                                                         
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/ChannelInfo.h"
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/Event.h"

//Root Includes    
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TVirtualFFT.h"
#include "TMath.h"
#include "TH1.h"
#include "TGraph.h"
#include "TApplication.h"

//C++ Includes 
#include <fstream> 
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <getopt.h>
#include <sstream>
#include <utility>
#include <cassert>

// 
void SubSystemCRT_NoiseFilter(std::string filename){
  //Check if it is root file before continue
  size_t length = strlen(filename.c_str());
  size_t suff = filename.find(".");
  if(filename.substr(suff,length) != ".root"){
    std::cerr << "Input file is not a .root. Please check." << std::endl;
  }else{
    //Parameters to remove the noise.
    double fThreshold      = 3;    //ADC cut off for the peaks.
      
    //PixelData::SubSystems::Event* Event;
    PixelData::SubSystems::Event* OutPutEvents = new PixelData::SubSystems::Event();

    //Output file
    //size_t suff = filename.find(".");
    TFile* outputfile = new TFile(TString::Format("%s_NoiseFilter.root",filename.substr(0,suff).c_str()),"RECREATE");
    TTree* outputtree = new TTree("EventTree","Events");  
    outputtree->Branch("Events",&OutPutEvents,16000,99);

    //File Inputs
    const char* filechar = filename.c_str();
    TFile* inputfile = new TFile(filechar);
    TTree* EventTree = (TTree*)inputfile->Get("EventTree");

    //Event Holder
    PixelData::SubSystems::Event* event  = new PixelData::SubSystems::Event();
    TBranch* branch  = EventTree->GetBranch("Events");
    branch->SetAddress(&event);

    for(Long64_t i=0;i<EventTree->GetEntries();i++){

      //Get the Event Information
      branch->GetEntry(i);
      std::cout << "Event Number: " << event->EventNumber() << std::endl;
      if((event->EventNumber()) > 100){continue;}
      //Get the channel Information
      const std::vector<PixelData::SubSystems::ChannelInfo> Channels = event->GetChannels();
      for(std::vector<PixelData::SubSystems::ChannelInfo>::const_iterator channel=Channels.begin(); channel!=Channels.end(); ++channel){
        const float BaseAdc                = channel->baseAdc;
        const std::vector<float> Waveform  = channel->waveform;
        const int NADC                     = Waveform.size();
        const float RmsAdcs                = channel->baseRmsAdc;
        // 
        //Waveform  histograms. 
        TH1D* wf_hist = new TH1D("wf_hist","wf_hist",NADC,0,NADC);
        //Loop over the waveforms and fill the wf_hist histogram
        for(int adc_it=0;adc_it<NADC;++adc_it){
          //Add the waveform to a TGraph
          wf_hist->Fill(adc_it,((Waveform[adc_it] - BaseAdc)));
        }
        //
        TH1D* wf_filtered = new TH1D("wf_filtered","wf_filtered",NADC,0,NADC);
        //
        for(int i = 0; i < (NADC); i++){
          double binContent = std::abs(wf_hist->GetBinContent(i));
          if(binContent < fThreshold){
            wf_filtered->Fill(i, 0);
          }else{
            wf_filtered->Fill(i,(wf_hist->GetBinContent(i)));
          }
        }//close the loop

        // apply smoothing to remaining data
        wf_filtered->Smooth(5); // 5 is the number of times to apply the smoothing function
        
        //Save cleaned waveform to file
        //
        TFile *fout = new TFile("cleaned_waveform.root", "RECREATE");
        wf_filtered->Write();
        fout->Close();
        
        //
        std::vector<float> cleaned_wf;
        //
        for(int adc_it=0;adc_it<NADC;++adc_it){
          //Get the y value of the graph and fill to vector of cleaned waveforms
          float y_value = wf_filtered->GetBinContent(adc_it);
          cleaned_wf.push_back(y_value);
        }//Close the loop
        //
        //Refill the channel data with the knew waveform
        //Event = event;
        //
        OutPutEvents->SetEventNumber((event->EventNumber()));
        //Add the channels to the OutPutEvents
        //
        int   Orichan            = channel->channel;
        float OribaseVoltage     = channel->baseVolt;
        float OribaselineAdc     = channel->baseAdc;
        float OribaseRmsVolts    = channel->baseRmsVolt;
        float OribaseRmsAdcs     = channel->baseRmsAdc;
        float OrimaxVolts        = channel->maxVolt;
        float OrimaxAdcs         = channel->maxAdc;
        float OripeaktimeSecs    = channel->peaktimeSec;
        float OripeaktimeTdcs    = channel->peaktimeTdc ;
        std::pair<int,int> Orits = channel->timestamp;
        float Orisum             = channel->integral;
        float OrisumCharge       = channel->integralCharge;
        //
        PixelData::SubSystems::ChannelInfo ChannelData = PixelData::SubSystems::ChannelInfo(Orichan, 0, 0,OribaseRmsAdcs, OribaseRmsVolts, OrimaxVolts, OrimaxAdcs, OripeaktimeSecs, OripeaktimeTdcs, cleaned_wf, Orits, Orisum, OrisumCharge);
        OutPutEvents->AddChannel(ChannelData);

        //Delete everything
        delete wf_hist;
        delete wf_filtered;
    
      }
      outputtree->Fill();
    }

    outputfile->cd();
    outputtree->Write();
    outputfile->Close();
    outputfile->Delete();
  
  }

}
