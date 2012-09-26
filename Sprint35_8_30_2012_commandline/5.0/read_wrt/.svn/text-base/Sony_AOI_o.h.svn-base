// $Header: /CAMCAD/4.5/read_wrt/Sony_AOI_o.h 17    4/03/06 12:47p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2005. All Rights Reserved.
*/

#if !defined(__Sony_AOI_o_h__)
#define __Sony_AOI_o_h__

#pragma once

#include "extents.h"

#define SONYAOI_DUMMYTYPE_UNKNOWN 0
#define SONYAOI_DUMMYTYPE_MISSING 1  // missing is a valid setting, not to be confused with a value not present
#define SONYAOI_DUMMYTYPE_OK_END  2


enum PcbSide
{
	sideTop,
	sideBottom
};

enum SonyPartStatusTag
{
	sonypartUnassigned,
	sonypartAssigned,
	sonypartError
};

enum SonyCatModNameKeyType
{
	catmodKeyNumber,
	catmodKeyName,
	catmodKeyCategoryOnlyNum,
	catmodKeyCategoryOnlyStr
};


extern CString sonyAoiGetDatastructPartnumber(CCEtoODBDoc *doc, DataStruct *partdata);


class CSonyDatabase;
class CSonyRsiXref;

class CSonyAoiSettings
{
private:
	int m_dummyType;
	double m_pixelConversion;  // size of pixel in microns
	int m_maxFrameSize;     // largest frame the machine can support
	CString m_masterLibraryLocation;
	CString m_masterLibraryPassword;
	bool m_exportLoaded; // true == export only fitted components, false == export all components
	CString m_rsiXrefLibraryLocation;


public:
	CSonyAoiSettings();
	//~CSonyAoiSettings();

	int getDummyType()						{ return m_dummyType; }
	double getPixelConversion()			{ return m_pixelConversion; }
	int getMaxFrameSize()					{ return m_maxFrameSize; }
	CString getMasterLibraryLocation()	{ return m_masterLibraryLocation; }
	CString getMasterLibraryPassword()	{ return m_masterLibraryPassword; }
	bool getExportLoadedOnly()				{ return m_exportLoaded; }
	CString getRsiXrefLibraryLocation()	{ return m_rsiXrefLibraryLocation; }

};

//------------------------------------------------------------------------------

class CSonyCatMod
{
private:
	int m_mastercat;
	int m_mastermod;
	int m_cat;
	int m_mod;

public:
	CSonyCatMod(const int category, const int model) { m_mastercat = category; m_mastermod = model; m_cat = 0; m_mod = 0; }

	int getMasterCategory()	{ return m_mastercat; }
	int getMasterModel()		{ return m_mastermod; }

	void setMappedCategory(int cat)	{ m_cat = cat; }
	void setMappedModel(int mod)		{ m_mod = mod; }
	int getMappedCategory()	{ return m_cat; }
	int getMappedModel()		{ return m_mod; }

};

class CSonyCatModArray : public CTypedPtrArrayWithMapContainer<CSonyCatMod>
{
private:
	int m_dummyCat;

public:
	CSonyCatModArray(int size);
   CSonyCatMod* add(const int category, const int model);
   CSonyCatMod* get(const int category, const int model);

	void getMappedCategoryModel(const int mastercat, const int mastermod, int *mappedcat,int *mappedmod);

	int getLowestMasterCategory();
	int getHighestMasterCategory();
	int getNextMasterCategory(int currentMasterCategory);

	int getHighestMappedCategory();

	void createConsecutiveMapping();

	void writeLogFile(CString side);

	int getDummyCat()	{return m_dummyCat; }

};

//------------------------------------------------------------------------------

class CSonyPart
{
private:
	CCEtoODBDoc *m_doc;
   DataStruct *m_partData;  // data ptr that contains insert for part

	int m_masterCategory;
	int m_masterModel;

	bool m_isOnTop;  // A given partnumber can be both on top
	bool m_isOnBot;  // and on bottom

	bool m_isFid;	// Usually we have a PCB_COMPONENT, but we might have a fid

	bool m_isUsed;  // Items may remain in list but become unused

	bool m_save;
	bool m_explode;
	int m_angleOffsetDegrees;

public:
	CSonyPart(CCEtoODBDoc *doc, DataStruct *pd);
	DataStruct *getData() {return m_partData;}
	CString getPartNumber();

	void setMasterCategory(int cat)  {m_masterCategory = cat;}
	int  getMasterCategory()         {return m_masterCategory;}

	void setMasterModel(int mod)     {m_masterModel = mod;}
	int  getMasterModel()            {return m_masterModel;}

	void setAngleOffset(int angleDegrees)	{m_angleOffsetDegrees = angleDegrees;}
	int  getAngleOffset()						{return m_angleOffsetDegrees;}

	void setSave(bool f)		{ m_save = f;    }
	bool getSave()				{ return m_save; }

	void setExplode(bool f)		{ m_explode = f;    }
	bool getExplode()				{ return m_explode; }

	void setIsOnTop(bool f)		{ m_isOnTop = f;    }
	bool getIsOnTop()				{ return m_isOnTop; }

	bool	getIsFid()				{ return m_isFid; }

	void setIsUsed(bool f)		{ m_isUsed = f;    }
	bool getIsUsed()				{ return m_isUsed; }

	void setIsOnBottom(bool f)	{ m_isOnBot = f;    }
	bool getIsOnBottom()			{ return m_isOnBot; }

	SonyPartStatusTag getStatus();	//	{ return (m_masterCategory > 0 ? sonypartAssigned : sonypartUnassigned); }
	CString getStatusText();			//	{ return (m_masterCategory > 0 ? "Assigned"       : "Unassigned"); }

	CString getGeomName()	{ return (m_doc->getBlockAt(m_partData->getInsert()->getBlockNumber()))->getName(); }
	CString getBitmap();

	void determineFramesize(double *frameSizeX, double *frameSizeY);

	CString getMasterCategoryName();
	CString getMasterModelName();
	CString getAngleOffsetString();


};


//------------------------------------------------------------------------------

class CSonyPartArray : public CTypedPtrArrayWithMapContainer<CSonyPart>
{
private:
	CCEtoODBDoc *m_doc;

public:
   CSonyPartArray(CCEtoODBDoc *doc, int size);

   CSonyPart* getOrAdd(DataStruct *partdata);
   CSonyPart* get(DataStruct *partdata);

	
};


//------------------------------------------------------------------------------

class CSonyBoard
{
private:
	CCEtoODBDoc *m_doc;
	FileStruct *m_activeCCFile;
	CSonyPartArray m_uniqueParts;
	CSonyCatModArray m_topCatModMap;
	CSonyCatModArray m_botCatModMap;
	double m_originX;
	double m_originY;
	CString m_exportFolder;
	CString m_boardName;
	DataStruct *m_topFid1;
	DataStruct *m_topFid2;
	DataStruct *m_botFid1;
	DataStruct *m_botFid2;
	int m_topPartCount;
	int m_botPartCount;

	int m_fidNumber; // used during output process
	CString m_suffixBoardName; // used during output process
	CBasesVector *m_pcbBasesVector; // used during output process
	bool m_packageOutlineMissing; // used during output process

	void writeCCFile(CStdioFileWriteFormat *wfp, PcbSide side);
	void writePanelPCBs(CStdioFileWriteFormat *wfp, CDataList &datalist, PcbSide side);
	void writePanelFids(CStdioFileWriteFormat *wfp, CDataList &datalist, PcbSide side);
	void writePCB(CStdioFileWriteFormat *wfp, BlockStruct *pcbblock, PcbSide side);
	void writePCBInserts(CStdioFileWriteFormat *wfp, BlockStruct *pcbblock, PcbSide side, int processingInsertType);
	void writePCBComponent(CStdioFileWriteFormat *wfp, DataStruct *compData);
	void writePCBFiducial(CStdioFileWriteFormat *wfp, DataStruct *compData);
	void writeSonyPartsRec(CStdioFileWriteFormat *wfp, CString title, CString refDes, CString partNumber, int category, int model, double sonyX, double sonyY, int sonyAngle, double frameSizeX, double frameSizeY);
	FileStruct *determineActiveCCFile(CCEtoODBDoc *doc);

	int countInsertType(InsertTypeTag desiredInsertType, BlockStruct *pcbblock, PcbSide side);
	int countInsertType(InsertTypeTag desiredInsertType, CDataList &datalist, PcbSide side);
	int countParts(PcbSide side); // call this one, the other two counters are support for this

	void setOrigin(CExtent *ext) { m_originX = ext->getXmin(); m_originY = ext->getYmin(); }

	void createBoardSideSonyDB(PcbSide side);

	void transferImageData(PcbSide side);

public:
	CSonyBoard(CCEtoODBDoc *doc);
	~CSonyBoard();
	void gatherUniqueParts();
	void gatherUniqueParts(BlockStruct *pcbblock);
	void gatherFiducials();
	void applyPartNumberCrossReference(CSonyPart *p);  // this part only
	void applyPartNumberCrossReference();  // all parts in list
	void savePartNumberCrossReference();
	void mapBoardSideCategoryModel();
	void writeTopTxt();
	void writeBottomTxt();
	bool getOverwritePermission();
	bool exportAll();

	CString getDefaultBoardName();	// default name based on cc data

	CString getBoardName()				{ return m_boardName; } // name for export, possibly user set via GUI
	void setBoardName(CString name)	{ m_boardName = name; }

	DataStruct *getTopFid1()		{ return m_topFid1; }
	DataStruct *getTopFid2()		{ return m_topFid2; }
	DataStruct *getBotFid1()		{ return m_botFid1; }
	DataStruct *getBotFid2()		{ return m_botFid2; }
	void setTopFid1(DataStruct *fid)	{ m_topFid1 = fid;  }
	void setTopFid2(DataStruct *fid)	{ m_topFid2 = fid;  }
	void setBotFid1(DataStruct *fid)	{ m_botFid1 = fid;  }
	void setBotFid2(DataStruct *fid)	{ m_botFid2 = fid;  }

	int getPartCount(PcbSide side);

	bool hasValidFidSelections();

	void setExportFolder(CString path)	{ m_exportFolder = path; }
	CString getExportFolder()				{ return m_exportFolder; }

	CFilePath getExportTxtFilePath(PcbSide side);
	CFilePath getExportDBFilePath(PcbSide side);

	CExtent GetOutlineExtent();

	CSonyPartArray& getUniqueParts() { return m_uniqueParts; }
	CSonyCatModArray& getTopCatModMap() {return m_topCatModMap; }
	CSonyCatModArray& getBotCatModMap() {return m_botCatModMap; }

	bool hasActiveFile()				{ return (m_activeCCFile != NULL); }
	FileStruct *getActiveFile()	{ return m_activeCCFile; }

	CCEtoODBDoc *getDoc()		{ return m_doc; }

	CSonyDatabase* getMasterSonyDB();
	CSonyRsiXref*  getMasterXref();

	void logUniqueParts(CString title);

	// not a great class for this method, but nothing else is handy.
	// maybe the settings class ought to do the part approval?
	bool partIsLoaded(DataStruct *partdata);
	
};

//------------------------------------------------------------------------------

class CSonyDatabaseCatModName
{
public:
	int m_catNum;
	int m_modNum;
	CString m_catName;
	CString m_modName;
	SonyCatModNameKeyType m_keyType;
	int m_libProExists; // -1 = not yet checked, 0 = does not exist, 1 = exists

public:
	CSonyDatabaseCatModName(int cat, int mod, CString catname, CString modname, SonyCatModNameKeyType keytype) { m_catNum = cat; m_modNum = mod; m_catName = catname; m_modName = modname; m_keyType = keytype; m_libProExists = -1; }
	int getCatNum()		{ return m_catNum; }
	int getModNum()		{ return m_modNum; }
	CString getCatName()	{ return m_catName; }
	CString getModName()	{ return m_modName; }
	SonyCatModNameKeyType getKeyType() { return m_keyType; }
	int  getLibProExists()	{ return m_libProExists; }
	void setLibProExists(int flag)	{ m_libProExists = flag; }
};

class CSonyDatabaseCatModNameArray : public CTypedPtrArrayWithMapContainer<CSonyDatabaseCatModName>
{
public:
	CSonyDatabaseCatModNameArray(int size);
   void add(const int category, const int model, CString catname, CString modname);
   CSonyDatabaseCatModName* get(const int category, const int model);
	CSonyDatabaseCatModName* get(CString category, CString model);
};

//------------------------------------------------------------------------------

class CSonyDatabase
{
private:
	CString m_db_location;
	CString m_db_password;

	CSonyDatabaseCatModNameArray m_catModNames;

	void loadCatModNames();
	void loadCatModNames(CString catnumname);
	void validateCatModNames();


public:
	CSonyDatabase(CString location, CString password);

	CString getLocation()	{ return m_db_location; }
	CString getPassword()	{ return m_db_password; }

	CString getBitmapFilename(int category, int model);
	void getFrameSize(int category, int model, int *xsize, int *ysize);
	CString getCategoryName(int category);
	CString getCategoryNameOptionList();  // returns "name1|name2|name3|name4...namen"
	int getCategoryNumber(CString catname);
	CString getModelNameOptionList(int category);
	int getModelNumber(int category, CString modelname);
	CString getModelName(int category, int model);
	bool isValidCatModCombo(int category, int model);
	bool isValidCatModCombo(CSonyDatabaseCatModName *cmname);

};

//------------------------------------------------------------------------------

class CSonyRsiXref
{
private:
	CString m_db_location;
	bool m_db_available;

public:
	CSonyRsiXref(CString databaseLocation);
	~CSonyRsiXref();

	bool get(CString partnumber, int *category, int *model, bool *explode, int *angle);
	void put(CString partnumber, int category, int model, bool explode, int angle);

	bool isAvailable()		{ return m_db_available; }
	CString getLocation()	{ return m_db_location;  }

};

#endif
