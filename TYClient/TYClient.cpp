// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>

#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>

#include <WebReq.hpp>
#include <DSIntf.hpp>
// #include "Service.h"
// ---------------------------------------------------------------------------
#pragma link "Web.WebReq.obj"
#pragma link "IndySystem.lib"
#pragma link "IndyCore.lib"
#pragma link "IndyProtocols.lib"
#pragma link "IdHTTPWebBrokerBridge.obj"
// ---CCRUN的贴子上得知,WebService服务程序,不带包编译后,加以下二个lib
#pragma link "soaprtl.lib"
#pragma link "crypt32.lib"

#pragma link "Midas.lib"
#pragma link "dsnap.lib"


USEFORM("SOAP\WebService\WebModuleUnit1.cpp", WebModule1); /* TWebModule: File Type */
USEFORM("FormLogin.cpp", FrmLogin);
USEFORM("FormMain.cpp", FrmMain);
USEFORM("FormStart.cpp", FrmStart);
//---------------------------------------------------------------------------
extern PACKAGE TComponentClass WebModuleClass;
extern "C" __stdcall DllGetDataSnapClassObject(REFCLSID rclsid, REFIID riid,
	void** ppv);

WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	try {
		HANDLE hMutex = CreateMutex(0, false, Application->Title.c_str());
		if (hMutex) {
			if (GetLastError() == ERROR_ALREADY_EXISTS) {
				CloseHandle(hMutex);
				ShowMessage("系统正在运行！");
				Application->Terminate();
				return 0;
			}
		}

		if (WebRequestHandler() != NULL) {
			WebRequestHandler()->WebModuleClass = WebModuleClass;
		}

		RegisterMidasLib(DllGetDataSnapClassObject); // ---注册Midas

		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		TStyleManager::TrySetStyle("Amethyst Kamri");

		Application->CreateForm(__classid(TFrmStart), &FrmStart);
		Application->CreateForm(__classid(TFrmLogin), &FrmLogin);
		Application->CreateForm(__classid(TFrmMain), &FrmMain);
		Application->Run();
	}
	catch (Exception &exception) {
		Application->ShowException(&exception);
	}
	catch (...) {
		try {
			throw Exception("");
		}
		catch (Exception &exception) {
			Application->ShowException(&exception);
		}
	}
	return 0;
}
// ---------------------------------------------------------------------------
