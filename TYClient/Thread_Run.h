//---------------------------------------------------------------------------

#ifndef Thread_RunH
#define Thread_RunH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------

typedef void __fastcall (__closure *fp_BaseProce)(void);	//---各流程均会执行的过程
typedef void __fastcall (__closure *fp_Proce)(void);
class Thread_Run : public TThread
{
private:
	void GetCallBack(fp_Proce fp_p);
	void SynFun(TThreadMethod thrMet);
protected:
	void __fastcall Execute();
public:
	__fastcall Thread_Run(bool CreateSuspended);
	bool GetTerminated()const;
	bool _bKill;

	fp_BaseProce	_fpBaseProce;
	fp_Proce 		_fpProc;
};
//---------------------------------------------------------------------------
#endif
