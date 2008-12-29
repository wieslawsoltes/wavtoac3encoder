//
// WAV to AC3 Encoder
// Copyright (C) 2007, 2008, 2009 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

#define WM_MY_EN_CHANGE WM_USER + 0x20

class CMyComboBoxEdit : public CEdit
{
    DECLARE_DYNAMIC(CMyComboBoxEdit)
public:
    CMyComboBoxEdit();
public:
    virtual ~CMyComboBoxEdit();
protected:
    virtual void PreSubclassWindow();
protected:
    DECLARE_MESSAGE_MAP()
protected:
    afx_msg BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
public:
    virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
public:
    afx_msg void OnEnChange();
private:
    CString szToolTipText;
private:
    bool bHaveToolTipText;
public:
    void SetTooltipText(CString szText);
public:
    CString GetTooltipText();
public:
    void DelTooltipText();
public:
    bool HaveTooltipText();
};
