#ifndef voDeviceInfoH
#define voDeviceInfoH

#include "voBaseInfo.h"

class TBalanceInfo : public TBaseDeviceInfo {
public:
	TBalanceInfo() {
		Init();
	}

	void Init() {
		Head_Tag = '\0';
		Tail_Tag = '\0';
		StartPos = 0;
		DataLen = 0;
		Zoom = 1;
		IsRev = false;
		CycleTime = 50;
		Cnt = 8;
		dwTimeOuts = 3000;
	}

public:
	unsigned char Head_Tag; // ---有效数据开头标志
	unsigned char Tail_Tag; // ---有效数据结尾标志
	int StartPos; // ---标志位开始后第N位为有效计量数据
	int DataLen; // ---有效数据长度
	double Zoom; // ---数据放大倍数
	bool IsRev; // ---数据是否倒置
	int CycleTime; // ---取稳定值的间隔时间,单位ms
	int Cnt; // ---取稳定值时的取值次数
	DWORD dwTimeOuts; // 每次收到仪表数据的时间限制  ms
};

class TReaderInfo : public TBaseDeviceInfo {
public:
	TReaderInfo() {
		Init();
	}

	void Init() {
		AntCnt = 0;
		memset(Ant, 0, sizeof(Ant));
	}

public:
	int AntCnt; // 天线数
	int Ant[8]; // ---Ant口号
};

// --- IO开头控制器,控制道闸 红绿灯 以及读取 定位器信号
class TIOCtrlInfo : public TBaseDeviceInfo { // ---打开或关闭控制器时一定要判断控制器状态

public:
	TIOCtrlInfo() {
		Init();
	}

	void Init() {
		UseDZ_Head = false;
		UseDZ_Tail = false;
		UseHLD_Head = false;
		UseHLD_Tail = false;
		UseDWQ = false;

		UpDZCmd_Head = "";
		DownDZCmd_Head = "";
		UpDZCmd_Tail = "";
		DownDZCmd_Tail = "";

		RedHLDCmd_Head = "";
		GreenHLDCmd_Head = "";
		CloseHLDCmd_Head = "";
		RedHLDCmd_Tail = "";
		GreenHLDCmd_Tail = "";
		CloseHLDCmd_Tail = "";

		SelDWQStaCmd = "";
		DWQMissEcho_Head = "";
		DWQMissEcho_Tail = "";
		DWQPassEcho_ALL = "";
		DWQMissEcho_ALL = "";

		dwTimeOuts = 2000;

		DZSta_Head = -1;
		DZSta_Tail = -1;
		HLDSta_Head = -1;
		HLDSta_Tail = -1;
		DWQSta_Head = -1;
		DWQSta_Tail = -1;
	}

public:
	bool UseDZ_Head; // ---是否使用前置道闸
	bool UseDZ_Tail; // ---是否使用后置道闸
	bool UseHLD_Head; // ---是否使用前置红绿灯
	bool UseHLD_Tail; // ---是否使用后置红绿灯
	bool UseDWQ; // ---是否使用前位器

	// ---指令：16进制的字符串，多条指令可用英文半角逗号隔开
	WideString UpDZCmd_Head; // --- 抬前杆命令
	WideString DownDZCmd_Head; // ---落前杆命令
	WideString UpDZCmd_Tail; // ---抬后杆命令
	WideString DownDZCmd_Tail; // ---落后杆命令

	WideString RedHLDCmd_Head; // ---前置红灯命令
	WideString GreenHLDCmd_Head; // ---前置绿灯命令
	WideString CloseHLDCmd_Head; // ---前置关灯命令
	WideString RedHLDCmd_Tail; // ---后置红灯命令
	WideString GreenHLDCmd_Tail; // ---后置绿灯命令
	WideString CloseHLDCmd_Tail; // ---后置关灯命令

	WideString SelDWQStaCmd; // ---查询定位器命令
	WideString DWQMissEcho_Head; // ---前定位遮挡的反馈
	WideString DWQMissEcho_Tail; // ---后定位遮挡的反馈
	WideString DWQPassEcho_ALL; // ---定位全通过的反馈
	WideString DWQMissEcho_ALL; // ---定位全折挡的反馈
	DWORD dwTimeOuts; // 每次收包的时间限制  ms

	// ---设备状态 0 up red,1 down green 2 err -1不存在
	int DZSta_Head;
	int DZSta_Tail;
	int HLDSta_Head;
	int HLDSta_Tail;
	int DWQSta_Head;
	int DWQSta_Tail;
};

// ---以下三个设备,由上面的统一IO开关类代替,暂不用
class TDZInfo : public TBaseDeviceInfo {
public:
private:
	// ---起 落 停 指令
	WideString UP_Cmd;
	WideString DOWN_Cmd;
	WideString STOP_Cmd;
};

class THLDInfo : public TBaseDeviceInfo {
public:
private:
	// ---红 绿 灭 指令
	WideString Red_Cmd;
	WideString Green_Cmd;
	WideString Close_Cmd;
};

class TDWQInfo : public TBaseDeviceInfo {
public:
private:
	// ---指令
	WideString sel_Cmd; // ---查询
	WideString FrontFail_Echo; // ---前置被挡反馈
	WideString BackFail_Echo; // ---后置被挡反馈
	WideString AllFail_Echo; // ---全部被挡反馈
	WideString AllPass_Echo; // ---全部通过反馈
};

class TLedInfo : public TBaseDeviceInfo {
public:
	TLedInfo() {
		Init();
	}

	void Init() {
		CardAddr = 1;
		Width = 0;
		Height = 0;
		FontSize = 10;
		SendMode = 0;
		ColorStyle = 0;
	}

public:
	int CardAddr; // ---控制卡地址(EQ:配置中从0开始,程序中加1)
	int Width; // ---LED宽
	int Height; // ---LED高
	int FontSize; // ---字号
	int SendMode; // ---0:节目 1：时实数据（发图片）
	int ColorStyle; // ---颜色模式,0单色,1双色
};

// ---设备信息汇总类
class TDeviceInfo {
public:
	TDeviceInfo() {
		Init();
	}

	void Init() {
		ReaderCnt = 0;
		LedCnt = 0;
		IoCtrlCnt = 0;
	}

public:
	// ---硬件数量
	int ReaderCnt; // ---现场最多2个主机实现双向	,或1主机2天线实现双向
	int LedCnt;
	int IoCtrlCnt;

	TBalanceInfo BalanceInfo;
	TReaderInfo ReaderInfo[4];
	TIOCtrlInfo IOCtrlInfo[8];
	TLedInfo LedInfo[4];
	TBaseDeviceInfo PrtInfo; // ---东华自助出厂机的打印机
};

#endif
