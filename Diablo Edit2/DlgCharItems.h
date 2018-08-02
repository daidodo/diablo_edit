#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "PropertyDialog.h"
#include "DlgSuspend.h"
#include "DlgPrefixSuffix.h"

#include <tuple>
#include <memory>

enum EEquip;
enum EPosition;
enum EPositionType;

//物品的视图
struct CItemView
{
	CD2Item & Item;						//对应的物品
	const UINT nPicRes;					//bmp图片资源索引
	const EEquip iEquip;				//物品可装备的位置
	EPosition iPosition;				//物品位置
	int iGridX, iGridY;					//网格坐标
	const int iGridWidth, iGridHeight;	//自身占用网格大小
	std::vector<int> vGemItems;			//镶嵌的物品在m_vItemViews的索引，-1表示没有
	CItemView(CD2Item & item, EEquip equip, EPosition pos, int x, int y);
	CSize ViewSize() const;
};

//网格位置的视图
class GridView
{
	std::vector<int> vItemIndex;	//网格内的物品在m_vItemViews里索引；-1表示没有
	const EPosition iPosition;		//位置索引
	const EPositionType iType;		//位置的类型，PositionType()
	const int iCol, iRow;			//行列数
public:
	const CRect Rect;				//大小与位置
	const EEquip iEquip;			//可装备的物品类型
	BOOL bEnabled = TRUE;			//是否启用（可交互）
	explicit GridView(EPosition pos);
	BOOL IsGrid() const;			//是否分成单个网格
	BOOL IsSockets() const;			//是否为镶嵌的孔
	BOOL CanEquip(EEquip equip) const { return (equip & iEquip) != 0; }	//是否可穿戴
	int ItemIndex(int x, int y) const;			//返回指定坐标的物品索引
	void ItemIndex(int index, int x, int y);	//设置指定坐标的物品索引
	void ItemIndex(int index, int x, int y, int width, int height);	//设置指定坐标和范围的物品索引
	CPoint IndexToXY(int x, int y, int width, int height) const;	//指定坐标和大小，得到物品的像素位置
	std::tuple<int, int, int> XYToPositionIndex(CPoint pos, BOOL II, BOOL corpseII, int col, int row) const;	//根据物品UI像素，得到位置索引和坐标
	BOOL PutItem(int index, int x, int y, int width, int height, EEquip equip);	//将物品放到指定位置，考虑空闲、大小、穿戴类型；返回是否成功
	void Reset();					//清空网格
};

// CDlgCharItems 对话框

class CDlgCharItems : public CPropertyDialog
{
	DECLARE_DYNAMIC(CDlgCharItems)

	std::vector<GridView> m_vGridView;		//所有网格的信息
	void DrawGrids(CPaintDC & dc);			//画所有网格
	BOOL m_bHasCharacter = FALSE;			//是否加载了人物
	
	//物品和位置
	std::vector<CItemView> m_vItemViews;	//所有的物品,除了镶嵌在孔里的
	BOOL m_bHasCorpse = FALSE;				//是否有尸体
	BOOL m_bSecondHand = FALSE;				//是否显示II手武器
	BOOL m_bCorpseSecondHand = FALSE;		//是否显示尸体的II手武器
	BOOL m_bHasMercenary = FALSE;			//是否有雇佣兵
	void AddItemInGrid(CD2Item & item, int body);			//将物品添加到网格中, body: 0-人物本身，1-尸体，2-雇佣兵，3-Golem
	CPoint GetItemPositionXY(const CItemView & view) const;	//得到物品的实际像素坐标

	//铸造台
	BYTE m_bItemLevel = 0;			//物品等级
	CComboBox m_cbQuality;			//物品质量
	//BOOL m_bItemInscribed;		//已经个人化
	CString m_ItemOwner = _T("");	//装备主人,包括个人化后的玩家名字,耳朵的主人等
	BOOL m_bItemSocket = FALSE;		//有孔
	BYTE m_bBaseSocket = 0;			//基础孔数
	BYTE m_bExtSocket = 0;			//额外孔数
	BOOL m_bEthereal = FALSE;		//无形,不可修复
	BOOL m_bIndestructible = FALSE;	//不可破坏
	short m_wItemQuantity = 0;		//数量,如果有的话
	short m_wItemDefence = 0;		//防御值,如果有的话
	short m_wCurDurability = 0;		//当前耐久度
	short m_wMaxDurability = 0;		//最大耐久度
	CListCtrl m_lcPropertyList;		//物品属性列表
	void ResetFoundry();			//初始化铸造台
	void ReadItemProperty(const CD2Item & item);	//读取物品的属性，并显示在锻造台
	CItemView & SelectedParentItemView();			//当前选中的父物品视图
	//const CItemView * SelectedItemView() const;	//当前选中的物品视图，没有返回0

	//悬浮窗
	static const int INFO_WINDOW_LEFT = 50;		//左边悬浮窗的位置X
	static const int INFO_WINDOW_RIGHT = 550;	//右边悬浮窗的位置X
	std::unique_ptr<CDlgSuspend> m_pDlgItemInfo;//显示物品信息的悬浮窗口
	BOOL m_bNotShowItemInfoDlg;					//是否隐藏物品信息悬浮窗
	CSliderCtrl m_scTrasparent;					//属性悬浮窗的透明度
	void ShowItemInfoDlg(const CD2Item * pItem, int x);	//显示/隐藏(pItem = 0)物品信息悬浮窗口。x用来选择窗口位置

	//界面文字
	CString m_sText[6];
	//CButton m_btButton[5];

	//鼠标
	int m_iSelectedItemIndex = -1;		//选中的物品在m_vItemViews中的索引
	int m_iSelectedSocketIndex = -1;	//选中的物品镶嵌在孔里时，对应在m_vItemViews中的索引
	int m_iPickedItemIndex = -1;		//当前鼠标拿起的物品在m_vItemViews中的索引
	HCURSOR m_hCursor;					//鼠标
	CPoint m_pMouse;					//鼠标位置
	std::tuple<int, int, int> HitTestPosition(CPoint pos, int col = 1, int row = 1) const;	//由像素XY和物品大小得到网格位置
	HCURSOR CreateAlphaCursor(const CItemView & itemView);	//把物品bmp转换成鼠标句柄
	BOOL PutItemInGrid(EPosition pos, int x, int y);	//尝试将已拿起的物品放到指定位置（不包括鼠标）

	//弹出菜单
	BOOL m_bClickOnItem = FALSE;		//是否点中了物品
	int m_iCopiedItemIndex = -1;		//复制的物品在m_vItemViews中的索引，-1为没有
public:
	//对话框数据
	enum { IDD = IDD_DIALOG_CharItems };
	CDlgCharItems(CWnd* pParent = NULL);   // 标准构造函数
	//虚函数
	void UpdateUI(CD2S_Struct & character);
	BOOL GatherData(CD2S_Struct & character);
	void ResetAll();
	void LoadText(void);	//加载控件的字符串内容
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
private:
	// UI
	void DrawItemXY(CPaintDC & dc, CPoint pos, const CItemView & itemView) const;	//在绝对坐标点画物品
	void DrawAllItemsInGrid(CPaintDC & dc) const;		//画网格内的所有物品，如果选中的物品镶嵌了物品，也要画出来

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCheck2();
    afx_msg void OnChangeHand();
    //afx_msg void OnPrefixSuffix();
	afx_msg void OnChangeCorpseHand();
	afx_msg void OnChangeCorpse();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnChangeMercenary();
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnItemImport();
	afx_msg void OnItemExport();
	afx_msg void OnItemCopy();
	afx_msg void OnItemPaste();
	afx_msg void OnItemModify();
	afx_msg void OnItemRemove();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
};
