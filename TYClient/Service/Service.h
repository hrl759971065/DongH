//---------------------------------------------------------------------------

#ifndef ServiceH
#define ServiceH
#include "ServAutoR.h"
//---------------------------------------------------------------------------

namespace NS_Rjssb{



class TService : public TServAutoR{ 	//---原来的TService与系统很多命名相冲突
public:
	TService();
	~TService();

	void LoadingThr();
	void SetThrProc(int nNodeID);

	void __fastcall _beginThr();
	void __fastcall _StopThr();
	String _ThrMsg;
	bool _bRuning;
};

extern TService global_Service;
};

#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using  namespace NS_Rjssb;
#endif

#endif
