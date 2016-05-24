//---------------------------------------------------------------------------

#ifndef FormLoginH
#define FormLoginH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "RzPanel.hpp"
#include <Vcl.Imaging.pngimage.hpp>
#include "RzLabel.hpp"
#include "RzEdit.hpp"
#include <Vcl.Mask.hpp>
#include "RzCmboBx.hpp"
#include "RzButton.hpp"
//---------------------------------------------------------------------------
class TFrmLogin : public TForm
{
__published:	// IDE-managed Components
	TRzPanel *RzPanel1;
	TImage *Image1;
	TRzGroupBox *RzGroupBox1;
	TRzLabel *RzLabel1;
	TRzEdit *E_Pass;
	TRzLabel *RzLabel2;
	TRzComboBox *C_User;
	TRzButton *RzButton1;
	TRzButton *RzButton2;
	void __fastcall RzButton1Click(TObject *Sender);
	void __fastcall RzButton2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TFrmLogin(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmLogin *FrmLogin;
//---------------------------------------------------------------------------
#endif