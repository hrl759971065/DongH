// ---------------------------------------------------------------------------
#include <vcl.h>

#pragma hdrstop

#include "ServBase.h"
#include <IniFiles.hpp>
// ---------------------------------------------------------------------------
#pragma link "jsoncpp.lib"
#pragma link "LX_DB.lib"
#pragma link "CommDevice.lib"
// #pragma link "Midas.lib"
#pragma package(smart_init)
bool _bRead = true;
bool bResult = true;

TServBase::TServBase() {
	for (int i = 0; i < 4; i++)
		_Reader[i] = NULL;
	for (int i = 0; i < 2; i++)
		_Led[i] = NULL;
	for (int i = 0; i < 4; i++)
		_IOCtrl[i] = NULL;
	_ComPrt = NULL;

	_connKYC = NULL;     //������
	_connKYZ = NULL;     //������
	_connHYS = NULL;     //������
	_connQSY = NULL;     //ȫˮ��
	_connCYG = NULL;     //������

	_connKYC = CreateLX_DB();
	_connKYZ = CreateLX_DB();
	_connHYS = CreateLX_DB();
	_connQSY = CreateLX_DB();
	_connCYG = CreateLX_DB();

	_ptSoap = NULL;

	_Thread = NULL;
	_Thread = new Thread_Run(true);

	_FServer_IdHTTPWebBrokerBridge = NULL;
	_dsMkGys = new TClientDataSet(NULL);

	_ListDS = L"";
	_ServerDate = L"";
	_bReset = false;
	_Sequence = 0;
	_strFaKaTis = L"";
	_bForceEndSamUnload = false;
	_bEnterSamUnload = false;
	_CarList = new TStringList();
	_bSendLED_DDInfo = false;
	_Thr_LedSend = new Thread_LedSend(true);
	for (int i = 0; i < 8; i++) {

		_LedBmp._bmDD[i] = new TBitmap();
		_LedBmp._bmGG[i] = new TBitmap();
	}
	_LedBmp.picNumDD = 0;
	_LedBmp.picNumGG = 0;

	_bSyncSoapData = false;
	//������ȡ��
	_BianMList = new TStringList();
	_bForceEndCunYG = false;
	_bEnterCunYG = false;
	CunYG_OpType=0;
	CunYG_OpDest=0;
    CunYG_UserName=NULL;
	ReadCfg();
}

TServBase::~TServBase() {
	_dsMkGys->Free();
}

String TServBase::GetLastErrS() const {
	return _LastErrS;
}

void TServBase::ReadCfg() {
	TMemIniFile *ini =
		new TMemIniFile(ExtractFilePath(Forms::Application->ExeName) +
		"cfg\\sys.ini");

	try {
		// ---ϵͳ��Ϣ
		_sysInfo.Title = ini->ReadString("System", "Title", "δ���ñ���");
		_sysInfo.Location = ini->ReadString("System", "Location", "δ�����ص�");
		_sysInfo.NodeID = ini->ReadString("System", "NodeID", "").ToInt();
		_sysInfo.TingID = ini->ReadString("System", "TCCID", "").ToInt();
		_sysInfo.CYJNO = ini->ReadString("System", "CYJNO", "");
		_sysInfo.RealTime = ini->ReadString("System", "RealTime", "");
		_sysInfo.TriggerWeight = ini->ReadString("System", "TriggerWeight", "")
			.ToInt();
		_sysInfo.QCHMode = ini->ReadString("System", "QCHMode", "").ToInt();
		_sysInfo.MinMZ = ini->ReadString("System", "MinMZ", "").ToInt();
		_sysInfo.MinPZ = ini->ReadString("System", "MinPZ", "").ToInt();
		_sysInfo.MaxPZ = ini->ReadString("System", "MaxPZ", "").ToInt();

		// _sysInfo.TingID >0 ? _sysInfo.TingID =1 : _sysInfo.TingID =0;

		// ---ƽ̨�ӿڵ��õ�ַ
		_sysInfo.pt_JiekUrl = ini->ReadString("Soap", "WSDL", "");

		// ---��ϵͳ�ṩ��WebService�ӿڵĶ˿ں�
		_sysInfo.JiekPort = ini->ReadString("WebServ", "Port", "");
		_sysInfo.IsOpenWserv =
			StrToBool(ini->ReadString("WebServ", "IsUse", ""));
	}
	catch (Exception &e) {
		_LastErrS = L"sys.ini�����ļ���ȡʧ��,���������ļ�:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		ShowMessage(_LastErrS);
	}

	ini->Free();

	ini = new TMemIniFile(ExtractFilePath(Forms::Application->ExeName) +
		"cfg\\device.ini");

	try {
		// ---�豸��Ϣ
		_DevInfo.BalanceInfo.Model = ini->ReadString("Meter", "Model", "ͨ��");
		_DevInfo.BalanceInfo.Use =
			StrToBool(ini->ReadString("Meter", "IsUse", ""));
		_DevInfo.BalanceInfo.CommMode = 0;
		_DevInfo.BalanceInfo.CommCfg.ComNum =
			ini->ReadString("Meter", "Com", "").ToInt();
		_DevInfo.BalanceInfo.CommCfg.dwBaudRate =
			ini->ReadString("Meter", "BaudRate", "").ToInt();
		_DevInfo.BalanceInfo.CommCfg.Chk =
			ini->ReadString("Meter", "ParityCheck", "")[1];
		_DevInfo.BalanceInfo.CommCfg.DataBits =
			ini->ReadString("Meter", "DataBit", "").ToInt();
		_DevInfo.BalanceInfo.CommCfg.Stops =
			ini->ReadString("Meter", "StopBit", "").ToInt();
		_DevInfo.BalanceInfo.CommCfg.UsePkg =
			StrToBool(ini->ReadString("Meter", "UsePkg", "false"));
		_DevInfo.BalanceInfo.CommCfg.DisWrite = false;
		_DevInfo.BalanceInfo.Head_Tag = ini->ReadString("Meter", "BTag", "")[2];
		_DevInfo.BalanceInfo.Tail_Tag = ini->ReadString("Meter", "ETag", "")[2];
		_DevInfo.BalanceInfo.StartPos = ini->ReadString("Meter", "StartVal", "")
			.ToInt();
		_DevInfo.BalanceInfo.DataLen = ini->ReadString("Meter", "DataLen", "")
			.ToInt();
		_DevInfo.BalanceInfo.IsRev =
			StrToBool(ini->ReadString("Meter", "IsRev", ""));
		_DevInfo.BalanceInfo.dwTimeOuts = 2000;
		_DevInfo.BalanceInfo.CommCfg.ComPkgType =
			ini->ReadString("Meter", "ComPkgType", "").ToInt();
		_DevInfo.BalanceInfo.CommCfg.PkgInterval =
			ini->ReadString("Meter", "Pkginterval", "").ToInt();

		_DevInfo.ReaderCnt = 4;
		for (int i = 0; i < _DevInfo.ReaderCnt; i++) {
			_DevInfo.ReaderInfo[i].Model =
				ini->ReadString("Reader_" + IntToStr(i + 1), "Model", "");
			_DevInfo.ReaderInfo[i].CommMode =
				ini->ReadString("Reader_" + IntToStr(i + 1), "CommMode", "")
				.ToInt();
			_DevInfo.ReaderInfo[i].CommCfg.ComNum =
				ini->ReadString("Reader_" + IntToStr(i + 1), "Com", "").ToInt();
			_DevInfo.ReaderInfo[i].CommCfg.dwBaudRate =
				ini->ReadString("Reader_" + IntToStr(i + 1), "BaudRate", "")
				.ToInt();
			_DevInfo.ReaderInfo[i].CommCfg.Chk =
				ini->ReadString("Reader_" + IntToStr(i + 1),
				"ParityCheck", "")[1];
			_DevInfo.ReaderInfo[i].CommCfg.DataBits =
				ini->ReadString("Reader_" + IntToStr(i + 1), "DataBit", "")
				.ToInt();
			_DevInfo.ReaderInfo[i].CommCfg.Stops =
				ini->ReadString("Reader_" + IntToStr(i + 1), "StopBit", "")
				.ToInt();
			_DevInfo.ReaderInfo[i].CommCfg.PkgInterval = 120; // ---������
			_DevInfo.ReaderInfo[i].CommCfg.UsePkg = true;
			_DevInfo.ReaderInfo[i].CommCfg.ComPkgType = FRAMETIMEOUT;
			_DevInfo.ReaderInfo[i].NetCfg.IP =
				ini->ReadString("Reader_" + IntToStr(i + 1), "IP", "");
			_DevInfo.ReaderInfo[i].NetCfg.Port =
				ini->ReadString("Reader_" + IntToStr(i + 1), "Port", "")
				.ToInt();
			_DevInfo.ReaderInfo[i].DevAddr =
				ini->ReadString("Reader_" + IntToStr(i + 1), "DevAddr", "")
				.ToInt();
			_DevInfo.ReaderInfo[i].AntCnt =
				ini->ReadString("Reader_" + IntToStr(i + 1), "AntCnt", "")
				.ToInt();
			for (int j = 0; j < _DevInfo.ReaderInfo[i].AntCnt; j++) {
				_DevInfo.ReaderInfo[i].Ant[j] =
					ini->ReadString("Reader_" + IntToStr(i + 1),
					"Ant" + IntToStr(j + 1), "").ToInt();
			}
			_DevInfo.ReaderInfo[i].Use =
				StrToBool(ini->ReadString("Reader_" + IntToStr(i + 1),
				"IsUse", ""));
		}

		_DevInfo.LedCnt = 2;
		for (int i = 0; i < _DevInfo.LedCnt; i++) {

			_DevInfo.LedInfo[i].Model =
				ini->ReadString("LED_" + IntToStr(i + 1), "Model", "");
			_DevInfo.LedInfo[i].CommMode =
				ini->ReadString("LED_" + IntToStr(i + 1), "CommMode", "")
				.ToInt();
			_DevInfo.LedInfo[i].CommCfg.ComNum =
				ini->ReadString("LED_" + IntToStr(i + 1), "Com", "").ToInt();
			_DevInfo.LedInfo[i].CommCfg.dwBaudRate =
				ini->ReadString("LED_" + IntToStr(i + 1), "BaudRate", "")
				.ToInt();
			_DevInfo.LedInfo[i].NetCfg.IP =
				ini->ReadString("LED_" + IntToStr(i + 1), "IP", "");
			_DevInfo.LedInfo[i].NetCfg.Port =
				ini->ReadString("LED_" + IntToStr(i + 1), "Port", "").ToInt();
			_DevInfo.LedInfo[i].CardAddr =
				ini->ReadString("LED_" + IntToStr(i + 1), "CardAddr", "")
				.ToInt();
			_DevInfo.LedInfo[i].Width =
				ini->ReadString("LED_" + IntToStr(i + 1), "Width", "").ToInt();
			_DevInfo.LedInfo[i].Height =
				ini->ReadString("LED_" + IntToStr(i + 1), "Height", "").ToInt();
			_DevInfo.LedInfo[i].FontSize =
				ini->ReadString("LED_" + IntToStr(i + 1), "FontSize", "")
				.ToInt();
			_DevInfo.LedInfo[i].SendMode = 0;
			_DevInfo.LedInfo[i].ColorStyle =
				ini->ReadString("LED_" + IntToStr(i + 1), "ColorStyle", "")
				.ToInt();
			_DevInfo.LedInfo[i].Use =
				StrToBool(ini->ReadString("LED_" + IntToStr(i + 1),
				"IsUse", ""));
		}

		_DevInfo.IoCtrlCnt = 4;
		for (int i = 0; i < 4; i++) {
			_DevInfo.IOCtrlInfo[i].Model = "ͨ��";
			_DevInfo.IOCtrlInfo[i].Use =
				StrToBool(ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"IsUse", ""));
			_DevInfo.IOCtrlInfo[i].CommMode = 0;
			_DevInfo.IOCtrlInfo[i].CommCfg.ComNum =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "Com", "").ToInt();
			_DevInfo.IOCtrlInfo[i].CommCfg.dwBaudRate =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "BaudRate", "")
				.ToInt();
			_DevInfo.IOCtrlInfo[i].CommCfg.Chk =
				ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"ParityCheck", "")[1];
			_DevInfo.IOCtrlInfo[i].CommCfg.DataBits =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "DataBit", "")
				.ToInt();
			_DevInfo.IOCtrlInfo[i].CommCfg.Stops =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "StopBit", "")
				.ToInt();
			_DevInfo.IOCtrlInfo[i].CommCfg.UsePkg = true;
			_DevInfo.IOCtrlInfo[i].CommCfg.PkgInterval = 50;
			_DevInfo.IOCtrlInfo[i].CommCfg.ComPkgType = FRAMETIMEOUT;
			_DevInfo.IOCtrlInfo[i].CommCfg.DisWrite = false;
			_DevInfo.IOCtrlInfo[i].UseDZ_Head =
				StrToBool(ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"IsUseDZ_Head", ""));
			_DevInfo.IOCtrlInfo[i].UseDZ_Tail =
				StrToBool(ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"IsUseDZ_Tail", ""));
			_DevInfo.IOCtrlInfo[i].UseHLD_Tail =
				StrToBool(ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"IsUseHLD", ""));
			_DevInfo.IOCtrlInfo[i].UseDWQ =
				StrToBool(ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"IsUseDWQ", ""));
			_DevInfo.IOCtrlInfo[i].DownDZCmd_Head =
				ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"CmdDzDown_Head", "");
			_DevInfo.IOCtrlInfo[i].UpDZCmd_Head =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "CmdDzUp_Head", "");
			_DevInfo.IOCtrlInfo[i].DownDZCmd_Tail =
				ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"CmdDzDown_Tail", "");
			_DevInfo.IOCtrlInfo[i].UpDZCmd_Tail =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "CmdDzUp_Tail", "");
			_DevInfo.IOCtrlInfo[i].RedHLDCmd_Tail =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "CmdRed", "");
			_DevInfo.IOCtrlInfo[i].GreenHLDCmd_Tail =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "CmdGreen", "");
			_DevInfo.IOCtrlInfo[i].CloseHLDCmd_Tail =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "CmdClose", "");
			_DevInfo.IOCtrlInfo[i].SelDWQStaCmd =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "CmdGetDW", "");
			_DevInfo.IOCtrlInfo[i].DWQMissEcho_ALL =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "EchoMiss_All", "");
			_DevInfo.IOCtrlInfo[i].DWQMissEcho_Head =
				ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"EchoMiss_Head", "");
			_DevInfo.IOCtrlInfo[i].DWQMissEcho_Tail =
				ini->ReadString("IOCtrl" + IntToStr(i + 1),
				"EchoMiss_Tail", "");
			_DevInfo.IOCtrlInfo[i].DWQPassEcho_ALL =
				ini->ReadString("IOCtrl" + IntToStr(i + 1), "EchoPass_All", "");
		}

		_DevInfo.PrtInfo.CommCfg.ComNum =
			ini->ReadString("Printer", "Com", "").ToInt();
		_DevInfo.PrtInfo.CommCfg.dwBaudRate =
			ini->ReadString("Printer", "BaudRate", "").ToInt();
		_DevInfo.PrtInfo.CommCfg.Chk =
			ini->ReadString("Printer", "ParityCheck", "")[1];
		_DevInfo.PrtInfo.CommCfg.DataBits =
			ini->ReadString("Printer", "DataBit", "").ToInt();
		_DevInfo.PrtInfo.CommCfg.Stops =
			ini->ReadString("Printer", "StopBit", "").ToInt();
		_DevInfo.PrtInfo.Use =
			StrToBool(ini->ReadString("Printer", "IsUse", ""));
	}
	catch (Exception &e) {
		_LastErrS = L"device.ini�����ļ���ȡʧ��,���������ļ�:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		ShowMessage(_LastErrS);
	}

	ini->Free();

	ini = new TMemIniFile(ExtractFilePath(Forms::Application->ExeName) +
		"cfg\\database.ini");

	try {
		// ---��Ԫ�������м��
		_connKYC_cfg.IsUse = StrToBool(ini->ReadString("KYCDB", "IsUse", ""));
		_connKYC_cfg.Provider = ini->ReadString("KYCDB", "Provider", "");
		_connKYC_cfg.User = ini->ReadString("KYCDB", "User", "");
		_connKYC_cfg.Pass = ini->ReadString("KYCDB", "Pass", "");
		_connKYC_cfg.Database = ini->ReadString("KYCDB", "Database", "");
		_connKYC_cfg.Server = ini->ReadString("KYCDB", "Server", "");

		// ---��Ԫ�������м��
		_connKYZ_cfg.IsUse = StrToBool(ini->ReadString("KYZDB", "IsUse", ""));
		_connKYZ_cfg.Provider = ini->ReadString("KYZDB", "Provider", "");
		_connKYZ_cfg.User = ini->ReadString("KYZDB", "User", "");
		_connKYZ_cfg.Pass = ini->ReadString("KYZDB", "Pass", "");
		_connKYZ_cfg.Database = ini->ReadString("KYZDB", "Database", "");
		_connKYZ_cfg.Server = ini->ReadString("KYZDB", "Server", "");

		// ---�������м��
		_connHYS_cfg.IsUse = StrToBool(ini->ReadString("HYSDB", "IsUse", ""));
		_connHYS_cfg.Provider = ini->ReadString("HYSDB", "Provider", "");
		_connHYS_cfg.User = ini->ReadString("HYSDB", "User", "");
		_connHYS_cfg.Pass = ini->ReadString("HYSDB", "Pass", "");
		_connHYS_cfg.Database = ini->ReadString("HYSDB", "Database", "");
		_connHYS_cfg.Server = ini->ReadString("HYSDB", "Server", "");
		// ---ȫˮ���м��
		_connQSY_cfg.IsUse = StrToBool(ini->ReadString("QSYDB", "IsUse", ""));
		_connQSY_cfg.Provider = ini->ReadString("QSYDB", "Provider", "");
		_connQSY_cfg.User = ini->ReadString("QSYDB", "User", "");
		_connQSY_cfg.Pass = ini->ReadString("QSYDB", "Pass", "");
		_connQSY_cfg.Database = ini->ReadString("QSYDB", "Database", "");
		_connQSY_cfg.Server = ini->ReadString("QSYDB", "Server", "");
		// ---�������м��
		_connCYG_cfg.IsUse = StrToBool(ini->ReadString("CYGDB", "IsUse", ""));
		_connCYG_cfg.Provider = ini->ReadString("CYGDB", "Provider", "");
		_connCYG_cfg.User = ini->ReadString("CYGDB", "User", "");
		_connCYG_cfg.Pass = ini->ReadString("CYGDB", "Pass", "");
		_connCYG_cfg.Database = ini->ReadString("CYGDB", "Database", "");
		_connCYG_cfg.Server = ini->ReadString("CYGDB", "Server", "");
	}
	catch (Exception &e) {
		_LastErrS = L"database.ini�����ļ���ȡʧ��,���������ļ�:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		ShowMessage(_LastErrS);
	}

	ini->Free();
}

// ---��ʼ��ƽ̨�ķ���ӿ�
bool TServBase::InitSoap() {
	try {
		_ptSoap = NULL;
		_ptSoap = GetNmlxServiceSoap(true, _sysInfo.pt_JiekUrl);
		if (NULL == _ptSoap) {
			_LastErrS = "Soap�ӿڳ�ʼ��ʧ��(ServJiek::InitSoap),������Ϣ:����Ϊ��";
			return false;
		}
		return true;

	}
	catch (Exception &e) {
		_LastErrS = "Soap�ӿڳ�ʼ��ʧ��(ServJiek::InitSoap),������Ϣ:" + e.Message;
		return false;
	}
	catch (...) {
		_LastErrS = "Soap�ӿڳ�ʼ��ʧ��(ServJiek::InitSoap),������Ϣ:δ֪����";
		return false;
	}
}

bool TServBase::StartServ() {
	try {
		if (NULL == _FServer_IdHTTPWebBrokerBridge)
			_FServer_IdHTTPWebBrokerBridge = new TIdHTTPWebBrokerBridge(NULL);
		if (!_FServer_IdHTTPWebBrokerBridge->Active) {
			_FServer_IdHTTPWebBrokerBridge->Bindings->Clear();
			_FServer_IdHTTPWebBrokerBridge->DefaultPort =
				StrToInt(_sysInfo.JiekPort);
			_FServer_IdHTTPWebBrokerBridge->Active = true;
		}
		return true;

	}
	catch (Exception &e) {
		_LastErrS = "��������ʧ��,������Ϣ:" + e.Message;
		return false;
	}
	catch (...) {
		_LastErrS = "��������ʧ��,������Ϣ:δ֪����";
		return false;
	}
}

bool TServBase::StopServ() {

   	try {
	_Thr_LedSend->_bClose=true;
	while(!_Thr_LedSend->_bFree)
	{
		::Sleep(100);
	};

		if (NULL == _FServer_IdHTTPWebBrokerBridge)
			_FServer_IdHTTPWebBrokerBridge = new TIdHTTPWebBrokerBridge(NULL);
		if (_FServer_IdHTTPWebBrokerBridge->Active) {
			_FServer_IdHTTPWebBrokerBridge->Active = false;
			_FServer_IdHTTPWebBrokerBridge->Bindings->Clear();
		}
		return true;

	}
	catch (Exception &e) {
		_LastErrS = "WebServiceֹͣʧ��,������Ϣ:" + e.Message;
		return false;
	}
	catch (...) {
		_LastErrS = "WebServiceֹͣʧ��,������Ϣ:δ֪����";
		return false;
	}

}

bool TServBase::ConnKYC() {
	try {
		if (NULL == _connKYC) {
			_LastErrS = "��Ԫ�������м�����Ӷ�������ʧ��";
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}
		if (!_connKYC->PutConnStrInfo(_connKYC_cfg)) {
			_LastErrS = L"��Ԫ�������м�����Ӷ�������ʧ��:" + _connKYC->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

		if (!_connKYC->Conn()) {
			_LastErrS = "��Ԫ�������м�����Ӷ�������ʧ��:" + _connKYC->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

	}
	catch (...) {
		_LastErrS = "���ӿ�Ԫ�������м��ʱ�����쳣:" + _connKYC->GetLastErr();
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
	return true;
}

bool TServBase::ConnKYZ() {
	try {
		if (NULL == _connKYZ) {
			_LastErrS = "��Ԫ�������м�����Ӷ�������ʧ��";
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

		if (!_connKYZ->PutConnStrInfo(_connKYZ_cfg)) {
			_LastErrS = L"��Ԫ�������м�����Ӷ�������ʧ��:" + _connKYZ->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

		if (!_connKYZ->Conn()) {
			_LastErrS = "��Ԫ�������м�����Ӷ�������ʧ��:" + _connKYZ->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

	}
	catch (...) {
		_LastErrS = "���ӿ�Ԫ�������м��ʱ�����쳣:" + _connKYZ->GetLastErr();
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
	return true;
}

bool TServBase::ConnHYS() {
	try {
		if (NULL == _connHYS) {
			_LastErrS = "�������м�����Ӷ�������ʧ��";
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}


		if (!_connHYS->PutConnStrInfo(_connHYS_cfg)) {
			_LastErrS = L"�������м�����Ӷ�������ʧ��:" + _connHYS->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

		if (!_connHYS->Conn()) {
			_LastErrS = "�������м�����Ӷ�������ʧ��:" + _connHYS->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

	}
	catch (...) {
		_LastErrS = "���ӻ������м��ʱ�����쳣:" + _connHYS->GetLastErr();
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
	return true;
}
//ȫˮ������
bool TServBase::ConnQSY() {
	try {
		if (NULL == _connQSY) {
			_LastErrS = "ȫˮ���м�����Ӷ�������ʧ��";
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}


		if (!_connQSY->PutConnStrInfo(_connQSY_cfg)) {
			_LastErrS = L"ȫˮ���м�����Ӷ�������ʧ��:" + _connQSY->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

		if (!_connQSY->Conn()) {
			_LastErrS = "ȫˮ���м�����Ӷ�������ʧ��:" + _connQSY->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

	}
	catch (...) {
		_LastErrS = "����ȫˮ���м��ʱ�����쳣:" + _connQSY->GetLastErr();
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
	return true;
}
bool TServBase::ConnCYG() {
	try {
		if (NULL == _connCYG) {
			_LastErrS = "�������м�����Ӷ�������ʧ��";
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

		if (!_connCYG->PutConnStrInfo(_connCYG_cfg)) {
			_LastErrS = L"�������м�����Ӷ�������ʧ��:" + _connCYG->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

		if (!_connCYG->Conn()) {
			_LastErrS = "�������м�����Ӷ�������ʧ��:" + _connCYG->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}

	}
	catch (...) {
		_LastErrS = "���Ӵ������м��ʱ�����쳣:" + _connCYG->GetLastErr();
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
	return true;
}

bool TServBase::JsonToList(Json::Value &jv, UTF8String name) {
	UTF8String str = NULL;

	if (jv.isString()) { // ---ԭ��

		str = name + L"=" + jv.asCString();
	}
	else if (jv.isInt() || jv.isNull()) { // ---int bool double null Ϊ���

		str = name + L"=" + IntToStr(jv.asInt());
	}
	else if (jv.isBool()) {

		str = name + L"=" + BoolToStr(jv.asInt());
	}
	else if (jv.isDouble()) {

		str = name + L"=" + FloatToStr(jv.asInt());
	}
	else if (jv.isArray()) {

		int n = jv.size();
		for (int i = 0; i < n; i++) {
			JsonToList(jv[i], name + L"[" + String(i) + L"]");
		}
	}
	else if (jv.isObject()) {
		for (Json::Value::iterator it = jv.begin(); it != jv.end(); it++) {
			Json::Value v = *it;
			JsonToList(v, name.IsEmpty() ? UTF8String(it.memberName()) :
				name + L"." + it.memberName());
		}
	}
	else {
		_LastErrS = "JsonToList:Json�ṹ����";
		return false;
	}

	if (str.Pos("JsonData[") > 0) {
		int rowNum;
		static int rowNum_old = -1;
		try {
			rowNum = str.SubString(str.Pos("JsonData[") + 9, 1).ToInt();
		}
		catch (Exception &e) {
			_LastErrS = "JsonToList:" + e.Message;
			return false;
		};

		str.Delete(1, str.Pos("."));

		if (rowNum_old != rowNum) {

			_ListDS.IsEmpty() ? _ListDS = str : _ListDS += L"\r\n" + str;

			rowNum_old = rowNum;

		}
		else if (rowNum_old == rowNum) { // ---ͬһ��
			_ListDS += L"|" + str;
		}
	}

	return true;
}

/*
 �ݹ�ķ�ʽ
 */
bool TServBase::JsonToDataSet_pt(Json::Value &jv, UTF8String name,
	TClientDataSet *CDS) {
	UTF8String str = NULL;
	static int rowNum_old;

	if (jv.isString()) { // ---ԭ��

		str = name + L"=" + jv.asCString();
	}
	else if (jv.isInt() || jv.isNull()) { // ---int bool double null Ϊ���

		str = name + L"=" + IntToStr(jv.asInt());
	}
	else if (jv.isBool()) {

		str = name + L"=" + BoolToStr(jv.asInt());
	}
	else if (jv.isDouble()) {

		str = name + L"=" + FloatToStr(jv.asInt());
	}
	else if (jv.isArray()) {

		int n = jv.size();
		for (int i = 0; i < n; i++) {
			if (0 == i) {
				rowNum_old = -1;
				CDS->Close();
				CDS->FieldDefs->Clear();
				Json::Value::Members mb = jv[i].getMemberNames();
				for (Json::Value::Members::iterator it = mb.begin();
				it != mb.end(); it++) {
					Json::Value val = *it;
					UTF8String colName = val.asCString();
					CDS->FieldDefs->Add(colName.Trim(), ftString, 48);
				}
				CDS->CreateDataSet();
				CDS->Open();
			}
			JsonToDataSet_pt(jv[i], name + L"[" + String(i) + L"]", CDS);
		}
	}
	else if (jv.isObject()) {
		for (Json::Value::iterator it = jv.begin(); it != jv.end(); it++) {
			Json::Value v = *it;
			JsonToDataSet_pt(v, name.IsEmpty() ? UTF8String(it.memberName()) :
				name + L"." + it.memberName(), CDS);
		}
	}
	else {
		_LastErrS = "JsonToList:Json�ṹ����";
		return false;
	}

	if (str.Pos("JsonData[") > 0) {
		int rowNum;
		try {
			rowNum = str.SubString(str.Pos("JsonData[") + 9, 1).ToInt();
		}
		catch (Exception &e) {
			_LastErrS = "JsonToList:" + e.Message;
			return false;
		};

		str.Delete(1, str.Pos("."));

		if (rowNum_old != rowNum) {

			/* if(rowNum_old != -1){
			 CDS->Post();
			 } */

			CDS->Append();
			// CDS->FieldByName(str.SubString(1,str.Pos("=")-1))->AsString =str.Delete(1,str.Pos("="));
			rowNum_old = rowNum;
		}
		String strField = str.SubString(1, str.Pos("=") - 1);
		String strVal = str.Delete(1, str.Pos("="));
		try {
			CDS->FieldValues[strField] = Variant(strVal);
		}
		catch (Exception &e) {
			_LastErrS = e.Message;
			return false;
		}
	}
	return true;
}

// ---�������
void TServBase::MakeDBGridColumnsAutoFixItsWidth(TDBGrid *grid) {
	int cc;
	int i, tmpLength;
	TDataSet *ds;
	int *aDgCLength;

	cc = grid->Columns->Count;
	ds = grid->DataSource->DataSet;
	aDgCLength = new int[cc];

	for (int i = 0; i < cc; i++) {

		aDgCLength[i] = grid->Columns->Items[i]->Title->Caption.Length();
	}

	ds->First();
	while (!ds->Eof) {
		for (int i = 0; i < cc; i++) {

			tmpLength = ds->Fields->Fields[i]->AsString.Length();
			if (tmpLength > aDgCLength[i]) {
				aDgCLength[i] = tmpLength;
			}
		}
		ds->Next();
	}

	for (int i = 0; i < cc; i++) {

		grid->Columns->Items[i]->Width = aDgCLength[i] * 8;
	}
}

/*
 nStyleType 0:ȫ��(������) 3������
 */
void TServBase::FormStyle(TForm *Form, int nStyleType) {
	switch (nStyleType) {

	case 0:
		// ---form Position
		Form->Top = 0;
		Form->Left = 0;

		// ---Go full screen
		// Form->ClientWidth =Screen->Width;	//ʹ���Դ�Ƥ����,�ŵ�BorderStyle��������,��֪Ϊ��???
		// Form->ClientHeight =Screen->Height;
		Form->Width = Screen->Width;
		Form->Height = Screen->Height;

		Form->BorderStyle = Forms::bsNone;
		Form->DefaultMonitor = dmDesktop;
		// Form->WindowState =wsMaximized;
		break;
	case 3:
		Form->ClientWidth = 810;
		Form->ClientHeight = 500;
		Form->Top = (Screen->Height - Form->Height) * 0.5;
		Form->Left = (Screen->Width - Form->Width) * 0.5;
		// Form->Position =poScreenCenter;	//---ִ�к󴰿���˸,Ϊ�Σ�����
		break;
	}

	Form->Refresh();
	SetForegroundWindow(Form->Handle);
	SetActiveWindow(Forms::Application->Handle);
}

void TServBase::CreateSelection
	(TWinControl *Parent /* ,DynamicArray<String> btn_Caption */) {
	_Selection = new TSelectionPanel(Parent);
}

// ---��TClientDataSet���л�ȡ�������е�ú��
void TServBase::GetAllMeikFromDS(DynamicArray<String> &MeikList) {
	_dsMkGys->Close();
	_dsMkGys->Filtered = false; // ---�رչ�������
	_dsMkGys->Open();
	MeikList.Length = _dsMkGys->RecordCount;
	_dsMkGys->First();
	for (int i = 0; i < _dsMkGys->RecordCount; i++) {

		MeikList[i] = _dsMkGys->FieldByName(L"MKMINGK")->AsString.Trim();
		_dsMkGys->Next();
	}
}

// ---��TClientDataSet���и���ú��������ȡ��Ӧ��
void TServBase::GetGongysFromDS(String Meik, DynamicArray<String> &GongysList) {
	_dsMkGys->Close();
	_dsMkGys->Filter = L"MKMINGK='" + Meik + L"'";
	_dsMkGys->Filtered = true; // ---�򿪹�������
	_dsMkGys->Open();
	GongysList.Length = _dsMkGys->RecordCount;

	_dsMkGys->First();
	for (int i = 0; i < _dsMkGys->RecordCount; i++) {

		GongysList[i] = _dsMkGys->FieldByName(L"GYSMINGC")->AsString.Trim();
		_dsMkGys->Next();
	}
}

// ---��TClientDataSet���и���ú��͹�Ӧ��������ȡú��
void TServBase::GetPinzFromDS(String Meik, String Gongys,
	DynamicArray<String> &PinzList) {
	_dsMkGys->Close();
	_dsMkGys->Filter = L"MKMINGK='" + Meik + L"' and GYSMINGC='" +
		Gongys + L"' ";
	_dsMkGys->Filtered = true; // ---�򿪹�������
	_dsMkGys->Open();
	PinzList.Length = _dsMkGys->RecordCount;

	_dsMkGys->First();
	for (int i = 0; i < _dsMkGys->RecordCount; i++) {

		PinzList[i] = _dsMkGys->FieldByName(L"WLMIMGC")->AsString.Trim();
		_dsMkGys->Next();
	}
}

// ---д������ϢͼƬ
void TServBase::WritePic_DDInfo() {
	int nCars = _CarList->Count;
	int nPages = _CarList->Count * 0.125 + 1; // ---ÿ��������ʾ8������
	int nWidth = _DevInfo.LedInfo[0].Width;
	int nHeight = _DevInfo.LedInfo[0].Height;
	int nRowHeight = nHeight * 0.2;
	int nColWidth = nWidth / 3;
	for (int i = 0; i < nPages && i < 8; i++) {

		int x(0), y(0);
		_LedBmp._bmDD[i]->Width = nWidth;
		_LedBmp._bmDD[i]->Height = nHeight;
		_LedBmp._bmDD[i]->PixelFormat = pf24bit;
		_LedBmp._bmDD[i]->Canvas->Brush->Color = clBlack;
		_LedBmp._bmDD[i]->Canvas->Pen->Color = clBlack;
		_LedBmp._bmDD[i]->Canvas->Rectangle(0, 0, nWidth, nHeight);
		_LedBmp._bmDD[i]->Canvas->Pen->Color = clRed;
		for (int j = 0; j < 4; j++) {
			_LedBmp._bmDD[i]->Canvas->MoveTo(0, nRowHeight*(j + 1));
			j % 2 == 1 ? _LedBmp._bmDD[i]->Canvas->LineTo(nColWidth * 2,
				nRowHeight * (j + 1)) : _LedBmp._bmDD[i]->Canvas->LineTo(nWidth,
				nRowHeight * (j + 1));
		}

		x = nColWidth;
		y = nRowHeight;
		_LedBmp._bmDD[i]->Canvas->MoveTo(x, y);
		_LedBmp._bmDD[i]->Canvas->LineTo(x, nHeight);
		x = nColWidth * 2;
		y = 0;
		_LedBmp._bmDD[i]->Canvas->MoveTo(x, y);
		_LedBmp._bmDD[i]->Canvas->LineTo(x, nHeight);

		_LedBmp._bmDD[i]->Canvas->Font->Size = 10;
		_LedBmp._bmDD[i]->Canvas->Font->Name = "����";
		_LedBmp._bmDD[i]->Canvas->Font->Color = clRed;
		x = 6;
		y = 4;
		_LedBmp._bmDD[i]->Canvas->TextOutW(x, y,
			L"���³��������(P" + IntToStr(i + 1) + L")");

		_LedBmp._bmDD[i]->Canvas->Font->Size = 8;
		x = nColWidth * 2 + 2;
		y = nRowHeight + 4;
		_LedBmp._bmDD[i]->Canvas->TextOutW(x, y,
			L"�ܳ�λ " + IntToStr(_ParkingNum_Total));
		x = nColWidth * 2 + 2;
		y = nRowHeight * 2 + 4;
		_LedBmp._bmDD[i]->Canvas->TextOutW(x, y,
			L"ʣ�೵λ " + IntToStr(_ParkingNum_Surplus));
		x = nColWidth * 2 + 2;
		y = nRowHeight * 3 + 4;
		_LedBmp._bmDD[i]->Canvas->TextOutW(x, y,
			L"�ѵ��� " + IntToStr(_ScheduledNum));
		x = nColWidth * 2 + 2;
		y = nRowHeight * 4 + 4;
		_LedBmp._bmDD[i]->Canvas->TextOutW(x, y,
			"δ���� " + IntToStr(_ToScheduleNum));

		_LedBmp._bmDD[i]->Canvas->Font->Size = 10;
		int nPageCars = nCars > 8 * (i + 1) ? 8 : nCars - 8 * i;
		for (int j = 0; j < nPageCars; j++) {

			x = nColWidth * (j % 2) + 2;
			y = nRowHeight + nRowHeight * (j / 2) + 4;
			_LedBmp._bmDD[i]->Canvas->TextOutW(x, y,
				_CarList->Strings[8*i + j]);
		}
#ifdef _DEBUG
		_LedBmp._bmDD[i]->SaveToFile(L"D:\\testLED" + IntToStr(i) + L".bmp");
#endif
		_LedBmp.picNumDD = i + 1;
	}
}

// ---д������ϢͼƬ
void TServBase::WritePic_GGInfo() {
	int nTitleWords = _GGTitle.Length(); // ---��������
	int nTextWords = _GGText.Length() + 2; // ---��������
	int nWordsOfUsed = 0; // ---��д������
	int nRowWords = 17; // ---ÿ������
	int nPages = nTextWords / (nRowWords * 5) + 1; // ---ÿ��19�ַ���5�У�8�����ģ�
	int nWidth = _DevInfo.LedInfo[0].Width;
	int nHeight = _DevInfo.LedInfo[0].Height;
	int nTitleRowHeight = nHeight * 0.2; // ---�����и�
	int nTextRowHeight = (nHeight - nTitleRowHeight * 2) / 4.9; ;

	for (int i = 0; i < nPages && i < 8; i++) {

		int x(6), y(4);
		_LedBmp._bmGG[i]->Width = nWidth;
		_LedBmp._bmGG[i]->Height = nHeight;
		_LedBmp._bmGG[i]->PixelFormat = pf24bit;
		_LedBmp._bmGG[i]->Canvas->Brush->Color = clBlack;
		_LedBmp._bmGG[i]->Canvas->Pen->Color = clBlack;
		_LedBmp._bmGG[i]->Canvas->Rectangle(0, 0, nWidth, nHeight);
		_LedBmp._bmGG[i]->Canvas->Pen->Color = clRed;

		_LedBmp._bmGG[i]->Canvas->Font->Size = 10;
		_LedBmp._bmGG[i]->Canvas->Font->Name = "����";
		_LedBmp._bmGG[i]->Canvas->Font->Color = clRed;

		x = (nWidth - nTitleWords * 13) / 2;
		_LedBmp._bmGG[i]->Canvas->TextOutW(x, y, _GGTitle);

		_LedBmp._bmGG[i]->Canvas->MoveTo(0, nTitleRowHeight);
		_LedBmp._bmGG[i]->Canvas->LineTo(nWidth, nTitleRowHeight);

		// ---����
		_LedBmp._bmGG[i]->Canvas->Font->Size = 8;
		int nRows = nTextWords - nRowWords * 5 * i > nRowWords * 5 ? 5 :
			(nTextWords - nRowWords * 5 * i) / nRowWords + 1; // ---��ǰҳ����
		for (int j = 0; j < nRows; j++) {

			y = nTitleRowHeight + nTextRowHeight * j + 4;
			if (0 == i && 0 == j) {

				x = 22;
				_LedBmp._bmGG[i]->Canvas->TextOutW(x, y,
					_GGText.SubString(1, nRowWords - 2));
				nWordsOfUsed = nRowWords - 2;
			}
			else {
				x = 2;
				_LedBmp._bmGG[i]->Canvas->TextOutW(x, y,
					_GGText.SubString(nWordsOfUsed + 1, nRowWords));
				nWordsOfUsed += nRowWords;
			}
		}
		_LedBmp._bmGG[i]->Canvas->MoveTo(0, nTitleRowHeight*4 + 2);
		_LedBmp._bmGG[i]->Canvas->LineTo(nWidth, nTitleRowHeight*4 + 2);
		x = 25;
		y = nHeight - nTitleRowHeight + 4;
		_LedBmp._bmGG[i]->Canvas->TextOutW(x, y,
			L"���ţ�ȼ�ϲ� " + Now().FormatString(L"yy-MM-dd HH:mm"));
#ifdef _DEBUG
		_LedBmp._bmGG[i]->SaveToFile(L"D:\\testLEDGG" + IntToStr(i) + L".bmp");
#endif
		_LedBmp.picNumGG = i + 1;
	}
}