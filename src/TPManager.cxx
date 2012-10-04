
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
  printf(" *************************************************\n"
	 " *                                               *\n"
	 " *                                               *\n"
	 " *                                               *\n"
	 " *                                               *\n"
	 " *************************************************\n");

  fNBurst = 2;

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

  if( fCurrEvt > fTotaEvt )
    DoStop();
  
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

  //
  //
  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_PMT->GetIntegral(i, hProj[0], 10); // pmt
      
      fSigExt_SHP->GetIntegral(i, hProj[2], 30); // shp
    }

  DrawAll();
  
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
  if( ! hProj[0]->GetEntries() ) return;

  // PMT
  //
  cMain->cd(1);
  hProj[0]->Draw();
  fSigExt_PMT->GetThresholdLine()->Draw();

  for(int j=0; j<fSigExt_PMT->GetNumbOfHits(); j++)
    {
      fSigExt_PMT->GetStartLine(j)->Draw();
      fSigExt_PMT->GetStoppLine(j)->Draw();
      fSigExt_PMT->GetOffset(j)->Draw("same");
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
    }
  
  cMain->cd(5);
  fSigExt_AMP->GetSignalDist(0)->Draw();

  // Shp
  //
  cMain->cd(3);
  hProj[2]->Draw();
  
  for(int i=0; i<fSigExt_PMT->GetNumbOfHits(); i++)
    {
      fSigExt_SHP->GetStartLine(i)->Draw();
      fSigExt_SHP->GetStoppLine(i)->Draw();
      fSigExt_SHP->GetOffset(i)->Draw("same");
    }
  
  cMain->cd(6);
  fSigExt_SHP->GetSignalDist(0)->Draw();

  for(int i=1; i<fSigExt_PMT->GetNumbOfHits(); i++)
    fSigExt_SHP->GetSignalDist(i)->Draw("same");
  
  cMain->Update();  
  return;
} 
