#ifndef ComonFunH
#define ComonFunH

#include <Classes.hpp>
#include "LX_DB.h"
//#include "Globals.h"

//---基类 虚方法
class IComonFun_DB{

public:
	//virtual ~IComonFun_DB(void){}
	virtual WideString GetLastErrS() const=0;	//得到最新的错误信息
	virtual bool 	ConnDB()=0;	//---连接数据库
	virtual void 	DisconDB()=0;
    virtual	void	Log_cs(WideString Log,WideString IP,WideString MCode,WideString Mingc,int nType,WideString User="") =0;
	virtual bool 	ReadSysInfo(TSysInfo *pSysInfo) =0;	//从数据库读系统信息
	virtual	bool 	ReadSysInfo_Bang(WideString SysID, int Index,TBangInfo *pBangInfo) =0;	//数据库读取地磅信息
	virtual bool 	CreateBangInfo_templet(WideString ParentID) =0;
	virtual bool 	CreateSysIDInfo_templet() =0;
	virtual bool    DelConfigItems(WideString ParentID,bool bDelSelf) =0; //删除ParentID下的所有配置文件,bDelSelf 是否删除自身

	virtual bool 	IsRun(WideString Location,wchar_t *Status) =0;
	virtual int 	SelCarInfo(WideString cardID,TCarInfo &CarInfo) =0;
	virtual	bool 	GetCarStatus(WideString Cheh,TCarInfo *pCarInfo) =0; //--- <0 异常, 0未验票 1未采样 2未过重 3未回皮 4未打磅单
	virtual bool 	GetCyfs(TCarInfo CarInfo,BYTE &nCyfs) =0;
	virtual bool  	InsCarMaoz(WideString Cheh,WideString Location,double fMaoz) =0;
	virtual bool 	InsCarPiz(WideString Cheh,WideString Location,double fPiz) =0;
	virtual bool 	GetGoodsKind(TCarInfo CarInfo,int &nOutKind) =0;
    virtual bool 	InsWuzData(WideString Cheh,WideString Pinz,WideString Location,double fZhongl) =0;
};

typedef IComonFun_DB* (__stdcall *pFnGetIComonFun_DB)(TDBConfig DbCfg,TFuncInfo FunInfo);

#endif

