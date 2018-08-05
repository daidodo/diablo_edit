#pragma once

#include "D2Item.h"

// CDlgFoundry 对话框

class CDlgFoundry : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFoundry)
	CD2Item & m_stItem;

	//UI
	CString m_sText[19];

public:
	CDlgFoundry(CD2Item & item, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFoundry();
	void InitUI();
	void ReadItem();
	void ReadItemEar(const CEar & ear);
	void ReadItemInfo(const CItemInfo & info);
	void ReadItemGold(const CGoldQuantity & gold);
	void ReadExtItemInfo(const CExtItemInfo & extInfo);
	void ReadTypeSpecificInfo(const CTypeSpecificInfo & spec);
	void ReadItemLongName(const CLongName & longName);


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_Foundry };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
private:
	CComboBox m_cbClass;
	CComboBox m_cbQuality;
	CComboBox m_cbMonster;
	CComboBox m_cbRareCraftedName[2];
	CComboBox m_cbSubType;
	CComboBox m_cbPrefix[3];
	CComboBox m_cbSuffix[3];
	CComboBox m_cbSetName;
	CComboBox m_cbUniqueName;
	CEdit m_edLevel;
	BOOL m_bUnidentified;
	BOOL m_bEthereal;
	CButton m_chIndestructible;
	CEdit m_edMaxDurability;
	CEdit m_edDurability;
	CEdit m_edQuantity;
	CEdit m_edOwner;
	CEdit m_edSockets;
	CString m_sExtSockets;
	CEdit m_edDefence;
public:
	afx_msg void OnBnClickedCheckIndestructible();
	afx_msg void OnCbnSelchangeComboQuality();
};
