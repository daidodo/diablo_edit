// PropertyDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "CharacterDialogBase.h"


// CCharacterDialogBase 对话框

IMPLEMENT_DYNAMIC(CCharacterDialogBase, CDialog)

CCharacterDialogBase::CCharacterDialogBase(UINT nIDTemplate, CWnd* pParent)
	: CDialog(nIDTemplate, pParent)
{
}

CCharacterDialogBase::~CCharacterDialogBase()
{
}

BEGIN_MESSAGE_MAP(CCharacterDialogBase, CDialog)
END_MESSAGE_MAP()


// CCharacterDialogBase 消息处理程序
