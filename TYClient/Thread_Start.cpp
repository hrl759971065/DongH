// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Thread_Start.h"
#include "FormMain.h"
#include "Service.h"
#include "FormStart.h"
#pragma package(smart_init)
// ---------------------------------------------------------------------------

// Important: Methods and properties of objects in VCL can only be
// used in a method called using Synchronize, for example:
//
// Synchronize(&UpdateCaption);
//
// where UpdateCaption could look like:
//
// void __fastcall Thread_Start::UpdateCaption()
// {
// Form1->Caption = "Updated in a thread";
// }
// ---------------------------------------------------------------------------

__fastcall Thread_Start::Thread_Start(bool CreateSuspended)
	: TThread(CreateSuspended) {
	Priority = tpHigher;
	FreeOnTerminate = true;
}

// ---------------------------------------------------------------------------
void __fastcall Thread_Start::Execute() {
	Synchronize(&Load);
	Terminate();
}
// ---------------------------------------------------------------------------

void __fastcall Thread_Start::Load() {
	::Sleep(1000);
	// ---界面信息初始化
	FrmMain->Caption = global_Service._sysInfo.Title + L"  " +
		global_Service._sysInfo.Location;
	switch (global_Service._sysInfo.NodeID) {

	case -2: // ---测试WebService
		FrmMain->PC_Model->ActivePage = FrmMain->TS_WebService;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->tmr_WebService->Enabled = true;
		global_Service._Thr_LedSend->Suspended = false; // ---启动LED控制的线程
		break;
	case -1: // ---测试模式
		FrmMain->PC_Model->ActivePage = FrmMain->TS_Test;
		global_Service.FormStyle(FrmMain, 3);
		break;
	case 0: // ---自助入厂
		FrmMain->PC_Model->ActivePage = FrmMain->TS_ZZRC;
		global_Service.FormStyle(FrmMain, 0);
		global_Service.CreateSelection(FrmMain->P_Operation);
		FrmMain->P_Operation->Left = FrmMain->Width * 0.05;
		FrmMain->P_Operation->Top = FrmMain->Height * 0.1; ;
		FrmMain->P_Operation->Width = FrmMain->ClientWidth * 0.65;
		FrmMain->P_Operation->Height = FrmMain->ClientHeight * 0.8;
		global_Service._Selection->Visible = false; // ---启动不显示 煤矿选择菜单
		break;
	case 1: // ---停车场
		FrmMain->PC_Model->ActivePage = FrmMain->TS_QCH;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    正在启动停车管理系统......");
		FrmMain->tmr_ZD->Enabled = false; // 重量曲线
		FrmMain->Chart1->Visible = false; // 重量曲线
		FrmMain->tmr_RealVal->Enabled = false; // 读取重量
		FrmMain->Label1->Visible = false; // 地磅仪表
		FrmMain->Label2->Visible = false; // 前定位
		FrmMain->Label3->Visible = false; // 后定位
		FrmMain->LED_Meter->Visible = false; // 仪表数值
		FrmMain->shp_DWQ_Head->Visible = false; // 前定位
		FrmMain->shp_DWQ_Tail->Visible = false; // 后定位
		FrmMain->Label4->Caption = "停车场状态";
		break;
	case 2: // --采样机
		FrmMain->PC_Model->ActivePage = FrmMain->TS_QCH;
		global_Service.FormStyle(FrmMain, 3); // 0 全屏 3  汽车衡
		FrmMain->Label1->Visible = false;
		FrmMain->Label2->Visible = false;
		FrmMain->Label3->Visible = false;
		FrmMain->LED_Meter->Visible = false;
		FrmMain->shp_DWQ_Head->Visible = false;
		FrmMain->shp_DWQ_Tail->Visible = false;
		FrmMain->B_DZ_UP_Tail->Visible = true;
		FrmMain->B_DZ_DOWN_Tail->Visible = true;
		FrmMain->Img_Down_Tail->Visible = true;
		FrmMain->Img_Up_Tail->Visible = true;
		FrmMain->Button10->Visible = true;
		FrmMain->Label4->Caption = "采样机状态";
		break;

	case 3: // ---汽车衡
		FrmMain->PC_Model->ActivePage = FrmMain->TS_QCH;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    正在启动无人值守汽车衡......");
		FrmMain->tmr_ZD->Enabled = true;
		FrmMain->Chart1->Visible = true;
		FrmMain->tmr_RealVal->Enabled = true;
		break;
	case 4: // ---煤场
		FrmMain->PC_Model->ActivePage = FrmMain->TS_QCH;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    正在启动煤场管理系统......");
		FrmMain->tmr_ZD->Enabled = false; // 重量曲线
		FrmMain->Chart1->Visible = false; // 重量曲线
		FrmMain->tmr_RealVal->Enabled = false; // 读取重量
		FrmMain->Label1->Visible = false; // 地磅仪表
		FrmMain->Label2->Visible = false; // 前定位
		FrmMain->Label3->Visible = false; // 后定位
		FrmMain->LED_Meter->Visible = false; // 仪表数值
		FrmMain->shp_DWQ_Head->Visible = false; // 前定位
		FrmMain->shp_DWQ_Tail->Visible = false; // 后定位
		FrmMain->Label4->Caption = "煤场状态";
		if (global_Service._sysInfo.TingID == 2) // 卸煤沟没有档杆
		{
			FrmMain->Button3->Visible = false;
			FrmMain->Button4->Visible = false;
			FrmMain->Img_Down_Head->Visible = false;
			FrmMain->Img_Up_Head->Visible = false;
		}
		break;
	case 5: // ---自助出厂
		FrmMain->PC_Model->ActivePage = FrmMain->TS_ZZCC;
		global_Service.FormStyle(FrmMain, 0);
		FrmMain->P_Operation_CC->Left = FrmMain->Width * 0.05;
		FrmMain->P_Operation_CC->Top = FrmMain->Height * 0.1; ;
		FrmMain->P_Operation_CC->Width = FrmMain->ClientWidth * 0.65;
		FrmMain->P_Operation_CC->Height = FrmMain->ClientHeight * 0.8;
		break;
	case 7: // ----采制化、斗轮机运行状态信息采集
		FrmMain->PC_Model->ActivePage = FrmMain->TS_ZTCJ;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    正在启动设备运行状态采集......");
		break;
	default:
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    功能模块(NodeID)配置错误,请检查配置文件......");
		return;

	}

	FrmMain->M_Load->Width = 309;
	FrmMain->M_Load->Height = 214;
	FrmMain->M_Load->Top = FrmMain->Height - FrmMain->M_Load->Height -
		FrmMain->SB_Status->Height - 60;
	FrmMain->M_Load->Left = 0;

	if (global_Service._sysInfo.IsOpenWserv) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    正在初始化服务......");
		if (global_Service.StartServ())
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    服务打开成功");
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    服务打开失败");
	}

	if (global_Service._connKYC_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    正在连接开元采样机中间库......");
		if (!global_Service.ConnKYC()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    开元采样机中间库连接成功");
	}

	if (global_Service._connKYZ_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    正在连接开元制样机中间库......");
		if (!global_Service.ConnKYZ()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    开元制样机中间库连接成功");
	}

	if (global_Service._connHYS_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    正在连接化验室中间库......");
		if (!global_Service.ConnHYS()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    化验室中间库连接成功");
	}

	if (global_Service._connQSY_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    正在连接全水样中间库......");
		if (!global_Service.ConnQSY()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    全水样中间库连接成功");
	}
	if (global_Service._connCYG_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    正在连接存样柜中间库......");
		if (!global_Service.ConnCYG()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    存样柜中间库连接成功");
	}
	InitDev(); // ---初始化硬件

	// --- -1:测试模式 (其它不需要启动线程的节点请自行添加到条件中进行排除)
	if (global_Service._sysInfo.NodeID != -1 &&
		global_Service._sysInfo.NodeID != -2) {

		global_Service.SetThrProc(global_Service._sysInfo.NodeID);
		global_Service._beginThr();
		FrmMain->tmr_Jiem->Enabled = true;
	}

	// FrmMain->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    正在同步系统时间......");

	/*
	 switch(Frm_Main->_Serv._SysInfo.NodeID){
	 case -2:	//---硬件测试模式
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->DevTest;
	 break;
	 case -1:	//---测试模式
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->JKCS;
	 if(!Frm_Main->_Serv._DbCfg_Cyj.Provider.IsEmpty()){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    正在连接采样机中间数据库......");
	 if(!Frm_Main->_Serv.ConnCYJ()){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   采样机中间数据库连接失败......");
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   采样机中间数据库连接成功......");
	 }
	 }
	 break;
	 case 0:
	 Frm_Main->_Serv.SetThrProc(Frm_Main->_Serv._SysInfo.NodeID);
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->QCH;
	 Frm_Main->Label1->Visible =false;
	 Frm_Main->Label2->Visible =false;
	 Frm_Main->Label3->Visible =false;
	 Frm_Main->LED_Meter->Visible =false;
	 Frm_Main->shp_DWQ_Head->Visible =false;
	 Frm_Main->shp_DWQ_Tail->Visible =false;
	 Frm_Main->Label4->Caption ="采样机状态";

	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    正在连接采样机中间数据库......");
	 if(!Frm_Main->_Serv.ConnCYJ()){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   采样机中间数据库连接失败......");
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   采样机中间数据库连接成功......");
	 }
	 break;
	 case 1:
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    正在启动无人值守汽车衡......");
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->QCH;
	 Frm_Main->tmr_ZD->Enabled =true;
	 Frm_Main->Chart1->Visible =true;
	 Frm_Main->tmr_RealVal->Enabled =true;
	 break;
	 case 2:
	 if(Frm_Main->_Serv._SysInfo.IsTimeDown){
	 Frm_Main->L_DownDZ->Visible =true;
	 Frm_Main->tmr_DownDZ->Enabled =true;	//---启动延时落杆timer
	 }

	 //Frm_Main->PC_Mode->ActivePage =Frm_Main->MC;
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->QCH;
	 Frm_Main->Label1->Visible =false;
	 //Frm_Main->Label2->Visible =false;
	 Frm_Main->Label2->Caption ="红处对射";
	 Frm_Main->Label3->Visible =false;
	 Frm_Main->LED_Meter->Visible =false;
	 //Frm_Main->shp_DWQ_Head->Visible =false;
	 Frm_Main->shp_DWQ_Tail->Visible =false;
	 Frm_Main->B_DZ_UP_Tail->Visible =false;
	 Frm_Main->B_DZ_DOWN_Tail->Visible =false;
	 Frm_Main->Img_Down_Tail->Visible =false;
	 Frm_Main->Img_Up_Tail->Visible =false;
	 Frm_Main->_Serv._WorkSta.DZSta_Tail =-2;
	 Frm_Main->Label4->Caption ="煤场状态";
	 break;
	 case 3:
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->DLJ;
	 GetDLJParams();

	 Frm_Main->_Serv._DLJParams.FieldsNum =Frm_Main->_Serv.GetCoalFieldZones_JK(Frm_Main->_Serv._DLJParams.LocationNO);
	 if(NULL ==Frm_Main->_Serv._coalFieldZoneArray) Frm_Main->_Serv._coalFieldZoneArray =new CoalFieldZone[Frm_Main->_Serv._DLJParams.FieldsNum];
	 memset(Frm_Main->_Serv._coalFieldZoneArray,0,sizeof(Frm_Main->_Serv._coalFieldZoneArray));
	 if(Frm_Main->_Serv.GetCoalFieldZonesResult_JK(Frm_Main->_Serv._coalFieldZoneArray)){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    煤场分区数据读取成功......");
	 UpdFieldsInfo();
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    煤场分区数据读取失败......");
	 }

	 Frm_Main->tmr_DLJ->Enabled =true;
	 break;
	 default:
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    功能模块(NodeID)配置错误,请检查配置文件......");
	 return;
	 }

	 if (Frm_Main->_Serv._SysInfo.NodeID != -1 ){
	 InitDev();

	 if (Frm_Main->_Serv._SysInfo.NodeID != -2 ){
	 Frm_Main->_Serv.SetThrProc(Frm_Main->_Serv._SysInfo.NodeID);	//---设置线程执行函数
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    自动模式开始工作......");
	 Frm_Main->_Serv._beginThr();

	 Frm_Main->tmr_Jiem->Enabled =true;
	 }
	 } */

	// ---应写本地日志
	global_Service._File.WriteLog("启动日志:\r\n" + FrmMain->M_Load->Text.Trim());
	FrmMain->PC_Model->Visible = true;
	FrmMain->SB_Status->Visible = true;
	::Sleep(1000);
	FrmMain->M_Load->Visible = false;
	FrmStart->Hide();
}

// ---硬件设备初始化
void __fastcall Thread_Start::InitDev() {
	// ---仪表连接
	if (global_Service._DevInfo.BalanceInfo.Use) {
		if (global_Service.OpenMeter()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    地磅仪表打开成功......");
		}
		else {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    地磅仪表打开失败:" + global_Service.GetLastErrS());
		}
	}
	else {
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    地磅仪表未启用......");
	}

	// ---天线连接
	for (int i = 0; i < 4; i++) {
		if (global_Service._DevInfo.ReaderInfo[i].Use) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    正在连接读写器(" + IntToStr(i + 1) + ")......");
			if (global_Service.OpenReader(i)) {
				FrmMain->M_Load->Lines->Add
					(Now().DateTimeString() + "\r\n    读写器连接成功(" +
					IntToStr(i + 1) + ")......");
			}
			else {
				FrmMain->M_Load->Lines->Add
					(Now().DateTimeString() + "\r\n    读写器连接失败(" +
					IntToStr(i + 1) + "):" + global_Service.GetLastErrS());
			}
		}
		else {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    车号识别器(" + IntToStr(i + 1) + ")未启用......");
		}
	}
	// ---LED连接
	for (int i = 0; i < 2; i++) {

		if (global_Service._DevInfo.LedInfo[i].Use)

			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    正在连接LED屏(" + IntToStr(i + 1) + ")......");
		if (global_Service.OpenLED(i)) {

			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    LED屏连接成功(" + IntToStr(i + 1) + ")......");
		}
		else {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    LED屏连接失败(" + IntToStr(i + 1) + "):" +
				global_Service.GetLastErrS());
		}
	}

	for (int i = 0; i < 4; i++) {

		if (global_Service._DevInfo.IOCtrlInfo[i].Use) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    正在连接开关控制器(" + IntToStr(i + 1) + ")......");
			if (global_Service.OpenIOCtrl(i)) {
				FrmMain->M_Load->Lines->Add
					(Now().DateTimeString() + "\r\n    开关控制器连接成功(" +
					IntToStr(i + 1) + ")......");
			}
			else {
				FrmMain->M_Load->Lines->Add
					(Now().DateTimeString() + "\r\n    开关控制器连接失败(" +
					IntToStr(i + 1) + "):" + global_Service.GetLastErrS());
			}
		}
	}

	// ---小票打印机连接
	if (global_Service._DevInfo.PrtInfo.Use) {
		if (global_Service.OpenPrt()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    小票打印机连接成功......");
		}
		else {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    小票打印机连接失败:" + global_Service.GetLastErrS());
		}
	}

	/*
	 //---LED屏连接
	 if(Frm_Main->_Serv._DevInfo.LedInfo[0].Use){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    正在连接LED屏......");
	 if(Frm_Main->_Serv.OpenLED()){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    LED屏打开成功......");
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    LED屏打开失败:"+Frm_Main->_Serv._LastErrs );
	 }
	 }else Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   LED屏未启用......");

	 //---IO开关模块连接
	 for (int i = 0; i < 4; i++) {
	 if(Frm_Main->_Serv._DevInfo.IOCtrlInfo[i].Use){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    正在连接IO开关模块"+IntToStr(i+1));
	 if(Frm_Main->_Serv.OpenIOCtrl(i)){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    IO开关模块"+IntToStr(i+1)+"连接成功......");
	 }else Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    IO开关模块"+IntToStr(i+1)+"连接失败:"+Frm_Main->_Serv._LastErrs);
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    IO开关模块"+IntToStr(i+1)+"未启用......");
	 }
	 } */
}
