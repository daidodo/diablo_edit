// DlgSkills.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgSkills.h"

// CDlgSkills 对话框

IMPLEMENT_DYNAMIC(CDlgSkills, CDialog)

CDlgSkills::CDlgSkills(int charClass,BYTE * skills,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSkills::IDD, pParent)
	, m_nCharClass(charClass)
	, m_pData(skills)
	, m_bAll(0)
{
	ASSERT(m_nCharClass >= 0 && m_nCharClass < ::theApp.CLASS_NAME_SIZE && m_pData);
	for(int i = 0;i < SKILL_SIZE;++i)
		m_pSkills[::theApp.CLASS_SKILL_INDEX[m_nCharClass][i]] = m_pData[i];
}

CDlgSkills::~CDlgSkills()
{}

void CDlgSkills::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_Skill1, m_sText[0]);
	DDX_Text(pDX, IDC_STATIC_Skill2, m_sText[1]);
	DDX_Text(pDX, IDC_STATIC_Skill3, m_sText[2]);
	DDX_Text(pDX, IDC_STATIC_Skill4, m_sText[3]);
	DDX_Text(pDX, IDC_STATIC_Skill5, m_sText[4]);
	DDX_Text(pDX, IDC_STATIC_Skill6, m_sText[5]);
	DDX_Text(pDX, IDC_STATIC_Skill7, m_sText[6]);
	DDX_Text(pDX, IDC_STATIC_Skill8, m_sText[7]);
	DDX_Text(pDX, IDC_STATIC_Skill9, m_sText[8]);
	DDX_Text(pDX, IDC_STATIC_Skill10, m_sText[9]);
	DDX_Text(pDX, IDC_STATIC_Skill11, m_sText[10]);
	DDX_Text(pDX, IDC_STATIC_Skill12, m_sText[11]);
	DDX_Text(pDX, IDC_STATIC_Skill13, m_sText[12]);
	DDX_Text(pDX, IDC_STATIC_Skill14, m_sText[13]);
	DDX_Text(pDX, IDC_STATIC_Skill15, m_sText[14]);
	DDX_Text(pDX, IDC_STATIC_Skill16, m_sText[15]);
	DDX_Text(pDX, IDC_STATIC_Skill17, m_sText[16]);
	DDX_Text(pDX, IDC_STATIC_Skill18, m_sText[17]);
	DDX_Text(pDX, IDC_STATIC_Skill19, m_sText[18]);
	DDX_Text(pDX, IDC_STATIC_Skill20, m_sText[19]);
	DDX_Text(pDX, IDC_STATIC_Skill21, m_sText[20]);
	DDX_Text(pDX, IDC_STATIC_Skill22, m_sText[21]);
	DDX_Text(pDX, IDC_STATIC_Skill23, m_sText[22]);
	DDX_Text(pDX, IDC_STATIC_Skill24, m_sText[23]);
	DDX_Text(pDX, IDC_STATIC_Skill25, m_sText[24]);
	DDX_Text(pDX, IDC_STATIC_Skill26, m_sText[25]);
	DDX_Text(pDX, IDC_STATIC_Skill27, m_sText[26]);
	DDX_Text(pDX, IDC_STATIC_Skill28, m_sText[27]);
	DDX_Text(pDX, IDC_STATIC_Skill29, m_sText[28]);
	DDX_Text(pDX, IDC_STATIC_Skill30, m_sText[29]);
	DDX_Text(pDX, IDC_STATIC_Tree1, m_sText[30]);
	DDX_Text(pDX, IDC_STATIC_Tree2, m_sText[31]);
	DDX_Text(pDX, IDC_STATIC_Tree3, m_sText[32]);
	DDX_Text(pDX, IDC_EDIT1, m_pSkills[0]);
	DDX_Text(pDX, IDC_EDIT2, m_pSkills[1]);
	DDX_Text(pDX, IDC_EDIT3, m_pSkills[2]);
	DDX_Text(pDX, IDC_EDIT4, m_pSkills[3]);
	DDX_Text(pDX, IDC_EDIT5, m_pSkills[4]);
	DDX_Text(pDX, IDC_EDIT6, m_pSkills[5]);
	DDX_Text(pDX, IDC_EDIT7, m_pSkills[6]);
	DDX_Text(pDX, IDC_EDIT8, m_pSkills[7]);
	DDX_Text(pDX, IDC_EDIT9, m_pSkills[8]);
	DDX_Text(pDX, IDC_EDIT10, m_pSkills[9]);
	DDX_Text(pDX, IDC_EDIT11, m_pSkills[10]);
	DDX_Text(pDX, IDC_EDIT12, m_pSkills[11]);
	DDX_Text(pDX, IDC_EDIT13, m_pSkills[12]);
	DDX_Text(pDX, IDC_EDIT14, m_pSkills[13]);
	DDX_Text(pDX, IDC_EDIT15, m_pSkills[14]);
	DDX_Text(pDX, IDC_EDIT16, m_pSkills[15]);
	DDX_Text(pDX, IDC_EDIT17, m_pSkills[16]);
	DDX_Text(pDX, IDC_EDIT18, m_pSkills[17]);
	DDX_Text(pDX, IDC_EDIT19, m_pSkills[18]);
	DDX_Text(pDX, IDC_EDIT20, m_pSkills[19]);
	DDX_Text(pDX, IDC_EDIT21, m_pSkills[20]);
	DDX_Text(pDX, IDC_EDIT22, m_pSkills[21]);
	DDX_Text(pDX, IDC_EDIT23, m_pSkills[22]);
	DDX_Text(pDX, IDC_EDIT24, m_pSkills[23]);
	DDX_Text(pDX, IDC_EDIT25, m_pSkills[24]);
	DDX_Text(pDX, IDC_EDIT26, m_pSkills[25]);
	DDX_Text(pDX, IDC_EDIT27, m_pSkills[26]);
	DDX_Text(pDX, IDC_EDIT28, m_pSkills[27]);
	DDX_Text(pDX, IDC_EDIT29, m_pSkills[28]);
	DDX_Text(pDX, IDC_EDIT30, m_pSkills[29]);
	DDX_Text(pDX, IDC_EDIT31, m_bAll);
}

void CDlgSkills::LoadText()
{
	SetWindowText(::theApp.OtherUI(1 + m_nCharClass));
	for(int i = 0;i < SKILL_SIZE;++i)
		m_sText[i] = ::theApp.ClassSkillName(i, m_nCharClass);
	for(int i = 0;i < TAB_SIZE;++i)
		m_sText[30 + i] = ::theApp.ClassSkillTabName(i, m_nCharClass);
	UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(CDlgSkills, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgSkills::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgSkills::OnBnClickedButton1)
END_MESSAGE_MAP()

BOOL CDlgSkills::OnInitDialog()
{
	CDialog::OnInitDialog();
	LoadText();
	return TRUE;
}

#include <fstream>
void CDlgSkills::OnBnClickedOk()
{
	//保存人物技能等级
	UpdateData(TRUE);
	//数据有效性检验
	for(int i = 0;i < SKILL_SIZE;++i)
		if(m_pSkills[i] > 127){
			::MessageBox(0, ::theApp.MsgBoxInfo(5),::theApp.MsgWarning(),MB_ICONWARNING);
			return;
		}
	for(int i = 0;i < SKILL_SIZE;++i)
		m_pData[i] = m_pSkills[::theApp.CLASS_SKILL_INDEX[m_nCharClass][i]]; 
	OnOK();
}

void CDlgSkills::OnBnClickedButton1()
{
	UpdateData(TRUE);
	if(m_bAll > 127)
		::MessageBox(0, ::theApp.MsgBoxInfo(5),::theApp.MsgWarning(),MB_ICONWARNING);
	else{
		::FillMemory(m_pSkills,sizeof(m_pSkills),m_bAll);
		UpdateData(FALSE);
	}
}
