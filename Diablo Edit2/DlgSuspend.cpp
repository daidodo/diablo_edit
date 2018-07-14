// DlgSuspend.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgSuspend.h"


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

LONG CDlgSuspend::GetItemInfo(const CD2Item * pItem)
{
	ASSERT(pItem);
	m_pItem = pItem;
	m_sItemMsg.clear();
    if(pItem->bEar){    //ear structure

    }else{              //item structure
        //Prefix, Suffix, Name
        CString name = ::theApp.ItemName(pItem->pItemData->NameIndex);
        BYTE quality = pItem->Quality();
        BYTE color = quality <= 3 ? WHITE : quality - 3;
        switch(quality){
        case 1:         //low
            name = ::theApp.String(414) + CString(" ") + name;
            break;
        case 3:         //high
            name = ::theApp.String(415) + CString(" ") + name;
            break;
        case 4:{        //magic
            CString prefix = ::theApp.MagicPrefix(pItem->pItemInfo->pExtItemInfo->wPrefix.Value());
            if(prefix != "^")
                name = prefix + CString(" ") + name;
            CString suffix = ::theApp.MagicSuffix(pItem->pItemInfo->pExtItemInfo->wSuffix.Value());
            if(suffix != "^")
                name += CString(" ") + suffix;
            break;}
        case 5:         //set(暂不支持)
            break;
        case 6:{        //rare
            CString first = ::theApp.RareCraftedName(pItem->pItemInfo->pExtItemInfo->pRareName->iName1);
            CString second = ::theApp.RareCraftedName(pItem->pItemInfo->pExtItemInfo->pRareName->iName2);
            if(first == "^")
                first = "";
            else
                first += CString(" ");
            if(second != "^")
                first += second + CString(" ");
            if(first.GetLength() > 0)
                m_sItemMsg.push_back(__MsgType(color,first));
            break;}
        case 7:{        //unique
            CString uniquename = ::theApp.UniqueName(pItem->pItemInfo->pExtItemInfo->wUniID.Value());
            if(uniquename.GetLength() > 0)
                m_sItemMsg.push_back(__MsgType(color,uniquename));
            break;}
        case 8:{        //crafted
            CString first = ::theApp.RareCraftedName(pItem->pItemInfo->pExtItemInfo->pCraftName->iName1);
            CString second = ::theApp.RareCraftedName(pItem->pItemInfo->pExtItemInfo->pCraftName->iName2);
            if(first == "^")
                first = "";
            else
                first += CString(" ");
            if(second != "^")
                first += second + CString(" ");
            if(first.GetLength() > 0)
                m_sItemMsg.push_back(__MsgType(color,first));
            break;}
        default:;
        }
        m_sItemMsg.push_back(__MsgType(color,name));
        //Defence or Attack
        if(pItem->pItemData->HasDef){     //有防御值
            CString defence;
            defence.Format(::theApp.String(416),UINT(pItem->pItemInfo->pTpSpInfo->iDefence.Value() - 10));
            m_sItemMsg.push_back(__MsgType(WHITE,defence));
        }else if(pItem->pItemData->Damage1Min){      //单手伤害
            CString dmg;
            dmg.Format(::theApp.String(417),pItem->pItemData->Damage1Min,pItem->pItemData->Damage1Max);
            m_sItemMsg.push_back(__MsgType(WHITE,dmg));
        }else if(pItem->pItemData->Damage2Min){      //双手伤害
            CString dmg;
            dmg.Format(::theApp.String(418),pItem->pItemData->Damage2Min,pItem->pItemData->Damage2Max);
            m_sItemMsg.push_back(__MsgType(WHITE,dmg));
        }
        //Quantity
        CString msg;
        if(pItem->pItemData->IsStacked){
            msg.Format(::theApp.String(382),UINT(pItem->pItemInfo->pTpSpInfo->iQuantity.Value()));
            m_sItemMsg.push_back(__MsgType(WHITE,msg));
        }else if(IsSameType(pItem->pItemInfo->sTypeName,"gld ")){
            msg.Format(::theApp.String(382),UINT(pItem->pItemInfo->pGold->wQuantity));
            m_sItemMsg.push_back(__MsgType(WHITE,msg));
        }
        //Durability or Indestructible
        if(pItem->pItemData->HasDur){
            if(pItem->pItemInfo->pTpSpInfo->iMaxDurability.Value()){   //有耐久度
                CString dur;
                dur.Format(::theApp.String(419),
                    UINT(pItem->pItemInfo->pTpSpInfo->iCurDur.Value()),
                    UINT(pItem->pItemInfo->pTpSpInfo->iMaxDurability.Value()));
                m_sItemMsg.push_back(__MsgType(WHITE,dur));
            }else{      //不可破坏
                m_sItemMsg.push_back(__MsgType(BLUE,::theApp.String(420)));
            }
        }
        //Property
		UINT socketnum = 0;   //sockets num
		if (!pItem->bSimple) {
			for (const auto & p : pItem->pItemInfo->pTpSpInfo->mProperty)
				if (p.first == 194)     //extend sockets
					socketnum = p.second;
				else
					m_sItemMsg.push_back(__MsgType(BLUE, ::theApp.PorpertyDescription(p.first, p.second)));
		}
        //Ethereal
        if(pItem->bEthereal)
            m_sItemMsg.push_back(__MsgType(BLUE,::theApp.String(421)));
        //Socket
        if(pItem->bSocketed){
            socketnum += pItem->pItemInfo->pTpSpInfo->iSocket.Value();
            CString socket;
            socket.Format(::theApp.String(422),socketnum);
            m_sItemMsg.push_back(__MsgType(BLUE,socket));
        }
    }
	//根据信息长度决定窗体长度
	CRect rect;
	GetWindowRect(&rect);
	SetWindowPos(0,rect.left,rect.top,WINDOW_WIDTH,FONT_HEIGHT * int(m_sItemMsg.size() + 1),SWP_NOACTIVATE);
	return 0;//FONT_HEIGHT * LONG(m_sItemMsg.size() + 1);
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
	rect.bottom = rect.top + FONT_HEIGHT;
	for(std::vector<__MsgType>::iterator i = m_sItemMsg.begin();i != m_sItemMsg.end();++i){ 
		dc.SetTextColor(FONT_COLOR[i->first]);
		dc.DrawTextEx(i->second.GetBuffer(),i->second.GetLength(),&rect,DT_CENTER,0);
		rect.top = rect.bottom;
		rect.bottom += FONT_HEIGHT;
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
