//
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
#include "MyListCtrl.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMyListCtrl, CListCtrl)

CMyListCtrl::CMyListCtrl()
{
    bUseTooltipsList = false;

    this->szToolTipText = _T("");
    this->bHaveToolTipText = false;
}

CMyListCtrl::~CMyListCtrl()
{
    listTooltips.RemoveAll();
}

void CMyListCtrl::PreSubclassWindow() 
{
    CListCtrl::PreSubclassWindow();

    EnableToolTips(TRUE);
}

BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
    ON_WM_CREATE()
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

void CMyListCtrl::SetTooltipText(CString szText)
{
    this->szToolTipText = szText;
    this->bHaveToolTipText = true;
}

CString CMyListCtrl::GetTooltipText()
{
    return this->szToolTipText;
}

void CMyListCtrl::DelTooltipText()
{
    this->bHaveToolTipText = false;
    this->szToolTipText = _T("");
}

bool CMyListCtrl::HaveTooltipText()
{
    return this->bHaveToolTipText;
}

int CMyListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if(CListCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

INT_PTR CMyListCtrl::OnToolHitTest(CPoint point, TOOLINFO *pTI) const
{
    int nRow, nCol;
    RECT rcCell;
    nRow = CellRectFromPoint(point, &rcCell, &nCol);

    if(nRow == -1) 
        return -1;

    pTI->hwnd = m_hWnd;
    pTI->uId = (UINT) ((nRow << 10) + (nCol & 0x3ff) + 1);
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    pTI->rect = rcCell;

    return pTI->uId;
}

int CMyListCtrl::CellRectFromPoint(CPoint &point, RECT *rcCell, int *nCol) const
{
    int nColNum;
    CHeaderCtrl *pHeader;
    int nColumnCount;

    if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
    {
        nColumnCount = 1;
    }
    else
    {
        pHeader = (CHeaderCtrl*) GetDlgItem(0);
        nColumnCount = pHeader->GetItemCount();
    }

    int nRow = GetTopIndex();
    int nBottom = nRow + GetCountPerPage();
    if(nBottom > GetItemCount())
        nBottom = GetItemCount();

    for(; nRow <= nBottom; nRow++)
    {
        CRect rcTemp, rcLabel;
        if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
        {
            GetItemRect(nRow, &rcTemp, LVIR_ICON);
            GetItemRect(nRow, &rcLabel, LVIR_LABEL);
        }
        else
        {
            GetItemRect(nRow, &rcTemp, LVIR_BOUNDS);
        }

        if(rcTemp.PtInRect(point) || rcLabel.PtInRect(point))
        {
            bool bIsInLabel = false;
            if(rcTemp.PtInRect(point))
                bIsInLabel = false;
            else if(rcLabel.PtInRect(point))
                bIsInLabel = true;

            for(nColNum = 0; nColNum < nColumnCount; nColNum++)
            {
                // get column width for report style
                int nColWidth = 100000;
                if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) == LVS_REPORT)
                    nColWidth = GetColumnWidth(nColNum);

                if((point.x >= rcTemp.left && point.x <= (rcTemp.left + nColWidth)) 
                    || (point.x >= rcLabel.left && point.x <= (rcLabel.left + nColWidth)))
                {
                    RECT rcClient;
                    GetClientRect(&rcClient);

                    if(nCol) 
                        *nCol = nColNum;

                    rcTemp.right = rcTemp.left + nColWidth;
                    rcLabel.right = rcLabel.left + nColWidth;

                    if(rcTemp.right > rcClient.right) 
                        rcTemp.right = rcClient.right;

                    if(rcLabel.right > rcClient.right)
                        rcLabel.right = rcClient.right;

                    if(!bIsInLabel)
                        *rcCell = rcTemp;
                    else
                        *rcCell = rcLabel;

                    return nRow;
                }

                rcTemp.left += nColWidth;
                rcLabel.left += nColWidth;
            }
        }
    }

    return -1;
}

BOOL CMyListCtrl::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
    TOOLTIPTEXTA *pTTTA = (TOOLTIPTEXTA *) pNMHDR;
    TOOLTIPTEXTW *pTTTW = (TOOLTIPTEXTW *) pNMHDR;
    static CString szTipText;
    UINT nID = pNMHDR->idFrom;

    if(nID == 0)
        return FALSE;

    int nRow = ((nID - 1) >> 10) & 0x3fffff;
    int nCol = (nID - 1) & 0x3ff;

    if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) == LVS_REPORT && bUseTooltipsList == true)
    {
        // tooltips are the same for all columns
        szTipText = listTooltips.GetAt(listTooltips.FindIndex(nRow)); 
    }
    else
    {
        szTipText = GetItemText(nRow, nCol);
    }

    // length of tooltips buffer
    static const int nMyTooltipsWidth = 4096;

    // check the tooltip text length
    if(szTipText.GetLength() > nMyTooltipsWidth)
        return FALSE;
    
    ::SendMessage(pNMHDR->hwndFrom, TTM_SETMAXTIPWIDTH, 0, (LPARAM) nMyTooltipsWidth);

    TCHAR szBuff[nMyTooltipsWidth] = _T("");
    _stprintf(szBuff, _T("%s"), szTipText);

#ifndef _UNICODE
    if(pNMHDR->code == TTN_NEEDTEXTA)
    {
        pTTTA->lpszText = szBuff;
    }
    else
    {
        wchar_t szTmpBuff[nMyTooltipsWidth];
        _mbstowcsz(szTmpBuff, szBuff, szTipText.GetLength() + 1);
        pTTTW->lpszText = szTmpBuff;
    }
#else
    if(pNMHDR->code == TTN_NEEDTEXTA)
    {
        char szTmpBuff[nMyTooltipsWidth];
        _wcstombsz(szTmpBuff, szBuff, szTipText.GetLength() + 1);
        pTTTA->lpszText = szTmpBuff;
    }
    else
    {
        pTTTW->lpszText = szBuff;
    }
#endif

    *pResult = 0;
    return TRUE;
}
