
/////////////////////////////////////////////////////////////////
//                                                             //
// author: i.keshelashvili@unibas.ch                           //
//                                                             //
//                                                             //
/////////////////////////////////////////////////////////////////

#ifndef TPSIGNALEXTRACTOR_HH
#define TPSIGNALEXTRACTOR_HH

#include <iostream>
#include <fstream>

#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TLine.h>

#include <TPConstants.hh>

using namespace std;

class TPSignalExtractor
{
private:
  int fMaxHits; // maximum number of hits
  int fNHits;   // store number of hits

  int fIntegrationWidth;
  int fBaselineWidth;
  int fRiseTime;

protected:
  //
  //
  TH1D* hHist;

  //
  //
  TH1D * hSignalDist[MAX_NHITS];

  // sweep
  //
  TH2D     *hAmpVsDel[MAX_NHITS];
  TProfile *hAmpVsDel_px[MAX_NHITS];
  //  TF1      *fAmpVsDel_px[MAX_NHITS];
  
  //
  //
  TF1  * fOffset[    MAX_NHITS];
  TF1  * fAmplitude[ MAX_NHITS];
  TF1  * fExpo[      MAX_NHITS];

  // threshold, start and stop lines
  //
  TLine * lThreshold;
  TLine * lStart[MAX_NHITS];
  TLine * lStopp[MAX_NHITS];


public:
  TPSignalExtractor(const char*, TH1D*);
  ~TPSignalExtractor();

  // 
  //
  void FindHits(int, TH1D*);

  // Set's
  //
  void SetMaxHits(int);
  void SetIntegrationWidth(int);
  void SetBaselineWidth(int);
  void SetRiseTime(int);

  void SetStartLine(int, double);

  // Get's
  //
  int    GetNumbOfHits()      { return fNHits; };

  TF1 *   GetOffsetFunction(int i) { return fOffset[i];    };
  TF1 *   GetAmplitudeFunct(int i) { return fAmplitude[i]; };

  TH1D * GetSignalDist(int i) { return hSignalDist[i]; };
  TH2D * GetAmpVsDel(int i)   { return hAmpVsDel[i];   };

  TLine * GetThresholdLine()   { return lThreshold; };
  TLine * GetStartLine(int i)  { return lStart[i];  };
  TLine * GetStoppLine(int i)  { return lStopp[i];  };
  
  double GetIntegral( int, TH1D*, double); // 
  double GetAmplitude(int, TH1D*, double); // 

  ClassDef( TPSignalExtractor, 0 ) // A basic class 
    };

#endif

