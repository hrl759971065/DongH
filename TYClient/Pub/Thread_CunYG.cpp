// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Thread_CunYG.h"
#include "Service.h"
#pragma package(smart_init)
// ---------------------------------------------------------------------------

// Important: Methods and properties of objects in VCL can only be
// used in a method called using Synchronize, for example:
//
// Synchronize(&UpdateCaption);
//
// where UpdateCaption could look like:
//
// void __fastcall Thread_LedSend::UpdateCaption()
// {
// Form1->Caption = "Updated in a thread";
// }
// ---------------------------------------------------------------------------

__fastcall Thread_CunYG::Thread_CunYG(bool CreateSuspended)
	: TThread(CreateSuspended) {
	FreeOnTerminate = true;
}

// ---------------------------------------------------------------------------
void __fastcall Thread_CunYG::Execute() {
	// ---- Place thread code here ----
	while (!Terminated) {

		Synchronize(&SyncFun_WritePic);
		for (int i = 0; i < global_Service._LedBmp.picNumDD; i++) {

		 bool A= global_Service.SendPicToLedRam(0,
				global_Service._LedBmp._bmDD[i]->Handle, 0, 0,
				global_Service._DevInfo.LedInfo[0].Width,
				global_Service._DevInfo.LedInfo[0].Height);
			Sleep(5000);
		}

		for (int i = 0; i < global_Service._LedBmp.picNumGG; i++) {

			global_Service.SendPicToLedRam(0,
				global_Service._LedBmp._bmGG[i]->Handle, 0, 0,
				global_Service._DevInfo.LedInfo[0].Width,
				global_Service._DevInfo.LedInfo[0].Height);
			Sleep(5000);
		}
		Sleep(100);
	}
}

// ---------------------------------------------------------------------------
void __fastcall Thread_CunYG::SyncFun_WritePic() {
	if (global_Service._bSendLED_DDInfo) {

		global_Service.WritePic_DDInfo();
		global_Service._bSendLED_DDInfo = false;
	}
	if (global_Service._bSendLED_GGInfo) {

		global_Service.WritePic_GGInfo();
		global_Service._bSendLED_GGInfo = false;
	}
}
