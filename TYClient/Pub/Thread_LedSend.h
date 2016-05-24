//---------------------------------------------------------------------------

#ifndef Thread_LedSendH
#define Thread_LedSendH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class Thread_LedSend : public TThread
{
private:
	void __fastcall SyncFun_WritePic();
protected:
	void __fastcall Execute();
public:
	__fastcall Thread_LedSend(bool CreateSuspended);
	bool _bFree;
	bool _bClose;
};
//---------------------------------------------------------------------------
#endif