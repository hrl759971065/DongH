// ---------------------------------------------------------------------------
#include "Unit1.h"
#include <SvcMgr.hpp>
#include "Service.h"

#include "Basic.h"
#include "Registry.hpp"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

using Vcl::Svcmgr::TService;

TNmlxDevControl *NmlxDevControl;
TFile _File;

// ---------------------------------------------------------------------------
__fastcall TNmlxDevControl::TNmlxDevControl(TComponent* Owner)
	: Vcl::Svcmgr::TService(Owner) {
	Interactive = true;
	_bStartup = false;
}

TServiceController __fastcall TNmlxDevControl::GetServiceController(void) {
	return (TServiceController) ServiceController;
}

void __stdcall ServiceController(unsigned CtrlCode) {
	NmlxDevControl->Controller(CtrlCode);
}

// ---------------------------------------------------------------------------
void __fastcall TNmlxDevControl::ServiceExecute(Vcl::Svcmgr::TService *Sender) {


	while (!Terminated) {

		this->ServiceThread->ProcessRequests(false);

		if (!_bStartup) {
			_File.WriteLog(L"Web服务重启并加载各项配置!Execute");
			global_Service.StopServ();
			global_Service.ReadCfg();

			Sleep(500);
			global_Service.CloseReader(0);
			if (global_Service._DevInfo.ReaderInfo[0].Use) {

				Sleep(100);
				if (!global_Service.OpenReader(0)) {

					_File.WriteLog_ErrS(L"读写器连接失败:" + global_Service.GetLastErrS());
				}
			 }
			Sleep(500);

			if (global_Service._connKYC_cfg.IsUse) { // ---Wuwenbin16.4.29新增

				if (!global_Service.ConnKYC()) {
					_File.WriteLog_ErrS
						(L"开元采样机中间库连接失败:" + global_Service.GetLastErrS());
				}
			}

			if (global_Service._connKYZ_cfg.IsUse) { // ---Wuwenbin16.4.29新增

				if (!global_Service.ConnKYZ()) {
					_File.WriteLog_ErrS
						(L"开元制样机中间库连接失败:" + global_Service.GetLastErrS());
				}
			}
			if (global_Service._connHYS_cfg.IsUse) {

				if (!global_Service.ConnHYS()) {
					_File.WriteLog_ErrS
						(L"开元化验室中间库连接失败:" + global_Service.GetLastErrS());
				}
			}
			if (global_Service._connQSY_cfg.IsUse) {

				if (!global_Service.ConnQSY()) {
					_File.WriteLog_ErrS
						(L"全水仪中间库连接失败:" + global_Service.GetLastErrS());
				}
			}
			if (global_Service._connCYG_cfg.IsUse) {

				if (!global_Service.ConnCYG()) {
					_File.WriteLog_ErrS
						(L"存样柜中间库连接失败:" + global_Service.GetLastErrS());
				}
			}
			global_Service.InitSoap(); // ---Wuwenbin16.4.29新增 初始化平台服务接口

			if (global_Service.StartServ()) {

				_bStartup = true; // ---表示启动成功
				_File.WriteLog(L"WebService启动成功!");
			}
			else {
				_File.WriteLog_ErrS(L"WebService启动失败!");
			}
			try{
				global_Service._Thr_LedSend->Suspended = false; // ---启动LED控制的线程
			}
			catch(Exception &exception){
			  _File.WriteLog_ErrS(L"Led线程启动失败!");
			}
		}

		::Sleep(100);
		if (global_Service._bEnterCunYG) { // ---haoriliang 16.5.21新增

			global_Service.CunYG_QuY();
		}
		::Sleep(100);
		if (global_Service._bEnterSamUnload) { // ---Wuwenbin16.4.28新增

			global_Service.SamUnloadProc();
		}
	}
	_File.WriteLog(L"Windws服务已停止!");

}
// ---------------------------------------------------------------------------

void __fastcall TNmlxDevControl::ServiceStart(Vcl::Svcmgr::TService *Sender,
	bool &Started) {
	_File.WriteLog(L"服务启动!Start");
	TRegistry *Reg = new TRegistry();
	String str = L"设备控制SOAP服务.\r\n立信电气：" + Now().FormatString("yyyy.MM.dd");
	try {
		Reg->RootKey = HKEY_LOCAL_MACHINE;
		if (Reg->OpenKey(L"SYSTEM\\CurrentControlSet\\Services\\" + Name,
			false)) {
			Reg->WriteString("Description", str);
		}
	}
	__finally {
		delete Reg;
	}

	_bStartup = false;
	Started = true;
	_File.WriteLog(L"服务启动成功!Start");
}
// ---------------------------------------------------------------------------

void __fastcall TNmlxDevControl::ServiceContinue(Vcl::Svcmgr::TService *Sender,
	bool &Continued)

{   _File.WriteLog(L"服务重启!Continue");
	_bStartup = false;
	Continued = true;
	_File.WriteLog(L"服务重启成功!Continue");
	/* while(!Terminated){

	 Sleep(10);
	 this->ServiceThread->ProcessRequests(false);
	 } */
	// global_Service.StopServ();
	// global_Service.StartServ();
}
// ---------------------------------------------------------------------------

void __fastcall TNmlxDevControl::ServiceShutdown(Vcl::Svcmgr::TService *Sender)
{
	_File.WriteLog(L"服务结束!Shutdown");
	global_Service.StopServ();
	_File.WriteLog(L"服务结束成功!Shutdown");
}
// ---------------------------------------------------------------------------

void __fastcall TNmlxDevControl::ServicePause(Vcl::Svcmgr::TService *Sender,
	bool &Paused) {
	// _bStartup =false;
	_File.WriteLog(L"服务暂停!Pause");
	Paused = true;
	global_Service.StopServ();
	_File.WriteLog(L"服务暂停成功!Pause");
}
// ---------------------------------------------------------------------------

void __fastcall TNmlxDevControl::ServiceStop(Vcl::Svcmgr::TService *Sender,
	bool &Stopped) {
	// _bStartup =false;
	_File.WriteLog(L"服务停止!Stop");
	Stopped = true;
	global_Service.StopServ();
	_File.WriteLog(L"服务停止成功!Stop");
}
// ---------------------------------------------------------------------------


