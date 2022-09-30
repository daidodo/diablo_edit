// CDlgFoundry.cpp: 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgFoundry.h"
#include "afxdialogex.h"

using namespace std;

const WORD NO_CODE = -1;

static CString stringOf(const CEdit & edit) {
	CString s;
	edit.GetWindowText(s);
	return s;
}

static int intOf(const CString & str) { return _tstoi(str); }

int intOf(const CEdit & edit) { return intOf(stringOf(edit)); }

class __EnableFlag {
	BOOL & f_;
	BOOL old_;
public:
	explicit __EnableFlag(BOOL & f) :f_(f), old_(f) { f_ = TRUE; }
	~__EnableFlag() { f_ = old_; }
};

// CDlgFoundry 对话框

IMPLEMENT_DYNAMIC(CDlgFoundry, CDialogEx)

CDlgFoundry::CDlgFoundry(CD2Item & item, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_Foundry, pParent)
	, m_stItem(item)
	, m_bUnidentified(FALSE), m_bEthereal(FALSE), m_sExtSockets(_T(""))
{}

CDlgFoundry::~CDlgFoundry() {}

void CDlgFoundry::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_LEVEL, m_sText[0]);
	DDX_Text(pDX, IDC_STATIC_OWNER, m_sText[1]);
	DDX_Text(pDX, IDC_STATIC_CLASS, m_sText[2]);
	DDX_Text(pDX, IDC_CHECK_UNIDENTIFIED, m_sText[3]);
	DDX_Text(pDX, IDC_CHECK_ETHEREAL, m_sText[4]);
	DDX_Text(pDX, IDC_STATIC_DURABILITY, m_sText[5]);
	DDX_Text(pDX, IDC_CHECK_INDESTRUCTIBLE, m_sText[6]);
	DDX_Text(pDX, IDC_STATIC_QUANTITY, m_sText[7]);
	DDX_Text(pDX, IDC_STATIC_QUALITY, m_sText[8]);
	DDX_Text(pDX, IDC_STATIC_Sockets, m_sText[9]);
	DDX_Text(pDX, IDC_STATIC_DEFENCE, m_sText[10]);
	DDX_Text(pDX, IDC_STATIC_MONSTER, m_sText[11]);
	DDX_Text(pDX, IDC_STATIC_FIRST_NAME, m_sText[12]);
	DDX_Text(pDX, IDC_STATIC_SECOND_NAME, m_sText[13]);
	DDX_Text(pDX, IDC_STATIC_SUBTYPE, m_sText[14]);
	DDX_Text(pDX, IDC_STATIC_PREFIX1, m_sText[15]);
	DDX_Text(pDX, IDC_STATIC_SUFFIX1, m_sText[16]);
	DDX_Text(pDX, IDC_STATIC_SET_NAME, m_sText[17]);
	DDX_Text(pDX, IDC_STATIC_UNIQUE_NAME, m_sText[18]);
	DDX_Text(pDX, IDC_CHECK_RUNEWORD, m_sText[19]);
	DDX_Text(pDX, IDOK, m_sText[20]);
	DDX_Text(pDX, IDCANCEL, m_sText[21]);
	DDX_Control(pDX, IDC_COMBO_CLASS, m_cbClass);
	DDX_Control(pDX, IDC_COMBO_QUALITY, m_cbQuality);
	DDX_Control(pDX, IDC_COMBO_MONSTER, m_cbMonster);
	DDX_Control(pDX, IDC_COMBO_FIRST_NAME, m_cbRareCraftedName[0]);
	DDX_Control(pDX, IDC_COMBO_SECOND_NAME, m_cbRareCraftedName[1]);
	DDX_Control(pDX, IDC_COMBO_SUBTYPE, m_cbSubType);
	DDX_Control(pDX, IDC_COMBO_PREFX1, m_cbPrefix[0]);
	DDX_Control(pDX, IDC_COMBO_PREFIX2, m_cbPrefix[1]);
	DDX_Control(pDX, IDC_COMBO_PREFIX3, m_cbPrefix[2]);
	DDX_Control(pDX, IDC_COMBO_SUFFIX1, m_cbSuffix[0]);
	DDX_Control(pDX, IDC_COMBO_SUFFIX2, m_cbSuffix[1]);
	DDX_Control(pDX, IDC_COMBO_SUFFIX3, m_cbSuffix[2]);
	DDX_Control(pDX, IDC_COMBO_SET_NAME, m_cbSetName);
	DDX_Control(pDX, IDC_COMBO_UNIQUE_NAME, m_cbUniqueName);
	DDX_Control(pDX, IDC_EDIT_LEVEL, m_edLevel);
	DDX_Check(pDX, IDC_CHECK_UNIDENTIFIED, m_bUnidentified);
	DDX_Check(pDX, IDC_CHECK_ETHEREAL, m_bEthereal);
	DDX_Control(pDX, IDC_CHECK_INDESTRUCTIBLE, m_chIndestructible);
	DDX_Control(pDX, IDC_EDIT_DURABILITY_MAX, m_edMaxDurability);
	DDX_Control(pDX, IDC_EDIT_DURABILITY, m_edDurability);
	DDX_Control(pDX, IDC_EDIT_QUANTITY, m_edQuantity);
	DDX_Control(pDX, IDC_EDIT_OWNER, m_edOwner);
	DDX_Control(pDX, IDC_EDIT_BASE_SOCKETS, m_edSockets);
	DDX_Text(pDX, IDC_STATIC_SOCKETS_EXT, m_sExtSockets);
	DDX_Control(pDX, IDC_EDIT_DEFENCE, m_edDefence);
	DDX_Control(pDX, IDC_TAB_PROPERTY, m_tabPropList);
	DDX_Control(pDX, IDC_LIST1, m_lstProperty);
	DDX_Control(pDX, IDC_EDIT1, m_edParam[0]);
	DDX_Control(pDX, IDC_EDIT2, m_edParam[1]);
	DDX_Control(pDX, IDC_EDIT3, m_edParam[2]);
	DDX_Control(pDX, IDC_EDIT4, m_edParam[3]);
	DDX_Control(pDX, IDC_COMBO_PARAM1, m_cbParam[0]);
	DDX_Control(pDX, IDC_COMBO_PARAM2, m_cbParam[1]);
	DDX_Text(pDX, IDC_STATIC_TIP1, m_sTip[0]);
	DDX_Text(pDX, IDC_STATIC_TIP2, m_sTip[1]);
	DDX_Text(pDX, IDC_STATIC_TIP3, m_sTip[2]);
	DDX_Text(pDX, IDC_STATIC_TIP4, m_sTip[3]);
	DDX_Control(pDX, IDC_CHECK_RUNEWORD, m_chRuneWord);
}

BEGIN_MESSAGE_MAP(CDlgFoundry, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_INDESTRUCTIBLE, &CDlgFoundry::OnBnClickedCheckIndestructible)
	ON_CBN_SELCHANGE(IDC_COMBO_QUALITY, &CDlgFoundry::OnCbnSelchangeComboQuality)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PROPERTY, &CDlgFoundry::OnTcnSelchangeTabProperty)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CDlgFoundry::OnLvnItemchangedList1)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgFoundry::OnEnChangeParam)
	ON_EN_CHANGE(IDC_EDIT2, &CDlgFoundry::OnEnChangeParam)
	ON_EN_CHANGE(IDC_EDIT3, &CDlgFoundry::OnEnChangeParam)
	ON_EN_CHANGE(IDC_EDIT4, &CDlgFoundry::OnEnChangeParam)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAM1, &CDlgFoundry::OnEnChangeParam)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAM2, &CDlgFoundry::OnEnChangeParam)
	ON_BN_CLICKED(IDC_CHECK_RUNEWORD, &CDlgFoundry::OnBnClickedCheckRuneword)
	ON_EN_CHANGE(IDC_EDIT_BASE_SOCKETS, &CDlgFoundry::OnEnChangeEditBaseSockets)
END_MESSAGE_MAP()


// CDlgFoundry 消息处理程序

void CDlgFoundry::InitUI() {
	//UI text
	int idx = 0;
	for (int i = 0; i < int(size(m_sText)); ++i)
		m_sText[i] = ::theApp.FoundryUI(idx++);
	for (int i = 0; i < int(size(m_aProps)); ++i)
		m_tabPropList.InsertItem(i, ::theApp.FoundryUI(idx++));
	//UI combox
	const auto & meta = m_stItem.MetaData();
	if (m_stItem.bEar) {
		for (UINT i = 0; i < ::theApp.CLASS_NAME_SIZE; ++i)
			m_cbClass.AddString(::theApp.ClassName(i));
	}
	if (!m_stItem.bSimple) {
		for (UINT i = 0, e = (meta.White ? 3 : ::theApp.ItemQualityNameSize()); i < e; ++i)
			m_cbQuality.AddString(::theApp.ItemQualityName(i));
		if (meta.HasMonsterID)
			for (UINT i = 0; i < ::theApp.MonsterNameSize(); ++i)
				m_cbMonster.AddString(::theApp.MonsterName(i));
		WCHAR tmp[] = _T("0");
		for (; tmp[0] < _T('8'); ++tmp[0])
			m_cbSubType.AddString(tmp);
		if (!meta.Normal && !meta.White) {
			for (UINT i = 0; i < ::theApp.RareCraftedNameSize(); ++i) {
				const auto name = ::theApp.RareCraftedName(i);
				m_cbRareCraftedName[0].AddString(name);
				m_cbRareCraftedName[1].AddString(name);
			}
			for (UINT i = 0; i < ::theApp.MagicPrefixSize(); ++i) {
				const auto str = ::theApp.MagicPrefix(i);
				for (auto & cb : m_cbPrefix)
					cb.AddString(str);
			}
			for (UINT i = 0; i < ::theApp.MagicSuffixSize(); ++i) {
				const auto str = ::theApp.MagicSuffix(i);
				for (auto & cb : m_cbSuffix)
					cb.AddString(str);
			}
			for (UINT i = 0; i < ::theApp.SetItemNameSize(); ++i)
				m_cbSetName.AddString(::theApp.SetItemName(i));
			for (UINT i = 0; i < ::theApp.UniqueNameSize(); ++i)
				m_cbUniqueName.AddString(::theApp.UniqueName(i));
		}
	}
	//Property list
	m_lstProperty.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);
	m_lstProperty.InsertColumn(0, ::theApp.FoundryUI(idx++), LVCFMT_LEFT, 60);
	m_lstProperty.InsertColumn(1, ::theApp.FoundryUI(idx++), LVCFMT_LEFT, 300);
	for (UINT i = 0; i < ::theApp.PropertyNameSize(); ++i) {
		m_lstProperty.InsertItem(i, CSFormat(_T("%3d"), i));				//属性ID
		m_lstProperty.SetItemText(i, 1, ::theApp.PropertyDescription(m_stItem.dwVersion, i));	//属性描述
	}
}

void CDlgFoundry::ReadItemEar(const CEar & ear) {
	m_edLevel.EnableWindow(TRUE);
	m_edLevel.SetWindowText(CSFormat(_T("%d"), ear.iEarLevel));
	m_edOwner.EnableWindow(TRUE);
	m_edOwner.SetWindowText(CString(ear.sEarName));
	m_cbClass.EnableWindow(TRUE);
	m_cbClass.SetCurSel(ear.iEarClass);
}

void CDlgFoundry::ReadItemGold(const CGoldQuantity & gold) {
	m_edQuantity.EnableWindow(TRUE);
	m_edQuantity.SetWindowText(CSFormat(_T("%d"), int(gold.wQuantity)));
}

void CDlgFoundry::ReadItemLongName(const CLongName & longName) {
	m_cbRareCraftedName[0].SetCurSel(longName.iName1);
	m_cbRareCraftedName[1].SetCurSel(longName.iName2);
	m_cbPrefix[0].SetCurSel(longName.wPref1.exist() ? longName.wPref1 : 0);
	m_cbPrefix[1].SetCurSel(longName.wPref2.exist() ? longName.wPref2 : 0);
	m_cbPrefix[2].SetCurSel(longName.wPref3.exist() ? longName.wPref3 : 0);
	m_cbSuffix[0].SetCurSel(longName.wSuff1.exist() ? longName.wSuff1 : 0);
	m_cbSuffix[1].SetCurSel(longName.wSuff2.exist() ? longName.wSuff2 : 0);
	m_cbSuffix[2].SetCurSel(longName.wSuff3.exist() ? longName.wSuff3 : 0);
}

void CDlgFoundry::ReadExtItemInfo(const CExtItemInfo & extInfo, BOOL bNormal) {
	m_edLevel.EnableWindow(TRUE);
	m_edLevel.SetWindowText(CSFormat(_T("%d"), int(extInfo.iDropLevel)));
	ASSERT(0 < extInfo.iQuality);
	if (bNormal) {	//item can only be normal
		m_cbQuality.SetCurSel(1);
	} else {
		m_cbQuality.EnableWindow(TRUE);
		m_cbQuality.SetCurSel(extInfo.iQuality - 1);
		OnCbnSelchangeComboQuality();
		switch (extInfo.iQuality) {
			case 1:		//low
				ASSERT(extInfo.loQual.exist());
				m_cbSubType.SetCurSel(extInfo.loQual);
				break;
			case 3:		//high
				ASSERT(extInfo.hiQual.exist());
				m_cbSubType.SetCurSel(extInfo.hiQual);
				break;
			case 4:		//magical
				ASSERT(extInfo.wPrefix.exist());
				ASSERT(extInfo.wSuffix.exist());
				m_cbPrefix[0].SetCurSel(extInfo.wPrefix);
				m_cbSuffix[0].SetCurSel(extInfo.wSuffix);
				break;
			case 5:		//set
				ASSERT(extInfo.wSetID.exist());
				m_cbSetName.SetCurSel(extInfo.wSetID);
				break;
			case 6:		//rare
				ASSERT(extInfo.pRareName.exist());
				ReadItemLongName(*extInfo.pRareName);
				break;
			case 7:		//unique
				ASSERT(extInfo.wUniID.exist());
				m_cbUniqueName.SetCurSel(extInfo.wUniID);
				break;
			case 8:		//crafted
				ASSERT(extInfo.pCraftName.exist());
				ReadItemLongName(*extInfo.pCraftName);
				break;
			default:;	//normal
		}
	}
	m_edOwner.EnableWindow(TRUE);
	if (extInfo.sPersonName.exist())
		m_edOwner.SetWindowText(CString(extInfo.sPersonName));
	if (extInfo.wMonsterID.exist()) {
		m_cbMonster.EnableWindow(TRUE);
		m_cbMonster.SetCurSel(extInfo.wMonsterID);
	}
}

void CDlgFoundry::ReadTypeSpecificInfo(CTypeSpecificInfo & spec, BOOL bRuneWord) {
	if (spec.iDefence.exist()) {
		m_edDefence.EnableWindow(TRUE);
		m_edDefence.SetWindowText(CSFormat(_T("%d"), spec.GetDefence()));
	}
	if (spec.iMaxDurability.exist()) {
		m_edMaxDurability.SetWindowText(CSFormat(_T("%d"), int(spec.iMaxDurability)));
		m_edDurability.SetWindowText(CSFormat(_T("%d"), spec.iCurDur.exist() ? int(spec.iCurDur) : 0));
		m_chIndestructible.EnableWindow(TRUE);
		m_chIndestructible.SetCheck(spec.iMaxDurability == 0);
		if (!m_chIndestructible.GetCheck()) {
			m_edMaxDurability.EnableWindow(TRUE);
			m_edDurability.EnableWindow(TRUE);
		}
	}
	m_edSockets.EnableWindow(TRUE);
	const auto s = spec.Sockets();
	m_edSockets.SetWindowText(CSFormat(_T("%d"), s.first));
	m_sExtSockets = CSFormat(_T("%d"), s.second);
	if (spec.iQuantity.exist()) {
		m_edQuantity.EnableWindow(TRUE);
		m_edQuantity.SetWindowText(CSFormat(_T("%d"), int(spec.iQuantity)));
	}
	m_aProps[0] = { spec.stPropertyList, TRUE };
	for (int i = 1; i <= 5; ++i)
		if (spec.apSetProperty[i - 1].exist())
			m_aProps[i].first = *spec.apSetProperty[i - 1];
	if (bRuneWord) {
		ASSERT(spec.stRuneWordPropertyList.exist());
		m_aProps[6] = { *spec.stRuneWordPropertyList, TRUE };
	}
}

void CDlgFoundry::ReadItemInfo(CItemInfo & info, BOOL bRuneWord, BOOL bNormal) {
	if (info.pGold.exist())
		return ReadItemGold(*info.pGold);
	if (info.pExtItemInfo.exist())
		ReadExtItemInfo(*info.pExtItemInfo, bNormal);
	if (info.pTpSpInfo.exist())
		ReadTypeSpecificInfo(*info.pTpSpInfo, bRuneWord);
}

void CDlgFoundry::ReadItem() {
	SetWindowText(m_stItem.ItemName());
	m_bUnidentified = !m_stItem.bIdentified;
	m_bEthereal = m_stItem.bEthereal;
	m_chRuneWord.EnableWindow(m_stItem.bRuneWord);
	m_chRuneWord.SetCheck(m_stItem.bRuneWord);
	if (m_stItem.pEar.exist())			//耳朵
		return ReadItemEar(*m_stItem.pEar);
	//普通物品
	ASSERT(m_stItem.pItemInfo.exist());
	ReadItemInfo(*m_stItem.pItemInfo, m_stItem.bRuneWord, m_stItem.MetaData().Normal);
}

void CDlgFoundry::validate(bool expr, int index) {
	if (expr)
		return;
	MessageBox(::theApp.MsgBoxInfo(index), ::theApp.MsgError(), MB_ICONERROR);
	throw 0;
}

void CDlgFoundry::WriteItemEar(CEar & ear) {
	ear.iEarLevel = intOf(m_edLevel);
	validate((0 < ear.iEarLevel && ear.iEarLevel <= 99), 22);
	const CString name = stringOf(m_edOwner);
	validate(::CheckCharName(name), 0);
	ear.sEarName = name;
	ear.iEarClass = m_cbClass.GetCurSel();
	validate((0 <= ear.iEarClass && ear.iEarClass < 7), 23);
}

void CDlgFoundry::WriteItemGold(CGoldQuantity & gold) {
	gold.wQuantity = intOf(m_edQuantity);
}

void CDlgFoundry::setPreSuffix(BOOL & exist, MayExist<WORD> & index, const CComboBox & cb, int max, int msg) {
	exist = (cb.GetCurSel() > 0);
	if (exist) {
		index.ensure() = cb.GetCurSel();
		validate(index < max, msg);
	} else
		index.reset();
}

void CDlgFoundry::WriteItemLongName(CLongName & longName)
{
	longName.iName1 = m_cbRareCraftedName[0].GetCurSel();
	validate(longName.iName1 < ::theApp.RareCraftedNameSize(), 34);
	longName.iName2 = m_cbRareCraftedName[1].GetCurSel();
	validate(longName.iName2 < ::theApp.RareCraftedNameSize(), 35);
	setPreSuffix(longName.bPref1, longName.wPref1, m_cbPrefix[0], ::theApp.MagicPrefixSize(), 36);
	setPreSuffix(longName.bPref2, longName.wPref2, m_cbPrefix[1], ::theApp.MagicPrefixSize(), 37);
	setPreSuffix(longName.bPref3, longName.wPref3, m_cbPrefix[2], ::theApp.MagicPrefixSize(), 38);
	setPreSuffix(longName.bSuff1, longName.wSuff1, m_cbSuffix[0], ::theApp.MagicSuffixSize(), 39);
	setPreSuffix(longName.bSuff2, longName.wSuff2, m_cbSuffix[1], ::theApp.MagicSuffixSize(), 40);
	setPreSuffix(longName.bSuff3, longName.wSuff3, m_cbSuffix[2], ::theApp.MagicSuffixSize(), 41);
}

static BOOL checkPersonalizedName(const CString & name) {
	if (name.IsEmpty() || name.GetLength() > 15)
		return FALSE;
	for (int i = 0; i < name.GetLength(); ++i)
		if (name[i] < 32 || 126 < name[i])
			return FALSE;
	return TRUE;
}

void CDlgFoundry::WriteExtItemInfo(CExtItemInfo & extInfo, BOOL & bPersonalized, const CItemMetaData & meta) {
	extInfo.iDropLevel = intOf(m_edLevel);
	validate((0 < extInfo.iDropLevel && extInfo.iDropLevel <= 99), 24);
	extInfo.iQuality = m_cbQuality.GetCurSel() + 1;
	validate((0 < extInfo.iQuality && extInfo.iQuality < 9), 25);
	extInfo.loQual.reset();
	extInfo.hiQual.reset();
	extInfo.wPrefix.reset();
	extInfo.wSuffix.reset();
	extInfo.wSetID.reset();
	extInfo.pRareName.reset();
	extInfo.wUniID.reset();
	extInfo.pCraftName.reset();
	switch (extInfo.iQuality) {
		case 1:		//low
			extInfo.loQual.ensure() = m_cbSubType.GetCurSel();
			validate((extInfo.loQual < 8), 26);
			break;
		case 3:		//high
			extInfo.hiQual.ensure() = m_cbSubType.GetCurSel();
			validate((extInfo.hiQual < 8), 26);
			break;
		case 4:		//magical
			extInfo.wPrefix.ensure() = m_cbPrefix[0].GetCurSel();
			validate(extInfo.wPrefix < ::theApp.MagicPrefixSize(), 44);
			extInfo.wSuffix.ensure() = m_cbSuffix[0].GetCurSel();
			validate(extInfo.wSuffix < ::theApp.MagicSuffixSize(), 45);
			break;
		case 5:		//set
			extInfo.wSetID.ensure() = m_cbSetName.GetCurSel();
			validate(extInfo.wSetID < ::theApp.SetItemNameSize(), 42);
			break;
		case 6:		//rare
			WriteItemLongName(extInfo.pRareName.ensure());
			break;
		case 7:		//unique
			extInfo.wUniID.ensure() = m_cbUniqueName.GetCurSel();
			validate(extInfo.wUniID < ::theApp.UniqueNameSize(), 43);
			break;
		case 8:		//crafted
			WriteItemLongName(extInfo.pCraftName.ensure());
			break;
		default:;	//normal
	}
	CString name = stringOf(m_edOwner);
	bPersonalized = !name.IsEmpty();
	if (bPersonalized){
		validate(checkPersonalizedName(name), 27);
		auto & v = extInfo.sPersonName.ensure();
		fill(v.begin(), v.end(), 0);
		for (int i = 0; i < name.GetLength(); ++i)
			v[i] = char(name[i]);
	}else
		extInfo.sPersonName.reset();
	if (meta.HasMonsterID) {
		extInfo.wMonsterID.ensure() = m_cbMonster.GetCurSel();
		validate((extInfo.wMonsterID < ::theApp.MonsterNameSize()), 33);
	} else
		extInfo.wMonsterID.reset();
}

static vector<pair<WORD, DWORD>> filterPropList(const vector<pair<WORD, DWORD>> & props) {
	vector<pair<WORD, DWORD>> ret;
	for (auto & p : props)
		if (p.first != NO_CODE)
			ret.push_back(p);
	return move(ret);
}

void CDlgFoundry::WriteTypeSpecificInfo(CTypeSpecificInfo & spec, const CItemMetaData & meta, BOOL & bSocketed, BOOL bIsSet, BOOL bRuneWord, int iGems) {
	if (meta.HasDef) {
		int d = intOf(m_edDefence);
		validate((0 <= d && d < 2038), 28);
		spec.SetDefence(d);
	} else
		spec.iDefence.reset();
	if (meta.HasDur) {
		spec.iMaxDurability.ensure() = (m_chIndestructible.GetCheck() ? 0 : intOf(m_edMaxDurability));
		validate((spec.iMaxDurability < 256), 29);
	} else
		spec.iMaxDurability.reset();
	if (spec.iMaxDurability.exist() && spec.iMaxDurability > 0) {
		spec.iCurDur.ensure() = intOf(m_edDurability);
		validate((spec.iCurDur <= spec.iMaxDurability), 30);
	}else
		spec.iCurDur.reset();
	const int s2 = intOf(m_sExtSockets);
	const int s1 = max(intOf(m_edSockets), s2 > 0 ? 1 : 0);
	validate((0 <= s1 && 0 <= s2 && s1 + s2 <= MAX_SOCKETS), 31);
	ASSERT(0 <= iGems);
	validate(iGems <= s1 + s2, 48);
	bSocketed = (s1 + s2 > 0);
	if (bSocketed) {
		spec.iSocket.ensure() = s1;
	} else
		spec.iSocket.reset();
	if (meta.IsStacked) {
		spec.iQuantity.ensure() = intOf(m_edQuantity);
		validate((spec.iQuantity < 512), 32);
	} else
		spec.iQuantity.reset();
	ASSERT(m_aProps[0].second);
	spec.stPropertyList.mProperty = filterPropList(m_aProps[0].first.mProperty);
	if (bIsSet) {
		spec.aHasSetPropList.ensure();
		for (UINT i = 0; i < spec.aHasSetPropList.size(); ++i) {
			spec.apSetProperty[i].reset();
			if (m_aProps[i].second) {	//enabled
				auto props = filterPropList(m_aProps[i + 1].first.mProperty);
				if (!props.empty())		//property list is not empty
					spec.apSetProperty[i].ensure().mProperty = move(props);
			}
		}
	}
	if (bRuneWord) {
		ASSERT(m_aProps[6].second);
		spec.stRuneWordPropertyList.ensure().mProperty = filterPropList(m_aProps[6].first.mProperty);
	} else
		spec.stRuneWordPropertyList.reset();
}

void CDlgFoundry::WriteItemInfo(CItemInfo & info, BOOL bSimple, BOOL & bPersonalized, const CItemMetaData & meta, BOOL & bSocketed, BOOL bRuneWord, int iGemIdxMax) {
	if (info.IsGold()) {
		WriteItemGold(info.pGold.ensure());
	} else
		info.pGold.reset();
	if (bSimple) {
		info.pExtItemInfo.reset();
		info.pTpSpInfo.reset();
	} else {
		WriteExtItemInfo(info.pExtItemInfo.ensure(), bPersonalized, meta);
		WriteTypeSpecificInfo(info.pTpSpInfo.ensure(), meta, bSocketed, info.pExtItemInfo->IsSet(), bRuneWord, max(iGemIdxMax + 1, info.pExtItemInfo->nGems));
	}
}

void CDlgFoundry::WriteItem(CD2Item & item) {
	item.bIdentified = !m_bUnidentified;
	item.bEthereal = m_bEthereal;
	item.bRuneWord = m_chRuneWord.GetCheck();
	if (item.bEar) {
		WriteItemEar(item.pEar.ensure());
		item.pItemInfo.reset();
	} else {
		WriteItemInfo(item.pItemInfo.ensure(), item.bSimple, item.bPersonalized, item.MetaData(), item.bSocketed, item.bRuneWord, item.GemIndexMax());
		item.pEar.reset();
	}
}

BOOL CDlgFoundry::GatherData() {
	try {
		UpdateData(TRUE);
		CD2Item item(m_stItem);
		WriteItem(item);
		m_stItem = item;
	} catch (...) {
		return FALSE;
	}
	return TRUE;
}

BOOL CDlgFoundry::OnInitDialog() {
	CDialogEx::OnInitDialog();
	InitUI();
	ReadItem();
	UpdatePropList();
	UpdateData(FALSE);
	return TRUE;
}

void CDlgFoundry::OnOK() {
	if (GatherData())
		CDialogEx::OnOK();
}

void CDlgFoundry::OnBnClickedCheckIndestructible() {
	const BOOL ch = m_chIndestructible.GetCheck();
	m_edDurability.EnableWindow(!ch);
	m_edMaxDurability.EnableWindow(!ch);
}

void CDlgFoundry::OnCbnSelchangeComboQuality() {
	//Disable all combox
	for (auto & cb : m_cbRareCraftedName)
		cb.EnableWindow(FALSE);
	for (auto & cb : m_cbPrefix)
		cb.EnableWindow(FALSE);
	for (auto & cb : m_cbSuffix)
		cb.EnableWindow(FALSE);
	m_cbSubType.EnableWindow(FALSE);
	m_cbSetName.EnableWindow(FALSE);
	m_cbUniqueName.EnableWindow(FALSE);
	//Set props & list
	const BOOL setEnabled = (m_cbQuality.GetCurSel() == 4);
	if (m_aProps[1].second != setEnabled) {	//修改了物品是否为套装
		for (int i = 0; i < 5; ++i)
			m_aProps[i + 1].second = setEnabled;
		const int tab = m_tabPropList.GetCurSel();
		if (0 < tab && tab < 6)				//当前展示的是套装属性列表
			m_lstProperty.EnableWindow(setEnabled);
		m_sExtSockets = CSFormat(_T("%d"), ExtendSockets());	//更新扩展孔数
		UpdateData(FALSE);
	}
	//Adjust ComboBoxes
	switch (m_cbQuality.GetCurSel()) {
		case 0:case 2:	//low & high
			m_cbSubType.EnableWindow(TRUE);
			break;
		case 3:			//magical
			m_cbPrefix[0].EnableWindow(TRUE);
			m_cbSuffix[0].EnableWindow(TRUE);
			break;
		case 4:			//set
			m_cbSetName.EnableWindow(TRUE);
			break;
		case 5:case 7:	//rare
			for (auto & cb : m_cbRareCraftedName)
				cb.EnableWindow(TRUE);
			for (auto & cb : m_cbPrefix)
				cb.EnableWindow(TRUE);
			for (auto & cb : m_cbSuffix)
				cb.EnableWindow(TRUE);
			break;
		case 6:			//unique
			m_cbUniqueName.EnableWindow(TRUE);
			break;
		default:;		//normal
	}
}

void CDlgFoundry::OnTcnSelchangeTabProperty(NMHDR *pNMHDR, LRESULT *pResult) {
	UpdatePropList();
	UpdateData(FALSE);
	*pResult = 0;
}

static int CALLBACK PropItemCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	auto & list = *reinterpret_cast<const CListCtrl *>(lParamSort);
	const auto i = static_cast<int>(lParam1), j = static_cast<int>(lParam2);
	const auto ch1 = list.GetCheck(i), ch2 = list.GetCheck(j);
	if (ch1 != ch2)
		return (ch1 ? -1 : 1);
	const auto s1 = list.GetItemText(i, 0), s2 = list.GetItemText(j, 0);
	return (s1 < s2 ? -1 : (s1 == s2 ? 0  : 1));
}

void CDlgFoundry::UpdatePropList() {
	{	//reset params & list
		__EnableFlag f1(m_bNoPropItemSelectMsg), f2(m_bNoParamChangeMsg);
		m_lstProperty.DeleteAllItems();
		SetupPropParameters(0, 0, FALSE);
	}
	//find property list
	const int i = m_tabPropList.GetCurSel();
	ASSERT(0 <= i && i < int(size(m_aProps)));
	const auto & p = m_aProps[i];
	m_lstProperty.EnableWindow(p.second);
	{	//insert checked items
		__EnableFlag f(m_bNoPropItemCheckMsg);
		for(UINT idx = 0;idx < p.first.mProperty.size();++idx){
			const auto code = p.first.mProperty[idx].first;
			if (code == NO_CODE)
				continue;
			const auto value = p.first.mProperty[idx].second;
			const int i = m_lstProperty.InsertItem(m_lstProperty.GetItemCount(), CSFormat(_T("%3d"), code));	//属性ID
			m_lstProperty.SetItemText(i, 1, ::theApp.PropertyDescription(m_stItem.dwVersion, code, value));	//属性描述
			m_lstProperty.SetItemData(i, idx + 1);	//设置为索引+1，非0
			m_lstProperty.SetCheck(i, TRUE);
		}
	}
	//add unchecked items and sort
	for (UINT c = 0; c < ::theApp.PropertyNameSize(); ++c) {
		const int i = m_lstProperty.InsertItem(m_lstProperty.GetItemCount(), CSFormat(_T("%3d"), c));	//属性ID
		m_lstProperty.SetItemText(i, 1, ::theApp.PropertyDescription(m_stItem.dwVersion, c));	//属性描述
	}
	m_lstProperty.SortItemsEx(PropItemCompare, reinterpret_cast<LPARAM>(&m_lstProperty));
}

void CDlgFoundry::SetupParamValueFrom(UINT idx,BOOL enabled, const std::vector<CPropParam> & param) {
	for (; idx < size(m_edParam); ++idx)
		if (idx < param.size())
			SetupParamValue(idx, enabled, param[idx]);
		else
			SetupParamValue(idx, FALSE, CPropParam());
}

void CDlgFoundry::SetupParamValue(UINT idx,BOOL enabled, const CPropParam & param) {
	ASSERT(idx < size(m_edParam));
	auto & ed = m_edParam[idx];
	ed.ShowWindow(SW_SHOW);
	ed.EnableWindow(enabled);
	if (idx < size(m_cbParam))
		m_cbParam[idx].ShowWindow(SW_HIDE);
	if (enabled) {
		ed.SetWindowText(CSFormat(_T("%d"), param.iValue));
		m_sTip[idx] = (CSFormat(_T("%d - %d"), param.iMin, param.iMax));
	} else {
		ed.SetWindowText(_T(""));
		m_sTip[idx] = (_T(""));
	}
}

void CDlgFoundry::SetupParamListGeneric(UINT idx, BOOL enabled, int value, function<int(CComboBox &)> setup) {
	ASSERT(idx < size(m_edParam) && idx < size(m_cbParam));
	auto & ed = m_edParam[idx];
	auto & cb = m_cbParam[idx]; 
	cb.ShowWindow(SW_SHOW);
	cb.EnableWindow(enabled);
	ed.ShowWindow(SW_HIDE);
	m_sTip[idx] = (_T(""));
	if (enabled) {
		cb.ResetContent();
		const int sel = setup(cb);
		ASSERT(0 <= sel && sel < cb.GetCount());
		cb.SetCurSel(sel);
	} else
		cb.SetCurSel(-1);
}

void CDlgFoundry::SetupParamClass(UINT idx, BOOL enabled, int value) {
	SetupParamListGeneric(idx, enabled, value, [=](auto & cb)->int {
		for (UINT i = 0; i < ::theApp.CLASS_NAME_SIZE; ++i) {
			const int j = cb.AddString(::theApp.ClassName(i));
			cb.SetItemData(j, i);
		}
		return value;
	});
}

void CDlgFoundry::SetupParamSkill(UINT idx, BOOL enabled, int value, BOOL classOnly) {
	auto addRangeClass = [=](int from, int to, CComboBox & cb)->int {	//将范围内的角色技能id加入到ComboBox里面
		int r = -1;
		for (int i = from; i < to; ++i) {
			const auto p = ::theApp.ClassSkillName(i);
			ASSERT(0 <= p.second);
			if ((i - from) % ::theApp.CLASS_SKILL_NAME_SIZE == 0) {	//加入角色名作为分隔符
				const int j = cb.AddString(CSFormat(_T("---%s---"), ::theApp.ClassName(p.second)));
				cb.SetItemData(j, i);
			}
			const int j = cb.AddString(p.first);
			cb.SetItemData(j, i);
			if (value == i)
				r = j;
		}
		return r;
	};
	auto addRangeOther = [=](int from, int to, CComboBox & cb)->int {	//将范围内的非角色技能id加入到ComboBox里面
		int r = -1;
		for (int i = from; i < to; ++i) {
			const auto p = ::theApp.ClassSkillName(i);
			ASSERT(p.second < 0);
			const int j = cb.AddString(p.first);
			cb.SetItemData(j, i);
			if (value == i)
				r = j;
		}
		return r;
	};
	SetupParamListGeneric(idx, enabled, value, [=](auto & cb)->int {
		//角色技能id范围：[6, 156)为亚马逊，法师，死灵，圣骑士，野蛮人技能，[221, 281)为德鲁伊，刺客技能。
		//其他技能id范围：[0, 6), [217, 221), 350(Delirium)
		const auto r1 = addRangeClass(6, 156, cb), r2 = addRangeClass(221, 281, cb);
		int r = max(r1, r2);
		if (!classOnly) {
			cb.AddString(_T("---Other---"));
			const int r3 = addRangeOther(0, 6, cb), r4 = addRangeOther(217, 221, cb), r5 = addRangeOther(350, 351, cb);
			r = max(r, max(r3, max(r4, r5)));
		}
		return r;
	});
}

void CDlgFoundry::SetupParamAuraSkill(UINT idx, BOOL enabled, int value) {
	SetupParamListGeneric(idx, enabled, value, [=](auto & cb)->int {
		int r = -1;
		//圣骑士的攻击和防御光环
		for (int i = 96; i < 126; ++i) {	//圣骑士skill id范围[96, 126)
			const auto k = ::theApp.CLASS_SKILL_INDEX[3][i - 96];	//圣骑士角色id为3
			if (k < 10)	//圣骑士技能树：作战技能(0-9)，攻击灵气(10-19)，防御性灵气(20-29)
				continue;
			const auto p = ::theApp.ClassSkillName(i);
			ASSERT(3 == p.second);	//圣骑士角色id为3
			const int j = cb.AddString(p.first);
			cb.SetItemData(j, i);
			if (value == i)
				r = j;
		}
		return r;
	});
}

void CDlgFoundry::SetupParamMonster(UINT idx, BOOL enabled, int value) {
	SetupParamListGeneric(idx, enabled, value, [=](auto & cb)->int {
		for (UINT i = 0; i < ::theApp.MonsterNameSize(); ++i) {
			const int j = cb.AddString(::theApp.MonsterName(i));
			cb.SetItemData(j, i);
		}
		return value;
	});
}

void CDlgFoundry::SetupParamTime(UINT idx, BOOL enabled, int value) {
	SetupParamListGeneric(idx, enabled, value, [=](auto & cb)->int {
		for (UINT i = 0; i < ::theApp.TimeNameSize(); ++i) {
			const int j = cb.AddString(::theApp.TimeName(i));
			cb.SetItemData(j, i);
		}
		return value;
	});
}

void CDlgFoundry::SetupPropParameters(int id, int value, BOOL checked) {
	ASSERT(0 <= id);
	const auto p = ::theApp.PropertyParameters(m_stItem.dwVersion, id, value);
	switch (id) {
		case 83:
			ASSERT(p.size() == 2);
			SetupParamClass(0, checked, p[0].iValue);
			SetupParamValueFrom(1, checked, p);
			break;
		case 97:case 107:
			ASSERT(p.size() == 2);
			SetupParamSkill(0, checked, p[0].iValue, TRUE);
			SetupParamValueFrom(1, checked, p);
			break;
		case 151:
			ASSERT(p.size() == 2);
			SetupParamAuraSkill(0, checked, p[0].iValue);
			SetupParamValueFrom(1, checked, p);
			break;
		case 155:case 179:case 180:
			ASSERT(p.size() == 2);
			SetupParamMonster(0, checked, p[0].iValue);
			SetupParamValueFrom(1, checked, p);
			break;
		case 188:
			ASSERT(p.size() == 3);
			SetupParamValue(0, checked, p[0]);
			SetupParamClass(1, checked, p[1].iValue);
			SetupParamValueFrom(2, checked, p);
			break;
		case 195:case 196:case 197:case 198:case 199:case 201:case 204:
			ASSERT(p.size() >= 3);
			SetupParamValue(0, checked, p[0]);
			SetupParamSkill(1, checked, p[1].iValue, FALSE);
			SetupParamValueFrom(2, checked, p);
			break;
		case 268:case 269:case 270:case 271:case 272:case 273:case 274:case 275:case 276:case 277:
		case 278:case 279:case 280:case 281:case 282:case 283:case 284:case 285:case 286:case 287:
		case 288:case 289:case 290:case 291:case 292:case 293:case 294:case 295:case 296:case 297:
		case 298:case 299:case 300:case 301:case 302:case 303:
			ASSERT(p.size() == 3);
			SetupParamTime(0, checked, p[0].iValue);
			SetupParamValueFrom(1, checked, p);
			break;
		default:SetupParamValueFrom(0, checked, p);
	}
}

BOOL CDlgFoundry::ChangePropItemSelection(LPNMLISTVIEW pNMLV) {
	if (m_bNoPropItemSelectMsg)
		return FALSE;
	if ((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED)) {
		//find out property list
		const int sel = m_tabPropList.GetCurSel();
		ASSERT(0 <= sel && sel < int(size(m_aProps)));
		const auto & p = m_aProps[sel].first.mProperty;
		//setup parameters
		UINT idx = m_lstProperty.GetItemData(pNMLV->iItem);
		__EnableFlag f(m_bNoParamChangeMsg);
		if (idx > 0) {
			--idx;
			ASSERT(idx < p.size());
			SetupPropParameters(p[idx].first, p[idx].second, TRUE);
		} else {
			const auto code = intOf(m_lstProperty.GetItemText(pNMLV->iItem, 0));
			SetupPropParameters(code, 0, FALSE);
		}
		return TRUE;
	}
	return FALSE;
}

static int getSelectedIndex(CListCtrl & list) {
	POSITION pos = list.GetFirstSelectedItemPosition();
	if (!pos)
		return -1;
	return list.GetNextSelectedItem(pos);
}

BOOL CDlgFoundry::ChangePropItemChecked(LPNMLISTVIEW pNMLV) {
	if (m_bNoPropItemCheckMsg)
		return FALSE;
	BOOL bPrevState = (BOOL)(((pNMLV->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if (bPrevState < 0)
		bPrevState = 0;
	BOOL bChecked = (BOOL)(((pNMLV->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if (bChecked < 0)
		bChecked = 0;
	if (bPrevState == bChecked)
		return FALSE;
	//find out property list
	const int sel = m_tabPropList.GetCurSel();
	ASSERT(0 <= sel && sel < int(size(m_aProps)));
	auto & p = m_aProps[sel].first.mProperty;
	//handle item
	const int idx = pNMLV->iItem;
	BOOL update = (idx == getSelectedIndex(m_lstProperty));
	int code = 0, value = 0;
	if (bChecked) {	//check a new item
		code = intOf(m_lstProperty.GetItemText(idx, 0));
		value = ::theApp.PropertyMetaData(m_stItem.dwVersion, code).DefaultValue();
		const int i = m_lstProperty.InsertItem(idx + 1, CSFormat(_T("%3d"), code));	//insert new item with same code
		m_lstProperty.SetItemText(i, 1, ::theApp.PropertyDescription(m_stItem.dwVersion, code, value));
		p.emplace_back(code, value);	//add property to list
		m_lstProperty.SetItemData(idx, p.size());
	} else {		//uncheck an item
		const UINT i = m_lstProperty.GetItemData(idx) - 1;
		ASSERT(i < p.size());
		code = p[i].first;
		p[i].first = NO_CODE;			//delete property from list
		m_lstProperty.DeleteItem(idx);	//delete item
	}
	if (update) {
		__EnableFlag f(m_bNoParamChangeMsg);
		SetupPropParameters((bChecked ? code : 0), value, bChecked);
	}
	//extend sockets
	if (194 == code) {
		m_sExtSockets = CSFormat(_T("%d"), ExtendSockets());
		UpdateData(FALSE);
	}
	return update;
}

void CDlgFoundry::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BOOL update = ChangePropItemSelection(pNMLV);
	update = (ChangePropItemChecked(pNMLV) || update);
	if(update)
		UpdateData(FALSE);
	*pResult = 0;
}

pair<BOOL, DWORD> CDlgFoundry::GatherParamaters(const CPropertyMetaDataItem & meta) const {
	vector<int> ret;
	for (UINT i = 0; i < size(m_edParam); ++i) {
		auto & ed = m_edParam[i];
		if (ed.IsWindowVisible()) {
			ret.push_back(intOf(ed));
		} else {
			ASSERT(i < size(m_cbParam));
			auto & cb = m_cbParam[i];
			const int sel = cb.GetCurSel();
			if (sel < 0 || cb.GetCount() <= sel)
				return make_pair(FALSE, 0);
			ret.push_back(cb.GetItemData(sel));	//用Item Data作为值
		}
	}
	return meta.GetValue(ret);
}

int CDlgFoundry::ExtendSockets() const {
	int r = 0;
	for (auto & p : m_aProps) {
		if (!p.second)
			continue;
		r += p.first.ExtSockets();
	}
	return r;
}

void CDlgFoundry::OnEnChangeParam() {
	const int it = getSelectedIndex(m_lstProperty);
	if (m_bNoParamChangeMsg || it < 0)
		return;
	//find out prop list
	const int sel = m_tabPropList.GetCurSel();
	ASSERT(0 <= sel && sel < int(size(m_aProps)));
	auto & pp = m_aProps[sel].first.mProperty;
	//find out code & meta
	const UINT idx = m_lstProperty.GetItemData(it) - 1;
	ASSERT(idx < pp.size());
	const auto code = pp[idx].first;
	auto & meta = ::theApp.PropertyMetaData(m_stItem.dwVersion, code);
	//update value
	const auto v = GatherParamaters(meta);
	if (!v.first)
		return;
	pp[idx].second = v.second;
	//update desc
	m_lstProperty.SetItemText(it, 1, ::theApp.PropertyDescription(m_stItem.dwVersion, code, v.second));
	//extend sockets
	if (194 == code) {
		m_sExtSockets = CSFormat(_T("%d"), ExtendSockets());
		UpdateData(FALSE);
	}
}

void CDlgFoundry::OnBnClickedCheckRuneword() {
	auto & p = m_aProps[6];	//Rune word prop list
	const BOOL e = m_chRuneWord.GetCheck();
	if(p.second != e){	//修改了物品是否为符文之语
		p.second = e;
		const int tab = m_tabPropList.GetCurSel();
		if (tab == 6)	//当前展示的是符文之语属性列表
			m_lstProperty.EnableWindow(e);
		m_sExtSockets = CSFormat(_T("%d"), ExtendSockets());	//更新扩展孔数
		UpdateData(FALSE);
	}
}


void CDlgFoundry::OnEnChangeEditBaseSockets()
{
	//当扩展孔数大于0时，基础孔数至少为1
	const int sockets = intOf(m_edSockets);
	if (sockets < 1) {
		const int extSockets = intOf(m_sExtSockets);
		if (extSockets > 0) m_edSockets.SetWindowTextW(_T("1"));
	}
}
