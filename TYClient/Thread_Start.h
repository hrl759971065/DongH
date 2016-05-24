//---------------------------------------------------------------------------

#ifndef Thread_StartH
#define Thread_StartH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class Thread_Start : public TThread
{
private:
	void __fastcall Load();
	void __fastcall InitDev();
protected:
	void __fastcall Execute();
public:
	__fastcall Thread_Start(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
