// $Header: /CAMCAD/4.6/ODBC_Lib.cpp 15    5/25/07 11:42a Rick Faltersack $

#include "stdafx.h"
#include "CCEtoODB.h"     // for getApp()
#include "rwlib.h"      // for CFilePath
#include "ODBC_Lib.h"

#include "odbcinst.h"   // for MDB file create capability

//---------------------------------------------------------------

bool CDBInterface::m_allowConnection = true;

//---------------------------------------------------------------

CString CDBGenericAttributeMap::LookupValue(CString attribName)
{
   CString ignoredKey;
   CDBNameValuePair *nvp = NULL;

   if (this->Lookup(ignoredKey, nvp))
      return nvp->GetValue();

   return "";
}

void CDBGenericAttributeMap::Add(CString name, CString value)
{
   this->SetAt(name, new CDBNameValuePair(name, value));
}

//---------------------------------------------------------------

CString CDBPart::getDataString(const CString& delimiter)
{
   CString dataString;

   dataString = 
      this->getPartNumber() + delimiter +
      this->getDeviceType() + delimiter +
      this->getValue() + delimiter +
      this->getPTol() + delimiter +
      this->getNTol() + delimiter +
      this->getSubclass() + delimiter +
      this->getPinmap() + delimiter +
      this->getDescription();

   return dataString;
}

//---------------------------------------------------------------

CString CDBStencilRule::getDataString(const CString& delimiter)
{
   CString dataString;

   dataString = 
      this->getEntity() + delimiter +
      this->getDesignatorPrefix() + delimiter +
      this->getDesignatorSuffix() + delimiter +
      this->getRule() + delimiter +
      //this->getUserName() + delimiter +   // NOT USED BY APP AS OF THIS WRITING
      //this->getDateTime() + delimiter +   // DITTO
      this->getComment();

   return dataString;
}

//---------------------------------------------------------------

bool CDBTableDef::ValidateDBTableColumns(CDBInterface *db)
{
   if (db != NULL)
   {
      CStringSupArray dbTableColNames;
      if (db->GetColumnNames( this->GetTableName(), dbTableColNames))
      {
         for (int i = 0; i < this->GetColumNames().GetCount(); i++)
         {
            CString colnamei = this->GetColumNames().GetAt(i);
            if (dbTableColNames.FindNoCase( colnamei ) < 0)
            {
               // error, col name not found
               return false;
            }
         }
         // All column names found
         return true;
      }
   }

   // Something someplace wasn't found, we'll call that invalid
   return false;
}

CDBPanelTemplateIDTableDef1::CDBPanelTemplateIDTableDef1()
{
   // PanelTemplateIDTable
   // TemplateName, UserID, DateTime

   SetTableName( "PanelTemplateIDTable" );  // Note, not "1" on this name!

   AddColumnName( "TemplateName" );
   AddColumnName( "Units" );
   AddColumnName( "UserID" );
   AddColumnName( QDATETIME );  // Field type text
}

CDBPanelTemplateIDTableDef2::CDBPanelTemplateIDTableDef2()
{
   // PanelTemplateIDTable
   // TemplateName, UserID, DateTime

   SetTableName( "PanelTemplateIDTable2" );

   AddColumnName( "TemplateName" );
   AddColumnName( "Units" );
   AddColumnName( "UserID" );
   AddColumnName( QDATETIME );  // Field type datetime
}



CDBPanelTemplateToolingHoleTableDef::CDBPanelTemplateToolingHoleTableDef()
{
   // PanelTemplateToolingHoleTable
   // TemplateName, Size, OriginX, OriginY

   SetTableName( "PanelTemplateToolingHoleTable" );

   AddColumnName( "TemplateName" );
   AddColumnName( "Size" );
   AddColumnName( "OriginX" );
   AddColumnName( "OriginY" );
}


CDBPanelTemplateFiducialTableDef::CDBPanelTemplateFiducialTableDef()
{
   // PanelTemplateFiducialTable
   // TemplateName, UserID, DateTime

   SetTableName( "PanelTemplateFiducialTable" );

   AddColumnName( "TemplateName" );
   AddColumnName( "Type" );
   AddColumnName( "Size" );
   AddColumnName( "OriginX" );
   AddColumnName( "OriginY" );
   AddColumnName( "Angle" );
   AddColumnName( "Mirrored" );

}

CDBPanelTemplatePcbInstanceTableDef::CDBPanelTemplatePcbInstanceTableDef()
{
   // PanelTemplatePCBInstanceTable
   // TemplateName, UserID, DateTime

   SetTableName( "PanelTemplatePCBInstanceTable" );

   AddColumnName( "TemplateName" );
   AddColumnName( "PCBName" );
   AddColumnName( "OriginX" );
   AddColumnName( "OriginY" );
   AddColumnName( "Angle" );
   AddColumnName( "Mirrored" );
   AddColumnName( "OriginType" );
}

CDBPanelTemplatePcbArrayTableDef::CDBPanelTemplatePcbArrayTableDef()
{
   // PanelTemplatePcbArrayTable
   // TemplateName, UserID, DateTime

   SetTableName( "PanelTemplatePCBArrayTable" );

   AddColumnName( "TemplateName" );
   AddColumnName( "PCBName" );
   AddColumnName( "OriginX" );
   AddColumnName( "OriginY" );
   AddColumnName( "Angle" );
   AddColumnName( "Mirrored" );
   AddColumnName( "OriginType" );
   AddColumnName( "CountX" );
   AddColumnName( "CountY" );
   AddColumnName( "StepX" );
   AddColumnName( "StepY" );
}

CDBPanelTemplateOutlineTableDef::CDBPanelTemplateOutlineTableDef()
{
   // PanelTemplateOutlineTable
   // TemplateName, 

   SetTableName( "PanelTemplateOutlineTable" );

   AddColumnName( "TemplateName" );
   AddColumnName( "OriginX" );
   AddColumnName( "OriginY" );
   AddColumnName( "SizeX" );
   AddColumnName( "SizeY" );
}

CDBParts1TableDef::CDBParts1TableDef()
{
   // PartsTable
   // ID, PartNumber, DeviceType, Value, pTol, nTol, Subclass, PINMapping, Description

   SetTableName( "PartsTable" ); // Original, no "1" in name

   AddColumnName( "ID" );
   AddColumnName( "PartNumber" );
   AddColumnName( "DeviceType" );
   AddColumnName( "Value" );
   AddColumnName( "pTol" );
   AddColumnName( "nTol" );
   AddColumnName( "Subclass" );
   AddColumnName( "PINMapping" );
   AddColumnName( "Description" );
}

CDBParts2TableDef::CDBParts2TableDef()
{
    // Newer 4.8 style Parts2Table 

   // PartsTable2
   // ID, PartNumber, DeviceType, Value, pTol, nTol, Subclass, PINMapping, Description
   // UserName, DateTime, Comment

   SetTableName( "PartsTable2" );

   AddColumnName( "ID" );
   AddColumnName( "PartNumber" );
   AddColumnName( "DeviceType" );
   AddColumnName( "Value" );
   AddColumnName( "pTol" );
   AddColumnName( "nTol" );
   AddColumnName( "Subclass" );
   AddColumnName( "PINMapping" );
   AddColumnName( "Description" );
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // In PartsTable2 this is a text field, aka varchar
   AddColumnName( "Comment" );
}

CDBParts3TableDef::CDBParts3TableDef()
{
   // Columnn set is the same as PartsTable2, but this one has actual
   // SQL Timestamp for DateTime column, so it is a different table.

   // PartsTable3
   // ID, PartNumber, DeviceType, Value, pTol, nTol, Subclass, PINMapping, Description
   // UserName, DateTime, Comment

   SetTableName( "PartsTable3" );

   AddColumnName( "ID" );
   AddColumnName( "PartNumber" );
   AddColumnName( "DeviceType" );
   AddColumnName( "Value" );
   AddColumnName( "pTol" );
   AddColumnName( "nTol" );
   AddColumnName( "Subclass" );
   AddColumnName( "PINMapping" );
   AddColumnName( "Description" );
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // This is a genuine SQL Timestamp field
   AddColumnName( "Comment" );
}

CDBCentroidsOldTableDef::CDBCentroidsOldTableDef()
{
   // OLD TABLE - Units for xy were not controlled in this era, units were current doc page units.
   // That resulted in mix of units in DB, and no way to tell what units they were so could
   // not support units conversion. See dts0100594062.

   // CENTROIDS
   // GeomName, Algorithm, X, Y, Rot

   SetTableName( "CENTROIDS" );

   AddColumnName( "GeomName" );
   AddColumnName( "Algorithm" );
   AddColumnName( "X" );
   AddColumnName( "Y" );
   AddColumnName( "Rot" );
}

CDBCentroidsMMTableDef::CDBCentroidsMMTableDef()
{
   // NEW TABLE - Units for xy are millimeters.
   // Table form did not change from old table, just control of units.
   // Created separate table to make it easy to tell, rather than add column for units.

   // CENTROIDS
   // GeomName, Algorithm, X, Y, Rot

   SetTableName( "CENTROIDS_MM" );

   AddColumnName( "GeomName" );
   AddColumnName( "Algorithm" );
   AddColumnName( "X" );     // mm
   AddColumnName( "Y" );     // mm
   AddColumnName( "Rot" );   // degrees
}

CDBCentroidsMM2TableDef::CDBCentroidsMM2TableDef()
{
    // Newer 4.8 style CENTROIDS_MM_2 table

   // CENTROIDS_MM_2
   // GeomName, Algorithm, X, Y, Rot, UserName, DateTime, Comment

   SetTableName( "CENTROIDS_MM_2" );

   AddColumnName( "GeomName" );
   AddColumnName( "Algorithm" );
   AddColumnName( "X" );     // mm
   AddColumnName( "Y" );     // mm
   AddColumnName( "Rot" );   // degrees
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // Field is type text
   AddColumnName( "Comment" );
}

CDBCentroidsMM3TableDef::CDBCentroidsMM3TableDef()
{
    // Newer 4.8 style CENTROIDS_MM_2 table

   // CENTROIDS_MM_2
   // GeomName, Algorithm, X, Y, Rot, UserName, DateTime, Comment

   SetTableName( "CENTROIDS_MM_3" );

   AddColumnName( "GeomName" );
   AddColumnName( "Algorithm" );
   AddColumnName( "X" );     // mm
   AddColumnName( "Y" );     // mm
   AddColumnName( "Rot" );   // degrees
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // Field is type datetime
   AddColumnName( "Comment" );
}

CDBPackagesTableDef::CDBPackagesTableDef()
{
   // Older 4.6 style packages table

   // Packages
   // GeomName, PackageAlias, OutlineMethod

   SetTableName( "Packages" );

   AddColumnName( "GeomName" ); // primary key
   AddColumnName( "PackageAlias" );
   AddColumnName( "OutlineMethod" );
}

CDBPackageInfo1TableDef::CDBPackageInfo1TableDef()
{
   // Newer 4.7 style package info table

   // PackageInfo
   // Name, NameType, PackageAlias, PackageHeight, OutlineMethod

   SetTableName( "PackageInfo" );  // Original, no "1" in name

   AddColumnName( "Name" );          // Name and NameType form composite primary key
   AddColumnName( "NameType" );
   AddColumnName( "PackageSource" );
   AddColumnName( "PackageAlias" );
   AddColumnName( "PackageHeight" );
   AddColumnName( "OutlineMethod" );
   AddColumnName( "DPMO" );
}

CDBPackageInfo2TableDef::CDBPackageInfo2TableDef()
{
   // Newer 4.8 style package info table

   // PackageInfo2
   // Name, NameType, PackageAlias, PackageHeight, OutlineMethod, DPMO, Family, 
   // UserName, DateTime, Comment

   SetTableName( "PackageInfo2" );

   AddColumnName( "Name" );          // Name and NameType form composite primary key
   AddColumnName( "NameType" );
   AddColumnName( "PackageSource" );
   AddColumnName( "PackageAlias" );
   AddColumnName( "PackageHeight" );
   AddColumnName( "OutlineMethod" );
   AddColumnName( "DPMO" );
   AddColumnName( "Family" );
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // Field is text
   AddColumnName( "Comment" );
}

CDBPackageInfo3TableDef::CDBPackageInfo3TableDef()
{
   // Newer 4.8 style package info table

   // PackageInfo2
   // Name, NameType, PackageAlias, PackageHeight, OutlineMethod, DPMO, Family, 
   // UserName, DateTime, Comment

   SetTableName( "PackageInfo3" );

   AddColumnName( "Name" );          // Name and NameType form composite primary key
   AddColumnName( "NameType" );
   AddColumnName( "PackageSource" );
   AddColumnName( "PackageAlias" );
   AddColumnName( "PackageHeight" );
   AddColumnName( "OutlineMethod" );
   AddColumnName( "DPMO" );
   AddColumnName( "Family" );
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );   // Field is genuine SQL timestamp
   AddColumnName( "Comment" );
}

CDBSubclassElementTableDef::CDBSubclassElementTableDef()
{
   // SubclassElement
   // SubclassName, ElementDevType, Pin1Refname, Pin2Refname, Pin3Refname, Value, PlusTol, MinusTol

   SetTableName( "SubclassElements" );

   AddColumnName( "SubclassName" );
   AddColumnName( "ElementName" );
   AddColumnName( "ElementDevType" );
   AddColumnName( "Pin1Refname" );
   AddColumnName( "Pin2Refname" );
   AddColumnName( "Pin3Refname" );
   AddColumnName( "Pin4Refname" );
   AddColumnName( "ElementValue" );
   AddColumnName( "PlusTol" );
   AddColumnName( "MinusTol" );
}

CDBGenericAttributeTableDef::CDBGenericAttributeTableDef()
{
   // GenericAttribute
   // Name, NameType, AttribName, AttribValue

   SetTableName( "GenericAttributes" );

   AddColumnName( "Name" );          // Name and NameType form composite primary key
   AddColumnName( "NameType" );
   AddColumnName( "AttribName" );
   AddColumnName( "AttribValue" );
   AddColumnName( "UserID" );
}


CDBOutlineVertexTableDef::CDBOutlineVertexTableDef()
{
   // GenericAttribute
   // Name, NameType, AttribName, AttribValue

   SetTableName( "OutlineVertices" );

   AddColumnName( "Name" );          // Name and NameType form composite primary key
   AddColumnName( "NameType" );
   AddColumnName( "PolyNum" );
   AddColumnName( "PointNum" );
   AddColumnName( "X" );
   AddColumnName( "Y" );
   AddColumnName( "Bulge" );
}

CDBStencilSettingsTableDef::CDBStencilSettingsTableDef()
{
   // StencilSettings
   // SettingName, SettingValue

   SetTableName( "StencilSettings" );

   AddColumnName( "SettingName" );
   AddColumnName( "SettingValue" );
}

CDBStencilRules1TableDef::CDBStencilRules1TableDef()
{
   // StencilRules
   // Entity, DesignatorPrefix, DesignatorSuffix, RuleString, UserName, DateTime, Comments

   SetTableName( "StencilRules" );  // Ooriginal recipe. No "1" in name

   AddColumnName( "Entity" );
   AddColumnName( "DesignatorPrefix" );
   AddColumnName( "DesignatorSuffix" );
   AddColumnName( "RuleString" );
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // Field is text
   AddColumnName( "Comments" );
}

CDBStencilRules2TableDef::CDBStencilRules2TableDef()
{
   // StencilRules
   // Entity, Designator1, Designator2, Designator3, Designator4, Designator5, RuleString, UserName, DateTime, Comments

   SetTableName( "StencilRules2" );

   AddColumnName( "Entity" );
   AddColumnName( "Designator1" );
   AddColumnName( "Designator2" );
   AddColumnName( "Designator3" );
   AddColumnName( "Designator4" );
   AddColumnName( "Designator5" );
   AddColumnName( "RuleString" );
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // Field is text
   AddColumnName( "Comments" );
}

CDBStencilRules3TableDef::CDBStencilRules3TableDef()
{
   // StencilRules
   // Entity, Designator1, Designator2, Designator3, Designator4, Designator5, RuleString, UserName, DateTime, Comments
   // This has same content as StencilRules2, but we want the latest update (4.7 Update 4) to write to a new table
   // due to DR 609040. The most straight forward was is as a separate table definition.

   SetTableName( "StencilRules3" );

   AddColumnName( "Entity" );
   AddColumnName( "Designator1" );
   AddColumnName( "Designator2" );
   AddColumnName( "Designator3" );
   AddColumnName( "Designator4" );
   AddColumnName( "Designator5" );
   AddColumnName( "RuleString" );
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // Field is text
   AddColumnName( "Comments" );
}

CDBStencilRules4TableDef::CDBStencilRules4TableDef()
{
   // StencilRules
   // Entity, Designator1, Designator2, Designator3, Designator4, Designator5, RuleString, UserName, DateTime, Comments
   // This has same content as StencilRules2, but we want the latest update (4.7 Update 4) to write to a new table
   // due to DR 609040. The most straight forward was is as a separate table definition.

   SetTableName( "StencilRules4" );

   AddColumnName( "Entity" );
   AddColumnName( "Designator1" );
   AddColumnName( "Designator2" );
   AddColumnName( "Designator3" );
   AddColumnName( "Designator4" );
   AddColumnName( "Designator5" );
   AddColumnName( "RuleString" );
   AddColumnName( "UserName" );
   AddColumnName( QDATETIME );  // Field is datateme aka SQL Timestamp
   AddColumnName( "Comments" );
}

CDBCustomMachineTableDef::CDBCustomMachineTableDef()
{
   // CustomMachines
   // MachineName, TemplateName

   SetTableName( "CustomMachines" );

   AddColumnName( "MachineName" );
   AddColumnName( "TemplateName" );
}

CDBCustomAssemblyTemplateTableDef::CDBCustomAssemblyTemplateTableDef()
{
   // Must be consistant with GeneralSetting in AssemblyTemplate.h

   // CustomAssemblyTemplate
   // TemplateName, SeparationCharacter, WriteHeader, ExplodePanel
   // WriteTopHeader, WriteBottomHeader, TopHeaderFilePath, BottomHeaderFilePath
   // ReportOrigin, OutputUnits, NumberOfDecimals, MirrorCoordinates
   // WriteSurface, SuppressInsertAttributeKeyword, SuppressInsertAttributeValue, IncludeInsertAttributeKeyword,
   // IncludeInsertAttributeValue, WriteBoards, WriteSmdComponents, WriteThruComponents,
   // WriteFiducials, WriteToolingHoles, WriteMechanicalComponents, WriteGenericComponents,
   // WriteXouts, WriteTestPoints, WriteTestProbes

   SetTableName( "CustomAssemblyTemplate" );

   AddColumnName( "TemplateName" );
   AddColumnName( "SeparationCharacter" );
   AddColumnName( "WriteHeader" );
   AddColumnName( "ExplodePanel" );
   AddColumnName( "WriteTopHeader" );
   AddColumnName( "WriteBottomHeader" );
   AddColumnName( "TopHeaderFilePath" );
   AddColumnName( "BottomHeaderFilePath" );
   AddColumnName( "ReportOrigin" ); 
   AddColumnName( "OutputUnits" ); 
   AddColumnName( "NumberOfDecimals" ); 
   AddColumnName( "MirrorCoordinates" ); 
   AddColumnName( "WriteSurface" ); 
   AddColumnName( "SuppressInsertAttributeKeyword" ); 
   AddColumnName( "SuppressInsertAttributeValue" ); 
   AddColumnName( "IncludeInsertAttributeKeyword" ); 
   AddColumnName( "IncludeInsertAttributeValue" ); 
   AddColumnName( "WriteBoards" );  
   AddColumnName( "WriteSmdComponents" );  
   AddColumnName( "WriteThruComponents" );  
   AddColumnName( "WriteFiducials" );  
   AddColumnName( "WriteToolingHoles" );  
   AddColumnName( "WriteMechanicalComponents" );  
   AddColumnName( "WriteGenericComponents" );  
   AddColumnName( "WriteXouts" );  
   AddColumnName( "WriteTestPoints" );  
   AddColumnName( "WriteTestProbes" );  

}

CDBCustomReportParameterTableDef::CDBCustomReportParameterTableDef()
{
   // CustomReportParameter
   // TemplateName, Priority, 
   // Command, Parameter, FieldName

   SetTableName( "CustomReportParameter" );

   AddColumnName( "TemplateName" );
   AddColumnName( "Priority" );
   AddColumnName( "Command" );
   AddColumnName( "Parameter" );
   AddColumnName( "FieldName" );
}

bool CDBInterface::ValidateTable(CDBTableDef &tableDef /*input*/, CString &msg /*output*/)
{
   // Validation at this stage is expecting that the table does exist.

   msg.Empty();

   if (tableDef.ValidateDBTableColumns( this ))
   {
      return true; // table is okay
   }
   else
   {
      msg += "\nError in ";
      msg += tableDef.GetTableName();
      msg += " table definition, missing one or more required column names.\n";
      msg += "Required column names are:\n";
      for (int i = 0; i < tableDef.GetColumNames().GetCount(); i++)
      {
         if (i > 0)
            msg += ", ";
         msg += tableDef.GetColumNames().GetAt(i);
      }
      msg += "\n";

      CStringArray dbColNames;
      if (this->GetColumnNames(tableDef.GetTableName(), dbColNames))
      {
         msg += "Column names found in DB are:\n";
         for (int i = 0; i < dbColNames.GetCount(); i++)
         {
            if (i > 0)
               msg += ", ";
            msg += dbColNames.GetAt(i);
         }
         msg += "\n";
      }
      else
      {
         msg += "No column names found in DB for ";
         msg += tableDef.GetTableName();
         msg += ".\n";
      }
   }

   return false;

}


CDBTableDefList::CDBTableDefList()
: m_curPos(NULL)
{
   // Add all table types that might be found in the DB.
   // This list is used in the overall table validator.

   this->AddTail(new CDBParts1TableDef);
   this->AddTail(new CDBParts2TableDef);
   this->AddTail(new CDBParts3TableDef);
   this->AddTail(new CDBCentroidsOldTableDef);
   this->AddTail(new CDBCentroidsMMTableDef);
   this->AddTail(new CDBCentroidsMM2TableDef);
   this->AddTail(new CDBCentroidsMM3TableDef);
   this->AddTail(new CDBPackagesTableDef);
   this->AddTail(new CDBPackageInfo1TableDef);
   this->AddTail(new CDBPackageInfo2TableDef);
   this->AddTail(new CDBPackageInfo3TableDef);
   this->AddTail(new CDBSubclassElementTableDef);
   this->AddTail(new CDBGenericAttributeTableDef);
   this->AddTail(new CDBOutlineVertexTableDef);
   this->AddTail(new CDBPanelTemplateIDTableDef1);
   this->AddTail(new CDBPanelTemplateIDTableDef2);
   this->AddTail(new CDBPanelTemplateToolingHoleTableDef);
   this->AddTail(new CDBPanelTemplateFiducialTableDef);
   this->AddTail(new CDBPanelTemplatePcbInstanceTableDef);
   this->AddTail(new CDBPanelTemplatePcbArrayTableDef);
   this->AddTail(new CDBPanelTemplateOutlineTableDef);
   this->AddTail(new CDBStencilSettingsTableDef);
   this->AddTail(new CDBStencilRules1TableDef);
   this->AddTail(new CDBStencilRules2TableDef);
   this->AddTail(new CDBStencilRules3TableDef);
   this->AddTail(new CDBStencilRules4TableDef);
   this->AddTail(new CDBCustomMachineTableDef);
   this->AddTail(new CDBCustomAssemblyTemplateTableDef);
   this->AddTail(new CDBCustomReportParameterTableDef);
}

bool CDBInterface::ValidateTables(bool reportError)
{
   // It is okay if a table does not exist, we will create it when needed.
   // If a table does exist, it must contain the correct column names.
   // Extra column names are okay, we don't particularly care if users extend the tables.

   CString errmsg;

   CDBTableDefList tableDefList;

   tableDefList.Reset();
   CDBTableDef *tabledef = NULL;
   while ((tabledef = tableDefList.GetNext()) != NULL)
   {
      if (TableExists( tabledef->GetTableName()))
      {
         CString msg;
         if (!ValidateTable(*tabledef, msg))
            errmsg += msg;
      }
   }

   if (!errmsg.IsEmpty())
   {
      errmsg += "\nThis database file can not be used in CAMCAD.\n";

      if (reportError)
         ErrorMessage(errmsg, "ODBC Part Lib Interface");
      else
         m_errorMessage += errmsg;  // Just append, might already be relevant info in current m_errorMessage

      return false;
   }


   return true;
}

bool CDBInterface::GetColumnNames(CString tablename, CStringArray &colnames)
{
   bool found = false;

   colnames.RemoveAll();

   if (m_dbConnected && !tablename.IsEmpty())
   {
      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {

         //SQLCHAR tblname[DB_STR_LEN];
         //tblname[0] = '\0';
         //SQLINTEGER cbTblName = SQL_NTS;

         SQLCHAR colname[DB_STR_LEN];
         colname[0] = '\0';
         SQLINTEGER cbColName = SQL_NTS;


         //rc = SQLTables( stmt, NULL, 0, NULL, 0, (SQLCHAR*)tablename.GetBuffer(0), tablename.GetLength(), NULL, 0);
         rc = SQLColumns(stmt, NULL, 0, NULL, 0, (SQLCHAR*)tablename.GetBuffer(0), tablename.GetLength(), NULL, 0);

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            found = true;

            while( (rc = SQLFetch( stmt )) != SQL_NO_DATA )
            {
               //rc = SQLGetData( stmt, 3, SQL_C_CHAR, tblname, sizeof(tblname), &cbTblName);
               rc = SQLGetData( stmt, 4, SQL_C_CHAR, colname, sizeof(colname), &cbColName);
               if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
               {
                  colnames.Add( colname );
               }
            }
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return found;
}

//---------------------------------------------------------------

CDBInterface::CDBInterface()
   : m_dbConnected(false)
   , m_attemptConnection(true)
   , m_hasDbHandle(false)
   , m_hasEnvHandle(false)
{
   // Do NOT reset m_allowConnection in constructor. It is a static value that transcends any 
   // particular instance of DBInterface, it applies as-is to any DBInterface being constructed.

   // Load sticky setting that indicates whether or not to attempt DB connections
   CString useDB( CAppRegistrySetting("Parts Library", "UseDBConnection").Get() );

   // We'll take blank and yes for affirmative, all else is negative
   useDB.Trim();
   m_attemptConnection = (useDB.IsEmpty() || useDB.CompareNoCase("Yes") == 0);
}

CDBInterface::~CDBInterface()
{
   Disconnect();
}

void CDBInterface::Disconnect()
{
   if (m_dbConnected)
      SQLDisconnect( m_dbc );
    
   if (m_hasDbHandle)
      SQLFreeHandle( SQL_HANDLE_DBC, m_dbc );

   if (m_hasEnvHandle)
      SQLFreeHandle( SQL_HANDLE_ENV, m_env );

   m_dbConnected = false;
   m_hasDbHandle = false;
   m_hasEnvHandle = false;
}

bool CDBInterface::Connect(CString dbpath, CString username, CString password, bool reportError)
{
   // It is okay is username and/or password are blank, so long as DB really doesn't
   // need them. We'll not know until we try to connect.
   // But not okay if dbpath is blank, must have a db to connect to or can't even try.

   m_errorMessage.Empty();

   if (!dbpath.IsEmpty() && GetConnectionAttemptEnabled())
   {
      // In case there is already an active connection
      Disconnect();

      // Make new connection...
      SQLRETURN rc;  // SQL Return code

      rc = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_env );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) 
      {
         m_hasEnvHandle = true;

         /* Set the ODBC version environment attribute */
         rc = SQLSetEnvAttr( m_env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

         rc = SQLAllocHandle( SQL_HANDLE_DBC, m_env, &m_dbc );
         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) 
         {
            m_hasDbHandle = true;

            CString connStrIN; // The connectin we'll try to make
            SQLCHAR connStrOUT[DB_STR_LEN]; // Connection elaborated by ODBC
            SQLSMALLINT truelen; // Len of connection str, possibly bigger than OUT buffer



            // Build connection string
            CFilePath filepath(dbpath);
            ///WORKS
            ///connStrIN.Format("DSN=MS Access Database;DBQ=%s;DefaultDir=%s;DriverId=25;FIL=MS Access;MaxBufferSize=2048;PageTimeOut=5;UID=%s;PWD=%s", 
            ///   dbpath, filepath.getDirectoryPath(), username, password);
            //Trying to minimize settings, this seems to work too.

            CString driver( GetDataSourceName() );
            if (driver.IsEmpty())
               driver = "CAMCAD"; // case dts0100412830, this is now the default

            connStrIN.Format("DSN=%s;DBQ=%s;DefaultDir=%s;UID=%s;PWD=%s", 
               driver, dbpath, filepath.getDirectoryPath(), username, password);

            rc = SQLDriverConnect( m_dbc, NULL, (SQLCHAR*)connStrIN.GetBuffer(0), connStrIN.GetLength(), connStrOUT, sizeof(connStrOUT), &truelen, SQL_DRIVER_NOPROMPT);

            if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO && fileExists(dbpath))
            {
               // DB connection failed, and DB file does exist.
               // Backward compatibility, pre 4.6, DSN may not have been set in registry and
               // 4.6 "CAMCAD" might not have been set up in system DSN table, try the 
               // MS "standard" name.

               driver = "MS Access Database"; // Default, works in USA, doesn't work, e.g. in Germany

               connStrIN.Format("DSN=%s;DBQ=%s;DefaultDir=%s;UID=%s;PWD=%s", 
                  driver, dbpath, filepath.getDirectoryPath(), username, password);

               rc = SQLDriverConnect( m_dbc, NULL, (SQLCHAR*)connStrIN.GetBuffer(0), connStrIN.GetLength(), connStrOUT, sizeof(connStrOUT), &truelen, SQL_DRIVER_NOPROMPT);
            }

            if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO && fileExists(dbpath))
            {
               // DB connection failed, and DB file does exist. Maybe it is a driver problem.
               // Try making the connection with the driver browser enabled.

               // Window handle, if we want to use browser capability
               HWND hwnd = NULL;
               CWnd *mainwnd = AfxGetMainWnd();
               if (mainwnd)
                  hwnd = mainwnd->m_hWnd;

               // connection with browser enabled
               rc = SQLDriverConnect( m_dbc, hwnd, (SQLCHAR*)connStrIN.GetBuffer(0), connStrIN.GetLength(), connStrOUT, sizeof(connStrOUT), &truelen, SQL_DRIVER_PROMPT);
            
               // If it worked, then save the DNS
               if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) 
               {
                  CStringArray params;
                  CSupString connStr(connStrOUT);
                  connStr.Parse(params, ";");
                  for (int parami = 0; parami < params.GetCount(); parami++)
                  {
                     CString p = params.GetAt(parami);
                     if (p.Left(4).CompareNoCase("DSN=") == 0)
                     {
                        p.Delete(0, 4);
                        SaveDataSourceName(p);
                        break; // no need to keep looping once DSN is found
                     }
                  }
               }
            }

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) 
            {
               // Connection succeeded
               m_dbConnected = true;
            }
            else
            {
               
               if (!fileExists(dbpath))
               {
                  m_errorMessage =  "SQL Database Connection Failed\n";
                  m_errorMessage += "File not found: [" + dbpath + "]";
                  if (reportError)
                     ErrorMessage(m_errorMessage, "ODBC Part Lib Interface");
               }
               else
               {
                  if (reportError)
                     ReportError(SQL_HANDLE_DBC, m_dbc, connStrIN, "SQL Database Connection Failed");
                  else
                     FormatErrorMessage(SQL_HANDLE_DBC, m_dbc, connStrIN, "SQL Database Connection Failed");
               }
            }
         }
         else
         {
            m_errorMessage = "SQL Driver Connection Failed";
            if (reportError)
               ErrorMessage(m_errorMessage, "ODBC Part Lib Interface");
         }
      }
      else
      {
         m_errorMessage = "SQL Environment Handle Allocation Failed";
         if (reportError)
            ErrorMessage(m_errorMessage, "ODBC Part Lib Interface");
      }
   }

   if (m_dbConnected)
   {
      if (!ValidateTables(reportError))
      {
         this->Disconnect();
      }
   }

   // NOT an "else" for previous test, note failure in validation will cause disconnection.
   // If connection was attempted and failed ...
   if (GetConnectionAttemptEnabled() && !m_dbConnected)
   {
      if (ErrorMessage("Do you want to disable further use of database connection?", "Database Connection Failed", MB_YESNO | MB_DEFBUTTON2) == IDYES)
      {
         this->SetConnectionAttemptEnabled(false);
      }
   }

   return m_dbConnected;
}

bool CDBInterface::CreateMDBFile(CString completeFilePath)
{
   BOOL retval = FALSE;

   if (!completeFilePath.IsEmpty() && !fileExists(completeFilePath))
   {
      // Window handle, if we want to enable SQLConfigDataSource dialogs.
      // Leave hwmd NULL is ya don't want such dialogs (could possibly present error
      // message or configuration param choosers).
      // We'll leave it on for now to see what we get.

      HWND hwnd = NULL;
      CWnd *mainwnd = AfxGetMainWnd();
      if (mainwnd)
         hwnd = mainwnd->m_hWnd;

      CString attributes;
      attributes.Format("CREATE_DB=\"%s\" General", completeFilePath);
      //attributes.Format("CREATE_DB=\"%s\"", completeFilePath);

      retval = SQLConfigDataSource(hwnd/*NULL*/ /*hwndParent*/, ODBC_ADD_DSN, "Microsoft Access Driver (*.mdb)",
         attributes.GetBuffer(0));

      if (!retval)
      {
         SQLRETURN rc;
         
         // Can be up to 8 records in error struct, numbered 1 to 8
         for (int errorRecNum = 1; errorRecNum < 9; errorRecNum++)
         {
            DWORD pfErrorCode;
            char lpszErrorMsg[256];
            WORD pcbErrorMsg;
            rc = SQLInstallerError( errorRecNum,  &pfErrorCode, lpszErrorMsg, sizeof(lpszErrorMsg), &pcbErrorMsg);

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
            {
               CString msg = "Database file creation failed.\n" + (CString)lpszErrorMsg + "\n";
               ErrorMessage(msg, "ODBC Part Lib Interface");
            }
         }
      }
   }

   return retval?true:false;
}

bool CDBInterface::PresentDBFileChooser()
{
   // Present the DB File chooser dialog.
   // Returns true if a usable file is selected (possibly an existing file or
   // a newly created empty file).
   // Returns false if the user does not select (or create) a valid file.
   // E.g. if user Cancels, then this returns false.
   // NOTE that the current setting (i.e. the setting already in place  before
   // the chooser was called) may in fact be valid, so false return from this
   // does not necessarily mean the current DB File setting is invalid.

   // I did want to have it validate the current setting, even on cancel, but
   // it looks weird to click Cancel and then get a bunch of error messages.


   CString partLibraryDatabaseName( this->GetLibraryDatabaseName() ); // Current setting is default for chooser

   int chooserStat = IDCANCEL;
   bool createNewCancelled = false;

   do
   {
      CFileDialog fileDlg(TRUE, "*.mdb", partLibraryDatabaseName, OFN_EXPLORER, "Database Files(*.mdb)|*.mdb|All Files(*.*)|*.*||", NULL);//this);
      
      if ( (chooserStat = fileDlg.DoModal()) == IDOK)
      {
         createNewCancelled = false;

         partLibraryDatabaseName = fileDlg.GetPathName().Trim();

         if (fileExists( partLibraryDatabaseName ))
         {
            this->SetLibraryDatabaseName( partLibraryDatabaseName );
         }
         else
         {
            // User might want to create a new DB file
            CString msg;
            msg = "File [" + partLibraryDatabaseName + "] does not exist.";
            int ans1 = ErrorMessage("Create new parts library database file?", msg, MB_OKCANCEL | MB_ICONQUESTION);

            if (ans1 == IDOK)
            {
               if (this->CreateMDBFile( partLibraryDatabaseName ))
               {
                  // success, do nothing
               }
               else
               {
                  // MDB file creation failed, but we'll call it cancelled here to keep var set down.
                  // Message already presented for this failure, we want to stop the
                  // connect & validation from presenting more.
                  createNewCancelled = true;
               }
            }
            else
            {
               createNewCancelled = true;
            }
         }

         // If something seems to have been selected or created, then validate it
         if (chooserStat == IDOK && !createNewCancelled)
         {
            // Connecting to DB will perform validation, so just try to connect
            // and see what happens. 
            if (this->Connect( partLibraryDatabaseName, "", "" )) // username and password are blank for camcad parts libs
            {
               // We save teh new setting only after validation has succeeded, so if the
               // user ends up cancelling out after an interation of failure, the setting
               // will still be as it was when all this choosing first started.
               this->SetLibraryDatabaseName( partLibraryDatabaseName );
               return true;  // and note also, upon this return from the chooser the DB is connected and ready to use
            }
         }

      }
   }
   while (chooserStat == IDOK && createNewCancelled); // user decided to cancel create new, offer file chooser again

   return false;
}

///////////////////////////////////////////////////////////////////////////

bool CDBInterface::SaveDataSourceName(CString dsn)
{
   // This has to do with the Driver, not the MDB file

   if (!dsn.IsEmpty())
   {
      CAppRegistryKey appKey;
      CRegistryKey settings = appKey.createSubKey("Settings");

      if (settings.isOpen())
      {
         CRegistryKey registryKey = settings.createSubKey("Parts Library");

         if (registryKey.isOpen())
         { 
            if (registryKey.setValue("DSN", dsn))
            {
               return true;
            }
         }
      }
   }

   return false;
}

CString CDBInterface::GetDataSourceName()
{
   // This has to do with the Driver, not the MDB file

   CString dsn;

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey("Parts Library");

      if (registryKey.isOpen())
      { 
         if (registryKey.getValue("DSN", dsn))
         {
            // Ok, got it.
         }
      }
   }

   return dsn;
}

///////////////////////////////////////////////////////////////////////////

void CDBInterface::nukeOldRegistryKey()
{
   // Delete the key used by the old ccpartslib.exe DB interface.
   // If they keys don't exist, the call fails. Not a big deal, we don't care.
   // Some day we can get rid of this.
   
   RegDeleteKey(HKEY_CURRENT_USER, "Software\\VB and VBA Program Settings\\CAMCAD Parts Library\\CCPartsLib");
   RegDeleteKey(HKEY_CURRENT_USER, "Software\\VB and VBA Program Settings\\CAMCAD Parts Library");
}

CString CDBInterface::GetLibraryDatabaseName()
{
   // When ccpartslib.exe was in the game, being a VB appm it put they registry entry
   // under the "VB and VBA Program Settings" area. Most of the rest of CAMCAD settings
   // are under "Mentor Graphics/CAMCAD" or "CAMCAD Professional". We're going to start
   // using a new entry in that more usual CAMCAD location. We'll maintain backward
   // compatibility by reading the old key and setting the new key, and then delete
   // the old key.

   // The "set" operation is in charge of teh actual deletion. What we do here in "get"
   // is check for the old key. If it exists, we get the value then set the new key.
   // Setting the new key will delete the old. Then we proceed with getting the value
   // from the new key. In this way, the first call to either set or get will update to
   // the new registry key.

   //
   // The old stuff.
   //
   HRESULT res;
   HKEY settingsKey;
   char dirPath[_MAX_PATH];
   dirPath[0] = '\0';

   res = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\VB and VBA Program Settings\\CAMCAD Parts Library\\CCPartsLib", 0, KEY_READ, &settingsKey);
   if (res == ERROR_SUCCESS)
   {
      DWORD bufLen = _MAX_PATH;
      DWORD lType;
      res = RegQueryValueEx(settingsKey, "DBName", NULL, &lType, (LPBYTE)dirPath, &bufLen);
      RegCloseKey(settingsKey);

      // Set the new key
      SetLibraryDatabaseName(dirPath);
   }


   //
   // The new stuff.
   //
#ifdef OLD_STYLE_NEW_STUFF
   CString dbpath;

   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey("Parts Library");

      if (registryKey.isOpen())
      { 
         if (registryKey.getValue("DBName", dbpath))
         {
            // Ok, got it.
         }
      }
   }
#else
   // Since days old style new stuff was written, the registry interface has been standardized and encapsulated.
   // Now this is all that is needed locally.

   CString dbpath = CAppRegistrySetting("Parts Library", "DBName").Get();
#endif


   if (dbpath.IsEmpty())
   {
      // Compiled in default, references sample DB in CAMCAD install folder
      dbpath = getApp().getUserPath() + "CCPartsLibrary.mdb";
   }

   return dbpath;
}

bool CDBInterface::SetLibraryDatabaseName(const CString libraryDatabaseName)
{
   // We're setting the DB name, so what ever is in the old registry key location does not
   // even matter. Just get rid of the old key.
   nukeOldRegistryKey();

   // Save setting in registry, return true if is appears to work, otherwise false
   CAppRegistryKey appKey;
   CRegistryKey settings = appKey.createSubKey("Settings");

   if (settings.isOpen())
   {
      CRegistryKey registryKey = settings.createSubKey("Parts Library");

      if (registryKey.isOpen())
      { 
         if (registryKey.setValue("DBName", libraryDatabaseName))
         {
            return true;
         }
      }
   }

   return false;

}

void CDBInterface::SetConnectionAttemptEnabled(bool flag)
{
   // Updated cached value in DBInterface itself
   m_attemptConnection = flag;

   // This is a sticky setting, so update registry
   CAppRegistrySetting("Parts Library", "UseDBConnection").Set(CString(m_attemptConnection?"Yes":"No"));
}

void CDBInterface::SetConnectionAttemptAllowed(bool flag)
{
   // Updated cached value in DBInterface itself
   m_allowConnection = flag;

   // The app is setting this based on startup conditions (command line param
   // in TXP/vPlan Test Session case). It is not a sticky setting, not saved in registry.
}

///////////////////////////////////////////////////////////////////////////

bool CDBInterface::TableExists(CString tablename)
{
   // Return true if DB is connected and named table exists,
   // otherwise false.

   bool found = false;

   if (m_dbConnected && !tablename.IsEmpty())
   {
      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {

         SQLCHAR name[DB_STR_LEN];
         name[0] = '\0';
         SQLINTEGER cbName = SQL_NTS;

         SQLCHAR fluff1[DB_STR_LEN];
         fluff1[0] = '\0';
         SQLINTEGER cbFluff1 = SQL_NTS;

         SQLCHAR fluff2[DB_STR_LEN];
         fluff2[0] = '\0';
         SQLINTEGER cbFluff2 = SQL_NTS;

         // The data we're after will be in the 3rd column of the result set

         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, fluff1, sizeof(fluff1), &cbFluff1);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, fluff2, sizeof(fluff2), &cbFluff2);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, name, sizeof(name), &cbName);

         rc = SQLTables( stmt, NULL, 0, NULL, 0, (SQLCHAR*)tablename.GetBuffer(0), tablename.GetLength(), NULL, 0);

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            while(!found && ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               // We don't really need this. If the result set is not empty, then it
               // found the table. But jic.
               if (tablename.CompareNoCase((char*)name) == 0)
                  found = true;
            }
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return found;
}


bool CDBInterface::CreateTable(CString tablename, CStringArray& columnnames)
{
   // Return true if DB is connected and named table exists,
   // otherwise false.

   bool tablecreated = false;

   if (m_dbConnected && !tablename.IsEmpty() && columnnames.GetCount() > 0)
   {
      if (TableExists(tablename))
      {
         // Hmmm... is that okay or is it an error?
         // Should check that the table has the right column names, if it does
         // then it is okay, if it doesn't then it is not okay.
         // But meanwhile, we'll just say it is okay, because the caller wants the
         // table created, and at some point it was, cuz it is there now.
         tablecreated = true;
      }
      else
      {
         // Create the table.
         // In ccPartsLib, all column types are variable length character string.

         CString query;
         query += "CREATE TABLE \"" + tablename + "\" ";
         query += "(";
         for (int i = 0; i < columnnames.GetCount(); i++)
         {
            CString column(columnnames.GetAt(i));
            if (!column.IsEmpty())
            {
               if (i > 0)
                  query += ", ";

               // Prior to CCASM1.0 Update 3, ALL columns in CAMCAD DB were
               // text, aka varchar. As of Update 3, the DataTime aka Timestamp
               // column is a genuine Timestamp field. (In msAccess, called DateTime,
               // in ODBC, called Timestamp, since date and time are together.)
               //int x = SQL_TYPE_TIMESTAMP;
               if (column.Compare(QDATETIME) == 0)
                  query += "\"" + column + "\" datetime";
               else
                  query += "\"" + column + "\" varchar";
            }
         }
         query += ")";

         SQLHSTMT stmt;
         SQLRETURN rc;

         rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
            {
               // Okay
               tablecreated = true;
            }
            else
            {
               ReportError(SQL_HANDLE_STMT, stmt, query);
            }

            SQLFreeHandle( SQL_HANDLE_STMT, stmt );
         }
      }
   }

   return tablecreated;
}

bool CDBInterface::DeleteTableEntries(CString tableName, CString columnName, CString value)
{
   if (this->IsConnected() && this->TableExists(tableName) && 
      !columnName.IsEmpty() && !value.IsEmpty())
   {
      // Get rid of existing entries
      CString query;
      query.Format("DELETE FROM %s WHERE \"%s\" = '%s'", tableName, columnName, value);

      SQLHSTMT deletestmt;
      SQLRETURN deleterc;

      deleterc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &deletestmt );

      if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO)
      {
         deleterc = SQLExecDirect( deletestmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO || deleterc == SQL_NO_DATA)
         {
            // Ok
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, deletestmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, deletestmt );
   }

   return true;
}

void CDBInterface::FormatErrorMessage(SQLSMALLINT handleType, SQLHANDLE handle, CString query, CString additionalMsg)
{
   SQLCHAR errormessage[256];
   SQLCHAR state[128];
   SQLINTEGER nativeerror;
   SQLSMALLINT actualLen;
   SQLRETURN rc;

   m_errorMessage = "";

   if (!additionalMsg.IsEmpty())
      m_errorMessage += additionalMsg + (CString)"\n";

   int recNum = 1;
   while ( (rc = SQLGetDiagRec(handleType, handle, recNum, state, &nativeerror, 
      errormessage, sizeof(errormessage), &actualLen)) == SQL_SUCCESS)
   {
      if (recNum++ > 1)
         m_errorMessage += "\n";

      m_errorMessage += (CString)errormessage;
   }
   if (m_errorMessage.IsEmpty())
      m_errorMessage = "A problem has been detected, cause is unknown.";

   if (!query.IsEmpty())
      m_errorMessage += "\nQuery: [" + query + "]";
}

void CDBInterface::ReportError(SQLSMALLINT handleType, SQLHANDLE handle, CString query, CString additionalMsg)
{
   FormatErrorMessage(handleType, handle, query, additionalMsg);
   ErrorMessage(GetErrorMessage(), "ODBC Part Lib Interface");
}

void CDBInterface::ReportDBSaveResult(int attemptCount, int successCount)
{
   if (attemptCount == 0)
   {
      // Didn't even try, no message
   }
   else if (successCount == 0)
   {
      ErrorMessage("Save to Library failed", "", MB_ICONINFORMATION);
   }
   else if (successCount != attemptCount)
   {
      ErrorMessage("Save to Library was partially successful", "", MB_ICONINFORMATION);
   }
   else
   {
      ErrorMessage("Save to Library was successful", "", MB_ICONINFORMATION);
   }
}


CDBPart *CDBInterface::LookupPart1(CString partnumber)
{
   CDBPart *part = NULL;

   CDBParts1TableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLINTEGER rowID;
      SQLINTEGER cbRowID = 0;

      SQLCHAR partnum[DB_STR_LEN];
      partnum[0] = '\0';
      SQLINTEGER cbPartnum = SQL_NTS;

      SQLCHAR devtype[DB_STR_LEN];
      devtype[0] = '\0';
      SQLINTEGER cbDevtype = SQL_NTS;

      SQLCHAR value[DB_STR_LEN];
      value[0] = '\0';
      SQLINTEGER cbValue = SQL_NTS;

      SQLCHAR ptol[DB_STR_LEN];
      ptol[0] = '\0';
      SQLINTEGER cbPtol = SQL_NTS;

      SQLCHAR ntol[DB_STR_LEN];
      ntol[0] = '\0';
      SQLINTEGER cbNtol = SQL_NTS;

      SQLCHAR subclass[DB_STR_LEN];
      subclass[0] = '\0';
      SQLINTEGER cbSubclass = SQL_NTS;

      SQLCHAR pinmap[DB_STR_LEN];
      pinmap[0] = '\0';
      SQLINTEGER cbPinmap = SQL_NTS;

      SQLCHAR desc[DB_STR_LEN];
      desc[0] = '\0';
      SQLINTEGER cbDesc = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_LONG, &rowID, 0, &cbRowID);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, partnum, sizeof(partnum), &cbPartnum);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, devtype, sizeof(devtype), &cbDevtype);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, value, sizeof(value), &cbValue);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, ptol, sizeof(ptol), &cbPtol);

         rc = SQLBindCol( stmt, 6, 
            SQL_C_CHAR, ntol, sizeof(ntol), &cbNtol);

         rc = SQLBindCol( stmt, 7, 
            SQL_C_CHAR, subclass, sizeof(subclass), &cbSubclass);

         rc = SQLBindCol( stmt, 8, 
            SQL_C_CHAR, pinmap, sizeof(pinmap), &cbPinmap);

         rc = SQLBindCol( stmt, 9, 
            SQL_C_CHAR, desc, sizeof(desc), &cbDesc);

         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT ID, PartNumber, DeviceType, Value, pTol, nTol, Subclass, PINMapping, Description FROM %s WHERE PartNumber = '%s'" , 
            tabledef.GetTableName(), partnumber);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               part = new CDBPart(rowID, partnum, devtype, value, ptol, ntol, subclass, pinmap, desc, "", "", "", NULL);
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   if (part != NULL)
   {
      // Fundemental Part lookup worked, add in the Generic Attributes
      CDBGenericAttributeMap *attmap = this->LookupGenericAttributes(partnumber, dbnamePartNumber);
      part->SetAttribMap(attmap);
   }

   return part;
}

CDBPart *CDBInterface::LookupPart2(CString partnumber)
{
   CDBParts2TableDef tabledef;
   return LookupPart2or3(tabledef, partnumber);
}

CDBPart *CDBInterface::LookupPart3(CString partnumber)
{
   CDBParts3TableDef tabledef;
   return LookupPart2or3(tabledef, partnumber);
}

CDBPart *CDBInterface::LookupPart2or3(CDBTableDef &tabledef, CString partnumber)
{
   // In PartTable2 the DateTime column is text.
   // In PartTable3 the DateTime column is datetime.
   // Looking it up as text works either way, so we do.

   CDBPart *part = NULL;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLINTEGER rowID;
      SQLINTEGER cbRowID = 0;

      SQLCHAR partnum[DB_STR_LEN];
      partnum[0] = '\0';
      SQLINTEGER cbPartnum = SQL_NTS;

      SQLCHAR devtype[DB_STR_LEN];
      devtype[0] = '\0';
      SQLINTEGER cbDevtype = SQL_NTS;

      SQLCHAR value[DB_STR_LEN];
      value[0] = '\0';
      SQLINTEGER cbValue = SQL_NTS;

      SQLCHAR ptol[DB_STR_LEN];
      ptol[0] = '\0';
      SQLINTEGER cbPtol = SQL_NTS;

      SQLCHAR ntol[DB_STR_LEN];
      ntol[0] = '\0';
      SQLINTEGER cbNtol = SQL_NTS;

      SQLCHAR subclass[DB_STR_LEN];
      subclass[0] = '\0';
      SQLINTEGER cbSubclass = SQL_NTS;

      SQLCHAR pinmap[DB_STR_LEN];
      pinmap[0] = '\0';
      SQLINTEGER cbPinmap = SQL_NTS;

      SQLCHAR desc[DB_STR_LEN];
      desc[0] = '\0';
      SQLINTEGER cbDesc = SQL_NTS;

      SQLCHAR username[DB_STR_LEN];
      username[0] = '\0';
      SQLINTEGER cbUserName = SQL_NTS;

      SQLCHAR datetime[DB_STR_LEN];
      datetime[0] = '\0';
      SQLINTEGER cbDDateTime = SQL_NTS;

      SQLCHAR comment[DB_STR_LEN];
      comment[0] = '\0';
      SQLINTEGER cbComment= SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_LONG, &rowID, 0, &cbRowID);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, partnum, sizeof(partnum), &cbPartnum);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, devtype, sizeof(devtype), &cbDevtype);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, value, sizeof(value), &cbValue);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, ptol, sizeof(ptol), &cbPtol);

         rc = SQLBindCol( stmt, 6, 
            SQL_C_CHAR, ntol, sizeof(ntol), &cbNtol);

         rc = SQLBindCol( stmt, 7, 
            SQL_C_CHAR, subclass, sizeof(subclass), &cbSubclass);

         rc = SQLBindCol( stmt, 8, 
            SQL_C_CHAR, pinmap, sizeof(pinmap), &cbPinmap);

         rc = SQLBindCol( stmt, 9, 
            SQL_C_CHAR, desc, sizeof(desc), &cbDesc);

         rc = SQLBindCol( stmt, 10, 
            SQL_C_CHAR, username, sizeof(username), &cbUserName);

         rc = SQLBindCol( stmt, 11, 
            SQL_C_CHAR, datetime, sizeof(datetime), &cbDDateTime);

         rc = SQLBindCol( stmt, 12, 
            SQL_C_CHAR, comment, sizeof(comment), &cbComment);

         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT ID, PartNumber, DeviceType, Value, pTol, nTol, Subclass, PINMapping, Description, UserName, \"DateTime\", Comment FROM %s WHERE PartNumber = '%s'" , 
            tabledef.GetTableName(), partnumber);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               part = new CDBPart(rowID, partnum, devtype, value, ptol, ntol, subclass, pinmap, desc, username, datetime, comment, NULL);
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   if (part != NULL)
   {
      // Fundemental Part lookup worked, add in the Generic Attributes
      CDBGenericAttributeMap *attmap = this->LookupGenericAttributes(partnumber, dbnamePartNumber);
      part->SetAttribMap(attmap);
   }

   return part;
}

CDBPart *CDBInterface::LookupPart(CString partnumber, bool backwardCompatible)
{
   CDBPart *cdbPart = LookupPart3(partnumber);

   if(cdbPart == NULL && backwardCompatible)
   {
      cdbPart = LookupPart2(partnumber);
   }

   if(cdbPart == NULL && backwardCompatible)
   {
      cdbPart = LookupPart1(partnumber);
   }

   return cdbPart;
}

CDBCentroid *CDBInterface::LookupCentroidLowLevel(CString geometryname, CDBTableDef &tabledef, PageUnitsTag pageUnits)
{
   CDBCentroid *cent = NULL;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR geomname[DB_STR_LEN];
      geomname[0] = '\0';
      SQLINTEGER cbGeomname = SQL_NTS;

      SQLCHAR algorithm[DB_STR_LEN];
      algorithm[0] = '\0';
      SQLINTEGER cbAlgorithm = SQL_NTS;

      SQLCHAR x[DB_STR_LEN];
      x[0] = '\0';
      SQLINTEGER cbX = SQL_NTS;

      SQLCHAR y[DB_STR_LEN];
      y[0] = '\0';
      SQLINTEGER cbY = SQL_NTS;

      SQLCHAR rot[DB_STR_LEN];
      rot[0] = '\0';
      SQLINTEGER cbRot = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, geomname, sizeof(geomname), &cbGeomname);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, algorithm, sizeof(algorithm), &cbAlgorithm);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, x, sizeof(x), &cbX);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, y, sizeof(y), &cbY);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, rot, sizeof(rot), &cbRot);

         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT GeomName, Algorithm, X, Y, Rot FROM %s WHERE GeomName = '%s'" , tabledef.GetTableName(), geometryname);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               cent = new CDBCentroid(geomname, algorithm, x, y, rot, pageUnits, "", "", "");
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return cent;
}

CDBCentroid *CDBInterface::LookupCentroidLowLevel1(CString geometryname, CDBTableDef &tabledef, PageUnitsTag pageUnits)
{
   CDBCentroid *cent = NULL;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR geomname[DB_STR_LEN];
      geomname[0] = '\0';
      SQLINTEGER cbGeomname = SQL_NTS;

      SQLCHAR algorithm[DB_STR_LEN];
      algorithm[0] = '\0';
      SQLINTEGER cbAlgorithm = SQL_NTS;

      SQLCHAR x[DB_STR_LEN];
      x[0] = '\0';
      SQLINTEGER cbX = SQL_NTS;

      SQLCHAR y[DB_STR_LEN];
      y[0] = '\0';
      SQLINTEGER cbY = SQL_NTS;

      SQLCHAR rot[DB_STR_LEN];
      rot[0] = '\0';
      SQLINTEGER cbRot = SQL_NTS;

      SQLCHAR username[DB_STR_LEN];
      username[0] = '\0';
      SQLINTEGER cbUserName = SQL_NTS;

      SQLCHAR datetime[DB_STR_LEN];
      datetime[0] = '\0';
      SQLINTEGER cbDDateTime = SQL_NTS;

      SQLCHAR comment[DB_STR_LEN];
      comment[0] = '\0';
      SQLINTEGER cbComment= SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, geomname, sizeof(geomname), &cbGeomname);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, algorithm, sizeof(algorithm), &cbAlgorithm);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, x, sizeof(x), &cbX);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, y, sizeof(y), &cbY);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, rot, sizeof(rot), &cbRot);

         rc = SQLBindCol( stmt, 6, 
            SQL_C_CHAR, username, sizeof(username), &cbUserName);

         rc = SQLBindCol( stmt, 7, 
            SQL_C_CHAR, datetime, sizeof(datetime), &cbDDateTime);

         rc = SQLBindCol( stmt, 8, 
            SQL_C_CHAR, comment, sizeof(comment), &cbComment);

         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT GeomName, Algorithm, X, Y, Rot, UserName, \"DateTime\", Comment FROM %s WHERE GeomName = '%s'" , tabledef.GetTableName(), geometryname);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               cent = new CDBCentroid(geomname, algorithm, x, y, rot, pageUnits, username, datetime, comment);
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return cent;
}

CDBCentroid *CDBInterface::LookupCentroid(CString geometryname, bool backwardCompatible)
{
   // This is the only Centroid lookup that should be called from outside world.
   // The rest are low level table lookup and backward compatibility support.
   // If backwardCompatible mode is true then if not found in new table look in old table.
   // If backwardCompatible is false then we are interested only in new table.

    // First try the MM3 table
   CDBCentroidsMM3TableDef mm3TblDef;
   CDBCentroid *cent = LookupCentroidLowLevel1(geometryname, mm3TblDef, pageUnitsMilliMeters);

   // If not found and backwardCompatible mode is true, then look in old table.
   if(cent == NULL && backwardCompatible)
   {
      CDBCentroidsMM2TableDef mm2TblDef;
      CDBCentroid *cent = LookupCentroidLowLevel1(geometryname, mm2TblDef, pageUnitsMilliMeters);
   }

   // If not found and backwardCompatible mode is true, then look in old old table.
   if(cent == NULL && backwardCompatible)
   {
      // Units are always millimeters in this table
      CDBCentroidsMMTableDef mmTblDef;
      cent = LookupCentroidLowLevel(geometryname, mmTblDef, pageUnitsMilliMeters);
   }

   // If not found and backwardCompatible mode is true, then look in old old old table.
   // Units are unknown in old table.
   if(cent == NULL && backwardCompatible)
   {

      CDBCentroidsOldTableDef oldTblDef;
      cent = LookupCentroidLowLevel(geometryname, oldTblDef, pageUnitsUndefined);
   }

   return cent;
}


CDBPackage *CDBInterface::LookupPackage(CString geometryname)
{
   CDBPackage *pkg = NULL;

   CDBPackagesTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR geomName[DB_STR_LEN];
      geomName[0] = '\0';
      SQLINTEGER cbGeomName = SQL_NTS;

      SQLCHAR packageAlias[DB_STR_LEN];
      packageAlias[0] = '\0';
      SQLINTEGER cbPackageAlias = SQL_NTS;

      SQLCHAR outlineMethod[DB_STR_LEN];
      outlineMethod[0] = '\0';
      SQLINTEGER cbOutlineMethod = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
#ifdef BINDSTYLE
         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, geomname, sizeof(geomname), &cbGeomname);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, algorithm, sizeof(algorithm), &cbAlgorithm);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, x, sizeof(x), &cbX);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, y, sizeof(y), &cbY);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, rot, sizeof(rot), &cbRot);
#endif
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT GeomName, PackageAlias, OutlineMethod FROM %s WHERE GeomName = '%s'" , 
            tabledef.GetTableName(), geometryname);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN rc1 = SQLGetData( stmt, 1, SQL_C_CHAR, geomName,      sizeof(geomName),      &cbGeomName);
               SQLRETURN rc2 = SQLGetData( stmt, 2, SQL_C_CHAR, packageAlias,  sizeof(packageAlias),  &cbPackageAlias);
               SQLRETURN rc3 = SQLGetData( stmt, 3, SQL_C_CHAR, outlineMethod, sizeof(outlineMethod), &cbOutlineMethod);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  pkg = new CDBPackage( geomName,
                     (rc2 == SQL_SUCCESS || rc2 == SQL_SUCCESS_WITH_INFO) ? packageAlias :  (CString)"",
                     (rc3 == SQL_SUCCESS || rc3 == SQL_SUCCESS_WITH_INFO) ? outlineMethod : (CString)"",
                     NULL
                     );
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   if (pkg != NULL)
   {
      // Fundemental Package lookup worked, add in the Generic Attributes
      CDBGenericAttributeMap *attmap = this->LookupGenericAttributes(geometryname, dbnameGeometry);
      pkg->SetAttribMap(attmap);
   }

   return pkg;
}


CDBPackageInfo *CDBInterface::LookupPackageInfo(CString name, EDBNameType nametype, bool backwardCompatible)
{
   // Full backward compatibility wrapper for package lookup.
   // First try new Package Info table.
   // If no entry found then if name type is Geometry then try old Package table.

   // 4.8 Update 3
   CDBPackageInfo *pkginfo = LookupPackageInfo3(name, nametype);
   
   if (pkginfo == NULL && backwardCompatible)
   {
      // What was new in previous 4.8
      pkginfo = LookupPackageInfo2(name, nametype);
   }

   if (pkginfo == NULL && backwardCompatible)
   {
      // The new 4.7 stuff
      pkginfo = LookupPackageInfo1(name, nametype);
   }

   if (pkginfo == NULL && nametype == dbnameGeometry && backwardCompatible)
   {
      // If no new 4.7 stuff try old 4.6 stuff

      pkginfo = LookupPackageInfo0(name, nametype);
   }

   return pkginfo;
}

CDBPackageInfo *CDBInterface::LookupPackageInfo0(CString name, EDBNameType nametype)
{
   CDBPackageInfo *pkginfo = NULL;

   CDBPackage *dbpkg = this->LookupPackage(name);

   if (dbpkg != NULL)
   {
      pkginfo = new CDBPackageInfo( name, dbnameGeometry,   // only support equivalent of packageSource = Geoemtry
         dbnameGeometry,                                    // only support equivalent of packageSource = Geoemtry
         dbpkg->getPackageAlias(),
         "", // Old table did not have height
         dbpkg->getOutlineMethod(),
         "",  // Old table did not have DPMO
         "",  // Old table did not have Family
         "",  // Old table did not have UserName
         "",  // Old table did not have DateTime
         "",  // Old table did not have Comment
         dbpkg->GiveAwayAttribMap()
         );

      delete dbpkg;
   }

   return pkginfo;
}

CDBPackageInfo *CDBInterface::LookupPackageInfo1(CString name, EDBNameType nametype)
{
   // This is not just for the PackageInfo table, it also looks up the attribs in GenericAttributes table

   CString nametypeStr( CDBPackageInfo::GetNameTypeStr(nametype) );

   CDBPackageInfo *pkg = NULL;

   CDBPackageInfo1TableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR itemName[DB_STR_LEN];
      itemName[0] = '\0';
      SQLINTEGER cbItemName = SQL_NTS;

      SQLCHAR nameType[DB_STR_LEN];
      nameType[0] = '\0';
      SQLINTEGER cbNameType = SQL_NTS;

      SQLCHAR packageSource[DB_STR_LEN];
      packageSource[0] = '\0';
      SQLINTEGER cbPackageSource = SQL_NTS;

      SQLCHAR packageAlias[DB_STR_LEN];
      packageAlias[0] = '\0';
      SQLINTEGER cbPackageAlias = SQL_NTS;

      SQLCHAR packageHeight[DB_STR_LEN];
      packageHeight[0] = '\0';
      SQLINTEGER cbPackageHeight = SQL_NTS;

      SQLCHAR outlineMethod[DB_STR_LEN];
      outlineMethod[0] = '\0';
      SQLINTEGER cbOutlineMethod = SQL_NTS;

      SQLCHAR dpmo[DB_STR_LEN];
      dpmo[0] = '\0';
      SQLINTEGER cbDpmo = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Name, NameType, PackageSource, PackageAlias, PackageHeight, OutlineMethod, DPMO FROM %s WHERE Name = '%s' AND NameType ='%s'" , 
            tabledef.GetTableName(), name, nametypeStr);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN rc1 = SQLGetData( stmt, 1, SQL_C_CHAR, itemName,      sizeof(itemName),      &cbItemName);
               SQLRETURN rc2 = SQLGetData( stmt, 2, SQL_C_CHAR, nameType,      sizeof(nameType),      &cbNameType);
               SQLRETURN rc3 = SQLGetData( stmt, 3, SQL_C_CHAR, packageSource, sizeof(packageSource), &cbPackageSource);
               SQLRETURN rc4 = SQLGetData( stmt, 4, SQL_C_CHAR, packageAlias,  sizeof(packageAlias),  &cbPackageAlias);
               SQLRETURN rc5 = SQLGetData( stmt, 5, SQL_C_CHAR, packageHeight, sizeof(packageHeight), &cbPackageHeight);
               SQLRETURN rc6 = SQLGetData( stmt, 6, SQL_C_CHAR, outlineMethod, sizeof(outlineMethod), &cbOutlineMethod);
               SQLRETURN rc7 = SQLGetData( stmt, 7, SQL_C_CHAR, dpmo,          sizeof(dpmo),          &cbDpmo);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  pkg = new CDBPackageInfo( itemName, nametype,
                     CDBPackageInfo::GetNameType( (rc3 == SQL_SUCCESS || rc3 == SQL_SUCCESS_WITH_INFO) ? packageSource : (CString)"" ),
                     (rc4 == SQL_SUCCESS || rc4 == SQL_SUCCESS_WITH_INFO) ? packageAlias  : (CString)"",
                     (rc5 == SQL_SUCCESS || rc5 == SQL_SUCCESS_WITH_INFO) ? packageHeight : (CString)"",
                     (rc6 == SQL_SUCCESS || rc6 == SQL_SUCCESS_WITH_INFO) ? outlineMethod : (CString)"",
                     (rc7 == SQL_SUCCESS || rc7 == SQL_SUCCESS_WITH_INFO) ? dpmo          : (CString)"",
                     (CString)"", // Family, no support in old table
                     (CString)"", // Username, no support in old table
                     (CString)"", // DateTime, no support in old table
                     (CString)"", // Comments, no support in old table
                     NULL
                     );
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   if (pkg != NULL)
   {
      // Fundemental Package lookup worked, add in the Generic Attributes
      CDBGenericAttributeMap *attmap = this->LookupGenericAttributes(name, nametype);
      pkg->SetAttribMap(attmap);
   }

   return pkg;
}
 
CDBPackageInfo *CDBInterface::LookupPackageInfo2(CString name, EDBNameType nametype)
{
   CDBPackageInfo2TableDef tabledef;
   return this->LookupPackageInfo23(tabledef, name, nametype);
}

CDBPackageInfo *CDBInterface::LookupPackageInfo3(CString name, EDBNameType nametype)
{
   CDBPackageInfo3TableDef tabledef;
   return this->LookupPackageInfo23(tabledef, name, nametype);
}

CDBPackageInfo *CDBInterface::LookupPackageInfo23(CDBTableDef &tabledef, CString name, EDBNameType nametype)
{
   // This is not just for the PackageInfo table, it also looks up the attribs in GenericAttributes table

   CString nametypeStr( CDBPackageInfo::GetNameTypeStr(nametype) );

   CDBPackageInfo *pkg = NULL;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR itemName[DB_STR_LEN];
      itemName[0] = '\0';
      SQLINTEGER cbItemName = SQL_NTS;

      SQLCHAR nameType[DB_STR_LEN];
      nameType[0] = '\0';
      SQLINTEGER cbNameType = SQL_NTS;

      SQLCHAR packageSource[DB_STR_LEN];
      packageSource[0] = '\0';
      SQLINTEGER cbPackageSource = SQL_NTS;

      SQLCHAR packageAlias[DB_STR_LEN];
      packageAlias[0] = '\0';
      SQLINTEGER cbPackageAlias = SQL_NTS;

      SQLCHAR packageHeight[DB_STR_LEN];
      packageHeight[0] = '\0';
      SQLINTEGER cbPackageHeight = SQL_NTS;

      SQLCHAR outlineMethod[DB_STR_LEN];
      outlineMethod[0] = '\0';
      SQLINTEGER cbOutlineMethod = SQL_NTS;

      SQLCHAR dpmo[DB_STR_LEN];
      dpmo[0] = '\0';
      SQLINTEGER cbDpmo = SQL_NTS;

      SQLCHAR family[DB_STR_LEN];
      family[0] = '\0';
      SQLINTEGER cbFamily = SQL_NTS;

      SQLCHAR userName[DB_STR_LEN];
      userName[0] = '\0';
      SQLINTEGER cbUsername = SQL_NTS;

      SQLCHAR dateTime[DB_STR_LEN];
      dateTime[0] = '\0';
      SQLINTEGER cbDateTime = SQL_NTS;

      SQLCHAR comment[DB_STR_LEN];
      comment[0] = '\0';
      SQLINTEGER cbComment = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Name, NameType, PackageSource, PackageAlias, PackageHeight, OutlineMethod, DPMO, Family, UserName, \"DateTime\", Comment FROM %s WHERE Name = '%s' AND NameType ='%s'" , 
            tabledef.GetTableName(), name, nametypeStr);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN rc1 = SQLGetData( stmt, 1, SQL_C_CHAR, itemName,      sizeof(itemName),      &cbItemName);
               SQLRETURN rc2 = SQLGetData( stmt, 2, SQL_C_CHAR, nameType,      sizeof(nameType),      &cbNameType);
               SQLRETURN rc3 = SQLGetData( stmt, 3, SQL_C_CHAR, packageSource, sizeof(packageSource), &cbPackageSource);
               SQLRETURN rc4 = SQLGetData( stmt, 4, SQL_C_CHAR, packageAlias,  sizeof(packageAlias),  &cbPackageAlias);
               SQLRETURN rc5 = SQLGetData( stmt, 5, SQL_C_CHAR, packageHeight, sizeof(packageHeight), &cbPackageHeight);
               SQLRETURN rc6 = SQLGetData( stmt, 6, SQL_C_CHAR, outlineMethod, sizeof(outlineMethod), &cbOutlineMethod);
               SQLRETURN rc7 = SQLGetData( stmt, 7, SQL_C_CHAR, dpmo,          sizeof(dpmo),          &cbDpmo);
               SQLRETURN rc8 = SQLGetData( stmt, 8, SQL_C_CHAR, family,        sizeof(family),        &cbFamily);
               SQLRETURN rc9 = SQLGetData( stmt, 9, SQL_C_CHAR, userName,      sizeof(userName),      &cbUsername);
               SQLRETURN rc10 = SQLGetData( stmt, 10, SQL_C_CHAR, dateTime,     sizeof(dateTime),      &cbDateTime);
               SQLRETURN rc11 = SQLGetData( stmt, 11, SQL_C_CHAR, comment,      sizeof(comment),       &cbComment);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  pkg = new CDBPackageInfo( itemName, nametype,
                     CDBPackageInfo::GetNameType( (rc3 == SQL_SUCCESS || rc3 == SQL_SUCCESS_WITH_INFO) ? packageSource : (CString)"" ),
                     (rc4 == SQL_SUCCESS || rc4 == SQL_SUCCESS_WITH_INFO) ? packageAlias  : (CString)"",
                     (rc5 == SQL_SUCCESS || rc5 == SQL_SUCCESS_WITH_INFO) ? packageHeight : (CString)"",
                     (rc6 == SQL_SUCCESS || rc6 == SQL_SUCCESS_WITH_INFO) ? outlineMethod : (CString)"",
                     (rc7 == SQL_SUCCESS || rc7 == SQL_SUCCESS_WITH_INFO) ? dpmo          : (CString)"",
                     (rc8 == SQL_SUCCESS || rc8 == SQL_SUCCESS_WITH_INFO) ? family        : (CString)"",
                     (rc9 == SQL_SUCCESS || rc9 == SQL_SUCCESS_WITH_INFO) ? userName      : (CString)"",
                     (rc10 == SQL_SUCCESS || rc10 == SQL_SUCCESS_WITH_INFO) ? dateTime    : (CString)"",
                     (rc11 == SQL_SUCCESS || rc11 == SQL_SUCCESS_WITH_INFO) ? comment     : (CString)"",
                     NULL
                     );
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   if (pkg != NULL)
   {
      // Fundemental Package lookup worked, add in the Generic Attributes
      CDBGenericAttributeMap *attmap = this->LookupGenericAttributes(name, nametype);
      pkg->SetAttribMap(attmap);
   }

   return pkg;
}

bool CDBSubclassElementList::containsSubclass(CString subclassName)
{
   // Return true if at least one element record for this subclass is in the list
   // Otherwise false

   for (int i = 0; i < this->GetCount(); i++)
   {
      CDBSubclassElement *dbel = (CDBSubclassElement*)this->GetAt(i);

      if (dbel != NULL)
      {
         if (subclassName.Compare(dbel->getSubclassName()) == 0)
         {
            return true;
         }
      }
   }

   return false;
}

CDBSubclassElementList *CDBInterface::LookupSubclassElements(CString querySubclassName)
{
   // Lookup all elements associated with querySubclassName.
   // If querySubclassName is blank then lookup all subclass elements.

   CDBSubclassElementList *list = new CDBSubclassElementList();

   CDBSubclassElementTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR subclassName[DB_STR_LEN];
      subclassName[0] = '\0';
      SQLINTEGER cbSubclassName = SQL_NTS;

      SQLCHAR elementName[DB_STR_LEN];
      elementName[0] = '\0';
      SQLINTEGER cbElementName = SQL_NTS;

      SQLCHAR devType[DB_STR_LEN];
      devType[0] = '\0';
      SQLINTEGER cbDevType = SQL_NTS;

      SQLCHAR pin1Name[DB_STR_LEN];
      pin1Name[0] = '\0';
      SQLINTEGER cbPin1Name = SQL_NTS;

      SQLCHAR pin2Name[DB_STR_LEN];
      pin2Name[0] = '\0';
      SQLINTEGER cbPin2Name = SQL_NTS;

      SQLCHAR pin3Name[DB_STR_LEN];
      pin3Name[0] = '\0';
      SQLINTEGER cbPin3Name = SQL_NTS;

      SQLCHAR pin4Name[DB_STR_LEN];
      pin4Name[0] = '\0';
      SQLINTEGER cbPin4Name = SQL_NTS;

      SQLCHAR value[DB_STR_LEN];
      value[0] = '\0';
      SQLINTEGER cbValue = SQL_NTS;

      SQLCHAR ptol[DB_STR_LEN];
      ptol[0] = '\0';
      SQLINTEGER cbPtol = SQL_NTS;

      SQLCHAR mtol[DB_STR_LEN];
      mtol[0] = '\0';
      SQLINTEGER cbMtol = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT SubclassName, ElementName, ElementDevType, Pin1Refname, Pin2Refname, Pin3Refname, Pin4Refname, ElementValue, PlusTol, MinusTol FROM %s" , tabledef.GetTableName());
 
         if (!querySubclassName.IsEmpty())
         {
            CString whereClause;
            whereClause.Format(" WHERE SubclassName ='%s'", querySubclassName);
            query += whereClause;
         }

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1 = SQLGetData( stmt,  1, SQL_C_CHAR, subclassName,  sizeof(subclassName),  &cbSubclassName);
               SQLRETURN  rc2 = SQLGetData( stmt,  2, SQL_C_CHAR, elementName,   sizeof(elementName),   &cbElementName);
               SQLRETURN  rc3 = SQLGetData( stmt,  3, SQL_C_CHAR, devType,       sizeof(devType),       &cbDevType);
               SQLRETURN  rc4 = SQLGetData( stmt,  4, SQL_C_CHAR, pin1Name,      sizeof(pin1Name),      &cbPin1Name);
               SQLRETURN  rc5 = SQLGetData( stmt,  5, SQL_C_CHAR, pin2Name,      sizeof(pin2Name),      &cbPin2Name);
               SQLRETURN  rc6 = SQLGetData( stmt,  6, SQL_C_CHAR, pin3Name,      sizeof(pin3Name),      &cbPin3Name);
               SQLRETURN  rc7 = SQLGetData( stmt,  7, SQL_C_CHAR, pin4Name,      sizeof(pin4Name),      &cbPin4Name);
               SQLRETURN  rc8 = SQLGetData( stmt,  8, SQL_C_CHAR, value,         sizeof(value),         &cbValue);
               SQLRETURN  rc9 = SQLGetData( stmt,  9, SQL_C_CHAR, ptol,          sizeof(ptol),          &cbPtol);
               SQLRETURN rc10 = SQLGetData( stmt, 10, SQL_C_CHAR, mtol,          sizeof(mtol),          &cbMtol);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBSubclassElement *el = new CDBSubclassElement(subclassName,
                     ( rc2 == SQL_SUCCESS ||  rc2 == SQL_SUCCESS_WITH_INFO) ? elementName   : (CString)"",
                     ( rc3 == SQL_SUCCESS ||  rc3 == SQL_SUCCESS_WITH_INFO) ? devType       : (CString)"",
                     ( rc4 == SQL_SUCCESS ||  rc4 == SQL_SUCCESS_WITH_INFO) ? pin1Name      : (CString)"",
                     ( rc5 == SQL_SUCCESS ||  rc5 == SQL_SUCCESS_WITH_INFO) ? pin2Name      : (CString)"",
                     ( rc6 == SQL_SUCCESS ||  rc6 == SQL_SUCCESS_WITH_INFO) ? pin3Name      : (CString)"",
                     ( rc7 == SQL_SUCCESS ||  rc7 == SQL_SUCCESS_WITH_INFO) ? pin4Name      : (CString)"",
                     ( rc8 == SQL_SUCCESS ||  rc8 == SQL_SUCCESS_WITH_INFO) ? value         : (CString)"",
                     ( rc9 == SQL_SUCCESS ||  rc9 == SQL_SUCCESS_WITH_INFO) ? ptol          : (CString)"",
                     (rc10 == SQL_SUCCESS || rc10 == SQL_SUCCESS_WITH_INFO) ? mtol          : (CString)""
                     );

                  list->Add(el);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return list;
}

bool CDBPanelTemplateList::ContainsName(CString templateName)
{
   // Return true if at least one element record for this subclass is in the list
   // Otherwise false

   for (int i = 0; i < this->GetCount(); i++)
   {
      CDBPanelTemplate *pt = (CDBPanelTemplate*)this->GetAt(i);

      if (pt != NULL)
      {
         if (templateName.Compare(pt->GetName()) == 0)
         {
            return true;
         }
      }
   }

   return false;
}

int CDBPanelTemplateList::GetNameArray(CStringArray &ar)
{
   // Return size of output

   ar.RemoveAll();

   for (int i = 0; i < this->GetCount(); i++)
   {
      CDBPanelTemplate *pt = (CDBPanelTemplate*)this->GetAt(i);

      if (pt != NULL)
      {
         ar.Add(pt->GetName());
      }
   }

   return ar.GetCount();
}


CDBPanelTemplate* CDBInterface::LookupPanelTemplateID(CString templateName)
{
   // First look in newest table, if not there look in older table.
   CDBPanelTemplateIDTableDef2 tabledef2;
   CDBPanelTemplate *panelTemplate = LookupPanelTemplateID(tabledef2, templateName);

   if (panelTemplate == NULL)
   {
      CDBPanelTemplateIDTableDef1 tabledef1;
      panelTemplate = LookupPanelTemplateID(tabledef1, templateName);
   }

   return panelTemplate;
}

CDBPanelTemplate* CDBInterface::LookupPanelTemplateID(CDBTableDef &tabledef, CString templateName)
{
   CDBPanelTemplate *panelTemplate = NULL;

   if (m_dbConnected && TableExists(tabledef.GetTableName()) &&
      !templateName.IsEmpty())
   {
      SQLCHAR tempName[DB_STR_LEN];
      tempName[0] = '\0';
      SQLINTEGER cbTempName = SQL_NTS;

      SQLCHAR units[DB_STR_LEN];
      units[0] = '\0';
      SQLINTEGER cbUnits = SQL_NTS;

      SQLCHAR userID[DB_STR_LEN];
      userID[0] = '\0';
      SQLINTEGER cbUserID = SQL_NTS;

      SQLCHAR dateTime[DB_STR_LEN];
      dateTime[0] = '\0';
      SQLINTEGER cbDateTime = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT TemplateName, Units, UserID, \"DateTime\" FROM %s WHERE TemplateName = '%s'" , tabledef.GetTableName(), templateName);
 
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1 = SQLGetData( stmt,  1, SQL_C_CHAR, tempName,      sizeof(tempName),      &cbTempName);
               SQLRETURN  rc2 = SQLGetData( stmt,  2, SQL_C_CHAR, units,         sizeof(units),         &cbUnits);
               SQLRETURN  rc3 = SQLGetData( stmt,  3, SQL_C_CHAR, userID,        sizeof(userID),        &cbUserID);
               SQLRETURN  rc4 = SQLGetData( stmt,  4, SQL_C_CHAR, dateTime,      sizeof(dateTime),      &cbDateTime);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  panelTemplate = new CDBPanelTemplate(templateName,
                     ( rc2 == SQL_SUCCESS ||  rc2 == SQL_SUCCESS_WITH_INFO) ? units         : (CString)"",
                     ( rc3 == SQL_SUCCESS ||  rc3 == SQL_SUCCESS_WITH_INFO) ? userID        : (CString)"",
                     ( rc4 == SQL_SUCCESS ||  rc4 == SQL_SUCCESS_WITH_INFO) ? dateTime      : (CString)""
                     );
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return panelTemplate;
}

bool CDBInterface::LookupPanelTemplateOutlines(CDBPanelTemplate &panelTemplate)
{
   // Must already have name set in panelTemplate

   panelTemplate.GetOutlineList().empty();

   CDBPanelTemplateOutlineTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()) &&
      !panelTemplate.GetName().IsEmpty())
   {
      SQLCHAR templateName[DB_STR_LEN];   // aka field1
      templateName[0] = '\0';
      SQLINTEGER cbTemplateName = SQL_NTS;

      SQLCHAR field2[DB_STR_LEN];
      field2[0] = '\0';
      SQLINTEGER cbField2 = SQL_NTS;

      SQLCHAR field3[DB_STR_LEN];
      field3[0] = '\0';
      SQLINTEGER cbField3 = SQL_NTS;

      SQLCHAR field4[DB_STR_LEN];
      field4[0] = '\0';
      SQLINTEGER cbField4 = SQL_NTS;

      SQLCHAR field5[DB_STR_LEN];
      field5[0] = '\0';
      SQLINTEGER cbField5 = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT TemplateName, OriginX, OriginY, SizeX, SizeY FROM %s WHERE TemplateName = '%s'" , tabledef.GetTableName(), panelTemplate.GetName());

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1 = SQLGetData( stmt,  1, SQL_C_CHAR, templateName,  sizeof(templateName),  &cbTemplateName);
               SQLRETURN  rc2 = SQLGetData( stmt,  2, SQL_C_CHAR, field2,        sizeof(field2),        &cbField2);
               SQLRETURN  rc3 = SQLGetData( stmt,  3, SQL_C_CHAR, field3,        sizeof(field3),        &cbField3);
               SQLRETURN  rc4 = SQLGetData( stmt,  4, SQL_C_CHAR, field4,        sizeof(field4),        &cbField4);
               SQLRETURN  rc5 = SQLGetData( stmt,  5, SQL_C_CHAR, field5,        sizeof(field5),        &cbField5);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBPanelTemplateOutline *item = new CDBPanelTemplateOutline(
                     ( rc2 == SQL_SUCCESS ||  rc2 == SQL_SUCCESS_WITH_INFO) ? field2        : (CString)"", // OriginX
                     ( rc3 == SQL_SUCCESS ||  rc3 == SQL_SUCCESS_WITH_INFO) ? field3        : (CString)"", // OriginY
                     ( rc4 == SQL_SUCCESS ||  rc4 == SQL_SUCCESS_WITH_INFO) ? field4        : (CString)"", // SizeX
                     ( rc5 == SQL_SUCCESS ||  rc5 == SQL_SUCCESS_WITH_INFO) ? field5        : (CString)""  // SizeY
                     );

                  panelTemplate.GetOutlineList().Add(item);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }

      return true;
   }

   return false;
}

bool CDBInterface::LookupPanelTemplatePcbInstances(CDBPanelTemplate &panelTemplate)
{
   // Must already have name set in panelTemplate

   panelTemplate.GetPcbInstanceList().empty();

   CDBPanelTemplatePcbInstanceTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()) &&
      !panelTemplate.GetName().IsEmpty())
   {
      SQLCHAR templateName[DB_STR_LEN];   // aka field1
      templateName[0] = '\0';
      SQLINTEGER cbTemplateName = SQL_NTS;

      SQLCHAR field2[DB_STR_LEN];
      field2[0] = '\0';
      SQLINTEGER cbField2 = SQL_NTS;

      SQLCHAR field3[DB_STR_LEN];
      field3[0] = '\0';
      SQLINTEGER cbField3 = SQL_NTS;

      SQLCHAR field4[DB_STR_LEN];
      field4[0] = '\0';
      SQLINTEGER cbField4 = SQL_NTS;

      SQLCHAR field5[DB_STR_LEN];
      field5[0] = '\0';
      SQLINTEGER cbField5 = SQL_NTS;

      SQLCHAR field6[DB_STR_LEN];
      field6[0] = '\0';
      SQLINTEGER cbField6 = SQL_NTS;

      SQLCHAR field7[DB_STR_LEN];
      field7[0] = '\0';
      SQLINTEGER cbField7 = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT TemplateName, PCBName, OriginX, OriginY, Angle, Mirrored, OriginType FROM %s WHERE TemplateName = '%s'" , tabledef.GetTableName(), panelTemplate.GetName());

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1 = SQLGetData( stmt,  1, SQL_C_CHAR, templateName,  sizeof(templateName),  &cbTemplateName);
               SQLRETURN  rc2 = SQLGetData( stmt,  2, SQL_C_CHAR, field2,        sizeof(field2),        &cbField2);
               SQLRETURN  rc3 = SQLGetData( stmt,  3, SQL_C_CHAR, field3,        sizeof(field3),        &cbField3);
               SQLRETURN  rc4 = SQLGetData( stmt,  4, SQL_C_CHAR, field4,        sizeof(field4),        &cbField4);
               SQLRETURN  rc5 = SQLGetData( stmt,  5, SQL_C_CHAR, field5,        sizeof(field5),        &cbField5);
               SQLRETURN  rc6 = SQLGetData( stmt,  6, SQL_C_CHAR, field6,        sizeof(field6),        &cbField6);
               SQLRETURN  rc7 = SQLGetData( stmt,  7, SQL_C_CHAR, field7,        sizeof(field7),        &cbField7);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBPanelTemplatePcbInstance *item = new CDBPanelTemplatePcbInstance(
                     ( rc2 == SQL_SUCCESS ||  rc2 == SQL_SUCCESS_WITH_INFO) ? field2        : (CString)"", // Name
                     ( rc3 == SQL_SUCCESS ||  rc3 == SQL_SUCCESS_WITH_INFO) ? field3        : (CString)"", // OriginX
                     ( rc4 == SQL_SUCCESS ||  rc4 == SQL_SUCCESS_WITH_INFO) ? field4        : (CString)"", // OriginY
                     ( rc5 == SQL_SUCCESS ||  rc5 == SQL_SUCCESS_WITH_INFO) ? field5        : (CString)"", // Angle
                     ( rc6 == SQL_SUCCESS ||  rc6 == SQL_SUCCESS_WITH_INFO) ? field6        : (CString)"", // Mirrored
                     ( rc7 == SQL_SUCCESS ||  rc7 == SQL_SUCCESS_WITH_INFO) ? field7        : (CString)""  // OriginType
                     );

                  panelTemplate.GetPcbInstanceList().Add(item);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }

      return true;
   }

   return false;
}

bool CDBInterface::LookupPanelTemplatePcbArrays(CDBPanelTemplate &panelTemplate)
{
   // Must already have name set in panelTemplate

   panelTemplate.GetPcbArrayList().empty();

   CDBPanelTemplatePcbArrayTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()) &&
      !panelTemplate.GetName().IsEmpty())
   {
      SQLCHAR templateName[DB_STR_LEN];   // aka field1
      templateName[0] = '\0';
      SQLINTEGER cbTemplateName = SQL_NTS;

      SQLCHAR field2[DB_STR_LEN];
      field2[0] = '\0';
      SQLINTEGER cbField2 = SQL_NTS;

      SQLCHAR field3[DB_STR_LEN];
      field3[0] = '\0';
      SQLINTEGER cbField3 = SQL_NTS;

      SQLCHAR field4[DB_STR_LEN];
      field4[0] = '\0';
      SQLINTEGER cbField4 = SQL_NTS;

      SQLCHAR field5[DB_STR_LEN];
      field5[0] = '\0';
      SQLINTEGER cbField5 = SQL_NTS;

      SQLCHAR field6[DB_STR_LEN];
      field6[0] = '\0';
      SQLINTEGER cbField6 = SQL_NTS;

      SQLCHAR field7[DB_STR_LEN];
      field7[0] = '\0';
      SQLINTEGER cbField7 = SQL_NTS;

      SQLCHAR field8[DB_STR_LEN];
      field8[0] = '\0';
      SQLINTEGER cbField8 = SQL_NTS;

      SQLCHAR field9[DB_STR_LEN];
      field9[0] = '\0';
      SQLINTEGER cbField9 = SQL_NTS;

      SQLCHAR field10[DB_STR_LEN];
      field10[0] = '\0';
      SQLINTEGER cbField10 = SQL_NTS;

      SQLCHAR field11[DB_STR_LEN];
      field11[0] = '\0';
      SQLINTEGER cbField11 = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT TemplateName, PCBName, OriginX, OriginY, Angle, Mirrored, OriginType, CountX, CountY, StepX, StepY FROM %s WHERE TemplateName = '%s'" , tabledef.GetTableName(), panelTemplate.GetName());

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1  = SQLGetData( stmt,  1, SQL_C_CHAR, templateName,  sizeof(templateName),  &cbTemplateName);
               SQLRETURN  rc2  = SQLGetData( stmt,  2, SQL_C_CHAR, field2,        sizeof(field2),        &cbField2);
               SQLRETURN  rc3  = SQLGetData( stmt,  3, SQL_C_CHAR, field3,        sizeof(field3),        &cbField3);
               SQLRETURN  rc4  = SQLGetData( stmt,  4, SQL_C_CHAR, field4,        sizeof(field4),        &cbField4);
               SQLRETURN  rc5  = SQLGetData( stmt,  5, SQL_C_CHAR, field5,        sizeof(field5),        &cbField5);
               SQLRETURN  rc6  = SQLGetData( stmt,  6, SQL_C_CHAR, field6,        sizeof(field6),        &cbField6);
               SQLRETURN  rc7  = SQLGetData( stmt,  7, SQL_C_CHAR, field7,        sizeof(field7),        &cbField7);
               SQLRETURN  rc8  = SQLGetData( stmt,  8, SQL_C_CHAR, field8,        sizeof(field8),        &cbField8);
               SQLRETURN  rc9  = SQLGetData( stmt,  9, SQL_C_CHAR, field9,        sizeof(field9),        &cbField9);
               SQLRETURN  rc10 = SQLGetData( stmt, 10, SQL_C_CHAR, field10,       sizeof(field10),       &cbField10);
               SQLRETURN  rc11 = SQLGetData( stmt, 11, SQL_C_CHAR, field11,       sizeof(field11),       &cbField11);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBPanelTemplatePcbArray *item = new CDBPanelTemplatePcbArray(
                     ( rc2  == SQL_SUCCESS ||  rc2  == SQL_SUCCESS_WITH_INFO) ? field2        : (CString)"", // Name
                     ( rc3  == SQL_SUCCESS ||  rc3  == SQL_SUCCESS_WITH_INFO) ? field3        : (CString)"", // OriginX
                     ( rc4  == SQL_SUCCESS ||  rc4  == SQL_SUCCESS_WITH_INFO) ? field4        : (CString)"", // OriginY
                     ( rc5  == SQL_SUCCESS ||  rc5  == SQL_SUCCESS_WITH_INFO) ? field5        : (CString)"", // Angle
                     ( rc6  == SQL_SUCCESS ||  rc6  == SQL_SUCCESS_WITH_INFO) ? field6        : (CString)"", // Mirrored
                     ( rc7  == SQL_SUCCESS ||  rc7  == SQL_SUCCESS_WITH_INFO) ? field7        : (CString)"", // OriginType
                     ( rc8  == SQL_SUCCESS ||  rc8  == SQL_SUCCESS_WITH_INFO) ? field8        : (CString)"", // CountX
                     ( rc9  == SQL_SUCCESS ||  rc9  == SQL_SUCCESS_WITH_INFO) ? field9        : (CString)"", // CountY
                     ( rc10 == SQL_SUCCESS ||  rc10 == SQL_SUCCESS_WITH_INFO) ? field10       : (CString)"", // StepX
                     ( rc11 == SQL_SUCCESS ||  rc11 == SQL_SUCCESS_WITH_INFO) ? field11       : (CString)""  // StepY
                     );

                  panelTemplate.GetPcbArrayList().Add(item);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }

      return true;
   }

   return false;
}

bool CDBInterface::LookupPanelTemplateFiducials(CDBPanelTemplate &panelTemplate)
{
   // Must already have name set in panelTemplate

   panelTemplate.GetFiducialList().empty();

   CDBPanelTemplateFiducialTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()) &&
      !panelTemplate.GetName().IsEmpty())
   {
      SQLCHAR templateName[DB_STR_LEN];   // aka field1
      templateName[0] = '\0';
      SQLINTEGER cbTemplateName = SQL_NTS;

      SQLCHAR field2[DB_STR_LEN];
      field2[0] = '\0';
      SQLINTEGER cbField2 = SQL_NTS;

      SQLCHAR field3[DB_STR_LEN];
      field3[0] = '\0';
      SQLINTEGER cbField3 = SQL_NTS;

      SQLCHAR field4[DB_STR_LEN];
      field4[0] = '\0';
      SQLINTEGER cbField4 = SQL_NTS;

      SQLCHAR field5[DB_STR_LEN];
      field5[0] = '\0';
      SQLINTEGER cbField5 = SQL_NTS;

      SQLCHAR field6[DB_STR_LEN];
      field6[0] = '\0';
      SQLINTEGER cbField6 = SQL_NTS;

      SQLCHAR field7[DB_STR_LEN];
      field7[0] = '\0';
      SQLINTEGER cbField7 = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT TemplateName, Type, Size, OriginX, OriginY, Angle, Mirrored FROM %s WHERE TemplateName = '%s'" , tabledef.GetTableName(), panelTemplate.GetName());

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1 = SQLGetData( stmt,  1, SQL_C_CHAR, templateName,  sizeof(templateName),  &cbTemplateName);
               SQLRETURN  rc2 = SQLGetData( stmt,  2, SQL_C_CHAR, field2,        sizeof(field2),        &cbField2);
               SQLRETURN  rc3 = SQLGetData( stmt,  3, SQL_C_CHAR, field3,        sizeof(field3),        &cbField3);
               SQLRETURN  rc4 = SQLGetData( stmt,  4, SQL_C_CHAR, field4,        sizeof(field4),        &cbField4);
               SQLRETURN  rc5 = SQLGetData( stmt,  5, SQL_C_CHAR, field5,        sizeof(field5),        &cbField5);
               SQLRETURN  rc6 = SQLGetData( stmt,  6, SQL_C_CHAR, field6,        sizeof(field6),        &cbField6);
               SQLRETURN  rc7 = SQLGetData( stmt,  7, SQL_C_CHAR, field7,        sizeof(field7),        &cbField7);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBPanelTemplateFiducial *item = new CDBPanelTemplateFiducial(
                     ( rc2 == SQL_SUCCESS ||  rc2 == SQL_SUCCESS_WITH_INFO) ? field2        : (CString)"", // Type
                     ( rc3 == SQL_SUCCESS ||  rc3 == SQL_SUCCESS_WITH_INFO) ? field3        : (CString)"", // Size
                     ( rc4 == SQL_SUCCESS ||  rc4 == SQL_SUCCESS_WITH_INFO) ? field4        : (CString)"", // X
                     ( rc5 == SQL_SUCCESS ||  rc5 == SQL_SUCCESS_WITH_INFO) ? field5        : (CString)"", // Y
                     ( rc6 == SQL_SUCCESS ||  rc6 == SQL_SUCCESS_WITH_INFO) ? field6        : (CString)"", // Angle
                     ( rc7 == SQL_SUCCESS ||  rc7 == SQL_SUCCESS_WITH_INFO) ? field7        : (CString)""  // Mirrored
                     );

                  panelTemplate.GetFiducialList().Add(item);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }

      return true;
   }

   return false;
}

bool CDBInterface::LookupPanelTemplateToolingHoles(CDBPanelTemplate &panelTemplate)
{
   // Must already have name set in panelTemplate

   panelTemplate.GetToolingHoleList().empty();

   CDBPanelTemplateToolingHoleTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()) &&
      !panelTemplate.GetName().IsEmpty())
   {
      SQLCHAR templateName[DB_STR_LEN];  // aka field1
      templateName[0] = '\0';
      SQLINTEGER cbTemplateName = SQL_NTS;

      SQLCHAR field2[DB_STR_LEN];
      field2[0] = '\0';
      SQLINTEGER cbField2 = SQL_NTS;

      SQLCHAR field3[DB_STR_LEN];
      field3[0] = '\0';
      SQLINTEGER cbField3 = SQL_NTS;

      SQLCHAR field4[DB_STR_LEN];
      field4[0] = '\0';
      SQLINTEGER cbField4 = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT TemplateName, Size, OriginX, OriginY FROM %s WHERE TemplateName = '%s'" , tabledef.GetTableName(), panelTemplate.GetName());

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1 = SQLGetData( stmt,  1, SQL_C_CHAR, templateName,  sizeof(templateName),  &cbTemplateName);
               SQLRETURN  rc2 = SQLGetData( stmt,  2, SQL_C_CHAR, field2,        sizeof(field2),        &cbField2);
               SQLRETURN  rc3 = SQLGetData( stmt,  3, SQL_C_CHAR, field3,        sizeof(field3),        &cbField3);
               SQLRETURN  rc4 = SQLGetData( stmt,  4, SQL_C_CHAR, field4,        sizeof(field4),        &cbField4);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBPanelTemplateToolingHole *item = new CDBPanelTemplateToolingHole(
                     ( rc2 == SQL_SUCCESS ||  rc2 == SQL_SUCCESS_WITH_INFO) ? field2        : (CString)"",
                     ( rc3 == SQL_SUCCESS ||  rc3 == SQL_SUCCESS_WITH_INFO) ? field3        : (CString)"",
                     ( rc4 == SQL_SUCCESS ||  rc4 == SQL_SUCCESS_WITH_INFO) ? field4        : (CString)""
                     );

                  panelTemplate.GetToolingHoleList().Add(item);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }

      return true;
   }

   return false;
}

CDBPanelTemplate *CDBInterface::LookupPanelTemplate(CString name)
{
   CDBPanelTemplate *pt = LookupPanelTemplateID(name);

   if (pt != NULL)
   {
      LookupPanelTemplateOutlines(*pt);
      LookupPanelTemplatePcbInstances(*pt);
      LookupPanelTemplatePcbArrays(*pt);
      LookupPanelTemplateFiducials(*pt);
      LookupPanelTemplateToolingHoles(*pt);
   }

   return pt;
}

CDBPanelTemplateList *CDBInterface::LookupPanelTemplates(bool templateNamesOnly)
{
   CDBPanelTemplateList *list = new CDBPanelTemplateList();

   CDBPanelTemplateIDTableDef2 tabledef;  // Not backward compatible

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR templateName[DB_STR_LEN];
      templateName[0] = '\0';
      SQLINTEGER cbTemplateName = SQL_NTS;

      SQLCHAR units[DB_STR_LEN];
      units[0] = '\0';
      SQLINTEGER cbUnits = SQL_NTS;

      SQLCHAR userID[DB_STR_LEN];
      userID[0] = '\0';
      SQLINTEGER cbUserID = SQL_NTS;

      SQLCHAR dateTime[DB_STR_LEN];
      dateTime[0] = '\0';
      SQLINTEGER cbDateTime = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT TemplateName, Units, UserID, \"DateTime\" FROM %s" , tabledef.GetTableName());
 
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1 = SQLGetData( stmt,  1, SQL_C_CHAR, templateName,  sizeof(templateName),  &cbTemplateName);
               SQLRETURN  rc2 = SQLGetData( stmt,  2, SQL_C_CHAR, units,         sizeof(units),         &cbUnits);
               SQLRETURN  rc3 = SQLGetData( stmt,  3, SQL_C_CHAR, userID,        sizeof(userID),        &cbUserID);
               SQLRETURN  rc4 = SQLGetData( stmt,  4, SQL_C_CHAR, dateTime,      sizeof(dateTime),      &cbDateTime);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBPanelTemplate *pt = new CDBPanelTemplate(templateName,
                     ( rc2 == SQL_SUCCESS ||  rc2 == SQL_SUCCESS_WITH_INFO) ? units         : (CString)"",
                     ( rc3 == SQL_SUCCESS ||  rc3 == SQL_SUCCESS_WITH_INFO) ? userID        : (CString)"",
                     ( rc4 == SQL_SUCCESS ||  rc4 == SQL_SUCCESS_WITH_INFO) ? dateTime      : (CString)""
                     );

                  list->Add(pt);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return list;
}

CDBGenericAttributeMap *CDBInterface::LookupGenericAttributes(CString name, EDBNameType nametype)
{
   // Lookup all attribs associated with name / nameType

   CString nametypeStr( CDBPackageInfo::GetNameTypeStr(nametype) );

   CDBGenericAttributeMap *attribMap = new CDBGenericAttributeMap;

   CDBGenericAttributeTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR itemName[DB_STR_LEN];
      itemName[0] = '\0';
      SQLINTEGER cbItemName = SQL_NTS;

      SQLCHAR nameType[DB_STR_LEN];
      nameType[0] = '\0';
      SQLINTEGER cbNameType = SQL_NTS;

      SQLCHAR attribName[DB_STR_LEN];
      attribName[0] = '\0';
      SQLINTEGER cbAttribName = SQL_NTS;

      SQLCHAR attribValue[DB_STR_LEN];
      attribValue[0] = '\0';
      SQLINTEGER cbAttribValue = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Name, NameType, AttribName, AttribValue FROM %s WHERE Name = '%s' AND NameType ='%s'" , tabledef.GetTableName(), name, nametypeStr);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN rc1 = SQLGetData( stmt, 1, SQL_C_CHAR, itemName,      sizeof(itemName),      &cbItemName);
               SQLRETURN rc2 = SQLGetData( stmt, 2, SQL_C_CHAR, nameType,      sizeof(nameType),      &cbNameType);
               SQLRETURN rc3 = SQLGetData( stmt, 3, SQL_C_CHAR, attribName,    sizeof(attribName),    &cbAttribName);
               SQLRETURN rc4 = SQLGetData( stmt, 4, SQL_C_CHAR, attribValue,   sizeof(attribValue),   &cbAttribValue);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBNameValuePair *nvp = new CDBNameValuePair(
                     (rc3 == SQL_SUCCESS || rc3 == SQL_SUCCESS_WITH_INFO) ? attribName  : (CString)"",
                     (rc4 == SQL_SUCCESS || rc4 == SQL_SUCCESS_WITH_INFO) ? attribValue : (CString)""
                     );

                  if (nvp->GetName().IsEmpty())  // somebody hosed the DB
                     delete nvp;
                  else
                     attribMap->SetAt(nvp->GetName(), nvp);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return attribMap;
}

//---------------------------------------------------------------------

int CDBOutlineVertexMap::VertexOrderSortFunc(const void *a, const void *b)
{
   CDBOutlineVertex* itemA = (CDBOutlineVertex*)(((SElement*) a )->pObject->m_object);
   CDBOutlineVertex* itemB = (CDBOutlineVertex*)(((SElement*) b )->pObject->m_object);

   int aval = itemA->getPolyNum();
   int bval = itemB->getPolyNum();

   if (aval < bval)
      return -1;
   else if (aval > bval)
      return 1;

   // Same poly num, ose point num
   aval = itemA->getPointNum();
   bval = itemB->getPointNum();

   if (aval < bval)
      return -1;
   else if (aval > bval)
      return 1;

   return 0;  // Should never get here, would mean two points have same ordinal number
}

//---------------------------------------------------------------------

CDBOutlineVertexMap *CDBInterface::LookupOutline(CString name, EDBNameType nametype)
{
   // Lookup all outline vertices associated with name / nameType

   CString nametypeStr( CDBPackageInfo::GetNameTypeStr(nametype) );

   CDBOutlineVertexMap *map = new CDBOutlineVertexMap;

   CDBOutlineVertexTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR itemName[DB_STR_LEN];
      itemName[0] = '\0';
      SQLINTEGER cbItemName = SQL_NTS;

      SQLCHAR nameType[DB_STR_LEN];
      nameType[0] = '\0';
      SQLINTEGER cbNameType = SQL_NTS;

      SQLCHAR polyNum[DB_STR_LEN];
      polyNum[0] = '\0';
      SQLINTEGER cbPolyNum = SQL_NTS;

      SQLCHAR pointNum[DB_STR_LEN];
      pointNum[0] = '\0';
      SQLINTEGER cbPointNum = SQL_NTS;

      SQLCHAR xxxStr[DB_STR_LEN];
      xxxStr[0] = '\0';
      SQLINTEGER cbXxxStr = SQL_NTS;
            
      SQLCHAR yyyStr[DB_STR_LEN];
      yyyStr[0] = '\0';
      SQLINTEGER cbYyyStr = SQL_NTS;
            
      SQLCHAR bulgeStr[DB_STR_LEN];
      bulgeStr[0] = '\0';
      SQLINTEGER cbBulgeStr = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Name, NameType, PolyNum, PointNum, X, Y, Bulge FROM %s WHERE Name = '%s' AND NameType ='%s'" , tabledef.GetTableName(), name, nametypeStr);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         int defaultPointNum = 0;

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN rc1 = SQLGetData( stmt, 1, SQL_C_CHAR, itemName,      sizeof(itemName),      &cbItemName);
               SQLRETURN rc2 = SQLGetData( stmt, 2, SQL_C_CHAR, nameType,      sizeof(nameType),      &cbNameType);
               SQLRETURN rc3 = SQLGetData( stmt, 3, SQL_C_CHAR, polyNum,       sizeof(polyNum),       &cbPolyNum);
               SQLRETURN rc4 = SQLGetData( stmt, 4, SQL_C_CHAR, pointNum,      sizeof(pointNum),      &cbPointNum);
               SQLRETURN rc5 = SQLGetData( stmt, 5, SQL_C_CHAR, xxxStr,        sizeof(xxxStr),        &cbXxxStr);
               SQLRETURN rc6 = SQLGetData( stmt, 6, SQL_C_CHAR, yyyStr,        sizeof(yyyStr),        &cbYyyStr);
               SQLRETURN rc7 = SQLGetData( stmt, 7, SQL_C_CHAR, bulgeStr,      sizeof(bulgeStr),      &cbBulgeStr);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  defaultPointNum++;
                  CString defaultPointNumStr;
                  defaultPointNumStr.Format("%d", defaultPointNum);

                  CDBOutlineVertex *v = new CDBOutlineVertex(name, nametype,
                     (rc3 == SQL_SUCCESS || rc3 == SQL_SUCCESS_WITH_INFO) ? polyNum  : (CString)"1",
                     (rc4 == SQL_SUCCESS || rc4 == SQL_SUCCESS_WITH_INFO) ? pointNum : defaultPointNumStr,
                     (rc5 == SQL_SUCCESS || rc5 == SQL_SUCCESS_WITH_INFO) ? xxxStr  :  (CString)"0.",
                     (rc5 == SQL_SUCCESS || rc5 == SQL_SUCCESS_WITH_INFO) ? yyyStr  :  (CString)"0.",
                     (rc5 == SQL_SUCCESS || rc5 == SQL_SUCCESS_WITH_INFO) ? bulgeStr : (CString)"0."
                     );

                  // Don't really need to be using a map, but this is the most prominent
                  // container collection class that has a sorting interface, which is needed.
                  CString key;
                  key.Format("%d", defaultPointNum);
                  map->SetAt(key, v);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return map;
}



bool CDBInterface::LookupGenericAttribute(CString name, EDBNameType nametype, CString incomingAttribName, CString& returnAttribValue)
{
   // Get this particular attrib value, returns true if attribute exists because
   // blank return attribValue is ambiguous.

   CString nametypeStr( CDBPackageInfo::GetNameTypeStr(nametype) );

   returnAttribValue = "";
   bool found = false;

   CDBGenericAttributeTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR itemName[DB_STR_LEN];
      itemName[0] = '\0';
      SQLINTEGER cbItemName = SQL_NTS;

      SQLCHAR nameType[DB_STR_LEN];
      nameType[0] = '\0';
      SQLINTEGER cbNameType = SQL_NTS;

      SQLCHAR attribName[DB_STR_LEN];
      attribName[0] = '\0';
      SQLINTEGER cbAttribName = SQL_NTS;

      SQLCHAR attribValue[DB_STR_LEN];
      attribValue[0] = '\0';
      SQLINTEGER cbAttribValue = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Name, NameType, AttribName, AttribValue FROM %s WHERE Name = '%s' AND NameType ='%s' AND AttribName = '%s'" , tabledef.GetTableName(), name, nametypeStr, incomingAttribName);

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            if ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN rc1 = SQLGetData( stmt, 1, SQL_C_CHAR, itemName,      sizeof(itemName),      &cbItemName);
               SQLRETURN rc2 = SQLGetData( stmt, 2, SQL_C_CHAR, nameType,      sizeof(nameType),      &cbNameType);
               SQLRETURN rc3 = SQLGetData( stmt, 3, SQL_C_CHAR, attribName,    sizeof(attribName),    &cbAttribName);
               SQLRETURN rc4 = SQLGetData( stmt, 4, SQL_C_CHAR, attribValue,   sizeof(attribValue),   &cbAttribValue);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  returnAttribValue = (rc4 == SQL_SUCCESS || rc4 == SQL_SUCCESS_WITH_INFO) ? attribValue : (CString)"";
                  found = true;
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return found;
}

bool CDBInterface::GetExistingGenericAttribNames(CStringArray &existingAttribNames)
{
   // All AttribNames, regardless of name key type.

   existingAttribNames.RemoveAll();
   bool found = false;

   CDBGenericAttributeTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR attribName[DB_STR_LEN];
      attribName[0] = '\0';
      SQLINTEGER cbAttribName = SQL_NTS;

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         CString query;
         query.Format("SELECT DISTINCT AttribName FROM %s;" , tabledef.GetTableName());

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN rc1 = SQLGetData( stmt, 1, SQL_C_CHAR, attribName,    sizeof(attribName),    &cbAttribName);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CString name = (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO) ? attribName : (CString)"";
                  
                  if (!name.IsEmpty())
                  {
                     found = true;
                     existingAttribNames.Add(name);
                  }
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return found;

}

CDBStencilSetting *CDBInterface::LookupStencilSetting(CString settingname)
{
   CDBStencilSetting *setting = NULL;

   CDBStencilSettingsTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR name[DB_STR_LEN];
      name[0] = '\0';
      SQLINTEGER cbName = SQL_NTS;

      SQLCHAR value[DB_STR_LEN];
      value[0] = '\0';
      SQLINTEGER cbValue = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, name, sizeof(name), &cbName);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, value, sizeof(value), &cbValue);


         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         // If passed in a setting name then look up that specific setting.
         // Otherwise get all the settings.
         CString query;
         query.Format("SELECT SettingName, SettingValue FROM %s", tabledef.GetTableName());
         if (!settingname.IsEmpty())
            query += " WHERE SettingName = '" + settingname + "'";

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or one entry in the result, but there are possibly more.
            // Which to take is arbitrary, if there are more than one. We take the first one.
            CDBStencilSetting *prevSetting = NULL;
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               setting = new CDBStencilSetting(name, value, prevSetting);
               prevSetting = setting;
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return setting;
}



#ifdef DEADCODE
CDBStencilRule *CDBInterface::LookupStencilRule1(CString entity, CString designatorPrefix)
{
   // Lookup entire set that matches entity and prefix, regardless of suffix.

   CDBStencilRule *stencilrule = NULL;

   CDBStencilRules1TableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR dbentity[DB_STR_LEN];
      dbentity[0] = '\0';
      SQLINTEGER cbDbentity = SQL_NTS;

      SQLCHAR despre[DB_STR_LEN];
      despre[0] = '\0';
      SQLINTEGER cbDespre = SQL_NTS;

      SQLCHAR dessuff[DB_STR_LEN];
      dessuff[0] = '\0';
      SQLINTEGER cbDessuff = SQL_NTS;

      SQLCHAR rulestr[DB_STR_LEN];
      rulestr[0] = '\0';
      SQLINTEGER cbRulestr = SQL_NTS;

      SQLCHAR username[DB_STR_LEN];
      username[0] = '\0';
      SQLINTEGER cbUsername = SQL_NTS;

      SQLCHAR datetime[DB_STR_LEN];
      datetime[0] = '\0';
      SQLINTEGER cbDatetime = SQL_NTS;

      SQLCHAR comment[DB_STR_LEN];
      comment[0] = '\0';
      SQLINTEGER cbComment = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, dbentity, sizeof(dbentity), &cbDbentity);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, despre, sizeof(despre), &cbDespre);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, dessuff, sizeof(dessuff), &cbDessuff);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, rulestr, sizeof(rulestr), &cbRulestr);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, username, sizeof(username), &cbUsername);

         rc = SQLBindCol( stmt, 6, 
            SQL_C_CHAR, datetime, sizeof(datetime), &cbDatetime);

         rc = SQLBindCol( stmt, 7, 
            SQL_C_CHAR, comment, sizeof(comment), &cbComment);

         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Entity, DesignatorPrefix, DesignatorSuffix, RuleString, UserName, DateTime, Comments FROM %s WHERE Entity = '%s'" , 
            tabledef.GetTableName(), entity);
         // next line is original style
         // queryStr += " AND DesignatorPrefix = '" + designatorPrefix + "'";
         // But I think it needs to be more like this:
         if (!designatorPrefix.IsEmpty())
            query += " AND DesignatorPrefix = '" + designatorPrefix + "'";
         else
            query += " AND (DesignatorPrefix = '' OR DesignatorPrefix IS NULL)";

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // These queries may return multiple rows
            CDBStencilRule *prevRule = NULL;
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               stencilrule = new CDBStencilRule(entity, despre, dessuff, rulestr, username, datetime, comment, prevRule);
               prevRule = stencilrule;
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return stencilrule;
}
#endif

CDBStencilRule2* CDBInterface::LookupStencilRule(const CString& entity, const CString& argDesignator1, const CString& argDesignator2)
{
   CDBStencilRule2* rule = LookupStencilRule4(entity, argDesignator1, argDesignator2);
   if (rule == NULL)
      rule = LookupStencilRule3(entity, argDesignator1, argDesignator2);
   if (rule == NULL)
      rule = LookupStencilRule2(entity, argDesignator1, argDesignator2);

   return rule;
}

CDBStencilRule2* CDBInterface::LookupStencilRule2(const CString& entity,const CString& argDesignator1,const CString& argDesignator2)
{
   CDBStencilRules2TableDef tabledef;
   return LookupStencilRuleTable2or3or4(tabledef, entity, argDesignator1, argDesignator2);
}

CDBStencilRule2* CDBInterface::LookupStencilRule3(const CString& entity,const CString& argDesignator1,const CString& argDesignator2)
{
   CDBStencilRules3TableDef tabledef;
   return LookupStencilRuleTable2or3or4(tabledef, entity, argDesignator1, argDesignator2);
}

CDBStencilRule2* CDBInterface::LookupStencilRule4(const CString& entity,const CString& argDesignator1,const CString& argDesignator2)
{
   CDBStencilRules4TableDef tabledef;
   return LookupStencilRuleTable2or3or4(tabledef, entity, argDesignator1, argDesignator2);
}

CDBStencilRule2* CDBInterface::LookupStencilRuleTable2or3or4(CDBTableDef &tabledef,const CString& entity,const CString& argDesignator1,const CString& argDesignator2)
{
   // Lookup entire set that matches entity and prefix, regardless of suffix.

   CDBStencilRule2 *stencilrule = NULL;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR dbentity[DB_STR_LEN];
      dbentity[0] = '\0';
      SQLINTEGER cbDbentity = SQL_NTS;

      SQLCHAR designator1[DB_STR_LEN];
      designator1[0] = '\0';
      SQLINTEGER cbDesignator1 = SQL_NTS;

      SQLCHAR designator2[DB_STR_LEN];
      designator2[0] = '\0';
      SQLINTEGER cbDesignator2 = SQL_NTS;

      SQLCHAR designator3[DB_STR_LEN];
      designator3[0] = '\0';
      SQLINTEGER cbDesignator3 = SQL_NTS;

      SQLCHAR designator4[DB_STR_LEN];
      designator4[0] = '\0';
      SQLINTEGER cbDesignator4 = SQL_NTS;

      SQLCHAR designator5[DB_STR_LEN];
      designator5[0] = '\0';
      SQLINTEGER cbDesignator5 = SQL_NTS;

      SQLCHAR rulestr[DB_STR_LEN];
      rulestr[0] = '\0';
      SQLINTEGER cbRulestr = SQL_NTS;

      SQLCHAR username[DB_STR_LEN];
      username[0] = '\0';
      SQLINTEGER cbUsername = SQL_NTS;

      SQLCHAR datetime[DB_STR_LEN];
      datetime[0] = '\0';
      SQLINTEGER cbDatetime = SQL_NTS;

      SQLCHAR comment[DB_STR_LEN];
      comment[0] = '\0';
      SQLINTEGER cbComment = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, dbentity, sizeof(dbentity), &cbDbentity);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, designator1, sizeof(designator1), &cbDesignator1);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, designator2, sizeof(designator2), &cbDesignator2);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, designator3, sizeof(designator3), &cbDesignator3);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, designator4, sizeof(designator4), &cbDesignator4);

         rc = SQLBindCol( stmt, 6, 
            SQL_C_CHAR, designator5, sizeof(designator5), &cbDesignator5);

         rc = SQLBindCol( stmt, 7, 
            SQL_C_CHAR, rulestr, sizeof(rulestr), &cbRulestr);

         rc = SQLBindCol( stmt, 8, 
            SQL_C_CHAR, username, sizeof(username), &cbUsername);

         rc = SQLBindCol( stmt, 9, 
            SQL_C_CHAR, datetime, sizeof(datetime), &cbDatetime);

         rc = SQLBindCol( stmt,10, 
            SQL_C_CHAR, comment, sizeof(comment), &cbComment);

         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Entity, Designator1, Designator2, Designator3, Designator4, Designator5, RuleString, UserName, DateTime, Comments FROM %s WHERE Entity = '%s'" , 
            tabledef.GetTableName(), entity);

         if (!argDesignator1.IsEmpty())
            query += " AND Designator1 = '" + argDesignator1 + "'";
         else
            query += " AND (Designator1 = '' OR Designator1 IS NULL)";

         if (!argDesignator2.IsEmpty())
            query += " AND Designator2 = '" + argDesignator2 + "'";
         else
            query += " AND (Designator2 = '' OR Designator2 IS NULL)";

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // These queries may return multiple rows
            CDBStencilRule2 *prevRule = NULL;
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               stencilrule = new CDBStencilRule2(entity, designator1, designator2, designator3, designator4, designator5, rulestr, username, datetime, comment, prevRule);
               prevRule = stencilrule;
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return stencilrule;
}
#ifdef DEADCODE
CDBStencilRule *CDBInterface::LookupStencilRule1(CString entity, CString designatorPrefix, CString designatorSuffix)
{
   // Lookup exact entity/prefix/suffix match

   CDBStencilRule *stencilrule = NULL;

   CDBStencilRules1TableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR dbentity[DB_STR_LEN];
      dbentity[0] = '\0';
      SQLINTEGER cbDbentity = SQL_NTS;

      SQLCHAR despre[DB_STR_LEN];
      despre[0] = '\0';
      SQLINTEGER cbDespre = SQL_NTS;

      SQLCHAR dessuff[DB_STR_LEN];
      dessuff[0] = '\0';
      SQLINTEGER cbDessuff = SQL_NTS;

      SQLCHAR rulestr[DB_STR_LEN];
      rulestr[0] = '\0';
      SQLINTEGER cbRulestr = SQL_NTS;

      SQLCHAR username[DB_STR_LEN];
      username[0] = '\0';
      SQLINTEGER cbUsername = SQL_NTS;

      SQLCHAR datetime[DB_STR_LEN];
      datetime[0] = '\0';
      SQLINTEGER cbDatetime = SQL_NTS;

      SQLCHAR comment[DB_STR_LEN];
      comment[0] = '\0';
      SQLINTEGER cbComment = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, dbentity, sizeof(dbentity), &cbDbentity);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, despre, sizeof(despre), &cbDespre);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, dessuff, sizeof(dessuff), &cbDessuff);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, rulestr, sizeof(rulestr), &cbRulestr);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, username, sizeof(username), &cbUsername);

         rc = SQLBindCol( stmt, 6, 
            SQL_C_CHAR, datetime, sizeof(datetime), &cbDatetime);

         rc = SQLBindCol( stmt, 7, 
            SQL_C_CHAR, comment, sizeof(comment), &cbComment);

         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Entity, DesignatorPrefix, DesignatorSuffix, RuleString, UserName, DateTime, Comments FROM %s WHERE Entity = '%s'" , 
            tabledef.GetTableName(), entity);
         
         if (!designatorPrefix.IsEmpty())
            query += " AND DesignatorPrefix = '" + designatorPrefix + "'";
         else
            query += " AND (DesignatorPrefix = '' OR DesignatorPrefix IS NULL)";

         if (!designatorSuffix.IsEmpty())
            query += " AND DesignatorSuffix = '" + designatorSuffix + "'";
         else
            query += " AND (DesignatorSuffix = '' OR DesignatorSuffix IS NULL)";

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // These queries may return multiple rows
            CDBStencilRule *prevRule = NULL;
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               stencilrule = new CDBStencilRule(entity, despre, dessuff, rulestr, username, datetime, comment, prevRule);
               prevRule = stencilrule;
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return stencilrule;
}
#endif

#ifdef DEADCODE // not used
CDBStencilRule2* CDBInterface::LookupStencilRule2(const CString& entity, const CString& argDesignator1, const CString& argDesignator2,
                                                  const CString& argDesignator3, const CString& argDesignator4, const CString& argDesignator5)
{
   CDBStencilRules2TableDef tabledef;
   return LookupStencilRuleTable2or3or4(tabledef, entity, argDesignator1, argDesignator2, argDesignator3, argDesignator4, argDesignator5);
}
#endif
#ifdef DEADCODE // not used
CDBStencilRule2* CDBInterface::LookupStencilRule3(const CString& entity, const CString& argDesignator1, const CString& argDesignator2,
                                                  const CString& argDesignator3, const CString& argDesignator4, const CString& argDesignator5)
{
   CDBStencilRules3TableDef tabledef;
   return LookupStencilRuleTable2or3or4(tabledef, entity, argDesignator1, argDesignator2, argDesignator3, argDesignator4, argDesignator5);
}
#endif
CDBStencilRule2* CDBInterface::LookupStencilRule4(const CString& entity, const CString& argDesignator1, const CString& argDesignator2,
                                                  const CString& argDesignator3, const CString& argDesignator4, const CString& argDesignator5)
{
   CDBStencilRules4TableDef tabledef;
   return LookupStencilRuleTable2or3or4(tabledef, entity, argDesignator1, argDesignator2, argDesignator3, argDesignator4, argDesignator5);
}

CDBStencilRule2* CDBInterface::LookupStencilRuleTable2or3or4(CDBTableDef &tabledef, 
                                                  const CString& entity, const CString& argDesignator1, const CString& argDesignator2,
                                                  const CString& argDesignator3, const CString& argDesignator4, const CString& argDesignator5)
{
   // Lookup exact entity/prefix/suffix match

   CDBStencilRule2 *stencilrule = NULL;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR dbentity[DB_STR_LEN];
      dbentity[0] = '\0';
      SQLINTEGER cbDbentity = SQL_NTS;

      SQLCHAR designator1[DB_STR_LEN];
      designator1[0] = '\0';
      SQLINTEGER cbDesignator1 = SQL_NTS;

      SQLCHAR designator2[DB_STR_LEN];
      designator2[0] = '\0';
      SQLINTEGER cbDesignator2 = SQL_NTS;

      SQLCHAR designator3[DB_STR_LEN];
      designator3[0] = '\0';
      SQLINTEGER cbDesignator3 = SQL_NTS;

      SQLCHAR designator4[DB_STR_LEN];
      designator4[0] = '\0';
      SQLINTEGER cbDesignator4 = SQL_NTS;

      SQLCHAR designator5[DB_STR_LEN];
      designator5[0] = '\0';
      SQLINTEGER cbDesignator5 = SQL_NTS;

      SQLCHAR rulestr[DB_STR_LEN];
      rulestr[0] = '\0';
      SQLINTEGER cbRulestr = SQL_NTS;

      SQLCHAR username[DB_STR_LEN];
      username[0] = '\0';
      SQLINTEGER cbUsername = SQL_NTS;

      SQLCHAR datetime[DB_STR_LEN];
      datetime[0] = '\0';
      SQLINTEGER cbDatetime = SQL_NTS;

      SQLCHAR comment[DB_STR_LEN];
      comment[0] = '\0';
      SQLINTEGER cbComment = SQL_NTS;


      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLBindCol( stmt, 1, 
            SQL_C_CHAR, dbentity, sizeof(dbentity), &cbDbentity);

         rc = SQLBindCol( stmt, 2, 
            SQL_C_CHAR, designator1, sizeof(designator1), &cbDesignator1);

         rc = SQLBindCol( stmt, 3, 
            SQL_C_CHAR, designator2, sizeof(designator2), &cbDesignator2);

         rc = SQLBindCol( stmt, 4, 
            SQL_C_CHAR, designator3, sizeof(designator3), &cbDesignator3);

         rc = SQLBindCol( stmt, 5, 
            SQL_C_CHAR, designator4, sizeof(designator4), &cbDesignator4);

         rc = SQLBindCol( stmt, 6, 
            SQL_C_CHAR, designator5, sizeof(designator5), &cbDesignator5);

         rc = SQLBindCol( stmt, 7, 
            SQL_C_CHAR, rulestr, sizeof(rulestr), &cbRulestr);

         rc = SQLBindCol( stmt, 8, 
            SQL_C_CHAR, username, sizeof(username), &cbUsername);

         rc = SQLBindCol( stmt, 9, 
            SQL_C_CHAR, datetime, sizeof(datetime), &cbDatetime);

         rc = SQLBindCol( stmt,10, 
            SQL_C_CHAR, comment, sizeof(comment), &cbComment);

         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT Entity, Designator1, Designator2, Designator3, Designator4, Designator5, RuleString, UserName, DateTime, Comments FROM %s WHERE Entity = '%s'" , 
            tabledef.GetTableName(), entity);
         
         if (!argDesignator1.IsEmpty())
            query += " AND Designator1 = '" + argDesignator1 + "'";
         else
            query += " AND (Designator1 = '' OR Designator1 IS NULL)";

         
         if (!argDesignator2.IsEmpty())
            query += " AND Designator2 = '" + argDesignator2 + "'";
         else
            query += " AND (Designator2 = '' OR Designator2 IS NULL)";

         
         if (!argDesignator3.IsEmpty())
            query += " AND Designator3 = '" + argDesignator3 + "'";
         else
            query += " AND (Designator3 = '' OR Designator3 IS NULL)";

         
         if (!argDesignator4.IsEmpty())
            query += " AND Designator4 = '" + argDesignator4 + "'";
         else
            query += " AND (Designator4 = '' OR Designator4 IS NULL)";

         
         if (!argDesignator5.IsEmpty())
            query += " AND Designator5 = '" + argDesignator5 + "'";
         else
            query += " AND (Designator5 = '' OR Designator5 IS NULL)";

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // These queries may return multiple rows
            CDBStencilRule2 *prevRule = NULL;
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               stencilrule = new CDBStencilRule2(entity, designator1, designator2, designator3, designator4, designator5, rulestr, username, datetime, comment, prevRule);
               prevRule = stencilrule;
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return stencilrule;
}

bool CDBCustomMachineList::containsTemplate(CString templateName)
{
   for(int idx = 0; idx < GetCount(); idx++)
   {
      CDBCustomMachine *ctmachine = GetAt(idx);
      if(ctmachine && !templateName.CompareNoCase(ctmachine->getTemplateName())) 
         return true;
   }
   return false;
}

CDBCustomMachineList *CDBInterface::LookupCustomMachines(CString queryMachineName)
{
   // Lookup all elements associated with queryMachineName.
   // If queryMachineName is blank then lookup all MachineNames.

   CDBCustomMachineList *list = new CDBCustomMachineList();

   CDBCustomMachineTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR machineName[DB_STR_LEN];
      machineName[0] = '\0';
      SQLINTEGER cbmachineName = SQL_NTS;

      SQLCHAR templateName[DB_STR_LEN];
      templateName[0] = '\0';
      SQLINTEGER cbtemplateName = SQL_NTS;      

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;
         query.Format("SELECT MachineName, TemplateName FROM %s" , tabledef.GetTableName());
 
         if (!queryMachineName.IsEmpty())
         {
            CString whereClause;
            whereClause.Format(" WHERE MachineName ='%s'", queryMachineName);
            query += whereClause;
         }

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               SQLRETURN  rc1 = SQLGetData( stmt,  1, SQL_C_CHAR, machineName,  sizeof(machineName),  &cbmachineName);
               SQLRETURN  rc2 = SQLGetData( stmt,  2, SQL_C_CHAR, templateName,   sizeof(templateName),   &cbtemplateName);

               if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
               {
                  CDBCustomMachine *cm = new CDBCustomMachine(machineName,
                     ( rc2 == SQL_SUCCESS ||  rc2 == SQL_SUCCESS_WITH_INFO) ? templateName   : (CString)"");

                  list->Add(cm);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return list;
}

CDBCustomAssemblyTemplateList* CDBInterface::LookupCustomAssemblyTemplates(CString queryTemplateName)
{
   // Lookup all elements associated with queryTemplateName.
   // If queryTemplateName is blank then lookup all TemplateNames.

   // SETTINGLEN must be consistance with General Setting in AssemblyTemplate.h plus TemplateName
   CDBCustomAssemblyTemplateList *list = new CDBCustomAssemblyTemplateList();
   int SETTINGLEN = list->getFieldCount(); 

   CDBCustomAssemblyTemplateTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR templateSettings[DB_STR_LEN];
      SQLINTEGER cbtemplateSettings;      

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;         
         query.Format("SELECT %s FROM %s" , list->getFieldName(), tabledef.GetTableName());
 
         if (!queryTemplateName.IsEmpty())
         {
            CString whereClause;
            whereClause.Format(" WHERE TemplateName ='%s'", queryTemplateName);
            query += whereClause;
         }

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               CStringArray settingArray;
               for(int idx = 0; idx < SETTINGLEN; idx ++)
               {
                  templateSettings[0] = '\0';
                  cbtemplateSettings = SQL_NTS;  
                  SQLRETURN  rc1 = SQLGetData( stmt,  idx + 1, SQL_C_CHAR, templateSettings,  sizeof(templateSettings),  &cbtemplateSettings);
                   if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
                   {
                      settingArray.Add(templateSettings);
                   }
               }

               if (settingArray.GetCount() == SETTINGLEN)
               {
                  CDBCustomAssemblyTemplate *catemplate = new CDBCustomAssemblyTemplate(settingArray);
                  list->Add(catemplate);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return list;
}

CDBCustomReportParameterList* CDBInterface::LookupCDBCustomReportParameter(CString queryTemplateName)
{
   // Lookup all elements associated with queryTemplateName.
   // If queryTemplateName is blank then lookup all TemplateNames.

   // SETTINGLEN must be consistance with General Setting in AssemblyTemplate.h plus TemplateName
   CDBCustomReportParameterList *list = new CDBCustomReportParameterList();
   int PARAMTERLEN = list->getFieldCount(); 

   CDBCustomReportParameterTableDef tabledef;

   if (m_dbConnected && TableExists(tabledef.GetTableName()))
   {
      SQLCHAR parameters[DB_STR_LEN];
      SQLINTEGER cbparameters;      

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         // Could use SELECT *, but then columns are at the mercy of order in the DB.
         // So make column order explicit in query.
         CString query;         
         query.Format("SELECT %s FROM %s" , list->getFieldName(), tabledef.GetTableName());
 
         if (!queryTemplateName.IsEmpty())
         {
            CString whereClause;
            whereClause.Format(" WHERE TemplateName ='%s'", queryTemplateName);
            query += whereClause;
         }

         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            // There should be zero or more entries in the result, save them all
            while ( ( rc = SQLFetch( stmt ) ) != SQL_NO_DATA )
            {
               CStringArray parameterArray;
               for(int idx = 0; idx < PARAMTERLEN; idx ++)
               {
                  parameters[0] = '\0';
                  cbparameters = SQL_NTS;  
                  SQLRETURN  rc1 = SQLGetData( stmt,  idx + 1, SQL_C_CHAR, parameters,  sizeof(parameters),  &cbparameters);
                   if (rc1 == SQL_SUCCESS || rc1 == SQL_SUCCESS_WITH_INFO)
                   {
                      parameterArray.Add(parameters);
                   }
               }

               if (parameterArray.GetCount() == PARAMTERLEN)
               {
                  CDBCustomReportParameter *crptParameter = new CDBCustomReportParameter(parameterArray);
                  list->Add(crptParameter);
               }
            }
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return list;
}

bool CDBInterface::SavePart(CDBPart *part)
{
   bool retval = false;

   CDBParts3TableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      part != NULL && !part->getPartNumber().IsEmpty())
   {
      CString query;
      CDBPart *existingPart = NULL;

      part->SetUserName(GetCurrentUserName());
      part->SetDateTime(GetCurrentTimeStamp());

      // Do not use backward compatible lookup here. Here, we want to know if
      // the part is in the most current table type only. If it exists in previous
      // table but not latest, and if we do backward compatible lookup, then we
      // end up trying to do an UPDATE to a record that does not exist, which fails.

      if ((existingPart = this->LookupPart(part->getPartNumber(), false)) != NULL)
      {
         // Exists, do update
         // Could get fancier here and compare existing to incoming values
         // and only update the differences.

         // FYI, It was a BAD IDEA to use an SQL reserved word for a column name (VALUE).

         query  = "UPDATE " + tabledef.GetTableName();
         query += " SET DeviceType = '" + part->getDeviceType() + "'";
         query += ",\"Value\" = '" + part->getValue() + "'";
         query += ",pTol = '" + part->getPTol() + "'";
         query += ",nTol = '" + part->getNTol() + "'";
         query += ",Subclass = '" + part->getSubclass() + "'";
         query += ",PINMapping = '" + part->getPinmap() + "'";
         query += ",Description = '" + part->getDescription() + "'";
         query += ",UserName = '" + part->getUserName() + "'";
         query += ",\"DateTime\" = " + FormatSQLTimeStamp(part->getDateTime());
         query += ",Comment = '" + part->getComment() + "'";
         query += " WHERE PartNumber = '" + part->getPartNumber() + "';";

         delete existingPart;
      }
      else
      {
         // Doesn't exist, do insert
         query  = "INSERT INTO " + tabledef.GetTableName();
         query += " (PartNumber, DeviceType, \"Value\", pTol, nTol, Subclass, PINMapping, Description, UserName, \"DateTime\", Comment)";
         query += " VALUES (";
         query += "'" + part->getPartNumber() + "'";
         query += ",'" + part->getDeviceType() + "'";
         query += ",'" + part->getValue() + "'";
         query += ",'" + part->getPTol() + "'";
         query += ",'" + part->getNTol() + "'";
         query += ",'" + part->getSubclass() + "'";
         query += ",'" + part->getPinmap() + "'";
         query += ",'" + part->getDescription() + "'";
         query += ",'" + part->getUserName() + "'";
         query += ","  + FormatSQLTimeStamp(part->getDateTime());
         query += ",'" + part->getComment() + "'";
         query += ")";
      }

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            retval = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );

      // Generic attributes
      this->SaveGenericAttributes(part->getPartNumber(), dbnamePartNumber, part->GetAttribMap());
   }

   return retval;
}


bool CDBInterface::SaveCentroid(CDBCentroid *centroid)
{
   // Always save to new MM table.
   // Brave move:  Delete centroid from old table if it exists. (Should clean out old table as users do updates.)
   // But only apply this delete if the save is successful.

   bool retval = false;

   CDBCentroidsMM3TableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      centroid != NULL && !centroid->getGeomName().IsEmpty())
   {
      CString query;
      CDBCentroid *existingCent = NULL;
      centroid->SetUserName(GetCurrentUserName());
      centroid->SetDateTime(GetCurrentTimeStamp());

      if ((existingCent = this->LookupCentroidLowLevel1(centroid->getGeomName(), tabledef, pageUnitsMilliMeters)) != NULL)
      {
         // Exists, do update
         // Could get fancier here and compare existing to incoming values
         // and only update the differences.

         query  = "UPDATE " + tabledef.GetTableName() + " SET";
         query += " Algorithm = '" + centroid->getAlgorithm() + "'";
         query += ", X = '" + centroid->getXStr(pageUnitsMilliMeters) + "'";
         query += ", Y = '" + centroid->getYStr(pageUnitsMilliMeters) + "'";
         query += ", Rot = '" + centroid->getRotStr() + "'";
         query += ", UserName = '" + centroid->getUserName() + "'";
         query += ", \"DateTime\" = " + FormatSQLTimeStamp(centroid->getDateTime());
         query += ", Comment = '" + centroid->getComment() + "'";
         query += " WHERE GeomName = '" + centroid->getGeomName() + "';";

         delete existingCent;
      }
      else
      {
         // Doesn't exist, do insert
         query  = "INSERT INTO " + tabledef.GetTableName();
         query += " (GeomName, Algorithm, X, Y, Rot, UserName, \"DateTime\", Comment)";
         query += " VALUES (";
         query += "'" + centroid->getGeomName() + "'";
         query += ",'" + centroid->getAlgorithm() + "'";
         query += ",'" + centroid->getXStr(pageUnitsMilliMeters) + "'";
         query += ",'" + centroid->getYStr(pageUnitsMilliMeters) + "'";
         query += ",'" + centroid->getRotStr() + "'";
         query += ",'" + centroid->getUserName() + "'";
         query += ","  + FormatSQLTimeStamp(centroid->getDateTime());
         query += ",'" + centroid->getComment() + "'";
         query += ")";
      }

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            retval = true;

            // Delete possible entry in old Centroids table
            CDBCentroidsOldTableDef oldtabledef;
            DeleteTableEntries(oldtabledef.GetTableName(), "GeomName", centroid->getGeomName());
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return retval;
}


bool CDBInterface::SavePackage(CDBPackage *package)
{
   bool retval = false;

   CDBPackagesTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      package != NULL && !package->getGeomName().IsEmpty())
   {
      CString query;
      CDBPackage *existingPkg = NULL;

      if ((existingPkg = this->LookupPackage(package->getGeomName())) != NULL)
      {
         // Exists, do update
         // Could get fancier here and compare existing to incoming values
         // and only update the differences.

         query  = "UPDATE " + tabledef.GetTableName() + " SET";
         query += " PackageAlias = '" + package->getPackageAlias() + "'";
         query += ", OutlineMethod = '" + package->getOutlineMethod() + "'";
         query += " WHERE GeomName = '" + package->getGeomName() + "';";

         delete existingPkg;
      }
      else
      {
         // Doesn't exist, do insert
         query  = "INSERT INTO " + tabledef.GetTableName();
         query += " (GeomName, PackageAlias, OutlineMethod)";
         query += " VALUES (";
         query += "'" + package->getGeomName() + "'";
         query += ",'" + package->getPackageAlias() + "'";
         query += ",'" + package->getOutlineMethod() + "'";
         query += ")";
      }

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            retval = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );


      // Generic attributes
      this->SaveGenericAttributes(package->getGeomName(), dbnameGeometry, package->GetAttribMap());
   }

   return retval;
}


bool CDBInterface::SavePackageInfo(CDBPackageInfo *package)
{
   bool retval = false;

   CDBPackageInfo3TableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      package != NULL && !package->getName().IsEmpty())
   {
      CString packageHeightStr( package->getPackageHeightStr(pageUnitsMilliMeters) );
      if (!packageHeightStr.IsEmpty())
         packageHeightStr += " mm";

      CString query;
      CDBPackageInfo *existingPkg = NULL;
      package->SetUserName(GetCurrentUserName());
      package->SetDateTime(GetCurrentTimeStamp());

      if ((existingPkg = this->LookupPackageInfo23(tabledef, package->getName(), package->getNameType())) != NULL)
      {
         // Exists, do update
         // Could get fancier here and compare existing to incoming values
         // and only update the differences.

         query  = "UPDATE " + tabledef.GetTableName() + " SET";
         query += " PackageSource = '" + package->getPackageSourceStr() + "'";
         query += ", PackageAlias = '" + package->getPackageAlias() + "'";
         query += ", PackageHeight = '" + packageHeightStr + "'";
         query += ", OutlineMethod = '" + package->getOutlineMethod() + "'";
         query += ", DPMO = '" + package->getDPMO() + "'";
         query += ", Family = '" + package->getFamily() + "'";
         query += ", UserName = '" + package->getUserName() + "'";
         query += ", \"DateTime\" = " + FormatSQLTimeStamp(package->getDateTime());
         query += ", Comment = '" + package->getComment() + "'";
         query += " WHERE Name = '" + package->getName() + "' AND NameType = '" + package->getNameTypeStr() + "';";

         delete existingPkg;
      }
      else
      {
         // Doesn't exist, do insert
         query  = "INSERT INTO  " + tabledef.GetTableName();
         query += " (Name, NameType, PackageSource, PackageAlias, PackageHeight, OutlineMethod, DPMO, Family, UserName, \"DateTime\", Comment)";
         query += " VALUES (";
         query += "'" + package->getName() + "'";
         query += ",'" + package->getNameTypeStr() + "'";
         query += ",'" + package->getPackageSourceStr() + "'";
         query += ",'" + package->getPackageAlias() + "'";
         query += ",'" + packageHeightStr + "'";
         query += ",'" + package->getOutlineMethod() + "'";
         query += ",'" + package->getDPMO() + "'";
         query += ",'" + package->getFamily() + "'";
         query += ",'" + package->getUserName() + "'";
         query += ","  + FormatSQLTimeStamp(package->getDateTime());
         query += ",'" + package->getComment() + "'";
         query += ")";
      }

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            retval = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );


      // Generic attributes
      this->SaveGenericAttributes(package->getName(), package->getNameType(), package->GetAttribMap());
   }

   return retval;
}


bool CDBInterface::SaveGenericAttributes(CString name, EDBNameType nametype, CDBGenericAttributeMap *attribMap)
{
   int numberSaved = 0;

   CString nametypeStr( CDBPackageInfo::GetNameTypeStr(nametype) );

   CDBGenericAttributeTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      attribMap != NULL && attribMap->GetCount() > 0)
   {
      POSITION mapPos = attribMap->GetStartPosition();
      while (mapPos != NULL)
      {
         CString ignoredAttribName;
         CDBNameValuePair *nvp = NULL;
         attribMap->GetNextAssoc(mapPos, ignoredAttribName, nvp);

         CString query;
         CString ignoredAttribValue;
         if (this->LookupGenericAttribute(name, nametype, nvp->GetName(), ignoredAttribValue))
         {
            // Exists, do update
            // Could get fancier here and compare existing to incoming values
            // and only update the differences.

            query  = "UPDATE " + tabledef.GetTableName() + " SET";
            query += " AttribValue = '" + nvp->GetValue() + "'";
            query += ", UserID = '" + GetCurrentUserName() + "'";
            query += " WHERE Name = '" + name + "' AND NameType = '" + nametypeStr + "' AND AttribName = '" + nvp->GetName() + "';";
         }
         else
         {
            // Doesn't exist, do insert
            query  = "INSERT INTO " + tabledef.GetTableName();
            query += " (Name, NameType, AttribName, AttribValue, UserID)";
            query += " VALUES (";
            query += "'" + name + "'";
            query += ",'" + nametypeStr + "'";
            query += ",'" + nvp->GetName() + "'";
            query += ",'" + nvp->GetValue() + "'";
            query += ",'" + GetCurrentUserName() + "'";
            query += ")";
         }

         SQLHSTMT stmt;
         SQLRETURN rc;

         rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
            {
               // Ok
               numberSaved++;
            }
            else
            {
               ReportError(SQL_HANDLE_STMT, stmt, query);
            }
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   bool retval = (attribMap != NULL && attribMap->GetCount() == numberSaved);

   return retval;
}



bool CDBInterface::SaveOutlineVertices(CString name, EDBNameType nametype, CDBOutlineVertexMap *map)
{
   int numberSaved = 0;

   CString nametypeStr( CDBPackageInfo::GetNameTypeStr(nametype) );

   CDBOutlineVertexTableDef tabledef;

   bool doneOnce = false;//*rcf TEMP way to fix this, rearrange code for better life

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      map != NULL && map->GetCount() > 0)
   {
      POSITION mapPos = map->GetStartPosition();
      while (mapPos != NULL)
      {
         CString ignoredAttribName;
         CDBOutlineVertex *v = NULL;
         map->GetNextAssoc(mapPos, ignoredAttribName, v);

         // Unlike Others that have an Update section, this is a set of stuff so upon
         // save operation, delete the existing one and add the new one.

         
         CString query;
#ifdef UPDATE_OUTLINE
         CString ignoredAttribValue;
         if (this->LookupGenericAttribute(name, nametype, nvp->GetName(), ignoredAttribValue))
         {
            // Exists, do update
            // Could get fancier here and compare existing to incoming values
            // and only update the differences.

            query  = "UPDATE " + tabledef.GetTableName() + " SET";
            query += " AttribValue = '" + nvp->GetValue() + "'";
            // ... set UserID here
            query += " WHERE Name = '" + name + "' AND NameType = '" + nametypeStr + "' AND AttribName = '" + nvp->GetName() + "';";
         }
         else
#else
         if (!doneOnce)
         {
            doneOnce = true;

            query  = "DELETE FROM " + tabledef.GetTableName();
            query += " WHERE Name = '" + name + "' AND NameType = '" + nametypeStr + "';";

            SQLHSTMT deletestmt;
            SQLRETURN deleterc;

            deleterc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &deletestmt );

            if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO)
            {
               deleterc = SQLExecDirect( deletestmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

               if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO || deleterc == SQL_NO_DATA)
               {
                  // Ok
                  ///numberSaved++;
               }
               else
               {
                  ReportError(SQL_HANDLE_STMT, deletestmt, query);
               }
            }

            SQLFreeHandle( SQL_HANDLE_STMT, deletestmt );
         }
#endif
         {
            // Doesn't exist, do insert
            query  = "INSERT INTO " + tabledef.GetTableName();
            query += " (Name, NameType, PolyNum, PointNum, X, Y, Bulge)";
            query += " VALUES (";
            query += "'" + name + "'";
            query += ",'" + nametypeStr + "'";
            query += ",'" + v->getPolyNumStr() + "'";
            query += ",'" + v->getPointNumStr() + "'";
            query += ",'" + v->getXStr() + "'";
            query += ",'" + v->getYStr() + "'";
            query += ",'" + v->getBulgeStr() + "'";
            query += ")";
         }

         SQLHSTMT stmt;
         SQLRETURN rc;

         rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
            {
               // Ok
               numberSaved++;
            }
            else
            {
               ReportError(SQL_HANDLE_STMT, stmt, query);
            }
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   bool retval = (map != NULL && map->GetCount() == numberSaved);

   return retval;
}

bool CDBInterface::SaveSubclassElement(CDBSubclassElement *el)
{
   bool saved = false;

   CDBSubclassElementTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      el != NULL)
   {
      CString query;

      query.Format("INSERT INTO %s", tabledef.GetTableName());
      query += " (SubclassName, ElementName, ElementDevType, Pin1Refname, Pin2Refname, Pin3Refname, Pin4Refname, ElementValue, PlusTol, MinusTol)";
      query += " VALUES (";
      query += "'"  + el->getSubclassName() + "'";
      query += ",'" + el->getElementName()  + "'";
      query += ",'" + el->getDeviceType()   + "'";
      query += ",'" + el->getPin1Refname()  + "'";
      query += ",'" + el->getPin2Refname()  + "'";
      query += ",'" + el->getPin3Refname()  + "'";
      query += ",'" + el->getPin4Refname()  + "'";
      query += ",'" + el->getValue()        + "'";
      query += ",'" + el->getPlusTol()      + "'";
      query += ",'" + el->getMinusTol()     + "'";
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}

bool CDBInterface::SaveSubclass(CString subclassName, CDBSubclassElementList *list)
{
   int numberSaved = 0;

   CDBSubclassElementTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      !subclassName.IsEmpty() && list != NULL)
   {
      // Get rid of existing entries
      DeleteSubclass(subclassName);

      // Add new elements
      for (int i = 0; i < list->GetCount(); i++)
      {
         CDBSubclassElement *el = (CDBSubclassElement*)list->GetAt(i);

         if (SaveSubclassElement(el))
            numberSaved++;
      }
   }


   bool retval = (list != NULL && list->GetCount() == numberSaved);

   return retval;
}

bool CDBInterface::DeleteSubclass(CString subclassName)
{
   CDBSubclassElementTableDef tabledef;

   if (this->IsConnected() && this->TableExists(tabledef) && 
      !subclassName.IsEmpty())
   {
      CString query;
      query.Format("DELETE FROM %s WHERE SubclassName = '%s'", tabledef.GetTableName(), subclassName);

      SQLHSTMT deletestmt;
      SQLRETURN deleterc;

      deleterc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &deletestmt );

      if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO)
      {
         deleterc = SQLExecDirect( deletestmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO || deleterc == SQL_NO_DATA)
         {
            // Ok
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, deletestmt, query);
         }
      }
      
      SQLFreeHandle( SQL_HANDLE_STMT, deletestmt );

      return true;
   }

   return false;
}

void CDBInterface::DeletePanelTemplateID(CDBTableDef &tabledef, CDBPanelTemplate *panelTemplate)
{
   if (this->IsConnected() && this->TableExists(tabledef.GetTableName()) && 
      panelTemplate != NULL && !panelTemplate->GetName().IsEmpty())
   {
      // Get rid of existing entries
      CString query;

      query.Format("DELETE FROM %s WHERE TemplateName = '%s'", tabledef.GetTableName(), panelTemplate->GetName());

      SQLHSTMT deletestmt;
      SQLRETURN deleterc;

      deleterc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &deletestmt );

      if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO)
      {
         deleterc = SQLExecDirect( deletestmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO || deleterc == SQL_NO_DATA)
         {
            // Ok
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, deletestmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, deletestmt );
   }
}

bool CDBInterface::SavePanelTemplate(CDBPanelTemplate *panelTemplate)
{
   bool saved = false;

   CDBPanelTemplateIDTableDef2 tabledef;   // The primary new table
   CDBPanelTemplateIDTableDef1 tabledef1;  // The old table



   if (this->IsConnected() && this->CreateTable(tabledef) && 
      panelTemplate != NULL && !panelTemplate->GetName().IsEmpty())
   {
      // Delete possible existing entries from both new and old tables.
      DeletePanelTemplateID(tabledef,  panelTemplate);
      DeletePanelTemplateID(tabledef1, panelTemplate);

      {
         // Update userID and timestamp
         panelTemplate->SetUserID( this->GetCurrentUserName() );
         panelTemplate->SetTimeStamp( this->GetCurrentTimeStamp() );

         // Save data
         CString query;

         query.Format("INSERT INTO %s", tabledef.GetTableName());
         query += " (TemplateName, Units, UserID, \"DateTime\")";
         query += " VALUES (";
         query += "'"  + panelTemplate->GetName() + "'";
         query += ",'" + panelTemplate->GetPageUnitsStr() + "'";
         query += ",'" + panelTemplate->GetUserID()  + "'";
         query += ","  + FormatSQLTimeStamp(panelTemplate->GetTimeStamp());
         query += ")";

         SQLHSTMT stmt;
         SQLRETURN rc;

         rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
            {
               // Ok
               saved = true;

               SavePanelTemplateOutlines(panelTemplate);
               SavePanelTemplatePcbInstances(panelTemplate);
               SavePanelTemplatePcbArrays(panelTemplate);
               SavePanelTemplateFiducials(panelTemplate);
               SavePanelTemplateToolingHoles(panelTemplate);
            }
            else
            {
               ReportError(SQL_HANDLE_STMT, stmt, query);
            }
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );

      }
   }

   return saved;
}

bool CDBInterface::SavePanelTemplateOutlines(CDBPanelTemplate *panelTemplate)
{
   bool numberSaved = false;

   CDBPanelTemplateOutlineTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      panelTemplate != NULL && !panelTemplate->GetName().IsEmpty())
   {
      // Get rid of existing entries
      DeleteTableEntries(tabledef.GetTableName(), "TemplateName", panelTemplate->GetName());

      // Add new elements
      for (int i = 0; i < panelTemplate->GetOutlineList().GetCount(); i++)
      {
         CDBPanelTemplateOutline *el = panelTemplate->GetOutlineList().GetAt(i);

         if (SavePanelTemplateOutline(panelTemplate->GetName(), el))
            numberSaved++;
      }
   }

   return numberSaved;
}

bool CDBInterface::SavePanelTemplateOutline(CString templateName, CDBPanelTemplateOutline *outline)
{
   bool saved = false;

   CDBPanelTemplateOutlineTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      outline != NULL && !templateName.IsEmpty())
   {
      // Save data
      CString query;

      query.Format("INSERT INTO %s", tabledef.GetTableName());
      query += " (TemplateName, OriginX, OriginY, SizeX, SizeY)";
      query += " VALUES (";
      query += "'"  + templateName + "'";
      query += ",'" + outline->GetOriginXStr()   + "'";
      query += ",'" + outline->GetOriginYStr()   + "'";
      query += ",'" + outline->GetSizeXStr()     + "'";
      query += ",'" + outline->GetSizeYStr()     + "'";
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}


bool CDBInterface::SavePanelTemplatePcbInstances(CDBPanelTemplate *panelTemplate)
{
   bool numberSaved = false;

   CDBPanelTemplatePcbInstanceTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      panelTemplate != NULL && !panelTemplate->GetName().IsEmpty())
   {
      // Get rid of existing entries
      DeleteTableEntries(tabledef.GetTableName(), "TemplateName", panelTemplate->GetName());

      // Add new elements
      for (int i = 0; i < panelTemplate->GetPcbInstanceList().GetCount(); i++)
      {
         CDBPanelTemplatePcbInstance *el = panelTemplate->GetPcbInstanceList().GetAt(i);

         if (SavePanelTemplatePcbInstance(panelTemplate->GetName(), el))
            numberSaved++;
      }
   }

   return numberSaved;
}

bool CDBInterface::SavePanelTemplatePcbInstance(CString templateName, CDBPanelTemplatePcbInstance *pcbInst)
{
   bool saved = false;

   CDBPanelTemplatePcbInstanceTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      pcbInst != NULL && !templateName.IsEmpty())
   {
      // Save data
      CString query;

      query.Format("INSERT INTO %s", tabledef.GetTableName());
      query += " (TemplateName, PcbName, OriginX, OriginY, Angle, Mirrored, OriginType)";
      query += " VALUES (";
      query += "'"  + templateName + "'";
      query += ",'" + pcbInst->GetName()  + "'";
      query += ",'" + pcbInst->GetOriginXStr()   + "'";
      query += ",'" + pcbInst->GetOriginYStr()   + "'";
      query += ",'" + pcbInst->GetAngleStr()   + "'";
      query += ",'" + pcbInst->GetMirroredStr()   + "'";
      query += ",'" + pcbInst->GetOriginType()   + "'";
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}


bool CDBInterface::SavePanelTemplatePcbArrays(CDBPanelTemplate *panelTemplate)
{
   bool numberSaved = false;

   CDBPanelTemplatePcbArrayTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      panelTemplate != NULL && !panelTemplate->GetName().IsEmpty())
   {
      // Get rid of existing entries
      DeleteTableEntries(tabledef.GetTableName(), "TemplateName", panelTemplate->GetName());

      // Add new elements
      for (int i = 0; i < panelTemplate->GetPcbArrayList().GetCount(); i++)
      {
         CDBPanelTemplatePcbArray *el = panelTemplate->GetPcbArrayList().GetAt(i);

         if (SavePanelTemplatePcbArray(panelTemplate->GetName(), el))
            numberSaved++;
      }
   }

   return numberSaved;
}

bool CDBInterface::SavePanelTemplatePcbArray(CString templateName, CDBPanelTemplatePcbArray *pcbAr)
{
   bool saved = false;

   CDBPanelTemplatePcbArrayTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      pcbAr != NULL && !templateName.IsEmpty())
   {
      // Save data
      CString query;

      query.Format("INSERT INTO %s", tabledef.GetTableName());
      query += " (TemplateName, PcbName, OriginX, OriginY, Angle, Mirrored, OriginType, CountX, CountY, StepX, StepY)";
      query += " VALUES (";
      query += "'"  + templateName + "'";
      query += ",'" + pcbAr->GetName()  + "'";
      query += ",'" + pcbAr->GetOriginXStr()   + "'";
      query += ",'" + pcbAr->GetOriginYStr()   + "'";
      query += ",'" + pcbAr->GetAngleStr()   + "'";
      query += ",'" + pcbAr->GetMirroredStr()   + "'";
      query += ",'" + pcbAr->GetOriginType()   + "'";
      query += ",'" + pcbAr->GetCountXStr()   + "'";
      query += ",'" + pcbAr->GetCountYStr()   + "'";
      query += ",'" + pcbAr->GetStepXStr()   + "'";
      query += ",'" + pcbAr->GetStepYStr()   + "'";
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}

bool CDBInterface::SavePanelTemplateToolingHoles(CDBPanelTemplate *panelTemplate)
{
   bool numberSaved = false;

   CDBPanelTemplateToolingHoleTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      panelTemplate != NULL && !panelTemplate->GetName().IsEmpty())
   {
      // Get rid of existing entries
      DeleteTableEntries(tabledef.GetTableName(), "TemplateName", panelTemplate->GetName());

      // Add new elements
      for (int i = 0; i < panelTemplate->GetToolingHoleList().GetCount(); i++)
      {
         CDBPanelTemplateToolingHole *el = panelTemplate->GetToolingHoleList().GetAt(i);

         if (SavePanelTemplateToolingHole(panelTemplate->GetName(), el))
            numberSaved++;
      }
   }

   return numberSaved;
}

bool CDBInterface::SavePanelTemplateToolingHole(CString templateName, CDBPanelTemplateToolingHole *ptToolingHole)
{
   bool saved = false;

   CDBPanelTemplateToolingHoleTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      ptToolingHole != NULL && !templateName.IsEmpty())
   {
      // Save data
      CString query;

      query.Format("INSERT INTO %s", tabledef.GetTableName());
      query += " (TemplateName, Size, OriginX, OriginY)";
      query += " VALUES (";
      query += "'"  + templateName + "'";
      query += ",'" + ptToolingHole->GetSizeStr()  + "'";
      query += ",'" + ptToolingHole->GetXStr()   + "'";
      query += ",'" + ptToolingHole->GetYStr()   + "'";
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}

bool CDBInterface::SavePanelTemplateFiducials(CDBPanelTemplate *panelTemplate)
{
   bool numberSaved = false;

   CDBPanelTemplateFiducialTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      panelTemplate != NULL && !panelTemplate->GetName().IsEmpty())
   {
      // Out with the old
      DeleteTableEntries(tabledef.GetTableName(), "TemplateName", panelTemplate->GetName());

      // In with the new
      for (int i = 0; i < panelTemplate->GetFiducialList().GetCount(); i++)
      {
         CDBPanelTemplateFiducial *el = panelTemplate->GetFiducialList().GetAt(i);

         if (SavePanelTemplateFiducial(panelTemplate->GetName(), el))
            numberSaved++;
      }
   }

   return numberSaved;
}

bool CDBInterface::SavePanelTemplateFiducial(CString templateName, CDBPanelTemplateFiducial *fid)
{
   bool saved = false;

   CDBPanelTemplateFiducialTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      fid != NULL && !templateName.IsEmpty())
   {
      // Save data
      CString query;

      query.Format("INSERT INTO %s", tabledef.GetTableName());
      query += " (TemplateName, Type, Size, OriginX, OriginY, Angle, Mirrored)";
      query += " VALUES (";
      query += "'"  + templateName + "'";
      query += ",'" + fid->GetType()        + "'";
      query += ",'" + fid->GetSizeStr()     + "'";
      query += ",'" + fid->GetOriginXStr()  + "'";
      query += ",'" + fid->GetOriginYStr()  + "'";
      query += ",'" + fid->GetAngleStr()    + "'";
      query += ",'" + fid->GetMirroredStr() + "'";
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}

bool CDBInterface::SaveStencilSetting(CDBStencilSetting *setting)
{
   bool retval = false;

   CDBStencilSettingsTableDef tabledef;
 
   if (this->IsConnected() && this->CreateTable(tabledef) && 
      setting != NULL && !setting->getName().IsEmpty())
   {
      CString query;
      CDBStencilSetting *existingSetting = NULL;

      if ((existingSetting = this->LookupStencilSetting(setting->getName())) != NULL)
      {
         // Exists, do update
         // Could get fancier here and compare existing to incoming values
         // and only update the differences.

         query  = "UPDATE " + tabledef.GetTableName();
         query += " SET SettingValue = '" + setting->getValue() + "'";
         query += " WHERE SettingName = '" + setting->getName() + "';";

         delete existingSetting;
      }
      else
      {
         // Doesn't exist, do insert
         query  = "INSERT INTO " + tabledef.GetTableName();
         query += " (SettingName, SettingValue)";
         query += " VALUES (";
         query += "'" + setting->getName() + "'";
         query += ",'" + setting->getValue() + "'";
         query += ")";
      }

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            retval = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return retval;
}

//----------------------------------------------------------------------------

bool CDBStencilRule2::operator== (CDBStencilRule2 &other)
{
   // User Name and Date/Time are excempt from comparison.
   // Only want to know if the rest is equal.


   if (this->getEntity().Compare( other.getEntity() ) != 0)
      return false;

   if (this->getDesignator1().Compare( other.getDesignator1() ) != 0)
      return false;

   if (this->getDesignator2().Compare( other.getDesignator2() ) != 0)
      return false;

   if (this->getDesignator3().Compare( other.getDesignator3() ) != 0)
      return false;

   if (this->getDesignator4().Compare( other.getDesignator4() ) != 0)
      return false;

   if (this->getDesignator5().Compare( other.getDesignator5() ) != 0)
      return false;

   if (this->getRule().Compare( other.getRule() ) != 0)
      return false;

   ////if (this->getUserName().Compare( other.getUserName() ) != 0)
   ////   return false;
   ////
   ////if (this->getDateTime().Compare( other.getDateTime() ) != 0)
   ////   return false;

   if (this->getComment().Compare( other.getComment() ) != 0)
      return false;

   // No differences found in tests above, must be equal
   return true;
}

//----------------------------------------------------------------------------

bool CDBInterface::SaveStencilRule(CDBStencilRule2 *rule)
{
   bool retval = false;

   CDBStencilRules4TableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      rule != NULL && !rule->getEntity().IsEmpty())
   {
      CString query;
      CDBStencilRule2 *existingRule = NULL;

      // For this lookup we want exact entity/prefix/suffix match
      if ((existingRule = this->LookupStencilRule4(rule->getEntity(), rule->getDesignator1(), rule->getDesignator2(), rule->getDesignator3(), rule->getDesignator4(), rule->getDesignator5())) != NULL)
      {
         // Exists, do update ONLY if different
         if (*existingRule == *rule)
         {
            // Already in DB, we'll count that as a success
            retval = true;
         }
         else
         {
            // Could get fancier here and compare existing to incoming values
            // and only update the differences.

            // query from old VB ccpartslib.exe
            // cmd = "SELECT *  FROM STENCILRULES WHERE Entity = '" & Entity & "' AND DesignatorPrefix = '" & DesignatorPrefix & "' AND DesignatorSuffix = '" & DesignatorSuffix & "'"
            // Also noted is ccpartslib.exe saved comment on insert, but not on update.

            query  = "UPDATE " + tabledef.GetTableName();
            query += " SET RuleString = '" + rule->getRule() + "'";
            query += ", Comments = '" + rule->getComment() + "'";
            query += ", UserName = '" + rule->getUserName() + "'";
            query += ", \"DateTime\" = " + FormatSQLTimeStamp(rule->getDateTime());
            query += " WHERE Entity = '" + rule->getEntity() + "'";

            if (!rule->getDesignator1().IsEmpty())
               query += " AND Designator1 = '" + rule->getDesignator1() + "'";
            else
               query += " AND (Designator1 = '' OR Designator1 IS NULL)";

            if (!rule->getDesignator2().IsEmpty())
               query += " AND Designator2 = '" + rule->getDesignator2() + "'";
            else
               query += " AND (Designator2 = '' OR Designator2 IS NULL)";

            if (!rule->getDesignator3().IsEmpty())
               query += " AND Designator3 = '" + rule->getDesignator3() + "'";
            else
               query += " AND (Designator3 = '' OR Designator3 IS NULL)";

            if (!rule->getDesignator4().IsEmpty())
               query += " AND Designator4 = '" + rule->getDesignator4() + "'";
            else
               query += " AND (Designator4 = '' OR Designator4 IS NULL)";

            if (!rule->getDesignator5().IsEmpty())
               query += " AND Designator5 = '" + rule->getDesignator5() + "'";
            else
               query += " AND (Designator5 = '' OR Designator5 IS NULL)";
         }

         while (existingRule != NULL)
         {
            CDBStencilRule2 *nextRule = rule->getNext();
            delete existingRule;
            existingRule = nextRule;
         }
      }
      else
      {
         // Doesn't exist, do insert
         query  = "INSERT INTO " + tabledef.GetTableName();
         query += " (Entity, Designator1, Designator2, Designator3, Designator4, Designator5, RuleString, UserName, \"DateTime\", Comments)";
         query += " VALUES (";
         query += "'" + rule->getEntity() + "'";
         query += ",'" + rule->getDesignator1() + "'";
         query += ",'" + rule->getDesignator2() + "'";
         query += ",'" + rule->getDesignator3() + "'";
         query += ",'" + rule->getDesignator4() + "'";
         query += ",'" + rule->getDesignator5() + "'";
         query += ",'" + rule->getRule() + "'";
         query += ",'" + rule->getUserName() + "'";
         query += ","  + FormatSQLTimeStamp(rule->getDateTime());
         query += ",'" + rule->getComment() + "'";
         query += ")";
      }

      // If query is blank then no update/save is needed
      if (!query.IsEmpty())
      {
         SQLHSTMT stmt;
         SQLRETURN rc;

         rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
            {
               // Ok
               retval = true;
            }
            else
            {
               ReportError(SQL_HANDLE_STMT, stmt, query);
            }
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return retval;
}

//----------------------------------------------------------------------------
#ifdef SUPPORT_RULE_KILLER
// This has been disabled, no longer killing old rules nor sharing tables.
// Func kept as reference for awhile.

bool CDBInterface::KillStencilRule(CString entity, CString designator1, CString designator2, CString designator3)
{
   bool retval = false;

   CDBStencilRules2TableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      !entity.IsEmpty() &&             // must have entity type
      !designator1.IsEmpty() &&        // must have side
      !designator2.IsEmpty())          // must have geometry name
   {
      CString UserName = this->GetCurrentUserName();
      CString DateTime = this->GetCurrentTimeStamp();

      CString query;

      query  = "UPDATE " + tabledef.GetTableName();
      query += " SET Entity = 'DEAD_" + entity + "'";
      query += ", UserName = '" + UserName + "'";
      query += ", \"DateTime\" = " + this->FormatSQLTimeStamp(DateTime);
      query += " WHERE Entity = '" + entity + "'";
      query += " AND Designator1 = '" + designator1 + "'";
      query += " AND Designator2 = '" + designator2 + "'";

      if (!designator3.IsEmpty())
         query += " AND Designator3 = '" + designator3 + "'";

      if (!query.IsEmpty())
      {
         SQLHSTMT stmt;
         SQLRETURN rc;

         rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
            {
               // Ok
               retval = true;
            }
            else
            {
               // Okay if delete fails, maybe nothing to delete ... ReportError(SQL_HANDLE_STMT, stmt, query);
            }
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return retval;
}
#endif

//----------------------------------------------------------------------------

bool CDBInterface::DeleteStencilRule(CString entity, CString designator1, CString designator2, CString designator3)
{
   bool retval = false;

   // Only support deleting from latest active table.
   // Earlier tables become read-only in later versions of CAMCAD.

   CDBStencilRules3TableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      !entity.IsEmpty() &&             // must have entity type
      !designator1.IsEmpty() &&        // must have side
      !designator2.IsEmpty())          // must have geometry name
   {
      CString query;

      query  = "DELETE From " + tabledef.GetTableName();
      query += " WHERE Entity = '" + entity + "'";
      query += " AND Designator1 = '" + designator1 + "'";
      query += " AND Designator2 = '" + designator2 + "'";

      if (!designator3.IsEmpty())
         query += " AND Designator3 = '" + designator3 + "'";

      if (!query.IsEmpty())
      {
         SQLHSTMT stmt;
         SQLRETURN rc;

         rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
         {
            rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

            if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
            {
               // Ok
               retval = true;
            }
            else
            {
               // Okay if delete fails, maybe nothing to delete ... ReportError(SQL_HANDLE_STMT, stmt, query);
            }
         }

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }

   return retval;
}

bool CDBInterface::SaveCustomMachine(CDBCustomMachine *cm)
{
   bool saved = false;

   CDBCustomMachineTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      cm != NULL)
   {
      CString query;

      query.Format("INSERT INTO %s", tabledef.GetTableName());
      query += " (MachineName, TemplateName)";
      query += " VALUES (";
      query += "'"  + cm->getMachineName() + "'";
      query += ",'" + cm->getTemplateName()  + "'";
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}

bool CDBInterface::DeleteCustomMachine(CString machineName)
{
   CDBCustomMachineTableDef tabledef;

   if (this->IsConnected() && this->TableExists(tabledef))
   {
      CString query;
      query.Format("DELETE FROM %s", tabledef.GetTableName());

      if (!machineName.IsEmpty())
      {
         CString whereClause;
         whereClause.Format(" WHERE MachineName ='%s'", machineName);
         query += whereClause;
      }
      SQLHSTMT deletestmt;
      SQLRETURN deleterc;

      deleterc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &deletestmt );

      if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO)
      {
         deleterc = SQLExecDirect( deletestmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO || deleterc == SQL_NO_DATA)
         {
            // Ok
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, deletestmt, query);
         }
      }
      
      SQLFreeHandle( SQL_HANDLE_STMT, deletestmt );

      return true;
   }

   return false;
}

bool CDBInterface::SaveCustomAssemblyTemplate(CDBCustomAssemblyTemplate *catemplate)
{
   bool saved = false;

   CDBCustomAssemblyTemplateTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      catemplate != NULL)
   {
      CString query;
      CDBCustomAssemblyTemplateList cdbtemplateList;

      query.Format("INSERT INTO %s (%s)", tabledef.GetTableName(), cdbtemplateList.getFieldName());
      query += " VALUES (";
      for(int idx = 0; idx < catemplate->getTemplateSettings().GetCount(); idx++)
      {
         query += (idx == 0)?"'":",'" ;
         query += catemplate->getTemplateSettings().GetAt(idx);
         query += "'";
      }
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}

bool CDBInterface::DeleteCustomAssemblyTemplate(CString templateName)
{
   CDBCustomAssemblyTemplateTableDef tabledef;

   if (this->IsConnected() && this->TableExists(tabledef))
   {
      CString query;
      query.Format("DELETE FROM %s", tabledef.GetTableName());

      if (!templateName.IsEmpty())
      {
         CString whereClause;
         whereClause.Format(" WHERE TemplateName ='%s'", templateName);
         query += whereClause;
      }
      SQLHSTMT deletestmt;
      SQLRETURN deleterc;

      deleterc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &deletestmt );

      if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO)
      {
         deleterc = SQLExecDirect( deletestmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO || deleterc == SQL_NO_DATA)
         {
            // Ok
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, deletestmt, query);
         }
      }
      
      SQLFreeHandle( SQL_HANDLE_STMT, deletestmt );

      return true;
   }

   return false;
}

bool CDBInterface::SaveCustomReportParameter(CDBCustomReportParameter *crptParameter)
{
   bool saved = false;

   CDBCustomReportParameterTableDef tabledef;

   if (this->IsConnected() && this->CreateTable(tabledef) && 
      crptParameter != NULL)
   {
      CString query;
      CDBCustomReportParameterList cdbrptParameter;

      query.Format("INSERT INTO %s (%s)", tabledef.GetTableName(), cdbrptParameter.getFieldName());
      query += " VALUES (";
      query += "'"  + crptParameter->getTemplateName() + "'";
      query += ",'" + crptParameter->getPriority() + "'";
      query += ",'" + crptParameter->getCommand() + "'";
      query += ",'" + crptParameter->getParameter() + "'";
      query += ",'" + crptParameter->getFieldName() + "'";
      query += ")";

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         rc = SQLExecDirect( stmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO || rc == SQL_NO_DATA)
         {
            // Ok
            saved = true;
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, stmt, query);
         }
      }

      SQLFreeHandle( SQL_HANDLE_STMT, stmt );
   }

   return saved;
}

bool CDBInterface::DeleteCustomReportParameter(CString templateName)
{
   CDBCustomReportParameterTableDef tabledef;

   if (this->IsConnected() && this->TableExists(tabledef))
   {
      CString query;
      query.Format("DELETE FROM %s", tabledef.GetTableName());

      if (!templateName.IsEmpty())
      {
         CString whereClause;
         whereClause.Format(" WHERE TemplateName ='%s'", templateName);
         query += whereClause;
      }
      SQLHSTMT deletestmt;
      SQLRETURN deleterc;

      deleterc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &deletestmt );

      if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO)
      {
         deleterc = SQLExecDirect( deletestmt, (SQLCHAR * )query.GetBuffer(0), SQL_NTS );

         if (deleterc == SQL_SUCCESS || deleterc == SQL_SUCCESS_WITH_INFO || deleterc == SQL_NO_DATA)
         {
            // Ok
         }
         else
         {
            ReportError(SQL_HANDLE_STMT, deletestmt, query);
         }
      }
      
      SQLFreeHandle( SQL_HANDLE_STMT, deletestmt );

      return true;
   }

   return false;
}

//--------------------------------------------------------------------------

SQLRETURN CDBInterface::executeQuery(SQLHSTMT& stmt,const CString& query)
{
   SQLRETURN rc = SQLExecDirect( stmt, (SQLCHAR * )((const char*)query), SQL_NTS );

   if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
   {
   }
   else
   {
      ReportError(SQL_HANDLE_STMT, stmt, query);
   }

   return rc;
}

void CDBInterface::migrateStencilData2()
{
   CDBStencilRules1TableDef stencilRules1TableDef;
   CDBStencilRules2TableDef stencilRules2TableDef;

   if (IsConnected() && TableExists(stencilRules1TableDef.GetTableName()) && !TableExists(stencilRules2TableDef.GetTableName()))
   {
      CreateTable(stencilRules2TableDef);

      SQLHSTMT stmt;
      SQLRETURN rc;

      rc = SQLAllocHandle( SQL_HANDLE_STMT, m_dbc, &stmt );

      if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
      {
         CString query;

         executeQuery(stmt, 
            "INSERT INTO StencilRules2 ( Entity, Designator1, RuleString, UserName, \"DateTime\", Comments ) "
            "SELECT Entity, 'pcb', 'pcb ' & RuleString, UserName, DateTime, Comments "
            "FROM StencilRules "
            "WHERE Entity='pcb'; "
            );

         executeQuery(stmt, 
            "INSERT INTO StencilRules2 ( Entity, Designator1, Designator2, RuleString, UserName, \"DateTime\", Comments ) "
            "SELECT Entity, DesignatorPrefix, DesignatorSuffix, RuleString, UserName, DateTime, Comments "
            "FROM StencilRules "
            "WHERE Entity='surface' or Entity='mount'; "
            );

         executeQuery(stmt, 
            "INSERT INTO StencilRules2 ( Entity, Designator1, Designator2, Designator3, RuleString, UserName, \"DateTime\", Comments ) "
            "SELECT Entity, 'top', DesignatorPrefix, DesignatorSuffix, Entity &  ' top.' & mid(RuleString,len(Entity)+2), UserName, DateTime, Comments "
            "FROM StencilRules "
            "WHERE Entity='geometry' or Entity='padstack'; "
            );

         executeQuery(stmt, 
            "INSERT INTO StencilRules2 ( Entity, Designator1, Designator2, Designator3, RuleString, UserName, \"DateTime\", Comments ) "
            "SELECT Entity, 'bottom', DesignatorPrefix, DesignatorSuffix, Entity &  ' bottom.' & mid(RuleString,len(Entity)+2), UserName, DateTime, Comments "
            "FROM StencilRules "
            "WHERE Entity='geometry' or Entity='padstack'; "
            );

         executeQuery(stmt, 
            "INSERT INTO StencilRules2 ( Entity, Designator1, Designator2, Designator3, Designator4, RuleString, UserName, \"DateTime\", Comments ) "
            "SELECT Entity, 'top', DesignatorPrefix, DesignatorSuffix,'0', "
            "Entity & ' top.' & mid(left(RuleString,InStr(len(Entity)+2,RuleString,' ')-1),len(Entity) + 2) & '.0 ' & mid(RuleString,InStr(len(Entity) + 2,RuleString,' ') + 1), "
            "UserName, DateTime, Comments "
            "FROM StencilRules "
            "WHERE Entity='geometryPin'; "
            );

         executeQuery(stmt, 
            "INSERT INTO StencilRules2 ( Entity, Designator1, Designator2, Designator3, Designator4, RuleString, UserName, \"DateTime\", Comments ) "
            "SELECT Entity, 'bottom', DesignatorPrefix, DesignatorSuffix,'0', "
            "Entity & ' bottom.' & mid(left(RuleString,InStr(len(Entity)+2,RuleString,' ')-1),len(Entity) + 2) & '.0 ' & mid(RuleString,InStr(len(Entity) + 2,RuleString,' ') + 1), "
            "UserName, DateTime, Comments "
            "FROM StencilRules "
            "WHERE Entity='geometryPin'; "
            );

         SQLFreeHandle( SQL_HANDLE_STMT, stmt );
      }
   }
}


CString CDBInterface::GetCurrentUserName()
{
   CString UserName;
   char acUserName[100];
   DWORD nUserName = sizeof(acUserName);
   if (GetUserName(acUserName, &nUserName))
      UserName = acUserName;

   return UserName;
}

CString CDBInterface::GetCurrentTimeStamp()
{
   CTime time = CTime::GetCurrentTime();

   // mm/dd/yyyy hh:mm:ssAM
   // Original flavor: CString DateTime = time.Format("%m/%d/%Y %I:%M:%S%p");

   // Formatted as per SQL_TIMESTAMP spec for ODBC.
   // Based on info found here: http://msdn.microsoft.com/en-us/library/ms190234(SQL.90).aspx.
   // Use 24 hour time, format like this:
   //            yyyy-mm-dd hh:mm:ss[.fff]    (fractions of second allowed, we do not bother)
   // e.g. { ts '1998-05-02 01:23:56.123' }
   // e.g. { ts '1998-05-02 01:23:56' }   (what we make, without fractions of second)
   // We just get the time string here, it will be saved in the DBPart (and other things) and
   // we don't want SQL syntax to be part of that.
   // To focus the implementation, another func just below will complete the formatting
   // for query construction.

   CString DateTime = time.Format("%Y-%m-%d %H:%M:%S");

   return DateTime;
}

CString CDBInterface::FormatSQLTimeStamp(CString sqlTimeStr)
{
   CString sqlTimestamp;
   sqlTimestamp.Format("{ts '%s'}", sqlTimeStr);

   return sqlTimestamp;
}

