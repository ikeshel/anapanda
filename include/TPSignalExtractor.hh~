
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// author: i.keshelashvili@unibas.ch                                    //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef TPSIGNALEXTRACTOR_HH
#define TPSIGNALEXTRACTOR_HH

#include <iostream>
#include <fstream>

#include <TH1D.h>
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

protected:

  TH1D* hHist;

  //
  //
  TH1D * hSignalDist[MAX_NHITS];

  //
  //
  TF1  * fOffset[MAX_NHITS];

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

  // Get's
  //
  int    GetNumbOfHits()      { return fNHits; };

  TF1*   GetOffset(int i)     { return fOffset[i]; };

  TH1D*  GetSignalDist(int i) { return hSignalDist[i]; };

  TLine* GetThresholdLine()   { return lThreshold; };
  TLine* GetStartLine(int i)  { return lStart[i]; };
  TLine* GetStoppLine(int i)  { return lStopp[i]; };
  
  double GetIntegral(int, TH1D*);

  ClassDef( TPSignalExtractor, 0 ) // A basic class 
    };

#endif

