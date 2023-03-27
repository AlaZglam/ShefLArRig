#include "AgMD2.h"
#include <string>

class RunParams
{
  std::string resourceName; bool setRN = false;
  ViInt64 numRecords; bool setNR = false;
  ViInt64 recordSize; bool setRS = false;
  ViReal64 sampleRate; bool setSR = false;
  // set timeout to -1 for no timeout
  ViInt32 timeoutInMS; bool setTIM = false;
  ViReal64 triggerDelay; bool setTD = false;
  ViConstString optChar; bool setOC = false;
  ViInt64 edgeThresh; bool setET = false;
  ViReal64 baseFrac; bool setBF = false;
  ViReal64 trigSigThresh; bool setTST = false;
  bool draw; bool setDRAW = false;
  float integrationsize=1; bool setIS = false;

 public:
  RunParams() {}

  void Reset()
  {
    setRN = false;
    setNR = false;
    setRS = false;
    setSR = false;
    setTIM = false;
    setTD = false;
    setOC = false;
    setET = false;
    setBF = false;
    setTST = false;
    setDRAW = false;
    setIS = false;
  }
  
  std::string GetResourceName()   { return resourceName; }
  ViInt64 GetNumRecords()    { return numRecords;   }
  ViInt64 GetRecordSize()    { return recordSize;   }
  ViReal64 GetSampleRate()   { return sampleRate;   }
  ViInt32 GetTimeoutInMS()   { return timeoutInMS;  }
  ViReal64 GetTriggerDelay() { return triggerDelay; }
  ViConstString GetOptChar() { return optChar;      }
  ViInt64 GetEdgeThreshold() { return edgeThresh;   }
  ViReal64 GetBaselineFraction() { return baseFrac; }
  ViReal64 GetTriggerSigmaThreshold() { return trigSigThresh; }
  bool GetDraw() { return draw; }
  float GetIntegrationSize(){return integrationsize;}

  void SetResourceName(std::string rn)
  {
    resourceName = rn;
    setRN = true;
    optChar = "Simulate=false, DriverSetup= Model=U5309A";
    setOC = true;
  }
  void SetNumRecords(ViInt64 nr)     { numRecords = nr;   setNR = true;  }
  void SetRecordSize(ViInt64 rs)     { recordSize = rs;   setRS = true;  }
  void SetSampleRate(ViReal64 sr)    { sampleRate = sr;   setSR = true;  }
  void SetTimeoutInMS(ViInt32 t)     { timeoutInMS = t;   setTIM = true; }
  void SetTriggerDelay(ViReal64 td)  { triggerDelay = td; setTD = true;  }
  void SetEdgeThreshold(ViInt64 et)  { edgeThresh = et;   setET = true;  }
  void SetBaselineFraction(ViReal64 bf) { baseFrac = bf; setBF = true; }
  void SetTriggerSigmaThreshold(ViReal64 tst) { trigSigThresh = tst; setTST = true; }
  void SetDraw(bool d) { draw = d; setDRAW = true; }
  void SetIntegrationSize(float is){integrationsize = is; setIS = true;}

  bool Complete() { return (setRN && setNR && setRS && setSR && setTIM && setTD && setOC && setET && setBF && setTST && setDRAW); }

  void Print()
  {
    printf("Incomplete or invalid RunParams:\n");
    if (!setRN)  printf("  ResourceName\n");
    if (!setNR)  printf("  NumRecords\n");
    if (!setRS)  printf("  RecordSize\n");
    if (!setSR)  printf("  SampleRate\n");
    if (!setTIM) printf("  TimeoutInMS\n");
    if (!setTD)  printf("  TriggerDelay\n");
    if (!setOC)  printf("  OptChar = %s\n",optChar);
    if (!setET)  printf("  EdgeDetectThreshold\n");
    if (!setBF)  printf("  BaselineCalcFraction\n");
    if (!setTST) printf("  TriggerSigmaThreshold\n");
    if (!setDRAW) printf("  Draw\n");
    if (!setIS)  printf("  Integration size not specified! sett to one"); 
    printf("\n");
  }
};
