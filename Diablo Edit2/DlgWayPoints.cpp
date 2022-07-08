// DlgWayPoints.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgWayPoints.h"


// CDlgWayPoints 对话框

IMPLEMENT_DYNAMIC(CDlgWayPoints, CCharacterDialogBase)

CDlgWayPoints::CDlgWayPoints(CWnd* pParent /*=NULL*/)
	: CCharacterDialogBase(CDlgWayPoints::IDD, pParent)
	, m_nLevel(0)
{
	::ZeroMemory(m_bUIData,sizeof(m_bUIData));
	::ZeroMemory(m_byteOriginData[0],sizeof(m_byteOriginData[0]));
	::ZeroMemory(m_byteOriginData[1],sizeof(m_byteOriginData[0]));
	::ZeroMemory(m_byteOriginData[2],sizeof(m_byteOriginData[0]));
}

CDlgWayPoints::~CDlgWayPoints()
{
}

void CDlgWayPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK0, m_bUIData[0]);
	DDX_Check(pDX, IDC_CHECK1, m_bUIData[1]);
	DDX_Check(pDX, IDC_CHECK2, m_bUIData[2]);
	DDX_Check(pDX, IDC_CHECK3, m_bUIData[3]);
	DDX_Check(pDX, IDC_CHECK4, m_bUIData[4]);
	DDX_Check(pDX, IDC_CHECK5, m_bUIData[5]);
	DDX_Check(pDX, IDC_CHECK6, m_bUIData[6]);
	DDX_Check(pDX, IDC_CHECK7, m_bUIData[7]);
	DDX_Check(pDX, IDC_CHECK8, m_bUIData[8]);
	DDX_Check(pDX, IDC_CHECK9, m_bUIData[9]);
	DDX_Check(pDX, IDC_CHECK10, m_bUIData[10]);
	DDX_Check(pDX, IDC_CHECK11, m_bUIData[11]);
	DDX_Check(pDX, IDC_CHECK12, m_bUIData[12]);
	DDX_Check(pDX, IDC_CHECK13, m_bUIData[13]);
	DDX_Check(pDX, IDC_CHECK14, m_bUIData[14]);
	DDX_Check(pDX, IDC_CHECK15, m_bUIData[15]);
	DDX_Check(pDX, IDC_CHECK16, m_bUIData[16]);
	DDX_Check(pDX, IDC_CHECK17, m_bUIData[17]);
	DDX_Check(pDX, IDC_CHECK18, m_bUIData[18]);
	DDX_Check(pDX, IDC_CHECK19, m_bUIData[19]);
	DDX_Check(pDX, IDC_CHECK20, m_bUIData[20]);
	DDX_Check(pDX, IDC_CHECK21, m_bUIData[21]);
	DDX_Check(pDX, IDC_CHECK22, m_bUIData[22]);
	DDX_Check(pDX, IDC_CHECK23, m_bUIData[23]);
	DDX_Check(pDX, IDC_CHECK24, m_bUIData[24]);
	DDX_Check(pDX, IDC_CHECK25, m_bUIData[25]);
	DDX_Check(pDX, IDC_CHECK26, m_bUIData[26]);
	DDX_Check(pDX, IDC_CHECK27, m_bUIData[27]);
	DDX_Check(pDX, IDC_CHECK28, m_bUIData[28]);
	DDX_Check(pDX, IDC_CHECK29, m_bUIData[29]);
	DDX_Check(pDX, IDC_CHECK30, m_bUIData[30]);
	DDX_Check(pDX, IDC_CHECK31, m_bUIData[31]);
	DDX_Check(pDX, IDC_CHECK32, m_bUIData[32]);
	DDX_Check(pDX, IDC_CHECK33, m_bUIData[33]);
	DDX_Check(pDX, IDC_CHECK34, m_bUIData[34]);
	DDX_Check(pDX, IDC_CHECK35, m_bUIData[35]);
	DDX_Check(pDX, IDC_CHECK36, m_bUIData[36]);
	DDX_Check(pDX, IDC_CHECK37, m_bUIData[37]);
	DDX_Check(pDX, IDC_CHECK38, m_bUIData[38]);
	DDX_Check(pDX, IDC_CHECK39, m_bUIData[39]);
	DDX_Radio(pDX, IDC_RADIO1, m_nLevel);
	DDX_Control(pDX, IDC_CHECK0, m_nSelAll);
	DDX_Text(pDX, IDC_CHECK0, m_sText[0]);
	DDX_Text(pDX, IDC_CHECK1, m_sText[1]);
	DDX_Text(pDX, IDC_CHECK2, m_sText[2]);
	DDX_Text(pDX, IDC_CHECK3, m_sText[3]);
	DDX_Text(pDX, IDC_CHECK4, m_sText[4]);
	DDX_Text(pDX, IDC_CHECK5, m_sText[5]);
	DDX_Text(pDX, IDC_CHECK6, m_sText[6]);
	DDX_Text(pDX, IDC_CHECK7, m_sText[7]);
	DDX_Text(pDX, IDC_CHECK8, m_sText[8]);
	DDX_Text(pDX, IDC_CHECK9, m_sText[9]);
	DDX_Text(pDX, IDC_CHECK10, m_sText[10]);
	DDX_Text(pDX, IDC_CHECK11, m_sText[11]);
	DDX_Text(pDX, IDC_CHECK12, m_sText[12]);
	DDX_Text(pDX, IDC_CHECK13, m_sText[13]);
	DDX_Text(pDX, IDC_CHECK14, m_sText[14]);
	DDX_Text(pDX, IDC_CHECK15, m_sText[15]);
	DDX_Text(pDX, IDC_CHECK16, m_sText[16]);
	DDX_Text(pDX, IDC_CHECK17, m_sText[17]);
	DDX_Text(pDX, IDC_CHECK18, m_sText[18]);
	DDX_Text(pDX, IDC_CHECK19, m_sText[19]);
	DDX_Text(pDX, IDC_CHECK20, m_sText[20]);
	DDX_Text(pDX, IDC_CHECK21, m_sText[21]);
	DDX_Text(pDX, IDC_CHECK22, m_sText[22]);
	DDX_Text(pDX, IDC_CHECK23, m_sText[23]);
	DDX_Text(pDX, IDC_CHECK24, m_sText[24]);
	DDX_Text(pDX, IDC_CHECK25, m_sText[25]);
	DDX_Text(pDX, IDC_CHECK26, m_sText[26]);
	DDX_Text(pDX, IDC_CHECK27, m_sText[27]);
	DDX_Text(pDX, IDC_CHECK28, m_sText[28]);
	DDX_Text(pDX, IDC_CHECK29, m_sText[29]);
	DDX_Text(pDX, IDC_CHECK30, m_sText[30]);
	DDX_Text(pDX, IDC_CHECK31, m_sText[31]);
	DDX_Text(pDX, IDC_CHECK32, m_sText[32]);
	DDX_Text(pDX, IDC_CHECK33, m_sText[33]);
	DDX_Text(pDX, IDC_CHECK34, m_sText[34]);
	DDX_Text(pDX, IDC_CHECK35, m_sText[35]);
	DDX_Text(pDX, IDC_CHECK36, m_sText[36]);
	DDX_Text(pDX, IDC_CHECK37, m_sText[37]);
	DDX_Text(pDX, IDC_CHECK38, m_sText[38]);
	DDX_Text(pDX, IDC_CHECK39, m_sText[39]);
	DDX_Text(pDX, IDC_RADIO1, m_sText[40]);
	DDX_Text(pDX, IDC_RADIO2, m_sText[41]);
	DDX_Text(pDX, IDC_RADIO3, m_sText[42]);

}


BEGIN_MESSAGE_MAP(CDlgWayPoints, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgWayPoints::OnBnClickedNormal)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgWayPoints::OnBnClickedNightmare)
	ON_BN_CLICKED(IDC_RADIO3, &CDlgWayPoints::OnBnClickedHell)
	ON_BN_CLICKED(IDC_CHECK0, &CDlgWayPoints::OnBnClicked_SelAll)
END_MESSAGE_MAP()

void CDlgWayPoints::UpdateUI(const CD2S_Struct & character)
{
	for(int i = 0;i < 3;++i){
		::CopyMemory(m_byteOriginData[i],character.Waypoints.wp[i].Waypoints,sizeof(m_byteOriginData[0]));
		GetOriginData(i,m_bWayData[i]);
	}
	::CopyMemory(m_bUIData,m_bWayData[m_nLevel],sizeof(m_bUIData));
	UpdateData(FALSE);
}

BOOL CDlgWayPoints::GatherData(CD2S_Struct & character)
{
	UpdateData(TRUE);
	::CopyMemory(m_bWayData[m_nLevel],m_bUIData,sizeof(m_bUIData));
	for(int i = 0;i < 3;++i){
		::ZeroMemory(character.Waypoints.wp[i].Waypoints,sizeof(character.Waypoints.wp[i].Waypoints));
		for(int j = 1,mask = 1,index = 0;j < 40;++j){
			(character.Waypoints.wp[i].Waypoints)[index] |= (m_bWayData[i][j] ? mask : 0);
			if(mask == 0x80){
				mask = 1;
				++index;
			}else
				mask <<= 1;
		}
		//处理CQuestInfoData::wTraval#
		character.QuestInfo.QIData[i].wTraval1 = m_bWayData[i][10] ? 1 : 0;
		character.QuestInfo.QIData[i].wTraval2 = m_bWayData[i][19] ? 1 : 0;
		character.QuestInfo.QIData[i].wTraval3 = m_bWayData[i][28] ? 1 : 0;
		character.QuestInfo.QIData[i].wTraval4 = m_bWayData[i][31] ? 1 : 0;
	}
	return TRUE;
}

void CDlgWayPoints::ResetAll(){
	for(int i = 0;i < 3;++i){
		::ZeroMemory(m_byteOriginData[i],sizeof(m_byteOriginData[i]));
		::ZeroMemory(m_bWayData[i],sizeof(m_bWayData[i]));
	}
	::ZeroMemory(m_bUIData,sizeof(m_bUIData));
	m_nLevel = 0;
	UpdateData(FALSE);
}

void CDlgWayPoints::GetOriginData(int level,BOOL * way)
{
	ASSERT(level >= 0 && level < 3 && _T("CDlgWayPoints::GetOriginData(int level)"));
	way[0] = BST_INDETERMINATE;
	for(int j = 1,mask = 1,index = 0;j < 40;++j){
		way[j] = (m_byteOriginData[level][index] & mask) != 0;
		if(mask == 0x80){
			mask = 1;
			++index;
		}else
			mask <<= 1;
	}
}

void CDlgWayPoints::LoadText(void)
{
	m_sText[0] = ::theApp.OtherUI(8);
	for(int i = 1;i < 40;++i)
		m_sText[i] = ::theApp.WayPointName(i - 1);
	for (UINT i = 0; i < ::theApp.DifficultyNameSize(); ++i)
		m_sText[40 + i] = ::theApp.DifficultyName(i);
	UpdateData(FALSE);
}

// CDlgWayPoints 消息处理程序

void CDlgWayPoints::OnBnClickedNormal()
{
	if(m_nLevel != 0){
        INT old = m_nLevel;
		UpdateData();
		::CopyMemory(m_bWayData[old],m_bUIData,sizeof(m_bUIData));
		m_nLevel = 0;
		::CopyMemory(m_bUIData,m_bWayData[0],sizeof(m_bUIData));
		UpdateData(FALSE);
	}
}

void CDlgWayPoints::OnBnClickedNightmare()
{
	if(m_nLevel != 1){
        INT old = m_nLevel;
		UpdateData();
		::CopyMemory(m_bWayData[old],m_bUIData,sizeof(m_bUIData));
		m_nLevel = 1;
		::CopyMemory(m_bUIData,m_bWayData[1],sizeof(m_bUIData));
		UpdateData(FALSE);
	}
}

void CDlgWayPoints::OnBnClickedHell()
{
	if(m_nLevel != 2){
        INT old = m_nLevel;
		UpdateData();
		::CopyMemory(m_bWayData[old],m_bUIData,sizeof(m_bUIData));
		m_nLevel = 2;
		::CopyMemory(m_bUIData,m_bWayData[2],sizeof(m_bUIData));
		UpdateData(FALSE);
	}
}

void CDlgWayPoints::OnBnClicked_SelAll()
{
	switch(m_nSelAll.GetCheck()){
		case BST_UNCHECKED:
			::ZeroMemory(m_bUIData,sizeof(m_bUIData));
			break;
		case BST_CHECKED:;
			for(int i = 0;i < 40;++i)
				m_bUIData[i] = TRUE;
			break;
		default:
			GetOriginData(m_nLevel,m_bUIData);
			break;
	}
	UpdateData(FALSE);
}
