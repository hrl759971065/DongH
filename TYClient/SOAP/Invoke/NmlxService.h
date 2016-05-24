// ************************************************************************ //
// The types declared in this file were generated from data read from the
// WSDL File described below:
// WSDL     : http://172.16.1.30:8099/Webservices/NmlxService.asmx?WSDL
//  >Import : http://172.16.1.30:8099/Webservices/NmlxService.asmx?WSDL>0
// Encoding : utf-8
// Version  : 1.0
// (2016-3-14 14:26:50 - - $Rev: 45757 $)
// ************************************************************************ //

#ifndef   NmlxServiceH
#define   NmlxServiceH

#include <System.hpp>
#include <InvokeRegistry.hpp>
#include <XSBuiltIns.hpp>
#include <SOAPHTTPClient.hpp>

#if !defined(SOAP_REMOTABLE_CLASS)
#define SOAP_REMOTABLE_CLASS __declspec(delphiclass)
#endif
#if !defined(IS_OPTN)
#define IS_OPTN 0x0001
#endif
#if !defined(IS_REF)
#define IS_REF 0x0080
#endif


namespace NS_NmlxService {

// ************************************************************************ //
// The following types, referred to in the WSDL document are not being represented
// in this file. They are either aliases[@] of other types represented or were referred
// to but never[!] declared in the document. The types from the latter category
// typically map to predefined/known XML or Embarcadero types; however, they could also 
// indicate incorrect WSDL documents that failed to declare or import a schema type.
// ************************************************************************ //
// !:string          - "http://www.w3.org/2001/XMLSchema"[Gbl]



// ************************************************************************ //
// Namespace : http://www.nmlxgroup.com/
// soapAction: http://www.nmlxgroup.com/Nmlx_DataService
// transport : http://schemas.xmlsoap.org/soap/http
// style     : document
// use       : literal
// binding   : NmlxServiceSoap12
// service   : NmlxService
// port      : NmlxServiceSoap12
// URL       : http://172.16.1.30:8099/Webservices/NmlxService.asmx
// ************************************************************************ //
__interface INTERFACE_UUID("{FE03212F-71AD-54FA-466C-3A2AEF2F4D01}") NmlxServiceSoap : public IInvokable
{
public:
  virtual UnicodeString   Nmlx_DataService(const UnicodeString request) = 0; 
};
typedef DelphiInterface<NmlxServiceSoap> _di_NmlxServiceSoap;

_di_NmlxServiceSoap GetNmlxServiceSoap(bool useWSDL=false, System::String addr= System::String(), Soaphttpclient::THTTPRIO* HTTPRIO=0);


};     // NS_NmlxService

#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using  namespace NS_NmlxService;
#endif



#endif // NmlxServiceH
