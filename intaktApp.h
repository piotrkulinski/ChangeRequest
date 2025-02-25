//---------------------------------------------------------------------------
//
// Name:        intaktApp.h
// Author:      pkulinski
// Created:     2008-06-23 16:07:36
// Description: 
//
//---------------------------------------------------------------------------

#ifndef __INTAKTDLGApp_h__
#define __INTAKTDLGApp_h__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#else
	#include <wx/wxprec.h>
#endif

//na potrzeby pomocy kontekstowej
#include "wx/artprov.h"
#include "wx/cshelp.h"
#include <wx/defs.h>

class intaktDlgApp : public wxApp
{
	public:
		bool OnInit();
		int OnExit();
		
        //---------------------------------------------------------------------------
        bool RunTest()
        {
        CreateMutex(NULL, TRUE, "UNIQUE_NAME");
            if (GetLastError() == ERROR_ALREADY_EXISTS) 
                return true;
            else
                return false;
        }
        //---------------------------------------------------------------------------		
    private:
        bool restart;
        
};

#endif
