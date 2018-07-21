// DlgSuspend.cpp : 实现文件
//

#include "stdafx.h"

#include <deque>
#include <numeric>

#include "Diablo Edit2.h"
#include "DlgSuspend.h"

using namespace std;

typedef deque<CString> __Tokens;
// CDlgSuspend 对话框

const COLORREF	CDlgSuspend::FONT_COLOR[] = {
	RGB(255,255,255),		//白色
	RGB(0,0,255),			//蓝色
	RGB(0,255,0),			//绿色
	RGB(255,255,0),			//黄金色
	RGB(0x94,0x80,0x64),	//暗金色
	RGB(255,128,0),			//橙色
};

IMPLEMENT_DYNAMIC(CDlgSuspend, CDialog)

CDlgSuspend::CDlgSuspend(CWnd* pParent,UINT transparency)
	: CDialog(CDlgSuspend::IDD, pParent)
    , m_nTransparency(transparency)
	, m_pItem(0)
{}

CDlgSuspend::~CDlgSuspend()
{}

void CDlgSuspend::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgSuspend, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

static CString text(const __Tokens & tokens) {
	CString ret;
	int i = 0;
	for (auto & t : tokens) {
		if (t.IsEmpty())
			continue;
		if (i++)
			ret += _T(" ");
		ret += t;
	}
	return ret;
}

void CDlgSuspend::AddMsg(BYTE color, const CString & msg) {
	if (!msg.IsEmpty())
		m_sItemMsg.emplace_back(color, msg);
}

void CDlgSuspend::AddPropertyList(BYTE color, const CPropertyList & propList, UINT & sockets) {
	for (const auto & p : propList.mProperty)
		if (p.first == 194)     //extend sockets
			sockets += p.second;
		else
			AddMsg(color, ::theApp.PorpertyDescription(p.first, p.second));
}

LONG CDlgSuspend::GetItemInfo(const CD2Item * pItem)
{
	ASSERT(pItem);
	m_pItem = pItem;
	m_sItemMsg.clear();
    if(pItem->bEar){    //ear structure
		AddMsg(WHITE, CSFormat(::theApp.ItemSuspendUI(10), CString(pItem->pEar->sEarName)));
		AddMsg(WHITE, ::theApp.ClassName(pItem->pEar->iEarClass));
		AddMsg(WHITE, CSFormat(::theApp.ItemSuspendUI(11), pItem->pEar->iEarLevel));
    }else{              //item structure
		BYTE quality = pItem->Quality();
		BYTE color = quality <= 3 ? WHITE : quality - 3;
		if (pItem->IsSet())	//Set item
			AddMsg(color, ::theApp.SetItemName(pItem->pItemInfo->pExtItemInfo->wSetID));
        //Prefix, Suffix, Name
		__Tokens name{ ::theApp.ItemName(pItem->pItemData->NameIndex) };
        switch(quality){
        case 1:         //low
            name.push_front(::theApp.ItemSuspendUI(0));
            break;
        case 3:         //high
            name.push_front(::theApp.ItemSuspendUI(1));
            break;
        case 4:        //magic
			name.push_front(::theApp.MagicPrefix(pItem->pItemInfo->pExtItemInfo->wPrefix));
            name.push_back(::theApp.MagicSuffix(pItem->pItemInfo->pExtItemInfo->wSuffix));
            break;
        case 5:         //set(暂不支持)
			//TODO
            break;
        case 6:{        //rare
			const auto & rare = *pItem->pItemInfo->pExtItemInfo->pRareName;
			__Tokens title{ ::theApp.RareCraftedName(rare.iName1), ::theApp.RareCraftedName(rare.iName2) };
			AddMsg(color, text(title));
            break;}
        case 7:{        //unique
            CString title = ::theApp.UniqueName(pItem->pItemInfo->pExtItemInfo->wUniID);
			AddMsg(color, title);
            break;}
        case 8:{        //crafted
			const auto & craft = *pItem->pItemInfo->pExtItemInfo->pCraftName;
			__Tokens title{ ::theApp.RareCraftedName(craft.iName1), ::theApp.RareCraftedName(craft.iName2) };
			AddMsg(color, text(title));
            break;}
        default:;
        }
		AddMsg(color, text(name));
		//Defence or Attack
		if (pItem->pItemData->HasDef) {				//有防御值
			AddMsg(WHITE, CSFormat(::theApp.ItemSuspendUI(2), UINT(pItem->pItemInfo->pTpSpInfo->iDefence - 10)));
		} else if (pItem->pItemData->Damage1Min) {		//单手伤害
			AddMsg(WHITE, CSFormat(::theApp.ItemSuspendUI(3), pItem->pItemData->Damage1Min, pItem->pItemData->Damage1Max));
		} else if (pItem->pItemData->Damage2Min)		//双手伤害
			AddMsg(WHITE, CSFormat(::theApp.ItemSuspendUI(4), pItem->pItemData->Damage2Min, pItem->pItemData->Damage2Max));
        //Quantity
		if (pItem->pItemData->IsStacked) {
			AddMsg(WHITE, CSFormat(::theApp.ItemSuspendUI(5), UINT(pItem->pItemInfo->pTpSpInfo->iQuantity)));
		} else if (pItem->pItemInfo->IsTypeName("gld "))
			AddMsg(WHITE, CSFormat(::theApp.ItemSuspendUI(5), UINT(pItem->pItemInfo->pGold->wQuantity)));
		//Durability or Indestructible
		if (pItem->pItemData->HasDur) {
			if (pItem->pItemInfo->pTpSpInfo->iMaxDurability) {   //有耐久度
				AddMsg(WHITE, CSFormat(::theApp.ItemSuspendUI(6),
					UINT(pItem->pItemInfo->pTpSpInfo->iCurDur),
					UINT(pItem->pItemInfo->pTpSpInfo->iMaxDurability)));
			} else       //不可破坏
				AddMsg(BLUE, ::theApp.ItemSuspendUI(7));
		}
        //Property
		UINT socketnum = 0;   //sockets num
		if (!pItem->bSimple) {
			AddPropertyList(BLUE, pItem->pItemInfo->pTpSpInfo->stPropertyList, socketnum);
			if (pItem->IsSet()) {
				auto & setProps = pItem->pItemInfo->pTpSpInfo->apSetProperty;
				for (size_t i = 0; i < std::size(setProps); ++i)
					if (setProps[i].exist())
						AddPropertyList(GREEN, *setProps[i], socketnum);
			}
		}
        //Ethereal
        if(pItem->bEthereal)
			AddMsg(BLUE,::theApp.ItemSuspendUI(8));
        //Socket
        if(pItem->bSocketed){
            socketnum += pItem->pItemInfo->pTpSpInfo->iSocket;
			AddMsg(BLUE, CSFormat(::theApp.ItemSuspendUI(9), pItem->pItemInfo->pExtItemInfo->nGems, socketnum));
        }
    }
	//根据信息条数和长度决定窗体长度和宽度
	LONG maxLen = accumulate(m_sItemMsg.begin(), m_sItemMsg.end(), 0, [](LONG m, auto & a) {return max(m, a.second.GetLength()); });
	CRect rect;
	GetWindowRect(&rect);
	SetWindowPos(0, rect.left, rect.top,
		max(WINDOW_WIDTH_MIN, WIDTH_PER_CHAR * maxLen),
		HEIGHT_PER_LINE * int(m_sItemMsg.size() + 1),
		SWP_NOACTIVATE);
	return 0;
}
// CDlgSuspend 消息处理程序


void CDlgSuspend::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	CBrush bh(RGB(0,0,0));
	dc.FillRect(rect,&bh);
	dc.SetBkColor(0);
	rect.top = HEIGHT_PER_LINE / 2;
	rect.bottom = rect.top + HEIGHT_PER_LINE;
	for (auto & p : m_sItemMsg) {
		dc.SetTextColor(FONT_COLOR[p.first]);
		dc.DrawTextEx(p.second.GetBuffer(), p.second.GetLength(), &rect, DT_CENTER, 0);
		rect.top = rect.bottom;
		rect.bottom += HEIGHT_PER_LINE;
	}
}

BOOL CDlgSuspend::OnInitDialog()
{
	CDialog::OnInitDialog();
	//设置窗体半透明
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE) ^ 0x80000);
	HINSTANCE hInst = LoadLibrary(_T("User32.DLL")); 
	if(hInst){ 
		typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
		MYFUNC fun = NULL;
		//取得SetLayeredWindowAttributes函数指针 
		fun = (MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
		if(fun)
			fun(GetSafeHwnd(),0,m_nTransparency,2); 
		FreeLibrary(hInst); 
	}
	return TRUE;
}

void CDlgSuspend::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rect(202,17,262,137);
	if(!rect.PtInRect(point))
		ShowWindow(SW_HIDE);
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgSuspend::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    ShowWindow(SW_HIDE);
}
