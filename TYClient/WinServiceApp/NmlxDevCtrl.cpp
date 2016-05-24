#include <SysUtils.hpp>
#include <SvcMgr.hpp>
#pragma hdrstop
#include <tchar.h>

#include <WebReq.hpp>

#pragma link "Web.WebReq.obj"
#pragma link "IndySystem.lib"
#pragma link "IndyCore.lib"
#pragma link "IndyProtocols.lib"
#pragma link "IdHTTPWebBrokerBridge.obj"
// ---CCRUN的贴子上得知,WebService服务程序,不带包编译后,加以下二个lib
#pragma link "soaprtl.lib"
#pragma link "crypt32.lib"

#define Application Svcmgr::Application


USEFORM("Unit1.cpp", NmlxDevControl); /* TService: File Type */
USEFORM("..\SOAP\WebService\WebModuleUnit1.cpp", WebModule1); /* TWebModule: File Type */
//---------------------------------------------------------------------------
extern PACKAGE TComponentClass WebModuleClass;

WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int) {
	try {
		// Windows 2003 Server requires StartServiceCtrlDispatcher to be
		// called before CoRegisterClassObject, which can be called indirectly
		// by Application.Initialize. TServiceApplication->DelayInitialize allows
		// Application->Initialize to be called from TService->Main (after
		// StartServiceCtrlDispatcher has been called).
		//
		// Delayed initialization of the Application object may affect
		// events which then occur prior to initialization, such as
		// TService->OnCreate. It is only recommended if the ServiceApplication
		// registers a class object with OLE and is intended for use with
		// Windows 2003 Server.
		//
		// Application->DelayInitialize = true;
		//
		if (WebRequestHandler() != NULL) {
			WebRequestHandler()->WebModuleClass = WebModuleClass;
		}

		if ((!Application->DelayInitialize) || (Application->Installing())) {
			Application->Initialize();
		}
		Application->CreateForm(__classid(TNmlxDevControl), &NmlxDevControl);
		Application->Run();
	}
	catch (Exception &exception) {
		Sysutils::ShowException(&exception, System::ExceptAddr());
	}
	catch (...) {
		try {
			throw Exception("");
		}
		catch (Exception &exception) {
			Sysutils::ShowException(&exception, System::ExceptAddr());
		}
	}
	return 0;
}
