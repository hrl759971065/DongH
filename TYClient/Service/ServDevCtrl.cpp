// ---------------------------------------------------------------------------

#pragma hdrstop

#include "ServDevCtrl.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)

TServDevCtrl::TServDevCtrl() {
	_sendStr_Old[0] = "";
	_sendStr_Old[1] = "";
}

TServDevCtrl::~TServDevCtrl() {

}

bool TServDevCtrl::OpenMeter() {
	if (NULL == _Meter)
		_Meter = (TCarMeter*)CreateDevice(CARMETER, "通用");
	if (_Meter->SetMeterComm(_DevInfo.BalanceInfo)) {
		if (!_Meter->OpenMeter()) {
			_LastErrS = _Meter->GetLastErrS();
			return false;
		}
		else
			return true;
	}
	else {
		_LastErrS = "地磅仪表初始化失败:" + _Meter->GetLastErrS();
		return false;
	}
}

/*
 Index
 0:发卡机 停车场入口 采样机 RFID
 1:停车场入口
 2:发卡机 停车场入口 采样机 地磅 IC卡机
 3:停车场出口IC卡机
 */
bool TServDevCtrl::OpenReader(int Index) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器停用";
		return false;
	}

	if (NULL == _Reader[Index])
		_Reader[Index] = (TReaders*)CreateDevice(READER,
		AnsiString(_DevInfo.ReaderInfo[Index].Model).c_str());
	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器打开失败,型号配置错误:" +
			_DevInfo.ReaderInfo[Index].Model;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
	if (_Reader[Index]->PutCfgInfo(_DevInfo.ReaderInfo[Index])) {
		if (!_Reader[Index]->OpenReader()) {
			_LastErrS = _Reader[Index]->GetLastErrS();
			return false;
		}
		else
			return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"号读写器初始化失败:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
}

bool TServDevCtrl::CloseReader(int Index) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器停用";
		return false;
	}

	if (NULL != _Reader[Index])
		return _Reader[Index]->CloseReader();
	_LastErrS = IntToStr(Index + 1) + L"号读写器对象为空,关闭失败";
	return false;
}

bool TServDevCtrl::OpenLED(int Index) {
	if (!_DevInfo.LedInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号Led屏停用";
		return false;
	}

	if (NULL == _Led[Index])
		_Led[Index] = (TLeds*)CreateDevice(LED,
		AnsiString(_DevInfo.LedInfo[Index].Model).c_str());
	if (!_Led[Index]->PutCfgInfo(_DevInfo.LedInfo[Index])) {

		_LastErrS = IntToStr(Index + 1) + L"号LED屏初始化失败:" +
			_Led[Index]->GetLastErrS();
		return false;
	}
	if (!_Led[Index]->Open()) {

		_LastErrS = IntToStr(Index + 1) + L"号LED屏打开失败:" +
			_Led[Index]->GetLastErrS();
		return false;
	}
	return true;
}

bool TServDevCtrl::OpenIOCtrl(int Index) {
	if (NULL == _IOCtrl[Index])
		_IOCtrl[Index] = (TIOCtrl*)CreateDevice(IOCTRL, "通用");

	if (_IOCtrl[Index]->SetIOCtrlComm(_DevInfo.IOCtrlInfo[Index])) {
		if (!_IOCtrl[Index]->OpenIOCtrl()) {
			_LastErrS = IntToStr(Index + 1) + L"号开关模块打开失败:" +
				_IOCtrl[Index]->GetLastErrS();
			return false;
		}
		else
			return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"号开关模块初始化失败:" +
			_IOCtrl[Index]->GetLastErrS();
		return false;
	}
}

// ---打开小票打印机串口
bool TServDevCtrl::OpenPrt() {
	if (NULL == _ComPrt)
		_ComPrt = (TComm*)CreateDevice(COMM, "通用");
	_ComPrt->PortNo = _DevInfo.PrtInfo.CommCfg.ComNum;
	_ComPrt->Baud = _DevInfo.PrtInfo.CommCfg.dwBaudRate;
	_ComPrt->Parity = String(_DevInfo.PrtInfo.CommCfg.Chk);
	_ComPrt->ByteSize = _DevInfo.PrtInfo.CommCfg.DataBits;
	_ComPrt->StopBits = _DevInfo.PrtInfo.CommCfg.Stops;
	if (_ComPrt->Open())
		return true;
	else {
		_LastErrS = L"打印机串口打开失败:" + _ComPrt->GetLastErrs();
		return false;
	}
}

bool TServDevCtrl::ClosePrt() {
	if (NULL == _ComPrt) {
		_LastErrS = L"打印机串口打开失败:对象为空";
		return false;
	}
	return _ComPrt->Close();

}

double TServDevCtrl::GetRealVal() {
	if (NULL == _Meter)
		return -1;
	return (_Meter->RealValue);
}

// ---取地磅稳定值 fVal输出值 strWarn输出警告信息，当返回false时，应参考 _LastErrS的信息
bool TServDevCtrl::GetStaticVal(double &fVal) {
	double array_fVal[64] = {0};
	for (int i = 0; i < 16; i++) {

		int nDWQSta = GetDWQSta();
		if (nDWQSta != ALLPASS && nDWQSta != -2) {

			_LastErrS = L"取稳定值:定位被挡";
			return false;
		}

		array_fVal[i] = _Meter->RealValue;
		if (i > 0) {
			if (array_fVal[i] != array_fVal[0]) {
				_LastErrS = L"取稳定值:数值不稳";
				return false;
			}
		}
		::Sleep(200);
	}
	fVal = array_fVal[0];
	return true;
}

bool TServDevCtrl::IsLeave() {
	if (GetRealVal() < 200 && !(GetRealVal() >= -14 && GetRealVal() <= -1))
		return true;
	else
		return false;
}

bool TServDevCtrl::ReadCardID(int Index, TCardInfo &CardInfo) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器停用";
		return false;
	}

	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器对象为空,读取失败";
		return false;
	}

	if (_DevInfo.ReaderInfo[Index].Model == L"IVT530" ||
		_DevInfo.ReaderInfo[Index].Model == L"K720" ||
		_DevInfo.ReaderInfo[Index].Model == L"RF610") { // ---IC

		unsigned char snr[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		if (_Reader[Index]->ReadCard(CardInfo, 0, snr)) {

			_WorkSta.CardID = String((char *)CardInfo.ICCardID_Hex);
			return true;
		}
		else {
			_LastErrS = IntToStr(Index + 1) + L"号读写器读取失败:" +
				_Reader[Index]->GetLastErrS();
			return false;
		}
	}
	else {
		if (_Reader[Index]->ReadCard(CardInfo)) {

			_WorkSta.CardID = CardInfo.CardID[0];
			return true;
		}
		else {
			_LastErrS = IntToStr(Index + 1) + L"号读写器读取失败:" +
				_Reader[Index]->GetLastErrS();
			return false;
		}
	}
}

// ---出票打磅单时读IC卡
bool TServDevCtrl::ReadICCard(int Index, TCardInfo &CardInfo) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器停用";
		return false;
	}

	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器对象为空,读取失败";
		return false;
	}

	char szPass[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	for (int s = 0; s < 16; s++) {

		for (int b = 0; b < 3; b++) {

			if (!_Reader[Index]->ReadCard_IC(s, b, 0, szPass, CardInfo)) {

				_LastErrS = IntToStr(s) + L"扇区" + IntToStr(b) + L"块读取失败：" +
					GetLastErrS();
				return false;
			}
			::Sleep(30);
		}
	}
	return true;
}

bool TServDevCtrl::WriteICCard(int Index, int nSector, int nBlock,
	AnsiString strKey, AnsiString strData) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器停用";
		return false;
	}

	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器对象为空,读取失败";
		return false;
	}

	TDataSwitch DS;
	unsigned char snr[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	DS.strHexToBuff(strKey, snr);

	unsigned char wData[16] = {0};
	strncpy(wData, strData.c_str(), 16);

	if (!_Reader[Index]->WriteCard_IC(nSector, nBlock, 0, snr, wData)) {

		_LastErrS = IntToStr(Index + 1) + L"号读写器写卡失败:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
	return true;
}

/*
 Index:发卡设备编号 SendType:发卡动作
 K720 SendType 12:发卡到读卡位置
 */
bool TServDevCtrl::SendICCard(int Index, int SendType) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号收发卡器停用";
		return false;
	}
	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"号收发卡器对象为空,读取失败";
		return false;
	}

	if (_Reader[Index]->DeviceControl(SendType)) {

		return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"号收发卡器:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
}

// ---回收IC卡,D3001
bool TServDevCtrl::ReceiveICCard(int Index, int aActionID) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号收发卡器停用";
		return false;
	}
	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"号收发卡器对象为空,读取失败";
		return false;
	}

	if (_Reader[Index]->DeviceControl(aActionID)) {

		return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"号收发卡器:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
}

// ---自助出厂系统的退卡过程,bRec 是否执行弹卡指令，第一次传false
// ---bReturned 卡是否已被弹出
bool TServDevCtrl::RetCardProc_ZZCC(String Reason, bool bRec, bool bReturned) {
	if (_bReset) {
		_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
		_File.WriteLog(L"在退卡的过程中进行强制流程复位");
		_bReset = false;
		return false;
	}

	_File.PlayWav(L"卡片异常", SND_FILENAME | SND_SYNC);
	_WorkSta.StrFlow = Reason + L",请取卡" + Now().TimeString();
	_File.WriteLog_ErrS(Reason + L",请取卡");
	ReceiveICCard(0, 2); // 执行弹卡指令
	_Sequence = 5; // ---弹卡成功

	Sleep(3000);
	_Sequence = 6; // ---卡已取回
	return false;

	if (!bRec) { // ---未执行弹卡指令
		if (!ReceiveICCard(0, 2)) { // ---执行失败
			_WorkSta.StrFlow = Reason + L",弹卡指令失败" + Now().TimeString();
			_File.WriteLog_ErrS(Reason + L",弹卡指令失败:" + GetLastErrS());
			Sleep(1000);
			RetCardProc_ZZCC(Reason, false, false);
			return true;
		}
		else {
			RetCardProc_ZZCC(Reason, true, false);
			return true;
		}
	}
	else {
		if (!bReturned) {
			Sleep(2500);
			// 判断卡的位置
			unsigned char szStatus[6] = {0};
			if (DeviceStatus(0, szStatus)) {

				String strStatus = DevStatusDecode_D3001(szStatus);
				if (strStatus == L"收卡机故障") {

					_WorkSta.StrFlow = strStatus + L"," + Now().TimeString();
					_File.WriteLog(L"RetCardProc_ZZCC:" + strStatus);
					Sleep(500);
					ReceiveICCard(0, 6); // ---复位
					RetCardProc_ZZCC(Reason, true, false);
					return true;

				}
				else if (strStatus != L"传感器1有卡") {
					RetCardProc_ZZCC(Reason, true, false);
					return true;
				}
				else {
					_Sequence = 5; // ---弹卡成功
					_WorkSta.StrFlow = Reason + L",请取卡" + Now().TimeString();
					_File.WriteLog_ErrS(Reason + L",请取卡");
					Sleep(1500);
					RetCardProc_ZZCC(Reason, true, true);
					return true;
				}
			}
			else {
				_WorkSta.StrFlow = L"收卡机状态读取失败:" + GetLastErrS() +
					Now().TimeString();
				_File.WriteLog(L"RetCardProc_ZZCC,收卡机状态读取失败:" + GetLastErrS());
				RetCardProc_ZZCC(Reason, true, false);
				return true;
			}
		}
		else { // ---等待司机取卡
			_File.PlayWav(L"请取卡", SND_FILENAME | SND_SYNC);
			BYTE szStatus[6] = {0};
			if (DeviceStatus(0, szStatus)) {

				String strStatus = DevStatusDecode_D3001(szStatus);
				if (strStatus == L"正常") {

					_Sequence = 6; // ---卡已取回
					return false;
				}
				else {
					_WorkSta.StrFlow = strStatus + "," + Now().TimeString();
					RetCardProc_ZZCC(Reason, true, true);
					return true;
				}
			}
			else {
				_WorkSta.StrFlow = L"收卡机状态读取失败:" + GetLastErrS() +
					Now().TimeString();
				_File.WriteLog(L"RetCardProc_ZZCC,收卡机状态读取失败:" + GetLastErrS());
				RetCardProc_ZZCC(Reason, true, true);
				return true;
			}
		}
	}
}

/*
 发卡装置状态查询
 */
bool TServDevCtrl::DeviceStatus(int Index, unsigned char *outStatus) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号收发卡器停用";
		return false;
	}
	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"号收发卡器对象为空,读取失败";
		return false;
	}

	if (_Reader[Index]->DeviceStatus(0, outStatus)) {

		return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"号收发卡器状态查询失败:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
}

String TServDevCtrl::DevStatusDecode(unsigned char *inStatus) {
	char szStatus[5] = {0};
	strncpy(szStatus, inStatus, 4);
	String Status = String(szStatus);
	if (Status == L"8000")
		return L"回收箱满";
	else if (Status == L"4000")
		return L"准备卡失败";
	else if (Status == L"2000")
		return L"正在准备卡";
	else if (Status == L"1000")
		return L"正在发卡";
	else if (Status == L"0800")
		return L"正在收卡";
	else if (Status == L"0400")
		return L"发卡出错";
	else if (Status == L"0040")
		return L"重叠卡";
	else if (Status == L"0020")
		return L"堵塞卡";
	else if (Status == L"0010")
		return L"卡箱预空";
	else if (Status == L"0018")
		return L"卡箱空"; // ---传感器123都没卡,即没有卡
	else if (Status == L"0016")
		return L"卡片在传感器2-3";
	else if (Status == L"0014")
		return L"卡片在传感器3";
	else if (Status == L"0013")
		return L"卡片在传感器1-2(读卡)";
	else if (Status == L"0002")
		return L"卡片在传感器2";
	else if (Status == L"0011")
		return L"卡片在传感器1";
	else
		return L"未知状态";
}

String TServDevCtrl::DevStatusDecode_D3001(unsigned char *inStatus) {
	char szStatus[8] = {0};
	strncpy(szStatus, inStatus, 6);

	if (0x31 == szStatus[3])
		return L"收卡机故障";
	if (0x31 == szStatus[1])
		return L"卡到位";

	TDataSwitch DS;
	AnsiString strTongd = DS.CharToBinStr(szStatus[0]); // ---第一字节通道状态

	if (strTongd[2] == '1') { // ---通道中有卡

		if (strTongd[8] == '1' && strTongd[7] == '1' && strTongd[6] == '1')
			return L"传感器123有卡";
		else if (strTongd[8] == '1' && strTongd[7] == '1' && strTongd[6] == '0')
			return L"传感器12有卡";
		else if (strTongd[8] == '1' && strTongd[7] == '0' && strTongd[6] == '0')
			return L"传感器1有卡";
		else if (strTongd[8] == '0' && strTongd[7] == '1' && strTongd[6] == '1')
			return L"传感器23有卡";
		else if (strTongd[8] == '1' && strTongd[7] == '0' && strTongd[6] == '1')
			return L"传感器13有卡";
	}
	if (strTongd[2] == '0')
		return L"正常"; // ---机器正常，且通道中无卡
	return L"未知状态";
}

bool TServDevCtrl::SendLed(int Index, String str) {
	if (!_DevInfo.LedInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号LED屏停用";
		_WorkSta.StrLed[Index] = IntToStr(Index + 1) + L"号LED停用";
		return false;
	}

	if (_sendStr_Old[Index] == str)
		return true;

	bool bRet;
	bRet = _Led[Index]->SendStr(str);
	if (bRet) {
		_WorkSta.StrLed[Index] = str;
		_sendStr_Old[Index] = str;
	}
	else {
		_WorkSta.StrLed[Index] = _Led[Index]->GetLastErrS();
	}
	return bRet;
}

bool TServDevCtrl::SendPicToLedRam(int Index, HBITMAP hBitmap, int nPosX,
	int nPosY, int Width, int Height) {
	if (!_DevInfo.LedInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"号LED屏停用";
		_WorkSta.StrLed[Index] = IntToStr(Index + 1) + L"号LED停用";
		return false;
	}

	return _Led[Index]->SendPicToRam_2(hBitmap, nPosX, nPosY, Width, Height);
}

// --- -2不使用定位 -1超时，0全通 1前挡 2后挡 3全挡
long TServDevCtrl::GetDWQSta() {
	long nDWQ;
	for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) {

		if (_DevInfo.IOCtrlInfo[i].Use) {

			if (_DevInfo.IOCtrlInfo[i].UseDWQ) {

				nDWQ = _IOCtrl[i]->DWQStatus;
				if (-1 == nDWQ) {
					_WorkSta.DWQSta_Head = -1;
					_WorkSta.DWQSta_Tail = -1;
				}
				else if (0 == nDWQ) {
					_WorkSta.DWQSta_Head = 0;
					_WorkSta.DWQSta_Tail = 0;
				}
				else if (1 == nDWQ) {
					_WorkSta.DWQSta_Head = 1;
					_WorkSta.DWQSta_Tail = 0;
				}
				else if (2 == nDWQ) {
					_WorkSta.DWQSta_Head = 0;
					_WorkSta.DWQSta_Tail = 1;
				}
				else if (3 == nDWQ) {
					_WorkSta.DWQSta_Head = 1;
					_WorkSta.DWQSta_Tail = 1;
				}
				return nDWQ;
			}
		}
	}
	_WorkSta.DWQSta_Head = -2;
	_WorkSta.DWQSta_Tail = -2;
	return -2;
}

// ---定位器检测过程
bool TServDevCtrl::DWQCheckProc(int nHeadDZFlag, String Numplate) {
	int nDWQSta;
	do {
		nDWQSta = GetDWQSta();
		if (-1 == nDWQSta) {
			_File.PlayWav(L"定位器异常.wav", SND_FILENAME | SND_SYNC);
			::Sleep(50);
			_WorkSta.StrFlow = L"定位数据超时...";
			SendLed(0, L"定位器数据超时");
			::Sleep(1000);
		}
		else if (HEADMISS == nDWQSta) {
			if (HEAD == nHeadDZFlag) {
				_File.PlayWav("定位器被挡.wav", SND_FILENAME | SND_SYNC);
				::Sleep(50);
				_WorkSta.StrFlow = L"车头挡定位...";
				SendLed(0, L"车头挡定位");
			}
			else {
				_File.PlayWav("定位器被挡.wav", SND_FILENAME | SND_SYNC);
				::Sleep(50);
				_WorkSta.StrFlow = L"车尾挡定位...";
				SendLed(0, L"车尾挡定位");
			}
		}
		else if (TAILMISS == nDWQSta) {
			if (TAIL == nHeadDZFlag) {
				_File.PlayWav("定位器被挡.wav", SND_FILENAME | SND_SYNC);
				::Sleep(50);
				_WorkSta.StrFlow = L"车头挡定位...";
				SendLed(0, L"车头挡定位");
			}
			else {
				_File.PlayWav("定位器被挡.wav", SND_FILENAME | SND_SYNC);
				::Sleep(50);
				_WorkSta.StrFlow = L"车尾挡定位...";
				SendLed(0, L"车尾挡定位");
			}
		}
		else if (ALLMISS == nDWQSta) {
			_File.PlayWav(L"定位器被挡.wav", SND_FILENAME | SND_SYNC);
			::Sleep(50);
			_WorkSta.StrFlow = "前后挡定位...";
			SendLed(0, "前后挡定位");
		}

		if (IsLeave() && nDWQSta != -2) {
			_WorkSta.StrFlow = L"定位时车辆下磅...";
			OutCtrl(DZ, TAIL, UP);
			_File.WriteLog(Numplate + L"定位时车辆下磅");
			return false;
		}
		if (_Thread->GetTerminated()) {
			_File.WriteLog(Numplate + L"定位时线程中止");
			_WorkSta.StrFlow = L"线程中止...";
			return false;
		}

	}
	while (nDWQSta != ALLPASS && nDWQSta != -2); // --- -2代表停用设备

	return true;
}

/***************************************************************************
 Function:	OutCtrl
 Param：     nDevice 设备代号 0道闸 1红绿灯
 nPos 位置代号，0前 1后;
 nAction 动作代号 0抬 1落  0红 1绿 2关
 Returns:	无
 ************************************************************************** */
void TServDevCtrl::OutCtrl(int nDev, int nPos, int nAction) {
	if (DZ == nDev) {

		if (HEAD == nPos) {

			for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) { // 查找可用控制设备

				if (_DevInfo.IOCtrlInfo[i].Use) {

					if (_DevInfo.IOCtrlInfo[i].UseDZ_Head) {

						_IOCtrl[i]->OutCtrl(nDev, nPos, nAction);
						_WorkSta.DZSta_Head = nAction;
						break;
					}
				}
			}
		}
		else if (TAIL == nPos) {

			for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) { // 查找可用控制设备

				if (_DevInfo.IOCtrlInfo[i].Use) {

					if (_DevInfo.IOCtrlInfo[i].UseDZ_Tail) {

						_IOCtrl[i]->OutCtrl(nDev, nPos, nAction);
						_WorkSta.DZSta_Tail = nAction;
						break;
					}
				}
			}
		}

	}
	else if (HLD == nDev) {

		if (HEAD == nPos) {

			for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) { // 查找可用控制设备

				if (_DevInfo.IOCtrlInfo[i].Use) {

					if (_DevInfo.IOCtrlInfo[i].UseHLD_Head) {

						_IOCtrl[i]->OutCtrl(nDev, nPos, nAction);
						_WorkSta.HLDSta_Head = nAction;
						break;
					}
				}
			}
		}
		else if (TAIL == nPos) {

			for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) { // 查找可用控制设备

				if (_DevInfo.IOCtrlInfo[i].Use) {

					if (_DevInfo.IOCtrlInfo[i].UseHLD_Tail) {

						_IOCtrl[i]->OutCtrl(nDev, nPos, nAction);
						_WorkSta.HLDSta_Tail = nAction;
						break;
					}
				}
			}
		}
	}
}

void TServDevCtrl::HLDCtrl(int nHead_cmd, int nTail_cmd) {
	OutCtrl(HLD, HEAD, nHead_cmd);
	::Sleep(100);
	OutCtrl(HLD, TAIL, nTail_cmd);
	::Sleep(100);
}

// ---计量成功或失败后,车辆下磅的过程
// ---wavName wav文件名,strLed LED屏内容 ,strSta界面状态内容 strList计量列表信息
// ---nHeadDZFlag 前杆标志  bHeadDZIsUp 是否抬前杆 bTailDZIsUp是否抬后杆
void TServDevCtrl::FinishProc(String wavName, String strLed, String strSta,
	String strList, bool bHeadDZIsUp, bool bTailDZIsUp) {
	String FormCaption = _sysInfo.Title + L"  " + _sysInfo.Location;
	HWND hWnd = ::FindWindow(L"TFrmMain", FormCaption.c_str());

	_WorkSta.StrFlow = strSta;

	PostMessage(hWnd, WM_JILLISTMSG, strList.Trim().Length(),
		(long)strList.c_str());
	_File.WriteLog(strList);
	_File.PlayWav(wavName);
	::Sleep(100);
	do {
		if (strLed.Pos("|") > 0) {
			SendLed(0, strLed.SubString(1, strLed.Pos("|") - 1));
			strLed.Delete(1, strLed.Pos("|"));
		}
		else {
			SendLed(0, strLed);
			strLed.Delete(1, strLed.Length());
		}
		::Sleep(4000);
	}
	while (!strLed.IsEmpty());

	::Sleep(1000);
	if (bHeadDZIsUp)
		OutCtrl(DZ, HEAD, UP);
	::Sleep(100);
	if (bTailDZIsUp)
		OutCtrl(DZ, TAIL, UP); // ---需要开后门(地感线圈)
	::Sleep(100);

	while (!IsLeave()) { // ---车量不下磅，一直循环

		if (_Thread->GetTerminated()) {

			_WorkSta.StrFlow = "线程中止...";
			return;
		}
		::Sleep(1000);
	}
}

// ---磅单打印
void TServDevCtrl::PrintBangd(TBangdInfo bdInfo) {
	bdInfo.Title = "       包头东华热电有限公司";
	bdInfo.Printer = _sysInfo.Location;
	AnsiString Text;
	Text = "\r\n      " + bdInfo.Title + "\r\n\r\n\r\n";
	Text = "\r\n             汽车煤计量票据 \r\n\r\n\r\n";
	Text += "  -------------------------------\r\n\r\n";
	Text += "  车 牌 号：" + bdInfo.Numplate + "\r\n\r\n";
	Text += "  供 应 商：" + bdInfo.Gongysmc + "\r\n\r\n";
	// Text +="  煤    矿："+bdInfo.Meikmc+"\r\n\r\n";
	Text += "  煤    种：" + bdInfo.Wulmc + "\r\n\r\n";
	Text += "  毛    重：" + Format("%.2f", ARRAYOFCONST((bdInfo.Maoz))) +
		"\r\n\r\n";
	Text += "  毛重时间：" + bdInfo.Zhongcsj + "\r\n\r\n";
	Text += "  皮    重：" + Format("%.2f", ARRAYOFCONST((bdInfo.Piz))) +
		"\r\n\r\n";
	Text += "  皮重时间：" + bdInfo.Qingcsj + "\r\n\r\n";
	Text += "  扣    矸：" + FloatToStr(bdInfo.Koug) + "\r\n\r\n";
	Text += "  扣    杂：" + FloatToStr(bdInfo.Kouz) + "\r\n\r\n";
	Text += "  扣    水：" + FloatToStr(bdInfo.Kous) + "\r\n\r\n";
	Text += "  净    重：" + Format("%.2f", ARRAYOFCONST((bdInfo.Jingz))) +
		"\r\n\r\n";
	Text += "  防 伪 码：" + bdInfo.JilsjID + "\r\n\r\n";
	Text += "  -------------------------------\r\n\r\n";
	Text += "  出 票 人：" + bdInfo.Printer + "\r\n\r\n";
	Text += "  出票时间：" + Now().FormatString("yyyy-MM-dd HH:mm:ss") +
		"\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n";

	int size = Text.Length();
	unsigned char *szPrt = new char[size + 1];
	memset(szPrt, 0, size);
	strncpy(szPrt, Text.c_str(), size);

	_ComPrt->Write(Text.c_str(), Text.Length());
	::Sleep(2000);
	_ComPrt->Write("\x1B\x40\x1B\x69\x01", 5);
	delete szPrt;
}

// ---停车场或煤场进出成功或失败后,车辆离开的过程
// ---wavName wav文件名,strLed LED屏内容 ,strSta界面状态内容 strList列表信息
// ---nHeadDZFlag 前杆标志  bHeadDZIsUp 是否抬前杆 bTailDZIsUp是否抬后杆
void TServDevCtrl::FinishCRC(String wavName, String strLed, String strSta,
	String strList, bool bHeadDZIsUp, bool bTailDZIsUp, int LedAdr) {
	String FormCaption = _sysInfo.Title + L"  " + _sysInfo.Location;
	HWND hWnd = ::FindWindow(L"TFrmMain", FormCaption.c_str());

	_WorkSta.StrFlow = strSta;

	PostMessage(hWnd, WM_JILLISTMSG, strList.Trim().Length(),
		(long)strList.c_str());
	_File.WriteLog(strList);
	_File.PlayWav(wavName);
	::Sleep(100);
	do {
		if (strLed.Pos("|") > 0) {
			SendLed(LedAdr, strLed.SubString(1, strLed.Pos("|") - 1));
			strLed.Delete(1, strLed.Pos("|"));
		}
		else {
			SendLed(LedAdr, strLed);
			strLed.Delete(1, strLed.Length());
		}
		::Sleep(4000);
	}
	while (!strLed.IsEmpty());

	::Sleep(1000);
	if (bHeadDZIsUp)
		OutCtrl(DZ, HEAD, UP);
	::Sleep(100);
	if (bTailDZIsUp)
		OutCtrl(DZ, TAIL, UP); // ---需要开后门(地感线圈)
	::Sleep(100);

	if (_Thread->GetTerminated()) {

		_WorkSta.StrFlow = "线程中止...";
		return;
	}
	::Sleep(10000);
}
