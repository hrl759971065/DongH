// ************************************************************************ //
// Implementation class for interface INmlxDeviceControl
// ************************************************************************ //
#include <stdio.h>
#include <vcl.h>
#pragma hdrstop

#if !defined(__NmlxDeviceControl_h__)
#include "NmlxDeviceControl.h"
#include "Service.h"
#include "JsonReq.h"
#include "JsonRet.h"
#endif

// ************************************************************************ //
// TNmlxDeviceControlImpl implements interface INmlxDeviceControl
// ************************************************************************ //
class TNmlxDeviceControlImpl : public TInvokableClass, public INmlxDeviceControl
{
public:
	// TNmlxDeviceControlImpl();
	String DataRequst(const String JsonReq);

	/* IUnknown */
	HRESULT STDMETHODCALLTYPE QueryInterface(const GUID& IID, void **Obj) {
		return GetInterface(IID, Obj) ? S_OK : E_NOINTERFACE;
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		return TInvokableClass::_AddRef();
	}

	ULONG STDMETHODCALLTYPE Release() {
		return TInvokableClass::_Release();
	}

private:
	TJsonReq _jsonReq;
	TJsonRet _jsonRet;

	// ---数据库对象
	// TLX_DB* _connKYC;	//---开元采样机
	// TLX_DB* _connKYZ;   //---开元制样机
};

// ---接口实现
String TNmlxDeviceControlImpl::DataRequst(const String JsonReq) {
	UTF8String JsonRet;
	global_Service._strReqJson = JsonReq;
	Json::Value JsonData;
	// ---安全验证
	if (!_jsonReq.IsAllowLogin(JsonReq)) {

		JsonRet = _jsonRet.CreatJsonRet(5, _jsonReq.GetLastErrS(), 0, JsonData);
		global_Service._strRetJson = JsonRet;
		global_Service._bSyncSoapData = true;
		return JsonRet;
	}
	// ---取业务ID
	int OperID;
	if (!_jsonReq.GetOperID(JsonReq, OperID)) {

		JsonRet = _jsonRet.CreatJsonRet(3, _jsonReq.GetLastErrS(), 0, JsonData);
		global_Service._strRetJson = JsonRet;
		global_Service._bSyncSoapData = true;
		return JsonRet;
	}

	TLX_DB* _connKYC = NULL; // ---开元采样机
	TLX_DB* _connKYZ = NULL; // ---开元制样机
	TLX_DB* _connCYG = NULL; // ---存样柜

	if (OperID <= 7) {

		if (NULL == _connKYC) {
			_connKYC = CreateLX_DB();
			if (!_connKYC->PutConnStrInfo(global_Service._connKYC_cfg)) {

				JsonRet = _jsonRet.CreatJsonRet(1,
					L"开元采样机数据库初始化失败:" + _connKYC->GetLastErr(), 0, JsonData);
				global_Service._strRetJson = JsonRet;
				global_Service._bSyncSoapData = true;
				return JsonRet;
			}
			if (!_connKYC->Conn()) {

				JsonRet = _jsonRet.CreatJsonRet(1,
					L"开元采样机数据库连接失败:" + _connKYC->GetLastErr(), 0, JsonData);
				global_Service._strRetJson = JsonRet;
				global_Service._bSyncSoapData = true;
				return JsonRet;
			}
		}

		if (NULL == _connKYZ) {
			_connKYZ = CreateLX_DB();
			if (!_connKYZ->PutConnStrInfo(global_Service._connKYZ_cfg)) {

				JsonRet = _jsonRet.CreatJsonRet(1,
					L"开元制样机数据库初始化失败:" + _connKYZ->GetLastErr(), 0, JsonData);
				global_Service._strRetJson = JsonRet;
				global_Service._bSyncSoapData = true;
				return JsonRet;
			}
			if (!_connKYZ->Conn()) {

				JsonRet = _jsonRet.CreatJsonRet(1,
					L"开元制样机数据库连接失败:" + _connKYZ->GetLastErr(), 0, JsonData);
				global_Service._strRetJson = JsonRet;
				global_Service._bSyncSoapData = true;
				return JsonRet;
			}
		}

	}
	if (OperID == 14) {

		if (NULL == _connCYG) {
			_connCYG = CreateLX_DB();
			if (!_connCYG->PutConnStrInfo(global_Service._connCYG_cfg)) {

				JsonRet = _jsonRet.CreatJsonRet(1,
					L"存样柜数据库初始化失败:" + _connCYG->GetLastErr(), 0, JsonData);
				global_Service._strRetJson = JsonRet;
				global_Service._bSyncSoapData = true;
				return JsonRet;
			}
			if (!_connCYG->Conn()) {

				JsonRet = _jsonRet.CreatJsonRet(1,
					L"存样柜数据库连接失败:" + _connCYG->GetLastErr(), 0, JsonData);
				global_Service._strRetJson = JsonRet;
				global_Service._bSyncSoapData = true;
				return JsonRet;
			}
		}
	}

	switch (OperID) {
	case 0:
		JsonRet = global_Service.SamStaInfo(_connKYC, JsonReq);
		break;
	case 1:
		JsonRet = global_Service.CollectorInfo(_connKYC, JsonReq);
		break;
	case 2:
		JsonRet = global_Service.SamControl(_connKYC, JsonReq);
		break;
	case 3:
		JsonRet = global_Service.ZYSamStaInfo(_connKYZ, JsonReq);
		break;
	case 4:
		JsonRet = global_Service.ZYSamDetailInfo(_connKYZ, JsonReq);
		break;
	case 5:
		JsonRet = global_Service.ZYSamControl(_connKYZ, JsonReq);
		break;
	case 6:
		JsonRet = global_Service.SamUnload(_connKYC, JsonReq);
		break;
	case 7:
		JsonRet = global_Service.ZYSamUnload(_connKYZ, JsonReq);
		break;
	case 8:
		JsonRet = global_Service.ReadCard(JsonReq);
		break;
	case 9:
		JsonRet = global_Service.WriteCard(JsonReq);
		break;
	case 10:
		JsonRet = global_Service.ZYSamUnload_New(JsonReq);
		break;
	case 11:
		JsonRet = global_Service.ForceEndSamLoad(JsonReq);
		break;
	case 12:
		JsonRet = global_Service.SendLED_Diaod(JsonReq);
		break;
	case 13:
		JsonRet = global_Service.SendLED_Gongg(JsonReq);
		break;
	case 14:
		JsonRet = global_Service.CunYG(JsonReq);
		break;
	case 15:
		JsonRet = global_Service.ForceEndCunYG(JsonReq);
		break;
	default:
		Json::Value JsonData;
		JsonRet = _jsonRet.CreatJsonRet(5, L"暂不支持此OperID", 0, JsonData);
	}

	if (_connKYC != NULL)
		delete _connKYC;
	if (_connKYZ != NULL)
		delete _connKYZ;
   //	if (_connCYG != NULL)
   //		delete _connCYG;

	global_Service._strRetJson = JsonRet;
	global_Service._bSyncSoapData = true;
	return JsonRet;
}

static void __fastcall NmlxDeviceControlFactory(System::TObject* &obj) {
	static _di_INmlxDeviceControl iInstance;
	static TNmlxDeviceControlImpl *instance = 0;
	if (!instance) {
		instance = new TNmlxDeviceControlImpl();
		instance->GetInterface(iInstance);
	}
	obj = instance;
}

// ************************************************************************ //
// The following routine registers the interface and implementation class
// ************************************************************************ //
static void RegTypes() {
	InvRegistry()->RegisterInterface(__delphirtti(INmlxDeviceControl));
	InvRegistry()->RegisterInvokableClass(__classid(TNmlxDeviceControlImpl),
		NmlxDeviceControlFactory);
}
#pragma startup RegTypes 32
