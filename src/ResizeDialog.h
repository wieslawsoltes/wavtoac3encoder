//
// WAV to AC3 Encoder
// Copyright (C) 2007,2008,2009,2010 Wiesław Šoltés <wieslaw.soltes@gmail.com>
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

const CSize AnchorNone         ( -1,  -1);
const CSize AnchorTopLeft      (  0,   0);
const CSize AnchorTopCenter    ( 50,   0); 
const CSize AnchorTopRight     (100,   0);
const CSize AnchorMiddleLeft   (  0,  50);
const CSize AnchorMiddleCenter ( 50,  50); 
const CSize AnchorMiddleRight  (100,  50);
const CSize AnchorBottomLeft   (  0, 100);
const CSize AnchorBottomCenter ( 50, 100);
const CSize AnchorBottomRight  (100, 100);

class CResizeDialog : public CDialog
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
private:
    BOOL m_bInitDone;
private:
    POINT m_ptMinTrackSize, m_ptMaxTrackSize;
private:
    POINT m_ptMaxPos, m_ptMaxSize;
private:
    CList<Layout,Layout> m_LayoutList;
private:
    void InitVars();
public:
    void CleanUp();
protected:
    virtual BOOL OnInitDialog();
protected:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI);
protected:
    afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
    DECLARE_MESSAGE_MAP()
public:
    CResizeDialog();
public:
    CResizeDialog(UINT nIDTemplate, CWnd *pParentWnd = NULL);
public:
    CResizeDialog(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL);
public:
    virtual ~CResizeDialog();
public:
    void UpdateLayout();
public:
    void ArrangeLayout();
public:
    void AddAnchor(HWND newWnd, CSize typeTL, CSize typeBR = AnchorNone);
public:
    void AddAnchor(UINT nCtrlID, CSize typeTL, CSize typeBR = AnchorNone);
public:
    void SetMaximizedRect(const CRect &rc); 
public:
    void ResetMaximizedRect();
public:
    void SetMinTrackSize(const CSize &size);
public:
    void ResetMinTrackSize();
public:
    void SetMaxTrackSize(const CSize &size);
public:
    void ResetMaxTrackSize();
public:
    void UpdateWindowPos(HWND hWnd, CRect newRC);
public:
    CString GetWindowRectStr();
public:
    void SetWindowRectStr(CString szData);
};
