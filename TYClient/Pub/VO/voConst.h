#ifndef voConstH
#define voConstH

// ---�Զ�����Ϣ
#define WM_JILLISTMSG (WM_USER+103)	//---selection��巢������Ϣ

#define DATATIMEOUTS 	-1	���ݳ�ʱ

#define DISABLE	-2	����

// ---���ݰ�����
#define COMPATIBLEPKG 0     //������ǰ�汾 (�õ�һ����Ա��32λ������ʾ�ֽ����Ľṹ)
#define FRAMEHEADTAIL 1     //��֡��β�Ϳ��Ʒ������ݰ�   (֡��+�����Ʒ�������+֡β)
#define FRAMETIMEOUT  2     //�ö��峬ʱʱ��ķ����ж����ݰ� (�ʺϵ�Ƭ���ļ�Э��)

// �豸 λ�� ����
#define DZ		0
#define HLD 	1
#define HEAD	0
#define TAIL	1
#define UP		0
#define	DOWN	1
#define RED		0
#define GREEN	1
#define CLOSE	2

// ��λ��״ֵ̬

#define ALLPASS 	0
#define HEADMISS 	1
#define TAILMISS 	2
#define ALLMISS 	3

#define ZHONGC		1
#define QINGC		2

// ---�豸�������
#define COMM		0	//---���ڶ���
#define CARMETER	1   //---�������Ǳ�
#define IOCTRL		2   //---IO����ģ��
#define	READER		3   //---��д��
#define LED			4   //---LED��ʾ��

const MaxBangNum = 10;

const MaxCmdSize = 128; // ���ʹ������������ֽ���
const MaxRecvBufSize = 512; // ���뻺������С
const MaxCard = 128; // ͬʱ��ȡ�����Ƭ��

// ---����ƽ̨WebService�ӿڵ��û�������
const UTF8String User_PTJK = L"yanp";
const UTF8String Pass_PTJK = L"1";

// ---Ȩ����
const UTF8String BankQX = L"�쿨";

#endif