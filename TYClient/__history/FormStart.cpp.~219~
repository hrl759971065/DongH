// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormStart.h"
#include "Service.h"
#include "FormLogin.h"
#include "FormMain.h"

#include "Basic.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RzEdit"
#pragma resource "*.dfm"
TFrmStart *FrmStart;

// ---------------------------------------------------------------------------
__fastcall TFrmStart::TFrmStart(TComponent* Owner) : TForm(Owner) {
	/* for(int i =0;i<23;i++) global_Service._CarList->Add("测X000"+Format("%.2d",ARRAYOFCONST((i))));
	 global_Service._ParkingNum_Surplus =21;
	 global_Service._ParkingNum_Total =105;
	 global_Service._ScheduledNum =6;
	 global_Service._ToScheduleNum =23;
	 global_Service.WritePic_DDInfo(); */
	/*
	global_Service._GGTitle = "公告";
	global_Service._GGText =
		"发布会上，上海市消保委委员、上海新消费研究中心主任、上海社科院社会学研究所所长杨雄介绍，在本次调研中，希望到迪士尼游玩的上海意愿消费者呈现大龄化、大众化、大型化三个特点。";
	global_Service.WritePic_GGInfo();
	*/
}

// ---------------------------------------------------------------------------
void __fastcall TFrmStart::FormShow(TObject *Sender) {
	// tmr1->Enabled =true;
}

// ---------------------------------------------------------------------------
void __fastcall TFrmStart::FormCreate(TObject *Sender) {
	AnimateWindow(Handle, 2000, AW_BLEND);
}

// ---------------------------------------------------------------------------
void __fastcall TFrmStart::tmr1Timer(TObject *Sender) {
	tmr1->Enabled = false;
	// AnimateWindow(Handle,3000,AW_HIDE || AW_BLEND);
	Hide();
	if (6 == global_Service._sysInfo.NodeID) {

		FrmLogin->Show();
	}
	else {
		FrmMain->Show();
	}
}

// ---------------------------------------------------------------------------
void __fastcall TFrmStart::FormClose(TObject *Sender, TCloseAction &Action) {
	// Action =caFree;
}
// ---------------------------------------------------------------------------
