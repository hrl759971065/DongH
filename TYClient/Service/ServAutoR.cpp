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
		// ---NodeID 0自助入厂 1停车场 2采样 3计量 4煤场 5自助出厂 6RFID卡办理
		_WorkSta.StrFlow = L"系统正在初始化," + Now().TimeString();
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
			_WorkSta.StrFlow = L"流程ID获取失败," + Now().TimeString();
			SendLed(0, L"故障,流程ID获取失败");
			Sleep(3000);
			return;
		}
	}

	String CurrDate;
	if (!GetServerSystemTime_pt(CurrDate)) {

		_ErrCnt++;
		_WorkSta.StrFlow = L"系统时间获取失败," + Now().TimeString();
		SendLed(0, L"获取系统时间失败");
		Sleep(2000);
		return;
	}

	if (0 == _sysInfo.NodeID) {

		if (_ServerDate != CurrDate.SubString(1, 10)) { // ---与服务器不是同一日期

			// ---取日煤矿供应商计划表
			if (GetRanmJih_pt(_dsMkGys)) {

				_ServerDate = CurrDate.SubString(1, 10);
			}
			else {
				_ErrCnt++;
				_WorkSta.StrFlow = L"获取当日来煤计划失败," + Now().TimeString();
				SendLed(0, L"获取当日来煤计划失败");
				Sleep(3000);
				return;
			}
		}

		// ---查询发卡装置的状态
		BYTE szStatus[4] = {0};
		if (!DeviceStatus(2, szStatus)) {

			_ErrCnt++;
			_WorkSta.StrFlow = L"发卡机状态查询失败:" + GetLastErrS() + "," +
				Now().TimeString();
			SendLed(0, L"发卡机状态查询失败:" + GetLastErrS());
			Sleep(3000);
			return;
		}
		// ---分析发卡机状态
		String strStatus = DevStatusDecode(szStatus);
		if (strStatus != L"卡箱预空" && strStatus != L"卡片在传感器3" && strStatus !=
			L"卡片在传感器1-2(读卡)") {

			_ErrCnt++;
			_WorkSta.StrFlow = L"发卡机未就绪:" + strStatus + L"," +
				Now().TimeString();
			SendLed(0, L"发卡机未就绪:" + strStatus);
			SendICCard(2, 2);
			Sleep(3000);
			return;
		}
	}

	if (5 == _sysInfo.NodeID) { // ---自助出厂

		_Sequence = 0;
		// ---查询收卡装置的状态
		BYTE szStatus[6] = {0};
		if (!DeviceStatus(0, szStatus)) {

			_ErrCnt++;
			_WorkSta.StrFlow = L"收卡机状态查询失败:" + GetLastErrS() + "," +
				Now().TimeString();
			Sleep(3000);
			return;
		}
		String strStatus = DevStatusDecode_D3001(szStatus);
		// if(strStatus !=L"正常"){
		if (strStatus != L"正常") {
			if (strStatus == L"卡到位") {
				ReceiveICCard(0, 1);   //---收卡到票箱
				_WorkSta.StrFlow = L"收卡到票箱," + Now().TimeString();
			}
			else {
				ReceiveICCard(0, 6); // ---复位
				_ErrCnt++;
				_WorkSta.StrFlow = L"收卡机未就绪:" + strStatus + "," +
					Now().TimeString();
			}
			Sleep(3000);
			return;
		}
	}
	_ErrCnt = 0;
}

/*
 自助入厂
 _Sequence 0:扫描RFID中 1:读取车辆信息中 2:判断车辆状态中
 3:等待司机确认中 4:等待司机确认矿源信息  5:进入发卡状态
 6:取消发卡 7:完成发卡
 */
void __fastcall TServAutoR::AutoProc_ZZRC() {
	_Sequence = 0; // ---扫描RFID中

	if (_ErrCnt != 0)
		return; // ---有故障

	SendLed(0, _sysInfo.Location);

	_WorkSta.Car = "";
	_WorkSta.CardID = "";

	TCardInfo CardInfo;
	String strRFID, strNumplate; // ---RFID卡号,车牌号
	_WorkSta.StrFlow = L"正在读RFID卡," + Now().TimeString();

	bool DeBug = false;
	if (!DeBug) {
		if (!ReadCardID(0, CardInfo)) {

			return;
		}
		_Sequence = 1; // ---读取车辆信息中
		strRFID = CardInfo.CardID[0];
	}
	else {
		_Sequence = 1; // ---读取车辆信息中
		strRFID = L"E2003020250B012617805F85";
	}
	_WorkSta.CardID = strRFID;
	_WorkSta.StrFlow = L"正在读取车辆信息," + Now().TimeString();
	TCarInfo CarInfo;
	if (!GetCarInfo_pt(strRFID, CarInfo)) {

		_WorkSta.StrFlow = L"车辆信息读取失败,RFID卡号:" + strRFID + L",错误信息" +
			GetLastErrS() + L"," + Now().TimeString();
		return;
	}
	strNumplate = CarInfo.NumPlate;
	_WorkSta.Car = strNumplate;
	SendLed(0, strNumplate);
	Sleep(3000);

	_Sequence = 2; // ---判断车辆信息中
	if (1 == CarInfo.IsLock) { // ---车辆被锁

		_WorkSta.StrFlow = L"车辆被锁," + Now().TimeString();
		SendLed(0, strNumplate + L" 车辆被锁");
		::Sleep(100);
		_File.PlayWav(L"锁车.wav");
		Sleep(5000);
		return;
	}

	int currFLowID;
	UTF8String FlowDescription; // ---业务描述
	if (!GetCarFlowID(strNumplate, currFLowID, FlowDescription)) {

		_WorkSta.StrFlow = L"读车辆流程错误," + Now().TimeString();
		SendLed(0, strNumplate + L" 读取车辆流程错误");
		Sleep(5000);
		return;
	}

	if (currFLowID != _FLowID[0]) {

		if (currFLowID < _FLowID[0]) {
			_WorkSta.StrFlow = L"流程不符:未" + FlowDescription + L" " +
				Now().TimeString();
			SendLed(0, strNumplate + L" 流程不符:未" + FlowDescription);
		}
		else {
			_WorkSta.StrFlow = L"流程不符:应" + FlowDescription + L" " +
				Now().TimeString();
			SendLed(0, strNumplate + L" 流程不符:应" + FlowDescription);
		}
		Sleep(5000);
		return;
	}
	SendLed(0, strNumplate + L" 请下车办卡");
	_File.PlayWav(L"请确认车辆信息.wav", SND_FILENAME | SND_SYNC);
	_Sequence = 3; // ---等待司机确认中
	while (3 == _Sequence) { // ---等待司机确认车辆信息或取消的循环

		_WorkSta.StrFlow = L"等待司机确认车辆信息," + Now().TimeString();

		Sleep(100);
	}
	if (_bReset) {
		_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
		_File.WriteLog(L"在司机确认车辆信息的过程中进行强制流程复位");
		_bReset = false;
		return;
	}
	while (4 == _Sequence) { // ---等待司机录入矿方信息或取消的循环

		_WorkSta.StrFlow = L"等待司机选择矿方信息," + Now().TimeString();
		Sleep(100);
	}
	if (_bReset) {
		_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
		_File.WriteLog(L"在司机选择矿方信息的过程中进行强制流程复位");
		_bReset = false;
		return;
	}
	// ---发卡
	if (5 == _Sequence) {

		_strFaKaTis = L"正在制卡中,请稍后。。。";
		_WorkSta.StrFlow = L"正在发卡," + Now().TimeString();
		_File.PlayWav(L"制卡中.wav");

		bool bResult;
		// ---发卡到读卡位置
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
				_File.WriteLog(L"在发卡到读卡机的过程中进行强制流程复位");
				_bReset = false;
				return;
			}
			if (!DeBug)
				bResult = SendICCard(2, 12);
			else
				bResult = true;

			if (!bResult)
				_WorkSta.StrFlow = L"发卡到读卡机时失败:" + GetLastErrS() + "," +
					Now().TimeString();
			else
				_WorkSta.StrFlow = L"成功发卡到读卡机," + Now().TimeString();
			Sleep(1000);
		}
		while (!bResult);

		// ---查询并分析发卡装置状态
		BYTE szStatus[4] = {0};
		String strStatus = "";
		do {
			if (!DeBug) {
				bResult = DeviceStatus(2, szStatus); // ---查询
				if (!bResult)
					_WorkSta.StrFlow = L"发卡机状态查询失败:" + GetLastErrS() + "," +
						Now().TimeString();
				else { // ---分析
					strStatus = DevStatusDecode(szStatus);
					_WorkSta.StrFlow = L"正在查询卡片状态(读卡位置),状态:" + strStatus + "," +
						Now().TimeString();
				}
			}
			else {
				bResult = true;
				strStatus = L"卡片在传感器1-2(读卡)";
			}
			if (_bReset) {
				// ---应收卡到传感器3,但实际设备无法收回
				_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
				_File.WriteLog(L"在检测卡到位的过程中进行强制流程复位");
				_bReset = false;
				return;
			}
			Sleep(1000);

		}
		while (strStatus != L"卡片在传感器1-2(读卡)"); // ---不确定啥状态可以读卡???

		// ---读卡
		TCardInfo CardInfo;
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
				_File.WriteLog(L"在读卡ID的过程中进行强制流程复位");
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
				_WorkSta.StrFlow = L"读IC卡失败:" + GetLastErrS() + "," +
					Now().TimeString();
			else
				_WorkSta.StrFlow = L"读IC卡成功," + Now().TimeString();
			Sleep(500);
		}
		while (!bResult);

		// ---写卡  6.4.22 Wuwenbin新增
		/* do{
		 if(_bReset){
		 _WorkSta.StrFlow =L"强制流程复位,"+Now().TimeString();
		 _File.WriteLog(L"在写IC卡的过程中进行强制流程复位");
		 _bReset =false;
		 return;
		 }
		 bResult =WriteICCard(2,1,0,"FFFFFFFFFFFF",AnsiString(strNumplate));
		 if(!bResult) {
		 _WorkSta.StrFlow =L"写IC卡失败:"+GetLastErrS()+","+Now().TimeString();
		 _File.WriteLog_ErrS(L"写IC卡失败:"+GetLastErrS());
		 }
		 else {
		 _WorkSta.StrFlow =L"写IC卡成功,"+Now().TimeString();
		 }
		 }while(!bResult);
		 */
		// ---调用接口IC卡和车进行绑定
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
				_File.WriteLog(L"在车卡绑定的过程中进行强制流程复位");
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
				_WorkSta.StrFlow = L"车卡(IC)绑定失败:" + GetLastErrS() + "," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"车卡(IC)绑定失败:" + GetLastErrS());
			}
			else {
				_WorkSta.StrFlow = L"车卡(IC)绑定成功," + Now().TimeString();
			}
			Sleep(500);
		}
		while (!bResult);

		// ---提交验票信息到平台
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
				_File.WriteLog(L"在提交验票信息的过程中进行强制流程复位");
				_bReset = false;
				return;
			}
			bResult = SubmitYanp_pt(strNumplate, _fKuangfl, _Meik, _Gongys,
				_Pinz); // ---16.4.22 Wuwenbin新增 fKuangfl参数
			if (!bResult)
				_WorkSta.StrFlow = L"数据提交失败:" + GetLastErrS() + "," +
					Now().TimeString();
			else
				_WorkSta.StrFlow = L"车卡(IC)数据提交成功," + Now().TimeString();
			Sleep(500);
		}
		while (!bResult);

		// ---发卡到取卡位置
		do {
			if (_bReset) {
				_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
				_File.WriteLog(L"在发卡到取卡口的过程中进行强制流程复位");
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
				_WorkSta.StrFlow = L"发卡到取卡口失败:" + GetLastErrS() + "," +
					Now().TimeString();
			else
				_WorkSta.StrFlow = L"成功发卡到取卡口," + Now().TimeString();
			Sleep(200);
		}
		while (!bResult);

		do { // ---检测卡是否到取卡口
			if (!DeBug) {
				bResult = DeviceStatus(2, szStatus); // ---查询
				if (!bResult)
					_WorkSta.StrFlow = L"发卡机状态查询失败:" + GetLastErrS() + "," +
						Now().TimeString();
				else { // ---分析
					strStatus = DevStatusDecode(szStatus);
					_WorkSta.StrFlow = L"正在查询卡片状态,状态:" + strStatus + "," +
						Now().TimeString();
				}
				if (_bReset) {
					// ---应收卡到传感器3,但实际设备无法收回
					_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
					_File.WriteLog(L"在检测卡到位(取卡口)的过程中进行强制流程复位");
					_bReset = false;
					return;
				}
			}
			else {
				bResult = true;
				strStatus = L"卡片在传感器1";
			}
			Sleep(200);
		}
		while (strStatus != L"卡片在传感器1");
		// ---万一还没来得急检测到状态,司机就拨卡,这样会会造成死循环??? while(strStatus !=L"卡片在传感器1")

		_strFaKaTis = L"请取走您的卡。。。";
		_Sequence = 7; // ---卡已到取卡口
		_File.PlayWav(L"请取卡.wav");
		do { // ---检测卡状态,等司机把卡拿走
			if (!DeBug) {

				bResult = DeviceStatus(2, szStatus); // ---查询
				if (!bResult)
					_WorkSta.StrFlow = L"发卡机状态查询失败:" + GetLastErrS() + "," +
						Now().TimeString();
				else { // ---分析
					strStatus = DevStatusDecode(szStatus);
					_WorkSta.StrFlow = L"正在查询卡片状态,状态:" + strStatus + "," +
						Now().TimeString();
				}
				/* if(_bReset){
				 //---应收卡到传感器3,但实际设备无法收回
				 _WorkSta.StrFlow =L"强制流程复位,"+Now().TimeString();
				 _File.WriteLog(L"在检测卡到位(取卡口)的过程中进行强制流程复位");
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
		while (strStatus == L"卡片在传感器1");

		_Sequence = 8; // ---完成发卡
	}
}

/*
 自助出厂
 _Sequence 0:等待司机点击取磅单 1:进入自动吸卡状态 2：卡到位
 3:通过卡号读车辆信息 4:读卡内信息  5:车号不一致，退回卡
 6:司机取回卡
 */
void __fastcall TServAutoR::AutoProc_ZZCC() {
	if (_ErrCnt != 0)
		return; // ---有故障

	_WorkSta.Car = "";
	_WorkSta.CardID = "";

	bool DeBug = false;
	TCardInfo CardInfo;
	 if (!DeBug) {

		 _WorkSta.StrFlow = L"正在关闭自动收卡," + Now().TimeString();
		 if (!ReceiveICCard(0, 5)) {

			 _WorkSta.StrFlow = L"关闭自动收卡出错：" + GetLastErrS() + "," +
			 Now().TimeString();
			 _File.WriteLog_ErrS(L"关闭自动收卡出错：" + GetLastErrS());
			 Sleep(2000);
			 return;
		 }
    }
	_WorkSta.StrFlow = L"等待选择取磅单," + Now().TimeString();
	while (0 == _Sequence) { // ---等待选择取磅单

		if (_bReset) {
			_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
			_File.WriteLog(L"设置自动收卡模式的过程中进行强制流程复位");
			_bReset = false;
			return;
		}
	}

	bool bResult;
	 _WorkSta.StrFlow = L"正在打开自动收卡," + Now().TimeString();
	 if (1 == _Sequence) { // ---自动吸卡

		 if (_bReset) {
			 _WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
			 _File.WriteLog(L"设置自动收卡模式的过程中进行强制流程复位");
			 _bReset = false;
			 return;
		 }

		 do {
			 bResult = ReceiveICCard(0, 4);
			 if (!bResult) {
				 _WorkSta.StrFlow = L"设置自动收卡模式失败:" + GetLastErrS() + "," +
				 Now().TimeString();
				 _File.WriteLog_ErrS(L"设置自动收卡模式失败:" + GetLastErrS());
			 }
			 Sleep(1000);
		 }
		 while (!bResult);

	 }

	// ---等待卡到位
	_WorkSta.StrFlow = L"正在等待卡到位," + Now().TimeString();
	while (true) {

		if (_bReset) {
			_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
			_File.WriteLog(L"等待卡到位的过程中进行强制流程复位");
			_bReset = false;
			return;
		}
		BYTE szStatus[6] = {0};
		if (!DeBug) {
			if (DeviceStatus(0, szStatus)) {

				String strStatus = DevStatusDecode_D3001(szStatus);
				if (strStatus == L"卡到位")
					break;
				_WorkSta.StrFlow = strStatus + L"," + Now().TimeString();
			}
			else {
				_WorkSta.StrFlow = + L"查询卡到位状态失败：" + GetLastErrS() + L"," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"查询卡到位状态失败:" + GetLastErrS());
			}
			ReceiveICCard(0, 6); // ---复位
			Sleep(1000);
		}
		else
			break;
	}

	_Sequence = 2; // ---卡到位，准备读卡
	_WorkSta.StrFlow = L"正在读取卡信息," + Now().TimeString();
	_File.PlayWav(L"读取卡信息.wav", SND_FILENAME | SND_SYNC);

	while (true) {

		if (_bReset) {
			_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
			_File.WriteLog(L"在读卡ID的过程中进行强制流程复位");
			_bReset = false;
			return;
		}
		if (!DeBug) {
			if (ReadCardID(2, CardInfo)) {
				_WorkSta.StrFlow = L"读卡成功," + Now().TimeString();
				break;
			}
			_WorkSta.StrFlow = L"读卡失败," + GetLastErrS() + L"," +
				Now().TimeString();
			if (!RetCardProc_ZZCC(L"读卡失败:" + GetLastErrS(), false, false))
				// 退卡到卡口
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

	// ---读车辆信息
	_WorkSta.StrFlow = L"正在通过卡号读取车辆信息," + Now().TimeString();
	_Sequence = 3; // ---通过卡号读取车辆信息
	TCarInfo CarInfo;
	while (true) {
		if (_bReset) {
			_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
			_File.WriteLog(L"在读取车辆信息的过程中进行强制流程复位");
			_bReset = false;
			return;
		}

		if (GetCarInfo_pt(CardID, CarInfo)) {

			_WorkSta.StrFlow = L"卡号取车辆信息成功," + Now().TimeString();
			_WorkSta.Car = CarInfo.NumPlate;
			break;
		}

		_WorkSta.StrFlow = L"卡号取车辆信息失败," + GetLastErrS() + L"," +
			Now().TimeString();
		_File.WriteLog_ErrS(L"卡号取车辆信息失败:" + GetLastErrS());
		if (!RetCardProc_ZZCC(L"卡号取车辆信息失败:" + GetLastErrS(), false, false))
			// 退卡到卡口
				return;
	}

	if (!DeBug) {
		// ---读取卡内数据
		_WorkSta.StrFlow = L"正在读取卡内信息," + Now().TimeString();
		_Sequence = 4; // ---通过卡号读取车辆信息
		while (true) {
			if (_bReset) {
				_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
				_File.WriteLog(L"在读取卡内信息的过程中进行强制流程复位");
				_bReset = false;
				return;
			}
			if (ReadICCard(2, CardInfo)) {

				_WorkSta.StrFlow = L"卡内信息读取成功," + Now().TimeString();
				break;
			}
			_WorkSta.StrFlow = L"卡内信息读取失败," + GetLastErrS() + L"," +
				Now().TimeString();
			_File.WriteLog_ErrS(L"卡内信息读取失败:" + GetLastErrS());
			if (!RetCardProc_ZZCC(L"卡内信息读取失败:" + GetLastErrS(), false,
				false)) // 退卡到卡口
					return;
		}

		// ---比较卡内车号和系统车号
		if (String((char *)CardInfo.Sector[1].Block[0]) != CarInfo.NumPlate)
		{ // ---车号不一致

			// ---将卡弹出,并等待司机取卡
			if (!RetCardProc_ZZCC(L"卡内车号与系统不符", false, false)) // 退卡到卡口
					return;
		}
	}
	// ---调用出票接口

	_Sequence = 7;
	double fKous(0), fKoug(0), fKouz(0), fKouqt(0);
	int nKoudNum;
	if (!DeBug) {
		try {
			nKoudNum = String((char *)CardInfo.Sector[1].Block[1]).ToInt();
			// ---扣吨次数
			for (int i = 0; i < nKoudNum; i++) { // ---整理扣吨数据

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
			// ---卡数据异常，应 退卡 提示司机取回卡
			// ---将卡弹出,并等待司机取卡
			_WorkSta.StrFlow = L"卡内扣吨信息异常," + e.Message + L"," +
				Now().TimeString();
			_File.WriteLog_ErrS(L"卡内扣吨信息异常:" + e.Message);
			if (!RetCardProc_ZZCC(L"卡内扣吨信息异常" + e.Message, false, false))
				// 退卡到卡口
					return;
		}
	}
	// ---调取平台准备打票

	_WorkSta.StrFlow = L"正在打印磅单," + Now().TimeString();
	TBangdInfo BangdInfo;

	if (!ChupData_pt(CarInfo.NumPlate, fKous, fKoug, fKouz, fKouqt, BangdInfo))
	{
		_WorkSta.StrFlow = L"调用出票接口失败：" + GetLastErrS() + "," +
			Now().TimeString();
		_File.WriteLog_ErrS(L"调用出票接口失败：" + GetLastErrS());
		if (!RetCardProc_ZZCC(L"调用出票接口失败：" + GetLastErrS(), false, false))
			// 退卡到卡口
				return;
	}
	if (!BingdingICCard_pt(CarInfo.NumPlate, "", 1)) {
		_WorkSta.StrFlow = L"车号IC卡解绑失败：" + GetLastErrS() + "," +
			Now().TimeString();
		_File.WriteLog_ErrS(L"车号IC卡解绑失败：" + GetLastErrS());
	}
	Sleep(1000);

	_WorkSta.StrFlow = L"正在打印磅单," + Now().TimeString();
	_File.PlayWav(L"正在打印磅单.wav", SND_FILENAME | SND_SYNC);
	PrintBangd(BangdInfo);
	Sleep(2500);
	_Sequence = 8; // ---磅单打印完毕
	_File.PlayWav(L"磅单打印完成.wav", SND_FILENAME | SND_SYNC);
	Sleep(1000);
	_Sequence = 9; // ---已取票
	// ---收卡到票箱
	_WorkSta.StrFlow = L"正在收卡到票箱," + Now().TimeString();

	ReceiveICCard(0, 1); // 收卡到票箱
	while (true) {
		if (_bReset) {
			_WorkSta.StrFlow = L"强制流程复位," + Now().TimeString();
			_File.WriteLog(L"等待卡到位的过程中进行强制流程复位");
			_bReset = false;
			return;
		}

		BYTE szStatus[6] = {0};
		if (DeviceStatus(0, szStatus)) {

			String strStatus = DevStatusDecode_D3001(szStatus);
			if (strStatus == L"正常")
				break;
			_WorkSta.StrFlow = strStatus + L"," + Now().TimeString();
		}
		else {
			_WorkSta.StrFlow = + L"查询卡到位状态失败：" + GetLastErrS() + L"," +
				Now().TimeString();
			_File.WriteLog_ErrS(L"查询卡到位状态失败:" + GetLastErrS());
		}
		ReceiveICCard(0, 6); // ---复位
		Sleep(1000);
	}
	_Sequence = 10; // ---已收卡到票箱
}

// ---停车场出入
void __fastcall TServAutoR::AutoProc_ParkingCR() {
	if (_ErrCnt != 0)
		return; // ---有故障

	bool Debug = false;

	SendLed(_sysInfo.TingID, _sysInfo.Location + (0 == _sysInfo.TingID ? L"入口" :
		L"出口"));

	_WorkSta.Car = "";
	_WorkSta.CardID = "";
	// ---扫描RFID中
	_WorkSta.StrRun = "运行中...";
	TCardInfo CardInfo;
	String strRFID, strNumplate, ICardID; // ---RFID卡号,车牌号,IC卡号
	_WorkSta.StrFlow = L"正在读RFID卡," + Now().TimeString();

	if (!Debug) {
		if (!ReadCardID(_sysInfo.TingID, CardInfo)) {

			return;
		}
		// ---读取车辆信息中
		_WorkSta.CardID = strRFID = CardInfo.CardID[0];
	}
	else {
		_WorkSta.CardID = strRFID = "E2003020250B012617805F85";
	}
	_WorkSta.StrFlow = L"正在读取车辆信息," + Now().TimeString();
	TCarInfo CarInfo;
	if (!GetCarInfo_pt(strRFID, CarInfo)) {

		_WorkSta.StrFlow = L"车辆信息读取失败,RFID卡号:" + strRFID + L",错误信息" +
			GetLastErrS() + L"," + Now().TimeString();
		return;
	}
	_WorkSta.Car = strNumplate = CarInfo.NumPlate;

	SendLed(_sysInfo.TingID, strNumplate);
	Sleep(3000);

	// ---判断车辆信息中
	if (1 == CarInfo.IsLock) { // ---车辆被锁

		FinishCRC(L"锁车.wav", strNumplate + L" 车辆被锁",
			L"车辆被锁," + Now().FormatString("hh:mm:ss"), strNumplate + L"^车辆被锁",
			false, false, _sysInfo.TingID);
		Sleep(5000);
		return;
	}

	int currFLowID;
	UTF8String FlowDescription; // ---业务描述
	if (!GetCarFlowID(strNumplate, currFLowID, FlowDescription)) {

		_WorkSta.StrFlow = L"读车辆流程错误," + Now().TimeString();
		SendLed(_sysInfo.TingID, strNumplate + L" 读取车辆流程错误");
		Sleep(5000);
		return;
	}

	int ChuRTag = -1; // ---出 和 入 的标记 0:入 1:出
	if (currFLowID == _FLowID[0])
		ChuRTag = 0;
	else if (currFLowID == _FLowID[1])
		ChuRTag = 1;

	if (-1 == ChuRTag) {

		if (currFLowID < _FLowID[0]) {
			FinishCRC(L"流程不符.wav", strNumplate + L" 流程不符:未" + FlowDescription,
				L"流程不符:未" + FlowDescription + "," + Now().FormatString
				("hh:mm:ss"), strNumplate + L"^流程不符:未" + FlowDescription, false,
				false, _sysInfo.TingID);
		}
		else if (currFLowID > _FLowID[1]) {
			FinishCRC(L"流程不符.wav", strNumplate + L" 流程不符:应" + FlowDescription,
				L"流程不符:应" + FlowDescription + "," + Now().FormatString
				("hh:mm:ss"), strNumplate + L"^流程不符:应" + FlowDescription, false,
				false, _sysInfo.TingID);
		}
		return;
	}
	switch (_sysInfo.TingID) {

	case 0: // ---入场
		if (1 == ChuRTag) {
			FinishCRC(L"已进入停车场.wav", strNumplate + L"该车已进入停车场",
				L"该车已进入停车场," + Now().FormatString("hh:mm:ss"),
				strNumplate + L"^该车已进入停车场", false, false, _sysInfo.TingID);
			return;
		}
		break;
	case 1: // ---出场
		if (0 == ChuRTag) {
			FinishCRC(L"未进入停车场.wav", strNumplate + L"该车还未进入停车场",
				L"该车还未进入停车场," + Now().FormatString("hh:mm:ss"),
				strNumplate + L"^该车还未进入停车场", false, false, _sysInfo.TingID);
			return;
		}
		break;
	}
	SendLed(_sysInfo.TingID, strNumplate + L" 请下车刷卡");
	_File.PlayWav(L"请刷卡.wav", SND_FILENAME | SND_SYNC);
	_WorkSta.StrFlow = "请刷卡," + Now().FormatString("hh:mm:ss");
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

		SendLed(_sysInfo.TingID, L"IC卡信息读取失败");
		_WorkSta.StrFlow = L"IC卡信息读取失败,IC卡号:" + ICardID + L",错误信息" +
			GetLastErrS() + L"," + Now().TimeString();
		Sleep(2000);
		return;
	}
	String Numplate = CarInfoIC.NumPlate;
	if (strNumplate != Numplate) {
		FinishCRC(L"车号与IC卡不一致.wav", strNumplate + L" 车号与IC卡不一致",
			L"车号与IC卡不一致:" + FlowDescription + "," + Now().FormatString
			("hh:mm:ss"), strNumplate + L"^车号与IC卡不一致:" + FlowDescription, false,
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
				_WorkSta.StrFlow = L"写IC卡车号失败:" + GetLastErrS() + "," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"写IC卡车号失败:" + GetLastErrS());
			}
			else {
				_WorkSta.StrFlow = L"写IC卡成功," + Now().TimeString();
			}
			bResult = WriteICCard(_sysInfo.TingID + 2, 1, 1, "FFFFFFFFFFFF",
				AnsiString(0));
			if (!bResult) {
				_WorkSta.StrFlow = L"置零IC卡扣吨次数失败:" + GetLastErrS() + "," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"置零IC卡扣吨次数失败:" + GetLastErrS());
			}
			else {
				_WorkSta.StrFlow = L"置零IC卡扣吨次数成功," + Now().TimeString();
			}
			bResult = WriteICCard(_sysInfo.TingID + 2, 1, 2, "FFFFFFFFFFFF",
				AnsiString(0));
			if (!bResult) {
				_WorkSta.StrFlow = L"置零IC卡扣吨总数失败:" + GetLastErrS() + "," +
					Now().TimeString();
				_File.WriteLog_ErrS(L"置零IC卡扣吨总数失败:" + GetLastErrS());
			}
			else {
				_WorkSta.StrFlow = L"置零IC卡扣吨总数成功," + Now().TimeString();
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
			String strLed = strNumplate + L" 请去往" +
				(0 == ChuRTag ? Station.Current_Station :
				Station.Next_Location);
			String strSta =
				L"验证成功,请去往" + (0 == ChuRTag ? Station.Current_Station :
				Station.Next_Location);
			String strList = strNumplate + L"^" + L"验证成功,请去往" +
				(0 == ChuRTag ? Station.Current_Station :
				Station.Next_Location);
			FinishCRC(0 == ChuRTag ? L"请进入停车场.wav" : L"请离开停车场.wav", strLed,
				strSta, strList, true, false, _sysInfo.TingID);
		}
		else {
			String strLed = strNumplate + L" 请去往指定区域";
			String strSta = L"验证成功,请去往指定区域";
			String strList = strNumplate + L"^" + L"验证成功,请去往指定区域";
			FinishCRC(0 == ChuRTag ? L"请进入停车场.wav" : L"请离开停车场.wav", strLed,
				strSta, strList, true, false, _sysInfo.TingID);
		}
	}
	else {
		String strLed = strNumplate + (0 == ChuRTag ? L"入场失败:" : L"离场失败:") +
			GetLastErrS();
		String strSta = (0 == ChuRTag ? L"入场失败:" : L"离场失败:") + GetLastErrS();
		String strList = strNumplate + L"^" + (0 == ChuRTag ? L"入场失败:" :
			L"离场失败:") + GetLastErrS();
		FinishCRC(L"请重试.wav", strLed, strSta, strList, false, false,
			_sysInfo.TingID);
	}

	/*
	 if(0==ChuRTag)
	 {
	 if(EnterExitParking_pt(strNumplate,_sysInfo.TingID,NextStation)){

	 String strLed =strNumplate+L"请去往"+NextStation;
	 String strSta =L"验证成功,请去往"+NextStation;
	 String strList=strNumplate+L"^"+L"验证成功,请去往"+NextStation;
	 FinishCRC(L"请进入停车场.wav",strLed,strSta,strList ,true,false,_sysInfo.TingID);
	 }else{
	 String strLed =strNumplate+L"入场失败:"+GetLastErrS();
	 String strSta =L"入场失败:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"入场失败:"+GetLastErrS();
	 FinishCRC(L"请重试.wav",strLed,strSta,strList ,false,false,_sysInfo.TingID);
	 }
	 }
	 else if(1==ChuRTag)
	 {
	 if(EnterExitParking_pt(strNumplate,_sysInfo.TingID,NextStation)){

	 String strLed =strNumplate+L"请去往"+NextStation;
	 String strSta =L"验证成功,请去往"+NextStation;
	 String strList=strNumplate+L"^"+L"验证成功,请去往"+NextStation;
	 FinishCRC(L"请离开停车场.wav",strLed,strSta,strList ,true,false,_sysInfo.TingID);
	 }else{
	 String strLed =strNumplate+L"离场失败:"+GetLastErrS();
	 String strSta =L"离场失败:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"离场失败:"+GetLastErrS();
	 FinishCRC(L"请重试.wav",strLed,strSta,strList ,false,false,_sysInfo.TingID);
	 }
	 }
	 */
	return;
}

void __fastcall TServAutoR::AutoProc_QCH() {
	_WorkSta.CardID = "";
	_WorkSta.Car = "";
	if (!_bInitial || _ErrCnt != 0) {

		_WorkSta.StrRun = "故障...";
		_WorkSta.StrFlow = "无法计量...";
		return; // ---未初始化
	}

	_WorkSta.CardID = "";
	_WorkSta.Car = "";
	_WorkSta.StrRun = "运行中...";
	_WorkSta.StrFlow = "等待计量...";
	SendLed(0, _sysInfo.Location);
	bool Debug = false;
	if (GetRealVal() < _sysInfo.TriggerWeight) { // ---未达到地磅触发重量

		HLDCtrl(GREEN, GREEN);
		return;
	}
	HLDCtrl(RED, RED);
	::Sleep(200);
	OutCtrl(DZ, HEAD, DOWN);

	SendLed(0, L"请刷卡");
	Sleep(100);
	_File.PlayWav(L"请刷卡.wav", SND_FILENAME | SND_SYNC);
	TCardInfo CardInfo;
	bool bResult;
	String ICardID;
	if (!Debug) {
		do {
			_WorkSta.StrFlow = L"请刷卡," + Now().FormatString("hh:mm:ss");
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
		_WorkSta.StrFlow = L"正在读取车辆信息," + Now().FormatString("hh:mm:ss");
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

		FinishProc(L"锁车.wav", Numplate + L" 车辆被锁",
			L"车辆被锁," + Now().FormatString("hh:mm:ss"), Numplate + L"^车辆被锁",
			false, true);
		return;
	}

	// ---取当前车的业务ID
	int CarFlowID;
	UTF8String strFlow; // ---业务描述
	do {
		_WorkSta.StrFlow = "正在读取车辆业务ID," + Now().FormatString("hh:mm:ss");
		bResult = GetCarFlowID(Numplate, CarFlowID, strFlow);
		if (!bResult) {
			SendLed(0, "业务ID读取失败");
		}
		Sleep(2000);
	}
	while (!bResult);

	int JilTag = -1; // ---重 和 轻 的标记 0:重 1:轻
	if (CarFlowID == _FLowID[0])
		JilTag = 0;
	else if (CarFlowID == _FLowID[1])
		JilTag = 1;

	if (-1 == JilTag) {

		FinishProc(L"流程不符.wav", Numplate + L" 流程不符:应" + strFlow,
			L"流程不符:应" + strFlow + "," + Now().FormatString("hh:mm:ss"),
			Numplate + L"^流程不符:应" + strFlow, false, true);
		return;
	}
	switch (_sysInfo.QCHMode) {

	case 1: // ---重磅
		if (1 == JilTag) {
			FinishProc(L"请去轻车衡.wav", Numplate + L" 请去轻车衡",
				L"应去轻车衡," + Now().FormatString("hh:mm:ss"),
				Numplate + L"^应去轻车衡", false, true);
			return;
		}
		break;
	case 2: // ---轻磅
		if (0 == JilTag) {
			FinishProc(L"请去重车衡.wav", Numplate + L" 请去重车衡",
				L"应去重车衡," + Now().FormatString("hh:mm:ss"),
				Numplate + L"^应去重车衡", false, true);
			return;
		}
		break;
	}

	SendLed(0, Numplate + L" 重量 " + Format("%.2f",
		ARRAYOFCONST((GetRealVal()*0.001))));
	Sleep(2000);
	if (!DWQCheckProc(0, Numplate))
		return; // ---定位检查过程
	SendLed(0, Numplate + L" 重量 " + Format("%.2f",
		ARRAYOFCONST((GetRealVal()*0.001))));
	Sleep(2000);

	_WorkSta.StrFlow = L"正在取稳定值," + Now().FormatString("hh:mm:ss");
	double fWeight = 0;
	do {
		bResult = GetStaticVal(fWeight);
		if (!bResult) {
			_WorkSta.StrFlow = L"取稳定值失败," + _LastErrS + "," + Now().FormatString
				("hh:mm:ss");
		}
		::Sleep(100);
	}
	while (!bResult);

	_WorkSta.StrFlow = L"正在提交计量数据...";
	::Sleep(2000);

	TStation Station;

	if (0 == JilTag) {

		if (fWeight < _sysInfo.MinMZ) {
			String strLed = Numplate + L" 毛重过低,下限:" + String(_sysInfo.MinMZ)
				+ L"Kg";
			String strSta = L"毛重过低," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"毛重过低,重量:" + String(fWeight) +
				L"Kg。毛重下限:" + String(_sysInfo.MinMZ) + L"Kg ";
			FinishProc(L"毛重过低.wav", strLed, strSta, strList, false, true);
			// ---单向磅异常时，不抬前杆
			return;
		}
		if (fWeight > 120000) {
			String strLed = Numplate + L"毛重超限,上限:120000Kg";
			String strSta = L"毛重超限," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"毛重超限,重量:" + String(fWeight) +
				"Kg。毛重上限:120000Kg ";
			FinishProc(L"毛重超限.wav", strLed, strSta, strList, false, true);
			// ---单向磅异常时，不抬前杆
			return;
		}

		if (SubmitJilInfo_pt(Numplate, fWeight * 0.001, 0)) {
			if (GetNextStation(Numplate, Station)) {
				String strLed = Numplate + L"称重成功" + String(fWeight * 0.001) +
					L"请去往" + Station.Next_Location + Station.Next_Station;
				String strSta = L"称重成功," + String(fWeight * 0.001) + L"T,请去往" +
					Station.Next_Location + Station.Next_Station;
				String strList = Numplate + L"^" + L"称重成功!" +
					String(fWeight * 0.001) + L"吨,请去往" + Station.Next_Location +
					Station.Next_Station;
				FinishProc(L"称重成功.wav", strLed, strSta, strList, true, true);
				// ---单向磅异常时，不抬前杆
			}
			else {
				String strLed = Numplate + L"称重成功" + String(fWeight * 0.001) +
					L"请去往指定卸煤区域";
				String strSta = L"称重成功," + String(fWeight * 0.001) +
					L"T,请去往指定卸煤区域";
				String strList = Numplate + L"^" + L"称重成功!" +
					String(fWeight * 0.001) + L"吨,请去往指定卸煤区域";
				FinishProc(L"称重成功.wav", strLed, strSta, strList, true, true);
				// ---单向磅异常时，不抬前杆
			}
		}
		else {
			String strLed = Numplate + L"称重失败" + String(fWeight * 0.001) +
				L"T " + GetLastErrS();
			String strSta = L"称重失败," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"称重失败:" + GetLastErrS() +
				L",重量:" + String(fWeight * 0.001) + L"吨";
			FinishProc(L"称重失败.wav", strLed, strSta, strList, false, true);
			// ---单向磅异常时，不抬前杆
		}
		return;

	}
	else if (1 == JilTag) {

		if (fWeight < _sysInfo.MinPZ || fWeight > _sysInfo.MaxPZ) {
			String strLed = Numplate + L"皮重超限,重量:" + String(fWeight) +
				L"Kg。下限:" + String(_sysInfo.MinPZ) + L"Kg,上限:" +
				String(_sysInfo.MaxPZ) + L"Kg";
			String strSta = L"皮重超限," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"皮重超限,重量:" + String(fWeight) +
				L"Kg。下限:" + String(_sysInfo.MinPZ) + L"Kg,上限:" +
				String(_sysInfo.MaxPZ) + L"Kg";
			FinishProc(L"皮重超限.wav", strLed, strSta, strList, false, true);
			// ---单向磅异常时，不抬前杆
			return;
		}

		if (SubmitJilInfo_pt(Numplate, fWeight * 0.001, 1)) {
			if (GetNextStation(Numplate, Station)) {
				String strLed = Numplate + L"回皮成功" + String(fWeight * 0.001); ;
				String strSta = L"回皮成功," + String(fWeight * 0.001) + "T";
				String strList = Numplate + L"^" + L"回皮成功!" +
					String(fWeight * 0.001) + L"吨";
				FinishProc(L"回皮成功.wav", strLed, strSta, strList, true, true);
				// ---单向磅异常时，不抬前杆
			}
		}
		else {
			String strLed = Numplate + L"回皮失败" + String(fWeight * 0.001) +
				L"T " + GetLastErrS(); ;
			String strSta = L"回皮失败," + String(fWeight * 0.001) + "T";
			String strList = Numplate + L"^" + L"回皮失败:" + GetLastErrS() +
				L",重量:" + String(fWeight * 0.001) + L"吨";
			FinishProc(L"回皮失败.wav", strLed, strSta, strList, false, true);
			// ---单向磅异常时，不抬前杆
		}
		return;
	}
}

// ---煤场口出入
void __fastcall TServAutoR::AutoProc_MCCR() {
	if (_ErrCnt != 0)
		return; // ---有故障
	// _sysInfo.TingID  0入口 1出口 2仅煤沟
	if (_sysInfo.TingID < 2) {
		SendLed(1 == _sysInfo.TingID ? 1 : 0,
			_sysInfo.Location + (0 == _sysInfo.TingID ? L"入口" : L"出口"));
	}
	else {
		SendLed(1 == _sysInfo.TingID ? 1 : 0, _sysInfo.Location);
	}
	_WorkSta.Car = "";
	_WorkSta.CardID = "";
	_WorkSta.StrRun = "运行中...";
	// _WorkSta.StrFlow ="等待刷卡...";
	TCardInfo CardInfo;
	String ICardID, strNumplate; // ---车牌号
	bool Debug = false;
	// ---扫描IC中
	_WorkSta.StrFlow = L"正在读IC卡," + Now().TimeString();
	if (!Debug) {
		if (!ReadCardID((1 == _sysInfo.TingID ? 1 : 0) + 2, CardInfo)) {
			return;
		}
		_WorkSta.CardID = ICardID = String((char *)CardInfo.ICCardID_Hex);
	}
	else {
		_WorkSta.CardID = ICardID = "A20AA8161608040001C4D41AA9D9791D";
	}
	_WorkSta.StrFlow = L"正在读取车辆信息," + Now().TimeString();
	TCarInfo CarInfo;
	if (!GetCarInfo_pt(ICardID, CarInfo)) {

		_WorkSta.StrFlow = L"车辆信息读取失败,IC卡号:" + ICardID + L",错误信息" +
			GetLastErrS() + L"," + Now().TimeString();
		SendLed(1 == _sysInfo.TingID ? 1 : 0, L"车辆信息读取失败," + GetLastErrS());
		return;
	}
	_WorkSta.Car = strNumplate = CarInfo.NumPlate;
	SendLed(1 == _sysInfo.TingID ? 1 : 0, strNumplate);
	Sleep(3000);

	// ---判断车辆信息中
	if (1 == CarInfo.IsLock) { // ---车辆被锁
		FinishCRC(L"锁车.wav", strNumplate + L"车辆被锁",
			L"车辆被锁," + Now().FormatString("hh:mm:ss"), strNumplate + L"^车辆被锁",
			false, false, 1 == _sysInfo.TingID ? 1 : 0);
		return;
	}

	int currFLowID;
	UTF8String FlowDescription; // ---业务描述
	if (!GetCarFlowID(strNumplate, currFLowID, FlowDescription)) {

		_WorkSta.StrFlow = L"读车辆流程错误," + GetLastErrS() + Now().TimeString();
		SendLed(1 == _sysInfo.TingID ? 1 : 0, strNumplate + L" 读取车辆流程错误" +
			GetLastErrS());
		Sleep(5000);
		return;
	}

	int ChuRTag = -1; // ---出 和 入 的标记 0:入 1:出
	if (currFLowID == _FLowID[0])
		ChuRTag = 0;
	else if (currFLowID == _FLowID[1])
		ChuRTag = 1;

	if (-1 == ChuRTag) {

		if (currFLowID < _FLowID[0]) {
			FinishCRC(L"流程不符.wav", strNumplate + L"流程不符:未" + FlowDescription,
				L"流程不符:未" + FlowDescription + "," + Now().FormatString
				("hh:mm:ss"), strNumplate + L"^流程不符:未" + FlowDescription, false,
				false, 1 == _sysInfo.TingID ? 1 : 0);
		}
		else if (currFLowID > _FLowID[1]) {
			FinishCRC(L"流程不符.wav", strNumplate + L"流程不符:应" + FlowDescription,
				L"流程不符:应" + FlowDescription + "," + Now().FormatString
				("hh:mm:ss"), strNumplate + L"^流程不符:应" + FlowDescription, false,
				false, 1 == _sysInfo.TingID ? 1 : 0);
		}
		return;
	}
	switch (_sysInfo.TingID) {

	case 0: // ---入场
		if (1 == ChuRTag) {
			FinishCRC(L"已进入煤场.wav", strNumplate + L"该车已进入煤场",
				L"该车已进入煤场," + Now().FormatString("hh:mm:ss"),
				strNumplate + L"^该车已进入煤场", false, true,
				1 == _sysInfo.TingID ? 1 : 0);
			return;
		}
		break;
	case 1: // ---出场
		if (0 == ChuRTag) {
			FinishCRC(L"未进入煤场.wav", strNumplate + L"该车还未进入煤场",
				L"该车还未进入煤场," + Now().FormatString("hh:mm:ss"),
				strNumplate + L"^该车还未进入煤场", false, true,
				1 == _sysInfo.TingID ? 1 : 0);
			return;
		}
		break;
	}
	TStation Station;

	if (!GetNextStation(strNumplate, Station)) {
		_WorkSta.StrFlow = L"获取车辆流程环节出错," + GetLastErrS() + Now().TimeString();
		SendLed(1 == _sysInfo.TingID ? 1 : 0, strNumplate + L" 获取车辆流程环节出错" +
			GetLastErrS());
		Sleep(5000);
		return;
	}
	if (_sysInfo.Location.Pos(Station.Next_Location) <
		1 && Station.Next_Location.Length() > 0) {

		_WorkSta.StrFlow = L"未到达指定区域,应去往" + Station.Next_Location +
			Now().TimeString();
		SendLed(1 == _sysInfo.TingID ? 1 : 0, strNumplate + L" 未到达指定区域,应去往" +
			Station.Next_Location);
		Sleep(5000);
		return;
	}
	UTF8String TmpLocation = Station.Next_Location;
	UTF8String TmpNextStation, TmpCurrentStation;

	// if(0==ChuRTag){
	if (_sysInfo.TingID < 2) { // 煤场
		if (EnterExitMC_pt(strNumplate, _sysInfo.TingID, TmpLocation)) {
			if (GetNextStation(strNumplate, Station)) {
				String strLed = strNumplate + L"请去往" +
					(1 == ChuRTag ? Station.Next_Location :
					Station.Current_Station);
				String strSta =
					L"验证成功,请去往" + (1 == ChuRTag ? Station.Next_Location :
					Station.Current_Station);
				String strList = strNumplate + L"^" + L"验证成功,请去往" +
					(1 == ChuRTag ? Station.Next_Location :
					Station.Current_Station);
				FinishCRC(L"请离开煤场.wav", strLed, strSta, strList, false, false,
					1 == _sysInfo.TingID ? 1 : 0);
			}
			else {
				String strLed = strNumplate + L"请去往轻车衡";
				String strSta = L"验证成功,请去往轻车衡" + GetLastErrS();
				String strList = strNumplate + L"^" + L"验证成功,请去往轻车衡" +
					GetLastErrS();
				FinishCRC(L"请离开煤场.wav", strLed, strSta, strList, false, false,
					1 == _sysInfo.TingID ? 1 : 0);
			}
		}
		else {
			String strLed = strNumplate + (0 == ChuRTag ? L"入场失败:" : L"离场失败:") +
				GetLastErrS();
			String strSta = (0 == ChuRTag ? L"入场失败:" : L"离场失败:") +
				GetLastErrS();
			String strList = strNumplate + L"^" + (0 == ChuRTag ? L"入场失败:" :
				L"离场失败:") + GetLastErrS();
			FinishCRC(L"请重试.wav", strLed, strSta, strList, false, false,
				1 == _sysInfo.TingID ? 1 : 0);
		}
	}
	else { // 煤沟

		if (EnterExitMC_pt(strNumplate, (1 == _sysInfo.TingID ? 1 : 0),
			TmpLocation)) {
			if (GetNextStation(strNumplate, Station)) {
				TmpCurrentStation = Station.Current_Station;
			}
			else {
				TmpCurrentStation = L"汽车卸煤沟";
			}
		}
		else {
			String strLed = strNumplate + L"入场失败:" + GetLastErrS();
			String strSta = L"入场失败:" + GetLastErrS();
			String strList = strNumplate + L"^" + L"入场失败:" + GetLastErrS();
			FinishCRC(L"请重试.wav", strLed, strSta, strList, false, false,
				1 == _sysInfo.TingID ? 1 : 0);
			return;
		}
		if (EnterExitMC_pt(strNumplate, (0 == _sysInfo.TingID ? 0 : 1),
			TmpLocation)) {
			if (GetNextStation(strNumplate, Station)) {
				TmpNextStation = Station.Next_Location;
			}
			else {
				TmpNextStation = L"轻车衡";
			}
		}
		else {

			String strLed = strNumplate + L"离场失败:" + GetLastErrS();
			String strSta = L"离场失败:" + GetLastErrS();
			String strList = strNumplate + L"^" + L"离场失败:" + GetLastErrS();
			FinishCRC(L"请重试.wav", strLed, strSta, strList, false, false,
				1 == _sysInfo.TingID ? 1 : 0);
			return;
		}
		String strLed = strNumplate + L"请去往" + TmpCurrentStation + L"卸煤,卸煤后去往" +
			TmpNextStation;
		String strSta = L"验证成功,请去往" + TmpCurrentStation + L"卸煤,卸煤后去往" +
			TmpNextStation;
		String strList = strNumplate + L"^" + L"验证成功,请去往" + TmpCurrentStation +
			L"卸煤,卸煤后去往" + TmpNextStation;
		FinishCRC(L"请离开煤场.wav", strLed, strSta, strList, false, false,
			1 == _sysInfo.TingID ? 1 : 0);
	}

	/*
	 }
	 else if(1==ChuRTag)
	 {
	 if(_sysInfo.TingID < 2){  //煤场
	 if(EnterExitMC_pt(strNumplate,_sysInfo.TingID)){

	 String strLed =strNumplate+L"请去往"+NextStation;
	 String strSta =L"验证成功,请去往"+NextStation;
	 String strList=strNumplate+L"^"+L"验证成功,请去往"+NextStation;
	 FinishCRC(L"请离开煤场.wav",strLed,strSta,strList ,true,false,1==_sysInfo.TingID ? 1:0);
	 }else{
	 String strLed =strNumplate+L"离场失败:"+GetLastErrS();
	 String strSta =L"离场失败:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"离场失败:"+GetLastErrS();
	 FinishCRC(L"请重试.wav",strLed,strSta,strList ,false,false,1==_sysInfo.TingID ? 1:0);
	 }
	 }
	 else{   //煤沟
	 if(!EnterExitMC_pt(strNumplate,1==_sysInfo.TingID ? 1:0)){
	 String strLed =strNumplate+L"入场失败:"+GetLastErrS();
	 String strSta =L"入场失败:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"入场失败:"+GetLastErrS();
	 FinishCRC(L"请重试.wav",strLed,strSta,strList ,false,false,1==_sysInfo.TingID ? 1:0);
	 }else if(!EnterExitMC_pt(strNumplate,0==_sysInfo.TingID ? 0:1)){
	 String strLed =strNumplate+L"离场失败:"+GetLastErrS();
	 String strSta =L"离场失败:"+GetLastErrS();
	 String strList=strNumplate+L"^"+L"离场失败:"+GetLastErrS();
	 FinishCRC(L"请重试.wav",strLed,strSta,strList ,false,false,1==_sysInfo.TingID ? 1:0);
	 }
	 else{
	 String strLed =strNumplate+L"请去往"+NextStation;
	 String strSta =L"验证成功,请去往"+NextStation;
	 String strList=strNumplate+L"^"+L"验证成功,请去往"+NextStation;
	 FinishCRC(L"请离开煤场.wav",strLed,strSta,strList ,false,false,1==_sysInfo.TingID ? 1:0);
	 }
	 }
	 }
	 */
	return;
}

// ---采样机接口
void __fastcall TServAutoR::AutoProc_CYJ() {

	if (_bRead) {
		_WorkSta.CardID = "";
		_WorkSta.Car = "";
		if (_ErrCnt != 0)
			return; // ---有故障

		SendLed(0, _sysInfo.Location);
	}
	// ---读取采样机状态
	_WorkSta.StrFlow = L"正在查询采样机状态," + Now().TimeString();
	::Sleep(1000);
	int nStatus;
	if (!SamStaInfo(_connKYC, _sysInfo.CYJNO, nStatus)) {

		_WorkSta.StrFlow = L"采样机状态查询失败:" + GetLastErrS() + L"," +
			Now().TimeString();
		::Sleep(3000);
		return;
	}
	_WorkSta.StrFlow = L"采样机状态:" + CYJStaDecode(nStatus) + L"," +
		Now().TimeString();
	_WorkSta.StrRun = CYJStaDecode(nStatus);
	GetRealTimeCaiYInfo(_sysInfo.CYJNO); // 采样机大车小车实时数据显示
	TCardInfo CardInfo;
	if (QueryIsStop(CarNumber)) { // 中途采样机故障暂停

		_WorkSta.StrFlow = L"中途暂停,请刷IC卡继续采样:" + CYJStaDecode(nStatus) + L"," +
			Now().TimeString();
		_File.WriteLog(CarNumber + "中途暂停,请刷IC卡，继续采样");
		_File.PlayWav(L"中途暂停.wav");
		SendLed(0, CarNumber + "中途暂停,请刷IC卡，继续采样");

		_WorkSta.Car = CarNumber;
		::Sleep(3000);
		if (!ReadCardID(2, CardInfo)) {
			_WorkSta.StrFlow = L"刷IC卡失败:" + GetLastErrS() + L"," +
				Now().TimeString();
			Sleep(300);
			return;
		}
		else if (String((char *)CardInfo.ICCardID_Hex) == StrICID) {
			_WorkSta.StrFlow = L"刷卡成功，继续采样";
			SendLed(0, CarNumber + "刷卡成功，继续采样");
			::Sleep(3000);
			if (!UpdateTagInfo(_sysInfo.CYJNO)) { // 更新控制表标志位
				_WorkSta.StrFlow = L"更新控制表标志位失败:" + GetLastErrS() + L"," +
					Now().TimeString();
				_File.WriteLog(CarNumber + L"更新控制表标志位失败:" + GetLastErrS() +
					L"," + Now().TimeString());
				Sleep(300);
				return;
			}
			_WorkSta.StrFlow = L"更新控制表标志位成功，继续采样";
			SendLed(0, CarNumber + "正在采样");
			::Sleep(3000);
		}
		else {
			_WorkSta.StrFlow = "车号与IC卡不一致";
			SendLed(0, CarNumber + "车号与IC卡不一致");
			_File.PlayWav(L"车号与IC卡不一致.wav");
			::Sleep(3000);
			return;
		}
	}
	TCYZBInfo CYZBInfo;
	GetCaiYZBInfo(CYZBInfo);  //-------检测采样准备状态，实时更新到平台画面
    _WorkSta.StrFlow = "正在检测采样记录信息是否完整...";
	::Sleep(500);
	CarSamplingData CayInfo = {0};
	if (GetCyComplete(CayInfo)) {

		_WorkSta.StrFlow = "正在上传采样记录";
		if (SubmitCaiYRecord_pt(CayInfo)) { // ---提交采样详细信息记录
			_WorkSta.StrFlow = "提交采样记录成功";
			_File.WriteLog(_WorkSta.StrFlow);
			::Sleep(1000);
		}else{
			_WorkSta.StrFlow = "提交采样记录失败" + GetLastErrS();
			_File.WriteLog(_WorkSta.StrFlow);
			::Sleep(800);
		}
	}else{
		_WorkSta.StrFlow = "无采样完成记录" + GetLastErrS(); // ---采样完成记录信息不完整
		::Sleep(200);
	}

    // ---检测采样机集样器状态信息
	_WorkSta.StrFlow = "正在检测集样罐状态信息...";
	::Sleep(500);
	SamperCollectionData CollectInfo = {0};
	if (!GetCollectorInfo(CollectInfo)) {
		_WorkSta.StrFlow = "更新集样罐信息失败" + GetLastErrS(); // ---更新的集样罐信息失败
		::Sleep(200);
	}
	else {
		_WorkSta.StrFlow = "上传集样罐状态信息成功...";
		_File.WriteLog(_WorkSta.StrFlow);
		::Sleep(500);
	}
	// ---读卡
	if (0 == nStatus || 1 == nStatus || 2 == nStatus || 3 == nStatus ||
		4 == nStatus) { // ---这些状态下,不读卡

		Sleep(3000);
		return;
	}
	if (_bRead) {
		_WorkSta.StrFlow = L"正在读RFID卡," + Now().TimeString();
	   //	::Sleep(2000);
		if (!ReadCardID(0, CardInfo)) { // ---读卡失败

			_WorkSta.StrFlow = L"读RFID卡失败:" + GetLastErrS() + L"," +
				Now().TimeString();
			Sleep(1000);
			return;
		}
		StrRFID = CardInfo.CardID[0];
		// "E20020292513015428300297"; //  "E2003065961101100830C59C";

	}
	_WorkSta.CardID = StrRFID;
	// ---读取车辆信息
	_WorkSta.StrFlow = L"正在查询车辆信息," + Now().TimeString();
	TCarInfo CarInfo;
	CoInitialize(NULL);
	if (!GetCarInfo_pt(StrRFID, CarInfo)) {

	   /*
		FinishCRC(L"车辆查询失败.wav", StrRFID + L"车辆信息查询失败",
			L"车辆信息查询失败:" + GetLastErrS() + L"," + Now().TimeString(),
			L"车辆信息查询失败:" + GetLastErrS() + L"," + Now().TimeString(), false,
			false, 0);
	   */
	  //  _File.PlayWav(L"中途暂停.wav");
	  	_WorkSta.StrFlow = L"车辆信息查询失败,"+GetLastErrS() + L"," + Now().TimeString();
		SendLed(0,L"车辆信息查询失败");
		Sleep(3000);
		return;
	}
	_WorkSta.Car = CarNumber = CarInfo.NumPlate;
	StrICID = CarInfo.ICCardID;
	SendLed(0, CarInfo.NumPlate);
	Sleep(1000);
	// ---判断车辆状态
	_WorkSta.StrFlow = L"正在判断车辆状态," + Now().TimeString();
	if (1 == CarInfo.IsLock) {
		FinishCRC(L"锁车.wav", CarInfo.NumPlate + L"车辆被锁",
			CarInfo.NumPlate + L"车辆被锁", CarInfo.NumPlate + L"车辆被锁", false,
			false, 0);
		Sleep(2000);
		return;
	}
	int currFLowID;
	UTF8String FlowDescription; // ---业务描述
	if (!GetCarFlowID(CarInfo.NumPlate, currFLowID, FlowDescription)) {

		FinishCRC(L"读车辆流程错误.wav", CarInfo.NumPlate + L" 读取车辆流程错误",
			CarInfo.NumPlate + L" 读取车辆流程错误", CarInfo.NumPlate + GetLastErrS() +
			L"," + Now().TimeString(), false, false, 0);
		Sleep(3000);
		return;
	}
	if (currFLowID != _FLowID[0]) {
		FinishCRC(L"流程不符.wav", CarInfo.NumPlate + L"流程不符：应" + FlowDescription,
			CarInfo.NumPlate + L"流程不符：应" + FlowDescription+ Now().TimeString(),
			L"流程不符:应" + FlowDescription  , false,
			false, 0);
		Sleep(5000);
		 return;
	}
	TCaiyInfo CaiyInfo;
	if (!GetCarCYInfo(CarInfo.NumPlate, CaiyInfo)) {    //---根据车号获取采样编码信息、采样点数、供应商信息、采样类型
		/*
		FinishCRC(L"采样信息查询失败.wav", CarInfo.NumPlate + L"获取采样批次信息失败",
			CarInfo.NumPlate + L"获取采样批次信息失败:" + GetLastErrS() + L"," +
			Now().TimeString(), CarInfo.NumPlate + L"获取采样批次信息失败:" +
			GetLastErrS() + L"," + Now().TimeString(), false, false, 0);
		*/
		_File.PlayWav(L"采样信息查询失败.wav");
		_WorkSta.StrFlow = L"获取采样批次信息失败："+GetLastErrS() + L"," + Now().TimeString();
		SendLed(0,L"车辆信息查询失败");
		Sleep(3000);
		return;
	}

	if (_bRead) {
		//-----将采样编码和制样编码传入化验室库中
		if(!WritePICItoHuay(CaiyInfo)){
			_WorkSta.StrFlow = "写入制样编码和化验编码到化验库失败"+GetLastErrS();
			::Sleep(500);
		}
		// ---准备采样  ,抬杆，车辆准备进入
		_WorkSta.StrFlow = "准备采样";
		::Sleep(3000);
		_File.PlayWav(L"准备采样.wav");
		OutCtrl(DZ, TAIL, UP);
		HLDCtrl(RED, RED);
		::Sleep(5000);
	}

	// ---刷IC卡,开始采样
	if (bResult) {

		_WorkSta.StrFlow = "刷IC卡，开始采样";
		SendLed(0, CarInfo.NumPlate + "请刷IC卡，准备采样");
		::Sleep(7000);
		_File.PlayWav(L"请刷卡.wav", SND_FILENAME | SND_SYNC);
		_WorkSta.StrFlow = "请刷卡," + Now().FormatString("hh:mm:ss");
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
			_WorkSta.StrFlow = "IC卡信息不符";
			_File.PlayWav(L"车号与IC卡不一致.wav", SND_FILENAME | SND_SYNC);
			SendLed(0, CarInfo.NumPlate + "IC卡信息不符");
			_bRead = false;
			return;
		}
		SendLed(0, CarInfo.NumPlate + "刷卡成功，开始采样");
		_File.PlayWav(L"请刷成功.wav", SND_FILENAME | SND_SYNC);
		_WorkSta.StrFlow = "刷卡成功，开始采样";
		::Sleep(500);
		_WorkSta.StrFlow = "正在写入采样信息到中间库";
		::Sleep(3000);
		if (!WriteCarInfoToCYJ(CarInfo, CaiyInfo)) { // --写入采样信息到开元中间库

			_WorkSta.StrFlow = GetLastErrS();
			_File.PlayWav(L"采样失败.wav"); // ---采样失败
			::Sleep(100);
			::Sleep(2000);
			_File.WriteLog(CarInfo.NumPlate + GetLastErrS());
			_bRead = false;
			bResult = true;
			return;
		}
		SendLed(0, CarInfo.NumPlate + "正在采样");
		UpdateCaiYZTInfo_pt(_sysInfo.Location, CarInfo.NumPlate);
		// 推送采样机、车号到页面，通知设备监控界面更新车辆信息
		_bRead = false;
	}

	_WorkSta.StrFlow = "正在检测采样是否完成...";
	::Sleep(3000);

	// -------采样完成，车辆放行
	if (QueryCMDState(CarNumber)) {
		if (!SubmitCaiYInfo_pt(CarNumber)) {
			_WorkSta.StrFlow = "提交采样流程失败" + GetLastErrS();
			::Sleep(2000);
			_File.WriteLog(CarInfo.NumPlate + GetLastErrS());
			return;
		}
		String strLed = CarInfo.NumPlate + L"采样完成";
		String strSta = L"采样完成";
		String strList = CarInfo.NumPlate + L"^" + L"采样完成!";
		FinishCRC(L"采样完成.wav", strLed, strSta, strList, true, false, 0);
		HLDCtrl(GREEN, GREEN);
		::Sleep(3000);
		_WorkSta.Car = "";
		_WorkSta.CardID = "";
		bResult = true;
		_bRead = true;
		return;
	}

}

// ----采制化、斗轮机设备运行状态采集
void __fastcall TServAutoR::AutoProc_ZTCJ() {

	String SQL;
	String Table[8] = {"CY_ERR_Tb", "ZY_ERR_Tb", "UploadInfo","TB_TESTRESULT_6550","ERROR_INFO","SAMPLEREC","CABINET_INFO","DEVICE_STATUS"};
	String Data_State[7] = {"DataStatus", "DataStatus", "isupload","ISGETVALUE","RDFLAG","DATA_STATUS","DATA_STATUS"};
	String Time[7] = {"ErrorTime","ErrorTime","TestTime","STARTINGTIME","CreateTime","OpTime","DepositTime"};
	String data[7] = {"0","0","1","1","0","0","0"};
	::Sleep(2000);

	try {

		memoStr = "----" + Now().DateTimeString() + "  服务启动...";
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
							_LastErrS = "上传采样机运行状态信息失败:" + GetLastErrS();
							memoStr = "数据上传错误,采样机名称:" + cyj_code + ",时间:" +
								ErrTime + ",运行信息:" + ErrInfo + ",错误:" +
								_LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "上传,采样机名称:" + cyj_code + ",时间:" +
								ErrTime + ",运行信息:" + ErrInfo;
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
							_LastErrS = "上传制样机运行状态信息失败:" + GetLastErrS();
							memoStr = "数据上传错误,采样机名称:" + cyj_code + ",时间:" +
								ErrTime + ",运行信息:" + ErrInfo + ",错误:" +
								_LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();

						}
						else {
							memoStr = "上传,制样机名称:" + cyj_code + ",时间:" +
								ErrTime + ",运行信息:" + ErrInfo + ",错误代码:" +
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
							_LastErrS = "上传化验数据失败:" + GetLastErrS();
							memoStr = "化验数据上传错误,化验编号:" + HYSInfo.S_NO + ",时间:" +
								HYSInfo.TestTime + ",错误:" + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "上传,化验编号:" + HYSInfo.S_NO + ",时间:" +
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
							_LastErrS = "上传全水数据失败:" + GetLastErrS();
							memoStr = "全水数据上传错误,编号:" + QSYInfo.SAMPLENO + ",开始时间:" +
								QSYInfo.STARTINGTIME + ",错误:" + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "上传,化验编号:" + QSYInfo.SAMPLENO + ",开始时间:" +
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
							_LastErrS = "上传存样柜数据失败:" + GetLastErrS();
							memoStr = "存样柜数据上传错误,编号:" + CYGInfo.SAMPLEID + ",样品存入时间:" +
								CYGInfo.DEPOSITTIME + ",错误:" + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "上传,存样柜编号:" + CYGInfo.SAMPLEID + ",样品存入时间:" +
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
							_LastErrS = "上传存样柜存取数据失败:" + GetLastErrS();
							memoStr = "存样柜存取上传错误,编号:" + CYGCQInfo.SampleID + ",样品存取操作时间:" +
								CYGCQInfo.OpTime + ",错误:" + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "上传,存样柜存取编号:" + CYGCQInfo.SampleID + ",样品存取操作时间:" +
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
							_LastErrS = "上传存样柜运行记录失败:" + GetLastErrS();
							memoStr = "上传,采样机名称:" + cyj_code + ",时间:" +
								ErrTime + ",运行信息:" + ErrInfo + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "上传,设备名称:" + cyj_code + ",时间:" +
								ErrTime + ",运行信息:" + ErrInfo;
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
							_LastErrS = "上传气动管道信息失败:" + GetLastErrS();
							memoStr = "上传设备名称:"+QDGDInfo.DeviceName+",运行信息:" + QDGDInfo.LineStatus + _LastErrS;
							_File.WriteLog(memoStr);
							::Sleep(2000);
							CDS->Next();
						}
						else {
							memoStr = "上传,设备名称:" +QDGDInfo.DeviceName +",运行信息:" +QDGDInfo.LineStatus;
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
		_LastErrS = "无更新记录:" + e.Message;
		memoStr = "系统错误:" + e.Message;
		_File.WriteLog(memoStr);
		::Sleep(2000);
	}
	if (_bRead) {
		memoStr = "	---采集服务暂停,30s后重新启动";
		::Sleep(30000);
	}
}
