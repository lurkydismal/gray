#if !defined(AFX_LOGINDLG_H__58A8C6B5_2AD3_4C62_882E_DC1F8D104CC5__INCLUDED_)
#define AFX_LOGINDLG_H__58A8C6B5_2AD3_4C62_882E_DC1F8D104CC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CDialog
{
// Construction
public:
    CLoginDlg(CWnd* pParent = NULL);   // standard constructor
    SamplePlayerData m_playerData; // global player data


// Dialog Data
    //{{AFX_DATA(CLoginDlg)
    enum { IDD = IDD_LOGIN };
    CString    m_email;
    CString    m_nick;
    CString    m_password;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLoginDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CLoginDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINDLG_H__58A8C6B5_2AD3_4C62_882E_DC1F8D104CC5__INCLUDED_)
