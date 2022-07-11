#pragma once
#include "afxwin.h"


// CDlgSelectChar 对话框

class CDlgSelectChar : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectChar)

public:
	explicit CDlgSelectChar(int & sel,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSelectChar();

// 对话框数据
	enum { IDD = IDD_DIALOG_SelectChar };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
//自定义成员
private:
	int & m_nSel;
	CComboBox m_cbSelectChar;
//自定义函数
public:
	void OnOK(){
		m_nSel = m_cbSelectChar.GetCurSel();
		CDialog::OnOK();
	}

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
