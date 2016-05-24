// ---------------------------------------------------------------------------

#ifndef ServJiekH
#define ServJiekH
#include "ServDevCtrl.h"
#include <string>
// ---------------------------------------------------------------------------
using namespace std;

class TServJiek : public TServDevCtrl {
public:
	TServJiek();
	~TServJiek();

	// ---采样机&联合制样机 中间库接口
	bool SamStaInfo(TLX_DB *connKYC, String Location, int &nStatus);
	UTF8String SamStaInfo(TLX_DB *connKYC, UTF8String JsonReq);
	String CYJStaDecode(int nStatus);

	UTF8String CollectorInfo(TLX_DB *connKYC, UTF8String JsonReq);
	UTF8String SamControl(TLX_DB *connKYC, UTF8String JsonReq);
	bool SamControl(TLX_DB *connKYC, String Location, int ActionID);

	UTF8String ZYSamStaInfo(TLX_DB *connKYZ, UTF8String JsonReq);
	bool ZYSamStaInfo(TLX_DB *connKYZ, String Location, String &StatusID);
	String ZYJStaDecode(int nStatus);

	UTF8String ZYSamDetailInfo(TLX_DB *connKYZ, UTF8String JsonReq);
	UTF8String ZYSamControl(TLX_DB *connKYZ, UTF8String JsonReq);
	UTF8String SamUnload(TLX_DB *connKYC, UTF8String JsonReq);
	bool SamUnload(TLX_DB *connKYC, String Location, String Cybm);
	bool ReadSamLoadStatus(TLX_DB *connKYC, String Location, String Cybm);

	UTF8String ZYSamUnload(TLX_DB *connKYZ, UTF8String JsonReq);
	bool ZYSamUnload(TLX_DB *connKYZ, String Location, String Zybm);
	bool ZYSamControl(TLX_DB *connKYZ, String Location, String Zybm, int Openr);
	UTF8String ZYSamUnload_New(UTF8String JsonReq);
	UTF8String ForceEndSamLoad(UTF8String JsonReq);
	bool ReadZyRecord(TLX_DB *connKYZ, String Location, String Zybm);

	void SamUnloadProc(); // ---卸料过程

	// ---平台调用此接口能过读写器读卡号
	UTF8String ReadCard(UTF8String JsonReq);
	UTF8String WriteCard(UTF8String JsonReq);

	// ---LED屏公告
	UTF8String SendLED_Diaod(UTF8String JsonReq);
	UTF8String SendLED_Gongg(UTF8String JsonReq);

	// ---存样柜
	UTF8String CunYG(UTF8String JsonReq);
	void TServJiek::CunYG_QuY();
	String CYGStaDecode(int nStatus);
	String TServJiek::CYGOpType_StaDecode(int OpType);
	bool CunYGStaInfo(TLX_DB *connCYG, String BianM, int OpType, int OpDest,
		UTF8String UserName, int &nStatus);
	bool CunYG_IsRun(TLX_DB *connCYG, int &RunOpType);
	bool CunYG_Insert(TLX_DB *connCYG, String BianM, int OpType, int OpDest,
		UTF8String UserName);
	bool CunYG_IsGetSta(TLX_DB *connCYG);
	UTF8String ForceEndCunYG(UTF8String JsonReq);

	// ----采样机中间库判断
	bool WriteCarInfoToCYJ(TCarInfo &CarInfo, TCaiyInfo &CaiyInfo);
	bool GetCyComplete(CarSamplingData &CayInfo);
	bool UpdateTagInfo(String Location);
	bool QueryCMDState(String Car);
	bool QueryIsStop(String Car);
	bool GetCollectorInfo(SamperCollectionData &CollectInfo);
	bool GetRealTimeCaiYInfo(String Machine);
	bool GetCaiYZBInfo(TCYZBInfo &CYZBInfo);  //----检测采样准备状态
	bool WritePICItoHuay(TCaiyInfo &CaiyInfo);     //----写入化验中间库

	// ---调用平台的WebService
	bool GetServerSystemTime_pt(String &sysTime);
	bool GetGeneralData_pt(UTF8String TableName, TClientDataSet *CDS);
	bool Login_pt(String USer, String Pass);
	bool GetFlowID(String FlowCode, int &FLowID);
	bool GetCarFlowID(String Numplate, int &FLowID,
		UTF8String &FlowDescription);
	bool GetCarCYInfo(String Numplate, TCaiyInfo &CaiyInfo); // 获取采样编码、采样点数、供应商
	bool GetCarInfo_pt(String cardID, TCarInfo &CarInfo);
	bool GetRanmJih_pt(TClientDataSet *ds);
	bool BingdingICCard_pt(String Numplate, String ICCardID, int Action);
	bool SubmitYanp_pt(String Numplate, double fKuangfl, String Meik,
		String Gongys, String Pinz);
	bool SubmitJilInfo_pt(String Numplate, double fZhongl, int JilTag);
	bool SubmitCaiYInfo_pt(UTF8String Car);
	bool SubmitCYZBInfo_pt(TCYZBInfo &CYZBInfo);
	bool SubmitCaiYRecord_pt(CarSamplingData param);
	bool SubmitJiYTInfo_pt(SamperCollectionData param);
	bool EnterExitParking_pt(String Numplate, int Action);
	bool EnterExitMC_pt(String Numplate, int Action, UTF8String Location);
	bool GetNextStation(String Numplate, TStation &Station);
	bool SaveZhiybjData_pt(String MachineCode, String strDec, String strCode);
	bool ChupData_pt(String Numplate, double kous, double koug, double kouz,
		double kouqt, TBangdInfo &BangdInfo);
	bool SaveCaiYZTInfo_pt(String MachineCode, String strTime, String strDec);
	bool SaveCYGYXInfo_pt(String MachineCode, String strTime, String strDec,String strCode);
	bool SaveZhiYZTInfo_pt(String MachineCode, String strTime, String strDec,
		String strCode);
	bool SaveHUAYInfo_pt(THYSInfo &HYSInfo);
	bool SaveQSYInfo_pt(TQSYInfo &QSYInfo);
	bool UpdateCaiYZTInfo_pt(String Machine, String Car);
	bool RealTimeCaiYZTInfo_pt(String Machine, String Car, int Dczb, int Xczb);
	bool UploadXlwcxx_pt(String Cybm, int nIsXL);
	bool SaveCYGInfo_pt(TCYGInfo &CYGInfo);
	bool SaveCYGCQInfo_pt(TCYGCQInfo &CYGCQInfo);
	bool SaveQDGDInfo_pt(TQDGDInfo &QDGDInfo);
protected:

private:
};
extern String Gkdw;
#endif
