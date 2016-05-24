// ---------------------------------------------------------------------------
// #include <vcl.h>

#pragma hdrstop

#include "Service.h"
#include "Thread_Start.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)

namespace NS_Rjssb {

	TService global_Service;

	TService::TService() {
		_bRuning = false;
	}

	TService::~TService() {

	}

	// ---软件启动时加载配置及初始化各对象
	void TService::LoadingThr() {
		Thread_Start * Thr = new Thread_Start(false); // ---直接启动
		// Thr->Suspended =false;	//---启动线程
	}

	void TService::SetThrProc(int nNodeID) {
		_Thread->_fpBaseProce = AutoBaseProc;
		switch (nNodeID) {
		case 0:
			_Thread->_fpProc = AutoProc_ZZRC;
			break;
		case 1:
			_Thread->_fpProc = AutoProc_ParkingCR;
			break;
		case 2:
			_Thread->_fpProc = AutoProc_CYJ;
			break;
		case 3:
			_Thread->_fpProc = AutoProc_QCH;
			break;
		case 4:
			_Thread->_fpProc = AutoProc_MCCR;
			break;
		case 5:
			_Thread->_fpProc = AutoProc_ZZCC;
			break;
			/* case 2:
			 _Thread->_fpProc=AutoProc_MC;
			 break;
			 case 3:
			 _Thread->_fpProc=AutoProc_DLJ;
			 break; */
		case 7:
			_Thread->_fpProc = AutoProc_ZTCJ;
			break;
		}
	}

	void __fastcall TService::_beginThr() {
		_Thread->Suspended = false; // Start();
		_bRuning = true;
	}

	void __fastcall TService::_StopThr() {
		try {
			_Thread->Terminate();

			while (!_Thread->_bKill) {
				_ThrMsg = "线程正在退出..." + Now().DateTimeString();
				Forms::Application->ProcessMessages();
				::Sleep(1);
			}
			_bRuning = false;
			// _Thread->Suspended =true;
			// _Thread->Terminate();
			// int n1=_Thread->WaitFor();
		}
		catch (...) {

		}
	}

};
