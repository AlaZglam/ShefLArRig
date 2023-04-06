//###################################################################\\
//####### Title:        SubSystemCRT_FourierTransform.cc      #######\\
//#######                                                     #######\\
//####### Author:       Ala Zglam                             #######\\
//####### Email:        aazglam1@sheffield.ac.uk              #######\\
//#######               ala.zeglam10@gmail.com                #######\\
//#######                                                     #######\\
//####### Description: Code Take the fourier transform of the #######\\
//#######              data and averages them.                #######\\
//#######                                                     #######\\
//####### Last Updated: 26 July 2022                          #######\\
//###################################################################\\


                                //#############################
                                //######## WARNING !!! ########
                                //#############################
//###########################################################################################
//#######      To run this file please use something similar to this:                 #######
//#######      root -l  'SubSystemCRT_FourierTransform.cc("YourROOTFileName.root")'   #######
//###########################################################################################
 

//Decoder Includes                                                                                                           
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/ChannelInfo.h"
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/Event.h"
//

//Root Includes    
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TVirtualFFT.h"
#include "TMath.h"
#include "TH1.h"

//C++ Includes 
#include <fstream> 
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <getopt.h>


//Code Take the fourier tranform of the data and averages them.  
void SubSystemCRT_FourierTransform(std::string filename){
  //Check if it is root file before continue
  size_t length = strlen(filename.c_str());
  size_t suff = filename.find(".");
  //
  if(filename.substr(suff,length) != ".root"){
    std::cerr << "Input file is not a .root. Please check." << std::endl;
  }else{
    //Parameters to remove the noise.

    //File Inputs
    const char* filechar = filename.c_str();
    TFile *inputfile = new TFile(filechar);
    TTree *EventTree = (TTree*)inputfile->Get("EventTree");

    //Event Holder
    PixelData::SubSystems::Event *event = new PixelData::SubSystems::Event();
    TBranch *branch  = EventTree->GetBranch("Events");
    branch->SetAddress(&event);
  
    //FFT histograms.  
    std::map<int,TH1D*> fft_avg_ch;
    std::map<int,int> avg_ch;

    std::vector<TH1D*> wf_hists;
  
    for(Long64_t i=0;i<EventTree->GetEntries();i++){

      //if(i==10){continue;}
      if(i > 100){continue;}

      //Get the Event Information
      branch->GetEntry(i);

      std::cout<<"Event Number: "<<event->EventNumber()<<std::endl;

      //Get the channel Information
      const std::vector<PixelData::SubSystems::ChannelInfo> Channels = event->GetChannels();
      for(std::vector<PixelData::SubSystems::ChannelInfo>::const_iterator channel=Channels.begin(); channel!=Channels.end(); ++channel){
        
        const float Pedestal               = channel->baseAdc;
        const std::vector<float> Waveform = channel->waveform;
        const int NADC                     = Waveform.size();
        const int Orichan                  = channel->channel;

        //Initial average FFT
        if(fft_avg_ch.find(Orichan) == fft_avg_ch.end()){
          std::string chan_string = "Average FFT for Channel " + std::to_string(Orichan); 
          const char* chan_name = chan_string.c_str(); 
          fft_avg_ch[(Orichan)] =  new TH1D(chan_name,chan_name,NADC,0,NADC);  
        
          for(int i=0; i<NADC; ++i){
            fft_avg_ch[(Orichan)]->SetBinContent(i,0);
          }
        }
      
        //Waveform  histograms. Slow don't care. 
        TH1D* wf_hist = new TH1D("wf_hist","wf_hist",NADC,0,NADC); 
      
        int adc_it=0; 
        for(auto const& tick: Waveform){
          //Add the waveform to a histogram
          wf_hist->Fill(adc_it,(tick - Pedestal)*217.6688);  //wf_hist->Fill(adc_it,(tick - Pedestal)*217.6688);
          ++adc_it;
        }
      
        //   if((Orichan) == 1){ 
        //	TH1D* clone = (TH1D*) wf_hist->Clone();
        //	wf_hists.push_back(clone);
        //      }
        //FFt Histogram
        TH1 * fft =0;
        TVirtualFFT::SetTransform(0);
        fft = wf_hist->FFT(fft, "MAG");

        //Add to the average fft.
        fft_avg_ch[(Orichan)]->Add(fft);
        ++avg_ch[(Orichan)];

        delete wf_hist;
        delete fft;

      }
    }
  
    size_t suff = filename.find(".");
    TFile* outputfile = new TFile(TString::Format("%s_FFT.root",filename.substr(0,suff).c_str()),"RECREATE");
    outputfile->cd();

    //  for(auto const& hist: wf_hists){
    //    hist->Write();
    //  }
  
    std::map<int,TH1D*> fft_avg_ch_scaled;
    for(auto const& chan: fft_avg_ch){
    
      chan.second->Scale(1/((float) chan.second->GetNbinsX()));

      std::string chan_string = "Average scaled FFT for Channel " + std::to_string(chan.first); 
      const char* chan_name = chan_string.c_str(); 
      fft_avg_ch_scaled[chan.first] =  new TH1D(chan_name,chan_name,chan.second->GetNbinsX(),0,1/5e-5);    
      for(int i=0; i<chan.second->GetNbinsX(); ++i){
        fft_avg_ch_scaled[chan.first]->SetBinContent(i,chan.second->GetBinContent(i));
      }
    }
  
    for(auto const& chan: fft_avg_ch_scaled){
      chan.second->Write();  
    }
  }
}
