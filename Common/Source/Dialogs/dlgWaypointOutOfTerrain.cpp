/*
   LK8000 Tactical Flight Computer -  WWW.LK8000.IT
   Released under GNU/GPL License v.2
   See CREDITS.TXT file for authors and copyrights

   $Id: dlgWaypointOutOfTerrain.cpp,v 1.1 2011/12/21 10:29:29 root Exp root $
*/

#include "externs.h"
#include "Dialogs.h"
#include "WindowControls.h"
#include "Waypointparser.h"
#include "dlgTools.h"


static WndForm *wf=NULL;

static void OnYesClicked(WindowControl * Sender){
	(void)Sender;
  wf->SetModalResult(wpTerrainBoundsYes);
}

static void OnYesAllClicked(WindowControl * Sender){
	(void)Sender;
  wf->SetModalResult(wpTerrainBoundsYesAll);
}

static void OnNoClicked(WindowControl * Sender){
	(void)Sender;
  wf->SetModalResult(wpTerrainBoundsNo);
}

static void OnNoAllClicked(WindowControl * Sender){
	(void)Sender;
  wf->SetModalResult(wpTerrainBoundsNoAll);
}


static CallBackTableEntry_t CallBackTable[]={
  ClickNotifyCallbackEntry(OnYesClicked),
  ClickNotifyCallbackEntry(OnYesAllClicked),
  ClickNotifyCallbackEntry(OnNoClicked),
  ClickNotifyCallbackEntry(OnNoAllClicked),
  EndCallBackEntry()
};

int dlgWaypointOutOfTerrain(TCHAR *Message){

  WndFrame* wfrm;
  int res = 0;

    TCHAR filename[MAX_PATH];
    LocalPathS(filename, TEXT("dlgWaypointOutOfTerrain.xml"));
    wf = dlgLoadFromXML(CallBackTable, 
                        filename, 
		        TEXT("IDR_XML_WAYPOINTTERRAIN"));

    if (wf) {

    
      wfrm = (WndFrame*)wf->FindByName(TEXT("frmWaypointOutOfTerrainText"));

      wfrm->SetCaption(Message);
      wfrm->SetCaptionStyle(
          DT_EXPANDTABS
        | DT_CENTER
        | DT_NOCLIP
        | DT_WORDBREAK);


      res = wf->ShowModal();
      delete wf;

    }

    wf = NULL;


  return(res);

}

