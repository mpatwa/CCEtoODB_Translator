// $Header: /CAMCAD/4.5/ODBC_Lib.h 11    2/14/07 8:19p Rick Faltersack $

#if !defined(__ODBC_LIB_h__)
#define __ODBC_LIB_h__

#pragma once

#include "afxdb.h"
#include "Units.h"
#include "General.h"

//------------------------------------------------------------------------------------------------
// To add a simple table type
// (1) Add an interface object, e.g. CDBPart
//     Note that (so far) all members are set via the constructor, only methods are "get" methods.
// (2) Add a table definition, e.g. CDBPartsTableDef
//     This is used mainly by the utility that validates table columns and creates tables on the fly.
// (3) Add an instance of the new tabledef to CDBTableDefList, in the constructor.
//     This list is used for overall DB table validation when a DB is opened.
// (4) Add "lookup" method to CDBInterface, e.g. LookupPackageInfo.
//     Cloning and modifying an existing one is probably easiest.
// (5) Add "save" method to CDBInterface, e.g. CDBSavePart
//     Again, clone and modify is probably easiest.

// For example of usage look at CDBPackage or CDBPackageInfo in DataDoctor.cpp and
// CDBInterface in DataDoctorPagePackages.cpp
//------------------------------------------------------------------------------------------------

// Name for timestamp column, historically called DateTime in this DB, we'll stay with that.
// Any table using a DateTime aka Timestamp column should do so using this for column name.
#define QDATETIME "DateTime"

class CDBInterface;

enum EDBNameType
{
   dbnamePartNumber,
   dbnameGeometry
};

//---------------------------------------------------------------

class CDBNameValuePair : public CObject
{
private:
   CString m_name;
   CString m_value;

public:
   CDBNameValuePair(CString name)                  { m_name = name; /*value is already blank*/ }
   CDBNameValuePair(CString name, CString value)   { m_name = name; m_value = value; }

   // Can get name, but only constructor can set name
   CString GetName()       { return m_name; }

   // Can get and set value
   CString GetValue()            { return m_value; }
   void SetValue(CString value)  { m_value = value; }
};

class CDBGenericAttributeMap : public CMapSortedStringToOb<CDBNameValuePair>
{
public:
   CString LookupValue(CString attribName);
   void Add(CString name, CString value);
};

//---------------------------------------------------------------

class CDBOutlineVertex : public CObject
{
private:
   CString m_name;
   EDBNameType m_nametype;
   int m_polynum;  // separates multiple polys in same outline
   int m_pointnum; // this pt's position in its poly
   double m_x;      // in millimeters
   double m_y;      //     "
   double m_bulge;

public:
   CDBOutlineVertex(CString name, EDBNameType nametype, int polynum, int pointnum, double x, double y, double bulge, PageUnitsTag fromUnits = pageUnitsMilliMeters)
   {
      m_name = name;
      m_nametype = nametype;
      m_polynum = polynum;
      m_pointnum = pointnum;
      m_x = x * getUnitsFactor(fromUnits, pageUnitsMilliMeters);
      m_y = y * getUnitsFactor(fromUnits, pageUnitsMilliMeters);
      m_bulge = bulge;
   }

   CDBOutlineVertex(CString name, EDBNameType nametype, CString polynum, CString pointnum, CString x, CString y, CString bulge, PageUnitsTag fromUnits = pageUnitsMilliMeters)
   {
      m_name = name;
      m_nametype = nametype;
      m_polynum = atoi(polynum);
      m_pointnum = atoi(pointnum);
      m_x = atof(x)* getUnitsFactor(fromUnits, pageUnitsMilliMeters);
      m_y = atof(y)* getUnitsFactor(fromUnits, pageUnitsMilliMeters);
      m_bulge = atof(bulge);
   }


   CString getName()                      { return m_name; }
   EDBNameType getNameType()              { return m_nametype; }

   int     getPolyNum()                   { return m_polynum; }
   CString getPolyNumStr()                { CString buf; buf.Format("%d", m_polynum); return buf; }

   int     getPointNum()                  { return m_pointnum; }
   CString getPointNumStr()               { CString buf; buf.Format("%d", m_pointnum); return buf; }

   double  getX()                         { return m_x; }
   double  getX(PageUnitsTag toUnits)     { return m_x * getUnitsFactor(pageUnitsMilliMeters, toUnits); }
   CString getXStr()                      { CString buf; buf.Format("%f", m_x); return buf; }

   double  getY()                         { return m_y; }
   double  getY(PageUnitsTag toUnits)     { return m_y * getUnitsFactor(pageUnitsMilliMeters, toUnits); }
   CString getYStr()                      { CString buf; buf.Format("%f", m_y); return buf; }

   double  getBulge()                     { return m_bulge; }
   CString getBulgeStr()                  { CString buf; buf.Format("%f", m_bulge); return buf; }
};

// There's proably a better collection class for this, but for now...

class CDBOutlineVertexMap : public CMapSortedStringToOb<CDBOutlineVertex>
{
public:
   static int VertexOrderSortFunc(const void *a, const void *b);
};

//---------------------------------------------------------------

class CDBPart
{
private:
   int m_rowID;
   CString m_partnumber;
   CString m_devicetype;
   CString m_value;
   CString m_ptol;
   CString m_ntol;
   CString m_subclass;
   CString m_pinmap;
   CString m_description;
   CString m_username;
   CString m_datetime;
   CString m_comment;

   CDBGenericAttributeMap *m_genericAttribs;

public:
   CDBPart(int rowId, CString pn, CString devtype, CString val, CString ptol, CString ntol, CString subclass, CString pinmap, CString desc, 
      CString username, CString datetime, CString comment, CDBGenericAttributeMap *attribMap)
   {
      m_rowID = rowId;
      m_partnumber = pn;
      m_devicetype = devtype;
      m_value = val;
      m_ptol = ptol;
      m_ntol = ntol;
      m_subclass = subclass;
      m_pinmap = pinmap;
      m_description = desc;
      m_username = username;
      m_datetime = datetime;
      m_comment = comment;

      m_genericAttribs = attribMap;
   }

   int     getRowId()         { return m_rowID; }
   CString getPartNumber()    { return m_partnumber; }
   CString getDeviceType()    { return m_devicetype; }
   CString getValue()         { return m_value; }
   CString getPTol()          { return m_ptol; }
   CString getNTol()          { return m_ntol; }
   CString getSubclass()      { return m_subclass; }
   CString getPinmap()        { return m_pinmap; }
   CString getDescription()   { return m_description; }

   CString getGenericAttribVal(CString attribName)    { return (m_genericAttribs != NULL) ? (m_genericAttribs->LookupValue(attribName)) : (""); }

   CDBGenericAttributeMap *GetAttribMap()             { return m_genericAttribs; }
   void SetAttribMap(CDBGenericAttributeMap *newmap)  { if (m_genericAttribs != NULL) delete m_genericAttribs; m_genericAttribs = newmap; }

   CString getDataString(const CString& delimeter);
   CString getUserName()      { return m_username; }
   CString getDateTime()      { return m_datetime; }
   CString getComment()       { return m_comment; }

   void SetUserName(CString username)        { m_username = username; }
   void SetDateTime(CString dateTime)        { m_datetime = dateTime; }

};

//---------------------------------------------------------------

class CDBCentroid
{
private:
   CString m_geomname;
   CString m_algorithm; // aka method
   CString m_x;
   CString m_y;
   CString m_rot;
   CString m_username;
   CString m_datetime;
   CString m_comment;

   PageUnitsTag m_unitsTag;

public:
   CDBCentroid(CString geomname, CString method, CString x, CString y, CString rot, PageUnitsTag unitsTag, CString username, CString datetime, CString comment)
   {
      // These don't depend on units, okay as-is
      m_geomname = geomname;
      m_algorithm = method;
      m_rot = rot;
      m_username = username;
      m_datetime = datetime;
      m_comment = comment;

      if (unitsTag == pageUnitsMilliMeters || unitsTag == pageUnitsUndefined)
      {
         // Use as-is for preferred units or unknown/undefined units
         m_x = x;
         m_y = y;
         m_unitsTag = unitsTag;
      }
      else
      {
         // Convert to DB standard units (mm)
         double xx = atof(x);
         double yy = atof(y);
         xx *= getUnitsFactor(unitsTag, pageUnitsMilliMeters);
         yy *= getUnitsFactor(unitsTag, pageUnitsMilliMeters);
         m_x.Format("%f", xx);
         m_y.Format("%f", yy);
         m_unitsTag = pageUnitsMilliMeters;
      }
   }

   CString getGeomName()    { return m_geomname; }
   CString getAlgorithm()   { return m_algorithm; }

   double getRot()          { return atof(m_rot); }
   CString getRotStr()      { return m_rot; } 

   double  getX(PageUnitsTag toUnits)     { double xx = atof(m_x); return (m_unitsTag != pageUnitsUndefined) ? (xx * getUnitsFactor(m_unitsTag, toUnits)) : (xx); }
   double  getY(PageUnitsTag toUnits)     { double yy = atof(m_y); return (m_unitsTag != pageUnitsUndefined) ? (yy * getUnitsFactor(m_unitsTag, toUnits)) : (yy); }
   CString getXStr(PageUnitsTag toUnits)  { CString buf; buf.Format("%f", getX(toUnits)); return buf; }
   CString getYStr(PageUnitsTag toUnits)  { CString buf; buf.Format("%f", getY(toUnits)); return buf; }

   CString getUserName()      { return m_username; }
   CString getDateTime()      { return m_datetime; }
   CString getComment()       { return m_comment; }

   void SetUserName(CString username)        { m_username = username; }
   void SetDateTime(CString dateTime)        { m_datetime = dateTime; }
};

//---------------------------------------------------------------

class CDBPackage
{
private:
   CString m_geomname;
   CString m_packagealias;
   CString m_outlinemethod;

   CDBGenericAttributeMap *m_genericAttribs;

public:
   CDBPackage(CString geomname, CString packagealias, CString outlinemethod, CDBGenericAttributeMap *attribMap)
   {
      m_geomname = geomname;
      m_packagealias = packagealias;
      m_outlinemethod = outlinemethod;
      m_genericAttribs = attribMap;
   }

   CString getGeomName()      { return m_geomname; }
   CString getPackageAlias()  { return m_packagealias; }
   CString getOutlineMethod() { return m_outlinemethod; }

   CDBGenericAttributeMap *GetAttribMap()             { return m_genericAttribs; }
   void SetAttribMap(CDBGenericAttributeMap *newmap)  { if (m_genericAttribs != NULL) delete m_genericAttribs; m_genericAttribs = newmap; }

   // For support of backward compat lookup
   CDBGenericAttributeMap *GiveAwayAttribMap()  { CDBGenericAttributeMap *temp = m_genericAttribs; m_genericAttribs = NULL; return temp; }

};

//---------------------------------------------------------------

class CDBPackageInfo
{
private:
   CString m_name;
   EDBNameType m_nametype;
   EDBNameType m_packagesource;
   CString m_packagealias;
   CString m_packageheight;  // always in millimeters in DB
   CString m_outlinemethod;
   CString m_dpmo;
   CString m_family;
   CString m_username;
   CString m_datetime;
   CString m_comment;

   CDBGenericAttributeMap *m_genericAttribs;

public:
   CDBPackageInfo(CString name, EDBNameType nametype, EDBNameType packagesource, CString packagealias, CString packageheight, CString outlinemethod, CString dpmo, 
      CString family, CString username, CString datetime, CString comment, CDBGenericAttributeMap *attribMap)
   {
      m_name = name;
      m_nametype = nametype;
      m_packagesource = packagesource;
      m_packagealias = packagealias;
      m_packageheight = packageheight;
      m_outlinemethod = outlinemethod;
      m_dpmo = dpmo;
      m_family = family;
      m_username = username;
      m_datetime = datetime;
      m_comment = comment;

      m_genericAttribs = attribMap;

      // Strip possible units text in height, will always be " mm" if present
      m_packageheight.Replace(" mm", "");
   }

   CDBPackageInfo(CString name, EDBNameType nametype, CString packagesource, CString packagealias, CString packageheight, CString outlinemethod, CString dpmo, 
      CString family, CString username, CString datetime, CString comment, CDBGenericAttributeMap *attribMap)
   {
      m_name = name;
      m_nametype = nametype;
      m_packagesource = GetNameType( packagesource );
      m_packagealias = packagealias;
      m_packageheight = packageheight;
      m_outlinemethod = outlinemethod;
      m_dpmo = dpmo;
      m_family = family;
      m_username = username;
      m_datetime = datetime;
      m_comment = comment;

      m_genericAttribs = attribMap;

      // Strip possible units text in height, will always be " mm" if present
      m_packageheight.Replace(" mm", "");
   }


   ~CDBPackageInfo() { if (m_genericAttribs != NULL) delete m_genericAttribs; }

   CDBGenericAttributeMap *GetAttribMap()             { return m_genericAttribs; }
   void SetAttribMap(CDBGenericAttributeMap *newmap)  { if (m_genericAttribs != NULL) delete m_genericAttribs; m_genericAttribs = newmap; }

   CString getName()                { return m_name; }
   EDBNameType getNameType()        { return m_nametype; }
   EDBNameType getPackageSource()   { return m_packagesource; }
   CString getPackageAlias()        { return m_packagealias; }
   CString getPackageHeight()       { return m_packageheight; }  // Raw text field in mm
   CString getOutlineMethod()       { return m_outlinemethod; }
   CString getDPMO()                { return m_dpmo; }
   CString getFamily()              { return m_family; }
   CString getUserName()            { return m_username; }
   CString getDateTime()            { return m_datetime; }
   CString getComment()             { return m_comment; }

   void SetUserName(CString username)        { m_username = username; }
   void SetDateTime(CString dateTime)        { m_datetime = dateTime; }

   double getPackageHeight(PageUnitsTag toUnits)      { return atof(m_packageheight) * getUnitsFactor(pageUnitsMilliMeters, toUnits); }
   CString getPackageHeightStr(PageUnitsTag toUnits)  { double h = this->getPackageHeight(toUnits); CString buf; if (h > 0.0) buf.Format("%.*f", GetDecimals(toUnits), h); return buf; } // empty string for h = 0 or less

   CString getGenericAttribVal(CString attribName)    { return (m_genericAttribs != NULL) ? (m_genericAttribs->LookupValue(attribName)) : (""); }

   CString getPackageSourceStr() { return GetNameTypeStr(m_packagesource); } 
   CString getNameTypeStr()      { return GetNameTypeStr(m_nametype); }
   static CString GetNameTypeStr(EDBNameType nt)   { return (nt == dbnamePartNumber ? "PartNumber" : "Geometry" ); }
   static EDBNameType GetNameType(CString str)     { str.Replace(" ", ""); return (str.CompareNoCase("PartNumber") == 0 ? dbnamePartNumber : dbnameGeometry); }

};

//---------------------------------------------------------------

class CDBSubclassElement : public CObject
{
private:
   CString m_subclassName;
   CString m_elementName;
   CString m_deviceType;
   CString m_pin1Refname;
   CString m_pin2Refname;
   CString m_pin3Refname;
   CString m_pin4Refname;
   CString m_value;
   CString m_plusTol;
   CString m_minusTol;


public:
   CDBSubclassElement(CString subclassname, CString elementname, CString devtype, CString pin1name, CString pin2name, CString pin3name, CString pin4name, CString value, CString ptol, CString mtol)
   {
      m_subclassName = subclassname;
      m_elementName  = elementname;
      m_deviceType   = devtype;
      m_pin1Refname  = pin1name;
      m_pin2Refname  = pin2name;
      m_pin3Refname  = pin3name;
      m_pin4Refname  = pin4name;
      m_value        = value;
      m_plusTol      = ptol;
      m_minusTol     = mtol;
   }

   CString getSubclassName()  { return m_subclassName; }
   CString getElementName()   { return m_elementName;  }
   CString getDeviceType()    { return m_deviceType;   }
   CString getPin1Refname()   { return m_pin1Refname;  }
   CString getPin2Refname()   { return m_pin2Refname;  }
   CString getPin3Refname()   { return m_pin3Refname;  }
   CString getPin4Refname()   { return m_pin4Refname;  }
   CString getValue()         { return m_value;        }
   CString getPlusTol()       { return m_plusTol;      }
   CString getMinusTol()      { return m_minusTol;     }

};

//---------------------------------------------------------------

class CDBSubclassElementList : public CTypedObArrayContainer<CDBSubclassElement*>
{
public:
   //CDBSubclassElementList& operator=(const CDBSubclassElementList &other);
   bool containsSubclass(CString subclassName);
};

//---------------------------------------------------------------
//---------------------------------------------------------------

//---------------------------------------------------------------

class CDBPanelTemplateToolingHole : public CObject
{
private:
   double m_size;
   double m_x;
   double m_y;

public:
   CDBPanelTemplateToolingHole(double size, double x, double y)     { m_size = size; m_x = x; m_y = y; }
   CDBPanelTemplateToolingHole(CString size, CString x, CString y)  { m_size = atof(size); m_x = atof(x); m_y = atof(y); }

   double GetSize()  { return m_size; }
   double GetX()     { return m_x; }
   double GetY()     { return m_y; }

   CString GetSizeStr()    { CString buf; buf.Format("%f", m_size); return buf; }
   CString GetXStr()       { CString buf; buf.Format("%f", m_x);    return buf; }
   CString GetYStr()       { CString buf; buf.Format("%f", m_y);    return buf; }
};

//---------------------------------------------------------------

class CDBPanelTemplateToolingHoleList : public CTypedObArrayContainer<CDBPanelTemplateToolingHole*>
{
};

//---------------------------------------------------------------

class CDBPanelTemplateFiducial : public CObject
{
private:
   CString m_type;   // aka shape
   double m_size;
   double m_originX;
   double m_originY;
   double m_angle;   // degrees
   bool m_mirrored;

public:
   CDBPanelTemplateFiducial(CString type, double size, double x, double y, double angle, bool mirrored)   
   { m_type = type; m_size = size; m_originX = x; m_originY = y; m_angle = angle; m_mirrored = mirrored; }

   CDBPanelTemplateFiducial(CString type, CString size, CString x, CString y, CString angle, CString mirrored)   
   { m_type = type; m_size = atof(size); m_originX = atof(x); m_originY = atof(y); m_angle = atof(angle); m_mirrored = (mirrored.Left(1).CompareNoCase("Y") == 0); }

   CString GetType()       { return m_type; }
   double GetSize()        { return m_size; }
   double GetOriginX()     { return m_originX; }
   double GetOriginY()     { return m_originY; }
   double GetAngle()       { return m_angle; }
   bool GetMirrored()      { return m_mirrored; }

   CString GetSizeStr()          { CString buf; buf.Format("%f", m_size);      return buf; }
   CString GetOriginXStr()       { CString buf; buf.Format("%f", m_originX);   return buf; }
   CString GetOriginYStr()       { CString buf; buf.Format("%f", m_originY);   return buf; }
   CString GetAngleStr()         { CString buf; buf.Format("%f", m_angle);     return buf; }
   CString GetMirroredStr()      { return (m_mirrored ? "Yes" : "No"); }
};

//---------------------------------------------------------------

class CDBPanelTemplateFiducialList : public CTypedObArrayContainer<CDBPanelTemplateFiducial*>
{
};

//---------------------------------------------------------------

class CDBPanelTemplatePcbInstance : public CObject
{
private:
   CString m_name;
   double m_originX;
   double m_originY;
   double m_angle;
   bool m_mirrored;
   CString m_originType;

public:
   CDBPanelTemplatePcbInstance(CString name, double x, double y, double angle, bool mirrored, CString originType)   
   { m_name = name; m_originX = x; m_originY = y; m_angle = angle; m_mirrored = mirrored; m_originType = originType; }

   CDBPanelTemplatePcbInstance(CString name, CString x, CString y, CString angle, CString mirrored, CString originType)   
   { m_name = name; m_originX = atof(x); m_originY = atof(y); m_angle = atof(angle); m_mirrored = (mirrored.Left(1).CompareNoCase("Y") == 0); m_originType = originType; }

   CString GetName()       { return m_name; }
   double GetOriginX()     { return m_originX; }
   double GetOriginY()     { return m_originY; }
   double GetAngle()       { return m_angle; }
   bool GetMirrored()      { return m_mirrored; }
   CString GetOriginType() { return m_originType; }

   CString GetOriginXStr()       { CString buf; buf.Format("%f", m_originX);   return buf; }
   CString GetOriginYStr()       { CString buf; buf.Format("%f", m_originY);   return buf; }
   CString GetAngleStr()         { CString buf; buf.Format("%f", m_angle);     return buf; }
   CString GetMirroredStr()      { return (m_mirrored ? "Yes" : "No"); }
};

//---------------------------------------------------------------

class CDBPanelTemplatePcbInstanceList : public CTypedObArrayContainer<CDBPanelTemplatePcbInstance*>
{
};

//---------------------------------------------------------------

class CDBPanelTemplatePcbArray : public CObject
{
private:
   CString m_name;
   double m_originX;
   double m_originY;
   double m_angle;
   bool m_mirrored;
   CString m_originType;
   int m_countX;
   int m_countY;
   double m_stepX;
   double m_stepY;

public:
   CDBPanelTemplatePcbArray(CString name, double x, double y, double angle, bool mirrored, CString originType,
      int countX, int countY, double stepX, double stepY)   
   { m_name = name; m_originX = x; m_originY = y; m_angle = angle; m_mirrored = mirrored; m_originType = originType,
   m_countX = countX; m_countY = countY; m_stepX = stepX; m_stepY = stepY; }

   CDBPanelTemplatePcbArray(CString name, CString x, CString y, CString angle, CString mirrored, CString originType,
      CString countX, CString countY, CString stepX, CString stepY)   
   { m_name = name; m_originX = atof(x); m_originY = atof(y); m_angle = atof(angle); m_mirrored = (mirrored.Left(1).CompareNoCase("Y") == 0); m_originType = originType,
   m_countX = atoi(countX); m_countY = atoi(countY); m_stepX = atof(stepX); m_stepY = atof(stepY); }

   CString GetName()       { return m_name; }
   double GetOriginX()     { return m_originX; }
   double GetOriginY()     { return m_originY; }
   double GetAngle()       { return m_angle; }
   bool GetMirrored()      { return m_mirrored; }
   CString GetOriginType() { return m_originType; }
   int GetCountX()         { return m_countX; }
   int GetCountY()         { return m_countY; }
   double GetStepX()       { return m_stepX; }
   double GetStepY()       { return m_stepY; }

   CString GetOriginXStr()       { CString buf; buf.Format("%f", m_originX);   return buf; }
   CString GetOriginYStr()       { CString buf; buf.Format("%f", m_originY);   return buf; }
   CString GetAngleStr()         { CString buf; buf.Format("%f", m_angle);     return buf; }
   CString GetMirroredStr()      { return (m_mirrored ? "Yes" : "No"); }
   CString GetCountXStr()        { CString buf; buf.Format("%d", m_countX);    return buf; }
   CString GetCountYStr()        { CString buf; buf.Format("%d", m_countY);    return buf; }
   CString GetStepXStr()         { CString buf; buf.Format("%f", m_stepX);     return buf; }
   CString GetStepYStr()         { CString buf; buf.Format("%f", m_stepY);     return buf; }
};

//---------------------------------------------------------------

class CDBPanelTemplatePcbArrayList : public CTypedObArrayContainer<CDBPanelTemplatePcbArray*>
{
};
//---------------------------------------------------------------

class CDBPanelTemplateOutline : public CObject
{
private:
   double m_originX;
   double m_originY;
   double m_sizeX;
   double m_sizeY;

public:
   CDBPanelTemplateOutline(double x, double y, double sizex, double sizey)  { m_originX = x; m_originY = y; m_sizeX = sizex; m_sizeY = sizey; }
   CDBPanelTemplateOutline(CString x, CString y, CString sizex, CString sizey)  { m_originX = atof(x); m_originY = atof(y); m_sizeX = atof(sizex); m_sizeY = atof(sizey); }

   double GetOriginX()     { return m_originX; }
   double GetOriginY()     { return m_originY; }
   double GetSizeX()       { return m_sizeX; }
   double GetSizeY()       { return m_sizeY; }

   CString GetOriginXStr()       { CString buf; buf.Format("%f", m_originX);   return buf; }
   CString GetOriginYStr()       { CString buf; buf.Format("%f", m_originY);   return buf; }
   CString GetSizeXStr()         { CString buf; buf.Format("%f", m_sizeX);     return buf; }
   CString GetSizeYStr()         { CString buf; buf.Format("%f", m_sizeY);     return buf; }
};

//---------------------------------------------------------------

class CDBPanelTemplateOutlineList : public CTypedObArrayContainer<CDBPanelTemplateOutline*>
{
};

//---------------------------------------------------------------

class CDBPanelTemplate : public CObject
{
private:
   CString m_templateName;
   CString m_userID;
   CString m_timestamp;
   PageUnitsTag m_pageUnits;  //*rcf BUG need to addd page units to ID table

   CDBPanelTemplateOutlineList     m_outlineList;
   CDBPanelTemplatePcbInstanceList m_pcbInstanceList;
   CDBPanelTemplatePcbArrayList    m_pcbArrayList;
   CDBPanelTemplateFiducialList    m_fiducialList;
   CDBPanelTemplateToolingHoleList m_toolingHoleList;

public:
   CDBPanelTemplate()                                             { m_pageUnits = pageUnitsInches; }
   CDBPanelTemplate(CString templateName, CString pageUnits) { m_pageUnits = StringToPageUnitsTag(pageUnits); m_templateName = templateName; }
   CDBPanelTemplate(CString templateName, CString pageUnits, CString userID, CString timestamp) { m_pageUnits = StringToPageUnitsTag(pageUnits); m_templateName = templateName; m_userID = userID, m_timestamp = timestamp; }

   CString GetName()             { return m_templateName; }
   CString GetUserID()           { return m_userID;       }
   CString GetTimeStamp()        { return m_timestamp;    }
   PageUnitsTag GetPageUnits()   { return m_pageUnits; }
   CString GetPageUnitsStr()     { return PageUnitsTagToString( m_pageUnits ); }

   void SetName(CString name)             { m_templateName = name; }
   void SetUserID(CString ID)             { m_userID = ID; }
   void SetTimeStamp(CString stamp)       { m_timestamp = stamp; }
   void SetPageUnits(PageUnitsTag units)  { m_pageUnits = units; }

   CDBPanelTemplateOutlineList&     GetOutlineList()       { return m_outlineList; }
   CDBPanelTemplatePcbInstanceList& GetPcbInstanceList()   { return m_pcbInstanceList; }
   CDBPanelTemplatePcbArrayList&    GetPcbArrayList()      { return m_pcbArrayList; }
   CDBPanelTemplateFiducialList&    GetFiducialList()      { return m_fiducialList; }
   CDBPanelTemplateToolingHoleList& GetToolingHoleList()   { return m_toolingHoleList; }
};

//---------------------------------------------------------------

class CDBPanelTemplateList : public CTypedObArrayContainer<CDBPanelTemplate*>
{
public:
   //CDBSubclassElementList& operator=(const CDBSubclassElementList &other);
   bool ContainsName(CString subclassName);
   int  GetNameArray(CStringArray &ar);
};

//---------------------------------------------------------------

class CDBStencilSetting
{
private:
   CString m_name;
   CString m_value;
   CDBStencilSetting *m_next;

public:
   CDBStencilSetting(CString name, CString value, CDBStencilSetting *next)
   {
      m_name = name;
      m_value = value;
      m_next = next;
   }

   CString getName()    { return m_name; }
   CString getValue()   { return m_value; }

   CDBStencilSetting* getNext() { return m_next; }

   //CString getDataString(const CString& delimeter);

};

//---------------------------------------------------------------

class CDBStencilRule
{
private:
   CString m_entity;
   CString m_desprefix;
   CString m_dessuffix;
   CString m_rule;
   CString m_username;
   CString m_datetime;
   CString m_comment;
   CDBStencilRule *m_next;

public:
   CDBStencilRule(const CString& entity, const CString& desprefix, const CString& dessuffix, const CString& rule, const CString& username, const CString& datetime, const CString& comment, CDBStencilRule *next)
   {
      m_entity = entity;
      m_desprefix = desprefix;
      m_dessuffix = dessuffix;
      m_rule = rule;
      m_username = username;
      m_datetime = datetime;
      m_comment = comment;
      m_next = next;
   }

   CString getEntity()              { return m_entity; }
   CString getDesignatorPrefix()    { return m_desprefix; }
   CString getDesignatorSuffix()    { return m_dessuffix; }
   CString getRule()                { return m_rule; }
   CString getUserName()            { return m_username; }
   CString getDateTime()            { return m_datetime; }
   CString getComment()             { return m_comment; }

   CString getDataString(const CString& delimeter);

   CDBStencilRule *getNext()        { return m_next; }

};

//---------------------------------------------------------------

class CDBStencilRule2
{
private:
   CString m_entity;
   CString m_designator1;
   CString m_designator2;
   CString m_designator3;
   CString m_designator4;
   CString m_designator5;
   CString m_rule;
   CString m_username;
   CString m_datetime;
   CString m_comment;
   CDBStencilRule2 *m_next;

public:
   CDBStencilRule2(const CString& entity, const CString& designator1, const CString& designator2, const CString& designator3, const CString& designator4, const CString& designator5, 
                   const CString& rule, const CString& username, const CString& datetime, const CString& comment, CDBStencilRule2 *next)
   {
      m_entity = entity;
      m_designator1 = designator1;
      m_designator2 = designator2;
      m_designator3 = designator3;
      m_designator4 = designator4;
      m_designator5 = designator5;
      m_rule = rule;
      m_username = username;
      m_datetime = datetime;
      m_comment = comment;
      m_next = next;
   }

   CString getEntity()      { return m_entity; }
   CString getDesignator1() { return m_designator1; }
   CString getDesignator2() { return m_designator2; }
   CString getDesignator3() { return m_designator3; }
   CString getDesignator4() { return m_designator4; }
   CString getDesignator5() { return m_designator5; }
   CString getRule()        { return m_rule; }
   CString getUserName()    { return m_username; }
   CString getDateTime()    { return m_datetime; }
   CString getComment()     { return m_comment; }

   CString getDataString(const CString& delimeter);

   CDBStencilRule2 *getNext()        { return m_next; }

   bool operator==(CDBStencilRule2 &other);
   bool operator!=(CDBStencilRule2 &other)    { return !(*this == other); }
};

//---------------------------------------------------------------

class CDBCustomMachine : public CObject
{
private:
   CString m_machineName;
   CString m_templateName;


public:
   CDBCustomMachine(CString machinename, CString templatename)
   {
      m_machineName = machinename;
      m_templateName  = templatename;
   }

   CString getMachineName()  { return m_machineName; }
   CString getTemplateName()   { return m_templateName;  }
};

//---------------------------------------------------------------

class CDBCustomMachineList : public CTypedObArrayContainer<CDBCustomMachine*>
{
public:
   bool containsTemplate(CString templateName);
};

//---------------------------------------------------------------

class CDBCustomAssemblyTemplate : public CObject
{
private:
   CStringArray m_TemplateSettings;


public:
   CDBCustomAssemblyTemplate(CStringArray& TemplateSettings)
   {
      m_TemplateSettings.RemoveAll();
      for(int idx = 0; idx < TemplateSettings.GetCount(); idx++)
         m_TemplateSettings.Add(TemplateSettings.GetAt(idx));
   }

   CStringArray &getTemplateSettings(){return m_TemplateSettings;}
   CString getTemplateName(){ return (m_TemplateSettings.GetCount())?m_TemplateSettings.GetAt(0):"";}
   CString getPCBSide(){return (m_TemplateSettings.GetCount() > 13)?m_TemplateSettings.GetAt(12):"";}
};

//---------------------------------------------------------------

class CDBCustomAssemblyTemplateList : public CTypedObArrayContainer<CDBCustomAssemblyTemplate*>
{
private:
   int m_fieldCount;

public:
   CDBCustomAssemblyTemplateList(){m_fieldCount = 27;}
   CString getFieldName()
   {
      CString field = "TemplateName, SeparationCharacter, WriteHeader, ExplodePanel,";
      field += " WriteTopHeader, WriteBottomHeader, TopHeaderFilePath, BottomHeaderFilePath,";
      field += " ReportOrigin, OutputUnits, NumberOfDecimals, MirrorCoordinates,";
      field += " WriteSurface, SuppressInsertAttributeKeyword, SuppressInsertAttributeValue, IncludeInsertAttributeKeyword,"; 
      field += " IncludeInsertAttributeValue, WriteBoards, WriteSmdComponents, WriteThruComponents,";
      field += " WriteFiducials, WriteToolingHoles, WriteMechanicalComponents, WriteGenericComponents,"; 
      field += " WriteXouts, WriteTestPoints, WriteTestProbes";

      return field;
   }

   int getFieldCount(){return m_fieldCount;}
};

//---------------------------------------------------------------

class CDBCustomReportParameter : public CObject
{
private:
   CString m_templateName;  
   CString m_Command;  
   CString m_Parameter;  
   CString m_FieldName;  
   CString m_Priority;

public:
   CDBCustomReportParameter(CString templateName, int Priority, CString Command, CString Parameter, CString FieldName)
   {
      m_templateName = templateName;
      m_Priority.Format("%d", Priority);
      m_Command = Command;
      m_Parameter = Parameter;
      m_FieldName = FieldName;
   }
   
   CDBCustomReportParameter(CStringArray &parameterlist)
   {
      if(parameterlist.GetCount() >= 5)
      {
         m_templateName = parameterlist.GetAt(0);
         m_Priority = parameterlist.GetAt(1);
         m_Command = parameterlist.GetAt(2);
         m_Parameter = parameterlist.GetAt(3);
         m_FieldName = parameterlist.GetAt(4);
      }
   }

   CString &getTemplateName(){return m_templateName;}
   CString &getCommand(){return m_Command;}
   CString &getParameter(){return m_Parameter;}
   CString &getFieldName(){return m_FieldName;}
   CString &getPriority(){return m_Priority;}
};

//---------------------------------------------------------------

class CDBCustomReportParameterList : public CTypedObArrayContainer<CDBCustomReportParameter*>
{
private:
   int m_fieldCount;

public:
   CString getFieldName()
   {
      CString field = "TemplateName, Priority, Command, Parameter, FieldName";
      return field;
   }

   CDBCustomReportParameterList(){m_fieldCount = 5;}
   int getFieldCount(){return m_fieldCount;}
};

//---------------------------------------------------------------

class CDBTableDef
{
private:
   CString m_tableName;
   CStringArray m_colNames;

protected:
   void SetTableName(CString tblname)  { m_tableName = tblname; }
   void AddColumnName(CString colname) { m_colNames.Add( colname ); }

public:
   CString GetTableName()           { return m_tableName; }
   CStringArray &GetColumNames()    { return m_colNames; }
   bool ValidateDBTableColumns(CDBInterface *db);
};

class CDBTableDefList
{
private:
   CTypedPtrListContainer<CDBTableDef*> m_list;
   POSITION m_curPos;

public:
   CDBTableDefList();

   void AddTail(CDBTableDef* tabledef)    { m_list.AddTail(tabledef); }
   CDBTableDef* GetNext()                 { return (m_curPos != NULL) ? m_list.GetNext(m_curPos) : NULL; }
   void Reset()                           { m_curPos = m_list.GetHeadPosition(); }

};

class CDBParts1TableDef : public CDBTableDef
{
public:
   CDBParts1TableDef();
};

class CDBParts2TableDef : public CDBTableDef
{
public:
   CDBParts2TableDef();
};

class CDBParts3TableDef : public CDBTableDef
{
public:
   CDBParts3TableDef();
};

class CDBCentroidsOldTableDef : public CDBTableDef
{
public:
   CDBCentroidsOldTableDef();
};


class CDBCentroidsMMTableDef : public CDBTableDef
{
public:
   CDBCentroidsMMTableDef();
};

class CDBCentroidsMM2TableDef : public CDBTableDef
{
public:
   CDBCentroidsMM2TableDef();
};

class CDBCentroidsMM3TableDef : public CDBTableDef
{
public:
   CDBCentroidsMM3TableDef();
};


class CDBPackagesTableDef : public CDBTableDef
{
public:
   CDBPackagesTableDef();
};

class CDBPackageInfo1TableDef : public CDBTableDef
{
public:
   CDBPackageInfo1TableDef();
};

class CDBPackageInfo2TableDef : public CDBTableDef
{
public:
   CDBPackageInfo2TableDef();
};

class CDBPackageInfo3TableDef : public CDBTableDef
{
public:
   CDBPackageInfo3TableDef();
};


class CDBSubclassElementTableDef : public CDBTableDef
{
public:
   CDBSubclassElementTableDef();
};

class CDBGenericAttributeTableDef : public CDBTableDef
{
public:
   CDBGenericAttributeTableDef();
};

class CDBOutlineVertexTableDef : public CDBTableDef
{
public:
   CDBOutlineVertexTableDef();
};

class CDBStencilSettingsTableDef : public CDBTableDef
{
public:
   CDBStencilSettingsTableDef();
};

class CDBStencilRules1TableDef : public CDBTableDef
{
public:
   CDBStencilRules1TableDef();
};

class CDBStencilRules2TableDef : public CDBTableDef
{
public:
   CDBStencilRules2TableDef();
};

class CDBStencilRules3TableDef : public CDBTableDef
{
public:
   CDBStencilRules3TableDef();
};

class CDBStencilRules4TableDef : public CDBTableDef
{
public:
   CDBStencilRules4TableDef();
};


class CDBPanelTemplateIDTableDef1 : public CDBTableDef
{
public:
   CDBPanelTemplateIDTableDef1();
};


class CDBPanelTemplateIDTableDef2 : public CDBTableDef
{
public:
   CDBPanelTemplateIDTableDef2();
};


class CDBPanelTemplateToolingHoleTableDef : public CDBTableDef
{
public:
   CDBPanelTemplateToolingHoleTableDef();
};

class CDBPanelTemplateFiducialTableDef : public CDBTableDef
{
public:
   CDBPanelTemplateFiducialTableDef();
};

class CDBPanelTemplatePcbInstanceTableDef : public CDBTableDef
{
public:
   CDBPanelTemplatePcbInstanceTableDef();
};

class CDBPanelTemplatePcbArrayTableDef : public CDBTableDef
{
public:
   CDBPanelTemplatePcbArrayTableDef();
};

class CDBPanelTemplateOutlineTableDef : public CDBTableDef
{
public:
   CDBPanelTemplateOutlineTableDef();
};

class CDBCustomMachineTableDef : public CDBTableDef
{
public:
   CDBCustomMachineTableDef();
};

class CDBCustomAssemblyTemplateTableDef : public CDBTableDef
{
public:
   CDBCustomAssemblyTemplateTableDef();
};

class CDBCustomReportParameterTableDef : public CDBTableDef
{
public:
   CDBCustomReportParameterTableDef();
};

//---------------------------------------------------------------

class CDBInterface
{
private:
   SQLHENV  m_env;  // db connection environment
   SQLHDBC  m_dbc;  // db connection handle
   bool m_dbConnected;
   bool m_attemptConnection;      // user can turn on/off connection attempts
   static bool m_allowConnection; // system can turn on/off connection, in particular false here overrides user choice to turn on connection
   bool m_hasDbHandle;
   bool m_hasEnvHandle;
   CString m_errorMessage;

   void nukeOldRegistryKey(); // Gets rid of old VB app ccpartslib.exe registry keys

private:
   bool SavePanelTemplateToolingHoles(CDBPanelTemplate *panelTemplate);
   bool SavePanelTemplateToolingHole(CString templateName, CDBPanelTemplateToolingHole *ptToolingHole);
   bool SavePanelTemplateFiducials(CDBPanelTemplate *panelTemplate);
   bool SavePanelTemplateFiducial(CString templateName, CDBPanelTemplateFiducial *fid);
   bool SavePanelTemplatePcbInstances(CDBPanelTemplate *panelTemplate);
   bool SavePanelTemplatePcbInstance(CString templateName, CDBPanelTemplatePcbInstance *pcbInst);
   bool SavePanelTemplatePcbArrays(CDBPanelTemplate *panelTemplate);
   bool SavePanelTemplatePcbArray(CString templateName, CDBPanelTemplatePcbArray *pcbAr);
   bool SavePanelTemplateOutlines(CDBPanelTemplate *panelTemplate);
   bool SavePanelTemplateOutline(CString templateName, CDBPanelTemplateOutline *outline);

   CDBPanelTemplate* LookupPanelTemplateID(CString templateName);
   CDBPanelTemplate* LookupPanelTemplateID(CDBTableDef &tabledef, CString templateName);
   bool LookupPanelTemplateOutlines(CDBPanelTemplate &panelTemplate); // Must already have name set in panelTemplate
   bool LookupPanelTemplatePcbInstances(CDBPanelTemplate &panelTemplate); // Must already have name set in panelTemplate
   bool LookupPanelTemplatePcbArrays(CDBPanelTemplate &panelTemplate); // Must already have name set in panelTemplate
   bool LookupPanelTemplateFiducials(CDBPanelTemplate &panelTemplate); // Must already have name set in panelTemplate
   bool LookupPanelTemplateToolingHoles(CDBPanelTemplate &panelTemplate); // Must already have name set in panelTemplate

   // For backward compatiblity management
   CDBCentroid *LookupCentroidLowLevel(CString geomname, CDBTableDef &tableDef, PageUnitsTag pageUnits);
   CDBCentroid *LookupCentroidLowLevel1(CString geometryname, CDBTableDef &tabledef, PageUnitsTag pageUnits); // 4.8 style centroid

   // Form of tables 2 and 3 is same, semantics of designator are different, but still can share table read
   CDBStencilRule2* LookupStencilRuleTable2or3or4(CDBTableDef &tabledef, const CString& entity, const CString& designator1, const CString& designator2); // lookup set, all that match entity/prefix regardless of suffix
   CDBStencilRule2* LookupStencilRuleTable2or3or4(CDBTableDef &tabledef, const CString& entity, const CString& designator1, const CString& designator2, const CString& designator3, const CString& designator4, const CString& designator5); // lookup exact entity/prefix/suffix match

public:
   const static int DB_STR_LEN = 256;  // Buffer size for data field retrieval

   CDBInterface();
   ~CDBInterface();

   bool SaveDataSourceName(CString dsn); // Has to do with the msAccess driver, not the MDB data file
   CString GetDataSourceName();

   bool CreateMDBFile(CString completeFilePath); // Create new, empty MDB file
   bool PresentDBFileChooser();        // Let user pick a DB file
   CString GetLibraryDatabaseName();   // Returns current setting for DB filename
   bool SetLibraryDatabaseName(const CString libraryDatabaseName); // Save DB file setting

   bool Connect(CString dbpath, CString username, CString password, bool reportError = true);     // dbpath = path to MS Access DB file, username and password supported in interface, but camcad uses "" for both
   bool Connect(bool reportError = true) { return Connect(this->GetLibraryDatabaseName(), "", "", reportError ); }  // connect with current db file settings, camcad does not currently use username or password
   void Disconnect();
   bool IsConnected()   { return m_dbConnected; }

   // Connection Attempt Allowed is a System Choice, user can not change this.
   // If connection is NOT Allowed then user choice to enable is irrelevant (overridden).
   static bool GetConnectionAttemptAllowed()              { return m_allowConnection; }
   static void SetConnectionAttemptAllowed(bool flag);

   // Connection Attempt Enabled is a User Choice (user can turn DB usage on or off)
   // For attempt to actually be enabled, it must also be allowed.
   bool GetConnectionAttemptEnabled()              { return m_attemptConnection && m_allowConnection; }
   void SetConnectionAttemptEnabled(bool flag);

   bool TableExists(CString tablename);
   bool TableExists(CDBTableDef &tbl)     { return TableExists(tbl.GetTableName()); }
   bool CreateTable(CString tablename, CStringArray& columnnames);
   bool CreateTable(CDBTableDef &tbl)   { return CreateTable( tbl.GetTableName(), tbl.GetColumNames() ); }
   bool DeleteTableEntries(CString tableName, CString columnName, CString value); // Delete from table rows with value in given column

   CString GetCurrentUserName();
   CString GetCurrentTimeStamp();
   CString FormatSQLTimeStamp(CString sqlTimeStr);

   bool GetExistingGenericAttribNames(CStringArray &existingAttribNames); // All AttribNames, regardless of name key type

   CString GetErrorMessage()  { return m_errorMessage; }
   void FormatErrorMessage(SQLSMALLINT handleType, SQLHANDLE handle, CString query, CString additionalMsg); // sets m_errorMessage
   void ReportError(SQLSMALLINT handleType, SQLHANDLE handle, CString query, CString additionalMsg);
   void ReportError(SQLSMALLINT handleType, SQLHANDLE handle, CString query) { ReportError(handleType, handle, query, ""); }
   void ReportDBSaveResult(int attemptCount, int successCount);
   
   // Lookups return an allocated item, it is up to caller to delete when done with it.
   // Return is NULL if desired lookup finds nothing, or if there are other DB connection failure issues.
private:
   CDBPart *LookupPart1(CString partnumber); // 4.7 style Parts
   CDBPart *LookupPart2(CString partnumber); // 4.8 style Parts
   CDBPart *LookupPart3(CString partnumber); // 4.8 style Parts, Update 3 style DateTime column format change

   CDBPart *LookupPart2or3(CDBTableDef &tbl, CString partnumber); // Shared, same lookup works for Part2 and Part3 tables.

public:
   CDBPart *LookupPart(CString partnumber, bool backwardCompatible = true); //lookup 4.8 , if not found try 4.7

   CDBCentroid *LookupCentroid(CString geomname, bool backwardCompatible = true);
   CDBPackage *LookupPackage(CString geomname); // 4.6 style Packages table

private:
   CDBPackageInfo *LookupPackageInfo0(CString name, EDBNameType nametype); // 4.6 and prior style Package Info table
   CDBPackageInfo *LookupPackageInfo1(CString name, EDBNameType nametype); // 4.7 style Package Info table
   CDBPackageInfo *LookupPackageInfo2(CString name, EDBNameType nametype); // 4.8 style Package Info table
   CDBPackageInfo *LookupPackageInfo3(CString name, EDBNameType nametype); // 4.8 style Package Info table, Update 3
   CDBPackageInfo *LookupPackageInfo23(CDBTableDef &tabledef, CString name, EDBNameType nametype); // 4.8 style Package Info table

public:
   CDBPackageInfo *LookupPackageInfo(CString name, EDBNameType nametype, bool backwardCompatible = true); // BC=BackwardCompatible, lookup 4.8 => 4.7, if not found try 4.6
   CDBGenericAttributeMap *LookupGenericAttributes(CString name, EDBNameType nametype);  // Get all attribs for name/nameType
   bool LookupGenericAttribute(CString name, EDBNameType nametype, CString attribName, CString &returnAttribValue); // Get this particular attrib value, returns true if attribute exists (blank return attribValue is ambiguous)
   CDBOutlineVertexMap *LookupOutline(CString name, EDBNameType nametype);

   CDBSubclassElementList *LookupSubclassElements(CString subclassName);

   CDBPanelTemplateList *LookupPanelTemplates(bool templateNamesOnly = false);
   CDBPanelTemplate *LookupPanelTemplate(CString name);

   CDBStencilSetting *LookupStencilSetting(CString settingname);

private:
   // Rule form 2, table 2
   CDBStencilRule2* LookupStencilRule2(const CString& entity, const CString& designator1, const CString& designator2); // lookup set, all that match entity/prefix regardless of suffix
   //not used CDBStencilRule2* LookupStencilRule2(const CString& entity, const CString& designator1, const CString& designator2, const CString& designator3, const CString& designator4, const CString& designator5); // lookup exact entity/prefix/suffix match

   // Rule form is still 2, but table is 3, while form is same the semantics of designator columns is different
   CDBStencilRule2* LookupStencilRule3(const CString& entity, const CString& designator1, const CString& designator2); // lookup set, all that match entity/prefix regardless of suffix
   //not used CDBStencilRule2* LookupStencilRule3(const CString& entity, const CString& designator1, const CString& designator2, const CString& designator3, const CString& designator4, const CString& designator5); // lookup exact entity/prefix/suffix match

   // Rule form is still 2, but table is 4, while form is same the semantics of designator columns is different
   CDBStencilRule2* LookupStencilRule4(const CString& entity, const CString& designator1, const CString& designator2); // lookup set, all that match entity/prefix regardless of suffix
   CDBStencilRule2* LookupStencilRule4(const CString& entity, const CString& designator1, const CString& designator2, const CString& designator3, const CString& designator4, const CString& designator5); // lookup exact entity/prefix/suffix match

   // Only used by save operation
   CDBStencilRule2* LookupStencilRule(const CString& entity, const CString& designator1, const CString& designator2, const CString& designator3, const CString& designator4, const CString& designator5); // lookup exact entity/prefix/suffix match

public:
   // Backward compatible lookup, uses the specific lookups above, this is only one used outside of DB code
   CDBStencilRule2* LookupStencilRule(const CString& entity, const CString& designator1, const CString& designator2); // lookup set, all that match entity/prefix regardless of suffix

   CDBCustomMachineList* LookupCustomMachines(CString queryMachineName);
   CDBCustomAssemblyTemplateList* LookupCustomAssemblyTemplates(CString queryTemplateName);
   CDBCustomReportParameterList* LookupCDBCustomReportParameter(CString queryTemplateName);

   SQLRETURN executeQuery(SQLHSTMT& stmt,const CString& query);

   void migrateStencilData2();

private:
   void DeletePanelTemplateID(CDBTableDef &tabledef, CDBPanelTemplate *panelTemplate);

public:
   bool SavePanelTemplate(CDBPanelTemplate *panelTemplate);

   bool SavePart(CDBPart *part);
   bool SavePart(CDBPart &part)  { return SavePart(&part); }

   bool SaveCentroid(CDBCentroid *centroid);
   bool SaveCentroid(CDBCentroid &centroid)  { return SaveCentroid(&centroid); }

   bool SavePackage(CDBPackage *package);    // 4.6 style packages tables (package info in component geometry)
   bool SavePackage(CDBPackage &package)     { return SavePackage(&package); }

   bool SavePackageInfo(CDBPackageInfo *package);  // 4.7 style package info tables (package info independant of component geometry)
   bool SavePackageInfo(CDBPackageInfo &package)     { return SavePackageInfo(&package); }

   bool SaveSubclassElement(CDBSubclassElement *el);
   bool SaveSubclass(CString subclassName, CDBSubclassElementList *list);
   bool DeleteSubclass(CString subclassName);

   bool SaveGenericAttributes(CString name, EDBNameType nametype, CDBGenericAttributeMap *attribMap);
   bool SaveGenericAttributes(CString name, EDBNameType nametype, CDBGenericAttributeMap &attribMap)  { return SaveGenericAttributes(name, nametype, &attribMap); }
   
   bool SaveOutlineVertices(CString name, EDBNameType nametype, CDBOutlineVertexMap *map);
   bool SaveOutlineVertices(CString name, EDBNameType nametype, CDBOutlineVertexMap &map)  { return SaveOutlineVertices(name, nametype, &map); }

   bool SaveStencilSetting(CDBStencilSetting *setting);
   bool SaveStencilSetting(CDBStencilSetting &setting)   { return SaveStencilSetting(&setting); }

   bool SaveStencilRule(CDBStencilRule2 *rule);
   bool SaveStencilRule(CDBStencilRule2 &rule)   { return SaveStencilRule(&rule); }
   
#ifdef SUPPORT_RULE_KILLER
   bool KillStencilRule(CString entity, CString designator1, CString designator2, CString designator3);   // Marks entity as "DEAD", makes it invisible to SG, like a ghost
#endif

   bool DeleteStencilRule(CString entity, CString designator1, CString designator2, CString designator3); // Delete rule entirely

   bool SaveCustomMachine(CDBCustomMachine *cm);
   bool DeleteCustomMachine(CString machineName);

   bool SaveCustomAssemblyTemplate(CDBCustomAssemblyTemplate *catemplate);
   bool DeleteCustomAssemblyTemplate(CString templateName);
   
   bool SaveCustomReportParameter(CDBCustomReportParameter *crptParameter);
   bool DeleteCustomReportParameter(CString templateName);

   bool ValidateTables(bool reportError = true);
   bool ValidateTable(CDBTableDef &tableDef, CString &msg);
   bool GetColumnNames(CString tablename, CStringArray &colnames);

};


#endif
