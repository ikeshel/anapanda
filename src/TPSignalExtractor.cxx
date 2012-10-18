///////////////////////////////////////////////////////////////////
//                                                               //
// author: i.keshelashvili@unibas.ch                             //
//                                                               //
//                                                               //
//                                                               //
///////////////////////////////////////////////////////////////////

#include "TPSignalExtractor.hh"

ClassImp( TPSignalExtractor )

//------------------------------------------------------------------------------
  TPSignalExtractor::TPSignalExtractor(const char* name, TH1D* hh)
{
  fMaxHits = MAX_NHITS;

  hHist = hh;

  // init start and stop
  //
  char szName[32];
  for( Int_t i=0; i<MAX_NHITS; i++)
    {
      // signal distribution histo
      //
      sprintf(szName, "%s_%i", name, i);
      hSignalDist[i] = new TH1D(szName, "", 15000, 0, 15000);
      hSignalDist[i]->SetStats(0);
      hSignalDist[i]->SetLineColor(myColor[i]+2);
      hSignalDist[i]->SetFillColor(myColor[i]);

      // amplitude vs delay 2D histos
      //
      sprintf(szName, "%s_AmVsDt_%i", name, i);
      hAmpVsDel[i] = new TH2D( szName, ";Delay [#mus];Norm. Amp. [%]", 
			       105, 0., 10.5,   100, 80, 120);
      hAmpVsDel[i]->SetMarkerColor(myColor[i]);


      sprintf(szName, "fOffset_%i", i);
      fOffset[i] = new TF1(szName, "pol0");
      fOffset[i]->SetLineColor(myColor[i]);

      sprintf(szName, "fAmp_%i", i);
      fAmplitude[i] = new TF1(szName, "pol0");
      fAmplitude[i]->SetLineColor(myColor[i]);

      sprintf(szName, "fExpo_%i", i);
      fExpo[i] = new TF1(szName, "pol0");
      fExpo[i]->SetLineColor(myColor[i]);

      lStart[i] = new TLine(0,0,0,4100);
      lStart[i]->SetLineColor(myColor[i]);

      lStopp[i] = new TLine(0,0,0,4100);
      lStopp[i]->SetLineColor(myColor[i]);
    }

  lThreshold = new TLine(0,0,7000,0);
  lThreshold->SetLineColor(2);
  lThreshold->SetLineStyle(3);
}

//------------------------------------------------------------------------------
TPSignalExtractor::~TPSignalExtractor()
{  
}

//------------------------------------------------------------------------------
void TPSignalExtractor::SetMaxHits(int n)
{
  fMaxHits = n;
  return;
}

//------------------------------------------------------------------------------
void TPSignalExtractor::SetIntegrationWidth(int w)
{
  fIntegrationWidth = w;
  return;
}

//------------------------------------------------------------------------------
void TPSignalExtractor::SetBaselineWidth(int w)
{
  fBaselineWidth = w;
  return;
}

//------------------------------------------------------------------------------
void TPSignalExtractor::SetRiseTime(int rt)
{
  fRiseTime = rt;
  return;
}

//------------------------------------------------------------------------------
void TPSignalExtractor::SetStartLine(int n, double t)
{
  lStart[n]->SetX1( t ); 
  lStart[n]->SetX2( t ); 

  lStopp[n]->SetX1( t +fIntegrationWidth ); 
  lStopp[n]->SetX2( t +fIntegrationWidth ); 

  return;
}

//------------------------------------------------------------------------------
void TPSignalExtractor::FindHits(int threshold, TH1D* hHist)
{  
  // threshold : 
  // hHist     : input histogram

  // |___________________________________________>
  // | ___    ____________    _____________
  // |    \  /            \  /
  // | ----\/--------------\/------ threshold
  // |     1               2   
  //  

  if( !hHist->GetEntries() ) return;

  int width   = 1; 
  int average = 4100;

  //
  //
  lThreshold->SetY1(threshold);
  lThreshold->SetY2(threshold);
   
  fNHits=0;

  // find start
  //
  for( int bin=(2*width+1); bin<hHist->GetNbinsX(); bin++)
    {
      average=hHist->Integral(bin-width, bin+width)/(2*width+1);
      
      if( average < threshold )
	{
    	  lStart[fNHits]->SetX1( hHist->GetBinCenter( bin-width));
    	  lStart[fNHits]->SetX2( hHist->GetBinCenter( bin-width));
	  
	  fNHits++;
	  //	  if( fNHits > fMaxHits ) return;

	  bin += (fBaselineWidth+fIntegrationWidth);
	}
    }

  return;
}

//------------------------------------------------------------------------------
double TPSignalExtractor::GetIntegral(int n, TH1D* hHist, double scale)
{
  // ______________________________________________>
  // |          |             |    | signal
  // |          |<--       -->|<-->|
  // |          |    width    |   offse
  // |        start          stop

  //  if( !hHist->GetEntries() ) return 0;

  static int start;
  static int stopp;
  static int offse = 5;
  
  start = lStart[n]->GetX1() -offse -fBaselineWidth;
  stopp = lStart[n]->GetX1() -offse;
  
  fOffset[n]->SetRange( start, stopp );
  hHist->Fit( fOffset[n], "+RQ0");
  
  start = lStart[n]->GetX1() -offse;
  stopp = lStart[n]->GetX1() -offse +fIntegrationWidth;

  lStart[n]->SetX1(start); // redefinition of start line
  lStart[n]->SetX2(start);
  
  lStopp[n]->SetX1(stopp); // stop line
  lStopp[n]->SetX2(stopp);
  
  fAmpl[n] = ( fOffset[n]->Integral(start, stopp) - 
	       hHist->Integral(start, stopp) );

  fAmpl[n] /= scale;
  hSignalDist[n]->Fill(fAmpl[n]);
    
  return fAmpl[n];
}

//------------------------------------------------------------------------------
double TPSignalExtractor::GetAmplitude(int n, TH1D* hHist, double scale)
{
  /*
  // ^     a2  __
  // |  a1 __    |\
  // |       |\  | \
  // |       |  \|  \
  // |_______|       \________
  // ______________________________>
  */
  if( !hHist->GetEntries() ) return 0;

  static int start;
  static int stopp;
  static int offse = 5;
  
  // extract offset
  // 
  start = lStart[n]->GetX1() -offse -fBaselineWidth;
  stopp = lStart[n]->GetX1() -offse;
  
  fOffset[n]->SetRange( start, stopp );
  hHist->Fit( fOffset[n], "+RQ0");
  
  // redefine start stop
  //
  start = lStart[n]->GetX1() -offse ;
  stopp = lStart[n]->GetX1() -offse +fRiseTime;

  lStart[n]->SetX1(start); // redefinition of start line
  lStart[n]->SetX2(start);
  
  lStopp[n]->SetX1(stopp); // stop line
  lStopp[n]->SetX2(stopp);
  
  // finding the maximum
  //
  static double amp;
  for(int bin=start; bin<(stopp-fIntegrationWidth); bin++)
    {
      amp = hHist->Integral(bin, bin+fIntegrationWidth);
      if( fAmpl[n] < amp )
	{
	  fAmpl[n] = amp;
	  fAmplitude[n]->SetRange(bin-1, bin+fIntegrationWidth);
	}
    }
  
  //  fAmpl[n]  = fAmpl[n]/(fIntegrationWidth+1);
  fAmpl[n]  = fAmpl[n];
  fAmplitude[n]->SetParameter(0, fAmpl[n]/(fIntegrationWidth+1));

  //  fAmpl[n] -= fOffset[n]->Eval(0);
  fAmpl[n] = fAmpl[n]-fOffset[n]->Eval(0)*(fIntegrationWidth+1);
  
  fAmpl[n] /= scale;
  hSignalDist[n]->Fill(fAmpl[n]);
    
  return fAmpl[n];
}

//------------------------------------------------------------------------------
void TPSignalExtractor::FillAmpVsDel()
{
  // in case of only 2 hits !!!
  //
  fTime[0] = (lStopp[0+1]->GetX1() - lStopp[0]->GetX1())*0.002; // 500MS/s
  hAmpVsDel[0]->Fill( fTime[0], fAmpl[0]);
  hAmpVsDel[1]->Fill( fTime[0], fAmpl[1]);

  // printf(" hAmpVsDel : %f  %f  %f \n", fTime[0], fAmpl[0], fAmpl[1]);

  return;
}

//------------------------------------------------------------------------------
void TPSignalExtractor::FillAmpVsDelNorm100(double * norm)
{
  // in case of only 2 hits !!!
  //
  fTime[0] = (lStopp[0+1]->GetX1() - lStopp[0]->GetX1())*0.002; // 500MS/s
  hAmpVsDel[0]->Fill( fTime[0], fAmpl[0]/norm[0]*100);
  hAmpVsDel[1]->Fill( fTime[0], fAmpl[1]/norm[1]*100);

  // printf(" hAmpVsDel : %f  %f  %f \n", fTime[0], fAmpl[0], fAmpl[1]);

  //
  //
//   for(int i=0; i<(fNHits-1); i++)
//     fTime[i] = (lStopp[i+1]->GetX1() - lStopp[i]->GetX1())*0.002; // 500MS/s

//   for(int i=0;i<fNHits;i++)
//     {
//       hAmpVsDel[i]->Fill( fTime[i], 100);
//     }
  return;
}
