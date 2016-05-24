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
	unsigned char Head_Tag; // ---��Ч���ݿ�ͷ��־
	unsigned char Tail_Tag; // ---��Ч���ݽ�β��־
	int StartPos; // ---��־λ��ʼ���NλΪ��Ч��������
	int DataLen; // ---��Ч���ݳ���
	double Zoom; // ---���ݷŴ���
	bool IsRev; // ---�����Ƿ���
	int CycleTime; // ---ȡ�ȶ�ֵ�ļ��ʱ��,��λms
	int Cnt; // ---ȡ�ȶ�ֵʱ��ȡֵ����
	DWORD dwTimeOuts; // ÿ���յ��Ǳ����ݵ�ʱ������  ms
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
	int AntCnt; // ������
	int Ant[8]; // ---Ant�ں�
};

// --- IO��ͷ������,���Ƶ�բ ���̵� �Լ���ȡ ��λ���ź�
class TIOCtrlInfo : public TBaseDeviceInfo { // ---�򿪻�رտ�����ʱһ��Ҫ�жϿ�����״̬

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
	bool UseDZ_Head; // ---�Ƿ�ʹ��ǰ�õ�բ
	bool UseDZ_Tail; // ---�Ƿ�ʹ�ú��õ�բ
	bool UseHLD_Head; // ---�Ƿ�ʹ��ǰ�ú��̵�
	bool UseHLD_Tail; // ---�Ƿ�ʹ�ú��ú��̵�
	bool UseDWQ; // ---�Ƿ�ʹ��ǰλ��

	// ---ָ�16���Ƶ��ַ���������ָ�����Ӣ�İ�Ƕ��Ÿ���
	WideString UpDZCmd_Head; // --- ̧ǰ������
	WideString DownDZCmd_Head; // ---��ǰ������
	WideString UpDZCmd_Tail; // ---̧�������
	WideString DownDZCmd_Tail; // ---��������

	WideString RedHLDCmd_Head; // ---ǰ�ú������
	WideString GreenHLDCmd_Head; // ---ǰ���̵�����
	WideString CloseHLDCmd_Head; // ---ǰ�ùص�����
	WideString RedHLDCmd_Tail; // ---���ú������
	WideString GreenHLDCmd_Tail; // ---�����̵�����
	WideString CloseHLDCmd_Tail; // ---���ùص�����

	WideString SelDWQStaCmd; // ---��ѯ��λ������
	WideString DWQMissEcho_Head; // ---ǰ��λ�ڵ��ķ���
	WideString DWQMissEcho_Tail; // ---��λ�ڵ��ķ���
	WideString DWQPassEcho_ALL; // ---��λȫͨ���ķ���
	WideString DWQMissEcho_ALL; // ---��λȫ�۵��ķ���
	DWORD dwTimeOuts; // ÿ���հ���ʱ������  ms

	// ---�豸״̬ 0 up red,1 down green 2 err -1������
	int DZSta_Head;
	int DZSta_Tail;
	int HLDSta_Head;
	int HLDSta_Tail;
	int DWQSta_Head;
	int DWQSta_Tail;
};

// ---���������豸,�������ͳһIO���������,�ݲ���
class TDZInfo : public TBaseDeviceInfo {
public:
private:
	// ---�� �� ͣ ָ��
	WideString UP_Cmd;
	WideString DOWN_Cmd;
	WideString STOP_Cmd;
};

class THLDInfo : public TBaseDeviceInfo {
public:
private:
	// ---�� �� �� ָ��
	WideString Red_Cmd;
	WideString Green_Cmd;
	WideString Close_Cmd;
};

class TDWQInfo : public TBaseDeviceInfo {
public:
private:
	// ---ָ��
	WideString sel_Cmd; // ---��ѯ
	WideString FrontFail_Echo; // ---ǰ�ñ�������
	WideString BackFail_Echo; // ---���ñ�������
	WideString AllFail_Echo; // ---ȫ����������
	WideString AllPass_Echo; // ---ȫ��ͨ������
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
	int CardAddr; // ---���ƿ���ַ(EQ:�����д�0��ʼ,�����м�1)
	int Width; // ---LED��
	int Height; // ---LED��
	int FontSize; // ---�ֺ�
	int SendMode; // ---0:��Ŀ 1��ʱʵ���ݣ���ͼƬ��
	int ColorStyle; // ---��ɫģʽ,0��ɫ,1˫ɫ
};

// ---�豸��Ϣ������
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
	// ---Ӳ������
	int ReaderCnt; // ---�ֳ����2������ʵ��˫��	,��1����2����ʵ��˫��
	int LedCnt;
	int IoCtrlCnt;

	TBalanceInfo BalanceInfo;
	TReaderInfo ReaderInfo[4];
	TIOCtrlInfo IOCtrlInfo[8];
	TLedInfo LedInfo[4];
	TBaseDeviceInfo PrtInfo; // ---���������������Ĵ�ӡ��
};

#endif