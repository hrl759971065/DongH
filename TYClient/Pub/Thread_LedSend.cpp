// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Thread_LedSend.h"
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

__fastcall Thread_LedSend::Thread_LedSend(bool CreateSuspended)
	: TThread(CreateSuspended) {
	FreeOnTerminate = true;
  	_bFree=false;
  	_bClose=false;
}

// ---------------------------------------------------------------------------
void __fastcall Thread_LedSend::Execute() {
	// ---- Place thread code here ----
	while (!Terminated) {

		if(_bClose){
			this->Terminate();
			_bFree=true;
		}

        ::Sleep(100);
		Synchronize(&SyncFun_WritePic);
		for (int i = 0; i < global_Service._LedBmp.picNumDD; i++) {
			if(_bClose) break;
			global_Service.SendPicToLedRam(0,
				global_Service._LedBmp._bmDD[i]->Handle, 0, 0,
				global_Service._DevInfo.LedInfo[0].Width,
				global_Service._DevInfo.LedInfo[0].Height);
			Sleep(5000);
		}

		for (int i = 0; i < global_Service._LedBmp.picNumGG; i++) {
            if(_bClose) break;
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
void __fastcall Thread_LedSend::SyncFun_WritePic() {
	if (global_Service._bSendLED_DDInfo) {

		global_Service.WritePic_DDInfo();
		global_Service._bSendLED_DDInfo = false;
	}
	if (global_Service._bSendLED_GGInfo) {

		global_Service.WritePic_GGInfo();
		global_Service._bSendLED_GGInfo = false;
	}
}
