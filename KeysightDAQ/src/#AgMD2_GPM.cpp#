#include "../include/AgMD2_GPM.h"

///
/// Based on Keysight IVI-C Driver Example Program
///
/// Initializes the driver, reads a few Identity interface properties, and performs a
/// multi-record acquisition.
///
/// See driver help topic "Programming the IVI-C Driver in Various Development Environments"
/// for additional programming information.
///
/// Runs in simulation mode without an instrument.
///

void AgMD2_GPM::Quit()
{
  ViSession session = GetSession();

  // Close the driver.
  AgMD2_close(session);
  printf("Driver closed \n");

  gApplication->Terminate(0);
}

// Utility function to check status error during driver API call.
void AgMD2_GPM::checkApiCall(ViStatus status, char const* functionName)
{
  ViInt32 ErrorCode;
  ViChar ErrorMessage[256];

  if (status > 0) // Warning occurred.
    {
      AgMD2_GetError(VI_NULL, &ErrorCode, sizeof(ErrorMessage), ErrorMessage);
      printf("** Warning during %s: 0x%08x, %s\n", functionName, ErrorCode, ErrorMessage);
    }
  else if (status < 0) // Error occurred.
    {
      
      AgMD2_GetError(VI_NULL, &ErrorCode, sizeof(ErrorMessage), ErrorMessage);
      printf("** Error during %s: 0x%08x, %s\n", functionName, ErrorCode, ErrorMessage);
      Quit();
      throw std::exception();
    }
}

void AgMD2_GPM::initialize_driver()
{
  ViSession session = 0;

  ViBoolean idQuery = VI_TRUE;
  ViBoolean reset = VI_TRUE;

  // Edit resource and options as needed. Resource is ignored if option Simulate=true.
  ViRsrc resource = (ViRsrc)rp.GetResourceName().c_str();

  std::cout << "Resource " << resource << std::endl;

  // An input signal is necessary if the example is run in non simulated mode, otherwise
  // the acquisition will time out.
  ViConstString options = rp.GetOptChar();

  std::cout << "Option string " << options << std::endl;

  // Initialize the driver. See driver help topic "Initializing the IVI-C Driver" for additional information.
  checkApiCall(AgMD2_InitWithOptions(resource, idQuery, reset, options, &session), "AgMD2_InitWithOptions");
  SetSession(session);
  printf("Driver initialized with session %u\n",session);

  
  ViChar str[128] = { '\0' };
  // Read and output a few attributes.
  checkApiCall(AgMD2_GetAttributeViString(session, "", AGMD2_ATTR_SPECIFIC_DRIVER_PREFIX, sizeof(str), str), "AgMD2_GetAttributeViString(AGMD2_ATTR_SPECIFIC_DRIVER_PREFIX)");
  printf("Driver prefix:      %s\n", str);
  checkApiCall(AgMD2_GetAttributeViString(session, "", AGMD2_ATTR_SPECIFIC_DRIVER_REVISION, sizeof(str), str), "AgMD2_GetAttributeViString(AGMD2_ATTR_SPECIFIC_DRIVER_REVISION)");
  printf("Driver revision:    %s\n", str);
  checkApiCall(AgMD2_GetAttributeViString(session, "", AGMD2_ATTR_SPECIFIC_DRIVER_VENDOR, sizeof(str), str), "AgMD2_GetAttributeViString(AGMD2_ATTR_SPECIFIC_DRIVER_VENDOR)");
  printf("Driver vendor:      %s\n", str);
  checkApiCall(AgMD2_GetAttributeViString(session, "", AGMD2_ATTR_SPECIFIC_DRIVER_DESCRIPTION, sizeof(str), str), "AgMD2_GetAttributeViString(AGMD2_ATTR_SPECIFIC_DRIVER_DESCRIPTION)");
  printf("Driver description: %s\n", str);
  checkApiCall(AgMD2_GetAttributeViString(session, "", AGMD2_ATTR_INSTRUMENT_MODEL, sizeof(str), str), "AgMD2_GetAttributeViString(AGMD2_ATTR_INSTRUMENT_MODEL)");
  printf("Instrument model:   %s\n", str);
  checkApiCall(AgMD2_GetAttributeViString(session, "", AGMD2_ATTR_INSTRUMENT_FIRMWARE_REVISION, sizeof(str), str), "AgMD2_GetAttributeViString(AGMD2_ATTR_INSTRUMENT_FIRMWARE_REVISION)");
  printf("Firmware revision:  %s\n", str);
  checkApiCall(AgMD2_GetAttributeViString(session, "", AGMD2_ATTR_INSTRUMENT_INFO_SERIAL_NUMBER_STRING, sizeof(str), str), "AgMD2_GetAttributeViString(AGMD2_ATTR_INSTRUMENT_INFO_SERIAL_NUMBER_STRING)");
  printf("Serial number:      %s\n", str);
  checkApiCall(AgMD2_GetAttributeViString(session, "", AGMD2_ATTR_INSTRUMENT_INFO_OPTIONS, sizeof(str), str), "AgMD2_GetAttributeViString(AGMD2_ATTR_INSTRUMENT_INFO_OPTIONS)");
  printf("Instrument options: %s\n", str);
  ViInt32 channelcount;
  checkApiCall(AgMD2_GetAttributeViInt32(session, "", AGMD2_ATTR_CHANNEL_COUNT, &channelcount), "AgMD2_GetAttributeViInt32(AGMD2_ATTR_CHANNEL_COUNT)");
  printf("Channel count:      %i\n", channelcount);
  ViInt32 bits;
  checkApiCall(AgMD2_GetAttributeViInt32(session, "", AGMD2_ATTR_INSTRUMENT_INFO_NBR_ADC_BITS, &bits), "AgMD2_GetAttributeViInt32(AGMD2_ATTR_INSTRUMENT_INFO_NBR_ADC_BITS)");
  printf("ADC bits:           %i\n", bits);
  printf("Session:            %u\n\n", session);
}

void AgMD2_GPM::configure_channels()
{
  ViSession session = GetSession();
  ViInt32 coupling = AGMD2_VAL_VERTICAL_COUPLING_DC;//same for all channels

  // Configure the acquisition.

  auto itr = cpm.begin();
  while (itr != cpm.end())
    {
      ViUInt8 num = itr->first;
      ChannelParams cp = itr->second;
      std::cout << "ChannelParams.Complete() = " << cp.Complete() << std::endl;
      if (!cp.GetUseChannel())
	{
	  itr++;
	  continue;
	}
      printf("Configuring acquisition -- Channel%i\n",num);
      printf("Range:           %g\n", cp.GetChannelRange());
      printf("Offset:          %g\n", cp.GetChannelOffset());
      printf("Coupling:        %s\n", coupling ? "DC" : "AC");
      checkApiCall(AgMD2_ConfigureChannel(session, 
					  cp.GetChannelName(), 
					  cp.GetChannelRange(),
					  cp.GetChannelOffset(),
					  coupling,
					  cp.GetUseChannel()),
		   "AgMD2_ConfigureChannel");
      itr++;
    }
}

void AgMD2_GPM::configure_acquisition()
{
  ViSession session = GetSession();
  printf("\nNumber of records:  %li\n", rp.GetNumRecords());
  printf("Record size:        %li\n", rp.GetRecordSize());
  printf("Sample rate:        %g\n", rp.GetSampleRate());
  checkApiCall(AgMD2_SetAttributeViInt64(session, "", AGMD2_ATTR_NUM_RECORDS_TO_ACQUIRE, 1), "AgMD2_SetAttributeViInt64(AGMD2_ATTR_NUM_RECORDS_TO_ACQUIRE)");
  checkApiCall(AgMD2_SetAttributeViInt64(session, "", AGMD2_ATTR_RECORD_SIZE, rp.GetRecordSize()), "AgMD2_SetAttributeViInt64(AGMD2_ATTR_RECORD_SIZE)");
  checkApiCall(AgMD2_SetAttributeViReal64(session, "", AGMD2_ATTR_SAMPLE_RATE, rp.GetSampleRate()), "AgMD2_SetAttributeViReal64(AGMD2_ATTR_SAMPLE_RATE)");

  // NOTE: The ConfigureAcquisition method (below) doesn't work for some reason, so set the values manually
  //checkApiCall(AgMD2_ConfigureAcquisition(session, 1, rp.GetRecordSize(), rp.GetSampleRate()), "AgMD2_ConfigureAcquisition");
}

void AgMD2_GPM::configure_triggers()
{
  // Configure the trigger.
  ViSession session = GetSession();

  ViReal64 triggerdelay = rp.GetTriggerDelay()*rp.GetRecordSize()/rp.GetSampleRate();

  auto itr = cpm.begin();
  while (itr != cpm.end())
    {
      ViUInt8 num = itr->first;
      ChannelParams cp = itr->second;

      if (!cp.GetActiveTrigger())
	{
	  itr++;
	  continue;
	}

      bool activeTrigger = cp.GetActiveTrigger();
      printf("Configuring trigger %i -- %s\n",num,cp.GetChannelName());
      printf("Source:          %s\n", cp.GetTriggerSource());
      printf("Level:           %g\n", cp.GetTriggerLevel());
      printf("Slope:           %i\n", cp.GetTriggerSlope());
      printf("Delay:           %g\n", triggerdelay);
      printf("Active Trigger:  %s\n", (activeTrigger) ? "true" : "false");
      checkApiCall(AgMD2_ConfigureEdgeTriggerSource(session,
						    cp.GetTriggerSource(),
						    cp.GetTriggerLevel(),
						    cp.GetTriggerSlope()),
		   "AgMD2_ConfigureEdgeTriggerSource");
      checkApiCall(AgMD2_SetAttributeViInt32(session,
					     cp.GetTriggerSource(),
					     AGMD2_ATTR_TRIGGER_TYPE,
					     AGMD2_VAL_EDGE_TRIGGER),
		   "AgMD2_SetAttributeViInt32(AGMD2_ATTR_TRIGGER_TYPE)");
      checkApiCall(AgMD2_SetAttributeViInt32(session,
					     cp.GetTriggerSource(),
					     AGMD2_ATTR_TRIGGER_COUPLING,
					     AGMD2_VAL_TRIGGER_COUPLING_DC),
		   "AgMD2_SetAttributeViInt32(AGMD2_ATTR_TRIGGER_COUPLING)");

      checkApiCall(AgMD2_SetAttributeViString(session, 
					      "", 
					      AGMD2_ATTR_ACTIVE_TRIGGER_SOURCE, 
					      cp.GetTriggerSource()), 
		   "AgMD2_SetAttributeViString(AGMD2_ATTR_ACTIVE_TRIGGER_SOURCE)"); 
  
      itr++;
    }
  
  checkApiCall(AgMD2_SetAttributeViReal64(session, "", AGMD2_ATTR_TRIGGER_DELAY, triggerdelay), "AgMD2_SetAttributeViReal64(AGMD2_ATTR_TRIGGER_DELAY)");
}

void AgMD2_GPM::calibrate()
{
  ViSession session = GetSession();
  // Calibrate the instrument.
  printf("Performing self-calibration\n");
  checkApiCall(AgMD2_SelfCalibrate(session), "AgMD2_SelfCalibrate");
}


void AgMD2_GPM::initialize_parameters()
{
  rp.Reset();
  cpm.clear();
  
  std::string line;
  std::ifstream defparams("/home/argonshef/LArAnalysis/srcs/SubSystems/DAQConfig/params.txt");
  //std::ifstream defparams("/home/argonshef/LArPurityPMT/WorkDir/params.txt");
  std::string partype; // put global, or channel number
  std::string parname;
  std::string parval;
  if (defparams.is_open())
    while (std::getline(defparams,line))
      {
	std::stringstream ss(line);
	ss >> partype;
	
	if (partype[0] == '#') continue;

	ss >> parname >> parval;

	if (partype == "global") 
	  {
	    if (parname == "Draw")
	      {
		if (parval == "true")
		  {
		    rp.SetDraw(true);
		  }
		else if (parval == "false")
		  {
		    rp.SetDraw(false);
		  }
	      }
	    if (parname == "ResourceName")
	    {
	      rp.SetResourceName(parval);
	    }
	    else if (parname == "NumRecords") 
	      {
		rp.SetNumRecords(std::stoll(parval));
	      }
	    else if (parname == "RecordSize") 
	      {
		rp.SetRecordSize(std::stoll(parval));
	      }
	    else if (parname == "SampleRate") 
	      {
		rp.SetSampleRate(std::stold(parval));
	      }
	    else if (parname == "TimeoutInMS") 
	      {
		rp.SetTimeoutInMS(std::stol(parval));
	      }
	    else if (parname == "TriggerDelay") 
	      {
		rp.SetTriggerDelay(std::stod(parval));
	      }
	    else if (parname == "EdgeDetectThreshold") 
	      {
		rp.SetEdgeThreshold(std::stoll(parval));
	      }
	    else if (parname == "BaselineCalcFraction") 
	      {
		rp.SetBaselineFraction(std::stod(parval));
	      }
	    else if (parname == "TriggerSigmaThreshold") 
	      {
		rp.SetTriggerSigmaThreshold(std::stod(parval));
	      }
	    else if (parname == "IntegrationSize")
	      {
		rp.SetIntegrationSize(std::stod(parval));
	      }
	  }
	else 
	  {
	    ViUInt8 num = std::stoi(partype);

	    if (num >= 1 && num <= 8)
	      {
		if (cpm.count(num) == 0)
		  {
		    cpm.emplace(num,ChannelParams(num));
		  }
		ChannelParams *cp = &cpm.at(num);

		if (parname == "UseChannel")
		  {
		    if (parval == "true") cp->UpdateUseChannel(true);
		    else if (parval == "false") cp->UpdateUseChannel(false);
		  }
		else if (parname == "ChannelNickname") 
		  {
		    cp->UpdateChannelNickname((ViString)parval.c_str());
		  }
		else if (parname == "ChannelPolarity") 
		  {
		    cp->UpdateChannelPolarity(std::stoi(parval));
		  }
		else if (parname == "ChannelRange") 
		  {
		    cp->UpdateChannelRange(std::stod(parval));
		  }
		else if (parname == "ChannelOffset") 
		  {
		    cp->UpdateChannelOffset(std::stod(parval));
		  }
		else if (parname == "TriggerLevel") 
		  {
		    cp->UpdateTriggerLevel(std::stod(parval));
		  }
		else if (parname == "TriggerSlope") 
		  {
		    cp->UpdateTriggerSlope(std::stoi(parval));
		  }
		else if (parname == "ActiveTrigger")
		  {
		    if (parval == "true") cp->UpdateActiveTrigger(true);
		    else if (parval == "false") cp->UpdateActiveTrigger(false);
		  }
	      }
	  }
      }
}

volatile sig_atomic_t sig_caught = 0;

void handler(int)
{
  sig_caught = 1;
}

int AgMD2_GPM::app()
{
  initialize_parameters();
  bool complete = true;
  if (!rp.Complete()) {
    rp.Print();
    complete = false;
  }
  auto itr = cpm.begin();
  while (itr != cpm.end())
    {
      if (!itr->second.GetUseChannel())
	{
	  itr++;
	  continue;
	}
      if (!itr->second.Complete())
	{
	  printf("Channel%i:\n",itr->first);
	  itr->second.Print();
	  complete = false;
	}
      itr++;
    }
  if (!complete) return -1;

  printf("  AgMD2_GPM\n\n");

  initialize_driver();

  configure_channels();

  configure_acquisition();

  configure_triggers();

  calibrate();

  int offsetTDC = rp.GetRecordSize()*TMath::Abs(rp.GetTriggerDelay()) - 20;
  float integralval = rp.GetIntegrationSize();

  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = handler;
  sigaction(SIGINT, &action, NULL);

  ViSession session = GetSession();

  Header head;
  std::cout << "Header size is " << sizeof(Header) << " bytes long." << std::endl;

  time_t now = time(0);
  struct tm tstruct;
  char filename[30];
  tstruct = *localtime(&now);
  strftime(filename,sizeof(filename),"LArPMT%Y%m%d_%H%M%S.dat", &tstruct);
  
  std::ofstream fileout(filename,std::ios::out|std::ios::binary);
  if (!fileout.is_open())
    {
      std::cerr << "error: open file for output failed!" << std::endl;
      return -1;
    }

  char filename2[30];
  strftime(filename2,sizeof(filename2),"tdiff_%Y%m%d_%H%M%S.txt", &tstruct);

  //std::ofstream filetdiff(filename2,std::ios::out|std::ios::app);
  //if (!filetdiff.is_open())
  //{
  //std::cerr << "error: open file for output tdiff failed!" << std::endl;
  //return -1;
  //}
  
  TCanvas* canv;
  std::map<ViUInt8, TH1I*> histMap;
  std::map<int,TH1I*> histDistMap;
  std::map<ViUInt8, TGaxis*> axMap;

  int clipmax = 0, clipmin = 0;

  if (rp.GetDraw())
    {
      canv = new TCanvas("c","DigiView",1400, 800);
      canv->Divide(2,1);

      itr = cpm.begin();
      while (itr != cpm.end())
	{
	  if (!itr->second.GetUseChannel())
	    {
	      itr++;
	      continue;
	    }
	  TString name = "h"; name += itr->first; TString nameDist = name + "Dist";
	  TString title = itr->second.GetChannelNickname();
	  int histDistsize = 256;
	  if(itr->second.GetChannelRange() == 0.25){histDistsize = 1024;} 
	  histMap.emplace(itr->first,new TH1I(name.Data(),title.Data(),rp.GetRecordSize(),0,0));
	  histDistMap.emplace(itr->first,new TH1I(nameDist.Data(),title.Data(),histDistsize,0,histDistsize));
	  axMap.emplace(itr->first,new TGaxis(1,0,1,1,0,1,510,"+L"));
	  histDistMap[itr->first]->GetXaxis()->SetCanExtend(true);
	  itr++;
	}
    }
  
  bool saturation_flag_high = false;
  bool saturation_flag_low = false;
  unsigned int success = 0;

  int i_record;
  bool print = false;

  //std::chrono::steady_clock::time_point daq_starttime = std::chrono::steady_clock::now();
  std::chrono::time_point<std::chrono::system_clock> rightnow = std::chrono::system_clock::now();
  auto TimeNow = rightnow.time_since_epoch();
  
  auto daq_starttime_seconds = std::chrono::duration_cast<std::chrono::seconds>(TimeNow);
  auto daq_starttime_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(TimeNow);

  try
    {
      for (i_record = 0; success < rp.GetNumRecords(); ++i_record)
	{
	  if (i_record % 100 == 0)
	    {
	      print = true;
	    }
	  else
	    {
	      print = false;
	    }
	  
	  if (sig_caught) break;
      
	  head.eventNumber = i_record;

	  if (print)
	    {
	      std::cout << "Event " << i_record << " -- Init" << std::flush;
	    }

	  checkApiCall(AgMD2_InitiateAcquisition(session), "AgMD2_InitiateAcquisition");

	  if (print)
	    {
	      std::cout << ", Waiting" << std::flush;
	    }
	  // this is cheating vvvvvvvvv
	  //checkApiCall(AgMD2_SendSoftwareTrigger(session), "AgMD2_SendSoftwareTrigger");

	  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	  checkApiCall(AgMD2_WaitForAcquisitionComplete(session, rp.GetTimeoutInMS()), "AgMD2_WaitForAcquisitionComplete");

	  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	  auto TriggerTime = now.time_since_epoch();

	  auto TriggerTime_seconds = std::chrono::duration_cast<std::chrono::seconds>(TriggerTime);
	  TriggerTime -= TriggerTime_seconds;
	  auto TriggerTime_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(TriggerTime);


	  float timediff_sec      = (daq_starttime_seconds - TriggerTime_seconds).count();
	  float timediff_nanosec  = (daq_starttime_nanoseconds - TriggerTime_nanoseconds).count();
 
          head.Timestamp.first = TriggerTime_seconds.count();
          head.Timestamp.second = TriggerTime_nanoseconds.count();
	  

	  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	  std::ofstream filetdiff(filename2,std::ios::out|std::ios::app);
	  filetdiff << std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "\n";
	  filetdiff.close();

	  if (print)
	    {
	      std::cout << ", Acquiring. Time since start " << timediff_sec << " sec " << timediff_nanosec <<  " nanosecond "  << std::flush;
	    }

	  checkApiCall(AgMD2_QueryMinWaveformMemory(session, 8, 1, 0, rp.GetRecordSize(), &head.memsize), "AgMD2_QueryMinWaveformMemory");
      
	  if (print)
	    {
	      std::cout << ", Writing " << std::flush;
	    }
	  
	  saturation_flag_high = false;
	  saturation_flag_low = false;

	  itr = cpm.begin();

	  while (itr != cpm.end())
	    {
	      if (!itr->second.GetUseChannel())
		{
		  itr++;
		  continue;
		}
	      head.channelNumber = itr->second.GetChannelNumber();
	      //if (!rp.GetDraw() && head.channelNumber < 5)
	      //{
	      //	  ++itr;
	      //	  continue;
	      //	}
	      ViChar* dataArray = new ViChar[head.memsize];
	      checkApiCall(AgMD2_FetchWaveformInt8(session,
						   itr->second.GetChannelName(),
						   head.memsize,
						   dataArray,
						   &head.actualPoints,
						   &head.firstValidPoint,
						   &head.initialXOffset,
						   &head.initialXTimeSeconds,
						   &head.initialXTimeFraction,
						   &head.xIncrement,
						   &head.scaleFactor,
						   &head.scaleOffset),
			   "AgMD2_FetchWaveformInt8");

	      if (rp.GetDraw())
		{
		  histMap[head.channelNumber]->Reset();
		  histMap[head.channelNumber]->GetYaxis()->SetRangeUser(-128,128);
		  histMap[head.channelNumber]->SetBins(head.actualPoints-1,head.initialXOffset,head.initialXOffset+(head.xIncrement * head.actualPoints));
		}
	    

	      int val_max = -9999;
	      int val_tot = 0;
	      if(itr->second.GetTriggerSlope() == 0){val_max = 9999;}
	      int val_n = 0;
	      int val_ped = 0; 
	      for (int i = 0; i < head.actualPoints; i++)
		{
		  int pol = itr->second.GetChannelPolarity();
		  //int pol = 1;
		  int val = pol*dataArray[i+head.firstValidPoint];
	 
		  if (rp.GetDraw())
		    {
	   
		      if(itr->second.GetTriggerSlope() == 0){if(val < val_max){val_max = val;}}
		      if(itr->second.GetTriggerSlope() == 1){if(val > val_max){val_max = val;}}
		      if(i < offsetTDC){val_ped += val; ++val_n;}
		      if(i > offsetTDC && i < (offsetTDC + integralval)){val_tot += val;}
		      histMap[head.channelNumber]->SetBinContent(i,val);
		    }
		  if (val >= 127) saturation_flag_high = true;
		  if (val <= -127) saturation_flag_low = true;
		}
	      
	      if (rp.GetDraw())
		{
		  float val_mean = val_ped/val_n;
		  
		  histDistMap[head.channelNumber]->Fill(TMath::Abs(val_tot-(integralval*val_mean)));
		}
	      
	      if (saturation_flag_high || saturation_flag_low)
		{
		  delete[] dataArray;
		  if (print)
		    {
		      std::cout << "X" << std::flush;
		    }
		  break;
		}
	      
	      //if (head.channelNumber >= 5)
	      //{
	      fileout.write((char*)&head,sizeof(Header));
	      fileout.write((char*)dataArray,head.memsize*sizeof(ViChar));
	      //}
	      delete[] dataArray;
	      if (print)
		{
		  std::cout << (int)head.channelNumber << std::flush;
		}	      
	      itr++;
	    }
	  if (print)
	    {
	      std::cout << ", Finished, " << success << " Recorded" << std::flush;
	    }

	  if (saturation_flag_high) ++clipmax;
	  if (saturation_flag_low) ++clipmin;

	  if (!saturation_flag_high && !saturation_flag_low)
	    {
	      success++;
	      
	      if (rp.GetDraw())
		{
		  int canvnumber = 1;
		  for (std::map<ViUInt8, TH1I*>::iterator hitr = histMap.begin(); hitr != histMap.end(); ++hitr) 
		    {
		      canv->cd(1);
		      histMap[hitr->first]->Draw();
		      axMap[hitr->first]->DrawAxis(gPad->GetUxmax(),gPad->GetUymin(),gPad->GetUxmax()-0.00000001,gPad->GetUymax(),gPad->GetUymin()*head.scaleFactor+head.scaleOffset,gPad->GetUymax()*head.scaleFactor+head.scaleOffset,510,"+L");
		      canv->cd(2);
		      histDistMap[hitr->first]->Draw();
		      canv->Update();
		      canv->Update();
		      canvnumber++;
		    }
		  canv->Modified();
		  canv->Update();
		}	    
	    }
	  
	  /*
	    if (i_record % 100 == 0)
	    {
	    std::cout << "Event " << i_record << ", " << success << " recorded." << std::endl;
	    }
	  */
	  if (print)
	    {
	      std::cout << std::endl << std::flush;
	    }
	}
    }
  catch (std::exception e) {}

  fileout.close();
  //filetdiff.close();

  time_t end = time(0);
  //char endtime[30];
  //struct tm tstructe;
  //tstructe = *localtime(&end);
  //strftime(endtime,sizeof(endtime),"%Y%m%d_%H%M%S", &tstructe);
  std::cout << "Time = " << end-now << " seconds" << std::endl; 
  
  std::cout << "\n " << i_record << " events recorded." << std::endl;

  std::cout << "\n " << clipmin << " events out of range LOW" << std::endl;
  std::cout << "\n " << clipmax << " events out of range HIGH" << std::endl;

  Quit();

  return 0;
}
