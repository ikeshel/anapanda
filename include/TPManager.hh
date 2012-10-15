
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// author: i.keshelashvili@unibas.ch                                    //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef TPMANAGER_HH
#define TPMANAGER_HH

#include <iostream>
#include <fstream>

#include <TObject.h>
#include <TTimer.h>
#include <TCanvas.h>
#include <TH1D.h>

#include <TPConstants.hh>
#include <TPConfig.hh>
#include <TPFileReader.hh>
#include <TPSignalExtractor.hh>

using namespace std;

class TPManager: 
  private TObject, public TPConfig, public TPFileReader
{
private:
  TCanvas* cMain;
  TTimer*  fTimer;

  bool  fbDrawOnOff;
  
protected:
  int fNBurst;
  int fDrawScale;
  
  TPSignalExtractor *fSigExt_PMT;
  TPSignalExtractor *fSigExt_AMP;
  TPSignalExtractor *fSigExt_SHP;
  
public:
  int fCurrEvt;
  
  TPManager();
  ~TPManager();

  // control functions
  //  
  void DoStart(int);
  void DoStop();
  void DoEvent(int);
  void DoDrawOnOff();
  
  // 
  //
  void Next();
  void DrawAll();

  // set
  //
  void SetDrawScale(unsigned int);

  ClassDef( TPManager, 0 ) // A basic class 
    };

#endif

