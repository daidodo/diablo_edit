﻿// DlgNewItem.cpp: 实现文件
//

#include "stdafx.h"
#include "Diablo Edit2.h"
#include "DlgNewItem.h"
#include "afxdialogex.h"

using namespace std;

const int GRID_WIDTH = 30;	//每个网格的边长(像素)

const int GRID[5] = { 120, 440, 2, 4 };	//展示物品图片的区域, {x, y, w, h}

const CRect GRID_RECT = { GRID[0], GRID[1], GRID[0] + GRID[2] * GRID_WIDTH, GRID[1] + GRID[3] * GRID_WIDTH };

// CDlgNewItem 对话框

IMPLEMENT_DYNAMIC(CDlgNewItem, CDialogEx)

CDlgNewItem::CDlgNewItem(unique_ptr<CD2Item> & ptr, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_NewItem, pParent)
	, m_pItem(ptr)
{}

CDlgNewItem::~CDlgNewItem()
{}

void CDlgNewItem::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDOK, m_sText[0]);
	DDX_Text(pDX, IDCANCEL, m_sText[1]);
	DDX_Control(pDX, IDC_TREE1, m_tcItems);
}

BEGIN_MESSAGE_MAP(CDlgNewItem, CDialogEx)
	ON_WM_PAINT()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CDlgNewItem::OnTvnSelchangedTree1)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CDlgNewItem::OnNMDblclkTree1)
END_MESSAGE_MAP()

static void DrawGrid(CPaintDC & dc, const CRect & rect) {
	CPen pen(PS_SOLID, 1, RGB(0, 200, 100));
	CPen * pOld = dc.SelectObject(&pen);
	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(rect.right, rect.top);
	dc.LineTo(rect.right, rect.bottom);
	dc.LineTo(rect.left, rect.bottom);
	dc.LineTo(rect.left, rect.top);
	dc.SelectObject(pOld);
}

void CDlgNewItem::InitUI() {
	//item tree
	int idx = 0;
	for (auto & c : ::theApp.ItemCatagories()) {
		auto p = m_tcItems.InsertItem(::theApp.ItemCatagoryName(idx++));
		m_tcItems.SetItemData(p, 0);
		for (auto & m : c) {
			auto it = m_tcItems.InsertItem(::theApp.ItemName(m.NameIndex), p);
			m_tcItems.SetItemData(it, m.dwTypeID);
		}
	}
}

void CDlgNewItem::LoadText(void) {
	int index = 0;
	SetWindowText(::theApp.NewItemUI(index++));
	for (auto & s : m_sText)
		s = ::theApp.NewItemUI(index++);
	UpdateData(FALSE);
}

// CDlgNewItem 消息处理程序

BOOL CDlgNewItem::OnInitDialog() {
	CDialogEx::OnInitDialog();
	InitUI();
	LoadText();
	return TRUE;
}

void DrawItemXY(CPaintDC & dc, int pic, CRect rect) {
	CBitmap bmp;
	bmp.LoadBitmap(pic);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap* pOld = memDC.SelectObject(&bmp);
	auto sz = rect.Size();
	dc.BitBlt(rect.left, rect.top, sz.cx, sz.cy, &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOld);
}

void CDlgNewItem::OnPaint() {
	CPaintDC dc(this);
	DrawGrid(dc, GRID_RECT);
	if (m_iDisplayItemPicIndex >= 0)
		DrawItemXY(dc, m_iDisplayItemPicIndex, m_rectDisplayItem);
}

void CDlgNewItem::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	const DWORD id = m_tcItems.GetItemData(pNMTreeView->itemNew.hItem);
	if (id > 0) {
		//Display item
		auto meta = ::theApp.ItemMetaData(id);
		ASSERT(meta != 0);
		m_iDisplayItemPicIndex = IDB_BITMAP0 + meta->PicIndex;
		const int w = ((meta->Range >> 4) & 0xF) * GRID_WIDTH, h = (meta->Range & 0xF) * GRID_WIDTH;
		const int x = (GRID_RECT.left + GRID_RECT.right - w) / 2, y = (GRID_RECT.top + GRID_RECT.bottom - h) / 2;
		m_rectDisplayItem = { x, y, x + w, y + h };
	} else {
		m_iDisplayItemPicIndex = -1;
	}
	InvalidateRect(GRID_RECT);
	*pResult = 0;
}

void CDlgNewItem::OnOK() {
	const auto it = m_tcItems.GetSelectedItem();
	if (it) {
		const DWORD id = m_tcItems.GetItemData(it);
		if(id > 0)
			m_pItem = make_unique<CD2Item>(id);
	}
	CDialogEx::OnOK();
}

void CDlgNewItem::OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult) {
	const auto it = m_tcItems.GetSelectedItem();
	if (it) {
		const DWORD id = m_tcItems.GetItemData(it);
		if (id > 0)
			OnOK();
	}
	*pResult = 0;
}
