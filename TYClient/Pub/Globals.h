//---------------------------------------------------------------------------

#ifndef GlobalsH
#define GlobalsH

#include <Classes.hpp>
#include "voBaseInfo.h"
#include "voBangInfo.h"
#include "voConst.h"
//#include <System.Classes.hpp>
//#include <Vcl.Controls.hpp>
//#include <Vcl.StdCtrls.hpp>
//---------------------------------------------------------------------------



/*typedef struct struct_DataSetInfo{

	int nRecordCount;   //记录数
	int nCols;  //列数
	int nCursor;    //当前游标
}DataSetInfo,*pDataSetInfo; */
//--------------------------------------------------------------------------

/*typedef struct tagDATAPKG{

	char *data;
	int  nLen;
}DATAPKG,*pDATAPKG; */
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

//---设备信息



//---以下4种设备都属于IO控制类型的设备，有可能使用同一控制器控制



/*typedef struct tagDEVICEINFO_DZ{	//---道匣

	wchar_t 		Model[32];			//---道匣或控制器型号
	NETDEVICE_CFG   NetCfg;     	//---网络配置
	COMM_CFG 		Comm;  		  //---串口配置
	int 			nCtrModel;		//---0：串口 1：网口
	//---起 落 停 指令
	char 			UP_Cmd[256];
	char 		    DOWN_Cmd[256];
	char 			STOP_Cmd[256];
}DEVICEINFO_DZ,*pDEVICEINFO_DZ;   */

/*typedef struct tagDEVICEINFO_HLD{	//---红绿灯

	wchar_t 			Model[32];			//---红绿灯或控制器型号
	NETDEVICE_CFG   NetCfg;     	//---网络配置
	COMM_CFG 		Comm;  		  //---串口配置
	int 			nCtrModel;		//---0：串口 1：网口
	//---红 绿 灭 指令
	char Red_Cmd[256];
	char Green_Cmd[256];
	char Close_Cmd[256];
}DEVICEINFO_HLD,*pDEVICEINFO_HLD;*/

/*typedef struct tagDEVICEINFO_DWQ{	//---定位器

	wchar_t 			Model[32];			//---定位器控制器型号
	NETDEVICE_CFG   NetCfg;     	//---网络配置
	COMM_CFG 		Comm;  		  //---串口配置
	int 			nCtrModel;		//---0：串口 1：网口

	//---指令
	char sel_Cmd[256];	//---查询
	char FrontFail_Echo[256];	//前置被挡反馈
	char BackFail_Echo[256];	//后置被挡反馈
	char AllFail_Echo[256];	//全部被挡反馈
	char AllPass_Echo[256];	//全部通过反馈
}DEVICEINFO_DWQ,*pDEVICEINFO_DWQ;*/
//-------------------------------------------------------------------------




//---Developer高级设置


//---基础信息


/*typedef struct tagCARSTATUS{	//---车辆状态

	wchar_t 	NumPlate[16];    //---车号
	wchar_t		Gongys[48];		 //---供应商
	wchar_t		Meik[48];		 //---煤矿
	wchar_t		Pinz[48];        //---品种
	wchar_t		Yscd[48];		 //---运输车队
	int			nIsCy;           //---0未采样 1已采样
	int			nIsWeight;       //---0未过重 1已过重
	int			nIsTare;         //---0未回皮 1已回皮
	TDateTime   tPpsj;			 //---匹配时间
}CARSTATUS,*pCARSTATUS; */

/*typedef struct UDP_PACKED_TMP_STRUCT{
    //---本地配置
    BYTE sLocation[16];     //---本地名
	BYTE sHostIP[16];   	//---主机IP
	//---天线配置
	BYTE sReaderIP[16]; 	//---天线IP
	BYTE nReaderAnt;    	//---天线端口
	BYTE bConn; 			//---天线是否连接
	BYTE sCar[12];  		//---当前车牌号
	BYTE sCurrStatus[32];   //---当前状态
	//---地磅仪表
	BYTE sMeterCom;   		//---串口号
	BYTE sRealValue[4]; 	//---地磅值
    //---道闸
    BYTE nDzCom;
	BYTE bDzStatus;  		//---0落 1抬
	//---交通灯
	BYTE nHldCom;    		//---红绿灯串口号
	BYTE nHldStatus; 		//---0红 1绿
	//红外对射状态
    BYTE  nDwqCom;   		//---定位器串口号
    BYTE bFront;            //---0挡 1通
    BYTE bBack;
}UDP_PACKED_TMP,*pUDP_PACKED_TMP;   */

#endif

