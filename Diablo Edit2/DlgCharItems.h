#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "PropertyDialog.h"
#include "DlgSuspend.h"
#include "DlgPrefixSuffix.h"

#include <tuple>

const int GRID_WIDTH = 30;		//每个网格的边长(像素)

//物品的视图
struct CItemView
{
	CD2Item & Item;						//对应的物品
	const UINT nPicRes;					//bmp图片资源索引
	int iPosition, iGridX, iGridY;		//物品位置(CDlgCharItems::EPosition), 以及网格坐标
	const int iGridWidth, iGridHeight;	//自身占用网格大小
	std::vector<CItemView> vGemItems;	//镶嵌的物品View
	CItemView(CD2Item & item, int pos, int x, int y);
	CSize ViewSize() const { return CSize(iGridWidth * GRID_WIDTH, iGridHeight*GRID_WIDTH); }
};

// CDlgCharItems 对话框

class CDlgCharItems : public CPropertyDialog
{
	DECLARE_DYNAMIC(CDlgCharItems)

	//网格位置
	enum EPosition {
		STASH,				//箱子
		INVENTORY,			//口袋
		CUBE,				//方块
		IN_BELT,			//腰带里
		IN_SOCKET,			//镶嵌在孔里

		GRID_COUNT,			//网格类型位置数量

		HEAD = GRID_COUNT,	//头
		NECK,				//项链
		BODY,				//身体
		RIGHT_HAND,			//武器右(I & II)
		LEFT_HAND,			//武器左(I & II)
		RIGHT_RING,			//戒指右
		LEFT_RING,			//戒指左
		BELT,				//腰带
		FOOT,				//鞋子
		GLOVE,				//手套

		IN_MOUSE,			//被鼠标拿起

		POSITION_END		//所有位置总数
	};
	const static WORD	POSITION_RECT[POSITION_END][4];	//每个位置(EPosition)在UI的起始坐标(像素),列数,行数(x,y,col,row)
	CRect m_rectGrid[POSITION_END];						//将POSITION_RECT转换成CRect, 方便使用
	void DrawGrids(CPaintDC & dc);						//画所有网格
	
	//物品和位置
	std::vector<CItemView> m_vItemViews;					//所有的物品,除了镶嵌在孔里的
	std::vector<std::vector<int>> m_vGridItems;				//网格内的物品在m_vItemViews里索引；如果是镶嵌的物品，则是CItemView::vGemItems的索引；-1表示没有
	BOOL m_bSecondHand = FALSE;								//是否显示II手武器
	BOOL UpdateGridItem(int index, int pos, int x, int y);	//将index物品放进网格位置(pos, x, y)，返回是否成功
	int GetGridItemIndex(int pos, int x, int y) const;		//得到网格位置的物品的m_vItemViews索引, -1表示没有
	void SetGridItemIndex(int pos, int x, int y, int index);//设置网格位置的物品的m_vItemViews1索引, -1表示没有
	CPoint GetItemPositionXY(const CItemView & view) const;	//得到物品的实际像素坐标
	
	//铸造台
	int m_iSelectedItemIndex = -1;	//选中的物品在m_vItemViews中的索引
	int m_iSelectedSocketIndex = -1;//选中的物品镶嵌在孔里时，对应孔的索引(m_iSelectedItemIndex表示带孔的物品)
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
	void ReadItemProperty(const CD2Item & item);  //读取物品的属性，并显示在锻造台
	const CItemView * SelectedParentItemView() const;	//当前选中的父物品视图，没有返回0
	const CItemView * SelectedItemView() const;			//当前选中的物品视图，没有返回0

	//悬浮窗
	static const int INFO_WINDOW_LEFT = 550;	//悬浮窗的位置X
	CDlgSuspend * m_pDlgItemInfo = 0;			//显示物品信息的悬浮窗口
	BOOL m_bNotShowItemInfoDlg;					//是否隐藏物品信息悬浮窗
	CSliderCtrl m_scTrasparent;					//属性悬浮窗的透明度
	void ShowItemInfoDlg(const CD2Item * pItem);//显示/隐藏(pItem = 0)物品信息悬浮窗口

	//界面文字
	CString m_sText[4];
	//CButton m_btButton[5];

	//鼠标
	CPoint m_pMouse;	//鼠标位置
	std::tuple<int, int, int> HitTestPosition(CPoint pos) const;	//由像素XY得到网格位置
public:
	CDlgCharItems(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCharItems();
// 对话框数据
	enum { IDD = IDD_DIALOG_CharItems };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
public:
	// 自定义函数
	// 虚函数
	void UpdateUI(CD2S_Struct & character);
	BOOL GatherData(CD2S_Struct & character);
	void ResetAll();
	void LoadText(void);	//加载控件的字符串内容
private:
	// UI
	void DrawItemXY(CPaintDC & dc, CPoint pos, const CItemView & itemView) const;	//在绝对坐标点画物品
	void DrawAllItemsInGrid(CPaintDC & dc) const;		//画网格内的所有物品，如果选中的物品镶嵌了物品，也要画出来

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCheck2();
    afx_msg void OnChangeHand();
    afx_msg void OnPrefixSuffix();
};
