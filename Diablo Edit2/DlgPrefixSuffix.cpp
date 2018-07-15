// DlgPrefixSuffix.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgPrefixSuffix.h"


// CDlgPrefixSuffix 对话框

IMPLEMENT_DYNAMIC(CDlgPrefixSuffix, CDialog)

CDlgPrefixSuffix::CDlgPrefixSuffix(int quality,int * selIndex,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPrefixSuffix::IDD, pParent)
    , m_nQuality(quality)
    , m_vSelect(selIndex)
{}

CDlgPrefixSuffix::~CDlgPrefixSuffix()
{}

void CDlgPrefixSuffix::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO1, m_cbFirstName);
    DDX_Control(pDX, IDC_COMBO2, m_cbLastName);
    DDX_Control(pDX, IDC_COMBO3, m_cbPrefix1);
    DDX_Control(pDX, IDC_COMBO4, m_cbSuffix1);
    DDX_Control(pDX, IDC_COMBO5, m_cbPrefix2);
    DDX_Control(pDX, IDC_COMBO7, m_cbSuffix2);
    DDX_Control(pDX, IDC_COMBO8, m_cbPrefix3);
    DDX_Control(pDX, IDC_COMBO9, m_cbSuffix3);
    DDX_Control(pDX, IDC_COMBO10, m_cbUniqueName);
    DDX_Control(pDX, IDC_COMBO11, m_cbSubType);
    DDX_Text(pDX, IDC_STATIC01, m_sText[0]);
    DDX_Text(pDX, IDC_STATIC02, m_sText[1]);
    DDX_Text(pDX, IDC_STATIC03, m_sText[2]);
    DDX_Text(pDX, IDC_STATIC04, m_sText[3]);
    DDX_Text(pDX, IDC_STATIC05, m_sText[4]);
    DDX_Text(pDX, IDC_STATIC06, m_sText[5]);
    DDX_Text(pDX, IDC_STATIC07, m_sText[6]);
    DDX_Text(pDX, IDC_STATIC08, m_sText[7]);
    DDX_Text(pDX, IDC_STATIC09, m_sText[8]);
    DDX_Text(pDX, IDC_STATIC10, m_sText[9]);
}


BEGIN_MESSAGE_MAP(CDlgPrefixSuffix, CDialog)
END_MESSAGE_MAP()

//自定义函数

// CDlgPrefixSuffix 消息处理程序
BOOL CDlgPrefixSuffix::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetWindowText(::theApp.PrefixSuffixUI(0));
    for(int i = 0;i < 10;++i)
        m_sText[i] = ::theApp.PrefixSuffixUI(1 + i);
    switch(m_nQuality){
        case 1:case 3:{ //low or high
            m_cbFirstName.EnableWindow(FALSE);
            m_cbLastName.EnableWindow(FALSE);
            m_cbPrefix1.EnableWindow(FALSE);
            m_cbSuffix1.EnableWindow(FALSE);
            m_cbPrefix2.EnableWindow(FALSE);
            m_cbSuffix2.EnableWindow(FALSE);
            m_cbPrefix3.EnableWindow(FALSE);
            m_cbSuffix3.EnableWindow(FALSE);
            m_cbUniqueName.EnableWindow(FALSE);
            m_cbSubType.EnableWindow();
            WCHAR tmp[] = _T("0");
            for(;tmp[0] < _T('8');++tmp[0])
                m_cbSubType.InsertString(-1,tmp);
            m_cbSubType.SetCurSel(m_vSelect[9]);
            break;}
        case 4:         //magic
            m_cbFirstName.EnableWindow(FALSE);
            m_cbLastName.EnableWindow(FALSE);
            m_cbPrefix1.EnableWindow();
            m_cbSuffix1.EnableWindow();
            m_cbPrefix2.EnableWindow(FALSE);
            m_cbSuffix2.EnableWindow(FALSE);
            m_cbPrefix3.EnableWindow(FALSE);
            m_cbSuffix3.EnableWindow(FALSE);
            m_cbUniqueName.EnableWindow(FALSE);
            m_cbSubType.EnableWindow(FALSE);
            for(UINT i = 0;i < ::theApp.MagicPrefixSize();++i)
				m_cbPrefix1.InsertString(-1, ::theApp.MagicPrefix01(i));
            for(UINT i = 0;i < ::theApp.MagicSuffixSize();++i)
				m_cbSuffix1.InsertString(-1, ::theApp.MagicSuffix01(i));
            m_cbPrefix1.SetCurSel(m_vSelect[2]);
            m_cbSuffix1.SetCurSel(m_vSelect[3]);
            break;
        case 5:         //set
            m_cbFirstName.EnableWindow(FALSE);
            m_cbLastName.EnableWindow(FALSE);
            m_cbPrefix1.EnableWindow(FALSE);
            m_cbSuffix1.EnableWindow(FALSE);
            m_cbPrefix2.EnableWindow(FALSE);
            m_cbSuffix2.EnableWindow(FALSE);
            m_cbPrefix3.EnableWindow(FALSE);
            m_cbSuffix3.EnableWindow(FALSE);
            m_cbUniqueName.EnableWindow(FALSE);
            m_cbSubType.EnableWindow(FALSE);
            break;
        case 6:case 8:  //rare or crafted
            m_cbFirstName.EnableWindow();
            m_cbLastName.EnableWindow();
            m_cbPrefix1.EnableWindow();
            m_cbSuffix1.EnableWindow();
            m_cbPrefix2.EnableWindow();
            m_cbSuffix2.EnableWindow();
            m_cbPrefix3.EnableWindow();
            m_cbSuffix3.EnableWindow();
            m_cbUniqueName.EnableWindow(FALSE);
            m_cbSubType.EnableWindow(FALSE);
            for(UINT i = 0;i < ::theApp.RareCraftedNameSize();++i){
                m_cbFirstName.InsertString(-1, ::theApp.RareCraftedName01(i));
                m_cbLastName.InsertString(-1, ::theApp.RareCraftedName01(i));
            }
            for(UINT i = 0;i < ::theApp.MagicPrefixSize();++i){
                m_cbPrefix1.InsertString(-1, ::theApp.MagicPrefix01(i));
                m_cbPrefix2.InsertString(-1, ::theApp.MagicPrefix01(i));
                m_cbPrefix3.InsertString(-1, ::theApp.MagicPrefix01(i));
            }
            for(UINT i = 0;i < ::theApp.MagicSuffixSize();++i){
                m_cbSuffix1.InsertString(-1, ::theApp.MagicSuffix01(i));
                m_cbSuffix2.InsertString(-1, ::theApp.MagicSuffix01(i));
                m_cbSuffix3.InsertString(-1, ::theApp.MagicSuffix01(i));
            }
            m_cbFirstName.SetCurSel(m_vSelect[0]);
            m_cbLastName.SetCurSel(m_vSelect[1]);
            m_cbPrefix1.SetCurSel(m_vSelect[2]);
            m_cbSuffix1.SetCurSel(m_vSelect[3]);
            m_cbPrefix2.SetCurSel(m_vSelect[4]);
            m_cbSuffix2.SetCurSel(m_vSelect[5]);
            m_cbPrefix3.SetCurSel(m_vSelect[6]);
            m_cbSuffix3.SetCurSel(m_vSelect[7]);
            break;
        case 7:         //unique
            m_cbFirstName.EnableWindow(FALSE);
            m_cbLastName.EnableWindow(FALSE);
            m_cbPrefix1.EnableWindow(FALSE);
            m_cbSuffix1.EnableWindow(FALSE);
            m_cbPrefix2.EnableWindow(FALSE);
            m_cbSuffix2.EnableWindow(FALSE);
            m_cbPrefix3.EnableWindow(FALSE);
            m_cbSuffix3.EnableWindow(FALSE);
            m_cbUniqueName.EnableWindow();
            m_cbSubType.EnableWindow(FALSE);
            for(UINT i = 0;i < ::theApp.UniqueNameSize();++i)
                m_cbUniqueName.InsertString(-1, ::theApp.UniqueName01(i));
            m_cbUniqueName.SetCurSel(m_vSelect[8]);
            break;
        default:
            m_cbFirstName.EnableWindow(FALSE);
            m_cbLastName.EnableWindow(FALSE);
            m_cbPrefix1.EnableWindow(FALSE);
            m_cbSuffix1.EnableWindow(FALSE);
            m_cbPrefix2.EnableWindow(FALSE);
            m_cbSuffix2.EnableWindow(FALSE);
            m_cbPrefix3.EnableWindow(FALSE);
            m_cbSuffix3.EnableWindow(FALSE);
            m_cbUniqueName.EnableWindow(FALSE);
            m_cbSubType.EnableWindow(FALSE);
    }
    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
}

