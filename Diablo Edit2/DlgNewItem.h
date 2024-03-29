﻿#pragma once

#include "D2Item.h"

#include <memory>

// CDlgNewItem 对话框

class CDlgNewItem : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNewItem)

	std::unique_ptr<CD2Item> & m_pItem;
	int m_iDisplayItemPicIndex = -1;	//展示物品的图片index，-1为没有
	CRect m_rectDisplayItem;			//展示物品的位置大小

	//UI
	CString m_sText[2];
	CTreeCtrl m_tcItems;

	void InitUI();
	void LoadText(void);	//加载控件的字符串内容
public:
	explicit CDlgNewItem(std::unique_ptr<CD2Item> & ptr, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgNewItem();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NewItem };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
