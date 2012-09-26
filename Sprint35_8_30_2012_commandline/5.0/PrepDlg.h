// $Header: /CAMCAD/5.0/read_wrt/PrepDlg.h 2     01/06/09 9:51a Sharry Huang $
#if !defined(__PrepDlg_h__)
#define __PrepDlg_h__

#pragma once

#include "CamCadDatabase.h"
#include "afxwin.h"


//------------------------------------------------------------------------
enum ECCorner
{
	LowerLeft  = 0,
	LowerRight = 1,
	UpperLeft  = 2,
	UpperRight = 3,
};

enum ERotation
{
	Rot0   = 0,
	Rot90  = 1,
	Rot180 = 2,
	Rot270 = 3,
};

enum MachineLocationTag
{
   MLTag_Corner = 0,
   MLTag_PosX,
   MLTag_PosY,
   MLTag_Rotation,
   MLTag_BlockNumber,
   MLTag_Last
};

#define QMACHINELOCATION_TOP "_LOCATION_TOP"
#define QMACHINELOCATION_BOT "_LOCATION_BOTTOM"

CString GetMachineAttributeName(CString machineName, bool isBottom);
CString GetMachineAttributeName(FileTypeTag machineType, bool isBottom);

/******************************************************************************
//   saveRestoreSettings
******************************************************************************/
class saveRestoreSettings
{
private:
	CCEtoODBDoc *m_doc;
	FileStruct *m_File;
   bool  m_BottomSide;
   bool  m_IsRestructed;
   bool  m_mirroringRequired;

	CPoint2d m_originalFileInsertPt;
	double m_originalFileRotRadians;
	bool m_originalFileMirror;

public:
	saveRestoreSettings(CCEtoODBDoc *doc, FileStruct *panelFile, bool BottomSide);
   void set(CCEtoODBDoc *doc, FileStruct *panelFile){m_doc = doc; m_File = panelFile;}
	void Save(bool isRestructed, bool mirroringRequired);
	void Restore();
   bool getPlaceBottom() {return m_BottomSide;}
   void setPlaceBottom(bool isBottom){m_BottomSide = isBottom;} 
};

/******************************************************************************
//   ExportFileSettings
******************************************************************************/
class ExportFileSettings
{
public:
	ECCorner m_corner;
	ERotation m_rotation;
	double m_offsetX;
	double m_offsetY;
   int m_MachineOriginBlockNumber;
   FileTypeTag m_fileType;

private:
   FileTypeTag getMachineFileType(CString machineName);
   void ApplyCCZByMachine(FileTypeTag fileType, bool bottomSide, double& fileInsertX, double& fileInsertY, double& angle, 
      bool& isMirror, bool& isRestructed);

public:
	ExportFileSettings()	{ResetSetting();}
	ExportFileSettings(ECCorner corner, double offsetX, double offsetY, ERotation rotation)	{m_corner = corner; m_offsetX = offsetX, m_offsetY = offsetY; m_rotation = rotation; m_fileType = fileTypeUnknown;}
   
   CString GetSettingsString();
   void set(ECCorner corner, double offsetX, double offsetY, ERotation rotation){m_corner = corner; m_offsetX = offsetX, m_offsetY = offsetY; m_rotation = rotation; m_fileType = fileTypeUnknown;}
   void ResetSetting(){m_corner = LowerLeft; m_offsetX = 0.0; m_offsetY = 0.0; m_rotation = Rot0; m_MachineOriginBlockNumber = 0;m_fileType = fileTypeUnknown;}
   void setFileType(FileTypeTag filetype) {m_fileType = filetype;}

	ExportFileSettings& operator=(const CString&);
	ExportFileSettings& operator=(ExportFileSettings&);
   
   int getMachineOriginBlockNumber(){ return m_MachineOriginBlockNumber;}
   void setMachineOriginBlockNumber(int blockNumber) { m_MachineOriginBlockNumber = blockNumber;}

   DataStruct *FindMachineOriginData(CCEtoODBDoc *doc, FileStruct *file);
   //POSITION FindMachineOriginDataPosition(CCEtoODBDoc *doc, FileStruct *file);

   void ApplyToCCZ(CCEtoODBDoc *doc, FileStruct *panelFile, saveRestoreSettings& restSettings, FileTypeTag fileType);
   bool GetMachineOriginOffset(CCEtoODBDoc *doc, FileStruct *file, double &fileInsertX, double &fileInsertY, double &fileangle, bool &isMirror, bool &mirroringRequired, bool bottomSide);
 
   CString getRotationString();
   CString getCornerString();
   ECCorner getCornerTag(CString cornerstr);
   double getRotationDegree();
   ERotation getRotationTag(CString rotationstr);
};

/******************************************************************************
//   CPrepDlg Dialog
******************************************************************************/
class CPrepDlg : public CDialog
{
	DECLARE_DYNAMIC(CPrepDlg)

public:
	CPrepDlg(CString machineName, bool isBothSide);   // standard constructor
	virtual ~CPrepDlg();

// Dialog Data
	enum { IDD = IDD_PREP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	CComboBox m_cboOriginCorner;
	CComboBox m_cboRotation;
	CEdit m_txtOffsetX;
	CEdit m_txtOffsetY;
   CCEtoODBDoc * m_Doc;
	FileStruct * m_File;
	ExportFileSettings m_topFileSettings;
	ExportFileSettings m_botFileSettings;
   FileTypeTag m_fileType;
   CString  m_machineName;

private:
   saveRestoreSettings m_originSetting;
	CPoint2d m_originalOutlineLL;
	CPoint2d m_originalFileInsertPt;
	double m_originalFileRotRadians;
	bool m_originalFileMirror;
   bool  m_IsBothSide;
	int m_radioBoardSide;


protected:
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedTop();
	afx_msg void OnBnClickedBottom();

private:
   void SaveMachineLocationSettings(bool isBottom);
   void SetControls(ExportFileSettings *fileSettings);
	void GetControls();
   void RestoreFile();
   
   bool MoveToBoardOrigin(bool bottomSide);   
   void getOutlineOffset(ECCorner corner, double &newOffsetX, double &newOffsetY);

   BlockStruct* CreateOriginGeometry(CString attributeName);
   DataStruct *CreateOriginData(int blocknumber, CString attributeName);
   ExportFileSettings *getFileSettingBySide(int isBottom);

public:   
   void ApplyToGraphic(ExportFileSettings *FileSettings);
   void ApplyToGraphic(int radioBoardSide);

   ExportFileSettings * getTopFileSettings(){ return &m_topFileSettings;}
   void SetTopFileSettings(ExportFileSettings &topFile) { m_topFileSettings = topFile;}
   
   ExportFileSettings * getBotFileSettings(){ return &m_botFileSettings;}
   void SetBotFileSettings(ExportFileSettings &BotFile) { m_botFileSettings = BotFile;}
   
   FileStruct* GetFileStruct() { return m_File;}
   void SetFileStruct (FileStruct* file) { m_File = file;}
   
   CCEtoODBDoc * GetDoc(){ return m_Doc;}
   void SetDoc(CCEtoODBDoc * doc) { m_Doc = doc;}
   
   void CreateMachineOrigin(bool isBottom);
};

#endif /*__PrepDlg_h__*/