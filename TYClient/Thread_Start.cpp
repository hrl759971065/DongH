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
	// ---������Ϣ��ʼ��
	FrmMain->Caption = global_Service._sysInfo.Title + L"  " +
		global_Service._sysInfo.Location;
	switch (global_Service._sysInfo.NodeID) {

	case -2: // ---����WebService
		FrmMain->PC_Model->ActivePage = FrmMain->TS_WebService;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->tmr_WebService->Enabled = true;
		global_Service._Thr_LedSend->Suspended = false; // ---����LED���Ƶ��߳�
		break;
	case -1: // ---����ģʽ
		FrmMain->PC_Model->ActivePage = FrmMain->TS_Test;
		global_Service.FormStyle(FrmMain, 3);
		break;
	case 0: // ---�����볧
		FrmMain->PC_Model->ActivePage = FrmMain->TS_ZZRC;
		global_Service.FormStyle(FrmMain, 0);
		global_Service.CreateSelection(FrmMain->P_Operation);
		FrmMain->P_Operation->Left = FrmMain->Width * 0.05;
		FrmMain->P_Operation->Top = FrmMain->Height * 0.1; ;
		FrmMain->P_Operation->Width = FrmMain->ClientWidth * 0.65;
		FrmMain->P_Operation->Height = FrmMain->ClientHeight * 0.8;
		global_Service._Selection->Visible = false; // ---��������ʾ ú��ѡ��˵�
		break;
	case 1: // ---ͣ����
		FrmMain->PC_Model->ActivePage = FrmMain->TS_QCH;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ��������ͣ������ϵͳ......");
		FrmMain->tmr_ZD->Enabled = false; // ��������
		FrmMain->Chart1->Visible = false; // ��������
		FrmMain->tmr_RealVal->Enabled = false; // ��ȡ����
		FrmMain->Label1->Visible = false; // �ذ��Ǳ�
		FrmMain->Label2->Visible = false; // ǰ��λ
		FrmMain->Label3->Visible = false; // ��λ
		FrmMain->LED_Meter->Visible = false; // �Ǳ���ֵ
		FrmMain->shp_DWQ_Head->Visible = false; // ǰ��λ
		FrmMain->shp_DWQ_Tail->Visible = false; // ��λ
		FrmMain->Label4->Caption = "ͣ����״̬";
		break;
	case 2: // --������
		FrmMain->PC_Model->ActivePage = FrmMain->TS_QCH;
		global_Service.FormStyle(FrmMain, 3); // 0 ȫ�� 3  ������
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
		FrmMain->Label4->Caption = "������״̬";
		break;

	case 3: // ---������
		FrmMain->PC_Model->ActivePage = FrmMain->TS_QCH;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ������������ֵ��������......");
		FrmMain->tmr_ZD->Enabled = true;
		FrmMain->Chart1->Visible = true;
		FrmMain->tmr_RealVal->Enabled = true;
		break;
	case 4: // ---ú��
		FrmMain->PC_Model->ActivePage = FrmMain->TS_QCH;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ��������ú������ϵͳ......");
		FrmMain->tmr_ZD->Enabled = false; // ��������
		FrmMain->Chart1->Visible = false; // ��������
		FrmMain->tmr_RealVal->Enabled = false; // ��ȡ����
		FrmMain->Label1->Visible = false; // �ذ��Ǳ�
		FrmMain->Label2->Visible = false; // ǰ��λ
		FrmMain->Label3->Visible = false; // ��λ
		FrmMain->LED_Meter->Visible = false; // �Ǳ���ֵ
		FrmMain->shp_DWQ_Head->Visible = false; // ǰ��λ
		FrmMain->shp_DWQ_Tail->Visible = false; // ��λ
		FrmMain->Label4->Caption = "ú��״̬";
		if (global_Service._sysInfo.TingID == 2) // жú��û�е���
		{
			FrmMain->Button3->Visible = false;
			FrmMain->Button4->Visible = false;
			FrmMain->Img_Down_Head->Visible = false;
			FrmMain->Img_Up_Head->Visible = false;
		}
		break;
	case 5: // ---��������
		FrmMain->PC_Model->ActivePage = FrmMain->TS_ZZCC;
		global_Service.FormStyle(FrmMain, 0);
		FrmMain->P_Operation_CC->Left = FrmMain->Width * 0.05;
		FrmMain->P_Operation_CC->Top = FrmMain->Height * 0.1; ;
		FrmMain->P_Operation_CC->Width = FrmMain->ClientWidth * 0.65;
		FrmMain->P_Operation_CC->Height = FrmMain->ClientHeight * 0.8;
		break;
	case 7: // ----���ƻ������ֻ�����״̬��Ϣ�ɼ�
		FrmMain->PC_Model->ActivePage = FrmMain->TS_ZTCJ;
		global_Service.FormStyle(FrmMain, 3);
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ���������豸����״̬�ɼ�......");
		break;
	default:
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ����ģ��(NodeID)���ô���,���������ļ�......");
		return;

	}

	FrmMain->M_Load->Width = 309;
	FrmMain->M_Load->Height = 214;
	FrmMain->M_Load->Top = FrmMain->Height - FrmMain->M_Load->Height -
		FrmMain->SB_Status->Height - 60;
	FrmMain->M_Load->Left = 0;

	if (global_Service._sysInfo.IsOpenWserv) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    ���ڳ�ʼ������......");
		if (global_Service.StartServ())
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ����򿪳ɹ�");
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    �����ʧ��");
	}

	if (global_Service._connKYC_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    �������ӿ�Ԫ�������м��......");
		if (!global_Service.ConnKYC()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ��Ԫ�������м�����ӳɹ�");
	}

	if (global_Service._connKYZ_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    �������ӿ�Ԫ�������м��......");
		if (!global_Service.ConnKYZ()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ��Ԫ�������м�����ӳɹ�");
	}

	if (global_Service._connHYS_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    �������ӻ������м��......");
		if (!global_Service.ConnHYS()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    �������м�����ӳɹ�");
	}

	if (global_Service._connQSY_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    ��������ȫˮ���м��......");
		if (!global_Service.ConnQSY()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ȫˮ���м�����ӳɹ�");
	}
	if (global_Service._connCYG_cfg.IsUse) {

		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"L\r\n    �������Ӵ������м��......");
		if (!global_Service.ConnCYG()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() + L"\r\n    " +
				global_Service.GetLastErrS());
		}
		else
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    �������м�����ӳɹ�");
	}
	InitDev(); // ---��ʼ��Ӳ��

	// --- -1:����ģʽ (��������Ҫ�����̵߳Ľڵ���������ӵ������н����ų�)
	if (global_Service._sysInfo.NodeID != -1 &&
		global_Service._sysInfo.NodeID != -2) {

		global_Service.SetThrProc(global_Service._sysInfo.NodeID);
		global_Service._beginThr();
		FrmMain->tmr_Jiem->Enabled = true;
	}

	// FrmMain->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    ����ͬ��ϵͳʱ��......");

	/*
	 switch(Frm_Main->_Serv._SysInfo.NodeID){
	 case -2:	//---Ӳ������ģʽ
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->DevTest;
	 break;
	 case -1:	//---����ģʽ
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->JKCS;
	 if(!Frm_Main->_Serv._DbCfg_Cyj.Provider.IsEmpty()){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    �������Ӳ������м����ݿ�......");
	 if(!Frm_Main->_Serv.ConnCYJ()){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   �������м����ݿ�����ʧ��......");
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   �������м����ݿ����ӳɹ�......");
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
	 Frm_Main->Label4->Caption ="������״̬";

	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    �������Ӳ������м����ݿ�......");
	 if(!Frm_Main->_Serv.ConnCYJ()){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   �������м����ݿ�����ʧ��......");
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   �������м����ݿ����ӳɹ�......");
	 }
	 break;
	 case 1:
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    ������������ֵ��������......");
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->QCH;
	 Frm_Main->tmr_ZD->Enabled =true;
	 Frm_Main->Chart1->Visible =true;
	 Frm_Main->tmr_RealVal->Enabled =true;
	 break;
	 case 2:
	 if(Frm_Main->_Serv._SysInfo.IsTimeDown){
	 Frm_Main->L_DownDZ->Visible =true;
	 Frm_Main->tmr_DownDZ->Enabled =true;	//---������ʱ���timer
	 }

	 //Frm_Main->PC_Mode->ActivePage =Frm_Main->MC;
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->QCH;
	 Frm_Main->Label1->Visible =false;
	 //Frm_Main->Label2->Visible =false;
	 Frm_Main->Label2->Caption ="�촦����";
	 Frm_Main->Label3->Visible =false;
	 Frm_Main->LED_Meter->Visible =false;
	 //Frm_Main->shp_DWQ_Head->Visible =false;
	 Frm_Main->shp_DWQ_Tail->Visible =false;
	 Frm_Main->B_DZ_UP_Tail->Visible =false;
	 Frm_Main->B_DZ_DOWN_Tail->Visible =false;
	 Frm_Main->Img_Down_Tail->Visible =false;
	 Frm_Main->Img_Up_Tail->Visible =false;
	 Frm_Main->_Serv._WorkSta.DZSta_Tail =-2;
	 Frm_Main->Label4->Caption ="ú��״̬";
	 break;
	 case 3:
	 Frm_Main->PC_Mode->ActivePage =Frm_Main->DLJ;
	 GetDLJParams();

	 Frm_Main->_Serv._DLJParams.FieldsNum =Frm_Main->_Serv.GetCoalFieldZones_JK(Frm_Main->_Serv._DLJParams.LocationNO);
	 if(NULL ==Frm_Main->_Serv._coalFieldZoneArray) Frm_Main->_Serv._coalFieldZoneArray =new CoalFieldZone[Frm_Main->_Serv._DLJParams.FieldsNum];
	 memset(Frm_Main->_Serv._coalFieldZoneArray,0,sizeof(Frm_Main->_Serv._coalFieldZoneArray));
	 if(Frm_Main->_Serv.GetCoalFieldZonesResult_JK(Frm_Main->_Serv._coalFieldZoneArray)){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    ú���������ݶ�ȡ�ɹ�......");
	 UpdFieldsInfo();
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    ú���������ݶ�ȡʧ��......");
	 }

	 Frm_Main->tmr_DLJ->Enabled =true;
	 break;
	 default:
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    ����ģ��(NodeID)���ô���,���������ļ�......");
	 return;
	 }

	 if (Frm_Main->_Serv._SysInfo.NodeID != -1 ){
	 InitDev();

	 if (Frm_Main->_Serv._SysInfo.NodeID != -2 ){
	 Frm_Main->_Serv.SetThrProc(Frm_Main->_Serv._SysInfo.NodeID);	//---�����߳�ִ�к���
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    �Զ�ģʽ��ʼ����......");
	 Frm_Main->_Serv._beginThr();

	 Frm_Main->tmr_Jiem->Enabled =true;
	 }
	 } */

	// ---Ӧд������־
	global_Service._File.WriteLog("������־:\r\n" + FrmMain->M_Load->Text.Trim());
	FrmMain->PC_Model->Visible = true;
	FrmMain->SB_Status->Visible = true;
	::Sleep(1000);
	FrmMain->M_Load->Visible = false;
	FrmStart->Hide();
}

// ---Ӳ���豸��ʼ��
void __fastcall Thread_Start::InitDev() {
	// ---�Ǳ�����
	if (global_Service._DevInfo.BalanceInfo.Use) {
		if (global_Service.OpenMeter()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    �ذ��Ǳ�򿪳ɹ�......");
		}
		else {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    �ذ��Ǳ��ʧ��:" + global_Service.GetLastErrS());
		}
	}
	else {
		FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    �ذ��Ǳ�δ����......");
	}

	// ---��������
	for (int i = 0; i < 4; i++) {
		if (global_Service._DevInfo.ReaderInfo[i].Use) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    �������Ӷ�д��(" + IntToStr(i + 1) + ")......");
			if (global_Service.OpenReader(i)) {
				FrmMain->M_Load->Lines->Add
					(Now().DateTimeString() + "\r\n    ��д�����ӳɹ�(" +
					IntToStr(i + 1) + ")......");
			}
			else {
				FrmMain->M_Load->Lines->Add
					(Now().DateTimeString() + "\r\n    ��д������ʧ��(" +
					IntToStr(i + 1) + "):" + global_Service.GetLastErrS());
			}
		}
		else {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    ����ʶ����(" + IntToStr(i + 1) + ")δ����......");
		}
	}
	// ---LED����
	for (int i = 0; i < 2; i++) {

		if (global_Service._DevInfo.LedInfo[i].Use)

			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
			"\r\n    ��������LED��(" + IntToStr(i + 1) + ")......");
		if (global_Service.OpenLED(i)) {

			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    LED�����ӳɹ�(" + IntToStr(i + 1) + ")......");
		}
		else {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    LED������ʧ��(" + IntToStr(i + 1) + "):" +
				global_Service.GetLastErrS());
		}
	}

	for (int i = 0; i < 4; i++) {

		if (global_Service._DevInfo.IOCtrlInfo[i].Use) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    �������ӿ��ؿ�����(" + IntToStr(i + 1) + ")......");
			if (global_Service.OpenIOCtrl(i)) {
				FrmMain->M_Load->Lines->Add
					(Now().DateTimeString() + "\r\n    ���ؿ��������ӳɹ�(" +
					IntToStr(i + 1) + ")......");
			}
			else {
				FrmMain->M_Load->Lines->Add
					(Now().DateTimeString() + "\r\n    ���ؿ���������ʧ��(" +
					IntToStr(i + 1) + "):" + global_Service.GetLastErrS());
			}
		}
	}

	// ---СƱ��ӡ������
	if (global_Service._DevInfo.PrtInfo.Use) {
		if (global_Service.OpenPrt()) {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    СƱ��ӡ�����ӳɹ�......");
		}
		else {
			FrmMain->M_Load->Lines->Add(Now().DateTimeString() +
				"\r\n    СƱ��ӡ������ʧ��:" + global_Service.GetLastErrS());
		}
	}

	/*
	 //---LED������
	 if(Frm_Main->_Serv._DevInfo.LedInfo[0].Use){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    ��������LED��......");
	 if(Frm_Main->_Serv.OpenLED()){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    LED���򿪳ɹ�......");
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    LED����ʧ��:"+Frm_Main->_Serv._LastErrs );
	 }
	 }else Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n   LED��δ����......");

	 //---IO����ģ������
	 for (int i = 0; i < 4; i++) {
	 if(Frm_Main->_Serv._DevInfo.IOCtrlInfo[i].Use){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    ��������IO����ģ��"+IntToStr(i+1));
	 if(Frm_Main->_Serv.OpenIOCtrl(i)){
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    IO����ģ��"+IntToStr(i+1)+"���ӳɹ�......");
	 }else Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    IO����ģ��"+IntToStr(i+1)+"����ʧ��:"+Frm_Main->_Serv._LastErrs);
	 }else{
	 Frm_Main->M_Load->Lines->Add(Now().DateTimeString()+"\r\n    IO����ģ��"+IntToStr(i+1)+"δ����......");
	 }
	 } */
}
