#pragma once

#include "D2Item.h"

#include <memory>
#include <functional>

int intOf(const CEdit & edit);

// CDlgFoundry 对话框

class CDlgFoundry : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgFoundry)

	CD2Item & m_stItem;
	std::pair<CPropertyList, BOOL> m_aProps[7];	//propList + setProps[5] + runeWord

	//UI
	CString m_sText[22];

	//Message
	BOOL m_bNoParamChangeMsg = FALSE;	//忽略属性参数改变消息
	BOOL m_bNoPropItemCheckMsg = FALSE;	//忽略属性列表项check/uncheck消息
	BOOL m_bNoPropItemSelectMsg = FALSE;//忽略属性列表项选中消息

public:
	CDlgFoundry(CD2Item & item, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgFoundry();
private:
	//读取物品数据，展示UI
	void InitUI();
	void ReadItem();
	void ReadItemEar(const CEar & ear);
	void ReadItemInfo(CItemInfo & info, BOOL bRuneWord, BOOL bNormal);
	void ReadItemGold(const CGoldQuantity & gold);
	void ReadExtItemInfo(const CExtItemInfo & extInfo, BOOL bNormal);
	void ReadTypeSpecificInfo(CTypeSpecificInfo & spec, BOOL bRuneWord);
	void ReadItemLongName(const CLongName & longName);
	//收集UI数据存入物品
	BOOL GatherData();
	void WriteItem(CD2Item & item);
	void WriteItemEar(CEar & ear);
	void WriteItemInfo(CItemInfo & info, BOOL bSimple, BOOL & bPersonalized, const CItemMetaData & meta, BOOL & bSocketed, BOOL bRuneWord, int iGemIdxMax);
	void WriteItemGold(CGoldQuantity & gold);
	void WriteExtItemInfo(CExtItemInfo & extInfo, BOOL & bPersonalized, const CItemMetaData & meta);
	void WriteTypeSpecificInfo(CTypeSpecificInfo & spec, const CItemMetaData & meta, BOOL & bSocketed, BOOL bIsSet, BOOL bRuneWord, int iGems);
	void WriteItemLongName(CLongName & longName);
	void validate(bool expr, int index);
	void setPreSuffix(BOOL & exist, MayExist<WORD> & index, const CComboBox & cb, int max, int msg);
	//属性列表和参数
	void UpdatePropList();	//根据属性TAB的选择，更新属性列表
	BOOL ChangePropItemSelection(LPNMLISTVIEW pNMLV);	//检查属性列表选中项的变化，更新参数控件
	BOOL ChangePropItemChecked(LPNMLISTVIEW pNMLV);		//检查属性列表check项的变化，增加/删除属性，更新参数控件
	void SetupPropParameters(int id, int value, BOOL checked);	//根据属性id和值，设置参数控件
	void SetupParamValue(UINT idx, BOOL enabled, const CPropParam & param);		//设置数值控件
	void SetupParamClass(UINT idx, BOOL enabled, int value);					//设置角色选择控件
	void SetupParamSkill(UINT idx, BOOL enabled, int value, BOOL classOnly);	//设置技能选择控件，classOnly表示是否只包括角色技能
	void SetupParamAuraSkill(UINT idx, BOOL enabled, int value);				//设置灵气技能选择控件
	void SetupParamMonster(UINT idx, BOOL enabled, int value);					//设置怪物选择控件
	void SetupParamTime(UINT idx, BOOL enabled, int value);						//设置时间选择控件
	void SetupParamListGeneric(UINT idx, BOOL enabled, int value, std::function<int(CComboBox &)> setup);	//设置下拉框参数控件，通用版本
	void SetupParamValueFrom(UINT idx, BOOL enabled, const std::vector<CPropParam> & param);	//设置后续数值控件
	std::pair<BOOL, DWORD> GatherParamaters(const CPropertyMetaDataItem & meta) const;		//收集参数控件的值，得到属性的参数
	int ExtendSockets() const;		//所有属性列表里扩展孔的总数

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
	CTabCtrl m_tabPropList;
	CListCtrl m_lstProperty;
	CEdit m_edParam[4];
	CComboBox m_cbParam[2];
	CString m_sTip[4];
	CButton m_chRuneWord;
public:
	void OnOK();
	afx_msg void OnBnClickedCheckIndestructible();	//无法破坏标志更改
	afx_msg void OnCbnSelchangeComboQuality();		//质量选项更改
	afx_msg void OnTcnSelchangeTabProperty(NMHDR *pNMHDR, LRESULT *pResult);//属性列表Tab选项更改
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);	//属性列表选择项更改或checked/unchecked
	afx_msg void OnEnChangeParam();					//参数修改，包括文件框和下拉框
	afx_msg void OnBnClickedCheckRuneword();		//符文之语标志修改
	afx_msg void OnEnChangeEditBaseSockets();		//基础孔数修改
};
