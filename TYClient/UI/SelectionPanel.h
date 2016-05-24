//---------------------------------------------------------------------------

#ifndef SelectionPanelH
#define SelectionPanelH

#include "RzPanel.hpp"
#include "RzLabel.hpp"
//---------------------------------------------------------------------------

//---ѡ�����
class TSelectionPanel : public TRzPanel{
public:
	__fastcall TSelectionPanel(TComponent* Owner);
	__fastcall ~TSelectionPanel();

	void SetBtnCaption(DynamicArray<String> btn_Caption);

	//---����
	__property String Title ={read =GetTitle,write =SetTitle};
private:
	void __fastcall Init();
	void __fastcall TSelectionPanel::CreateBtn(/*TWinControl *Parent*/);
	void __fastcall BtnClick(TObject *Sender);
	void __fastcall CreateTitle();

	TRzLabel    *_Title;
	TButton		*_Btn[5][4];
	TButton		*_Btn_Switch;

	TRzPanel	*_P_Result;

	String GetTitle();
	void SetTitle(String aVal);
};

#endif
