// ************************************************************************ //
// The types declared in this file were generated from data read from the
// WSDL File described below:
// WSDL     : http://172.16.1.30:8099/Webservices/NmlxService.asmx?WSDL
// >Import : http://172.16.1.30:8099/Webservices/NmlxService.asmx?WSDL>0
// Encoding : utf-8
// Version  : 1.0
// (2016-3-14 14:26:50 - - $Rev: 45757 $)
// ************************************************************************ //

#include <System.hpp>
#pragma hdrstop

#if !defined(NmlxServiceH)
#include "NmlxService.h"
#endif

namespace NS_NmlxService {

	_di_NmlxServiceSoap GetNmlxServiceSoap(bool useWSDL, System::String addr,
		Soaphttpclient::THTTPRIO* HTTPRIO) {
		static const char* defWSDL =
			"http://172.16.1.30:8099/Webservices/NmlxService.asmx?WSDL";
		static const char* defURL =
			"http://172.16.1.30:8099/Webservices/NmlxService.asmx";
		static const char* defSvc = "NmlxService";
		static const char* defPrt = "NmlxServiceSoap12";
		if (addr == "")
			addr = useWSDL ? defWSDL : defURL;
		Soaphttpclient::THTTPRIO* rio = HTTPRIO ? HTTPRIO :
			new Soaphttpclient::THTTPRIO(0);
		if (useWSDL) {
			rio->WSDLLocation = addr;
			rio->Service = defSvc;
			rio->Port = defPrt;
		}
		else {
			rio->URL = addr;
		}
		_di_NmlxServiceSoap service;
		rio->QueryInterface(service);
		if (!service && !HTTPRIO)
			delete rio;
		return service;
	}

	// ************************************************************************ //
	// This routine registers the interfaces and types exposed by the WebService.
	// ************************************************************************ //
	static void RegTypes() {
		/* NmlxServiceSoap */
		InvRegistry()->RegisterInterface(__delphirtti(NmlxServiceSoap),
			L"http://www.nmlxgroup.com/", L"utf-8");
		InvRegistry()->RegisterDefaultSOAPAction(__delphirtti(NmlxServiceSoap),
			L"http://www.nmlxgroup.com/Nmlx_DataService");
		InvRegistry()->RegisterInvokeOptions(__delphirtti(NmlxServiceSoap),
			ioDocument);
		InvRegistry()->RegisterInvokeOptions(__delphirtti(NmlxServiceSoap),
			ioSOAP12);
		/* NmlxServiceSoap.Nmlx_DataService */
		InvRegistry()->RegisterMethodInfo(__delphirtti(NmlxServiceSoap),
			"Nmlx_DataService", "", "[ReturnName='Nmlx_DataServiceResult']",
			IS_OPTN);
	}
#pragma startup RegTypes 32

}; // NS_NmlxService
