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
	int AccessType; // ---���ݷ��ʷ�ʽ, 0:ֱ�ӷ���Զ�����ݿ� 1:DataSnap 2:Rest 3:WebService
};

class TSysInfo { // ---��Exe������Ϣ

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

	int NodeID; // ---���ܽڵ� 0 �������� 1ͣ���� 2 ���� 3���� 4
	int TingID; // --- 0:ͣ������� 1��ͣ��������
	String CYJNO; // ---���������
	String RealTime; // ---ȡ��ʱ����
	String Title; // ---�������
	String Location; // ---�ص�����

	int TriggerWeight; // ---�ذ��������� Kg
	int QCHMode; // ---������ģʽ:1�ذ� 2��� 3����ͨ��
	bool IsUseCy; // ---����ʱ�Ƿ��жϲ���
	bool IsUseXM; // ---��Ƥʱ�Ƿ��ж�жú
	int MinMZ; // ---��Сë�� Kg
	int MinPZ; // ---��СƤ��
	int MaxPZ; // ---���Ƥ��
	AnsiString JiekIP; // ---�ӿ�IP
	AnsiString JiekPort; // ---�ӿڶ˿�
	String pt_JiekUrl;
	bool IsOpenWserv; // ---�Ƿ񿪷�WebService����

};

// ---������Ϣ
class TFrameSet { // ---���ݰ�ͷβ����

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

class TCommCfg { // ---��������

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
		ComPkgType = 0; // �������ݰ�ʱ������
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
	int ComPkgType; // �������ݰ�ʱ������
	TFrameSet FrameSet;
	long PkgInterval;
	bool DisWrite;
};

class TNetCfg { // ---��������

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

class TBaseDeviceInfo { // ---���û�����Ϣ

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
	WideString Model; // ---�ͺ�:ͨ��
	bool Use; // ---�Ƿ�ʹ��
	TCommCfg CommCfg; // ---0������ 1������
	TNetCfg NetCfg;
	int CommMode;
	int DevAddr; // ---�豸ͨѶ��ַ(ʮ����)
};

// ---ʶ����Ϣ
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
	int Cnts; // �����Ŀ���
	WideString CardID[MaxCard]; // MaxCard �ſ�Ƭ
	WideString UserData_6C[MaxCard]; // MaxCard �ſ�Ƭ
	int Ant[MaxCard]; // ---���ĸ�����ʶ��

	// ---IC����Ϣ
	struct Sector_struct {
		unsigned char Block[4][16]; // ---ÿ������4����,ÿ����16�ֽ�
	};

	Sector_struct Sector[16]; // ---16������

	unsigned char ICCardID[32];
	unsigned char ICCardID_Hex[64];
};

// ---��ȡƽ̨������Ϣ
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

// ---������Ϣ
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
		Koup = 0.00; // ---��ƽ
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

// ---������Ϣ��״̬
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
	WideString CardID; // ����
	WideString ICCardID;
	WideString NumPlate; // ����
	WideString XCB; // ��Ӧcard_plate�е� �г�����
	WideString NumPlate_Guac; // ��Ӧcard_plate�е� �ҳ���
	WideString ChedMC; // ��Ӧcard_plate�е� ������
	int IsLock; // ����״̬ 0δ�� 1����
	int RoundFlag; // ����״̬��־	��ϵͳ 1�ѹ��� 0����
	// ëƤ������ ��
	double MaxM;
	double MinM;
	double MaxP;
	double MinP;
	double BiaoZ; // ---��׼������
	WideString carType; // ��Ӧcard_plate�е� ��ж ƽ���
	WideString carType_ID; // ��Ӧcard_plate�е� ABCDEF����

	int ReaderIndex; // �������(���ĸ�����ʶ��)
	int AntIndex; // ���߱��(���ĸ�������ʶ��)

	WideString Gongys; // ---��Ӧ��
	WideString Meik; // ---ú��
	WideString Pinz; // ---Ʒ��
	WideString Yscd; // ---���䳵��
	int IsCy; // ---0δ���� 1�Ѳ���
	int IsWeight; // ---0δ���� 1�ѹ���
	int IsTare; // ---0δ��Ƥ 1�ѻ�Ƥ
	TDateTime PPSJ; // ---ƥ��ʱ��

	// ---����������
	int Legth;
	int Width;
	int Dipg;
	int Lajs;
	int Ljwz[12];
	int Leib; // --- 0:�볧ú 1:���� ���忴ƽ̨��Լ��
	int YewZT; // ---ҵ��״̬ID,����ο�ƽ̨Լ��
	int GuaStart;
	int GuaEnd;
};

// ----������Ϣ����
class THYSInfo {
public:
	THYSInfo() {
		Init();
	}

	void Init() {
		S_NO = ""; // ��Ʒ���
		Mt = 0; // ȫˮ��
		Mad = 0; // �ոɻ�ˮ��
		Mar = 0; // �յ���ˮ��
		Aad = 0; // �ոɻ��ҷ�
		Ad = 0; // �ɻ��ҷ�
		Aar = 0; // �յ����ҷ�

		Vad = 0; // �ӷ���
		Vd = 0; // �ɻ��ӷ���
		Varb = 0; // �յ����ӷ���
		Vdaf = 0; // �����޻һ�
		Stad = 0; // �ոɻ���
		Std = 0; // �ɻ�ȫ��
		Star = 0; // �յ���ȫ��
		Fcad = 0; // �ոɻ��̶�̼

		Fcd = 0; // �ɻ��̶�̼
		Fcar = 0; // �յ����̶�̼
		Had = 0; // �ոɻ���
		Qbad = 0; // ��Ͳ������
		QbadCal = 0; // ��Ͳ��������
		Qgrad = 0; // �ոɻ���λ��ֵ
		QgradCal = 0; // �ոɻ���λ��ֵ��
		Qgrd = 0; // �ɻ���λ��ֵ
		QgrdCal = 0; // �ɻ���λ��ֵ��
		Qnetar = 0; // �յ�����λ��ֵ
		QnetarCal = 0; // �յ�����λ��ֵ��
		TestTime = ""; // ����ʱ��
		JZTZ = 0; // ��������
		isupload = 0; // ��ȡ��־λ��1 �ɶ�ȡ 2 ��ȡ��ı�־λ

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
//----ȫˮ������
class TQSYInfo {
public:
	TQSYInfo() {
		Init();
	}

	void Init() {

		SAMPLENO = ""; // ��Ʒ���
		POSITIONNO = 0; // �����ţ�������ţ�
		SAMPLENAME = ""; // ��Ʒ����
		TRAYWEIGHT = 0; // ǳ������
		SAMPLEWEIGHT = ""; // ��Ʒ����
		LEFTWEIGHT = ""; // ����

		MOISTURE = 0; // ˮ��
		LOSSRATE = 0; // ˮ����ʧ��
		REVISEMOISTURE = 0; // ����ˮ��
		STARTINGTIME = ""; // ʵ�鿪ʼʱ��
		ENDINGTIME = ""; // ʵ�����ʱ��
		STATUS = ""; // ʵ��״̬
		TEMPOFCONSTANTTEMPERATURE = 0; // �����¶ȣ��棩
		TEMPERATURECOMPENSATION = 0; // �¶Ȳ���

		TIMEOFCONSTANTTEMPERATURE = 0; // ����ʱ�䣨min��
		TIMESOFDRYING = 0; // ����Ը������
		INTERVALTIME = 0; // ����Ը���ʱ�䣨min��
		ANALYSISPRECISION = 0; // ��������
		NUMBERINGMETHOD = ""; // ��ŷ�ʽ���˹����/������ţ�
		OPERATOR = ""; // �����ߣ�ʵ��Ա��
		REMARK = ""; // ��ע
		ID = 0; // ����ԴID

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
//--��Ʒ����Ϣ��
class TCYGInfo {
public:
	TCYGInfo() {
		Init();
	}

	void Init() {

		REC_NO = 0; // ID
		GGName = ""; // ����(�������1-10)
		CCNAME = ""; // ����(A1��A14B1��B14)
		WWNAME = ""; // λ��(1��30)
		ENABLED = 0; // λ���Ƿ����(1������0��������)
		DEPOSITTIME = ""; // ú��ƿ����ʱ��

		MAKESAMPLETIME = 0; // ����ʱ��
		SAMPLETYPE = 0; // ú������(6��6mmȫˮ��3��3mm�����21��0.2mm������22��0.2mm�����)
		SAMPLEID = 0; // ú������
		SAMPLESTATUS = ""; // ú��ƿ�ڴ����ڵ�״̬(1������ƿ2������ƿ3����Чƿ����λ����ú��ƿ����RFIDоƬ��ȡ����FFFFFFFFF��4����ƿ��00000000��)

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

//--��Ʒ��ʷ��ȡ��¼Ϣ��
class TCYGCQInfo {
public:
	TCYGCQInfo() {
		Init();
	}

	void Init() {

		REC_NO = 0; // ����
		GGName = ""; // �������������1-10��
		CCNAME = ""; // ������A1��A14��B1��B14��
		WWNAME = ""; // λ����1��30��
		OpType = 0; // ��ȡ�������ͣ�1���Զ��������Զ���������˹�������->�������2���Զ�ȡ�����������->�����ң�3���Զ��������������->�˹������䣩
		OpTime = ""; // ��ȡ����ʱ��

		SourceName = ""; // ��ȡú������Դ�豸����(�Զ�����վ1-10���Զ������䣩
		SAMPLETYPE = 0; // ú�����ͣ�6��6mmȫˮ��3��3mm�����21��0.2mm������22��0.2mm�������
		DestinationName = ""; // ��ȡú����Ŀ���豸����(�������1-10��������䣩
		MakeSampleTime = ""; // ����ʱ��
		SampleID  = "";      //ú������
		IsSuccessed = 0;      //�Ƿ��ȡ�ɹ�
		Operator = "";     //������
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

//--�豸״̬��Ϣ��  �����ܵ�
class TQDGDInfo {
public:
	TQDGDInfo() {
		Init();
	}

	void Init() {

		REC_NO = 0; // Ψһ��ʶ
		DeviceName = ""; // �豸����
		DeviceStatus1 = 0; // �豸����״̬
		DeviceStatus2 = 0; // �豸ƿ��״̬
		DeviceStatus3 = 0; // ����ú������״̬
		LineStatus = 0; // ����豸���ӵĴ���ܵ�״̬
	}

public:

	int REC_NO;
	WideString DeviceName;
	int DeviceStatus1;
	int DeviceStatus2;
	int DeviceStatus3;
	int LineStatus;
};

// ----������Ϣ
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
//---������׼��״̬��Ϣ
class TCYZBInfo {
public:
	TCYZBInfo() {
		Init();
	}

	void Init() {

		BARREL_READY = ""; // �Ƿ�ѡ�����Ͱ
		CAR_READY = ""; // ����ȷ��
		JLC_DOOR_READY = 0; // ���ϲ����Ƿ�ر�
		CSB_BIGCAR_READY = 0; // �󳵳�������Ӧ״̬
		CYJ_BACK_READY = 0; // �������Ƿ�λ��ԭλ
		FYT_DOOR_READY = ""; // ����Ͱ�����Ƿ�ر�
		DATA_SEND_READY = 0;  //�����Ƿ��ϴ�
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

// ---��������Ϣ
struct SamperCollectionData {
	int BucketWeight;
	int BucketNo;
	int GrandBucketPoint;
	char SamplingCode[50];
	char SimplingLocation[100];
	char SimpLingUpdateTime[20];
};

typedef struct UDP_PACKED_TMP_STRUCT {
	// ---��������
	BYTE sLocation[16]; // ---������
	BYTE sHostIP[16]; // ---����IP
	// ---��������
	BYTE sReaderIP[16]; // ---����IP
	BYTE nReaderAnt; // ---���߶˿�
	BYTE bConn; // ---�����Ƿ�����
	BYTE sCar[12]; // ---��ǰ���ƺ�
	BYTE sCurrStatus[32]; // ---��ǰ״̬
	// ---�ذ��Ǳ�
	BYTE sMeterCom; // ---���ں�
	BYTE sRealValue[4]; // ---�ذ�ֵ
	// ---��բ
	BYTE nDzCom;
	BYTE bDzStatus; // ---0�� 1̧
	// ---��ͨ��
	BYTE nHldCom; // ---���̵ƴ��ں�
	BYTE nHldStatus; // ---0�� 1��
	// �������״̬
	BYTE nDwqCom; // ---��λ�����ں�
	BYTE bFront; // ---0�� 1ͨ
	BYTE bBack;
} UDP_PACKED_TMP, *pUDP_PACKED_TMP;

typedef struct tagDATAPKG {

	char *data;
	int nLen;
} DATAPKG, *pDATAPKG;

typedef struct struct_DataSetInfo {

	int nRecordCount; // ��¼��
	int nCols; // ����
	int nCursor; // ��ǰ�α�
} DataSetInfo, *pDataSetInfo;

// ---��ȡƽ̨��ǰ����λ������һ����λ����Ϣ
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
