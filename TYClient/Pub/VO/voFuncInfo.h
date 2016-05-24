// ---Developer高级设置

#ifndef voFuncInfoH
#define voFuncInfoH

#include <Classes.hpp>

// ---煤场常用函数配置
class TMCFuncInfo {
public:
	TMCFuncInfo() {
		Init();
	}

	void Init() {
		FIsRun_FunName = "";
		FSelCarInfo_FunName = "";
		FGetCarStatus_FunName = "";
		FGetCyfs_FunName = "";
		FInsCarMaoz_FunName = "";
		FInsCarPiz_FunName = "";
		FGetGoodsKind_FunNuame = "";
	}

public:
	__property WideString IsRun_FunName = {
		read = FIsRun_FunName, write = FIsRun_FunName}; // ---子系统启停用判断函数名
	__property WideString SelCarInfo_FunName = {
		read = FSelCarInfo_FunName, write = FSelCarInfo_FunName
	}; // ---车辆信息查询函数名
	__property WideString GetCarStatus_FunName = {
		read = FGetCarStatus_FunName, write = FGetCarStatus_FunName
	}; // ---车辆状态查询函数名
	__property WideString GetCyfs_FunName = {
		read = FGetCyfs_FunName, write = FGetCyfs_FunName}; // ---查询采样方式函数名
	__property WideString InsCarMaoz_FunName = {
		read = FInsCarMaoz_FunName, write = FInsCarMaoz_FunName}; // ---记录汽车毛重
	__property WideString InsCarPiz_FunName = {
		read = FInsCarPiz_FunName, write = FInsCarPiz_FunName}; // ---记录汽车皮重
	__property WideString GetGoodsKind_FunNuame = {
		read = FGetGoodsKind_FunNuame, write = FGetGoodsKind_FunNuame
	}; // ---读取物料分类：厂煤 物资车 等

private:
	WideString FIsRun_FunName;
	WideString FSelCarInfo_FunName;
	WideString FGetCarStatus_FunName;
	WideString FGetCyfs_FunName;
	WideString FInsCarMaoz_FunName;
	WideString FInsCarPiz_FunName;
	WideString FGetGoodsKind_FunNuame;
};

class TFuncInfo {
public:
	__property TMCFuncInfo MCFuncInfo = {read = FMCFuncInfo, write = FMCFuncInfo
	};

private:
	TMCFuncInfo FMCFuncInfo;
};

#endif
