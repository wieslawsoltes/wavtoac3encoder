#pragma once

#include <string>
#include <vector>
#include <afxcmn.h>
#include <afxwin.h>
#include "controls\MyDialogEx.h"
#include "controls\MyEdit.h"
#include "controls\MyListCtrl.h"
#include "utilities\StringHelper.h"
#include "utilities\Log.h"
#include "configuration\Configuration.h"

namespace dialogs
{
    class CEnginesDlg : public controls::CMyDialogEx
    {
        DECLARE_DYNAMIC(CEnginesDlg)
    public:
        CEnginesDlg(CWnd* pParent = nullptr);
        virtual ~CEnginesDlg();
        enum { IDD = IDD_DIALOG_ENGINES };
    public:
        controls::CMyListCtrl m_LstEngines;
        controls::CMyEdit m_EdtEngineName;
        controls::CMyEdit m_EdtEnginePath;
    public:
        config::CConfiguration * pConfig;
        std::vector<config::CEngine> m_Engines;
        std::wstring szCurrentFileName;
        int nCurrSel;
        volatile bool bUpdateList;
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);
        DECLARE_MESSAGE_MAP()
    public:
        virtual BOOL OnInitDialog();
    public:
        void InitLang();
        bool UpdateEngines();
        bool LoadEngines(std::wstring szFileName);
        bool SaveEngines(std::wstring szFileName);
    public:
        afx_msg void OnBnClickedButtonEnginesBrowse();
        afx_msg void OnBnClickedButtonEnginesImport();
        afx_msg void OnBnClickedButtonEnginesExport();
        afx_msg void OnBnClickedButtonEnginesAdd();
        afx_msg void OnBnClickedButtonEnginesRemove();
        afx_msg void OnLvnItemchangedListEngines(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnEnChangeEditEngineName();
        afx_msg void OnEnChangeEditEnginePath();
        afx_msg void OnLvnKeydownListEngines(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnBnClickedCancel();
        afx_msg void OnBnClickedOk();
    };
}
