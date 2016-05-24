//---------------------------------------------------------------------------

#ifndef ServBaseH
#define ServBaseH
#include <Vcl.Forms.hpp>
#include <Vcl.DBGrids.hpp>
#include <Datasnap.DBClient.hpp>
#include <IdHTTPWebBrokerBridge.hpp>
#include "NmlxService.h"
#include "CommDevice.h"
#include "json.h"
#include "LX_DB.h"
#include "Basic.h"
#include "JsonReq.h"
#include "JsonRet.h"
#include "Thread_Run.h"
#include "SelectionPanel.h"
#include "Thread_LedSend.h"

//---------------------------------------------------------------------------
//---用于界面更新值
class TWorkSta{
public:
	TWorkSta(){Init();}
	void Init(){
		IsUpd =false;
		RunFlag =false;
		StrRun ="";
		CardID ="";
		Car ="";
		StrFlow ="";
		DZSta_Head =-1;
		DZSta_Tail =-1;
		HLDSta_Head =-1;
		HLDSta_Tail =-1;
		DWQSta_Head =-1;
		DWQSta_Tail =-1;
		StrLed[0] ="";
		StrLed[1] ="";
		memoMsg ="";
		NetCnt =0;
		for(int i=0;i <16;i++){IP[i]="";}
		memset(PingVal,0,sizeof(PingVal));
        DownDZRunTime =0;
		IsDownFlag =false;
	}
public:
	bool IsUpd ;
	bool	RunFlag ;
	String StrRun ;
	String CardID ;
	String Car ;
	String StrFlow ;
	int DZSta_Head ;
	int DZSta_Tail ;
	int HLDSta_Head ;
	int HLDSta_Tail ;
	int DWQSta_Head ;
	int DWQSta_Tail ;
	String StrLed[2] ;
	String memoMsg ;
	int NetCnt ;
	String IP[16];
	int PingVal[16];

	DWORD DownDZRunTime;	//---启动延时落杆的开始计时
	bool  IsDownFlag;		//---煤场延时落杆的信号
};

class TServBase{
public:
	TServBase();
	~TServBase();
	String GetLastErrS() const;

	void ReadCfg();
	bool ConnKYC();
	bool ConnKYZ();
	bool ConnHYS();
	bool ConnQSY();
	bool ConnCYG();
	bool StartServ();
	bool StopServ();
	bool InitSoap();

	//---界面操作
	void MakeDBGridColumnsAutoFixItsWidth(TDBGrid *grid);
	void FormStyle(TForm *Form,int nStyleType);
	void CreateSelection(TWinControl *Parent/*,DynamicArray<String> btn_Caption*/);

	void GetAllMeikFromDS(DynamicArray<String> &MeikList);
	void GetGongysFromDS(String Meik,DynamicArray<String> &GongysList);
	void GetPinzFromDS(String Meik,String Gongys,DynamicArray<String> &PinzList);

	void WritePic_DDInfo();
	void WritePic_GGInfo();

	TSysInfo 		_sysInfo;
	TDBConfig   	_connKYC_cfg;
	TDBConfig   	_connKYZ_cfg;
	TDBConfig   	_connHYS_cfg;
	TDBConfig   	_connQSY_cfg;
	TDBConfig   	_connCYG_cfg;
	TDeviceInfo		_DevInfo;
	TFile			_File;

	TWorkSta		_WorkSta;

	TSelectionPanel *_Selection;
	TClientDataSet 	*_dsMkGys;	//---存储每是煤矿和供应商计划表
	String			_ServerDate;	//---服务器的日期
    double 	_fKuangfl;
	String 	_Meik;
	String 	_Gongys;
	String 	_Pinz;

	bool			_bReset;		//---发卡机,是否复位?

	int				_Sequence;	//---流程序列
	String 			_strFaKaTis;	//---发卡提示信息

	bool _bEnterSamUnload;	//---是否进入卸料过程

	//---LED公告屏信息
	TStrings 		*_CarList;	//---当前调度车辆
	int         	_ParkingNum_Total;	//---停车场总车位数
	int				_ParkingNum_Surplus;	//---停车场剩余车位
	int				_ScheduledNum;	//---当前场内的被调度车数
	int 			_ToScheduleNum;	//---当前场内未调试车数
	String          _GGTitle;	//---公告标题
	String			_GGText;    //---公告正文
	String			_GGDepartment;	//---公告部门
	String			_GGDate;		//---公告日期
	bool			_bSendLED_DDInfo;	//---是否发送LED调运信息
	bool			_bSendLED_GGInfo;	//---是否发送LED公告信息
	Thread_LedSend 	*_Thr_LedSend;
	struct LedBmp_Struct{
		TBitmap	*_bmDD[8];	//---发送到LED的调度图片
		TBitmap	*_bmGG[8];	//---发送到LED的公告图片
		int picNumDD;
		int picNumGG;
	};
	LedBmp_Struct _LedBmp;

	//---用于WebService测试界面同步收发数据
	UTF8String	_strReqJson;
	UTF8String	_strRetJson;
	bool		_bSyncSoapData;
	//存样柜取样
	TStrings 		*_BianMList;	//---当前要取样的编码
	bool _bEnterCunYG ;   //是否终止取样操作
	bool _bForceEndCunYG ;   //是否进行取样操作
	int CunYG_OpType;   //   CunYG_OpType 取样类型 1：取样  2：弃样
	int CunYG_OpDest;   //   CunYG_OpDest 取样目的地 1：化验室半自动存取室  2：弃样室
	UTF8String CunYG_UserName; //操作人员

protected:
	//---设备对象
	TCarMeter 	*_Meter;
	TReaders 	*_Reader[4];
	TLeds		*_Led[2];
	TIOCtrl		*_IOCtrl[4];
	TComm		*_ComPrt;	//---打印机串口

	//---数据库对象
	TLX_DB* _connKYC;	//---开元采样机
	TLX_DB* _connKYZ;   //---开元制样机
	TLX_DB* _connHYS;   //---化验室数据库
	TLX_DB* _connQSY;   //---全水样数据库
	TLX_DB* _connCYG;   //---存样柜数据库
	_di_NmlxServiceSoap _ptSoap;
	UTF8String _ListDS;	//---DataSet

	//---处理Json的输入和输出参数
	TJsonReq _jReq;
	TJsonRet _jRet;


	String _LastErrS;

    Thread_Run	*_Thread;

	//---平台调用本系统WebService服务用到的变量
	bool _bForceEndSamUnload;	//---强制结束卸样过程
	String _Cybm;
	String _Zybm;
	String _CYLocation;
	String _ZYLocation;


	bool JsonToList(Json::Value &jv, UTF8String name);
	bool JsonToDataSet_pt(Json::Value &jv, UTF8String name,TClientDataSet *DS);
private:



	TIdHTTPWebBrokerBridge *_FServer_IdHTTPWebBrokerBridge;
};
extern bool	_bRead,bResult;	//---采样机是否需要读卡

#endif
