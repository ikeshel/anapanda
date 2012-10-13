
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// LinkDef.h                                                            //
//                                                                      //
// author: i.keshelashvili@unibas.ch                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifdef __CINT__ 

// turn everything off
#pragma link off all globals; 
#pragma link off all classes; 
#pragma link off all functions; 
#pragma link off all typedef; 

#pragma link C++ nestedclasses; 
#pragma link C++ nestedtypedef; 

// 
#pragma link C++ class TPManager+;
#pragma link C++ class TPConfig+;
#pragma link C++ class TPFileReader+;
#pragma link C++ class TPSignalExtractor+;

#endif
