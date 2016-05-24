//---------------------------------------------------------------------------

#ifndef FormMainH
#define FormMainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RzPanel.hpp"
#include <Vcl.ExtCtrls.hpp>
#include "RzTabs.hpp"
#include "RzSplit.hpp"
#include "RzStatus.hpp"
#include "RzGrids.hpp"
#include <Vcl.Grids.hpp>
#include "RzEdit.hpp"
#include <Vcl.Mask.hpp>
#include "RzLabel.hpp"
#include "RzButton.hpp"
#include "RzRadChk.hpp"
#include "RzSndMsg.hpp"
#include "RzDBGrid.hpp"
#include <Vcl.DBGrids.hpp>
#include <Data.DB.hpp>
#include <Datasnap.DBClient.hpp>
#include <Vcl.ImgList.hpp>
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.Graphics.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include "RzBtnEdt.hpp"
#include "RzLstBox.hpp"
#include <Vcl.ButtonGroup.hpp>
#include <Vcl.Buttons.hpp>
#include "RzPrgres.hpp"
#include "RzBorder.hpp"
#include "RzListVw.hpp"
#include <Vcl.ComCtrls.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include "voConst.h"
#include "RzBmpBtn.hpp"
//---------------------------------------------------------------------------
//---�Զ�����Ϣ
#define WM_SELECTION (WM_USER+102)	//---selection��巢������Ϣ

class TFrmMain : public TForm
{
__published:	// IDE-managed Components
	TRzStatusBar *SB_Status;
	TRzPageControl *PC_Model;
	TRzTabSheet *TS_Banka;
	TRzPanel *RzPanel1;
	TRzPanel *RzPanel2;
	TRzSplitter *RzSplitter1;
	TRzGroupBox *RzGroupBox2;
	TRzGroupBox *RzGroupBox1;
	TRzGroupBox *RzGroupBox3;
	TRzEdit *RzEdit2;
	TRzGroupBox *RzGroupBox4;
	TRzEdit *RzEdit3;
	TRzLabel *RzLabel1;
	TRzGroupBox *RzGroupBox5;
	TRzEdit *RzEdit1;
	TRzGroupBox *RzGroupBox6;
	TRzEdit *RzEdit4;
	TRzGroupBox *RzGroupBox7;
	TRzEdit *RzEdit5;
	TRzButton *RzButton1;
	TRzButton *RzButton2;
	TRzButton *RzButton3;
	TRzGroupBox *RzGroupBox8;
	TRzGroupBox *RzGroupBox9;
	TRzDateTimeEdit *RzDateTimeEdit1;
	TRzGroupBox *RzGroupBox10;
	TRzDateTimeEdit *RzDateTimeEdit2;
	TRzGroupBox *RzGroupBox11;
	TRzEdit *RzEdit6;
	TRzCheckBox *RzCheckBox1;
	TRzButton *RzButton4;
	TRzSendMessage *RzSendMessage1;
	TRzDBGrid *RzDBGrid1;
	TDataSource *DataSource1;
	TClientDataSet *ClientDataSet1;
	TMemo *Memo1;
	TButton *Button1;
	TRzMemo *M_Load;
	TRzTabSheet *TS_ZZRC;
	TImageList *ImageList1;
	TImage *Image1;
	TButton *Button2;
	TRzLabel *RzLabel2;
	TImage *Image2;
	TRzLabel *RzLabel3;
	TRzPanel *P_Operation;
	TRzPanel *P_Info;
	TRzPageControl *RzPageControl1;
	TRzTabSheet *TabSheet2;
	TRzPanel *P_Numplate;
	TRzButton *Btn_OkNumplate;
	TRzPageControl *RzPageControl2;
	TRzTabSheet *RzTabSheet1;
	TRzPanel *P_Meik;
	TRzButton *Btn_SelMK;
	TRzPageControl *RzPageControl3;
	TRzTabSheet *RzTabSheet2;
	TRzPanel *P_Pinz;
	TRzButton *RzButton7;
	TRzLabel *RzLabel4;
	TRzClockStatus *RzClockStatus1;
	TTimer *tmr_Exit;
	TRzPageControl *RzPageControl4;
	TRzTabSheet *RzTabSheet3;
	TRzPanel *P_Gongys;
	TRzButton *RzButton8;
	TRzStatusPane *SP_CardID;
	TRzStatusPane *SP_Status;
	TRzStatusPane *RzStatusPane1;
	TRzStatusPane *SP_Numplate;
	TTimer *tmr_Jiem;
	TImage *Img_Oper;
	TImageList *ImgList_Btn;
	TRzShapeButton *Btn_Ok;
	TRzShapeButton *Btn_Cancel;
	TRzGroupBox *GB_Tis;
	TRzLabel *RzLabel5;
	TRzLabel *L_Tis;
	TRzProgressBar *RzProgressBar1;
	TTimer *tmr_Tis;
	TRzTabSheet *TS_QCH;
	TRzGroupBox *RzGroupBox12;
	TRzLEDDisplay *LED_Meter;
	TLabel *Label1;
	TImage *Img_Down_Head;
	TImage *Img_Up_Head;
	TImage *Img_Down_Tail;
	TImage *Img_Up_Tail;
	TLabel *Label2;
	TLabel *Label3;
	TShape *shp_DWQ_Head;
	TShape *shp_DWQ_Tail;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label9;
	TButton *Button3;
	TButton *Button4;
	TButton *B_DZ_UP_Tail;
	TButton *B_DZ_DOWN_Tail;
	TEdit *E_QCHSta;
	TEdit *E_QCHFlow;
	TEdit *E_QCHLed;
	TEdit *E_QCHCar;
	TEdit *E_QCHCardID;
	TChart *Chart1;
	TFastLineSeries *Series1;
	TRzListView *Lv_Jil;
	TTimer *tmr_RealVal;
	TTimer *tmr_ZD;
	TRzTabSheet *TS_Test;
	TButton *Button5;
	TRzGroupBox *GB_Guz;
	TRzLabel *RzLabel7;
	TRzRichEdit *RE_Beiz;
	TButton *Button6;
	TButton *Button7;
	TRzGroupBox *GB_KFL;
	TRzPanel *P_KFL_LR;
	TRzBmpButton *RzBmpButton1;
	TRzBmpButton *RzBmpButton2;
	TRzBmpButton *RzBmpButton3;
	TRzBmpButton *RzBmpButton4;
	TRzBmpButton *RzBmpButton5;
	TRzBmpButton *RzBmpButton6;
	TRzBmpButton *RzBmpButton7;
	TRzBmpButton *RzBmpButton8;
	TRzBmpButton *RzBmpButton9;
	TRzBmpButton *RzBmpButton10;
	TRzBmpButton *RzBmpButton11;
	TRzBmpButton *RzBmpButton12;
	TRzShapeButton *Btn_KflOk;
	TRzPageControl *RzPageControl5;
	TRzTabSheet *RzTabSheet4;
	TRzPanel *P_KFL;
	TRzButton *RzButton5;
	TButton *Button8;
	TEdit *Edit1;
	TRzTabSheet *TS_ZZCC;
	TImage *Image3;
	TRzLabel *RzLabel6;
	TRzLabel *RzLabel8;
	TImage *Image4;
	TRzLabel *RzLabel9;
	TRzClockStatus *RzClockStatus2;
	TRzPanel *P_Operation_CC;
	TImage *Image5;
	TRzGroupBox *GB_Guz_CC;
	TRzLabel *RzLabel12;
	TRzRichEdit *RE_Beiz_CC;
	TRzPanel *RzPanel4;
	TRzShapeButton *B_GetTicket;
	TRzLabel *RzLabel13;
	TRzGroupBox *GB_GetTicket;
	TRzPanel *P_GetTicket;
	TRzShapeButton *B_GetTicket_RCM;
	TRzShapeButton *B_GetTicket_ZYM;
	TRzPanel *P_GTis;
	TRzLabel *L_GTis;
	TImage *Image6;
	TButton *Button9;
	TRzShapeButton *Btn_C;
	TButton *Button10;
	TRzTabSheet *TS_ZTCJ;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TLabel *Label8;
	TButton *Button11;
	TEdit *Edit2;
	TTabSheet *TabSheet3;
	TLabel *Label10;
	TLabel *Label11;
	TMemo *Memo2;
	TMemo *Memo3;
	TTimer *Timer1;
	TTimer *tmr_WebService;
	TRzTabSheet *TS_WebService;
	TRzRichEdit *RE_WebService;
	void __fastcall RzButton4Click(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall RzDBGrid1DrawColumnCell(TObject *Sender, const TRect &Rect, int DataCol,
          TColumn *Column, TGridDrawState State);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall Btn_SelMKClick(TObject *Sender);
	void __fastcall RzButton7Click(TObject *Sender);
	void __fastcall tmr_ExitTimer(TObject *Sender);
	void __fastcall RzButton8Click(TObject *Sender);
	void __fastcall tmr_JiemTimer(TObject *Sender);
	void __fastcall Btn_OkNumplateClick(TObject *Sender);
	void __fastcall Btn_OkClick(TObject *Sender);
	void __fastcall Btn_CancelClick(TObject *Sender);
	void __fastcall tmr_TisTimer(TObject *Sender);
	void __fastcall SP_StatusDblClick(TObject *Sender);
	void __fastcall tmr_RealValTimer(TObject *Sender);
	void __fastcall tmr_ZDTimer(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
	void __fastcall B_DZ_UP_TailClick(TObject *Sender);
	void __fastcall B_DZ_DOWN_TailClick(TObject *Sender);
	void __fastcall Button5Click(TObject *Sender);
	void __fastcall Button6Click(TObject *Sender);
	void __fastcall Button7Click(TObject *Sender);

	void __fastcall NumBntClick(TObject *Sender);
	void __fastcall Btn_KflOkClick(TObject *Sender);
	void __fastcall Button8Click(TObject *Sender);
	void __fastcall B_GetTicketClick(TObject *Sender);
	void __fastcall B_GetTicket_RCMClick(TObject *Sender);
	void __fastcall Btn_CClick(TObject *Sender);
	void __fastcall Button10Click(TObject *Sender);
	void __fastcall Button11Click(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall tmr_WebServiceTimer(TObject *Sender);
private:	// User declarations
	void DBGridRecordSize(TColumn *mColumn);
	void __fastcall UpdJiem();
protected:
	void __fastcall MsgOper_Selection(Messages::TMessage &Message);
	void __fastcall JilListMsg(Messages::TMessage &Message);
	//���Զ�����Ϣ������Ϣѭ��
	BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(WM_SELECTION,Messages::TMessage,MsgOper_Selection);
		MESSAGE_HANDLER(WM_JILLISTMSG,Messages::TMessage,JilListMsg);
	END_MESSAGE_MAP(TForm);
public:		// User declarations
	__fastcall TFrmMain(TComponent* Owner);

	int _CurrentIndex;	//---��ǰ�����λ��ֵ
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmMain *FrmMain;
//---------------------------------------------------------------------------
#endif