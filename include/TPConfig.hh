
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// author: i.keshelashvili@unibas.ch                                    //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef _TPCONFIG_HH_
#define _TPCONFIG_HH_

#include "stdio.h"

#include <iostream>
#include <fstream>

#include <TSystem.h>
#include <TString.h>

using namespace std;

const int MAX_LINE = 1000;

//
// 
class TPConfig
{
private:
  Int_t   nLine;
  TString strLine[MAX_LINE];
  TString ExtractName(TString);

  void ReadConfigFile(const char*);  

protected:

  
public:
  TPConfig();
  ~TPConfig();
  
  TString      GetConfigName( TString );

  unsigned int GetConfigHex( TString );

  int          GetConfigInt( TString );

  float        GetConfigFloat( TString );

  //
  //
  void WriteConfigFile();

  ClassDef(TPConfig, 0)
};

#endif
