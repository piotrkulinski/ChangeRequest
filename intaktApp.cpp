//---------------------------------------------------------------------------
//
// Name:        intaktApp.cpp
// Author:      Piotr Kuliñski (c) piotr.kulinski@gmail.com
// Created:     2008-06-23 16:07:36
// Description: 
//
//---------------------------------------------------------------------------

#include "intaktApp.h"
#include "intaktDlg.h"

//do podania has³a
#include "md5.h"
#include <wx/textdlg.h>

#define dbg( txt ) //wxMessageBox(txt,"DEBUG",wxICON_INFORMATION);

IMPLEMENT_APP(intaktDlgApp)
//DECLARE_APP(intaktDlgApp)

bool intaktDlgApp::OnInit()
{
   bool show=false;
    
   restart=show;
   wxString curdir = ::wxGetCwd();

   if (wxApp::GetAppName().First("$$$intakt")!=wxNOT_FOUND)
   {
       ::wxCopyFile(curdir+"\\$$$intakt.exe",curdir+"\\intakt.exe"); 
       restart=true;
       return (ShellExecute(NULL, NULL, curdir+"\\intakt.exe", NULL, NULL, SW_SHOWNORMAL),0);
       return restart;
   } else {
        if (::wxFileExists(curdir+"\\$$$intakt.exe"))
        {
            ::wxRemoveFile(curdir+"\\$$$intakt.exe");
            if (wxMessageBox(wxT("Program zosta³ zaktualizowany\n\n"\
                                 "Chcesz obejrzeæ zmiany?"),
                             wxT("Informacja"), wxICON_INFORMATION | wxYES_NO )==wxYES) {
                show=true;
            };
        }
    };

//   if (RunTest()) {
//      wxMessageBox(wxT("Program jest ju¿ uruchomiony"),wxT("Uwaga"));
//      exit(1);
//    };
    
//    wxMessageBox("Tu1");	
  wxString summd5 = static_cast<wxString>(md5(wxGetEmailAddress().Lower().c_str()));
  if (summd5.CmpNoCase(_("BA8DB9A8CB3707EC514B840E767F6341"))>0 & //dla laptopa
      summd5.CmpNoCase(_("AA99EB9C70AED9E9B501B0C268BC6F20"))>0 ) //dla stacjonarki
  {
   wxPasswordEntryDialog *haslo = new wxPasswordEntryDialog(0,_("  Podaj has³o uruchomieniowe"),_("Autoryzacja"));
   if (haslo->ShowModal()==wxID_OK)
   {
       if (static_cast<wxString>(md5(haslo->GetValue().c_str())).CmpNoCase(_("D01DAEF32ED9F74D23A257C7D07C5CCB")))
       {
          wxMessageBox(_("B³¹d autoryzacji"),_("B³¹d"));
          if (haslo) delete haslo;         
          return false;
		};
	} else {
      if (haslo) delete haslo;
      return false;
	};
    if (haslo) delete haslo;
  };
	
    wxHelpProvider::Set( new wxSimpleHelpProvider );

	intaktDlg* dialog = new intaktDlg(NULL);
	//usuwa minimalizacjê i wstawia znak pytajnika
    //dialog->SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	SetTopWindow(dialog);
	dialog->Show(true);
	dbg("intaktDlgApp::OnInit()");
	return true;
}
 
int intaktDlgApp::OnExit()
{
    dbg("intaktDlgApp::OnExit()");
	return 0;
}

