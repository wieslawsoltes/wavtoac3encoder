﻿//
// WAV to AC3 Encoder
// Copyright (C) 2007-2013 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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
#include "MyButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMyButton::CMyButton()
{
    this->bIsBold = false;

    this->szToolTipText = _T("");
    this->bHaveToolTipText = false;
}

CMyButton::~CMyButton()
{
    m_BoldFont.DeleteObject();
    m_StdFont.DeleteObject();
}

void CMyButton::PreSubclassWindow() 
{
    CFont* pFont = GetFont();
    if(!pFont)
    {
        HFONT hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
        if(hFont == NULL)
            hFont = (HFONT) GetStockObject(ANSI_VAR_FONT);
        if(hFont)
            pFont = CFont::FromHandle(hFont);
    }

    ASSERT(pFont->GetSafeHandle());

    LOGFONT lf;

    pFont->GetLogFont(&lf);
    m_StdFont.CreateFontIndirect(&lf);

    lf.lfWeight = FW_BOLD;
    m_BoldFont.CreateFontIndirect(&lf);

    SetBold(bIsBold);

    CButton::PreSubclassWindow();

    EnableToolTips(TRUE);
}

BEGIN_MESSAGE_MAP(CMyButton, CButton)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

void CMyButton::SetTooltipText(CString szText)
{
    this->szToolTipText = szText;
    this->bHaveToolTipText = true;
}

CString CMyButton::GetTooltipText()
{
    return this->szToolTipText;
}

void CMyButton::DelTooltipText()
{
    this->bHaveToolTipText = false;
    this->szToolTipText = _T("");
}

bool CMyButton::HaveTooltipText()
{
    return this->bHaveToolTipText;
}

void CMyButton::SetBold(bool bBold)
{
    if(::IsWindow(GetSafeHwnd()))
    {
        if(bBold == true)
        {
            SetFont(&m_BoldFont);
            bIsBold = true;
        }
        else
        {
            SetFont(&m_StdFont);
            bIsBold = false;
        }
        Invalidate(); 
    }
}

bool CMyButton::GetBold()
{
    return bIsBold;
}

INT_PTR CMyButton::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
    CRect rcTemp;
    GetClientRect(rcTemp);
    if(!rcTemp.PtInRect(point))
        return -1;

    pTI->hwnd = m_hWnd;
    pTI->uId = 1;
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    pTI->rect = rcTemp;

    return pTI->uId;
    // return CButton::OnToolHitTest(point, pTI);
}

BOOL CMyButton::OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
    TOOLTIPTEXTA *pTTTA = (TOOLTIPTEXTA *) pNMHDR;
    TOOLTIPTEXTW *pTTTW = (TOOLTIPTEXTW *) pNMHDR;
    static CString szTipText;
    UINT nID = pNMHDR->idFrom;

    if(nID == 0)
        return FALSE;

    if(this->HaveTooltipText())
        szTipText = this->GetTooltipText();
    else
        GetWindowText(szTipText);

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
