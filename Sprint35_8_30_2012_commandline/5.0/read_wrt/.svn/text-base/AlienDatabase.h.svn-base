// $Header: /CAMCAD/4.5/read_wrt/AlienDatabase.h 13    8/18/06 8:17p Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2002. All Rights Reserved.

#if ! defined (__AlienDatabase_h__)
#define __AlienDatabase_h__

#include "CamCadDatabase.h"
#include "RwLib.h"

//_____________________________________________________________________________
class CAlienDatabase : public CObject
{
private:
   FileTypeTag m_databaseTypeId;  // values defined in dbutil.h, e.g. Type_Alcatel_GMF
   double m_inchesPerUnit;
   double m_pageUnitsPerUnit;
   CFilePath m_databaseFilePath;
   FileStruct* m_boardFile;
   FileStruct* m_panelFile;
   CCamCadDatabase& m_camCadDatabase;

public:
   CAlienDatabase(CCamCadDatabase& camCadDatabase,FileTypeTag databaseTypeId);
   virtual ~CAlienDatabase();

   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }
   CCamCadData& getCamCadData() { return getCamCadDatabase().getCamCadData(); }
   const CFilePath& getDatabaseFilePath() { return m_databaseFilePath; }
   void setDatabaseFilePath(const CString& databaseFilePath) { m_databaseFilePath.setPath(databaseFilePath); }
   double getInchesPerUnit() { return m_inchesPerUnit; }
   double getPageUnitsPerUnit() { return m_pageUnitsPerUnit; }
   void setInchesPerUnit(double inchesPerUnit);
   void convertToInches(CPoint2d& coordinate);
   void convertToInches(double& unit);
   void convertToInches(int& unit);
   double toInches(double unit);
   double toInches(const CString& unitString);
   double toInches(double unit,PageUnitsTag fromUnits);
   double toInches(const CString& unitString,PageUnitsTag fromUnits);
   double toPageUnits(double unit);
   double toPageUnits(const CString& unitString);
   double toPageUnits(double unit,PageUnitsTag fromUnits);
   double toPageUnits(const CString& unitString,PageUnitsTag fromUnits);

   CString getBoardName() { return m_databaseFilePath.getFileName(); }
   FileStruct* getBoardFile();
   FileStruct* getPanelFile();
   BlockStruct* getBoardBlock() { return getBoardFile()->getBlock(); }
   BlockStruct* getPanelBlock() { return getPanelFile()->getBlock(); }
   int getBoardFileNum() { return getBoardFile()->getFileNumber(); }
   int getPanelFileNum() { return getPanelFile()->getFileNumber(); }

   virtual bool read(const CString& filePath,PageUnitsTag pageUnits,CWriteFormat& errorLog) = 0;
};

#endif


