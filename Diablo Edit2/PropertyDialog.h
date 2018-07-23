#pragma once

#include "D2S_Struct.h"

// CPropertyDialog 对话框

class CPropertyDialog : public CDialog
{
	DECLARE_DYNAMIC(CPropertyDialog)

public:
	CPropertyDialog(UINT nIDTemplate,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPropertyDialog();
// 自定义函数
	virtual void UpdateUI(CD2S_Struct & character) = 0;
	virtual BOOL GatherData(CD2S_Struct & character) = 0;
	virtual void ResetAll() = 0;
	virtual void LoadText() = 0;	//加载控件的字符串内容
	void OnOK(){}
	void OnCancel(){}
// 对话框数据

protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
};
