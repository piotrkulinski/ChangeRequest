//---------------------------------------------------------------------------
//
// Name:        intaktDlg.h
// Author:      pkulinski
// Created:     2008-06-23 16:07:36
// Description: intaktDlg class declaration
//
//---------------------------------------------------------------------------

#ifndef __INTAKTDLG_h__
#define __INTAKTDLG_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/dialog.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/dirdlg.h>
#include <wx/fontdlg.h>
#include <wx/panel.h>
#include <wx/notebook.h>
#include <wx/tglbtn.h>
#include <wx/statline.h>
#include <wx/statbox.h>
#include <wx/datectrl.h>
#include <wx/dateevt.h>
#include <wx/hyperlink.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
////Header Include End
#include <wx/richtext/richtextctrl.h>

#include <wx/timer.h>

#include "wklejka.h"
#include "intaktTray.h"
#include "md5file.h"

#include <wx/combo.h>
#include <wx/bmpcbox.h>

#include "wx/odcombo.h"
#include <wx/imaglist.h>

#include <wx/busyinfo.h>
#include <wx/protocol/ftp.h>   //update programu w oparciu o FTPa
#include <wx/stream.h>
#include <wx/wfstream.h>

#include <wx/utils.h>

#include "TreeCtrlComboPopup.h"
#include "MyComboBox.h"
#include "ListBoxPopup.h"

#include "MySpellCheckDialog.h"
#include "XmlSpellCheckDialog.h"
#include "XmlPersonalDictionaryDialog.h"
#include "AspellInterface.h"
#include "MySpellInterface.h"
#include "SpellCheckerOptionsDialog.h"

//w¹tek autologowania
#include "ThreadAutoRunApp.h"

//hashmap dla sekcji other
#include <hashmap.h>
#include <map.h>
//using namespace std;
#include <wx/toolbar.h>  //(to allow wxWidgets to select an appropriate toolbar class)
#include <wx/tbarbase.h> //(the base class)

////Dialog Style Start
#undef intaktDlg_STYLE
#define intaktDlg_STYLE wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxDIALOG_EX_CONTEXTHELP | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

class MyProcess ;

   
class intaktDlg : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		intaktDlg(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("intakt"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = intaktDlg_STYLE);
		virtual ~intaktDlg();
		void WxCheckBox1Click(wxCommandEvent& event);
		void WxBitmapButton2Click(wxCommandEvent& event);
		void WxCheckBox2Click(wxCommandEvent& event);
		void WxButton1Click(wxCommandEvent& event);
		void WxBitmapButton1Click(wxCommandEvent& event);
		void WxBitmapButton4Click(wxCommandEvent& event);
		void generujClick(wxCommandEvent& event);
		bool laduj_bazy(wxArrayString&, wxString& );
        bool xladuj_bazy(wxArrayString& arr, wxString& b1, wxString& section);		
		void WxCheckBox3Click(wxCommandEvent& event);
		void WxHyperLinkCtrl1HyperLink(wxHyperlinkEvent& event);
		void WxCheckBox4Click(wxCommandEvent& event);
		void WxCheckBox5Click(wxCommandEvent& event);
		void WxButton2Click(wxHyperlinkEvent& event);
		void schowekClick(wxCommandEvent& event);
		void cr_new(wxCommandEvent& event);
		void laduj_ze_schowka(void);	
		void helpF1(wxKeyEvent& event);
		void LoadAppDefault(void);
		void WxCheckBox1Click1(wxCommandEvent& event);
		// Stworzenie backupu bazy danych, z wykorzystaniem osql
		wxString backup_database(bool kierunek);
		void dbcopyClick(wxCommandEvent& event);
		void WxCheckBox3Click1(wxCommandEvent& event);
		void bedzie_zalacznikClick(wxCommandEvent& event);
		void srv_nameSelected(wxCommandEvent& event );
		void run_manager(wxCommandEvent& event );
		void WxChoice4Selected(wxCommandEvent& event );
		void bWersjaClick(wxCommandEvent& event);
		void bConnectionClick(wxCommandEvent& event);
		void cr_stat_propClick(wxCommandEvent& event);
		void WxButton3Click(wxCommandEvent& event);
		void intaktDlgActivate(wxActivateEvent& event);
		void k2lClick(wxCommandEvent& event);
		void k1pClick(wxCommandEvent& event);
		void k1lClick(wxCommandEvent& event);
		void btn_getposClick(wxCommandEvent& event);
        // polling output of async processes
        void OnTimer(wxTimerEvent& event);
        void OnIdle(wxIdleEvent& event);
    	void PutRegUser(); //wrzucenie do klucza i schowka loginu i has³a
    	
    	//Pe³ne ³adowanie drzewa
    	void LoadTree();
    	
	public:
		//Do not add custom control declarations between 
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxFileDialog *WxOpenFileDialog1;
		wxMenu *WxPopupMenu1;
		wxDirDialog *WyborKatalogu;
		wxFontDialog *WxFontDialog1;
		wxCheckBox *ZalLiczMD5;
		wxStaticText *WxStaticText6;
		wxPanel *WxToolbar2DBase;
		wxPanel *WxToolbar1DBase;
		wxPanel *WxToolbarPanel;
		wxPanel *pg_05;
		wxPanel *pg_04;
		wxPanel *pg_03;
		wxPanel *pg_02;
		wxPanel *pg_01;
		wxNotebook *pg_CR;
		wxBitmapButton *btDeleteDatabase;
		wxButton *btRestore;
		wxButton *btn_cfg;
		wxTextCtrl *ed_haslo;
		wxTextCtrl *ed_login;
		wxStaticText *st_haslo;
		wxStaticText *st_login;
		wxCheckBox *aspell;
		wxStaticText *WxStaticText8;
		wxToggleButton *TreeRun;
		wxBitmapButton *ctx_help;
		wxButton *about;
		wxStaticLine *WxStaticLine7;
		wxCheckBox *r_zapytaj;
		wxStaticLine *WxStaticLine5;
		wxStaticText *tcr_pow;
		wxTextCtrl *cr_powiazane;
		wxBitmapButton *WxBitmapButton6;
		wxBitmapButton *WxBitmapButton5;
		wxBitmapButton *btn_getpos;
		wxBitmapButton *btn_getadm;
		wxBitmapButton *btn_getusr;
		wxBitmapButton *usun;
		wxBitmapButton *Save;
		wxStaticText *st_szablon;
		wxCheckBox *r_critical_dkj;
		wxCheckBox *r_usterka_dkj;
		wxStaticText *WxStaticText4;
		wxCheckBox *isview;
		wxTextCtrl *m5;
		wxStaticText *lm5;
		wxTextCtrl *m4;
		wxStaticText *lm4;
		wxTextCtrl *m3;
		wxStaticText *lm3;
		wxStaticText *lm2;
		wxTextCtrl *m2;
		wxStaticText *lm1;
		wxTextCtrl *m1;
		wxButton *k2l;
		wxButton *k1p;
		wxButton *k1l;
		wxButton *WxButton3;
		wxCheckBox *r_unt;
		wxCheckBox *r_inne;
		wxCheckBox *r_test;
		wxCheckBox *r_migracja;
		wxCheckBox *r_rozwojowe;
		wxCheckBox *r_critical;
		wxCheckBox *r_support;
		wxCheckBox *r_drobna_zmiana;
		wxCheckBox *r_usterka;
		wxStaticBox *WxStaticBox1;
		wxStaticLine *WxStaticLine2;
		wxStaticLine *WxStaticLine1;
		wxComboBox *PosHotel;
		wxComboBox *ComputerName;
		wxButton *WxButton2;
		wxStaticText *WxStaticText2;
		wxStaticText *WxStaticText1;
		wxButton *myrun;
		wxCheckBox *WxCheckBox3;
		wxHyperlinkCtrl *WxHyperLinkCtrl2;
		wxButton *dbcopy;
		wxCheckBox *version;
		wxCheckBox *WxCheckBox1;
		wxCheckBox *cr_stat_propozycja;
		wxCheckBox *cr_stat_quest;
		wxCheckBox *cr_stat_prop;
		wxCheckBox *cr_stat_uzg;
		wxButton *cr_clear;
		wxStaticBox *cr_status;
		wxCheckBox *issound;
		wxButton *schowek;
		wxCheckBox *bedzie_zalacznik;
		wxCheckBox *WxCheckBox5;
		wxCheckBox *WxCheckBox4;
		wxDatePickerCtrl *WxDatePickerCtrl1;
		wxHyperlinkCtrl *WxHyperLinkCtrl1;
		wxComboBox *db_name;
		wxTextCtrl *WxEdit7;
		wxTextCtrl *WxEdit6;
		wxButton *generuj;
		wxButton *WxButton1;
		wxCheckBox *WxCheckBox2;
		wxBitmapButton *WxBitmapButton4;
		wxTextCtrl *WxEdit5;
		wxBitmapButton *WxBitmapButton2;
		wxBitmapButton *WxBitmapButton1;
		wxTextCtrl *WxEdit3;
		wxStaticText *WxStaticText7;
		wxTextCtrl *WxEdit1;
		wxCheckBox *ext_zalaczniki;
		wxStaticText *WxStaticText5;
		wxStaticText *srv_login;
		wxStaticText *WxStaticText3;
		wxTextCtrl *nr_cr;
        ////GUI Control Declaration End
        //wxOwnerDrawnComboBox *ctrl_szablon;
        wxGenericComboCtrl *ctrl_szablon;
        TreeCtrlComboPopup *edn_szablon;
        wxImageList* imageList;
        //serwer sql
        wxMyComboBox *srv_name;
        
        //ikony na buttonie do chowania i pokazywania okna
        wxBitmap bkd;
        wxBitmap bku;
        //sterownie ikonami
        bool show_buffor;
        
        MyTaskBarIcon *m_taskBarIcon;   //Tray
        wxString baza; //nazwa pliku inicjuj¹cego .ini
		
        wxTextCtrl *WxMemo1;
		wxTextCtrl *memo;

        // the idle event wake up timer
        wxTimer m_timerIdleWakeUp;		
        MyProcess *ctrl_cmd;
        wklejkaDlg *ctrl_cmd_win;
        //wxRichTextCtrl *WxMemo1;
		//wxRichTextCtrl *memo;
		
		wxTextCtrl *WxEdit2;
        wxString cfg_sound;
       	wxArrayString arrayStringFor_db_name;
    	wxArrayString arrayStringFor_WxChoice1;
    	wxArrayString real_app;
    	wxArrayString arrayStringFor_srv_name;    	
   		wxArrayString real_pass;
   		wxArrayString real_path;  //œcie¿ka przypisana do wersji
    	wxArrayString TemplateName;
    	wxArrayString TemplateDef;
    	
    	wxArrayString pg_title;
    	wxArrayString pg_source;
    	wxArrayString pg_tpl;

        wxToolBar *tb;
//        wxToolBar *tbDB1,*tbDB2;

        //wskaŸnik na strony z CR-kami
    	wxPanel *pg[10];
         enum 
         {
            SEC_APPLICATION = 0,
            SEC_DBASE = 1,
            SEC_SERVER = 2,
            SEC_VERSION = 3,
            SEC_BACKUP = 4,
            SEC_SOUND = 5,
            SEC_APP=6,            
            SEC_OTHER=7,             
            SEC_SZABLONY=8,
            SEC_MACRODEF=9,
            SEC_PAGE=10,
            SEC_MAX=11
        };
  	    wxString section[SEC_MAX];		
		void btn_getadmClick(wxCommandEvent& event);
		//void SetSqlStm(wxString x); // sets the value of SqlStm
		void SetSqlStm(const wxString &x);
		wxString GetSqlStm(); // returns the value of SqlStm
		void btn_getusrClick(wxCommandEvent& event);
		void isviewClick(wxCommandEvent& event);
		void bCRTextClick(wxCommandEvent& event);
		void nr_crUpdated(wxCommandEvent& event);
		void nr_crMaxLen(wxCommandEvent& event);
		// No description
		void keymemo(wxCommandEvent& event);
		void SaveClick(wxCommandEvent& event);
		void usunClick(wxCommandEvent& event);
		void ed_szablonSelected(wxCommandEvent& event );
		void ed_szablonSelectedTree(wxCommandEvent& event );
		// Pobranie wersji pliku
		wxString GetVersionPositive();
		void WxButton4Click(wxCommandEvent& event);
		// No description
		wxString GetConnectionPositivePOS();
		// Pobranie tabeli konwersji
		void btn_getconversion1(wxCommandEvent& event);
		void WxBitmapButton6Click(wxCommandEvent& event);
		void WxBitmapButton7Click(wxCommandEvent& event);
		void OnMenuInsertMemoCR(wxCommandEvent& event);
		void memo_sizeClick(wxCommandEvent& event);
		void r_zapytajClick(wxCommandEvent& event);
		void aboutClick(wxCommandEvent& event);
		void intaktDlgKeyDown(wxKeyEvent& event);
		void ctx_helpClick(wxCommandEvent& event);
		void intaktDlgLeftDown(wxMouseEvent& event);
		void XSelectSzablon(wxString NazwaSzablonu);

		//Wykonanie polecenie agl z pliku
		bool OSQLExecuteFile(wxString& stm,int sync = wxEXEC_ASYNC);
		wxString GetConnectionOSQL(bool sspi);
		void btExecSQL(wxCommandEvent& event);
		
		//wstawienie, modyfikacja i usuniêcie szablonu w drzewie
		void TreeMenu(wxCommandEvent& event);
		
		//wstawienie, modyfikacja i usuniêcie definicji uruchamianego programu
        wxComboCtrl *app_ctrl;
        wxListBoxPopup *app_list;
		void AppMenu(wxCommandEvent& event);
		
        //£adowanie g³ównych katalogów wersji
        wxComboCtrl *ver_ctrl;
        wxListBoxPopup *ver_list;

        //menu dla mackr %
        wxMenu *macro_mnu;
        
        void SelectApplication(wxCommandEvent& event);
		
	public:
		/**
		 * Usuniêcie wskazanego szablonu z drzewa.
		 * Przy usuwaniu usuniêcie root dla grupy spowoduje przepiêcie 
		 * dzieci do grupy nadrzêdnej
		 */
		void TreeTemplateDelete(wxCommandEvent& event);
		
		// Wstawienie szablonu do drzewa
		void TreeTemplateInsert(wxCommandEvent& event);
		
		// No description
		void TreeTemplateModify(wxCommandEvent& event);
		
		// Wstawienie nowej grupy dla szablonów
		void TreeTemplateInsertGroup(wxCommandEvent& event);
		
		//Wrzucenie szablonu 'tpl' do kontrolek, tpl to ustalony ci¹g z separatorami
		void TemplateLoadControl(wxString tpl);
		
		//kopiowanie wybranego szablonu jako nowy wpis z now¹ nazw¹
        void TreeTemplateCopy(wxCommandEvent& event);
		
        //utworzenie sformatowanego szablonu na podstawie zawartoœci kontrolek
        wxString TemplateFromControl();
		
		// Usuniêcie wpisu w sekcji [application]

		void AppMenuDelete(wxCommandEvent& event);
		void AppMenuEdit(wxCommandEvent& event);
		void SaveApplicationSection();

		// Edycja w liœcie wersji
        void VerMenu(wxCommandEvent& event);		
		void VerMenuEdit(wxCommandEvent& event);		
		void VerMenuDelete(wxCommandEvent& event);
		void MacroMenuShow(wxCommandEvent& event); //zdarzenie dla menu makr %
        void SaveVersionSection();
		void TreeRunClick(wxCommandEvent& event);
		// Realizuje przesuwanie opcji Drag&Drop w liœcie ListBoxPopup
		void ItemMove(wxCommandEvent& event);
		void cr_clearClick(wxCommandEvent& event);

        ThreadAutoRunApp *AutoRunAppStart;              // w¹tek autologowania
		bool ThreadAutoRunAppStart(bool ncontrol);      // Odpalenie w¹tku autologowania
		bool ThreadAutoRunAppStop();		            // Zatrzymanie w¹tku i odpiêcie procedury zdarzeniowej
		/**
		 * Automatyczne wyszukanie okna logowania, 
		 * wyszukanie kontrolek, 
		 * wpisanie has³a i wciœniêcia OK
		 * 
		 * Login jest wrzucany do rejestru - tak jak robi¹ to aplikacje 
		 * hospitality. Metoda jest wywo³ywana przez w¹tek i podejmuje sprawdzanie 
		 * istnienia okna przez 25 s. Jeœli okno zostanie znalezione przez
		 * kolejne 10 s, podejmujemy wyszukiwanie kontrolek.
		 * Powodem powy¿szego jest to, ze okno mo¿e byæ wykryte
		 * ale jeszcze siê nie pojawi³o, co powoduje niemo¿noœæ
		 * u¿ycia jego kontrolek.
		 */
		void AutoRunApp(wxCommandEvent& event);
		void btn_cfgClick(wxCommandEvent& event);
		
		//tu przechowam parametry z sekcji Other
		//poniewa¿ ka¿dy wpis w tej sekcji to nazwany parametr z przypisan¹ wartoœci¹, HshMat jest idealny
		//do tego typu przechowywania
		//WX_DECLARE_STRING_HASH_MAP( wxString, Other );
		typedef std::map<wxString,wxString> Other;
		Other other_param;
		
   		// Katalog aplikacji
		wxString working_dir;
		
		//upadate przez FTP
		bool restart;
		void UpdateFromFTP(wxCommandEvent& event);
		void btRestoreClick(wxCommandEvent& event);

		/**
		 * Pobranie rekordów informacyjnych z backupu
		 */
		unsigned int GetLogicalName(wxString stm, wxArrayString &a);
		void btDeleteDatabaseClick(wxCommandEvent& event);
		void dn_nameLKM(wxMouseEvent &event);
        void bWersjaClickRibbon(wxCommandEvent& event);
		void VerContextMenu(wxMouseEvent& event);
		void ContextMenuMacro(); //klikniêcie w naro¿niku ikony
		void VerShowContextMenu();
		void pg_CRPageChanging(wxNotebookEvent& event);
		void pg_CRPageChanged(wxNotebookEvent& event);

        //za³adowanie wszystkich zak³adek do tablicy
        void LoadAllPages();

		//zapis zak³adek do ini-ka
		void SaveAllPages();
		
		//przechwycenie logowania do pola wxTextCtrl
		wxLogTextCtrl  *logger;
		wxLog *m_logOld;
		/**
		 * £adowanie do kontrolek ustawieñ z podanego
		 * Connection String
		 */
		void LoadFromConnectionString(wxString cConnection);
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_MM1 = 1183,
			ID_MM2 = 1184,
			ID_MM3 = 1185,
			ID_MM4 = 1186,
			ID_MM5 = 1187,
			ID_CONNP = 1167,
			ID_CONN = 1169,
			ID_S = 1170,
			ID_DD = 1189,
			ID_U = 1171,
			ID_P = 1172,
			ID_DAPP = 1173,
			ID_DARC = 1174,
			ID_VER = 1175,
			ID_DT = 1176,
			ID_DT1 = 1177,
			ID_TM = 1178,
			ID_LN = 1179,
			ID_L1 = 1180,
			ID_L2 = 1181,
			ID_L3 = 1182,
			
			ID_ZalLiczMD5 = 1230,
			ID_WXSTATICTEXT6 = 1034,
			ID_WxToolbar2DBase = 1229,
			ID_WxToolbar1DBase = 1228,
			ID_WXPANEL1 = 1227,
			ID_PG_05 = 1226,
			ID_PG_04 = 1225,
			WX_PG_03 = 1224,
			ID_PG_02 = 1223,
			ID_PG_01 = 1222,
			ID_PG_CR = 1221,
			ID_WXbtDeleteDatabase = 1217,
			ID_WXbtRestore = 1216,
			ID_WXBUTTONCFG = 1215,
			ID_EDHASLO = 1214,
			ID_EDLOGIN = 1213,
			ID_WXSTHASLO = 1212,
			ID_WXSTLOGIN = 1211,
			ID_WXASPEL = 1210,
			ID_WXSTATICTEXT85 = 1209,
			ID_WXTOGGLEBUTTON1 = 1208,
			ID_WXBITMAPBUTTON11 = 1207,
			ID_WXBUTTON4 = 1206,
			ID_WXSTATICLINE7 = 1205,
			ID_WXCHECKBOX62 = 1204,
			ID_WXSTATICLINE5 = 1200,
			ID_WXSTATICTEXT80 = 1192,
			ID_WXEDIT20 = 1191,
			ID_WXBITMAPBUTTON60 = 1166,
			ID_WXBITMAPBUTTON53 = 1118,
			ID_BTN_GETPOS = 1117,
			ID_WXBITMAPBUTTON52 = 1116,
			ID_WXBITMAPBUTTON51 = 1115,
			ID_WXBITMAPBUTTON6 = 1113,
			ID_WXBITMAPBUTTON5 = 1112,
			ID_WXSTATICTEXT8 = 1111,
			ID_WXCHECKBOX71 = 1109,
			ID_WXCHECKBOX61 = 1108,
			ID_WXSTATICTEXT4 = 1105,
			ID_WXCHECKBOX6 = 1104,
			ID_M5 = 1102,
			ID_LM5 = 1101,
			ID_M4 = 1099,
			ID_LM4 = 1098,
			ID_M3 = 1096,
			ID_LM3 = 1095,
			ID_LM2 = 1093,
			ID_M2 = 1092,
			ID_LM1 = 1090,
			ID_M1 = 1089,
			ID_K2L = 1085,
			ID_K1P = 1084,
			ID_K1L = 1083,
			ID_WXBUTTON3 = 1082,
			ID_R_UNT = 1081,
			ID_R_INNE = 1080,
			ID_R_TEST = 1079,
			ID_R_MIGRACJA = 1078,
			ID_R_ROZWOJOWE = 1077,
			ID_R_CRITICAL = 1076,
			ID_R_SUPPORT = 1075,
			ID_R_DROBNA_ZMIANA = 1074,
			ID_R_USTERKA = 1073,
			ID_WXSTATICBOX1 = 1072,
			ID_WXSTATICLINE2 = 1071,
			ID_WXSTATICLINE1 = 1067,
			ID_POSHOTEL = 1066,
			ID_COMPUTERNAME = 1065,
			ID_WXBUTTON2 = 1064,
			ID_WXSTATICTEXT2 = 1062,
			ID_WXSTATICTEXT1 = 1060,
			ID_MYRUN = 1059,
			ID_WXCHECKBOX3 = 1058,
			ID_WXHYPERLINKCTRL2 = 1057,
			ID_DBCOPY = 1055,
			ID_VERSION = 1054,
			ID_WXCHECKBOX1 = 1053,
			ID_CR_STAT_PROPOZYCJA = 1052,
			ID_CR_STAT_QUEST = 1051,
			ID_CR_STAT_PROP = 1050,
			ID_CR_STAT_UZG = 1049,
			ID_CR_CLEAR = 1047,
			ID_CR_STATUS = 1048,
			ID_ISSOUND = 1046,
			ID_SCHOWEK = 1044,
			ID_BEDZIE_ZALACZNIK = 1042,
			ID_WXCHECKBOX5 = 1041,
			ID_WXCHECKBOX4 = 1040,
			ID_WXDATEPICKERCTRL1 = 1039,
			ID_WXHYPERLINKCTRL1 = 1038,
			ID_DB_NAME = 1036,
			ID_WXEDIT7 = 1032,
			ID_WXEDIT6 = 1031,
			ID_GENERUJ = 1026,
			ID_WXBUTTON1 = 1025,
			ID_WXCHECKBOX2 = 1023,
			ID_WXBITMAPBUTTON4 = 1022,
			ID_WXEDIT5 = 1020,
			ID_WXBITMAPBUTTON2 = 1018,
			ID_WXBITMAPBUTTON1 = 1017,
			ID_WXEDIT3 = 1014,
			ID_WXSTATICTEXT7 = 1013,
			ID_WXEDIT1 = 1010,
			ID_EXT_ZALACZNIKI = 1008,
			ID_WXSTATICTEXT5 = 1006,
			ID_SRV_LOGIN = 1004,
			ID_WXSTATICTEXT3 = 1003,
			ID_WXEDIT1107 = 1107,
			////GUI Enum Control ID End
			ID_WXEDIT2 = 1009,			
            ID_WXMEMO1 = 1008,
            ID_WXMEMO2 = 1012,
            ID_SRV_NAME = 1300,
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
		enum
		{
            BIT_CONNECTION=1,
            BIT_VERSION=2,
            BIT_BACKUP=4,
            BIT_TITLE=8,
            BIT_CR=16,
            BIT_ZALACZNIK=32,
            BIT_SHOWWIN=64,
            BIT_RODZAJ=128,
            BIT_CRPOWIAZANE=256
        };

        enum //toolbar dla memo
        {
            TBM_EXECSQL=20091,
            TBM_HELP=20090,
            TBM_ASPELL=20089,
            TBM_NEWCR=20081,
            TBM_PASTE=20082,
            TBM_USECR=20083,
            TBM_SHOW =20084,
            TBM_CONN =20085,
            TBM_MACRO=20086,
            TBM_MACROUSR =20087,
            TBM_BUFF =20088
        };
	
	private:
		void OnClose(wxCloseEvent& event);
		void CreateGUIControls();
		// Twórca Programu
		wxString mcopyright;
		wxString mcopyright_nopl; //bez polskich
		// No description
		unsigned int mask;
		// Zapytanie do bazy danych
		wxString SqlStm;
		// No description
		wxString ini_macro[5];
		// Zmienia makra w podanym ³añcuchu i zwraca zmodyfikowany ³añcuch
		wxString MakeMacro(wxString& strcompile);
		// Pobranie sformatowanego ADOString ConnectionString
		wxString GetConnectionString();
		/**
		 * Po³¹czenie do bazy danych MDB
		 * Zapis/Modyfikacja utworzonej CR
		 */
		int dbSave(unsigned int);
		wxSpellCheckEngineInterface* pSpellChecker;
		void SetDefaultMySpellOptions(MySpellInterface *m_pMySpellInterface);
		
		
//	protected:

};

#endif
