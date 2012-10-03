// Mainframe macro generated from application: /kernph/kernph_x86_64/pkg/root/5.34/gcc4.2/bin/root.exe
// By ROOT version 5.34/01 on 2012-09-25 12:17:49

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TRootCanvas
#include "TRootCanvas.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "Riostream.h"

void control()
{
  int ButtWidth  = 150;
  int ButtHeight = 22;

  // main frame
  //
  TGMainFrame *fMainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
  fMainFrame->SetName("fMainFrame");
  fMainFrame->SetLayoutBroken(kTRUE);

  // vertical frame
  TGVerticalFrame *fVerFr_StaSto = new TGVerticalFrame(fMainFrame,486,96,kVerticalFrame);
  fVerFr_StaSto->SetName("fVerFr_StaSto");

  // "fGroupFrame671" group frame
  TGGroupFrame *topGroupFrame = new TGGroupFrame(fVerFr_StaSto,"Start Stop Control");

  TGTextButton *Start = new TGTextButton(topGroupFrame,"Start/Stop");
  Start->SetTextJustify(36);
  Start->SetMargins(0,0,0,0);
  Start->SetWrapLength(-1);
  Start->Resize(ButtWidth,ButtHeight);
  topGroupFrame->AddFrame(Start, new TGLayoutHints(kLHintsLeft | kLHintsTop,10,0,10,0));

//   TGTextButton *Stop = new TGTextButton(topGroupFrame,"Stop");
//   Stop->SetTextJustify(36);
//   Stop->SetMargins(0,0,0,0);
//   Stop->SetWrapLength(-1);
//   Stop->Resize(ButtWidth,ButtHeight);

//   topGroupFrame->AddFrame(Stop, new TGLayoutHints(kLHintsLeft | kLHintsTop ,2,2,2,2));

  topGroupFrame->SetLayoutManager(new TGVerticalLayout(topGroupFrame));
  topGroupFrame->Resize(482,92);
  fVerFr_StaSto->AddFrame(topGroupFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));

  fMainFrame->AddFrame(fVerFr_StaSto, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));
  //  fVerFr_StaSto->MoveResize(2,2,486,96);

  fMainFrame->SetMWMHints(kMWMDecorAll,
			  kMWMFuncAll,
			  kMWMInputModeless);
  fMainFrame->MapSubwindows();

  fMainFrame->Resize(fMainFrame->GetDefaultSize());
  fMainFrame->MapWindow();
  fMainFrame->MoveResize(1000, 700, 600,400);
}  
