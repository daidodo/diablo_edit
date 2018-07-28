// DlgCharItems.cpp : 实现文件
//

#include "stdafx.h"

#include "Diablo Edit2.h"
#include "DlgCharItems.h"

using namespace std;

const int GRID_WIDTH = 30;		//每个网格的边长(像素)

//物品能装备的位置
enum EEquip {
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
};

static EEquip ItemToEquip(int iEquip) {
	ASSERT(0 <= iEquip && iEquip <= 10);
	return EEquip(1 << iEquip);
}

//所有网格位置
enum EPosition {
	STASH,				//箱子
	INVENTORY,			//口袋
	CUBE,				//方块
	IN_BELT,			//腰带里
	IN_SOCKET,			//镶嵌在孔里

	GRID_COUNT,			//网格类型位置数量

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

	IN_MOUSE,			//被鼠标拿起

	POSITION_END		//所有位置总数
};

//每个位置(EPosition)在UI的起始坐标(像素),列数,行数
//left,top,col,row
const WORD POSITION_RECT[POSITION_END][4] = {
	{10,5,6,8},		//箱子
	{10,255,10,4},	//口袋
	{320,255,3,4},	//方块
	{420,255,4,4},	//腰带里
	{70,385,6,1},	//孔
	{300,5,2,2},	//头
	{365,30,1,1},	//项链
	{300,70,2,3},	//身体
	{200,30,2,4},	//武器右
	{400,30,2,4},	//武器左
	{265,165,1,1},	//戒指右
	{365,165,1,1},	//戒指左
	{300,165,2,1},	//腰带		
	{400,155,2,2},	//鞋子
	{200,155,2,2},	//手套
	{480,30,2,4},	//被鼠标拿起
};

tuple<EPosition, int, int> ItemToPosition(int iLocation, int iPosition, int iColumn, int iRow, int iStoredIn) {
	int pos = -1, x = 0, y = 0;	//物品的位置(EPosition)和坐标
	switch (iLocation) {
		case 0:		//grid
			pos = (iStoredIn == 1 ? INVENTORY : (iStoredIn == 4 ? CUBE : (iStoredIn == 5 ? STASH : -1)));
			x = iColumn;
			y = iRow;
			break;
		case 1:		//equipped
			pos = iPosition;
			if (0 < pos && pos <= 10)
				pos += GRID_COUNT - 1;
			else if (pos <= 12) {	//左右手II
				pos += GRID_COUNT - 8;
				x = 1;
			} else
				pos = -1;
			break;
		case 2:		//in belt(物品排列方式与其他网格不同)
			pos = IN_BELT;
			x = iColumn % 4;
			y = POSITION_RECT[pos][3] - iColumn / 4 - 1;
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

//struct CItemView

CItemView::CItemView(CD2Item & item, EEquip equip, EPosition pos, int x, int y)
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
}

CSize CItemView::ViewSize() const { return CSize(iGridWidth * GRID_WIDTH, iGridHeight*GRID_WIDTH); }

// CDlgCharItems 对话框

IMPLEMENT_DYNAMIC(CDlgCharItems, CPropertyDialog)

CDlgCharItems::CDlgCharItems(CWnd* pParent /*=NULL*/)
    : CPropertyDialog(CDlgCharItems::IDD, pParent)
{
	//m_vRectGrid
	for (auto & p : POSITION_RECT) {
		auto left = p[0];
		auto top = p[1];
		auto right = p[0] + GRID_WIDTH * p[2];
		auto bottom = p[1] + GRID_WIDTH * p[3];
		m_vRectGrid.emplace_back(left, top, right, bottom);
	}
	//m_vGridItems
	m_vGridItems.resize(POSITION_END);
	for (UINT i = 0; i < size(m_vGridItems); ++i)
		if (i < GRID_COUNT)
			m_vGridItems[i].resize(POSITION_RECT[i][2] * POSITION_RECT[i][3], -1);
		else
			m_vGridItems[i].resize(1, -1);
	//武器II
	m_vGridItems[RIGHT_HAND].push_back(-1);
	m_vGridItems[LEFT_HAND].push_back(-1);
 }

void CDlgCharItems::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_x, m_pMouse.x);
    DDX_Text(pDX, IDC_STATIC_y, m_pMouse.y);
    DDX_Check(pDX, IDC_CHECK2, m_bNotShowItemInfoDlg);
    //DDX_Control(pDX, IDC_LIST1, m_lcPropertyList);
    //DDX_Control(pDX, IDC_COMBO1, m_cbQuality);
    ////DDX_Text(pDX, IDC_EDIT1, m_sItemName);
    //DDX_Text(pDX, IDC_EDIT2, m_bItemLevel);
    ////DDX_Check(pDX, IDC_CHECK3, m_bItemInscribed);
    //DDX_Text(pDX, IDC_EDIT4, m_ItemOwner);
    //DDV_MaxChars(pDX, m_ItemOwner, 15);
    //DDX_Check(pDX, IDC_CHECK5, m_bItemSocket);
    //DDX_Text(pDX, IDC_EDIT5, m_bBaseSocket);
    //DDX_Text(pDX, IDC_EDIT7, m_bExtSocket);
    //DDX_Text(pDX, IDC_EDIT8, m_wItemQuantity);
    //DDX_Text(pDX, IDC_EDIT9, m_wItemDefence);
    //DDX_Check(pDX, IDC_CHECK6, m_bEthereal);
    //DDX_Check(pDX, IDC_CHECK7, m_bIndestructible);
    //DDX_Text(pDX, IDC_EDIT12, m_wCurDurability);
    //DDX_Text(pDX, IDC_EDIT44, m_wMaxDurability);
    DDX_Control(pDX, IDC_SLIDER1, m_scTrasparent);
    DDX_Check(pDX, IDC_CHECK1, m_bSecondHand);
 //   DDX_Text(pDX, IDC_STATIC1, m_sText[0]);
 //   DDX_Text(pDX, IDC_STATIC2, m_sText[1]);
 //   DDX_Text(pDX, IDC_STATIC3, m_sText[2]);
 //   DDX_Text(pDX, IDC_STATIC4, m_sText[3]);
 //   DDX_Text(pDX, IDC_STATIC5, m_sText[4]);
 //   DDX_Text(pDX, IDC_STATIC6, m_sText[5]);
 //   DDX_Text(pDX, IDC_STATIC7, m_sText[6]);
 //   DDX_Text(pDX, IDC_STATIC8, m_sText[7]);
 //   DDX_Text(pDX, IDC_CHECK2, m_sText[8]);
 //   DDX_Text(pDX, IDC_CHECK5, m_sText[9]);
 //   DDX_Text(pDX, IDC_CHECK6, m_sText[10]);
	//DDX_Text(pDX, IDC_CHECK7, m_sText[11]);
	DDX_Text(pDX, IDC_STATIC_Sockets, m_sText[0]);
	DDX_Text(pDX, IDC_STATIC_Mouse, m_sText[1]);
	DDX_Text(pDX, IDC_STATIC_Cube, m_sText[2]);
	DDX_Text(pDX, IDC_STATIC_Belt, m_sText[3]);
 //   DDX_Control(pDX, IDC_BUTTON1, m_btButton[0]);
 //   DDX_Control(pDX, IDC_BUTTON2, m_btButton[1]);
 //   DDX_Control(pDX, IDC_BUTTON3, m_btButton[2]);
 //   DDX_Control(pDX, IDC_BUTTON4, m_btButton[3]);
	//DDX_Control(pDX, IDC_BUTTON5, m_btButton[4]);
}

BEGIN_MESSAGE_MAP(CDlgCharItems, CDialog)
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
//    ON_WM_SETCURSOR()
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_CHECK2, &CDlgCharItems::OnBnClickedCheck2)
    ON_BN_CLICKED(IDC_CHECK1, &CDlgCharItems::OnChangeHand)
    //ON_BN_CLICKED(IDC_BUTTON2, &CDlgCharItems::OnPrefixSuffix)
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

void CDlgCharItems::UpdateUI(CD2S_Struct & character) {
	m_vItemViews.clear();
	m_vItemViews.reserve(character.ItemList.vItems.size());
	for (UINT i = 0; i < character.ItemList.vItems.size(); ++i) {
		auto & item = character.ItemList.vItems[i];
		auto t = ItemToPosition(item.iLocation, item.iPosition, item.iColumn, item.iRow, item.iStoredIn);
		auto equip = ItemToEquip(item.MetaData().Equip);
		auto & view = m_vItemViews.emplace_back(item, equip, get<0>(t), get<1>(t), get<2>(t));
		UpdateGridItem(int(m_vItemViews.size() - 1), view.iPosition, view.iGridX, view.iGridY);
		//Gems
		int j = 0;
		for (auto & gem : item.aGemItems)
			view.vGemItems.emplace_back(gem, ItemToEquip(gem.MetaData().Equip), IN_SOCKET, j++, 0);
	}
	m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
	Invalidate();
}

int CDlgCharItems::GetGridItemIndex(EPosition pos, int x, int y) const {
	ASSERT(pos < POSITION_END);
	ASSERT(0 <= x && 0 <= y);
	const UINT idx = x + y * POSITION_RECT[pos][2];
	auto & grid = m_vGridItems[pos];
	ASSERT(idx < grid.size());
	return grid[idx];
}

void CDlgCharItems::SetGridItemIndex(EPosition pos, int x, int y, int index) {
	ASSERT(pos < POSITION_END);
	ASSERT(0 <= x && 0 <= y);
	const UINT idx = x + y * POSITION_RECT[pos][2];
	auto & grid = m_vGridItems[pos];
	ASSERT(idx < grid.size());
	grid[idx] = index;
}

CPoint CDlgCharItems::GetItemPositionXY(const CItemView & view) const {
	ASSERT(view.iPosition < POSITION_END);
	auto & p = POSITION_RECT[view.iPosition];
	if (view.iPosition < GRID_COUNT)
		return CPoint(p[0] + view.iGridX * GRID_WIDTH, p[1] + view.iGridY * GRID_WIDTH);
	int x = p[0] + (p[2] - view.iGridWidth) * GRID_WIDTH / 2;
	int y = p[1] + (p[3] - view.iGridHeight) * GRID_WIDTH / 2;
	return CPoint(x, y);
}

const CItemView * CDlgCharItems::SelectedParentItemView() const {
	if(0 <= m_iSelectedItemIndex && m_iSelectedItemIndex < int(m_vItemViews.size()))
		return &m_vItemViews[m_iSelectedItemIndex];
	return 0;
}

const CItemView * CDlgCharItems::SelectedItemView() const {
	const auto parent = SelectedParentItemView();
	if(parent && 0 <= m_iSelectedSocketIndex && m_iSelectedSocketIndex < int(parent->vGemItems.size()))
		return &parent->vGemItems[m_iSelectedSocketIndex];
	return parent;
}

BOOL CDlgCharItems::UpdateGridItem(int index, EPosition pos, int x, int y) {
	ASSERT(0 <= index && index < int(m_vItemViews.size()));
	ASSERT(pos < POSITION_END);
	ASSERT(0 <= x && 0 <= y);
	auto & view = m_vItemViews[index];
	const auto equip = view.Item.MetaData().Equip;	//物品的可穿戴位置
	if (pos < GRID_COUNT) {	//带网格的位置
		if (x + view.iGridWidth > POSITION_RECT[pos][2] || y + view.iGridHeight > POSITION_RECT[pos][3])
			return FALSE;	//物品在网格外面
		if (pos == IN_BELT && equip != 9)
			return FALSE;	//物品不能放到腰带里
		for (int i = 0; i < view.iGridWidth; ++i)
			for (int j = 0; j < view.iGridHeight; ++j)
				if (-1 != GetGridItemIndex(pos, x + i, y + j))
					return FALSE;	//网格里有物品
		for (int i = 0; i < view.iGridWidth; ++i)
			for (int j = 0; j < view.iGridHeight; ++j)
				SetGridItemIndex(pos, x + i, y + j, index);
	} else {	//身体部位和鼠标
		if (pos != IN_MOUSE) {
			const BYTE EQUIP[] = { 1,2,3,4,4,5,5,6,7,8 };	//网格位置对应的物品可穿戴类型
			if (equip != EQUIP[pos - HEAD])
				return FALSE;	//物品不能穿戴到指定部位
		}
		if (-1 != GetGridItemIndex(pos, x, y))
			return FALSE;	//已穿戴物品
		SetGridItemIndex(pos, x, y, index);
	}
	view.iPosition = pos;
	view.iGridX = x;
	view.iGridY = y;
	return TRUE;
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
    for(int i = 0;i < POSITION_END;++i)
		if(i < GRID_COUNT)
			::DrawGrid(dc, m_vRectGrid[i], GRID_WIDTH, GRID_WIDTH);
		else
			::DrawGrid(dc, m_vRectGrid[i]);
	dc.SelectObject(pOld);
}

void CDlgCharItems::DrawItemXY(CPaintDC & dc, CPoint pos, const CItemView & view) const
{
	CBitmap bmp;
	CDC memDC;
	bmp.LoadBitmap(view.nPicRes);
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
		if ((RIGHT_HAND == view.iPosition || LEFT_HAND == view.iPosition)
			&& view.iGridX != (m_bSecondHand ? 1 : 0))	//在左右手上，分I, II显示不同物品
			continue;
		auto pos = GetItemPositionXY(view);
		DrawItemXY(dc, pos, view);
		if (i == m_iSelectedItemIndex) {	//选中的物品
			selectedGrid = CRect(pos, view.ViewSize());
			//Draw gems in sockets
			auto & gems = view.vGemItems;
			for (UINT j = 0; j < POSITION_RECT[IN_SOCKET][2] && j < gems.size(); ++j) {
				auto & gemView = gems[j];
				pos = GetItemPositionXY(gemView);
				DrawItemXY(dc, pos, gemView);
				if (j == m_iSelectedSocketIndex)	//选中的宝石
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

tuple<int, int, int> CDlgCharItems::HitTestPosition(CPoint pos) const {
	for (int i = 0; i < GRID_COUNT; ++i) {
		auto & rect = m_vRectGrid[i];
		if (rect.PtInRect(pos)) {
			const int x = (pos.x - rect.left) / GRID_WIDTH;
			const int y = (pos.y - rect.top) / GRID_WIDTH;
			return make_tuple(i, x, y);
		}
	}
	for (int i = HEAD; i < POSITION_END; ++i) {
		auto & rect = m_vRectGrid[i];
		if (rect.PtInRect(pos)) {
			if (RIGHT_HAND == i || LEFT_HAND == i)
				return make_tuple(i, (m_bSecondHand ? 1 : 0), 0);
			else
				return make_tuple(i, 0, 0);
		}
	}
	return make_tuple(-1, -1, -1);
}

void CDlgCharItems::ShowItemInfoDlg(const CD2Item * pItem){
    if(!m_bNotShowItemInfoDlg && pItem && (!m_pDlgItemInfo || pItem != m_pDlgItemInfo->GetItemPtr())){
        if(!m_pDlgItemInfo){
			m_pDlgItemInfo = make_unique<CDlgSuspend>(this, m_scTrasparent.GetPos());
            m_pDlgItemInfo->Create(CDlgSuspend::IDD,NULL);
        }
        LONG height = m_pDlgItemInfo->GetItemInfo(pItem);
        CRect rect,rect1;
        m_pDlgItemInfo->GetWindowRect(&rect);
        GetWindowRect(&rect1);
		m_pDlgItemInfo->MoveWindow(rect1.left + INFO_WINDOW_LEFT, rect1.top, rect.Width(), rect.Height(), TRUE);
        m_pDlgItemInfo->ShowWindow(SW_SHOWNOACTIVATE); //显示对话框
        m_pDlgItemInfo->Invalidate();
    }else if(!pItem && m_pDlgItemInfo)
        m_pDlgItemInfo.reset();
}

void CDlgCharItems::ReadItemProperty(const CD2Item & item) {

   // //m_sItemName = ::theApp.ItemName(item.MetaData().NameIndex);
   // if(m_bItemSocket = item.bSocketed)
   //     m_bBaseSocket = item.pItemInfo->pTpSpInfo->iSocket;
   // m_bEthereal = item.bEthereal;
   // //m_bItemInscribed = item.bPersonalized;
   // if(item.bEar){   //ear structure
   //     m_bItemLevel = item.pEar->iEarLevel;
   //     m_ItemOwner = item.pEar->sEarName;
   // }else{
   //     if(item.pItemInfo->IsTypeName("gld "))
   //         m_wItemQuantity = item.pItemInfo->pGold->wQuantity;
   //     if(!item.bSimple){
   //         m_bItemLevel = item.pItemInfo->pExtItemInfo->iDropLevel;
   //         if(item.bPersonalized)
   //             m_ItemOwner = &item.pItemInfo->pExtItemInfo->sPersonName[0];
   //         m_cbQuality.SetCurSel(item.pItemInfo->pExtItemInfo->iQuality - 1);
   //         if(item.MetaData().IsStacked)
   //             m_wItemQuantity = item.pItemInfo->pTpSpInfo->iQuantity;
   //         if(item.MetaData().HasDef)
   //             m_wItemDefence = item.pItemInfo->pTpSpInfo->iDefence - 10;
   //         if(item.MetaData().HasDur){
   //             m_wMaxDurability = item.pItemInfo->pTpSpInfo->iMaxDurability;
   //             if(!(m_bIndestructible = (m_wMaxDurability == 0)))
   //                 m_wCurDurability = item.pItemInfo->pTpSpInfo->iCurDur;
   //         }
			//for (const auto & p : item.pItemInfo->pTpSpInfo->stPropertyList.mProperty) {
			//	if (p.first == 194) {	//Adds X extra sockets to the item
			//		m_bExtSocket = BYTE(p.second);
			//	} else {
			//		int i = m_lcPropertyList.InsertItem(0, CSFormat(_T("%3d"), UINT(p.first)));	//属性代码
			//		m_lcPropertyList.SetItemText(i, 1, ::theApp.PorpertyDescription(p.first, p.second)); //属性描述
			//	}
			//}
			////TODO: Set property lists
   //     }
   // }

   // UpdateData(FALSE);
}

void CDlgCharItems::ResetFoundry()
{
    ////m_sItemName = _T("");
    //m_bItemSocket = FALSE;
    //m_bBaseSocket = m_bExtSocket = 0;
    //m_bEthereal = FALSE;
    ////m_bItemInscribed = FALSE;
    //m_bItemLevel = 0;
    //m_ItemOwner = _T("");
    //m_wItemQuantity = 0;
    //m_cbQuality.SetCurSel(-1);
    //m_wItemDefence = 0;
    //m_wMaxDurability = m_wCurDurability = 0;
    //m_lcPropertyList.DeleteAllItems();

    //UpdateData(FALSE);
}

BOOL CDlgCharItems::GatherData(CD2S_Struct & character)
{
	// TODO:

    return TRUE;
}

void CDlgCharItems::ResetAll()
{
	m_vItemViews.clear();
	for (auto & grid : m_vGridItems)
		fill(grid.begin(), grid.end(), -1);
    m_bSecondHand = FALSE;
	m_iSelectedItemIndex = m_iSelectedSocketIndex = -1;
	m_pDlgItemInfo.reset();
	ResetFoundry();

    Invalidate();
}

void CDlgCharItems::LoadText(void)
{
	int index = 0;
    for(auto & text : m_sText)
        text = ::theApp.CharItemsUI(index++);
    //m_cbQuality.ResetContent();
	//for(UINT i = 0;i < ::theApp.ItemQualityNameSize();++i)
	//	m_cbQuality.AddString(::theApp.ItemQualityName(i));
    //设置属性列表的标题文字
    /*LVCOLUMN col;
    col.cchTextMax = 20;
    col.mask = LVCF_TEXT;
    col.pszText = (LPWSTR)::theApp.CharItemsUI(index++).GetString();
    m_lcPropertyList.SetColumn(0,&col);
    col.pszText = (LPWSTR)::theApp.CharItemsUI(index++).GetString();
    m_lcPropertyList.SetColumn(1,&col);*/

	UpdateData(FALSE);
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
	const CD2Item * item = 0;
	auto t = HitTestPosition(point);
	const int pos = get<0>(t), x = get<1>(t), y = get<2>(t);
	if (0 <= pos) {		//在有效网格里
		const int index = GetGridItemIndex(EPosition(pos), x, y);
		if (0 <= index) {	//有物品
			if (IN_SOCKET == pos) {	//镶嵌的宝石
				auto & gems = SelectedParentItemView()->vGemItems;
				ASSERT(index < int(gems.size()));
				item = &gems[index].Item;
			} else {	//其他物品
				ASSERT(index < int(m_vItemViews.size()));
				item = &m_vItemViews[index].Item;
			}
		}
	}
	ShowItemInfoDlg(item);

	m_pMouse = point;
	UpdateData(FALSE);
	CPropertyDialog::OnMouseMove(nFlags, point);
}

void CDlgCharItems::OnLButtonDown(UINT nFlags, CPoint point)
{
	auto t = HitTestPosition(point);
	const int pos = get<0>(t), x = get<1>(t), y = get<2>(t);
	if (IN_SOCKET == pos) {
		const int index = GetGridItemIndex(IN_SOCKET, x, y);
		if (0 <= index && index != m_iSelectedSocketIndex) {
			ResetFoundry();
			auto & gems = SelectedParentItemView()->vGemItems;
			ASSERT(0 <= index && index < int(gems.size()));
			m_iSelectedSocketIndex = index;
			ReadItemProperty(gems[m_iSelectedSocketIndex].Item);
			Invalidate();
		}
	} else if (0 <= pos) {
		const int index = GetGridItemIndex(EPosition(pos), x, y);
		if (0 <= index && (index != m_iSelectedItemIndex || 0 <= m_iSelectedSocketIndex)) {
			if (0 <= m_iSelectedItemIndex)
				ResetFoundry();
			ASSERT(0 <= index && index < int(m_vItemViews.size()));
			const auto & view = m_vItemViews[index];
			m_iSelectedItemIndex = index;
			m_iSelectedSocketIndex = -1;
			ReadItemProperty(view.Item);
			//设置镶嵌的物品
			for (int i = 0; i < int(m_vGridItems[IN_SOCKET].size()); ++i)
				SetGridItemIndex(IN_SOCKET, i, 0, (i < int(view.vGemItems.size()) ? i : -1));
			Invalidate();
		}
	}
	CPropertyDialog::OnLButtonDown(nFlags, point);
}

void CDlgCharItems::OnRButtonUp(UINT nFlags, CPoint point)
{

    CPropertyDialog::OnRButtonUp(nFlags, point);
}

BOOL CDlgCharItems::OnInitDialog()
{
    CPropertyDialog::OnInitDialog();
	/*m_lcPropertyList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lcPropertyList.InsertColumn(0, _T(""), LVCFMT_LEFT, 60);
	m_lcPropertyList.InsertColumn(1, _T(""), LVCFMT_LEFT, 225);*/
	m_scTrasparent.SetRange(0, 255);
	m_scTrasparent.SetPos(200);
    LoadText();
    return TRUE;
}

void CDlgCharItems::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CPropertyDialog::OnShowWindow(bShow, nStatus);
    //if(!bShow)			//在少数情况下，会出现隐藏物品属性窗口时悬浮窗还在的情况
    //    ShowItemInfoDlg(0);
}

void CDlgCharItems::OnBnClickedCheck2()
{
    m_bNotShowItemInfoDlg = !m_bNotShowItemInfoDlg;
    UpdateData(TRUE);
}

void CDlgCharItems::OnChangeHand()
{
    m_bSecondHand = !m_bSecondHand;
    //更新左右手
    for(int i = RIGHT_HAND;i <= LEFT_HAND;++i){
        CRect rect(POSITION_RECT[i][0],POSITION_RECT[i][1],0,0);
        rect.right = rect.left + POSITION_RECT[i][2] * GRID_WIDTH;
        rect.bottom = rect.top + POSITION_RECT[i][3] * GRID_WIDTH;
        InvalidateRect(&rect);
    }
}

//void CDlgCharItems::OnPrefixSuffix()
//{
//    std::vector<int> selIndex(10,-1);
//	auto view = SelectedItemView();
//    if(view){
//        auto & item = view->Item;
//        if(item.pItemInfo.exist() && item.pItemInfo->pExtItemInfo.exist()){
//            switch(m_cbQuality.GetCurSel() + 1){
//                case 1:     //low
//                    if(item.pItemInfo->pExtItemInfo->loQual.exist())
//                        selIndex[9] = item.pItemInfo->pExtItemInfo->loQual;
//                    break;
//                case 3:     //high
//                    if(item.pItemInfo->pExtItemInfo->hiQual.exist())
//                        selIndex[9] = item.pItemInfo->pExtItemInfo->hiQual;
//                    break;
//                case 4:     //magic
//                    if(item.pItemInfo->pExtItemInfo->wPrefix.exist())
//                        selIndex[2] = item.pItemInfo->pExtItemInfo->wPrefix;
//                    if(item.pItemInfo->pExtItemInfo->wSuffix.exist())
//                        selIndex[3] = item.pItemInfo->pExtItemInfo->wSuffix;
//                    break;
//                case 5:     //set
//                    break;
//                case 6:     //rare
//                    if(item.pItemInfo->pExtItemInfo->pRareName.exist()){
//                        selIndex[0] = item.pItemInfo->pExtItemInfo->pRareName->iName1;
//                        selIndex[1] = item.pItemInfo->pExtItemInfo->pRareName->iName2;
//                        if(item.pItemInfo->pExtItemInfo->pRareName->bPref1)
//                            selIndex[2] = item.pItemInfo->pExtItemInfo->pRareName->wPref1;
//                        if(item.pItemInfo->pExtItemInfo->pRareName->bSuff1)
//                            selIndex[3] = item.pItemInfo->pExtItemInfo->pRareName->wSuff1;
//                        if(item.pItemInfo->pExtItemInfo->pRareName->bPref2)
//                            selIndex[4] = item.pItemInfo->pExtItemInfo->pRareName->wPref2;
//                        if(item.pItemInfo->pExtItemInfo->pRareName->bSuff2)
//                            selIndex[5] = item.pItemInfo->pExtItemInfo->pRareName->wSuff2;
//                        if(item.pItemInfo->pExtItemInfo->pRareName->bPref3)
//                            selIndex[6] = item.pItemInfo->pExtItemInfo->pRareName->wPref3;
//                        if(item.pItemInfo->pExtItemInfo->pRareName->bSuff3)
//                            selIndex[7] = item.pItemInfo->pExtItemInfo->pRareName->wSuff3;
//                    }
//                    break;
//                case 7:     //unique
//                    if(item.pItemInfo->pExtItemInfo->wUniID.exist())
//                        selIndex[8] = item.pItemInfo->pExtItemInfo->wUniID;
//                    break;
//                case 8:     //crafted
//                    if(item.pItemInfo->pExtItemInfo->pCraftName.exist()){
//                        selIndex[0] = item.pItemInfo->pExtItemInfo->pCraftName->iName1;
//                        selIndex[1] = item.pItemInfo->pExtItemInfo->pCraftName->iName2;
//                        if(item.pItemInfo->pExtItemInfo->pCraftName->bPref1)
//                            selIndex[2] = item.pItemInfo->pExtItemInfo->pCraftName->wPref1;
//                        if(item.pItemInfo->pExtItemInfo->pCraftName->bSuff1)
//                            selIndex[3] = item.pItemInfo->pExtItemInfo->pCraftName->wSuff1;
//						if (item.pItemInfo->pExtItemInfo->pCraftName->bPref2)
//							selIndex[4] = item.pItemInfo->pExtItemInfo->pCraftName->wPref2;
//                        if(item.pItemInfo->pExtItemInfo->pCraftName->bSuff2)
//                            selIndex[5] = item.pItemInfo->pExtItemInfo->pCraftName->wSuff2;
//                        if(item.pItemInfo->pExtItemInfo->pCraftName->bPref3)
//                            selIndex[6] = item.pItemInfo->pExtItemInfo->pCraftName->wPref3;
//                        if(item.pItemInfo->pExtItemInfo->pCraftName->bSuff3)
//                            selIndex[7] = item.pItemInfo->pExtItemInfo->pCraftName->wSuff3;
//                    }
//                    break;
//                default:;
//            }
//        }
//    }
//    CDlgPrefixSuffix dlgPrefix(m_cbQuality.GetCurSel() + 1,&selIndex[0],this);
//    dlgPrefix.DoModal();
//}
