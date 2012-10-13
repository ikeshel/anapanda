

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// author: i.keshelashvili@unibas.ch                                    //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TPConfig.hh"


//------------------------------------------------------------------------------
TPConfig::TPConfig()
{
  //  printf("constructor\n");

  // read the main configuration file
  //
  ReadConfigFile("config/config.cfg");
}


//------------------------------------------------------------------------------
TPConfig::~TPConfig()
{
  printf("destructor TPConfig::~TPConfig() \n");
}


//------------------------------------------------------------------------------
void TPConfig::ReadConfigFile(const Char_t* szFin)
{
  //
  ifstream infile;
  
  infile.open( szFin );
  
  if ( !infile.is_open() )
    {
      printf("\n ERROR: opening \"%s\" file ! ! !\n\n", szFin);
      gSystem->Exit(0);
      //exit(0);
    }
  else
    {
      printf("\n ---------------------------------------- \n");
      printf(" Read File : \"%s\"\n", szFin);

      while ( infile.good() )
	{
	  strLine[nLine]="";
	  strLine[nLine].ReadLine(infile);
      
	  if( strLine[nLine].BeginsWith("#") )
	    {
	      // 	      printf("Comment : %s \n", strLine[nLine].Data());
	    }
          //
          else if( strLine[nLine].Contains("FILE") )
            {
              TString strFile = this->ExtractName( strLine[nLine] );
              
              this->ReadConfigFile( strFile.Data() );
  
            }
	  nLine++;
	  if( nLine >= MAX_LINE )
	    {
	      printf("\n ERROR: Number of lines is more than MAX_LINE ! ! !\n\n");
	      gSystem->Exit(0);
	    }
	}
    }
  
  infile.close();
  return;
}

//------------------------------------------------------------------------------
TString TPConfig::ExtractName( TString strIn )
{
  Ssiz_t aa = strIn.First(":")+1;
  Ssiz_t bb = strIn.Length()-aa;
  
  TString cc = strIn(aa, bb);
  cc.ReplaceAll(" ","");

  return cc;
}

//------------------------------------------------------------------------------
TString TPConfig::GetConfigName( TString name )
{
  TString strOut = "";
  
  for( int i=0; i<nLine; i++)
    {
      if( !( strLine[i].BeginsWith("#") ) && 
	  strLine[i].Contains( name )    )
	{
	  strOut = this->ExtractName( strLine[i] );
	}      
    }
  return strOut; 
}

//------------------------------------------------------------------------------
unsigned int TPConfig::GetConfigHex( TString name )
{
  unsigned int out = 0;
  
  for( int i=0; i<nLine; i++)
    {
      if( !( strLine[i].BeginsWith("#") ) && 
	  strLine[i].Contains( name )    )
	{
	  sscanf(this->ExtractName( strLine[i] ).Data(), "%x", &out);
	}      
    }
  return out; 
}

//------------------------------------------------------------------------------
int TPConfig::GetConfigInt( TString name )
{
  int out = 0;
  
  for( int i=0; i<nLine; i++)
    {
      if( !( strLine[i].BeginsWith("#") ) && 
	  strLine[i].Contains( name )    )
	{
	  sscanf(this->ExtractName( strLine[i] ).Data(), "%i", &out);
	}      
    }
  return out; 
}

//------------------------------------------------------------------------------
void TPConfig::WriteConfigFile()
{
  //
  ofstream outfile;
  outfile.open ("config/test.txt");
  
  // >> i/o operations here <<
  
  outfile.close();  
}
