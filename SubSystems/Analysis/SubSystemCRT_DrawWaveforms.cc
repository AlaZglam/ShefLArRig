//###################################################################\\
//####### Title:        SubSystemCRT_DrawWaveforms.cc         #######\\
//#######                                                     #######\\
//####### Author:       Ala Zglam                             #######\\
//####### Email:        aazglam1@sheffield.ac.uk              #######\\
//#######               ala.zeglam10@gmail.com                #######\\
//#######                                                     #######\\
//####### Description:  This module draws two waveform for    #######\\
//#######               Subsystmes CRTs.                      #######\\
//#######                                                     #######\\
//####### Last Updated: 26 July 2022                          #######\\
//###################################################################\\

                             //#############################
                             //######## WARNING !!! ########
                             //#############################
//######################################################################################################
//#######      To run this file please use something similar to this:                            #######
//#######      root -l  'SubSystemCRT_DrawWaveforms.cc("YourROOTFileName.root", Event Number)'   #######
//######################################################################################################
//

//Decoder Includes                                                                                                                         
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/ChannelInfo.h"
#include "/home/argonshef/LArAnalysis/srcs/SubSystems/DAQDecoder/Event.h"

//Root Includes    
#include "TFile.h"
#include "TTree.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h" 
#include "TGraph.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TApplication.h"

//C++ Includes 
#include <fstream> 
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <getopt.h>
#include <numeric>      // std::accumulate

void SubSystemCRT_DrawWaveforms(std::string filename, int NumberOfEvent){
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

    //Map of the channel waveforsm
    std::map<std::string,TGraph*> WaveformGraph;
 
    for(Long64_t eventNum=0;eventNum<EventTree->GetEntries();eventNum++){
      inputfile->cd();
      //Get the Event Information
      //
      branch->GetEntry(eventNum);
      int eventNumber = event->EventNumber();
      if(eventNumber == NumberOfEvent){
   
      std::cout << " Event Number: " << eventNumber << std::endl;
    
      std::string Canvas_string = "Event " + std::to_string(event->EventNumber()) + " Waveforms";
      const char* Canvas_name = Canvas_string.c_str();
      TCanvas *c = new TCanvas(Canvas_name,Canvas_name);
      c->SetCanvasSize(750,750);
      c->SetWindowSize(1000,3000);
      c->Divide(2,3,0.01,0.01);
    
      //Get the channel Information
      const std::vector<PixelData::SubSystems::ChannelInfo> Channels = event->GetChannels();
    
      for(std::vector<PixelData::SubSystems::ChannelInfo>::const_iterator channel=Channels.begin(); channel!=Channels.end(); ++channel){
      
        const float Pedestal              = channel->baseAdc;
        const std::vector<float> Waveform = channel->waveform;
        const int NADC                    = Waveform.size();
        const int Orichan                 = channel->channel;
        const float RmsAdcs               = channel->baseRmsAdc;
        //Get the average in waveform 
        float WF_average = ((std::accumulate(Waveform.begin(), Waveform.end(), 0.0)) / NADC);
        std::cout<<" WF_average: "<<WF_average<<" Pedestal "<<Pedestal<<std::endl;
        
        std::string CanvasName    = "Empty";
        if(Orichan == 3){CanvasName = "North Side CRT";}
        if(Orichan == 4){CanvasName = "South Side CRT";}
        if(Orichan == 5){CanvasName = "Logic Channel";}
        if(Orichan == 6){CanvasName = "Cold CRT";}
        if(Orichan == 7){CanvasName = "Top CRT";}
        if(Orichan == 8){CanvasName = "Bottom CRT";}
      
        if(CanvasName == "Empty"){continue;}
        //Create the histograms
        std::string Graph_string = "Waveform Channel for PMT: " + CanvasName;
        const char* Graph_name = Graph_string.c_str();
        WaveformGraph[Graph_string]  = new TGraph();
        WaveformGraph[Graph_string]->SetTitle(Graph_name);
        WaveformGraph[Graph_string]->GetXaxis()->SetTitle("Time (tick)");
        WaveformGraph[Graph_string]->GetYaxis()->SetTitle("Amplitude (ADC)");
        // WaveformGraph[Graph_string]->GetXaxis()->SetTickSize(0.01);
        // WaveformGraph[Graph_string]->GetXaxis()->SetTitleSize(0.9);
        // WaveformGraph[Graph_string]->GetXaxis()->SetLabelSize(0.9);
        // WaveformGraph[Graph_string]->GetYaxis()->SetTickSize(0.01);
        // WaveformGraph[Graph_string]->GetYaxis()->SetTitleSize(0.9);
        // WaveformGraph[Graph_string]->GetYaxis()->SetLabelSize(0.6);
        // WaveformGraph[Graph_string]->GetYaxis()->SetLabelOffset(0.05);
        (TPad*) c->cd((Orichan -2));

        for(int adc_it=0; adc_it<NADC; ++adc_it){
          //if((std::abs(Waveform[adc_it] - Pedestal)) < 1.5){WaveformGraph[Graph_string]->SetPoint(WaveformGraph[Graph_string]->GetN(),adc_it,(0));}
          WaveformGraph[Graph_string]->SetPoint(WaveformGraph[Graph_string]->GetN(),adc_it,(Waveform[adc_it] - Pedestal));
        }
      
        WaveformGraph[Graph_string]->SetLineColor(4);
        WaveformGraph[Graph_string]->Draw();
        c->Update();
      }

      c->cd();
      c->Draw("a");
      gPad->Modified();
      gPad->Update();

      std::cout << "Event Complete" << std::endl;
      //
      }
    }
  }
}

