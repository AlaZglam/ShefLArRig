#include "AgMD2.h"

class PulseAnalysis
{
 public:
  PulseAnalysis() {}

  int pulseHeight(ViChar *data, ViInt64 pts, double nst, double ednt)
  {
    double max = -999;
    int max_location;
    double baseline, baseline1 = (double)data[0], basevar, basevar1 = 0, stdev;
    double currMag, prevMag = (double)data[0];
    int basecount = 0;
    bool triggerFlag = false;
    int successFlag = 0;
    for (size_t n = 0; n < (unsigned)pts; ++n)
      {
	++basecount;
	baseline = baseline1 + (data[n] - baseline1) / basecount;
	basevar = basevar1 + (data[n] - baseline1) * (data[n] - baseline);
	baseline1 = baseline;
	basevar1 = basevar;
	stdev = (basecount>1) ? std::sqrt(basevar / (basecount - 1)) : 0.0;

	if (n > 0.1*pts)
	  {
	    if (std::abs(data[n] - baseline) > nst*stdev)
	      {
		triggerFlag = true;
		for (size_t i = n-ednt; i <= n; ++i)
		  {
		    for (size_t j = n-ednt; j < i; ++j)
		      {
			currMag = std::abs(data[n] - baseline);
			if (currMag < prevMag) triggerFlag = false;
			prevMag = currMag;
		      }
		  }
		if (triggerFlag)
		  {
		    setBaseline(baseline);
		    setStdev(stdev);
		    successFlag = 1;
		    break;
		  }
	      }
	  }
      }
    for (size_t n = 0; n < (unsigned)pts ; n++)
      {
	if (data[n] > max)
	  {
	    max = data[n];
	    max_location = n;
	  }
      }
    double maxavg = 0;
    for (size_t n = max_location-2; n <= (unsigned)max_location+2 ; n++)
      {
	maxavg += data[n];
      }
    setPulseHeight(maxavg/5.0);
    if (successFlag == 1) return 1;
    return -1;
  }

  double getBaseline() { return bsl; }
  double getStdev() { return std; }
  double getPulseHeight() { return ph; }

 private:
  double bsl;
  double std;
  double ph;
  void setBaseline(double b) { bsl = b; }
  void setStdev(double s) { std = s; }
  void setPulseHeight(double p) { ph = p; }
};
