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

// ---采样机&联合制样机 中间库接口

/*************************************************
 Function:		SamStaInfo
 Description:	采样机总体状态查询
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: MACHIN_STATUS_INFO
 Table Updated:  无
 Others:
 Author: 		武文斌
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
			_LastErrS = L"未查到数据";
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
		return L"故障中";
	case 1:
		return L"正在采样";
	case 2:
		return L"正在制样";
	case 3:
		return L"正在卸样";
	case 4:
		return L"暂停";
	case 5:
		return L"待机可采";
	case 6:
		return L"待机可卸";
	default:
		return L"未知状态:" + IntToStr(nStatus);
	}
}

/*************************************************
 Function:		CollectorInfo
 Description:	采样机集样器状态查询
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: MACHIN_STATUS_INFO
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-22
 ************************************************ */
UTF8String TServJiek::CollectorInfo(TLX_DB *connKYC, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		Json::Value val;
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}
	String Sql = L"select BARRELCODE 桶号,CY_CODE 采样编码,DOWNCONT 子样数,WEIGHT 重量,LAST_UPDATE_TIME 时间 \
	             from BARREL_STATUS_INFO where CYJ_MACHINE='" + Location + "' ";
	try {

		connKYC->GetDataSet(Sql);
		if (0 == connKYC->DSInfo.nRecordCount) {
			connKYC->CloseDataset();
			_LastErrS = "未查到数据";
			_File.WriteLog_ErrS(L"CollectorInfo:" + _LastErrS);
			return _jRet.CreatJsonRet(1, L"未查到数据", 0, Ret_JsonData);
		}
		else {

			int nRows = connKYC->DSInfo.nRecordCount;

			TStrings *List = new TStringList();
			WideString colList = "";
			connKYC->GetFieldNames(colList); // ---取列名
			List->Text = String(colList);

			Json::Value JsonData; // ---最后要生成对象数组
			Json::Value dataset; // ---数据集对象

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
 Description:	采样机控制
 Input:          JsonReq:Json格式请求串
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: MACHIN_CMD_INFO
 Table Updated:  MACHIN_CMD_INFO
 Others:
 Author: 		武文斌
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

		return _jRet.CreatJsonRet(0, L"成功", 0, Ret_JsonData);
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
			_LastErrS = L"要操作的数据不唯一";
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
 Description:	制样机总体状态查询
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: ZY_Status_Tb
 Table Updated:
 Others:
 Author: 		武文斌
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
			_LastErrS = L"未查到制样机状态数据";
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
 Description:	制样机详细状态查询
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: ZY_State_Tb
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-23
 ************************************************ */
UTF8String TServJiek::ZYSamDetailInfo(TLX_DB *connKYZ, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		Json::Value val;
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}
	String Sql = L"select DeviceCode 设备编号,DeviceName 设备名称,SampleID 制样编码,DeviceStatus 工作状态,LastUpdateTime 时间 \
	             from ZY_State_Tb where MachineCode='" + Location + "' ";
	try {

		_connKYZ->GetDataSet(Sql);
		if (0 == _connKYZ->DSInfo.nRecordCount) {
			_connKYZ->CloseDataset();
			_LastErrS = "未查到数据";
			return _jRet.CreatJsonRet(1, L"未查到数据", 0, Ret_JsonData);
		}
		else {

			int nRows = _connKYZ->DSInfo.nRecordCount;

			TStrings *List = new TStringList();
			WideString colList = "";
			_connKYZ->GetFieldNames(colList); // ---取列名
			List->Text = String(colList);

			Json::Value JsonData; // ---最后要生成对象数组
			Json::Value dataset; // ---数据集对象

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
		return L"停止";
	case 1:
		return L"正在运行中(不可制下一批样)";
	case 2:
		return L"故障";
	case 3:
		return L"就绪(可以卸样)";
	case 4:
		return L"急停";
	case 5:
		return L"可以制样,等待制样";
	default:
		return L"未知状态";
	}
}

/*************************************************
 Function:		ZYSamControl
 Description:	制样机控制
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: ZY_Cmd_Tb
 Table Updated:  ZY_Cmd_Tb
 Others:
 Author: 		武文斌
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

			return _jRet.CreatJsonRet(0, L"插入成功", 0, Ret_JsonData);
		}
		else if (1 == _connKYZ->DSInfo.nRecordCount) {

			_connKYZ->EditDS();
			_connKYZ->FieldAsName(L"CommandCode", ActionID);
			_connKYZ->FieldAsName(L"DataStatus", 0);
			_connKYZ->FieldAsName(L"SendCommandTime", Now().CurrentDateTime());
			_connKYZ->PostDS();
			_connKYZ->CloseDataset();

			return _jRet.CreatJsonRet(0, L"更新成功", 0, Ret_JsonData);
		}
		else {
			_connKYZ->CloseDataset();
			return _jRet.CreatJsonRet(1, L"要操作的数据不唯一", 0, Ret_JsonData);
		}
	}
	catch (...) {
		_connKYZ->Disconn();
		_LastErrS = _connKYZ->GetLastErr();
		return _jRet.CreatJsonRet(1, UTF8String(_LastErrS).c_str(), 0,
			Ret_JsonData);
	}
}

// ---采样机卸料
/*************************************************
 Function:		SamUnload
 Description:	写入采样机卸料信息
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: MACHIN_CMD_INFO
 Table Updated:  MACHIN_CMD_INFO
 Others:
 Author: 		武文斌
 Date:			2016-03-22
 ************************************************ */
UTF8String TServJiek::SamUnload(TLX_DB *connKYC, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	// ---取编码
	UTF8String Cybm;
	if (!_jReq.GetCZYBM(JsonReq, Cybm, 0)) {
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	if (SamUnload(connKYC, Location, Cybm)) {

		return _jRet.CreatJsonRet(0, L"成功", 0, Ret_JsonData);
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
			_LastErrS = L"要操作的数据不唯一";
			return false;
		}
	}
	catch (...) {
		connKYC->Disconn();
		_LastErrS = connKYC->GetLastErr();
		return false;
	}
}


/* 读取采样机卸料完成状态 */
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
			_LastErrS = L"要操作的数据不唯一";
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
 Description:	写入制样机卸料信息
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: ZY_Interface_Tb
 Table Updated:  ZY_Interface_Tb
 Others:
 Author: 		武文斌
 Date:			2016-03-22
 ************************************************ */
UTF8String TServJiek::ZYSamUnload(TLX_DB *connKYZ, UTF8String JsonReq) {
	UTF8String Location;
	Json::Value Ret_JsonData;
	if (!_jReq.GetLocation(JsonReq, Location)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	// ---取编码
	UTF8String Zybm;
	if (!_jReq.GetCZYBM(JsonReq, Zybm, 1)) {
		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}

	if (ZYSamUnload(connKYZ, Location, Zybm)) {

		return _jRet.CreatJsonRet(0, L"成功", 0, Ret_JsonData);
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
			_LastErrS = L"要操作的数据不唯一";
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
			_LastErrS = L"要操作的数据不唯一";
			return false;
		}
	}
	catch (...) {
		connKYZ->Disconn();
		_LastErrS = connKYZ->GetLastErr();
		return false;
	}
}

/* 读取制样记录 */
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

/* 整合后的制样接口,供第三方调用 */
UTF8String TServJiek::ZYSamUnload_New(UTF8String JsonReq) {
	Json::Value Ret_JsonData;
	if (_bEnterSamUnload) {

		if (!SaveZhiybjData_pt(_ZYLocation, "正在卸料,请稍侯", L"0")) {

		_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" + _LastErrS);
		}
		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}

		return _jRet.CreatJsonRet(1, L"正在卸料,请稍侯", 0, Ret_JsonData);
	}

	UTF8String CYLocation, ZYLocation;

	if (!_jReq.GetCZYLocation(JsonReq, CYLocation, ZYLocation)) {

		return _jRet.CreatJsonRet(2, _jReq.GetLastErrS(), 0, Ret_JsonData);
	}
	// ---取编码
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

	_bEnterSamUnload = true; // ---进入卸料状态
	return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);
}

// ---强制中止卸料
UTF8String TServJiek::ForceEndSamLoad(UTF8String JsonReq) {
	Json::Value Ret_JsonData;
	if (!_bEnterSamUnload) {

		return _jRet.CreatJsonRet(1, L"当前不在制样过程中,无须中止", 0, Ret_JsonData);
	}

	_bEnterSamUnload = false;
	_bForceEndSamUnload = true;

	return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);
}

// ---卸料过程,在Windows服务的线程中调用
void TServJiek::SamUnloadProc() {
	_bForceEndSamUnload = false; // ---一旦等于true,制样过程应强制结束
	// ---判断采样机状态
	int nStatus;
	if (!SamStaInfo(_connKYC, _CYLocation, nStatus)) {

		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		// ---调平台接口
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
		// ---调平台接口把信息写回
		String strMsg = L"采样机不具备卸样条件:" + CYJStaDecode(nStatus);
		_File.WriteLog_ErrS(L"SamUnloadProc:" + strMsg);
		if (!SaveZhiybjData_pt(_ZYLocation, strMsg, IntToStr(nStatus))) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}

	// ---判断制样机状态是否可以卸样
	String StatusID;
	if (!ZYSamStaInfo(_connKYZ, _ZYLocation, StatusID)) {

		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		// ---调平台接口
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
		// ---调平台接口把信息写回
		String strMsg = L"由于制样机未就绪,导致采样机不能卸样:" + ZYJStaDecode
			(StatusID.ToIntDef(-1));
		if (!SaveZhiybjData_pt(_ZYLocation, strMsg, StatusID)) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_File.WriteLog_ErrS(L"SamUnloadProc:" + strMsg);
		_bEnterSamUnload = false;
		return;
	}

	// ---采样机卸料
	if (!SamUnload(_connKYC, _CYLocation, _Cybm)) {

		if (!UploadXlwcxx_pt(_Cybm, 0)) {

			_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		_File.WriteLog_ErrS(L"SamUnload@SamUnloadProc:" + _LastErrS);
		if (!SaveZhiybjData_pt(_ZYLocation, "采样机卸料失败:" + _LastErrS,
			IntToStr(nStatus))) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}

	bool bRet;
	// ---等待进入卸料状态
	do {
		nStatus = -1;
		SamStaInfo(_connKYC, _CYLocation, nStatus); // 获取采样机状态
		_File.WriteLog(L"SamUnloadProc:等待进入卸料状态,采样机状态:" +
			CYJStaDecode(nStatus));
		if (_bForceEndSamUnload) {

			_bEnterSamUnload = false; // ---退出卸料状态
			_File.WriteLog(L"SamUnloadProc:等待进入卸料状态时,被强制中止");
			return;
		}

		if (0 == nStatus) {

			_File.WriteLog(L"SamUnloadProc:等待进入卸料状态时,采样机:" +
				CYJStaDecode(nStatus));
			_bEnterSamUnload = false;
			return;
		}
		::Sleep(3000);
	}
	while (nStatus != 3);

	_File.WriteLog(L"SamUnloadProc:采样机进入卸料状态");
    // ---写入制样机制样信息
	if (!ZYSamUnload(_connKYZ, _ZYLocation, _Zybm)) {

		_File.WriteLog_ErrS(L"ZYSamUnload@SamUnloadProc:" + _LastErrS);
		if (!SaveZhiybjData_pt(_ZYLocation, "写入制样机制样信息失败:" + _LastErrS, L"-1"))
		{

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}
	// ---等待卸料完成
	do {
		bRet = ReadSamLoadStatus(_connKYC, _CYLocation, _Cybm);
		if (_bForceEndSamUnload) {

			_bEnterSamUnload = false; // ---退出卸料状态
			_File.WriteLog(L"SamUnloadProc:等待采样机卸料完成时,被强制中止");
			return;
		}
		::Sleep(3000);
	}
	while (!bRet);

	_File.WriteLog(L"SamUnloadProc:采样机卸料完成,等待制样机进入可制样状态");
	// ---再次判断制样机状态是否可以制样
	do{
		StatusID=-1;
		if (!ZYSamStaInfo(_connKYZ, _ZYLocation, StatusID)) {

			// ---调平台接口
			_File.WriteLog_ErrS(L"采样机卸料后，ZYSamStaInfo@SamUnloadProc:" + _LastErrS);
			if (!SaveZhiybjData_pt(_ZYLocation, _LastErrS, L"-1")) {

				_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
					_LastErrS);
			}
			_bEnterSamUnload = false;
			return;
		}
		if (_bForceEndSamUnload) {

			_bEnterSamUnload = false; // ---退出卸料状态
			_File.WriteLog(L"SamUnloadProc:等待进入可制样状态时,被强制中止");
			return;
		}
		::Sleep(2000);
	}
	while(StatusID == L"3");
	if (StatusID != L"5") {

		// ---调平台接口把信息写回
		String strMsg = L"采样机卸料后，由于制样机未就绪,导致制样机不能制样:" +
			ZYJStaDecode(StatusID.ToIntDef(-1));
		_File.WriteLog_ErrS(L"SamUnloadProc:" + strMsg);
		if (!SaveZhiybjData_pt(_ZYLocation, _LastErrS, StatusID)) {

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		//推送到平台 已卸样未制样
		if (!UploadXlwcxx_pt(_Cybm, 3)) {

		_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}
	_File.WriteLog(L"SamUnloadProc:制样机进入可制样状态,写入制样信息");

	// ---写入制样机制样信息
	if (!ZYSamUnload(_connKYZ, _ZYLocation, _Zybm)) {

		_File.WriteLog_ErrS(L"ZYSamUnload@SamUnloadProc:" + _LastErrS);
		if (!SaveZhiybjData_pt(_ZYLocation, "写入制样机制样信息失败:" + _LastErrS, L"-1"))
		{

			_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
				_LastErrS);
		}
		_bEnterSamUnload = false;
		return;
	}
	 // ---启动制样机
	 _File.WriteLog(L"SamUnloadProc:启动制样机开始制样");
	 if (!ZYSamControl(_connKYZ, _ZYLocation, _Zybm,1)) {

		 _File.WriteLog_ErrS(L"ZYSamUnload@SamUnloadProc:" + _LastErrS);
		 if (!SaveZhiybjData_pt(_ZYLocation, "写入制样机制样信息失败:" + _LastErrS, L"-1"))
		 {

		 _File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" +
		 _LastErrS);
		 }
		 _bEnterSamUnload = false;
	 	return;
	 }

	// ---读取制样记录，等待制样完成
	_File.WriteLog(L"SamUnloadProc:等待制样机完成制样");
	do {
		bRet = ReadZyRecord(_connKYZ, _ZYLocation, _Zybm);
		if (_bForceEndSamUnload) {

			_bEnterSamUnload = false; // ---退出卸料状态
			_File.WriteLog(L"SamUnloadProc:等待制样机制样完成时,被强制中止");
			return;
		}
		::Sleep(3000);
	}
	while (!bRet);
	_File.WriteLog(L"SamUnloadProc:制样机制样完成");

	if (!SaveZhiybjData_pt(_ZYLocation, "制样机制样完成", L"0")) {

		_File.WriteLog_ErrS(L"SaveZhiybjData_pt@SamUnloadProc:" + _LastErrS);
	}
	if (!UploadXlwcxx_pt(_Cybm, 1)) {

		_File.WriteLog_ErrS(L"UploadXlwcxx_pt@SamUnloadProc:" + _LastErrS);
	}

	_bEnterSamUnload = false; // ---退出卸料状态
}

// ---读写器读卡接口,供平台使用

/*************************************************
 Function:		ReadCardID
 Description:	通过读写器读取卡号
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed:
 Table Updated:
 Others:
 Author: 		武文斌
 Date:			2016-03-29
 ************************************************ */
UTF8String TServJiek::ReadCard(UTF8String JsonReq) {
	Json::Value Ret_JsonData;
	TCardInfo CardInfo;

	if (ReadCardID(0, CardInfo)) {

		String strCard = "";
		Json::Value json_Row; // ---每行一个对象,插入到 Ret_JsonData中,组成数组
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

		_File.WriteLog(L"读卡成功,卡数:" + IntToStr(CardInfo.Cnts) + L",卡号:" +
			strCard);
		return _jRet.CreatJsonRet(0, L"", CardInfo.Cnts, Ret_JsonData);
	}
	else {
		return _jRet.CreatJsonRet(1, L"读卡失败:" + _LastErrS, CardInfo.Cnts,
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

			_LastErrS = L"写卡失败:错误的扇区编号:" + IntToStr(nSector);
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (0 == nSector && 0 == nBlock) {

			_LastErrS = L"写卡失败:0区0块为只读区";
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (3 == nBlock) {

			_LastErrS = L"写卡失败:块3为密码存储区,暂不支持写入";
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (nBlock < 0 || nBlock > 3) {

			_LastErrS = L"写卡失败:错误的块编号:" + IntToStr(nBlock);
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		TDataSwitch DS;
		if (!DS.IsValid_HexStr(strPsw)) {

			_LastErrS = L"写卡失败:密码格式错误:" + strPsw;
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (strData.Length() > 16) {

			_LastErrS = L"写卡失败:数据超过16字节:" + strData;
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}

		if (!WriteICCard(0, nSector, nBlock, strPsw, strData)) {

			_LastErrS = L"写卡失败:" + _LastErrS;
			_File.WriteLog(_LastErrS);
			return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
		}
		return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);

	}
	catch (Exception &e) {

		_LastErrS = L"写卡失败:" + e.Message;
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
	catch (...) {

		_LastErrS = L"写卡失败:未知错误";
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
}

// ---停车场LED公告信息
/* 调度信息 */
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
		Json::Value JsonData_Numplate; // ---车号数组
		JsonData = jv_req["JsonData"];
		JsonData_Numplate = JsonData["Numplate"];

		_CarList->Clear();
		for (unsigned int i = 0; i < JsonData_Numplate.size(); i++) {

			_CarList->Add(UTF8String(JsonData_Numplate[i].asCString()));
		}

		_ParkingNum_Total = JsonData["ParkingNum_Total"].asInt(); // ---停车场总车位数
		_ParkingNum_Surplus = JsonData["ParkingNum_Surplus"].asInt();
		// ---停车场剩余车位
		_ScheduledNum = JsonData["ScheduledNum"].asInt(); // ---当前场内的被调度车数
		_ToScheduleNum = JsonData["ToScheduleNum"].asInt(); // ---当前场内未调试车数

		_bSendLED_DDInfo = true;
		return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);

	}
	catch (Exception &e) {

		_LastErrS = L"SendLED_Diaod异常:" + e.Message + L",传参:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
	catch (...) {

		_LastErrS = L"SendLED_Diaod异常:未知错误。传参:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(2, _LastErrS, 0, Ret_JsonData);
	}
}

// ---LDE公告信息
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

		_GGTitle = UTF8String(JsonData["Title"].asCString()); // ---标题
		_GGText = UTF8String(JsonData["Text"].asCString()); // ---停车场剩余车位
		_GGDepartment = UTF8String(JsonData["Department"].asCString());
		// ---当前场内的被调度车数
		_GGDate = UTF8String(JsonData["Date"].asCString()); // ---当前场内未调试车数

		_bSendLED_GGInfo = true;
		return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);

	}
	catch (Exception &e) {

		_LastErrS = L"SendLED_Gongg:" + e.Message + L",传参:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
	catch (...) {

		_LastErrS = L"SendLED_Gongg:未知错误。传参:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(2, _LastErrS, 0, Ret_JsonData);
	}
}

// ---写入采样信息到采样机中间库
bool TServJiek::WriteCarInfoToCYJ(TCarInfo &CarInfo, TCaiyInfo &CaiyInfo) {

	String Sql = "select * from MACHIN_CMD_INFO where CYJ_MACHINE='" +
		_sysInfo.CYJNO + "'";

	String _Sql = "select * from RLCARMST where CYJ_MACHINE='" +
		_sysInfo.CYJNO + "'";

	try {

		_connKYC->GetDataSet(_Sql);
		if (_connKYC->DSInfo.nRecordCount > 0) { // ---无数据,插入

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
				_LastErrS = "车辆信息表更新时sql执行失败:" + e.Message;
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
							_LastErrS = "车辆拉筋信息更新时sql执行失败" + e.Message;
							_File.WriteLog_ErrS(_LastErrS);
							return false;
						}

					}
				}
			}
			catch (...) {
				_LastErrS = "解析平台提供的采样区域字符串时出错:" + String(CarInfo.Ljwz[12]);
				_File.WriteLog_ErrS(_LastErrS);
				return false;
			}
		}
		else {
			_connKYC->CloseDataset();
			_LastErrS = "未查到数据";
			_File.WriteLog_ErrS(_LastErrS);
			return false;
		}
		::Sleep(3000);

		_connKYC->CloseDataset();

		try {
			_connKYC->GetDataSet(Sql);
			if (_connKYC->DSInfo.nRecordCount > 0) { // ---有数据,更新
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
					_LastErrS = "采样控制表更新时sql执行失败" + e.Message;
					_File.WriteLog_ErrS(_LastErrS);
					return false;
				}

			}
			else {
				_connKYC->CloseDataset();
				_LastErrS = "未查到数据";
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

// ---判断采样记录是否完整
bool TServJiek::GetCyComplete(CarSamplingData &CayInfo) {
	String Sql = "select * from TAKE_SAMPLE_REC where CYJ_MACHINE='" +
		_sysInfo.CYJNO + "'     \
					and DataStatus =0 and WEIGHT_DataStatus = 1";

	TClientDataSet *CDS = new TClientDataSet(NULL);
	try {
		OleVariant OleVar;
		OleVar = _connKYC->GetDatasetEX(Sql);
		CDS->Data = OleVar;
		if (CDS->RecordCount > 0) { // ---检测到采样完成信号

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

			// ---取完数据后,更新标志位
			Sql = "update TAKE_SAMPLE_REC set DataStatus =1,WEIGHT_DataStatus = 2 where DataStatus =0 and WEIGHT_DataStatus = 1";
			if (!_connKYC->SQLExcute(Sql)) {
				_LastErrS = "更新采样记录标志位失败";
				_File.WriteLog_ErrS(_LastErrS);
				return false;
			}
			else
				return true;

		}
		else {
			_LastErrS = "无采样完成记录";
			CDS->Free();
			return false;
		}
	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "读取采样完成信号时出错:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		CDS->Free();
		return false;
	}
}

// ---------判断采样机状态是否完成采样
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
			_LastErrS = "无采样记录,采样未完成:" + _connKYC->GetLastErr();
			return false;
		}
	}
	catch (...) {
		_connKYC->Disconn();
		_LastErrS = "查询采样状态是否完成失败:" + _connKYC->GetLastErr();
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
}

// ---------判断采样机状态是否完中途暂停
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
			_LastErrS = "采样机中途未暂停";
			return false;
		}
	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "查询采样机是否中途暂停失败:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
}

// ---中途停止，刷卡更新采样机控制标志
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
				_LastErrS = "车辆信息表更新标志位时sql执行失败:" + _connKYC->GetLastErr();
				return false;
			}
			_connKYC->CloseDataset();
		}
		else {
			_connKYC->Disconn();
			_LastErrS = "无采样记录，更新标志位失败:" + _connKYC->GetLastErr();
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
				_LastErrS = "采样控制表更新启动标志位时sql执行失败:" + _connKYC->GetLastErr();
				return false;
			}
			_connKYC->CloseDataset();
			return true;
		}
		else {
			_connKYC->Disconn();
			_LastErrS = "无采样记录，更新标志位失败:" + _connKYC->GetLastErr();
			return false;
		}

	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "更新中间库启动标志失败:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	}
}

// ---检测采样机集样器状态信息
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
					_LastErrS = "上传集样罐信息失败:" + GetLastErrS();
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
			_LastErrS = "无更新记录:" + _connKYC->GetLastErr();
			return false;
		}

	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "查看集样器信息时数据库打开失败:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		CDS->Free();
		return false;
	}
}

// ---检测采样机运行实时大车、小车位置
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
				_LastErrS = "上传实时运行数据失败:" + GetLastErrS();
				_File.WriteLog_ErrS(_LastErrS);
			}

			CDS->Free();
			return true;
		}
		else {
			CDS->Free();
			_LastErrS = "无更新记录:" + _connKYC->GetLastErr();
			return false;
		}

	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "查看实时运行信息时数据库打开失败:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		CDS->Free();
		return false;
	}
}

//---写入采样编码和化验编码到化验室中间库
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
		_LastErrS = "插入值时化验室数据库打开失败:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		return false;
	 }
}

// ---检测采样机采样准备状态信息
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
				_LastErrS = "上传采样准备状态失败:" + GetLastErrS();
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
			_LastErrS = "无更新记录:" + _connKYC->GetLastErr();
			return false;
		}

	}
	catch (Exception &e) {
		_connKYC->Disconn();
		_LastErrS = "查看实时运行信息时数据库打开失败:" + e.Message;
		_File.WriteLog_ErrS(_LastErrS);
		CDS->Free();
		return false;
	}
}

// ---平台提供的接口服务

// ---取服务器时间
bool TServJiek::GetServerSystemTime_pt(String &sysTime) {
	// ---暂未开放,用本地时间代替
	sysTime = Now().FormatString(L"yyyy-MM-dd HH:mm:ss");
	return true;
}

// ---读取基础数据
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
		_LastErrS = L"平台接口Nmlx_DataService异常:" + e.Message;
		_File.WriteLog_ErrS(L"GetGeneralData_pt:" + _LastErrS);
		return false;
	}
	catch (...) {
		_LastErrS = "平台接口Nmlx_DataService异常:未知错误";
		_File.WriteLog_ErrS(L"GetGeneralData_pt:" + _LastErrS);
		return false;
	}
}

/*************************************************
 Function:		Login_pt
 Description:	登录验证
 Input:          JsonReq:Json格式请求数字
 Output:         无
 Return:         返回JsonRet字符串
 Calls:          详见函数
 Table Accessed: MACHIN_STATUS_INFO
 Table Updated:  无
 Others:
 Author: 		武文斌
 Date:			2016-03-24
 ************************************************ */
bool TServJiek::Login_pt(String User, String Pass) {
	UTF8String JsonReq;
	UTF8String JsonRet;
	// ---权限判断
	Json::Value JsonData_QX;
	JsonData_QX["UserName"] = UTF8Encode(User).c_str();
	JsonData_QX["RoleName"] = BankQX.c_str();
	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"31", JsonData_QX);
	JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	if (_jRet.RsultAnalysis(JsonRet))
		return true;
	else {
		_LastErrS = L"无权限:" + _jRet.GetLastErrS();
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
		_LastErrS = L"Login_pt,服务报异常:" + e.Message;
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
 根据流程代码获取流程ID
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
		_LastErrS = L"GetFlowID,服务报异常:" + e.Message;
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

		_LastErrS = L"取流程ID失败:JsonData为空";
		_File.WriteLog_ErrS(L"GetFlowID:" + _LastErrS);
		return false;
	}
	if (!JsonData.isArray()) {

		_LastErrS = L"取流程ID失败:JsonData不是数组";
		_File.WriteLog_ErrS(L"GetFlowID:" + _LastErrS);
		return false;
	}
	int i = 0;
	if (!JsonData[i]["SORTCODE"].isInt()) {

		_LastErrS = L"取流程ID失败:SORTCODE不是整型";
		_File.WriteLog_ErrS(L"GetFlowID:" + _LastErrS);
		return false;
	}
	FLowID = JsonData[i]["SORTCODE"].asInt();
	return true;
}

/*

 根据车号获取采样编码信息、采样点数、供应商信息、采样类型
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
		_LastErrS = L"GetCarCYInfo,服务报异常:" + e.Message;
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

		_LastErrS = L"取批次信息失败:JsonData为空";
		_File.WriteLog_ErrS(L"GetCarCYInfo:" + _LastErrS);
		return false;
	}
	/*
	if (!JsonData.isArray()) {

		_LastErrS = L"取流程ID失败:JsonData不是Json对象";
		_File.WriteLog_ErrS(L"GetCarFlowID:" + _LastErrS);
		return false;
	}
	int i = 0;
	*/
	if (!JsonData["Dians"].isString()) {

		_LastErrS = L"取批次失败:Dians不是整型";
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
 根据车号获取当前车的流程ID
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
		_LastErrS = L"GetCarFlowID,服务报异常:" + e.Message;
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

		_LastErrS = L"取流程ID失败:JsonData为空";
		_File.WriteLog_ErrS(L"GetCarFlowID:" + _LastErrS);
		return false;
	}
	if (!JsonData.isArray()) {

		_LastErrS = L"取流程ID失败:JsonData不是Json对象";
		_File.WriteLog_ErrS(L"GetCarFlowID:" + _LastErrS);
		return false;
	}
	int i = 0;
	if (!JsonData[i]["SORTCODE"].isInt()) {

		_LastErrS = L"取流程ID失败:SORTCODE不是整型";
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

// ---读取车辆信息
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
		_LastErrS = L"Login_pt,服务报异常:" + e.Message;
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
		_LastErrS = L"异常:" + e.Message;
		_File.WriteLog_ErrS(L"GetCarInfo_pt:" + _LastErrS);
		return false;
	}
	catch (...) {

		CDS->Close();
		CDS->Free();
		_LastErrS = L"异常:未知错误";
		_File.WriteLog_ErrS(L"GetCarInfo_pt:" + _LastErrS);
		return false;
	}
}

// ---读取燃煤的入厂计划:计划内的煤矿供应商关系表
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
		_LastErrS = L"GetRanmJih_pt,服务报异常:" + e.Message;
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
 将车号和IC卡号进行绑定或解绑
 Action 0:绑定 1:解绑
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
		_LastErrS = L"BingdingICCard_pt,服务报异常:" + e.Message;
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
 提交验票信息
 16.4.22 Wuwenbin新增 fKuangfl参数
 */
bool TServJiek::SubmitYanp_pt(String Numplate, double fKuangfl, String Meik,
	String Gongys, String Pinz) {
	String sysTime;
	UTF8String Yunsfs = L"公路";
	UTF8String Yunsdw = UTF8Encode(Meik);
	UTF8String MeiC = L"无";
	UTF8String MeiD = L"无";

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
		_LastErrS = L"SubmitYanp_pt,服务报异常:" + e.Message;
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
 提交计量数据到平台
 JilTag 0：计重 1：计皮
 */
bool TServJiek::SubmitJilInfo_pt(String Numplate, double fZhongl, int JilTag) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"自动过衡";

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
		_LastErrS = L"SubmitJilInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitJilInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---提交采样详细记录信息到平台

bool TServJiek::SubmitCaiYRecord_pt(CarSamplingData param) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"采样记录接口";

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
		_LastErrS = L"SubmitCaiYRecord_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitCaiYRecord_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ----提交采样完成流程服务到平台
bool TServJiek::SubmitCaiYInfo_pt(UTF8String Car) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"采样机接口服务";

	Json::Value JsonData;
	JsonData["CHEPH"] = Car.c_str();
	JsonData["CAOZRY"] = CaozRY.c_str();
	JsonData["YEWDD"] = UTF8Encode(String(_sysInfo.Location)).c_str();

	JsonReq = _jReq.CreateJsonReq_pt(User_PTJK, Pass_PTJK, L"33", JsonData);

	try {
		JsonRet = _ptSoap->Nmlx_DataService(JsonReq);
	}
	catch (Exception &e) {
		_LastErrS = L"SubmitCaiYInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitCaiYInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}
//----提交采样机准备状态信息到画面
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
		_LastErrS = L"SubmitCYZBInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitCYZBInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ----提交集样罐状态信息到平台
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
		_LastErrS = L"SubmitJiYTInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SubmitJiYTInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---进出停车场
// ---Action 0:进 1:出
bool TServJiek::EnterExitParking_pt(String Numplate, int Action) {
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"自助入场";

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
		_LastErrS = L"EnterExitParking_pt,服务报异常:" + e.Message;
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

		_LastErrS = L"出入停车场失败:JsonData为空";
		_File.WriteLog_ErrS(L"EnterExitParking_pt:" + _LastErrS);
		return false;
	}
	if (!JsonData.isObject()) {

		_LastErrS = L"出入停车场失败:JsonData不是Json对象";
		_File.WriteLog_ErrS(L"EnterExitParking_pt:" + _LastErrS);
		return false;
	}

	if (JsonData["Address"].isNull()) {

		_LastErrS = L"出入停车场失败:Address为NULL";
		_File.WriteLog_ErrS(L"EnterExitParking_pt:" + _LastErrS);
		return false;
	}

	// NextStation =JsonData["Address"].asCString();

	return true;
}

// ---进出煤场
// ---Action 0:进 1:出
bool TServJiek::EnterExitMC_pt(String Numplate, int Action, UTF8String Location)
{
	UTF8String JsonReq;
	UTF8String JsonRet;

	UTF8String CaozRY = L"自助入场";

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
		_LastErrS = L"EnterExitMC_pt,服务报异常:" + e.Message;
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
		_LastErrS = L"GetNextStation,服务报异常:" + e.Message;
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

		_LastErrS = L"获取下一流程失败:JsonData为空";
		_File.WriteLog_ErrS(L"GetNextStation:" + _LastErrS);
		return false;
	}
	if (!JsonData.isArray()) {

		_LastErrS = L"获取下一流程失败:JsonData不是Json数组";
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

// ---上传制样信息
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
		_LastErrS = L"SaveZhiybjData_pt,服务报异常:" + e.Message;
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
 出票数据
 */
bool TServJiek::ChupData_pt(String Numplate, double kous, double koug,
	double kouz, double kouqt, TBangdInfo &BangdInfo) {
	UTF8String JsonReq;
	UTF8String JsonRet;
	UTF8String CaozRY = L"自助出厂";
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
		_LastErrS = L"SaveZhiybjData_pt,服务报异常:" + e.Message;
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

		_LastErrS = L"出票失败:JsonData为空";
		_File.WriteLog_ErrS(L"ChupData:" + _LastErrS);
		return false;
	}
	if (!JsonData.isArray()) {

		_LastErrS = L"出票失败:JsonData不是Json数组";
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

// ---上传卸料完成信息 nIsXL:0未完成 1完成
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
		_LastErrS = L"UploadXlwcxx_pt,服务报异常:" + e.Message;
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

// ---提交采样机运行状态信息
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
		_LastErrS = L"SaveCaiYZTInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCaiYZTInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---提交制样机运行状态信息
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
		_LastErrS = L"SaveZhiYZTInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveZhiYZTInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ----提交化验汇总数据信息
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
		_LastErrS = L"SaveHUAYInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveHUAYInfo_pt:" + _LastErrS);
		return false;
	}

	return true;

}

// ---推送采样机启动信息到平台，通知设备监控界面更新车辆信息
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
		_LastErrS = L"UpdateCaiYZTInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@UpdateCaiYZTInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}

// ---推送采样机实时运行数据大车、小车位置
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
		_LastErrS = L"RealTimeCaiYZTInfo_pt,服务报异常:" + e.Message;
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
//----提交全水样数据
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
		_LastErrS = L"SaveQSYInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveQSYInfo_pt:" + _LastErrS);
		return false;
	}

	return true;

}

// ---存样柜
UTF8String TServJiek::CunYG(UTF8String JsonReq) {
	Json::Value jv_req;
	Json::Value Ret_JsonData;
	if (_bEnterCunYG) {

		return _jRet.CreatJsonRet(1, L"正在取样操作,请稍侯", 0, Ret_JsonData);
	}
	if (!_jRet.JsonStrToJson_pt(JsonReq, jv_req)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"CunYG:" + _LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData); ;
	}

	try {
		Json::Value JsonData;
		Json::Value JsonData_Yangp; // ---取样编码数组
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

		_LastErrS = L"CunYG异常:" + e.Message + L",传参:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(1, _LastErrS, 0, Ret_JsonData);
	}
	catch (...) {

		_LastErrS = L"CunYG异常:未知错误。传参:" + String(JsonReq);
		_File.WriteLog(_LastErrS);
		return _jRet.CreatJsonRet(2, _LastErrS, 0, Ret_JsonData);
	}
}

// ---取样过程,在Windows服务的线程中调用
void TServJiek::CunYG_QuY() {
	_bForceEndCunYG = false; // ---一旦等于true,取样过程应强制结束
	// ---判断存样柜状态
	int nStatus;
	int RunOpType = -1;
	if (!CunYG_IsRun(_connCYG, RunOpType)) {

		_File.WriteLog_ErrS(L"IsRun_CunYG@CunYG_QuY:" + _LastErrS);
		if (!SaveCYGYXInfo_pt(0, Now(), L"查询存样柜是否正在操作失败",0)){
			 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
			 _LastErrS);
		 }
		_bEnterCunYG = false;
		return;
	}

	if (RunOpType > 0) {
		_File.WriteLog(L"CunYG_QuY:存样柜正在进行" + CYGOpType_StaDecode(RunOpType)
			+ L"操作");
		if (!SaveCYGYXInfo_pt(0, Now(), L"存样柜正在进行" + CYGOpType_StaDecode(RunOpType)
			+ L"操作",0)){
			 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
			 _LastErrS);
		 }
		_bEnterCunYG = false;
		return;
	}
	if (!CunYG_IsGetSta(_connCYG)) {
		_File.WriteLog_ErrS(L"CunYG_IsGetSta@CunYG_QuY:" + _LastErrS);
		if (!SaveCYGYXInfo_pt(0, Now(), L"提交操作结果失败",0)){
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
			if (!SaveCYGYXInfo_pt(0, Now(), L"发送"+CYGOpType_StaDecode(CunYG_OpType)+L"信息失败",0)){
				 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
				 _LastErrS);
			 }
		}
		else {
			_File.WriteLog(L"CunYG_QuY:存样柜开始进行" + CYGOpType_StaDecode
				(CunYG_OpType) + L"操作");
			bool bRet;
			do {
				nStatus = -1;
				// 获取存样柜状态
				bRet = CunYGStaInfo(_connCYG, _BianMList->Strings[i],
					CunYG_OpType, CunYG_OpDest, CunYG_UserName, nStatus);
				if (_bForceEndCunYG) {

					_bEnterCunYG = false; // ---退出取样状态
					_File.WriteLog(L"CunYG_QuY:等待存样柜操作,被强制中止");
					return;
				}
				::Sleep(1000);
			}
			while (!bRet);
			_File.WriteLog(L"CunYG_QuY:存样柜操作完成,操作状态:" + CYGStaDecode(nStatus));
			// 完成
			if (!SaveCYGYXInfo_pt(0, Now(), L"存样柜操作完成,操作状态:" + CYGStaDecode(nStatus),0)){
				 _File.WriteLog_ErrS(L"SaveCYGYXInfo_pt@CunYG_QuY:" +
				 _LastErrS);
			 }
		}

	}

	_bEnterCunYG = false; // ---退出卸料状态
}

// 判断存样柜是否正在取样、弃样操作
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

// 判断存样柜是否存在未提取的取样、弃样操作
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
			if (!SaveCYGYXInfo_pt(0, Now(), L"存样柜" + CYGOpType_StaDecode(OpType)
			+ L"操作状态:" + CYGStaDecode(SampleStatus),0)){
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

// 提交取样弃样操作
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

// 获取操作完成状态
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
			_LastErrS = L"要操作是数据不唯一";
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
		return L"执行取弃任务故障";
	case 1:
		return L"该存取计划已添加完成";
	case 2:
		return L"存查样柜内指定位置（柜名、层名、位名）无效、或不存在、或不可用";
	case 3:
		return L"指定的煤样信息（煤样编码、煤样类型）无效、或不存在";
	default:
		return L"其它 ：取弃任务已执行过，不执行" + IntToStr(nStatus);
	}
}

String TServJiek::CYGOpType_StaDecode(int OpType) {
	switch (OpType) {
	case 1:
		return L"取样";
	case 2:
		return L"弃样";
	default:
		return L"未知" + IntToStr(OpType);
	}
}

// ---强制中止取样、弃样
UTF8String TServJiek::ForceEndCunYG(UTF8String JsonReq) {
	Json::Value Ret_JsonData;
	if (!_bEnterCunYG) {

		return _jRet.CreatJsonRet(1, L"当前不在取样、弃样过程中,无须中止", 0, Ret_JsonData);
	}

	_bEnterCunYG = false;
	_bForceEndCunYG = true;

	return _jRet.CreatJsonRet(0, L"", 0, Ret_JsonData);
}
//---存样柜信息上传
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
		_LastErrS = L"SaveCYGInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCYGInfo_pt:" + _LastErrS);
		return false;
	}

	return true;

}
//---存样柜存取记录上传
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
		_LastErrS = L"SaveCYGCQInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCYGCQInfo_pt:" + _LastErrS);
		return false;
	}

	return true;
}
//---存样柜运行记录到平台
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
		_LastErrS = L"SaveCYGYXInfo_pt,服务报异常:" + e.Message;
		return false;
	}

	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCYGYXInfo_pt:" + _LastErrS);
		return false;
	}

	return true;

}

//---气动管道信息提交到平台
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
		_LastErrS = L"SaveCYGCQInfo_pt,服务报异常:" + e.Message;
		return false;
	}
	if (!_jRet.RsultAnalysis(JsonRet)) {

		_LastErrS = _jRet.GetLastErrS();
		_File.WriteLog_ErrS(L"RsultAnalysis@SaveCYGCQInfo_pt:" + _LastErrS);
		return false;
	}
	return true;
}

