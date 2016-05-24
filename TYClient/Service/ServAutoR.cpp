// ---------------------------------------------------------------------------
#pragma hdrstop

#include "ServAutoR.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)

String StrRFID = "", StrICID = "", CarNumber = "";

TServAutoR::TServAutoR() {
	_bInitial = false;
	_ErrCnt = 99;
	memset(_FLowID, -1, sizeof(_FLowID));
}

TServAutoR::~TServAutoR() {
}

void __fastcall TServAutoR::AutoBaseProc() {
	// _ErrCnt =0;

	if (!_bInitial) {
		bool bResult;
		// ---NodeID 0�����볧 1ͣ���� 2���� 3���� 4ú�� 5�������� 6RFID������
		_WorkSta.StrFlow = L"ϵͳ���ڳ�ʼ��," + Now().TimeString();
		switch (_sysInfo.NodeID) {

		case 0:
			bResult = GetFlowID("Yanp", _FLowID[0]);
			break;
		case 1:
			bResult =
				(GetFlowID("TingccIn", _FLowID[0]) && GetFlowID("TingccOut",
				_FLowID[1]));
			break;
		case 2:
			bResult = GetFlowID("CAIY", _FLowID[0]);
			break;
		case 3:
			bResult = (GetFlowID("Chengz", _FLowID[0]) && GetFlowID("Huip",
				_FLowID[1]));
			break;
		case 4:
			bResult = (GetFlowID("MeicIn", _FLowID[0]) && GetFlowID("MeicOut",
				_FLowID[1]));
			break;
		case 5:
			bResult = GetFlowID("Chup", _FLowID[0]);
			break;
		}

		if (bResult)
			_bInitial = true;
		else {
			_ErrCnt++;
			_WorkSta.StrFlow = L"����ID��ȡʧ��," + Now().TimeString();
			SendLed(0, L"����,����ID��ȡʧ��");
			Sleep(3000);
			return;
		}
	}

	String CurrDate;
	if (!GetServerSystemTime_pt(CurrDate)) {

		_ErrCnt++;
		_WorkSta.StrFlow = L"ϵͳʱ���ȡʧ��," + Now().TimeString();
		SendLed(0, L"��ȡϵͳʱ��ʧ��");
		Sleep(2000);
		return;
	}

	if (0 == _sysInfo.NodeID) {

		if (_ServerDate != CurrDate.SubString(1, 10)) { // ---�����������ͬһ����

			// ---ȡ��ú��Ӧ�̼ƻ���
			if (GetRanmJih_pt(_dsMkGys)) {

				_ServerDate = CurrDate.SubString(1, 10);
			}
			else {
				_ErrCnt++;
				_WorkSta.StrFlow = L"��ȡ������ú�ƻ�ʧ��," + Now().TimeString();
				SendLed(0, L"��ȡ������ú�ƻ�ʧ��");
				Sleep(3000);
				return;
			}
		}

		// ---��ѯ����װ�õ�״̬
		BYTE szStatus[4] = {0};
		if (!DeviceStatus(2, szStatus)) {

			_ErrCnt++;
			_WorkSta.StrFlow = L"������״̬��ѯʧ��:" + GetLastErrS() + "," +
				Now().TimeString();
			SendLed(0, L"������״̬��ѯʧ��:" + GetLastErrS());
			Sleep(3000);
			return;
		}
		// ---����������״̬
		String strStatus = DevStatusDecode(szStatus);
		if (strStatus != L"����Ԥ��" && strStatus != L"��Ƭ�ڴ�����3" && strStatus !=
			L"��Ƭ�ڴ�����1-2(����)") {

			_ErrCnt++;
			_WorkSta.StrFlow = L"������δ����:" + strStatus + L"," +
				Now().TimeString();
			SendLed(0, L"������δ����:" + strStatus);
			SendICCard(2, 2);
			Sleep(3000);
			return;
		}
	}

	if (5 == _sysInfo.NodeID) { // ---��������

		_Sequence = 0;
		// ---��ѯ�տ�װ�õ�״̬
		BYTE szStatus[6] = {0};
		if (!DeviceStatus(0, szStatus)) {

			_ErrCnt++;
			_WorkSta.StrFlow = L"�տ���״̬��ѯʧ��:" + GetLastErrS() + "," +
				Now().TimeString();
			Sleep(3000);
			return;
		}
		String strStatus = DevStatusDecode_D3001(szStatus);
		// if(strStatus !=L"����"){
		if (strStatus != L"����") {
			if (strStatus == L"����λ") {
				ReceiveICCard(0, 1);   //---�տ���Ʊ��
				_WorkSta.StrFlow = L"�տ���Ʊ��," + Now().TimeString();
			}
			else {
				ReceiveICCard(0, 6); // ---��λ
				_ErrCnt++;
				_WorkSta.StrFlow = L"�տ���δ����:" + strStatus + "," +
					Now().TimeString();
			}
			Sleep(3000);
			return;
		}
	}
	_ErrCnt = 0;
}

/*
 �����볧
 _Sequence 0:ɨ��RFID�� 1:��ȡ������Ϣ�� 2:�жϳ���״̬��
 3:�ȴ�˾��ȷ���� 4:�ȴ�˾��ȷ�Ͽ�Դ��Ϣ  5:���뷢��״̬
 6:ȡ������ 7:��ɷ���
 */
void __fastcall TServAutoR::AutoProc_ZZRC() {
	_Sequence = 0; // ---ɨ��RFID��

	if (_ErrCnt != 0)
		return; // ---�й���

	SendLed(0, _sysInfo.Location);

	_WorkSta.Car = "";
	_WorkSta.CardID = "";

	TCardInfo CardInfo;
	String strRFID, strNumplate; // ---RFID����,���ƺ�
	_WorkSta.StrFlow = L"���ڶ�RFID��," + Now().TimeString();

	bool DeBug = false;
	if (!DeBug) {
		if (!ReadCardID(0, CardInfo)) {

			return;
		}
		_Sequence = 1; // ---��ȡ������Ϣ��
		strRFID = CardInfo.CardID[0];
	}
	else {
		_Sequence = 1; // ---��ȡ������Ϣ��
		strRFID = L"E2003020250B012617805F85";
	}
	_WorkSta.CardID = strRFID;
	_WorkSta.StrFlow = L"���ڶ�ȡ������Ϣ," + Now().TimeString();
	TCarInfo CarInfo;
	if (!GetCarInfo_pt(strRFID, CarInfo)) {

		_WorkSta.StrFlow = L"������Ϣ��ȡʧ��,RFID����:" + strRFID + L",������Ϣ" +
			GetLastErrS() + L"," + Now().TimeString();
		return;
	}
	strNumplate = CarInfo.NumPlate;
	_WorkSta.Car = strNumplate;
	SendLed(0, strNumplate);
	Sleep(3000);

	_Sequence = 2; // ---�жϳ�����Ϣ��
	if (1 == CarInfo.IsLock) { // ---��������

		_WorkSta.StrFlow = L"��������," + Now().TimeString();
		SendLed(0, strNumplate + L" ��������");
		::Sleep(100);
		_File.PlayWav(L"����.wav");
		Sleep(5000);
		return;
	}

	int currFLowID;
	UTF8String FlowDescription; // ---ҵ������
	if (!GetCarFlowID(strNumplate, currFLowID, FlowDescription)) {

		_WorkSta.StrFlow = L"���������̴���," + Now().TimeString();
		SendLed(0, strNumplate + L" ��ȡ�������̴���");
		Sleep(5000);
		return;
	}

	if (currFLowID != _FLowID[0]) {

		if (currFLowID < _FLowID[0]) {
			_WorkSta.StrFlow = L"���̲���:δ" + FlowDescription + L" " +
				Now().TimeString();
			SendLed(0, strNumplate + L" ���̲���:δ" + FlowDescription);
		}
		else {
			_WorkSta.StrFlow = L"���̲���:Ӧ" + FlowDescription + L" " +
				Now().TimeString();
			SendLed(0, strNumplate + L" ���̲���:Ӧ" + FlowDescription);
		}
		Sleep(5000);
		return;
	}
	SendLed(0, strNumplate + L" ���³��쿨");
	_File.PlayWav(L"��ȷ�ϳ�����Ϣ.wav", SND_FILENAME | SND_SYNC);
	_Sequence = 3; // ---�ȴ�˾��ȷ����
	while (3 == _Sequence) { // ---�ȴ�˾��ȷ�ϳ�����Ϣ��ȡ����ѭ��

		_WorkSta.StrFlow = L"�ȴ�˾��ȷ�ϳ�����Ϣ," + Now().TimeString();

		Sleep(100);
	}
	if (_bReset) {
		_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
		_File.WriteLog(L"��˾��ȷ�ϳ�����Ϣ�Ĺ����н���ǿ�����̸�λ");
		_bReset = false;
		return;
	}
	while (4 == _Sequence) { // ---�ȴ�˾��¼�����Ϣ��ȡ����ѭ��

		_WorkSta.StrFlow = L"�ȴ�˾��ѡ�����Ϣ," + Now().TimeString();
		Sleep(100);
	}
	if (_bReset) {
		_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
		_File.WriteLog(L"��˾��ѡ�����Ϣ�Ĺ����н���ǿ�����̸�λ");
		_bReset = false;
		return;
	}
	// ---����
	if (5 == _Sequence) {

		_strFaKaTis = L"�����ƿ���,���Ժ󡣡���";
		_WorkSta.StrFlow = L"���ڷ���," + Now().TimeString();
		_File.PlayWav(L"�ƿ���.wav");

		bool bResult;
		// ---����������λ��
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
				_File.WriteLog(L"�ڷ������������Ĺ����н���ǿ�����̸�λ");
				_bReset = false;
				return;
			}
			if (!DeBug)
				bResult = SendICCard(2, 12);
			else
				bResult = true;

			if (!bResult)
				_WorkSta.StrFlow = L"������������ʱʧ��:" + GetLastErrS() + "," +
					Now().TimeString();
			else
				_WorkSta.StrFlow = L"�ɹ�������������," + Now().TimeString();
			Sleep(1000);
		}
		while (!bResult);

		// ---��ѯ����������װ��״̬
		BYTE szStatus[4] = {0};
		String strStatus = "";
		do {
			if (!DeBug) {
				bResult = DeviceStatus(2, szStatus); // ---��ѯ
				if (!bResult)
					_WorkSta.StrFlow = L"������״̬��ѯʧ��:" + GetLastErrS() + "," +
						Now().TimeString();
				else { // ---����
					strStatus = DevStatusDecode(szStatus);
					_WorkSta.StrFlow = L"���ڲ�ѯ��Ƭ״̬(����λ��),״̬:" + strStatus + "," +
						Now().TimeString();
				}
			}
			else {
				bResult = true;
				strStatus = L"��Ƭ�ڴ�����1-2(����)";
			}
			if (_bReset) {
				// ---Ӧ�տ���������3,��ʵ���豸�޷��ջ�
				_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
				_File.WriteLog(L"�ڼ�⿨��λ�Ĺ����н���ǿ�����̸�λ");
				_bReset = false;
				return;
			}
			Sleep(1000);

		}
		while (strStatus != L"��Ƭ�ڴ�����1-2(����)"); // ---��ȷ��ɶ״̬���Զ���???

		// ---����
		TCardInfo CardInfo;
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
				_File.WriteLog(L"�ڶ���ID�Ĺ����н���ǿ�����̸�λ");
				_bReset = false;
				return;
			}
			if (!DeBug) {
				bResult = ReadCardID(2, CardInfo);
			}
			else {
				bResult = true;
			}
			if (!bResult)
				_WorkSta.StrFlow = L"��IC��ʧ��:" + GetLastErrS() + "," +
					Now().TimeString();
			else
				_WorkSta.StrFlow = L"��IC���ɹ�," + Now().TimeString();
			Sleep(500);
		}
		while (!bResult);

		// ---д��  6.4.22 Wuwenbin����
		/* do{
		 if(_bReset){
		 _WorkSta.StrFlow =L"ǿ�����̸�λ,"+Now().TimeString();
		 _File.WriteLog(L"��дIC���Ĺ����н���ǿ�����̸�λ");
		 _bReset =false;
		 return;
		 }
		 bResult =WriteICCard(2,1,0,"FFFFFFFFFFFF",AnsiString(strNumplate));
		 if(!bResult) {
		 _WorkSta.StrFlow =L"дIC��ʧ��:"+GetLastErrS()+","+Now().TimeString();
		 _File.WriteLog_ErrS(L"дIC��ʧ��:"+GetLastErrS());
		 }
		 else {
		 _WorkSta.StrFlow =L"дIC���ɹ�,"+Now().TimeString();
		 }
		 }while(!bResult);
		 */
		// ---���ýӿ�IC���ͳ����а�
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
				_File.WriteLog(L"�ڳ����󶨵Ĺ����н���ǿ�����̸�λ");
				_bReset = false;
				return;
			}
			if (!DeBug) {
				bResult = BingdingICCard_pt(strNumplate,
					String((char *)CardInfo.ICCardID_Hex), 0);
			}
			else {
				bResult = BingdingICCard_pt(strNumplate,
					L"026DFD62F0080400014175715248001D", 0);
			}
			if (!bResult) {
				_WorkSta.StrFlow = L"����(IC)��ʧ��:" + GetLastErrS() + "," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"����(IC)��ʧ��:" + GetLastErrS());
			}
			else {
				_WorkSta.StrFlow = L"����(IC)�󶨳ɹ�," + Now().TimeString();
			}
			Sleep(500);
		}
		while (!bResult);

		// ---�ύ��Ʊ��Ϣ��ƽ̨
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
				_File.WriteLog(L"���ύ��Ʊ��Ϣ�Ĺ����н���ǿ�����̸�λ");
				_bReset = false;
				return;
			}
			bResult = SubmitYanp_pt(strNumplate, _fKuangfl, _Meik, _Gongys,
				_Pinz); // ---16.4.22 Wuwenbin���� fKuangfl����
			if (!bResult)
				_WorkSta.StrFlow = L"�����ύʧ��:" + GetLastErrS() + "," +
					Now().TimeString();
			else
				_WorkSta.StrFlow = L"����(IC)�����ύ�ɹ�," + Now().TimeString();
			Sleep(500);
		}
		while (!bResult);

		// ---������ȡ��λ��
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
				_File.WriteLog(L"�ڷ�����ȡ���ڵĹ����н���ǿ�����̸�λ");
				_bReset = false;
				return;
			}
			// bResult =SendICCard(2,14);
			if (!DeBug) {
				bResult = SendICCard(2, 0);
			}
			else {
				bResult = true;
			}
			if (!bResult)
				_WorkSta.StrFlow = L"������ȡ����ʧ��:" + GetLastErrS() + "," +
					Now().TimeString();
			else
				_WorkSta.StrFlow = L"�ɹ�������ȡ����," + Now().TimeString();
			Sleep(200);
		}
		while (!bResult);

		do { // ---��⿨�Ƿ�ȡ����
			if (!DeBug) {
				bResult = DeviceStatus(2, szStatus); // ---��ѯ
				if (!bResult)
					_WorkSta.StrFlow = L"������״̬��ѯʧ��:" + GetLastErrS() + "," +
						Now().TimeString();
				else { // ---����
					strStatus = DevStatusDecode(szStatus);
					_WorkSta.StrFlow = L"���ڲ�ѯ��Ƭ״̬,״̬:" + strStatus + "," +
						Now().TimeString();
				}
				if (_bReset) {
					// ---Ӧ�տ���������3,��ʵ���豸�޷��ջ�
					_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
					_File.WriteLog(L"�ڼ�⿨��λ(ȡ����)�Ĺ����н���ǿ�����̸�λ");
					_bReset = false;
					return;
				}
			}
			else {
				bResult = true;
				strStatus = L"��Ƭ�ڴ�����1";
			}
			Sleep(200);
		}
		while (strStatus != L"��Ƭ�ڴ�����1");
		// ---��һ��û���ü���⵽״̬,˾���Ͳ���,������������ѭ��??? while(strStatus !=L"��Ƭ�ڴ�����1")

		_strFaKaTis = L"��ȡ�����Ŀ�������";
		_Sequence = 7; // ---���ѵ�ȡ����
		_File.PlayWav(L"��ȡ��.wav");
		do { // ---��⿨״̬,��˾���ѿ�����
			if (!DeBug) {

				bResult = DeviceStatus(2, szStatus); // ---��ѯ
				if (!bResult)
					_WorkSta.StrFlow = L"������״̬��ѯʧ��:" + GetLastErrS() + "," +
						Now().TimeString();
				else { // ---����
					strStatus = DevStatusDecode(szStatus);
					_WorkSta.StrFlow = L"���ڲ�ѯ��Ƭ״̬,״̬:" + strStatus + "," +
						Now().TimeString();
				}
				/* if(_bReset){
				 //---Ӧ�տ���������3,��ʵ���豸�޷��ջ�
				 _WorkSta.StrFlow =L"ǿ�����̸�λ,"+Now().TimeString();
				 _File.WriteLog(L"�ڼ�⿨��λ(ȡ����)�Ĺ����н���ǿ�����̸�λ");
				 _bReset =false;
				 return;
				 } */
			}
			else {
				bResult = true;
				strStatus = "";
			}
			Sleep(1000);

		}
		while (strStatus == L"��Ƭ�ڴ�����1");

		_Sequence = 8; // ---��ɷ���
	}
}

/*
 ��������
 _Sequence 0:�ȴ�˾�����ȡ���� 1:�����Զ�����״̬ 2������λ
 3:ͨ�����Ŷ�������Ϣ 4:��������Ϣ  5:���Ų�һ�£��˻ؿ�
 6:˾��ȡ�ؿ�
 */
void __fastcall TServAutoR::AutoProc_ZZCC() {
	if (_ErrCnt != 0)
		return; // ---�й���

	_WorkSta.Car = "";
	_WorkSta.CardID = "";

	bool DeBug = false;
	TCardInfo CardInfo;
	 if (!DeBug) {

		 _WorkSta.StrFlow = L"���ڹر��Զ��տ�," + Now().TimeString();
		 if (!ReceiveICCard(0, 5)) {

			 _WorkSta.StrFlow = L"�ر��Զ��տ�����" + GetLastErrS() + "," +
			 Now().TimeString();
			 _File.WriteLog_ErrS(L"�ر��Զ��տ�����" + GetLastErrS());
			 Sleep(2000);
			 return;
		 }
    }
	_WorkSta.StrFlow = L"�ȴ�ѡ��ȡ����," + Now().TimeString();
	while (0 == _Sequence) { // ---�ȴ�ѡ��ȡ����

		if (_bReset) {
			_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
			_File.WriteLog(L"�����Զ��տ�ģʽ�Ĺ����н���ǿ�����̸�λ");
			_bReset = false;
			return;
		}
	}

	bool bResult;
	 _WorkSta.StrFlow = L"���ڴ��Զ��տ�," + Now().TimeString();
	 if (1 == _Sequence) { // ---�Զ�����

		 if (_bReset) {
			 _WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
			 _File.WriteLog(L"�����Զ��տ�ģʽ�Ĺ����н���ǿ�����̸�λ");
			 _bReset = false;
			 return;
		 }

		 do {
			 bResult = ReceiveICCard(0, 4);
			 if (!bResult) {
				 _WorkSta.StrFlow = L"�����Զ��տ�ģʽʧ��:" + GetLastErrS() + "," +
				 Now().TimeString();
				 _File.WriteLog_ErrS(L"�����Զ��տ�ģʽʧ��:" + GetLastErrS());
			 }
			 Sleep(1000);
		 }
		 while (!bResult);

	 }

	// ---�ȴ�����λ
	_WorkSta.StrFlow = L"���ڵȴ�����λ," + Now().TimeString();
	while (true) {

		if (_bReset) {
			_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
			_File.WriteLog(L"�ȴ�����λ�Ĺ����н���ǿ�����̸�λ");
			_bReset = false;
			return;
		}
		BYTE szStatus[6] = {0};
		if (!DeBug) {
			if (DeviceStatus(0, szStatus)) {

				String strStatus = DevStatusDecode_D3001(szStatus);
				if (strStatus == L"����λ")
					break;
				_WorkSta.StrFlow = strStatus + L"," + Now().TimeString();
			}
			else {
				_WorkSta.StrFlow = + L"��ѯ����λ״̬ʧ�ܣ�" + GetLastErrS() + L"," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"��ѯ����λ״̬ʧ��:" + GetLastErrS());
			}
			ReceiveICCard(0, 6); // ---��λ
			Sleep(1000);
		}
		else
			break;
	}

	_Sequence = 2; // ---����λ��׼������
	_WorkSta.StrFlow = L"���ڶ�ȡ����Ϣ," + Now().TimeString();
	_File.PlayWav(L"��ȡ����Ϣ.wav", SND_FILENAME | SND_SYNC);

	while (true) {

		if (_bReset) {
			_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
			_File.WriteLog(L"�ڶ���ID�Ĺ����н���ǿ�����̸�λ");
			_bReset = false;
			return;
		}
		if (!DeBug) {
			if (ReadCardID(2, CardInfo)) {
				_WorkSta.StrFlow = L"�����ɹ�," + Now().TimeString();
				break;
			}
			_WorkSta.StrFlow = L"����ʧ��," + GetLastErrS() + L"," +
				Now().TimeString();
			if (!RetCardProc_ZZCC(L"����ʧ��:" + GetLastErrS(), false, false))
				// �˿�������
					return;
		}
		else
			break;
	}

	String CardID = "";
	if (!DeBug)
		CardID = String((char *)CardInfo.ICCardID_Hex);
	else
		CardID = "A20AA8161608040001C4D41AA9D9791D";

	_WorkSta.CardID = CardID;

	// ---��������Ϣ
	_WorkSta.StrFlow = L"����ͨ�����Ŷ�ȡ������Ϣ," + Now().TimeString();
	_Sequence = 3; // ---ͨ�����Ŷ�ȡ������Ϣ
	TCarInfo CarInfo;
	while (true) {
		if (_bReset) {
			_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
			_File.WriteLog(L"�ڶ�ȡ������Ϣ�Ĺ����н���ǿ�����̸�λ");
			_bReset = false;
			return;
		}

		if (GetCarInfo_pt(CardID, CarInfo)) {

			_WorkSta.StrFlow = L"����ȡ������Ϣ�ɹ�," + Now().TimeString();
			_WorkSta.Car = CarInfo.NumPlate;
			break;
		}

		_WorkSta.StrFlow = L"����ȡ������Ϣʧ��," + GetLastErrS() + L"," +
			Now().TimeString();
		_File.WriteLog_ErrS(L"����ȡ������Ϣʧ��:" + GetLastErrS());
		if (!RetCardProc_ZZCC(L"����ȡ������Ϣʧ��:" + GetLastErrS(), false, false))
			// �˿�������
				return;
	}

	if (!DeBug) {
		// ---��ȡ��������
		_WorkSta.StrFlow = L"���ڶ�ȡ������Ϣ," + Now().TimeString();
		_Sequence = 4; // ---ͨ�����Ŷ�ȡ������Ϣ
		while (true) {
			if (_bReset) {
				_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
				_File.WriteLog(L"�ڶ�ȡ������Ϣ�Ĺ����н���ǿ�����̸�λ");
				_bReset = false;
				return;
			}
			if (ReadICCard(2, CardInfo)) {

				_WorkSta.StrFlow = L"������Ϣ��ȡ�ɹ�," + Now().TimeString();
				break;
			}
			_WorkSta.StrFlow = L"������Ϣ��ȡʧ��," + GetLastErrS() + L"," +
				Now().TimeString();
			_File.WriteLog_ErrS(L"������Ϣ��ȡʧ��:" + GetLastErrS());
			if (!RetCardProc_ZZCC(L"������Ϣ��ȡʧ��:" + GetLastErrS(), false,
				false)) // �˿�������
					return;
		}

		// ---�ȽϿ��ڳ��ź�ϵͳ����
		if (String((char *)CardInfo.Sector[1].Block[0]) != CarInfo.NumPlate)
		{ // ---���Ų�һ��

			// ---��������,���ȴ�˾��ȡ��
			if (!RetCardProc_ZZCC(L"���ڳ�����ϵͳ����", false, false)) // �˿�������
					return;
		}
	}
	// ---���ó�Ʊ�ӿ�

	_Sequence = 7;
	double fKous(0), fKoug(0), fKouz(0), fKouqt(0);
	int nKoudNum;
	if (!DeBug) {
		try {
			nKoudNum = String((char *)CardInfo.Sector[1].Block[1]).ToInt();
			// ---�۶ִ���
			for (int i = 0; i < nKoudNum; i++) { // ---����۶�����

				String strKoud = String((Char*)CardInfo.Sector[i + 2].Block[0]);
				switch (strKoud.SubString(strKoud.Pos("|") + 1, 1).ToInt()) {

				case 1:
					fKous += strKoud.SubString(1, strKoud.Pos("|") - 1)
						.ToDouble();
					break;
				case 2:
					fKoug += strKoud.SubString(1, strKoud.Pos("|") - 1)
						.ToDouble();
					break;
				case 3:
					fKouz += strKoud.SubString(1, strKoud.Pos("|") - 1)
						.ToDouble();
					break;
				case 4:
					fKouqt += strKoud.SubString(1, strKoud.Pos("|") - 1)
						.ToDouble();
					break;
				}
			}
		}
		catch (Exception &e) {
			// ---�������쳣��Ӧ �˿� ��ʾ˾��ȡ�ؿ�
			// ---��������,���ȴ�˾��ȡ��
			_WorkSta.StrFlow = L"���ڿ۶���Ϣ�쳣," + e.Message + L"," +
				Now().TimeString();
			_File.WriteLog_ErrS(L"���ڿ۶���Ϣ�쳣:" + e.Message);
			if (!RetCardProc_ZZCC(L"���ڿ۶���Ϣ�쳣" + e.Message, false, false))
				// �˿�������
					return;
		}
	}
	// ---��ȡƽ̨׼����Ʊ

	_WorkSta.StrFlow = L"���ڴ�ӡ����," + Now().TimeString();
	TBangdInfo BangdInfo;

	if (!ChupData_pt(CarInfo.NumPlate, fKous, fKoug, fKouz, fKouqt, BangdInfo))
	{
		_WorkSta.StrFlow = L"���ó�Ʊ�ӿ�ʧ�ܣ�" + GetLastErrS() + "," +
			Now().TimeString();
		_File.WriteLog_ErrS(L"���ó�Ʊ�ӿ�ʧ�ܣ�" + GetLastErrS());
		if (!RetCardProc_ZZCC(L"���ó�Ʊ�ӿ�ʧ�ܣ�" + GetLastErrS(), false, false))
			// �˿�������
				return;
	}
	if (!BingdingICCard_pt(CarInfo.NumPlate, "", 1)) {
		_WorkSta.StrFlow = L"����IC�����ʧ�ܣ�" + GetLastErrS() + "," +
			Now().TimeString();
		_File.WriteLog_ErrS(L"����IC�����ʧ�ܣ�" + GetLastErrS());
	}
	Sleep(1000);

	_WorkSta.StrFlow = L"���ڴ�ӡ����," + Now().TimeString();
	_File.PlayWav(L"���ڴ�ӡ����.wav", SND_FILENAME | SND_SYNC);
	PrintBangd(BangdInfo);
	Sleep(2500);
	_Sequence = 8; // ---������ӡ���
	_File.PlayWav(L"������ӡ���.wav", SND_FILENAME | SND_SYNC);
	Sleep(1000);
	_Sequence = 9; // ---��ȡƱ
	// ---�տ���Ʊ��
	_WorkSta.StrFlow = L"�����տ���Ʊ��," + Now().TimeString();

	ReceiveICCard(0, 1); // �տ���Ʊ��
	while (true) {
		if (_bReset) {
			_WorkSta.StrFlow = L"ǿ�����̸�λ," + Now().TimeString();
			_File.WriteLog(L"�ȴ�����λ�Ĺ����н���ǿ�����̸�λ");
			_bReset = false;
			return;
		}

		BYTE szStatus[6] = {0};
		if (DeviceStatus(0, szStatus)) {

			String strStatus = DevStatusDecode_D3001(szStatus);
			if (strStatus == L"����")
				break;
			_WorkSta.StrFlow = strStatus + L"," + Now().TimeString();
		}
		else {
			_WorkSta.StrFlow = + L"��ѯ����λ״̬ʧ�ܣ�" + GetLastErrS() + L"," +
				Now().TimeString();
			_File.WriteLog_ErrS(L"��ѯ����λ״̬ʧ��:" + GetLastErrS());
		}
		ReceiveICCard(0, 6); // ---��λ
		Sleep(1000);
	}
	_Sequence = 10; // ---���տ���Ʊ��
}

// ---ͣ��������
void __fastcall TServAutoR::AutoProc_ParkingCR() {
	if (_ErrCnt != 0)
		return; // ---�й���

	bool Debug = false;

	SendLed(_sysInfo.TingID, _sysInfo.Location + (0 == _sysInfo.TingID ? L"���" :
		L"����"));

	_WorkSta.Car = "";
	_WorkSta.CardID = "";
	// ---ɨ��RFID��
	_WorkSta.StrRun = "������...";
	TCardInfo CardInfo;
	String strRFID, strNumplate, ICardID; // ---RFID����,���ƺ�,IC����
	_WorkSta.StrFlow = L"���ڶ�RFID��," + Now().TimeString();

	if (!Debug) {
		if (!ReadCardID(_sysInfo.TingID, CardInfo)) {

			return;
		}
		// ---��ȡ������Ϣ��
		_WorkSta.CardID = strRFID = CardInfo.CardID[0];
	}
	else {
		_WorkSta.CardID = strRFID = "E2003020250B012617805F85";
	}
	_WorkSta.StrFlow = L"���ڶ�ȡ������Ϣ," + Now().TimeString();
	TCarInfo CarInfo;
	if (!GetCarInfo_pt(strRFID, CarInfo)) {

		_WorkSta.StrFlow = L"������Ϣ��ȡʧ��,RFID����:" + strRFID + L",������Ϣ" +
			GetLastErrS() + L"," + Now().TimeString();
		return;
	}
	_WorkSta.Car = strNumplate = CarInfo.NumPlate;

	SendLed(_sysInfo.TingID, strNumplate);
	Sleep(3000);

	// ---�жϳ�����Ϣ��
	if (1 == CarInfo.IsLock) { // ---��������

		FinishCRC(L"����.wav", strNumplate + L" ��������",
			L"��������," + Now().FormatString("hh:mm:ss"), strNumplate + L"^��������",
			false, false, _sysInfo.TingID);
		Sleep(5000);
		return;
	}

	int currFLowID;
	UTF8String FlowDescription; // ---ҵ������
	if (!GetCarFlowID(strNumplate, currFLowID, FlowDescription)) {

		_WorkSta.StrFlow = L"���������̴���," + Now().TimeString();
		SendLed(_sysInfo.TingID, strNumplate + L" ��ȡ�������̴���");
		Sleep(5000);
		return;
	}

	int ChuRTag = -1; // ---�� �� �� �ı�� 0:�� 1:��
	if (currFLowID == _FLowID[0])
		ChuRTag = 0;
	else if (currFLowID == _FLowID[1])
		ChuRTag = 1;

	if (-1 == ChuRTag) {

		if (currFLowID < _FLowID[0]) {
			FinishCRC(L"���̲���.wav", strNumplate + L" ���̲���:δ" + FlowDescription,
				L"���̲���:δ" + FlowDescription + "," + Now().FormatString
				("hh:mm:ss"), strNumplate + L"^���̲���:δ" + FlowDescription, false,
				false, _sysInfo.TingID);
		}
		else if (currFLowID > _FLowID[1]) {
			FinishCRC(L"���̲���.wav", strNumplate + L" ���̲���:Ӧ" + FlowDescription,
				L"���̲���:Ӧ" + FlowDescription + "," + Now().FormatString
				("hh:mm:ss"), strNumplate + L"^���̲���:Ӧ" + FlowDescription, false,
				false, _sysInfo.TingID);
		}
		return;
	}
	switch (_sysInfo.TingID) {

	case 0: // ---�볡
		if (1 == ChuRTag) {
			FinishCRC(L"�ѽ���ͣ����.wav", strNumplate + L"�ó��ѽ���ͣ����",
				L"�ó��ѽ���ͣ����," + Now().FormatString("hh:mm:ss"),
				strNumplate + L"^�ó��ѽ���ͣ����", false, false, _sysInfo.TingID);
			return;
		}
		break;
	case 1: // ---����
		if (0 == ChuRTag) {
			FinishCRC(L"δ����ͣ����.wav", strNumplate + L"�ó���δ����ͣ����",
				L"�ó���δ����ͣ����," + Now().FormatString("hh:mm:ss"),
				strNumplate + L"^�ó���δ����ͣ����", false, false, _sysInfo.TingID);
			return;
		}
		break;
	}
	SendLed(_sysInfo.TingID, strNumplate + L" ���³�ˢ��");
	_File.PlayWav(L"��ˢ��.wav", SND_FILENAME | SND_SYNC);
	_WorkSta.StrFlow = "��ˢ��," + Now().FormatString("hh:mm:ss");
	TCardInfo CardInfoIC;
	TCarInfo CarInfoIC;
	if (!Debug) {
		bool bResult;
		int i = 0;
		do {

			bResult = ReadCardID(_sysInfo.TingID + 2, CardInfoIC);
			Sleep(500);
			i++;
			if (i >= 50) {
				Sleep(2000);
				return;
			}
		}
		while (!bResult);

		_WorkSta.CardID = ICardID = String((char *)CardInfoIC.ICCardID_Hex);
	}
	else {
		_WorkSta.CardID = ICardID = "1234567890";
	}
	if (!GetCarInfo_pt(ICardID, CarInfoIC)) {

		SendLed(_sysInfo.TingID, L"IC����Ϣ��ȡʧ��");
		_WorkSta.StrFlow = L"IC����Ϣ��ȡʧ��,IC����:" + ICardID + L",������Ϣ" +
			GetLastErrS() + L"," + Now().TimeString();
		Sleep(2000);
		return;
	}
	String Numplate = CarInfoIC.NumPlate;
	if (strNumplate != Numplate) {
		FinishCRC(L"������IC����һ��.wav", strNumplate + L" ������IC����һ��",
			L"������IC����һ��:" + FlowDescription + "," + Now().FormatString
			("hh:mm:ss"), strNumplate + L"^������IC����һ��:" + FlowDescription, false,
			false, _sysInfo.TingID);
		Sleep(2000);
		return;
	}

	if (!Debug) {
		bool bResult;
		int i = 0;
		do {

			bResult = WriteICCard(_sysInfo.TingID + 2, 1, 0, "FFFFFFFFFFFF",
				AnsiString(strNumplate));
			if (!bResult) {
				_WorkSta.StrFlow = L"дIC������ʧ��:" + GetLastErrS() + "," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"дIC������ʧ��:" + GetLastErrS());
			}
			else {
				_WorkSta.StrFlow = L"дIC���ɹ�," + Now().TimeString();
			}
			bResult = WriteICCard(_sysInfo.TingID + 2, 1, 1, "FFFFFFFFFFFF",
				AnsiString(0));
			if (!bResult) {
				_WorkSta.StrFlow = L"����IC���۶ִ���ʧ��:" + GetLastErrS() + "," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"����IC���۶ִ���ʧ��:" + GetLastErrS());
			}
			else {
				_WorkSta.StrFlow = L"����IC���۶ִ����ɹ�," + Now().TimeString();
			}
			bResult = WriteICCard(_sysInfo.TingID + 2, 1, 2, "FFFFFFFFFFFF",
				AnsiString(0));
			if (!bResult) {
				_WorkSta.StrFlow = L"����IC���۶�����ʧ��:" + GetLastErrS() + "," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"����IC���۶�����ʧ��:" + GetLastErrS());
			}
			else {
				_WorkSta.StrFlow = L"����IC���۶������ɹ�," + Now().TimeString();
			}

			Sleep(500);
			i++;
			if (i >= 50) {
				Sleep(2000);
				return;
			}
		}
		while (!bResult);
	}

	TStation Station;

	if (EnterExitParking_pt(strNumplate, _sysInfo.TingID)) {
		if (GetNextStation(strNumplate, Station)) {
			String strLed = strNumplate + L" ��ȥ��" +
				(0 == ChuRTag ? Station.Current_Station :
				Station.Next_Location);
			String strSta =
				L"��֤�ɹ�,��ȥ��" + (0 == ChuRTag ? Station.Current_Station :
				Station.Next_Location);
			String strList = strNumplate + L"^" + L"��֤�ɹ�,��ȥ��" +
				(0 == ChuRTag ? Station.Current_Station :
				Station.Next_Location);
			FinishCRC(0 == ChuRTag ? L"�����ͣ����.wav" : L"���뿪ͣ����.wav", strLed,
				strSta, strList, true, false, _sysInfo.TingID);
		}
		else {
			String strLed = strNumplate + L" ��ȥ��ָ������";
			String strSta = L"��֤�ɹ�,��ȥ��ָ������";
			String strList = strNumplate + L"^" + L"��֤�ɹ�,��ȥ��ָ������";
			FinishCRC(0 == ChuRTag ? L"�����ͣ����.wav" : L"���뿪ͣ����.wav", strLed,
				strSta, strList, true, false, _sysInfo.TingID);
		}
	}
	else {
		String strLed = strNumplate + (0 == ChuRTag ? L"�볡ʧ��:" : L"�볡ʧ��:") +
			GetLastErrS();
		String strSta = (0 == ChuRTag ? L"�볡ʧ��:" : L"�볡ʧ��:") + GetLastErrS();
		String strList = strNumplate + L"^" + (0 == ChuRTag ? L"�볡ʧ��:" :
			L"�볡ʧ��:") + GetLastErrS();
		FinishCRC(L"������.wav", strLed, strSta, strList, false, false,
			_sysInfo.TingID);
	}

	/*
	 if(0==ChuRTag)
	 {
	 if(EnterExitParking_pt(strNumplate,_sysInfo.TingID,NextStation)){

	 String strLed =strNumplate+L"��ȥ��"+NextStation;
	 String strSta =L"��֤�ɹ�,��ȥ��"+NextStation;
	 String strList=strNumplate+L"^"+L"��֤�ɹ�,��ȥ��"+NextStation;
	 FinishCRC(L"�����ͣ����.wav",strLed,strSta,strList ,true,false,_sysInfo.TingID);
	 }else{
	 String strLed =strNumplate+L"�볡ʧ��:"+GetLastErrS();
	 String strSta =L"�볡ʧ��:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"�볡ʧ��:"+GetLastErrS();
	 FinishCRC(L"������.wav",strLed,strSta,strList ,false,false,_sysInfo.TingID);
	 }
	 }
	 else if(1==ChuRTag)
	 {
	 if(EnterExitParking_pt(strNumplate,_sysInfo.TingID,NextStation)){

	 String strLed =strNumplate+L"��ȥ��"+NextStation;
	 String strSta =L"��֤�ɹ�,��ȥ��"+NextStation;
	 String strList=strNumplate+L"^"+L"��֤�ɹ�,��ȥ��"+NextStation;
	 FinishCRC(L"���뿪ͣ����.wav",strLed,strSta,strList ,true,false,_sysInfo.TingID);
	 }else{
	 String strLed =strNumplate+L"�볡ʧ��:"+GetLastErrS();
	 String strSta =L"�볡ʧ��:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"�볡ʧ��:"+GetLastErrS();
	 FinishCRC(L"������.wav",strLed,strSta,strList ,false,false,_sysInfo.TingID);
	 }
	 }
	 */
	return;
}

void __fastcall TServAutoR::AutoProc_QCH() {
	_WorkSta.CardID = "";
	_WorkSta.Car = "";
	if (!_bInitial || _ErrCnt != 0) {

		_WorkSta.StrRun = "����...";
		_WorkSta.StrFlow = "�޷�����...";
		return; // ---δ��ʼ��
	}

	_WorkSta.CardID = "";
	_WorkSta.Car = "";
	_WorkSta.StrRun = "������...";
	_WorkSta.StrFlow = "�ȴ�����...";
	SendLed(0, _sysInfo.Location);
	bool Debug = false;
	if (GetRealVal() < _sysInfo.TriggerWeight) { // ---δ�ﵽ�ذ���������

		HLDCtrl(GREEN, GREEN);
		return;
	}
	HLDCtrl(RED, RED);
	::Sleep(200);
	OutCtrl(DZ, HEAD, DOWN);

	SendLed(0, L"��ˢ��");
	Sleep(100);
	_File.PlayWav(L"��ˢ��.wav", SND_FILENAME | SND_SYNC);
	TCardInfo CardInfo;
	bool bResult;
	String ICardID;
	if (!Debug) {
		do {
			_WorkSta.StrFlow = L"��ˢ��," + Now().FormatString("hh:mm:ss");
			bResult = ReadCardID(2, CardInfo);
			Sleep(300);
			if (IsLeave()) {

				Sleep(2000);
				return;
			}
		}
		while (!bResult);
		ICardID = String((char *)CardInfo.ICCardID_Hex);
	}
	else {
		ICardID = "1234567890";
	}
	_WorkSta.CardID = ICardID;
	TCarInfo CarInfo;
	do {
		_WorkSta.StrFlow = L"���ڶ�ȡ������Ϣ," + Now().FormatString("hh:mm:ss");
		bResult = GetCarInfo_pt(ICardID, CarInfo);
		Sleep(2000);
		if (IsLeave()) {

			Sleep(2000);
			return;
		}
	}
	while (!bResult);
	String Numplate = CarInfo.NumPlate;
	_WorkSta.Car = Numplate;
	SendLed(0, Numplate);
	if (1 == CarInfo.IsLock) {

		FinishProc(L"����.wav", Numplate + L" ��������",
			L"��������," + Now().FormatString("hh:mm:ss"), Numplate + L"^��������",
			false, true);
		return;
	}

	// ---ȡ��ǰ����ҵ��ID
	int CarFlowID;
	UTF8String strFlow; // ---ҵ������
	do {
		_WorkSta.StrFlow = "���ڶ�ȡ����ҵ��ID," + Now().FormatString("hh:mm:ss");
		bResult = GetCarFlowID(Numplate, CarFlowID, strFlow);
		if (!bResult) {
			SendLed(0, "ҵ��ID��ȡʧ��");
		}
		Sleep(2000);
	}
	while (!bResult);

	int JilTag = -1; // ---�� �� �� �ı�� 0:�� 1:��
	if (CarFlowID == _FLowID[0])
		JilTag = 0;
	else if (CarFlowID == _FLowID[1])
		JilTag = 1;

	if (-1 == JilTag) {

		FinishProc(L"���̲���.wav", Numplate + L" ���̲���:Ӧ" + strFlow,
			L"���̲���:Ӧ" + strFlow + "," + Now().FormatString("hh:mm:ss"),
			Numplate + L"^���̲���:Ӧ" + strFlow, false, true);
		return;
	}
	switch (_sysInfo.QCHMode) {

	case 1: // ---�ذ�
		if (1 == JilTag) {
			FinishProc(L"��ȥ�ᳵ��.wav", Numplate + L" ��ȥ�ᳵ��",
				L"Ӧȥ�ᳵ��," + Now().FormatString("hh:mm:ss"),
				Numplate + L"^Ӧȥ�ᳵ��", false, true);
			return;
		}
		break;
	case 2: // ---���
		if (0 == JilTag) {
			FinishProc(L"��ȥ�س���.wav", Numplate + L" ��ȥ�س���",
				L"Ӧȥ�س���," + Now().FormatString("hh:mm:ss"),
				Numplate + L"^Ӧȥ�س���", false, true);
			return;
		}
		break;
	}

	SendLed(0, Numplate + L" ���� " + Format("%.2f",
		ARRAYOFCONST((GetRealVal()*0.001))));
	Sleep(2000);
	if (!DWQCheckProc(0, Numplate))
		return; // ---��λ������
	SendLed(0, Numplate + L" ���� " + Format("%.2f",
		ARRAYOFCONST((GetRealVal()*0.001))));
	Sleep(2000);

	_WorkSta.StrFlow = L"����ȡ�ȶ�ֵ," + Now().FormatString("hh:mm:ss");
	double fWeight = 0;
	do {
		bResult = GetStaticVal(fWeight);
		if (!bResult) {
			_WorkSta.StrFlow = L"ȡ�ȶ�ֵʧ��," + _LastErrS + "," + Now().FormatString
				("hh:mm:ss");
		}
		::Sleep(100);
	}
	while (!bResult);

	_WorkSta.StrFlow = L"�����ύ��������...";
	::Sleep(2000);

	TStation Station;

	if (0 == JilTag) {

		if (fWeight < _sysInfo.MinMZ) {
			String strLed = Numplate + L" ë�ع���,����:" + String(_sysInfo.MinMZ)
				+ L"Kg";
			String strSta = L"ë�ع���," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"ë�ع���,����:" + String(fWeight) +
				L"Kg��ë������:" + String(_sysInfo.MinMZ) + L"Kg ";
			FinishProc(L"ë�ع���.wav", strLed, strSta, strList, false, true);
			// ---������쳣ʱ����̧ǰ��
			return;
		}
		if (fWeight > 120000) {
			String strLed = Numplate + L"ë�س���,����:120000Kg";
			String strSta = L"ë�س���," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"ë�س���,����:" + String(fWeight) +
				"Kg��ë������:120000Kg ";
			FinishProc(L"ë�س���.wav", strLed, strSta, strList, false, true);
			// ---������쳣ʱ����̧ǰ��
			return;
		}

		if (SubmitJilInfo_pt(Numplate, fWeight * 0.001, 0)) {
			if (GetNextStation(Numplate, Station)) {
				String strLed = Numplate + L"���سɹ�" + String(fWeight * 0.001) +
					L"��ȥ��" + Station.Next_Location + Station.Next_Station;
				String strSta = L"���سɹ�," + String(fWeight * 0.001) + L"T,��ȥ��" +
					Station.Next_Location + Station.Next_Station;
				String strList = Numplate + L"^" + L"���سɹ�!" +
					String(fWeight * 0.001) + L"��,��ȥ��" + Station.Next_Location +
					Station.Next_Station;
				FinishProc(L"���سɹ�.wav", strLed, strSta, strList, true, true);
				// ---������쳣ʱ����̧ǰ��
			}
			else {
				String strLed = Numplate + L"���سɹ�" + String(fWeight * 0.001) +
					L"��ȥ��ָ��жú����";
				String strSta = L"���سɹ�," + String(fWeight * 0.001) +
					L"T,��ȥ��ָ��жú����";
				String strList = Numplate + L"^" + L"���سɹ�!" +
					String(fWeight * 0.001) + L"��,��ȥ��ָ��жú����";
				FinishProc(L"���سɹ�.wav", strLed, strSta, strList, true, true);
				// ---������쳣ʱ����̧ǰ��
			}
		}
		else {
			String strLed = Numplate + L"����ʧ��" + String(fWeight * 0.001) +
				L"T " + GetLastErrS();
			String strSta = L"����ʧ��," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"����ʧ��:" + GetLastErrS() +
				L",����:" + String(fWeight * 0.001) + L"��";
			FinishProc(L"����ʧ��.wav", strLed, strSta, strList, false, true);
			// ---������쳣ʱ����̧ǰ��
		}
		return;

	}
	else if (1 == JilTag) {

		if (fWeight < _sysInfo.MinPZ || fWeight > _sysInfo.MaxPZ) {
			String strLed = Numplate + L"Ƥ�س���,����:" + String(fWeight) +
				L"Kg������:" + String(_sysInfo.MinPZ) + L"Kg,����:" +
				String(_sysInfo.MaxPZ) + L"Kg";
			String strSta = L"Ƥ�س���," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"Ƥ�س���,����:" + String(fWeight) +
				L"Kg������:" + String(_sysInfo.MinPZ) + L"Kg,����:" +
				String(_sysInfo.MaxPZ) + L"Kg";
			FinishProc(L"Ƥ�س���.wav", strLed, strSta, strList, false, true);
			// ---������쳣ʱ����̧ǰ��
			return;
		}

		if (SubmitJilInfo_pt(Numplate, fWeight * 0.001, 1)) {
			if (GetNextStation(Numplate, Station)) {
				String strLed = Numplate + L"��Ƥ�ɹ�" + String(fWeight * 0.001); ;
				String strSta = L"��Ƥ�ɹ�," + String(fWeight * 0.001) + "T";
				String strList = Numplate + L"^" + L"��Ƥ�ɹ�!" +
					String(fWeight * 0.001) + L"��";
				FinishProc(L"��Ƥ�ɹ�.wav", strLed, strSta, strList, true, true);
				// ---������쳣ʱ����̧ǰ��
			}
		}
		else {
			String strLed = Numplate + L"��Ƥʧ��" + String(fWeight * 0.001) +
				L"T " + GetLastErrS(); ;
			String strSta = L"��Ƥʧ��," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"��Ƥʧ��:" + GetLastErrS() +
				L",����:" + String(fWeight * 0.001) + L"��";
			FinishProc(L"��Ƥʧ��.wav", strLed, strSta, strList, false, true);
			// ---������쳣ʱ����̧ǰ��
		}
		return;
	}
}

// ---ú���ڳ���
void __fastcall TServAutoR::AutoProc_MCCR() {
	if (_ErrCnt != 0)
		return; // ---�й���
	// _sysInfo.TingID  0��� 1���� 2��ú��
	if (_sysInfo.TingID < 2) {
		SendLed(1 == _sysInfo.TingID ? 1 : 0,
			_sysInfo.Location + (0 == _sysInfo.TingID ? L"���" : L"����"));
	}
	else {
		SendLed(1 == _sysInfo.TingID ? 1 : 0, _sysInfo.Location);
	}
	_WorkSta.Car = "";
	_WorkSta.CardID = "";
	_WorkSta.StrRun = "������...";
	// _WorkSta.StrFlow ="�ȴ�ˢ��...";
	TCardInfo CardInfo;
	String ICardID, strNumplate; // ---���ƺ�
	bool Debug = false;
	// ---ɨ��IC��
	_WorkSta.StrFlow = L"���ڶ�IC��," + Now().TimeString();
	if (!Debug) {
		if (!ReadCardID((1 == _sysInfo.TingID ? 1 : 0) + 2, CardInfo)) {
			return;
		}
		_WorkSta.CardID = ICardID = String((char *)CardInfo.ICCardID_Hex);
	}
	else {
		_WorkSta.CardID = ICardID = "A20AA8161608040001C4D41AA9D9791D";
	}
	_WorkSta.StrFlow = L"���ڶ�ȡ������Ϣ," + Now().TimeString();
	TCarInfo CarInfo;
	if (!GetCarInfo_pt(ICardID, CarInfo)) {

		_WorkSta.StrFlow = L"������Ϣ��ȡʧ��,IC����:" + ICardID + L",������Ϣ" +
			GetLastErrS() + L"," + Now().TimeString();
		SendLed(1 == _sysInfo.TingID ? 1 : 0, L"������Ϣ��ȡʧ��," + GetLastErrS());
		return;
	}
	_WorkSta.Car = strNumplate = CarInfo.NumPlate;
	SendLed(1 == _sysInfo.TingID ? 1 : 0, strNumplate);
	Sleep(3000);

	// ---�жϳ�����Ϣ��
	if (1 == CarInfo.IsLock) { // ---��������
		FinishCRC(L"����.wav", strNumplate + L"��������",
			L"��������," + Now().FormatString("hh:mm:ss"), strNumplate + L"^��������",
			false, false, 1 == _sysInfo.TingID ? 1 : 0);
		return;
	}

	int currFLowID;
	UTF8String FlowDescription; // ---ҵ������
	if (!GetCarFlowID(strNumplate, currFLowID, FlowDescription)) {

		_WorkSta.StrFlow = L"���������̴���," + GetLastErrS() + Now().TimeString();
		SendLed(1 == _sysInfo.TingID ? 1 : 0, strNumplate + L" ��ȡ�������̴���" +
			GetLastErrS());
		Sleep(5000);
		return;
	}

	int ChuRTag = -1; // ---�� �� �� �ı�� 0:�� 1:��
	if (currFLowID == _FLowID[0])
		ChuRTag = 0;
	else if (currFLowID == _FLowID[1])
		ChuRTag = 1;

	if (-1 == ChuRTag) {

		if (currFLowID < _FLowID[0]) {
			FinishCRC(L"���̲���.wav", strNumplate + L"���̲���:δ" + FlowDescription,
				L"���̲���:δ" + FlowDescription + "," + Now().FormatString
				("hh:mm:ss"), strNumplate + L"^���̲���:δ" + FlowDescription, false,
				false, 1 == _sysInfo.TingID ? 1 : 0);
		}
		else if (currFLowID > _FLowID[1]) {
			FinishCRC(L"���̲���.wav", strNumplate + L"���̲���:Ӧ" + FlowDescription,
				L"���̲���:Ӧ" + FlowDescription + "," + Now().FormatString
				("hh:mm:ss"), strNumplate + L"^���̲���:Ӧ" + FlowDescription, false,
				false, 1 == _sysInfo.TingID ? 1 : 0);
		}
		return;
	}
	switch (_sysInfo.TingID) {

	case 0: // ---�볡
		if (1 == ChuRTag) {
			FinishCRC(L"�ѽ���ú��.wav", strNumplate + L"�ó��ѽ���ú��",
				L"�ó��ѽ���ú��," + Now().FormatString("hh:mm:ss"),
				strNumplate + L"^�ó��ѽ���ú��", false, true,
				1 == _sysInfo.TingID ? 1 : 0);
			return;
		}
		break;
	case 1: // ---����
		if (0 == ChuRTag) {
			FinishCRC(L"δ����ú��.wav", strNumplate + L"�ó���δ����ú��",
				L"�ó���δ����ú��," + Now().FormatString("hh:mm:ss"),
				strNumplate + L"^�ó���δ����ú��", false, true,
				1 == _sysInfo.TingID ? 1 : 0);
			return;
		}
		break;
	}
	TStation Station;

	if (!GetNextStation(strNumplate, Station)) {
		_WorkSta.StrFlow = L"��ȡ�������̻��ڳ���," + GetLastErrS() + Now().TimeString();
		SendLed(1 == _sysInfo.TingID ? 1 : 0, strNumplate + L" ��ȡ�������̻��ڳ���" +
			GetLastErrS());
		Sleep(5000);
		return;
	}
	if (_sysInfo.Location.Pos(Station.Next_Location) <
		1 && Station.Next_Location.Length() > 0) {

		_WorkSta.StrFlow = L"δ����ָ������,Ӧȥ��" + Station.Next_Location +
			Now().TimeString();
		SendLed(1 == _sysInfo.TingID ? 1 : 0, strNumplate + L" δ����ָ������,Ӧȥ��" +
			Station.Next_Location);
		Sleep(5000);
		return;
	}
	UTF8String TmpLocation = Station.Next_Location;
	UTF8String TmpNextStation, TmpCurrentStation;

	// if(0==ChuRTag){
	if (_sysInfo.TingID < 2) { // ú��
		if (EnterExitMC_pt(strNumplate, _sysInfo.TingID, TmpLocation)) {
			if (GetNextStation(strNumplate, Station)) {
				String strLed = strNumplate + L"��ȥ��" +
					(1 == ChuRTag ? Station.Next_Location :
					Station.Current_Station);
				String strSta =
					L"��֤�ɹ�,��ȥ��" + (1 == ChuRTag ? Station.Next_Location :
					Station.Current_Station);
				String strList = strNumplate + L"^" + L"��֤�ɹ�,��ȥ��" +
					(1 == ChuRTag ? Station.Next_Location :
					Station.Current_Station);
				FinishCRC(L"���뿪ú��.wav", strLed, strSta, strList, false, false,
					1 == _sysInfo.TingID ? 1 : 0);
			}
			else {
				String strLed = strNumplate + L"��ȥ���ᳵ��";
				String strSta = L"��֤�ɹ�,��ȥ���ᳵ��" + GetLastErrS();
				String strList = strNumplate + L"^" + L"��֤�ɹ�,��ȥ���ᳵ��" +
					GetLastErrS();
				FinishCRC(L"���뿪ú��.wav", strLed, strSta, strList, false, false,
					1 == _sysInfo.TingID ? 1 : 0);
			}
		}
		else {
			String strLed = strNumplate + (0 == ChuRTag ? L"�볡ʧ��:" : L"�볡ʧ��:") +
				GetLastErrS();
			String strSta = (0 == ChuRTag ? L"�볡ʧ��:" : L"�볡ʧ��:") +
				GetLastErrS();
			String strList = strNumplate + L"^" + (0 == ChuRTag ? L"�볡ʧ��:" :
				L"�볡ʧ��:") + GetLastErrS();
			FinishCRC(L"������.wav", strLed, strSta, strList, false, false,
				1 == _sysInfo.TingID ? 1 : 0);
		}
	}
	else { // ú��

		if (EnterExitMC_pt(strNumplate, (1 == _sysInfo.TingID ? 1 : 0),
			TmpLocation)) {
			if (GetNextStation(strNumplate, Station)) {
				TmpCurrentStation = Station.Current_Station;
			}
			else {
				TmpCurrentStation = L"����жú��";
			}
		}
		else {
			String strLed = strNumplate + L"�볡ʧ��:" + GetLastErrS();
			String strSta = L"�볡ʧ��:" + GetLastErrS();
			String strList = strNumplate + L"^" + L"�볡ʧ��:" + GetLastErrS();
			FinishCRC(L"������.wav", strLed, strSta, strList, false, false,
				1 == _sysInfo.TingID ? 1 : 0);
			return;
		}
		if (EnterExitMC_pt(strNumplate, (0 == _sysInfo.TingID ? 0 : 1),
			TmpLocation)) {
			if (GetNextStation(strNumplate, Station)) {
				TmpNextStation = Station.Next_Location;
			}
			else {
				TmpNextStation = L"�ᳵ��";
			}
		}
		else {

			String strLed = strNumplate + L"�볡ʧ��:" + GetLastErrS();
			String strSta = L"�볡ʧ��:" + GetLastErrS();
			String strList = strNumplate + L"^" + L"�볡ʧ��:" + GetLastErrS();
			FinishCRC(L"������.wav", strLed, strSta, strList, false, false,
				1 == _sysInfo.TingID ? 1 : 0);
			return;
		}
		String strLed = strNumplate + L"��ȥ��" + TmpCurrentStation + L"жú,жú��ȥ��" +
			TmpNextStation;
		String strSta = L"��֤�ɹ�,��ȥ��" + TmpCurrentStation + L"жú,жú��ȥ��" +
			TmpNextStation;
		String strList = strNumplate + L"^" + L"��֤�ɹ�,��ȥ��" + TmpCurrentStation +
			L"жú,жú��ȥ��" + TmpNextStation;
		FinishCRC(L"���뿪ú��.wav", strLed, strSta, strList, false, false,
			1 == _sysInfo.TingID ? 1 : 0);
	}

	/*
	 }
	 else if(1==ChuRTag)
	 {
	 if(_sysInfo.TingID < 2){  //ú��
	 if(EnterExitMC_pt(strNumplate,_sysInfo.TingID)){

	 String strLed =strNumplate+L"��ȥ��"+NextStation;
	 String strSta =L"��֤�ɹ�,��ȥ��"+NextStation;
	 String strList=strNumplate+L"^"+L"��֤�ɹ�,��ȥ��"+NextStation;
	 FinishCRC(L"���뿪ú��.wav",strLed,strSta,strList ,true,false,1==_sysInfo.TingID ? 1:0);
	 }else{
	 String strLed =strNumplate+L"�볡ʧ��:"+GetLastErrS();
	 String strSta =L"�볡ʧ��:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"�볡ʧ��:"+GetLastErrS();
	 FinishCRC(L"������.wav",strLed,strSta,strList ,false,false,1==_sysInfo.TingID ? 1:0);
	 }
	 }
	 else{   //ú��
	 if(!EnterExitMC_pt(strNumplate,1==_sysInfo.TingID ? 1:0)){
	 String strLed =strNumplate+L"�볡ʧ��:"+GetLastErrS();
	 String strSta =L"�볡ʧ��:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"�볡ʧ��:"+GetLastErrS();
	 FinishCRC(L"������.wav",strLed,strSta,strList ,false,false,1==_sysInfo.TingID ? 1:0);
	 }else if(!EnterExitMC_pt(strNumplate,0==_sysInfo.TingID ? 0:1)){
	 String strLed =strNumplate+L"�볡ʧ��:"+GetLastErrS();
	 String strSta =L"�볡ʧ��:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"�볡ʧ��:"+GetLastErrS();
	 FinishCRC(L"������.wav",strLed,strSta,strList ,false,false,1==_sysInfo.TingID ? 1:0);
	 }
	 else{
	 String strLed =strNumplate+L"��ȥ��"+NextStation;
	 String strSta =L"��֤�ɹ�,��ȥ��"+NextStation;
	 String strList=strNumplate+L"^"+L"��֤�ɹ�,��ȥ��"+NextStation;
	 FinishCRC(L"���뿪ú��.wav",strLed,strSta,strList ,false,false,1==_sysInfo.TingID ? 1:0);
	 }
	 }
	 }
	 */
	return;
}

// ---�������ӿ�
void __fastcall TServAutoR::AutoProc_CYJ() {

	if (_bRead) {
		_WorkSta.CardID = "";
		_WorkSta.Car = "";
		if (_ErrCnt != 0)
			return; // ---�й���

		SendLed(0, _sysInfo.Location);
	}
	// ---��ȡ������״̬
	_WorkSta.StrFlow = L"���ڲ�ѯ������״̬," + Now().TimeString();
	::Sleep(1000);
	int nStatus;
	if (!SamStaInfo(_connKYC, _sysInfo.CYJNO, nStatus)) {

		_WorkSta.StrFlow = L"������״̬��ѯʧ��:" + GetLastErrS() + L"," +
			Now().TimeString();
		::Sleep(3000);
		return;
	}
	_WorkSta.StrFlow = L"������״̬:" + CYJStaDecode(nStatus) + L"," +
		Now().TimeString();
	_WorkSta.StrRun = CYJStaDecode(nStatus);
	GetRealTimeCaiYInfo(_sysInfo.CYJNO); // ��������С��ʵʱ������ʾ
	TCardInfo CardInfo;
	if (QueryIsStop(CarNumber)) { // ��;������������ͣ

		_WorkSta.StrFlow = L"��;��ͣ,��ˢIC����������:" + CYJStaDecode(nStatus) + L"," +
			Now().TimeString();
		_File.WriteLog(CarNumber + "��;��ͣ,��ˢIC������������");
		_File.PlayWav(L"��;��ͣ.wav");
		SendLed(0, CarNumber + "��;��ͣ,��ˢIC������������");

		_WorkSta.Car = CarNumber;
		::Sleep(3000);
		if (!ReadCardID(2, CardInfo)) {
			_WorkSta.StrFlow = L"ˢIC��ʧ��:" + GetLastErrS() + L"," +
				Now().TimeString();
			Sleep(300);
			return;
		}
		else if (String((char *)CardInfo.ICCardID_Hex) == StrICID) {
			_WorkSta.StrFlow = L"ˢ���ɹ�����������";
			SendLed(0, CarNumber + "ˢ���ɹ�����������");
			::Sleep(3000);
			if (!UpdateTagInfo(_sysInfo.CYJNO)) { // ���¿��Ʊ��־λ
				_WorkSta.StrFlow = L"���¿��Ʊ��־λʧ��:" + GetLastErrS() + L"," +
					Now().TimeString();
				_File.WriteLog(CarNumber + L"���¿��Ʊ��־λʧ��:" + GetLastErrS() +
					L"," + Now().TimeString());
				Sleep(300);
				return;
			}
			_WorkSta.StrFlow = L"���¿��Ʊ��־λ�ɹ�����������";
			SendLed(0, CarNumber + "���ڲ���");
			::Sleep(3000);
		}
		else {
			_WorkSta.StrFlow = "������IC����һ��";
			SendLed(0, CarNumber + "������IC����һ��");
			_File.PlayWav(L"������IC����һ��.wav");
			::Sleep(3000);
			return;
		}
	}
	TCYZBInfo CYZBInfo;
	GetCaiYZBInfo(CYZBInfo);  //-------������׼��״̬��ʵʱ���µ�ƽ̨����
    _WorkSta.StrFlow = "���ڼ�������¼��Ϣ�Ƿ�����...";
	::Sleep(500);
	CarSamplingData CayInfo = {0};
	if (GetCyComplete(CayInfo)) {

		_WorkSta.StrFlow = "�����ϴ�������¼";
		if (SubmitCaiYRecord_pt(CayInfo)) { // ---�ύ������ϸ��Ϣ��¼
			_WorkSta.StrFlow = "�ύ������¼�ɹ�";
			_File.WriteLog(_WorkSta.StrFlow);
			::Sleep(1000);
		}else{
			_WorkSta.StrFlow = "�ύ������¼ʧ��" + GetLastErrS();
			_File.WriteLog(_WorkSta.StrFlow);
			::Sleep(800);
		}
	}else{
		_WorkSta.StrFlow = "�޲�����ɼ�¼" + GetLastErrS(); // ---������ɼ�¼��Ϣ������
		::Sleep(200);
	}

    // ---��������������״̬��Ϣ
	_WorkSta.StrFlow = "���ڼ�⼯����״̬��Ϣ...";
	::Sleep(500);
	SamperCollectionData CollectInfo = {0};
	if (!GetCollectorInfo(CollectInfo)) {
		_WorkSta.StrFlow = "���¼�������Ϣʧ��" + GetLastErrS(); // ---���µļ�������Ϣʧ��
		::Sleep(200);
	}
	else {
		_WorkSta.StrFlow = "�ϴ�������״̬��Ϣ�ɹ�...";
		_File.WriteLog(_WorkSta.StrFlow);
		::Sleep(500);
	}
	// ---����
	if (0 == nStatus || 1 == nStatus || 2 == nStatus || 3 == nStatus ||
		4 == nStatus) { // ---��Щ״̬��,������

		Sleep(3000);
		return;
	}
	if (_bRead) {
		_WorkSta.StrFlow = L"���ڶ�RFID��," + Now().TimeString();
	   //	::Sleep(2000);
		if (!ReadCardID(0, CardInfo)) { // ---����ʧ��

			_WorkSta.StrFlow = L"��RFID��ʧ��:" + GetLastErrS() + L"," +
				Now().TimeString();
			Sleep(1000);
			return;
		}
		StrRFID = CardInfo.CardID[0];
		// "E20020292513015428300297"; //  "E2003065961101100830C59C";

	}
	_WorkSta.CardID = StrRFID;
	// ---��ȡ������Ϣ
	_WorkSta.StrFlow = L"���ڲ�ѯ������Ϣ," + Now().TimeString();
	TCarInfo CarInfo;
	CoInitialize(NULL);
	if (!GetCarInfo_pt(StrRFID, CarInfo)) {

	   /*
		FinishCRC(L"������ѯʧ��.wav", StrRFID + L"������Ϣ��ѯʧ��",
			L"������Ϣ��ѯʧ��:" + GetLastErrS() + L"," + Now().TimeString(),
			L"������Ϣ��ѯʧ��:" + GetLastErrS() + L"," + Now().TimeString(), false,
			false, 0);
	   */
	  //  _File.PlayWav(L"��;��ͣ.wav");
	  	_WorkSta.StrFlow = L"������Ϣ��ѯʧ��,"+GetLastErrS() + L"," + Now().TimeString();
		SendLed(0,L"������Ϣ��ѯʧ��");
		Sleep(3000);
		return;
	}
	_WorkSta.Car = CarNumber = CarInfo.NumPlate;
	StrICID = CarInfo.ICCardID;
	SendLed(0, CarInfo.NumPlate);
	Sleep(1000);
	// ---�жϳ���״̬
	_WorkSta.StrFlow = L"�����жϳ���״̬," + Now().TimeString();
	if (1 == CarInfo.IsLock) {
		FinishCRC(L"����.wav", CarInfo.NumPlate + L"��������",
			CarInfo.NumPlate + L"��������", CarInfo.NumPlate + L"��������", false,
			false, 0);
		Sleep(2000);
		return;
	}
	int currFLowID;
	UTF8String FlowDescription; // ---ҵ������
	if (!GetCarFlowID(CarInfo.NumPlate, currFLowID, FlowDescription)) {

		FinishCRC(L"���������̴���.wav", CarInfo.NumPlate + L" ��ȡ�������̴���",
			CarInfo.NumPlate + L" ��ȡ�������̴���", CarInfo.NumPlate + GetLastErrS() +
			L"," + Now().TimeString(), false, false, 0);
		Sleep(3000);
		return;
	}
	if (currFLowID != _FLowID[0]) {
		FinishCRC(L"���̲���.wav", CarInfo.NumPlate + L"���̲�����Ӧ" + FlowDescription,
			CarInfo.NumPlate + L"���̲�����Ӧ" + FlowDescription+ Now().TimeString(),
			L"���̲���:Ӧ" + FlowDescription  , false,
			false, 0);
		Sleep(5000);
		 return;
	}
	TCaiyInfo CaiyInfo;
	if (!GetCarCYInfo(CarInfo.NumPlate, CaiyInfo)) {    //---���ݳ��Ż�ȡ����������Ϣ��������������Ӧ����Ϣ����������
		/*
		FinishCRC(L"������Ϣ��ѯʧ��.wav", CarInfo.NumPlate + L"��ȡ����������Ϣʧ��",
			CarInfo.NumPlate + L"��ȡ����������Ϣʧ��:" + GetLastErrS() + L"," +
			Now().TimeString(), CarInfo.NumPlate + L"��ȡ����������Ϣʧ��:" +
			GetLastErrS() + L"," + Now().TimeString(), false, false, 0);
		*/
		_File.PlayWav(L"������Ϣ��ѯʧ��.wav");
		_WorkSta.StrFlow = L"��ȡ����������Ϣʧ�ܣ�"+GetLastErrS() + L"," + Now().TimeString();
		SendLed(0,L"������Ϣ��ѯʧ��");
		Sleep(3000);
		return;
	}

	if (_bRead) {
		//-----������������������봫�뻯���ҿ���
		if(!WritePICItoHuay(CaiyInfo)){
			_WorkSta.StrFlow = "д����������ͻ�����뵽�����ʧ��"+GetLastErrS();
			::Sleep(500);
		}
		// ---׼������  ,̧�ˣ�����׼������
		_WorkSta.StrFlow = "׼������";
		::Sleep(3000);
		_File.PlayWav(L"׼������.wav");
		OutCtrl(DZ, TAIL, UP);
		HLDCtrl(RED, RED);
		::Sleep(5000);
	}

	// ---ˢIC��,��ʼ����
	if (bResult) {

		_WorkSta.StrFlow = "ˢIC������ʼ����";
		SendLed(0, CarInfo.NumPlate + "��ˢIC����׼������");
		::Sleep(7000);
		_File.PlayWav(L"��ˢ��.wav", SND_FILENAME | SND_SYNC);
		_WorkSta.StrFlow = "��ˢ��," + Now().FormatString("hh:mm:ss");
		bResult = ReadCardID(2, CardInfo);
		if (!bResult) {
			_bRead = false;
			bResult = true;
			return;
		}
		else if (String((char*)CardInfo.ICCardID_Hex) == StrICID) {
			bResult = false;
		}
		else {
			_WorkSta.StrFlow = "IC����Ϣ����";
			_File.PlayWav(L"������IC����һ��.wav", SND_FILENAME | SND_SYNC);
			SendLed(0, CarInfo.NumPlate + "IC����Ϣ����");
			_bRead = false;
			return;
		}
		SendLed(0, CarInfo.NumPlate + "ˢ���ɹ�����ʼ����");
		_File.PlayWav(L"��ˢ�ɹ�.wav", SND_FILENAME | SND_SYNC);
		_WorkSta.StrFlow = "ˢ���ɹ�����ʼ����";
		::Sleep(500);
		_WorkSta.StrFlow = "����д�������Ϣ���м��";
		::Sleep(3000);
		if (!WriteCarInfoToCYJ(CarInfo, CaiyInfo)) { // --д�������Ϣ����Ԫ�м��

			_WorkSta.StrFlow = GetLastErrS();
			_File.PlayWav(L"����ʧ��.wav"); // ---����ʧ��
			::Sleep(100);
			::Sleep(2000);
			_File.WriteLog(CarInfo.NumPlate + GetLastErrS());
			_bRead = false;
			bResult = true;
			return;
		}
		SendLed(0, CarInfo.NumPlate + "���ڲ���");
		UpdateCaiYZTInfo_pt(_sysInfo.Location, CarInfo.NumPlate);
		// ���Ͳ����������ŵ�ҳ�棬֪ͨ�豸��ؽ�����³�����Ϣ
		_bRead = false;
	}

	_WorkSta.StrFlow = "���ڼ������Ƿ����...";
	::Sleep(3000);

	// -------������ɣ���������
	if (QueryCMDState(CarNumber)) {
		if (!SubmitCaiYInfo_pt(CarNumber)) {
			_WorkSta.StrFlow = "�ύ��������ʧ��" + GetLastErrS();
			::Sleep(2000);
			_File.WriteLog(CarInfo.NumPlate + GetLastErrS());
			return;
		}
		String strLed = CarInfo.NumPlate + L"�������";
		String strSta = L"�������";
		String strList = CarInfo.NumPlate + L"^" + L"�������!";
		FinishCRC(L"�������.wav", strLed, strSta, strList, true, false, 0);
		HLDCtrl(GREEN, GREEN);
		::Sleep(3000);
		_WorkSta.Car = "";
		_WorkSta.CardID = "";
		bResult = true;
		_bRead = true;
		return;
	}

}

// ----���ƻ������ֻ��豸����״̬�ɼ�
void __fastcall TServAutoR::AutoProc_ZTCJ() {

	String SQL;
	String Table[8] = {"CY_ERR_Tb", "ZY_ERR_Tb", "UploadInfo","TB_TESTRESULT_6550","ERROR_INFO","SAMPLEREC","CABINET_INFO","DEVICE_STATUS"};
	String Data_State[7] = {"DataStatus", "DataStatus", "isupload","ISGETVALUE","RDFLAG","DATA_STATUS","DATA_STATUS"};
	String Time[7] = {"ErrorTime","ErrorTime","TestTime","STARTINGTIME","CreateTime","OpTime","DepositTime"};
	String data[7] = {"0","0","1","1","0","0","0"};
	::Sleep(2000);

	try {

		memoStr = "----" + Now().DateTimeString() + "  ��������...";
		::Sleep(2000);
		for (int i = 0; i < 8; i++) {
			if(7 == i){
			   SQL = "select * from DEVICE_STATUS ";

			}else{
			   SQL = "select * from " + Table[i] + " where " +
				Data_State[i] + " = "+data[i]+" order by "+Time[i]+" desc";
			}
			String cyj_code = "", ErrTime = "", ErrCode = "", ErrInfo = "";
			int REC_NO = 0;
			TClientDataSet *CDS = new TClientDataSet(NULL);
			OleVariant OleVar;
			switch (i) {
				case 0:
					if(_connKYC_cfg.IsUse){
					OleVar = _connKYC->GetDatasetEX(SQL);
					CDS->Data = OleVar;
					break;
					}else continue;
				case 1:
					if(_connKYZ_cfg.IsUse){
					OleVar = _connKYZ->GetDatasetEX(SQL);
					CDS->Data = OleVar;
					break;
					}else continue;
				case 2:
					if(_connHYS_cfg.IsUse){
					OleVar = _connHYS->GetDatasetEX(SQL);
					CDS->Data = OleVar;
					break;
					}else continue;
				case 3:
					if(_connQSY_cfg.IsUse){
					OleVar = _connQSY->GetDatasetEX(SQL);
					CDS->Data = OleVar;
					break;
					}else continue;
				case 4:
					if(_connCYG_cfg.IsUse){
					OleVar = _connCYG->GetDatasetEX(SQL);
					CDS->Data = OleVar;
					break;
					}else continue;
				case 5:
					if(_connCYG_cfg.IsUse){
					OleVar = _connCYG->GetDatasetEX(SQL);
					CDS->Data = OleVar;
					break;
					}else continue;
				case 6:
					if(_connCYG_cfg.IsUse){
					OleVar = _connCYG->GetDatasetEX(SQL);
					CDS->Data = OleVar;
					break;
					}else continue;
				case 7:
					if(_connCYG_cfg.IsUse){
					OleVar = _connCYG->GetDatasetEX(SQL);
					CDS->Data = OleVar;
					break;
					}else continue;
			}

			if (CDS->RecordCount > 0) {

				switch (i) {
				case 0:
					for (int j = 0; j < CDS->RecordCount; j++) {
						cyj_code =
							CDS->FieldByName("MachineCode")->AsString.c_str();
						ErrTime =
							CDS->FieldByName("ErrorTime")->AsString.c_str();
						ErrInfo = CDS->FieldByName("ErrorDec")->AsString.c_str();

						CoInitialize(NULL);
						if (!SaveCaiYZTInfo_pt(cyj_code, ErrTime, ErrInfo)) {
							_LastErrS = "�ϴ�����������״̬��Ϣʧ��:" + GetLastErrS();
							memoStr = "�����ϴ�����,����������:" + cyj_code + ",ʱ��:" +
								ErrTime + ",������Ϣ:" + ErrInfo + ",����:" +
								_LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "�ϴ�,����������:" + cyj_code + ",ʱ��:" +
								ErrTime + ",������Ϣ:" + ErrInfo;
							::Sleep(2000);
							_File.WriteLog(memoStr);
							SQL = "update " + Table[i] + " set " +
								Data_State[i] + " = 1 where                          \
								DataStatus = 0";
							_connKYC->SQLExcute(SQL);
							CDS->Next();
						}
					}
					break;
				case 1:
					for (int j = 0; j < CDS->RecordCount; j++) {
						cyj_code =
							CDS->FieldByName("MachineCode")->AsString.c_str();
						ErrTime =
							CDS->FieldByName("ErrorTime")->AsString.c_str();
						ErrInfo =
							CDS->FieldByName("ErrorDec")->AsString.c_str();
						ErrCode =
							CDS->FieldByName("ErrorCode")->AsString.c_str();

						CoInitialize(NULL);
						if (!SaveZhiYZTInfo_pt(cyj_code, ErrTime, ErrInfo,
							ErrCode)) {
							_LastErrS = "�ϴ�����������״̬��Ϣʧ��:" + GetLastErrS();
							memoStr = "�����ϴ�����,����������:" + cyj_code + ",ʱ��:" +
								ErrTime + ",������Ϣ:" + ErrInfo + ",����:" +
								_LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();

						}
						else {
							memoStr = "�ϴ�,����������:" + cyj_code + ",ʱ��:" +
								ErrTime + ",������Ϣ:" + ErrInfo + ",�������:" +
								ErrCode;
							::Sleep(2000);
							_File.WriteLog(memoStr);
							SQL = "update " + Table[i] + " set " +
								Data_State[i] + " = 1 where                     \
								DataStatus = 0";
							_connKYZ->SQLExcute(SQL);
							CDS->Next();
						}
					}
					break;
				case 2:
					for (int j = 0; j < CDS->RecordCount; j++) {

						THYSInfo HYSInfo;
						HYSInfo.S_NO =
							CDS->FieldByName("S_NO")->AsString.c_str();
						HYSInfo.Mt = CDS->FieldByName("Mt")->AsFloat;
						HYSInfo.Mad = CDS->FieldByName("Mad")->AsFloat;
						HYSInfo.Mar = CDS->FieldByName("Mar")->AsFloat;
						HYSInfo.Aad = CDS->FieldByName("Aad")->AsFloat;
						HYSInfo.Ad = CDS->FieldByName("Ad")->AsFloat;
						HYSInfo.Aar = CDS->FieldByName("Aar")->AsFloat;
						HYSInfo.Vad = CDS->FieldByName("Vad")->AsFloat;
						HYSInfo.Vd = CDS->FieldByName("Vd")->AsFloat;
						HYSInfo.Varb = CDS->FieldByName("Varb")->AsFloat;
						HYSInfo.Vdaf = CDS->FieldByName("Vdaf")->AsFloat;
						HYSInfo.Stad = CDS->FieldByName("Stad")->AsFloat;
						HYSInfo.Std = CDS->FieldByName("Std")->AsFloat;
						HYSInfo.Star = CDS->FieldByName("Star")->AsFloat;
						HYSInfo.Fcad = CDS->FieldByName("Fcad")->AsFloat;
						HYSInfo.Fcd = CDS->FieldByName("Fcd")->AsFloat;
						HYSInfo.Fcar = CDS->FieldByName("Fcar")->AsFloat;
						HYSInfo.Had = CDS->FieldByName("Had")->AsFloat;
						HYSInfo.Qbad = CDS->FieldByName("Qbad")->AsFloat;
						HYSInfo.QbadCal = CDS->FieldByName("QbadCal")->AsFloat;
						HYSInfo.Qgrad = CDS->FieldByName("Qgrad")->AsFloat;
						HYSInfo.QgrdCal = CDS->FieldByName("QgrdCal")->AsFloat;
						HYSInfo.Qnetar = CDS->FieldByName("Qnetar")->AsFloat;
						HYSInfo.QnetarCal =
							CDS->FieldByName("QnetarCal")->AsFloat;
						HYSInfo.TestTime =
							CDS->FieldByName("TestTime")->AsString.c_str();
						HYSInfo.JZTZ = CDS->FieldByName("JZTZ")->AsInteger;
						HYSInfo.isupload =
							CDS->FieldByName("isupload")->AsInteger;

						CoInitialize(NULL);
						if (!SaveHUAYInfo_pt(HYSInfo)) {
							_LastErrS = "�ϴ���������ʧ��:" + GetLastErrS();
							memoStr = "���������ϴ�����,������:" + HYSInfo.S_NO + ",ʱ��:" +
								HYSInfo.TestTime + ",����:" + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "�ϴ�,������:" + HYSInfo.S_NO + ",ʱ��:" +
								HYSInfo.TestTime + "";
							::Sleep(2000);
							_File.WriteLog(memoStr);
							SQL = "update " + Table[i] + " set " +
								Data_State[i] + " = 2 where         \
								Mt = "+HYSInfo.Mt+" and Mad = "+HYSInfo.Mad+"";
							_connHYS->SQLExcute(SQL);
							CDS->Next();

						}
					}
				case 3:
					for ( int j = 0; j <CDS->RecordCount ; j++ ) {

						TQSYInfo QSYInfo;
						QSYInfo.ID =
							CDS->FieldByName("ID")->AsInteger;
						QSYInfo.SAMPLENO = CDS->FieldByName("SAMPLENO")->AsString.c_str();
						QSYInfo.POSITIONNO= CDS->FieldByName("POSITIONNO")->AsInteger;
						QSYInfo.SAMPLENAME = CDS->FieldByName("SAMPLENAME")->AsString.c_str();
						QSYInfo.TRAYWEIGHT = CDS->FieldByName("TRAYWEIGHT")->AsFloat;
						QSYInfo.SAMPLEWEIGHT = CDS->FieldByName("SAMPLEWEIGHT")->AsString.c_str();
						QSYInfo.LEFTWEIGHT = CDS->FieldByName("LEFTWEIGHT")->AsString.c_str();
						QSYInfo.MOISTURE = CDS->FieldByName("MOISTURE")->AsFloat;
						QSYInfo.STARTINGTIME = CDS->FieldByName("STARTINGTIME")->AsString.c_str();
						QSYInfo.ENDINGTIME = CDS->FieldByName("ENDINGTIME")->AsString.c_str();
						QSYInfo.STATUS = CDS->FieldByName("STATUS")->AsString.c_str();
						QSYInfo.TEMPOFCONSTANTTEMPERATURE = CDS->FieldByName("TEMPOFCONSTANTTEMPERATURE")->AsFloat;
						QSYInfo.TIMEOFCONSTANTTEMPERATURE = CDS->FieldByName("TIMEOFCONSTANTTEMPERATURE")->AsFloat;
						QSYInfo.TEMPERATURECOMPENSATION = CDS->FieldByName("TEMPERATURECOMPENSATION")->AsFloat;
						QSYInfo.TIMESOFDRYING = CDS->FieldByName("TIMESOFDRYING")->AsFloat;
						QSYInfo.INTERVALTIME = CDS->FieldByName("INTERVALTIME")->AsFloat;
						QSYInfo.ANALYSISPRECISION = CDS->FieldByName("ANALYSISPRECISION")->AsFloat;
						QSYInfo.NUMBERINGMETHOD = CDS->FieldByName("NUMBERINGMETHOD")->AsString.c_str();
						QSYInfo.OPERATOR = CDS->FieldByName("OPERATOR")->AsString.c_str();
						QSYInfo.REMARK = CDS->FieldByName("REMARK")->AsString.c_str();

						CoInitialize(NULL);
						if (!SaveQSYInfo_pt(QSYInfo)) {
							_LastErrS = "�ϴ�ȫˮ����ʧ��:" + GetLastErrS();
							memoStr = "ȫˮ�����ϴ�����,���:" + QSYInfo.SAMPLENO + ",��ʼʱ��:" +
								QSYInfo.STARTINGTIME + ",����:" + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "�ϴ�,������:" + QSYInfo.SAMPLENO + ",��ʼʱ��:" +
								QSYInfo.STARTINGTIME + "";
							::Sleep(2000);
							_File.WriteLog(memoStr);
							SQL = "update " + Table[i] + " set " +
								Data_State[i] + " = 2 where                          \
								TRAYWEIGHT ="+QSYInfo.TRAYWEIGHT+" and SAMPLEWEIGHT ="+QSYInfo.SAMPLEWEIGHT+"";
							_connQSY->SQLExcute(SQL);
							CDS->Next();

						}
					}
				case 6:
					for ( int j = 0; j <CDS->RecordCount ; j++ ) {

						TCYGInfo CYGInfo;
						String str = "True";
						CYGInfo.REC_NO = CDS->FieldByName("REC_NO")->AsInteger;
						CYGInfo.GGName = CDS->FieldByName("GGName")->AsString.c_str();
						CYGInfo.CCNAME = CDS->FieldByName("CCName")->AsString.c_str();
						CYGInfo.WWNAME = CDS->FieldByName("WWName")->AsString.c_str();
						str == CDS->FieldByName("Enabled")->AsString.c_str()?CYGInfo.ENABLED =1: CYGInfo.ENABLED =0;
						CYGInfo.DEPOSITTIME = CDS->FieldByName("DepositTime")->AsString.c_str();
						CYGInfo.MAKESAMPLETIME = CDS->FieldByName("MakeSampleTime")->AsString.c_str();
						CYGInfo.SAMPLETYPE = CDS->FieldByName("SampleType")->AsInteger;
						CYGInfo.SAMPLEID = CDS->FieldByName("SampleId")->AsString.c_str();
						CYGInfo.SAMPLESTATUS = CDS->FieldByName("SampleStatus")->AsString.c_str();

						CoInitialize(NULL);
						if (!SaveCYGInfo_pt(CYGInfo)) {
							_LastErrS = "�ϴ�����������ʧ��:" + GetLastErrS();
							memoStr = "�����������ϴ�����,���:" + CYGInfo.SAMPLEID + ",��Ʒ����ʱ��:" +
								CYGInfo.DEPOSITTIME + ",����:" + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "�ϴ�,��������:" + CYGInfo.SAMPLEID + ",��Ʒ����ʱ��:" +
								CYGInfo.DEPOSITTIME + "";
							::Sleep(2000);
							_File.WriteLog(memoStr);
							SQL = "update " + Table[i] + " set " +
								Data_State[i] + " = 1 where                          \
								SampleId ="+CYGInfo.SAMPLEID+" and REC_NO ="+CYGInfo.REC_NO+"";
							_connCYG->SQLExcute(SQL);
							CDS->Next();

						}
					}
				case 5:
					for ( int j = 0; j <CDS->RecordCount ; j++ ) {

						TCYGCQInfo CYGCQInfo;
						String str = "True";
						CYGCQInfo.REC_NO = CDS->FieldByName("REC_NO")->AsInteger;
						CYGCQInfo.GGName = CDS->FieldByName("GGName")->AsString.c_str();
						CYGCQInfo.CCNAME = CDS->FieldByName("CCNAME")->AsString.c_str();
						CYGCQInfo.WWNAME = CDS->FieldByName("WWNAME")->AsString.c_str();
						CYGCQInfo.OpType = CDS->FieldByName("OpType")->AsInteger;
						CYGCQInfo.OpTime = CDS->FieldByName("OpTime")->AsString.c_str();
						CYGCQInfo.MakeSampleTime = CDS->FieldByName("MakeSampleTime")->AsString.c_str();
						CYGCQInfo.SAMPLETYPE = CDS->FieldByName("SAMPLETYPE")->AsInteger;
						CYGCQInfo.SourceName = CDS->FieldByName("SourceName")->AsString.c_str();
						CYGCQInfo.DestinationName = CDS->FieldByName("DestinationName")->AsString.c_str();
						CYGCQInfo.SampleID = CDS->FieldByName("SampleID")->AsString.c_str();
						str == CDS->FieldByName("IsSuccessed")->AsString.c_str()?CYGCQInfo.IsSuccessed =1:CYGCQInfo.IsSuccessed =0;
						CYGCQInfo.Operator = CDS->FieldByName("Operator")->AsString.c_str();

						CoInitialize(NULL);
						if (!SaveCYGCQInfo_pt(CYGCQInfo)) {
							_LastErrS = "�ϴ��������ȡ����ʧ��:" + GetLastErrS();
							memoStr = "�������ȡ�ϴ�����,���:" + CYGCQInfo.SampleID + ",��Ʒ��ȡ����ʱ��:" +
								CYGCQInfo.OpTime + ",����:" + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "�ϴ�,�������ȡ���:" + CYGCQInfo.SampleID + ",��Ʒ��ȡ����ʱ��:" +
								CYGCQInfo.OpTime + "";
							::Sleep(2000);
							_File.WriteLog(memoStr);
							SQL = "update " + Table[i] + " set " +
								Data_State[i] + " = 1 where                          \
								SampleID ="+CYGCQInfo.SampleID+" and REC_NO ="+CYGCQInfo.REC_NO+"";
							_connCYG->SQLExcute(SQL);
							CDS->Next();

						}
					}
				case 4:
					for ( int j = 0; j <CDS->RecordCount ; j++ ) {

						REC_NO = CDS->FieldByName("REC_NO")->AsInteger;
						cyj_code =
							CDS->FieldByName("DeviceCode")->AsString.c_str();
						ErrTime =
							CDS->FieldByName("CreateTime")->AsString.c_str();
						ErrInfo =
							CDS->FieldByName("ErrorDesc")->AsString.c_str();
						ErrCode =
							CDS->FieldByName("ErrorCode")->AsString.c_str();

						CoInitialize(NULL);
						if (!SaveCYGYXInfo_pt(cyj_code, ErrTime, ErrInfo,
							ErrCode)) {
							_LastErrS = "�ϴ����������м�¼ʧ��:" + GetLastErrS();
							memoStr = "�ϴ�,����������:" + cyj_code + ",ʱ��:" +
								ErrTime + ",������Ϣ:" + ErrInfo + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "�ϴ�,�豸����:" + cyj_code + ",ʱ��:" +
								ErrTime + ",������Ϣ:" + ErrInfo;
							::Sleep(2000);
							_File.WriteLog(memoStr);
							SQL = "update " + Table[i] + " set " +
								Data_State[i] + " = 1 where                    \
								RDFLAG = 0 and REC_NO="+REC_NO+"";
							_connCYG->SQLExcute(SQL);
							CDS->Next();

						}
					}
				case 7:
					for ( int j = 0; j <CDS->RecordCount ; j++ ) {

						TQDGDInfo QDGDInfo;
						QDGDInfo.REC_NO =	CDS->FieldByName("REC_NO")->AsInteger;
						QDGDInfo.DeviceName = CDS->FieldByName("DeviceName")->AsString.c_str();
						QDGDInfo.DeviceStatus1 =  CDS->FieldByName("DeviceStatus1")->AsInteger;
						QDGDInfo.DeviceStatus2 = CDS->FieldByName("DeviceStatus2")->AsInteger;
						QDGDInfo.DeviceStatus3 = CDS->FieldByName("DeviceStatus3")->AsInteger;
						QDGDInfo.LineStatus = CDS->FieldByName("LineStatus")->AsInteger;

						CoInitialize(NULL);
						if (!SaveQDGDInfo_pt(QDGDInfo)) {
							_LastErrS = "�ϴ������ܵ���Ϣʧ��:" + GetLastErrS();
							memoStr = "�ϴ��豸����:"+QDGDInfo.DeviceName+",������Ϣ:" + QDGDInfo.LineStatus + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "�ϴ�,�豸����:" +QDGDInfo.DeviceName +",������Ϣ:" +QDGDInfo.LineStatus;
							::Sleep(2000);
							_File.WriteLog(memoStr);
							CDS->Next();
						}
					}
					break;
				}
			}
			CDS->Free();
		}
	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "�޸��¼�¼:" + e.Message;
		memoStr = "ϵͳ����:" + e.Message;
		_File.WriteLog(memoStr);
		::Sleep(2000);
	}
	if (_bRead) {
		memoStr = "	---�ɼ�������ͣ,30s����������";
		::Sleep(30000);
	}
}
