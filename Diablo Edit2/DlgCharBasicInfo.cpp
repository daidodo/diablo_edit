// DlgCharBasicInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgCharBasicInfo.h"
#include "DlgWayPoints.h"
#include "DlgQuestInfo.h"
#include "DlgSkills.h"

const DWORD	CDlgCharBasicInfo::LEVEL_AND_EXPERIENCE[100] = {
	0,500,1500,3750,7875,14175,22680,32886,44396,57715,
	72144,90180,112725,140906,176132,220165,275207,344008,430010,537513,
	671891,839864,1049830,1312287,1640359,2050449,2563061,3203826,3902260,4663553,
	5493363,6397855,7383752,8458379,9629723,10906488,12298162,13815086,15468534,17270791,
	19235252,21376515,23710491,26254525,29027522,32050088,35344686,38935798,42850109,47116709,
	51767302,56836449,62361819,68384473,74949165,82104680,89904191,98405658,107672256,117772849,
	128782495,140783010,153863570,168121381,183662396,200602101,219066380,239192444,261129853,285041630,
	311105466,339515048,370481492,404234916,441026148,481128591,524840254,572485967,624419793,681027665,
	742730244,809986056,883294891,963201521,1050299747,1145236814,1248718217,1361512946,1484459201,1618470619,
	1764543065,1923762030,2097310703,2286478756,2492671933,2717422497,2962400612,3229426756,3520485254,3837739017
};

// CDlgCharBasicInfo 对话框

IMPLEMENT_DYNAMIC(CDlgCharBasicInfo, CCharacterDialogBase)

CDlgCharBasicInfo::CDlgCharBasicInfo(CWnd* pParent /*=NULL*/)
	: CCharacterDialogBase(CDlgCharBasicInfo::IDD, pParent)
	, m_dlgTabPage(0)
	, m_sVersion(_T(""))
	, m_sName(_T(""))
	, m_sCharTitle(_T(""))
	, m_uCharLevel(1)
	, m_bLadder(FALSE)
	, m_bExpansion(FALSE)
	, m_bHardcore(FALSE)
	, m_bDiedBefore(FALSE)
	, m_tTime(CTime::GetCurrentTime())
	, m_dwExperience(0)
	, m_dwStrength(0)
	, m_dwDexterity(0)
	, m_dwEnergy(0)
	, m_dwVitality(0)
	, m_dwStatPointsRemaining(0)
	, m_dwMaximumStamina(0)
	, m_dwCurrentStamina(0)
	, m_dwMaximumLife(0)
	, m_dwCurrentLife(0)
	, m_dwMaximumMana(0)
	, m_dwCurrentMana(0)
	, m_dwSkillPointsRemaining(0)
	, m_dwGoldInBody(0)
	, m_dwGoldInStash(0)
	, m_sGoldinPer(_T("0-10000"))
	, m_dwMaxGoldInBody(0)
	, m_dwMaxGoldInStash(0)
	, m_sGoldinStash(_T("0-50000"))
{
	::ZeroMemory(m_bSkills,sizeof(m_bSkills));
}

CDlgCharBasicInfo::~CDlgCharBasicInfo()
{
	if(m_dlgTabPage){
		for(int i = 0;i < m_nTabPageCount;++i)
			delete m_dlgTabPage[i];
		delete [] m_dlgTabPage;
		m_dlgTabPage = 0;
	}
}

void CDlgCharBasicInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sVersion);
	DDX_Control(pDX, IDC_COMBO1, m_cbCharClass);
	DDX_Check(pDX, IDC_CHECK1, m_bLadder);
	DDX_Check(pDX, IDC_CHECK2, m_bExpansion);
	DDX_Check(pDX, IDC_CHECK3, m_bHardcore);
	DDX_Text(pDX, IDC_EDIT2, m_sName);
	DDV_MaxChars(pDX, m_sName, 15);
	DDX_Check(pDX, IDC_CHECK4, m_bDiedBefore);
	DDX_Text(pDX, IDC_EDIT3, m_sCharTitle);
	DDX_Text(pDX, IDC_EDIT4, m_uCharLevel);
	DDX_Control(pDX, IDC_COMBO2, m_cbWeaponSet);
	DDX_Control(pDX, IDC_COMBO3, m_cbLastDifficult);
	DDX_Control(pDX, IDC_COMBO4, m_cbLastACT);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_tTime);
	DDX_Text(pDX, IDC_EDIT5, m_dwExperience);
	DDX_Control(pDX, IDC_COMBO6, m_cbLevelAndExp);
	DDX_Text(pDX, IDC_EDIT6, m_dwStrength);
	DDX_Text(pDX, IDC_EDIT7, m_dwDexterity);
	DDX_Text(pDX, IDC_EDIT8, m_dwVitality);
	DDX_Text(pDX, IDC_EDIT9, m_dwEnergy);
	DDX_Text(pDX, IDC_EDIT10, m_dwStatPointsRemaining);
	DDX_Text(pDX, IDC_EDIT14, m_dwMaximumStamina);
	DDX_Text(pDX, IDC_EDIT17, m_dwCurrentStamina);
	DDX_Text(pDX, IDC_EDIT12, m_dwMaximumLife);
	DDX_Text(pDX, IDC_EDIT15, m_dwCurrentLife);
	DDX_Text(pDX, IDC_EDIT13, m_dwMaximumMana);
	DDX_Text(pDX, IDC_EDIT16, m_dwCurrentMana);
	DDX_Text(pDX, IDC_STATIC0, m_sText[0]);
	DDX_Text(pDX, IDC_STATIC1, m_sText[1]);
	DDX_Text(pDX, IDC_STATIC2, m_sText[2]);
	DDX_Text(pDX, IDC_STATIC3, m_sText[3]);
	DDX_Text(pDX, IDC_STATIC4, m_sText[4]);
	DDX_Text(pDX, IDC_STATIC5, m_sText[5]);
	DDX_Text(pDX, IDC_STATIC6, m_sText[6]);
	DDX_Text(pDX, IDC_STATIC7, m_sText[7]);
	DDX_Text(pDX, IDC_STATIC8, m_sText[8]);
	DDX_Text(pDX, IDC_STATIC9, m_sText[9]);
	DDX_Text(pDX, IDC_STATIC10, m_sText[10]);
	DDX_Text(pDX, IDC_STATIC11, m_sText[11]);
	DDX_Text(pDX, IDC_STATIC12, m_sText[12]);
	DDX_Text(pDX, IDC_STATIC13, m_sText[13]);
	DDX_Text(pDX, IDC_STATIC14, m_sText[14]);
	DDX_Text(pDX, IDC_STATIC15, m_sText[15]);
	DDX_Text(pDX, IDC_STATIC16, m_sText[16]);
	DDX_Text(pDX, IDC_STATIC17, m_sText[17]);
	DDX_Text(pDX, IDC_STATIC18, m_sText[18]);
	DDX_Text(pDX, IDC_STATIC19, m_sText[19]);
	DDX_Text(pDX, IDC_STATIC20, m_sText[20]);
	DDX_Text(pDX, IDC_CHECK1, m_sText[21]);
	DDX_Text(pDX, IDC_CHECK2, m_sText[22]);
	DDX_Text(pDX, IDC_CHECK3, m_sText[23]);
	DDX_Text(pDX, IDC_CHECK4, m_sText[24]);
	DDX_Text(pDX, IDC_STATIC21, m_sText[25]);
	DDX_Text(pDX, IDC_STATIC22, m_sText[26]);
	DDX_Text(pDX, IDC_STATIC23, m_sText[27]);
	DDX_Control(pDX, IDC_BUTTON1, m_btnSkills);
	DDX_Text(pDX, IDC_EDIT11, m_dwSkillPointsRemaining);
	DDX_Text(pDX, IDC_EDIT18, m_dwGoldInBody);
	DDX_Text(pDX, IDC_EDIT19, m_dwGoldInStash);
	DDX_Text(pDX, IDC_STATIC_GoldinPer, m_sGoldinPer);
	DDX_Text(pDX, IDC_STATIC_GoldinSta, m_sGoldinStash);
	DDX_Control(pDX, IDC_TAB1, m_tcBasicInfo);
}


BEGIN_MESSAGE_MAP(CDlgCharBasicInfo, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgCharBasicInfo::OnBnClicked_Skills)
	ON_EN_CHANGE(IDC_EDIT4, &CDlgCharBasicInfo::OnEnChangeLevel)
	ON_WM_PAINT()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CDlgCharBasicInfo::OnTcnSelchangeTab1)
END_MESSAGE_MAP()

// 更新显示的人物信息
void CDlgCharBasicInfo::UpdateUI(const CD2S_Struct & character)
{
	switch(character.dwVersion){
		case 0x60:m_sVersion = _T("1.10");break;
		case 0x5C:m_sVersion = _T("1.09");break;
		case 0x59:m_sVersion = _T("Standard 1.08");break;
		case 0x57:m_sVersion = _T("1.07/Expansion 1.08");break;
		case 0x47:m_sVersion = _T("1.00-1.06");break;
		default:m_sVersion = ::theApp.MsgUnknown();
	}
	m_cbCharClass.SetCurSel(character.charClass);
	m_bLadder = (character.charType & 0x40) != 0;
	m_bExpansion = (character.charType & 0x20) != 0;
	m_bHardcore = (character.charType & 4) != 0;
	m_bDiedBefore = (character.charType & 8) != 0;
	m_sName = character.Name;
	if(character.charTitle == 0xF)
		m_sCharTitle = _T("Patriarch/Matriarch");
	else if(character.charTitle >= 0xA)
		m_sCharTitle = _T("Champion");
	else if(character.charTitle >= 5)
		m_sCharTitle = _T("Slayer");
	else
		m_sCharTitle.Empty();
	m_tTime = character.dwTime;
	m_cbWeaponSet.SetCurSel(character.dwWeaponSet);
	for(int i = 0;i < 3;++i)
		if((character.Town)[i]){
			m_cbLastDifficult.SetCurSel(i);
			m_cbLastACT.SetCurSel((character.Town)[i] & 7);
		}
	m_dwStrength = character.PlayerStats.m_adwValue[0];
	m_dwEnergy = character.PlayerStats.m_adwValue[1];
	m_dwDexterity = character.PlayerStats.m_adwValue[2];
	m_dwVitality = character.PlayerStats.m_adwValue[3];
	m_dwStatPointsRemaining = character.PlayerStats.m_adwValue[4];
	m_dwSkillPointsRemaining = character.PlayerStats.m_adwValue[5];
	m_dwCurrentLife = character.PlayerStats.m_adwValue[6] >> 8;
	m_dwMaximumLife = character.PlayerStats.m_adwValue[7] >> 8;
	m_dwCurrentMana = character.PlayerStats.m_adwValue[8] >> 8;
	m_dwMaximumMana = character.PlayerStats.m_adwValue[9] >> 8;
	m_dwCurrentStamina = character.PlayerStats.m_adwValue[0xA] >> 8;
	m_dwMaximumStamina = character.PlayerStats.m_adwValue[0xB] >> 8;
	m_uCharLevel = BYTE(character.PlayerStats.m_adwValue[0xC]);
	m_dwExperience = character.PlayerStats.m_adwValue[0xD];
	m_dwGoldInBody = character.PlayerStats.m_adwValue[0xE];
	m_dwGoldInStash = character.PlayerStats.m_adwValue[0xF];
	m_cbLevelAndExp.SetCurSel(m_uCharLevel);

	::CopyMemory(m_bSkills,character.Skills.bSkillLevel,sizeof(m_bSkills));
	//更新最大Gold数量
	m_dwMaxGoldInBody = m_uCharLevel * 10000;
	m_sGoldinPer.Format(_T("0-%u"),m_dwMaxGoldInBody);
	m_dwMaxGoldInStash = (m_uCharLevel < 31 ? (m_uCharLevel / 10 + 1) * 50000 : (m_uCharLevel / 2 + 1) * 50000);
	m_sGoldinStash.Format(_T("0-%u"),m_dwMaxGoldInStash);
	
	for(int i = 0;i < m_nTabPageCount;++i)
		m_dlgTabPage[i]->UpdateUI(character);

	UpdateData(FALSE);
}

BOOL CDlgCharBasicInfo::GatherData(CD2S_Struct & character)
{
	UpdateData(TRUE);
	if(!::SetCharName(character.Name, m_sName)){
		::MessageBox(0, ::theApp.MsgBoxInfo(0), ::theApp.MsgError(), MB_ICONERROR);
		return FALSE;
	}
	if(m_uCharLevel < 1 || m_uCharLevel > 127){
		::MessageBox(0, ::theApp.MsgBoxInfo(1),::theApp.MsgError(),MB_ICONERROR);
		return FALSE;
	}
	if(m_dwStrength > 1023 ||
		m_dwEnergy > 1023 ||
		m_dwDexterity > 1023 ||
		m_dwVitality > 1023 ||
		m_dwStatPointsRemaining > 1023 ||
		m_dwSkillPointsRemaining > 255 ||
		m_dwCurrentLife > 8191 ||
		m_dwMaximumLife > 8191 ||
		m_dwCurrentMana > 8191 ||
		m_dwMaximumMana > 8191 ||
		m_dwCurrentStamina > 8191 ||
		m_dwMaximumStamina > 8191){
			::MessageBox(0, ::theApp.MsgBoxInfo(2), ::theApp.MsgError(), MB_ICONERROR);
			return FALSE;
	}
	if(m_dwGoldInBody > m_dwMaxGoldInBody || m_dwGoldInStash > m_dwMaxGoldInStash){
		::MessageBox(0, ::theApp.MsgBoxInfo(3),::theApp.MsgError(),MB_ICONERROR);
		return FALSE;
	}
	character.charClass = m_cbCharClass.GetCurSel();
	m_bLadder ? character.charType |= 0x40 : character.charType &= ~0x40;
	m_bExpansion ? character.charType |= 0x20 : character.charType &= ~0x20;
	m_bHardcore ? character.charType |= 4 : character.charType &= ~4;
	m_bDiedBefore ? character.charType |= 8 : character.charType &= ~8;
	//character.charTitle
	character.dwTime = DWORD(m_tTime.GetTime());
	character.dwWeaponSet = m_cbWeaponSet.GetCurSel();
	::ZeroMemory(character.Town,sizeof(character.Town));
	(character.Town)[m_cbLastDifficult.GetCurSel()] = 0x80 + m_cbLastACT.GetCurSel();
	character.PlayerStats.m_adwValue[0] = m_dwStrength;
	character.PlayerStats.m_adwValue[1] = m_dwEnergy;
	character.PlayerStats.m_adwValue[2] = m_dwDexterity;
	character.PlayerStats.m_adwValue[3] = m_dwVitality;
	character.PlayerStats.m_adwValue[4] = m_dwStatPointsRemaining;
	character.PlayerStats.m_adwValue[5] = m_dwSkillPointsRemaining;
	character.PlayerStats.m_adwValue[6] = m_dwCurrentLife << 8;
	character.PlayerStats.m_adwValue[7] = m_dwMaximumLife << 8;
	character.PlayerStats.m_adwValue[8] = m_dwCurrentMana << 8;
	character.PlayerStats.m_adwValue[9] = m_dwMaximumMana << 8;
	character.PlayerStats.m_adwValue[0xA] = m_dwCurrentStamina << 8;
	character.PlayerStats.m_adwValue[0xB] = m_dwMaximumStamina << 8;
	character.PlayerStats.m_adwValue[0xC] = character.charLevel = m_uCharLevel;
	character.PlayerStats.m_adwValue[0xD] = m_dwExperience;
	character.PlayerStats.m_adwValue[0xE] = m_dwGoldInBody;
	character.PlayerStats.m_adwValue[0xF] = m_dwGoldInStash;
	::CopyMemory(character.Skills.bSkillLevel,m_bSkills,sizeof(m_bSkills));
    //其他窗体的数据
	for(int i = 0;i < m_nTabPageCount;++i)
		if(!m_dlgTabPage[i]->GatherData(character))
			return FALSE;
	return TRUE;
}

void CDlgCharBasicInfo::ResetAll()
{
	//CString
	m_sVersion = m_sName = m_sCharTitle = _T("");
	//CComboBox
	m_cbCharClass.SetCurSel(0);
	m_cbWeaponSet.SetCurSel(0);
	m_cbLastDifficult.SetCurSel(0);
	m_cbLastACT.SetCurSel(0);
	m_cbLevelAndExp.SetCurSel(0);
	//BOOL
	m_bLadder = m_bExpansion = m_bHardcore = m_bDiedBefore = FALSE;
	//UINT & DWORD
	m_uCharLevel = m_dwExperience = m_dwStrength = m_dwDexterity = m_dwEnergy = m_dwVitality = m_dwStatPointsRemaining = 
		m_dwMaximumStamina = m_dwCurrentStamina = m_dwMaximumLife = m_dwCurrentLife = m_dwMaximumMana = m_dwCurrentMana = 0;
	//CTime
	m_tTime = CTime::GetCurrentTime();
	//Array
	::ZeroMemory(m_bSkills,sizeof(m_bSkills));
	//子窗体
	for(int i = 0;i < m_nTabPageCount;++i)
		m_dlgTabPage[i]->ResetAll();

	UpdateData(FALSE);
}

void CDlgCharBasicInfo::InitUI(void)
{
	m_cbWeaponSet.AddString(_T("I"));
	m_cbWeaponSet.AddString(_T("II"));

	m_cbLastACT.AddString(_T("I"));
	m_cbLastACT.AddString(_T("II"));
	m_cbLastACT.AddString(_T("III"));
	m_cbLastACT.AddString(_T("IV"));
	m_cbLastACT.AddString(_T("V"));
	{
		CString lev,exp;
		for(int i = 0;i < 99;++i){
			lev.Format(_T("%d"),i + 1);
			exp.Format(_T("%u"),LEVEL_AND_EXPERIENCE[i]);
			CString spa(_T(' '),14 - lev.GetLength() - exp.GetLength());
			m_cbLevelAndExp.AddString(lev + spa + exp);
		}
	}
	if(!m_dlgTabPage){
		m_tcBasicInfo.InsertItem(0, _T(""));
		m_tcBasicInfo.InsertItem(1, _T(""));
		//在此处添加新的属性页,并在LoadText里更改界面文字的重载入
		m_nTabPageCount = m_tcBasicInfo.GetItemCount();

		m_dlgTabPage = new CCharacterDialogBase*[m_nTabPageCount];
		m_dlgTabPage[0] = new CDlgWayPoints;
		m_dlgTabPage[0]->Create(CDlgWayPoints::IDD, &m_tcBasicInfo);
		m_dlgTabPage[0]->ShowWindow(SW_HIDE);
		m_dlgTabPage[1] = new CDlgQuestInfo;
		m_dlgTabPage[1]->Create(CDlgQuestInfo::IDD, &m_tcBasicInfo);
		m_dlgTabPage[1]->ShowWindow(SW_HIDE);
		//在此处添加新的属性窗体

		m_nTabCurSel = 0;
		m_tcBasicInfo.SetCurSel(m_nTabCurSel);
		m_dlgTabPage[m_nTabCurSel]->ShowWindow(SW_SHOW);
	}
}

void CDlgCharBasicInfo::LoadText(void)
{
    TCITEM tci;
    tci.mask = TCIF_TEXT;
    //小站,任务
    tci.pszText = (LPWSTR)::theApp.CharBasicInfoUI(1).GetString();
    m_tcBasicInfo.SetItem(0,&tci);
    tci.pszText = (LPWSTR)::theApp.CharBasicInfoUI(2).GetString();
    m_tcBasicInfo.SetItem(1,&tci);
	m_btnSkills.SetWindowText(::theApp.CharBasicInfoUI(3));

    int sel = m_cbCharClass.GetCurSel();
    m_cbCharClass.ResetContent();
    for(UINT i = 0;i < ::theApp.CLASS_NAME_SIZE;++i)
        m_cbCharClass.AddString(::theApp.ClassName(i));
    m_cbCharClass.SetCurSel(sel);

    sel = m_cbLastDifficult.GetCurSel();
    m_cbLastDifficult.ResetContent();
	for(UINT i = 0;i < ::theApp.DifficultyNameSize();++i)
		m_cbLastDifficult.AddString(::theApp.DifficultyName(i));
    m_cbLastDifficult.SetCurSel(sel);

	for(int i = 0;i < 28;++i)
		m_sText[i] = ::theApp.CharBasicInfoUI(4 + i);

	for(int i = 0;i < m_nTabPageCount;++i)
		m_dlgTabPage[i]->LoadText();

	UpdateData(FALSE);
}

// CDlgCharBasicInfo 消息处理程序

BOOL CDlgCharBasicInfo::OnInitDialog()
{
	CCharacterDialogBase::OnInitDialog();
	InitUI();
	LoadText();
	return TRUE;
}

void CDlgCharBasicInfo::OnBnClicked_Skills()
{
	int charClass = m_cbCharClass.GetCurSel();
	if(charClass >= 0 && charClass < 7){
		CDlgSkills dlgSkill(charClass,m_bSkills);
		dlgSkill.DoModal();
	}else
		::MessageBox(0, ::theApp.MsgBoxInfo(4),::theApp.MsgError(),MB_ICONERROR);
}

void CDlgCharBasicInfo::OnEnChangeLevel()
{
	UpdateData(TRUE);
	if(m_uCharLevel && m_uCharLevel < 128){
		m_dwMaxGoldInBody = m_uCharLevel * 10000;
		m_sGoldinPer.Format(_T("0-%u"),m_dwMaxGoldInBody);
		m_dwMaxGoldInStash = (m_uCharLevel < 31 ? (m_uCharLevel / 10 + 1) * 50000 : (m_uCharLevel / 2 + 1) * 50000);
		m_sGoldinStash.Format(_T("0-%u"),m_dwMaxGoldInStash);
		UpdateData(FALSE);
	}
}

void CDlgCharBasicInfo::OnPaint()
{
	CCharacterDialogBase::OnPaint();
	CRect rect;
	GetClientRect(&rect);
	rect.top += 235;
	m_tcBasicInfo.MoveWindow(rect);
	m_tcBasicInfo.GetClientRect(&rect);
	rect.top += 20;
	if(m_dlgTabPage)
		for(int i = 0;i < m_nTabPageCount;++i)
			m_dlgTabPage[i]->MoveWindow(rect);
}

void CDlgCharBasicInfo::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_dlgTabPage[m_nTabCurSel]->ShowWindow(SW_HIDE);
	m_nTabCurSel = m_tcBasicInfo.GetCurSel();
	m_dlgTabPage[m_nTabCurSel]->ShowWindow(SW_SHOW);
	*pResult = 0;
}
