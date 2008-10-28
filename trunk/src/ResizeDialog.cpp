﻿//
// WAV to AC3 Encoder
// Copyright (C) 2007-2008 Wiesław Šoltés <wieslaw.soltes@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/* $Id$ */

#include "stdafx.h"
#include "EncWAVtoAC3.h"
#include "ResizeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CResizeDialog::CResizeDialog()
{
    InitVars();
}

CResizeDialog::CResizeDialog(UINT nIDTemplate, CWnd *pParentWnd)
: CDialog(nIDTemplate, pParentWnd)
{
    InitVars();
}

CResizeDialog::CResizeDialog(LPCTSTR lpszTemplateName, CWnd *pParentWnd)
: CDialog(lpszTemplateName, pParentWnd)
{
    InitVars();
}

CResizeDialog::~CResizeDialog()
{
    m_LayoutList.RemoveAll();
}

BEGIN_MESSAGE_MAP(CResizeDialog, CDialog)
    ON_WM_GETMINMAXINFO()
    ON_WM_SIZE()
END_MESSAGE_MAP()

void CResizeDialog::InitVars() 
{
    m_bInitDone = FALSE;
    m_bUseMinTrack = TRUE;
    m_bUseMaxTrack = FALSE;
    m_bUseMaxRect = FALSE;
}

void CResizeDialog::CleanUp() 
{
    m_bInitDone = FALSE;

    m_LayoutList.RemoveAll();

    m_bInitDone = TRUE;
}

BOOL CResizeDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();

    CRect rc;
    GetWindowRect(&rc);

    m_ptMinTrackSize.x = rc.Width();
    m_ptMinTrackSize.y = rc.Height();

    m_bInitDone = TRUE;

    return TRUE;
}

void CResizeDialog::OnSize(UINT nType, int cx, int cy) 
{
    CWnd::OnSize(nType, cx, cy);

    if((nType == SIZE_MAXHIDE) || (nType == SIZE_MAXSHOW))
        return;

    if(m_bInitDone)
        ArrangeLayout();
}

void CResizeDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI) 
{
    if(!m_bInitDone)
        return;

    if(m_bUseMinTrack)
        lpMMI->ptMinTrackSize = m_ptMinTrackSize;

    if(m_bUseMaxTrack)
        lpMMI->ptMaxTrackSize = m_ptMaxTrackSize;

    if(m_bUseMaxRect)
    {
        lpMMI->ptMaxPosition = m_ptMaxPos;
        lpMMI->ptMaxSize = m_ptMaxSize;
    }
}

void CResizeDialog::AddAnchor(HWND newWnd, CSize typeTL, CSize typeBR)
{
    ASSERT((newWnd != NULL) && ::IsWindow(newWnd));
    ASSERT(::IsChild(*this, newWnd));
    ASSERT(typeTL != AnchorNone);

    CString szStatic;
    GetClassName(newWnd, szStatic.GetBufferSetLength(MAX_PATH), MAX_PATH);
    szStatic.ReleaseBuffer();
    szStatic.MakeUpper();

    if(szStatic == _T("BUTTON"))
    {
        DWORD dwStyle = GetWindowLong(newWnd, GWL_STYLE);
        if(dwStyle & BS_GROUPBOX)
            SetWindowLong(newWnd, GWL_STYLE, dwStyle | WS_CLIPSIBLINGS);
    }

    BOOL bHScroll = FALSE;
    if(szStatic == _T("LISTBOX"))
        bHScroll = TRUE;

    CRect wndrc, objrc;
    GetClientRect(&wndrc);
    ::GetWindowRect(newWnd, &objrc);
    ScreenToClient(&objrc);

    CSize marginTL, marginBR;

    if(typeBR == AnchorNone)
        typeBR = typeTL;

    marginTL.cx = objrc.left   - wndrc.Width()  * typeTL.cx / 100;
    marginTL.cy = objrc.top    - wndrc.Height() * typeTL.cy / 100;
    marginBR.cx = objrc.right  - wndrc.Width()  * typeBR.cx / 100;
    marginBR.cy = objrc.bottom - wndrc.Height() * typeBR.cy / 100;

    Layout layout;
    this->InitLayout(layout, newWnd, typeTL, marginTL, typeBR, marginBR, bHScroll);
    m_LayoutList.AddTail(layout);
}

void CResizeDialog::AddAnchor(UINT nCtrlID, CSize typeTL, CSize typeBR)
{
    AddAnchor(::GetDlgItem(*this, nCtrlID), typeTL, typeBR);
}

void CResizeDialog::UpdateLayout()
{
    POSITION pos = m_LayoutList.GetHeadPosition();
    while(pos != NULL)
    {
        Layout layout = m_LayoutList.GetNext(pos);
        Layout layoutUpdate;

        if(pos == NULL)
            break;

        CRect wndrc, objrc;
        GetClientRect(&wndrc);
        ::GetWindowRect(layout.hWnd, &objrc);
        ScreenToClient(&objrc);

        CSize marginTL, marginBR;

        marginTL.cx = objrc.left   - wndrc.Width()  * layout.typeTL.cx / 100;
        marginTL.cy = objrc.top    - wndrc.Height() * layout.typeTL.cy / 100;
        marginBR.cx = objrc.right  - wndrc.Width()  * layout.typeBR.cx / 100;
        marginBR.cy = objrc.bottom - wndrc.Height() * layout.typeBR.cy / 100;

        this->InitLayout(layoutUpdate, 
            layout.hWnd, 
            layout.typeTL, marginTL, 
            layout.typeBR, marginBR, 
            layout.bAdjHscroll);

        m_LayoutList.SetAt(pos, layoutUpdate);
    }
}

void CResizeDialog::ArrangeLayout()
{
    CRect wndRC;
    GetClientRect(&wndRC);

    HDWP hdwp = BeginDeferWindowPos(m_LayoutList.GetCount());

    POSITION pos = m_LayoutList.GetHeadPosition();
    while(pos != NULL)
    {
        Layout layout = m_LayoutList.GetNext(pos);

        CRect objrc, newrc;
        CWnd *wnd = CWnd::FromHandle(layout.hWnd);

        wnd->GetWindowRect(&objrc);
        ScreenToClient(&objrc);

        newrc.left   = layout.marginTL.cx + wndRC.Width()  * layout.typeTL.cx / 100;
        newrc.top    = layout.marginTL.cy + wndRC.Height() * layout.typeTL.cy / 100;
        newrc.right  = layout.marginBR.cx + wndRC.Width()  * layout.typeBR.cx / 100;
        newrc.bottom = layout.marginBR.cy + wndRC.Height() * layout.typeBR.cy / 100;

        if(!newrc.EqualRect(&objrc))
        {
            if(layout.bAdjHscroll)
            {
                int nDiff = newrc.Width() - objrc.Width();
                int nMax = wnd->GetScrollLimit(SB_HORZ);
                if((nMax > 0) && (wnd->GetScrollPos(SB_HORZ) > (nMax - nDiff)))
                {
                    wnd->MoveWindow(&newrc);
                    wnd->Invalidate();
                }
            }

            DeferWindowPos(hdwp, layout.hWnd, 
                NULL, 
                newrc.left, newrc.top,
                newrc.Width(), newrc.Height(), 
                SWP_NOZORDER | SWP_NOACTIVATE);

        }
    }

    EndDeferWindowPos(hdwp);
}

void CResizeDialog::SetMaximizedRect(const CRect& rc)
{
    m_bUseMaxRect = TRUE;
    m_ptMaxPos = rc.TopLeft();
    m_ptMaxSize.x = rc.Width();
    m_ptMaxSize.y = rc.Height();
}

void CResizeDialog::ResetMaximizedRect()
{
    m_bUseMaxRect = FALSE;
}

void CResizeDialog::SetMinTrackSize(const CSize &size)
{
    m_bUseMinTrack = TRUE;
    m_ptMinTrackSize.x = size.cx;
    m_ptMinTrackSize.y = size.cy;
}

void CResizeDialog::ResetMinTrackSize()
{
    m_bUseMinTrack = FALSE;
}

void CResizeDialog::SetMaxTrackSize(const CSize &size)
{
    m_bUseMaxTrack = TRUE;
    m_ptMaxTrackSize.x = size.cx;
    m_ptMaxTrackSize.y = size.cy;
}

void CResizeDialog::ResetMaxTrackSize()
{
    m_bUseMaxTrack = FALSE;
}

void CResizeDialog::UpdateWindowPos(HWND hWnd, CRect newRC)
{
    HDWP hDwp = BeginDeferWindowPos(1);

    DeferWindowPos(hDwp, hWnd, 
                NULL, 
                newRC.left, newRC.top,
                newRC.Width(), newRC.Height(), 
                SWP_NOZORDER | SWP_NOACTIVATE);

    EndDeferWindowPos(hDwp);
}

CString CResizeDialog::GetWindowRectStr()
{
    CString szData;
    WINDOWPLACEMENT wp;

    ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
    wp.length = sizeof(WINDOWPLACEMENT);

    GetWindowPlacement(&wp);
    RECT &rc = wp.rcNormalPosition;

    szData.Format(_T("%d %d %d %d %d %d"), 
        rc.left, rc.top, 
        rc.right, rc.bottom, 
        wp.showCmd, wp.flags);

    return szData;
}

void CResizeDialog::SetWindowRectStr(CString szData)
{
    WINDOWPLACEMENT wp;

    if(szData.IsEmpty())
        return;

    ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
    wp.length = sizeof(WINDOWPLACEMENT);

    GetWindowPlacement(&wp);
    RECT &rc = wp.rcNormalPosition;

    if(_stscanf(szData, _T("%d %d %d %d %d %d"), 
        &rc.left, &rc.top, 
        &rc.right, &rc.bottom, 
        &wp.showCmd, &wp.flags) == 6)
    {
        SetWindowPlacement(&wp);
    }
}
