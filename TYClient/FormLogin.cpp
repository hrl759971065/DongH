// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormLogin.h"
#include "FormMain.h"
#include "Service.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RzPanel"
#pragma link "RzLabel"
#pragma link "RzEdit"
#pragma link "RzCmboBx"
#pragma link "RzButton"
#pragma resource "*.dfm"
TFrmLogin *FrmLogin;

// ---------------------------------------------------------------------------
__fastcall TFrmLogin::TFrmLogin(TComponent* Owner) : TForm(Owner) {
}

// ---------------------------------------------------------------------------
void __fastcall TFrmLogin::RzButton1Click(TObject *Sender) {
	Hide();
	FrmMain->Show();
	return;

	if (C_User->Text.Trim().IsEmpty()) {

		ShowMessage(L"�������û���\r\n");
		return;
	}
	if (E_Pass->Text.Trim().IsEmpty()) {

		ShowMessage(L"����������\r\n");
		return;
	}
	if (global_Service.Login_pt(C_User->Text.Trim(), E_Pass->Text.Trim())) {

		FrmMain->ShowModal();
		Close();
	}
	else {
		ShowMessage(L"��¼ʧ��:" + global_Service.GetLastErrS());
	}

}
// ---------------------------------------------------------------------------

void __fastcall TFrmLogin::RzButton2Click(TObject *Sender) {
	Forms::Application->MainForm->Close();
}
// ---------------------------------------------------------------------------
