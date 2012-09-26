// $Header: /CAMCAD/4.3/AssignGraphicClassToLayer.h 4     8/12/03 9:05p Kurt Van Ness $

// AssignGraphicClassToLayer dialog
#include "ccdoc.h"

class AssignGraphicClassToLayer : public CDialog
{
   DECLARE_DYNAMIC(AssignGraphicClassToLayer)

public:
   AssignGraphicClassToLayer(CWnd* pParent = NULL);   // standard constructor
   AssignGraphicClassToLayer(CCEtoODBDoc *doc);
   virtual ~AssignGraphicClassToLayer();
// Dialog Data
   enum { IDD = IDD_LYR_GR_CLASS };
   CCEtoODBDoc *doc;

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()

public:
   CComboBox m_layerCombo;
   CComboBox m_graphicCombo;
   virtual BOOL OnInitDialog();
   afx_msg void OnBnClickedBtnAssign();
};

