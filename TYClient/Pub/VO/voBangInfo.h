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
	WideString FLocation; // ---�ذ���
	int FRunFlag; // ---0:�����У�1��ʱ������� 2��ͣ��

	// ---����ʱ��� ʱ��������
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
	}; // ---������
	__property int RunMode = {read = FRunMode, write = FRunMode
	}; // ---0:�ذ� 1����� 2������ͨ��(�����ַ���) 3:���ַ���  ������Ϊ"ͨ��"ʱ,������
	__property int ReadMode = {read = FReadMode, write = FReadMode
	}; // ---0:�ﵽ��������,1:�������ϰ�   ������Ϊ"ͨ��"ʱ,������
	__property int TriggerWeight = {
		read = FTriggerWeight, write = FTriggerWeight}; // ---��λKg,�ﵽ������ʱ�����ذ�����
	__property int HeadDZDownTpye = {
		read = FHeadDZDownTpye, write = FHeadDZDownTpye
	}; // ---ǰ��բ��˷�ʽ:0�ﵽ������� 1�������ź����
	__property bool TailDZIsDown = {read = FTailDZIsDown, write = FTailDZIsDown
	}; // ---�ϰ���,���õ�բ�Ƿ����
	__property int NeedCY = {read = FNeedCY, write = FNeedCY
	}; // ---0:���� 1:�°����� 2:���ϵȴ�����
	__property int MaxPZ = {read = FMaxPZ, write = FMaxPZ}; // ---Ƥ������
	__property int MinPZ = {read = FMinPZ, write = FMinPZ}; // ---Ƥ������
	__property int MinMZ = {read = FMinMZ, write = FMinMZ}; // ---ë������

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

// ---�ذ�������Ϣ
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