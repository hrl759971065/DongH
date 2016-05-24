// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormMain.h"
#include "Service.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RzPanel"
#pragma link "RzTabs"
#pragma link "RzSplit"
#pragma link "RzStatus"
#pragma link "RzGrids"
#pragma link "RzEdit"
#pragma link "RzLabel"
#pragma link "RzButton"
#pragma link "RzRadChk"
#pragma link "RzSndMsg"
#pragma link "RzDBGrid"
#pragma link "RzBtnEdt"
#pragma link "RzLstBox"
#pragma link "RzPrgres"
#pragma link "RzBorder"
#pragma link "RzListVw"
#pragma link "RzBmpBtn"
#pragma resource "*.dfm"

using namespace Vcl::Forms;

TFrmMain *FrmMain;

// ---------------------------------------------------------------------------
void __fastcall TFrmMain::MsgOper_Selection(Messages::TMessage &Message) {
	String strMsg = String((wchar_t *)Message.LParam);
	/* 暂不用组件ID值的方法啦
	 if(_CurrentIndex ==P_Meik->ComponentIndex) P_Meik->Caption =strMsg;
	 else if(_CurrentIndex ==P_Gongys->ComponentIndex) P_Gongys->Caption =strMsg;
	 else if(_CurrentIndex ==P_Pinz->ComponentIndex) P_Pinz->Caption =strMsg; */

	// ---用strMsg的值来判断
	String strItem = strMsg.SubString(strMsg.Pos("|") + 1,
		strMsg.Length() - strMsg.Pos("|"));
	String strVal = strMsg.SubString(1, strMsg.Pos("|") - 1);
	if (strItem == L"MK") {

		P_Meik->Font->Size = 14;
		P_Meik->Font->Color = clNavy;
		P_Meik->Font->Style = TFontStyles() << fsBold >> fsUnderline;
		P_Meik->Caption = strVal;

		P_Gongys->Font->Size = 18;
		P_Gongys->Font->Color = clRed;
		P_Gongys->Font->Style = TFontStyles() << fsBold << fsUnderline;
		P_Gongys->Caption = L"请选择";
	}
	else if (strItem == L"GYS") {

		P_Gongys->Font->Size = 14;
		P_Gongys->Font->Color = clNavy;
		P_Gongys->Font->Style = TFontStyles() << fsBold >> fsUnderline;
		P_Gongys->Caption = strVal;

		P_Pinz->Font->Size = 18;
		P_Pinz->Font->Color = clRed;
		P_Pinz->Font->Style = TFontStyles() << fsBold << fsUnderline;
		P_Pinz->Caption = L"请选择";
	}
	else if (strItem == L"PZ") {

		P_Pinz->Font->Size = 14;
		P_Pinz->Font->Color = clNavy;
		P_Pinz->Font->Style = TFontStyles() << fsBold >> fsUnderline;
		P_Pinz->Caption = strVal;
		Btn_Ok->Visible = true; // ---显示确认和取消按钮
		Btn_Cancel->Visible = true;
	}
}

void __fastcall TFrmMain::JilListMsg(Messages::TMessage &Message) {
	String strMsg = String((wchar_t *)Message.LParam);

	int nXuh = Lv_Jil->Items->Count + 1;
	TListItem *item = Lv_Jil->Items->Insert(0);
	item->Caption = IntToStr(nXuh);
	item->SubItems->Add(strMsg.SubString(1, strMsg.Pos("^") - 1));
	item->SubItems->Add(Now().DateTimeString());
	item->SubItems->Add(strMsg.SubString(strMsg.Pos("^") + 1,
		strMsg.Length() - strMsg.Pos("^")));
	TForm::Dispatch(&Message);
}

void TFrmMain::DBGridRecordSize(TColumn *mColumn) {

}

__fastcall TFrmMain::TFrmMain(TComponent* Owner) : TForm(Owner) {
	global_Service.LoadingThr();
	_CurrentIndex = -1;
	global_Service.InitSoap();

	// ---Test  global_Service.SubmitYanp_pt(L"蒙BDW122",L"神华",L"立信","煤");
}

// ---------------------------------------------------------------------------
void __fastcall TFrmMain::RzButton4Click(TObject *Sender) {
	ClientDataSet1->Close();
	ClientDataSet1->Filter = "性别='男'";
	ClientDataSet1->Filtered = true;
	ClientDataSet1->Open();
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button1Click(TObject *Sender) {
	// ---Test
	// UTF8String str =global_Service.SamStaInfo(Memo1->Text);
	// return;
	if (!global_Service.GetGeneralData_pt(Memo1->Text, ClientDataSet1)) {
		ShowMessage(global_Service.GetLastErrS());
	}

	global_Service.MakeDBGridColumnsAutoFixItsWidth(RzDBGrid1);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::RzDBGrid1DrawColumnCell(TObject *Sender,
	const TRect &Rect, int DataCol, TColumn *Column, TGridDrawState State) {

	if (gdSelected == State.ToInt()) {
		return;
	}

	if (((TDBGrid*)Sender)->DataSource->DataSet->RecNo % 2) {

		((TDBGrid*)Sender)->Canvas->Brush->Color = clRed; // ---定义背景颜色
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::FormClose(TObject *Sender, TCloseAction &Action) {
	Action = caFree;
	if (global_Service._sysInfo.NodeID != -1) {
		tmr_Exit->Enabled = true;
		global_Service._StopThr();
	}
	Forms::Application->MainForm->Close();
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button2Click(TObject *Sender) {
	// global_Service.FormStyle(this,0);
	global_Service.StopServ();
	Forms::Application->Terminate();
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Btn_SelMKClick(TObject *Sender) {
	Img_Oper->Visible = false;

	P_Meik->Font->Size = 18;
	P_Meik->Font->Color = clRed;
	P_Meik->Font->Style = TFontStyles() << fsBold << fsUnderline;
	P_Meik->Caption = L"请选择";

	_CurrentIndex = P_Meik->ComponentIndex;
	global_Service._Selection->Title = L"煤矿快捷选择";

	DynamicArray<String>MeikList;
	global_Service.GetAllMeikFromDS(MeikList);
	global_Service._Selection->SetBtnCaption(MeikList);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::RzButton7Click(TObject *Sender) {
	/* _CurrentIndex =P_Pinz->ComponentIndex;
	 global_Service._Selection->Title =L"煤种选择"; */
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::tmr_ExitTimer(TObject *Sender) {
	SP_Status->Caption = global_Service._ThrMsg;
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::RzButton8Click(TObject *Sender) {
	/* _CurrentIndex =P_Gongys->ComponentIndex;
	 global_Service._Selection->Title =L"供应商选择"; */
}
// ---------------------------------------------------------------------------

// ---更新界面值
void __fastcall TFrmMain::UpdJiem() {
	// ---更新道闸状态
	if (0 == global_Service._WorkSta.DZSta_Head) {
		Img_Down_Head->Visible = false;
		// 卸煤沟不显示道闸
		if (global_Service._sysInfo.NodeID == 4 &&
			global_Service._sysInfo.TingID == 2)
			Img_Up_Head->Visible = false;
		else
			Img_Up_Head->Visible = true;
	}
	else {
		// 卸煤沟不显示道闸
		if (global_Service._sysInfo.NodeID == 4 &&
			global_Service._sysInfo.TingID == 2)
			Img_Down_Head->Visible = false;
		else
			Img_Down_Head->Visible = true;
		Img_Up_Head->Visible = false;
	}

	if (0 == global_Service._WorkSta.DZSta_Tail) {
		Img_Down_Tail->Visible = false;
		Img_Up_Tail->Visible = true;
	}
	else if (1 == global_Service._WorkSta.DZSta_Tail) {
		Img_Down_Tail->Visible = true;
		Img_Up_Tail->Visible = false;
	}
	// ---红外对射
	if (-2 == global_Service._WorkSta.DWQSta_Head) {
		shp_DWQ_Head->Brush->Color = clWhite;
	}
	else if (-1 == global_Service._WorkSta.DWQSta_Head) {
		shp_DWQ_Head->Brush->Color = clSilver;
	}
	else if (0 == global_Service._WorkSta.DWQSta_Head) {
		shp_DWQ_Head->Brush->Color = clLime;
	}
	else if (1 == global_Service._WorkSta.DWQSta_Head) {
		shp_DWQ_Head->Brush->Color = clRed;
	}

	if (-2 == global_Service._WorkSta.DWQSta_Tail) {
		shp_DWQ_Tail->Brush->Color = clWhite;
	}
	else if (-1 == global_Service._WorkSta.DWQSta_Tail) {
		shp_DWQ_Tail->Brush->Color = clSilver;
	}
	else if (0 == global_Service._WorkSta.DWQSta_Tail) {
		shp_DWQ_Tail->Brush->Color = clLime;
	}
	else if (1 == global_Service._WorkSta.DWQSta_Tail) {
		shp_DWQ_Tail->Brush->Color = clRed;
	}

	switch (global_Service._sysInfo.NodeID) {

	case 0: // ---自助入厂
		switch (global_Service._Sequence) {

		case 0:
			Btn_OkNumplate->Caption = L"扫描中";
			Btn_OkNumplate->Enabled = false;
			// Btn_SelMK->Enabled =false;
			Btn_Ok->Visible = false;
			Btn_Cancel->Visible = false;
			break;
		case 1:
			Btn_OkNumplate->Caption = L"读取中";
			Btn_OkNumplate->Enabled = false;
			// Btn_SelMK->Enabled =false;
			Btn_Ok->Visible = false;
			Btn_Cancel->Visible = false;
			break;
		case 2:
			Btn_OkNumplate->Caption = L"判断中";
			Btn_OkNumplate->Enabled = false;
			// Btn_SelMK->Enabled =false;
			Btn_Ok->Visible = false;
			Btn_Cancel->Visible = false;
			break;
		case 7: // ---卡已到取卡口,不能取消
			Btn_Cancel->Visible = false;
			break;
		}

		if (3 == global_Service._Sequence) {

			P_Numplate->Caption = global_Service._WorkSta.Car;
			if (Btn_OkNumplate->Caption != L"已确认") {

				Btn_OkNumplate->Caption = L"确认";
				Btn_OkNumplate->Enabled = true;
			}

			Btn_Cancel->Visible = true;
			// Btn_SelMK->Enabled =true;
		}

		if (global_Service._Sequence < 3) {
			P_Numplate->Caption = L"";
			P_Meik->Caption = L"";
			P_Gongys->Caption = L"";
			P_Pinz->Caption = L"";
			P_KFL->Caption = L"0.00";
		}

		break;
	case 1:
		// ---状态更新
		E_QCHSta->Text = global_Service._WorkSta.StrRun;
		E_QCHCardID->Text = global_Service._WorkSta.CardID;
		E_QCHCar->Text = global_Service._WorkSta.Car;
		E_QCHFlow->Text = global_Service._WorkSta.StrFlow;
		E_QCHLed->Text =
			global_Service._WorkSta.StrLed[global_Service._sysInfo.TingID];
		break;
	case 2:
		E_QCHSta->Text = global_Service._WorkSta.StrRun;
		E_QCHCardID->Text = global_Service._WorkSta.CardID;
		E_QCHCar->Text = global_Service._WorkSta.Car;
		E_QCHFlow->Text = global_Service._WorkSta.StrFlow;
		E_QCHLed->Text = global_Service._WorkSta.StrLed[0];
		break;
	case 3:
		// ---状态更新
		E_QCHSta->Text = global_Service._WorkSta.StrRun;
		E_QCHCardID->Text = global_Service._WorkSta.CardID;
		E_QCHCar->Text = global_Service._WorkSta.Car;
		E_QCHFlow->Text = global_Service._WorkSta.StrFlow;
		E_QCHLed->Text = global_Service._WorkSta.StrLed[0];
		break;
	case 4:
		// ---状态更新
		E_QCHSta->Text = global_Service._WorkSta.StrRun;
		E_QCHCardID->Text = global_Service._WorkSta.CardID;
		E_QCHCar->Text = global_Service._WorkSta.Car;
		E_QCHFlow->Text = global_Service._WorkSta.StrFlow;
		E_QCHLed->Text =
			global_Service._WorkSta.StrLed
			[1 == global_Service._sysInfo.TingID ? 1 : 0];
		break;
	case 5: // ---自助出场
		switch (global_Service._Sequence) {

		case 1:
			L_GTis->Caption = L"请将磁卡插入右下方卡片回收口中";
			break;
		case 2:
			L_GTis->Caption = L"正在读取卡片信息，请稍后," + Now().TimeString();
			break;
		case 3:
			L_GTis->Caption = L"正在读取车辆信息，请稍后," + Now().TimeString();
			break;
		case 5:
			L_GTis->Caption = L"卡内车号与系统不符，请取回您的卡," + Now().TimeString();
			global_Service._Sequence = -1;
			break;
		case 6: // ---司机已取回卡
			P_GetTicket->Visible = false;
			// B_GetTicket->Enabled =true;
			B_GetTicket->Visible = true;
			GB_GetTicket->Visible = false;
			break;
		case 7:
			L_GTis->Caption = L"正在打印磅单，请稍后," + Now().TimeString();
			break;
		case 8:
			L_GTis->Caption = L"磅单打印完毕，请取票," + Now().TimeString();
			break;
		case 9: // ---司机取票后
			// P_GetTicket->Visible =false;
			// B_GetTicket->Enabled =true;
			// B_GetTicket->Visible =true;
			break;
		case 10: // ---已收卡到票箱
			P_GetTicket->Visible = false;
			GB_GetTicket->Visible = false;
			// B_GetTicket->Enabled =true;
			B_GetTicket->Visible = true;
		}
		break;
	}
	SP_Status->Caption = global_Service._WorkSta.StrFlow;
	SP_Numplate->Caption = global_Service._WorkSta.Car;
	SP_CardID->Caption = global_Service._WorkSta.CardID;
}

void __fastcall TFrmMain::tmr_JiemTimer(TObject *Sender) {
	tmr_Jiem->Enabled = false;
	global_Service.GetDWQSta();
	UpdJiem();
	tmr_Jiem->Enabled = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Btn_OkNumplateClick(TObject *Sender) {
	// ---应改为先录入矿发量,在矿发量的确认按钮中执行下面的代码

	if (((TRzButton*)Sender)->Caption == L"确认") {

		((TRzButton *)Sender)->Caption = L"已确认";
		((TRzButton *)Sender)->Enabled = false;
		Btn_Cancel->Visible = true;
		Img_Oper->Visible = false;

		// ---录入矿发量   16.4.22 Wuwenbin
		GB_KFL->Visible = true;

		P_KFL->Font->Size = 18;
		P_KFL->Font->Color = clRed;
		P_KFL->Font->Style = TFontStyles() << fsBold << fsUnderline;
		P_KFL->Caption = L"0.00";

		global_Service._Selection->Title = L"请录入矿发净重";
		global_Service._File.PlayWav(L"请录入矿发净重.wav");

		global_Service._Sequence = 4; // ---告诉线程司机已确认车辆信息
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Btn_OkClick(TObject *Sender) {
	// ---制卡 并提示司机取卡
	global_Service._fKuangfl = P_KFL->Caption.ToDouble(); // ---16.4.22 Wuwenbin

	global_Service._Meik = P_Meik->Caption;
	global_Service._Gongys = P_Gongys->Caption;
	global_Service._Pinz = P_Pinz->Caption;
	global_Service._Sequence = 5; // ---告诉线程进入发卡状态并跳出等待循环

	Btn_Ok->Visible = false;
	// Btn_Cancel->Visible =false;
	GB_Tis->Visible = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Btn_CancelClick(TObject *Sender) {
	Img_Oper->Visible = true;
	global_Service._Selection->Visible = false; // ---不显示 煤矿选择菜单
	global_Service._Sequence = 6; // ---告诉线程取消发卡并跳出等待循环

	Btn_Ok->Visible = false;
	Btn_Cancel->Visible = false;
	global_Service._bReset = true; // ---强制流程复位
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::tmr_TisTimer(TObject *Sender) {
	// _Sequence =4;	//---告诉线程进入发卡状态并跳出等待循环
	if (0 == global_Service._sysInfo.NodeID) {

		if (global_Service._Sequence >= 5) {

			if (RzProgressBar1->Percent >= 100)
				RzProgressBar1->Percent = 0;
			RzProgressBar1->Percent += 5;
			L_Tis->Caption = global_Service._strFaKaTis;
		}
		else {
			GB_Tis->Visible = false;
		}

		if (global_Service._Sequence < 4) {

			Img_Oper->Visible = true;
		}

		if (0 == global_Service._ErrCnt) {

			GB_Guz->Visible = false;
			GB_Guz_CC->Visible = false;
		}
		else {
			RE_Beiz->Text = global_Service._WorkSta.StrFlow;
			GB_Guz->Visible = true;
			GB_Guz_CC->Visible = true;
		}
	}
	else if (5 == global_Service._sysInfo.NodeID) {

		if (0 == global_Service._ErrCnt) {

			GB_Guz_CC->Visible = false;
		}
		else {
			RE_Beiz->Text = global_Service._WorkSta.StrFlow;
			GB_Guz_CC->Visible = true;
		}
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::SP_StatusDblClick(TObject *Sender) {
	M_Load->Visible = !M_Load->Visible;
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::tmr_RealValTimer(TObject *Sender) {
	if (3 == global_Service._sysInfo.NodeID) { // ---汽车衡
		tmr_RealVal->Enabled = false;
		LED_Meter->Caption = FloatToStr(global_Service.GetRealVal());
		tmr_RealVal->Enabled = true;
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::tmr_ZDTimer(TObject *Sender) {
	static double fx = 0;
	if (fx > 100) {
		fx = 0;
	}
	double tmpX;

	for (int t = 0; t < Chart1->SeriesCount(); t++) {

		tmpX = Chart1->Series[t]->XValues->Value[1] -
			Chart1->Series[t]->XValues->Value[0];
		Chart1->Series[t]->Delete(0);
		Chart1->Series[t]->AddXY(Chart1->Series[t]->XValues->Last() + tmpX,
			global_Service.GetRealVal(), Now().FormatString("ss"), clTeeColor);
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button3Click(TObject *Sender) {
	global_Service.OutCtrl(DZ, HEAD, UP);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button4Click(TObject *Sender) {
	global_Service.OutCtrl(DZ, HEAD, DOWN);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::B_DZ_UP_TailClick(TObject *Sender) {
	global_Service.OutCtrl(DZ, TAIL, UP);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::B_DZ_DOWN_TailClick(TObject *Sender) {
	global_Service.OutCtrl(DZ, TAIL, DOWN);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button5Click(TObject *Sender) {
	TBangdInfo bdInfo;
	bdInfo.Gongysmc = "测试供应商";
	bdInfo.Jingz = 30.5;
	bdInfo.Koug = 0.5;
	bdInfo.Kous = 0.1;
	bdInfo.Kouz = 0.4;
	bdInfo.Maoz = 71.5;
	bdInfo.Zhongcsj = "2016-4-6 21:42:21";
	bdInfo.Meikmc = "测试煤矿";
	bdInfo.Numplate = "测X00001";
	bdInfo.Wulmc = "混煤";
	bdInfo.Piz = 40;
	bdInfo.Qingcsj = "2016-4-6 22:42:21";
	bdInfo.Printer = "测试员";
	bdInfo.PrtTime = "2016-4-6 22:52:21";
	global_Service.PrintBangd(bdInfo);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button6Click(TObject *Sender) {
	BYTE szStatusInfo[5] = {0};
	global_Service.DeviceStatus(2, szStatusInfo);
	String strStatus = global_Service.DevStatusDecode(szStatusInfo);
	ShowMessage(L"状态:" + strStatus);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button7Click(TObject *Sender) {
	TCardInfo CardInfo;
	if (global_Service.ReadCardID(2, CardInfo)) {
		ShowMessage(L"卡号:" + String((char *)CardInfo.ICCardID_Hex));
	}
	else {
		ShowMessage(L"读卡失败:" + global_Service.GetLastErrS());
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::NumBntClick(TObject *Sender) {
	if (P_KFL_LR->Caption.Pos("矿") > 0)
		P_KFL_LR->Caption = L"";
	String strVal = IntToStr(((TButton*)Sender)->Tag);
	String strCpt = P_KFL_LR->Caption;
	if (10 == strVal.ToInt()) {

		if (strCpt.Pos(".") > 0)
			return;
		strCpt += L".";
		P_KFL_LR->Caption = strCpt;
	}
	else if (11 == strVal.ToInt()) {

		strCpt.Delete(strCpt.Length(), 1);
		P_KFL_LR->Caption = strCpt;
	}
	else {
		if (strVal == L"0") {

			// ---判断是否存在非0
			bool bResult = true;
			for (int i = 0; i < strCpt.Length(); i++) {

				bResult = false;
				if (strCpt[i + 1] != '0') {

					bResult = true;
					break;
				}
			}
			if (!bResult)
				return;
		}

		strCpt += strVal;
		P_KFL_LR->Caption = strCpt;
	}
}

void __fastcall TFrmMain::Btn_KflOkClick(TObject *Sender) {
	// ---16.4.22 Wuwenbin
	String strKfl = P_KFL_LR->Caption;
	double fKFL;
	if (!TryStrToFloat(strKfl, fKFL)) {

		ShowMessage(L"您输入的不是有效的数字,请重新输入");
		return;
	}

	if (fKFL < 10 || fKFL > 128) {

		ShowMessage(L"请输入正确的矿发净重");
		return;
	}

	P_KFL_LR->Caption = L"矿发净重 单位:吨(T)";

	P_KFL->Caption = strKfl;

	GB_KFL->Visible = false;

	P_Meik->Font->Size = 18;
	P_Meik->Font->Color = clRed;
	P_Meik->Font->Style = TFontStyles() << fsBold << fsUnderline;
	P_Meik->Caption = L"请选择";

	_CurrentIndex = P_Meik->ComponentIndex;
	global_Service._Selection->Title = L"请选择煤矿";
	global_Service._File.PlayWav(L"请选择煤矿.wav");

	DynamicArray<String>MeikList;
	global_Service.GetAllMeikFromDS(MeikList);
	global_Service._Selection->SetBtnCaption(MeikList);
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button8Click(TObject *Sender) {
	int nSector = 1;
	int nBlock = 0;
	UTF8String strPsw = "FFFFFFFFFFFF";
	UTF8String strData = Edit1->Text.Trim();

	bool bResult = global_Service.WriteICCard(2, 1, 0, strPsw, strData);
	if (!bResult) {
		ShowMessage(L"写IC卡失败:" + global_Service.GetLastErrS());
	}
	else {
		ShowMessage(L"写IC卡成功");
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::B_GetTicketClick(TObject *Sender) {
	GB_GetTicket->Visible = true;
	P_GetTicket->Visible = true;
	P_GTis->Visible = false;
	// B_GetTicket->Enabled =false;
	B_GetTicket->Visible = false;
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::B_GetTicket_RCMClick(TObject *Sender) {
	// ---弹出提示，并将收卡口设置为自动收卡模式
	global_Service._Sequence = 1; // ---进入自动收卡模式
	global_Service._File.PlayWav("插卡提示.wav");
	Sleep(1000);
	P_GetTicket->Visible = false;
	P_GTis->Visible = true;

}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Btn_CClick(TObject *Sender) {
	B_GetTicket->Visible = true;
	GB_GetTicket->Visible = false;
	// B_GetTicket->Enabled =true;
	global_Service._bReset = true;
}

void __fastcall TFrmMain::Button10Click(TObject *Sender) {
	_bRead = true;
	bResult = true;
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::Button11Click(TObject *Sender) {
	FrmMain->Visible = false;
}

// ---------------------------------------------------------------------------
void __fastcall TFrmMain::Timer1Timer(TObject *Sender) {
	if (global_Service._bEnterCunYG) { // ---haoriliang 16.5.21新增

		global_Service.CunYG_QuY();
	}
	::Sleep(100);
	if (global_Service._bEnterSamUnload) { // ---Wuwenbin16.4.28新增

		global_Service.SamUnloadProc();
	}
}
// ---------------------------------------------------------------------------

void __fastcall TFrmMain::tmr_WebServiceTimer(TObject *Sender) {
	if (global_Service._bSyncSoapData) {

		RE_WebService->Lines->Add(Now().DateTimeString() + L":");
		RE_WebService->Lines->Add(L"请求数据：" + global_Service._strReqJson);
		RE_WebService->Lines->Add(L"回复数据：" + global_Service._strRetJson);
		global_Service._bSyncSoapData = false;
	}
}
// ---------------------------------------------------------------------------


