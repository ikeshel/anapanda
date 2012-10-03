//////////////////////////////////////////////////////////////////////////
//                                                                      //
// author: i.keshelashvili@unibas.ch                                    //
//                                                                      //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TPSignalExtractor.hh"

ClassImp( TPSignalExtractor )

//-----------------------------------------------
  TPSignalExtractor::TPSignalExtractor(char name[32], TH1D* hh)
{
  fMaxHits = MAX_NHITS;

  hHist = hh;

  // init start and stop
  //
  char szName[32];
  for( Int_t i=0; i<MAX_NHITS; i++)
    {
      sprintf(szName, "%s_%i", name, i);
      hSignalDist[i] = new TH1D(szName, "", 4100, 0, 4100);

      sprintf(szName, "fOffset_%i", i);
      fOffset[i] = new TF1(szName, "pol0");
      fOffset[i]->SetLineColor(myColor[i]);


      lStart[i] = new TLine(0,0,0,4100);
      lStart[i]->SetLineColor(myColor[i]);

      lStopp[i] = new TLine(0,0,0,4100);
      lStopp[i]->SetLineColor(myColor[i]);
    }

  lThreshold = new TLine(0,0,7000,0);
  lThreshold->SetLineColor(2);
  lThreshold->SetLineStyle(3);
}

//-----------------------------------------------
TPSignalExtractor::~TPSignalExtractor()
{  
  
}

//-----------------------------------------------
void TPSignalExtractor::SetMaxHits(int n)
{
  fMaxHits = n;
  return;
}

//-----------------------------------------------
void TPSignalExtractor::SetIntegrationWidth(int w)
{
  fIntegrationWidth = w;
  return;
}

//-----------------------------------------------
void TPSignalExtractor::FindHits(int threshold, TH1D* hHist)
{  
  // hHist     : input histogram
  // threshold : 
  // edge      : 0 - falling edge; 1 - rising edge

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

	  bin += 200;
	}
    }

  return;
}

//-----------------------------------------------
double TPSignalExtractor::GetIntegral(int n, TH1D* hHist)
{
  // ______________________________________________>
  // |          |             |    | signal
  // |           <--       --> <-->
  // |          |    width    |   offse
  // |        start          stop

  if( !hHist->GetEntries() )
    return 0;

  static int start;
  static int stopp;
  static int offse = 5;
  
  start = lStart[n]->GetX1() -offse -fIntegrationWidth;
  stopp = lStart[n]->GetX1() -offse;
  
  fOffset[n]->SetRange( start, stopp );
  hHist->Fit( fOffset[n], "+RQ0");
  
  start = lStart[n]->GetX1() -offse ;
  stopp = lStart[n]->GetX1() -offse +fIntegrationWidth;

  lStart[n]->SetX1(start); // redefinition of start line
  lStart[n]->SetX2(start);
  
  lStopp[n]->SetX1(stopp); // stop line
  lStopp[n]->SetX2(stopp);
  
  double amp = ( fOffset[n]->Eval(0)*fIntegrationWidth - 
		 hHist->Integral(start, stopp) );

  hSignalDist[n]->Fill(amp/10);
    
  return amp;
}
