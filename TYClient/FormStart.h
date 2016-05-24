//---------------------------------------------------------------------------

#ifndef FormStartH
#define FormStartH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "RzEdit.hpp"
#include <Vcl.Graphics.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Imaging.pngimage.hpp>
//#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TFrmStart : public TForm
{
__published:	// IDE-managed Components
	TTimer *tmr1;
	TImage *Image1;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall tmr1Timer(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
	__fastcall TFrmStart(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmStart *FrmStart;
//---------------------------------------------------------------------------
#endif
