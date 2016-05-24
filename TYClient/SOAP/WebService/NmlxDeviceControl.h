// ************************************************************************ //
// Invokable interface declaration header for NmlxDeviceControl
// ************************************************************************ //
#ifndef   NmlxDeviceControlH
#define   NmlxDeviceControlH

#include <System.hpp>
#include <InvokeRegistry.hpp>
#include <XSBuiltIns.hpp>
#include <Types.hpp>



// ************************************************************************ //
//  Invokable interfaces must derive from IInvokable
//  The methods of the interface will be exposed via SOAP
// ************************************************************************ //
__interface INTERFACE_UUID("{D6E77F1C-F418-4DEF-9CF6-58804D91D3B7}") INmlxDeviceControl : public IInvokable
{
public:
	virtual String DataRequst(const String JsonReq) =0;
};
typedef DelphiInterface<INmlxDeviceControl> _di_INmlxDeviceControl;


#endif // NmlxDeviceControlH
//---------------------------------------------------------------------------


