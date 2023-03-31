//###################################################################\\
//####### Title:        SubSystemCRT_FFTNoiseFilter.cc        #######\\
//#######                                                     #######\\
//####### Author:       Ala Zglam                             #######\\
//####### Email:        aazglam1@sheffield.ac.uk              #######\\
//#######               ala.zeglam10@gmail.com                #######\\
//#######                                                     #######\\
//####### Description: A noise filter based on a Band-pass    #######\\
//#######              filter-based algorithm is applied to   #######\\
//#######              remove the noise frequencies.          #######\\
//#######                                                     #######\\
//####### Last Updated: 26 July 2022                          #######\\
//###################################################################\\
//


                               //#############################
                               //######## WARNING !!! ########
                               //#############################
//##########################################################################################
//#######      To run this file please use something similar to this:                #######
//#######      root -l  'SubSystemCRTs_FFTNoiseFilter.cc("YourROOTFileName.root")'   #######
//##########################################################################################
 

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

//A band pass filter on the code. 
void SubSystemCRT_FFTNoiseFilter(std::string filename){
  //Check if it is root file before continue
  size_t length = strlen(filename.c_str());
  size_t suff = filename.find(".");
  if(filename.substr(suff,length) != ".root"){
    std::cerr << "Input file is not a .root. Please check." << std::endl;
  }else{
    //Parameters to remove the noise.
    double Upper_cutoff_freq = 18e3; //Below this frequency peaks above the threshold are not removed. Heed warning.
    double Lower_cutoff_freq = 2e3; //Above this frequency peaks above the threshold are not removed. Heed warning.
    //double fPeakCutOff      = 10;    //ADC cut off for the peaks.
      
    //PixelData::SubSystems::Event* Event;
    PixelData::SubSystems::Event* OutPutEvents = new PixelData::SubSystems::Event();

    //Output file
    //size_t suff = filename.find(".");
    TFile* outputfile = new TFile(TString::Format("%s_FFTNoiseFilter.root",filename.substr(0,suff).c_str()),"RECREATE");
    TTree* outputtree = new TTree("EventTree","Events");  
    outputtree->Branch("Events",&OutPutEvents,16000,99);

    //File Inputs
    const char* filechar = filename.c_str();
    TFile* inputfile = new TFile(filechar);
    TTree* EventTree = (TTree*)inputfile->Get("EventTree");

    //Event Holder
    PixelData::SubSystems::Event* event = new PixelData::SubSystems::Event();
    TBranch* branch = EventTree->GetBranch("Events");
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
        //
        //FFt Histogram
        TH1* fft =0;
        //Apply FFT
        TVirtualFFT::SetTransform(0);
        fft = wf_hist->FFT(0, "MAG");
        //
        //Get the fft arrays
        TVirtualFFT *fft_vert = TVirtualFFT::GetCurrentTransform();
        //
        Double_t* re_fft = new Double_t[NADC];
        Double_t* im_fft = new Double_t[NADC];
        //
        fft_vert->GetPointsComplex(re_fft, im_fft);
        //Normalise to the correct ADC
        for(int i=0; i<NADC; ++i){
          re_fft[i] /= TMath::Sqrt(NADC);
          im_fft[i] /= TMath::Sqrt(NADC);
        }
        //
        //Filter out high-frequency components
        //
        for(int i = 0; i < (NADC); i++){
          double freq = ((i* NADC)/NADC);  //note to get frequence you need to multiply number of Bins and divide by sampling frequency (x-max); and in this case, they are same.
          //
          if(freq > Lower_cutoff_freq  && freq < Upper_cutoff_freq){
            re_fft[i] = 0;
            im_fft[i] = 0;
          }
        }
        //
        // Transform the modified magnitude spectrum back to the time domain 
        Int_t nBins = NADC;
        // Inverse transform the modified magnitude spectrum back to the time domain
        TVirtualFFT *fft_inverse = TVirtualFFT::FFT((Int_t) 1,&nBins, "C2R M K");
        fft_inverse->SetPointsComplex(re_fft,im_fft);
        fft_inverse->Transform();

        //Fill the waveform
        TH1 *hFiltered = 0;
        hFiltered = TH1::TransformHisto(fft_inverse,hFiltered,"Re");
        //
        std::vector<float> cleaned_wf;
        //
        for(int adc_it=0;adc_it<NADC;++adc_it){
          //Get the y value of the graph and fill to vector of cleaned waveforms
          float y_value = (hFiltered->GetBinContent(adc_it)/TMath::Sqrt(NADC)); //->GetBinContent(adc_it);
          cleaned_wf.push_back(y_value);
          //Add the waveform to a TGraph
          //
        }//Close the loop
        //       
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
        delete fft;
        delete hFiltered;
        delete fft_inverse;
    
      }
      outputtree->Fill();
    }

    outputfile->cd();
    outputtree->Write();
    outputfile->Close();
    outputfile->Delete();
  
  }

}
