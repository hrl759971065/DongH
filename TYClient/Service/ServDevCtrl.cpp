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
		_Meter = (TCarMeter*)CreateDevice(CARMETER, "ͨ��");
	if (_Meter->SetMeterComm(_DevInfo.BalanceInfo)) {
		if (!_Meter->OpenMeter()) {
			_LastErrS = _Meter->GetLastErrS();
			return false;
		}
		else
			return true;
	}
	else {
		_LastErrS = "�ذ��Ǳ���ʼ��ʧ��:" + _Meter->GetLastErrS();
		return false;
	}
}

/*
 Index
 0:������ ͣ������� ������ RFID
 1:ͣ�������
 2:������ ͣ������� ������ �ذ� IC����
 3:ͣ��������IC����
 */
bool TServDevCtrl::OpenReader(int Index) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д��ͣ��";
		return false;
	}

	if (NULL == _Reader[Index])
		_Reader[Index] = (TReaders*)CreateDevice(READER,
		AnsiString(_DevInfo.ReaderInfo[Index].Model).c_str());
	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д����ʧ��,�ͺ����ô���:" +
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
		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д����ʼ��ʧ��:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
}

bool TServDevCtrl::CloseReader(int Index) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д��ͣ��";
		return false;
	}

	if (NULL != _Reader[Index])
		return _Reader[Index]->CloseReader();
	_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д������Ϊ��,�ر�ʧ��";
	return false;
}

bool TServDevCtrl::OpenLED(int Index) {
	if (!_DevInfo.LedInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"��Led��ͣ��";
		return false;
	}

	if (NULL == _Led[Index])
		_Led[Index] = (TLeds*)CreateDevice(LED,
		AnsiString(_DevInfo.LedInfo[Index].Model).c_str());
	if (!_Led[Index]->PutCfgInfo(_DevInfo.LedInfo[Index])) {

		_LastErrS = IntToStr(Index + 1) + L"��LED����ʼ��ʧ��:" +
			_Led[Index]->GetLastErrS();
		return false;
	}
	if (!_Led[Index]->Open()) {

		_LastErrS = IntToStr(Index + 1) + L"��LED����ʧ��:" +
			_Led[Index]->GetLastErrS();
		return false;
	}
	return true;
}

bool TServDevCtrl::OpenIOCtrl(int Index) {
	if (NULL == _IOCtrl[Index])
		_IOCtrl[Index] = (TIOCtrl*)CreateDevice(IOCTRL, "ͨ��");

	if (_IOCtrl[Index]->SetIOCtrlComm(_DevInfo.IOCtrlInfo[Index])) {
		if (!_IOCtrl[Index]->OpenIOCtrl()) {
			_LastErrS = IntToStr(Index + 1) + L"�ſ���ģ���ʧ��:" +
				_IOCtrl[Index]->GetLastErrS();
			return false;
		}
		else
			return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"�ſ���ģ���ʼ��ʧ��:" +
			_IOCtrl[Index]->GetLastErrS();
		return false;
	}
}

// ---��СƱ��ӡ������
bool TServDevCtrl::OpenPrt() {
	if (NULL == _ComPrt)
		_ComPrt = (TComm*)CreateDevice(COMM, "ͨ��");
	_ComPrt->PortNo = _DevInfo.PrtInfo.CommCfg.ComNum;
	_ComPrt->Baud = _DevInfo.PrtInfo.CommCfg.dwBaudRate;
	_ComPrt->Parity = String(_DevInfo.PrtInfo.CommCfg.Chk);
	_ComPrt->ByteSize = _DevInfo.PrtInfo.CommCfg.DataBits;
	_ComPrt->StopBits = _DevInfo.PrtInfo.CommCfg.Stops;
	if (_ComPrt->Open())
		return true;
	else {
		_LastErrS = L"��ӡ�����ڴ�ʧ��:" + _ComPrt->GetLastErrs();
		return false;
	}
}

bool TServDevCtrl::ClosePrt() {
	if (NULL == _ComPrt) {
		_LastErrS = L"��ӡ�����ڴ�ʧ��:����Ϊ��";
		return false;
	}
	return _ComPrt->Close();

}

double TServDevCtrl::GetRealVal() {
	if (NULL == _Meter)
		return -1;
	return (_Meter->RealValue);
}

// ---ȡ�ذ��ȶ�ֵ fVal���ֵ strWarn���������Ϣ��������falseʱ��Ӧ�ο� _LastErrS����Ϣ
bool TServDevCtrl::GetStaticVal(double &fVal) {
	double array_fVal[64] = {0};
	for (int i = 0; i < 16; i++) {

		int nDWQSta = GetDWQSta();
		if (nDWQSta != ALLPASS && nDWQSta != -2) {

			_LastErrS = L"ȡ�ȶ�ֵ:��λ����";
			return false;
		}

		array_fVal[i] = _Meter->RealValue;
		if (i > 0) {
			if (array_fVal[i] != array_fVal[0]) {
				_LastErrS = L"ȡ�ȶ�ֵ:��ֵ����";
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

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д��ͣ��";
		return false;
	}

	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д������Ϊ��,��ȡʧ��";
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
			_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д����ȡʧ��:" +
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
			_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д����ȡʧ��:" +
				_Reader[Index]->GetLastErrS();
			return false;
		}
	}
}

// ---��Ʊ�����ʱ��IC��
bool TServDevCtrl::ReadICCard(int Index, TCardInfo &CardInfo) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д��ͣ��";
		return false;
	}

	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д������Ϊ��,��ȡʧ��";
		return false;
	}

	char szPass[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	for (int s = 0; s < 16; s++) {

		for (int b = 0; b < 3; b++) {

			if (!_Reader[Index]->ReadCard_IC(s, b, 0, szPass, CardInfo)) {

				_LastErrS = IntToStr(s) + L"����" + IntToStr(b) + L"���ȡʧ�ܣ�" +
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

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д��ͣ��";
		return false;
	}

	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д������Ϊ��,��ȡʧ��";
		return false;
	}

	TDataSwitch DS;
	unsigned char snr[32] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	DS.strHexToBuff(strKey, snr);

	unsigned char wData[16] = {0};
	strncpy(wData, strData.c_str(), 16);

	if (!_Reader[Index]->WriteCard_IC(nSector, nBlock, 0, snr, wData)) {

		_LastErrS = IntToStr(Index + 1) + L"�Ŷ�д��д��ʧ��:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
	return true;
}

/*
 Index:�����豸��� SendType:��������
 K720 SendType 12:����������λ��
 */
bool TServDevCtrl::SendICCard(int Index, int SendType) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"���շ�����ͣ��";
		return false;
	}
	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"���շ���������Ϊ��,��ȡʧ��";
		return false;
	}

	if (_Reader[Index]->DeviceControl(SendType)) {

		return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"���շ�����:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
}

// ---����IC��,D3001
bool TServDevCtrl::ReceiveICCard(int Index, int aActionID) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"���շ�����ͣ��";
		return false;
	}
	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"���շ���������Ϊ��,��ȡʧ��";
		return false;
	}

	if (_Reader[Index]->DeviceControl(aActionID)) {

		return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"���շ�����:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
}

// ---��������ϵͳ���˿�����,bRec �Ƿ�ִ�е���ָ���һ�δ�false
// ---bReturned ���Ƿ��ѱ�����
bool TServDevCtrl::RetCardProc_ZZCC(String Reason, bool bRec, bool bReturned) {
	if (_bReset) {
		_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
		_File.WriteLog(L"���˿��Ĺ����н���ǿ�����̸�λ");
		_bReset = false;
		return false;
	}

	_File.PlayWav(L"��Ƭ�쳣", SND_FILENAME | SND_SYNC);
	_WorkSta.StrFlow = Reason + L",��ȡ��" + Now().TimeString();
	_File.WriteLog_ErrS(Reason + L",��ȡ��");
	ReceiveICCard(0, 2); // ִ�е���ָ��
	_Sequence = 5; // ---�����ɹ�

	Sleep(3000);
	_Sequence = 6; // ---����ȡ��
	return false;

	if (!bRec) { // ---δִ�е���ָ��
		if (!ReceiveICCard(0, 2)) { // ---ִ��ʧ��
			_WorkSta.StrFlow = Reason + L",����ָ��ʧ��" + Now().TimeString();
			_File.WriteLog_ErrS(Reason + L",����ָ��ʧ��:" + GetLastErrS());
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
			// �жϿ���λ��
			unsigned char szStatus[6] = {0};
			if (DeviceStatus(0, szStatus)) {

				String strStatus = DevStatusDecode_D3001(szStatus);
				if (strStatus == L"�տ�������") {

					_WorkSta.StrFlow = strStatus + L"," + Now().TimeString();
					_File.WriteLog(L"RetCardProc_ZZCC:" + strStatus);
					Sleep(500);
					ReceiveICCard(0, 6); // ---��λ
					RetCardProc_ZZCC(Reason, true, false);
					return true;

				}
				else if (strStatus != L"������1�п�") {
					RetCardProc_ZZCC(Reason, true, false);
					return true;
				}
				else {
					_Sequence = 5; // ---�����ɹ�
					_WorkSta.StrFlow = Reason + L",��ȡ��" + Now().TimeString();
					_File.WriteLog_ErrS(Reason + L",��ȡ��");
					Sleep(1500);
					RetCardProc_ZZCC(Reason, true, true);
					return true;
				}
			}
			else {
				_WorkSta.StrFlow = L"�տ���״̬��ȡʧ��:" + GetLastErrS() +
					Now().TimeString();
				_File.WriteLog(L"RetCardProc_ZZCC,�տ���״̬��ȡʧ��:" + GetLastErrS());
				RetCardProc_ZZCC(Reason, true, false);
				return true;
			}
		}
		else { // ---�ȴ�˾��ȡ��
			_File.PlayWav(L"��ȡ��", SND_FILENAME | SND_SYNC);
			BYTE szStatus[6] = {0};
			if (DeviceStatus(0, szStatus)) {

				String strStatus = DevStatusDecode_D3001(szStatus);
				if (strStatus == L"����") {

					_Sequence = 6; // ---����ȡ��
					return false;
				}
				else {
					_WorkSta.StrFlow = strStatus + "," + Now().TimeString();
					RetCardProc_ZZCC(Reason, true, true);
					return true;
				}
			}
			else {
				_WorkSta.StrFlow = L"�տ���״̬��ȡʧ��:" + GetLastErrS() +
					Now().TimeString();
				_File.WriteLog(L"RetCardProc_ZZCC,�տ���״̬��ȡʧ��:" + GetLastErrS());
				RetCardProc_ZZCC(Reason, true, true);
				return true;
			}
		}
	}
}

/*
 ����װ��״̬��ѯ
 */
bool TServDevCtrl::DeviceStatus(int Index, unsigned char *outStatus) {
	if (!_DevInfo.ReaderInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"���շ�����ͣ��";
		return false;
	}
	if (NULL == _Reader[Index]) {

		_LastErrS = IntToStr(Index + 1) + L"���շ���������Ϊ��,��ȡʧ��";
		return false;
	}

	if (_Reader[Index]->DeviceStatus(0, outStatus)) {

		return true;
	}
	else {
		_LastErrS = IntToStr(Index + 1) + L"���շ�����״̬��ѯʧ��:" +
			_Reader[Index]->GetLastErrS();
		return false;
	}
}

String TServDevCtrl::DevStatusDecode(unsigned char *inStatus) {
	char szStatus[5] = {0};
	strncpy(szStatus, inStatus, 4);
	String Status = String(szStatus);
	if (Status == L"8000")
		return L"��������";
	else if (Status == L"4000")
		return L"׼����ʧ��";
	else if (Status == L"2000")
		return L"����׼����";
	else if (Status == L"1000")
		return L"���ڷ���";
	else if (Status == L"0800")
		return L"�����տ�";
	else if (Status == L"0400")
		return L"��������";
	else if (Status == L"0040")
		return L"�ص���";
	else if (Status == L"0020")
		return L"������";
	else if (Status == L"0010")
		return L"����Ԥ��";
	else if (Status == L"0018")
		return L"�����"; // ---������123��û��,��û�п�
	else if (Status == L"0016")
		return L"��Ƭ�ڴ�����2-3";
	else if (Status == L"0014")
		return L"��Ƭ�ڴ�����3";
	else if (Status == L"0013")
		return L"��Ƭ�ڴ�����1-2(����)";
	else if (Status == L"0002")
		return L"��Ƭ�ڴ�����2";
	else if (Status == L"0011")
		return L"��Ƭ�ڴ�����1";
	else
		return L"δ֪״̬";
}

String TServDevCtrl::DevStatusDecode_D3001(unsigned char *inStatus) {
	char szStatus[8] = {0};
	strncpy(szStatus, inStatus, 6);

	if (0x31 == szStatus[3])
		return L"�տ�������";
	if (0x31 == szStatus[1])
		return L"����λ";

	TDataSwitch DS;
	AnsiString strTongd = DS.CharToBinStr(szStatus[0]); // ---��һ�ֽ�ͨ��״̬

	if (strTongd[2] == '1') { // ---ͨ�����п�

		if (strTongd[8] == '1' && strTongd[7] == '1' && strTongd[6] == '1')
			return L"������123�п�";
		else if (strTongd[8] == '1' && strTongd[7] == '1' && strTongd[6] == '0')
			return L"������12�п�";
		else if (strTongd[8] == '1' && strTongd[7] == '0' && strTongd[6] == '0')
			return L"������1�п�";
		else if (strTongd[8] == '0' && strTongd[7] == '1' && strTongd[6] == '1')
			return L"������23�п�";
		else if (strTongd[8] == '1' && strTongd[7] == '0' && strTongd[6] == '1')
			return L"������13�п�";
	}
	if (strTongd[2] == '0')
		return L"����"; // ---������������ͨ�����޿�
	return L"δ֪״̬";
}

bool TServDevCtrl::SendLed(int Index, String str) {
	if (!_DevInfo.LedInfo[Index].Use) {

		_LastErrS = IntToStr(Index + 1) + L"��LED��ͣ��";
		_WorkSta.StrLed[Index] = IntToStr(Index + 1) + L"��LEDͣ��";
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

		_LastErrS = IntToStr(Index + 1) + L"��LED��ͣ��";
		_WorkSta.StrLed[Index] = IntToStr(Index + 1) + L"��LEDͣ��";
		return false;
	}

	return _Led[Index]->SendPicToRam_2(hBitmap, nPosX, nPosY, Width, Height);
}

// --- -2��ʹ�ö�λ -1��ʱ��0ȫͨ 1ǰ�� 2�� 3ȫ��
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

// ---��λ��������
bool TServDevCtrl::DWQCheckProc(int nHeadDZFlag, String Numplate) {
	int nDWQSta;
	do {
		nDWQSta = GetDWQSta();
		if (-1 == nDWQSta) {
			_File.PlayWav(L"��λ���쳣.wav", SND_FILENAME | SND_SYNC);
			::Sleep(50);
			_WorkSta.StrFlow = L"��λ���ݳ�ʱ...";
			SendLed(0, L"��λ�����ݳ�ʱ");
			::Sleep(1000);
		}
		else if (HEADMISS == nDWQSta) {
			if (HEAD == nHeadDZFlag) {
				_File.PlayWav("��λ������.wav", SND_FILENAME | SND_SYNC);
				::Sleep(50);
				_WorkSta.StrFlow = L"��ͷ����λ...";
				SendLed(0, L"��ͷ����λ");
			}
			else {
				_File.PlayWav("��λ������.wav", SND_FILENAME | SND_SYNC);
				::Sleep(50);
				_WorkSta.StrFlow = L"��β����λ...";
				SendLed(0, L"��β����λ");
			}
		}
		else if (TAILMISS == nDWQSta) {
			if (TAIL == nHeadDZFlag) {
				_File.PlayWav("��λ������.wav", SND_FILENAME | SND_SYNC);
				::Sleep(50);
				_WorkSta.StrFlow = L"��ͷ����λ...";
				SendLed(0, L"��ͷ����λ");
			}
			else {
				_File.PlayWav("��λ������.wav", SND_FILENAME | SND_SYNC);
				::Sleep(50);
				_WorkSta.StrFlow = L"��β����λ...";
				SendLed(0, L"��β����λ");
			}
		}
		else if (ALLMISS == nDWQSta) {
			_File.PlayWav(L"��λ������.wav", SND_FILENAME | SND_SYNC);
			::Sleep(50);
			_WorkSta.StrFlow = "ǰ�󵲶�λ...";
			SendLed(0, "ǰ�󵲶�λ");
		}

		if (IsLeave() && nDWQSta != -2) {
			_WorkSta.StrFlow = L"��λʱ�����°�...";
			OutCtrl(DZ, TAIL, UP);
			_File.WriteLog(Numplate + L"��λʱ�����°�");
			return false;
		}
		if (_Thread->GetTerminated()) {
			_File.WriteLog(Numplate + L"��λʱ�߳���ֹ");
			_WorkSta.StrFlow = L"�߳���ֹ...";
			return false;
		}

	}
	while (nDWQSta != ALLPASS && nDWQSta != -2); // --- -2����ͣ���豸

	return true;
}

/***************************************************************************
 Function:	OutCtrl
 Param��     nDevice �豸���� 0��բ 1���̵�
 nPos λ�ô��ţ�0ǰ 1��;
 nAction �������� 0̧ 1��  0�� 1�� 2��
 Returns:	��
 ************************************************************************** */
void TServDevCtrl::OutCtrl(int nDev, int nPos, int nAction) {
	if (DZ == nDev) {

		if (HEAD == nPos) {

			for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) { // ���ҿ��ÿ����豸

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

			for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) { // ���ҿ��ÿ����豸

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

			for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) { // ���ҿ��ÿ����豸

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

			for (int i = 0; i < _DevInfo.IoCtrlCnt; i++) { // ���ҿ��ÿ����豸

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

// ---�����ɹ���ʧ�ܺ�,�����°��Ĺ���
// ---wavName wav�ļ���,strLed LED������ ,strSta����״̬���� strList�����б���Ϣ
// ---nHeadDZFlag ǰ�˱�־  bHeadDZIsUp �Ƿ�̧ǰ�� bTailDZIsUp�Ƿ�̧���
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
		OutCtrl(DZ, TAIL, UP); // ---��Ҫ������(�ظ���Ȧ)
	::Sleep(100);

	while (!IsLeave()) { // ---�������°���һֱѭ��

		if (_Thread->GetTerminated()) {

			_WorkSta.StrFlow = "�߳���ֹ...";
			return;
		}
		::Sleep(1000);
	}
}

// ---������ӡ
void TServDevCtrl::PrintBangd(TBangdInfo bdInfo) {
	bdInfo.Title = "       ��ͷ�����ȵ����޹�˾";
	bdInfo.Printer = _sysInfo.Location;
	AnsiString Text;
	Text = "\r\n      " + bdInfo.Title + "\r\n\r\n\r\n";
	Text = "\r\n             ����ú����Ʊ�� \r\n\r\n\r\n";
	Text += "  -------------------------------\r\n\r\n";
	Text += "  �� �� �ţ�" + bdInfo.Numplate + "\r\n\r\n";
	Text += "  �� Ӧ �̣�" + bdInfo.Gongysmc + "\r\n\r\n";
	// Text +="  ú    ��"+bdInfo.Meikmc+"\r\n\r\n";
	Text += "  ú    �֣�" + bdInfo.Wulmc + "\r\n\r\n";
	Text += "  ë    �أ�" + Format("%.2f", ARRAYOFCONST((bdInfo.Maoz))) +
		"\r\n\r\n";
	Text += "  ë��ʱ�䣺" + bdInfo.Zhongcsj + "\r\n\r\n";
	Text += "  Ƥ    �أ�" + Format("%.2f", ARRAYOFCONST((bdInfo.Piz))) +
		"\r\n\r\n";
	Text += "  Ƥ��ʱ�䣺" + bdInfo.Qingcsj + "\r\n\r\n";
	Text += "  ��    ����" + FloatToStr(bdInfo.Koug) + "\r\n\r\n";
	Text += "  ��    �ӣ�" + FloatToStr(bdInfo.Kouz) + "\r\n\r\n";
	Text += "  ��    ˮ��" + FloatToStr(bdInfo.Kous) + "\r\n\r\n";
	Text += "  ��    �أ�" + Format("%.2f", ARRAYOFCONST((bdInfo.Jingz))) +
		"\r\n\r\n";
	Text += "  �� α �룺" + bdInfo.JilsjID + "\r\n\r\n";
	Text += "  -------------------------------\r\n\r\n";
	Text += "  �� Ʊ �ˣ�" + bdInfo.Printer + "\r\n\r\n";
	Text += "  ��Ʊʱ�䣺" + Now().FormatString("yyyy-MM-dd HH:mm:ss") +
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

// ---ͣ������ú�������ɹ���ʧ�ܺ�,�����뿪�Ĺ���
// ---wavName wav�ļ���,strLed LED������ ,strSta����״̬���� strList�б���Ϣ
// ---nHeadDZFlag ǰ�˱�־  bHeadDZIsUp �Ƿ�̧ǰ�� bTailDZIsUp�Ƿ�̧���
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
		OutCtrl(DZ, TAIL, UP); // ---��Ҫ������(�ظ���Ȧ)
	::Sleep(100);

	if (_Thread->GetTerminated()) {

		_WorkSta.StrFlow = "�߳���ֹ...";
		return;
	}
	::Sleep(10000);
}