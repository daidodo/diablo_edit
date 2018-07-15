// DlgSelectChar.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgSelectChar.h"


// CDlgSelectChar 对话框

IMPLEMENT_DYNAMIC(CDlgSelectChar, CDialog)

CDlgSelectChar::CDlgSelectChar(int & sel,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectChar::IDD, pParent)
	, m_nSel(sel)
{}

CDlgSelectChar::~CDlgSelectChar(){}

void CDlgSelectChar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbSelectChar);
}


BEGIN_MESSAGE_MAP(CDlgSelectChar, CDialog)
END_MESSAGE_MAP()


// CDlgSelectChar 消息处理程序

BOOL CDlgSelectChar::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetWindowText(::theApp.OtherUI(0));
	for(UINT i = 0;i < ::theApp.ClassNameSize();++i)
		m_cbSelectChar.AddString(::theApp.ClassName(i));
	m_cbSelectChar.SetCurSel(0);

	return TRUE;
}
