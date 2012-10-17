
////////////////////////////////////////////////////////////////////
//                                                                //
// author: i.keshelashvili@unibas.ch                              //
//                                                                //
//                                                                //
////////////////////////////////////////////////////////////////////

#include "TPManager.hh"

ClassImp( TPManager )

//------------------------------------------------------------------------------
  TPManager::TPManager()
{
  printf("  **************************************************** \n"
	 "  *                                                  * \n"
	 "  *        W E L C O M E  to  A N A P A N D A        * \n"
	 "  *                                                  * \n"
	 "  *                                                  * \n"
	 "  **************************************************** \n");

  fNBurst = 2;
  fDrawScale = 1;

  fbDrawOnOff = kTRUE;

  fTimer = new TTimer(1);
  fTimer->Connect("Timeout()", "TPManager", this, "Next()");

  cMain = new TCanvas("cMain","Main Canvas", 0, 0, 1000, 800);
  cMain->Divide(3, 3, .001, .001);

  //
  //
  fSigExt_PMT = new TPSignalExtractor("PMT", hProj[0]);
  fSigExt_PMT->SetBaselineWidth(    GetConfigInt("PMT.BaselineWidth")    );
  fSigExt_PMT->SetIntegrationWidth( GetConfigInt("PMT.IntegrationWidth") );

  //
  //
  fSigExt_AMP = new TPSignalExtractor("AMP", hProj[1]);
  fSigExt_AMP->SetBaselineWidth(    GetConfigInt("AMP.BaselineWidth")    );
  fSigExt_AMP->SetIntegrationWidth( GetConfigInt("AMP.IntegrationWidth") );
  fSigExt_AMP->SetRiseTime(         GetConfigInt("AMP.RiseTime") );

  //
  //
  fSigExt_SHP = new TPSignalExtractor("SHP", hProj[2]);
  fSigExt_SHP->SetBaselineWidth(    GetConfigInt("SHP.BaselineWidth")    );
  fSigExt_SHP->SetIntegrationWidth( GetConfigInt("SHP.IntegrationWidth") );
}

//------------------------------------------------------------------------------
TPManager::~TPManager()
{  
}

//------------------------------------------------------------------------------
void TPManager::SetDrawScale(unsigned int sc=1)
{
  fDrawScale = sc;
  return;
}

//------------------------------------------------------------------------------
void TPManager::DoStart(int interval=1)
{  
  fTimer->Start(interval);
  Info("DoStart()", "-done");
  return;
}

//------------------------------------------------------------------------------
void TPManager::DoStop()
{  
  fTimer->Stop();
  DrawAll();
  Info("DoStop()", "-done");
  return;
}

//------------------------------------------------------------------------------
void TPManager::DoEvent(int ev)
{  
  fCurrEvt = ev; // set the current event
  
  for(int i=0; i<fNChannel; i++) // extract current event from 2D histo
    GetProjection(i, fCurrEvt);
  
  if( ! hProj[0]->GetEntries() ) return; // if event is empty

  // set the start lines
  //
  fSigExt_PMT->FindHits( GetConfigInt("PMT.Threshold"), hProj[0] );

  // time restriction
  //
  static int timelimit = GetConfigInt("Burst.MinDelay");
  if( (fSigExt_PMT->GetStartLine(1)->GetX1() - fSigExt_PMT->GetStartLine(0)->GetX1()) < timelimit )
    return;

  // set the start lines
  //
  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_AMP->SetStartLine( i, fSigExt_PMT->GetStartLine(i)->GetX1());
      fSigExt_SHP->SetStartLine( i, fSigExt_PMT->GetStartLine(i)->GetX1());
    }

  // Estract amplitudes
  //
  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_PMT->GetIntegral(  i, hProj[0], GetConfigFloat("PMT.Scale") );      
      fSigExt_AMP->GetAmplitude( i, hProj[1], GetConfigFloat("AMP.Scale") ); 
      fSigExt_SHP->GetIntegral(  i, hProj[2], GetConfigFloat("SHP.Scale") ); 
    }

  // fill time distributions
  //
  static double normPMT[]={20, 32};
  static double normAMP[]={ 3, 5};
  static double normSHP[]={19, 34};

  if( fCurrEvt == 100 )
    {
      normPMT[0] = fSigExt_PMT->GetSignalDist(0)->GetMean();
      normPMT[1] = fSigExt_PMT->GetSignalDist(1)->GetMean();
      normAMP[0] = fSigExt_AMP->GetSignalDist(0)->GetMean();
      normAMP[1] = fSigExt_AMP->GetSignalDist(1)->GetMean();
      normSHP[0] = fSigExt_SHP->GetSignalDist(0)->GetMean();
      normSHP[1] = fSigExt_SHP->GetSignalDist(1)->GetMean();
    }
  else if( fCurrEvt > 100 )
    {
      //   fSigExt_PMT->FillAmpVsDel();
      //   fSigExt_AMP->FillAmpVsDel();
      //   fSigExt_SHP->FillAmpVsDel();
      
      fSigExt_PMT->FillAmpVsDelNorm100(normPMT);
      fSigExt_AMP->FillAmpVsDelNorm100(normAMP);
      fSigExt_SHP->FillAmpVsDelNorm100(normSHP);
    }
  
  // Draw all histos
  //
  if( fbDrawOnOff )
    if( !(fCurrEvt%fDrawScale) || fCurrEvt==0 )
      DrawAll();
  
  return; 
} 

//------------------------------------------------------------------------------
void TPManager::DoDrawOnOff()
{  
  if( fbDrawOnOff )
    fbDrawOnOff = kFALSE;
  else
    fbDrawOnOff = kTRUE;
  DrawAll();
  return;
}

//------------------------------------------------------------------------------
void TPManager::Next()
{  
  fCurrEvt++;
  
  if( !(fCurrEvt%fTotaEvtPrc) )
    printf(" - analysed %i%% \n", int(fCurrEvt/fTotaEvtPrc) );
  
  DoEvent(fCurrEvt);
  
  if( fCurrEvt > fTotaEvt ) DoStop();

  return;
} 

//------------------------------------------------------------------------------
void TPManager::DrawAll()
{  
  // PMT
  //
  cMain->cd(1);
  hProj[0]->Draw();
  fSigExt_PMT->GetThresholdLine()->Draw();

  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_PMT->GetStartLine(i)->Draw();
      fSigExt_PMT->GetStoppLine(i)->Draw();
      fSigExt_PMT->GetOffsetFunction(i)->Draw("same");
    }

  cMain->cd(4);
  fSigExt_PMT->GetSignalDist(0)->Draw();
  for(int i=1; i<fSigExt_PMT->GetNumbOfHits(); i++)
    fSigExt_PMT->GetSignalDist(i)->Draw("same");

  cMain->cd(7);
  fSigExt_PMT->GetAmpVsDel(0)->Draw();
  for(int i=1; i<fSigExt_PMT->GetNumbOfHits(); i++)
    fSigExt_PMT->GetAmpVsDel(i)->Draw("same");

  // Amp
  //
  cMain->cd(2);
  hProj[1]->Draw();
  
  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_AMP->GetStartLine(i)->Draw();
      fSigExt_AMP->GetStoppLine(i)->Draw();
      fSigExt_AMP->GetOffsetFunction(i)->Draw("same");
      fSigExt_AMP->GetAmplitudeFunct(i)->Draw("same");
    }
  
  cMain->cd(5);
  fSigExt_AMP->GetSignalDist(0)->Draw();
  for(int i=1; i<fSigExt_PMT->GetNumbOfHits(); i++)
    fSigExt_AMP->GetSignalDist(i)->Draw("same");
  
  cMain->cd(8);
  fSigExt_AMP->GetAmpVsDel(0)->Draw();
  for(int i=1; i<fSigExt_PMT->GetNumbOfHits(); i++)
    fSigExt_AMP->GetAmpVsDel(i)->Draw("same");

  // Shp
  //
  cMain->cd(3);
  hProj[2]->Draw();
  
  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_SHP->GetStartLine(i)->Draw();
      fSigExt_SHP->GetStoppLine(i)->Draw();
      fSigExt_SHP->GetOffsetFunction(i)->Draw("same");
    }
  
  cMain->cd(6);
  fSigExt_SHP->GetSignalDist(0)->Draw();
  for(int i=1; i<fSigExt_PMT->GetNumbOfHits(); i++)
    fSigExt_SHP->GetSignalDist(i)->Draw("same");
  
  cMain->cd(9);
  fSigExt_SHP->GetAmpVsDel(0)->Draw();
  for(int i=1; i<fSigExt_PMT->GetNumbOfHits(); i++)
    fSigExt_SHP->GetAmpVsDel(i)->Draw("same");

  cMain->Update();  
  return;
} 

//------------------------------------------------------------------------------
void TPManager::WriteCanvas()
{
  gSystem->Exec("mkdir -v pics");

  TString strFileName = fFile->GetName();
  strFileName.Remove( strFileName.Last('.')  );

  printf("filename : %s \n", strFileName.Data() );

  cMain->Print(strFileName.Append(".png"));

  return;
}
