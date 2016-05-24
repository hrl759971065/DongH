#ifndef CommDeviceH
#define CommDeviceH

// #include "Globals.h"
#include "voDeviceInfo.h"

// #include "Readers.h"	//---test

#ifdef DLL_EXPORT
#define DLL_API extern "C" __declspec(dllexport)
#else
#define DLL_API extern "C" __declspec(dllimport)
#endif

typedef void __fastcall(__closure * lpfnCommNotify)(TObject * Sender,
	int NotifyType);
typedef void __fastcall(__closure * lpfnPackageNotify)(TObject * Sender,
	int NotifyType);

class TComm {

public:
	/* virtual ~IComm(void)
	 {
	 } */

	__property int PortNo = {read = fGetPort, write = fSetPort};
	__property WideString PortName = {read = fGetPortName};
	__property DWORD Baud = {read = fGetBaud, write = fSetBaud
	}; /* default = br115200 ���ú�˿ڹر� */
	__property WideString Parity = {read = fGetParity, write = fSetParity
	}; /* default = ptNoParity ���ú�˿ڹر� */
	__property BYTE ByteSize = {read = fGetByteSize, write = fSetByteSize
	}; /* default = 8 ���ú�˿ڹر� */
	__property BYTE StopBits = {read = fGetStopBits, write = fSetStopBits
	}; /* default = sbOneStopBit 0,1,2 = 1, 1.5, 2 ���ú�˿ڹر� */

	__property long InBufSize = {read = fGetInBufSize, write = fSetInBufSize
	}; /* default = 8192 ���ڶ����ݻ��������, ��λ: �ֽ� */
	__property long OutBufSize = {read = fGetOutBufSize, write = fSetOutBufSize
	}; /* default = 8192 ����д���ݻ��������, ��λ: �ֽ� */

	__property bool UsePackage = {read = fGetUsePackage, write = fSetUsePackage
	};
	__property long QueueSize = {read = fGetQSize, write = fSetQSize
	}; /* default =   16 */
	__property long PackageSize = {read = fGetPkgSz, write = fSetPkgSz
	}; /* default = 4096 */
	__property int PackageType = {read = fGetPkgType, write = fSetPkgType
	}; /* default = cptFrameTimeout */
	__property long PackageInterval = {
		read = fGetPkgInterval, write = fSetPkgInterval}; /* default = 50 */
	__property TFrameSet FrameSettings = {
		read = fGetFrameSettings, write = fSetFrameSettings};
	__property bool DisableWrite = {
		read = fGetDisableWrite, write = fSetDisableWrite}; // true:��������������
	__property DWORD TimeOuts = {read = fGetTimeOuts, write = fSetTimeOuts};
	__property bool IsOpen = {read = fGetIsOpen}; // ---�˿�״̬

	virtual lpfnCommNotify GetOnCommNotify(void) const = 0;
	virtual void SetOnCommNotify(lpfnCommNotify) = 0;
	virtual lpfnPackageNotify GetOnPackage(void) const = 0;
	virtual void SetOnPackage(lpfnPackageNotify) = 0;

	virtual bool Open() = 0;
	virtual bool Close() = 0;

	virtual long Read(void far *s, long n) = 0;
	virtual long Write(const void far *s, long n) = 0;
	virtual long Command(const char far *s) = 0;

	virtual bool ReadPackage(void *lpPkg) = 0;
	virtual bool WritePackage(const void *lpPkg) = 0;
	virtual unsigned long ReadPackage(void *lpPkg, unsigned long BufSize) = 0;
	virtual unsigned long WritePackage(const void *lpPkg,
		unsigned long nBytes) = 0;

	virtual void PurgeRead(bool bAbort = false) = 0;
	virtual void PurgeWrite(bool bAbort = false) = 0;

	virtual WideString GetLastErrs() = 0;

private:
	virtual int fGetPort(void) const = 0;
	virtual void fSetPort(int n) = 0;
	virtual WideString fGetPortName(void) const = 0;
	virtual DWORD fGetBaud(void) const = 0;
	virtual void fSetBaud(DWORD br) = 0;
	virtual WideString fGetParity(void) const = 0;
	virtual void fSetParity(WideString) = 0;
	virtual BYTE fGetByteSize(void) const = 0;
	virtual void fSetByteSize(BYTE) = 0;
	virtual BYTE fGetStopBits(void) const = 0;
	virtual void fSetStopBits(BYTE) = 0;

	virtual long fGetInBufSize(void) const = 0;
	virtual void fSetInBufSize(long) = 0;
	virtual long fGetOutBufSize(void) const = 0;
	virtual void fSetOutBufSize(long) = 0;

	virtual bool fGetUsePackage(void) const = 0;
	virtual void fSetUsePackage(bool bup) = 0;
	virtual long fGetQSize(void) const = 0;
	virtual void fSetQSize(long) = 0;
	virtual long fGetPkgSz(void) const = 0;
	virtual void fSetPkgSz(long) = 0;
	virtual int fGetPkgType(void) const = 0;
	virtual void fSetPkgType(int) = 0;
	virtual long fGetPkgInterval(void) const = 0;
	virtual void fSetPkgInterval(long) = 0;
	virtual TFrameSet fGetFrameSettings(void) = 0;
	virtual void fSetFrameSettings(TFrameSet fs) = 0;
	virtual bool fGetDisableWrite(void) const = 0;
	virtual void fSetDisableWrite(bool) = 0;
	virtual DWORD fGetTimeOuts(void) const = 0;
	virtual void fSetTimeOuts(DWORD) = 0;
	virtual bool fGetIsOpen(void) const = 0;
};

class TCarMeter {

public:
	// virtual ~IComm_CarMeter(void){}

	virtual bool SetMeterComm(TBalanceInfo MeterInfo) = 0;
	virtual bool OpenMeter() = 0;
	virtual bool CloseMeter() = 0;
	virtual WideString GetLastErrS() = 0;

	__property double RealValue = {read = fGetRealValue};
	__property DATAPKG RealData = {read = fGetRealData};
	__property WideString PortName = {read = fGetPortName};
	__property DWORD TimeOuts = {read = fGetTimeOuts}; // �Ǳ����ݽ��յĳ�ʱʱ�� ms
	__property bool OverTime = {read = fGetOverTime}; // �Ƿ񳬹� TimeOuts ���趨ֵ
	__property bool IsOpen = {read = fGetIsOpen};

private:
	virtual double fGetRealValue(void) const = 0;
	virtual DATAPKG fGetRealData(void) const = 0;
	virtual WideString fGetPortName(void) const = 0;
	virtual DWORD fGetTimeOuts(void) const = 0;
	virtual bool fGetOverTime(void) const = 0;
	virtual bool fGetIsOpen(void) const = 0;
};

class TIOCtrl {

public:
	// virtual 	~IComm_IOCtrl(void){}

	virtual bool SetIOCtrlComm(TIOCtrlInfo) = 0; // ����IO��������������
	virtual bool OpenIOCtrl() = 0;
	virtual bool CloseIOCtrl() = 0;
	virtual void OutCtrl(int, int, int) = 0;

	virtual WideString GetLastErrS() = 0;

	__property WideString PortName = {read = fGetPortName};
	__property DWORD TimeOuts = {read = fGetTimeOuts};
	__property bool OverTime = {read = fGetOverTime}; // �Ƿ񳬹� TimeOuts ���趨ֵ
	__property long DWQStatus = {read = fGetDWQStatus}; // -1��ʱ��0ȫͨ 1ǰ�� 2�� 3ȫ��
	__property bool IsOpen = {read = fGetIsOpen};

private:
	virtual WideString fGetPortName(void) const = 0;
	virtual DWORD fGetTimeOuts(void) const = 0;
	virtual bool fGetOverTime(void) const = 0;
	virtual long fGetDWQStatus(void) const = 0;
	virtual bool fGetIsOpen(void) const = 0;
};

class TReaders {

public:
	// virtual 	~IReaders(void){}

	virtual WideString GetLastErrS() const = 0;
	virtual bool PutCfgInfo(TReaderInfo) = 0;
	virtual bool OpenReader() = 0;
	virtual bool CloseReader() = 0;
	virtual bool ReadCard(TCardInfo &CardInfo, int nKeyMode = 0,
		unsigned char *key = '\0') = 0;
	virtual bool ReadUserData_6C(TCardInfo &CardInfo) = 0;
	virtual bool ReadCard_IC(int nSector, int nBlock, int nKeyMode,
		unsigned char *key, TCardInfo &CardInfo) = 0;
	virtual bool WriteCard_IC(int nSector, int nBlock, int nKeyMode,
		unsigned char *key, unsigned char *wData) = 0;
	virtual bool DeviceControl(int ActionID) = 0;
	virtual bool DeviceStatus(int statusID, unsigned char *outStatus) = 0;
};

class TLeds {

public:
	// virtual 	~ILeds(void){}
	virtual WideString GetLastErrS() const = 0;
	virtual bool PutCfgInfo(TLedInfo LedInfo) = 0;
	virtual bool Open() = 0;
	virtual bool Close() = 0;
	virtual bool SendStr(WideString str) = 0;
	virtual bool SendPicToRam(HBITMAP hBitmap) = 0;
	virtual bool SendPicToRam_2(HBITMAP hBitmap, int nPosX, int nPosY,
		int Width, int Height) = 0;
};

/* typedef TComm* (__stdcall *pFnCreateComm)(void);
 typedef TCarMeter* (__stdcall *pFnCreateComm_CarMeter)(void);
 typedef TIOCtrl* (__stdcall *pFnCreateComm_IOCtrl)(void);
 typedef IReaders* (__stdcall *pFnCreateReaders)(void);
 typedef TLeds* (__stdcall *pFnCreateLeds)(void); */

typedef void* (__stdcall*pFnCreateDevice)(int aKind, char *aModel);
// ---��̬����ʱʹ��

/* extern "C" __declspec(dllexport) TComm* __stdcall CreateComm(void);
 extern "C" __declspec(dllexport) TCarMeter* __stdcall CreateComm_CarMeter(void);
 extern "C" __declspec(dllexport) TIOCtrl* __stdcall CreateComm_IOCtrl(void);
 extern "C" __declspec(dllexport) IReaders* __stdcall CreateReaders(void);
 extern "C" __declspec(dllexport) TLeds* __stdcall CreateLeds(void); */

extern "C" __declspec(dllexport) void* __stdcall CreateDevice(int aKind,
	char *aModel); // ---��̬����ʱʹ��

#endif