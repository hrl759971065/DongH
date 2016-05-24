//---------------------------------------------------------------------------

#ifndef Thread_CunYGH
#define Thread_CunYGH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class Thread_CunYG : public TThread
{
private:
	void __fastcall SyncFun_WritePic();
protected:
	void __fastcall Execute();
public:
	__fastcall Thread_CunYG(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
