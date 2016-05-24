#ifndef voBangInfoH
#define voBangInfoH

#include "voDeviceInfo.h"
#include "voFuncInfo.h"
#include "voBaseInfo.h"

class TBangSysInfo {
public:
	TBangSysInfo() {
		Init();
	}

	void Init() {
		FPort = 0;
	}

public:
	__property WideString Location = {read = FLocation, write = FLocation};
	__property int RunFlag = {read = FRunFlag, write = FRunFlag};
	__property TDateTime BeginTime = {read = FBeginTime, write = FBeginTime};
	__property TDateTime EndTime = {read = FEndTime, write = FEndTime};
	__property int Port = {read = FPort, write = FPort};

private:
	WideString FLocation; // ---地磅名
	int FRunFlag; // ---0:可运行，1：时间段运行 2：停用

	// ---运行时间段 时分秒区间
	TDateTime FBeginTime;
	TDateTime FEndTime;
	int FPort;
};

class TBangSysFlowInfo {
public:
	TBangSysFlowInfo() {
		Init();
	}

	void Init() {
		FMainFlow = "";
		FRunMode = 0;
		FReadMode = 0;
		FTriggerWeight = 0;
		FHeadDZDownTpye = 0;
		FTailDZIsDown = false;
		FNeedCY = 0;
		FMaxPZ = 0;
		FMinPZ = 0;
		FMinMZ = 0;
	}

public:
	__property WideString MainFlow = {read = FMainFlow, write = FMainFlow
	}; // ---主流程
	__property int RunMode = {read = FRunMode, write = FRunMode
	}; // ---0:重磅 1：轻磅 2：轻重通用(不区分方向) 3:区分方向  主流程为"通用"时,起作用
	__property int ReadMode = {read = FReadMode, write = FReadMode
	}; // ---0:达到重量读卡,1:读卡后上磅   主流程为"通用"时,起作用
	__property int TriggerWeight = {
		read = FTriggerWeight, write = FTriggerWeight}; // ---单位Kg,达到此重量时触发地磅工作
	__property int HeadDZDownTpye = {
		read = FHeadDZDownTpye, write = FHeadDZDownTpye
	}; // ---前道闸落杆方式:0达到重量落杆 1读到车号后落杆
	__property bool TailDZIsDown = {read = FTailDZIsDown, write = FTailDZIsDown
	}; // ---上磅后,后置道闸是否落杆
	__property int NeedCY = {read = FNeedCY, write = FNeedCY
	}; // ---0:忽略 1:下磅采样 2:磅上等待采样
	__property int MaxPZ = {read = FMaxPZ, write = FMaxPZ}; // ---皮重上限
	__property int MinPZ = {read = FMinPZ, write = FMinPZ}; // ---皮重下限
	__property int MinMZ = {read = FMinMZ, write = FMinMZ}; // ---毛重下限

private:
	WideString FMainFlow;
	int FRunMode;
	int FReadMode;
	int FTriggerWeight;
	int FHeadDZDownTpye;
	bool FTailDZIsDown;
	int FNeedCY;
	int FMaxPZ;
	int FMinPZ;
	int FMinMZ;
};

// ---地磅配置信息
class TBangInfo : public TBangSysInfo {
public:
	__property TBangSysInfo BangSysInfo = {
		read = FBangSysInfo, write = FBangSysInfo};
	__property TBangSysFlowInfo BangSysFlowInfo = {
		read = FBangSysFlowInfo, write = FBangSysFlowInfo};
	__property TDBConfig DBConfig = {read = FDBConfig, write = FDBConfig};
	__property TMCFuncInfo MCFuncInfo = {read = TMCFuncInfo, write = TMCFuncInfo
	};
	__property TDeviceInfo DeviceInfo = {read = FDeviceInfo, write = FDeviceInfo
	};

private:
	TBangSysInfo FBangSysInfo;
	TBangSysFlowInfo FBangSysFlowInfo;
	TDBConfig FDBConfig;
	TMCFuncInfo TMCFuncInfo;
	TDeviceInfo FDeviceInfo;
};

#endif
