//---------------------------------------------------------------------------

#ifndef ServAutoRH
#define ServAutoRH
#include "ServJiek.h"
//---------------------------------------------------------------------------

class TServAutoR : public TServJiek{
public:
	TServAutoR();
	~TServAutoR();

	int _ErrCnt;
protected:
	void __fastcall AutoBaseProc();

	void __fastcall AutoProc_ZZRC();
	void __fastcall AutoProc_ZZCC();
	void __fastcall AutoProc_ParkingCR();
	void __fastcall AutoProc_CoalYardCR();
	void __fastcall AutoProc_QCH();
	void __fastcall AutoProc_MCCR();
	void __fastcall AutoProc_CYJ();
	void __fastcall AutoProc_ZTCJ();
private:
	bool _bInitial;
	int _FLowID[2];
	String memoStr;
};
extern String StrRFID,StrICID,CarNumber;
#endif
