//---------------------------------------------------------------------------
//
// Name:        intaktDlg.cpp
// Author:      Piotr Kuliñski (c) piotr.kulinski@gmail.com 
// Created:     2008-06-23 16:07:36
// Description: intaktDlg class implementation
//
//  "@S@";    //serwer
//  "@U@";    //u¿ytkownik
//  "@P@";    //has³o do serwera
//  "@D@";    //baza danych
//  "@DAPP@"; //katalog aplikacji
//  "@DARC@"; //katalog archiwum
//---------------------------------------------------------------------------

/* ChangeLog 2010-06-01
2010-06-01 22:05:09 UTC+0200 Piotr Kuliñski <piotr.kulinski@gmail.com> 
   Poprawka destruktora g³ównego obiektu.
   By³y wywo³ywane destruktory dla klas podpinanych pod kontroli, a nie powinny,
   choæ tak naprawdê nie wiadomo dlaczego. W makrze wxDELETE jest zabezpieczenie
   aby nie wykonywa³ dela na NULL-owych obiektach.
*/

#include <wx/event.h>
#include <wx/dnd.h>
#include <wx/print.h>
#include "intaktApp.h"
#include "Objects/MingW/intakt_private.h"

#include <wx/busyinfo.h>
#include <wx/tooltip.h>
#include <wx/clipbrd.h>
#include <wx/file.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/sound.h>
#include <wx/tokenzr.h>

#include <wx/process.h>
#include <wx/defs.h>
#include <wx/aboutdlg.h>
#include <wx/icon.h>
#include <wx/net/email.h>

#include "intaktTray.h"
#include "DBPrintSettings.h"
#include "intaktDlg.h"
#include "lstPOS.h"
#include "RodzajZgloszenia.h"
#include "IntaktConf.h"
#include "../include/wxdll.h"  //version.dll
#include "../include/tcoder.h"

//obs³uga bufora systemu
#include "buffer.h"
#include "tools.h"
#include "DialogEditApp.h"
#include <wx/dynarray.h>
#include <map>

#define debug( txt ) //wxMessageBox(txt);
#define dbg( txt ) //wxMessageBox(txt,"DEBUG",wxICON_INFORMATION);
#define mbox(txt) wxMessageBox(txt,"DEBUG",wxICON_INFORMATION);

#include "Images/awxled.xpm"
#include "Images/awxledin.xpm"

#include "Images/ID_MM1_XPM.xpm"
#include "Images/intaktDlg_WxStaticBitmap2_XPM.xpm"
//Do not add custom headers
//wxDev-C++ designer will remove them
////Header Include Start
#include "Images/Self_intaktDlg_XPM.xpm"
//#include "Images/intaktDlg_ID_MM1_XPM.xpm"

#include "Images/intaktDlg_btDeleteDatabase_XPM.xpm"
#include "Images/intaktDlg_ctx_help_XPM.xpm"
#include "Images/intaktDlg_WxBitmapButton6_XPM.xpm"
#include "Images/intaktDlg_WxBitmapButton5_XPM.xpm"
#include "Images/intaktDlg_btn_getpos_XPM.xpm"
#include "Images/intaktDlg_btn_getadm_XPM.xpm"
#include "Images/intaktDlg_btn_getusr_XPM.xpm"
#include "Images/intaktDlg_usun_XPM.xpm"
#include "Images/intaktDlg_Save_XPM.xpm"
#include "Images/intaktDlg_WxBitmapButton4_XPM.xpm"
#include "Images/intaktDlg_WxBitmapButton2_XPM.xpm"
#include "Images/intaktDlg_WxBitmapButton1_XPM.xpm"
////Header Include End


#define wxDELETEX(z,x) wxDELETE(x) //wxMessageBox(wxString::Format("%d: %s",__LINE__,z)); if (x) {delete x; x=NULL;};
#define wxDELETEXA(z,x) wxMessageBox(wxString::Format("%d: %s",__LINE__,z)); if (x) {delete [] x; x=NULL;};
//na potrzeby autologowania
HWND hButton ;    // Handle Buttona
HWND h_login;     //login
static unsigned int TEditNN=0;
static unsigned int TButtonNN=0;

template <typename MTyp>	
class DnDText : public wxTextDropTarget
{
public:
    DnDText(MTyp *pOwner) { m_pOwner = pOwner; }
    virtual bool OnDropText(wxCoord x, wxCoord y, const wxString& text);
private:
    MTyp *m_pOwner;
};
 
template <typename MTyp>
bool DnDText<MTyp>::OnDropText(wxCoord, wxCoord, const wxString& text)
{  
   wxClassInfo *iclass = m_pOwner->GetClassInfo();
   if (wxString("wxTextCtrl").CmpNoCase(iclass->GetClassName())==0)
   {
     if (text.Contains("Provider=SQLOLEDB.1")) //if (wxMessageBox(wxT("Wygl¹da na to ¿e kopiujesz Connection String\nczy chcesz aby program ustawi³ kontrolki do po³¹czenia z baz¹?")))
        ((intaktDlg*)((wxTextCtrl*)m_pOwner)->GetParent())->LoadFromConnectionString(text);
     else
        ((wxTextCtrl*)m_pOwner)->WriteText(text);
   }
   else if (wxString("wxComboBox").CmpNoCase(iclass->GetClassName())==0)
     {
      if (text.Contains("Provider=SQLOLEDB.1")) //if (wxMessageBox(wxT("Wygl¹da na to ¿e kopiujesz Connection String\nczy chcesz aby program ustawi³ kontrolki do po³¹czenia z baz¹?")))
         ((intaktDlg*)((wxComboBox*)m_pOwner)->GetParent())->LoadFromConnectionString(text);
      else
         ((wxComboBox*)m_pOwner)->SetValue(text);
      };

    return true;
}

//----------------------------------------------------------------------------
// intaktDlg
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(intaktDlg,wxDialog)
	////Manual Code Start
	//EVT_TEXT(4000,intaktDlg::ed_szablonSelected)
	EVT_TEXT_ENTER(4000,intaktDlg::ed_szablonSelected)
	EVT_COMBOBOX(4000,intaktDlg::ed_szablonSelectedTree)
	EVT_MENU(10101, intaktDlg::TreeMenu)
	EVT_MENU(10102, intaktDlg::TreeMenu)
	EVT_MENU(10103, intaktDlg::TreeMenu)
	EVT_MENU(10104, intaktDlg::TreeMenu)
	EVT_MENU(10105, intaktDlg::TreeMenu)

	EVT_COMBOBOX(10200,intaktDlg::SelectApplication)
	EVT_MENU(10201, intaktDlg::AppMenu)
	EVT_MENU(10202, intaktDlg::AppMenu)
	EVT_MENU(10203, intaktDlg::AppMenu)

	EVT_MENU(10301, intaktDlg::VerMenu)
	EVT_MENU(10302, intaktDlg::VerMenu)
	EVT_MENU(10303, intaktDlg::VerMenu)
	EVT_MENU(7001, intaktDlg::bWersjaClick)
	EVT_MENU(7002, intaktDlg::bWersjaClick)
	EVT_MENU(7003, intaktDlg::bWersjaClick)
	EVT_MENU(7004, intaktDlg::bWersjaClick)
	EVT_MENU(7005, intaktDlg::bWersjaClick)
	EVT_MENU(7006, intaktDlg::bWersjaClick)  //odbiór z komentarzem
	//EVT_RIGHT_DOWN(intaktDlg::intaktDlgLeftDown)
	//EVT_RIGHT_DOWN(ID_SRV_NAME,intaktDlg::intaktDlgLeftDown)
	//EVT_COMMAND_RIGHT_CLICK(ID_SRV_NAME, intaktDlg::TreeMenu)
	//wybór serwera powinien ustawiæ kontrolki z loginem i has³em
	EVT_COMBOBOX(ID_SRV_NAME,intaktDlg::srv_nameSelected)
	EVT_COMBOBOX(10300,intaktDlg::WxChoice4Selected)
	EVT_COMBOBOX(10220,intaktDlg::ItemMove)   //przesuwanie w liœcie dla aplikacji
	EVT_COMBOBOX(10320,intaktDlg::ItemMove)   //przesuwanie w liœcie dla wersji
	//polling
	EVT_IDLE(intaktDlg::OnIdle)
	EVT_TIMER(wxID_ANY, intaktDlg::OnTimer)
	//toolbar
	EVT_TOOL(TBM_EXECSQL,intaktDlg::btExecSQL)
	EVT_TOOL(TBM_HELP, intaktDlg::ctx_helpClick)
	EVT_TOOL(TBM_ASPELL, intaktDlg::cr_clearClick)
	EVT_TOOL(TBM_NEWCR, intaktDlg::cr_new)
	EVT_TOOL(TBM_PASTE, intaktDlg::schowekClick)
	EVT_TOOL(TBM_USECR, intaktDlg::bWersjaClickRibbon)
	EVT_TOOL(TBM_SHOW , intaktDlg::bCRTextClick)
	EVT_TOOL(TBM_CONN , intaktDlg::bConnectionClick)
	EVT_TOOL(TBM_MACRO, intaktDlg::WxBitmapButton7Click)
	EVT_TOOL(TBM_MACROUSR, intaktDlg::MacroMenuShow)
	EVT_TOOL(TBM_BUFF , intaktDlg::memo_sizeClick)
	//menu makr %
	EVT_MENU_RANGE(6001,6200, intaktDlg::OnMenuInsertMemoCR)
	////Manual Code End
	
	EVT_CLOSE(intaktDlg::OnClose)
	EVT_CHAR(intaktDlg::intaktDlgKeyDown)
	EVT_KEY_DOWN(intaktDlg::intaktDlgKeyDown)
	EVT_LEFT_DOWN(intaktDlg::intaktDlgLeftDown)
	EVT_ACTIVATE(intaktDlg::intaktDlgActivate)
	EVT_MENU(ID_MM1 , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_MM2 , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_MM3 , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_MM4 , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_MM5 , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_CONNP , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_CONN , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_S , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_DD , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_U , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_P , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_DAPP , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_DARC , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_VER , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_DT , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_DT1 , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_TM , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_LN , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_L1 , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_L2 , intaktDlg::OnMenuInsertMemoCR)
	EVT_MENU(ID_L3 , intaktDlg::OnMenuInsertMemoCR)
	
	EVT_NOTEBOOK_PAGE_CHANGED(ID_PG_CR,intaktDlg::pg_CRPageChanged)
	EVT_NOTEBOOK_PAGE_CHANGING(ID_PG_CR,intaktDlg::pg_CRPageChanging)
	EVT_BUTTON(ID_WXbtDeleteDatabase,intaktDlg::btDeleteDatabaseClick)
	EVT_BUTTON(ID_WXbtRestore,intaktDlg::btRestoreClick)
	EVT_BUTTON(ID_WXBUTTONCFG,intaktDlg::btn_cfgClick)
	EVT_TOGGLEBUTTON(ID_WXTOGGLEBUTTON1,intaktDlg::TreeRunClick)
	EVT_BUTTON(ID_WXBITMAPBUTTON11,intaktDlg::ctx_helpClick)
	EVT_BUTTON(ID_WXBUTTON4,intaktDlg::aboutClick)
	EVT_BUTTON(ID_WXBITMAPBUTTON60,intaktDlg::WxBitmapButton6Click)
	EVT_BUTTON(ID_WXBITMAPBUTTON53,intaktDlg::btn_getconversion1)
	EVT_BUTTON(ID_BTN_GETPOS,intaktDlg::btn_getposClick)
	EVT_BUTTON(ID_WXBITMAPBUTTON52,intaktDlg::btn_getadmClick)
	EVT_BUTTON(ID_WXBITMAPBUTTON51,intaktDlg::btn_getusrClick)
	EVT_BUTTON(ID_WXBITMAPBUTTON6,intaktDlg::usunClick)
	EVT_BUTTON(ID_WXBITMAPBUTTON5,intaktDlg::SaveClick)
	EVT_CHECKBOX(ID_WXCHECKBOX6,intaktDlg::isviewClick)
	EVT_BUTTON(ID_K2L,intaktDlg::k2lClick)
	EVT_BUTTON(ID_K1P,intaktDlg::k1pClick)
	EVT_BUTTON(ID_K1L,intaktDlg::k1lClick)
	EVT_BUTTON(ID_WXBUTTON3,intaktDlg::WxButton3Click)
	EVT_BUTTON(ID_WXBUTTON2,intaktDlg::run_manager)
	EVT_BUTTON(ID_MYRUN,intaktDlg::run_manager)
	EVT_CHECKBOX(ID_WXCHECKBOX3,intaktDlg::WxCheckBox3Click1)
	EVT_HYPERLINK(ID_WXHYPERLINKCTRL2,intaktDlg::WxButton2Click)
	EVT_BUTTON(ID_DBCOPY,intaktDlg::dbcopyClick)
	EVT_CHECKBOX(ID_WXCHECKBOX1,intaktDlg::WxCheckBox1Click1)
	EVT_CHECKBOX(ID_CR_STAT_PROP,intaktDlg::cr_stat_propClick)
	EVT_BUTTON(ID_CR_CLEAR,intaktDlg::cr_clearClick)
	EVT_BUTTON(ID_SCHOWEK,intaktDlg::UpdateFromFTP)
	EVT_CHECKBOX(ID_BEDZIE_ZALACZNIK,intaktDlg::bedzie_zalacznikClick)
	EVT_CHECKBOX(ID_WXCHECKBOX5,intaktDlg::WxCheckBox5Click)
	EVT_CHECKBOX(ID_WXCHECKBOX4,intaktDlg::WxCheckBox4Click)
	EVT_HYPERLINK(ID_WXHYPERLINKCTRL1,intaktDlg::WxHyperLinkCtrl1HyperLink)
	EVT_BUTTON(ID_GENERUJ,intaktDlg::generujClick)
	EVT_BUTTON(ID_WXBUTTON1,intaktDlg::WxButton1Click)
	EVT_CHECKBOX(ID_WXCHECKBOX2,intaktDlg::WxCheckBox2Click)
	EVT_BUTTON(ID_WXBITMAPBUTTON4,intaktDlg::WxBitmapButton4Click)
	EVT_BUTTON(ID_WXBITMAPBUTTON2,intaktDlg::WxBitmapButton2Click)
	EVT_BUTTON(ID_WXBITMAPBUTTON1,intaktDlg::WxBitmapButton1Click)
	EVT_CHECKBOX(ID_EXT_ZALACZNIKI,intaktDlg::WxCheckBox1Click)
	
	EVT_TEXT_MAXLEN(ID_WXEDIT1107,intaktDlg::nr_crMaxLen)
END_EVENT_TABLE()
////Event Table End

// This is the handler for process termination events
class ControlApp : public wxProcess
{
public:
    ControlApp(intaktDlg *parent, char *txt=NULL)
        : wxProcess(parent)
    {
        m_parent = parent;
        ctxt=txt;
    }

    virtual void OnTerminate(int pid, int status);

protected:
    intaktDlg *m_parent;
    char *ctxt;
};

void ControlApp::OnTerminate(int pid, int status)
{
    m_parent->SetTitle(wxT("Change Request - CR"));
    m_parent->Iconize(false);
    if (ctxt!=NULL)
       wxMessageBox(wxString(ctxt),"");
    delete this;
};

// This is the handler for process termination events
class MyProcess : public wxProcess
{
public:
    MyProcess(intaktDlg *parent, const wxString& cmd, const wxString& pplik)
        : wxProcess(parent), m_cmd(cmd)
    {
        m_parent = parent;
        plik=pplik;
        Redirect(); //na potrzeby przechwytywania wejœcia/wyjœcia
    }

    // instead of overriding this virtual function we might as well process the
    // event from it in the frame class - this might be more convenient in some
    // cases
    virtual void OnTerminate(int pid, int status);
    
    //przechwytywanie (Redirect()) lini poleceñ
    bool HasInput();

protected:
    intaktDlg *m_parent;
    wxString m_cmd;
    wxString plik;
};

bool MyProcess::HasInput()
{
    bool hasInput = false;

    if ( IsInputAvailable() )
    {
        wxTextInputStream tis(*GetInputStream());
        wxString msg;
        msg << tis.ReadLine();
        msg << '\r';
        m_parent->memo->AppendText(msg);
        hasInput = true;
    }

    if ( IsErrorAvailable() )
    {
        wxTextInputStream tis(*GetErrorStream());
        wxString msg;
        msg << _T(" (stderr): ") << tis.ReadLine();
        m_parent->memo->AppendText(msg);
        hasInput = true;
    }

    return hasInput;
};

void MyProcess::OnTerminate(int pid, int status)
{
    wxString libname="7-zip32.dll";
 
     while ( HasInput() );
    
    m_parent->memo->AppendText(wxT("\n--- koniec ---"));
    m_parent->ctrl_cmd=0;
  if (!plik.IsEmpty())
  {  
    int answer = wxMessageBox("Upakowaæ kopiê bazy danych?\n"+plik,"Kopia bazy",wxICON_QUESTION | wxYES_NO);

    if (answer == wxYES)
    {
       try {
          wxDynamicLibrary lib;
          lib.Load(libname);
          if ( !lib.IsLoaded() )
          {
              wxMessageBox("B³¹d ³adowania biblioteki\n"+libname);
          }
          else
          {
              char res[100];
              long int ret=0;
              typedef int (*wSevenZip)(const HWND _hwnd, const char*, char*, long int);
      
              wSevenZip SevenZip = (wSevenZip)lib.GetSymbol("SevenZip");
              if ( !SevenZip )
              {
                  wxMessageBox("Niepoprawne odwo³anie do funkcji w DLL");
              }
              else
              {
                  wxString cur=::wxGetCwd();
                  ::wxSetWorkingDirectory(plik.Mid(0,plik.Last(wxChar('\\'))+1));
                  answer=SevenZip(0,"a -tzip \""+plik+".zip\" \""+plik.substr(plik.Last(wxChar('\\'))+1)+"\"",res,ret);  
                  if (answer==0 && ::wxFileExists(plik+".zip"))
                  {
                    if (wxMessageBox("Plik upakowany\nUsun¹æ rozpakowan¹ kopiê?","Upakowano",wxICON_QUESTION | wxYES_NO)==wxYES)
                       ::wxRemoveFile(plik);
                  };
                  ::wxSetWorkingDirectory(cur);
              };
           lib.Unload();
          };
      
          } catch(...) {
            wxMessageBox(_T("Problem z 7-zip32.dll"),_T("B³¹d"));
          };
    };
  }; //pplik

    delete this;
};


intaktDlg::intaktDlg(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
:wxDialog(parent, id, title, position, size, style)
{
    CreateGUIControls();
}

/* ChangeLog 2010-06-01
2010-06-01 22:05:09 UTC+0200 Piotr Kuliñski <piotr.kulinski@gmail.com> 
   Poprawka destruktora g³ównego obiektu.
   By³y wywo³ywane destruktory dla klas podpinanych pod kontroli, a nie powinny,
   choæ tak naprawdê nie wiadomo dlaczego. W makrze wxDELETE jest zabezpieczenie
   aby nie wykonywa³ dela na NULL-owych obiektach.
*/
intaktDlg::~intaktDlg()
{   
    wxBusyInfo wait(wxT("Zwalnianie zasobów..."));
    this->Freeze();

	wxDELETEX("aspell",aspell);
	wxDELETEX("WxPopupMenu1",WxPopupMenu1);
	wxDELETEX("WxFontDialog1",WxFontDialog1);
	wxDELETEX("WyborKatalogu",WyborKatalogu);
	wxDELETEX("WxOpenFileDialog1",WxOpenFileDialog1);
	wxDELETEX("WxStaticText8",WxStaticText8);
	wxDELETEX("TreeRun",TreeRun);
	wxDELETEX("ctx_help",ctx_help);
	wxDELETEX("about",about);
	wxDELETEX("WxStaticLine7",WxStaticLine7);
	wxDELETEX("r_zapytaj",r_zapytaj);
	wxDELETEX("WxStaticLine5",WxStaticLine5);
	wxDELETEX("tcr_pow",tcr_pow);
	wxDELETEX("cr_powiazane",cr_powiazane);
	wxDELETEX("WxBitmapButton6",WxBitmapButton6);
	wxDELETEX("WxBitmapButton5",WxBitmapButton5);
	wxDELETEX("btn_getpos",btn_getpos);
	wxDELETEX("btn_getadm",btn_getadm);
	wxDELETEX("btn_getusr",btn_getusr);
	wxDELETEX("usun",usun);
	wxDELETEX("Save",Save);
	wxDELETEX("st_szablon",st_szablon);
	wxDELETEX("r_critical_dkj",r_critical_dkj);
	wxDELETEX("r_usterka_dkj",r_usterka_dkj);
	wxDELETEX("WxStaticText4",WxStaticText4);
	wxDELETEX("isview",isview);
	wxDELETEX("m5",m5);
	wxDELETEX("lm5",lm5);
	wxDELETEX("m4",m4);
	wxDELETEX("lm4",lm4);
	wxDELETEX("m3",m3);
	wxDELETEX("lm3",lm3);
	wxDELETEX("lm2",lm2);
	wxDELETEX("m2",m2);
	wxDELETEX("lm1",lm1);
	wxDELETEX("m1",m1);
	wxDELETEX("k2l",k2l);
	wxDELETEX("k1p",k1p);
	wxDELETEX("k1l",k1l);
	wxDELETEX("WxButton3",WxButton3);
	wxDELETEX("r_unt",r_unt);
	wxDELETEX("r_inne",r_inne);
	wxDELETEX("r_test",r_test);
	wxDELETEX("r_migracja",r_migracja);
	wxDELETEX("r_rozwojowe",r_rozwojowe);
	wxDELETEX("r_critical",r_critical);
	wxDELETEX("r_support",r_support);
	wxDELETEX("r_drobna_zmiana",r_drobna_zmiana);
	wxDELETEX("r_usterka",r_usterka);
	wxDELETEX("WxStaticBox1",WxStaticBox1);
	wxDELETEX("WxStaticLine2",WxStaticLine2);
	wxDELETEX("WxStaticLine1",WxStaticLine1);
	wxDELETEX("PosHotel",PosHotel);
	wxDELETEX("ComputerName",ComputerName);
	wxDELETEX("WxButton2",WxButton2);
	wxDELETEX("WxStaticText2",WxStaticText2);
	wxDELETEX("WxStaticText1",WxStaticText1);
	wxDELETEX("myrun",myrun);
	wxDELETEX("WxCheckBox3",WxCheckBox3);
	wxDELETEX("WxHyperLinkCtrl2",WxHyperLinkCtrl2);
	wxDELETEX("dbcopy",dbcopy);
	wxDELETEX("version",version);
	wxDELETEX("WxCheckBox1",WxCheckBox1);
	wxDELETEX("cr_stat_propozycja",cr_stat_propozycja);
	wxDELETEX("cr_stat_quest",cr_stat_quest);
	wxDELETEX("cr_stat_prop",cr_stat_prop);
	wxDELETEX("cr_stat_uzg",cr_stat_uzg);
	wxDELETEX("cr_clear",cr_clear);
	wxDELETEX("cr_status",cr_status);
	wxDELETEX("issound",issound);
	wxDELETEX("schowek",schowek);
	wxDELETEX("bedzie_zalacznik",bedzie_zalacznik);
	wxDELETEX("WxCheckBox5",WxCheckBox5);
	wxDELETEX("WxCheckBox4",WxCheckBox4);
	wxDELETEX("WxDatePickerCtrl1",WxDatePickerCtrl1);
	wxDELETEX("WxHyperLinkCtrl1",WxHyperLinkCtrl1);
	wxDELETEX("db_name",db_name);
	wxDELETEX("WxStaticText6",WxStaticText6);
	wxDELETEX("WxEdit7",WxEdit7);
	wxDELETEX("WxEdit6",WxEdit6);
	wxDELETEX("generuj",generuj);
	wxDELETEX("WxButton1",WxButton1);
	wxDELETEX("WxCheckBox2",WxCheckBox2);
	wxDELETEX("WxBitmapButton4",WxBitmapButton4);
	wxDELETEX("WxEdit5",WxEdit5);
	wxDELETEX("WxBitmapButton2",WxBitmapButton2);
	wxDELETEX("WxBitmapButton1",WxBitmapButton1);
	wxDELETEX("WxEdit3",WxEdit3);
	wxDELETEX("WxStaticText7",WxStaticText7);
	wxDELETEX("WxEdit1",WxEdit1);
	wxDELETEX("ext_zalaczniki",ext_zalaczniki);
	wxDELETEX("WxStaticText5",WxStaticText5);
	wxDELETEX("srv_login",srv_login);
	wxDELETEX("WxStaticText3",WxStaticText3);
	wxDELETEX("nr_cr",nr_cr);
	wxDELETEX("ctrl_szablon",ctrl_szablon);
	//wxDELETEX("edn_szablon",edn_szablon);
	//wxDELETEXA("imageList",imageList);
	wxDELETEX("srv_name",srv_name);
	wxDELETEX("m_taskBarIcon",m_taskBarIcon);
	wxDELETEX("WxMemo1",WxMemo1);
	wxDELETEX("memo",memo);
	wxDELETEX("WxEdit2",WxEdit2);
	wxDELETEX("app_ctrl",app_ctrl);
	//wxDELETEX("app_list",app_list);
	wxDELETEX("ver_ctrl",ver_ctrl);
	//wxDELETEX("ver_list",ver_list);
    wxDELETEX("tb",tb);
	wxDELETEX("WxToolbarPanel",WxToolbarPanel);
//    wxDELETEX("tbDB1",tbDB1);
//    wxDELETEX("tbDB2",tbDB2);
//	wxDELETEX("WxToolbar1DBase",WxToolbar1DBase);
//	wxDELETEX("WxToolbar2DBase",WxToolbar2DBase);
	
    if (restart)
       ShellExecute(NULL, NULL, "$$$intakt.exe", NULL, NULL, SW_SHOWNORMAL);
       	
};

void intaktDlg::CreateGUIControls()
{
    wxBusyInfo wait(wxT("£adowanie zasobów..."));
    
        mcopyright = "(c) Piotr Kuliñski, 2009-2010";
        mcopyright_nopl = "(c) Piotr Kuliñski, 2009-2010";
        working_dir=::wxGetCwd();

        //ustawienie maski generowania, w zale¿noœci od ustawionych wstepnie bitów do
        //bufora pójd¹ odpowiednie wpisy
        //| BIT_SHOWWIN nie pokazujemy
        mask = (BIT_CONNECTION |
                BIT_BACKUP |
                BIT_VERSION |
                BIT_TITLE |
                BIT_CR |
                BIT_ZALACZNIK |
                BIT_CRPOWIAZANE );

        //czas trwania ToolTip/Hint
        wxToolTip::SetDelay(40000);

        section[SEC_APPLICATION] = "application";
        section[SEC_DBASE]       = "dbase";
        section[SEC_SERVER]      = "server";
        section[SEC_VERSION]     = "version";
        section[SEC_BACKUP]      = "backup";
        section[SEC_SOUND]       = "sound";
        section[SEC_APP]         = "app";
        section[SEC_OTHER]       = "other";
        section[SEC_SZABLONY]    = "szablony";
        section[SEC_MACRODEF]    = "macrodef";        
        section[SEC_PAGE]        = "page";
        
        ini_macro[0] = "@S@"; //serwer
        ini_macro[1] = "@U@"; //uzytkownik
        ini_macro[2] = "@P@"; //has³o do serwera
        ini_macro[3] = "@D@"; //baza danych
        ini_macro[4] = "@DAPP@"; //kalatlog aplikacji

        baza="intakt.ini";

    if (!xladuj_bazy(arrayStringFor_WxChoice1,baza,section[SEC_APPLICATION]))
        {
        arrayStringFor_WxChoice1.Add(wxT("Hotel"))   ; real_app.Add(wxT("hotel\\hotel.exe"));
        arrayStringFor_WxChoice1.Add(wxT("Manager")) ; real_app.Add(wxT("manager\\manager.exe"));
        arrayStringFor_WxChoice1.Add(wxT("Pos"))     ; real_app.Add(wxT("pos\\pos.exe"));
        arrayStringFor_WxChoice1.Add(wxT("Catering")); real_app.Add(wxT("catering\\catering.exe"));
        arrayStringFor_WxChoice1.Add(wxT("Centrala")); real_app.Add(wxT("centrala\\centrala.exe"));
    } else {
      size_t n;
      wxString s;
      for (size_t q=0; q<arrayStringFor_WxChoice1.Count();q++)
      {
        s = arrayStringFor_WxChoice1.Item(q);
        n = s.Find('=');
        if (n>0) {
            arrayStringFor_WxChoice1[q]=s.SubString(0,n-1).Trim().Trim(true);
            s=s.SubString(n+1,s.Length());
            real_app.Add(s.Trim().Trim(true));
        } else {
          real_app.Add(s+".exe");
        };
      };
    };

    if (!xladuj_bazy(arrayStringFor_db_name,baza,section[SEC_DBASE]))
    {
    arrayStringFor_db_name.Add(wxT("dkj_hotel_fr11"));
    };

    if (!xladuj_bazy(arrayStringFor_srv_name,baza,section[SEC_SERVER]))
    {
    arrayStringFor_srv_name.Add(wxT("test-dkj,1435;sa;nnn"));
    arrayStringFor_srv_name.Add(wxT("pkulinski,1435;sa;nnn"));
    }; 

    for (size_t q=0; q<arrayStringFor_srv_name.Count();q++)
    {
      wxStringTokenizer s(arrayStringFor_srv_name.Item(q), wxT(";"));
      real_pass.Add(arrayStringFor_srv_name.Item(q));
      arrayStringFor_srv_name[q]=s.GetNextToken();
    };

    wxArrayString arrayStringFor_WxChoice4;
    if (!xladuj_bazy(arrayStringFor_WxChoice4,baza,section[SEC_VERSION]))
    {
    arrayStringFor_WxChoice4.Add(wxT("2006.3.fr11=c:\\"));
    };
    for (size_t q=0; q<arrayStringFor_WxChoice4.Count();q++)
    {
      wxStringTokenizer s(arrayStringFor_WxChoice4.Item(q), wxT("="));
      arrayStringFor_WxChoice4[q]=s.GetNextToken();
      real_path.Add(s.GetNextToken());
    };

    //za³adowanie  zak³adek
    //LoadAllPages();

    m_taskBarIcon = new MyTaskBarIcon(this);
    if (!m_taskBarIcon->SetIcon(wxIcon(intaktDlg_WxStaticBitmap2_XPM_xpm /*Self_intaktDlg_XPM*/), wxT("CR - Change Request")))
        wxMessageBox(wxT("Nie mogê ustawiæ ikony."));

	//wxArrayString arrayStringFor_srv_name;
	srv_name = new wxMyComboBox(this, ID_SRV_NAME, wxT("srv_name"), wxPoint(52, 104), wxSize(225, 21), arrayStringFor_srv_name, 0, wxDefaultValidator, wxT("srv_name"));

    WxEdit2 = new wxTextCtrl(this, ID_WXEDIT2, wxT(""), wxPoint(334,85), wxSize(555,31), wxTE_RICH2 , wxDefaultValidator, wxT("WxEdit2"));
    WxEdit2->SetFont(wxFont(12, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Segoe UI")));
    WxEdit2->SetDropTarget(new DnDText<wxTextCtrl>(WxEdit2));
    
    WxMemo1 = new wxTextCtrl(this, ID_WXMEMO1, wxT(""), wxPoint(289,118), wxSize(600,382), wxTE_RICH2 |wxSTATIC_BORDER | wxTAB_TRAVERSAL | wxTE_MULTILINE, wxDefaultValidator, wxT("WxMemo1"));
    WxMemo1->SetDropTarget(new DnDText<wxTextCtrl>(WxMemo1));
    WxMemo1->SetMaxLength(0);
    WxMemo1->SetFocus();
    WxMemo1->SetInsertionPointEnd();
    WxMemo1->SetBackgroundColour(wxColour(255,245,236));
    WxMemo1->SetForegroundColour(wxColour(*wxBLACK));
    WxMemo1->SetFont(wxFont(12, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Verdana")));
    WxMemo1->SetHelpText(wxT("Makra które mo¿na u¿ywaæ w tym polu to:\n"\
                            "_______________________________________\n"\
                            "@S@ - serwer\n"\
                            "@U@ - u¿ytkownik\n"\
                            "@P@ - has³o do serwera\n"\
                            "@D@ - baza danych\n"\
                            "@DAPP@ - katalog aplikacji\n"\
                            "@DARC@ - katalog archiwum\n"\
                            "@CONN@ - Connection String\n"\
                            "@CONP@ - Connection String dla POSa\n"\
                            "@VER@ - Wersja pliku\n"\
                            "@DT@ - aktualna data\n"\
                            "@DT+y,m,t,d@ - aktualna data + przesuniêcie w czasie\n"\
                            "@TM@ - aktualny czas\n"\
                            "@LN:n:s@ - linia sk³adaj¹ca siê z <n> ci¹gów <s>\n"\
                            "@L1@ - linia 10 znaków '-'\n"\
                            "@L2@ - linia 10 znaków '='\n"\
                            "@L3@ - linia 10 znaków '.'\n"\
                            "@M1@-@M5@ - w³asne definiowalne makra\n"\
                            "@MEMO01@-@MEMO05@ - zawartoœæ zak³adki memo\n"\
                            "%macrodef% - makra\\akronimy definiowane przez u¿ytkownika"
                            ));

    ////GUI Items Creation Start

	wxInitAllImageHandlers();   //Initialize graphic format handlers

	nr_cr = new wxTextCtrl(this, ID_WXEDIT1107, wxT(""), wxPoint(334, 50), wxSize(101, 31), 0, wxDefaultValidator, wxT("nr_cr"));
	nr_cr->SetMaxLength(5);
	nr_cr->SetForegroundColour(wxColour(0,0,128));
	nr_cr->SetFont(wxFont(18, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Microsoft Sans Serif")));

	WxStaticText3 = new wxStaticText(this, ID_WXSTATICTEXT3, wxT("Server:"), wxPoint(7, 107), wxDefaultSize, 0, wxT("WxStaticText3"));

	srv_login = new wxStaticText(this, ID_SRV_LOGIN, wxT("Login:"), wxPoint(7, 131), wxDefaultSize, 0, wxT("srv_login"));

	WxStaticText5 = new wxStaticText(this, ID_WXSTATICTEXT5, wxT("Temat:"), wxPoint(288, 91), wxDefaultSize, 0, wxT("WxStaticText5"));

	ext_zalaczniki = new wxCheckBox(this, ID_EXT_ZALACZNIKI, wxT("za³¹czniki niestandardowe"), wxPoint(289, 575), wxSize(157, 18), 0, wxDefaultValidator, wxT("ext_zalaczniki"));

	WxEdit1 = new wxTextCtrl(this, ID_WXEDIT1, wxT(""), wxPoint(7, 474), wxSize(237, 24), 0, wxDefaultValidator, wxT("WxEdit1"));

	WxStaticText7 = new wxStaticText(this, ID_WXSTATICTEXT7, wxT("Opis zg³oszenia"), wxPoint(503, 10), wxDefaultSize, 0, wxT("WxStaticText7"));
	WxStaticText7->SetForegroundColour(wxColour(0,128,128));
	WxStaticText7->SetFont(wxFont(16, wxSWISS, wxITALIC, wxBOLD, false, wxT("Verdana")));

	WxEdit3 = new wxTextCtrl(this, ID_WXEDIT3, wxT(".\\positive\\screenshot\\"), wxPoint(289, 594), wxSize(213, 22), 0, wxDefaultValidator, wxT("WxEdit3"));
	WxEdit3->Enable(false);

	wxBitmap WxBitmapButton1_BITMAP (intaktDlg_WxBitmapButton1_XPM);
	WxBitmapButton1 = new wxBitmapButton(this, ID_WXBITMAPBUTTON1, WxBitmapButton1_BITMAP, wxPoint(504, 595), wxSize(23, 21), wxBU_AUTODRAW, wxDefaultValidator, wxT("WxBitmapButton1"));
	WxBitmapButton1->Enable(false);

	wxBitmap WxBitmapButton2_BITMAP (intaktDlg_WxBitmapButton2_XPM);
	WxBitmapButton2 = new wxBitmapButton(this, ID_WXBITMAPBUTTON2, WxBitmapButton2_BITMAP, wxPoint(251, 477), wxSize(23, 21), wxBU_AUTODRAW, wxDefaultValidator, wxT("WxBitmapButton2"));

	WxEdit5 = new wxTextCtrl(this, ID_WXEDIT5, wxT(""), wxPoint(7, 449), wxSize(237, 21), 0, wxDefaultValidator, wxT("WxEdit5"));
	WxEdit5->Enable(false);
	WxEdit5->SetHelpText(wxT("Katalog instalacyjny zostanie dope³niony na podstawie wybranej aplikacji."));

	wxBitmap WxBitmapButton4_BITMAP (intaktDlg_WxBitmapButton4_XPM);
	WxBitmapButton4 = new wxBitmapButton(this, ID_WXBITMAPBUTTON4, WxBitmapButton4_BITMAP, wxPoint(251, 449), wxSize(23, 21), wxBU_AUTODRAW, wxDefaultValidator, wxT("WxBitmapButton4"));
	WxBitmapButton4->Enable(false);

	WxCheckBox2 = new wxCheckBox(this, ID_WXCHECKBOX2, wxT("Kopia bazy danych"), wxPoint(566, 72), wxSize(118, 17), 0, wxDefaultValidator, wxT("WxCheckBox2"));
	WxCheckBox2->Show(false);
	WxCheckBox2->Enable(false);

	WxButton1 = new wxButton(this, ID_WXBUTTON1, wxT("Font"), wxPoint(1030, 570), wxSize(61, 19), 0, wxDefaultValidator, wxT("WxButton1"));

	generuj = new wxButton(this, ID_GENERUJ, wxT("Z&g³oszenie"), wxPoint(755, 543), wxSize(135, 46), 0, wxDefaultValidator, wxT("generuj"));
	generuj->SetFont(wxFont(14, wxSWISS, wxNORMAL, wxNORMAL, false));

	WxEdit6 = new wxTextCtrl(this, ID_WXEDIT6, wxT("sa"), wxPoint(52, 128), wxSize(224, 22), 0, wxDefaultValidator, wxT("WxEdit6"));

	WxEdit7 = new wxTextCtrl(this, ID_WXEDIT7, wxT("password2"), wxPoint(52, 152), wxSize(224, 22), wxTE_PASSWORD, wxDefaultValidator, wxT("WxEdit7"));

	wxArrayString arrayStringFor_db_name;
	db_name = new wxComboBox(this, ID_DB_NAME, wxT(""), wxPoint(52, 177), wxSize(224, 22), arrayStringFor_db_name, 0, wxDefaultValidator, wxT("db_name"));
	db_name->SetBackgroundColour(wxColour(143,231,118));
	db_name->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("????")));

	WxHyperLinkCtrl1 = new wxHyperlinkCtrl(this, ID_WXHYPERLINKCTRL1, wxT("Baza"), wxT("http://wxdsgn.sf.net"), wxPoint(7, 179), wxSize(34, 18), wxNO_BORDER | wxHL_CONTEXTMENU, wxT("WxHyperLinkCtrl1"));
	WxHyperLinkCtrl1->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, true, wxT("MS Sans Serif")));

	wxDateTime WxDatePickerCtrl1_Date(24,wxDateTime::Jun,2008,23,22,22,562);
	WxDatePickerCtrl1 = new wxDatePickerCtrl(this, ID_WXDATEPICKERCTRL1, WxDatePickerCtrl1_Date, wxPoint(288, 14), wxSize(90, 21) , wxDP_DROPDOWN, wxDefaultValidator, wxT("WxDatePickerCtrl1"));
	WxDatePickerCtrl1->Enable(false);

	WxCheckBox4 = new wxCheckBox(this, ID_WXCHECKBOX4, wxT("data kompilacji"), wxPoint(384, 17), wxSize(95, 16), 0, wxDefaultValidator, wxT("WxCheckBox4"));
	WxCheckBox4->Enable(false);

	WxCheckBox5 = new wxCheckBox(this, ID_WXCHECKBOX5, wxT("Aplikacja"), wxPoint(7, 45), wxSize(59, 17), wxALIGN_RIGHT, wxDefaultValidator, wxT("WxCheckBox5"));

	bedzie_zalacznik = new wxCheckBox(this, ID_BEDZIE_ZALACZNIK, wxT("do³¹czam za³¹czniki do CR"), wxPoint(289, 555), wxSize(175, 18), 0, wxDefaultValidator, wxT("bedzie_zalacznik"));

	schowek = new wxButton(this, ID_SCHOWEK, wxT("Aktualizacja"), wxPoint(809, 55), wxSize(82, 19), 0, wxDefaultValidator, wxT("schowek"));

	issound = new wxCheckBox(this, ID_ISSOUND, wxT("generuj z dŸwiêkiem"), wxPoint(560, 555), wxSize(125, 19), 0, wxDefaultValidator, wxT("issound"));
	issound->SetValue(true);

	cr_status = new wxStaticBox(this, ID_CR_STATUS, wxT("Status"), wxPoint(910, 289), wxSize(181, 109));
	cr_status->SetToolTip(wxT("#UZG - uzgodniæ\n#PZM - propozycja zmian\n#ASK - zapytanie"));
	cr_status->SetHelpText(wxT("#UZG - uzgodniæ\n#PZM - propozycja zmian\n#ASK - zapytanie"));

	cr_clear = new wxButton(this, ID_CR_CLEAR, wxT("&MySpell"), wxPoint(733, 6), wxSize(64, 19), 0, wxDefaultValidator, wxT("cr_clear"));
	cr_clear->Show(false);

	cr_stat_uzg = new wxCheckBox(this, ID_CR_STAT_UZG, wxT("uzgodniæ"), wxPoint(922, 305), wxSize(158, 17), 0, wxDefaultValidator, wxT("cr_stat_uzg"));
	cr_stat_uzg->SetForegroundColour(wxColour(255,0,0));
	cr_stat_uzg->SetFont(wxFont(9, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Segoe UI")));

	cr_stat_prop = new wxCheckBox(this, ID_CR_STAT_PROP, wxT("propozycja"), wxPoint(922, 327), wxSize(158, 16), 0, wxDefaultValidator, wxT("cr_stat_prop"));
	cr_stat_prop->SetForegroundColour(wxColour(0,128,0));
	cr_stat_prop->SetFont(wxFont(9, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Segoe UI")));

	cr_stat_quest = new wxCheckBox(this, ID_CR_STAT_QUEST, wxT("zapytanie"), wxPoint(922, 348), wxSize(158, 17), 0, wxDefaultValidator, wxT("cr_stat_quest"));
	cr_stat_quest->SetForegroundColour(wxColour(0,0,255));
	cr_stat_quest->SetFont(wxFont(9, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Segoe UI")));

	cr_stat_propozycja = new wxCheckBox(this, ID_CR_STAT_PROPOZYCJA, wxT("projekt zmian"), wxPoint(922, 370), wxSize(156, 16), 0, wxDefaultValidator, wxT("cr_stat_propozycja"));
	cr_stat_propozycja->SetForegroundColour(wxColour(0,0,255));
	cr_stat_propozycja->SetFont(wxFont(9, wxSWISS, wxNORMAL, wxNORMAL, false, wxT("Segoe UI")));

	WxCheckBox1 = new wxCheckBox(this, ID_WXCHECKBOX1, wxT("katalog instalacyjny aplikacji"), wxPoint(7, 427), wxSize(165, 20), 0, wxDefaultValidator, wxT("WxCheckBox1"));

	version = new wxCheckBox(this, ID_VERSION, wxT("version"), wxPoint(176, 427), wxSize(64, 19), 0, wxDefaultValidator, wxT("version"));
	version->SetToolTip(wxT("Do³¹czenie informacji o wersji pliku wykonywalnego"));
	version->SetValue(true);

	dbcopy = new wxButton(this, ID_DBCOPY, wxT("Wykonaj kopiê"), wxPoint(7, 503), wxSize(113, 27), 0, wxDefaultValidator, wxT("dbcopy"));
	dbcopy->SetToolTip(wxT("Program automatycznie utworzy nazwê pliku na podanej œcie¿ce"));

	WxHyperLinkCtrl2 = new wxHyperlinkCtrl(this, ID_WXHYPERLINKCTRL2, wxT("Has³o"), wxT("http://wxdsgn.sf.net"), wxPoint(7, 155), wxSize(40, 17), wxNO_BORDER | wxHL_CONTEXTMENU, wxT("WxHyperLinkCtrl2"));
	WxHyperLinkCtrl2->SetFont(wxFont(8, wxSWISS, wxNORMAL, wxNORMAL, true, wxT("MS Sans Serif")));

	WxCheckBox3 = new wxCheckBox(this, ID_WXCHECKBOX3, wxT("zapytaj"), wxPoint(464, 556), wxSize(81, 19), 0, wxDefaultValidator, wxT("WxCheckBox3"));
	WxCheckBox3->SetValue(true);

	myrun = new wxButton(this, ID_MYRUN, wxT("Uruchom"), wxPoint(563, 52), wxSize(73, 19), 0, wxDefaultValidator, wxT("myrun"));
	myrun->Show(false);
	myrun->Enable(false);

	WxStaticText1 = new wxStaticText(this, ID_WXSTATICTEXT1, wxT("ComputerName / LOCAL"), wxPoint(96, 249), wxDefaultSize, 0, wxT("WxStaticText1"));

	WxStaticText2 = new wxStaticText(this, ID_WXSTATICTEXT2, wxT("Pos"), wxPoint(96, 293), wxDefaultSize, 0, wxT("WxStaticText2"));

	WxButton2 = new wxButton(this, ID_WXBUTTON2, wxT("&Uruchom"), wxPoint(7, 348), wxSize(269, 55), 0, wxDefaultValidator, wxT("WxButton2"));
	WxButton2->SetFont(wxFont(16, wxSWISS, wxNORMAL, wxNORMAL, false));

	wxArrayString arrayStringFor_ComputerName;
	arrayStringFor_ComputerName.Add(wxT("adm"));
	arrayStringFor_ComputerName.Add(wxT("Adm"));
	arrayStringFor_ComputerName.Add(wxT("administrator"));
	arrayStringFor_ComputerName.Add(wxT("LOKAL_(%X)"));
	ComputerName = new wxComboBox(this, ID_COMPUTERNAME, wxT("adm"), wxPoint(95, 267), wxSize(180, 21), arrayStringFor_ComputerName, wxCB_SORT, wxDefaultValidator, wxT("ComputerName"));
	ComputerName->SetToolTip(wxT("Nazwa komputera - wrzucana w pliki ini"));

	wxArrayString arrayStringFor_PosHotel;
	arrayStringFor_PosHotel.Add(wxT("HOTEL_1"));
	arrayStringFor_PosHotel.Add(wxT("HOTEL1"));
	arrayStringFor_PosHotel.Add(wxT("POS_1"));
	arrayStringFor_PosHotel.Add(wxT("POS_1:no:10.0.1.240"));
	arrayStringFor_PosHotel.Add(wxT("POR_MASTER"));
	arrayStringFor_PosHotel.Add(wxT("MASTER"));
	PosHotel = new wxComboBox(this, ID_POSHOTEL, wxT("HOTEL_1"), wxPoint(95, 311), wxSize(180, 21), arrayStringFor_PosHotel, wxCB_SORT, wxDefaultValidator, wxT("PosHotel"));
	PosHotel->SetToolTip(wxT("Nazwa POS'a - wrzucana w pliki konfiguracyjne"));

	WxStaticLine1 = new wxStaticLine(this, ID_WXSTATICLINE1, wxPoint(283, 5), wxSize(-1, 625), wxLI_VERTICAL);

	WxStaticLine2 = new wxStaticLine(this, ID_WXSTATICLINE2, wxPoint(898, 10), wxSize(-1, 620), wxLI_VERTICAL);
	WxStaticLine2->SetForegroundColour(wxColour(wxT("GREEN")));
	WxStaticLine2->SetBackgroundColour(wxColour(wxT("GREEN")));

	WxStaticBox1 = new wxStaticBox(this, ID_WXSTATICBOX1, wxT(" Rodzaj "), wxPoint(910, 15), wxSize(181, 266));
	WxStaticBox1->SetHelpText(wxT("Okreœlenie rodzaju zg³oszenia. Informacja o rodzaju CR zostanie podpiêta na koñcu CR."));

	r_usterka = new wxCheckBox(this, ID_R_USTERKA, wxT("Usterka"), wxPoint(920, 55), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_usterka"));
	r_usterka->SetToolTip(wxT("@r1@"));

	r_drobna_zmiana = new wxCheckBox(this, ID_R_DROBNA_ZMIANA, wxT("Drobna zmiana"), wxPoint(920, 75), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_drobna_zmiana"));
	r_drobna_zmiana->SetToolTip(wxT("@r2@"));

	r_support = new wxCheckBox(this, ID_R_SUPPORT, wxT("Support"), wxPoint(920, 115), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_support"));
	r_support->SetToolTip(wxT("@r4@"));

	r_critical = new wxCheckBox(this, ID_R_CRITICAL, wxT("B³¹d krytyczny"), wxPoint(920, 35), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_critical"));
	r_critical->SetToolTip(wxT("@r0@"));

	r_rozwojowe = new wxCheckBox(this, ID_R_ROZWOJOWE, wxT("Prace rozwojowe"), wxPoint(920, 95), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_rozwojowe"));
	r_rozwojowe->SetToolTip(wxT("@r3@"));

	r_migracja = new wxCheckBox(this, ID_R_MIGRACJA, wxT("Migracja danych"), wxPoint(920, 135), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_migracja"));
	r_migracja->SetToolTip(wxT("@r5@"));

	r_test = new wxCheckBox(this, ID_R_TEST, wxT("Test"), wxPoint(920, 155), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_test"));
	r_test->SetToolTip(wxT("@r6@"));

	r_inne = new wxCheckBox(this, ID_R_INNE, wxT("Inne"), wxPoint(920, 175), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_inne"));
	r_inne->SetToolTip(wxT("@r7@"));

	r_unt = new wxCheckBox(this, ID_R_UNT, wxT("Uniemo¿liwia testy"), wxPoint(920, 195), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_unt"));
	r_unt->SetToolTip(wxT("@r8@"));

	WxButton3 = new wxButton(this, ID_WXBUTTON3, wxT(">>"), wxPoint(215, 610), wxSize(60, 21), 0, wxDefaultValidator, wxT("WxButton3"));

	k1l = new wxButton(this, ID_K1L, wxT("<<"), wxPoint(760, 610), wxSize(60, 20), 0, wxDefaultValidator, wxT("k1l"));

	k1p = new wxButton(this, ID_K1P, wxT(">>"), wxPoint(830, 610), wxSize(60, 20), 0, wxDefaultValidator, wxT("k1p"));

	k2l = new wxButton(this, ID_K2L, wxT("<<"), wxPoint(1030, 610), wxSize(60, 20), 0, wxDefaultValidator, wxT("k2l"));

	m1 = new wxTextCtrl(this, ID_M1, wxT(""), wxPoint(946, 408), wxSize(146, 22), 0, wxDefaultValidator, wxT("m1"));

	lm1 = new wxStaticText(this, ID_LM1, wxT("@M1@"), wxPoint(907, 411), wxDefaultSize, 0, wxT("lm1"));

	m2 = new wxTextCtrl(this, ID_M2, wxT(""), wxPoint(946, 432), wxSize(146, 22), 0, wxDefaultValidator, wxT("m2"));

	lm2 = new wxStaticText(this, ID_LM2, wxT("@M2@"), wxPoint(907, 435), wxDefaultSize, 0, wxT("lm2"));

	lm3 = new wxStaticText(this, ID_LM3, wxT("@M3@"), wxPoint(907, 459), wxDefaultSize, 0, wxT("lm3"));

	m3 = new wxTextCtrl(this, ID_M3, wxT(""), wxPoint(946, 456), wxSize(146, 22), 0, wxDefaultValidator, wxT("m3"));

	lm4 = new wxStaticText(this, ID_LM4, wxT("@M4@"), wxPoint(907, 483), wxDefaultSize, 0, wxT("lm4"));

	m4 = new wxTextCtrl(this, ID_M4, wxT(""), wxPoint(946, 480), wxSize(146, 22), 0, wxDefaultValidator, wxT("m4"));

	lm5 = new wxStaticText(this, ID_LM5, wxT("@M5@"), wxPoint(907, 507), wxDefaultSize, 0, wxT("lm5"));

	m5 = new wxTextCtrl(this, ID_M5, wxT(""), wxPoint(946, 504), wxSize(146, 22), 0, wxDefaultValidator, wxT("m5"));

	isview = new wxCheckBox(this, ID_WXCHECKBOX6, wxT("generowanie podgl¹du"), wxPoint(560, 575), wxSize(129, 17), 0, wxDefaultValidator, wxT("isview"));

	WxStaticText4 = new wxStaticText(this, ID_WXSTATICTEXT4, wxT("CR"), wxPoint(290, 60), wxDefaultSize, 0, wxT("WxStaticText4"));

	r_usterka_dkj = new wxCheckBox(this, ID_WXCHECKBOX61, wxT("Usterka (DKJ)"), wxPoint(920, 234), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_usterka_dkj"));
	r_usterka_dkj->SetToolTip(wxT("@ru@"));
	r_usterka_dkj->SetValue(true);

	r_critical_dkj = new wxCheckBox(this, ID_WXCHECKBOX71, wxT("B³¹d krytyczny (DKJ)"), wxPoint(920, 214), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_critical_dkj"));
	r_critical_dkj->SetToolTip(wxT("@rc@"));

	st_szablon = new wxStaticText(this, ID_WXSTATICTEXT8, wxT("Szablon"), wxPoint(8, 0), wxDefaultSize, 0, wxT("st_szablon"));

	wxBitmap Save_BITMAP (intaktDlg_Save_XPM);
	Save = new wxBitmapButton(this, ID_WXBITMAPBUTTON5, Save_BITMAP, wxPoint(735, 28), wxSize(35, 37), wxBU_AUTODRAW, wxDefaultValidator, wxT("Save"));
	Save->Show(false);

	wxBitmap usun_BITMAP (intaktDlg_usun_XPM);
	usun = new wxBitmapButton(this, ID_WXBITMAPBUTTON6, usun_BITMAP, wxPoint(694, 8), wxSize(35, 37), wxBU_AUTODRAW, wxDefaultValidator, wxT("usun"));
	usun->Show(false);

	wxBitmap btn_getusr_BITMAP (intaktDlg_btn_getusr_XPM);
	btn_getusr = new wxBitmapButton(this, ID_WXBITMAPBUTTON51, btn_getusr_BITMAP, wxPoint(54, 209), wxSize(37, 37), wxTRANSPARENT_WINDOW | wxNO_BORDER | wxBU_AUTODRAW | wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM, wxDefaultValidator, wxT("btn_getusr"));
	btn_getusr->SetToolTip(wxT("Wybór u¿ytkownika i skopiowanie has³a do bufora"));

	wxBitmap btn_getadm_BITMAP (intaktDlg_btn_getadm_XPM);
	btn_getadm = new wxBitmapButton(this, ID_WXBITMAPBUTTON52, btn_getadm_BITMAP, wxPoint(54, 255), wxSize(37, 37), wxTRANSPARENT_WINDOW | wxNO_BORDER | wxBU_AUTODRAW | wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM, wxDefaultValidator, wxT("btn_getadm"));
	btn_getadm->SetToolTip(wxT("Wybór komputera administracyjnego"));

	wxBitmap btn_getpos_BITMAP (intaktDlg_btn_getpos_XPM);
	btn_getpos = new wxBitmapButton(this, ID_BTN_GETPOS, btn_getpos_BITMAP, wxPoint(54, 299), wxSize(37, 37), wxTRANSPARENT_WINDOW | wxNO_BORDER | wxBU_AUTODRAW | wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM, wxDefaultValidator, wxT("btn_getpos"));
	btn_getpos->SetToolTip(wxT("Wybór POS-a sprzeda¿owego"));

	wxBitmap WxBitmapButton5_BITMAP (intaktDlg_WxBitmapButton5_XPM);
	WxBitmapButton5 = new wxBitmapButton(this, ID_WXBITMAPBUTTON53, WxBitmapButton5_BITMAP, wxPoint(7, 255), wxSize(37, 37), wxBU_AUTODRAW | wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM, wxDefaultValidator, wxT("WxBitmapButton5"));
	WxBitmapButton5->SetToolTip(wxT("Pobranie informacji o liczbie konwersji na bazie danych."));
	WxBitmapButton5->SetHelpText(wxT("Pobranie informacji o liczbie konwersji na bazie danych."));
	WxBitmapButton5->SetFont(wxFont(7, wxSWISS, wxNORMAL, wxNORMAL, false));

	wxBitmap WxBitmapButton6_BITMAP (intaktDlg_WxBitmapButton6_XPM);
	WxBitmapButton6 = new wxBitmapButton(this, ID_WXBITMAPBUTTON60, WxBitmapButton6_BITMAP, wxPoint(7, 299), wxSize(37, 37), wxBU_AUTODRAW | wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM, wxDefaultValidator, wxT("WxBitmapButton6"));
	WxBitmapButton6->SetToolTip(wxT("Uproszczona konfiguracja POS-ów"));
	WxBitmapButton6->SetHelpText(wxT("Uproszczona konfiguracja POS-ów"));

	cr_powiazane = new wxTextCtrl(this, ID_WXEDIT20, wxT(""), wxPoint(385, 505), wxSize(69, 25), 0, wxDefaultValidator, wxT("cr_powiazane"));
	cr_powiazane->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false));

	tcr_pow = new wxStaticText(this, ID_WXSTATICTEXT80, wxT("CR powi¹zane"), wxPoint(289, 508), wxDefaultSize, 0, wxT("tcr_pow"));
	tcr_pow->SetFont(wxFont(10, wxSWISS, wxNORMAL, wxNORMAL, false));

	WxStaticLine5 = new wxStaticLine(this, ID_WXSTATICLINE5, wxPoint(291, 540), wxSize(599, -1), wxLI_HORIZONTAL);

	r_zapytaj = new wxCheckBox(this, ID_WXCHECKBOX62, wxT("Zapytaj"), wxPoint(920, 260), wxSize(161, 16), 0, wxDefaultValidator, wxT("r_zapytaj"));
	r_zapytaj->SetToolTip(wxT("@ru@"));
	r_zapytaj->SetValue(true);

	WxStaticLine7 = new wxStaticLine(this, ID_WXSTATICLINE7, wxPoint(915, 253), wxSize(172, -1), wxLI_HORIZONTAL);

	about = new wxButton(this, ID_WXBUTTON4, wxT("O programie"), wxPoint(809, 8), wxSize(82, 19), 0, wxDefaultValidator, wxT("about"));

	wxBitmap ctx_help_BITMAP (intaktDlg_ctx_help_XPM);
	ctx_help = new wxBitmapButton(this, ID_WXBITMAPBUTTON11, ctx_help_BITMAP, wxPoint(694, 49), wxSize(37, 39), wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM, wxDefaultValidator, wxT("ctx_help"));
	ctx_help->SetToolTip(wxT("CrgGen\nF1 - pomoc kontekstowa"));
	ctx_help->Show(false);

	TreeRun = new wxToggleButton(this, ID_WXTOGGLEBUTTON1, wxT("Uruchamiaj"), wxPoint(208, 10), wxSize(69, 25), wxNO_FULL_REPAINT_ON_RESIZE, wxDefaultValidator, wxT("TreeRun"));
	TreeRun->SetForegroundColour(wxColour(0,0,0));

	WxStaticText8 = new wxStaticText(this, ID_WXSTATICTEXT85, wxT("Wersja pakietu"), wxPoint(8, 72), wxDefaultSize, 0, wxT("WxStaticText8"));

	aspell = new wxCheckBox(this, ID_WXASPEL, wxT("sprawdzaj pisowniê"), wxPoint(560, 595), wxSize(137, 17), 0, wxDefaultValidator, wxT("aspell"));
	aspell->SetValue(true);

	st_login = new wxStaticText(this, ID_WXSTLOGIN, wxT("Login"), wxPoint(97, 208), wxDefaultSize, 0, wxT("st_login"));

	st_haslo = new wxStaticText(this, ID_WXSTHASLO, wxT("Has³o"), wxPoint(96, 229), wxDefaultSize, 0, wxT("st_haslo"));

	ed_login = new wxTextCtrl(this, ID_EDLOGIN, wxT(""), wxPoint(129, 208), wxSize(145, 17), 0, wxDefaultValidator, wxT("ed_login"));

	ed_haslo = new wxTextCtrl(this, ID_EDHASLO, wxT(""), wxPoint(129, 226), wxSize(145, 17), 0, wxDefaultValidator, wxT("ed_haslo"));

	btn_cfg = new wxButton(this, ID_WXBUTTONCFG, wxT("Konfiguracja"), wxPoint(809, 31), wxSize(82, 19), 0, wxDefaultValidator, wxT("btn_cfg"));

	btRestore = new wxButton(this, ID_WXbtRestore, wxT("Odzyskaj kopiê"), wxPoint(132, 503), wxSize(113, 27), 0, wxDefaultValidator, wxT("btRestore"));

	wxBitmap btDeleteDatabase_BITMAP (intaktDlg_btDeleteDatabase_XPM);
	btDeleteDatabase = new wxBitmapButton(this, ID_WXbtDeleteDatabase, btDeleteDatabase_BITMAP, wxPoint(8, 209), wxSize(37, 37), wxBU_AUTODRAW, wxDefaultValidator, wxT("btDeleteDatabase"));
	btDeleteDatabase->SetToolTip(wxT("Usuniêcie bazy danych z serwera"));

	pg_CR = new wxNotebook(this, ID_PG_CR, wxPoint(289, 120), wxSize(603, 376), wxNB_DEFAULT);

	pg_01 = new wxPanel(pg_CR, ID_PG_01, wxPoint(4, 24), wxSize(595, 348));
	pg_CR->AddPage(pg_01, wxT("memo 01"));

	pg_02 = new wxPanel(pg_CR, ID_PG_02, wxPoint(4, 24), wxSize(595, 348));
	pg_CR->AddPage(pg_02, wxT("memo 02"));

	pg_03 = new wxPanel(pg_CR, WX_PG_03, wxPoint(4, 24), wxSize(595, 348));
	pg_CR->AddPage(pg_03, wxT("memo 03"));

	pg_04 = new wxPanel(pg_CR, ID_PG_04, wxPoint(4, 24), wxSize(595, 348));
	pg_CR->AddPage(pg_04, wxT("memo 04"));

	pg_05 = new wxPanel(pg_CR, ID_PG_05, wxPoint(4, 24), wxSize(595, 348));
	pg_CR->AddPage(pg_05, wxT("memo 05"));

	WxToolbarPanel = new wxPanel(this, ID_WXPANEL1, wxPoint(456, 500), wxSize(434, 38));

	WxToolbar1DBase = new wxPanel(this, ID_WxToolbar1DBase, wxPoint(907, 529), wxSize(42, 124));
	WxToolbar1DBase->Show(false);

	WxToolbar2DBase = new wxPanel(this, ID_WxToolbar2DBase, wxPoint(949, 529), wxSize(80, 124));

	WxStaticText6 = new wxStaticText(this, ID_WXSTATICTEXT6, wxT("CR#"), wxPoint(7, 533), wxSize(209, 132), wxRAISED_BORDER | wxTRANSPARENT_WINDOW | wxTAB_TRAVERSAL | wxNO_FULL_REPAINT_ON_RESIZE | wxNO_BORDER | wxST_NO_AUTORESIZE, wxT("WxStaticText6"));
	WxStaticText6->SetForegroundColour(wxColour(255,0,0));
	WxStaticText6->SetFont(wxFont(72, wxSWISS, wxNORMAL, wxBOLD, false));

	ZalLiczMD5 = new wxCheckBox(this, ID_ZalLiczMD5, wxT("oblicz MD5"), wxPoint(464, 575), wxSize(83, 17), 0, wxDefaultValidator, wxT("ZalLiczMD5"));

	WxFontDialog1 =  new wxFontDialog(this);

	WyborKatalogu =  new wxDirDialog(this, wxT("Choose a directory"), wxT(""));

	WxPopupMenu1 = new wxMenu(wxT(""));wxMenuItem * ID_MM1_mnuItem_obj = new wxMenuItem (WxPopupMenu1, ID_MM1, wxT("Makro 1"), wxT("@M1@ - makro definiowalne"), wxITEM_NORMAL);
	wxBitmap ID_MM1_mnuItem_obj_BMP(ID_MM1_XPM);
	ID_MM1_mnuItem_obj->SetBitmap(ID_MM1_mnuItem_obj_BMP);
	WxPopupMenu1->Append(ID_MM1_mnuItem_obj);
	WxPopupMenu1->Append(ID_MM2, wxT("Makro 2"), wxT("@M2@ - makro definiowalne"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_MM3, wxT("Makro 3"), wxT("@M3@ - makro definiowalne"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_MM4, wxT("Makro 4"), wxT("@M4@ - makro definiowalne"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_MM5, wxT("Makro 5"), wxT("@M5@ - makro definiowalne"), wxITEM_NORMAL);
	WxPopupMenu1->AppendSeparator();
	WxPopupMenu1->Append(ID_CONNP, wxT("Connection String Delphi"), wxT("@CONN@ - Connection String"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_CONN, wxT("Connection POS"), wxT("@CONNP@ - po³¹czenie do POS-a"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_S, wxT("Serwer bazy danych"), wxT("@S@ - serwer bazy danych"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_DD, wxT("Baza danych"), wxT("@D@ - baza danych"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_U, wxT("U¿ytkownik"), wxT("@U@ - u¿ytkownik serwera"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_P, wxT("Has³o do serwera"), wxT("@P@ - has³o do serwer bazy danych"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_DAPP, wxT("Katalog instalacji aplikacji"), wxT("@DAPP@ - katalog aplikacji"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_DARC, wxT("Katalog archiwum"), wxT("@DARC@ - katalog archiwum"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_VER, wxT("Wersja pliku zaszyta w Exe"), wxT("@VER@ - Wersja pliku"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_DT, wxT("Aktualna data"), wxT("@DT@ - aktualna data"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_DT1, wxT("Aktualna data + przesuniêcie w czasie"), wxT("@DT+y,m,t,d@ - aktualna data + przesuniêcie w czasie"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_TM, wxT("Aktualny czas"), wxT("@TM@ - aktualny czas"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_LN, wxT("Linia sk³adaj¹ca siê z <n> ci¹gów <s>"), wxT("@LN:n:s@ - linia sk³adaj¹ca siê z <n> ci¹gów <s>"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_L1, wxT("Linia 10 znaków '-'"), wxT("@L1@ - linia 10 znaków '-'"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_L2, wxT("Linia 10 znaków '='"), wxT("@L2@ - linia 10 znaków '='"), wxITEM_NORMAL);
	WxPopupMenu1->Append(ID_L3, wxT("Linia 10 znaków '.'"), wxT("@L3@ - linia 10 znaków '.'"), wxITEM_NORMAL);

	WxOpenFileDialog1 =  new wxFileDialog(this, wxT("Wybierz plik/i"), wxT(""), wxT(""), wxT("*.*"), wxOPEN | wxMULTIPLE | wxCHANGE_DIR);

	SetTitle(wxT("Change Request - CR"));
	SetIcon(Self_intaktDlg_XPM);
	SetSize(8,8,1104,677);
	Center();
	
    ////GUI Items Creation End
    wxBusyInfo wait2(wxT("£adowanie default-ów..."));

    //na potrzeby ³adowania listy baz z serwera, jeœli w combo u¿yliœmy %
    db_name->Connect(wxID_ANY ,wxEVT_LEFT_DOWN,wxMouseEventHandler( intaktDlg::dn_nameLKM ) , NULL, this);
     
    //£adowanie g³ównych katalogów wersji (FR-ek, RC)
    ver_ctrl = new wxComboCtrl(this,10300,wxEmptyString,wxPoint(83,67),wxSize(194,21),wxCB_READONLY);
    ver_ctrl->UseAltPopupWindow();

    ver_list= new wxListBoxPopup();
    ver_ctrl->SetPopupControl(ver_list);
    ver_list->Set(arrayStringFor_WxChoice4);	
	
    //£adowanie listy uruchamianych aplikacji
    app_ctrl = new wxComboCtrl(this,10200,wxEmptyString,wxPoint(83,43),wxSize(194,21),wxCB_READONLY);
    app_ctrl->UseAltPopupWindow();

    app_list= new wxListBoxPopup();
    app_ctrl->SetPopupControl(app_list);
    app_list->Set(arrayStringFor_WxChoice1);

    //£adowanie szablonów do drzewa kategorii                                                  //500,50
    ctrl_szablon = new wxGenericComboCtrl(this,4000 /*wxID_ANY*/ ,"<Wybierz szablon>", wxPoint(7,12), wxSize(200,22),wxCB_SORT|wxTE_PROCESS_ENTER|wxCB_READONLY);
    ctrl_szablon->UseAltPopupWindow();

    edn_szablon = new TreeCtrlComboPopup(); //ctrl_szablon);
    ctrl_szablon->SetPopupControl(edn_szablon);
    
    imageList = new wxImageList(16, 16);    
    imageList->Add(wxIcon(awxledin_xpm));
    imageList->Add(wxIcon(awxled_xpm));
    edn_szablon->AssignImageList(imageList);
    
    wxTreeItemId rootId = edn_szablon->AddRoot(wxT("<hidden_root>"));
    ctrl_szablon->SetButtonPosition(50, // button width
                                    -1, // button height
                                    wxLEFT, // side
                                    0 // horizontal spacing
                                    );
    
    //Drag&Drop
    db_name->SetDropTarget(new DnDText<wxComboBox>(db_name));
    ComputerName->SetDropTarget(new DnDText<wxComboBox>(ComputerName));
    srv_name->SetDropTarget(new DnDText<wxComboBox>(srv_name));
    //nr_cr->SetDropTarget(new DnDText<wxTextCtrl>(nr_cr));
    //r_usterka->SetDropTarget(new DnDText<wxCheckBox>(r_usterka));
    //lm1->SetDropTarget(new DnDText<wxStaticText>(lm1));

    //POMOC
    ctrl_szablon->SetHelpText(wxT("Szablon do wczytania.\n" \
                                  "W celu dodania\\modyfikacji\\usuniêcia nowego ustawienia rozwiñ listê strza³k¹ w dó³\n" \
                                  "nastêpnie kliknij na rozwiniêtej liœcie prawym klawiszem myszki."));


    app_ctrl->SetHelpText(wxT("Aplikacja do uruchomienia.\n" \
                              "W celu dodania\\modyfikacji\\usuniêcia nowego ustawienia rozwiñ listê strza³k¹ w dó³\n" \
                              "nastêpnie kliknij na rozwiniêtej liœcie prawym klawiszem myszki."));
/*                                
  	ed_szablon->SetHelpText(wxT("Szablon umo¿liwia zapisanie specyficznych ustawieñ wywo³añ programów. "\
                               "Zapisywanie i kasowanie odbywa siê online. "\
                               "Sortowanie nastêpuje przy nastêpnym uruchomieniu programu."));
*/    
    ComputerName->SetHelpText(wxT("Dla aplikacji Manager, Hotele, Catering podajemy tu nazwê komputera administracyjnego.\n"\
                                  "Dla POS-a podajemy tu nazwê lokalu, jeœli u¿yjemy makra %X wówczas pod makro\n"\
                                  "podstawi siê unikalny 4-cyfrowy (hex) numer lokalu"));
    PosHotel->SetHelpText(wxT("Dla aplikacji Hotele i Catering podajemy tu nazwê pos-a z tabeli posów.\n"\
                              "Dla POS-a gastronomicznego podajemy tu równie¿ nazwê pos-a z tabeli posób. Dodatkowo mo¿emy\n"\
                              "dla niego okreœliæ dodatkowe parametry uruchomieniowe: czy jest to master pos,\n" \
                              "czy te¿ okreœliæ ip komputera. Definiujemy ci¹g POS:no|yes:localhost|xxx.xxx.xxx.xxx\n"\
                              "Pierwszy leksem to nazwa POS-a, drugi to wartoœæ parametru \'master\', trzeci to wartoœæ\n" \
                              "parametru \'masterIP\'"));
    db_name->SetHelpText(wxT("Jeœli w polu wyboru pos³u¿ysz siê znakiem procentu (like), wówczas\n"\
                              "za³adujê bazy danych z wybranego serwera, wg podanego wzorca\n"\
                              "W przypadku du¿ej iloœci baz, proces ³adowania mo¿e zaj¹æ du¿o czasu."));
                                  
    WxPopupMenu1->SetHelpString(ID_MM2,wxT("Pomoc"));

	show_buffor=false;

    SetSize(190,35,288,670);
	Center();

    WxDatePickerCtrl1->SetValue(wxDateTime::Now());

    //domyœlny dŸwiêk   
    other_param["filesound"]=wxT("c:\\windows\\media\\notify.wav");
    
    WxEdit5->Enable();
    WxBitmapButton4->Enable();
    WxCheckBox1->SetValue(true);
    
    WxEdit5->SetValue(wxT(".\\positive\\version\\"));
    WyborKatalogu->SetPath(WxEdit5->GetValue());

    srv_name->SetSelection(0);

    int wybor=srv_name->GetSelection();
    wxStringTokenizer s(real_pass.Item(wybor), wxT(";"));
    s.GetNextToken();

    WxEdit6->SetValue(s.GetNextToken());
    WxEdit7->SetValue(s.GetNextToken());

    memo = new wxTextCtrl(this, ID_WXMEMO2, wxT(""), wxPoint(289,400), wxSize(600,100),wxTE_RICH2 | wxSTATIC_BORDER | wxTE_MULTILINE | wxHSCROLL /*|wxTE_BESTWRAP*/   , wxDefaultValidator, wxT("memo"));
    memo->SetWindowStyle(wxHSCROLL | wxVSCROLL|wxALWAYS_SHOW_SB );
    memo->SetDropTarget(new DnDText<wxTextCtrl>(memo));
    memo->SetMaxLength(0);
    memo->SetFocus();
    memo->SetInsertionPointEnd();
    memo->SetBackgroundColour(wxColour(255,235,236));
    memo->SetForegroundColour(wxColour(*wxBLACK));
    memo->SetFont(wxFont(12, wxSWISS, wxNORMAL,wxNORMAL, false, wxT("Courier")));
    memo->Show(false);
    memo->SetHelpText(wxT("Bufor systemu, teksty tu wpisane\n" \
                          "s¹ ³adowane do pliku bufora przy wyjœciu z programu\n"\
                          "i odczytywane przy ³adowaniu programu.\n"\
                          "Swoistego rodzaju notatnik"));

    TreeRun->SetHelpText(wxT("Ustawienie automatycznego uruchamiania.\nPo w³¹czeniu opcji i wyborze szablonu program zostanie\nautomatycznie uruchomiony."));
    TreeRun->SetToolTip(TreeRun->GetHelpText());
    
   	logger = new wxLogTextCtrl( memo );
    m_logOld = wxLog::SetActiveTarget( logger );
    wxLog::SetTimestamp( NULL );
    
    laduj_ze_schowka();
	memo->Enable(false);
    //w³¹czenie domyœlnych ustawieñ dla pola "Aplikacja"
    WxCheckBox5->SetValue(true);

    //wxMessageBox("£adowanie pozosta³ych opcji z sekcji [other]");  
    other_param["autohide"]="1";
    other_param["autologin"]="1";
    other_param["autospell"]="1";
    other_param["ccrrodzaj"]="000000000011";    
    arrayStringFor_WxChoice1.Clear();
    if (xladuj_bazy(arrayStringFor_WxChoice1,baza,section[SEC_OTHER]))
    {
      wxString st,sv;
      for (size_t q=0; q<arrayStringFor_WxChoice1.Count();q++)
      {
        wxStringTokenizer s(arrayStringFor_WxChoice1.Item(q), wxT("="));
        
        st=s.GetNextToken().Trim(false).Trim().Lower();         
        if (st[0]!=';') 
        {       
          sv=s.GetNextToken().Trim(false).Trim(); 
            other_param[st]=sv;    
            if        (st=="isview")    { isview->SetValue(sv[0]=='1');
            } else if (st=="issound")   { issound->SetValue(sv[0]=='1');
            } else if (st=="backup")    { WxEdit1->SetValue(sv);
            } else if (st=="rootdir")   { WxEdit5->SetValue(sv); WyborKatalogu->SetPath(sv);
            } else if (st=="@m1@")      { m1->SetValue(sv);
            } else if (st=="@m2@")      { m2->SetValue(sv);
            } else if (st=="@m3@")      { m3->SetValue(sv);
            } else if (st=="@m4@")      { m4->SetValue(sv);
            } else if (st=="@m5@")      { m5->SetValue(sv);
            } else if (st=="zalaczniki"){ WxEdit3->SetValue(sv);
            } ;
        }; 
      };
    };

    //³adowanie sekcji parametrów definiowalnych    
    arrayStringFor_WxChoice1.Clear();
    if (xladuj_bazy(arrayStringFor_WxChoice1,baza,section[SEC_MACRODEF]))
    {
      wxString st,sv;
      for (size_t q=0; q<arrayStringFor_WxChoice1.Count();q++)
      {
        wxStringTokenizer s(arrayStringFor_WxChoice1.Item(q), wxT("="));
       
        st=s.GetNextToken().Trim(false).Trim().Lower();         
        if (st[0]!=';') 
        {       
          sv=s.GetNextToken().Trim(false).Trim(); 
          other_param[st]=sv;
        }; 
      };
    };
        
    aspell->SetValue(other_param["autospell"][0]=='1');
    
    other_param["ccrrodzaj"].Pad(12,'0');
	r_critical     ->SetValue(other_param["ccrrodzaj"][ 0]=='1');
	r_usterka      ->SetValue(other_param["ccrrodzaj"][ 1]=='1');
	r_drobna_zmiana->SetValue(other_param["ccrrodzaj"][ 2]=='1');
	r_rozwojowe    ->SetValue(other_param["ccrrodzaj"][ 3]=='1');
	r_support      ->SetValue(other_param["ccrrodzaj"][ 4]=='1');
	r_migracja     ->SetValue(other_param["ccrrodzaj"][ 5]=='1');
	r_test         ->SetValue(other_param["ccrrodzaj"][ 6]=='1');
	r_inne         ->SetValue(other_param["ccrrodzaj"][ 7]=='1');
	r_unt          ->SetValue(other_param["ccrrodzaj"][ 8]=='1');
	r_critical_dkj ->SetValue(other_param["ccrrodzaj"][ 9]=='1');
	r_usterka_dkj  ->SetValue(other_param["ccrrodzaj"][10]=='1');
	r_zapytaj      ->SetValue(other_param["ccrrodzaj"][11]=='1');
	
    //za³adowanie szablonów
    LoadTree();

    //podpiêcie zdarzenia automatycznego logowania
    //zdarzenie jest odpalane przez w¹tek ThreadAutoRunApp 
    Connect(4000,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(intaktDlg::AutoRunApp) );

    //MyAspell
    pSpellChecker = new MySpellInterface(new MySpellingDialog(NULL));
    SetDefaultMySpellOptions((MySpellInterface*)pSpellChecker);  
    pSpellChecker->InitializeSpellCheckEngine();    


    //TOOLBAR dla MEMO
    #include "Images/db_connection.xpm"
	wxBitmap bmp_db_connection(db_connection_xpm);
	
    #include "Images/intaktDlg_bbCRText_XPM.xpm"
	wxBitmap bbCRText_BITMAP (intaktDlg_bbCRText_XPM);
	
    #include "Images/intaktDlg_bbWersja_XPM.xpm"
	wxBitmap bbWersja_BITMAP (intaktDlg_bbWersja_XPM);
	
    #include "Images/macro.xpm"
	wxBitmap bmp_macro(macro_xpm);
	
    #include "Images/macrom.xpm"
	wxBitmap bmp_macrom(macrom_xpm);
	
    #include "Images/intaktDlg_WxBitmapButton8_XPM.xpm"
	bku=wxBitmap(intaktDlg_WxBitmapButton8_XPM);
	
    #include "Images/intaktDlg_WxBitmapButton8d_XPM.xpm"
    bkd=wxBitmap(intaktDlg_WxBitmapButton8d_XPM_xpm); //kd);

	
    #include "Images/intaktDlg_WxBitmapButton9_XPM.xpm"
	wxBitmap WxBitmapButton9_BITMAP(intaktDlg_WxBitmapButton9_XPM);
	
    #include "Images/intaktDlg_WxBitmapButton10_XPM.xpm"
	wxBitmap WxBitmapButton10_BITMAP(intaktDlg_WxBitmapButton10_XPM);

    #include "Images/pisownia.xpm"
	wxBitmap pisownia(pisownia_xpm);

    #include "Images/ctx_help.xpm"
	wxBitmap ctx_help(ctx_help_xpm);

    #include "Images/run.xpm"
	wxBitmap sqlrun(run_xpm);

    tb = new wxToolBar(WxToolbarPanel, 20000, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxNO_BORDER |wxTB_NODIVIDER); //  wxTB_NOALIGN
    tb->AddTool(TBM_EXECSQL,wxT("Skrypt"),sqlrun,wxT("Uruchomienie treœci jako skryptu"));
    tb->AddTool(TBM_HELP,wxT("Pomoc"),ctx_help,wxT("Pomoc kontekstowa"));
    tb->AddTool(TBM_ASPELL,wxT("Pisownia"),pisownia,wxT("Sprawdzenie pisowni"));
    tb->AddTool(TBM_NEWCR,wxT("Czyœæ"),WxBitmapButton10_BITMAP,wxT("Wyczyszczenie treœci tematu i CR - nowe zg³oszenie"));
    tb->AddSeparator();
    tb->AddTool(TBM_PASTE,wxT("Wklej"),WxBitmapButton9_BITMAP,wxT("Wklejenie do treœci zawartoœci ze schowka"));
    tb->AddTool(TBM_USECR,wxT("Odbiór"),bbWersja_BITMAP,wxT("Odbiór CR"));
    tb->AddTool(TBM_SHOW ,wxT("Podgl¹d"),bbCRText_BITMAP,wxT("Wygenerowanie podgl¹du treœci"));
    tb->AddTool(TBM_CONN ,wxT("Connection"),bmp_db_connection,wxT("Wygenerowanie jedynie Connection String\r\ni wrzucenie do schowka systemowego"));
    tb->AddTool(TBM_MACRO,wxT("Makra @"),bmp_macro,wxT("Lista makr które mog¹ byæ u¿yte w treœci CR"));
    tb->AddTool(TBM_MACROUSR,wxT("Makra %"),bmp_macrom,wxT("Lista makr %"));
    tb->AddTool(TBM_BUFF ,wxT("Bufor"),bku,wxT("Uwidocznienie/ukrycie bufora pomocniczego\ndo przechowywania dodatkowych treœci - notatnik"));
    tb->Realize();
    
    LoadAppDefault();
    
    pg[0]=pg_01;
    pg[1]=pg_02;

        //za³adowanie  zak³adek
    LoadAllPages();
}

void intaktDlg::OnClose(wxCloseEvent& /*event*/)
{

  pSpellChecker->UninitializeSpellCheckEngine();
  wxDELETE(pSpellChecker);
  
  //WxMemo1->DoSaveFile("intakt.buf",0);
  try
  {
    SaveAllPages();
  dbg("OnClose 1");
    buffer buf(working_dir+"\\intakt.buf");
    if (buf.open(true))
        buf.save((WxEdit2->GetValue()!="" ? WxEdit2->GetValue()+"\r\n---\r\n" : "")+WxMemo1->GetValue());
    buf.close();
dbg("OnClose 2");
    buf.setfile(working_dir+"\\intakt.buf2");
    if (buf.open(true))
        buf.save(memo->GetValue());
    buf.close();
dbg("OnClose 3");
  } catch(...) {
    wxMessageBox(_T("B³¹d zrzutu do bufora"),_T("B³¹d"));
  };
   dbg("OnClose 4");
  if (m_taskBarIcon!=NULL)
     { delete m_taskBarIcon; m_taskBarIcon=0;};
    dbg("OnClose 5"); 
  //wxDELETE(ctrl_szablon);
  //wxDELETE(edn_szablon);
  
   dbg("OnClose 6");  
  Destroy();
    dbg("OnClose 7");
}

void intaktDlg::WxCheckBox1Click(wxCommandEvent& event)
{
    bool wlacz = ext_zalaczniki->IsChecked();

    WxEdit3->Enable(wlacz);
    WxBitmapButton1->Enable(wlacz);
    ZalLiczMD5->Enable(wlacz);
}

void intaktDlg::WxBitmapButton2Click(wxCommandEvent& event)
{
        WxOpenFileDialog1->SetWildcard("BAK backup|*.bak|*.bac|*.bac|ZIP upakowane|*.zip|*.7z|*.7z|*.rar|*.rar");
        WxOpenFileDialog1->SetPath(WxEdit1->GetValue());
        //WxOpenFileDialog1->ShowModal();
        if (WxOpenFileDialog1->ShowModal()==wxID_OK)
            WxEdit1->SetValue(WxOpenFileDialog1->GetPath());
}

void intaktDlg::WxCheckBox2Click(wxCommandEvent& event)
{
    bool wlacz = WxCheckBox2->IsChecked();
    WxEdit1->Enable(wlacz);
    WxBitmapButton2->Enable(wlacz);
}

void intaktDlg::WxButton1Click(wxCommandEvent& event)
{
    wxFontData xFont;

    WxFontDialog1->ShowModal();
       xFont = WxFontDialog1->GetFontData();
        WxMemo1->SetFont(xFont.GetChosenFont());
        WxMemo1->SetForegroundColour(xFont.GetColour());
}

void intaktDlg::WxBitmapButton1Click(wxCommandEvent& event)
{
   // insert your code here
   WxOpenFileDialog1->SetWildcard("Wszystkie|*.*|Video|*.avi|PNG obrazki|*.png|JPG obrazki|*.jpg|PDF wydruki|*.pdf");
   WxOpenFileDialog1->SetPath(WxEdit3->GetValue());
   if (WxOpenFileDialog1->ShowModal()==wxID_OK )
       WxEdit3->SetValue(WxOpenFileDialog1->GetPath());
}

void intaktDlg::WxBitmapButton4Click(wxCommandEvent& event)
{
        // insert your code here
    if (WyborKatalogu->ShowModal()==wxID_OK)
    {
//       WyborKatalogu->GetPath()
     WxEdit5->SetValue(WyborKatalogu->GetPath());
    };
}

void intaktDlg::generujClick(wxCommandEvent& event)
{
    wxFileName tstfile;
    wxDateTime czas;
    wxString buf="";
    wxString sep=", ";
    wxString tmp;
  
    if (( mask & BIT_CONNECTION ) && db_name->GetValue()!="")
    {
        if (app_ctrl->GetValue().Contains("POS"))
        {
          buf+=GetConnectionPositivePOS();
        } else {
          buf+="--- Use Connection String ---\n";
          buf+=GetConnectionString();
        };
    };
    if (( mask & BIT_BACKUP ) && WxCheckBox2->IsChecked())
    {
        if (!(buf=="")) buf+="\n\n";
        buf+="--- Kopia bazy danych ---\n";
        buf+=WxEdit1->GetValue();
    }
    if (( mask & BIT_VERSION ) && WxCheckBox5->IsChecked())
    {
        if (!(buf=="")) 
            buf+="\n\n";

        if (event.GetString().Contains("V_"))
           buf+="--- Odebrane na poni¿szej wersji ---\n\n";
        else if (event.GetString().Contains("VC"))
           buf+="--- Cofniêta na poni¿szej wersji ---\n\n";
        else if (event.GetString().Contains("VB"))
           buf+="--- Cofniêta z b³êdem DPO na poni¿szej wersji ---\n\n";
        else if (event.GetString().Contains("VK"))
           buf+="--- Konwersja na poni¿szej wersji ---\n\n";
        else
           buf+="--- Informacja o wersji ---\n";

 
        tstfile.SetPath(MakeMacro(real_app[app_list->GetSelection()]));        
        buf+=wxString::Format("Modu³: %s\nWersja: %s (%s)",
             app_ctrl->GetValue().c_str(),
             ver_ctrl->GetValue().c_str(),
             GetInfo((char*)tstfile.GetFullPath().c_str(),"ProductVersion"));
             
        if (WxCheckBox4->IsChecked())
        {
            buf+="\nZ dnia: "+(WxDatePickerCtrl1->GetValue()).FormatISODate();
        }
        else if (WxCheckBox1->IsChecked())  //jeœli wybrano katalog aplikacji wówczas data i czas zostanie pobrany z pliku
        {
            tstfile.SetPath(MakeMacro(real_app[app_list->GetSelection()]));
            if (wxFile::Exists(tstfile.GetFullPath()))
            {

              czas=tstfile.GetModificationTime();
              buf+="\nModyfikacja exe: "+czas.FormatISODate()+", "+czas.FormatISOTime();
              if (version->IsChecked())
              {
                  wxVersionDLL v;
                  buf+= wxString::Format(" (%s)",v.GetFileVersion(tstfile.GetFullPath()).c_str());
              };
            } else {
              buf+="\n"+tstfile.GetFullPath();
            };
        };
    };

    if ( mask & BIT_TITLE )
    {
      //tytu³
      if (!(buf=="")) buf+="\n\n";

      //dopisanie do tytu³u statusu
      unsigned int nlen = buf.Length();
      if (cr_stat_prop->IsChecked()) buf+="#PZM";
      if (cr_stat_uzg->IsChecked()) buf+="#UZG";
      if (cr_stat_quest->IsChecked()) buf+="#ASK";
      if (cr_stat_propozycja->IsChecked()) buf+="#PR";
      if (nlen != buf.Length()) buf+=": ";
       
      //dodanie tytu³u
      if (aspell->IsChecked()) {
          wxLogNull logNo;
          wxString strCorrectedText1 = pSpellChecker->CheckSpelling(WxEdit2->GetValue());
          WxEdit2->SetValue(strCorrectedText1);
      };
      buf+=WxEdit2->GetValue();
      buf+="\n";
      for (unsigned int q=0;q<57;q++)
      {
          buf+="_";
      }
    };

    if ( mask & BIT_CR )
    {
      if (aspell->IsChecked()) {
          wxString strCorrectedText2 = pSpellChecker->CheckSpelling(WxMemo1->GetValue());
          WxMemo1->SetValue(strCorrectedText2);
      };
      buf+="\n"+WxMemo1->GetValue();
      buf=MakeMacro(buf);
      buf=MakeMacro(buf);      
    };


    if ((mask & BIT_CRPOWIAZANE) && cr_powiazane->GetValue().Length()>0)
    {
        //cr_powiazane->GetValue().
        buf+="\n\nCR powi¹zane: "+MakeMacro(cr_powiazane->GetValue().Trim());
    };
       
    unsigned long int nrodzaj=0;
    nrodzaj  = ( r_critical->IsChecked()       ? 1 : 0);
    nrodzaj |= (( r_usterka->IsChecked()       ? 1 : 0)<<1);
    nrodzaj |= (( r_drobna_zmiana->IsChecked() ? 1 : 0)<<2);
    nrodzaj |= (( r_inne->IsChecked()          ? 1 : 0)<<3);
    nrodzaj |= (( r_test->IsChecked()          ? 1 : 0)<<4);
    nrodzaj |= (( r_migracja->IsChecked()      ? 1 : 0)<<5);
    nrodzaj |= (( r_rozwojowe->IsChecked()     ? 1 : 0)<<6);
    nrodzaj |= (( r_support->IsChecked()       ? 1 : 0)<<7);
    nrodzaj |= (( r_unt->IsChecked()           ? 1 : 0)<<8);
    nrodzaj |= (( r_critical_dkj->IsChecked()  ? 1 : 0)<<9);
    nrodzaj |= (( r_usterka_dkj->IsChecked()   ? 1 : 0)<<10);



    //wxMessageBox(wxString::Format("maska bitowa rodzaju CR %i",(unsigned int)nrodzaj));
    //debug(wxString::Format("maska bitowa rodzaju CR %i",(unsigned int)nrodzaj));
    //wxMessageBox( event.GetString() );
    if ( r_critical->IsChecked() ||
         r_usterka->IsChecked() || 
         r_drobna_zmiana->IsChecked() || 
         r_inne->IsChecked() ||
         r_test->IsChecked() || 
         r_migracja->IsChecked() || 
         r_rozwojowe->IsChecked() || 
         r_support->IsChecked() ||
         r_unt->IsChecked() ||
         r_critical_dkj->IsChecked() ||
         r_usterka_dkj->IsChecked() ||
         r_zapytaj->IsChecked() )
        {
        mask |= BIT_RODZAJ ;
    } else {            // mask ~= (~mask | BIT_RODZAJ);
      mask = ~mask;
      mask |= BIT_RODZAJ;
      mask = ~mask;
    };
                             //zaremowane bo wskakiwa³o do wersji
    if ( !(event.GetString().Contains("V")) && ((mask & BIT_RODZAJ) || bedzie_zalacznik->IsChecked() || WxCheckBox3->IsChecked()))
    {
       buf+="\n\n--";
    };

    //dodanie rodzaju zg³oszenia
    if (!(event.GetString().Contains("V")) && (mask & BIT_RODZAJ))
     {
        if (r_zapytaj->IsChecked())
            r_zapytajClick(event);
            
        buf+="\n";
        buf+="Zg³oszono jako:";
        tmp="";
        if (r_critical->IsChecked())      tmp += sep+r_critical->GetLabelText();
        if (r_usterka->IsChecked())       tmp += sep+r_usterka->GetLabelText();
        if (r_drobna_zmiana->IsChecked()) tmp += sep+r_drobna_zmiana->GetLabelText();
        if (r_rozwojowe->IsChecked())     tmp += sep+r_rozwojowe->GetLabelText();
        if (r_support->IsChecked())       tmp += sep+r_support->GetLabelText();
        if (r_migracja->IsChecked())      tmp += sep+r_migracja->GetLabelText();
        if (r_test->IsChecked())          tmp += sep+r_test->GetLabelText();
        if (r_inne->IsChecked())          tmp += sep+r_inne->GetLabelText();
        if (r_unt->IsChecked())           tmp += sep+r_unt->GetLabelText();
        if (r_critical_dkj->IsChecked())  tmp += sep+r_critical_dkj->GetLabelText();
        if (r_usterka_dkj->IsChecked())   tmp += sep+r_usterka_dkj->GetLabelText();

     buf+=tmp.Mid(1);
     }
      

    //dodatkowe za³¹czniki       
    if (mask & BIT_ZALACZNIK)
    {
      bool zal=(bedzie_zalacznik->IsChecked() | WxCheckBox3->IsChecked());
      if (zal)
      {
          //buf+="\n\n--";
          if (WxCheckBox3->IsChecked()) {
              zal=(bool)(wxMessageBox(_T("Czy bêdziesz za³¹cza³ za³¹czniki?"),_T("Odpowiedz"),wxYES_NO)==wxYES);
          } else {
          }
          if (zal)
             buf+="\nDodatkowe informacje w za³¹cznikach.";
      };
      
      if (ext_zalaczniki->IsChecked())
      {
          buf+="\n\n--- Informacje dodatkowe ---\n";  //ta fraza mo¿e byæ wrzucona jako makro dodatkowe
          if (!(WxEdit3->IsEmpty()))
          {
                wxArrayString paths;
                WxOpenFileDialog1->GetPaths(paths);
                for (size_t I=0; I<paths.Count();I++)
                {
                  buf+=paths[I];
                  if (ZalLiczMD5->IsChecked() && wxFile::Exists(paths[I]))
                  {
                   wxString summd5 = static_cast<wxString>(md5file(paths[I].c_str()));
                   buf+="  MD5("+summd5+")";
                  };
                  buf += "\n";
                };
          };
          //wy³¹czenie za³¹czników dodatkowych
          ext_zalaczniki->SetValue(!ext_zalaczniki->IsChecked());
          WxCheckBox1Click(event);
      }
      
    };

    wklejkaDlg *win_intakt = new wklejkaDlg(this);
    win_intakt->memo_wklejka->AppendText(buf);

    if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(win_intakt->memo_wklejka->GetValue()));
      wxTheClipboard->Close();
    };

    if (issound->IsChecked())
       wxSound(other_param["filesound"]).Play(wxSOUND_ASYNC);

    if (isview->IsChecked()) //mask & BIT_SHOWWIN)
    {
      win_intakt->ShowModal();
    };
    
    if (win_intakt!=NULL)
       delete win_intakt;
           
    //na razie wy³¹czam obs³ugê bazy danych
    if (nr_cr->GetValue().Length()>0)
        dbSave(0);
}

bool intaktDlg::laduj_bazy(wxArrayString& arr, wxString& b1)
{

  wxTextFile file(b1);
  if (!file.Exists ())
  {
    file.Create(b1);
    return (FALSE);
  };

  if (file.Open ())
  {
    wxString s;
    if (file.Eof())
       return (FALSE);

    for ( s = file.GetFirstLine(); !file.Eof(); s = file.GetNextLine() )
    {
      arr.Add(wxT(s));
    }
   }
 return (TRUE);
}

void intaktDlg::WxCheckBox3Click(wxCommandEvent& event)
{
    wxMessageBox("has³o: "+WxEdit7->GetValue(),"Zawartoœæ pola has³o");
}

void intaktDlg::WxHyperLinkCtrl1HyperLink(wxHyperlinkEvent& event)
{
  wxString db="intakt.ini";
  wxTextFile file(db);

  ::wxSetWorkingDirectory(working_dir);

  if (!file.Exists())
  {
    file.Create(db);
  };
  if (file.Exists ())
  {
      ControlApp *kontrola_uruchomienia;
      kontrola_uruchomienia = new ControlApp(this,"Zmiany w bazie zostan¹ uwzglêdnione przy kolejnym uruchomieniu programu");
      ::wxExecute("notepad.exe "+(wxString)file.GetName(),wxEXEC_ASYNC,kontrola_uruchomienia);
  };
}

/*
 * WxCheckBox4Click
 */
void intaktDlg::WxCheckBox4Click(wxCommandEvent& event)
{
    WxDatePickerCtrl1->Enable(WxCheckBox4->IsChecked());
}

/*
 * WxCheckBox5Click
 */
void intaktDlg::WxCheckBox5Click(wxCommandEvent& event)
{
    LoadAppDefault();
}

void intaktDlg::LoadAppDefault(void)
{
    bool lwlacz =  WxCheckBox5->IsChecked();
    if (lwlacz)
    {
        ver_list->SetSelection(0);
    }
    app_list->Enable(lwlacz);
    ver_list->Enable(lwlacz);
    if (WxCheckBox4->IsChecked()) {
        WxDatePickerCtrl1->Enable(lwlacz);
    }
        WxCheckBox4->Enable(lwlacz);
};

/*
 * WxButton2Click
 */
void intaktDlg::WxButton2Click(wxHyperlinkEvent& event) // wxCommandEvent& event)
{
    wxMessageBox("Wprowadzone has³o\n\n>"+WxEdit7->GetValue()+"<","Zwartoœæ pola has³o");
}

void intaktDlg::laduj_ze_schowka()
{
  buffer buf(working_dir+"\\intakt.buf2");
  if (buf.open())
  {
    memo->AppendText(buf.get());
    buf.close();
   };
   buf.close(); //tak dla bezpieczeñstwa

  buf.setfile(working_dir+"\\intakt.buf");
  if (buf.open())
  {
    //***WxMemo1->AppendText(buf.get());
    buf.close();
    return;
   };
   buf.close(); //tak dla bezpieczeñstwa

  if (wxTheClipboard->Open())
  {
    wxTextDataObject data;
    wxTheClipboard->GetData(data);
    WxMemo1->AppendText(data.GetText());
    wxTheClipboard->Close();
  }
}
/*
 * schowekClick
 */
void intaktDlg::schowekClick(wxCommandEvent& event)
{
    laduj_ze_schowka();
}

void intaktDlg::cr_new(wxCommandEvent& event)
{
   //wyczyszczenie page
    pg_title[pg_CR->GetSelection()]=wxEmptyString;
    pg_source[pg_CR->GetSelection()]=wxEmptyString;
    
    WxMemo1->Clear();
    WxEdit2->Clear();
    cr_powiazane->Clear();
    cr_stat_uzg->SetValue(false);
    cr_stat_prop->SetValue(false);
    cr_stat_quest->SetValue(false);
    WxEdit2->SetFocus();
};

bool intaktDlg::xladuj_bazy(wxArrayString& arr, wxString& b1, wxString& sec)
{
wxTextFile file(b1);
if (!file.Exists ())
{
    file.Create(b1);
    if (file.Exists())
    {
    file.AddLine("; Copyright(c) Piotr Kuliñski, 2008-2009. All rights reserved.\r\n",wxTextFileType_Dos);
    for (unsigned int q=0; q<SEC_MAX; q++) {
        file.AddLine("["+section[q]+"]",wxTextFileType_Dos);
        switch (q)
        {
            case SEC_APPLICATION: file.AddLine("Hotele=@DAPP@hotele\\hotele.exe\r\n" \
                                               "Manager=@DAPP@manager\\manager.exe\r\n" \
                                               "POS=@DAPP@pos\\pos.exe\r\n" \
                                               "SetupSql=Skrypty SQL\\SetupSql.exe"); break;
            case SEC_DBASE:       file.AddLine("positive\r\n"); break;
            case SEC_VERSION:     file.AddLine("2006.3.fr11\r\n2008.1\r\n"); break;
            case SEC_SERVER:      file.AddLine("test-dkj,1435;sa;nnn\r\n" \
                                               "test-dkj,1434;sa;nnn\r\n" ); break;
            case SEC_BACKUP:      file.AddLine("\\\\kosz\\smieci\\Kulinski Piotr\\bazy\\\r\n"); break;
            case SEC_SOUND:       file.AddLine("c:\\windows\\media\\notify.waw"); break;
            case SEC_APP:         file.AddLine("C:\\positive\\version\\2006.3\\RC\\13"); break;
            default: break;
        };
    };
    file.Write(wxTextFileType_Dos);
    file.Close();
    wxMessageBox("Plik bazy danych utworzony z domyœlnymi wartoœciami");
    }
    else
    {
        wxMessageBox("Nie mo¿na utworzyæ pliku \""+b1+"\" w katalogu Ÿród³owym programu.\n" \
                 "Byæ mo¿e nie masz praw zapisu do tego katalogu.\n"\
                 "W³¹cz odpowiednie uprawnienia, b¹dŸ wybierz inny katalog instalacyjny.",\
                 "B³¹d uruchomienia programu");
        exit(0);
    };
//    return (FALSE);
  };

  if (file.Open())
  {
    wxString s,sp;
    if (file.Eof())
       return (FALSE);

    sec=sec.Lower();
    s=file.GetFirstLine();
    while (!file.Eof())
    {
      if (s=="["+sec+"]")
      {
        sp=file.GetNextLine();
        while (!file.Eof())
        {
         sp=sp.Trim();
         if (sp[0] != '[')
           {
             if (sp[0] != ';' && sp!="")
             {
                 //wxMessageBox(s+":"+sp);
                 arr.Add(wxT(sp));
             };
             sp=file.GetNextLine();
             sp=sp.Trim();
           }
         else
           {
            return (TRUE);
           }
        }
      } else
        s=file.GetNextLine();
    }
   }
 return (TRUE);
};

void intaktDlg::helpF1(wxKeyEvent& event)
{
    if (event.GetKeyCode()==WXK_F1)
        wxMessageBox("Press");
}

void intaktDlg::WxCheckBox1Click1(wxCommandEvent& event)
{
   bool wlacz = WxCheckBox1->IsChecked();

   WxEdit5->Enable(wlacz);
   WxBitmapButton4->Enable(wlacz);
   version->Enable(wlacz);
}

// Stworzenie backupu bazy danych, z wykorzystaniem osql
wxString intaktDlg::backup_database(bool kierunek)
{
    
MyProcess *kontrola_backup;

wxString cbackup="";
wxString b1("bk.bat");
wxDateTime czas = wxDateTime::Now();
wxTextFile file(b1);
wxString frmTime;

if (kierunek) //restor
{
frmTime=WxEdit1->GetValue();
frmTime=frmTime.Mid(0,frmTime.Last(wxChar('\\'))+1);

cbackup=frmTime+db_name->GetValue();
frmTime=czas.FormatISODate();
frmTime.Replace("-","");
cbackup+="#"+frmTime;

frmTime=czas.FormatISOTime();
frmTime.Replace(":","");
cbackup+="_"+frmTime+".bak";
} else {
  cbackup=WxEdit1->GetValue();
};

//wxMessageBox(cbackup);

    file.Create(b1);
    if (file.Exists())
    {
     if (!show_buffor)
     {
       wxCommandEvent e;
       memo_sizeClick(e);
     };
       memo->AppendText(wxT("\n--- start ---"));
       if (kierunek) {
        file.AddLine("@echo off\r\n"\
                     "echo Czekaj, wykonuje kopie bazy danych...\r\n" \
                     "osql -U "+WxEdit6->GetValue()+\
                         " -P "+WxEdit7->GetValue()+\
                         " -S "+srv_name->GetValue()+\
                         " -Q \"backup database ["+db_name->GetValue()+"] to disk=N'"+cbackup+"' with format, stats=1", \
                     wxTextFileType_Dos);
        } else {
            frmTime="restore database ["+db_name->GetValue()+"] "\
                    "from disk=N'"+cbackup+"' with replace,"\
                    "stats=1";
            wxArrayString logicalfile;
            bool IsPath=false;
            if (GetLogicalName("RESTORE FILELISTONLY from disk= N'"+cbackup+"'",logicalfile)>0) 
              {
               memo->AppendText("\n");
               memo->AppendText("Database: "+db_name->GetValue()+"\n");
               memo->AppendText("Backup: "+cbackup+"\n");
               memo->AppendText("LFile: "+logicalfile[0]+" -> pk_"+db_name->GetValue()+"\n");
               memo->AppendText("LLog: "+logicalfile[2]+" -> pk_"+db_name->GetValue()+"_log\n");
                              
               wxArrayString ains,insname,apath,alog;
               if (GetLogicalName("select 'default',COALESCE(convert(nvarchar(20), serverproperty('InstanceName')), 'MSSQLSERVER')",ains)>0) //pobranie aktualnej instancji, jesli nie ma intancji domyslnie jest MSSQLSERVER
                if (GetLogicalName("exec master.dbo.xp_regread N'HKEY_LOCAL_MACHINE', N'Software\\Microsoft\\Microsoft SQL Server\\Instance Names\\SQL', '"+ains[1]+"'",insname)>0) //wyszukanie w rejestrze wybrnej instancji - do nie jest przypisany odpowiedni podklucz
                  if (GetLogicalName("master.dbo.xp_regread N'HKEY_LOCAL_MACHINE', 'Software\\Microsoft\\Microsoft SQL Server\\"+insname[1]+"\\MSSQLServer', 'DefaultData'",apath)>0) //na podstawie podklucza pobranie ustawieñ domyœlnych dla œciezki restoru
                  {
                      GetLogicalName("master.dbo.xp_regread N'HKEY_LOCAL_MACHINE', 'Software\\Microsoft\\Microsoft SQL Server\\"+insname[1]+"\\MSSQLServer', 'DefaultLog'",alog);
                      IsPath=true;
                      memo->AppendText("RFile:  "+logicalfile[1]+" -> "+(IsPath ? apath[1] : "D:" )+"\\pk_"+db_name->GetValue()+".mdf\n");
                      memo->AppendText("RLog:   "+logicalfile[3]+" -> "+(IsPath ? alog[1]  : "D:" )+"\\pk_"+db_name->GetValue()+"_log.ldf\n");
                  };
               frmTime+=","\
                        "move N'"+logicalfile[0]+"' TO '"+(IsPath ? apath[1] : "D:" )+"\\pk_"+db_name->GetValue()+".mdf',"\
                        "move N'"+logicalfile[2]+"' TO '"+(IsPath ? alog[1]  : "D:" )+"\\pk_"+db_name->GetValue()+"_log.ldf';"\
                        "alter database ["+db_name->GetValue()+"] modify file (NAME= N'"+logicalfile[0]+"', NEWNAME= N'pk_"+db_name->GetValue()+"');"\
                        "alter database ["+db_name->GetValue()+"] modify file (NAME= N'"+logicalfile[2]+"', NEWNAME= N'pk_"+db_name->GetValue()+"_log');";
            };
            
            file.AddLine("@echo off\r\n"\
                         "echo Czekaj, odtwarzam baze danych...\r\n"\
                         "@echo on\r\n"\
                         "osql -U "+WxEdit6->GetValue()+\
                             " -P "+WxEdit7->GetValue()+\
                             " -S "+srv_name->GetValue()+\
                             " -Q \""+frmTime+"\"",\
                         wxTextFileType_Dos);
        };
    file.Write(wxTextFileType_Dos);
    file.Close();
    
    kontrola_backup = new MyProcess(this,file.GetName(),cbackup);
    ctrl_cmd=kontrola_backup;
    memo->AppendText(wxT("\n\n"));
    
    ::wxExecute(file.GetName(),wxEXEC_ASYNC,kontrola_backup);
    
    WxEdit1->SetValue(cbackup);
//    Iconize(true);
//    wxSleep(1);
//    Iconize(false);
    };
return cbackup;
};

void intaktDlg::dbcopyClick(wxCommandEvent& event)
{
  backup_database(true);
}
void intaktDlg::btRestoreClick(wxCommandEvent& event)
{
  if (::wxFileExists(WxEdit1->GetValue()))
     backup_database(false);
  else
     wxMessageBox(wxT("Wskazana kopia bazy danych nie istnieje"),wxT("Brak kopii bazy"));  
}
void intaktDlg::WxCheckBox3Click1(wxCommandEvent& event)
{
    bool zal=WxCheckBox3->IsChecked();
    bedzie_zalacznik->SetValue(!zal);
}

/*
 * bedzie_zalacznikClick
 */
void intaktDlg::bedzie_zalacznikClick(wxCommandEvent& event)
{
    bool zal=bedzie_zalacznik->IsChecked();
    if (zal)
       WxCheckBox3->SetValue(!zal);
}

/*
 * srv_nameSelected
 */
void intaktDlg::srv_nameSelected(wxCommandEvent& event )
{
    int wybor=srv_name->GetSelection();
    wxStringTokenizer s(real_pass.Item(wybor), wxT(";"));
    s.GetNextToken();
    WxEdit6->SetValue(s.GetNextToken());
    WxEdit7->SetValue(s.GetNextToken());
}

void intaktDlg::run_manager(wxCommandEvent& event)
{
    wxFileName tstfile;
    wxString cat;
    wxString tmp;
    unsigned int napp=0;
    int ntmp;
    wxString mmakefile="Plik konfiguracyjny wygenerowany automatem przez program CRGen.";
    
    cat=MakeMacro(real_app[app_list->GetSelection()]);
    
    wxStringTokenizer s(cat, wxT("|"));                                                                            
    cat=s.GetNextToken().Trim(true);
    
    tstfile.SetPath(cat);

    //nie sprawdzam istnienia pliku, poniewa¿ jeœli s¹ parametry uruchomieniowe to nie umie sprawdziæ,)
    if (1)
    {
    cat = cat.Lower();
    if      (cat.First("hotele.exe")>0)                    napp=100;
    else if (cat.First("manager.exe")>0)                   napp=1000;
    else if (cat.First("pos.exe")>0)                       napp=200;
    else if (cat.First("pos.jar")>0)                       napp=201;
    else if (cat.First("catering.exe")>0)                  napp=300;
    else if (cat.First("centrala.exe")>0)                  napp=400;
    else if (cat.First("setupsql.exe")>0)                  napp=500;
    else if (cat.First("lsiphoneservice.exe")>0)           napp=600;
    else if (cat.First("kds.exe")>0)                       napp=700;
    else if (cat.First("pagerservice.exe")>0)              napp=800;
    else if (cat.First("_install-service-telexper.bat")>0) napp=900;
    else if (cat.First("hospitalityassistant.cab")>0)      napp=1100;
    else if (cat.First("miniposwindowsmobile.cab")>0)      napp=1200;
    else if (cat.First("crm.exe")>0)                       napp=1400;

    cat=cat.Mid(0,cat.Last(wxChar('\\')));
    debug("1 = "+cat);
    ::wxSetWorkingDirectory(cat);

    switch (napp) {
        case 100: { cat=cat+"\\hotele.ini"; break;};
        case 201:
        case 200: { cat=cat+"\\POS.properties"; break;};
        case 300: { cat=cat+"\\catering.ini"; break;};
        case 400: { cat=cat+"\\administrator.ini"; break;};
        case 500: { cat=cat+"\\"+db_name->GetValue()+".ini"; break;};
        case 600: { cat=cat+"\\lsiphoneservice.ini"; break;};
        case 700: { cat=cat+"\\KDS.settings"; break;};
        case 800: { cat=cat+"\\PagerService.exe.config"; break;};
        case 900: { cat=cat+"\\telex.properties"; break;};
        case 1000:{ cat=cat+"\\administrator.ini"; break;};
        case 1100:{ cat=cat+"\\HospitalityAssistantConfig.xml";break;};
        case 1200:{ cat=cat+"\\MiniPOS.config";break;};
        case 1400:{ cat=cat+"\\crm.ini";break;};
        default:  { cat=cat+"\\administrator.ini"; break;};
    };

    wxTextFile file(_T(cat));
    debug("2= "+cat);
     if (napp>0 && file.Exists())
     {
        ::wxCopyFile(cat,cat+"1",true);
        ::wxRemoveFile(cat);
     };
     debug("3");
     if (napp>0)
     {
       debug(cat);
       if (file.Create() == TRUE)
       {
        debug("Plik utworzony");
        switch (napp)
        {
        case 201:
        case 200:
            {
            //Wpis jest parsowany: POS:yes|no:auto|xxx.xxx.xxx.xxx
            wxStringTokenizer s(PosHotel->GetValue(), wxT(":"));
            wxString c_pos=s.GetNextToken();
            wxString c_mas=s.GetNextToken();
            wxString c_iph=s.GetNextToken();
            wxString c_det=s.GetNextToken(); //DataExchangeType

            //nazwa lokalu jest pobierana z ComputerName
            wxString c_loc=ComputerName->GetValue();
            c_loc.Replace("%X",wxString::Format("%X",(unsigned int)rand()).c_str());

            tmp = srv_name->GetValue();
            tmp.Replace(",",":");
            file.AddLine(\
            "##"+mcopyright+"\r\n"\
            "##"+mmakefile+"\r\n"\
            "#\r\n" \
            "##System musi byæ taki sam dla wszystkich POS-ów w lokalu\r\n" \
            "system="+c_loc+"\r\n" \
            "pos="+c_pos+"\r\n\r\n" \
            \
            "##SOLO_MASTER_NO_RMI - dzia³a jako samodzielny pos, w tym samym lokalu co pozosta³e\r\n"\
            "##SINGLE_MASTER_RMI - pracuje jak dotychczas, musi byæ uruchomiony w tym samym lokalu masterpos\r\n"\
            "DataExchangeType="+(c_det.IsEmpty()?"SOLO_MASTER_NO_RMI":"SINGLE_MASTER_RMI")+"\r\n\r\n"\
            \
            "##jeœli POS pe³ni rolê serwera RMI wówczas musimy podaæ 'yes', jeœli pe³ni rolê slave podajemy 'no' albo 'solo'\r\n"\
            "master="+(c_det.IsEmpty() || c_mas.IsEmpty()?"yes":c_mas.c_str())+"\r\n" \
            "\r\n" \
            "##adres b¹dŸ nazwa kanoniczna dla adresu IP. Jest to adres komputera, na którym jest uruchomiony MASTER POS. \r\n" \
            "##Natomiast dla samego master posa ustawiamy auto - auto oznacza wyszukiwanie po UDP\r\n" \
            "masterIP="+(c_det.IsEmpty() || c_iph.IsEmpty()?"auto":c_iph.c_str())+"\r\n" \
            "\r\n" \
            "##ustawienie interface g³ównego, na którym ma nas³uchiwaæ RMI - opcja wykorzystywana przy dwóch kartach sieciowych\r\n" \
            "##parametrem jest IP ustawiony na karcie sieciowej\r\n" \
            "#masterInterface=10.0.1.139\r\n" \
            "\r\n" \
            "##b³êdy bazy s¹ przerzucane do slave POSa (true/false, wykorzystywane z triggerami)\r\n"\
            "dbErrorToClient=true\r\n"\
            "\r\n" \
            "##typ bazy danych\r\n"\
            "DatabaseType=MSSQL\r\n"\
            "\r\n" \
            "##jeœli chcemy podaæ inn¹ instancjê, wówczas na koñcu linii umieszczamy wpis: ;instance=<nazwa_instancji>\r\n"\
            "##lub za nazw¹ serwera SQL wpisujemy: ,<port>, gdzie <port> to numer portu, na którym dzia³a instancja\r\n", 
            wxTextFileType_Dos);

            ntmp=tmp.First('\\');
            debug(wxString::Format("%i",ntmp));
            if (ntmp>0) {
                file.AddLine("DBUrl=jdbc:jtds:sqlserver://"+tmp.Mid(0,ntmp)+"/"+db_name->GetValue()+";instance="+tmp.Mid(ntmp+1)+"\r\n",wxTextFileType_Dos);
            } else {
                file.AddLine("DBUrl=jdbc:jtds:sqlserver://"+tmp+"/"+db_name->GetValue()+"\r\n",wxTextFileType_Dos);
            };
            
            file.AddLine(\
            "\r\n" \
            "Username="+koduj(WxEdit6->GetValue(),true)+"\r\n" \
            "Password="+koduj(WxEdit7->GetValue(),true)+"\r\n" \
            "\r\n" \
            "##ustawienie kodowania dla znaków wyœwietlanych na POSie, jeœli nie ma ustawionych wtedy mamy bezpolskie\r\n"\
            "Encoding=Cp1250\r\n" \
            "\r\n" \
            "##ustawienie wagi na porcie COM1 (od wersji 2010.1 SPA RC03.9 przeniesione na bazê danych)\r\n" \
            "#WagaCOM=COM1\r\n" \
            "\r\n"\
            "##typ wagi (od wersji 2010.1 SPA RC03.9 przeniesione na bazê danych)\r\n" \
            "##typ wagi RADWAG obs³uguje transmisjê ci¹g³¹, ANGEL - na pobranie\r\n" \
            "#WagaTyp=RADWAG\r\n" \
            "\r\n" \
            "##okreœla COM, na którym ma pracowaæ czytnik Asec (zbli¿eniowy)\r\n"\
            "#AsecCardPort=9\r\n"\
            "\r\n"\
            "##dane czytnika\r\n"\
            "cardReaderType=mCardSCM\r\n"\
            "cardReaderPort=1\r\n"\
            "\r\n"\
            "##Ustawienie portu powoduje, ¿e pos zmienia siê w serwer nas³uchuj¹cy\r\n"\
            "##Serwer prezentuje dane na rachunkach wyœwietlaj¹c je na ekranie\r\n"\
            "#exchangeDataPort=<port>\r\n"\
            "\r\n"\
            "\r\n"\
            "##informacje dodatkowe\r\n"\
            "##no-log - uruchomienie POSa z tym parametrem powoduje pominiêcie logowania - szybciej dzia³a\r\n"\
            "##no-test-db - nie testuje istnienia bazy danych\r\n" \
            "##no-test-master - nie wyszukuje mastera??\r\n"
            , 
            wxTextFileType_Dos);
            break;
            };
            
            
        case 500:
            {
            file.AddLine(\
            "; "+mcopyright+"\r\n"\
            "; "+mmakefile+\
            "\r\n;\r\n" \
            "[DODATKOWE]\r\n" \
            "Wybor=1\r\n" \
            "WyborSkryptow=1\r\n" \
            "NaprawaBazy=0\r\n" \
            "Testy=0\r\n" \
            "TestyEnd=0\r\n" \
            "UsunIndeksyClustered\r\n" \
            "UsunTriggery=1\r\n" \
            "Przyciecie=0\r\n" \
            "Default=1\r\n" \
            "Kopia=2\r\n" \
            ";Jezyk=1252\r\n" \
            "ChangeCompatibilityLevel=1\r\n"\
            \
            "[TESTY]\r\n" \
            "Check_collation_column.sql=1\r\n" \
            \
            "[TESTYEND]\r\n" \
            "Check_collation_column.sql=0\r\n" \
            \
            "[BAZA]\r\n" \
            "Wybor=1\r\n" \
            "Serwer="+srv_name->GetValue()+"\r\n" \
            "Nazwa="+db_name->GetValue()+"\r\n" \
            "LogoWindows=0\r\n" \
            "User="+WxEdit6->GetValue()+"\r\n" \
            "Password="+WxEdit7->GetValue()+"\r\n" \
            \
            "[APLIKACJE]\r\n" \
            "Wybor=1\r\n" \
            "LSI=1\r\n" \
            "Pos-UWSPOLNIONE=1\r\n" \
            "Pos-CATERING=1\r\n" \
            "Pos-HOTELE=1\r\n" \
            "Pos-UWSPOLNIONE2005=1\r\n" \
            "Pos-Manager=1\r\n" \
            "Pos-Centrala=0\r\n" \
            \
            ";[IDENTITY]\r\n" \
            ";Wybor=1\r\n" \
            ";Wartosc=(1,1)\r\n" \
            ";Plik=\r\n" \
            ";Opcja=0\r\n" \
            \
            "[SKRYPTY]\r\n" \
            "K 05-1 Widoki C 01.sql=0\r\n" \
            "K 07 Triggery C 01.sql=0\r\n" \
            "K 07 Triggery C 02.sql=0\r\n" \
            "K 07 Triggery L 01.sql=1\r\n" \
            "K 07 Triggery L 02.sql=1\r\n",
             wxTextFileType_Dos);
            break;
            };
            
            
        case 600:
            {
            file.AddLine(\
            "[BazaDanych]\r\n" \
            "Serwer="+srv_name->GetValue()+"\r\n" \
            "Baza="+db_name->GetValue()+"\r\n" \
            "Uzytkownik="+WxEdit6->GetValue()+"\r\n" \
            "Haslo="+WxEdit7->GetValue()+"\r\n", wxTextFileType_Dos);
            break;
            };


        case 700:
            {
            file.AddLine(\
            "<?xml version=\"1.0\"?>\r\n" \
            "<settings>\r\n" \
             "<DbDataSource>"+srv_name->GetValue()+"</DbDataSource>\r\n" \
             "<DbUserID>"+WxEdit6->GetValue()+"</DbUserID>\r\n" \
             "<DbPassword>"+WxEdit7->GetValue()+"</DbPassword>\r\n" \
             "<DbInitialCatalog>"+db_name->GetValue()+"</DbInitialCatalog>\r\n" \
             "<RodzajSerwisu>1</RodzajSerwisu>\r\n" \
             "<NumerSerwisu>1</NumerSerwisu>\r\n" \
             "<RozszerzoneLogowanie>1</RozszerzoneLogowanie>\r\n" \
            "</settings>", wxTextFileType_Dos);
            break;
            };
            
            
        case 800:
            {
            file.AddLine(\
           "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"\
           "<configuration>\r\n"\
               "<configSections>\r\n"\
                  "<sectionGroup name=\"applicationSettings\" type=\"System.Configuration.ApplicationSettingsGroup, System, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089\">\r\n"\
             "<section name=\"PagerService.Properties.Settings\" type=\"System.Configuration.ClientSettingsSection, System, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089\" requirePermission=\"false\" />\r\n"\
               "</sectionGroup>\r\n"\
             "</configSections>\r\n"\
             "<applicationSettings>\r\n"\
            "<PagerService.Properties.Settings>\r\n"\
             "<setting name=\"dev_1_name\" serializeAs=\"String\">\r\n"\
               "<value>LRS</value>\r\n"\
               "</setting>\r\n"\
             "<setting name=\"dev_1_port\" serializeAs=\"String\">\r\n"\
               "<value>COM1</value>\r\n"\
               "</setting>\r\n"\
             "<setting name=\"dev_2_name\" serializeAs=\"String\">\r\n"\
               "<value />\r\n"\
               "</setting>\r\n"\
             "<setting name=\"dev_2_port\" serializeAs=\"String\">\r\n"\
               "<value />\r\n"\
               "</setting>\r\n"\
             "<setting name=\"dev_3_name\" serializeAs=\"String\">\r\n"\
               "<value />\r\n"\
               "</setting>\r\n"\
             "<setting name=\"dev_2_port\" serializeAs=\"String\">\r\n"\
               "<value />\r\n"\
               "</setting>\r\n"\
             "<setting name=\"dev_1_restaurant\" serializeAs=\"String\">\r\n"\
               "<value>1</value>\r\n"\
               "</setting>\r\n"\
             "<setting name=\"sql_user\" serializeAs=\"String\">\r\n"\
              "<value>\r\n"+WxEdit6->GetValue()+"\r\n"\
               "</value>\r\n"\
               "</setting>\r\n"\
             "<setting name=\"sql_password\" serializeAs=\"String\">\r\n"\
               "<value>\r\n"+WxEdit7->GetValue()+"\r\n"
               "</value>\r\n"\
               "</setting>\r\n"\
             "<setting name=\"sql_server\" serializeAs=\"String\">\r\n"\
               "<value>\r\n"+srv_name->GetValue()+"\r\n" \
               "</value>\r\n"\
               "</setting>\r\n"\
             "<setting name=\"sql_database\" serializeAs=\"String\">\r\n"\
               "<value>\r\n"+db_name->GetValue()+"\r\n"\
               "</value>\r\n"\
               "</setting>\r\n"\
             "</PagerService.Properties.Settings>\r\n"\
           "</applicationSettings>\r\n"\
           "</configuration>\r\n", wxTextFileType_Dos);
            break;
            };


        case 900:
            {
            file.AddLine(\
            "DB jdbc:jtds:sqlserver:/"+srv_name->GetValue()+"/"+db_name->GetValue()+"\r\n" \
            "user "+WxEdit6->GetValue()+"\r\n" \
            "password "+WxEdit7->GetValue(), wxTextFileType_Dos);
            break;
            };


        case 1200:
            {
            file.AddLine(\
            "<MiniPOSConfiguration>\r\n"\
                "<LocalBaseName>"+db_name->GetValue()+"</LocalBaseName>\r\n" \
                "<POSName>"+srv_name->GetValue()+"</POSName>\r\n" \
                "<Login>"+WxEdit6->GetValue()+"</Login>\r\n" \
                "<Password>"+koduj(WxEdit7->GetValue(),true)+"</Password>\r\n" \
            "</MiniPOSConfiguration>", wxTextFileType_Dos);
            break;
            };

        //chart
        case 1300:
            {
            file.AddLine(\
            "<?xml version=\"1.0\" standalone=\"yes\"?>\r\n" \
            "<DSKonfiguracja xmlns=\"http://tempuri.org/DSKonfiguracja.xsd\">\r\n" \
              "<Konfiguracja>\r\n" \
                "<Serwer>"+srv_name->GetValue()+"</Serwer>\r\n" \
                "<Baza>"+db_name->GetValue()+"</Baza>\r\n" \
                "<Login>"+WxEdit6->GetValue()+"</Login>\r\n" \
                "<Haslo>"+WxEdit7->GetValue()+"</Haslo>\r\n" \
                "<Profil>ChartSQL</Profil>\r\n" \
                "<AktualizujAutomatycznie>false</AktualizujAutomatycznie>\r\n" \
              "</Konfiguracja>\r\n" \
            "</DSKonfiguracja>", wxTextFileType_Dos);
            break;
            };


        case 1100:
            {
             file.AddLine(\
             "<?xml version=\"1.0\" standalone=\"yes\"?>\r\n"\
             "\r\n"\
             "<!-- "+mcopyright_nopl+" -->\r\n"\
             "<!-- "+mmakefile +" -->\r\n"\
             "\r\n" \
             "<config>\r\n"\
               "\t<setting key=\"SQL.DBManager.DataBaseType\" type=\"LSISoftware.PocketPC.Common.SQL.DatabaseType, SQL, Version=1.0, Culture=neutral, PublicKeyToken=null\">\r\n"\
               "\t<DatabaseType>RemoteMSSQL</DatabaseType>\r\n"\
               "\t<!--DatabaseType>MixedLocalRemote</DatabaseType-->\r\n"\
               "\t<!--DatabaseType>LocalReplicated</DatabaseType-->\r\n"\
               "\t</setting>\r\n"\
               "\t<setting key=\"HospitalityAssistant.DataPath\" type=\"System.String\"><string>Data\\</string></setting>\r\n"\
               "\t<setting key=\"HospitalityAssistant.Language\" type=\"System.String\"><string>PL</string></setting>\r\n"\
               "\t<setting key=\"HospitalityAssistant.ClientName\" type=\"System.String\"><string>Hospitality Assistant</string></setting>\r\n"\
               "\t<setting key=\"HospitalityAssistant.SQL.LocalReplicatedDB.ClientSeed\" type=\"System.Int32\"><int>1</int></setting>\r\n"\
               "\t<setting key=\"HospitalityAssistant.SQL.LocalReplicatedDB.Increment\" type=\"System.Int32\"><int>100</int></setting>\r\n"\
             "\r\n"\
             "<!-- Cztery ponizsze linie istotne dla bazy lokalowej -->\r\n"\
               "\t<setting key=\"SQL.RemoteMSSqlDB.DataSource\" type=\"System.String\"><string>"+srv_name->GetValue()+"</string></setting>\r\n"\
               "\t<setting key=\"SQL.RemoteMSSqlDB.Database\" type=\"System.String\"><string>"+db_name->GetValue()+"</string></setting>\r\n"\
               "\t<setting key=\"SQL.RemoteMSSqlDB.Login\" type=\"System.String\"><string>"+WxEdit6->GetValue()+"</string></setting>\r\n"\
               "\t<setting key=\"SQL.RemoteMSSqlDB.Password\" type=\"System.String\"><string>"+WxEdit7->GetValue()+"</string></setting>\r\n"\
             "\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.PublisherDatabase\" type=\"System.String\"><string>pk_lokal_2008_4</string></setting>\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.Publication\" type=\"System.String\"><string>pk_lokal_2008_4</string></setting>\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.SubscriberDatabase\" type=\"System.String\"><string>replDB.sdf</string></setting>\r\n"\
             "\r\n"\
               "\t<setting key=\"Logging.Logfile\" type=\"System.String\"><string>log.txt</string></setting>\r\n"\
             "<!-- PRACA: -->\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.InternetURL\" type=\"System.String\"><string>http://kwlodarczyk/sql_vero_sklep/sqlcesa30.dll</string></setting>\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.InternetLogin\" type=\"System.String\"><string>IIS_SQL</string></setting>\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.InternetPassword\" type=\"System.String\"><string>sa</string></setting>\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.Publisher\" type=\"System.String\"><string>sql-test\\mssql2005,1533</string></setting>\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.PublisherLogin\" type=\"System.String\"><string>sa</string></setting>\r\n"\
               "\t<setting key=\"SQL.LocalReplicatedDB.PublisherPassword\" type=\"System.String\"><string>superdkj</string></setting>\r\n"\
             "</config>\r\n", 
            wxTextFileType_Dos);
            break;
            };


        case 400:
        case 1400:
            {
             file.AddLine(\
             "; "+mcopyright+"\r\n"\
             "; "+mmakefile+\
             "\r\n;\r\n" \
             "[BazyDanych]\r\n" \
             "Count=1\r\n" \
             "Baza1Nazwa="+MakeMacro(other_param["dispbase"])+"\r\n" \
             "Baza1Typ=MsSQL\r\n" \
             "Baza1Def="+ \
                     koduj_connecting(\
                       "Provider=SQLOLEDB.1;Password="+WxEdit7->GetValue()+\
                       ";Persist Security Info=True;User ID="+WxEdit6->GetValue()+\
                       ";Initial Catalog="+db_name->GetValue()+\
                       ";Data Source="+srv_name->GetValue() \
                     ) + \
                     "\r\n" \
             "Domyslna="+MakeMacro(other_param["dispbase"])+"\r\n" \
             "\r\n" \
             "[Application]", wxTextFileType_Dos);

             file.AddLine("ComputerName="+ComputerName->GetValue(), wxTextFileType_Dos);

             if (napp==1400) //crm
                 file.AddLine("NazwaCzytnikaGarntner=REC22_1", wxTextFileType_Dos);
             
             file.AddLine("pos='"+PosHotel->GetValue()+"'\r\n", wxTextFileType_Dos);

             file.AddLine(\
             "Eksport=.\\WYDRUKI\r\n" \
              "\r\n" \
              "[Czytniki_kart]\r\n" \
              "Rodzaj_czytnika = 3\r\n" \
              "FC_NONE = 0;\r\n" \
              "FC_TOWITOKO = 1;\r\n" \
              "FC_ASSEC = 2;\r\n" \
              "FC_MCARDSCM = 3;\r\n" \
              "FC_ZBLIZENIOWY = 4;\r\n", wxTextFileType_Dos);
            break;
            };


        default:
            {
             file.AddLine(\
             "; "+mcopyright+"\r\n"\
             "; "+mmakefile+\
             "\r\n\r\n" \
             "[BazyDanych]\r\n" \
             " Count=1\r\n" \
             " Baza1Nazwa="+MakeMacro(other_param["dispbase"])+"\r\n" \
             " Baza1Def="+ \
                     koduj_connecting(\
                       "Provider=SQLOLEDB.1;Password="+WxEdit7->GetValue()+\
                       ";Persist Security Info=True;User ID="+WxEdit6->GetValue()+\
                       ";Initial Catalog="+db_name->GetValue()+\
                       ";Data Source="+srv_name->GetValue() \
                     ) + \
                     "\r\n" \
             " Domyslna="+MakeMacro(other_param["dispbase"])+"\r\n" \
             "\r\n" \
             "[Application]\r\n" \
             " ComputerName="+ComputerName->GetValue()+"\r\n" \
             " pos='"+PosHotel->GetValue()+"'\r\n", wxTextFileType_Dos);

             file.AddLine(\
             " Eksport=.\\WYDRUKI\r\n" \
              "\r\n" \
              "; Definicja czytnika kart\r\n"\
              "[Czytniki_kart]\r\n" \
              " Rodzaj_czytnika = 3\r\n" \
              " ;FC_NONE = 0\r\n" \
              " ;FC_TOWITOKO = 1\r\n" \
              " ;FC_ASSEC = 2\r\n" \
              " ;FC_MCARDSCM = 3\r\n" \
              " ;FC_ZBLIZENIOWY = 4\r\n" \
              "\r\n" \
              ";  Globalne - pozosta³oœæ, ale mo¿na sterowaæ identyfikatorem aplikacji\r\n" \
              "; [Globalne]\r\n" \
              ";  Numer_Aplikacji=111\r\n" \
              "\r\n" \
              ";  SUBCARDS - kartoteka klientów sport - serwer kart\r\n" \
              "; [SUBCARDS]\r\n" \
              ";  adres_serwera_kart=localhost\r\n" \
              ";  port_serwera_kart=1000\r\n" \
              "\r\n" \
              ";  Parametry dla wydruku tektowego, ustawiane przez wybór w parametrach w Manager/Hotel\r\n"\
              "; [Wydruk_tekstowy]\r\n"\
              ";  Port przyjmuje wartoœci dostêpne dla COM i LPT (pobiera z systemu)\r\n"\
              ";  Port=COM3\r\n"\
              ";  Dokumenty_magazynowe=1\r\n"\
              ";  Faktury=1\r\n"\
              ";  Ilosc_znakow_w_wierszu=100\r\n"\
              ";  Ilosc_linii_na_stronie=60\r\n"\
              ";  Wydruk_ozdobny=1\r\n"
              , wxTextFileType_Dos);
            break;
            };
        };

        file.Write(wxTextFileType_Dos);
        file.Close();
       }
      else {
        wxMessageBox("Brak wybranego programu w podanej lokalizacji\n\r"+tstfile.GetFullPath());
        //return;
      };
    };
     SetTitle(wxT("Change Request - CR\n")+\
              app_ctrl->GetValue().Trim()+\
              "\n"+(wxString)ver_ctrl->GetValue()+\
              "\n"+(wxString)db_name->GetValue());

     ControlApp *kontrola_uruchomienia;
     if (other_param["autohide"][0]=='1')
     {
         Iconize(true);
         kontrola_uruchomienia = new ControlApp(this);
     } else
         kontrola_uruchomienia = NULL;

     //if (issound->IsChecked())
     //   wxSound(other_param["filesound"]).Play(wxSOUND_ASYNC);
     
     if (napp==(int)500) //to uruchamiamy z parametrem
     {
       tmp=MakeMacro(real_app[app_list->GetSelection()]);
       debug(MakeMacro(tmp));
       tmp=tmp.Mid(tmp.Last(wxChar('\\'))+1);
       if (other_param["autohide"][0]=='1')
       {
        ::wxExecute(tmp+" "+db_name->GetValue()+".ini",wxEXEC_ASYNC,kontrola_uruchomienia);
       } else
        ::wxExecute(tmp+" "+db_name->GetValue()+".ini",wxEXEC_ASYNC);
     } 
     else if (napp==(int)1100 || //ICS - tego nie uruchamiamy, tylko potrzebny konfig
              napp==(int)1200  ) //PDA - tego nie uruchamiamy, tylko potrzebny konfig
     {
       wxMessageBox("Plik konfiguracji utworzony\r\n"+(wxString)file.GetName());
       tmp=MakeMacro(real_app[app_list->GetSelection()]);
       debug(MakeMacro(tmp));
       tmp=tmp.Mid(tmp.Last(wxChar('\\'))+1);
     }
     else if (napp==(int)201)
     {
       tmp=MakeMacro(real_app[app_list->GetSelection()]);

       //jeœli javwa jest w katalogu wy¿ej odpalaj w³aœnie t¹ java
       if (::wxFileExists("..\\jre\\bin\\javaw.exe"))
           tmp="..\\jre\\bin\\javaw.exe -jar "+tmp;
        else
           tmp="javaw -jar "+tmp;
       
       debug(MakeMacro("javaw -jar "+tmp));
       if (other_param["autohide"][0]=='1') {
         ::wxExecute(tmp,wxEXEC_ASYNC,kontrola_uruchomienia);
       } else 
         ::wxExecute(tmp,wxEXEC_ASYNC);
    
     }
     else
     {
       tmp=MakeMacro(real_app[app_list->GetSelection()]);
       debug(MakeMacro(tmp));
       
     //ustawiono autologowanie do aplikacji Hospitality (Manager, Hotele, Catering, Centrala)
     if (other_param["autologin"][0]=='1') {
         ThreadAutoRunAppStart(0);
     } else
       PutRegUser();
                                              
     //jeœli tmp ma kilka programów oddzielonych separatorem potoku '|' to uruchamiamy po kolei,
     //a ostatni kontroluje aplikacjê wywo³uj¹c¹  
     wxStringTokenizer s(tmp, wxT("|"));                                                        
     do {                                                                                       
       if (other_param["autohide"][0]=='1') {
          ::wxExecute(s.GetNextToken().Trim(false),wxEXEC_ASYNC,kontrola_uruchomienia);
       } else 
          ::wxExecute(s.GetNextToken().Trim(false),wxEXEC_ASYNC);	                                     
	  } while (s.HasMoreTokens()); 
    
     if (other_param["autologin"][0]=='1')
         ThreadAutoRunAppStop();                                                              
     };
  }; //if(1)
}

void intaktDlg::WxChoice4Selected(wxCommandEvent& event )
{
 size_t nsel=ver_list->GetSelection(); 
 //FindString(ver_ctrl->GetValue());
 if (nsel>=0) {
   ver_ctrl->SetValue(ver_list->GetStringValue());
   if (real_path[nsel]!="")
     WxEdit5->SetValue(MakeMacro(real_path[nsel]));
 };
}

void intaktDlg::bWersjaClickRibbon(wxCommandEvent& event)
{
        wxPoint p=::wxGetMousePosition();
        wxPoint pc=ClientToScreen(WxToolbarPanel->GetPosition());
   if ((p.x-pc.x)>23)                   //prawa strona buttona
     {
       VerShowContextMenu();
    } else
    {
       bWersjaClick(event);
    };
};

/*
 * Pobranie jedynie Wersji pliku
 */
void intaktDlg::bWersjaClick(wxCommandEvent& event)
{
    unsigned int tmp_mask = mask;

    mask = BIT_VERSION;
    if (event.GetId()==7002)
    {
        //mask |= BIT_CR;
        event.SetString("VC");  //cofniecie CR
    }
    else if (event.GetId()==7006)
    {
        mask |= BIT_CR;
        event.SetString("V_");  //cofniecie CR
    }
    else if (event.GetId()==7005)
    {
        mask |= BIT_CONNECTION;
        mask |= BIT_CR;       
        event.SetString("VC");
    }
    else if (event.GetId()==7003)
        event.SetString("VK");
    else if (event.GetId()==7004)
        event.SetString("VB");
    else            
        event.SetString("V_");  //jeœli w przekazanym evencie ustawimy "V" wówczas nie dogenerowywuje siê rodzaj usterki
        
    generujClick(event);
    mask = tmp_mask;
    Iconize(true);    

}

/*
 * Pobranie jedynie Connection String
 */
void intaktDlg::bConnectionClick(wxCommandEvent& event)
{
    unsigned int tmp_mask = mask;
    mask = BIT_CONNECTION;
    event.SetString("V"); //jeœli w przekazanym evencie ustawimy "V" wówczas nie dogenerowywuje siê rodzaj usterki
    generujClick(event);
    mask = tmp_mask;
    Iconize(true);    
};

void intaktDlg::cr_stat_propClick(wxCommandEvent& event)
{
}

void intaktDlg::WxButton3Click(wxCommandEvent& event)
{
   k1l->Enable(true);
   k1p->Enable(true);
   WxButton3->Enable(false);
   if (k2l->IsEnabled())
   {
      k1p->Enable(false);
      SetSize(88,35,1104,670);
    }
   else
      SetSize(190,35,900,670);
   Center();   
   
}

void intaktDlg::intaktDlgActivate(wxActivateEvent& event)
{
	this->SetFocus();
}

void intaktDlg::k2lClick(wxCommandEvent& event)
{
   SetSize(190,35,900,670);
   Center();
   k1l->Enable(true);
   k1p->Enable(true);
   k2l->Enable(false);
}

void intaktDlg::k1pClick(wxCommandEvent& event)
{
   SetSize(88,35,1104,670);
   Center();
   k1p->Enable(false);
   k2l->Enable(true);
}

void intaktDlg::k1lClick(wxCommandEvent& event)
{
   SetSize(190,35,288,670); 
   Center();                
   k1l->Enable(false);
   k1p->Enable(false);
   WxButton3->Enable(true);
};

void intaktDlg::btn_getposClick(wxCommandEvent& event)
{
	lstPOS *posy = new lstPOS(this,PosHotel,this,1);
	posy->SetTitle(wxT("Lista zdefiniowanych POS-ów"));
	SetSqlStm("select nazwa,ip,id from posy");
    posy->Show();
}

/*
 * btn_getadmClick
 */
void intaktDlg::btn_getadmClick(wxCommandEvent& event)
{
	lstPOS *posy = new lstPOS(this,ComputerName,this,1);
	posy->SetTitle(wxT("Lista komputerów administracyjnych"));
    SetSqlStm("select nazwa,ip,id from komputery");
    posy->Show();
}

wxString intaktDlg::GetSqlStm()
{
	return SqlStm;
}

void intaktDlg::SetSqlStm(const wxString &x)
{
	SqlStm = x;
}

/*
 * btn_getusrClick
 */
void intaktDlg::btn_getusrClick(wxCommandEvent& event)
{
	wxString SqlStm;
	wxString app;
	
    app=(app_ctrl->GetValue().Lower());
    
	lstPOS *musr = new lstPOS(this,ComputerName,this,2);
	musr->SetTitle(_("Lista u¿ytkowników"));
	SqlStm="select nazwa_skrocona,haslo,pos_pin+' - '+imie+' '+nazwisko "\
           "from uzytkownicy "\
           "where isnull(nazwa_skrocona,'')<>'' and rodzaj=0 ";
	
	if (app.Contains("hotel"))
	    SqlStm+="and (Aplikacja=20 or isnull(nazwa_skrocona,'')='serwis') ";
	else if (app.Contains("catering"))
	    SqlStm+="and (Aplikacja=10 or isnull(nazwa_skrocona,'')='serwis') ";
	else if (app.Contains("pos") or  app.Contains("manager"))
    	SqlStm+="and (Aplikacja=0 or isnull(nazwa_skrocona,'')='serwis') ";
	
	SqlStm+="order by nazwa_skrocona";
    SetSqlStm(SqlStm);

    musr->Disconnect(1004,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(lstPOS::WxButton1Click) ); //wxCommandEvent& event)
    musr->Connect(1004,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(lstPOS::usrSaveReg) ); //wxCommandEvent& event)
   (musr->btn_get)->SetLabel("Save RegKey");

    musr->Show();
}

// Zmienia makra w podanym ³añcuchu i zwraca zmodyfikowany ³añcuch
wxString intaktDlg::MakeMacro(wxString& strcompile)
{
    unsigned int nlen = strcompile.Length();
    wxString tmp="";
    wxString strnew("");
    
	for (unsigned int q=0; q<nlen; q++)
	{
        //makra sta³e %% efiniowalne przez u¿ytkownika, iloœæ makr dowolna
        if (strcompile[q]=='%')
        {
            unsigned int nok=endclose_macro(strcompile,'%',q,10);
            if (nok>0)
            {
              wxString tmp1=strcompile.Mid(q,nok+1).Lower();
              wxString macro = other_param[tmp1];
              macro.Replace("\\n","\n",true);
              strnew.Append(MakeMacro(macro)); //rekurencja, aby podmieniæ makra @@
              q+=nok;
              continue;
            };
        };
#define dbg_macro(tmp); // wxMessageBox(tmp);

        //makra niedefinowalne @@, prócz piêciu definiowany online @m<1..5>@
        if (strcompile[q]=='@')
        {
            if (strcompile[q+7]=='@' && strcompile.Mid(q,4)=="memo") {
                tmp=strcompile.Mid(q,8).Lower();
                dbg_macro(tmp);
                if (tmp=="@memo01@") {
                    strnew.append(pg_source[0]); q+=7;
                } else
                if (tmp=="@memo02@") {
                    strnew.append(pg_source[1]); q+=7;
                } else
                if (tmp=="@memo03@") {
                    strnew.append(pg_source[2]); q+=7;
                } else
                if (tmp=="@memo04@") {
                    strnew.append(pg_source[3]); q+=7;
                } else
                if (tmp=="@memo05@") {
                    strnew.append(pg_source[4]); q+=7;
                }
            }
            else
            if (strcompile[q+2]=='@') {
                tmp=strcompile.Mid(q,3).Lower();
                dbg_macro(tmp);
                if (tmp=="@s@") {
                    strnew.append(srv_name->GetValue());q+=2;
                } else
                if (tmp=="@u@") {
                    strnew.append(WxEdit6->GetValue());q+=2;
                } else
                if (tmp=="@p@") {
                    strnew.append(WxEdit7->GetValue());q+=2;
                } else
                if (tmp=="@d@") {
                    strnew.append(db_name->GetValue());q+=2;
                };
            }
            else if ( strcompile[q+3]=='@' || strcompile[q+3]=='+' || strcompile[q+3]==':' ) {
                tmp=strcompile.Mid(q,4).Lower();
                dbg_macro(tmp);
                if (tmp=="@m1@") {
                    strnew.append(m1->GetValue()); q+=3;
                } else
                if (tmp=="@m2@") {
                    strnew.append(m2->GetValue()); q+=3;
                } else
                if (tmp=="@m3@") {
                    strnew.append(m3->GetValue()); q+=3;
                } else
                if (tmp=="@m4@") {
                    strnew.append(m4->GetValue()); q+=3;
                } else
                if (tmp=="@m5@") {
                    strnew.append(m5->GetValue()); q+=3;
                } else
                if (strcompile.Mid(q,3)=="@LN" && strcompile[q+3]==':') { //mamy DateSpan                  
                    wxString dts="";
                    unsigned int t; 
                    const unsigned char n=150; 
                    for (t=0; strcompile[q+t+4]!='@' && t<n; t++) { //sprawdzamy maksymalnie do 'n' znaków
                       dts.Append(strcompile[q+t+4] );
                    };
                    if (t<n) { //wygl¹da ¿e siê odczyta³o poprawnie, wiêc tniemy
                      q+=(4+t);
                      long il;
                      wxStringTokenizer s(dts,":");
                      s.GetNextToken().ToLong(&il);
                      dts=s.GetNextToken();
                      for (t=0;t<(unsigned int)il;t++)
                         strnew.append(dts);
                    }
                } else
                if (strcompile.Mid(q,3)=="@DT" && strcompile[q+3]=='+') { //mamy DateSpan                  
                    wxString dts="";
                    unsigned int t; 
                    for (t=0; strcompile[q+t+4]!='@' && t<16; t++) { //sprawdzamy maksymalnie do 16 znaków
                       dts.Append(strcompile[q+t+4] );
                    };
                    if (t<16) { //wygl¹da ¿e siê odczyta³o poprawnie, wiêc tniemy
                       long y,m,w,d;
                       y=m=w=d=0;
                       wxString st;
                       wxStringTokenizer s(dts, wxT(","));
                       if (s.CountTokens()==4) { //poprawny format
                           st=s.GetNextToken(); st.ToLong(&y); 
                           st=s.GetNextToken(); st.ToLong(&m); 
                           st=s.GetNextToken(); st.ToLong(&w); 
                           st=s.GetNextToken(); st.ToLong(&d); 
                       };
                       wxDateTime czas = wxDateTime::Now();                       
                       wxDateSpan przesuniecie((int)y,(int)m,(int)w,(int)d);                     
                       czas.Add(przesuniecie);                       
                       strnew.append(czas.FormatISODate());
                       q+=(4+t);
                    };
                } else
                if (strcompile.Mid(q,4)=="@DT@") {                  
                    wxDateTime czas = wxDateTime::Now();
                    strnew.append(czas.FormatISODate()); q+=3;
                } else
                if (strcompile.Mid(q,4)=="@TM@") {
                    wxDateTime czas = wxDateTime::Now();
                    strnew.append(czas.FormatISOTime()); q+=3;
                } else
                if (strcompile.Mid(q,4)=="@L1@") {
                    strnew.append(10,'-'); q+=3;
                } else
                if (strcompile.Mid(q,4)=="@L2@") {
                    strnew.append(10,'='); q+=3;
                } else
                if (strcompile.Mid(q,4)=="@L3@") {
                    strnew.append(10,'.'); q+=3;
                } else
                if (tmp=="@r0@") { strnew.append(r_critical->GetLabelText());      q+=3; } else
                if (tmp=="@r1@") { strnew.append(r_usterka->GetLabelText());       q+=3; } else
                if (tmp=="@r2@") { strnew.append(r_drobna_zmiana->GetLabelText()); q+=3; } else
                if (tmp=="@r3@") { strnew.append(r_rozwojowe->GetLabelText());     q+=3; } else
                if (tmp=="@r4@") { strnew.append(r_support->GetLabelText());       q+=3; } else
                if (tmp=="@r5@") { strnew.append(r_migracja->GetLabelText());      q+=3; } else
                if (tmp=="@r6@") { strnew.append(r_test->GetLabelText());          q+=3; } else
                if (tmp=="@r7@") { strnew.append(r_inne->GetLabelText());          q+=3; } else
                if (tmp=="@r8@") { strnew.append(r_unt->GetLabelText());           q+=3; } else
                if (tmp=="@rc@") { strnew.append(r_critical_dkj->GetLabelText());  q+=3; } else
                if (tmp=="@ru@") { strnew.append(r_usterka_dkj->GetLabelText());   q+=3; }
                else
                 strnew.Append(strcompile[q],1);
            }
            else if (strcompile[q+5]=='@') {
                tmp=strcompile.Mid(q,6).Lower();
                dbg_macro(tmp);
                if (tmp=="@dapp@") {           //wstawienie pola 'katalog aplikacji'
                   strnew.append(WxEdit5->GetValue()); q+=5;
                } else if (tmp=="@darc@") {    //wstawienie katalogu archiwum
                   strnew.append(WxEdit1->GetValue()); q+=5;
                } else if (tmp=="@conn@") {    //wstawienie connectionstring
                   strnew.append(GetConnectionString()); q+=5;
                } else if (tmp=="@conp@") {    //wersja Connection String dla POSA
                   strnew.append(GetConnectionPositivePOS()); q+=5;
                };
            }
            else if (strcompile[q+4]=='@') { //intaktDlg::GetVersionPositivePOS()
                tmp=strcompile.Mid(q,5).Lower();
                dbg_macro(tmp);
                if (tmp=="@ver@") {           //wstawienie pola 'wersji pliku'
                   strnew.append(GetVersionPositive()); q+=4;
                } else if (tmp=="@pos@") {  //nazwa POS-a
                   strnew.append(PosHotel->GetValue()); q+=4;
                } else if (tmp=="@dbn@") {
                   strnew.append(db_name->GetValue()); q+=4;
                };
            }
            else {
                strnew.Append(strcompile[q],1);
            };
        } else {
            strnew.Append(strcompile[q],1);
        };
    };
    
 return strnew;
}

void intaktDlg::bCRTextClick(wxCommandEvent& event)
{
    unsigned int tmp_mask = mask;
    mask = BIT_CR;
    event.SetString("V");  //jeœli w przekazanym evencie ustawimy "V" wówczas nie dogenerowywuje siê rodzaj usterki    
    generujClick(event);
    mask = tmp_mask;
    Iconize(true);    
}

void intaktDlg::isviewClick(wxCommandEvent& event)
{
    if (isview->IsChecked())
    {
      mask |= BIT_SHOWWIN;
    } else {    
      mask = ~mask;
      mask |= BIT_SHOWWIN;
      mask = ~mask;
    }  
}

wxString intaktDlg::GetConnectionPositivePOS()
{
    wxString buf="";
    wxString tmp;
    int ntmp;

    //Wpis jest parsowany: POS:yes|no:auto|xxx.xxx.xxx.xxx
    wxStringTokenizer s(PosHotel->GetValue(), wxT(":"));
    wxString c_pos=s.GetNextToken();
    wxString c_mas=s.GetNextToken();
    wxString c_iph=s.GetNextToken();    
    //nazwa lokalu jest pobierana z ComputerName
    wxString c_loc=ComputerName->GetValue();
    c_loc.Replace("%X",wxString::Format("%X",(unsigned int)rand()).c_str());
    
    tmp = srv_name->GetValue();
    tmp.Replace(",",":");
    if (WxEdit6->GetValue().Contains(_("backup")))
        buf+=_("BAZA WY£¥CZNIE DO ODCZYTU I BACKUP'u\n\n");
        
    buf+=\
    "system="+c_loc+"\r\n" \
    "pos="+c_pos+"\r\n"\
    "master="+(c_mas.IsEmpty()?"yes":c_mas.c_str())+"\r\n" \
    "masterIP="+(c_iph.IsEmpty()?"auto":c_iph.c_str())+"\r\n" \
    "dbErrorToClient=true\r\n"\
    "DatabaseType=MSSQL\r\n"\
    "DBUrl=jdbc:jtds:sqlserver://";
    ntmp=tmp.First('\\');
    if (ntmp>0) {
        buf+=tmp.Mid(0,ntmp)+"/"+db_name->GetValue()+";instance="+tmp.Mid(ntmp+1)+"\r\n";
    } else {
        buf+=tmp+"/"+db_name->GetValue()+"\r\n";
    };
    buf+=\
    "##Username="+WxEdit6->GetValue()+"\r\n" \
    "##Password="+WxEdit7->GetValue()+"\r\n";
    if (!WxEdit6->GetValue().Contains(_("backup")))
    {
      buf+= \
      "Username="+koduj(WxEdit6->GetValue(),true)+"\r\n" \
      "Password="+koduj(WxEdit7->GetValue(),true)+"\r\n";        
    }
    buf+="Encoding=Cp1250\r\n";

    return buf;
}

// Pobranie sformatowanego ADOString ConnectionString
wxString intaktDlg::GetConnectionString()
{
    //Zawsze ktoœ puszcza³ skrypty na bazie któr¹ testowa³em, albo ostro w niej
    //miesza³. Teraz jedynie mo¿na utworzyæ kopiê i mieæ podgl¹d danych
    if (WxEdit6->GetValue().Contains(_("backup")))
    	return wxString::Format(
               _("BAZA WY£¥CZNIE DO ODCZYTU I BACKUP'u\n"\
                 "User ID=%s; Password=%s; Initial Catalog=%s; Data Source=%s"),
                 WxEdit6->GetValue().c_str(),
                 WxEdit7->GetValue().c_str(),
                 db_name->GetValue().c_str(),
                 srv_name->GetValue().c_str()
               );

	return wxString::Format(
           _("Provider=SQLOLEDB.1; Password=%s; Persist Security Info=True; User ID=%s; Initial Catalog=%s; Data Source=%s"),
          WxEdit7->GetValue().c_str(),
          WxEdit6->GetValue().c_str(),
          db_name->GetValue().c_str(),
          srv_name->GetValue().c_str() );
};

void intaktDlg::nr_crUpdated(wxCommandEvent& event)
{
	if (event.GetString().Length()>6)
        event.SetString(event.GetString().Mid(0,event.GetString().Length()-1));
}

void intaktDlg::nr_crMaxLen(wxCommandEvent& event)
{
	nr_cr->SetValue(nr_cr->GetValue().Mid(0,nr_cr->GetLineLength(0) ));
}

/*
 * Po³¹czenie do bazy danych MDB
 * Zapis/Modyfikacja utworzonej CR
 */
int intaktDlg::dbSave(unsigned int)
{
    unsigned char n=0;
	wxString ConnectionString;
	ConnectionString="Driver={Microsoft Access Driver (*.mdb)};DBQ=intakt.mdb;Uid=Administrator;Pwd=nnn;";
    wxDbConnectInf DbConnectInf;
    DbConnectInf.AllocHenv();
    wxDb *db = new wxDb(DbConnectInf.GetHenv());
    db->Open(ConnectionString);
    if (db->IsOpen())
    {
        wxString dbBuff;
        wxDateTime czas;
        wxChar nazwa[7];
        SDWORD cb;
        memset(nazwa,0,7);
        czas.Now();
        db->ExecSql("select cr from nagl where cr='"+nr_cr->GetValue()+"'");
      
         while (n==0 && db->GetNext())
         {
          db->GetData(1,SQL_C_WXCHAR,&nazwa,7,&cb);
          n=1;
         };
         
        if (n==0) {
           dbBuff=WxEdit2->GetValue().c_str(); 
           dbBuff=MakeMacro(dbBuff); 
           dbBuff=MakeMacro(dbBuff);
           db->ExecSql(\
           wxString::Format("insert into nagl(cr,cr_temat,modul,exe_wersjtxt) values('%s','%s','%s','%s')",
                            nr_cr->GetValue().c_str(),
                            dbBuff.c_str(),
                            app_ctrl->GetValue().c_str(),
                            ver_ctrl->GetValue().c_str()
                            ));
           db->CommitTrans();
           
           dbBuff=WxMemo1->GetValue().c_str(); 
           dbBuff=MakeMacro(dbBuff); 
           dbBuff=MakeMacro(dbBuff);           
           db->ExecSql(\
           wxString::Format("insert into tresc(cr,tresc) values('%s','%s')",
                            nr_cr->GetValue().c_str(),
                            dbBuff.c_str()
                            ));
           db->CommitTrans();
        } else {
           
           dbBuff=WxEdit2->GetValue().c_str(); 
           dbBuff=MakeMacro(dbBuff); 
           dbBuff=MakeMacro(dbBuff);
           db->ExecSql(\
               wxString::Format("update nagl set cr='%s',cr_temat='%s',modul='%s',exe_wersjatxt='%s',dt_modyfikacji=Now() where cr='%s'",
                                nr_cr->GetValue().c_str(),
                                dbBuff.c_str(),
                                app_ctrl->GetValue().c_str(),
                                ver_ctrl->GetValue().c_str(),
                                nr_cr->GetValue().c_str()
                                               ) );
           db->CommitTrans();
           dbBuff=WxMemo1->GetValue().c_str(); 
           dbBuff=MakeMacro(dbBuff); 
           dbBuff=MakeMacro(dbBuff);            
           db->ExecSql(\
               wxString::Format("update tresc set tresc='%s',dt_modyfikacji=Now() where cr='%s'",
                                dbBuff.c_str(),                            
                                nr_cr->GetValue().c_str()) );
           db->CommitTrans();
        };
        db->CommitTrans();
        db->Close();
        delete db; db=0;
        return 1;        
    } else {
        wxMessageBox(_T("B³¹d otwarcia bazy danych"));
        delete db; db=0;
        return 0;
    };      
};

void intaktDlg::keymemo(wxCommandEvent& event)
{
   if (WxMemo1->GetRange(WxMemo1->GetLastPosition()-1,1)=="@")
        wxMessageBox("makro");
};

void intaktDlg::SaveClick(wxCommandEvent& event)
{
;
}

void intaktDlg::usunClick(wxCommandEvent& event)
{
;
}

void intaktDlg::TemplateLoadControl(wxString tpl)
{
  wxString sv;
  int nsel;
  
      wxStringTokenizer s(tpl, wxT(";"));

      sv=s.GetNextToken().Trim(false).Trim();
      nsel=app_list->FindString(sv);
      if (nsel>=0) {
 	      app_list->SetSelection(nsel);
 	      app_ctrl->SetValue(sv);
      }
      else {
 	      app_list->SetSelection(-1);
 	      app_ctrl->SetValue(_T(""));
      };

      sv=s.GetNextToken().Trim(false).Trim();
      nsel = ver_list->FindString(sv);
      if (nsel>=0) {
          ver_list->SetSelection(nsel);
          ver_ctrl->SetValue(sv);
          WxEdit5->SetValue(MakeMacro(real_path[nsel]));
          //wxCommandEvent event;
          //WxChoice4Selected(event); //wywo³anie zdarzenia zwi¹zanego z wyborem wersji
      }
      else {
          ver_list->SetSelection(-1);
          ver_ctrl->SetValue(_T(""));
          WxEdit5->SetValue(_T(""));
      };

      sv=s.GetNextToken().Trim(false).Trim();
      nsel=srv_name->FindString(sv);
      if (nsel>=0) {
          srv_name->SetSelection(nsel);
      };

      sv=s.GetNextToken().Trim(false).Trim();
      WxEdit6->SetValue(sv);

      sv=s.GetNextToken().Trim(false).Trim();
      WxEdit7->SetValue(sv);

      sv=s.GetNextToken().Trim(false).Trim();
      db_name->SetValue(sv);

      sv=s.GetNextToken().Trim(false).Trim();
      ComputerName->SetValue(sv);

      sv=s.GetNextToken().Trim(false).Trim();
      PosHotel->SetValue(sv);

      sv=s.GetNextToken().Trim(false).Trim();
      ed_login->SetValue(sv);

      sv=s.GetNextToken().Trim(false).Trim();
      ed_haslo->SetValue(sv);
};

/*
  Name: 
  Copyright: 
  Author: Piotr Kuliñski
  Date: 03-02-2010 16:04
  Description:  Wstawienie definicji wybranego szablonu do kontrolek
*/
void intaktDlg::XSelectSzablon(wxString NazwaSzablonu)
{
      wxString st,sv;
      //int nsel;
      for (size_t q=0; q<arrayStringFor_WxChoice1.Count();q++)
      {
        wxStringTokenizer s1(arrayStringFor_WxChoice1.Item(q), wxT("="));

        st=s1.GetNextToken().Trim(false).Trim();

        if (st[0]!=';' && st==NazwaSzablonu)
        {
          TemplateLoadControl(s1.GetNextToken().Trim(false).Trim());
        };
      };    
};

/*
  Name: 
  Copyright: 
  Author: Piotr Kuliñski
  Date: 03-02-2010 16:04
  Description: Za³adowanie szablonu poprzez wybór z drzewa szablonów
*/
void intaktDlg::ed_szablonSelectedTree(wxCommandEvent& event )
{
  XSelectSzablon( GetTemplateFromTree(edn_szablon) );  
  if (TreeRun->GetValue())
      run_manager(event);
  else
  {
      wxPoint p=WxButton2->GetPosition();
      wxSize  s=WxButton2->GetSize();
      p.x+=(s.x/2);
      p.y+=(s.y/2);  
      this->WarpPointer(p.x,p.y);	      
      WxButton2->SetFocus();
  };
  pg_CR->SetPageText(pg_CR->GetSelection(),app_ctrl->GetValue());
};


void intaktDlg::ed_szablonSelected(wxCommandEvent& event )
{
//  XSelectSzablon(ed_szablon->GetValue());    
};

// Pobranie wersji pliku
wxString intaktDlg::GetVersionPositive()
{
    wxString buf="";
    wxFileName tstfile;
    wxDateTime czas;
    
	/* TODO (#1#): Implement intaktDlg::GetVersionPositive() */

    buf+="Modu³: "+app_ctrl->GetValue()+\
       "\nWersja: "+ver_ctrl->GetValue();
    if (WxCheckBox4->IsChecked())
    {
        buf+="\nZ dnia: "+(WxDatePickerCtrl1->GetValue()).FormatISODate();
    }
    else if (WxCheckBox1->IsChecked())  //jeœli wybrano katalog aplikacji wówczas data i czas zostanie pobrany z pliku
    {
        //tstfile.SetPath(WxEdit5->GetValue()+"\\"+real_app[WxChoice1->GetCurrentSelection()]);
        tstfile.SetPath(MakeMacro(real_app[app_list->GetSelection()]));
        if (wxFile::Exists(tstfile.GetFullPath()))
        {

          czas=tstfile.GetModificationTime();
          buf+="\nModyfikacja exe: "+czas.FormatISODate()+", "+czas.FormatISOTime();
          if (version->IsChecked())
          {
              wxVersionDLL v;
              buf+=" ("+v.GetFileVersion(tstfile.GetFullPath())+")";
          };
        };
    };
    return buf;
}


void intaktDlg::WxButton4Click(wxCommandEvent& event)
{
//	wxGetApp().GetPrinting()->PreviewBuffer(memo->GetBuffer());
}

// Pobranie tabeli konwersji
void intaktDlg::btn_getconversion1(wxCommandEvent& event)
{
	/* TODO (#1#): Implement intaktDlg::btn_getconversion1() */
	lstPOS *musr = new lstPOS(this,ComputerName,this,1);
	musr->SetTitle(wxT("Ostatnie konwersje"));
    SetSqlStm("select "\
              "CONVERT(CHAR(10), data, 102) AS 'Data', "\
              "COUNT(CONVERT(CHAR(10), data, 102)) AS 'Skryptow' "\
              "FROM lsi_konwersje "\
              "GROUP BY (CONVERT(CHAR(10), data, 102)) "\
              "ORDER BY (CONVERT(CHAR(10), data, 102)) DESC");
;
    musr->Show();
}

void intaktDlg::WxBitmapButton6Click(wxCommandEvent& event)
{
	DBPrintSettings *prnset = new DBPrintSettings(this);
	prnset->OnPrint(event);
}

void intaktDlg::WxBitmapButton7Click(wxCommandEvent& event)
{
	PopupMenu(WxPopupMenu1, wxPoint(WxToolbarPanel->GetPosition().x,118));  //661,118
}

void intaktDlg::MacroMenuShow(wxCommandEvent& event)
{
	ContextMenuMacro();
}

void intaktDlg::OnMenuInsertMemoCR(wxCommandEvent& event)
{
	if (event.GetId()>=6001 && event.GetId()<=6200)
        {
            wxMenuItem *mi=macro_mnu->FindItem(event.GetId());
            if (mi)
            {
              wxStringTokenizer s(mi->GetLabel(), wxT("="));
              WxMemo1->WriteText(s.GetNextToken().Trim());
            };
        }
    else
	switch (event.GetId())
	{
        case ID_CONNP:  {WxMemo1->WriteText("'@CONN@'"); break; };
        case ID_CONN:   {WxMemo1->WriteText("'@CONP@'"); break; };
        case ID_S:      {WxMemo1->WriteText("'@S@'"); break; };
        case ID_U:      {WxMemo1->WriteText("'@U@'"); break; };
        case ID_P:      {WxMemo1->WriteText("'@P@'"); break; };        
        case ID_DD:     {WxMemo1->WriteText("'@D@'"); break; };
        case ID_DAPP:   {WxMemo1->WriteText("'@DAPP@'"); break; };
        case ID_DARC:   {WxMemo1->WriteText("'@DARC@'"); break; };
        case ID_VER:    {WxMemo1->WriteText("'@VER@'"); break; };
        case ID_DT:     {WxMemo1->WriteText("'@DT@'"); break; };
        case ID_DT1:    {WxMemo1->WriteText("'@DT+y,m,t,d@'"); break; };
        case ID_LN:     {WxMemo1->WriteText("'@LN:10:+-@'"); break; };
        case ID_L1:     {WxMemo1->WriteText("'@L1@'"); break; };
        case ID_L2:     {WxMemo1->WriteText("'@L2@'"); break; };
        case ID_L3:     {WxMemo1->WriteText("'@L3@'"); break; };
        case ID_MM1:    {WxMemo1->WriteText("'@M1@'"); break; };
        case ID_MM2:    {WxMemo1->WriteText("'@M2@'"); break; };
        case ID_MM3:    {WxMemo1->WriteText("'@M3@'"); break; };
        case ID_MM4:    {WxMemo1->WriteText("'@M4@'"); break; };
        case ID_MM5:    {WxMemo1->WriteText("'@M5@'"); break; };
        //case 6001:
        //case 6200:
        default:        {WxMemo1->WriteText(wxString::Format("'@%i@'",event.GetId())); break; };
    };
    event.Skip();
};

void intaktDlg::memo_sizeClick(wxCommandEvent& event)
{
	// przesuniêcie okna dó³/góra
    wxRect okno;
    okno=pg_CR->GetRect();
    wxPanel *p;

 	// przesuniêcie okna w górê   
	if (!show_buffor)
	{
        okno.height-=110;
        pg_CR->SetSize(okno);
        p=(wxPanel*)pg_CR->GetPage(event.GetSelection());
        WxMemo1->SetSize(0,0,p->GetSize().GetWidth()-1,p->GetSize().GetHeight()-2);
        memo->Show(true);
        memo->Enable(true);
        tb->SetToolNormalBitmap(TBM_BUFF,bkd);
    } else if (show_buffor) // przesuniêcie okna w dó³
	{      
        okno.height+=110;
        memo->Show(false);
        memo->Enable(false);
        pg_CR->SetSize(okno);
        p=(wxPanel*)pg_CR->GetPage(event.GetSelection());
        WxMemo1->SetSize(0,0,p->GetSize().GetWidth()-1,p->GetSize().GetHeight()-2);
        tb->SetToolNormalBitmap(TBM_BUFF,bku);
    };
    show_buffor=!show_buffor;
};

void intaktDlg::r_zapytajClick(wxCommandEvent& event)
{
    RodzajZgloszenia *status = new RodzajZgloszenia(this);
    status->ShowModal();
}

void intaktDlg::aboutClick(wxCommandEvent& event)
{
    //wxIcon ikona(wxIcon(_("wxICON_SMALL_FILE"), wxBITMAP_TYPE_ICO_RESOURCE));
    wxAboutDialogInfo info;
    info.SetWebSite(_("www.client.pl"));
    //info.SetIcon( ikona );
    info.SetName(_(PRODUCT_NAME));
    info.SetVersion(wxString::Format("%i.%i",VER_MAJOR,VER_MINOR));
    //info.SetVersion(_("1.5"));
    info.SetDescription(_("\n\n" \
                          "PROGRAM NA WEWNÊTRZNE POTRZEBY" ));
    info.SetCopyright(wxString::Format(_T("\n%s, <piotr.kulinski@gmail.com>"),LEGAL_COPYRIGHT));

    wxAboutBox(info);
};

void intaktDlg::intaktDlgKeyDown(wxKeyEvent& event)
{
 if (event.GetKeyCode()==WXK_F1)
     wxContextHelp ch( this ); 
}

void intaktDlg::ctx_helpClick(wxCommandEvent& event)
{
     wxContextHelp ch( this );
}

void intaktDlg::intaktDlgLeftDown(wxMouseEvent& event)
{
    Iconize(true);
    event.Skip();
};

void intaktDlg::TreeMenu(wxCommandEvent& event)
{
    switch (event.GetId()) {
        case 10101: { TreeTemplateInsert(event); break; };
        case 10102: { TreeTemplateModify(event); break; };
        case 10103: { TreeTemplateDelete(event); break; };                
        case 10104: { TreeTemplateInsertGroup(event); break; };
        case 10105: { TreeTemplateCopy(event); break; };
        default:    { wxMessageBox(wxString::Format("TreeMenu %i",event.GetId())); break; };
    };
};

void intaktDlg::AppMenu(wxCommandEvent& event)
{
    switch (event.GetId()) {
        case 10201: { AppMenuEdit(event); break; };
        case 10202: { AppMenuEdit(event); break; };
        case 10203: { AppMenuDelete(event); break; };                
        default:    { wxMessageBox(wxString::Format("AppMenu %i",event.GetId())); break; };
    };
};

void intaktDlg::VerMenu(wxCommandEvent& event)
{
    switch (event.GetId()) {
        case 10301: { VerMenuEdit(event); break; };
        case 10302: { VerMenuEdit(event); break; };
        case 10303: { VerMenuDelete(event); break; };                
        default:    { wxMessageBox(wxString::Format("VerMenu %i",event.GetId())); break; };
    };
};

wxString intaktDlg::TemplateFromControl()
{
    return app_ctrl->GetValue()+";"+\
           ver_ctrl->GetValue()+";"+\
           srv_name->GetValue()+";"+\
           WxEdit6->GetValue()+";"+\
           WxEdit7->GetValue()+";"+\
           db_name->GetValue()+";"+\
           ComputerName->GetValue()+";"+\
           PosHotel->GetValue()+";"+\
           ed_login->GetValue()+";"+\
           ed_haslo->GetValue();
};

// Wstawienie nowej grupy dla szablonów
void intaktDlg::TreeTemplateInsertGroup(wxCommandEvent& event)
{
    wxTreeItemId root=edn_szablon->GetRootItem();
    wxString t=::wxGetTextFromUser("Nowy grupa", "Podaj nazwê grupy","",this);
    if (::wxIsEmpty(t.fn_str()))
       return;
    
    edn_szablon->AppendItem(root,t);
    
    TemplateName.Add(t);
     TemplateDef.Add(app_ctrl->GetValue()+";"+\
                     ver_ctrl->GetValue()+";"+\
                     srv_name->GetValue()+";"+\
                     WxEdit6->GetValue()+";"+\
                     WxEdit7->GetValue()+";"+\
                     db_name->GetValue()+";"+\
                     ComputerName->GetValue()+";"+\
                     PosHotel->GetValue()+";"+\
                     ed_login->GetValue()+";"+\
                     ed_haslo->GetValue());
                     
    arrayStringFor_WxChoice1.Clear();
    for(unsigned int i=0; i< TemplateName.GetCount();i++)
    {
        arrayStringFor_WxChoice1.Add(TemplateName.Item(i)+_T("=")+TemplateDef.Item(i));
    };                             
    arrayStringFor_WxChoice1.Sort();       
}

// Wstawienie szablonu do drzewa
void intaktDlg::TreeTemplateInsert(wxCommandEvent& event)
{
    wxString NewTemplate = GetTemplateFromTree(edn_szablon); //pobranie aktualnego wyboru (motion)

//    wxString t=::wxGetTextFromUser("Nowy szablon", "Podaj nazwê szablonu",ctrl_szablon->GetValue(),this);
    wxString t=::wxGetTextFromUser("Nowy szablon", "Podaj nazwê szablonu",app_ctrl->GetValue(),this);
    
    if (::wxIsEmpty(t.fn_str()))
       return;
           
    NewTemplate+=_T(":")+t;  //Wpisanie noweg oszablonu

    //wstawienie do drzewa nowego elementu 
    load_tree(edn_szablon, NewTemplate);
    ctrl_szablon->SetValue(t);
    	
    TemplateName.Add(NewTemplate);
     TemplateDef.Add(app_ctrl->GetValue()+";"+\
                     ver_ctrl->GetValue()+";"+\
                     srv_name->GetValue()+";"+\
                     WxEdit6->GetValue()+";"+\
                     WxEdit7->GetValue()+";"+\
                     db_name->GetValue()+";"+\
                     ComputerName->GetValue()+";"+\
                     PosHotel->GetValue()+";"+\
                     ed_login->GetValue()+";"+\
                     ed_haslo->GetValue());
                     
    arrayStringFor_WxChoice1.Clear();
    for(unsigned int i=0; i< TemplateName.GetCount();i++)
    {
        arrayStringFor_WxChoice1.Add(TemplateName.Item(i)+_T("=")+TemplateDef.Item(i));
    };                             
    arrayStringFor_WxChoice1.Sort();
    WriteSectionToFile(working_dir+"\\"+baza,"szablony",arrayStringFor_WxChoice1);
}

/*
 * Usuniêcie wskazanego szablonu z drzewa.
 * Przy usuwaniu usuniêcie root dla grupy spowoduje przepiêcie 
 * dzieci do grupy nadrzêdnej
 */
void intaktDlg::TreeTemplateDelete(wxCommandEvent& event)
{
    int nseek;    
    wxString CurrentTemplate = GetTemplateFromTree(edn_szablon); //pobranie aktualnego wyboru (motion)
    wxTreeItemId prev;
    wxTreeItemId cur = edn_szablon->GetCurrentSelectionItem();
    
      if (wxMessageBox("Na pewno chcesz usun¹æ szablon?", "PotwierdŸ", wxYES_NO | wxICON_QUESTION, this)!=wxYES)
         return;
    
    prev = edn_szablon->GetItemParent(cur);
    edn_szablon->SetCurrentSelectionItem(prev); //ustawienie na nadrzêdnym

    edn_szablon->Delete(cur);
    
    nseek=TemplateName.Index(CurrentTemplate); //wyszukanie szablonu i pobranie indeksu

    if (nseek>=0)
    {
        arrayStringFor_WxChoice1.RemoveAt(nseek);
        TemplateName.RemoveAt(nseek);
        TemplateDef.RemoveAt(nseek);        
    };
        nseek=TemplateName.Index(CurrentTemplate); //wyszukanie szablonu i pobranie indeksu

    if (nseek>=0)
    {
        arrayStringFor_WxChoice1.RemoveAt(nseek);
        TemplateName.RemoveAt(nseek);
        TemplateDef.RemoveAt(nseek);        
    };   

    WriteSectionToFile(working_dir+"\\"+baza,"szablony",arrayStringFor_WxChoice1);
}

void intaktDlg::TreeTemplateModify(wxCommandEvent& event)
{
    int npos,nseek;    
    wxString NewNameTemplate;
    wxString t=ctrl_szablon->GetValue();
    wxString CurrentTemplate = GetTemplateFromTree(edn_szablon); //pobranie aktualnego wyboru (motion) 
    wxTreeItemId cur = edn_szablon->GetCurrentSelectionItem();
    
    t=::wxGetTextFromUser("Zmiana nazwy szablonu", "Podaj nazwê szablonu",edn_szablon->GetItemText(cur),this);
    if (::wxIsEmpty(t.fn_str()))
       return;
       
    nseek=TemplateName.Index(CurrentTemplate); //wyszukanie szablonu i pobranie indeksu

    if (nseek>=0)
    {
     //modyfikacja definicji szablonu  - to zawsze jest wybrane tylko dla liœcia
     TemplateDef[nseek]=app_ctrl->GetValue()+";"+\
                        ver_ctrl->GetValue()+";"+\
                        srv_name->GetValue()+";"+\
                        WxEdit6->GetValue()+";"+\
                        WxEdit7->GetValue()+";"+\
                        db_name->GetValue()+";"+\
                        ComputerName->GetValue()+";"+\
                        PosHotel->GetValue()+";"+\
                        ed_login->GetValue()+";"+\
                        ed_haslo->GetValue();
    };    
    
    npos = CurrentTemplate.Find(':',true);
    if (npos!=wxNOT_FOUND) {
        NewNameTemplate = CurrentTemplate.SubString(0,npos-1)+":"+t;
    }
    else {
        NewNameTemplate = t;    
    };
    npos=CurrentTemplate.Length()-1;    
    edn_szablon->SetItemText(cur,t);
    ctrl_szablon->SetValue(t);
    edn_szablon->SetCurrentSelectionItem(cur);

    //teraz trzeba przejechaæ szablony i zmieniæ wybran¹ frazê na nowopodan¹
    arrayStringFor_WxChoice1.Clear();
    for(unsigned int i=0; i< TemplateName.GetCount();i++)
    {
        t=TemplateName.Item(i);
        if (t.SubString(0,npos)==CurrentTemplate) //znaleziono wybrany szablon
            TemplateName[i]=NewNameTemplate+t.Mid(npos+1);        

        arrayStringFor_WxChoice1.Add(TemplateName.Item(i)+_T("=")+TemplateDef.Item(i));
    };                             
    arrayStringFor_WxChoice1.Sort();

    WriteSectionToFile(working_dir+"\\"+baza,"szablony",arrayStringFor_WxChoice1);
    XSelectSzablon( GetTemplateFromTree(edn_szablon) );
}

/*
* Kopiowanie wybranej ga³êzi z now¹ nazw¹
*/
void intaktDlg::TreeTemplateCopy(wxCommandEvent& event)
{
    int npos,nseek;
    wxString NewNameTemplate;
    wxString t=ctrl_szablon->GetValue();
    wxString CurrentTemplate = GetTemplateFromTree(edn_szablon); //pobranie aktualnego wyboru (motion)
    wxTreeItemId cur = edn_szablon->GetCurrentSelectionItem();

    t=::wxGetTextFromUser("Nazwa dla kopiowanego szablonu", "Podaj nazwê szablonu",edn_szablon->GetItemText(cur),this);
    if (::wxIsEmpty(t.fn_str()))
       return;
    else
      if (t==edn_szablon->GetItemText(cur))
      {
            wxMessageBox(_("Nazwa szablonu musi byæ unikalna"));
            return;
        };

    nseek=TemplateName.Index(CurrentTemplate); //wyszukanie szablonu i pobranie indeksu

    npos = CurrentTemplate.Find(':',true);
    if (npos!=wxNOT_FOUND) {
        NewNameTemplate = CurrentTemplate.SubString(0,npos-1)+":"+t;
    }
    else {
        NewNameTemplate = t;
    };
    //wxMessageBox(NewNameTemplate);
    
    npos=CurrentTemplate.Length()-1;
    edn_szablon->SetItemText(cur,t);
    ctrl_szablon->SetValue(t);
    edn_szablon->SetCurrentSelectionItem(cur);

    //teraz trzeba przejechaæ szablony i zmieniæ wybran¹ frazê na nowopodan¹
    arrayStringFor_WxChoice1.Clear();
    int ncount=TemplateName.GetCount();
    for(unsigned int i=0; i<ncount;i++)
    {
        t=TemplateName.Item(i);
        //wxLogMessage("item: %s (%c,%c) -> %s, wyciête %s",CurrentTemplate.c_str(),t[npos],t[npos+1],t.c_str(),t.SubString(0,npos).c_str());
        if (t.SubString(0,npos)==CurrentTemplate && 
           (t[npos+1]==' ' || 
            t[npos+1]==':' || 
            t[npos+1]=='=')) //Sopiowanie wyranego do nowego
        {
            TemplateName.Add(NewNameTemplate+t.Mid(npos+1)+_T("=")+TemplateDef.Item(i));
            TemplateDef.Add(TemplateDef.Item(i));
            wxLogMessage("kopiowanie: %s %s",t.SubString(0,npos).c_str(),TemplateName.Item(i).c_str());
        }
        
    };
    ncount=TemplateName.GetCount();
    for(unsigned int i=0; i<ncount;i++)
    {
        arrayStringFor_WxChoice1.Add(TemplateName.Item(i)+_T("=")+TemplateDef.Item(i));
    }
    arrayStringFor_WxChoice1.Sort();

    WriteSectionToFile(working_dir+"\\"+baza,"szablony",arrayStringFor_WxChoice1);
    //edn_szablon->DeleteAllItems();
    //load_tree(edn_szablon, NewNameTemplate);
    //ctrl_szablon->SetValue(NewNameTemplate);
    //XSelectSzablon( GetTemplateFromTree(edn_szablon) );

    LoadTree();    
}


void intaktDlg::SaveApplicationSection()
{
  wxArrayString aapp;
  unsigned int nlist;

  nlist=app_list->GetCount();
  aapp=app_list->GetStrings();
  for ( unsigned int i=0; i<nlist; i++ )
  {
    if (aapp[i]=="") aapp[i]+=_T(" ");
    aapp[i]+=(_T("=")+real_app[i]);
  };
  WriteSectionToFile(working_dir+"\\"+baza,"application",aapp);
}

// Usuniêcie wpisu w sekcji [application]
void intaktDlg::AppMenuDelete(wxCommandEvent& event)
{
  unsigned int nsel=app_list->GetSelection();
  if (wxMessageBox(_T("Na pewno chcesz usun¹æ ustawienie?"), _T("PotwierdŸ"),wxYES_NO | wxICON_QUESTION, this)==wxYES)
  {
   app_list->Delete(nsel);
   app_list->SetSelection(-1);
   app_ctrl->SetValue(wxEmptyString);
      
   real_app.RemoveAt(nsel);
   SaveApplicationSection();
  };
}

/*
 * Modyfikacja wpisu w aplikacjach
 * Dodawanie, modyfikacja
 */
void intaktDlg::AppMenuEdit(wxCommandEvent& event)
{
  unsigned int nsel=app_list->GetSelection();
  if (event.GetId()==10201)
  {
     DialogEditApp *editapp = new DialogEditApp(this,1,wxT("Edycja ustawienia"));
     editapp->SetExtraStyle(wxDIALOG_EX_CONTEXTHELP );
     editapp->SetValueApp(app_list->GetString(nsel));
     editapp->SetValuePath(real_app[nsel]);
     if (editapp->ShowModal()==wxID_OK)
     {
       app_list->SetString(nsel,editapp->GetValueApp());
       app_ctrl->SetValue(editapp->GetValueApp());
       real_app[nsel]=editapp->GetValuePath();  
       SaveApplicationSection();     
     };
  } else if (event.GetId()==10202)
  {
     DialogEditApp *editapp = new DialogEditApp(this,1,wxT("Dodanie nowego ustawienia"));
     editapp->SetExtraStyle(wxDIALOG_EX_CONTEXTHELP );
     if (editapp->ShowModal()==wxID_OK)
     {
       app_list->Insert(editapp->GetValueApp(),nsel+1);
       app_list->SetSelection(nsel+1);
       app_ctrl->SetValue(editapp->GetValueApp());
       real_app.Insert(editapp->GetValuePath(),nsel+1);
       SaveApplicationSection();     
     };    
  };
}

void intaktDlg::SaveVersionSection()
{
  wxArrayString aver;
  unsigned int nlist;

  nlist=ver_list->GetCount();
  aver=ver_list->GetStrings();
  for ( unsigned int i=0; i<nlist; i++ )
  {
    aver[i]+=(_T("=")+real_path[i]);
  };
  WriteSectionToFile(working_dir+"\\"+baza,"version",aver);
}

// Edycja w liœcie wersji
void intaktDlg::VerMenuEdit(wxCommandEvent& event)
{
  unsigned int nsel=ver_list->GetSelection();
  if (event.GetId()==10301)
  {
     DialogEditApp *editapp = new DialogEditApp(this,2,wxT("Edycja ustawienia"));
     editapp->SetExtraStyle(wxDIALOG_EX_CONTEXTHELP );
     editapp->SetValueApp(ver_list->GetString(nsel));
     editapp->SetValuePath(real_path[nsel]);
     if (editapp->ShowModal()==wxID_OK)
     {
       ver_list->SetString(nsel,editapp->GetValueApp());
       ver_ctrl->SetValue(editapp->GetValueApp());
       real_path[nsel]=editapp->GetValuePath();  
       WxEdit5->SetValue(MakeMacro(real_path[nsel]));       
       SaveVersionSection();     
     } else {
       ver_list->SetSelection(-1);
       ver_ctrl->SetValue(wxEmptyString);
       WxEdit5->SetValue(wxEmptyString);      
      };
  } else if (event.GetId()==10302)
  {
     DialogEditApp *editapp = new DialogEditApp(this,2,wxT("Dodanie nowego ustawienia"));
     editapp->SetExtraStyle(wxDIALOG_EX_CONTEXTHELP );
     if (editapp->ShowModal()==wxID_OK)
     {
       ver_list->Insert(editapp->GetValueApp(),nsel+1);
       ver_list->SetSelection(nsel+1);
       ver_ctrl->SetValue(editapp->GetValueApp());
       real_path.Insert(editapp->GetValuePath(),nsel+1);
       SaveVersionSection();     
     };    
  };
}

// Usuniêcie z listy wersji
void intaktDlg::VerMenuDelete(wxCommandEvent& event)
{
  unsigned int nsel=ver_list->GetSelection();
  if (wxMessageBox(_T("Na pewno chcesz usun¹æ ustawienie?"), _T("PotwierdŸ"),wxYES_NO | wxICON_QUESTION, this)==wxYES)
  {
   ver_list->Delete(nsel);
   ver_list->SetSelection(-1);
   ver_ctrl->SetValue(wxEmptyString);
   real_path.RemoveAt(nsel);
   SaveVersionSection();
  };
}

void intaktDlg::TreeRunClick(wxCommandEvent& event)
{
 wxFont fnt=TreeRun->GetFont();
 fnt.SetUnderlined(TreeRun->GetValue());
 TreeRun->SetFont(fnt);
}

// Realizuje przesuwanie opcji Drag&Drop w liœcie ListBoxPopup
void intaktDlg::ItemMove(wxCommandEvent& event)
{
	long int ito;
	long int ifrom=event.GetExtraLong();
	wxString value_move;
	
	//przesuwanie na liœcie aplikacji
	if (event.GetId()==10220)
	{		
		ito=app_list->GetSelection();
		
		if (ito<0 ||ifrom==ito || ifrom==0 && ito<=0) return;
		
		value_move=app_list->GetString(ifrom);
		app_list->Delete(ifrom);
		app_list->Insert(value_move,ito+(ifrom>ito ? 0 : -1 ));
		//app_list->SetSelection(ito+(ifrom<ito ? -1 : 1 ));
		
		//modyfikacja tablic i zapis do inika
	    value_move=real_app[ifrom];
	    real_app.RemoveAt(ifrom);
		real_app.Insert(value_move,ito+(ifrom>ito ? 0 : -1 ));
        SaveApplicationSection(); 
        
        app_list->Refresh();
	};
	
	//przesuwanie na liœcie wersji
	if (event.GetId()==10320)
	{
		ito=ver_list->GetSelection();
		//wxMessageBox(wxString::Format(" to= %i",ito));
		if (ito<0 || ifrom==ito || ifrom==0 && ito<=0) return;
		value_move=ver_list->GetString(ifrom);
		ver_list->Delete(ifrom);
		ver_list->Insert(value_move,ito+(ifrom>ito ? 0 : -1 ));
		
		//modyfikacja tablic i zapis do inika
	    value_move=real_path[ifrom];
	    real_path.RemoveAt(ifrom);
	    real_path.Insert(value_move,ito+(ifrom>ito ? 0 : -1 ));
        SaveVersionSection();
		
        ver_list->Refresh();
	};	

}

void intaktDlg::SetDefaultMySpellOptions(MySpellInterface *m_pMySpellInterface)
{
    SpellCheckEngineOption DictionaryPathOption(_T("dictionary-path"), _T("Dictionary Path"), wxFileName::GetCwd(), SpellCheckEngineOption::DIR);
    // If you need to set an option but don't want the option to be user editable, use SetShowOption as follows:
    //  DictionaryPathOption.SetShowOption(false);
    m_pMySpellInterface->AddOptionToMap(DictionaryPathOption);
    SpellCheckEngineOption LanguageOption(_T("language"), _T("Language"), _("Polish (Poland)"), SpellCheckEngineOption::STRING);
    LanguageOption.SetDependency(_T("dictionary-path"));
    m_pMySpellInterface->AddOptionToMap(LanguageOption);

    // Add custom dictionary entries
    m_pMySpellInterface->AddCustomMySpellDictionary(_("Polish (Poland)"), _("pl_PL"));
    m_pMySpellInterface->ApplyOptions();

    // Set the personal dictionary file
    m_pMySpellInterface->OpenPersonalDictionary(working_dir+wxFILE_SEP_PATH+_("personaldictionary.dic"));
}

void intaktDlg::cr_clearClick(wxCommandEvent& event)
{
    //testowo
    //wxSpellCheckEngineInterface* pSpellChecker = new MySpellInterface(new MySpellingDialog(NULL));
    //SetDefaultMySpellOptions((MySpellInterface*)pSpellChecker);  
    //pSpellChecker->InitializeSpellCheckEngine();
    wxString strCorrectedText = pSpellChecker->CheckSpelling(WxEdit2->GetValue());
    WxEdit2->SetValue(strCorrectedText);

    strCorrectedText = pSpellChecker->CheckSpelling(WxMemo1->GetValue());
    WxMemo1->SetValue(strCorrectedText);   
}

//funkcja jako wskaŸnik jest przekazywana do EnumChildWindows, która wywo³uje j¹
//dla ka¿dej znalezionej kontrolki.
BOOL CALLBACK enum_func(HWND hwnd, LPARAM lParam) {
  char class_name[30];

  GetClassName(hwnd, class_name, sizeof(class_name));

  //³apiemy handle Kontrolki do podawania has³a (tylko instance1 czyli 'has³o')
  if (strcmp(class_name, "TEdit") == 0) {
    ++TEditNN;
    if (TEditNN==1) *(HWND*)lParam = hwnd;
    if (TEditNN==2) h_login = hwnd ;
  }
  
  //³apiemy handle Button (tylko instance2 czyli button OK)
  if(0==strcmp(class_name,"TButton"))
  {
     if (++TButtonNN==2) {
         hButton = hwnd ;
     };
  };
  return TRUE;
};

/**
 * void AutoRunApp(wxCommandEvent& event)
 *
 * Automatyczne wyszukanie okna logowania, 
 * wyszukanie kontrolek, wpisanie has³a i wciœniêcia OK
 * 
 * Login jest wrzucany do rejestru - tak jak robi¹ to aplikacje  hospitality. 
 * Metoda jest wywo³ywana przez w¹tek i podejmuje sprawdzanie istnienia okna przez 25 s. 
 * Jeœli okno zostanie znalezione przez kolejne 10 s, podejmujemy wyszukiwanie kontrolek.
 * Powodem powy¿szego jest to, ze okno mo¿e byæ wykryte ale jeszcze siê nie pojawi³o, 
 * co powoduje niemo¿noœæ u¿ycia jego kontrolek.
 */
void intaktDlg::AutoRunApp(wxCommandEvent& event)
{
 //unsigned ntime=10000;
 //unsigned ninterv=200;
 HWND hWnd=0;
 
// while ( (!(hWnd = ::FindWindow(NULL,"Logowanie"))) && ntime>0 ) {
//     wxMilliSleep(ninterv);
//     ntime-=ninterv;
// };
//
// if (hWnd) {
// WxMemo1->AppendText("szukam");
 if ((hWnd = ::FindWindow("TLogOnForm",NULL))) { //"Logowanie"
     HWND h_edit=0;
     //potrzebne, bo okno zostaje z³apane ale kiedy jest niewidoczne (a czasem doœæ d³ugo trwa ³adowanie)
     //to komunikaty s³ane do okna nie s¹ przez nie odbierane
//     ntime=10000;
//     while (ntime>0 && (!h_edit) ) {
          TEditNN=0;
          TButtonNN=0;
          EnumChildWindows(hWnd, enum_func, (LPARAM)&h_edit);
//          wxMilliSleep(ninterv);
//          ntime-=ninterv;
//     };
//     if (ntime>0) {
//     WxMemo1->AppendText("->loguje");
     if (h_edit) {            
        if (!(ed_login->GetValue().empty())) {
         SendMessage(h_login, WM_SETTEXT, (WPARAM)NULL, (LPARAM)ed_login->GetValue().c_str() ); // (LPARAM)WxEdit1->GetValue().c_str());
         SendMessage(h_edit,  WM_SETTEXT, (WPARAM)NULL, (LPARAM)ed_haslo->GetValue().c_str() ); // (LPARAM)WxEdit1->GetValue().c_str());
         SendMessage(hButton,BM_CLICK,0,0);
        };
//        WxMemo1->AppendText("\n");        
        AutoRunAppStart->Delete();
//        wxMessageBox("wywo³anie usuniecia watku");
     };
//  wxMessageBox("wywo³anie usuniecia watku");
//  AutoRunAppStart->Delete();
};
// WxMemo1->AppendText("\n");

}

// Odpalenie w¹tku autologowania
bool intaktDlg::ThreadAutoRunAppStart(bool ncontrol)
{ 
//    if (!AutoRunAppStart) {
        AutoRunAppStart = new ThreadAutoRunApp(this);
        if (AutoRunAppStart->Create()!=wxTHREAD_NO_ERROR) {
            wxLogMessage(wxT("B³¹d podczas tworzenia w¹tku AutoLogowania!"));
            return false;            
    	};
//    };

    //podpiêcie zdarzenia automatycznego logowania
    //zdarzenie jest odpalane przez w¹tek ThreadAutoRunApp 
    //Connect(4000,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(intaktDlg::AutoRunApp) );
    //wxMessageBox(wxT("Start w¹tku"));
    AutoRunAppStart->Run();
  
    return true;
};

// Zatrzymanie w¹tku i odpiêcie procedury zdarzeniowej
bool intaktDlg::ThreadAutoRunAppStop()
{
    //AutoRunAppStart->Delete();
    return true;
}

void intaktDlg::btn_cfgClick(wxCommandEvent& event)
{
	IntaktConf *icfg = new IntaktConf(this);
	icfg->ShowModal();
	delete icfg;
}

void intaktDlg::UpdateFromFTP(wxCommandEvent& event)
{
   wxBusyInfo wait(wxT("Aktualizacja oprogramowania..."));

   wxString vsiec;
   unsigned int i;
   
   wxFTP ftp;

   ftp.SetUser("update");
   ftp.SetPassword("UpddfhgfWt$6$%^");
   if ( !ftp.Connect(wxT("ftp.client.pl")) ) {
        wxLogError(wxT("Nie mogê po³¹czyæ siê z serwerem"));
        return;
   };
   
    ftp.Pwd();
    ftp.ChDir("/update/intakt");
    wxInputStream *in = ftp.GetInputStream("version.txt");
    if (!in) {
        wxLogError(wxT("Nie mogê pobraæ aktualizacji"));
    } else {
        size_t size = 50; 
        char *data = new char[size];
           
        if (!in->Read(data, size)) {
            wxLogError(wxT("B³¹d odczytu ze strumienia"));
        } else {
          //oczekujemy iluœ tam znaków zakoñczonych przecinkiem,
          //pod uwagê bierzemy numery i kropkê
          i=0;
          vsiec="";
          for (;i<in->LastRead();i++)
          {
            if (data[i]==',') {
                break;
            } else if ((data[i]>='0' && data[i]<='9') || data[i]=='.')
                       vsiec.Append(data[i]);
          };
          if (vsiec > wxString::Format("%i.%i.%i.%i",VER_MAJOR,VER_MINOR,VER_RELEASE,VER_BUILD) ) 
          {
             if (wxMessageBox(wxT("Dostêpna jest nowsza wersja oprogramowania\n"\
                                  "\nZaktualizowaæ program?"),
                              wxT("Aktualizacja"),
                              wxICON_INFORMATION | wxYES_NO )==wxYES) {
                wxDELETEA(data);
                delete in;
                size_t nread;
                size=2048;
                data = new char[size];
                wxInputStream *in = ftp.GetInputStream("intakt.exe");
                wxFileOutputStream *out = new wxFileOutputStream(".\\$$$intakt.exe");

                if (!in) {
                    wxLogError(wxT("Nie mogê pobraæ nowej wersji"));
                } else {
                    wxBusyInfo wait2(wxT("Aktualizacja oprogramowania..."));                    
                    do {
                      in->Read(data,size);
                      nread=in->LastRead();
                      if (nread>0)
                          out->Write(data,nread);                      
                    } while (nread>0);

                    restart=true;
                    wxDELETEA(data);
                    wxDELETE(out);
                    if (ftp.IsConnected()) 
                        ftp.Destroy();
                    Destroy();
                };                
             }
          } else
             wxMessageBox(wxT("Posiadasz ju¿ aktualn¹ wersjê oprogramowania"),wxT("Aktualizacja"),wxICON_INFORMATION);
        }
        wxDELETEA(data);
    };
    if (ftp.IsConnected()) ftp.Destroy();
}

// input polling
void intaktDlg::OnIdle(wxIdleEvent& event)
{
    if ( ctrl_cmd && ctrl_cmd->HasInput() )
    {
        event.RequestMore();
    }
}

void intaktDlg::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    wxWakeUpIdle();
};

/*
 * Pobranie rekordów w postaci string-u
 * rekordy oddzielone znakiem \n a kolumny w rekordzie znakiem ;
 */
unsigned int intaktDlg::GetLogicalName(wxString stm, wxArrayString &a)
{
  unsigned int nrow=0;
  wxString ConnectionString;
  ConnectionString="Driver={SQL Server}"\
                   ";Server="+srv_name->GetValue()+\
                   ";Database=master"\
                   ";Uid="+WxEdit6->GetValue()+\
                   ";Pwd="+WxEdit7->GetValue();
  wxDbConnectInf DbConnectInf;
  DbConnectInf.AllocHenv();
  wxDb *db = new wxDb(DbConnectInf.GetHenv());
  db->Open(ConnectionString);
  if (db->IsOpen())
  {
      wxChar nazwa[255];
      SDWORD cd;
      db->ExecSql(stm);
      while (db->GetNext())
      {
        nrow++;
        db->GetData(1,SQL_C_WXCHAR,&nazwa,255,&cd);  //wxString(nazwa);
        a.Add(static_cast<wxString>(nazwa));
        db->GetData(2,SQL_C_WXCHAR,&nazwa,255,&cd);
        a.Add(static_cast<wxString>(nazwa));
      };
    db->CommitTrans();
    db->Close();
  };
  wxDELETE(db);
  return nrow;
};

void intaktDlg::btDeleteDatabaseClick(wxCommandEvent& event)
{
  int answer = wxMessageBox(wxT("Czy na pewno chcesz usun¹æ bazê danych z serwera?"),
                            wxT("Usuniêcie bazy danych"),wxICON_QUESTION | wxYES_NO);
  if (answer == wxYES)
  {
    MyProcess *cmd_del;
    wxString b1("dbdel.bat");
    wxTextFile file(b1);
    file.Create(b1);
    if (file.Exists())
    {
        file.AddLine("@echo off\r\n"\
                     "echo Czekaj, usuwanie bazy danych...\r\n" \
                     "osql -U "+WxEdit6->GetValue()+\
                         " -P "+WxEdit7->GetValue()+\
                         " -S "+srv_name->GetValue()+\
                         " -Q \"exec sp_dbremove '"+db_name->GetValue()+"'", \
                     wxTextFileType_Dos);
                             
        file.Write(wxTextFileType_Dos);
        file.Close();

        cmd_del = new MyProcess(this,file.GetName(),wxEmptyString);
        ctrl_cmd=cmd_del;
        WxMemo1->AppendText(wxT("\n"));

        ::wxExecute(file.GetName(),wxEXEC_ASYNC,cmd_del);

    };
  };
}

void intaktDlg::btExecSQL(wxCommandEvent& event)
{
    wxString csql=WxMemo1->GetValue();
    OSQLExecuteFile(csql);
}

bool intaktDlg::OSQLExecuteFile(wxString& stm,int sync)
{
    wxString b1 = wxFileName::CreateTempFileName("$$")+wxT(".bat");
    wxString fsql = wxFileName::CreateTempFileName("$$")+wxT(".sql");
    wxTextFile file(b1);
    wxTextFile filesql(fsql);
    file.Create(b1);
    filesql.Create(fsql);
    if (file.Exists() && filesql.Exists())
    {
       filesql.AddLine(stm);
       filesql.Write(wxTextFileType_Dos);
       filesql.Close();
       
       file.AddLine("@echo off\n"\
                    "osql"+GetConnectionOSQL(false)+" -w 2000 -d \""+db_name->GetValue()+"\" -i \""+fsql+"\"",wxTextFileType_Dos);
       file.Write(wxTextFileType_Dos);
       file.Close();

	   MyProcess *xcmd_del = new MyProcess(this,file.GetName(),wxEmptyString);
       ctrl_cmd=xcmd_del;

      ::wxExecute(file.GetName(),sync,xcmd_del);

	   };
  return true;
};

wxString intaktDlg::GetConnectionOSQL(bool sspi)
{
  wxString ConnectionString;

  if (sspi) {
      ConnectionString=wxT(" -E -S ")+srv_name->GetValue()+" ";
    }
  else {
      ConnectionString=" -U "+WxEdit6->GetValue()+\
                       " -P "+WxEdit7->GetValue()+\
                       " -S "+srv_name->GetValue();
    };
	return ConnectionString;
}


void intaktDlg::dn_nameLKM(wxMouseEvent &event)
{
  if (db_name->GetValue().Contains("%"))
  {
      unsigned int nrow=0;
      wxString ConnectionString="Driver={SQL Server}"\
                               ";Server="+srv_name->GetValue()+\
                               ";Database=master"\
                               ";Uid="+WxEdit6->GetValue()+\
                               ";Pwd="+WxEdit7->GetValue();
      wxDbConnectInf DbConnectInf;
      DbConnectInf.AllocHenv();
      wxDb *db = new wxDb(DbConnectInf.GetHenv());
      db->Open(ConnectionString);
      if (db->IsOpen())
      {
          wxChar nazwa[255];
          SDWORD cd;
          db->ExecSql("select name "\
                      "from master.dbo.sysdatabases "\
                      "where name like '"+db_name->GetValue()+"' and name not in ('master','model','msdb','tempdb')"\
                      "order by name");
          db_name->Clear();                      
          while (db->GetNext())
          {
            nrow++;
            db->GetData(1,SQL_C_WXCHAR,&nazwa,255,&cd);  //wxString(nazwa);
            db_name->Append(static_cast<wxString>(nazwa));
          };
        db->CommitTrans();
        db->Close();
      };
      wxDELETE(db);
  };   
  
  event.Skip();
};

void intaktDlg::VerShowContextMenu()
{
 wxMenu mnu;
 mnu.Append(7001,"Odbiór","");
 mnu.Append(7006,"Odbiór z komentarzem",""); 
 mnu.Append(7002,"Cofniêcie","");
 mnu.Append(7005,"Cofniêcie + connection + komentarz","");
 mnu.Append(7003,"Konwersja bazy","");
 mnu.AppendSeparator();
 mnu.Append(7004,"Cofniêta z b³êdem DPO","");
 
 if ( PopupMenu(&mnu, WxToolbarPanel->GetPosition().x,WxToolbarPanel->GetPosition().y+(WxToolbarPanel->GetSize().GetHeight())) )
 {
 };
};

void intaktDlg::VerContextMenu(wxMouseEvent& event)
{
 VerShowContextMenu();
 event.Skip();
};

void intaktDlg::ContextMenuMacro()
{
    macro_mnu = new wxMenu(wxT(""));
    long int nrow=6000;
    std::map<wxString,wxString>::iterator it;
    for( it = other_param.begin(); it != other_param.end(); ++it )
       if ((it->first)[0]=='%')
        macro_mnu->Append(++nrow, it->first+" = "+it->second.Mid(0,40), it->second, wxITEM_NORMAL);

    if ( nrow>6000 && PopupMenu(macro_mnu, WxToolbarPanel->GetPosition().x,WxToolbarPanel->GetPosition().y+(WxToolbarPanel->GetSize().GetHeight())) )
    {
    };

    wxDELETE(macro_mnu);
};

void intaktDlg::pg_CRPageChanged(wxNotebookEvent& event)
{
 wxPanel *tmp_pg;
 tmp_pg=(wxPanel*)pg_CR->GetPage(event.GetSelection());
 if (pg_title.Count()>0 &
    (static_cast<unsigned int>(event.GetSelection()+1))<=pg_title.Count())
    {
     WxEdit2->SetValue(pg_title[event.GetSelection()]);
     WxMemo1->SetValue(pg_source[event.GetSelection()]);
     TemplateLoadControl(pg_tpl[event.GetSelection()]);
     pg_CR->SetPageText(pg_CR->GetSelection(),app_ctrl->GetValue());
    };
 WxMemo1->Reparent(tmp_pg);
 WxMemo1->SetSize(0,0,tmp_pg->GetSize().GetWidth()-1,tmp_pg->GetSize().GetHeight()-2);
}

/*
 * Opuszczamy zak³adkê, wiêc ³adujemy jej zawartoœæ do tabeli
 */
void intaktDlg::pg_CRPageChanging(wxNotebookEvent& event)
{
 static bool first=true;  //musi byæ bo przy pierwszym uruchomieniu w WxMemo1 jeszcze nic nie ma
 
 if (!first & pg_title.Count()>0 &
    (static_cast<unsigned int>(event.GetSelection()+1))<=pg_title.Count())
    {
     pg_title[event.GetSelection()]=WxEdit2->GetValue();
     pg_source[event.GetSelection()]=WxMemo1->GetValue();
     pg_tpl[event.GetSelection()]=TemplateFromControl();
     pg_CR->SetPageText(pg_CR->GetSelection(),app_ctrl->GetValue());
    } else {
      first=false;
    };
};

void intaktDlg::SaveAllPages()
{
  wxArrayString tmp;
  wxString src;
   
  //odœwie¿enie tablicy o aktualn¹ zak³adkê
  pg_title[pg_CR->GetSelection()]=WxEdit2->GetValue();
  pg_source[pg_CR->GetSelection()]=WxMemo1->GetValue();
  pg_tpl[pg_CR->GetSelection()]=TemplateFromControl();
   
  for ( unsigned int i=0; i<pg_title.Count(); i++ )
  {
    src=pg_source[i];
    src.Replace("\n","\\n",true);
    tmp.Add(wxString::Format("title_0%i=",i+1)+pg_title[i]);
    tmp.Add(wxString::Format("source_0%i=",i+1)+src);
    tmp.Add(wxString::Format("tpl_0%i=",i+1)+pg_tpl[i]);
  };
  WriteSectionToFile(working_dir+"\\"+baza,"page",tmp);
  
};

void intaktDlg::LoadAllPages()
{
    wxArrayString atmp;
    
    if (!xladuj_bazy(atmp,baza,section[SEC_PAGE]))
    {
    } else {
      for (size_t q=0; q<atmp.Count();q++)
      {
       //pobranie tytu³u
       wxStringTokenizer s(atmp.Item(q), wxT("="));
       s.GetNextToken(); pg_title.Add(s.GetNextToken());

       //pobranie zawartoœci zak³adki
       wxStringTokenizer sm(atmp.Item(++q), wxT("="));
       sm.GetNextToken();
       wxString src=sm.GetNextToken();
       src.Replace("\\n","\n",true);
       pg_source.Add(src);
       
       //pobranie szablonu
       wxStringTokenizer st(atmp.Item(++q), wxT("="));
       st.GetNextToken();
       src=st.GetNextToken();
       pg_tpl.Add(src);
      };
      //za³adowanie kontrolek z pierwszej strony
      WxEdit2->SetValue(pg_title[0]);
      WxMemo1->SetValue(pg_source[0]);
      TemplateLoadControl(pg_tpl[0]);
    };
};

void intaktDlg::PutRegUser()
{
wxRegKey *pRegKey;
wxString ckey,capp;

capp=(app_ctrl->GetValue()).Trim();
ckey="HKEY_LOCAL_MACHINE\\Software\\LASER\\Positive\\";

if (capp.Contains("Hotele"))   ckey+=capp;
if (capp.Contains("Manager"))  ckey+="Admin";
if (capp.Contains("Catering")) ckey+=capp;
if (capp.Contains("Centrala")) {ckey.Remove(ckey.Length()-10);ckey+=capp.Upper();};
ckey+="\\Host";

pRegKey = new wxRegKey(ckey);

    if( pRegKey->Exists() )
    {
       pRegKey->SetValue("OstatnioZalogowany",ed_login->GetValue());
       if (wxTheClipboard->Open())
       {
           wxTheClipboard->SetData(new wxTextDataObject(ed_haslo->GetValue()));
           wxTheClipboard->Close();
       };
    };
}

/*
 * £adowanie do kontrolek ustawieñ z podanego
 * Connection String
 */
void intaktDlg::LoadFromConnectionString(wxString cConnection)
{
    wxArrayString ctok=::wxStringTokenize(cConnection,wxT(";"));
    for (size_t n=0;n<ctok.Count();n++)
    {
        wxStringTokenizer cpar(ctok[n], wxT("="));
        wxString key=cpar.GetNextToken().Trim(false).Trim().Lower();
                 key.Replace(wxT(" "),wxT(""));
        wxString val=cpar.GetNextToken().Trim(false).Trim().Lower();
        if ( key.Contains(wxT("password")) )
             WxEdit7->SetValue(val);
        else if ( key.Contains(wxT("userid")) )
             WxEdit6->SetValue(val);
        else if ( key.Contains(wxT("initialcatalog")) )
             db_name->SetValue(val);
        else if ( key.Contains(wxT("datasource")) )
             srv_name->SetValue(val);
    };
}

void intaktDlg::SelectApplication(wxCommandEvent& event)
{
 pg_CR->SetPageText(pg_CR->GetSelection(),app_list->GetString(event.GetInt()));
};


void intaktDlg::LoadTree()
{
    arrayStringFor_WxChoice1.Clear();
    if (xladuj_bazy(arrayStringFor_WxChoice1,baza,section[SEC_SZABLONY]))
    {    
      wxString st,sv;
      arrayStringFor_WxChoice1.Sort(true);
      for (size_t q=0; q<arrayStringFor_WxChoice1.Count();q++)
      {
        //nazwa szablonu oddzielona znakiem = od jego definicji
        wxStringTokenizer s(arrayStringFor_WxChoice1.Item(q), wxT("="));

        //pobranie jedynie nazwy szablonu, bez spacji wiod¹cych i koñcowych
        st=s.GetNextToken().Trim(false).Trim();
        
        //jeœli szablon nie jest za komentowany to ³adujemy do kontrolek
        if (st[0]!=';')
        {
          load_tree(edn_szablon,st);    //dodatkowo za³adowanie do drzewa, poziomy oddzielane dwukropkiem
          TemplateName.Add(st);         //nazwa szablonu
          TemplateDef.Add(s.GetNextToken().Trim(false).Trim());  //definicja szablonu      
         };
      };    
    };
};
