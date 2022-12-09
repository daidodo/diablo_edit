
// MainFrm.cpp: CMainFrame 类的实现
//

#include "stdafx.h"
#include "Diablo Edit2.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_MENUSELECT()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

afx_msg void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) {
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);
	switch (nItemID) {
		//System menu
		case ID_FILE_NEW:		SetMessageText(::theApp.MenuPrompt(0)); break;
		case ID_FILE_OPEN:		SetMessageText(::theApp.MenuPrompt(1)); break;
		case ID_FILE_SAVE:		SetMessageText(::theApp.MenuPrompt(2)); break;
		case ID_FILE_SAVE_AS:	SetMessageText(::theApp.MenuPrompt(3)); break;
		case ID_CLOSE_FILE:		SetMessageText(::theApp.MenuPrompt(4)); break;
		case ID_APP_EXIT:		SetMessageText(::theApp.MenuPrompt(5)); break;
		case ID_VIEW_TOOLBAR:	SetMessageText(::theApp.MenuPrompt(6)); break;
		case ID_VIEW_STATUS_BAR:SetMessageText(::theApp.MenuPrompt(7)); break;
		case ID_APP_ABOUT:		SetMessageText(::theApp.MenuPrompt(8)); break;
		case ID_LANGUAGE1:		SetMessageText(::theApp.LangTitle(0)); break;
		case ID_LANGUAGE2:		SetMessageText(::theApp.LangTitle(1)); break;
		case ID_LANGUAGE3:		SetMessageText(::theApp.LangTitle(2)); break;
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

