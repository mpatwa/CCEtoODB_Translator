// $Header: /CAMCAD/4.6/Consolidator.h 5     4/26/07 10:18p Rick Faltersack $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#if !defined(_CONSOLIDATOR_H__INCLUDED_)
#define _CONSOLIDATOR_H__INCLUDED_

//
// CONTROLS
//
//===========================================================================================
//

class CGeometryConsolidatorControl
{
	// This class contains controls to customize consolidator behavior.
	// Settings for Consolidator (currently) are found in individual CAD Importer ".in" settings
	// files, for which each importer has its own reader/parser implementation. This
	// class makes it easy to add and maintain consistent format and parsing, across all the
	// various importers. In essence, when something is to be added or changed re settings
	// commands in .in files, the changes only need be done in this class, and all users
	// of this class automatically get the new stuff.

private:
	bool m_enabled;        // On/off switch for Consolidator

	bool m_insertsOnly;    // When true, turns off consideration of polylines during geom comparison.
	                       // This is what the PINS mode command currently maps to.

   bool m_useOriginalName; // When true, turns on matching of OriginalName fields in geomemtries.
                           // When false, geometry comparison looks at geometry content only.

   double m_tolerance;     // For floating point comparisons. Might eventually want to separate
                           // tolerance for location from tolerance for sizes.

public:
	CGeometryConsolidatorControl()		
   { m_enabled = true; m_insertsOnly = false; m_useOriginalName = true; m_tolerance = 1.0E-5; }

	bool GetIsEnabled()		   { return m_enabled; }
	bool GetInsertsOnly()	   { return m_insertsOnly; }
   bool GetUseOriginalName()  { return m_useOriginalName; }
   double GetTolerance()      { return m_tolerance; }

	void SetIsEnabled(bool f)	      { m_enabled = f; }
	void SetInsertsOnly(bool f)	   { m_insertsOnly = f; }
   void SetUseOriginalName(bool f)	{ m_useOriginalName = f; }
   void SetTolerance(double f)      { m_tolerance = f; }

	bool Parse(CString commandline);  // Parses line of text, returns true if this line is a Consolidator command,
									  // returns false if it is not. This makes it possible to use Parse() in the
	                                  // typical if-elseif-elseif parsing code found in most importers.
									  // Caller should pass the entire command line as found in settings file.
	                                  // Saves/updates settings that are found.
			

};

//
//===========================================================================================


//
// CONSOLIDATOR
//
//===========================================================================================
//
// Originally I had the isEquivalent and hasEquivalent functionality in the DataStruct
// and CDataList classes. But these are not full fledged general purpose implementations.
// In particular, the DataStruct::isEquivalent is tailored to the Geometry Consolidator
// by taking advantage of the knowledge that lower levels in the data hiearchy were
// consolidated first. E.g. the process first handles apertures and then padstacks and
// then components. So to compare the padstack a compnent's pin inserts does not required 
// drilling into the padstack data. At this point in processing if they are the same then 
// they will insert the same block, and so that is all that is checked. A general purpose
// implementation should drill down into the inserted blocks and compare their own
// data.

class CDSWrapper
{
private:
   DataStruct *m_data;
   CCEtoODBDoc *m_doc;

public:
   CDSWrapper(DataStruct *data, CCEtoODBDoc *doc) { m_data = data; m_doc = doc;}
   bool isEquivalent(DataStruct *otherData, double tolerance);
};

class CDLWrapper
{
private:
   CDataList *m_datalist;
   CCEtoODBDoc *m_doc;

public:
   CDLWrapper(CDataList *datalist, CCEtoODBDoc *doc) { m_datalist = datalist; m_doc = doc;}
   bool hasEquivalent(DataStruct *otherData, double tolerance);
   bool isEquivalent(CDataList *otherList, bool insertsOnly, double tolerance);
   bool hasPins(CCEtoODBDoc *doc);
};

//===========================================================================================

class CApertureMap : public CMapWordToPtr
{
public:
   BlockStruct *FindEquivalent(CCEtoODBDoc *doc, BlockStruct *ap);
};

class CPadstackMap : public CMapWordToPtr
{
public:
   BlockStruct *FindEquivalent(BlockStruct *padstack, CCEtoODBDoc *doc, double tolerance);
};

class CGeometryMap : public CMapWordToPtr
{
public:
   BlockStruct *FindEquivalent(CCEtoODBDoc *doc, BlockStruct *component, bool insertsOnly, bool considerOriginalName, double tolerance);
};

//===========================================================================================

class CGeometryConsolidator
{
private:
   CCEtoODBDoc *m_doc;
   CString m_logFilename;
   CFormatStdioFile m_logFile;
   double m_tolerance;

   CGeometryConsolidatorControl *m_activeControl;

   CApertureMap m_allAperturesInDoc;
   CApertureMap m_uniqueApertures;     // block number and BlockStruct ptr for unique apertures
   CApertureMap m_duplicateApertures;  // block number of dup aperture, BlockStruct ptr for UNIQUE aperture replacement

   CPadstackMap m_allPadstacksInDoc; // ptr is BlockStruct referenced by an insertTypePin, int index is block number
   CPadstackMap m_uniquePadstacks;
   CPadstackMap m_duplicatePadstacks;

   CGeometryMap m_allComponentsInDoc;
   CGeometryMap m_uniqueComponents;
   CGeometryMap m_duplicateComponents;


   void CollectAllInsertRefnames(CMapStringToPtr &refnameMap);

   double MinimizedApInsertAngleDeg(double insertAngleDeg, ApertureShapeTag shapeTag, double sizeA, double sizeB);

   void CollectAllApertures();
   void NormalizeApertureRotation();
   void NormalizeCircleSquareRectangleOblongApertures();
   void SimplifyComplexApertures();
   void ClassifyApertures();
   void CollectAllPadStacks();
   void ConsolidateApertureUsageInPadstacks();
   void ClassifyPadstacks();
   void ConsolidatePadstackUsageInComponents();
   void CollectAllComponentGeometries();
   void ClassifyComponentGeometries();
   void ConsolidateComponentGeometryUsage();
   void UpdateCompPins();
   void ScrubNames();

   void CleanWidthTable();

   void ReportConsolidation();


public:
   CGeometryConsolidator(CCEtoODBDoc *doc, CString logFilename) { m_doc = doc; m_logFilename = logFilename; m_tolerance = 1.0E-5; }
   void Go(CGeometryConsolidatorControl *controls = NULL); // Do whole consolidation job, controls to mod behavior are optional
};



#endif