
// Diablo Edit2View.cpp: CDiabloEdit2View 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Diablo Edit2.h"
#endif

#include "Diablo Edit2Doc.h"
#include "Diablo Edit2View.h"
#include "MainFrm.h"

#include "DlgCharBasicInfo.h"
//#include "DlgSkills.h"
#include "DlgSelectChar.h"
#include "DlgCharItems.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDiabloEdit2View

IMPLEMENT_DYNCREATE(CDiabloEdit2View, CFormView)

BEGIN_MESSAGE_MAP(CDiabloEdit2View, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CDiabloEdit2View::OnTcnSelchangeTab1)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_OPEN, &CDiabloEdit2View::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CDiabloEdit2View::OnFileSave)
	ON_COMMAND(ID_CLOSE_FILE, &CDiabloEdit2View::OnCloseFile)
	ON_UPDATE_COMMAND_UI(ID_LANGUAGE1, &CDiabloEdit2View::OnUpdateLanguage1)
	ON_COMMAND(ID_LANGUAGE1, &CDiabloEdit2View::OnLanguage1)
	ON_COMMAND(ID_LANGUAGE2, &CDiabloEdit2View::OnLanguage2)
	ON_COMMAND(ID_LANGUAGE3, &CDiabloEdit2View::OnLanguage3)
	ON_UPDATE_COMMAND_UI(ID_LANGUAGE2, &CDiabloEdit2View::OnUpdateLanguage2)
	ON_UPDATE_COMMAND_UI(ID_LANGUAGE3, &CDiabloEdit2View::OnUpdateLanguage3)
	ON_COMMAND(ID_FILE_SAVE_AS, &CDiabloEdit2View::OnFileSaveAs)
	ON_COMMAND(ID_FILE_NEW, &CDiabloEdit2View::OnFileNew)
END_MESSAGE_MAP()

// CDiabloEdit2View 构造/析构

CDiabloEdit2View::CDiabloEdit2View() noexcept
	: CFormView(IDD_DIABLOEDIT2_FORM)
	, m_dlgTabPage(0)
{
	// TODO: 在此处添加构造代码

}

CDiabloEdit2View::~CDiabloEdit2View()
{
	if(m_dlgTabPage){
		for(int i = 0;i < m_nTabPageCount;++i)
			delete m_dlgTabPage[i];
		delete [] m_dlgTabPage;
		m_dlgTabPage = 0;
	}
}

void CDiabloEdit2View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tcTab);
}

void CDiabloEdit2View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	//初始化界面
	InitUI();
	RefreshUI();
}

// CDiabloEdit2View 诊断

#ifdef _DEBUG
void CDiabloEdit2View::AssertValid() const
{
	CFormView::AssertValid();
}

void CDiabloEdit2View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDiabloEdit2Doc* CDiabloEdit2View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDiabloEdit2Doc)));
	return (CDiabloEdit2Doc*)m_pDocument;
}
#endif //_DEBUG


// CDiabloEdit2View 消息处理程序

void CDiabloEdit2View::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	m_dlgTabPage[m_nTabCurSel]->ShowWindow(SW_HIDE);
	m_nTabCurSel = m_tcTab.GetCurSel();
	m_dlgTabPage[m_nTabCurSel]->ShowWindow(SW_SHOW);
}

void CDiabloEdit2View::OnPaint()
{
	RefreshUI();
	CFormView::OnPaint();
}

void CDiabloEdit2View::InitUI(void)
{
	if(!m_dlgTabPage){
		m_tcTab.InsertItem(0,_T(""));
		m_tcTab.InsertItem(1,_T(""));
		//在此处添加新的属性页
		m_nTabPageCount = m_tcTab.GetItemCount();

		m_dlgTabPage = new CCharacterDialogBase*[m_nTabPageCount];
		m_dlgTabPage[0] = new CDlgCharBasicInfo;
		m_dlgTabPage[0]->Create(CDlgCharBasicInfo::IDD, &m_tcTab);
		m_dlgTabPage[1] = new CDlgCharItems;
		m_dlgTabPage[1]->Create(CDlgCharItems::IDD, &m_tcTab);
		m_dlgTabPage[1]->ShowWindow(SW_HIDE);
		//在此处添加新的属性窗体
		m_nTabCurSel = 0;
		m_tcTab.SetCurSel(m_nTabCurSel);
		m_dlgTabPage[m_nTabCurSel]->ShowWindow(SW_SHOW);

		LoadText();

		// resize views
		ResizeParentToFit();
		RefreshUI();
	}
}

void CDiabloEdit2View::RefreshUI(void)
{
	// In case of resize, the tab control needs to resize
	CRect rect;
	GetClientRect(&rect);
	m_tcTab.MoveWindow(rect);
	m_tcTab.GetClientRect(&rect);
	m_tcTab.AdjustRect(FALSE, &rect);
	if (m_dlgTabPage)
		for (int i = 0; i < m_nTabPageCount; ++i)
			m_dlgTabPage[i]->MoveWindow(rect);
}

void CDiabloEdit2View::OnFileNew()
{
	const INT INIT_POINTS[7][7] = {
		{20,25,15,20,84 << 8,50 << 8,15 << 8},		//亚马逊
		{10,25,35,10,74 << 8,40 << 8,35 << 8},		//法师
		{15,25,25,15,79 << 8,45 << 8,25 << 8},		//死灵法师
		{25,20,15,25,89 << 8,55 << 8,15 << 8},		//圣骑士
		{30,20,10,25,92 << 8,55 << 8,10 << 8},		//野蛮人
		{15,20,20,25,84 << 8,55 << 8,20 << 8},		//德鲁依
		{20,20,25,20,95 << 8,50 << 8,25 << 8},		//刺客
	};
	if(m_sPathName.GetLength() && m_sTitle.GetLength()){
		int mb = MessageBox(CSFormat(::theApp.MsgBoxInfo(8), m_sTitle), ::theApp.MsgWarning(), MB_YESNOCANCEL | MB_ICONWARNING);
		if(mb == IDCANCEL)
			return;
		else if(mb == IDOK)
			OnFileSave();
	}
	int sel;
	CDlgSelectChar dlg(sel);
	if (dlg.DoModal() == IDOK) {
		m_Character = ::theApp.NewCharacter();
		for (int i = 0; i < 4; ++i)
			m_Character.PlayerStats.m_adwValue[i] = INIT_POINTS[sel][i];
		m_Character.PlayerStats.m_adwValue[0xA] =
			m_Character.PlayerStats.m_adwValue[0xB] = INIT_POINTS[sel][4];
		m_Character.PlayerStats.m_adwValue[6] =
			m_Character.PlayerStats.m_adwValue[7] = INIT_POINTS[sel][5];
		m_Character.PlayerStats.m_adwValue[8] =
			m_Character.PlayerStats.m_adwValue[9] = INIT_POINTS[sel][6];
		m_Character.charClass = sel;
		m_Character.dwTime = DWORD(CTime::GetCurrentTime().GetTime());
		GetDocument()->SetTitle(m_sTitle = _T("new"));
		m_sPathName.Empty();
		for (int i = 0; i < m_nTabPageCount; ++i)
			m_dlgTabPage[i]->UpdateUI(m_Character);
	}
}

void CDiabloEdit2View::OnFileOpen()
{
	CFileDialog open(TRUE,_T("d2s"),0,OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,_T("Diablo II Character(*.d2s)|*.d2s|All File(*.*)|*.*||"));
	if(open.DoModal() == IDOK){
		if(m_sPathName.GetLength()){
			int mb = MessageBox(CSFormat(::theApp.MsgBoxInfo(8), m_sTitle), ::theApp.MsgWarning(), MB_YESNOCANCEL | MB_ICONWARNING);
			if(mb == IDCANCEL)
				return;
			else if(mb == IDOK)
				OnFileSave();
		}
		m_sPathName = open.GetPathName();
		if (ReadD2sFile(m_sPathName)) {
			for(int i = 0;i < m_nTabPageCount;++i)
				m_dlgTabPage[i]->UpdateUI(m_Character);
			m_sTitle = open.GetFileTitle();
			GetDocument()->SetTitle(m_sTitle);
			GetDocument()->SetPathName(m_sPathName);
		}else
			m_sPathName.Empty();
	}
}

BOOL CDiabloEdit2View::GatherAllData(void)
{
	for(int i = 0;i < m_nTabPageCount;++i)
		if(!m_dlgTabPage[i]->GatherData(m_Character))
			return FALSE;
	return TRUE;
}

void CDiabloEdit2View::LoadText()
{
    TCITEM tci;
    tci.mask = TCIF_TEXT;
	GetDocument()->SetTitle(::theApp.MsgNoTitle());
	//系统菜单
	CMenu * systemMenu = dynamic_cast<CMainFrame *>(GetParentFrame())->GetMenu();
	ASSERT(systemMenu);
	systemMenu->ModifyMenu(0, MF_BYPOSITION, 0, ::theApp.SystemMenu(0));
	systemMenu->ModifyMenu(1, MF_BYPOSITION, 0, ::theApp.SystemMenu(1));
	systemMenu->ModifyMenu(2, MF_BYPOSITION, 0, ::theApp.SystemMenu(2));
	systemMenu->ModifyMenu(ID_FILE_NEW, MF_BYCOMMAND, ID_FILE_NEW, ::theApp.SystemMenu(3));
	systemMenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND, ID_FILE_OPEN, ::theApp.SystemMenu(4));
	systemMenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND, ID_FILE_SAVE, ::theApp.SystemMenu(5));
	systemMenu->ModifyMenu(ID_FILE_SAVE_AS, MF_BYCOMMAND, ID_FILE_SAVE_AS, ::theApp.SystemMenu(6));
	systemMenu->ModifyMenu(ID_CLOSE_FILE, MF_BYCOMMAND, ID_CLOSE_FILE, ::theApp.SystemMenu(7));
	systemMenu->ModifyMenu(ID_APP_EXIT, MF_BYCOMMAND, ID_APP_EXIT, ::theApp.SystemMenu(8));
	systemMenu->ModifyMenu(ID_VIEW_TOOLBAR, MF_BYCOMMAND, ID_VIEW_TOOLBAR, ::theApp.SystemMenu(9));
	systemMenu->ModifyMenu(ID_VIEW_STATUS_BAR, MF_BYCOMMAND, ID_VIEW_STATUS_BAR, ::theApp.SystemMenu(10));
	systemMenu->ModifyMenu(ID_APP_ABOUT, MF_BYCOMMAND, ID_APP_ABOUT, ::theApp.SystemMenu(12));
	CMenu * subMenu = systemMenu->GetSubMenu(1);
	ASSERT(subMenu);
	subMenu->ModifyMenu(2, MF_BYPOSITION, 0, ::theApp.SystemMenu(11));
	//TAB
    tci.pszText = (LPWSTR)::theApp.CharBasicInfoUI(0).GetString();
    m_tcTab.SetItem(0,&tci);
    tci.pszText = (LPWSTR)::theApp.OtherUI(9).GetString();
    m_tcTab.SetItem(1,&tci);
	//子窗口
	for (int i = 0; i < m_nTabPageCount; ++i)
		m_dlgTabPage[i]->LoadText();
	Invalidate();
}

BOOL CDiabloEdit2View::ReadD2sFile(const CString & path) {
	try {
		m_Character.ReadFile(path);
	} catch (const CString & msg) {
		MessageBox(msg, ::theApp.MsgError(), MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

BOOL CDiabloEdit2View::WriteD2sFile(const CString & path) {
	try {
		m_Character.WriteFile(path);
	} catch (const CString & msg) {
		MessageBox(msg, ::theApp.MsgError(), MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

void CDiabloEdit2View::OnFileSave()
{
	if(m_sTitle.GetLength() > 0 && GatherAllData()){
		if(m_sPathName.GetLength()){
			if(m_sTitle.MakeLower() == m_Character.name().MakeLower() ||
				MessageBox(::theApp.MsgBoxInfo(9), ::theApp.MsgWarning(), MB_YESNO | MB_ICONWARNING) == IDYES)
			{
				WriteD2sFile(m_sPathName);
				GetDocument()->SetTitle(m_sTitle = m_Character.name());
			}
		}else{
			CFileDialog save_as(FALSE, 0, m_Character.name() + _T(".d2s"), OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("Diablo II Character(*.d2s)|*.d2s|All File(*.*)|*.*||"));
			if(save_as.DoModal() == IDOK){
				m_sPathName = save_as.GetPathName();
				WriteD2sFile(m_sPathName);
				GetDocument()->SetTitle(m_sTitle = m_Character.name());
				GetDocument()->SetPathName(m_sPathName);
			}
		}
	}
}

void CDiabloEdit2View::OnFileSaveAs()
{
	if(m_sTitle.GetLength() && GatherAllData()){
		CFileDialog save_as(FALSE, 0, m_Character.name() + _T(".d2s"), OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("Diablo II Character(*.d2s)|*.d2s|All File(*.*)|*.*||"));
		if(save_as.DoModal() == IDOK){
			m_sPathName = save_as.GetPathName();
			WriteD2sFile(m_sPathName);
			GetDocument()->SetTitle(m_sTitle = m_Character.name());
			GetDocument()->SetPathName(m_sPathName);
		}
	}
}

void CDiabloEdit2View::OnCloseFile()
{
	if(m_sTitle.GetLength()){
		m_sTitle = m_sPathName = _T("");
		GetDocument()->SetTitle(::theApp.MsgNoTitle());
		if(m_dlgTabPage)
			for(int i = 0;i < m_nTabPageCount;++i)
				m_dlgTabPage[i]->ResetAll();
		m_Character.Reset();
	}
}

void CDiabloEdit2View::OnUpdateLanguage1(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(::theApp.m_nLangIndex == 0);
	pCmdUI->SetText(::theApp.LangTitle(0));
}
void CDiabloEdit2View::OnUpdateLanguage2(CCmdUI *pCmdUI)
{
	if(::theApp.LangCount() > 1){ 
		pCmdUI->SetRadio(::theApp.m_nLangIndex == 1);
		pCmdUI->SetText(::theApp.LangTitle(1));
	}else
		pCmdUI->Enable(FALSE);
}
void CDiabloEdit2View::OnUpdateLanguage3(CCmdUI *pCmdUI)
{
	if(::theApp.LangCount() > 2){ 
		pCmdUI->SetRadio(::theApp.m_nLangIndex == 2);
		pCmdUI->SetText(::theApp.LangTitle(2));
	}else
		pCmdUI->Enable(FALSE);
}
void CDiabloEdit2View::OnLanguage1()
{
	if(::theApp.m_nLangIndex != 0 && m_dlgTabPage){
		::theApp.m_nLangIndex = 0;
        LoadText();
	}
}
void CDiabloEdit2View::OnLanguage2()
{
	if(::theApp.m_nLangIndex != 1 && m_dlgTabPage){
		::theApp.m_nLangIndex = 1;
        LoadText();
	}
}
void CDiabloEdit2View::OnLanguage3()
{
	if(::theApp.m_nLangIndex != 2 && m_dlgTabPage){
		::theApp.m_nLangIndex = 2;
        LoadText();
	}
}

