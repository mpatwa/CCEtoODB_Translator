// $Header: /CAMCAD/4.5/Response.h 31    12/07/06 5:02p Rick Faltersack $
     
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#if ! defined (__Response_h__)
#define __Response_h__

#pragma once

#define EnablePartLibrary

void SendResponse(CString strKeyword, CString strValue);

#if defined(EnablePartLibrary)

bool PartLibTableExists(CString TableName);
bool PartLibCreateTable(CString TableName, CString ColumnNames); // col names separated by char(6)

#endif // defined(EnablePartLibrary)


#define EnableSonyAoiLibrary
#if defined(EnableSonyAoiLibrary)

void GetAllCatNames(CString DBName, CString DBPassword, CString& out_CatNames);
void GetAllModNames(CString DBName, CString DBPassword, CString CatID, CString& out_ModNames);

void GetBitMapName(CString DBName, CString DBPassword, CString CatID, CString  ModelID, CString& out_BitMapName);
void GetFrameSize(CString DBName, CString DBPassword, CString CatID, CString ModelID, CString& out_XSize, CString& out_YSize); 

//Not being used:
//void GetCatName(CString DBName, CString DBPassword, CString CatID, CString out_CatName) 
//void GetModName(CString DBName, CString DBPassword, CString ModID,CString CatID, CString out_ModName) 

bool GetXREFCatModel(CString DBName, CString PartNumber, CString& out_Cat, CString& out_Mod, CString& out_Explode, double& out_Angle); 
void SaveXREFCatModel(CString DBName, CString PartNumber, CString Cat, CString Mod, CString Explode, double Angle); 

void CreateDB(CString DBName, CString DBPassword, CString DBDest);

void CreateLibModTable(CString DBDest, CString RSIDBPassword, CString CatID);
void CreateLibProTable(CString DBName, CString RSIDBPassword, CString tableName);

void PutLibCatList(CString DBName, CString DBPassword, CString SourceCat,CString DestDB,  CString DestCatID); 
void PutLibModData(CString DBName, CString DBPassword, CString SourceCat, CString SourceMod, CString DestDB,  CString DestCatID, CString DestMod);
void CreateAndCopyTableLibPro(CString DBName, CString DBPassword, CString SourceCat, CString SourceMod, CString DBDest, CString DestCat, CString DestMod);

void AppendToLibCatList(CString DBName, CString CatID, CString OrderNum,  CString CatName, CString RSIDBPassword); 
void AppendToLibMod(CString DBName, CString RSIDBPassword, CString CatID, CString ModID, CString OrderNum, CString ModelName, CString UpdateFlag, CString ImageData);
void AppendToLibPro(CString DBName, CString RSIDBPassword, CString CatID, CString ModID, CString ProcNo, CString ProcClass, CString ProcData);

int TableExists(CString DBName, CString DBPassword, CString TableName);

#endif // defined(EnableSonyAoiLibrary)


void migrateStencilData2();

void SetStencilRules2Data(const CString& Entity, const CString& Designator1, const CString& Designator2, const CString& Designator3, const CString& Designator4, const CString& Designator5, const CString& RulesString);
void SetStencilRules2DataSet(const CString& RulesString);
void GetStencilRules2Data(CString& out_RulesString, const CString& Entity, const CString& Designator1, const CString& Designator2);
void GetStencilRules2DataSet(CString& out_RulesString,const CString& entityAndDesignatorsSet);

void SetStencilSetting(const CString& settingName,const CString& settingValue);
void GetStencilSetting(CString& settingValue,const CString& settingName);
void GetStencilSettings(CString& settingValues);


#endif  // defined (__Response_h__)
