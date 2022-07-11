// DlgCharItems.cpp : 实现文件
//

#include "stdafx.h"

#include "Diablo Edit2.h"
#include "DlgCharItems.h"
#include "DlgFoundry.h"
#include "DlgNewItem.h"

#include <functional>

using namespace std;

//Popup menu item IDs
#define ID_ITEM_IMPORT                  100
#define ID_ITEM_EXPORT                  101
#define ID_ITEM_COPY                    102
#define ID_ITEM_PASTE                   103
#define ID_ITEM_MODIFY                  104
#define ID_ITEM_NEW						105
#define ID_ITEM_NEW_FROM				106
#define ID_ITEM_REMOVE                  107

const int GRID_WIDTH = 30;	//每个网格的边长(像素)

//物品类型和能装备的位置
enum EEquip {
	E_ANY = -1,				//可接受任意物品

	E_STORAGE = 1,			//不可穿戴，只能放在存储箱里
	E_HEAD = 1 << 1,		//头盔
	E_NECK = 1 << 2,		//项链
	E_BODY = 1 << 3,		//衣服
	E_HAND = 1 << 4,		//武器和盾牌
	E_RING = 1 << 5,		//戒指
	E_BELT = 1 << 6,		//腰带
	E_FOOT = 1 << 7,		//鞋子
	E_GLOVE = 1 << 8,		//手套
	E_IN_BELT = 1 << 9,		//放在腰带里（药品等）
	E_SOCKET = 1 << 10,		//可镶嵌（珠宝，符文等）
	E_A_BOX = 1 << 11,		//该物品是赫拉迪卡方块
};

static EEquip ItemToEquip(const CD2Item & item) {
	if (item.IsBox())
		return E_A_BOX;
	auto & meta = item.MetaData();
	if (0 == meta.Equip)
		return (meta.IsGem ? E_SOCKET : E_STORAGE);
	ASSERT(meta.Equip <= 10);
	return EEquip(1 << meta.Equip);
}

//所有网格位置
enum EPosition {
	STASH,				//箱子(6*8)
	STASH_D2R,			//D2 Resurrected大箱子(10*10)
	INVENTORY,			//口袋
	CUBE,				//方块
	IN_BELT,			//腰带里
	IN_SOCKET,			//镶嵌在孔里
	GRID_COUNT,

	HEAD = GRID_COUNT,	//头
	NECK,				//项链
	BODY,				//身体
	RIGHT_HAND,			//武器右(I & II)
	LEFT_HAND,			//武器左(I & II)
	RIGHT_RING,			//戒指右
	LEFT_RING,			//戒指左
	BELT,				//腰带
	FOOT,				//鞋子
	GLOVE,				//手套

	CORPSE_HEAD,		//尸体的头
	CORPSE_NECK,		//尸体的项链
	CORPSE_BODY,		//尸体的身体
	CORPSE_RIGHT_HAND,	//尸体的武器右(I & II)
	CORPSE_LEFT_HAND,	//尸体的武器左(I & II)
	CORPSE_RIGHT_RING,	//尸体的戒指右
	CORPSE_LEFT_RING,	//尸体的戒指左
	CORPSE_BELT,		//尸体的腰带
	CORPSE_FOOT,		//尸体的鞋子
	CORPSE_GLOVE,		//尸体的手套
	CORPSE_END,

	MERCENARY_HEAD = CORPSE_END,//雇佣兵的头
	MERCENARY_BODY,				//雇佣兵的身体
	MERCENARY_RIGHT_HAND,		//雇佣兵的武器右
	MERCENARY_LEFT_HAND,		//雇佣兵的武器左
	MERCENARY_END,

	GOLEM = MERCENARY_END,		//生成金属石魔的物品

	POSITION_END,				//所有网格位置总数

	IN_MOUSE = POSITION_END,	//被鼠标拿起
	IN_RECYCLE,					//被删除
};

static BOOL IsCorpse(EPosition pos) { return CORPSE_HEAD <= pos && pos < CORPSE_END; }

static BOOL IsMercenary(EPosition pos) { return MERCENARY_HEAD <= pos && pos < MERCENARY_END; }

static BOOL IsInMouse(EPosition pos) { return IN_MOUSE == pos; }

static BOOL IsInSocket(EPosition pos) { return IN_SOCKET == pos; }

static BOOL IsInRecycle(EPosition pos) { return IN_RECYCLE == pos; }

static BOOL HasNormalII(EPosition pos) { return RIGHT_HAND == pos || LEFT_HAND == pos; }

static BOOL HasCorpseII(EPosition pos) { return CORPSE_RIGHT_HAND == pos || CORPSE_LEFT_HAND == pos; }

static BOOL HasII(EPosition pos) { return HasNormalII(pos) || HasCorpseII(pos); }

static BOOL IsGrid(EPosition pos) { return pos < GRID_COUNT; }

static BOOL IsGolem(EPosition pos) { return GOLEM == pos; }

//每个位置(EPosition)在UI的起始坐标(像素),列数,行数
//left,top,col,row,equip
const int POSITION_INFO[POSITION_END][5] = {
	{10,5,6,8,E_ANY},		//箱子
	{10,5,10,10,E_ANY},		//D2R大箱子
	{10,315,10,4,E_ANY},	//口袋
	{320,315,3,4,~E_A_BOX},	//方块
	{420,315,4,4,E_IN_BELT},//腰带里
	{70,445,MAX_SOCKETS,1,E_SOCKET},	//孔

	{420,30,2,2,E_HEAD},	//头
	{485,65,1,1,E_NECK},	//项链
	{420,95,2,3,E_BODY},	//身体
	{320,65,2,4,E_HAND},	//武器右
	{520,65,2,4,E_HAND},	//武器左
	{385,190,1,1,E_RING},	//戒指右
	{485,190,1,1,E_RING},	//戒指左
	{420,190,2,1,E_BELT},	//腰带		
	{520,190,2,2,E_FOOT},	//鞋子
	{320,190,2,2,E_GLOVE},	//手套

	{780,30,2,2,E_HEAD},	//尸体的头
	{845,65,1,1,E_NECK},	//尸体的项链
	{780,95,2,3,E_BODY},	//尸体的身体
	{680,65,2,4,E_HAND},	//尸体的武器右
	{880,65,2,4,E_HAND},	//尸体的武器左
	{745,190,1,1,E_RING},	//尸体的戒指右
	{845,190,1,1,E_RING},	//尸体的戒指左
	{780,190,2,1,E_BELT},	//尸体的腰带		
	{880,190,2,2,E_FOOT},	//尸体的鞋子
	{680,190,2,2,E_GLOVE},	//尸体的手套

	{660,280,2,2,E_HEAD},	//雇佣兵的头
	{660,345,2,3,E_BODY},	//雇佣兵的身体
	{560,315,2,4,E_HAND},	//雇佣兵的武器右
	{760,315,2,4,E_HAND},	//雇佣兵的武器左

	{600,30,2,4,E_ANY},		//生成金属石魔的物品
};

static CRect PositionToRect(EPosition pos) {
	ASSERT(pos < POSITION_END);
	auto & p = POSITION_INFO[pos];
	return CRect(p[0], p[1], p[0] + GRID_WIDTH * p[2], p[1] + GRID_WIDTH * p[3]);
}

static int PositionToCol(EPosition pos) {
	ASSERT(pos < POSITION_END);
	return POSITION_INFO[pos][2];
}

static int PositionToRow(EPosition pos) {
	ASSERT(pos < POSITION_END);
	return POSITION_INFO[pos][3];
}

static EEquip PositionToEquip(EPosition pos) {
	ASSERT(pos < POSITION_END);
	return EEquip(POSITION_INFO[pos][4]);
}

//body: 0-人物本身，1-尸体，2-雇佣兵，3-Golem
static tuple<EPosition, int, int> ItemToPosition(int iLocation, int iPosition, int iColumn, int iRow, int iStoredIn, int body, BOOL isD2R) {
	if (3 == body)
		return make_tuple(GOLEM, 0, 0);
	int pos = -1, x = 0, y = 0;	//物品的位置(EPosition)和坐标
	switch (iLocation) {
		case 0:		//grid
			pos = (iStoredIn == 1 ? INVENTORY : (iStoredIn == 4 ? CUBE : (iStoredIn == 5 ? (isD2R ? STASH_D2R : STASH) : -1)));
			x = iColumn;
			y = iRow;
			break;
		case 1:		//equipped
			if (0 < iPosition && iPosition <= 12) {
				x = (iPosition <= 10 ? 0 : 1);
				pos = iPosition  + GRID_COUNT - 1 - 7 * x;
				if (1 == body)
					pos += CORPSE_HEAD - HEAD;
				else if (body == 2) {
					switch (pos) {
						case HEAD:pos = MERCENARY_HEAD; break;
						case BODY:pos = MERCENARY_BODY; break;
						case RIGHT_HAND:pos = MERCENARY_RIGHT_HAND; break;
						case LEFT_HAND:pos = MERCENARY_LEFT_HAND; break;
						default:ASSERT(FALSE && _T("Invalid body position of mercenary"));
					}
				}
			}
			break;
		case 2:		//in belt(物品排列方式与其他网格不同)
			pos = IN_BELT;
			x = iColumn % 4;
			y = 3 - iColumn / 4;
			break;
		case 4:		//in hand(鼠标)
			pos = IN_MOUSE;
			break;
		default:;
	}
	if (pos < 0)
		ASSERT(FALSE && _T("Invalid item position"));
	return make_tuple(EPosition(pos), x, y);
}

static tuple<int, int, int, int, int> PositionToItem(EPosition pos, int x, int y) {
	ASSERT(0 <= x && 0 <= y);
	int loc = 0, body = 0, col = 0, row = 0, store = 0;
	switch (pos) {
		//grid
		case STASH:
		case STASH_D2R: col = x, row = y, store = 5; break;
		case INVENTORY:	col = x, row = y, store = 1; break;
		case CUBE:		col = x, row = y, store = 4; break;
		case IN_BELT:	loc = 2, col = (3 - y) * 4 + x; break;
		case IN_SOCKET:	loc = 6, col = x; break;
		//body
		case RIGHT_HAND:
		case LEFT_HAND:	body = (1 == x ? 7 : 0);	//II hand adjustment
		case HEAD:
		case NECK:
		case BODY:
		case RIGHT_RING:
		case LEFT_RING:
		case BELT:
		case FOOT:
		case GLOVE:		loc = 1, body += pos - HEAD + 1; break;
		//corpse
		case CORPSE_RIGHT_HAND:
		case CORPSE_LEFT_HAND:	body = (1 == x ? 7 : 0);	//II hand adjustment
		case CORPSE_HEAD:
		case CORPSE_NECK:
		case CORPSE_BODY:
		case CORPSE_RIGHT_RING:
		case CORPSE_LEFT_RING:
		case CORPSE_BELT:
		case CORPSE_FOOT:
		case CORPSE_GLOVE:		loc = 1, body += pos - CORPSE_HEAD + 1; break;
		//mercenary
		case MERCENARY_HEAD:		loc = 1, body = 1; break;
		case MERCENARY_BODY:		loc = 1, body = 3; break;
		case MERCENARY_RIGHT_HAND:	loc = 1, body = 4; break;
		case MERCENARY_LEFT_HAND:	loc = 1, body = 5; break;
		//other
		case IN_MOUSE:	loc = 4; break;
	}
	return make_tuple(loc, body, col, row, store);
}

//struct CItemView

CItemView::CItemView(const CD2Item & item, EEquip equip, EPosition pos, int x, int y)
	: Item(item)
	, nPicRes(IDB_BITMAP0 + item.MetaData().PicIndex)
	, iEquip(equip)
	, iPosition(pos)
	, iGridX(x)
	, iGridY(y)
	, iGridWidth((item.MetaData().Range >> 4) & 0xF)
	, iGridHeight(item.MetaData().Range & 0xF)
{
	ASSERT(0 < iGridWidth && 0 < iGridHeight);
	vGemItems.resize(item.Sockets(), -1);
	ASSERT(int(vGemItems.size()) <= PositionToCol(IN_SOCKET));
}

CSize CItemView::ViewSize() const { return CSize(iGridWidth * GRID_WIDTH, iGridHeight*GRID_WIDTH); }

int CItemView::GemCount() const {
	int r = 0;
	for (int i : vGemItems)
		if (0 <= i)
			++r;
	return r;
}

const CD2Item & CItemView::UpdateItem(std::vector<CItemView> & vItemViews) {
	//update position
	const auto t = PositionToItem(iPosition, iGridX, iGridY);
	Item.iLocation = get<0>(t);
	Item.iPosition = get<1>(t);
	Item.iColumn = get<2>(t);
	Item.iRow = get<3>(t);
	Item.iStoredIn = get<4>(t);
	//update gems
	Item.aGemItems.clear();
	int gems = 0;
	for (int i : vGemItems) {
		if (i < 0)
			continue;
		ASSERT(i < int(vItemViews.size()));
		Item.aGemItems.push_back(vItemViews[i].UpdateItem(vItemViews));
		++gems;
	}
	if (Item.pItemInfo.exist() && Item.pItemInfo->pExtItemInfo.exist()) {
		Item.pItemInfo->pExtItemInfo->nGems = gems;
	} else
		ASSERT(0 == gems);
	return Item;
}

//struct GridView

GridView::GridView(EPosition pos)
	: iPosition(pos)
	, iCol(PositionToCol(pos))
	, iRow(PositionToRow(pos))
	, Rect(PositionToRect(pos))
	, iEquip(PositionToEquip(pos))
{
	vItemIndex.resize((IsGrid() ? iCol * iRow : (::HasII(pos) ? 2 : 1)), -1);
}

BOOL GridView::IsGrid() const { return ::IsGrid(iPosition); }

BOOL GridView::IsSockets() const { return ::IsInSocket(iPosition); }

int GridView::ItemIndex(int x, int y) const {
	ASSERT(0 <= x && 0 <= y);
	const UINT idx = x + y * iCol;
	ASSERT(idx < vItemIndex.size());
	return vItemIndex[idx];
}

void GridView::ItemIndex(int index, int x, int y) {
	ASSERT(0 <= x && x < iCol);
	ASSERT(0 <= y && y < iRow);
	const UINT idx = x + y * iCol;
	ASSERT(idx < vItemIndex.size());
	vItemIndex[idx] = index;
}

void GridView::ItemIndex(int index, int x, int y, int width, int height) {
	ASSERT(0 < width && 0 < height);
	if (IsGrid())
		for (int i = 0; i < width; ++i)
			for (int j = 0; j < height; ++j)
				ItemIndex(index, x + i, y + j);
	else
		ItemIndex(index, x, y);
}

CPoint GridView::IndexToXY(int x, int y, int width, int height) const {
	ASSERT(0 <= x && 0 <= y);
	ASSERT(0 < width && width <= iCol);
	ASSERT(0 < height && height <= iRow);
	if(IsGrid())
		return CPoint(Rect.left + x * GRID_WIDTH, Rect.top + y * GRID_WIDTH);
	x = Rect.left + (iCol - width) * GRID_WIDTH / 2;
	y = Rect.top + (iRow - height) * GRID_WIDTH / 2;
	return CPoint(x, y);
}

tuple<int, int, int> GridView::XYToPositionIndex(CPoint pos, BOOL II, BOOL corpseII, int col, int row) const {
	ASSERT(0 <= pos.x && 0 <= pos.y);
	ASSERT(0 < col && 0 < row);
	if (IsGrid()) {
		int x = (pos.x - (col - 1) * GRID_WIDTH / 2 - Rect.left) / GRID_WIDTH;
		int y = (pos.y - (row - 1) * GRID_WIDTH / 2 - Rect.top) / GRID_WIDTH;
		ASSERT(0 <= x && x < iCol);
		ASSERT(0 <= y && y < iRow);
		return make_tuple(iPosition, x, y);
	} else if(HasNormalII(iPosition)){
		return make_tuple(iPosition, (II ? 1 : 0), 0);
	} else if (HasCorpseII(iPosition))
		return make_tuple(iPosition, (corpseII ? 1 : 0), 0);
	return make_tuple(iPosition, 0, 0);
}

pair<BOOL, int> GridView::PutItem(int index, int x, int y, int width, int height, EEquip equip){
	ASSERT(0 <= index);
	ASSERT(0 <= x && 0 <= y);
	ASSERT(0 < width && 0 < height);
	if (!CanEquip(equip))
		return make_pair(FALSE, -1);	//不能穿戴在此位置
	if (IsGrid()) {
		if (x + width > iCol || y + height > iRow)
			return make_pair(FALSE, -1);	//物品在网格外面
		int exist = -1;
		for (int i = 0; i < width; ++i)
			for (int j = 0; j < height; ++j) {
				const int e = ItemIndex(x + i, y + j);
				if (e >= 0 && exist >= 0 && e != exist)
					return make_pair(FALSE, -1);	//网格里有不止一个物品
				exist = max(exist, e);
			}
		if (exist >= 0)
			return make_pair(FALSE, exist);	//被一个物品占用了
		ItemIndex(index, x, y, width, height);
	} else {
		const int e = ItemIndex(x, y);
		if (e >= 0)
			return make_pair(FALSE, e);	//被一个物品占用了
		ItemIndex(index, x, y);
	}
	return make_pair(TRUE, -1);
}

void GridView::Reset() {
	fill(vItemIndex.begin(), vItemIndex.end(), -1);
}

// CDlgCharItems 对话框

IMPLEMENT_DYNAMIC(CDlgCharItems, CCharacterDialogBase)

CDlgCharItems::CDlgCharItems(CWnd* pParent /*=NULL*/)
    : CCharacterDialogBase(CDlgCharItems::IDD, pParent)
{
	//鼠标
	m_hCursor = ::LoadCursor(0, IDC_ARROW);
	//初始化所有网格
	for(int i = STASH;i < POSITION_END;++i)
		m_vGridView.emplace_back(EPosition(i));
	//屏蔽尸体部位
	for (int i = CORPSE_HEAD; i < CORPSE_END; ++i)
		m_vGridView[i].Enable(m_bHasCorpse);
	//屏蔽雇佣兵部位
	for (int i = MERCENARY_HEAD; i < MERCENARY_END; ++i)
		m_vGridView[i].Enable(m_bHasMercenary);
	//选择箱子大小
	SetD2R(TRUE);
}

void CDlgCharItems::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_x, m_pMouse.x);
	DDX_Text(pDX, IDC_STATIC_y, m_pMouse.y);
	DDX_Check(pDX, IDC_CHECK2, m_bNotShowItemInfoDlg);
	DDX_Control(pDX, IDC_SLIDER1, m_scTrasparent);
	DDX_Check(pDX, IDC_CHECK1, m_bSecondHand);
	DDX_Check(pDX, IDC_CHECK_Corpse, m_bHasCorpse);
	DDX_Check(pDX, IDC_CHECK_Mercenary, m_bHasMercenary);
	DDX_Text(pDX, IDC_STATIC_Sockets, m_sText[0]);
	DDX_Text(pDX, IDC_STATIC_Golem, m_sText[1]);
	DDX_Text(pDX, IDC_STATIC_Cube, m_sText[2]);
	DDX_Text(pDX, IDC_STATIC_Belt, m_sText[3]);
	DDX_Text(pDX, IDC_CHECK_Corpse, m_sText[4]);
	DDX_Text(pDX, IDC_CHECK_Mercenary, m_sText[5]);
	DDX_Text(pDX, IDC_STATIC_MERC_NAME, m_sText[6]);
	DDX_Text(pDX, IDC_STATIC_MERC_TYPE, m_sText[7]);
	DDX_Text(pDX, IDC_STATIC_MERC_EXP, m_sText[8]);
	DDX_Text(pDX, IDC_CHECK3, m_sText[9]);
	DDX_Text(pDX, IDC_STATIC_TIP, m_sText[10]);
	DDX_Control(pDX, IDC_COMBO_MERC_NAME, m_cbMercName);
	DDX_Control(pDX, IDC_COMBO_MERC_TYPE, m_cbMercType);
	DDX_Control(pDX, IDC_EDIT_MERC_EXP, m_edMercExp);
	DDX_Control(pDX, IDC_CHECK4, m_chCorpseSecondHand);
	DDX_Control(pDX, IDC_CHECK3, m_chMercDead);
	DDX_Control(pDX, IDC_LIST_RECYCLE, m_lstRecycle);
}

BEGIN_MESSAGE_MAP(CDlgCharItems, CDialog)
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_CHECK2, &CDlgCharItems::OnBnClickedCheck2)
    ON_BN_CLICKED(IDC_CHECK1, &CDlgCharItems::OnChangeHand)
    ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_CHECK4, &CDlgCharItems::OnChangeCorpseHand)
	ON_BN_CLICKED(IDC_CHECK_Corpse, &CDlgCharItems::OnChangeCorpse)
	ON_BN_CLICKED(IDC_CHECK_Mercenary, &CDlgCharItems::OnChangeMercenary)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ITEM_IMPORT, &CDlgCharItems::OnItemImport)
	ON_COMMAND(ID_ITEM_EXPORT, &CDlgCharItems::OnItemExport)
	ON_COMMAND(ID_ITEM_COPY, &CDlgCharItems::OnItemCopy)
	ON_COMMAND(ID_ITEM_PASTE, &CDlgCharItems::OnItemPaste)
	ON_COMMAND(ID_ITEM_MODIFY, &CDlgCharItems::OnItemModify)
	ON_COMMAND(ID_ITEM_NEW, &CDlgCharItems::OnItemNew)
	ON_COMMAND(ID_ITEM_NEW_FROM, &CDlgCharItems::OnItemNewFrom)
	ON_COMMAND(ID_ITEM_REMOVE, &CDlgCharItems::OnItemRemove)
	ON_WM_MENUSELECT()
	ON_CBN_SELCHANGE(IDC_COMBO_MERC_TYPE, &CDlgCharItems::OnCbnSelchangeComboMercType)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RECYCLE, &CDlgCharItems::OnNMClickListRecycle)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RECYCLE, &CDlgCharItems::OnNMDblclkListRecycle)
END_MESSAGE_MAP()

void CDlgCharItems::UpdateUI(const CD2S_Struct & character) {
	ResetAll();
	m_bHasCharacter = TRUE;
	SetD2R(character.isD2R());
	//Character items
	for (auto & item : character.ItemList.vItems) 
		AddItemInGrid(item, 0);
	//Corpse items
	if (character.HasCorpse()) {
		if (!m_bHasCorpse)
			OnChangeCorpse();
		for (auto & item : character.stCorpse.pCorpseData->stItems.vItems)
			AddItemInGrid(item, 1);
	}
	//Mercenary
	if (character.HasMercenary()) {
		if (!m_bHasMercenary)
			OnChangeMercenary();
		ASSERT(character.wMercType < m_cbMercType.GetCount());
		m_cbMercType.SetCurSel(character.wMercType);
		OnCbnSelchangeComboMercType();
		ASSERT(character.wMercName < m_cbMercName.GetCount());
		m_cbMercName.SetCurSel(character.wMercName);
		m_edMercExp.SetWindowText(CSFormat(_T("%d"), character.dwMercExp));
		m_chMercDead.SetCheck(character.bMercDead);
		ASSERT(character.stMercenary.stItems.exist());
		for (auto & item : character.stMercenary.stItems->vItems)
			AddItemInGrid(item, 2);
	}
	//Golem
	if (character.stGolem.pItem.exist())
		AddItemInGrid(*character.stGolem.pItem, 3);
	UpdateData(FALSE);
	Invalidate();
}

BOOL CDlgCharItems::GatherData(CD2S_Struct & character) {
	UpdateData(TRUE);
	//validate
	if (m_bHasMercenary) {
		if (m_cbMercType.GetCurSel() < 0) {
			MessageBox(::theApp.MsgBoxInfo(46), ::theApp.MsgError(), MB_ICONERROR);
			return FALSE;
		}
		if (m_cbMercName.GetCurSel() < (m_bIsD2R ? 0 : 1)) {
			MessageBox(::theApp.MsgBoxInfo(47), ::theApp.MsgError(), MB_ICONERROR);
			return FALSE;
		}
	}
	//gather all items
	CItemList normal, corpse, merc;
	MayExist<CD2Item> golem;
	for (auto & view : m_vItemViews) {
		if (IsInRecycle(view.iPosition) || IsInSocket(view.iPosition))
			continue;
		auto & item = view.UpdateItem(m_vItemViews);
		if (::IsCorpse(view.iPosition)) {	// corpse items
			corpse.vItems.push_back(item);
		} else if (::IsMercenary(view.iPosition)) {	//mercenary items
			merc.vItems.push_back(item);
		} else if(::IsGolem(view.iPosition)) {
			ASSERT(!golem.exist());
			golem.ensure() = item;
		} else
			normal.vItems.push_back(item);
	}
	character.ItemList.SwapItems(normal);
	//corpse
	if (m_bHasCorpse) {
		character.stCorpse.wCount = 1;
		character.stCorpse.pCorpseData.ensure().stItems.SwapItems(corpse);
	} else {
		character.stCorpse.wCount = 0;
		character.stCorpse.pCorpseData.reset();
	}
	//mercenary
	if (m_bHasMercenary) {
		if(!character.dwMercControl)
			character.dwMercControl = rand() | 0x10;
		character.wMercType = m_cbMercType.GetCurSel();
		character.wMercName = m_cbMercName.GetCurSel();
		character.dwMercExp = intOf(m_edMercExp);
		character.bMercDead = m_chMercDead.GetCheck();
		character.stMercenary.stItems.ensure().SwapItems(merc);
	} else {
		character.dwMercControl = 0;
		character.wMercType = 0;
		character.wMercName = 0;
		character.dwMercExp = 0;
		character.bMercDead = FALSE;
		character.stMercenary.stItems.reset();
	}
	//golem
	character.stGolem.bHasGolem = golem.exist();
	character.stGolem.pItem.swap(golem);
	return TRUE;
}

void CDlgCharItems::AddItemInGrid(const CD2Item & item, int body) {
	EEquip equip = ItemToEquip(item);
	auto t = ItemToPosition(item.iLocation, item.iPosition, item.iColumn, item.iRow, item.iStoredIn, body, m_bIsD2R);
	EPosition pos = get<0>(t);
	const int x = get<1>(t), y = get<2>(t);
	const int index = m_vItemViews.size();
	 m_vItemViews.emplace_back(item, equip, pos, x, y);
	if (::IsInMouse(pos)) {
		ASSERT(m_iPickedItemIndex < 0);
		m_iPickedItemIndex = index;
		m_hCursor = CreateAlphaCursor(m_vItemViews[index]);
	}else if (!m_vGridView[pos].PutItem(index, x, y, m_vItemViews[index].iGridWidth, m_vItemViews[index].iGridHeight, equip).first)
		ASSERT(FALSE && _T("Cannot put item in grid"));
	//Sockets & Gems
	if (!item.aGemItems.empty()) {
		ASSERT(item.aGemItems.size() <= m_vItemViews[index].vGemItems.size());
		for (auto & gem : item.aGemItems) {
			ASSERT(0 <= gem.iColumn && gem.iColumn < int(m_vItemViews[index].vGemItems.size()));
			ASSERT(m_vItemViews[index].vGemItems[gem.iColumn] < 0);
			m_vItemViews[index].vGemItems[gem.iColumn] = m_vItemViews.size();
			m_vItemViews.emplace_back(gem, ItemToEquip(gem), IN_SOCKET, gem.iColumn, 0);
		}
	}
}

CPoint CDlgCharItems::GetItemPositionXY(const CItemView & view) const {
	ASSERT(view.iPosition < POSITION_END);
	return m_vGridView[view.iPosition].IndexToXY(view.iGridX, view.iGridY, view.iGridWidth, view.iGridHeight);
}

CItemView & CDlgCharItems::SelectedParentItemView() {
	ASSERT(0 <= m_iSelectedItemIndex && m_iSelectedItemIndex < int(m_vItemViews.size()));
	return m_vItemViews[m_iSelectedItemIndex];
}

CItemView & CDlgCharItems::SelectedItemView() {
	if (m_iSelectedSocketIndex < 0)
		return SelectedParentItemView();
	ASSERT(m_iSelectedSocketIndex < int(m_vItemViews.size()));
	return m_vItemViews[m_iSelectedSocketIndex];
}

CItemView & CDlgCharItems::PickedItemView() {
	ASSERT(0 <= m_iPickedItemIndex && m_iPickedItemIndex < int(m_vItemViews.size()));
	return m_vItemViews[m_iPickedItemIndex];
}

//画一个网格或矩形
static void DrawGrid(CPaintDC & dc, const CRect & rect, int intervalX = 0, int intervalY = 0)
{
	if (intervalX == 0)
		intervalX = rect.Width();
	if (intervalY == 0)
		intervalY = rect.Height();
	for (int x = rect.left; x <= rect.right; x += intervalX) {
		dc.MoveTo(x, rect.top);
		dc.LineTo(x, rect.bottom);
	}
	for (int y = rect.top; y <= rect.bottom; y += intervalY) {
		dc.MoveTo(rect.left, y);
		dc.LineTo(rect.right, y);
	}
}

void CDlgCharItems::DrawGrids(CPaintDC & dc)
{
    CPen pen(PS_SOLID,1,RGB(0,200,100));
    CPen * pOld = dc.SelectObject(&pen);
	for (const auto & g : m_vGridView) {
		if (!g.Visible())continue;
		if (g.IsGrid())
			::DrawGrid(dc, g.Rect, GRID_WIDTH, GRID_WIDTH);
		else
			::DrawGrid(dc, g.Rect);
	}
	dc.SelectObject(pOld);
}

void CDlgCharItems::DrawItemXY(CPaintDC & dc, CPoint pos, const CItemView & view) const
{
	CBitmap bmp;
	bmp.LoadBitmap(view.nPicRes);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap* pOld = memDC.SelectObject(&bmp);
	auto sz = view.ViewSize();
	dc.BitBlt(pos.x, pos.y, sz.cx,sz.cy, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOld);
}

void CDlgCharItems::DrawAllItemsInGrid(CPaintDC & dc) const
{
	CRect selectedGrid(0, 0, 0, 0), selectedSocket(0, 0, 0, 0);
	for (size_t i = 0; i < m_vItemViews.size(); ++i) {
		auto & view = m_vItemViews[i];
		if (::IsInRecycle(view.iPosition))
			continue;	//被删除
		if (::IsInMouse(view.iPosition))
			continue;	//被鼠标拿起
		if (::IsInSocket(view.iPosition))
			continue;	//镶嵌在孔里
		//在左右手上，分I, II显示不同物品，包括尸体
		if ((RIGHT_HAND == view.iPosition || LEFT_HAND == view.iPosition)
			&& view.iGridX != (m_bSecondHand ? 1 : 0))
			continue;
		if ((CORPSE_RIGHT_HAND == view.iPosition || CORPSE_LEFT_HAND == view.iPosition)
			&& view.iGridX != (m_chCorpseSecondHand.GetCheck() ? 1 : 0))
			continue;
		auto pos = GetItemPositionXY(view);
		DrawItemXY(dc, pos, view);
		if (i == m_iSelectedItemIndex) {	//选中的物品
			selectedGrid = CRect(pos, view.ViewSize());
			//Draw gems in sockets
			for (int g : view.vGemItems) {
				if (g < 0)
					continue;
				auto & gemView = m_vItemViews[g];
				pos = GetItemPositionXY(gemView);
				DrawItemXY(dc, pos, gemView);
				if (g == m_iSelectedSocketIndex)	//选中的宝石
					selectedSocket = CRect(pos, gemView.ViewSize());
			}
		}
	}
	// 高亮选中的宝石
	const int PEN_WIDTH = 2;
	if (selectedSocket.top) {
		CPen pen(PS_SOLID, PEN_WIDTH, RGB(255, 0, 0));
		CPen * pOld = dc.SelectObject(&pen);
		DrawGrid(dc, selectedSocket);
		dc.SelectObject(pOld);
	}
	// 高亮选中的物品
	if (selectedGrid.top) {
		CPen pen(PS_SOLID, PEN_WIDTH, selectedSocket.top ? RGB(220, 50, 100) : RGB(255, 0, 0));
		CPen * pOld = dc.SelectObject(&pen);
		DrawGrid(dc, selectedGrid);
		dc.SelectObject(pOld);
	}
}

void CDlgCharItems::SetD2R(BOOL v) {
	m_bIsD2R = v;
	//选择箱子大小
	m_vGridView[STASH].Enable(!v);
	m_vGridView[STASH].Visible(!v);
	m_vGridView[STASH_D2R].Enable(v);
	m_vGridView[STASH_D2R].Visible(v);
}


tuple<int, int, int> CDlgCharItems::HitTestPosition(CPoint pos, int col, int row) const {
	for (auto & g : m_vGridView)
		if (g.Enable() && g.Rect.PtInRect(pos))
			return g.XYToPositionIndex(pos, m_bSecondHand, m_chCorpseSecondHand.GetCheck(), col, row);
	return make_tuple(-1, -1, -1);
}

void CDlgCharItems::ShowItemInfoDlg(const CD2Item * pItem, int x, int gems){
    if(!m_bNotShowItemInfoDlg && pItem && (!m_pDlgItemInfo || pItem != m_pDlgItemInfo->GetItemPtr())){
        if(!m_pDlgItemInfo){
			m_pDlgItemInfo = make_unique<CDlgSuspend>(this, m_scTrasparent.GetPos());
            m_pDlgItemInfo->Create(CDlgSuspend::IDD,NULL);
        }
        m_pDlgItemInfo->GetItemInfo(pItem, gems);
        CRect rect,rect1;
        m_pDlgItemInfo->GetWindowRect(&rect);
        GetWindowRect(&rect1);
		//Adjust Suspend Window position
		if (x < INFO_WINDOW_RIGHT - GRID_WIDTH * 2)
			rect1.left += INFO_WINDOW_RIGHT;
		else
			rect1.left += INFO_WINDOW_LEFT;
		m_pDlgItemInfo->MoveWindow(rect1.left, rect1.top, rect.Width(), rect.Height(), TRUE);
        m_pDlgItemInfo->ShowWindow(SW_SHOWNOACTIVATE); //显示对话框
        m_pDlgItemInfo->Invalidate();
    }else if(!pItem && m_pDlgItemInfo)
        m_pDlgItemInfo.reset();
}

void CDlgCharItems::ResetAll()
{
	for (auto & grid : m_vGridView)
		grid.Reset();

	m_vItemViews.clear();
	m_bHasCorpse = FALSE;
	m_bSecondHand = FALSE;
	m_chCorpseSecondHand.SetCheck(FALSE);
	m_chCorpseSecondHand.EnableWindow(FALSE);

	m_pDlgItemInfo.reset();

	m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
	if (m_iPickedItemIndex >= 0) {
		::DestroyIcon(m_hCursor);
		m_hCursor = ::LoadCursor(0, IDC_ARROW);
		m_iPickedItemIndex = -1;
	}

	m_bClickOnItem = FALSE;
	m_iCopiedItemIndex = -1;

	m_bHasMercenary = FALSE;
	m_cbMercType.SetCurSel(-1);
	m_cbMercName.SetCurSel(-1);
	m_edMercExp.SetWindowText(_T(""));
	m_chMercDead.SetCheck(FALSE);
	m_iMercNameGroup = -1;

	m_lstRecycle.DeleteAllItems();

	m_bHasCharacter = FALSE;

	UpdateData(FALSE);
	Invalidate();
}

static void loadTextMercCB(CComboBox & cb, int sz, function<CString (int i)> name) {
	int sel = cb.GetCurSel();			//保存当前选中项
	cb.ResetContent();					//删除旧项
	for (int i = 0; i < sz; ++i)		//更新文字
		cb.InsertString(i, name(i));
	if (sz > 0)							//重新设置选择项
		cb.SetCurSel(min(sel, sz - 1));
}

void CDlgCharItems::LoadText(void)
{
	int index = 0;
    for(auto & text : m_sText)
		text = ::theApp.CharItemsUI(index++);
	//Recycle header text
	LVCOLUMN c;
	c.mask = LVCF_TEXT;
	CString text = ::theApp.CharItemsUI(index++);
	c.pszText = text.GetBuffer();
	m_lstRecycle.SetColumn(0, &c);
	//Recycle items' name
	for (int i = 0; i < m_lstRecycle.GetItemCount(); ++i) {
		const UINT idx = m_lstRecycle.GetItemData(i);
		ASSERT(idx < m_vItemViews.size());
		m_lstRecycle.SetItemText(i, 0, m_vItemViews[idx].Item.ItemName());
	}
	//Mercenary ComboBoxes
	loadTextMercCB(m_cbMercType, ::theApp.MercenaryTypeNameSize(), [](int i) {return ::theApp.MercenaryTypeName(i); });
	m_iMercNameGroup = -1;	//force reloading merc name list
	OnCbnSelchangeComboMercType();
	UpdateData(FALSE);
}

void CDlgCharItems::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) {
	CCharacterDialogBase::OnMenuSelect(nItemID, nFlags, hSysMenu);
	CFrameWnd & frame = *GetParentFrame();
	switch (nItemID) {	//Tips message
		case ID_ITEM_IMPORT:	frame.SetMessageText(::theApp.MenuPrompt(9)); break;
		case ID_ITEM_EXPORT:	frame.SetMessageText(::theApp.MenuPrompt(10)); break;
		case ID_ITEM_COPY:		frame.SetMessageText(::theApp.MenuPrompt(11)); break;
		case ID_ITEM_PASTE:		frame.SetMessageText(::theApp.MenuPrompt(12)); break;
		case ID_ITEM_MODIFY:	frame.SetMessageText(::theApp.MenuPrompt(13)); break;
		case ID_ITEM_NEW:		frame.SetMessageText(::theApp.MenuPrompt(14)); break;
		case ID_ITEM_NEW_FROM:	frame.SetMessageText(::theApp.MenuPrompt(15)); break;
		case ID_ITEM_REMOVE:	frame.SetMessageText(::theApp.MenuPrompt(16)); break;
	}
}

// CDlgCharItems 消息处理程序

void CDlgCharItems::OnPaint()
{
    CPaintDC dc(this);
    DrawGrids(dc);
    DrawAllItemsInGrid(dc);	//画网格里的物品
}

void CDlgCharItems::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_iPickedItemIndex < 0) {	//未拿起物品
		const CD2Item * item = 0;
		int gems = 0;
		auto t = HitTestPosition(point);
		const int pos = get<0>(t), x = get<1>(t), y = get<2>(t);
		if (0 <= pos) {		//在有效网格里
			auto & grid = m_vGridView[pos];
			int index = grid.ItemIndex(x, y);
			if (0 <= index) {	//有物品
				ASSERT(index < int(m_vItemViews.size()));
				item = &m_vItemViews[index].Item;
				gems = m_vItemViews[index].GemCount();
			}
		}
		ShowItemInfoDlg(item, point.x, gems);
	}
	m_pMouse = point;
	UpdateData(FALSE);
	CCharacterDialogBase::OnMouseMove(nFlags, point);
}

pair<BOOL, int> CDlgCharItems::PutItemInGrid(EPosition pos, int x, int y) {
	ASSERT(pos < POSITION_END);
	auto & view = PickedItemView();
	auto & grid = m_vGridView[pos];
	const auto r = grid.PutItem(m_iPickedItemIndex, x, y, view.iGridWidth, view.iGridHeight, view.iEquip);
	if (r.first) {	//放入成功
		view.iPosition = pos;
		view.iGridX = x;
		view.iGridY = y;
		::DestroyIcon(m_hCursor);
		m_hCursor = ::LoadCursor(0, IDC_ARROW);
		//没有修改m_iPickedItemIndex
		return make_pair(TRUE, -1);
	} else if (r.second >= 0) {	//有一个物品占用位置
		ASSERT(r.second < int(m_vItemViews.size()));
		auto & v = m_vItemViews[r.second];
		grid.ItemIndex(-1, v.iGridX, v.iGridY, v.iGridWidth, v.iGridHeight);	//腾出位置
		const auto r2 = grid.PutItem(m_iPickedItemIndex, x, y, view.iGridWidth, view.iGridHeight, view.iEquip);	//再放一次
		ASSERT(r2.first);
		view.iPosition = pos;
		view.iGridX = x;
		view.iGridY = y;
		::DestroyIcon(m_hCursor);
		v.iPosition = IN_MOUSE;	//拿起新物品
		m_hCursor = CreateAlphaCursor(v);	//设置鼠标为新物品图片
		//没有修改m_iPickedItemIndex
		return make_pair(TRUE, r.second);
	}
	return make_pair(FALSE, -1);
}

void CDlgCharItems::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_iPickedItemIndex < 0) {	//拿起物品
		auto t = HitTestPosition(point);
		const int pos = get<0>(t), x = get<1>(t), y = get<2>(t);
		if (pos >= 0) {		//在网格范围内
			auto & grid = m_vGridView[pos];
			if (grid.Enable()) {
				int index = grid.ItemIndex(x, y);
				if (index >= 0) {	//点中了物品
					ASSERT(index < int(m_vItemViews.size()));
					if (grid.IsSockets()) {	//拿起镶嵌的宝石
						auto & gems = SelectedParentItemView().vGemItems;
						ASSERT(x < int(gems.size()));
						gems[x] = -1;
					}
					auto & view = m_vItemViews[index];
					m_iPickedItemIndex = index;
					m_hCursor = CreateAlphaCursor(view);  //设置鼠标为物品图片
					grid.ItemIndex(-1, view.iGridX, view.iGridY, view.iGridWidth, view.iGridHeight);
					view.iPosition = IN_MOUSE;
					ShowItemInfoDlg(0, 0, 0);
					Invalidate();
				}
			}
		}
	} else {	//放下物品
		auto & view = PickedItemView();
		auto t = HitTestPosition(point, view.iGridWidth, view.iGridHeight);
		const int pos = get<0>(t), x = get<1>(t), y = get<2>(t);
		if (pos >= 0) {		//在网格范围内
			auto & grid = m_vGridView[pos];
			if (grid.Enable()) {
				if (grid.IsSockets()) {	//给物品镶嵌宝石
					if (0 <= m_iSelectedItemIndex	//有选中物品
						&& x < int(SelectedParentItemView().vGemItems.size())) {	//有足够的孔数
						const auto r = PutItemInGrid(EPosition(pos), x, y);
						if (r.first) {	//镶嵌成功
							SelectedParentItemView().vGemItems[x] = m_iPickedItemIndex;
							m_iPickedItemIndex = r.second;
							Invalidate();
						}
					}
				} else{
					const auto r = PutItemInGrid(EPosition(pos), x, y);
					if (r.first) {	//放下成功
						if (m_iSelectedSocketIndex == m_iPickedItemIndex)	//将镶嵌的宝石抠出来了
							m_iSelectedSocketIndex = -1;
						m_iPickedItemIndex = r.second;
						Invalidate();
					}
				}
			}
		}
	}
	CCharacterDialogBase::OnLButtonDown(nFlags, point);
}

void CDlgCharItems::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bClickOnItem = FALSE;
	if (m_iPickedItemIndex < 0) {	//未拿起物品
		auto t = HitTestPosition(point);
		const int pos = get<0>(t), x = get<1>(t), y = get<2>(t);
		if (pos >= 0) {		//在网格范围内
			auto & grid = m_vGridView[pos];
			if (grid.Enable()) {
				int index = grid.ItemIndex(x, y);
				if (index >= 0) {	//点中了物品
					m_bClickOnItem = TRUE;
					if (grid.IsSockets()) {	//是镶嵌的宝石
						if (index != m_iSelectedSocketIndex) {
							m_iSelectedSocketIndex = index;
							Invalidate();
						}
					} else if (index != m_iSelectedItemIndex || 0 <= m_iSelectedSocketIndex) {	//其他物品
						if (index != m_iSelectedItemIndex) {
							const auto & view = m_vItemViews[index];
							for (int i = 0; i < PositionToCol(IN_SOCKET); ++i)
								m_vGridView[IN_SOCKET].ItemIndex((i < int(view.vGemItems.size()) ? view.vGemItems[i] : -1), i, 0);
						}
						m_iSelectedItemIndex = index;
						m_iSelectedSocketIndex = -1;
						Invalidate();
					}
				}
			}
		}
	}
    CCharacterDialogBase::OnRButtonUp(nFlags, point);
}

void CDlgCharItems::OnContextMenu(CWnd* /*pWnd*/, CPoint point) {
	if (m_bHasCharacter && m_iPickedItemIndex < 0) {	//未拿起物品
		CRect rect;
		m_lstRecycle.GetWindowRect(&rect);
		if (rect.PtInRect(point))
			return;	//Inside of Recycle list
		/*	创建弹出菜单：
				Import
				Export
				---
				Copy
				Paste
				---
				Modify
				Remove
		*/
		CMenu menu;
		menu.CreatePopupMenu();
		ASSERT(::IsMenu(menu.m_hMenu));
		//Texts
		menu.AppendMenu(MF_STRING, ID_ITEM_IMPORT, ::theApp.CharItemPopupMenu(0));
		menu.AppendMenu(MF_STRING, ID_ITEM_EXPORT, ::theApp.CharItemPopupMenu(1));
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, ID_ITEM_MODIFY, ::theApp.CharItemPopupMenu(4));
		menu.AppendMenu(MF_STRING, ID_ITEM_COPY, ::theApp.CharItemPopupMenu(2));
		menu.AppendMenu(MF_STRING, ID_ITEM_PASTE, ::theApp.CharItemPopupMenu(3));
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, ID_ITEM_NEW, ::theApp.CharItemPopupMenu(5));
		menu.AppendMenu(MF_STRING, ID_ITEM_NEW_FROM, ::theApp.CharItemPopupMenu(6));
		menu.AppendMenu(MF_STRING, ID_ITEM_REMOVE, ::theApp.CharItemPopupMenu(7));
		//Appearance
		menu.EnableMenuItem(ID_ITEM_IMPORT, (m_bClickOnItem ? MF_DISABLED : MF_ENABLED));
		menu.EnableMenuItem(ID_ITEM_EXPORT, (m_bClickOnItem ? MF_ENABLED : MF_DISABLED));
		menu.EnableMenuItem(ID_ITEM_COPY, (m_bClickOnItem && !SelectedItemView().Item.IsBox() ? MF_ENABLED : MF_DISABLED));
		menu.EnableMenuItem(ID_ITEM_PASTE, (0 <= m_iCopiedItemIndex ? MF_ENABLED : MF_DISABLED));
		menu.EnableMenuItem(ID_ITEM_MODIFY, (m_bClickOnItem && SelectedItemView().Item.IsEditable() ? MF_ENABLED : MF_DISABLED));
		menu.EnableMenuItem(ID_ITEM_NEW, (m_bClickOnItem ? MF_DISABLED : MF_ENABLED));
		menu.EnableMenuItem(ID_ITEM_NEW_FROM, (m_bClickOnItem ? MF_ENABLED : MF_DISABLED));
		menu.EnableMenuItem(ID_ITEM_REMOVE, (m_bClickOnItem ? MF_ENABLED : MF_DISABLED));
		m_bClickOnItem = FALSE;

		menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

BOOL CDlgCharItems::OnInitDialog()
{
    CCharacterDialogBase::OnInitDialog();
	m_lstRecycle.InsertColumn(0, _T(""), LVCFMT_LEFT, 140);
	m_scTrasparent.SetRange(0, 255);
	m_scTrasparent.SetPos(200);
    return TRUE;
}

void CDlgCharItems::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CCharacterDialogBase::OnShowWindow(bShow, nStatus);
    //if(!bShow)			//在少数情况下，会出现隐藏物品属性窗口时悬浮窗还在的情况
    //    ShowItemInfoDlg(0);
}

void CDlgCharItems::OnBnClickedCheck2()
{
    m_bNotShowItemInfoDlg = !m_bNotShowItemInfoDlg;
}

void CDlgCharItems::OnChangeHand()
{
	if (!m_bHasCharacter) {	//Don't change if there is no character
		UpdateData(FALSE);
		return;
	}
	//调整选中的物品
	const int i1 = m_vGridView[RIGHT_HAND].ItemIndex((m_bSecondHand ? 1 : 0), 0);
	const int i2 = m_vGridView[LEFT_HAND].ItemIndex((m_bSecondHand ? 1 : 0), 0);
	if (i1 == m_iSelectedItemIndex || i2 == m_iSelectedItemIndex) {
		m_vGridView[IN_SOCKET].Reset();
		m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
	}
	m_bSecondHand = !m_bSecondHand;
	Invalidate();
 }

void CDlgCharItems::OnChangeCorpseHand() {
	const BOOL s = m_chCorpseSecondHand.GetCheck();	//改变后的值
	//调整选中的物品
	const int i1 = m_vGridView[CORPSE_RIGHT_HAND].ItemIndex((s ? 0 : 1), 0);
	const int i2 = m_vGridView[CORPSE_LEFT_HAND].ItemIndex((s ? 0 : 1), 0);
	if (i1 == m_iSelectedItemIndex || i2 == m_iSelectedItemIndex) {
		m_vGridView[IN_SOCKET].Reset();
		m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
	}
	Invalidate();
}

void CDlgCharItems::OnChangeCorpse() {
	if (!m_bHasCharacter) {	//Don't change if there is no character
		UpdateData(FALSE);
		return;
	}
	m_bHasCorpse = !m_bHasCorpse;
	for (int i = CORPSE_HEAD; i < CORPSE_END; ++i)
		m_vGridView[i].Enable(m_bHasCorpse);
	m_chCorpseSecondHand.EnableWindow(m_bHasCorpse);
	//de-select item on corpse
	if (!m_bHasCorpse && 0 <= m_iSelectedItemIndex
			&& IsCorpse(SelectedParentItemView().iPosition)) {
		m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
		m_vGridView[IN_SOCKET].Reset();
		Invalidate();
	}
}

void CDlgCharItems::OnChangeMercenary() {
	if (!m_bHasCharacter) {	//Don't change if there is no character
		UpdateData(FALSE);
		return;
	}
	m_bHasMercenary = !m_bHasMercenary;
	for (int i = MERCENARY_HEAD; i < MERCENARY_END; ++i)
		m_vGridView[i].Enable(m_bHasMercenary);
	m_cbMercName.EnableWindow(m_bHasMercenary);
	m_cbMercType.EnableWindow(m_bHasMercenary);
	m_edMercExp.EnableWindow(m_bHasMercenary);
	m_chMercDead.EnableWindow(m_bHasMercenary);
	//de-select item on mercenary
	if (!m_bHasMercenary && 0 <= m_iSelectedItemIndex
			&& IsMercenary(SelectedParentItemView().iPosition)) {
		m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
		m_vGridView[IN_SOCKET].Reset();
		Invalidate();
	}
}

static HCURSOR CreateCursorFromBitmap(int picIndex, CSize sz) {
	// Load bitmap
	CBitmap bmp;
	if (!bmp.LoadBitmap(picIndex))
		ASSERT(FALSE);
	// Create an empty mask bitmap.
	CBitmap monobmp;
	monobmp.CreateBitmap(sz.cx, sz.cy, 1, 1, NULL);
	// Icon header
	ICONINFO ii;
	ii.fIcon = FALSE;  // Change fIcon to TRUE to create an alpha icon
	ii.xHotspot = sz.cx >> 1;
	ii.yHotspot = sz.cy >> 1;
	ii.hbmMask = monobmp;
	ii.hbmColor = bmp;
	// Create the alpha cursor with the alpha DIB section, and return it.
	return ::CreateIconIndirect(&ii);
}

HCURSOR CDlgCharItems::CreateAlphaCursor(const CItemView & itemView) {
	return ::CreateCursorFromBitmap(itemView.nPicRes, itemView.ViewSize());
}

BOOL CDlgCharItems::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (nHitTest == HTCLIENT) {
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CCharacterDialogBase::OnSetCursor(pWnd, nHitTest, message);
}

void CDlgCharItems::OnItemImport() {
	CFileDialog open(TRUE, _T("d2i"), 0, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Diablo II Item(*.d2i)|*.d2i|All File(*.*)|*.*||"));
	if (open.DoModal() == IDOK) {
		CD2Item item;
		if (item.ReadFile(CFile(open.GetPathName(), CFile::modeRead))) {
			item.iLocation = 4;	//设置物品被鼠标拿起
			AddItemInGrid(item, 0);
		}
	}
}

void CDlgCharItems::OnItemExport() {
	//update item
	auto & view = SelectedItemView();
	auto & item = view.UpdateItem(m_vItemViews);
	//serialize to file
	CFileDialog save_item(FALSE, 0, view.ItemName() + _T(".d2i"), OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST, _T("Diablo II Item(*.d2i)|*.d2i|All File(*.*)|*.*||"));
	if (save_item.DoModal() == IDOK)
		item.WriteFile(CFile(save_item.GetPathName(), CFile::modeCreate | CFile::modeWrite));
}

void CDlgCharItems::OnItemCopy() {
	m_iCopiedItemIndex = (0 <= m_iSelectedSocketIndex ? m_iSelectedSocketIndex : m_iSelectedItemIndex);
	OnItemPaste();
}

void CDlgCharItems::OnItemPaste() {
	ASSERT(0 <= m_iCopiedItemIndex && m_iCopiedItemIndex < int(m_vItemViews.size()));
	CD2Item item(m_vItemViews[m_iCopiedItemIndex].Item);
	item.iLocation = 4;	//设置物品被鼠标拿起
	AddItemInGrid(item, 0);
}

void CDlgCharItems::OnItemModify() {
	ShowItemInfoDlg(0, 0, 0);	//Hide suspend window
	auto & view = SelectedItemView();
	view.UpdateItem(m_vItemViews);
	CDlgFoundry dlg(view.Item, this);
	dlg.DoModal();
}

void CDlgCharItems::OnItemNew() {
	unique_ptr<CD2Item> item;
	CDlgNewItem dlg(item, this);
	dlg.DoModal();
	if (!item)
		return;
	item->iLocation = 4;	//设置物品被鼠标拿起
	AddItemInGrid(*item, 0);
}

void CDlgCharItems::OnItemNewFrom() {
	const auto & view = SelectedItemView();
	unique_ptr<CD2Item> item;
	CDlgNewItem dlg(item, this);
	dlg.DoModal();
	if (!item)
		return;
	//copy property list
	if (view.Item.HasPropertyList() && item->HasPropertyList())
		item->pItemInfo->pTpSpInfo->stPropertyList = view.Item.pItemInfo->pTpSpInfo->stPropertyList;
	item->iLocation = 4;	//设置物品被鼠标拿起
	AddItemInGrid(*item, 0);
}

void CDlgCharItems::OnItemRemove() {
	auto & view = SelectedItemView();
	ASSERT(view.iPosition < int(m_vGridView.size()));
	auto & grid = m_vGridView[view.iPosition];
	if (grid.IsSockets()) {	//如果删除镶嵌的宝石，先解除镶嵌关系
		auto & gems = SelectedParentItemView().vGemItems;
		ASSERT(0 <= view.iGridX && view.iGridX < int(gems.size()));
		gems[view.iGridX] = -1;
	} else
		m_vGridView[IN_SOCKET].Reset();	//一般物品，重置镶孔
	//移走物品
	grid.ItemIndex(-1, view.iGridX, view.iGridY, view.iGridWidth, view.iGridHeight);
	view.iPosition = IN_RECYCLE;
	//放入回收站
	const int i = m_lstRecycle.InsertItem(m_lstRecycle.GetItemCount(), view.Item.ItemName());
	m_lstRecycle.SetItemData(i, m_iSelectedItemIndex);
	//重置选中索引
	if (m_iSelectedSocketIndex >= 0)
		m_iSelectedSocketIndex = -1;
	else
		m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
	Invalidate();
}

static int mercNameGroup(int type) {
	if (type < 0)
		return - 1;
	const int INDEX[] = {5, 14, 23};
	return lower_bound(begin(INDEX), end(INDEX), type, less<int>()) - begin(INDEX);
}

void CDlgCharItems::OnCbnSelchangeComboMercType() {
	const int g = mercNameGroup(m_cbMercType.GetCurSel());
	if (m_iMercNameGroup == g)
		return;
	//change merc name list
	switch (g) {
		case 0:
			loadTextMercCB(m_cbMercName, ::theApp.MercenaryNameScoutSize(),
				[=](int i) {return !m_bIsD2R && !i ? _T("") : ::theApp.MercenaryScoutName(i); });
			break;
		case 1:
			loadTextMercCB(m_cbMercName, ::theApp.MercenaryNameMercSize(), 
				[=](int i) {return !m_bIsD2R && !i ? _T("") : ::theApp.MercenaryMercName(i); });
			break;
		case 2:
			loadTextMercCB(m_cbMercName, ::theApp.MercenaryNameSorcerorSize(),
				[=](int i) {return !m_bIsD2R && !i ? _T("") : ::theApp.MercenarySorcerorName(i); });
			break;
		case 3:
			loadTextMercCB(m_cbMercName, ::theApp.MercenaryNameBarbarianSize(), 
				[=](int i) {return !m_bIsD2R && !i ? _T("") : ::theApp.MercenaryBarbarianName(i); });
			break;
		default:loadTextMercCB(m_cbMercName, 0, [](int i) {return _T(""); });
	}
	m_iMercNameGroup = g;
	m_cbMercName.Invalidate();
}

void CDlgCharItems::OnNMClickListRecycle(NMHDR *pNMHDR, LRESULT *pResult) {
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (0 <= m_iPickedItemIndex) {	//拿起了物品，将物品放入回收站
		auto & view = PickedItemView();
		//放入回收站
		view.iPosition = IN_RECYCLE;
		const int i = m_lstRecycle.InsertItem(m_lstRecycle.GetItemCount(), view.Item.ItemName());
		m_lstRecycle.SetItemData(i, m_iPickedItemIndex);
		//重置鼠标
		::DestroyIcon(m_hCursor);
		m_hCursor = ::LoadCursor(0, IDC_ARROW);
		//reset selected item
		if (m_iPickedItemIndex == m_iSelectedSocketIndex)
			m_iSelectedSocketIndex = -1;
		else if (m_iPickedItemIndex == m_iSelectedItemIndex) {
			m_vGridView[IN_SOCKET].Reset();	//重置镶孔
			m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
		}
		//reset picked item
		m_iPickedItemIndex = -1;
	}
	*pResult = 0;
}

void CDlgCharItems::OnNMDblclkListRecycle(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	const int item = pNMItemActivate->iItem;
	if (item >= 0 && m_iPickedItemIndex < 0) {	//双击有效项并且未拿起物品
		const UINT idx = m_lstRecycle.GetItemData(item);
		ASSERT(idx < m_vItemViews.size());
		auto & view = m_vItemViews[idx];
		//resume gems
		for (int i : view.vGemItems) {
			if (i < 0)
				continue;
			ASSERT(i < int(m_vItemViews.size()));
			m_vItemViews[i].iPosition = IN_SOCKET;
		}
		//resume item to mouse
		view.iPosition = IN_MOUSE;
		m_hCursor = CreateAlphaCursor(view);  //设置鼠标为物品图片
		m_iPickedItemIndex = idx;
		//delelte item in recycle list
		m_lstRecycle.DeleteItem(item);
	}
	*pResult = 0;
}
