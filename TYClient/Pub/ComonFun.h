#ifndef ComonFunH
#define ComonFunH

#include <Classes.hpp>
#include "LX_DB.h"
//#include "Globals.h"

//---���� �鷽��
class IComonFun_DB{

public:
	//virtual ~IComonFun_DB(void){}
	virtual WideString GetLastErrS() const=0;	//�õ����µĴ�����Ϣ
	virtual bool 	ConnDB()=0;	//---�������ݿ�
	virtual void 	DisconDB()=0;
    virtual	void	Log_cs(WideString Log,WideString IP,WideString MCode,WideString Mingc,int nType,WideString User="") =0;
	virtual bool 	ReadSysInfo(TSysInfo *pSysInfo) =0;	//�����ݿ��ϵͳ��Ϣ
	virtual	bool 	ReadSysInfo_Bang(WideString SysID, int Index,TBangInfo *pBangInfo) =0;	//���ݿ��ȡ�ذ���Ϣ
	virtual bool 	CreateBangInfo_templet(WideString ParentID) =0;
	virtual bool 	CreateSysIDInfo_templet() =0;
	virtual bool    DelConfigItems(WideString ParentID,bool bDelSelf) =0; //ɾ��ParentID�µ����������ļ�,bDelSelf �Ƿ�ɾ������

	virtual bool 	IsRun(WideString Location,wchar_t *Status) =0;
	virtual int 	SelCarInfo(WideString cardID,TCarInfo &CarInfo) =0;
	virtual	bool 	GetCarStatus(WideString Cheh,TCarInfo *pCarInfo) =0; //--- <0 �쳣, 0δ��Ʊ 1δ���� 2δ���� 3δ��Ƥ 4δ�����
	virtual bool 	GetCyfs(TCarInfo CarInfo,BYTE &nCyfs) =0;
	virtual bool  	InsCarMaoz(WideString Cheh,WideString Location,double fMaoz) =0;
	virtual bool 	InsCarPiz(WideString Cheh,WideString Location,double fPiz) =0;
	virtual bool 	GetGoodsKind(TCarInfo CarInfo,int &nOutKind) =0;
    virtual bool 	InsWuzData(WideString Cheh,WideString Pinz,WideString Location,double fZhongl) =0;
};

typedef IComonFun_DB* (__stdcall *pFnGetIComonFun_DB)(TDBConfig DbCfg,TFuncInfo FunInfo);

#endif
