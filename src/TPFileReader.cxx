
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// author: i.keshelashvili@unibas.ch                                    //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TPFileReader.hh"

ClassImp( TPFileReader )

//-----------------------------------------------
TPFileReader::TPFileReader()
{
  // printf(" *************************************************\n"
  // 	 " *                                               *\n"
  // 	 " *                                               *\n"
  // 	 " *                                               *\n"
  // 	 " *                                               *\n"
  // 	 " *************************************************\n");
}

//-----------------------------------------------
TPFileReader::~TPFileReader()
{  
  
}

//-----------------------------------------------
void TPFileReader::ReadHistoFile(char *szFin, int n)
{  	
  // szFin    - filename
  // fNChannel - number of channels 

  fNChannel = n;

  fFile = TFile::Open(szFin);

//   if( !fFile )
//     {
//       Info("TPFileReader", " ERROR opening file!");
//     }
    
//   strcpy(szFileName, fFile->GetName() );
//   printf("-File (%s) %i\n", fFile->GetName(), fFile );

  gROOT->cd();

  char histname[32];

  for(int i=0; i<fNChannel; i++)
    {
      sprintf(histname, "hADC_%i", i);

      h2D[i] = (TH2I*) fFile->Get(histname)->Clone(histname);
    }
  fFile->Close();

  // 
  //
  fTotaEvt = Get2DHisto(0)->GetYaxis()->GetNbins();
  fTotaEvtPrc = int(fTotaEvt/100);

  return;
}

//-----------------------------------------------
TH1D* TPFileReader::GetProjection(int ch, int event)
{
  if( !h2D[ch] ) return 0;
  
//   if( hProj[ch] )
//     {
//       delete hProj[ch];
//       hProj[ch]=0;
//     }

  char projname[32];
  sprintf(projname, "hSADC_%i", ch);
  hProj[ch] = (TH1D*) h2D[ch]->ProjectionX(projname, event, event);
  //  hProj[ch] = (TH1D*) h2D[ch]->ProjectionX(projname, event, event)->Clone();
  hProj[ch]->GetYaxis()->SetRangeUser(0, 4100);
  hProj[ch]->SetStats(0);

  return hProj[ch];
}
