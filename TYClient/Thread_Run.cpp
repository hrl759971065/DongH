// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Thread_Run.h"
#pragma package(smart_init)
// ---------------------------------------------------------------------------

// Important: Methods and properties of objects in VCL can only be
// used in a method called using Synchronize, for example:
//
// Synchronize(&UpdateCaption);
//
// where UpdateCaption could look like:
//
// void __fastcall Thread_Run::UpdateCaption()
// {
// Form1->Caption = "Updated in a thread";
// }
// ---------------------------------------------------------------------------

__fastcall Thread_Run::Thread_Run(bool CreateSuspended)
	: TThread(CreateSuspended) {
	_fpProc = NULL;
	_bKill = true;
}

// ---------------------------------------------------------------------------
void __fastcall Thread_Run::Execute() {
	try {
		_bKill = false;
		CoInitialize(NULL);
		while (!Terminated) {

			GetCallBack(_fpBaseProce);
			GetCallBack(_fpProc);
			::Sleep(500);
		}
		_bKill = true;
	}
	catch (...) {
		_bKill = true;
	}
}
// ---------------------------------------------------------------------------

void Thread_Run::GetCallBack(fp_Proce fp_p) {
	fp_p();
}

void Thread_Run::SynFun(TThreadMethod thrMet) {
	Synchronize(thrMet);
}

bool Thread_Run::GetTerminated() const {
	return Terminated;
}
