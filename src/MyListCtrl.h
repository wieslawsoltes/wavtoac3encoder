//
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

#pragma once

class CMyListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(CMyListCtrl)
public:
    CMyListCtrl();
    virtual ~CMyListCtrl();
protected:
    afx_msg void PreSubclassWindow();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
protected:
    DECLARE_MESSAGE_MAP()
protected:
    virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
private:
    CMyListCtrl& operator=(const CMyListCtrl &x);
    CMyListCtrl(const CMyListCtrl &x);
public:
    bool bUseTooltipsList; // use only for LVS_REPORT style
    CList<CString,CString&> listTooltips; 
private:
    CString szToolTipText;
    bool bHaveToolTipText;
public:
    void SetTooltipText(CString szText);
    CString GetTooltipText();
    void DelTooltipText();
    bool HaveTooltipText();
};
