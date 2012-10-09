
////////////////////////////////////////////////////////////////////
//                                                                //
// author: i.keshelashvili@unibas.ch                              //
//                                                                //
//                                                                //
////////////////////////////////////////////////////////////////////

#include "TPManager.hh"

ClassImp( TPManager )

//-----------------------------------------------
TPManager::TPManager()
{
  printf("  ****************************************************\n"
	 "  *                                                  *\n"
	 "  *        W E L C O M E  to  A N A P A N D A        *\n"
	 "  *                                                  *\n"
	 "  *                                                  *\n"
	 "  ****************************************************\n");

  fNBurst = 2;

  fbDrawOnOff = kTRUE;

  fTimer = new TTimer(1);
  fTimer->Connect("Timeout()", "TPManager", this, "Next()");

  cMain = new TCanvas("cMain","Main Canvas", 0, 0, 1000, 800);
  cMain->Divide(3, 3, .001, .001);

  //
  //
  fSigExt_PMT = new TPSignalExtractor("PMT", hProj[0]);
  fSigExt_PMT->SetBaselineWidth(300);
  fSigExt_PMT->SetIntegrationWidth(30);

  //
  //
  fSigExt_AMP = new TPSignalExtractor("AMP", hProj[1]);
  fSigExt_AMP->SetBaselineWidth(300);
  fSigExt_AMP->SetIntegrationWidth(100);
  fSigExt_AMP->SetRiseTime(600);

  //
  //
  fSigExt_SHP = new TPSignalExtractor("SHP", hProj[2]);
  fSigExt_SHP->SetBaselineWidth(300);
  fSigExt_SHP->SetIntegrationWidth(150);
}

//-----------------------------------------------
TPManager::~TPManager()
{  
}

//-----------------------------------------------
void TPManager::DoStart(int interval=1)
{  
  fTimer->Start(interval);
  Info("DoStart()", "-done");
  return;
}

//-----------------------------------------------
void TPManager::DoStop()
{  
  fTimer->Stop();
  Info("DoStop()", "-done");
  return;
}

//-----------------------------------------------
void TPManager::DoEvent(int ev)
{  
  fCurrEvt = ev;

  if( fCurrEvt > fTotaEvt ) DoStop();
  
  for(int i=0; i<fNChannel; i++)
    GetProjection(i, fCurrEvt);

  if( ! hProj[0]->GetEntries() ) return;

  // set the start lines
  //
  fSigExt_PMT->FindHits(3000, hProj[0]);

  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_AMP->SetStartLine( i, 
				 fSigExt_PMT->GetStartLine(i)->GetX1());
      fSigExt_SHP->SetStartLine( i, 
				 fSigExt_PMT->GetStartLine(i)->GetX1());
    }

  // Estract amplitudes
  //
  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_PMT->GetIntegral(i, hProj[0], 7.7); // pmt
      
      fSigExt_AMP->GetAmplitude(i, hProj[1], 1); // PreAmp
      
      fSigExt_SHP->GetIntegral(i, hProj[2], 40); // shp
    }

  if( fbDrawOnOff )
    DrawAll();
  
  return; 
} 

//-----------------------------------------------
void TPManager::DoDrawOnOff()
{  
  if( fbDrawOnOff )
    fbDrawOnOff = kFALSE;
  else
    fbDrawOnOff = kTRUE;

  return;
}

//-----------------------------------------------
void TPManager::Next()
{  
  fCurrEvt++;
  
  if( !(fCurrEvt%fTotaEvtPrc) )
    printf(" - analysed %i%% \n", int(fCurrEvt/fTotaEvtPrc) );
  
  DoEvent(fCurrEvt);
  
  return;
} 

//-----------------------------------------------
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
  
  cMain->Update();  
  return;
} 
