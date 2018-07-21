#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "PropertyDialog.h"
#include "DlgSuspend.h"
#include "DlgPrefixSuffix.h"

//物品视图
struct CItemView
{
	const UINT		nPicRes;			//bmp图片资源索引
	const WORD		Range;				//自身占用网格大小,x<<4 + y
	WORD Pos = -1;						//高8位为位置,Stash,Inventory...,低8位为坐标(x<<4 + y)
	CD2Item & Item;						//对应的物品
	std::vector<CItemView> vGemItems;	//镶嵌的物品View
public:
	CItemView(UINT resID,WORD range,CD2Item & item):nPicRes(resID),Range(range),Item(item){}
};

//内联
inline WORD MAKE_GRID(int x,int y){return (x << 4) + y;}
inline WORD MAKE_GRID(int i,int x,int y){return (WORD(i) << 8) + (x << 4) + y;}
inline int INDEX(WORD grid){return grid >> 8;}
inline int COL(WORD grid){return (grid >> 4) & 0xF;}
inline int ROW(WORD grid){return grid & 0xF;}

// CDlgCharItems 对话框

class CDlgCharItems : public CPropertyDialog
{
	DECLARE_DYNAMIC(CDlgCharItems)
//自定义常量
	const static WORD	INVALID_ITEM = -1;
	const static UINT	BMP_INDEX_BASE = IDB_BITMAP0;			//所有物品图片资源的起始编号
	const static BYTE	GRID_WIDTH = 30;						//每格边长
	const static BYTE	GRID_NUMBER = 4;						//存储网格个数
	const static BYTE	GRID_BODY_NUMBER = GRID_NUMBER + 10;    //GRID_NUMBER + 身体穿戴部位数
	const static WORD	GRID_RECT[GRID_BODY_NUMBER][4];			//网格起始坐标,列数,行数(x,y,col,row)
public:
	CDlgCharItems(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCharItems();

// 对话框数据
	enum { IDD = IDD_DIALOG_CharItems };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
// 自定义函数
public:
	void UpdateUI(const CD2S_Struct & character);
	BOOL GatherData(CD2S_Struct & character);
	void ResetAll();
	void LoadText(void);	//加载控件的字符串内容
private:
	// UI
	void DrawGrids(CPaintDC & dc);	//画所有网格
	void DrawItemXY(CPaintDC & dc,CPoint pos, const CItemView & itemView);	//在绝对坐标点画物品
	void DrawItemsInGrid(CPaintDC & dc);   //画网格内的所有物品
	WORD HitTestItem(const CPoint & pos,WORD range = MAKE_GRID(1,1));	//返回网格索引gridIndex
	BOOL PutItemInGrid(WORD itemIndex, WORD gridIndex);  //把m_vItemViews中索引为itemIndex的物品放到网格索引gridIndex的位置
	void ShowItemInfoDlg(const CD2Item * pItem);    //显示/隐藏(pItem = 0)物品信息悬浮窗口
	void ReadItemProperty(WORD itemIndex);  //读取m_vItemViews中索引为itemIndex的物品的属性，并显示在锻造台
    void ResetFoundry();    //初始化铸造台
    //内联
    CPoint GRID2XY(WORD gridIndex) const{		//由grid索引(高8位为位置,低8位为坐标(x<<4 + y))得到实际像素坐标位置,只能用于储存箱，口袋，方块，孔
		int i = INDEX(gridIndex);
		ASSERT(i < GRID_NUMBER);
		return CPoint(m_rectGrid[i].left + GRID_WIDTH * COL(gridIndex),m_rectGrid[i].top + GRID_WIDTH * ROW(gridIndex));
	}
	CPoint GRID2XY(WORD gridID,WORD range) const{	//已知身体部位编号,根据物品大小决定图像位置，只能用于身体部位
		ASSERT(gridID >= GRID_NUMBER && GRID_NUMBER < GRID_BODY_NUMBER);
		return CPoint(GRID_RECT[gridID][0] + (GRID_RECT[gridID][2] - COL(range)) * GRID_WIDTH / 2,
			GRID_RECT[gridID][1] + (GRID_RECT[gridID][3] - ROW(range)) * GRID_WIDTH / 2);
	}
	void SET_GRID_ITEM(int i,int x,int y,int itemIndex){		//设置网格位置的物品m_vItemViews索引,INVALID_ITEM表示没有
		m_iGridItems[i][x + y * GRID_RECT[i][2]] = itemIndex;
	}
	void SET_GRID_ITEM(WORD gridIndex,int itemIndex){
		SET_GRID_ITEM(INDEX(gridIndex),COL(gridIndex),ROW(gridIndex),itemIndex);
	}
	int GET_GRID_ITEM(WORD gridIndex) const{		//得到网格位置的物品m_vItemViews索引,INVALID_ITEM表示没有
		return GET_GRID_ITEM(INDEX(gridIndex),COL(gridIndex),ROW(gridIndex));
	}
	int GET_GRID_ITEM(int i,int x,int y) const{
		return m_iGridItems[i][x + y * GRID_RECT[i][2]];
	}
// 自定义成员
private: 
	//装备物品
	std::vector<CItemView> m_vItemViews;			//所有的物品,除了鼠标拿起来的
	CRect m_rectGrid[GRID_BODY_NUMBER];				//网格区域,箱子0,口袋1,方块2，孔3
	std::vector<std::vector<WORD>> m_iGridItems;	//网格内的物品的m_vItemViews索引
	BOOL m_bSecondHand = FALSE;						//是否显示II手武器

	//鼠标
	CPoint m_pMouse;				//鼠标位置

	//铸造台
	WORD m_iSelectedItemIndex = INVALID_ITEM;	//当前选中的物品在m_vItemViews中的索引
	//CString m_sItemName;			//物品名字
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

	//悬浮窗
	CDlgSuspend * m_pDlgItemInfo = 0;	//显示物品信息的悬浮窗口
	BOOL m_bNotShowItemInfoDlg;			//是否不显示物品信息悬浮窗
	CSliderCtrl m_scTrasparent;			//属性悬浮窗的透明度

	//界面文字
    CString m_sText[13];
    CButton m_btButton[5];

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
