// DockablePaneBase.cpp : implementation file
//

#include "stdafx.h"
#include "DockablePaneBase.h"
#include "DockablePaneChildView.h"
#include "controller.h"


// CDockablePaneBase2
CDockablePaneBase2::CDockablePaneBase2() :
m_dlg(NULL)
{
}

CDockablePaneBase2::CDockablePaneBase2(CDockablePaneChildView2 *childView, UINT resourceID)
	: m_dlg(childView)
	, m_dlgResourceID(resourceID)
{
}

CDockablePaneBase2::~CDockablePaneBase2()
{
	SAFE_DELETE(m_dlg);
}


BEGIN_MESSAGE_MAP(CDockablePaneBase2, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEACTIVATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CDockablePaneBase2 message handlers

int CDockablePaneBase2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	//cController::Get()->GetSerialComm().AddObserver(this);

	if (m_dlg)
	{
		m_dlg->Create(m_dlgResourceID, this);
		m_dlg->ShowWindow(SW_SHOW);
	}

	return 0;
}


// ������ ũ�Ⱑ �ٲ��, �ڽ� �䵵 ���� �ٲ��.
void CDockablePaneBase2::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (m_dlg && GetSafeHwnd())
	{
		m_dlg->MoveWindow(CRect(0, 0, cx, cy));
	}
}


// �޴�â �ȶ߰� ��.
void CDockablePaneBase2::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// nothing~
}


// CDockablePane �ڽ����� CView�� ���� ��, OnMouseActivate() �Լ����� �״¹����� �ذ��ϱ� ���� �ڵ��.
// https://social.msdn.microsoft.com/Forums/en-US/e496c4da-221e-4c12-88d0-8b351a3a8986/cdockablepane-assertion-error-on-onmouseactivate?forum=vcgeneral
int CDockablePaneBase2::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	CFrameWnd* pParentFrame = GetParentFrame();
	if ((pParentFrame == pDesktopWnd) || (pDesktopWnd->IsChild(pParentFrame)))
	{
		return CDockablePane::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}
	return MA_NOACTIVATE;
}


BOOL CDockablePaneBase2::OnEraseBkgnd(CDC* pDC)
{
	//return __super::OnEraseBkgnd(pDC);
	return TRUE;
}