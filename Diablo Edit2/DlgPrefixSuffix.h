#pragma once
#include "afxwin.h"


// CDlgPrefixSuffix 对话框

class CDlgPrefixSuffix : public CDialog
{
	DECLARE_DYNAMIC(CDlgPrefixSuffix)

public:
	CDlgPrefixSuffix(int quality,int * selIndex,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPrefixSuffix();

// 对话框数据
	enum { IDD = IDD_DIALOG_PrefixSuffix };

private:
//自定义成员
    //前后缀名字
    CComboBox m_cbFirstName;
    CComboBox m_cbLastName;
    CComboBox m_cbPrefix1;
    CComboBox m_cbSuffix1;
    CComboBox m_cbPrefix2;
    CComboBox m_cbSuffix2;
    CComboBox m_cbPrefix3;
    CComboBox m_cbSuffix3;
    CComboBox m_cbUniqueName;
    CComboBox m_cbSubType;
    //选择索引
    int m_nQuality;
    int * m_vSelect;
    //界面文字
    CString m_sText[10];
//自定义函数
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
