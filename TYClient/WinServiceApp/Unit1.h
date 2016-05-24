//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <SvcMgr.hpp>
#include <vcl.h>
//---------------------------------------------------------------------------
class TNmlxDevControl : public TService
{
__published:    // IDE-managed Components
	void __fastcall ServiceExecute(TService *Sender);
	void __fastcall ServiceStart(TService *Sender, bool &Started);
	void __fastcall ServiceContinue(TService *Sender, bool &Continued);
	void __fastcall ServiceShutdown(TService *Sender);
	void __fastcall ServicePause(TService *Sender, bool &Paused);
	void __fastcall ServiceStop(TService *Sender, bool &Stopped);
private:        // User declarations
	bool	_bStartup;
public:         // User declarations
	__fastcall TNmlxDevControl(TComponent* Owner);
	TServiceController __fastcall GetServiceController(void);

	friend void __stdcall ServiceController(unsigned CtrlCode);
};
//---------------------------------------------------------------------------
extern PACKAGE TNmlxDevControl *NmlxDevControl;
//---------------------------------------------------------------------------
#endif