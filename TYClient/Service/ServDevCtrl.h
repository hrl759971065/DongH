//---------------------------------------------------------------------------

#ifndef ServDevCtrlH
#define ServDevCtrlH
#include "ServBase.h"
//---------------------------------------------------------------------------

class TServDevCtrl : public TServBase{
public:
	TServDevCtrl();
	~TServDevCtrl();

	bool OpenMeter();
	bool OpenReader(int Index);
	bool CloseReader(int Index);
	bool OpenLED(int Index);
	bool OpenIOCtrl(int Index);
	bool OpenPrt();
	bool ClosePrt();

	bool SendLed(int Index,String str);
	bool TServDevCtrl::SendPicToLedRam(int Index,HBITMAP hBitmap,int nPosX,int nPosY,int Width,int Height);

	bool ReadCardID(int Index,TCardInfo &CardInfo);
	bool ReadICCard(int Index,TCardInfo &CardInfo);
	bool WriteICCard(int Index,int nSector,int nBlock,AnsiString strKey,AnsiString strData);
	bool SendICCard(int Index,int SendType);
	bool ReceiveICCard(int Index,int aActionID);
	bool RetCardProc_ZZCC(String Reason,bool bRec,bool bReturned);
	bool DeviceStatus(int Index,unsigned char *outStatus);
	String DevStatusDecode(unsigned char *inStatus);
	String DevStatusDecode_D3001(unsigned char *inStatus);

    double GetRealVal();
	bool GetStaticVal(double &fVal);
	bool IsLeave();

	long GetDWQSta();
	bool DWQCheckProc(int nHeadDZFlag,String Numplate);
	void OutCtrl(int nDev,int nPos,int nAction);
	void HLDCtrl(int nHead_cmd,int nTail_cmd);

	void FinishProc(String wavName,String strLed,String strSta,String strList,bool bHeadDZIsUp,bool bTailDZIsUp);
	void FinishCRC(String wavName,String strLed,String strSta,String strList,bool bHeadDZIsUp,bool bTailDZIsUp,int LedAdr);

	void PrintBangd(TBangdInfo bdInfo);
protected:

private:
	String 	   _sendStr_Old[2];
};

#endif
