// $Header: /CAMCAD/4.5/Prj_Path.h 12    5/20/05 11:37a Lynn Phung $

#if !defined(AFX_PRJ_PATH_H__2F6A21B0_6D28_11D3_865D_004005408E44__INCLUDED_)
#define AFX_PRJ_PATH_H__2F6A21B0_6D28_11D3_865D_004005408E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// ProjectPath dialog
class ProjectPath : public CDialog
{
// Construction
public:
   ProjectPath(CWinApp *app, CWnd* pParent = NULL);   // standard constructor
   enum { IDD = IDD_PROJECT_PATH };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBrowse();
   afx_msg void OnAdd();
   afx_msg void OnRemove();
   afx_msg void OnSelect();
	afx_msg void OnReset();
	afx_msg void OnProjectPathOptionClicked();

private:
   CWinApp* m_app;
   CListBox m_list;
	CString m_pathByfirstFileLoaded;
   CString m_pathByCurrentSelection;
	BOOL m_projectPathOption;

	void fillList();

public:
	CString GetPathByFirstFileLoaded () const				{ return m_pathByfirstFileLoaded;		}
	CString GetPathByCurrentSelection () const			{ return m_pathByCurrentSelection;		}
	int GetProjectPathOption() const							{ return m_projectPathOption;				}

	void SetPathByFirstFileLoaded(const CString path)	{ m_pathByfirstFileLoaded = path;		}
	void SetPathByCurrentSelection(const CString path)	{ m_pathByCurrentSelection = path;		}
	void SetProjectPathOption(const int option)			
	{
		if (option < 0 || m_projectPathOption > 1)
			m_projectPathOption = 0;
		else
			m_projectPathOption = option;
	}
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJ_PATH_H__2F6A21B0_6D28_11D3_865D_004005408E44__INCLUDED_)
