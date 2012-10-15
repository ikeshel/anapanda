
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// author: i.keshelashvili@unibas.ch                                    //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef TPFILEREADER_HH
#define TPFILEREADER_HH

#include <iostream>
#include <fstream>

#include <TROOT.h>
#include <TFile.h>
#include <TH2I.h>
#include <TH1D.h>

#include <TPConstants.hh>

using namespace std;

class TPFileReader
{
private:
  TFile  *fFile;
  Char_t szFileName[128];

protected:
  int fNChannel;    // number of readout channel
  int fTotaEvt;     // total number of events in file
  int fTotaEvtPrc;  // 1% of total number of events

  TH2I * h2D[   MAX_CHANNEL ];
  TH1D * hProj[ MAX_CHANNEL ];

public:
  TPFileReader();
  ~TPFileReader();

  //
  //
  void ReadHistoFile(char*, int);
  //  void ReadBinaryFile();
  
  // Get's
  //
  TH2I* Get2DHisto(int i) { return h2D[i]; };
  char* GetFileName()     { return szFileName; };

  TH1D* GetProjection(int,int);

  ClassDef( TPFileReader, 0 ) // A basic class 
    };

#endif

