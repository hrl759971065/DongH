#ifndef voConstH
#define voConstH

// ---自定义消息
#define WM_JILLISTMSG (WM_USER+103)	//---selection面板发出的消息

#define DATATIMEOUTS 	-1	数据超时

#define DISABLE	-2	禁用

// ---数据包类型
#define COMPATIBLEPKG 0     //兼容以前版本 (用第一个成员是32位整数表示字节数的结构)
#define FRAMEHEADTAIL 1     //用帧首尾和控制符的数据包   (帧首+带控制符的数据+帧尾)
#define FRAMETIMEOUT  2     //用定义超时时间的方法判断数据包 (适合单片机的简单协议)

// 设备 位置 动作
#define DZ		0
#define HLD 	1
#define HEAD	0
#define TAIL	1
#define UP		0
#define	DOWN	1
#define RED		0
#define GREEN	1
#define CLOSE	2

// 定位器状态值

#define ALLPASS 	0
#define HEADMISS 	1
#define TAILMISS 	2
#define ALLMISS 	3

#define ZHONGC		1
#define QINGC		2

// ---设备分类代号
#define COMM		0	//---串口对象
#define CARMETER	1   //---汽车衡仪表
#define IOCTRL		2   //---IO开关模块
#define	READER		3   //---读写器
#define LED			4   //---LED显示屏

const MaxBangNum = 10;

const MaxCmdSize = 128; // 发送串口命令的最大字节数
const MaxRecvBufSize = 512; // 输入缓冲区大小
const MaxCard = 128; // 同时读取的最大卡片数

// ---访问平台WebService接口的用户和密码
const UTF8String User_PTJK = L"yanp";
const UTF8String Pass_PTJK = L"1";

// ---权限名
const UTF8String BankQX = L"办卡";

#endif
