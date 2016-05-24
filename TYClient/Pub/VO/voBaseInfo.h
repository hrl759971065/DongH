#ifndef voBaseInfoH
#define voBaseInfoH

#include <Classes.hpp>
#include "voConst.h"

class THostInfo {
public:
	THostInfo() {
		Init();
	}

	void Init() {
		BaseBoardID = "";
		ProcessorID = "";
		HardDiskID = "";
	}

public:
	WideString BaseBoardID;
	WideString ProcessorID;
	WideString HardDiskID;
};

class TVersionInfo {
public:
	TVersionInfo() {
		Init();
	}

	void Init() {
		FilePathName = "";
		FileDescription = "";
		FileVersion = "";
	}

public:
	WideString FilePathName;
	WideString FileDescription;
	WideString FileVersion;
};

class TDBConfig {
public:
	TDBConfig() {
		Init();
	}

	void Init() {
		IsUse = true;
		Provider = "";
		User = "";
		Pass = "";
		Server = "";
		Database = "";
		Port = 0;
		IsPool = false;
		IsDirect = true;
		AccessType = 0;
	}

public:
	bool IsUse;
	WideString Provider;
	WideString User;
	WideString Pass;
	WideString Server;
	WideString Database;
	long Port;
	bool IsPool;
	bool IsDirect;
	int AccessType; // ---数据访问方式, 0:直接访问远程数据库 1:DataSnap 2:Rest 3:WebService
};

class TSysInfo { // ---主Exe程序信息

public:
	TSysInfo() {
		Init();
	}

	void Init() {
		SysID = "";
		SysName = "";
		LocalIP = "";
		CpuSN = "";
		BiosSN = "";
		MachineCode = "";
		ChkMachineCode = "";
		UseMachineCode = false;
		UseVerChk = false;
		PaidCnt = 0;
		YanpCnt = 0;
		CaiyjCnt = 0;
		BangCnt = 0;
		NodeID = -1;
		Title = "";
		Location = "";
		TriggerWeight = 0;
		QCHMode = 3;
		IsUseCy = false;
		IsUseXM = false;
		MinMZ = 0;
		MinPZ = 0;
		MaxPZ = 0;
		pt_JiekUrl = "";
		IsOpenWserv = false;
	}

public:
	WideString SysID;
	WideString SysName;
	WideString LocalIP;
	WideString CpuSN;
	WideString BiosSN;
	WideString MachineCode;
	WideString ChkMachineCode;
	bool UseMachineCode;
	bool UseVerChk;
	int PaidCnt;
	int YanpCnt;
	int CaiyjCnt;
	int BangCnt;
	TDBConfig DBConfig;

	int NodeID; // ---功能节点 0 自助发卡 1停车场 2 采样 3计量 4
	int TingID; // --- 0:停车场入口 1：停车场出口
	String CYJNO; // ---采样机别称
	String RealTime; // ---取数时间间隔
	String Title; // ---程序标题
	String Location; // ---地点命名

	int TriggerWeight; // ---地磅触发重量 Kg
	int QCHMode; // ---汽车衡模式:1重磅 2轻磅 3轻重通用
	bool IsUseCy; // ---过重时是否判断采样
	bool IsUseXM; // ---回皮时是否判断卸煤
	int MinMZ; // ---最小毛重 Kg
	int MinPZ; // ---最小皮重
	int MaxPZ; // ---最大皮重
	AnsiString JiekIP; // ---接口IP
	AnsiString JiekPort; // ---接口端口
	String pt_JiekUrl;
	bool IsOpenWserv; // ---是否开放WebService服务

};

// ---串口信息
class TFrameSet { // ---数据包头尾设置

public:
	TFrameSet() {
		Init();
	}

	void Init() {
		DoubleFrameMark = false;
		FrameHead = 0x0;
		FrameTail = 0x0;
		FrameCtrl = 0x0;
	}

public:
	bool DoubleFrameMark;
	unsigned char FrameHead;
	unsigned char FrameTail;
	unsigned char FrameCtrl;
};

class TCommCfg { // ---串口配置

public:
	TCommCfg() {
		Init();
	}

	void Init() {
		ComNum = 0;
		dwBaudRate = 0;
		Chk = 'N';
		DataBits = 8;
		Stops = 0;
		UsePkg = false;
		ComPkgType = 0; // 启用数据包时需配置
		PkgInterval = 50;
		DisWrite = false;
	}

public:
	int ComNum;
	DWORD dwBaudRate;
	char Chk;
	BYTE DataBits;
	BYTE Stops;
	bool UsePkg;
	int ComPkgType; // 启用数据包时需配置
	TFrameSet FrameSet;
	long PkgInterval;
	bool DisWrite;
};

class TNetCfg { // ---网络配置

public:
	TNetCfg() {
		Init();
	}

	void Init() {
		IP = "";
		Port = 0;
		UserName = "";
		Pass = "";
	}

public:
	WideString IP;
	int Port;
	WideString UserName;
	WideString Pass;
};

class TBaseDeviceInfo { // ---设置基础信息

public:
	TBaseDeviceInfo() {
		Init();
	}

	void Init() {
		Model = "";
		Use = false;
		CommMode = 0;
		DevAddr = 0;
	}

public:
	WideString Model; // ---型号:通用
	bool Use; // ---是否使用
	TCommCfg CommCfg; // ---0：串口 1：网口
	TNetCfg NetCfg;
	int CommMode;
	int DevAddr; // ---设备通讯地址(十进制)
};

// ---识别卡信息
class TCardInfo {
public:
	TCardInfo() {
		Init();
	}

	void Init() {
		Cnts = 0;
		for (int i = 0; i < MaxCard; i++) {
			CardID[i] = "";
		}
		for (int i = 0; i < MaxCard; i++) {
			UserData_6C[i] = "";
		}
		memset(Ant, 0, sizeof(Ant));

		memset(&Sector, 0, sizeof(Sector));
		memset(ICCardID, 0, sizeof(ICCardID));
		memset(ICCardID_Hex, 0, sizeof(ICCardID_Hex));
	}

public:
	int Cnts; // 读到的卡数
	WideString CardID[MaxCard]; // MaxCard 张卡片
	WideString UserData_6C[MaxCard]; // MaxCard 张卡片
	int Ant[MaxCard]; // ---被哪个天线识别

	// ---IC卡信息
	struct Sector_struct {
		unsigned char Block[4][16]; // ---每个扇区4个块,每个块16字节
	};

	Sector_struct Sector[16]; // ---16个扇区

	unsigned char ICCardID[32];
	unsigned char ICCardID_Hex[64];
};

// ---获取平台采样信息
class TCaiyInfo {
public:
	TCaiyInfo() {
		Init();
	}

	void Init() {

		CaiyBH = "";
		CaiyDS = "";
		GongYS = "";
		CaiyLX = "";
		ZhiyBH = "";
		HuayBH = "";
	}

public:
	WideString CaiyBH;
	UTF8String GongYS;
	UTF8String CaiyLX;
	WideString CaiyDS;
	WideString ZhiyBH;
	WideString HuayBH;

};

// ---磅单信息
class TBangdInfo {

public:
	TBangdInfo() {
		Init();
	}

	void Init() {
		Title = "";
		Numplate = "";
		Gongysmc = "";
		Meikmc = "";
		Wulmc = "";
		Zhongcsj = "";
		Qingcsj = "";
		Maoz = 0.00;
		Piz = 0.00;
		Jingz = 0.00;
		Rickd = 0.00;
		Koup = 0.00; // ---扣平
		Kous = 0.00;
		Kouz = 0.00;
		Koug = 0.00;
		Kouqt = 0.00;
		Yingk = 0.00;
		PrtTime = "";
		Printer = "";
		JilsjID = "";
	}

public:
	AnsiString Title;
	AnsiString Numplate;
	AnsiString Gongysmc;
	AnsiString Meikmc;
	AnsiString Wulmc;
	AnsiString Zhongcsj;
	AnsiString Qingcsj;
	double Maoz;
	double Piz;
	double Jingz;
	double Rickd;
	double Koup;
	double Kous;
	double Kouz;
	double Koug;
	double Kouqt;
	double Yingk;
	AnsiString Printer;
	AnsiString PrtTime;
	AnsiString JilsjID;
};

// ---车辆信息及状态
class TCarInfo {
public:
	TCarInfo() {
		Init();
	}

	void Init() {
		CardID = "";
		NumPlate = "";
		XCB = "";
		NumPlate_Guac = "";
		ChedMC = "";
		IsLock = false;
		RoundFlag = 0;

		MaxM = 0;
		MinM = 0;
		MaxP = 0;
		MinP = 0;
		carType = "";
		carType_ID = "";

		ReaderIndex = 0;
		AntIndex = 0;

		Gongys = "";
		Meik = "";
		Pinz = "";
		Yscd = "";
		IsCy = -1;
		IsWeight = -1;
		IsTare = -1;
		Legth = 0;
		Width = 0;
		Dipg = 0;
		Lajs = 0;
		memset(Ljwz, 0, sizeof(Ljwz));
		Leib = 0;
		YewZT = 0;
		BiaoZ = 0;
		GuaStart = 0;
		GuaEnd = 0;
		ICCardID = "";
	}

public:
	WideString CardID; // 卡号
	WideString ICCardID;
	WideString NumPlate; // 车号
	WideString XCB; // 对应card_plate中的 行车本号
	WideString NumPlate_Guac; // 对应card_plate中的 挂车号
	WideString ChedMC; // 对应card_plate中的 车队名
	int IsLock; // 锁车状态 0未锁 1被锁
	int RoundFlag; // 车辆状态标志	老系统 1已过重 0其它
	// 毛皮上下限 吨
	double MaxM;
	double MinM;
	double MaxP;
	double MinP;
	double BiaoZ; // ---标准载重量
	WideString carType; // 对应card_plate中的 自卸 平板等
	WideString carType_ID; // 对应card_plate中的 ABCDEF车型

	int ReaderIndex; // 主机编号(被哪个主机识别)
	int AntIndex; // 天线编号(被哪个天线所识别)

	WideString Gongys; // ---供应商
	WideString Meik; // ---煤矿
	WideString Pinz; // ---品种
	WideString Yscd; // ---运输车队
	int IsCy; // ---0未采样 1已采样
	int IsWeight; // ---0未过重 1已过重
	int IsTare; // ---0未回皮 1已回皮
	TDateTime PPSJ; // ---匹配时间

	// ---采样用数据
	int Legth;
	int Width;
	int Dipg;
	int Lajs;
	int Ljwz[12];
	int Leib; // --- 0:入厂煤 1:其它 具体看平台的约定
	int YewZT; // ---业务状态ID,具体参考平台约定
	int GuaStart;
	int GuaEnd;
};

// ----化验信息汇总
class THYSInfo {
public:
	THYSInfo() {
		Init();
	}

	void Init() {
		S_NO = ""; // 样品编号
		Mt = 0; // 全水分
		Mad = 0; // 空干基水分
		Mar = 0; // 收到基水分
		Aad = 0; // 空干基灰分
		Ad = 0; // 干基灰分
		Aar = 0; // 收到基灰分

		Vad = 0; // 挥发分
		Vd = 0; // 干基挥发分
		Varb = 0; // 收到基挥发分
		Vdaf = 0; // 干燥无灰基
		Stad = 0; // 空干基硫
		Std = 0; // 干基全硫
		Star = 0; // 收到基全硫
		Fcad = 0; // 空干基固定碳

		Fcd = 0; // 干基固定碳
		Fcar = 0; // 收到基固定碳
		Had = 0; // 空干基氢
		Qbad = 0; // 弹筒发热量
		QbadCal = 0; // 弹筒发热量大卡
		Qgrad = 0; // 空干基高位热值
		QgradCal = 0; // 空干基高位热值大卡
		Qgrd = 0; // 干基高位热值
		QgrdCal = 0; // 干基高位热值大卡
		Qnetar = 0; // 收到基低位热值
		QnetarCal = 0; // 收到基低位热值大卡
		TestTime = ""; // 测试时间
		JZTZ = 0; // 焦渣特征
		isupload = 0; // 读取标志位，1 可读取 2 读取后改标志位

	}

public:
	WideString S_NO;
	double Mt;
	double Mad;
	double Mar;
	double Aad;
	double Ad;
	double Aar;
	double Vad;
	double Vd;
	double Varb;
	double Vdaf;
	double Stad;
	double Std;
	double Star;
	double Fcad;
	double Fcd;
	double Fcar;
	double Had;
	double Qbad;
	double QbadCal;
	double Qgrad;
	double QgradCal;
	double Qgrd;
	double QgrdCal;
	double Qnetar;
	double QnetarCal;
	WideString TestTime;
	int JZTZ;
	int isupload;

};
//----全水样数据
class TQSYInfo {
public:
	TQSYInfo() {
		Init();
	}

	void Init() {

		SAMPLENO = ""; // 样品编号
		POSITIONNO = 0; // 烘箱编号（坩埚编号）
		SAMPLENAME = ""; // 样品名称
		TRAYWEIGHT = 0; // 浅盘重量
		SAMPLEWEIGHT = ""; // 样品重量
		LEFTWEIGHT = ""; // 残重

		MOISTURE = 0; // 水分
		LOSSRATE = 0; // 水份损失率
		REVISEMOISTURE = 0; // 修正水份
		STARTINGTIME = ""; // 实验开始时间
		ENDINGTIME = ""; // 实验结束时间
		STATUS = ""; // 实验状态
		TEMPOFCONSTANTTEMPERATURE = 0; // 恒温温度（℃）
		TEMPERATURECOMPENSATION = 0; // 温度补偿

		TIMEOFCONSTANTTEMPERATURE = 0; // 恒温时间（min）
		TIMESOFDRYING = 0; // 检测性干燥次数
		INTERVALTIME = 0; // 检测性干燥时间（min）
		ANALYSISPRECISION = 0; // 恒重质量
		NUMBERINGMETHOD = ""; // 编号方式（人工编号/机器编号）
		OPERATOR = ""; // 操作者（实验员）
		REMARK = ""; // 备注
		ID = 0; // 数据源ID

	}

public:

	WideString SAMPLENO;
	int POSITIONNO;
	WideString SAMPLENAME;
	double TRAYWEIGHT;
	WideString SAMPLEWEIGHT;
	WideString LEFTWEIGHT;
	double MOISTURE;
	double LOSSRATE;
	double REVISEMOISTURE;
	WideString STARTINGTIME;
	WideString ENDINGTIME;
	WideString STATUS;
	double TEMPOFCONSTANTTEMPERATURE;
	double TIMEOFCONSTANTTEMPERATURE;
	double TEMPERATURECOMPENSATION;
	double TIMESOFDRYING;
	double INTERVALTIME;
	double ANALYSISPRECISION;
	WideString NUMBERINGMETHOD;
	WideString OPERATOR;
	WideString REMARK;
	int ID;
};
//--样品柜信息表
class TCYGInfo {
public:
	TCYGInfo() {
		Init();
	}

	void Init() {

		REC_NO = 0; // ID
		GGName = ""; // 柜名(存查样柜1-10)
		CCNAME = ""; // 层名(A1到A14B1到B14)
		WWNAME = ""; // 位名(1到30)
		ENABLED = 0; // 位置是否可用(1：可用0：不可用)
		DEPOSITTIME = ""; // 煤样瓶存入时间

		MAKESAMPLETIME = 0; // 制样时间
		SAMPLETYPE = 0; // 煤样类型(6：6mm全水样3：3mm存查样21：0.2mm化验样22：0.2mm存查样)
		SAMPLEID = 0; // 煤样编码
		SAMPLESTATUS = ""; // 煤样瓶在存查柜内的状态(1：正常瓶2：超期瓶3：无效瓶（该位置有煤样瓶，但RFID芯片读取错误FFFFFFFFF）4：无瓶（00000000）)

	}

public:

	int REC_NO;
	WideString GGName;
	WideString CCNAME;
	WideString WWNAME;
	int ENABLED;
	WideString DEPOSITTIME;
	WideString MAKESAMPLETIME;
	int SAMPLETYPE;
	WideString SAMPLEID;
	WideString SAMPLESTATUS;
};

//--样品历史存取记录息表
class TCYGCQInfo {
public:
	TCYGCQInfo() {
		Init();
	}

	void Init() {

		REC_NO = 0; // 主键
		GGName = ""; // 柜名（存查样柜1-10）
		CCNAME = ""; // 层名（A1到A14，B1到B14）
		WWNAME = ""; // 位名（1到30）
		OpType = 0; // 存取操作类型（1：自动存样（自动制样间或人工制样间->存查样柜）2：自动取样（存查样柜->化验室）3：自动弃样（存查样柜->人工制样间）
		OpTime = ""; // 存取操作时间

		SourceName = ""; // 存取煤样的来源设备名称(自动存样站1-10（自动制样间）
		SAMPLETYPE = 0; // 煤样类型（6：6mm全水样3：3mm存查样21：0.2mm化验样22：0.2mm存查样）
		DestinationName = ""; // 存取煤样的目标设备名称(存查样柜1-10（存查样间）
		MakeSampleTime = ""; // 制样时间
		SampleID  = "";      //煤样编码
		IsSuccessed = 0;      //是否存取成功
		Operator = "";     //操作人
	}

public:

	int REC_NO;
	WideString GGName;
	WideString CCNAME;
	WideString WWNAME;
	int OpType;
	WideString OpTime;
	WideString MakeSampleTime;
	int SAMPLETYPE;
	WideString SourceName;
	WideString DestinationName;
	WideString SampleID;
	int IsSuccessed;
	WideString Operator;
};

//--设备状态信息表  气动管道
class TQDGDInfo {
public:
	TQDGDInfo() {
		Init();
	}

	void Init() {

		REC_NO = 0; // 唯一标识
		DeviceName = ""; // 设备名称
		DeviceStatus1 = 0; // 设备运行状态
		DeviceStatus2 = 0; // 设备瓶满状态
		DeviceStatus3 = 0; // 存查柜煤样超期状态
		LineStatus = 0; // 与该设备连接的传输管道状态
	}

public:

	int REC_NO;
	WideString DeviceName;
	int DeviceStatus1;
	int DeviceStatus2;
	int DeviceStatus3;
	int LineStatus;
};

// ----采样信息
struct CarSamplingData {
	char SePicModPath[300];
	char PicCarnoPath[300];
	int SimplingPoint;
	char SimplingTarget[100];
	int SetBucketWeight;
	int SetBucketNo;
	char SamplingPerson[100];
	char SimplingLocation[100];
	char SimplingType[10];
	char SimplingStartTime[20];
	char SimplingEndTime[20];
	char SupplierCode[100];
	char BatchInfoId[40];
	char SimplingCode[100];
	char VehicleNo[50];
	char SerialNo[30];
	double CarLength;
	double CarWidth;
	double ChassisHeight;
	double EmptyBucketWeight;

	double TrailerSimplingPoint;
	double Vehiclecarlength;
	double Vehiclecarwidth;
	double Chassisheightone;
	double Clearance;
	char TrailerSimplingLocation[100];
	char TrailerSimplingTarget[100];
};
//---采样机准备状态信息
class TCYZBInfo {
public:
	TCYZBInfo() {
		Init();
	}

	void Init() {

		BARREL_READY = ""; // 是否选择分样桶
		CAR_READY = ""; // 车型确认
		JLC_DOOR_READY = 0; // 集料仓门是否关闭
		CSB_BIGCAR_READY = 0; // 大车超声波感应状态
		CYJ_BACK_READY = 0; // 采样机是否位于原位
		FYT_DOOR_READY = ""; // 分样桶仓门是否关闭
		DATA_SEND_READY = 0;  //数据是否上传
	}

public:

	WideString BARREL_READY;
	WideString CAR_READY;
	int JLC_DOOR_READY;
	int CSB_BIGCAR_READY;
	int CYJ_BACK_READY;
	WideString FYT_DOOR_READY;
	int DATA_SEND_READY;
};

// ---集样罐信息
struct SamperCollectionData {
	int BucketWeight;
	int BucketNo;
	int GrandBucketPoint;
	char SamplingCode[50];
	char SimplingLocation[100];
	char SimpLingUpdateTime[20];
};

typedef struct UDP_PACKED_TMP_STRUCT {
	// ---本地配置
	BYTE sLocation[16]; // ---本地名
	BYTE sHostIP[16]; // ---主机IP
	// ---天线配置
	BYTE sReaderIP[16]; // ---天线IP
	BYTE nReaderAnt; // ---天线端口
	BYTE bConn; // ---天线是否连接
	BYTE sCar[12]; // ---当前车牌号
	BYTE sCurrStatus[32]; // ---当前状态
	// ---地磅仪表
	BYTE sMeterCom; // ---串口号
	BYTE sRealValue[4]; // ---地磅值
	// ---道闸
	BYTE nDzCom;
	BYTE bDzStatus; // ---0落 1抬
	// ---交通灯
	BYTE nHldCom; // ---红绿灯串口号
	BYTE nHldStatus; // ---0红 1绿
	// 红外对射状态
	BYTE nDwqCom; // ---定位器串口号
	BYTE bFront; // ---0挡 1通
	BYTE bBack;
} UDP_PACKED_TMP, *pUDP_PACKED_TMP;

typedef struct tagDATAPKG {

	char *data;
	int nLen;
} DATAPKG, *pDATAPKG;

typedef struct struct_DataSetInfo {

	int nRecordCount; // 记录数
	int nCols; // 列数
	int nCursor; // 当前游标
} DataSetInfo, *pDataSetInfo;

// ---获取平台当前流程位置与下一流程位置信息
class TStation {
public:
	TStation() {
		Init();
	}

	void Init() {

		Current_Location = "";
		Current_Station = "";
		Next_Location = "";
		Next_Station = "";
	}

public:
	// UTF8String  WideString
	UTF8String Current_Location;
	UTF8String Current_Station;
	UTF8String Next_Location;
	UTF8String Next_Station;
};

#endif
