#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "PropertyDialog.h"
#include "DlgSuspend.h"
#include "DlgPrefixSuffix.h"

//物品视图
struct CItemView
{
	const UINT		nPicRes;		//bmp图片资源索引
	const WORD		Range;			//自身占用网格大小,x<<4 + y
	WORD Pos;						//高8位为位置,Stash,Inventory...,低8位为坐标(x<<4 + y)
	CD2Item * const pItem;	        //指向物品的指针
	CItemView(UINT resID,WORD range,CD2Item * pIT):nPicRes(resID),Range(range),pItem(pIT){}
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
	const static BYTE	GRID_NUMBER = 3;						//存储网格个数
	const static BYTE	GRID_BODY_NUMBER = GRID_NUMBER + 10 + 1;//GRID_NUMBER + 身体穿戴部位数 + 铸造台(foundry)
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
	void InitUI();
	void DrawGrid(CPaintDC * pDC);	//画网格
	void DrawItemXY(CPaintDC * pDC,CPoint pos,CItemView * pVI);	//在绝对坐标点画物品
	void DrawItemsInGrid(CPaintDC * pDC);   //画网格内的所有物品
	WORD HitTestItem(const CPoint & pos,WORD range = MAKE_GRID(1,1));	//返回网格索引gridIndex
	BOOL PutItemInGrid(WORD itemIndex,WORD gridIndex);  //把m_vpItems中索引为itemIndex的物品放到网格索引gridIndex的位置
	void PickItemFromGrid(WORD itemIndex);  //把m_vpItems中索引为itemIndex的物品从网格里拿起来
	void DestroyAllItems();
	void ShowItemInfoDlg(const CD2Item * pItem);    //显示/隐藏(pItem = 0)物品信息悬浮窗口
	void ReadItemProperty(WORD itemIndex);  //读取m_vpItems中索引为itemIndex的物品的属性(放在了锻造台上)
    HCURSOR CreateAlphaCursor(const CItemView & itemView);  //把物品bmp转换成鼠标句柄
    void ResetFoundry();    //初始化铸造台
    //内联
    CPoint GRID2XY(WORD gridIndex) const{		//由grid索引(高8位为位置,低8位为坐标(x<<4 + y))得到实际像素坐标位置,只能用于储存箱，口袋，方块等
		int i = INDEX(gridIndex);
		ASSERT(i < GRID_NUMBER);
		return CPoint(m_rectGrid[i].left + GRID_WIDTH * COL(gridIndex),m_rectGrid[i].top + GRID_WIDTH * ROW(gridIndex));
	}
	CPoint GRID2XY(WORD gridID,WORD range) const{	//已知身体部位编号（铸造台编号）,根据物品大小决定图像位置，只能用于身体部位和铸造台
		ASSERT(gridID >= GRID_NUMBER && GRID_NUMBER < GRID_BODY_NUMBER);
		return CPoint(GRID_RECT[gridID][0] + (GRID_RECT[gridID][2] - COL(range)) * GRID_WIDTH / 2,
			GRID_RECT[gridID][1] + (GRID_RECT[gridID][3] - ROW(range)) * GRID_WIDTH / 2);
	}
	void SET_GRID_ITEM(int i,int x,int y,int itemIndex){		//设置网格位置的物品m_vpItems索引,INVALID_ITEM表示没有
		m_iGridItems[i][x + y * GRID_RECT[i][2]] = itemIndex;
	}
	void SET_GRID_ITEM(WORD gridIndex,int itemIndex){
		SET_GRID_ITEM(INDEX(gridIndex),COL(gridIndex),ROW(gridIndex),itemIndex);
	}
	int GET_GRID_ITEM(WORD gridIndex) const{		//得到网格位置的物品m_vpItems索引,INVALID_ITEM表示没有
		return GET_GRID_ITEM(INDEX(gridIndex),COL(gridIndex),ROW(gridIndex));
	}
	int GET_GRID_ITEM(int i,int x,int y) const{
		return m_iGridItems[i][x + y * GRID_RECT[i][2]];
	}
// 自定义成员
private: 
	HCURSOR m_hCursor;						//光标
	WORD m_iPickedItemIndex;				//当前鼠标拿起的物品在m_vpItems中的索引
	CItemView * m_pPickedItem;				//当前鼠标拿起的物品
	CRect m_rectGrid[GRID_BODY_NUMBER];		//网格区域,箱子索引0,口袋索引1,方块索引2
	std::vector<std::vector<WORD> >	m_iGridItems;	//网格内的物品的m_vpItems索引
	std::vector<CItemView *> m_vpItems;		//所有的物品,除了鼠标拿起来的
	CPoint m_pMouse;						//鼠标位置
	CDlgSuspend * m_pDlgItemInfo;			//显示物品信息的悬浮窗口
	BOOL m_bNotShowItemInfoDlg;				//指示不显示物品信息悬浮窗
    CSliderCtrl m_scTrasparent;             //属性悬浮窗的透明度
    BOOL m_bSecondHand;                     //是否显示II手武器
	//铸造台上物品的属性
	//CString m_sItemName;		//物品名字
	BOOL m_bItemSocket;			//有孔
	BYTE m_bBaseSocket;			//基础孔数
	BOOL m_bEthereal;			//无形,不可修复
	//BOOL m_bItemInscribed;		//已经个人化
	BYTE m_bItemLevel;			//物品等级
	CString m_ItemOwner;		//装备主人,包括个人化后的玩家名字,耳朵的主人等
	short m_wItemQuantity;		//数量,如果有的话
	short m_wItemDefence;		//防御值,如果有的话
	short m_wMaxDurability;		//最大耐久度
	BOOL m_bIndestructible;		//不可破坏
	short m_wCurDurability;		//当前耐久度
	CComboBox m_cbQuality;		//物品质量
	CListCtrl m_lcPropertyList;	//物品属性列表
	BYTE m_bExtSocket;			//额外孔数
    //界面文字
    CString m_sText[12];
    CButton m_btButton[5];

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCheck2();
    afx_msg void OnChangeHand();
    afx_msg void OnPrefixSuffix();
};
