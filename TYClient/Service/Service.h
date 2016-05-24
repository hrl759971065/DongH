//---------------------------------------------------------------------------

#ifndef ServiceH
#define ServiceH
#include "ServAutoR.h"
//---------------------------------------------------------------------------

namespace NS_Rjssb{



class TService : public TServAutoR{ 	//---ԭ����TService��ϵͳ�ܶ��������ͻ
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
