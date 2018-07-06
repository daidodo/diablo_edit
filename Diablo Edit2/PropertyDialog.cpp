// PropertyDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "PropertyDialog.h"


// CPropertyDialog 对话框

IMPLEMENT_DYNAMIC(CPropertyDialog, CDialog)

CPropertyDialog::CPropertyDialog(UINT nIDTemplate, CWnd* pParent)
	: CDialog(nIDTemplate, pParent)
{
}

CPropertyDialog::~CPropertyDialog()
{
}

BEGIN_MESSAGE_MAP(CPropertyDialog, CDialog)
END_MESSAGE_MAP()


// CPropertyDialog 消息处理程序
