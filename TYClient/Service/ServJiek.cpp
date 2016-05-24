// ---------------------------------------------------------------------------

#pragma hdrstop

#include "ServJiek.h"
#include "MD5.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
String Gkdw = "";

TServJiek::TServJiek() {

}

TServJiek::~TServJiek() {

}

// ---������&���������� �м��ӿ�

/*************************************************
 Function:		SamStaInfo
 Description:	����������״̬��ѯ
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: MACHIN_STATUS_INFO
 Table Updated:  ��
 Others:
 Author: 		���ı�
 Date:			2016-03-22
 ************************************************ */

UTF8String TServJiek::SamStaInfo(TLX_DB *connKYC, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	int nStatus;
	if (SamStaInfo(connKYC, Location, nStatus)) {

		Ret_JsonData["Status"] = UTF8Encode(IntToStr(nStatus)).c_str();
		return _jRet.CreatJsonRet(0, L"", 1, Ret_JsonData);
	}
	else {
		return _jRet.CreatJsonRet(1, UTF8Encode(_LastErrS), 0, Ret_JsonData);
	}
}

bool TServJiek::SamStaInfo(TLX_DB *connKYC, String Location, int &nStatus) {
	String Sql =
		L"select SAM_READY from MACHIN_STATUS_INFO where CYJ_MACHINE='" +
		Location + "' ";
	try {

		connKYC->GetDataSet(Sql);
		if (0 == connKYC->DSInfo.nRecordCount) {
			connKYC->CloseDataset();
			_LastErrS = L"δ�鵽����";
			_File.WriteLog_ErrS(L"SamStaInfo:" + _LastErrS);
			return false;
		}
		else {
			nStatus = connKYC->FieldAsName(0, L"SAM_READY")->AsInteger;
			connKYC->CloseDataset();
			return true;
		}
	}
	catch (...) {
		connKYC->Disconn();
		_LastErrS = connKYC->GetLastErr();
		_File.WriteLog_ErrS(L"SamStaInfo:" + _LastErrS);
		return false;
	}
}

String TServJiek::CYJStaDecode(int nStatus) {
	switch (nStatus) {
	case 0:
		return L"������";
	case 1:
		return L"���ڲ���";
	case 2:
		return L"��������";
	case 3:
		return L"����ж��";
	case 4:
		return L"��ͣ";
	case 5:
		return L"�����ɲ�";
	case 6:
		return L"������ж";
	default:
		return L"δ֪״̬:" + IntToStr(nStatus);
	}
}

/*************************************************
 Function:		CollectorInfo
 Description:	������������״̬��ѯ
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: MACHIN_STATUS_INFO
 Table Updated:  ��
 Others:
 Author: 		���ı�
 Date:			2016-03-22
 ************************************************ */
UTF8String TServJiek::CollectorInfo(TLX_DB *connKYC, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		Json::Value val;
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}
	String Sql = L"select BARRELCODE Ͱ��,CY_CODE ��������,DOWNCONT ������,WEIGHT ����,LAST_UPDATE_TIME ʱ�� \
	             from BARREL_STATUS_INFO where CYJ_MACHINE='" + Location + "' ";
	try {

		connKYC->GetDataSet(Sql);
		if (0 == connKYC->DSInfo.nRecordCount) {
			connKYC->CloseDataset();
			_LastErrS = "δ�鵽����";
			_File.WriteLog_ErrS(L"CollectorInfo:" + _LastErrS);
			return _jRet.CreatJsonRet(1, L"δ�鵽����", 0, Ret_JsonData);
		}
		else {

			int nRows = connKYC->DSInfo.nRecordCount;

			TStrings *List = new TStringList();
			WideString colList = "";
			connKYC->GetFieldNames(colList); // ---ȡ����
			List->Text = String(colList);

			Json::Value JsonData; // ---���Ҫ���ɶ�������
			Json::Value dataset; // ---���ݼ�����

			for (int i = 0; i < nRows; i++) {

				for (int j = 0; j < List->Count; j++) {

					dataset[UTF8Encode(List->Strings[j]).c_str()] =
						UTF8Encode(connKYC->FieldAsName(i,
						List->Strings[j])->AsString).c_str();
				}
				JsonData.append(dataset);
			}
			List->Free();
			connKYC->CloseDataset();
			return _jRet.CreatJsonRet(0, NULL, nRows, JsonData);
		}
	}
	catch (...) {
		connKYC->Disconn();
		_LastErrS = connKYC->GetLastErr();
		_File.WriteLog_ErrS(L"CollectorInfo:" + _LastErrS);
		return _jRet.CreatJsonRet(1, UTF8String(_LastErrS).c_str(), 0,
			Ret_JsonData);
	}
}

/*************************************************
 Function:		SamControl
 Description:	����������
 Input:          JsonReq:Json��ʽ����
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: MACHIN_CMD_INFO
 Table Updated:  MACHIN_CMD_INFO
 Others:
 Author: 		���ı�
 Date:			2016-03-22
 ************************************************ */
UTF8String TServJiek::SamControl(TLX_DB *connKYC, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	int ActionID;
	if (!_jReq.GetActionID(JsonReq, ActionID)) {
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	if (SamControl(connKYC, Location, ActionID)) {

		return _jRet.CreatJsonRet(0, L"�ɹ�", 0, Ret_JsonData);
	}
	else {
		return _jRet.CreatJsonRet(1, UTF8Encode(GetLastErrS()), 0,
		Ret_JsonData);
	}

}

bool TServJiek::SamControl(TLX_DB *connKYC, String Location, int ActionID) {
	String Sql = L"select * from MACHIN_CMD_INFO where CYJ_MACHINE ='" +
		Location + "'  ";
	try {

		_connKYC->GetDataSet(Sql);
		if (0 == _connKYC->DSInfo.nRecordCount) {

			_connKYC->AppendDS();
			_connKYC->FieldAsName(L"CYJ_MACHINE", Location.ToInt());
			_connKYC->FieldAsName(L"CY_CONTROL", ActionID);
			_connKYC->FieldAsName(L"SEND_TIME", Now().CurrentDateTime());
			_connKYC->PostDS();
			_connKYC->CloseDataset();

			return true;
		}
		else if (1 == _connKYC->DSInfo.nRecordCount) {

			_connKYC->EditDS();
			_connKYC->FieldAsName(L"CY_CONTROL", ActionID);
			_connKYC->FieldAsName(L"SEND_TIME", Now().CurrentDateTime());
			_connKYC->PostDS();
			_connKYC->CloseDataset();

			return true;
		}
		else {
			_connKYC->CloseDataset();
			_LastErrS = L"Ҫ���������ݲ�Ψһ";
			return false;
		}
	}
	catch (...) {
		_connKYC->Disconn();
		_LastErrS = _connKYC->GetLastErr();
		return false;
	}
}

/*************************************************
 Function:		ZYSamStaInfo
 Description:	����������״̬��ѯ
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: ZY_Status_Tb
 Table Updated:
 Others:
 Author: 		���ı�
 Date:			2016-03-23
 ************************************************ */
UTF8String TServJiek::ZYSamStaInfo(TLX_DB *connKYZ, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	String StatusID;
	if (ZYSamStaInfo(connKYZ, Location, StatusID)) {

		Ret_JsonData["Status"] = UTF8Encode(StatusID).c_str();
		return _jRet.CreatJsonRet(0, L"", 1, Ret_JsonData);
	}
	else {
		return _jRet.CreatJsonRet(1, UTF8Encode(_LastErrS), 0, Ret_JsonData);
	}
}

bool TServJiek::ZYSamStaInfo(TLX_DB *connKYZ, String Location, String &StatusID)
{
	String Sql = L"select SamReady from ZY_Status_Tb where MachineCode='" +
		Location + "' ";
	try {

		_connKYZ->GetDataSet(Sql);
		if (0 == _connKYZ->DSInfo.nRecordCount) {
			_connKYZ->CloseDataset();
			_LastErrS = L"δ�鵽������״̬����";
			return false;
		}
		else {
			StatusID = _connKYZ->FieldAsName(0, L"SamReady")->AsString;
			_connKYZ->CloseDataset();
			return true;
		}
	}
	catch (...) {
		_connKYZ->Disconn();
		_LastErrS = _connKYZ->GetLastErr();
		return false;
	}
}

/*************************************************
 Function:		CollectorInfo
 Description:	��������ϸ״̬��ѯ
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: ZY_State_Tb
 Table Updated:  ��
 Others:
 Author: 		���ı�
 Date:			2016-03-23
 ************************************************ */
UTF8String TServJiek::ZYSamDetailInfo(TLX_DB *connKYZ, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		Json::Value val;
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}
	String Sql = L"select DeviceCode �豸���,DeviceName �豸����,SampleID ��������,DeviceStatus ����״̬,LastUpdateTime ʱ�� \
	             from ZY_State_Tb where MachineCode='" + Location + "' ";
	try {

		_connKYZ->GetDataSet(Sql);
		if (0 == _connKYZ->DSInfo.nRecordCount) {
			_connKYZ->CloseDataset();
			_LastErrS = "δ�鵽����";
			return _jRet.CreatJsonRet(1, L"δ�鵽����", 0, Ret_JsonData);
		}
		else {

			int nRows = _connKYZ->DSInfo.nRecordCount;

			TStrings *List = new TStringList();
			WideString colList = "";
			_connKYZ->GetFieldNames(colList); // ---ȡ����
			List->Text = String(colList);

			Json::Value JsonData; // ---���Ҫ���ɶ�������
			Json::Value dataset; // ---���ݼ�����

			for (int i = 0; i < nRows; i++) {

				for (int j = 0; j < List->Count; j++) {

					dataset[UTF8Encode(List->Strings[j]).c_str()] =
						UTF8Encode(_connKYZ->FieldAsName(i,
						List->Strings[j])->AsString).c_str();
				}
				JsonData.append(dataset);
			}
			List->Free();
			_connKYZ->CloseDataset();
			_connKYZ->SQLExcute
				(L"Update ZY_State_Tb Set DataStatus=1 where MachineCode='" +
				Location + "' ");
			return _jRet.CreatJsonRet(0, NULL, nRows, JsonData);
		}
	}
	catch (...) {
		_connKYZ->Disconn();
		_LastErrS = _connKYZ->GetLastErr();
		return _jRet.CreatJsonRet(1, UTF8String(_LastErrS).c_str(), 0,
			Ret_JsonData);
	}
}

String TServJiek::ZYJStaDecode(int nStatus) {
	switch (nStatus) {
	case 0:
		return L"ֹͣ";
	case 1:
		return L"����������(��������һ����)";
	case 2:
		return L"����";
	case 3:
		return L"����(����ж��)";
	case 4:
		return L"��ͣ";
	case 5:
		return L"��������,�ȴ�����";
	default:
		return L"δ֪״̬";
	}
}

/*************************************************
 Function:		ZYSamControl
 Description:	����������
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: ZY_Cmd_Tb
 Table Updated:  ZY_Cmd_Tb
 Others:
 Author: 		���ı�
 Date:			2016-03-23
 ************************************************ */
UTF8String TServJiek::ZYSamControl(TLX_DB *connKYZ, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	int ActionID;
	if (!_jReq.GetActionID(JsonReq, ActionID)) {
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	String Sql = L"select * from ZY_Cmd_Tb where MachineCode ='" +
		Location + "'  ";
	try {

		_connKYZ->GetDataSet(Sql);
		if (0 == _connKYZ->DSInfo.nRecordCount) {

			_connKYZ->AppendDS();
			_connKYZ->FieldAsName(L"MachineCode", Location.ToInt());
			_connKYZ->FieldAsName(L"CommandCode", ActionID);
			_connKYZ->FieldAsName(L"DataStatus", 0);
			_connKYZ->FieldAsName(L"SendCommandTime", Now().CurrentDateTime());
			_connKYZ->PostDS();
			_connKYZ->CloseDataset();

			return _jRet.CreatJsonRet(0, L"����ɹ�", 0, Ret_JsonData);
		}
		else if (1 == _connKYZ->DSInfo.nRecordCount) {

			_connKYZ->EditDS();
			_connKYZ->FieldAsName(L"CommandCode", ActionID);
			_connKYZ->FieldAsName(L"DataStatus", 0);
			_connKYZ->FieldAsName(L"SendCommandTime", Now().CurrentDateTime());
			_connKYZ->PostDS();
			_connKYZ->CloseDataset();

			return _jRet.CreatJsonRet(0, L"���³ɹ�", 0, Ret_JsonData);
		}
		else {
			_connKYZ->CloseDataset();
			return _jRet.CreatJsonRet(1, L"Ҫ���������ݲ�Ψһ", 0, Ret_JsonData);
		}
	}
	catch (...) {
		_connKYZ->Disconn();
		_LastErrS = _connKYZ->GetLastErr();
		return _jRet.CreatJsonRet(1, UTF8String(_LastErrS).c_str(), 0,
			Ret_JsonData);
	}
}

// ---������ж��
/*************************************************
 Function:		SamUnload
 Description:	д�������ж����Ϣ
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: MACHIN_CMD_INFO
 Table Updated:  MACHIN_CMD_INFO
 Others:
 Author: 		���ı�
 Date:			2016-03-22
 ************************************************ */
UTF8String TServJiek::SamUnload(TLX_DB *connKYC, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	// ---ȡ����
	UTF8String Cybm;
	if (!_jReq.GetCZYBM(JsonReq, Cybm, 0)) {
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	if (SamUnload(connKYC, Location, Cybm)) {

		return _jRet.CreatJsonRet(0, L"�ɹ�", 0, Ret_JsonData);
	}
	else {
		_File.WriteLog_ErrS(L"SamUnload:" + _LastErrS);
		return _jRet.CreatJsonRet(1, UTF8String(_LastErrS).c_str(), 0,
			Ret_JsonData);
	}
}

bool TServJiek::SamUnload(TLX_DB *connKYC, String Location, String Cybm) {
	String Sql = L"select * from KY_DOWN_OPT where /*CY_CODE='" + Cybm +
		"' and*/ CYJ_MACHINE ='" + Location + "'  "; // haoriliang
	try {

		connKYC->GetDataSet(Sql);
		if (0 == connKYC->DSInfo.nRecordCount) {

			connKYC->AppendDS();
			connKYC->FieldAsName(L"CYJ_MACHINE", Location.ToInt());
			connKYC->FieldAsName(L"CY_CODE", Cybm);
			connKYC->FieldAsName(L"XL_Start", 1);
			connKYC->FieldAsName(L"XL_Finish", 0);
			connKYC->FieldAsName(L"LastDateTime", Now());
			connKYC->PostDS();
			connKYC->CloseDataset();

			return true;
		}
		else if (1 == connKYC->DSInfo.nRecordCount) {

			connKYC->EditDS();
			connKYC->FieldAsName(L"CY_CODE", Cybm);
			connKYC->FieldAsName(L"XL_Start", 1);
			connKYC->FieldAsName(L"XL_Finish", 0);
			connKYC->FieldAsName(L"LastDateTime", Now());
			connKYC->PostDS();
			connKYC->CloseDataset();

			return true;
		}
		else {
			connKYC->CloseDataset();
			_LastErrS = L"Ҫ���������ݲ�Ψһ";
			return false;
		}
	}
	catch (...) {
		connKYC->Disconn();
		_LastErrS = connKYC->GetLastErr();
		return false;
	}
}


/* ��ȡ������ж�����״̬ */
bool TServJiek::ReadSamLoadStatus(TLX_DB *connKYC, String Location, String Cybm)
{
	String Sql = L"select * from KY_DOWN_OPT where CY_CODE='" + Cybm +
		"' and CYJ_MACHINE ='" + Location + "'  ";
	try {

		connKYC->GetDataSet(Sql);
		if (0 == connKYC->DSInfo.nRecordCount) {

			connKYC->CloseDataset();
			return false;
		}
		else if (1 == connKYC->DSInfo.nRecordCount) {

			if (2 == connKYC->FieldAsName(0,
				L"XL_Start")->AsInteger && 1 == connKYC->FieldAsName(0,
				L"XL_Finish")->AsInteger) {
				connKYC->EditDS();
				connKYC->FieldAsName(L"XL_Finish", 0);
				connKYC->FieldAsName(L"LastDateTime", Now());
				connKYC->CloseDataset();
				return true;
			}
			else {
				connKYC->CloseDataset();
				return false;
			}

		}
		else {
			connKYC->CloseDataset();
			_LastErrS = L"Ҫ���������ݲ�Ψһ";
			return false;
		}
	}
	catch (...) {
		connKYC->Disconn();
		_LastErrS = connKYC->GetLastErr();
		return false;
	}
}

/*************************************************
 Function:		ZYSamUnload
 Description:	д��������ж����Ϣ
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: ZY_Interface_Tb
 Table Updated:  ZY_Interface_Tb
 Others:
 Author: 		���ı�
 Date:			2016-03-22
 ************************************************ */
UTF8String TServJiek::ZYSamUnload(TLX_DB *connKYZ, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	// ---ȡ����
	UTF8String Zybm;
	if (!_jReq.GetCZYBM(JsonReq, Zybm, 1)) {
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	if (ZYSamUnload(connKYZ, Location, Zybm)) {

		return _jRet.CreatJsonRet(0, L"�ɹ�", 0, Ret_JsonData);
	}
	else {
		return _jRet.CreatJsonRet(1, UTF8String(_LastErrS).c_str(), 0,
			Ret_JsonData);
	}
}

bool TServJiek::ZYSamUnload(TLX_DB *connKYZ, String Location, String Zybm) {
	String Sql = L"select * from ZY_Interface_Tb /* where SampleID='" + Zybm +
		"' */  "; // haoriliang
	try {

		connKYZ->GetDataSet(Sql);
		if (0 == connKYZ->DSInfo.nRecordCount) {

			connKYZ->AppendDS();
			connKYZ->FieldAsName(L"SampleID", Zybm);
			connKYZ->FieldAsName(L"DataStatus", 0);
			connKYZ->PostDS();
			connKYZ->CloseDataset();

			return true;
		}
		else if (1 == connKYZ->DSInfo.nRecordCount) {

			connKYZ->EditDS();
			connKYZ->FieldAsName(L"SampleID", Zybm);
			connKYZ->FieldAsName(L"DataStatus", 0);
			connKYZ->PostDS();
			connKYZ->CloseDataset();

			return true;
		}
		else {
			connKYZ->CloseDataset();
			_LastErrS = L"Ҫ���������ݲ�Ψһ";
			return false;
		}
	}
	catch (...) {
		connKYZ->Disconn();
		_LastErrS = connKYZ->GetLastErr();
		return false;
	}
}

bool TServJiek::ZYSamControl(TLX_DB *connKYZ, String Location, String Zybm,
	int Openr) {
	String Sql = L"select * from ZY_Cmd_Tb  where MachineCode='" + Location +
		"'   "; // haoriliang
	try {

		connKYZ->GetDataSet(Sql);
		if (0 == connKYZ->DSInfo.nRecordCount) {

			connKYZ->AppendDS();
			connKYZ->FieldAsName(L"MachineCode", Location);
			connKYZ->FieldAsName(L"CommandCode", Openr);
			connKYZ->FieldAsName(L"SampleCode", Zybm);
			connKYZ->FieldAsName(L"SendCommandTime", Now());
			connKYZ->FieldAsName(L"DataStatus", 0);
			connKYZ->PostDS();
			connKYZ->CloseDataset();

			return true;
		}
		else if (1 == connKYZ->DSInfo.nRecordCount) {

			connKYZ->EditDS();
			connKYZ->FieldAsName(L"CommandCode", Openr);
			connKYZ->FieldAsName(L"SampleCode", Zybm);
			connKYZ->FieldAsName(L"SendCommandTime", Now());
			connKYZ->FieldAsName(L"DataStatus", 0);
			connKYZ->PostDS();
			connKYZ->CloseDataset();

			return true;
		}
		else {
			connKYZ->CloseDataset();
			_LastErrS = L"Ҫ���������ݲ�Ψһ";
			return false;
		}
	}
	catch (...) {
		connKYZ->Disconn();
		_LastErrS = connKYZ->GetLastErr();
		return false;
	}
}

/* ��ȡ������¼ */
bool TServJiek::ReadZyRecord(TLX_DB *connKYZ, String Location, String Zybm) {
	String Sql = L"select * from ZY_Record_Tb where MachineCode='" + Location +
		"' and SampleID='" + Zybm + "' /* and DataStatus=0 */";
	try {

		connKYZ->GetDataSet(Sql);
		if (0 == connKYZ->DSInfo.nRecordCount) {

			connKYZ->CloseDataset();
			return false;
		}
		else {

			connKYZ->EditDS();
			connKYZ->FieldAsName(L"DataStatus", 1);
			connKYZ->PostDS();
			connKYZ->CloseDataset();
			return true;
		}
	}
	catch (...) {
		connKYZ->Disconn();
		_LastErrS = connKYZ->GetLastErr();
		return false;
	}
}

/* ���Ϻ�������ӿ�,������������ */
UTF8String TServJiek::ZYSamUnload_New(UTF8String JsonReq) {
	Json::Value Ret_JsonData;
	if (_bEnterSamUnload) {

		if (!SaveZhiybjData_pt(_ZYLocation, "����ж��,���Ժ�", L"0")) {

		_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" + _LastErrS);
		}
		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}

		return _jRet.CreatJsonRet(1, L"����ж��,���Ժ�", 0, Ret_JsonData);
	}

	UTF8String CYLocation, ZYLocation;

	if (!_jReq.GetCZYLocation(JsonReq, CYLocation, ZYLocation)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}
	// ---ȡ����
	UTF8String Cybm, Zybm;
	if (!_jReq.GetCZYBM(JsonReq, Cybm, 0)) {
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}
	if (!_jReq.GetCZYBM(JsonReq, Zybm, 1)) {
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	_CYLocation = CYLocation;
	_ZYLocation = ZYLocation;
	_Cybm = Cybm;
	_Zybm = Zybm;

	_bEnterSamUnload = true; // ---����ж��״̬
	return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);
}

// ---ǿ����ֹж��
UTF8String TServJiek::ForceEndSamLoad(UTF8String JsonReq) {
	Json::Value Ret_JsonData;
	if (!_bEnterSamUnload) {

		return _jRet.CreatJsonRet(1, L"��ǰ��������������,������ֹ", 0, Ret_JsonData);
	}

	_bEnterSamUnload = false;
	_bForceEndSamUnload = true;

	return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);
}

// ---ж�Ϲ���,��Windows������߳��е���
void TServJiek::SamUnloadProc() {
	_bForceEndSamUnload = false; // ---һ������true,��������Ӧǿ�ƽ���
	// ---�жϲ�����״̬
	int nStatus;
	if (!SamStaInfo(_connKYC, _CYLocation, nStatus)) {

		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		// ---��ƽ̨�ӿ�
		_File.WriteLog_ErrS(L"SamStaInfo@SamUnloadProc:" + _LastErrS);
		if (!SaveZhiybjData_pt(_ZYLocation, _LastErrS, L"-1")) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}

	if (nStatus != 6) {

		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		// ---��ƽ̨�ӿڰ���Ϣд��
		String strMsg = L"���������߱�ж������:" + CYJStaDecode(nStatus);
		_File.WriteLog_ErrS(L"SamUnloadProc:" + strMsg);
		if (!SaveZhiybjData_pt(_ZYLocation, strMsg, IntToStr(nStatus))) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}

	// ---�ж�������״̬�Ƿ����ж��
	String StatusID;
	if (!ZYSamStaInfo(_connKYZ, _ZYLocation, StatusID)) {

		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		// ---��ƽ̨�ӿ�
		_File.WriteLog_ErrS(L"ZYSamStaInfo@SamUnloadProc:" + _LastErrS);
		if (!SaveZhiybjData_pt(_ZYLocation, _LastErrS, -1)) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}

	if (StatusID != L"3") {

		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		// ---��ƽ̨�ӿڰ���Ϣд��
		String strMsg = L"����������δ����,���²���������ж��:" + ZYJStaDecode
			(StatusID.ToIntDef(-1));
		if (!SaveZhiybjData_pt(_ZYLocation, strMsg, StatusID)) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_File.WriteLog_ErrS(L"SamUnloadProc:" + strMsg);
		_bEnterSamUnload = false;
		return;
	}

	// ---������ж��
	if (!SamUnload(_connKYC, _CYLocation, _Cybm)) {

		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		_File.WriteLog_ErrS(L"SamUnload@SamUnloadProc:" + _LastErrS);
		if (!SaveZhiybjData_pt(_ZYLocation, "������ж��ʧ��:" + _LastErrS,
			IntToStr(nStatus))) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}

	bool bRet;
	// ---�ȴ�����ж��״̬
	do {
		nStatus = -1;
		SamStaInfo(_connKYC, _CYLocation, nStatus); // ��ȡ������״̬
		_File.WriteLog(L"SamUnloadProc:�ȴ�����ж��״̬,������״̬:" +
			CYJStaDecode(nStatus));
		if (_bForceEndSamUnload) {

			_bEnterSamUnload = false; // ---�˳�ж��״̬
			_File.WriteLog(L"SamUnloadProc:�ȴ�����ж��״̬ʱ,��ǿ����ֹ");
			return;
		}

		if (0 == nStatus) {

			_File.WriteLog(L"SamUnloadProc:�ȴ�����ж��״̬ʱ,������:" +
				CYJStaDecode(nStatus));
			_bEnterSamUnload = false;
			return;
		}
		::Sleep(3000);
	}
	while (nStatus != 3);

	_File.WriteLog(L"SamUnloadProc:����������ж��״̬");
    // ---д��������������Ϣ
	if (!ZYSamUnload(_connKYZ, _ZYLocation, _Zybm)) {

		_File.WriteLog_ErrS(L"ZYSamUnload@SamUnloadProc:" + _LastErrS);
		if (!SaveZhiybjData_pt(_ZYLocation, "д��������������Ϣʧ��:" + _LastErrS, L"-1"))
		{

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}
	// ---�ȴ�ж�����
	do {
		bRet = ReadSamLoadStatus(_connKYC, _CYLocation, _Cybm);
		if (_bForceEndSamUnload) {

			_bEnterSamUnload = false; // ---�˳�ж��״̬
			_File.WriteLog(L"SamUnloadProc:�ȴ�������ж�����ʱ,��ǿ����ֹ");
			return;
		}
		::Sleep(3000);
	}
	while (!bRet);

	_File.WriteLog(L"SamUnloadProc:������ж�����,�ȴ����������������״̬");
	// ---�ٴ��ж�������״̬�Ƿ��������
	do{
		StatusID=-1;
		if (!ZYSamStaInfo(_connKYZ, _ZYLocation, StatusID)) {

			// ---��ƽ̨�ӿ�
			_File.WriteLog_ErrS(L"������ж�Ϻ�ZYSamStaInfo@SamUnloadProc:" + _LastErrS);
			if (!SaveZhiybjData_pt(_ZYLocation, _LastErrS, L"-1")) {

				_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
					_LastErrS);
			}
			_bEnterSamUnload = false;
			return;
		}
		if (_bForceEndSamUnload) {

			_bEnterSamUnload = false; // ---�˳�ж��״̬
			_File.WriteLog(L"SamUnloadProc:�ȴ����������״̬ʱ,��ǿ����ֹ");
			return;
		}
		::Sleep(2000);
	}
	while(StatusID == L"3");
	if (StatusID != L"5") {

		// ---��ƽ̨�ӿڰ���Ϣд��
		String strMsg = L"������ж�Ϻ�����������δ����,������������������:" +
			ZYJStaDecode(StatusID.ToIntDef(-1));
		_File.WriteLog_ErrS(L"SamUnloadProc:" + strMsg);
		if (!SaveZhiybjData_pt(_ZYLocation, _LastErrS, StatusID)) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		//���͵�ƽ̨ ��ж��δ����
		if (!UploadXlwcxx_pt(_Cybm, 3)) {

		_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}
	_File.WriteLog(L"SamUnloadProc:���������������״̬,д��������Ϣ");

	// ---д��������������Ϣ
	if (!ZYSamUnload(_connKYZ, _ZYLocation, _Zybm)) {

		_File.WriteLog_ErrS(L"ZYSamUnload@SamUnloadProc:" + _LastErrS);
		if (!SaveZhiybjData_pt(_ZYLocation, "д��������������Ϣʧ��:" + _LastErrS, L"-1"))
		{

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}
	 // ---����������
	 _File.WriteLog(L"SamUnloadProc:������������ʼ����");
	 if (!ZYSamControl(_connKYZ, _ZYLocation, _Zybm,1)) {

		 _File.WriteLog_ErrS(L"ZYSamUnload@SamUnloadProc:" + _LastErrS);
		 if (!SaveZhiybjData_pt(_ZYLocation, "д��������������Ϣʧ��:" + _LastErrS, L"-1"))
		 {

		 _File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
		 _LastErrS);
		 }
		 _bEnterSamUnload = false;
	 	return;
	 }

	// ---��ȡ������¼���ȴ��������
	_File.WriteLog(L"SamUnloadProc:�ȴ��������������");
	do {
		bRet = ReadZyRecord(_connKYZ, _ZYLocation, _Zybm);
		if (_bForceEndSamUnload) {

			_bEnterSamUnload = false; // ---�˳�ж��״̬
			_File.WriteLog(L"SamUnloadProc:�ȴ��������������ʱ,��ǿ����ֹ");
			return;
		}
		::Sleep(3000);
	}
	while (!bRet);
	_File.WriteLog(L"SamUnloadProc:�������������");

	if (!SaveZhiybjData_pt(_ZYLocation, "�������������", L"0")) {

		_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" + _LastErrS);
	}
	if (!UploadXlwcxx_pt(_Cybm, 1)) {

		_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
	}

	_bEnterSamUnload = false; // ---�˳�ж��״̬
}

// ---��д�������ӿ�,��ƽ̨ʹ��

/*************************************************
 Function:		ReadCardID
 Description:	ͨ����д����ȡ����
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed:
 Table Updated:
 Others:
 Author: 		���ı�
 Date:			2016-03-29
 ************************************************ */
UTF8String TServJiek::ReadCard(UTF8String JsonReq) {
	Json::Value Ret_JsonData;
	TCardInfo CardInfo;

	if (ReadCardID(0, CardInfo)) {

		String strCard = "";
		Json::Value json_Row; // ---ÿ��һ������,���뵽 Ret_JsonData��,�������
		if (_DevInfo.ReaderInfo[0].Model == L"IVT530") {

			json_Row["CardID"] = (char *)CardInfo.ICCardID_Hex;
			Ret_JsonData.append(json_Row);
			strCard = (char *)CardInfo.ICCardID_Hex;
		}
		else {
			for (int i = 0; i < CardInfo.Cnts; i++) {

				json_Row["CardID"] = UTF8String(CardInfo.CardID[i]).c_str();
				Ret_JsonData.append(json_Row);
				strCard += CardInfo.CardID[i] + L" ";
			}
		}

		_File.WriteLog(L"�����ɹ�,����:" + IntToStr(CardInfo.Cnts) + L",����:" +
			strCard);
		return _jRet.CreatJsonRet(0, L"", CardInfo.Cnts, Ret_JsonData);
	}
	else {
		return _jRet.CreatJsonRet(1, L"����ʧ��:" + _LastErrS, CardInfo.Cnts,
			Ret_JsonData);
	}
}

UTF8String TServJiek::WriteCard(UTF8String JsonReq) {
	Json::Value jv_req;
	Json::Value Ret_JsonData;

	if (!_jRet.JsonStrToJson_pt(JsonReq, jv_req)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"WriteCard:" + _LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData); ;
	}

	try {
		Json::Value JsonData;
		JsonData = jv_req["JsonData"];

		int nSector = JsonData["Sector"].asInt();
		int nBlock = JsonData["Block"].asInt();
		UTF8String strPsw = JsonData["Psw"].asCString();
		UTF8String strData = JsonData["Data"].asCString();

		if (nSector < 0 || nBlock > 15) {

			_LastErrS = L"д��ʧ��:������������:" + IntToStr(nSector);
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (0 == nSector && 0 == nBlock) {

			_LastErrS = L"д��ʧ��:0��0��Ϊֻ����";
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (3 == nBlock) {

			_LastErrS = L"д��ʧ��:��3Ϊ����洢��,�ݲ�֧��д��";
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (nBlock < 0 || nBlock > 3) {

			_LastErrS = L"д��ʧ��:����Ŀ���:" + IntToStr(nBlock);
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		TDataSwitch DS;
		if (!DS.IsValid_HexStr(strPsw)) {

			_LastErrS = L"д��ʧ��:�����ʽ����:" + strPsw;
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (strData.Length() > 16) {

			_LastErrS = L"д��ʧ��:���ݳ���16�ֽ�:" + strData;
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (!WriteICCard(0, nSector, nBlock, strPsw, strData)) {

			_LastErrS = L"д��ʧ��:" + _LastErrS;
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}
		return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);

	}
	catch (Exception &e) {

		_LastErrS = L"д��ʧ��:" + e.Message;
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
	catch (...) {

		_LastErrS = L"д��ʧ��:δ֪����";
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
}

// ---ͣ����LED������Ϣ
/* ������Ϣ */
UTF8String TServJiek::SendLED_Diaod(UTF8String JsonReq) {
	Json::Value jv_req;
	Json::Value Ret_JsonData;

	if (!_jRet.JsonStrToJson_pt(JsonReq, jv_req)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"SendLED_Diaod:" + _LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData); ;
	}

	try {
		Json::Value JsonData;
		Json::Value JsonData_Numplate; // ---��������
		JsonData = jv_req["JsonData"];
		JsonData_Numplate = JsonData["Numplate"];

		_CarList->Clear();
		for (unsigned int i = 0; i < JsonData_Numplate.size(); i++) {

			_CarList->Add(UTF8String(JsonData_Numplate[i].asCString()));
		}

		_ParkingNum_Total = JsonData["ParkingNum_Total"].asInt(); // ---ͣ�����ܳ�λ��
		_ParkingNum_Surplus = JsonData["ParkingNum_Surplus"].asInt();
		// ---ͣ����ʣ�೵λ
		_ScheduledNum = JsonData["ScheduledNum"].asInt(); // ---��ǰ���ڵı����ȳ���
		_ToScheduleNum = JsonData["ToScheduleNum"].asInt(); // ---��ǰ����δ���Գ���

		_bSendLED_DDInfo = true;
		return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);

	}
	catch (Exception &e) {

		_LastErrS = L"SendLED_Diaod�쳣:" + e.Message + L",����:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
	catch (...) {

		_LastErrS = L"SendLED_Diaod�쳣:δ֪���󡣴���:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(2, _LastErrS, 0, Ret_JsonData);
	}
}

// ---LDE������Ϣ
UTF8String TServJiek::SendLED_Gongg(UTF8String JsonReq) {
	Json::Value jv_req;
	Json::Value Ret_JsonData;

	if (!_jRet.JsonStrToJson_pt(JsonReq, jv_req)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"SendLED_Gongg:" + _LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData); ;
	}

	try {
		Json::Value JsonData;
		JsonData = jv_req["JsonData"];

		_GGTitle = UTF8String(JsonData["Title"].asCString()); // ---����
		_GGText = UTF8String(JsonData["Text"].asCString()); // ---ͣ����ʣ�೵λ
		_GGDepartment = UTF8String(JsonData["Department"].asCString());
		// ---��ǰ���ڵı����ȳ���
		_GGDate = UTF8String(JsonData["Date"].asCString()); // ---��ǰ����δ���Գ���

		_bSendLED_GGInfo = true;
		return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);

	}
	catch (Exception &e) {

		_LastErrS = L"SendLED_Gongg:" + e.Message + L",����:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
	catch (...) {

		_LastErrS = L"SendLED_Gongg:δ֪���󡣴���:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(2, _LastErrS, 0, Ret_JsonData);
	}
}

// ---д�������Ϣ���������м��
bool TServJiek::WriteCarInfoToCYJ(TCarInfo &CarInfo, TCaiyInfo &CaiyInfo) {

	String Sql = "select * from MACHIN_CMD_INFO where CYJ_MACHINE='" +
		_sysInfo.CYJNO + "'";

	String _Sql = "select * from RLCARMST where CYJ_MACHINE='" +
		_sysInfo.CYJNO + "'";

	try {

		_connKYC->GetDataSet(_Sql);
		if (_connKYC->DSInfo.nRecordCount > 0) { // ---������,����

			String Sql_1 = "update RLCARMST set            \
							CAR_NUMBER='" + CarInfo.NumPlate + "',                                 \
							CAR_LONG='" + CarInfo.Legth + "',                                 \
							CAR_WIDTH='" + CarInfo.Width + "',                                 \
							CAR_HEIGHT='" + CarInfo.Dipg + "',                                  \
							LJ_SUM='" + CarInfo.Lajs + "',                                  \
							G_START='" + CarInfo.GuaStart + "',                               \
							G_END='" + CarInfo.GuaEnd + "',                                \
							DataStatus =0                                             \
							where CYJ_MACHINE ='" + _sysInfo.CYJNO + "'";

			try {
				_connKYC->SQLExcute(Sql_1);
			}
			catch (Exception &e) {
				_connKYC->Disconn();
				_LastErrS = "������Ϣ������ʱsqlִ��ʧ��:" + e.Message;
				_File.WriteLog_ErrS(_LastErrS);
				return false;
			}
			try {
				for (int i = 0; i < 6; i++) {
					if (!String(CarInfo.Ljwz[i]).IsEmpty()) {

						String Sql_2 = "update RLCARMST set LJ_" +
							IntToStr(i + 1) + "=" + CarInfo.Ljwz[i] + "      \
									   where CYJ_MACHINE = '" +
							_sysInfo.CYJNO + "'";

						try {

							_connKYC->SQLExcute(Sql_2);
						}
						catch (Exception &e) {
							_connKYC->Disconn();
							_LastErrS = "����������Ϣ����ʱsqlִ��ʧ��" + e.Message;
							_File.WriteLog_ErrS(_LastErrS);
							return false;
						}

					}
				}
			}
			catch (...) {
				_LastErrS = "����ƽ̨�ṩ�Ĳ��������ַ���ʱ����:" + String(CarInfo.Ljwz[12]);
				_File.WriteLog_ErrS(_LastErrS);
				return false;
			}
		}
		else {
			_connKYC->CloseDataset();
			_LastErrS = "δ�鵽����";
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}
		::Sleep(3000);

		_connKYC->CloseDataset();

		try {
			_connKYC->GetDataSet(Sql);
			if (_connKYC->DSInfo.nRecordCount > 0) { // ---������,����
				String Sql = "update MACHIN_CMD_INFO set            \
								CAR_NUMBER='" + CarInfo.NumPlate + "',                                 \
								CY_CODE='" + CaiyInfo.CaiyBH + "',                                 \
								RFID_ID='" + CarInfo.CardID + "',                                 \
								CY_TYPE='" + CaiyInfo.CaiyLX + "',                                  \
								CY_POINT='" + CaiyInfo.CaiyDS + "',                                  \
								UINT='" + CaiyInfo.GongYS + "',                               \
								CY_CONTROL = 1,                                \
								CY_STATE = 0                                             \
								where CYJ_MACHINE ='" + _sysInfo.CYJNO + "'";
				try {
					_connKYC->SQLExcute(Sql);
				}
				catch (Exception &e) {
					_connKYC->Disconn();
					_LastErrS = "�������Ʊ�����ʱsqlִ��ʧ��" + e.Message;
					_File.WriteLog_ErrS(_LastErrS);
					return false;
				}

			}
			else {
				_connKYC->CloseDataset();
				_LastErrS = "δ�鵽����";
				_File.WriteLog_ErrS(_LastErrS);
				return false;
			}
			_connKYC->CloseDataset();
		}
		catch (...) {
			_connKYC->Disconn();
			_LastErrS = _connKYC->GetLastErr();
			_File.WriteLog_ErrS(_LastErrS);
			return false;

		}
		return true;

	}
	catch (...) {
		_connKYC->Disconn();
		_LastErrS = _connKYC->GetLastErr();
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
}

// ---�жϲ�����¼�Ƿ�����
bool TServJiek::GetCyComplete(CarSamplingData &CayInfo) {
	String Sql = "select * from TAKE_SAMPLE_REC where CYJ_MACHINE='" +
		_sysInfo.CYJNO + "'     \
					and DataStatus =0 and WEIGHT_DataStatus = 1";

	TClientDataSet *CDS = new TClientDataSet(NULL);
	try {
		OleVariant OleVar;
		OleVar = _connKYC->GetDatasetEX(Sql);
		CDS->Data = OleVar;
		if (CDS->RecordCount > 0) { // ---��⵽��������ź�

			strcpy(CayInfo.SimplingLocation,
				AnsiString(_sysInfo.Location).c_str());
			strcpy(CayInfo.VehicleNo,
				CDS->FieldByName("CAR_NUMBER")->AsAnsiString.c_str());
			strcpy(CayInfo.SupplierCode,
				CDS->FieldByName("CY_CODE")->AsAnsiString.c_str());
			strcpy(CayInfo.SimplingType,
				CDS->FieldByName("CY_TYPE")->AsAnsiString.c_str());
			strcpy(CayInfo.BatchInfoId,
				CDS->FieldByName("ZY_TYPE")->AsAnsiString.c_str());
			CayInfo.SimplingPoint = CDS->FieldByName("CY_POINT")->AsInteger;
			CayInfo.SetBucketNo = CDS->FieldByName("BARREL_CODE")->AsInteger;
			strcpy(CayInfo.SimplingTarget,
				CDS->FieldByName("COORDINATE")->AsAnsiString.c_str());
			strcpy(CayInfo.SimplingStartTime,
				CDS->FieldByName("BEGIN_DATE")->AsAnsiString.c_str());
			strcpy(CayInfo.SimplingEndTime,
				CDS->FieldByName("END_DATE")->AsAnsiString.c_str());
			CayInfo.SetBucketWeight = CDS->FieldByName("WEIGHT")->AsFloat;
			strcpy(CayInfo.SamplingPerson,
				CDS->FieldByName("CY_USER")->AsAnsiString.c_str());

			CDS->Free();

			// ---ȡ�����ݺ�,���±�־λ
			Sql = "update TAKE_SAMPLE_REC set DataStatus =1,WEIGHT_DataStatus = 2 where DataStatus =0 and WEIGHT_DataStatus = 1";
			if (!_connKYC->SQLExcute(Sql)) {
				_LastErrS = "���²�����¼��־λʧ��";
				_File.WriteLog_ErrS(_LastErrS);
				return false;
			}
			else
				return true;

		}
		else {
			_LastErrS = "�޲�����ɼ�¼";
			CDS->Free();
			return false;
		}
	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "��ȡ��������ź�ʱ����:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		CDS->Free();
		return false;
	}
}

// ---------�жϲ�����״̬�Ƿ���ɲ���
bool TServJiek::QueryCMDState(String Car) {
	String Sql = "select * from MACHIN_CMD_INFO where CYJ_MACHINE='" +
		_sysInfo.CYJNO + "' and CAR_NUMBER ='" + Car + "' and CY_STATE =3";
	try {
		_connKYC->GetDataSet(Sql);
		if (_connKYC->DSInfo.nRecordCount > 0) {

			return true;

		}
		else {
			_connKYC->Disconn();
			_LastErrS = "�޲�����¼,����δ���:" + _connKYC->GetLastErr();
			return false;
		}
	}
	catch (...) {
		_connKYC->Disconn();
		_LastErrS = "��ѯ����״̬�Ƿ����ʧ��:" + _connKYC->GetLastErr();
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
}

// ---------�жϲ�����״̬�Ƿ�����;��ͣ
bool TServJiek::QueryIsStop(String Car) {
	String Sql = "select * from MACHIN_CMD_INFO where CYJ_MACHINE='" +
		_sysInfo.CYJNO + "' and CAR_NUMBER ='" + Car + "' and CY_STATE =2";
	try {
		_connKYC->GetDataSet(Sql);
		if (_connKYC->DSInfo.nRecordCount > 0) {

			return true;

		}
		else {
			_connKYC->Disconn();
			_LastErrS = "��������;δ��ͣ";
			return false;
		}
	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "��ѯ�������Ƿ���;��ͣʧ��:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
}

// ---��;ֹͣ��ˢ�����²��������Ʊ�־
bool TServJiek::UpdateTagInfo(String Location) {

	String _Sql = "select * from RLCARMST where CYJ_MACHINE='" + Location +
		"' and DataStatus =1";

	String Sql_ = "select * from MACHIN_CMD_INFO where CYJ_MACHINE='" +
		Location + "' and CY_STATE =2";

	try {

		_connKYC->GetDataSet(_Sql);
		if (_connKYC->DSInfo.nRecordCount > 0) {
			String Sql =
				"update RLCARMST set DataStatus =0 where CYJ_MACHINE='" +
				Location + "'";

			if (!_connKYC->SQLExcute(Sql)) {
				_connKYC->Disconn();
				_LastErrS = "������Ϣ�����±�־λʱsqlִ��ʧ��:" + _connKYC->GetLastErr();
				return false;
			}
			_connKYC->CloseDataset();
		}
		else {
			_connKYC->Disconn();
			_LastErrS = "�޲�����¼�����±�־λʧ��:" + _connKYC->GetLastErr();
			return false;
		}
		::Sleep(300);

		_connKYC->GetDataSet(Sql_);
		if (_connKYC->DSInfo.nRecordCount > 0) {
			String Sql1 =
				"update MACHIN_CMD_INFO set CY_STATE =0,CY_CONTROL =1 where CYJ_MACHINE='" +
				Location + "'";

			if (!_connKYC->SQLExcute(Sql1)) {
				_connKYC->Disconn();
				_LastErrS = "�������Ʊ�����������־λʱsqlִ��ʧ��:" + _connKYC->GetLastErr();
				return false;
			}
			_connKYC->CloseDataset();
			return true;
		}
		else {
			_connKYC->Disconn();
			_LastErrS = "�޲�����¼�����±�־λʧ��:" + _connKYC->GetLastErr();
			return false;
		}

	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "�����м��������־ʧ��:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
}

// ---��������������״̬��Ϣ
bool TServJiek::GetCollectorInfo(SamperCollectionData &CollectInfo) {
	String _Sql = "select * from BARREL_STATUS_INFO where STATUS= 1";

	TClientDataSet *CDS = new TClientDataSet(NULL);
	try {
		OleVariant OleVar;
		OleVar = _connKYC->GetDatasetEX(_Sql);
		CDS->Data = OleVar;
		if (CDS->RecordCount > 0) {

			for (int i = 0; i < CDS->RecordCount; i++) {

				strcpy(CollectInfo.SimplingLocation,
					AnsiString(_sysInfo.Location).c_str());
				CollectInfo.BucketNo =
					CDS->FieldByName("BARRELCODE")->AsInteger;
				strcpy(CollectInfo.SamplingCode,
					CDS->FieldByName("CY_CODE")->AsAnsiString.c_str());
				CollectInfo.GrandBucketPoint =
					CDS->FieldByName("DOWNCONT")->AsInteger;
				CollectInfo.BucketWeight =
					CDS->FieldByName("WEIGHT")->AsInteger;
				strcpy(CollectInfo.SimpLingUpdateTime,
					CDS->FieldByName("LAST_UPDATE_TIME")->AsAnsiString.c_str());

				if (!SubmitJiYTInfo_pt(CollectInfo)) {
					_LastErrS = "�ϴ���������Ϣʧ��:" + GetLastErrS();
					_File.WriteLog_ErrS(_LastErrS);
					CDS->Next();
				}
				_Sql = "update BARREL_STATUS_INFO set STATUS= 2 where                          \
								STATUS= 1";
				_connKYC->SQLExcute(_Sql);

				CDS->Next();
			}
			CDS->Free();
			return true;
		}
		else {
			CDS->Free();
			_LastErrS = "�޸��¼�¼:" + _connKYC->GetLastErr();
			return false;
		}

	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "�鿴��������Ϣʱ���ݿ��ʧ��:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		CDS->Free();
		return false;
	}
}

// ---������������ʵʱ�󳵡�С��λ��
bool TServJiek::GetRealTimeCaiYInfo(String Machine) {
	String _Sql = "select * from CYJ_CAR_POINT where CYJ_MACHINE = " +
		Machine + "";

	TClientDataSet *CDS = new TClientDataSet(NULL);
	try {
		OleVariant OleVar;
		OleVar = _connKYC->GetDatasetEX(_Sql);
		CDS->Data = OleVar;
		if (CDS->RecordCount > 0) {

			String CYJMachine = "", CarNumber = "", Gkdw = "";
			int Dczb = 0, Xczb = 0, Sdzb = 0, Ycds = 0;

			CYJMachine = CDS->FieldByName("CYJ_MACHINE")->AsString.c_str();
			CarNumber = CDS->FieldByName("CAR_NUMBER")->AsString.c_str();
			Dczb = CDS->FieldByName("CYJ_CAR_BIG")->AsInteger;
			Xczb = CDS->FieldByName("CYJ_CAR_SMALL")->AsInteger;
			// Sdzb=CDS->FieldByName("ErrorCode")->AsInteger;
			// Gkdw=CDS->FieldByName("ErrorCode")->AsString.c_str();
			// Ycds=CDS->FieldByName("ErrorCode")->AsInteger;

			if (!RealTimeCaiYZTInfo_pt(CYJMachine, CarNumber, Dczb, Xczb)) {
				_LastErrS = "�ϴ�ʵʱ��������ʧ��:" + GetLastErrS();
				_File.WriteLog_ErrS(_LastErrS);
			}

			CDS->Free();
			return true;
		}
		else {
			CDS->Free();
			_LastErrS = "�޸��¼�¼:" + _connKYC->GetLastErr();
			return false;
		}

	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "�鿴ʵʱ������Ϣʱ���ݿ��ʧ��:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		CDS->Free();
		return false;
	}
}

//---д���������ͻ�����뵽�������м��
bool TServJiek::WritePICItoHuay(TCaiyInfo &CaiyInfo){
	 String _Sql = "select * from ZyHyCode where MakeSampleNo = "+CaiyInfo.ZhiyBH+
					"and TestSampleNo= "+CaiyInfo.HuayBH+"";
	 try {
		 _connHYS->GetDataSet(_Sql);
		 if(_connHYS->DSInfo.nRecordCount == 0){
			_connHYS->AppendDS();
			_connHYS->FieldAsName(L"MakeSampleNo", CaiyInfo.ZhiyBH);
			_connHYS->FieldAsName(L"TestSampleNo", CaiyInfo.HuayBH);
			_connHYS->PostDS();
			_connHYS->CloseDataset();

			return true;
		 }
	 } catch (Exception &e) {
		_connHYS->CloseDataset();
		_LastErrS = "����ֵʱ���������ݿ��ʧ��:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	 }
}

// ---������������׼��״̬��Ϣ
bool TServJiek::GetCaiYZBInfo(TCYZBInfo &CYZBInfo) {
	String _Sql = "select * from CYJ1_READY_STATUS where CHANGE_STATUS = 1";

	TClientDataSet *CDS = new TClientDataSet(NULL);
	try {
		OleVariant OleVar;
		OleVar = _connKYC->GetDatasetEX(_Sql);
		CDS->Data = OleVar;
		if (CDS->RecordCount > 0) {

			CYZBInfo.BARREL_READY = CDS->FieldByName("BARREL_READY")->AsString.c_str();
			CYZBInfo.CAR_READY = CDS->FieldByName("CAR_READY")->AsString.c_str();
			CYZBInfo.JLC_DOOR_READY = CDS->FieldByName("JLC_DOOR_READY")->AsInteger;
			CYZBInfo.CSB_BIGCAR_READY = CDS->FieldByName("CSB_BIGCAR_READY")->AsInteger;
			CYZBInfo.CYJ_BACK_READY = CDS->FieldByName("CYJ_BACK_READY")->AsInteger;
			CYZBInfo.FYT_DOOR_READY = CDS->FieldByName("FYR_DOOR_READY")->AsString.c_str();
			CYZBInfo.DATA_SEND_READY = CDS->FieldByName("DATA_SEND_READY")->AsInteger;


			if (!SubmitCYZBInfo_pt(CYZBInfo)) {
				_LastErrS = "�ϴ�����׼��״̬ʧ��:" + GetLastErrS();
				_File.WriteLog_ErrS(_LastErrS);
			}

			CDS->Free();

			_Sql = "update CYJ1_READY_STATUS set CHANGE_STATUS = 0 where CHANGE_STATUS =1";
			_connKYC->SQLExcute(_Sql);
			_connKYC->Disconn();
			return true;
		}
		else {
			CDS->Free();
			_LastErrS = "�޸��¼�¼:" + _connKYC->GetLastErr();
			return false;
		}

	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "�鿴ʵʱ������Ϣʱ���ݿ��ʧ��:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		CDS->Free();
		return false;
	}
}

// ---ƽ̨�ṩ�Ľӿڷ���

// ---ȡ������ʱ��
bool TServJiek::GetServerSystemTime_pt(String &sysTime) {
	// ---��δ����,�ñ���ʱ�����
	sysTime = Now().FormatString(L"yyyy-MM-dd HH:mm:ss");
	return true;
}

// ---��ȡ��������
bool TServJiek::GetGeneralData_pt(UTF8String TableName, TClientDataSet *CDS) {
	// ---test
	/* Json::Value val_Obj;
	 if(!JsonStr_correct_pt(TableName,val_Obj )){
	 return false;
	 }
	 if(!JsonToDataSet_pt(val_Obj, L"",CDS)){
	 return false;
	 }
	 CDS->Post();
	 return true; */

	try {
		Json::Value root;

		root["LoginName"] = "yanp";
		root["PassWord"] = "1";
		root["operType"] = "27";
		root["JsonData"] = TableName.c_str();

		Json::FastWriter writer;
		UTF8String strJson_In = UTF8String(writer.write(root).c_str());

		UTF8String strJson_Out = _ptSoap->Nmlx_DataService(strJson_In);

		Json::Value val_Out;
		if (!_jRet.JsonStrToJson_pt(strJson_Out, val_Out)) {
			_LastErrS = _jRet.GetLastErrS();
			_File.WriteLog_ErrS(L"JsonStrToJson_pt:" + _LastErrS);
			return false;
		}

		if (!JsonToDataSet_pt(val_Out, L"", CDS)) {
			_File.WriteLog_ErrS(L"JsonToDataSet_pt:" + _LastErrS);
			return false;
		}
		CDS->Post();
		return true;

	}
	catch (Exception &e) {
		_LastErrS = L"ƽ̨�ӿ�Nmlx_DataService�쳣:" + e.Message;
		_File.WriteLog_ErrS(L"GetGeneralData_pt:" + _LastErrS);
		return false;
	}
	catch (...) {
		_LastErrS = "ƽ̨�ӿ�Nmlx_DataService�쳣:δ֪����";
		_File.WriteLog_ErrS(L"GetGeneralData_pt:" + _LastErrS);
		return false;
	}
}

/*************************************************
 Function:		Login_pt
 Description:	��¼��֤
 Input:          JsonReq:Json��ʽ��������
 Output:         ��
 Return:         ����JsonRet�ַ���
 Calls:          �������
 Table Accessed: MACHIN_STATUS_INFO
 Table Updated:  ��
 Others:
 Author: 		���ı�
 Date:			2016-03-24
 ************************************************ */
bool TServJiek::Login_pt(String User, String Pass) {
	UTF8String JsonReq;
	UTF8String JsonRet;
	// ---Ȩ���ж�
	Json::Value JsonData_QX;
	JsonData_QX["UserName"] = UTF8Encode(User).c_str();
	JsonData_QX["RoleName"] = BankQX.c_str();
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"31", JsonData_QX);
	JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	if (_jRet.RsultAnalysis(JsonRet))
		return true;
	else {
		_LastErrS = L"��Ȩ��:" + _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"Login_pt:" + _LastErrS);
		return false;
	}

	Json::Value JsonData_Lg;
	JsonData_Lg["USERNAME"] = UTF8Encode(User).c_str();
	JsonData_Lg["USERPASSWORD"] = UTF8Encode(Pass).c_str();
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"3", JsonData_Lg);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"Login_pt,�����쳣:" + e.Message;
		return false;
	}

	if (_jRet.RsultAnalysis(JsonRet))
		return true;
	else {
		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis:" + _LastErrS);
		return false;
	}
}

/*
 �������̴����ȡ����ID
 */
bool TServJiek::GetFlowID(String FlowCode, int &FLowID) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["DICTIONARYDETAILCODE"] = UTF8Encode(FlowCode).c_str();
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"6", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"GetFlowID,�����쳣:" + e.Message;
		return false;
	}

	Json::Value jv;
	if (!_jRet.JsonStrToJson_pt(JsonRet, jv)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"JsonStrToJson_pt@GetFlowID:" + _LastErrS);
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@GetFlowID:" + _LastErrS);
		return false;
	}

	JsonData = jv["JsonData"];
	if (JsonData.isNull()) {

		_LastErrS = L"ȡ����IDʧ��:JsonDataΪ��";
		_File.WriteLog_ErrS(L"GetFlowID:" + _LastErrS);
		return false;
	}
	if (!JsonData.isArray()) {

		_LastErrS = L"ȡ����IDʧ��:JsonData��������";
		_File.WriteLog_ErrS(L"GetFlowID:" + _LastErrS);
		return false;
	}
	int i = 0;
	if (!JsonData[i]["SORTCODE"].isInt()) {

		_LastErrS = L"ȡ����IDʧ��:SORTCODE��������";
		_File.WriteLog_ErrS(L"GetFlowID:" + _LastErrS);
		return false;
	}
	FLowID = JsonData[i]["SORTCODE"].asInt();
	return true;
}

/*

 ���ݳ��Ż�ȡ����������Ϣ��������������Ӧ����Ϣ����������
 */

bool TServJiek::GetCarCYInfo(String Numplate, TCaiyInfo &CaiyInfo) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"37", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"GetCarCYInfo,�����쳣:" + e.Message;
		return false;
	}

	Json::Value jv;
	if (!_jRet.JsonStrToJson_pt(JsonRet, jv)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"JsonStrToJson_pt@GetCarCYInfo:" + _LastErrS);
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@GetCarCYInfo:" + _LastErrS);
		return false;
	}

	JsonData = jv["JsonData"];
	if (JsonData.isNull()) {

		_LastErrS = L"ȡ������Ϣʧ��:JsonDataΪ��";
		_File.WriteLog_ErrS(L"GetCarCYInfo:" + _LastErrS);
		return false;
	}
	/*
	if (!JsonData.isArray()) {

		_LastErrS = L"ȡ����IDʧ��:JsonData����Json����";
		_File.WriteLog_ErrS(L"GetCarFlowID:" + _LastErrS);
		return false;
	}
	int i = 0;
	*/
	if (!JsonData["Dians"].isString()) {

		_LastErrS = L"ȡ����ʧ��:Dians��������";
		_File.WriteLog_ErrS(L"GetCarCYInfo:" + _LastErrS);
		return false;
	}
	CaiyInfo.CaiyDS = JsonData["Dians"].asCString();
	Gkdw = CaiyInfo.GongYS = JsonData["GysMingc"].asCString();
	CaiyInfo.CaiyBH = JsonData["Caiybh"].asCString();
	CaiyInfo.CaiyLX = JsonData["CaiyLx"].asCString();
	CaiyInfo.ZhiyBH = JsonData["Zhiybh"].asCString();
	CaiyInfo.HuayBH = JsonData["Huaybh"].asCString();
	return true;
}

/*
 ���ݳ��Ż�ȡ��ǰ��������ID
 */
bool TServJiek::GetCarFlowID(String Numplate, int &FLowID,
	UTF8String &FlowDescription) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"32", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"GetCarFlowID,�����쳣:" + e.Message;
		return false;
	}

	Json::Value jv;
	if (!_jRet.JsonStrToJson_pt(JsonRet, jv)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"JsonStrToJson_pt@GetCarFlowID:" + _LastErrS);
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@GetCarFlowID:" + _LastErrS);
		return false;
	}

	JsonData = jv["JsonData"];
	if (JsonData.isNull()) {

		_LastErrS = L"ȡ����IDʧ��:JsonDataΪ��";
		_File.WriteLog_ErrS(L"GetCarFlowID:" + _LastErrS);
		return false;
	}
	if (!JsonData.isArray()) {

		_LastErrS = L"ȡ����IDʧ��:JsonData����Json����";
		_File.WriteLog_ErrS(L"GetCarFlowID:" + _LastErrS);
		return false;
	}
	int i = 0;
	if (!JsonData[i]["SORTCODE"].isInt()) {

		_LastErrS = L"ȡ����IDʧ��:SORTCODE��������";
		_File.WriteLog_ErrS(L"GetCarFlowID:" + _LastErrS);
		return false;
	}
	FLowID = JsonData[i]["SORTCODE"].asInt();
	FlowDescription = JsonData[i]["YEWMINGC"].asCString();
	/* Station.Current_Location=JsonData[i]["Current_Yewdd_1"].asCString();
	 Station.Current_Station=JsonData[i]["Current_Yewdd_2"].asCString();
	 Station.Next_Location=JsonData[i]["YEWDD"].asCString();
	 Station.Next_Station=JsonData[i]["YEWDD_2"].asCString();
	 */
	return true;
}

// ---��ȡ������Ϣ
bool TServJiek::GetCarInfo_pt(String cardID, TCarInfo &CarInfo) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["CARDID"] = UTF8Encode(cardID).c_str();
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"28", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"Login_pt,�����쳣:" + e.Message;
		return false;
	}

	Json::Value jv;
	if (!_jRet.JsonStrToJson_pt(JsonRet, jv)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"JsonStrToJson_pt@GetCarInfo_pt:" + _LastErrS);
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@GetCarInfo_pt:" + _LastErrS);
		return false;
	}

	TClientDataSet *CDS = new TClientDataSet(NULL);
	if (!JsonToDataSet_pt(jv, L"", CDS)) {

		_File.WriteLog_ErrS(L"JsonToDataSet_pt@GetCarInfo_pt:" + _LastErrS);
		return false;
	}
	CDS->Post();

	try {
		CDS->First();
		CarInfo.CardID = cardID;
		CarInfo.NumPlate = CDS->FieldByName("CHEPH")->AsString.Trim();
		CarInfo.ICCardID = CDS->FieldByName("ICCARDID")->AsString.Trim();
		CarInfo.Legth = CDS->FieldByName("CHEXCD")->AsFloat;
		CarInfo.Width = CDS->FieldByName("CHEXKD")->AsFloat;
		CarInfo.Dipg = CDS->FieldByName("DIPGD")->AsFloat;
		CarInfo.Lajs = CDS->FieldByName("LAJSL")->AsInteger;
		CarInfo.GuaStart = CDS->FieldByName("G_START")->AsInteger;
		CarInfo.GuaEnd = CDS->FieldByName("G_END")->AsInteger;

		for (int i = 0; i < CarInfo.Lajs && i < 12; i++) {

			UTF8String key = L"LAJWZ" + IntToStr(i + 1);
			CarInfo.Ljwz[i] = CDS->FieldByName(L"LAJWZ" + IntToStr(i + 1))
				->AsFloat;
		}

		CarInfo.carType_ID = CDS->FieldByName("CARTYPE")->AsString.Trim();
		CarInfo.MaxM = CDS->FieldByName("MAXMZ")->AsFloat;
		CarInfo.MinP = CDS->FieldByName("MINPZ")->AsFloat;
		CarInfo.BiaoZ = CDS->FieldByName("BIAOZ")->AsFloat;

		CDS->FieldByName("IS_SUOC")->AsString.Trim() == "Y" ? CarInfo.IsLock =
			1 : CarInfo.IsLock = 0;
		CDS->FieldByName("LEIB")->AsString.Trim() == "ranm" ? CarInfo.Leib =
			0 : CarInfo.Leib = 1;
		CarInfo.YewZT = CDS->FieldByName("YEWZT")->AsInteger;

		CDS->Close();
		CDS->Free();
		return true;
	}
	catch (Exception &e) {

		CDS->Close();
		CDS->Free();
		_LastErrS = L"�쳣:" + e.Message;
		_File.WriteLog_ErrS(L"GetCarInfo_pt:" + _LastErrS);
		return false;
	}
	catch (...) {

		CDS->Close();
		CDS->Free();
		_LastErrS = L"�쳣:δ֪����";
		_File.WriteLog_ErrS(L"GetCarInfo_pt:" + _LastErrS);
		return false;
	}
}

// ---��ȡȼú���볧�ƻ�:�ƻ��ڵ�ú��Ӧ�̹�ϵ��
bool TServJiek::GetRanmJih_pt(TClientDataSet *ds) {
	UTF8String JsonReq;
	UTF8String JsonRet;
	String strServerDate;
	if (!GetServerSystemTime_pt(strServerDate))
		return false;

	strServerDate = strServerDate.SubString(1, 10); // --- yyyy-mm-dd

	Json::Value JsonData;
	JsonData["RIQ"] = UTF8Encode(strServerDate).c_str();
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"34", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"GetRanmJih_pt,�����쳣:" + e.Message;
		return false;
	}

	Json::Value jv;
	if (!_jRet.JsonStrToJson_pt(JsonRet, jv)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"JsonStrToJson_pt@GetRanmJih_pt:" + _LastErrS);
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@GetRanmJih_pt:" + _LastErrS);
		return false;
	}

	ds->Close();
	ds->FieldDefs->Clear();
	if (!JsonToDataSet_pt(jv, L"", ds)) {

		_File.WriteLog_ErrS(L"JsonToDataSet_pt@GetRanmJih_pt:" + _LastErrS);
		return false;
	}
	ds->Post();
	return true;
}

/*
 �����ź�IC���Ž��а󶨻���
 Action 0:�� 1:���
 */
bool TServJiek::BingdingICCard_pt(String Numplate, String ICCardID, int Action)
{
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();
	JsonData["ICCARD"] = UTF8Encode(ICCardID).c_str();
	0 == Action ? JsonData["TYPE"] = "Bingding" : JsonData["TYPE"] =
		"UnBingding";
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"38", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"BingdingICCard_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@BingdingICCard_pt:" + _LastErrS);
		return false;
	}

	return true;
}

/*
 �ύ��Ʊ��Ϣ
 16.4.22 Wuwenbin���� fKuangfl����
 */
bool TServJiek::SubmitYanp_pt(String Numplate, double fKuangfl, String Meik,
	String Gongys, String Pinz) {
	String sysTime;
	UTF8String Yunsfs = L"��·";
	UTF8String Yunsdw = UTF8Encode(Meik);
	UTF8String MeiC = L"��";
	UTF8String MeiD = L"��";

	if (!GetServerSystemTime_pt(sysTime)) {

		return false;
	}
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["GYSMINGC"] = UTF8Encode(Gongys).c_str();
	JsonData["MKMINGC"] = UTF8Encode(Meik).c_str();
	JsonData["WLMINGC"] = UTF8Encode(Pinz).c_str();
	JsonData["YSFSMINGC"] = Yunsfs.c_str();
	JsonData["YSDWMINGC"] = Yunsdw.c_str();
	JsonData["DAOHRQ"] = UTF8Encode(sysTime.SubString(1, 10)).c_str();
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();
	JsonData["YEWDD"] = UTF8Encode(_sysInfo.Location).c_str();
	JsonData["MCMINGC"] = MeiC.c_str();
	JsonData["MDMINGC"] = MeiD.c_str();
	JsonData["KUANGFM"] = 1000;
	JsonData["KUANGFP"] = 1000;
	JsonData["KUANGFL"] = fKuangfl;

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"4", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SubmitYanp_pt,�����쳣:" + e.Message;
		return false;
	}
	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitYanp_pt:" + _LastErrS);
		return false;
	}

	return true;
}

/*
 �ύ�������ݵ�ƽ̨
 JilTag 0������ 1����Ƥ
 */
bool TServJiek::SubmitJilInfo_pt(String Numplate, double fZhongl, int JilTag) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"�Զ�����";

	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();
	0 == JilTag ? JsonData["MAOZ"] = fZhongl : JsonData["PIZ"] = fZhongl;
	JsonData["CAOZRY"] = CaozRY.c_str();
	JsonData["YEWDD"] = UTF8Encode(_sysInfo.Location).c_str();

	if (0 == JilTag)
		JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"7", JsonData);
	else
		JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"8", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SubmitJilInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitJilInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---�ύ������ϸ��¼��Ϣ��ƽ̨

bool TServJiek::SubmitCaiYRecord_pt(CarSamplingData param) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"������¼�ӿ�";

	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(String(param.VehicleNo)).c_str();
	JsonData["CAIYTH"] = UTF8Encode(String(param.SetBucketNo)).c_str();
	JsonData["SHOUYTH"] = UTF8Encode(String(param.SetBucketNo)).c_str();
	JsonData["CAIYRY"] = UTF8Encode(String(param.SamplingPerson)).c_str();
	JsonData["CAIYJH"] = UTF8Encode(String(param.SimplingLocation)).c_str();
	JsonData["CAIYDS"] = param.SimplingPoint;
	JsonData["CAIYSJ"] = UTF8Encode(String(param.SimplingEndTime)).c_str();
	JsonData["KAISSJ"] = UTF8Encode(String(param.SimplingStartTime)).c_str();
	JsonData["JIESSJ"] = UTF8Encode(String(param.SimplingEndTime)).c_str();
	JsonData["CAIYBH"] = UTF8Encode(String(param.SupplierCode)).c_str();
	JsonData["CAIYZB"] = UTF8Encode(String(param.SimplingTarget)).c_str();
	JsonData["ZHONGL"] = param.SetBucketWeight;
	JsonData["CAIYLX"] = UTF8Encode(String(param.SimplingType)).c_str();
	JsonData["ZHIYLX"] = UTF8Encode(String(param.BatchInfoId)).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"35", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SubmitCaiYRecord_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitCaiYRecord_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ----�ύ����������̷���ƽ̨
bool TServJiek::SubmitCaiYInfo_pt(UTF8String Car) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"�������ӿڷ���";

	Json::Value JsonData;
	JsonData["CHEPH"] = Car.c_str();
	JsonData["CAOZRY"] = CaozRY.c_str();
	JsonData["YEWDD"] = UTF8Encode(String(_sysInfo.Location)).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"33", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SubmitCaiYInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitCaiYInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}
//----�ύ������׼��״̬��Ϣ������
bool TServJiek::SubmitCYZBInfo_pt(TCYZBInfo &CYZBInfo) {
	UTF8String JsonReq;
	UTF8String JsonRet;


	Json::Value JsonData;
	JsonData["BARREL_READY"] = UTF8Encode(CYZBInfo.BARREL_READY).c_str();
	JsonData["CAR_READY"] = UTF8Encode(CYZBInfo.CAR_READY).c_str();
	JsonData["JLC_DOOR_READY"] = CYZBInfo.JLC_DOOR_READY;
	JsonData["CSB_BIGCAR_READY"] = CYZBInfo.CSB_BIGCAR_READY;
	JsonData["CYJ_BACK_READY"] = CYZBInfo.CYJ_BACK_READY;
	JsonData["FYT_DOOR_READY"] = UTF8Encode(CYZBInfo.FYT_DOOR_READY).c_str();
	JsonData["DATA_SEND_READY"] = CYZBInfo.DATA_SEND_READY;

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"49", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SubmitCYZBInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitCYZBInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ----�ύ������״̬��Ϣ��ƽ̨
bool TServJiek::SubmitJiYTInfo_pt(SamperCollectionData param) {

	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["JIYTH"] = UTF8Encode(String(param.BucketNo)).c_str();
	JsonData["CAIYBH"] = UTF8Encode(String(param.SamplingCode)).c_str();
	JsonData["JIYGZYS"] = UTF8Encode(String(param.GrandBucketPoint)).c_str();
	JsonData["JIYGZL"] = UTF8Encode(String(param.BucketWeight)).c_str();
	JsonData["CAIYJH"] = UTF8Encode(String(param.SimplingLocation)).c_str();
	JsonData["UPDATETIME"] = UTF8Encode(String(param.SimpLingUpdateTime))
		.c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"36", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SubmitJiYTInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitJiYTInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---����ͣ����
// ---Action 0:�� 1:��
bool TServJiek::EnterExitParking_pt(String Numplate, int Action) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"�����볡";

	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();
	JsonData["CAOZRY"] = CaozRY.c_str();

	0 == Action ? JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"18",
		JsonData) : JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK,
		L"19", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"EnterExitParking_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@EnterExitParking_pt:" + _LastErrS);
		return false;
	}

	Json::Value jv;
	if (!_jRet.JsonStrToJson_pt(JsonRet, jv)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"JsonStrToJson_pt@EnterExitParking_pt:" +
			_LastErrS);
		return false;
	}

	JsonData = jv["JsonData"];
	if (JsonData.isNull()) {

		_LastErrS = L"����ͣ����ʧ��:JsonDataΪ��";
		_File.WriteLog_ErrS(L"EnterExitParking_pt:" + _LastErrS);
		return false;
	}
	if (!JsonData.isObject()) {

		_LastErrS = L"����ͣ����ʧ��:JsonData����Json����";
		_File.WriteLog_ErrS(L"EnterExitParking_pt:" + _LastErrS);
		return false;
	}

	if (JsonData["Address"].isNull()) {

		_LastErrS = L"����ͣ����ʧ��:AddressΪNULL";
		_File.WriteLog_ErrS(L"EnterExitParking_pt:" + _LastErrS);
		return false;
	}

	// NextStation =JsonData["Address"].asCString();

	return true;
}

// ---����ú��
// ---Action 0:�� 1:��
bool TServJiek::EnterExitMC_pt(String Numplate, int Action, UTF8String Location)
{
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"�����볡";

	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();
	// JsonData["YEWDD"] = UTF8Encode(_sysInfo.Location).c_str();
	JsonData["YEWDD"] = Location.c_str();
	JsonData["CAOZRY"] = CaozRY.c_str();

	0 == Action ? JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"9",
		JsonData) : JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK,
		L"10", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"EnterExitMC_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@EnterExitMC_pt:" + _LastErrS);
		return false;
	}

	return true;
}

bool TServJiek::GetNextStation(String Numplate, TStation &Station) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"32", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"GetNextStation,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@GetNextStation:" + _LastErrS);
		return false;
	}

	Json::Value jv;
	if (!_jRet.JsonStrToJson_pt(JsonRet, jv)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"JsonStrToJson_pt@GetNextStation:" + _LastErrS);
		return false;
	}

	JsonData = jv["JsonData"];
	if (JsonData.isNull()) {

		_LastErrS = L"��ȡ��һ����ʧ��:JsonDataΪ��";
		_File.WriteLog_ErrS(L"GetNextStation:" + _LastErrS);
		return false;
	}
	if (!JsonData.isArray()) {

		_LastErrS = L"��ȡ��һ����ʧ��:JsonData����Json����";
		_File.WriteLog_ErrS(L"GetNextStation:" + _LastErrS);
		return false;
	}
	int i = 0;
	Station.Current_Location = JsonData[i]["Current_Yewdd_1"].asCString();
	Station.Current_Station = JsonData[i]["Current_Yewdd_2"].asCString();
	Station.Next_Location = JsonData[i]["YEWDD"].asCString();
	Station.Next_Station = JsonData[i]["YEWDD_2"].asCString();
	return true;
}

// ---�ϴ�������Ϣ
bool TServJiek::SaveZhiybjData_pt(String MachineCode, String strDec,
	String strCode) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	// Json::Value JsonData_New;
	JsonData["MACHINECODE"] = UTF8Encode(MachineCode).c_str();
	JsonData["ERRORCODE"] = UTF8Encode(strCode).c_str();
	JsonData["ERRORDEC"] = UTF8Encode(strDec).c_str();
	JsonData["ERRORTIME"] =
		UTF8Encode(Now().FormatString("yyyy-MM-dd HH:mm:ss")).c_str();

	// JsonData_New.append(JsonData);
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"41", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);

	}
	catch (Exception &e) {
		_LastErrS = L"SaveZhiybjData_pt,�����쳣:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"SaveZhiybjData_pt:" + _LastErrS);
		return false;
	}

	return true;
}

/*
 ��Ʊ����
 */
bool TServJiek::ChupData_pt(String Numplate, double kous, double koug,
	double kouz, double kouqt, TBangdInfo &BangdInfo) {
	UTF8String JsonReq;
	UTF8String JsonRet;
	UTF8String CaozRY = L"��������";
	Json::Value JsonData;
	JsonData["CHEPH"] = UTF8Encode(Numplate).c_str();
	JsonData["KOUS"] = kous;
	JsonData["KOUG"] = koug;
	JsonData["KOUZ"] = kouz;
	JsonData["KOUQT"] = kouqt;
	JsonData["CAOZRY"] = CaozRY.c_str();
	JsonData["YEWDD"] = UTF8Encode(_sysInfo.Location).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"11", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);

	}
	catch (Exception &e) {
		_LastErrS = L"SaveZhiybjData_pt,�����쳣:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"ChupData:" + _LastErrS);
		return false;
	}

	Json::Value jv;
	if (!_jRet.JsonStrToJson_pt(JsonRet, jv)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"JsonStrToJson_pt@GetNextStation:" + _LastErrS);
		return false;
	}

	JsonData = jv["JsonData"];
	if (JsonData.isNull()) {

		_LastErrS = L"��Ʊʧ��:JsonDataΪ��";
		_File.WriteLog_ErrS(L"ChupData:" + _LastErrS);
		return false;
	}
	if (!JsonData.isArray()) {

		_LastErrS = L"��Ʊʧ��:JsonData����Json����";
		_File.WriteLog_ErrS(L"ChupData:" + _LastErrS);
		return false;
	}
	int i = 0;
	BangdInfo.Numplate = Numplate;
	BangdInfo.Gongysmc = Utf8ToAnsi(JsonData[i]["GYSMINGC"].asCString());
	BangdInfo.Meikmc = Utf8ToAnsi(JsonData[i]["MKMINGC"].asCString());
	BangdInfo.Wulmc = Utf8ToAnsi(JsonData[i]["WULMC"].asCString());
	BangdInfo.Maoz = JsonData[i]["MAOZ"].asDouble();
	BangdInfo.Piz = JsonData[i]["PIZ"].asDouble();
	BangdInfo.Jingz = JsonData[i]["JINGZ"].asDouble();
	BangdInfo.Kous = JsonData[i]["KOUS"].asDouble();
	BangdInfo.Koug = JsonData[i]["KOUG"].asDouble();
	BangdInfo.Kouz = JsonData[i]["KOUZ"].asDouble();
	BangdInfo.Kouqt = JsonData[i]["KOUQT"].asDouble();
	BangdInfo.Zhongcsj = Utf8ToAnsi(JsonData[i]["ZHONGCSJ"].asCString());
	BangdInfo.Qingcsj = Utf8ToAnsi(JsonData[i]["QINGCSJ"].asCString());
	BangdInfo.JilsjID = Utf8ToAnsi(JsonData[i]["RL_JILSJB_FK"].asCString());
	return true;
}

// ---�ϴ�ж�������Ϣ nIsXL:0δ��� 1���
bool TServJiek::UploadXlwcxx_pt(String Cybm, int nIsXL) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["CAIYBH"] = UTF8Encode(Cybm).c_str();
	JsonData["ISXL"] = nIsXL;

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"42", JsonData);
	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);

	}
	catch (Exception &e) {
		_LastErrS = L"UploadXlwcxx_pt,�����쳣:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"UploadXlwcxx_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---�ύ����������״̬��Ϣ
bool TServJiek::SaveCaiYZTInfo_pt(String MachineCode, String strTime,
	String strDec) {

	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["MACHINECODE"] = UTF8Encode(_sysInfo.Location).c_str();
	JsonData["ERRORDEC"] = UTF8Encode(strDec).c_str();
	JsonData["ERRORTIME"] = UTF8Encode(strTime).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"40", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SaveCaiYZTInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCaiYZTInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---�ύ����������״̬��Ϣ
bool TServJiek::SaveZhiYZTInfo_pt(String MachineCode, String strTime,
	String strDec, String strCode) {

	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["MACHINECODE"] = UTF8Encode(_sysInfo.Location).c_str();
	JsonData["ERRORDEC"] = UTF8Encode(strDec).c_str();
	JsonData["ERRORTIME"] = UTF8Encode(strTime).c_str();
	JsonData["ERRORCODE"] = UTF8Encode(strCode).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"41", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SaveZhiYZTInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveZhiYZTInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ----�ύ�������������Ϣ
bool TServJiek::SaveHUAYInfo_pt(THYSInfo &HYSInfo) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	Json::Value json_Row;
	 JsonData["HUAYLSH"] =UTF8Encode(HYSInfo.S_NO).c_str();
	 JsonData["HUAYDH"] =UTF8Encode(HYSInfo.S_NO).c_str();
	 JsonData["HUAYBM"] =UTF8Encode(HYSInfo.S_NO).c_str();
	 JsonData["ZHILFLB"] =UTF8Encode(HYSInfo.S_NO).c_str();
	 JsonData["HUAYDJH"] =UTF8Encode(String(HYSInfo.isupload)).c_str();
	 JsonData["HUAYSJ"] =UTF8Encode(HYSInfo.TestTime).c_str();
	 json_Row["RANLZB"] =UTF8Encode(String(HYSInfo.Mt)).c_str();
	 json_Row["HUAYSZ"] =UTF8Encode(String(HYSInfo.Mt)).c_str();
	 json_Row["HUAYRY"] =UTF8Encode(String(HYSInfo.JZTZ)).c_str();
	 json_Row["HUAYSJ"] =UTF8Encode(HYSInfo.TestTime).c_str();


	JsonData.append(json_Row);
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"15", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SaveHUAYInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveHUAYInfo_pt:" + _LastErrS);
		return false;
	}

	return true;

}

// ---���Ͳ�����������Ϣ��ƽ̨��֪ͨ�豸��ؽ�����³�����Ϣ
bool TServJiek::UpdateCaiYZTInfo_pt(String Machine, String Car) {

	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["CAIYJH"] = UTF8Encode(_sysInfo.Location).c_str();
	JsonData["CHEPH"] = UTF8Encode(Car).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"44", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"UpdateCaiYZTInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@UpdateCaiYZTInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---���Ͳ�����ʵʱ�������ݴ󳵡�С��λ��
bool TServJiek::RealTimeCaiYZTInfo_pt(String Machine, String Car, int Dczb,
	int Xczb) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["CAIYJH"] = UTF8Encode(_sysInfo.Location).c_str();
	JsonData["CHEPH"] = UTF8Encode(Car).c_str();
	JsonData["GKDW"] = UTF8Encode(Gkdw).c_str();
	JsonData["DCZB"] = Dczb;
	JsonData["XCZB"] = Xczb;
	JsonData["SDZB"] = 0;
	JsonData["YCDS"] = 2;

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"43", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"RealTimeCaiYZTInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@RealTimeCaiYZTInfo_pt:" +
			_LastErrS);
		return false;
	}

	return true;
}
//----�ύȫˮ������
bool TServJiek::SaveQSYInfo_pt(TQSYInfo &QSYInfo){
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	Json::Value json_Row;
	json_Row["SAMPLENO"] =UTF8Encode(QSYInfo.SAMPLENO).c_str();
	json_Row["POSITIONNO"] =QSYInfo.POSITIONNO;
	json_Row["SAMPLENAME"] =UTF8Encode(QSYInfo.SAMPLENAME).c_str();
	json_Row["TRAYWEIGHT"] =QSYInfo.TRAYWEIGHT;
	json_Row["SAMPLEWEIGHT"] =UTF8Encode(QSYInfo.SAMPLEWEIGHT).c_str();
	json_Row["LEFTWEIGHT"] =UTF8Encode(QSYInfo.LEFTWEIGHT).c_str();
	json_Row["MOISTURE"] =QSYInfo.MOISTURE;
	json_Row["LOSSRATE"] =QSYInfo.LOSSRATE;
	json_Row["REVISEMOISTURE"] =QSYInfo.REVISEMOISTURE;
	json_Row["STARTINGTIME"] =UTF8Encode(QSYInfo.STARTINGTIME).c_str();
	json_Row["ENDINGTIME"] =UTF8Encode(QSYInfo.ENDINGTIME).c_str();
	json_Row["STATUS"] =UTF8Encode(QSYInfo.STATUS).c_str();
	json_Row["TEMPOFCONSTANTTEMPERATURE"] =QSYInfo.TEMPOFCONSTANTTEMPERATURE;
	json_Row["TIMEOFCONSTANTTEMPERATURE"] =QSYInfo.TIMEOFCONSTANTTEMPERATURE;
	json_Row["TEMPERATURECOMPENSATION"] =QSYInfo.TEMPERATURECOMPENSATION;
	json_Row["TIMESOFDRYING"] =QSYInfo.TIMESOFDRYING;
	json_Row["INTERVALTIME"] =QSYInfo.INTERVALTIME;
	json_Row["ANALYSISPRECISION"] =QSYInfo.ANALYSISPRECISION;
	json_Row["NUMBERINGMETHOD"] =UTF8Encode(QSYInfo.NUMBERINGMETHOD).c_str();
	json_Row["OPERATOR"] =UTF8Encode(QSYInfo.OPERATOR).c_str();
	json_Row["REMARK"] =UTF8Encode(QSYInfo.REMARK).c_str();
	json_Row["ID"] =UTF8Encode(String(QSYInfo.ID)).c_str();

	JsonData.append(json_Row);
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"39", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SaveQSYInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveQSYInfo_pt:" + _LastErrS);
		return false;
	}

	return true;

}

// ---������
UTF8String TServJiek::CunYG(UTF8String JsonReq) {
	Json::Value jv_req;
	Json::Value Ret_JsonData;
	if (_bEnterCunYG) {

		return _jRet.CreatJsonRet(1, L"����ȡ������,���Ժ�", 0, Ret_JsonData);
	}
	if (!_jRet.JsonStrToJson_pt(JsonReq, jv_req)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"CunYG:" + _LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData); ;
	}

	try {
		Json::Value JsonData;
		Json::Value JsonData_Yangp; // ---ȡ����������
		JsonData = jv_req["JsonData"];
		JsonData_Yangp = JsonData["Yangp"];

		_BianMList->Clear();
		for (unsigned int i = 0; i < JsonData_Yangp.size(); i++) {

			_BianMList->Add(UTF8String(JsonData_Yangp[i]["BianM"].asCString()) +
				UTF8String(JsonData_Yangp[i]["LeiX"].asCString()));
		}
		CunYG_OpType = JsonData["OpType"].asInt();
		CunYG_OpDest = JsonData["OpDest"].asInt();
		CunYG_UserName = UTF8String(JsonData["UserName"].asCString());
		_bEnterCunYG = true;
		return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);

	}
	catch (Exception &e) {

		_LastErrS = L"CunYG�쳣:" + e.Message + L",����:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
	catch (...) {

		_LastErrS = L"CunYG�쳣:δ֪���󡣴���:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(2, _LastErrS, 0, Ret_JsonData);
	}
}

// ---ȡ������,��Windows������߳��е���
void TServJiek::CunYG_QuY() {
	_bForceEndCunYG = false; // ---һ������true,ȡ������Ӧǿ�ƽ���
	// ---�жϴ�����״̬
	int nStatus;
	int RunOpType = -1;
	if (!CunYG_IsRun(_connCYG, RunOpType)) {

		_File.WriteLog_ErrS(L"IsRun_CunYG@CunYG_QuY:" + _LastErrS);
		if (!SaveCYGYXInfo_pt(0, Now(), L"��ѯ�������Ƿ����ڲ���ʧ��",0)){
			 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
			 _LastErrS);
		 }
		_bEnterCunYG = false;
		return;
	}

	if (RunOpType > 0) {
		_File.WriteLog(L"CunYG_QuY:���������ڽ���" + CYGOpType_StaDecode(RunOpType)
			+ L"����");
		if (!SaveCYGYXInfo_pt(0, Now(), L"���������ڽ���" + CYGOpType_StaDecode(RunOpType)
			+ L"����",0)){
			 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
			 _LastErrS);
		 }
		_bEnterCunYG = false;
		return;
	}
	if (!CunYG_IsGetSta(_connCYG)) {
		_File.WriteLog_ErrS(L"CunYG_IsGetSta@CunYG_QuY:" + _LastErrS);
		if (!SaveCYGYXInfo_pt(0, Now(), L"�ύ�������ʧ��",0)){
			 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
			 _LastErrS);
		 }
		_bEnterCunYG = false;
		return;
	}

	for (int i = 0; i < _BianMList->Count; i++) {

		if (!CunYG_Insert(_connCYG, _BianMList->Strings[i], CunYG_OpType,
			CunYG_OpDest, CunYG_UserName)) {

			_File.WriteLog_ErrS(L"CunYG_Insert@CunYG_QuY:" + _LastErrS);
			if (!SaveCYGYXInfo_pt(0, Now(), L"����"+CYGOpType_StaDecode(CunYG_OpType)+L"��Ϣʧ��",0)){
				 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
				 _LastErrS);
			 }
		}
		else {
			_File.WriteLog(L"CunYG_QuY:������ʼ����" + CYGOpType_StaDecode
				(CunYG_OpType) + L"����");
			bool bRet;
			do {
				nStatus = -1;
				// ��ȡ������״̬
				bRet = CunYGStaInfo(_connCYG, _BianMList->Strings[i],
					CunYG_OpType, CunYG_OpDest, CunYG_UserName, nStatus);
				if (_bForceEndCunYG) {

					_bEnterCunYG = false; // ---�˳�ȡ��״̬
					_File.WriteLog(L"CunYG_QuY:�ȴ����������,��ǿ����ֹ");
					return;
				}
				::Sleep(1000);
			}
			while (!bRet);
			_File.WriteLog(L"CunYG_QuY:������������,����״̬:" + CYGStaDecode(nStatus));
			// ���
			if (!SaveCYGYXInfo_pt(0, Now(), L"������������,����״̬:" + CYGStaDecode(nStatus),0)){
				 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
				 _LastErrS);
			 }
		}

	}

	_bEnterCunYG = false; // ---�˳�ж��״̬
}

// �жϴ������Ƿ�����ȡ������������
bool TServJiek::CunYG_IsRun(TLX_DB *connCYG, int &RunOpType) {
	String Sql =
		L"select * from OPDOC where SampleStatus is null and DATA_STATUS = 0 ";
	try {

		connCYG->GetDataSet(Sql);
		if (connCYG->DSInfo.nRecordCount > 0) {
			RunOpType = connCYG->FieldAsName(0, L"OpType")->AsInteger;
			connCYG->CloseDataset();
		}
		return true;
	}
	catch (...) {
		connCYG->Disconn();
		_LastErrS = connCYG->GetLastErr();
		_File.WriteLog_ErrS(L"CunYG_IsRun:" + _LastErrS);
		return false;
	}
}

// �жϴ������Ƿ����δ��ȡ��ȡ������������
bool TServJiek::CunYG_IsGetSta(TLX_DB *connCYG) {
	String Sql =
		L"select * from OPDOC where SampleStatus is not null and DATA_STATUS = 0 ";
	try {

		connCYG->GetDataSet(Sql);
		for (int i = 0; i < connCYG->DSInfo.nRecordCount; i++) {
		   int OpType = connCYG->FieldAsName(0, L"OpType")->AsInteger;
		   int SampleStatus = connCYG->FieldAsName(0, L"SampleStatus")->AsInteger;
			connCYG->EditDS();
			connCYG->FieldAsName(L"DATA_STATUS", 1);
			connCYG->PostDS();
			if (!SaveCYGYXInfo_pt(0, Now(), L"������" + CYGOpType_StaDecode(OpType)
			+ L"����״̬:" + CYGStaDecode(SampleStatus),0)){
			 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_IsGetSta:" +
			 _LastErrS);
			 }
		}
		connCYG->CloseDataset();
		return true;
	}
	catch (...) {
		connCYG->Disconn();
		_LastErrS = connCYG->GetLastErr();
		_File.WriteLog_ErrS(L"CunYG_IsGetSta:" + _LastErrS);
		return false;
	}
}

// �ύȡ����������
bool TServJiek::CunYG_Insert(TLX_DB *connCYG, String BianM, int OpType,
	int OpDest, UTF8String UserName) {
	String Sql =
		L"insert into OPDOC (SampleId,OpType,OpDest,Operator) values ('" +
		BianM + "','" + OpType + "','" + OpDest + "','" + UserName + "') ";
	try {
		if (!connCYG->SQLExcute(Sql)) {
			connCYG->Disconn();
			_LastErrS = connCYG->GetLastErr();
			_File.WriteLog_ErrS(L"CunYG_Insert:" + _LastErrS);
			return false;

		}
		return true;
	}
	catch (...) {
		connCYG->Disconn();
		_LastErrS = connCYG->GetLastErr();
		_File.WriteLog_ErrS(L"CunYG_Insert:" + _LastErrS);
		return false;
	}
}

// ��ȡ�������״̬
bool TServJiek::CunYGStaInfo(TLX_DB *connCYG, String BianM, int OpType,
	int OpDest, UTF8String UserName, int &nStatus) {
	String Sql = L"select * from OPDOC where SampleId='" + BianM +
		"' and OpType= '" + OpType +
		"' and DATA_STATUS=0 and SampleStatus is not null";
	try {

		connCYG->GetDataSet(Sql);
		if (connCYG->DSInfo.nRecordCount == 0) {
			connCYG->CloseDataset();
			return false;
		}
		if (connCYG->DSInfo.nRecordCount == 1) {
			nStatus = connCYG->FieldAsName(0, L"SampleStatus")->AsInteger;
			connCYG->EditDS();
			connCYG->FieldAsName(L"DATA_STATUS", 1);
			connCYG->PostDS();
			connCYG->CloseDataset();
			return true;
		}
		else {
			connCYG->CloseDataset();
			_LastErrS = L"Ҫ���������ݲ�Ψһ";
			_File.WriteLog_ErrS(L"CunYGStaInfo:" + _LastErrS);
			return false;
		}
	}
	catch (...) {
		connCYG->Disconn();
		_LastErrS = connCYG->GetLastErr();
		_File.WriteLog_ErrS(L"CunYGStaInfo:" + _LastErrS);
		return false;
	}
}

String TServJiek::CYGStaDecode(int nStatus) {
	switch (nStatus) {
	case 0:
		return L"ִ��ȡ���������";
	case 1:
		return L"�ô�ȡ�ƻ����������";
	case 2:
		return L"���������ָ��λ�ã�������������λ������Ч���򲻴��ڡ��򲻿���";
	case 3:
		return L"ָ����ú����Ϣ��ú�����롢ú�����ͣ���Ч���򲻴���";
	default:
		return L"���� ��ȡ��������ִ�й�����ִ��" + IntToStr(nStatus);
	}
}

String TServJiek::CYGOpType_StaDecode(int OpType) {
	switch (OpType) {
	case 1:
		return L"ȡ��";
	case 2:
		return L"����";
	default:
		return L"δ֪" + IntToStr(OpType);
	}
}

// ---ǿ����ֹȡ��������
UTF8String TServJiek::ForceEndCunYG(UTF8String JsonReq) {
	Json::Value Ret_JsonData;
	if (!_bEnterCunYG) {

		return _jRet.CreatJsonRet(1, L"��ǰ����ȡ��������������,������ֹ", 0, Ret_JsonData);
	}

	_bEnterCunYG = false;
	_bForceEndCunYG = true;

	return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);
}
//---��������Ϣ�ϴ�
bool TServJiek::SaveCYGInfo_pt(TCYGInfo &CYGInfo){
    UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	Json::Value json_Row;

	json_Row["REC_NO"] =CYGInfo.REC_NO;
	json_Row["GGName"] =UTF8Encode(CYGInfo.GGName).c_str();
	json_Row["CCNAME"] =UTF8Encode(CYGInfo.CCNAME).c_str();
	json_Row["WWNAME"] =UTF8Encode(CYGInfo.WWNAME).c_str();
	json_Row["ENABLED"] =CYGInfo.ENABLED;
	json_Row["DEPOSITTIME"] =UTF8Encode(CYGInfo.DEPOSITTIME).c_str();
	json_Row["MAKESAMPLETIME"] =UTF8Encode(CYGInfo.MAKESAMPLETIME).c_str();
	json_Row["SAMPLETYPE"] =CYGInfo.SAMPLETYPE;
	json_Row["SAMPLEID"] =UTF8Encode(CYGInfo.SAMPLEID).c_str();
	json_Row["SAMPLESTATUS"] =UTF8Encode(CYGInfo.SAMPLESTATUS).c_str();

	JsonData.append(json_Row);
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"45", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SaveCYGInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCYGInfo_pt:" + _LastErrS);
		return false;
	}

	return true;

}
//---�������ȡ��¼�ϴ�
bool TServJiek::SaveCYGCQInfo_pt(TCYGCQInfo &CYGCQInfo){
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	Json::Value json_Row;

	json_Row["REC_NO"] =CYGCQInfo.REC_NO;
	json_Row["GGName"] =UTF8Encode(CYGCQInfo.GGName).c_str();
	json_Row["CCName"] =UTF8Encode(CYGCQInfo.CCNAME).c_str();
	json_Row["WWName"] =UTF8Encode(CYGCQInfo.WWNAME).c_str();
	json_Row["OpType"] =CYGCQInfo.OpType;
	json_Row["OpTime"] =UTF8Encode(CYGCQInfo.OpTime).c_str();
	json_Row["MakeSampleTime"] =UTF8Encode(CYGCQInfo.MakeSampleTime).c_str();
	json_Row["SAMPLETYPE"] =CYGCQInfo.SAMPLETYPE;
	json_Row["SourceName"] =UTF8Encode(CYGCQInfo.SourceName).c_str();
	json_Row["DestinationName"] =UTF8Encode(CYGCQInfo.DestinationName).c_str();
	json_Row["SampleID"] =UTF8Encode(CYGCQInfo.SampleID).c_str();
	json_Row["IsSuccessed"] =CYGCQInfo.IsSuccessed;
	json_Row["Operator"] =UTF8Encode(CYGCQInfo.Operator).c_str();

	JsonData.append(json_Row);
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"47", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SaveCYGCQInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCYGCQInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}
//---���������м�¼��ƽ̨
bool TServJiek::SaveCYGYXInfo_pt(String MachineCode, String strTime,
	 String strDec,String strCode){
	UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	JsonData["MACHINECODE"] = UTF8Encode(MachineCode).c_str();
	JsonData["ERRORDEC"] = UTF8Encode(strDec).c_str();
	JsonData["ERRORTIME"] = UTF8Encode(strTime).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"46", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SaveCYGYXInfo_pt,�����쳣:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCYGYXInfo_pt:" + _LastErrS);
		return false;
	}

	return true;

}

//---�����ܵ���Ϣ�ύ��ƽ̨
bool TServJiek::SaveQDGDInfo_pt(TQDGDInfo &QDGDInfo){
    UTF8String JsonReq;
	UTF8String JsonRet;

	Json::Value JsonData;
	Json::Value json_Row;
	json_Row["REC_NO"] =UTF8Encode(String(QDGDInfo.REC_NO)).c_str();
	json_Row["DeviceName"] =UTF8Encode(QDGDInfo.DeviceName).c_str();
	json_Row["DeviceStatus1"] =QDGDInfo.DeviceStatus1;
	json_Row["DeviceStatus2"] =QDGDInfo.DeviceStatus2;
	json_Row["DeviceStatus3"] =QDGDInfo.DeviceStatus3;
	json_Row["LineStatus"] =QDGDInfo.LineStatus;

	JsonData.append(json_Row);
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"48", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SaveCYGCQInfo_pt,�����쳣:" + e.Message;
		return false;
	}
	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCYGCQInfo_pt:" + _LastErrS);
		return false;
	}
	return true;
}
