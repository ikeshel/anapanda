
#include <stdlib.h>

#include <TROOT.h>
#include <TSystem.h>
#include <TTimer.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH2I.h>
#include <TH1D.h>
#include <TF1.h>
#include <TLine.h>
#include <TMarker.h>
#include <TProfile.h>

//#define SHOW_TIME_DIST

//#define GEN_MEASUREMENT
#define PMT_MEASUREMENT

//
//
const Color_t myColor[] = {kRed, kGreen+1, kRed+2, kGreen+2, kRed+1, kGreen+1};

const Double_t FixedSlope = -3.39865e-05; // 300mV

const Double_t SamplingRate = 500.0e+6; // 500MS/s

const Int_t MinStartDist = 550; // minimum distance between starts

const Int_t StartOffset = 30; // time interval between threshold start and baseline

const Int_t N_CHANNEL = 3; // SADC channel
const Int_t N_BURST   = 2; // number of bursts

Int_t event;
Int_t event_first;
Int_t event_lastt;

Int_t start[N_BURST];
Int_t stopp[N_BURST];

//
TTimer ftimer("Next()", 1);

Double_t BurstDelay;
Double_t BurstRate;

//
//
TCanvas *c2D;
TCanvas *cMain;

#ifdef SHOW_TIME_DIST
TCanvas *cTimeDist;
#endif

TFile  *file; //
Char_t szFileName[64]; //
Char_t szName[64];
Char_t szExte[64];
Char_t szLine[128];

// main histo
//
TH2I *h2D[N_CHANNEL]; //
TH1D *hProj[N_CHANNEL];

// sweep
//
TH2D     *hAmpVsDel[N_CHANNEL*N_BURST];
TProfile *hAmpVsDel_px[N_CHANNEL*N_BURST];
TF1      *fAmpVsDel_px[N_BURST];

Double_t NormPMTVsDel[N_BURST];
Double_t NormAmpVsDel[N_BURST];
Double_t NormShpVsDel[N_BURST];

void SetNormAmpVsDel(Double_t d1, Double_t d2) { NormAmpVsDel[0]=d1; NormAmpVsDel[1]=d2; }

// start-stop
//
TLine *lStart[N_BURST];

TLine *lStartPMT[2*N_BURST];
TLine *lStoppPMT[2*N_BURST];

TLine *lStartShp[2*N_BURST];
TLine *lStoppShp[2*N_BURST];

// signal distributions
//
Double_t NormPMTSignal[N_BURST]={0};
Double_t NormAmpSignal[N_BURST]={0};
Double_t NormShpSignal[N_BURST]={0};

Double_t PMTSignal[N_BURST];
Double_t AmpSignal[N_BURST];
Double_t ShpSignal[N_BURST];

// PMT
//
TF1     *fPMTOffset[N_BURST];
TMarker *mPMTSignal[2*N_BURST];

// Amp
//
TF1     *fAmpOffset[N_CHANNEL];
TF1     *fAmpSignal[N_BURST];
TMarker *mAmpSignal[2*N_BURST];

// Shp
//
TF1     *fShpOffset[N_BURST];
TMarker *mShpSignal[2*N_BURST];

//
//
TH1D   *hSignalDist[N_CHANNEL*N_BURST];
TF1    *fSignalDist[N_CHANNEL*N_BURST];

TH1D   *hSignalVsEvent[8];

TH2D   *hPMTVsAmp;


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
Int_t InThr[N_BURST]; // thresholds for PMT
TLine *lThreshold[N_BURST];

void SetThreshold(Int_t th_0, Int_t th_1) 
{
  InThr[0] = th_0; 
  InThr[1] = th_1;
  
  lThreshold[0]->SetY1(InThr[0]);
  lThreshold[0]->SetY2(InThr[0]);

  lThreshold[1]->SetY1(InThr[1]);
  lThreshold[1]->SetY2(InThr[1]);
  return;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void InitAll()
{
  if( !cMain )
    {
      cMain = new TCanvas("cMain", "cMain", 0, 0, 1100, 700);
      cMain->Divide(3, 3, .001, .001);
    }

#ifdef SHOW_TIME_DIST
  if( !cTimeDist )
    {
      cTimeDist = new TCanvas("cTimeDist", "cTimeDist", 300, 600, 1200, 400);
      cTimeDist->Divide(1, 3, .001, .001);
    }
#endif

  // PMT
  //
  for( Int_t i=0; i<N_BURST; i++)
    {
      // threshold lines
      //
      lThreshold[i] = new TLine(0,0,7000,0);
      lThreshold[i]->SetLineColor(myColor[i]);
      lThreshold[i]->SetLineStyle(3);

      // start lines
      //
      lStart[i] = new TLine(0,0,0,4100);
      lStart[i]->SetLineColor(myColor[i]);

      // PMT
      //
      sprintf(szName, "fPmtSignal_%i", i);
      fPMTOffset[i]  = new TF1(szName, "pol0");
      fPMTOffset[i]->SetLineColor(myColor[i]);

      // Amp
      //
      sprintf(szName, "fAmpSignal_%i", i);
      fAmpSignal[i]  = new TF1(szName, "expo");
      fAmpSignal[i]->SetLineColor(myColor[i]);
      
      sprintf(szName, "fAmpOffset_%i", i);
      fAmpOffset[i]  = new TF1(szName, "pol0");
      fAmpOffset[i]->SetLineColor(myColor[i]);

      // Shp
      //
      sprintf(szName, "fShpOffset_%i", i);
      fShpOffset[i]  = new TF1(szName, "pol0");
      fShpOffset[i]->SetLineColor(myColor[i]);
    }

  // init markers
  //
  Int_t myMarkerColor[] = {kRed+2, kRed+2, kGreen+2, kGreen+2};
  Int_t myMarkerStyle[] = {22, 23, 22, 23};

  for( Int_t i=0; i<(2*N_BURST); i++)
    {
      lStartPMT[i] = new TLine(0,0,0,4100);
      lStartPMT[i]->SetLineColor(myColor[i]);

      lStoppPMT[i] = new TLine(0,0,0,4100);
      lStoppPMT[i]->SetLineColor(myColor[i]);

      lStartShp[i] = new TLine(0,0,0,4100);
      lStartShp[i]->SetLineColor(myColor[i]);

      lStoppShp[i] = new TLine(0,0,0,4100);
      lStoppShp[i]->SetLineColor(myColor[i]);

      mPMTSignal[i] = new TMarker( 0, 0, 21);
      mPMTSignal[i]->SetMarkerColor(myMarkerColor[i]);
      mPMTSignal[i]->SetMarkerStyle(myMarkerStyle[i]);

      mAmpSignal[i] = new TMarker( 0, 0, 21);
      mAmpSignal[i]->SetMarkerColor(myMarkerColor[i]);
      mAmpSignal[i]->SetMarkerStyle(myMarkerStyle[i]);

      mShpSignal[i] = new TMarker( 0, 0, 21);
      mShpSignal[i]->SetMarkerColor(myMarkerColor[i]);
      mShpSignal[i]->SetMarkerStyle(myMarkerStyle[i]);
    }

  // init distributions
  //
  Char_t szHName[(N_CHANNEL*N_BURST)][32]={ "hPMT_1","hPMT_2",
					    "hAmp_1","hAmp_2",
					    "hShp_1","hShp_2"};
  for( Int_t i=0; i<(N_CHANNEL*N_BURST); i++)
    {
      
      hSignalDist[i] = new TH1D( szHName[i], "", 4100, 0, 4100);
      hSignalDist[i]->SetFillColor(myColor[i]);
    }

  hPMTVsAmp = new TH2D("hPMTVsAmp", ";PMT;Amp", 4100, 0, 4100, 4100, 0, 4100);
  
  //
  //
  Int_t xbins = h2D[0]->GetNbinsY();

  Double_t delay_low = 0.;
  Double_t delay_upp = 10.5;
  Int_t    delay_bin = (delay_upp-delay_low)/0.100;

  Double_t yupp = 110;
  Double_t ylow = 90;

  // PMT
  //
  hAmpVsDel[0] = new TH2D("hAmpVsDel_0", ";Delay [#mus];Norm. VPTT [%]", 
			  delay_bin, delay_low, delay_upp, 
			  1000, ylow, yupp);
  hAmpVsDel[0]->SetMarkerColor(kRed+1);

  hAmpVsDel[1] = new TH2D("hAmpVsDel_1", ";Delay [#mus];Norm. VPTT [%]", 
			  delay_bin, delay_low, delay_upp, 
			  1000, ylow, yupp);
  hAmpVsDel[1]->SetMarkerColor(kGreen+1);

  // Apm
  //
  hAmpVsDel[2] = new TH2D("hAmpVsDel_2", ";Delay [#mus];Norm. Input [%]", 
			  delay_bin, delay_low, delay_upp, 
			  1000, ylow, yupp);
  hAmpVsDel[2]->SetMarkerColor(kRed+1);

  hAmpVsDel[3] = new TH2D("hAmpVsDel_3", ";Delay [#mus];Norm. Input [%]", 
			  delay_bin, delay_low, delay_upp, 
			  1000, ylow, yupp);
  hAmpVsDel[3]->SetMarkerColor(kGreen+1);

  // Shp
  //
  hAmpVsDel[4] = new TH2D("hAmpVsDel_4", ";Delay [#mus];Norm. Input [%]", 
			  delay_bin, delay_low, delay_upp, 
			  1000, ylow, yupp);
  hAmpVsDel[4]->SetMarkerColor(kRed+1);

  hAmpVsDel[5] = new TH2D("hAmpVsDel_5", ";Delay [#mus];Norm. Input [%]", 
			  delay_bin, delay_low, delay_upp, 
			  1000, ylow, yupp);
  hAmpVsDel[5]->SetMarkerColor(kGreen+1);

  //
  //
  fAmpVsDel_px[0] = new TF1("fAmpVsDel_px_0", "pol1", delay_low, delay_upp);
  fAmpVsDel_px[1] = new TF1("fAmpVsDel_px_1", "pol1", delay_low, delay_upp);

  //
  //
  hSignalVsEvent[0] = new TH1D("hSignalVsEvent_0", ";Event;", xbins, 0, xbins);
  hSignalVsEvent[1] = new TH1D("hSignalVsEvent_1", ";Event;", xbins, 0, xbins);
  hSignalVsEvent[2] = new TH1D("hSignalVsEvent_2", ";Event;", xbins, 0, xbins);
  hSignalVsEvent[3] = new TH1D("hSignalVsEvent_3", ";Event;", xbins, 0, xbins);

  hSignalVsEvent[4] = new TH1D("hSignalVsEvent_4", ";Event;", xbins, 0, xbins);
  hSignalVsEvent[5] = new TH1D("hSignalVsEvent_5", ";Event;", xbins, 0, xbins);

  hSignalVsEvent[6] = new TH1D("hSignalVsEvent_6", ";Event;", xbins, 0, xbins);
  hSignalVsEvent[7] = new TH1D("hSignalVsEvent_7", ";Event;", xbins, 0, xbins);

  hSignalVsEvent[0]->SetLineColor(myColor[0]);
  hSignalVsEvent[1]->SetLineColor(myColor[1]);
  hSignalVsEvent[2]->SetLineColor(myColor[2]);
  hSignalVsEvent[3]->SetLineColor(myColor[3]);

  hSignalVsEvent[4]->SetLineColor(myColor[0]);
  hSignalVsEvent[5]->SetLineColor(myColor[1]);

  hSignalVsEvent[6]->SetLineColor(myColor[0]);
  hSignalVsEvent[7]->SetLineColor(myColor[1]);

  // fir fucntions
  //
  fSignalDist[0]  = new TF1("fSignalDist_0", "gaus"); // PMT
  fSignalDist[1]  = new TF1("fSignalDist_1", "gaus");
  fSignalDist[2]  = new TF1("fSignalDist_2", "gaus"); // VPT
  fSignalDist[3]  = new TF1("fSignalDist_3", "gaus");
  fSignalDist[4]  = new TF1("fSignalDist_4", "gaus"); // Shaper
  fSignalDist[5]  = new TF1("fSignalDist_5", "gaus");

  fSignalDist[0]->SetLineColor(myColor[0]);
  fSignalDist[1]->SetLineColor(myColor[1]);
  fSignalDist[2]->SetLineColor(myColor[2]);
  fSignalDist[3]->SetLineColor(myColor[3]);
  fSignalDist[4]->SetLineColor(myColor[4]);
  fSignalDist[5]->SetLineColor(myColor[5]);

  return;
} // InitAll

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void WriteAll()
{
  // write into file out.txt
  //
  FILE *fp;
  fp=fopen("out.txt", "a");
  fprintf(fp, "%s\n", szLine);
  fclose(fp);

  //
  //
  Char_t szOut[64];

  //
  sprintf(szOut, "pics/%s_%s_.png", cMain->GetName(), szFileName);
  cMain->SaveAs(szOut);
   
  sprintf(szOut, "pics/%s_%s_.root", cMain->GetName(), szFileName);
  cMain->SaveAs(szOut);


#ifdef SHOW_TIME_DIST
  sprintf(szOut, "pics/%s_%s_.png", cTimeDist->GetName(), szFileName);
  cTimeDist->SaveAs(szOut);
  
  sprintf(szOut, "pics/%s_%s_.root", cTimeDist->GetName(), szFileName);
  cTimeDist->SaveAs(szOut);
#endif

} // WriteAll

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void ReadFile(Char_t *szFin)
{
  if( gROOT->GetFile() )
    file = (TFile*) gFile;
  else
    file = TFile::Open(szFin);
  
  strcpy(szFileName, file->GetName() );
	
  gROOT->cd();

  if( h2D[0] )
    {
      h2D[0]->Delete();
      h2D[0]=0;
    }
  h2D[0] = (TH2I*) file->Get("hADC_0")->Clone("hSADC_0");

  if( h2D[1] )
    {
      h2D[1]->Delete();
      h2D[1]=0;
    }
  h2D[1] = (TH2I*) file->Get("hADC_1")->Clone("hSADC_1");

  if( h2D[2] )
    {
      h2D[2]->Delete();
      h2D[2]=0;
    }
  h2D[2] = (TH2I*) file->Get("hADC_2")->Clone("hSADC_2");

  file->Close();

  return;
} // ReadFile

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void GetProjection(Int_t first, Int_t last)
{
  if( hProj[0] )
    {
      hProj[0]->Delete();
      hProj[0]=0;
    }
  hProj[0] = h2D[0]->ProjectionX("hSADC_0_px", first, last);

  if( hProj[1] )
    {
      hProj[1]->Delete();
      hProj[1]=0;
    }
  hProj[1] = h2D[1]->ProjectionX("hSADC_1_px", first, last);

  if( hProj[2] )
    {
      hProj[2]->Delete();
      hProj[2]=0;
    }
  hProj[2] = h2D[2]->ProjectionX("hSADC_2_px", first, last);

  return;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void Draw2D()
{
  c2D = new TCanvas("c2D", "c2D", 10, 10, 600, 750);
  c2D->Divide(1, 3, .001, .001);

  c2D->cd(1);
  h2D[0]->Draw("colz");
  
  c2D->cd(2);
  h2D[1]->Draw("colz");

  c2D->cd(3);
  h2D[2]->Draw("colz");

  return;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void DrawAll()
{  
  // PMT
  //
  cMain->cd(1);

  //  hProj[0]->GetXaxis()->SetRangeUser(start[0]-300, 2*start[1]-start[0]+300);
  hProj[0]->GetYaxis()->SetRangeUser(0, 4100);
  hProj[0]->SetStats(0);
  hProj[0]->SetTitle(szFileName);
  hProj[0]->Draw("");

  fPMTOffset[0]->Draw("same");
  fPMTOffset[1]->Draw("same");

  lStartPMT[0]->Draw();
  lStartPMT[1]->Draw();

  lStoppPMT[0]->Draw();
  lStoppPMT[1]->Draw();

  // mPMTSignal[0]->Draw();
  // mPMTSignal[1]->Draw();
  // mPMTSignal[2]->Draw();
  // mPMTSignal[3]->Draw();

  lThreshold[0]->Draw();
  lThreshold[1]->Draw();

  // Amp
  //
  cMain->cd(2);

  //  hProj[1]->GetXaxis()->SetRangeUser(start[0]-300, 2*start[1]-start[0]+300);
  hProj[1]->GetYaxis()->SetRangeUser(0, 4100);
  hProj[1]->SetStats(0);
  hProj[1]->Draw("");

  fAmpOffset[0]->Draw("same");
  fAmpOffset[1]->Draw("same");

  lStart[0]->Draw();
  lStart[1]->Draw();

  fAmpSignal[0]->Draw("same");
  fAmpSignal[1]->Draw("same");

  mAmpSignal[0]->Draw();
  mAmpSignal[1]->Draw();
  mAmpSignal[2]->Draw();
  mAmpSignal[3]->Draw();

  // Shaper
  //
  cMain->cd(3);

  //  hProj[2]->GetXaxis()->SetRangeUser(start[0]-300, 2*start[1]-start[0]+300);
  hProj[2]->GetYaxis()->SetRangeUser(0, 4100);
  hProj[2]->SetStats(0);
  hProj[2]->Draw("");

  fShpOffset[0]->Draw("same");
  fShpOffset[1]->Draw("same");

  lStartShp[0]->Draw();
  lStartShp[1]->Draw();

  lStoppShp[0]->Draw();
  lStoppShp[1]->Draw();

  // mShpSignal[0]->Draw();
  // mShpSignal[1]->Draw();
  // mShpSignal[2]->Draw();
  // mShpSignal[3]->Draw();


  //
  //
  cMain->cd(4);
  hAmpVsDel[0]->SetStats(0);
  hAmpVsDel[0]->Draw();
  hAmpVsDel[1]->Draw("same");

  if( hAmpVsDel_px[0] )
    {
      hAmpVsDel_px[0]->Draw("same");
      hAmpVsDel_px[1]->Draw("same");
    }

  //
  //
  cMain->cd(5);
  hAmpVsDel[2]->SetStats(0);
  hAmpVsDel[2]->Draw();
  hAmpVsDel[3]->Draw("same");

  if( hAmpVsDel_px[2] )
    {
      hAmpVsDel_px[2]->Draw("same");
      hAmpVsDel_px[3]->Draw("same");
    }

  //
  //
  cMain->cd(6);
  hAmpVsDel[4]->SetStats(0);
  hAmpVsDel[4]->Draw();
  hAmpVsDel[5]->Draw("same");

  if( hAmpVsDel_px[4] )
    {
      hAmpVsDel_px[4]->Draw("same");
      hAmpVsDel_px[5]->Draw("same");
    }

  //
  //
  cMain->cd(7);
  hSignalDist[0]->SetStats(0);
  //  hSignalDist[0]->GetYaxis()->SetRangeUser(0, 1600);
  hSignalDist[0]->Draw();
  hSignalDist[1]->Draw("same");

  //
  //
  cMain->cd(8);
  hSignalDist[2]->SetStats(0);
  //  hSignalDist[2]->GetYaxis()->SetRangeUser(0, 1400);
  hSignalDist[2]->Draw();
  hSignalDist[3]->Draw("same");

  cMain->cd(9);
  hSignalDist[4]->SetStats(0);
  //  hSignalDist[4]->GetYaxis()->SetRangeUser(0, 1400);
  hSignalDist[4]->Draw();
  hSignalDist[5]->Draw("same");

  cMain->Update();

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  //
#ifdef SHOW_TIME_DIST

  cTimeDist->cd(1);

  hSignalVsEvent[3]->Draw("");
  hSignalVsEvent[0]->Draw("same");
  hSignalVsEvent[1]->Draw("same");
  hSignalVsEvent[2]->Draw("same");
  
  cTimeDist->cd(2);
  hSignalVsEvent[4]->Draw("");
  hSignalVsEvent[5]->Draw("same");

  cTimeDist->cd(3);
  hSignalVsEvent[6]->Draw("");
  hSignalVsEvent[7]->Draw("same");
  
  cTimeDist->Update();
#endif

  return;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void FindEdges()
{
  // - - - - - - - - - - - - - - - - - - - - - - - -
  //  PMT
  //
#ifdef GEN_MEASUREMENT
  int width=3;

  int ave_1=0;

  int minimum=0;

  // find start for Amp
  //
  for( int bin=width+10; bin<hProj[1]->GetNbinsX(); bin++)
    {
      ave_1 = hProj[0]->Integral(bin-width, bin)/width;
      
      if( ave_1 < InThr[0] && minimum == 0 )
	{
	  start[minimum] = hProj[1]->FindBin( bin );
	  
  	  lStart[minimum]->SetX1(start[minimum]);
  	  lStart[minimum]->SetX2(start[minimum]);
	  
	  minimum++;
	  bin += 100;
	}
      else if( ave_1 < InThr[1] && minimum == 1 )
	{
	  start[minimum] = hProj[1]->FindBin( bin );
	  
  	  lStart[minimum]->SetX1(start[minimum]);
  	  lStart[minimum]->SetX2(start[minimum]);
	  
	  minimum++;
	  bin += 100;
	}
    }

  BurstRate  = 1.e-3/( (start[1]-start[0]) / SamplingRate ); // kHz
  BurstDelay = ( (start[1]-start[0])/ SamplingRate ) * 1e+6; // us

  Char_t szTitle[64];
  sprintf(szTitle, "Sampling: %3.0f [MS/s] | Burst: %4.0f [kHz]", SamplingRate/1e+6, BurstRate );
  hProj[1]->SetTitle(szTitle);
#endif

  // - - - - - - - - - - - - - - - - - - - - - - - -
  //  P M T
  //

#ifdef PMT_MEASUREMENT

  int width=2;

  int ave_1=0;

  int minimum=0;

  // find start for Amp
  //
  for( int bin=width+10; bin<hProj[0]->GetNbinsX(); bin++)
    {
      ave_1 = hProj[0]->Integral(bin-width, bin)/width;
      
      if( ave_1 < InThr[0] && minimum == 0 )
	{
	  start[minimum] = hProj[1]->FindBin( bin );
	  
  	  lStart[minimum]->SetX1(start[minimum]);
  	  lStart[minimum]->SetX2(start[minimum]);
	  
	  minimum++;
	  bin += 100;
	}
      else if( ave_1 < InThr[1] && minimum == 1 )
	{
	  start[minimum] = hProj[1]->FindBin( bin );
	  
  	  lStart[minimum]->SetX1(start[minimum]);
  	  lStart[minimum]->SetX2(start[minimum]);
	  
	  minimum++;
	  bin += 100;
	}
    }

  BurstRate  = 1.e-3/( (start[1]-start[0]) / SamplingRate ); // kHz
  BurstDelay = ( (start[1]-start[0])/ SamplingRate ) * 1e+6; // us

  Char_t szTitle[64];
  sprintf(szTitle, "Sampling: %3.0f [MS/s] | Burst: %4.0f [kHz]", SamplingRate/1e+6, BurstRate );
  hProj[1]->SetTitle(szTitle);
#endif
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void GetPMTSignals()
{

  // pmt
  //  
  Int_t scale = 1;
  Int_t risetime = 100;
  
  // find the minimum for shaper
  //
  Int_t minBin[N_BURST] = {0};

  const Int_t IntegralWidth = 10; // min plus minus 

  for( int i=0; i<N_BURST; i++)
    {
      // baseline fit
      //
      fPMTOffset[i]->SetRange( (start[i]-StartOffset-300) ,
	                       (start[i]-StartOffset) );
      hProj[0]->Fit( fPMTOffset[i], "+RQ0");

      // find minimum
      //
      PMTSignal[i] = 4100;
      
      for(int bin=start[i]; bin<(start[i]+risetime); bin++)
	if( PMTSignal[i] > hProj[0]->GetBinContent(bin) )
	  {
	    minBin[i] = bin;
	    PMTSignal[i] = hProj[0]->GetBinContent(bin);
	  }

      Int_t PMTlow = minBin[i]-IntegralWidth;
      Int_t PMTupp = minBin[i]+IntegralWidth;

      PMTSignal[i] = ( (fPMTOffset[i]->Eval(0)*(2*IntegralWidth+1)) -
		       (hProj[0]->Integral(PMTlow, PMTupp)) ) / 20;
      
      lStartPMT[i]->SetX1(PMTlow);
      lStartPMT[i]->SetX2(PMTlow);
      lStoppPMT[i]->SetX1(PMTupp);
      lStoppPMT[i]->SetX2(PMTupp);
      
      // mPMTSignal[2*i]->SetX( minBin[i]-0.5 );
      // mPMTSignal[2*i]->SetY( fPMTOffset[i]->Eval(0) );
 
      // mPMTSignal[2*i+1]->SetX( minBin[i]-0.5 );
      // mPMTSignal[2*i+1]->SetY( hProj[0]->GetBinContent(minBin[i]) );
 
      hSignalDist[i]->Fill( PMTSignal[i]/scale );
 
      hSignalVsEvent[i]->SetBinContent( event, PMTSignal[i] / NormPMTSignal[i]*100.);
      hSignalVsEvent[i]->SetBinContent( event, PMTSignal[i] / AmpSignal[i] ); 
    }
  
  //   hPMTVsAmp->Fill( PMTSignal[0], AmpSignal[0] );

  if( event == 100)
    {
      NormAmpVsDel[0] = hSignalDist[0]->GetMean();
      NormAmpVsDel[1] = hSignalDist[1]->GetMean();

      printf("Norm_0 %f\n", NormAmpVsDel[0]);
      printf("Norm_1 %f\n", NormAmpVsDel[1]);  
    }
  else if( event > 100 )
    {
      // 
      //
      Double_t r1 =  PMTSignal[0] ;// / AmpSignal[0] ;
      Double_t r2 =  PMTSignal[1] ;// / AmpSignal[1] ;
      
      hAmpVsDel[0]->Fill( BurstDelay, r1/NormAmpVsDel[0]*100 );
      hAmpVsDel[1]->Fill( BurstDelay, r2/NormAmpVsDel[1]*100 );
    }

} // end GetPMTSignal


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void GetAmpSignals()
{
  int risetime = 600;
  if( risetime > (start[1]-start[0]))
    risetime = (start[1]-start[0]-3);

  int length = start[1]-start[0] - 200;

  if( start[1] > start[0]+risetime ) 
    {
      fAmpSignal[0]->SetRange(start[0]+risetime, start[0]+length );  
      //  fAmpSignal[0]->SetParameter(1, FixedSlope);
      hProj[1]->Fit( fAmpSignal[0], "+R0Q");
    }


  // find the minimum for shaper
  //
  Int_t minBin[N_BURST] = {0};

  const Int_t IntegralWidth = 30; // min plus minus bins

  for( int i=0; i<N_BURST; i++)
    {
      // baseline fit
      //
      fAmpOffset[i]->SetRange( (start[i]-StartOffset-100), 
                               (start[i]-StartOffset) );
      hProj[1]->Fit( fAmpOffset[i], "+RQ0");
 
      //
      //
      AmpSignal[i] = 0;
      
      for(int bin=start[i]; bin<(start[i]+risetime); bin++)
	if( AmpSignal[i] < hProj[1]->GetBinContent(bin) )
	  {
	    minBin[i] = bin;
	    AmpSignal[i] = hProj[1]->GetBinContent(bin);
	  }
      
      AmpSignal[i] = hProj[1]->Integral(minBin[i]-IntegralWidth, minBin[i]+IntegralWidth)/(2*IntegralWidth+1);

      mAmpSignal[2*i]->SetX( minBin[i]-0.5 );
      mAmpSignal[2*i]->SetY( fAmpOffset[i]->Eval(0) );

      mAmpSignal[2*i+1]->SetX( minBin[i]-0.5 );
      mAmpSignal[2*i+1]->SetY( AmpSignal[i] );

      AmpSignal[i] = fabs( mAmpSignal[2*i+1]->GetY()- mAmpSignal[2*i]->GetY() );

      hSignalDist[i+2]->Fill( AmpSignal[i] );

      hSignalVsEvent[i+2]->SetBinContent( event, AmpSignal[i] / NormAmpSignal[i]*100.);
      hSignalVsEvent[i+2]->SetBinContent( event, AmpSignal[i] / AmpSignal[i] ); 
    }

 
  // GetRMS for normalization
  //
  if( event == 100 )
    {
      NormAmpSignal[0] = hSignalDist[2]->GetMean();
      NormAmpSignal[1] = hSignalDist[3]->GetMean();
    }
  else if( event > 100 )
    {
      hSignalVsEvent[2]->SetBinContent( event, AmpSignal[0]/NormAmpSignal[0]*100.); 
      hSignalVsEvent[3]->SetBinContent( event, AmpSignal[1]/NormAmpSignal[1]*100.);   
      
      // 
      //
      hAmpVsDel[2]->Fill( BurstDelay, AmpSignal[0]/NormAmpSignal[0]*100. );
      hAmpVsDel[3]->Fill( BurstDelay, AmpSignal[1]/NormAmpSignal[1]*100. );
    }
} // end GetAmpSignals


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// 
void GetShpSignals()
{
  // Shaper
  //
  Int_t scale = 1;
  Int_t risetime = 200;

  Int_t minus =  30;
  Int_t pluss = 400;

  // find the minimum for shaper
  //
  Int_t minBin[N_BURST] = {0};

  for( int i=0; i<N_BURST; i++)
    {
      // baseline fit
      //
      fShpOffset[i]->SetRange( (start[i]-StartOffset-300), 
                               (start[i]-StartOffset) );
      hProj[2]->Fit( fShpOffset[i], "+RQ0");

      ShpSignal[i] = 4100;
      
      for(int bin=start[i]; bin<(start[i]+risetime); bin++)
	if( ShpSignal[i] > hProj[2]->GetBinContent(bin) )
	  {
	    minBin[i] = bin;
	    ShpSignal[i] = hProj[2]->GetBinContent(bin);
	  }
      
      Int_t ShpLow = minBin[i]-minus;
      Int_t ShpUpp = minBin[i]+pluss;
      
      ShpSignal[i] = ( (fShpOffset[i]->Eval(0)*(minus+pluss+1)) -
      		       (hProj[2]->Integral(ShpLow, ShpUpp)) ) / 100;

      //  const Int_t IntegralWidth = 3; // min plus minus 3 bin
      //      ShpSignal[i] = hProj[2]->Integral(minBin[i]-IntegralWidth, minBin[i]+IntegralWidth)/(2*IntegralWidth+1);
      
      lStartShp[i]->SetX1(ShpLow);
      lStartShp[i]->SetX2(ShpLow);
      lStoppShp[i]->SetX1(ShpUpp);
      lStoppShp[i]->SetX2(ShpUpp);
      
      // mShpSignal[2*i]->SetX( minBin[i]-0.5 );
      // mShpSignal[2*i]->SetY( fShpOffset[i]->Eval(0) );
      
      // mShpSignal[2*i+1]->SetX( minBin[i]-0.5 );
      // mShpSignal[2*i+1]->SetY( ShpSignal[i] );
      
      hSignalDist[i+4]->Fill( ShpSignal[i]/scale );
      
      hSignalVsEvent[i+4]->SetBinContent( event, ShpSignal[i] / NormShpSignal[i]*100.);
      hSignalVsEvent[i+4]->SetBinContent( event, ShpSignal[i] / AmpSignal[i] ); 
    }
  
  //  hPMTVsAmp->Fill( ShpSignal[0], AmpSignal[0] );
  
  if( event == 100)
    {
      NormShpVsDel[0] = hSignalDist[4]->GetMean();
      NormShpVsDel[1] = hSignalDist[5]->GetMean();
      
      printf("Norm_0 %f\n", NormShpVsDel[0]);
      printf("Norm_1 %f\n", NormShpVsDel[1]);
  
    }
  else if( event > 100 )
    {
      // 
      //
      Double_t r1 =  ShpSignal[0] ;// / ShpSignal[0] ;
      Double_t r2 =  ShpSignal[1] ;// / ShpSignal[1] ;
      
      hAmpVsDel[4]->Fill( BurstDelay, r1/NormShpVsDel[0]*100 );
      hAmpVsDel[5]->Fill( BurstDelay, r2/NormShpVsDel[1]*100 );
    }

} // end GetShpSignal


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void DrawDistributions()
{
  Int_t mean[6], rms[6];

  Int_t Nrms = 9;

  // fit gen distribution
  //
  mean[0] = hSignalDist[0]->GetMean();
  rms[0]  = hSignalDist[0]->GetRMS();
  fSignalDist[0]->SetRange(mean[0]-Nrms*rms[0], mean[0]+Nrms*rms[0] );
  hSignalDist[0]->Fit(fSignalDist[0], "+R0Q");

  mean[1] = hSignalDist[1]->GetMean();
  rms[1]  = hSignalDist[1]->GetRMS();
  fSignalDist[1]->SetRange(mean[1]-Nrms*rms[1], mean[1]+Nrms*rms[1] );
  hSignalDist[1]->Fit(fSignalDist[1], "+R0Q");

  cMain->cd(7);
  //  hSignalDist[0]->GetXaxis()->SetRangeUser(mean[0]-7*rms[0], mean[1]+7*rms[1] );
  hSignalDist[0]->GetXaxis()->SetRangeUser(mean[0]-100, mean[1]+100 );
  hSignalDist[0]->Draw("E1");
  fSignalDist[0]->Draw("same");

  hSignalDist[1]->Draw("E1same");
  fSignalDist[1]->Draw("same");

  // fit Amp distribution
  //
  mean[2] = hSignalDist[2]->GetMean();
  rms[2]  = hSignalDist[2]->GetRMS();
  fSignalDist[2]->SetRange(mean[2]-Nrms*rms[2], mean[2]+Nrms*rms[2] );
  hSignalDist[2]->Fit(fSignalDist[2], "+R0Q");

  mean[3] = hSignalDist[3]->GetMean();
  rms[3]  = hSignalDist[3]->GetRMS();
  fSignalDist[3]->SetRange(mean[3]-Nrms*rms[3], mean[3]+Nrms*rms[3] );
  hSignalDist[3]->Fit(fSignalDist[3], "+R0Q");

  cMain->cd(8);
  hSignalDist[2]->GetXaxis()->SetRangeUser(mean[2]-100, mean[3]+100 );
  //  hSignalDist[2]->GetXaxis()->SetRangeUser( 0, 2000);
  hSignalDist[2]->Draw("E1");
  fSignalDist[2]->Draw("same");

  hSignalDist[3]->Draw("E1same");
  fSignalDist[3]->Draw("same");

  // fit Shp distribution
  //
  mean[4] = hSignalDist[4]->GetMean();
  rms[4]  = hSignalDist[4]->GetRMS();
  fSignalDist[4]->SetRange(mean[4]-Nrms*rms[4], mean[4]+Nrms*rms[4] );
  hSignalDist[4]->Fit(fSignalDist[4], "+R0Q");

  mean[5] = hSignalDist[5]->GetMean();
  rms[5]  = hSignalDist[5]->GetRMS();
  fSignalDist[5]->SetRange(mean[5]-Nrms*rms[5], mean[5]+Nrms*rms[5] );
  hSignalDist[5]->Fit(fSignalDist[5], "+R0Q");

  cMain->cd(9);
  //  hSignalDist[2]->GetXaxis()->SetRangeUser(mean[2]-7*rms[2], mean[3]+7*rms[3] );
  hSignalDist[4]->GetXaxis()->SetRangeUser(mean[4]-100, mean[5]+100 );
  hSignalDist[4]->Draw("E1");
  fSignalDist[4]->Draw("same");

  hSignalDist[5]->Draw("E1same");
  fSignalDist[5]->Draw("same");

  cMain->Update();
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void PrintAll()
{
  Char_t szSplitLine_0[64];
  Char_t szSplitLine_1[64];
  Char_t szTitle[128];

  // PMT
  //
  sprintf(szSplitLine_0, "PMT 1 FWHM/Amp: %6.4f%%", 
	  100*2.35*fSignalDist[0]->GetParameter(2)/fSignalDist[0]->GetParameter(1) );
  printf(" %s\n", szSplitLine_0);
  
  sprintf(szSplitLine_1, "PMT 2 FWHM/Amp: %6.4f%%", 
	  100*2.35*fSignalDist[1]->GetParameter(2)/fSignalDist[1]->GetParameter(1) );
  printf(" %s\n", szSplitLine_0);

  sprintf(szTitle, "#splitline{%s}{%s}", szSplitLine_0, szSplitLine_1 );
  hSignalDist[0]->SetTitle(szTitle);

  // PreAmp
  //
  sprintf(szSplitLine_0, "PreAmp 1 FWHM/Amp: %6.4f%%",
	  100*2.35*fSignalDist[2]->GetParameter(2)/fSignalDist[2]->GetParameter(1) );
  printf(" %s\n", szTitle);
  
  sprintf(szSplitLine_1, "PreAmp 2 FWHM/Amp: %6.4f%%", 
	  100*2.35*fSignalDist[3]->GetParameter(2)/fSignalDist[3]->GetParameter(1) );
  printf(" %s\n", szTitle);
  
  sprintf(szTitle, "#splitline{%s}{%s}", szSplitLine_0, szSplitLine_1 );
  hSignalDist[2]->SetTitle(szTitle);
  
  // Shaper
  //
  sprintf(szSplitLine_0, "Shp 1 FWHM/Amp: %6.4f%%",
	  100*2.35*fSignalDist[4]->GetParameter(2)/fSignalDist[4]->GetParameter(1) );
  printf(" %s\n", szTitle);
  
  sprintf(szSplitLine_1, "Shp 2 FWHM/Amp: %6.4f%%", 
	  100*2.35*fSignalDist[5]->GetParameter(2)/fSignalDist[5]->GetParameter(1) );
  printf(" %s\n", szTitle);
  
  sprintf(szTitle, "#splitline{%s}{%s}", szSplitLine_0, szSplitLine_1 );
  hSignalDist[4]->SetTitle(szTitle);
  //
  //
  sprintf(szLine, "%s ;", szFileName);

  // PMT
  //
  sprintf(szLine, "%s %f ;", szLine, fSignalDist[0]->GetParameter(1) );
  sprintf(szLine, "%s %f ;", szLine, fSignalDist[1]->GetParameter(1) );
	
  // amp
  //
  sprintf(szLine, "%s %f ;", szLine, fSignalDist[2]->GetParameter(1) );
  sprintf(szLine, "%s %f ;", szLine, fSignalDist[3]->GetParameter(1) );

  // Shaper
  //
  sprintf(szLine, "%s %f ;", szLine, fSignalDist[4]->GetParameter(1) );
  sprintf(szLine, "%s %f ;", szLine, fSignalDist[5]->GetParameter(1) );


  printf(" %s \n", szLine);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void FitProfiles()
{
  // PMT
  //
  hAmpVsDel_px[0] = hAmpVsDel[0]->ProfileX("hAmpVsDel_px0");
  //hAmpVsDel_px[0]->Fit(fAmpVsDel_px[0], "+R0");
  hAmpVsDel_px[0]->SetMarkerColor(kRed+3);
  hAmpVsDel_px[0]->SetMarkerSize(0.5);
  hAmpVsDel_px[0]->SetMarkerStyle(21);

  hAmpVsDel_px[1] = hAmpVsDel[1]->ProfileX("hAmpVsDel_px1");
  //hAmpVsDel_px[1]->Fit(fAmpVsDel_px[1], "+R0");
  hAmpVsDel_px[1]->SetMarkerColor(kGreen+3);
  hAmpVsDel_px[1]->SetMarkerSize(0.5);
  hAmpVsDel_px[1]->SetMarkerStyle(21);

  // Amp
  //
  hAmpVsDel_px[2] = hAmpVsDel[2]->ProfileX("hAmpVsDel_px2");
  //hAmpVsDel_px[2]->Fit(fAmpVsDel_px[2], "+R0");
  hAmpVsDel_px[2]->SetMarkerColor(kRed+3);
  hAmpVsDel_px[2]->SetMarkerSize(0.5);
  hAmpVsDel_px[2]->SetMarkerStyle(21);

  hAmpVsDel_px[3] = hAmpVsDel[3]->ProfileX("hAmpVsDel_px3");
  //hAmpVsDel_px[3]->Fit(fAmpVsDel_px[3], "+R0");
  hAmpVsDel_px[3]->SetMarkerColor(kGreen+3);
  hAmpVsDel_px[3]->SetMarkerSize(0.5);
  hAmpVsDel_px[3]->SetMarkerStyle(21);

  // Shaper
  //
  hAmpVsDel_px[4] = hAmpVsDel[4]->ProfileX("hAmpVsDel_px4");
  //hAmpVsDel_px[2]->Fit(fAmpVsDel_px[2], "+R0");
  hAmpVsDel_px[4]->SetMarkerColor(kRed+3);
  hAmpVsDel_px[4]->SetMarkerSize(0.5);
  hAmpVsDel_px[4]->SetMarkerStyle(21);

  hAmpVsDel_px[5] = hAmpVsDel[5]->ProfileX("hAmpVsDel_px5");
  //hAmpVsDel_px[3]->Fit(fAmpVsDel_px[3], "+R0");
  hAmpVsDel_px[5]->SetMarkerColor(kGreen+3);
  hAmpVsDel_px[5]->SetMarkerSize(0.5);
  hAmpVsDel_px[5]->SetMarkerStyle(21);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//
void Next()
{
  GetProjection(event, event);
  
  FindEdges(); 

  if( (start[1]-start[0])> MinStartDist ) // limit the burst distance
    {
      GetAmpSignals();
      
      GetPMTSignals();
      
      GetShpSignals();

      if( !(event%100) )
	{
	  DrawAll();  
	  DrawDistributions();
	  PrintAll();
	  printf("event: %i\n", event);
	}  
    }
  event++;

  if( event == event_lastt )
    {
      ftimer.Stop();
                  
      FitProfiles();
      
      PrintAll();

      DrawAll();  

      DrawDistributions();

      WriteAll();
      
      gSystem->Exit(1);
    }
}
  
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// x
// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void analyze( Char_t *szFin, 
	      Int_t first=2,
	      Int_t lastt=0)
{
  ReadFile(szFin);
  InitAll();

  SetThreshold( 3000, 3000 ); // set the thresholds

  event_first = first;  
  event_lastt=h2D[0]->GetNbinsY()-1;
  if( lastt>1 && lastt<event_lastt )
    event_lastt = lastt;
  event = event_first;
  
  ftimer.Start();

  return;
} 

void analyze() // root -b -q 'analyze.C++' - needs it
{
  gSystem->Exit(1);
}
