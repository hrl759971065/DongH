// ---------------------------------------------------------------------------
// #inclue <vcl.h>
#pragma hdrstop

#include "SelectionPanel.h"
#include "Service.h"
// #include "voConst.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)

// ---自定义消息
#define WM_SELECTION (WM_USER+102)	//---selection面板发出的消息

__fastcall TSelectionPanel::TSelectionPanel(TComponent* Owner) : TRzPanel(Owner)
{
	_Title = NULL;
	for (int row = 0; row < 5; row++) {

		for (int col = 0; col < 4; col++) {

			_Btn[row][col] = NULL;
		}
	}
	_Btn_Switch = NULL;
	Init();
}

__fastcall TSelectionPanel::~TSelectionPanel() {

}

void __fastcall TSelectionPanel::Init() {
	Parent = (TWinControl*)Owner;
	Transparent = true;
	Left = Parent->Width * 0.04;
	Top = Parent->Height * 0.04;
	Width = Parent->Width - Parent->Width * 0.08;
	Height = Parent->Height - Parent->Height * 0.08;

	CreateTitle();
	CreateBtn();
}

void __fastcall TSelectionPanel::CreateTitle() {
	_Title = new TRzLabel(this);
	_Title->Parent = this;
	_Title->Left = 30;
	_Title->Top = 10;
	_Title->Transparent = true;

	_Title->Name = L"L_Title";
	_Title->Font->Name = L"微软雅黑";
	_Title->Font->Size = 16;
	_Title->Font->Style = TFontStyles() << fsBold;
	_Title->Font->Color = clNavy;
}

void __fastcall TSelectionPanel::CreateBtn( /* TWinControl *Parent */) {
	int nBtnWidth = 144;
	int nBtnHeight = 64;

	int nLeft = (this->Width * 0.25 - nBtnWidth) * 0.5; // ---初始左边距
	int nTop = 48; // ---初始右边距
	int nHSpace = nLeft * 2; // ---按钮的水平间距
	int nVSpace = nBtnHeight * 0.45; // ---按钮的纵向间距

	for (int row = 0; row < 5; row++) {

		for (int col = 0; col < 4; col++) {

			_Btn[row][col] = new TButton(this->Handle);
			_Btn[row][col]->Parent = this;
			_Btn[row][col]->Width = nBtnWidth;
			_Btn[row][col]->Height = nBtnHeight;
			_Btn[row][col]->Font->Name = L"微软雅黑";
			_Btn[row][col]->Font->Size = 16;
			_Btn[row][col]->Font->Style = TFontStyles() << fsBold;
			_Btn[row][col]->Font->Color = clNavy;
			_Btn[row][col]->Name = L"Btn_" + IntToStr(row) + L"_" +
				IntToStr(col);
			_Btn[row][col]->Caption = L"煤矿品种" + IntToStr(row * 4 + col + 1);

			_Btn[row][col]->OnClick = BtnClick;

			if (0 == col)
				_Btn[row][col]->Left = nLeft;
			else
				_Btn[row][col]->Left = _Btn[row][col - 1]->Left + nBtnWidth +
					nHSpace;

			if (0 == row)
				_Btn[row][col]->Top = nTop;
			else
				_Btn[row][col]->Top = _Btn[row - 1][col]->Top + nBtnHeight +
					nVSpace;
		}
	}

	_Btn_Switch = new TButton(this->Handle);
	_Btn_Switch->Parent = this;
	_Btn_Switch->Width = nBtnWidth * 1.2;
	_Btn_Switch->Height = nBtnHeight * 0.7;
	_Btn_Switch->Left = Width - _Btn_Switch->Width - 24;
	_Btn_Switch->Top = Height - _Btn_Switch->Height - 16;

	_Btn_Switch->Font->Name = L"楷体";
	_Btn_Switch->Font->Size = 16;
	_Btn_Switch->Font->Style = TFontStyles() << fsBold;
	_Btn_Switch->Font->Color = clNavy;
	_Btn_Switch->Name = L"Btn_Switch";
	_Btn_Switch->Caption = L"切换到查询";
}

void __fastcall TSelectionPanel::BtnClick(TObject *Sender) {
	static String currMeik, currGongys;
	String str = ((TButton*)Sender)->Caption;
	String FormCaption = global_Service._sysInfo.Title + L"  " +
		global_Service._sysInfo.Location;
	HWND hWnd = ::FindWindow(L"TFrmMain", FormCaption.c_str());

	if (_Title->Caption.Pos("煤矿") > 0) {

		currMeik = str;
		// ---继续根据煤矿名选出供应商名
		DynamicArray<String>GongysList;
		global_Service.GetGongysFromDS(currMeik, GongysList);
		_Title->Caption = "供应商选择";
		SetBtnCaption(GongysList);
		str += L"|MK";

	}
	else if (_Title->Caption.Pos("供应商") > 0) {

		currGongys = str;
		// ---继续根据煤矿和供应商名选出品种名称
		DynamicArray<String>PinzList;
		global_Service.GetPinzFromDS(currMeik, currGongys, PinzList);
		_Title->Caption = "煤种选择";
		SetBtnCaption(PinzList);
		str += L"|GYS";
	}
	else if (_Title->Caption.Pos("煤种") > 0) {

		Visible = false;
		str += L"|PZ";
	}
	SendMessage(hWnd, WM_SELECTION, str.Trim().Length(), (long)str.c_str());
}

String TSelectionPanel::GetTitle() {
	return _Title->Caption;
}

void TSelectionPanel::SetTitle(String aVal) {
	_Title->Caption = aVal;
}

/*
 设置按钮上的Caption
 */
void TSelectionPanel::SetBtnCaption(DynamicArray<String>btn_Caption) {
	Visible = false;
	for (int row = 0; row < 5; row++) {

		for (int col = 0; col < 4; col++) {

			_Btn[row][col]->Caption = "";
			_Btn[row][col]->Visible = false;
		}
	}

	int num;
	btn_Caption.Length > 20 ? num = 20 : num = btn_Caption.Length;
	for (int i = 0; i < num; i++) {

		_Btn[i / 4][i % 4]->Caption = btn_Caption[i];
		_Btn[i / 4][i % 4]->Visible = true;
	}
	Visible = true;
}
