//
// WAV to AC3 Encoder
// Copyright (C) 2007-2016 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

#pragma once

const CSize AnchorNone(-1, -1);
const CSize AnchorTopLeft(0, 0);
const CSize AnchorTopCenter(50, 0);
const CSize AnchorTopRight(100, 0);
const CSize AnchorMiddleLeft(0, 50);
const CSize AnchorMiddleCenter(50, 50);
const CSize AnchorMiddleRight(100, 50);
const CSize AnchorBottomLeft(0, 100);
const CSize AnchorBottomCenter(50, 100);
const CSize AnchorBottomRight(100, 100);

class CResizeDialog : public CDialogEx
{
private:
    typedef struct
    {
        HWND hWnd;
        BOOL bAdjHscroll;
        SIZE typeTL, marginTL;
        SIZE typeBR, marginBR;
    } Layout;
private:
    void InitLayout(Layout &layout, HWND hw, SIZE tTL, SIZE mTL, SIZE tBR, SIZE mBR, BOOL hscroll)
    {
        layout.hWnd = hw;
        layout.typeTL = tTL;
        layout.marginTL = mTL;
        layout.typeBR = tBR;
        layout.marginBR = mBR;
        layout.bAdjHscroll = hscroll;
    };
private:
    BOOL m_bUseMaxTrack, m_bUseMinTrack, m_bUseMaxRect;
    BOOL m_bInitDone;
    POINT m_ptMinTrackSize, m_ptMaxTrackSize;
    POINT m_ptMaxPos, m_ptMaxSize;
    CList<Layout, Layout> m_LayoutList;
private:
    void InitVars();
public:
    void CleanUp();
protected:
    virtual BOOL OnInitDialog();
protected:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI);
    afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
    DECLARE_MESSAGE_MAP()
public:
    CResizeDialog();
    CResizeDialog(UINT nIDTemplate, CWnd *pParentWnd = NULL);
    CResizeDialog(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL);
    virtual ~CResizeDialog();
public:
    void UpdateLayout();
    void ArrangeLayout();
    void AddAnchor(HWND newWnd, CSize typeTL, CSize typeBR = AnchorNone);
    void AddAnchor(UINT nCtrlID, CSize typeTL, CSize typeBR = AnchorNone);
    void SetMaximizedRect(const CRect &rc);
    void ResetMaximizedRect();
    void SetMinTrackSize(const CSize &size);
    void ResetMinTrackSize();
    void SetMaxTrackSize(const CSize &size);
    void ResetMaxTrackSize();
    void UpdateWindowPos(HWND hWnd, CRect newRC);
    CString GetWindowRectStr();
    void SetWindowRectStr(CString szData);
};
