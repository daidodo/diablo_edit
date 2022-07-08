// DlgQuestInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgQuestInfo.h"

const WORD CDlgQuestInfo::QUEST_COMPLETE[27] = {
	0x1001,0x101d,0x900d,0x101d,0x1055,0x101d,		//Act I
	0x101d,0x1c39,0x100d,0x1181,0x1005,0x1e25,		//Act II
	0x1001,0x10fd,0x11d9,0x1001,0x100d,0x1871,		//Act III
	0x1001,0x1301,0x1001,							//Act IV
	0x9021,0x1001,0x178d,0x901d,0x132d,0x169d		//Act V
};

// CDlgQuestInfo 对话框

IMPLEMENT_DYNAMIC(CDlgQuestInfo, CCharacterDialogBase)

CDlgQuestInfo::CDlgQuestInfo(CWnd* pParent /*=NULL*/)
	: CCharacterDialogBase(CDlgQuestInfo::IDD, pParent)
	, m_nLevel(0)
{
	::ZeroMemory(m_bUIData,sizeof(m_bUIData));
	for(int i = 0;i < LEVEL_SIZE;++i)
		::ZeroMemory(m_bQuestInfo[i],sizeof(m_bQuestInfo[0]));
}

CDlgQuestInfo::~CDlgQuestInfo()
{
}

void CDlgQuestInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_nLevel);
	DDV_MinMaxInt(pDX, m_nLevel, 0, 2);
	//Act I
	DDX_Check(pDX, IDC_CHECK1, m_bUIData[0]);
	DDX_Check(pDX, IDC_CHECK2, m_bUIData[1]);
	DDX_Check(pDX, IDC_CHECK5, m_bUIData[2]);
	DDX_Check(pDX, IDC_CHECK3, m_bUIData[3]);
	DDX_Check(pDX, IDC_CHECK4, m_bUIData[4]);
	DDX_Check(pDX, IDC_CHECK6, m_bUIData[5]);
	DDX_Check(pDX, IDC_CHECK29, m_bUIData[28]);
	//Act II
	DDX_Check(pDX, IDC_CHECK7, m_bUIData[6]);
	DDX_Check(pDX, IDC_CHECK8, m_bUIData[7]);
	DDX_Check(pDX, IDC_CHECK9, m_bUIData[8]);
	DDX_Check(pDX, IDC_CHECK10, m_bUIData[9]);
	DDX_Check(pDX, IDC_CHECK11, m_bUIData[10]);
	DDX_Check(pDX, IDC_CHECK12, m_bUIData[11]);
	//Act III
	DDX_Check(pDX, IDC_CHECK16, m_bUIData[12]);
	DDX_Check(pDX, IDC_CHECK15, m_bUIData[13]);
	DDX_Check(pDX, IDC_CHECK14, m_bUIData[14]);
	DDX_Check(pDX, IDC_CHECK13, m_bUIData[15]);
	DDX_Check(pDX, IDC_CHECK17, m_bUIData[16]);
	DDX_Check(pDX, IDC_CHECK18, m_bUIData[17]);
	//Act IV
	DDX_Check(pDX, IDC_CHECK19, m_bUIData[18]);
	DDX_Check(pDX, IDC_CHECK21, m_bUIData[19]);
	DDX_Check(pDX, IDC_CHECK20, m_bUIData[20]);
	//Act V
	DDX_Check(pDX, IDC_CHECK22, m_bUIData[21]);
	DDX_Check(pDX, IDC_CHECK23, m_bUIData[22]);
	DDX_Check(pDX, IDC_CHECK24, m_bUIData[23]);
	DDX_Check(pDX, IDC_CHECK25, m_bUIData[24]);
	DDX_Check(pDX, IDC_CHECK26, m_bUIData[25]);
	DDX_Check(pDX, IDC_CHECK27, m_bUIData[26]);
	//Addition
	DDX_Check(pDX, IDC_CHECK28, m_bUIData[27]);
	//Text string
	DDX_Text(pDX, IDC_CHECK1, m_sText[0]);
	DDX_Text(pDX, IDC_CHECK2, m_sText[1]);
	DDX_Text(pDX, IDC_CHECK3, m_sText[2]);
	DDX_Text(pDX, IDC_CHECK4, m_sText[3]);
	DDX_Text(pDX, IDC_CHECK5, m_sText[4]);
	DDX_Text(pDX, IDC_CHECK6, m_sText[5]);
	DDX_Text(pDX, IDC_CHECK7, m_sText[6]);
	DDX_Text(pDX, IDC_CHECK8, m_sText[7]);
	DDX_Text(pDX, IDC_CHECK9, m_sText[8]);
	DDX_Text(pDX, IDC_CHECK10, m_sText[9]);
	DDX_Text(pDX, IDC_CHECK11, m_sText[10]);
	DDX_Text(pDX, IDC_CHECK12, m_sText[11]);
	DDX_Text(pDX, IDC_CHECK13, m_sText[12]);
	DDX_Text(pDX, IDC_CHECK14, m_sText[13]);
	DDX_Text(pDX, IDC_CHECK15, m_sText[14]);
	DDX_Text(pDX, IDC_CHECK16, m_sText[15]);
	DDX_Text(pDX, IDC_CHECK17, m_sText[16]);
	DDX_Text(pDX, IDC_CHECK18, m_sText[17]);
	DDX_Text(pDX, IDC_CHECK19, m_sText[18]);
	DDX_Text(pDX, IDC_CHECK20, m_sText[19]);
	DDX_Text(pDX, IDC_CHECK21, m_sText[20]);
	DDX_Text(pDX, IDC_CHECK22, m_sText[21]);
	DDX_Text(pDX, IDC_CHECK23, m_sText[22]);
	DDX_Text(pDX, IDC_CHECK24, m_sText[23]);
	DDX_Text(pDX, IDC_CHECK25, m_sText[24]);
	DDX_Text(pDX, IDC_CHECK26, m_sText[25]);
	DDX_Text(pDX, IDC_CHECK27, m_sText[26]);
	DDX_Text(pDX, IDC_CHECK28, m_sText[27]);
	DDX_Text(pDX, IDC_CHECK29, m_sText[28]);
	DDX_Text(pDX, IDC_RADIO1, m_sText[29]);
	DDX_Text(pDX, IDC_RADIO2, m_sText[30]);
	DDX_Text(pDX, IDC_RADIO3, m_sText[31]);
	DDX_Control(pDX, IDC_CHECK29, m_cbResetStats);
}

BEGIN_MESSAGE_MAP(CDlgQuestInfo, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgQuestInfo::OnBnClicked_Normal)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgQuestInfo::OnBnClicked_Nightmare)
	ON_BN_CLICKED(IDC_RADIO3, &CDlgQuestInfo::OnBnClicked_Hell)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgQuestInfo::OnBnClickedCheck1)
END_MESSAGE_MAP()

void CDlgQuestInfo::UpdateUI(const CD2S_Struct & character)
{
	for(int i = 0;i < LEVEL_SIZE;++i){
		for(int j = 0;j < 6;++j){
			m_bQuestInfo[i][j] = (character.QuestInfo.QIData[i].wActI[j] & 1) != 0;
			m_bQuestInfo[i][6 + j] = (character.QuestInfo.QIData[i].wActII[j] & 1) != 0;
			m_bQuestInfo[i][12 + j] = (character.QuestInfo.QIData[i].wActIII[j] & 1) != 0;
			m_bQuestInfo[i][21 + j] = (character.QuestInfo.QIData[i].wActV[j] & 1) != 0;
			if(j < 3)
				m_bQuestInfo[i][18 + j] = (character.QuestInfo.QIData[i].wActIV[j] & 1) != 0;
		}
		m_bQuestInfo[i][27] = (character.QuestInfo.QIData[i].wActI[3] & 0x400) != 0;
		m_bQuestInfo[i][28] = character.QuestInfo.QIData[i].bResetStats == 1;
	}
	::CopyMemory(m_bUIData,m_bQuestInfo[m_nLevel],sizeof(m_bUIData));
	m_cbResetStats.EnableWindow(m_bUIData[0]);
	UpdateData(FALSE);
}

BOOL CDlgQuestInfo::GatherData(CD2S_Struct & character)
{
	UpdateData();
	::CopyMemory(m_bQuestInfo[m_nLevel],m_bUIData,sizeof(m_bUIData));
	for(int i = 0;i < LEVEL_SIZE;++i){
		for(int j = 0;j < 6;++j){
			if(((character.QuestInfo.QIData[i].wActI)[j] & 1) != m_bQuestInfo[i][j])
				(character.QuestInfo.QIData[i].wActI)[j] = (m_bQuestInfo[i][j] ? QUEST_COMPLETE[j] : 0);
			if(((character.QuestInfo.QIData[i].wActII)[j] & 1) != m_bQuestInfo[i][6 + j])
				(character.QuestInfo.QIData[i].wActII)[j] = (m_bQuestInfo[i][6 + j] ? QUEST_COMPLETE[6 + j] : 0);
			if(((character.QuestInfo.QIData[i].wActIII)[j] & 1) != m_bQuestInfo[i][12 + j])
				(character.QuestInfo.QIData[i].wActIII)[j] = (m_bQuestInfo[i][12 + j] ? QUEST_COMPLETE[12 + j] : 0);
			if(((character.QuestInfo.QIData[i].wActV)[j] & 1) != m_bQuestInfo[i][21 + j])
				(character.QuestInfo.QIData[i].wActV)[j] = (m_bQuestInfo[i][21 + j] ? QUEST_COMPLETE[21 + j] : 0);
			if(j < 3 && ((character.QuestInfo.QIData[i].wActIV)[j] & 1) != m_bQuestInfo[i][18 + j])
				(character.QuestInfo.QIData[i].wActIV)[j] = (m_bQuestInfo[i][18 + j] ? QUEST_COMPLETE[18 + j] : 0);
			m_bQuestInfo[i][27] ? (character.QuestInfo.QIData[i].wActI)[3] |= 0x400 : (character.QuestInfo.QIData[i].wActI)[3] &= ~0x400;
			character.QuestInfo.QIData[i].bResetStats = m_bQuestInfo[i][0] ? (m_bQuestInfo[i][28] ? 1 : 2) : 0;
		}
	}
	return TRUE;
}

void CDlgQuestInfo::ResetAll()
{
	m_nLevel = 0;
	for(int i = 0;i < LEVEL_SIZE;++i)
		::ZeroMemory(m_bQuestInfo[i],sizeof(m_bQuestInfo[i]));
	::ZeroMemory(m_bUIData,sizeof(m_bUIData));
	m_cbResetStats.EnableWindow(m_bUIData[0]);
	UpdateData(FALSE);
}

void CDlgQuestInfo::LoadText(void)
{
	for(int i = 0;i < QUEST_NAME_SIZE;++i)
		m_sText[i] = ::theApp.QuestName(i);
	for (UINT i = 0; i < LEVEL_SIZE; ++i)
		m_sText[QUEST_NAME_SIZE + i] = ::theApp.DifficultyName(i);
	m_cbResetStats.EnableWindow(m_bUIData[0]);
	UpdateData(FALSE);
}

// CDlgQuestInfo 消息处理程序

void CDlgQuestInfo::OnBnClicked_Normal()
{
	if(m_nLevel != 0){
        INT old = m_nLevel;
        UpdateData();
		::CopyMemory(m_bQuestInfo[old],m_bUIData,sizeof(m_bUIData));
		m_nLevel = 0;
		::CopyMemory(m_bUIData,m_bQuestInfo[0],sizeof(m_bUIData));
		m_cbResetStats.EnableWindow(m_bUIData[0]);
		UpdateData(FALSE);
	}
}

void CDlgQuestInfo::OnBnClicked_Nightmare()
{
	if(m_nLevel != 1){
        INT old = m_nLevel;
        UpdateData();
		::CopyMemory(m_bQuestInfo[old],m_bUIData,sizeof(m_bUIData));
		m_nLevel = 1;
		::CopyMemory(m_bUIData,m_bQuestInfo[1],sizeof(m_bUIData));
		m_cbResetStats.EnableWindow(m_bUIData[0]);
		UpdateData(FALSE);
	}
}

void CDlgQuestInfo::OnBnClicked_Hell()
{
	if(m_nLevel != 2){
        INT old = m_nLevel;
        UpdateData();
		::CopyMemory(m_bQuestInfo[old],m_bUIData,sizeof(m_bUIData));
		m_nLevel = 2;
		::CopyMemory(m_bUIData,m_bQuestInfo[2],sizeof(m_bUIData));
		m_cbResetStats.EnableWindow(m_bUIData[0]);
		UpdateData(FALSE);
	}
}

void CDlgQuestInfo::OnBnClickedCheck1()
{
	UpdateData();
	m_cbResetStats.EnableWindow(m_bUIData[0]);
	UpdateData(FALSE);
}
