#pragma once

#include "D2Item.h"

// CDlgSuspend 对话框

class CDlgSuspend : public CDialog
{
	DECLARE_DYNAMIC(CDlgSuspend)
public:
	CDlgSuspend(CWnd* pParent,UINT transparency);   // 标准构造函数
	virtual ~CDlgSuspend();
// 对话框数据
	enum { IDD = IDD_DIALOG_Suspend };
//自定义类型
private:
	typedef std::pair<BYTE,CString>	__MsgType;	//<字体颜色,内容>
//自定义数据
public:
	static const LONG WINDOW_WIDTH = 273;
	static const COLORREF	FONT_COLOR[];		//字体颜色
	enum {WHITE,BLUE,GREEN,RARE,UNIQUE,CRAFT};	//颜色索引
	static const LONG FONT_HEIGHT = 17;
private:
    UINT m_nTransparency;       //窗体透明度
	const CD2Item * m_pItem;
	std::vector<__MsgType> m_sItemMsg;	//物品的信息
//自定义函数
public:
	LONG GetItemInfo(const CD2Item * pItem);		//返回窗体的高度
	const CD2Item * GetItemPtr() const{return m_pItem;}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
