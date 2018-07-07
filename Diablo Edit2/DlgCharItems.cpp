// DlgCharItems.cpp : 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgCharItems.h"


// CDlgCharItems 对话框

const WORD	CDlgCharItems::GRID_RECT[CDlgCharItems::GRID_BODY_NUMBER][4] = {
    {10,5,6,8},		//箱子		0	left,top,col,row
    {10,255,10,4},	//口袋		1
    {340,255,3,4},	//方块		2
    {300,5,2,2},	//头		3
    {365,30,1,1},	//项链		4
    {300,70,2,3},	//身体		5
    {200,30,2,4},	//武器右	6
    {400,30,2,4},	//武器左	7
    {265,165,1,1},	//戒指右	8
    {365,165,1,1},	//戒指左	9
    {300,165,2,1},	//腰带		10
    {400,155,2,2},	//鞋子		11
    {200,155,2,2},	//手套		12
    //{805,30,2,4},	//铸造台	13
};

IMPLEMENT_DYNAMIC(CDlgCharItems, CPropertyDialog)

CDlgCharItems::CDlgCharItems(CWnd* pParent /*=NULL*/)
    : CPropertyDialog(CDlgCharItems::IDD, pParent)
{
    //网格
    for(int i = 0;i < GRID_BODY_NUMBER;++i){
        m_rectGrid[i].left = GRID_RECT[i][0];
        m_rectGrid[i].top = GRID_RECT[i][1];
        m_rectGrid[i].right = GRID_RECT[i][0] + GRID_WIDTH * GRID_RECT[i][2];
        m_rectGrid[i].bottom = GRID_RECT[i][1] + GRID_WIDTH * GRID_RECT[i][3];
    }
    m_iGridItems.resize(GRID_BODY_NUMBER);
    for(int i = 0;i < GRID_NUMBER;++i)
        m_iGridItems[i].resize(GRID_RECT[i][2]  * GRID_RECT[i][3],INVALID_ITEM);
    for(int i = GRID_NUMBER;i < GRID_BODY_NUMBER;++i)
        m_iGridItems[i].resize(1,INVALID_ITEM);
    //武器II
    m_iGridItems[6].push_back(INVALID_ITEM);
    m_iGridItems[7].push_back(INVALID_ITEM);
}

CDlgCharItems::~CDlgCharItems()
{
    DestroyAllItems();
    if(m_pDlgItemInfo)
        delete m_pDlgItemInfo;
}

void CDlgCharItems::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_x, m_pMouse.x);
    DDX_Text(pDX, IDC_STATIC_y, m_pMouse.y);
    DDX_Check(pDX, IDC_CHECK2, m_bNotShowItemInfoDlg);
    DDX_Control(pDX, IDC_LIST1, m_lcPropertyList);
    DDX_Control(pDX, IDC_COMBO1, m_cbQuality);
    //DDX_Text(pDX, IDC_EDIT1, m_sItemName);
    DDX_Text(pDX, IDC_EDIT2, m_bItemLevel);
    //DDX_Check(pDX, IDC_CHECK3, m_bItemInscribed);
    DDX_Text(pDX, IDC_EDIT4, m_ItemOwner);
    DDV_MaxChars(pDX, m_ItemOwner, 15);
    DDX_Check(pDX, IDC_CHECK5, m_bItemSocket);
    DDX_Text(pDX, IDC_EDIT5, m_bBaseSocket);
    DDX_Text(pDX, IDC_EDIT7, m_bExtSocket);
    DDX_Text(pDX, IDC_EDIT8, m_wItemQuantity);
    DDX_Text(pDX, IDC_EDIT9, m_wItemDefence);
    DDX_Check(pDX, IDC_CHECK6, m_bEthereal);
    DDX_Check(pDX, IDC_CHECK7, m_bIndestructible);
    DDX_Text(pDX, IDC_EDIT12, m_wCurDurability);
    DDX_Text(pDX, IDC_EDIT44, m_wMaxDurability);
    DDX_Control(pDX, IDC_SLIDER1, m_scTrasparent);
    DDX_Check(pDX, IDC_CHECK1, m_bSecondHand);
    DDX_Text(pDX, IDC_STATIC1, m_sText[0]);
    DDX_Text(pDX, IDC_STATIC2, m_sText[1]);
    DDX_Text(pDX, IDC_STATIC3, m_sText[2]);
    DDX_Text(pDX, IDC_STATIC4, m_sText[3]);
    DDX_Text(pDX, IDC_STATIC5, m_sText[4]);
    DDX_Text(pDX, IDC_STATIC6, m_sText[5]);
    DDX_Text(pDX, IDC_STATIC7, m_sText[6]);
    DDX_Text(pDX, IDC_STATIC8, m_sText[7]);
    DDX_Text(pDX, IDC_CHECK2, m_sText[8]);
    DDX_Text(pDX, IDC_CHECK5, m_sText[9]);
    DDX_Text(pDX, IDC_CHECK6, m_sText[10]);
    DDX_Text(pDX, IDC_CHECK7, m_sText[11]);
    DDX_Control(pDX, IDC_BUTTON1, m_btButton[0]);
    DDX_Control(pDX, IDC_BUTTON2, m_btButton[1]);
    DDX_Control(pDX, IDC_BUTTON3, m_btButton[2]);
    DDX_Control(pDX, IDC_BUTTON4, m_btButton[3]);
    DDX_Control(pDX, IDC_BUTTON5, m_btButton[4]);
}

BEGIN_MESSAGE_MAP(CDlgCharItems, CDialog)
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
//    ON_WM_SETCURSOR()
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_CHECK2, &CDlgCharItems::OnBnClickedCheck2)
    ON_BN_CLICKED(IDC_CHECK1, &CDlgCharItems::OnChangeHand)
    ON_BN_CLICKED(IDC_BUTTON2, &CDlgCharItems::OnPrefixSuffix)
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

//画一个网格或矩形
void DrawGrid(CPaintDC & dc, const CRect & rect, int intervalX = 0, int intervalY = 0)
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
	//储存箱，口袋，方块
    for(int i = 0;i < GRID_NUMBER;++i)
		::DrawGrid(dc, m_rectGrid[i], GRID_WIDTH, GRID_WIDTH);
    //身体部位
    for(int i = GRID_NUMBER;i < GRID_BODY_NUMBER;++i)
		::DrawGrid(dc, m_rectGrid[i]);

	dc.SelectObject(pOld);
}

void CDlgCharItems::DrawItemXY(CPaintDC & dc, CPoint pos, const CItemView & itemView)
{
	//Draw item image
	CBitmap bmp;
	CDC memDC;
	bmp.LoadBitmap(itemView.nPicRes);
	memDC.CreateCompatibleDC(&dc);
	CBitmap* pOld = memDC.SelectObject(&bmp);
	dc.BitBlt(pos.x, pos.y, COL(itemView.Range) * GRID_WIDTH, ROW(itemView.Range) * GRID_WIDTH, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOld);
}

void CDlgCharItems::DrawItemsInGrid(CPaintDC & dc)
{
	CRect selected(0, 0, 0, 0);
	for (size_t i = 0; i < m_vpItems.size(); ++i) {
		if (!m_vpItems[i])
			continue;
		auto & itemView = *m_vpItems[i];
		CPoint pos;
		int gridID = INDEX(itemView.Pos);
		if (gridID < GRID_NUMBER)    //在箱子,背包,方块里
			pos = GRID2XY(itemView.Pos);
		else if (gridID != 6 && gridID != 7) //身上(除了左右手)
			pos = GRID2XY(gridID, itemView.Range);
		else {       //左右手,分I,II
			if (m_bSecondHand != COL(itemView.Pos))
				continue;
			pos = GRID2XY(gridID, itemView.Range);
		}
		DrawItemXY(dc, pos, itemView);

		if (i == m_iSelectedItemIndex) {
			CSize sz(COL(itemView.Range) * GRID_WIDTH, ROW(itemView.Range) * GRID_WIDTH);
			selected = CRect(pos, sz);
		}
	}
	// 高亮选中的物品
	if (selected.top != 0) {
		CPen pen(PS_SOLID, 2, RGB(255, 0, 0));
		CPen * pOld = dc.SelectObject(&pen);
		DrawGrid(dc, selected);
		dc.SelectObject(pOld);
	}
}

WORD CDlgCharItems::HitTestItem(const CPoint & pos,WORD range)
{
    for(int i = 0;i < GRID_NUMBER;++i){ //箱子,背包,方块
        if(m_rectGrid[i].PtInRect(pos)){
            int x = (pos.x - GRID_WIDTH * (COL(range) - 1) / 2 - m_rectGrid[i].left) / GRID_WIDTH;
            int y = (pos.y - GRID_WIDTH * (ROW(range) - 1) / 2 - m_rectGrid[i].top) / GRID_WIDTH;
            return MAKE_GRID(i,x,y);
        }
    }
    for(int i = GRID_NUMBER;i < GRID_BODY_NUMBER;++i)   //其他
        if(m_rectGrid[i].PtInRect(pos)){
            if(i == 6 || i == 7)    //左右手
                return MAKE_GRID(i,m_bSecondHand,0);
            else    //其他
                return MAKE_GRID(i,0,0);
        }
    return INVALID_ITEM;
}

BOOL CDlgCharItems::PutItemInGrid(WORD itemIndex, WORD gridIndex) {
	ASSERT(itemIndex >= 0 && itemIndex < WORD(m_vpItems.size()) && m_vpItems[itemIndex]);
	if (gridIndex == INVALID_ITEM)
		return FALSE;
	int i = INDEX(gridIndex);
	if (i < GRID_NUMBER) {	//存储箱,背包,方块
		int x0 = COL(gridIndex);
		int y0 = ROW(gridIndex);
		CItemView & rit = *m_vpItems[itemIndex];
		if (x0 + COL(rit.Range) > GRID_RECT[i][2] ||
			y0 + ROW(rit.Range) > GRID_RECT[i][3])
			return FALSE;   //物品在网格外面了
		for (int x = 0; x < COL(rit.Range); ++x)
			for (int y = 0; y < ROW(rit.Range); ++y)
				if (GET_GRID_ITEM(i, x0 + x, y0 + y) != INVALID_ITEM)
					return FALSE;   //网格里有物品
		m_vpItems[itemIndex]->Pos = gridIndex;
		for (int x = 0; x < COL(rit.Range); ++x)
			for (int y = 0; y < ROW(rit.Range); ++y)
				SET_GRID_ITEM(i, x0 + x, y0 + y, itemIndex);
	} else {					//身体或锻造台
		const WORD MAP[10] = { 1,2,3,4,4,5,5,6,7,8 };
		if (m_vpItems[itemIndex]->pItem->pItemData->Equip() != MAP[i - 3])
			return FALSE;
		int c = COL(gridIndex), r = ROW(gridIndex);
		if (GET_GRID_ITEM(i, c, r) != INVALID_ITEM)
			return FALSE;
		m_vpItems[itemIndex]->Pos = MAKE_GRID(i, c, r);
		SET_GRID_ITEM(i, c, r, itemIndex);
	}
	return TRUE;
}

void CDlgCharItems::DestroyAllItems()
{
	for(auto pItem : m_vpItems)
		delete pItem;
}

void CDlgCharItems::ShowItemInfoDlg(const CD2Item * pItem)
{
    if(!m_bNotShowItemInfoDlg && pItem && (!m_pDlgItemInfo || pItem != m_pDlgItemInfo->GetItemPtr())){
        if(!m_pDlgItemInfo){
            m_pDlgItemInfo = new CDlgSuspend(this,m_scTrasparent.GetPos());
            m_pDlgItemInfo->Create(CDlgSuspend::IDD,NULL);
        }
        LONG height = m_pDlgItemInfo->GetItemInfo(pItem);
        CRect rect,rect1;
        m_pDlgItemInfo->GetWindowRect(&rect);
        GetWindowRect(&rect1);
        m_pDlgItemInfo->MoveWindow(rect1.left + 475,rect1.top,rect.Width(),rect.Height(),TRUE);
        m_pDlgItemInfo->ShowWindow(SW_SHOWNOACTIVATE); //显示对话框
        m_pDlgItemInfo->Invalidate();
    }else if(!pItem && m_pDlgItemInfo){
        delete m_pDlgItemInfo;
        m_pDlgItemInfo = 0;
    }
}

void CDlgCharItems::ReadItemProperty(WORD itemIndex)
{
    ASSERT(itemIndex >= 0 && itemIndex < WORD(m_vpItems.size()) && m_vpItems[itemIndex] && m_vpItems[itemIndex]->pItem);
    const CD2Item & rit = *m_vpItems[itemIndex]->pItem;
    //m_sItemName = ::theApp.ItemName(rit.pItemData->NameIndex());
    if(m_bItemSocket = rit.bSocketed)
        m_bBaseSocket = rit.pItemInfo->pTpSpInfo->iSocket.Value();
    m_bEthereal = rit.bEthereal;
    //m_bItemInscribed = rit.bPersonalized;
    if(rit.bEar){   //ear structure
        m_bItemLevel = rit.pEar->iEarLevel;
        m_ItemOwner = rit.pEar->sEarName;
    }else{
        if(IsSameType(rit.pItemInfo->sTypeName,"gld "))
            m_wItemQuantity = rit.pItemInfo->pGold->wQuantity;
        if(!rit.bSimple){
            m_bItemLevel = rit.pItemInfo->pExtItemInfo->iDropLevel;
            if(rit.bPersonalized)
                m_ItemOwner = rit.pItemInfo->pExtItemInfo->sPersonName.Value();
            m_cbQuality.SetCurSel(rit.pItemInfo->pExtItemInfo->iQuality - 1);
            if(rit.pItemData->IsStacked())
                m_wItemQuantity = rit.pItemInfo->pTpSpInfo->iQuantity.Value();
            if(rit.pItemData->HasDef())
                m_wItemDefence = rit.pItemInfo->pTpSpInfo->iDefence.Value() - 10;
            if(rit.pItemData->HasDur()){
                m_wMaxDurability = rit.pItemInfo->pTpSpInfo->iMaxDurability.Value();
                if(!(m_bIndestructible = (m_wMaxDurability == 0)))
                    m_wCurDurability = rit.pItemInfo->pTpSpInfo->iCurDur.Value();
            }
            for(std::map<WORD,DWORD>::const_iterator it = rit.pItemInfo->pTpSpInfo->mProperty.begin();
                it != rit.pItemInfo->pTpSpInfo->mProperty.end();++it)
            {
                if(it->first == 194){	//Adds X extra sockets to the item
                    m_bExtSocket = BYTE(it->second);
                }else{
                    CString tmp;
                    tmp.Format(_T("%3d"),UINT(it->first)); //属性代码
                    int i = m_lcPropertyList.InsertItem(0,tmp);
                    m_lcPropertyList.SetItemText(i,1,::theApp.PorpertyDescription(it->first,it->second)); //属性描述
                }
            }
        }
    }

    UpdateData(FALSE);
}

void CDlgCharItems::ResetFoundry()
{
    //m_sItemName = _T("");
    m_bItemSocket = FALSE;
    m_bBaseSocket = m_bExtSocket = 0;
    m_bEthereal = FALSE;
    //m_bItemInscribed = FALSE;
    m_bItemLevel = 0;
    m_ItemOwner = _T("");
    m_wItemQuantity = 0;
    m_cbQuality.SetCurSel(-1);
    m_wItemDefence = 0;
    m_wMaxDurability = m_wCurDurability = 0;
    m_lcPropertyList.DeleteAllItems();
    UpdateData(FALSE);
}

//虚函数重载
void CDlgCharItems::UpdateUI(const CD2S_Struct & character)
{
    if(!m_vpItems.empty())
        ResetAll();
    m_vpItems.resize(character.ItemList.nItems);
    for(WORD i = 0;i < character.ItemList.nItems;++i){
        ASSERT(character.ItemList.vpItems[i] && _T("CDlgCharItems::UpdateUI(const CD2S_Struct & character)"));
        CD2Item & rit = *character.ItemList.vpItems[i];
        //if(rit.bEar){				//耳朵
        //    //...
        //}else if(rit.pItemData){	//物品
        int index = INVALID_ITEM,x,y;
        switch(rit.iLocation){
            case 0:		//grid
                index = (rit.iStoredIn == 1 ? 1 : (rit.iStoredIn == 4 ? 2 : (rit.iStoredIn == 5 ? 0 : INVALID_ITEM)));
                x = rit.iColumn;
                y = rit.iRow;
                break;
            case 1:		//equipped
                if(rit.iPosition){
                    if(rit.iPosition <= 10){
                        index = rit.iPosition + 2;
                        x = y = 0;
                    }else if(rit.iPosition <= 12){
                        index = rit.iPosition - 5;
                        x = 1;
                        y = 0;
                    }
                }
                break;
            case 2:		//in belt
                break;
            case 4:		//in hand(鼠标)
                break;
            case 6:		//glued into a socket
                break;
            default:;
        }
        if(index != INVALID_ITEM){
            m_vpItems[i] = new CItemView(BMP_INDEX_BASE + rit.pItemData->PicIndex(),rit.pItemData->Range(),&rit);
            PutItemInGrid(i,MAKE_GRID(index,x,y));
        }
        //}
    }
    Invalidate();
}

BOOL CDlgCharItems::GatherData(CD2S_Struct & character)
{

    return FALSE;
}

void CDlgCharItems::ResetAll()
{
    DestroyAllItems();
    m_vpItems.clear();
    m_bSecondHand = FALSE;
    for(int i = 0;i < GRID_BODY_NUMBER;++i)
        std::fill(m_iGridItems[i].begin(),m_iGridItems[i].end(),INVALID_ITEM);
    ResetFoundry();
    Invalidate();
}

void CDlgCharItems::LoadText(void)
{
    for(int i = 0;i < 12;++i)
        m_sText[i] = ::theApp.String(424 + i);
    for(int i = 0;i < 5;++i)
        m_btButton[i].SetWindowText(::theApp.String(436 + i));
    m_cbQuality.ResetContent();
    for(int i = 0;i < 8;++i)
        m_cbQuality.AddString(::theApp.String(386 + i));
    //设置属性列表的标题文字
    LVCOLUMN col;
    col.cchTextMax = 20;
    col.mask = LVCF_TEXT;
    col.pszText = (LPWSTR)::theApp.String(383).GetString(); 
    m_lcPropertyList.SetColumn(0,&col);
    col.pszText = (LPWSTR)::theApp.String(384).GetString(); 
    m_lcPropertyList.SetColumn(1,&col);

	UpdateData(FALSE);
}

// CDlgCharItems 消息处理程序

void CDlgCharItems::OnPaint()
{
    CPaintDC dc(this);
    DrawGrids(dc);					//画储存箱
    DrawItemsInGrid(dc);			//画网格里的物品
}

void CDlgCharItems::OnMouseMove(UINT nFlags, CPoint point)
{
	int grid = HitTestItem(point);
	if (grid != INVALID_ITEM) {// && INDEX(grid) < GRID_BODY_NUMBER - 1){	//在非锻造台的网格范围内
		int item = GET_GRID_ITEM(grid);
		if (item != INVALID_ITEM) {		//悬停在物品上
			ShowItemInfoDlg(m_vpItems[item]->pItem);
		} else
			ShowItemInfoDlg(0);
	} else
		ShowItemInfoDlg(0);

	m_pMouse = point;
	UpdateData(FALSE);
	CPropertyDialog::OnMouseMove(nFlags, point);
}

void CDlgCharItems::OnLButtonDown(UINT nFlags, CPoint point)
{
	int grid = HitTestItem(point);
	if (grid != INVALID_ITEM) {		//在网格范围内
		int item = GET_GRID_ITEM(grid);
		if (item != INVALID_ITEM && item != m_iSelectedItemIndex) {	//点中了另一件物品
			if (m_iSelectedItemIndex != INVALID_ITEM)
				ResetFoundry();
			m_iSelectedItemIndex = item;
			ReadItemProperty(m_iSelectedItemIndex);
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
	m_lcPropertyList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_lcPropertyList.InsertColumn(0, ::theApp.String(383), LVCFMT_LEFT, 60);
	m_lcPropertyList.InsertColumn(1, ::theApp.String(384), LVCFMT_LEFT, 225);
	m_scTrasparent.SetRange(0, 255);
	m_scTrasparent.SetPos(200);
    LoadText();
    return TRUE;
}

void CDlgCharItems::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CPropertyDialog::OnShowWindow(bShow, nStatus);
    if(!bShow)			//在少数情况下，会出现隐藏物品属性窗口时悬浮窗还在的情况
        ShowItemInfoDlg(0);
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
    for(int i = 6;i < 8;++i){
        CRect rect(GRID_RECT[i][0],GRID_RECT[i][1],0,0);
        rect.right = rect.left + GRID_RECT[i][2] * GRID_WIDTH;
        rect.bottom = rect.top + GRID_RECT[i][3] * GRID_WIDTH;
        InvalidateRect(&rect);
    }
}

void CDlgCharItems::OnPrefixSuffix()
{
    std::vector<int> selIndex(10,-1);
    int itemIndex = m_iGridItems[GRID_BODY_NUMBER - 1][0];
    if(itemIndex != INVALID_ITEM){
        CD2Item & rit = *m_vpItems[itemIndex]->pItem;
        if(!!rit.pItemInfo && !!rit.pItemInfo->pExtItemInfo){
            switch(m_cbQuality.GetCurSel() + 1){
                case 1:     //low
                    if(!!rit.pItemInfo->pExtItemInfo->loQual)
                        selIndex[9] = rit.pItemInfo->pExtItemInfo->loQual.Value();
                    break;
                case 3:     //high
                    if(!!rit.pItemInfo->pExtItemInfo->hiQual)
                        selIndex[9] = rit.pItemInfo->pExtItemInfo->hiQual.Value();
                    break;
                case 4:     //magic
                    if(!!rit.pItemInfo->pExtItemInfo->wPrefix)
                        selIndex[2] = rit.pItemInfo->pExtItemInfo->wPrefix.Value();
                    if(!!rit.pItemInfo->pExtItemInfo->wSuffix)
                        selIndex[3] = rit.pItemInfo->pExtItemInfo->wSuffix.Value();
                    break;
                case 5:     //set
                    break;
                case 6:     //rare
                    if(!!rit.pItemInfo->pExtItemInfo->pRareName){
                        selIndex[0] = rit.pItemInfo->pExtItemInfo->pRareName->iName1;
                        selIndex[1] = rit.pItemInfo->pExtItemInfo->pRareName->iName2;
                        if(rit.pItemInfo->pExtItemInfo->pRareName->bPref1)
                            selIndex[2] = rit.pItemInfo->pExtItemInfo->pRareName->wPref1.Value();
                        if(rit.pItemInfo->pExtItemInfo->pRareName->bSuff1)
                            selIndex[3] = rit.pItemInfo->pExtItemInfo->pRareName->wSuff1.Value();
                        if(rit.pItemInfo->pExtItemInfo->pRareName->bPref2)
                            selIndex[4] = rit.pItemInfo->pExtItemInfo->pRareName->wPref2.Value();
                        if(rit.pItemInfo->pExtItemInfo->pRareName->bSuff2)
                            selIndex[5] = rit.pItemInfo->pExtItemInfo->pRareName->wSuff2.Value();
                        if(rit.pItemInfo->pExtItemInfo->pRareName->bPref3)
                            selIndex[6] = rit.pItemInfo->pExtItemInfo->pRareName->wPref3.Value();
                        if(rit.pItemInfo->pExtItemInfo->pRareName->bSuff3)
                            selIndex[7] = rit.pItemInfo->pExtItemInfo->pRareName->wSuff3.Value();
                    }
                    break;
                case 7:     //unique
                    if(!!rit.pItemInfo->pExtItemInfo->wUniID)
                        selIndex[8] = rit.pItemInfo->pExtItemInfo->wUniID.Value();
                    break;
                case 8:     //crafted
                    if(!!rit.pItemInfo->pExtItemInfo->pCraftName){
                        selIndex[0] = rit.pItemInfo->pExtItemInfo->pCraftName->iName1;
                        selIndex[1] = rit.pItemInfo->pExtItemInfo->pCraftName->iName2;
                        if(rit.pItemInfo->pExtItemInfo->pCraftName->bPref1)
                            selIndex[2] = rit.pItemInfo->pExtItemInfo->pCraftName->wPref1.Value();
                        if(rit.pItemInfo->pExtItemInfo->pCraftName->bSuff1)
                            selIndex[3] = rit.pItemInfo->pExtItemInfo->pCraftName->wSuff1.Value();
                        if(rit.pItemInfo->pExtItemInfo->pCraftName->bPref2)
                            selIndex[4] = rit.pItemInfo->pExtItemInfo->pCraftName->wPref2.Value();
                        if(rit.pItemInfo->pExtItemInfo->pCraftName->bSuff2)
                            selIndex[5] = rit.pItemInfo->pExtItemInfo->pCraftName->wSuff2.Value();
                        if(rit.pItemInfo->pExtItemInfo->pCraftName->bPref3)
                            selIndex[6] = rit.pItemInfo->pExtItemInfo->pCraftName->wPref3.Value();
                        if(rit.pItemInfo->pExtItemInfo->pCraftName->bSuff3)
                            selIndex[7] = rit.pItemInfo->pExtItemInfo->pCraftName->wSuff3.Value();
                    }
                    break;
                default:;
            }
        }
    }
    CDlgPrefixSuffix dlgPrefix(m_cbQuality.GetCurSel() + 1,&selIndex[0],this);
    dlgPrefix.DoModal();
}
