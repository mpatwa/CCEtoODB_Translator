// $Header: /CAMCAD/4.5/Api.h 53    10/06/06 5:31p Moustafa Noureddine $
 
/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-97. All Rights Reserved.
*/           

#if !defined(AFX_API_H__C3DB3A30_0B71_11D2_BA40_0080ADB36DBB__INCLUDED_)
#define AFX_API_H__C3DB3A30_0B71_11D2_BA40_0080ADB36DBB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// RETURN CODES
#define RC_SUCCESS                  0
#define RC_TOO_MANY_ERRORS          1
#define RC_ITEM_ALREADY_EXISTS      2
#define RC_GENERAL_ERROR            -1
#define RC_NO_ACTIVE_DOC            -2
#define RC_ITEM_NOT_FOUND           -3
#define RC_ILLEGAL_POSITION         -4
#define RC_NO_ATTRIBS               -5
#define RC_WRONG_TYPE               -6
#define RC_NOTHING_SELECTED         -7
#define RC_INDEX_OUT_OF_RANGE       -8
#define RC_EMPTY_ARRAY              -9
#define RC_NO_LICENSE               -10
#define RC_UNINITIALIZED_VARIABLES  -11
#define RC_NOT_ALLOWED              -12
#define RC_VALUE_OUT_OF_RANGE       -13
#define RC_HOLE_IN_ARRAY            -14
#define RC_DFM_TEST_FAILED          -15
#define RC_MISSING_DLL              -16
#define RC_FILE_NOT_FOUND           -17
#define RC_UNKNOWN_FORMAT           -18
#define RC_INVALID_FORMAT				-19

BOOL APILevelAllowed(int level);

/////////////////////////////////////////////////////////////////////////////
// API command target
class API : public CCmdTarget
{
   DECLARE_DYNCREATE(API)

   API();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(API)
   public:
   virtual void OnFinalRelease();
   //}}AFX_VIRTUAL

// Implementation
   virtual ~API();

   // Generated message map functions
   //{{AFX_MSG(API)
      // NOTE - the ClassWizard will add and remove member functions here.
   //}}AFX_MSG

   DECLARE_MESSAGE_MAP()
   DECLARE_OLECREATE(API)

   // Generated OLE dispatch map functions
   //{{AFX_DISPATCH(API)
   afx_msg short GraphAperture(LPCTSTR name, short shape, double sizeA, double sizeB, double xOffset, double yOffset, double rotation, short dcode);
   afx_msg short OverrideColorByRefDes(LPCTSTR RefDes, BOOL On, short red, short green, short blue);
   afx_msg short OverrideColorRemoveAll();
   afx_msg short OverrideColorByAttribute(LPCTSTR keyword, LPCTSTR value, BOOL On, short red, short green, short blue);
   afx_msg short Exit();
   afx_msg short ZoomLevel(double scaleDenom, short scaleNum);
   afx_msg short ZoomWindow(double x1, double y1, double x2, double y2);
   afx_msg short PanRef(LPCTSTR refdes);
   afx_msg short PanCoord(double x, double y);
   afx_msg short Redraw();
   afx_msg short LayerColor(LPCTSTR name, short red, short green, short blue);
   afx_msg short LayerShow(LPCTSTR name, BOOL show);
   afx_msg short LayerShowAll(BOOL show);
   afx_msg short TopView();
   afx_msg short BottomView();
   afx_msg short ColorSet(short set);
   afx_msg short ClearSelected();
   afx_msg short ZoomExtentsOfComponents(LPCTSTR compNames);
   afx_msg short CloseActiveDocument();
   afx_msg short SetAttribByRefDes(LPCTSTR keyword, LPCTSTR value, short method, LPCTSTR refDes);
   afx_msg short ShadeByRefDes(LPCTSTR refDes, BOOL On, short red, short green, short blue, BOOL Before, BOOL Transparent, BOOL Hatched);
   afx_msg short RemoveShading();
   afx_msg short ShowActiveDocument(short mode);
   afx_msg short GraphTool(LPCTSTR name, float size, short tcode, short type);
   afx_msg short ZoomExtentsOfComponentsMargin(LPCTSTR compNames, short margin);
   afx_msg short TurnOnResponse(LPCTSTR programID);
   afx_msg short TurnOffResponse();
   afx_msg short HighlightNet(LPCTSTR netName, short red, short green, short blue);
   afx_msg short HighlightNetClear(LPCTSTR netName);
   afx_msg short HighlightNetClearAll();
   afx_msg short ZoomExtentsOfNets(LPCTSTR netNames);
   afx_msg short ZoomExtentsOfNetsMargin(LPCTSTR netNames, short margin);
   afx_msg short HighlightPin(LPCTSTR comp, LPCTSTR pin, short red, short green, short blue);
   afx_msg short ShowMainWindow(short mode);
   afx_msg short SizeMainWindow(short x, short y, short width, short height);
   afx_msg short GetFileArray(VARIANT FAR* fileArray);
   afx_msg short GetNetArray(long filePosition, VARIANT FAR* netArray);
   afx_msg short GetNet(long filePosition, long netPosition, BSTR FAR* netname);
   afx_msg short GetCompPinArray(long filePosition, long netPosition, VARIANT FAR* compPinArray);
   afx_msg short GetCompPin(long filePosition, long netPosition, long compPinPosition, BSTR FAR* compName, BSTR FAR* pinName, float FAR* x, float FAR* y);
   afx_msg short GeneratePinLocs();
   afx_msg short GetKeyword(short keyword, BSTR FAR* InKeyword, BSTR FAR* CCKeyword, BSTR FAR* OutKeyword, short FAR* type, BOOL FAR* internal);
   afx_msg short GetAttribKeywordArray(VARIANT FAR* keywordArray);
   afx_msg short GetAttribValue(short keyword, BSTR FAR* value);
   afx_msg short SetNetCurrentAttrib(long filePosition, long netPosition);
   afx_msg short SetCompPinCurrentAttrib(long filePosition, long netPosition, long compPinPosition);
   afx_msg short GetGeometry(short geometryNumber, BSTR FAR* name, short FAR* geometryType, short FAR* geometryIsA);
   afx_msg short GetDataArray(short geometryNumber, VARIANT FAR* dataArray);
   afx_msg short GetData(short geometryNumber, long dataPosition, short FAR* dataIsA, short FAR* graphicClass, short FAR* layer);
   afx_msg short GetDataInsert(short geometryNumber, long dataPosition, short FAR* insertedGeometryNumber, float FAR* x, float FAR* y, float FAR* scale, float FAR* rotation, short FAR* mirror, short FAR* insertType, BSTR FAR* referenceName);
   afx_msg short SetDataCurrentAttrib(short geometryNumber, long dataPosition);
   afx_msg short SetGeometryCurrentAttrib(short geometryNumber);
   afx_msg short GetDataArrayByType(short geometryNumber, short dataType, VARIANT FAR* dataArray);
   afx_msg short GetDataArrayByGraphicClass(short geometryNumber, short graphicClass, VARIANT FAR* dataArray);
   afx_msg short GetDataArrayByInsertType(short geometryNumber, short insertType, VARIANT FAR* dataArray);
   afx_msg short GetDataPolyStruct(short geometryNumber, long dataPosition, VARIANT FAR* polyArray);
   afx_msg short GetDataPoly(short geometryNumber, long dataPosition, long polyPosition, short FAR* widthIndex, BOOL FAR* filled, BOOL FAR* closed, BOOL FAR* voidPoly, VARIANT FAR* pointArray);
   afx_msg short GetDataPolyPoint(short geometryNumber, long dataPosition, long polyPosition, long pointPosition, float FAR* x, float FAR* y, float FAR* bulgeNumber);
   afx_msg short DrawCross(double x, double y);
   afx_msg short GetKeywordIndex(LPCTSTR keywordString, short FAR* keyword);
   afx_msg short SetSelectedDataCurrentAttrib();
   afx_msg short SetAttribValue(short keyword, LPCTSTR value, short overwriteMethod);
   afx_msg short SetAttribValueOnSelected(short keyword, LPCTSTR value, short overwriteMethod);
   afx_msg short GetGeometryArrayByType(short geometryType, VARIANT FAR* geometryArray);
   afx_msg short GetDocumentArray(VARIANT FAR* documentArray);
   afx_msg short GetDocument(long documentPosition, BSTR FAR* name);
   afx_msg short ActivateDocument(long documentPosition);
   afx_msg short GetDRCArray(long filePosition, VARIANT FAR* drcArray);
   afx_msg short GetDRC(long filePosition, long drcPosition, BSTR FAR* string, double FAR* x, double FAR* y, short FAR* failureRange, short FAR* algorithmIndex, short FAR* algorithmType, BSTR FAR* comment, short FAR* drcClass);
   afx_msg short DeleteDRC(long entityNumber);
   afx_msg short GetActiveFile(long FAR* filePosition);
   afx_msg short AddDrcMarker(long filePosition, float x, float y, LPCTSTR layer, LPCTSTR comment, long FAR* entityNumber);
   afx_msg short LockCAMCAD(short lockCodes);
   afx_msg short LoadColorSet(LPCTSTR filename);
   afx_msg short CurrentSettings(short fileNumber, short layerNumber);
   afx_msg short GetLayerArrayByType(short layerType, VARIANT FAR* layerArray);
   afx_msg short SetLayerCurrentAttrib(short layerNumber);
   afx_msg short GetViewSurface(short FAR* surface);
   afx_msg short GetViewportCoords(double FAR* left, double FAR* right, double FAR* top, double FAR* bottom);
   afx_msg short GetFile(long filePosition, BSTR FAR* name, short FAR* number, float FAR* x, float FAR* y, float FAR* scale, float FAR* angle, short FAR* mirror, short FAR* type, short FAR* geometryNumber);
   afx_msg short GetNetCompPinData(long filePosition, VARIANT FAR* array);
   afx_msg short GetNetViaData(long filePosition, VARIANT FAR* array);
   afx_msg short GetComponentData(long filePosition, VARIANT FAR* array);
   afx_msg short GetNetTraceData(long filePosition, VARIANT FAR* array);
   afx_msg short LoadDataFile(LPCTSTR filename);
   afx_msg short FindVia(LPCTSTR NetName, double x, double y, double delta, short FAR* geometryNumber, long FAR* dataPosition);
   afx_msg short IssueCommand(long command);
   afx_msg short ShowInsertType(short insertType, BOOL On);
   afx_msg short ShowGraphicClass(short graphicClass, BOOL On);
   afx_msg short HighlightPinClear(LPCTSTR comp, LPCTSTR pin);
   afx_msg short HighlightPinClearAll();
   afx_msg short GetDocumentExtents(double FAR* left, double FAR* right, double FAR* top, double FAR* bottom);
   afx_msg short OverrideColorDRC(long entityNumber, BOOL On, short red, short green, short blue);
   afx_msg short FindDRC(long entityNumber, long FAR* filePosition, long FAR* drcPosition);
   afx_msg short GetNetTraceDataByNetname(long filePosition, LPCTSTR netName, VARIANT FAR* array);
   afx_msg short GetAPIVersion(short FAR* majorVersionNumber, short FAR* minorVersionNumber);
   afx_msg short OverrideColorData(short geometryNumber, long dataPosition, BOOL On, short red, short green, short blue);
   //afx_msg short Import(short fileType, LPCTSTR filenames, LPCTSTR formatString);
   afx_msg short Blank1();
   afx_msg short ListConstants(short list, VARIANT FAR* array);
   afx_msg short AutoRedraw(BOOL On);
   afx_msg short GetZoomLevel(double FAR* zoomLevel);
   afx_msg short SetZoomLevel(double zoomLevel);
   afx_msg short SetLayerType(short layerNumber, short layerType);
   afx_msg short SetLayerShow(short layerNumber, short show);
   afx_msg short SetLayerMirror(short layerNumber, short mirrorLayerNumber);
   afx_msg short SetLayerColor(short layerNumber, short red, short green, short blue);
   afx_msg short FindLayer(LPCTSTR layerName, short FAR* layerNumber);
   afx_msg short SetFileShow(long filePosition, BOOL show);
   afx_msg short GetPageUnits(BSTR FAR* units);
   afx_msg short SetDataInsertInserttype(short geometryNumber, long dataPosition, short inserttype);
   afx_msg short FindNet(long filePosition, LPCTSTR netname, long FAR* netPosition);
   afx_msg short GetPlacementData(long filePosition, short insertType, VARIANT FAR* array);
   afx_msg short GetActiveDocument(long FAR* documentPosition);
   afx_msg short GetFile2(long filePosition, BSTR FAR* name, short FAR* number, BOOL FAR* On, float FAR* x, float FAR* y, float FAR* scale, float FAR* angle, short FAR* mirror, short FAR* type, short FAR* geometryNumber);
   afx_msg short GetLayer2(short layerNumber, BSTR FAR* name, short FAR* layerType, short FAR* show, short FAR* mirrorLayerNumber, BSTR FAR* comment, short FAR* red, short FAR* green, short FAR* blue);
   afx_msg short GetLayer(short layerNumber, BSTR FAR* name, short FAR* layerType, short FAR* show, BSTR FAR* comment);
   afx_msg short SetLayerWorldView(short layerNumber, BOOL show);
   afx_msg short GetLayer3(short layerNumber, BSTR FAR* name, short FAR* layerType, short FAR* show, short FAR* mirrorLayerNumber, BSTR FAR* comment, short FAR* red, short FAR* green, short FAR* blue, short FAR* worldView);
   afx_msg short HideData(short geometryNumber, long dataPosition, BOOL Hide);
   afx_msg short HideAll(BOOL Hide);
   afx_msg short FindDataByRefname(LPCTSTR refname, short FAR* geometryNumber, long FAR* dataPosition);
   afx_msg short HideInserts(short inserttype, BOOL Hide);
   afx_msg short HideNetsAll(BOOL Hide);
   afx_msg short HideNet(LPCTSTR netname, BOOL Hide);
   afx_msg short SetPageSize(double xmin, double xmax, double ymin, double ymax);
   afx_msg short SaveDataFileAs(LPCTSTR filename);
   afx_msg short SetAPILevel(short code);
   afx_msg short GetGeometryData(short geometryType, VARIANT FAR* array);
   afx_msg short SetFileRotation(long filePosition, double rotation);
   afx_msg short UserCoord();
   afx_msg short TurnOffResponseByID(LPCTSTR programID);
   afx_msg short GenerateMarker(short shape, double sizeA, double sizeB, double penWidth);
   afx_msg short APIDocument(long documentPosition);
   afx_msg short NetLengthPoly(short geometryNumber, long dataPosition, double FAR* length);
   afx_msg short NetLengthSegment(double x1, double y1, double x2, double y2, double bulge, double FAR* length);
   afx_msg short NetLengthNet(long filePosition, LPCTSTR NetName, double FAR* length);
   afx_msg short NetLengthPins(long filePosition, LPCTSTR comp1, LPCTSTR pin1, LPCTSTR comp2, LPCTSTR pin2, double FAR* length);
   afx_msg short LayersUsed(VARIANT FAR* array);
   afx_msg short GetSelectedArray(VARIANT FAR* selectedArray);
   afx_msg short GetSelected(long selectedPosition, short FAR* geometryNumber, long FAR* dataPosition);
   afx_msg short GetSubSelectLevel(short FAR* subSelectLevel);
   afx_msg short GetSubSelect(short subSelectLevel, short FAR* geometryNumber, long FAR* dataPosition, float FAR* insert_x, float FAR* insert_y, float FAR* scale, float FAR* rotation, short FAR* mirror, short FAR* layer);
   afx_msg short NetLengthNetManhattan(long filePosition, LPCTSTR NetName, double FAR* length);
   afx_msg short NetLengthPinsManhattan(long filePosition, LPCTSTR comp1, LPCTSTR pin1, LPCTSTR comp2, LPCTSTR pin2, double FAR* length);
   afx_msg short GetNetData(long filePosition, VARIANT FAR* array);
   afx_msg short HideDrawings(BOOL Hide);
   afx_msg short GetFilePlaced(long filePosition, BOOL FAR* placed);
   afx_msg short HighlightPinsOfANet(LPCTSTR netName, short red, short green, short blue);
   afx_msg short HighlightPinsOfANetClear(LPCTSTR netName);
   afx_msg short OverrideColorByInsertType(short insertType, BOOL On, short red, short green, short blue);
   afx_msg short LayersUsedByNet(long filePosition, LPCTSTR netName, VARIANT FAR* array);
   afx_msg short Export(short fileType, LPCTSTR filename);
   afx_msg short OverrideColorByGeometry(short geometryNumber, BOOL On, short red, short green, short blue);
   afx_msg short TempAddInsert(long filePosition, short geometryNumber, LPCTSTR refName, short insertType, double x, double y, double rotation, short mirror, short layerNumber, long FAR* dataPosition);
   afx_msg short SetGeometryName(short geometryNumber, LPCTSTR name);
   afx_msg short dfmFlatten(short geometryNumber, short layerNumber, short textMode);
   afx_msg short SetFileGeometry(long filePosition, short geometryNumber);
   afx_msg short dfmDistanceCheck(short geometryNumber, double acceptableDistance, double marginalDistance, double criticalDistance, LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch);
   afx_msg short License(short licenseNumber, long accessCode);
   afx_msg short dfmDistanceCheckDiffNets(short geometryNumber, double acceptableDistance, double marginalDistance, double criticalDistance, LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch);
   afx_msg short dfmDistanceCheckByClass(short geometryNumber, short class1, short class2, double acceptableDistance, double marginalDistance, double criticalDistance, LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch);
   afx_msg short CreateAperture(LPCTSTR name, short shape, double sizeA, double sizeB, double xOffset, double yOffset, double rotation, short dcode, short FAR* widthIndex, short FAR* geometryNumber);
   afx_msg short CreateLayer(LPCTSTR name, short FAR* layerNumber);
   afx_msg short UserCoord2(BOOL On);
   afx_msg short GetLayerStackup(short layerNumber, short FAR* electrical, short FAR* physical, short FAR* artwork);
   afx_msg short dfmCreateDFxOutline(short geometryNumber, short algorithm, BOOL Overwrite);
   afx_msg short SetLayerVisible(short layerNumber, BOOL visible);
   afx_msg short SetLayerEditable(short layerNumber, BOOL editable);
   afx_msg short GetLayerVisible(short layerNumber, BOOL FAR* visible);
   afx_msg short GetLayerEditable(short layerNumber, BOOL FAR* editable);
   afx_msg short GetCentroidData(long filePosition, short insertType, VARIANT FAR* array);
   afx_msg short GetGeometryExtents(short geometryNumber, double FAR* left, double FAR* right, double FAR* top, double FAR* bottom);
   afx_msg short HideCAMCAD(BOOL Hide);
   afx_msg short SplashMessage(LPCTSTR message);
   afx_msg short SplashProgressRange(short range);
   afx_msg short SplashProgressPosition(short position);
   afx_msg short DeleteGeometry(short geometryNumber);
   afx_msg short dfmFlattenByInserttypes(short geometryNumber, short layerNumber, short inserttype1, short inserttype2);
   afx_msg short dfmComponentShadow(long filePosition, double accLeftMargin, double accRightMargin, double accTopMargin, double accBottomMargin, double marLeftMargin, double marRightMargin, double marTopMargin, double marBottomMargin, double crtLeftMargin, double crtRightMargin, double crtTopMargin, double crtBottomMargin);
   afx_msg short dfmDistanceCheckByInserttype(short geometryNumber, short inserttype1, short inserttype2, double acceptableDistance, double marginalDistance, double criticalDistance, LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch);
   afx_msg short dfmDistanceCheckClassInserttype(short geometryNumber, short class1, short inserttype2, double acceptableDistance, double marginalDistance, double criticalDistance, LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch);
   afx_msg short MakeMaufacturingRefDes();
   afx_msg short ExportDXF(LPCTSTR filename, double realWidth, BOOL explode, BOOL exportHidden, BOOL addLineEnds);
   afx_msg short CheckLicense(short licenseNumber, BOOL FAR* allowed);
   afx_msg short CheckImportFormatLicense(short format, BOOL FAR* allowed);
   afx_msg short dfmCreateFlattenGeometry(long filePosition, short FAR* geometryNumber);
   afx_msg short CreateKeyword(LPCTSTR name, short valueType, short group, short FAR* keyword);
   afx_msg short RemoveAttrib(short keyword);
   afx_msg short GetDRCAlgorithm(short algorithmNumber, BSTR FAR* algorithmName);
   afx_msg short FindData(long entityNumber, short FAR* geometryNumber, long FAR* dataPosition);
   afx_msg short GetDRCMeasure(long filePosition, long drcPosition, long FAR* entity1, short FAR* type1, double FAR* x1, double FAR* y1, long FAR* entity2, short FAR* type2, double FAR* x2, double FAR* y2);
   afx_msg short InitRealPartInfo();
   afx_msg short dfmDistanceCheckTwoNets(short geometryNumber, LPCTSTR netname1, LPCTSTR netname2, double acceptableDistance, double marginalDistance, double criticalDistance, LPCTSTR algName, short algType, LPCTSTR layerSuffix, BOOL allowAttribOverride, BOOL canTouch);
   afx_msg short dfmDistanceCheckByPackage(short geometryNumber, LPCTSTR packageKeyword, LPCTSTR package1, LPCTSTR package2, double acceptableDistance, double marginalDistance, double criticalDistance, LPCTSTR algName, short algType);
   afx_msg short dfmSetMaxErrorsForTest(short maxErrors);
   afx_msg short dfmHaltTest();
   afx_msg short GetTypeArray(long filePosition, VARIANT FAR* typeArray);
   afx_msg short GetType(long filePosition, long typePosition, BSTR FAR* name, short FAR* geometryNumber);
   afx_msg short SetTypeCurrentAttrib(long filePosition, long typePosition);
   afx_msg short dfmAcidTraps(short geometryNumber, double acceptableAngle, double marginalAngle, double criticalAngle, LPCTSTR layerSuffix);
   afx_msg short AddInsert(short geometryNumber, short insertedGeometryNumber, LPCTSTR refName, short insertType, double x, double y, double rotation, short mirror, short layerNumber, long FAR* dataPosition);
   afx_msg short AddPolyEntity(short geometryNumber, short layerNumber, long FAR* dataPosition);
   afx_msg short AddPoly(short geometryNumber, long dataPosition, short widthIndex, long FAR* polyPosition);
   afx_msg short AddPolyPnt(short geometryNumber, long dataPosition, long polyPosition, double x, double y, double bulge, long FAR* pntPosition);
   afx_msg short SetAttribVisible(short keyword, BOOL visible, double x, double y, double height, double width, double angle, short layer, short penWidthIndex, BOOL proportional, BOOL neverMirror);
   afx_msg short FindCompPin(long filePosition, long netPosition, LPCTSTR comp, LPCTSTR pin, long FAR* cpPosition);
   afx_msg short GetNetCompPinData2(long filePosition, VARIANT FAR* array);
   afx_msg short CopyFile(long filePosition, LPCTSTR newFileName, long FAR* newFilePosition);
   afx_msg short SetFileMirror(long filePosition, BOOL mirror);
   afx_msg short DeleteEntity(short geometryNumber, long dataPosition);
   afx_msg short RestructureFileAroundOrigin(long filePosition);
   afx_msg short GetPlacementData2(long filePosition, short insertType, VARIANT FAR* array);
   afx_msg short GetDRC2(long filePosition, long drcPosition, BSTR FAR* string, double FAR* x, double FAR* y, short FAR* failureRange, short FAR* algorithmIndex, short FAR* algorithmType, long FAR* insertEntity, BSTR FAR* comment, short FAR* drcClass);
   afx_msg short GetDRCNets(long filePosition, long drcPosition, VARIANT FAR* netnameArray);
   afx_msg short DeleteByLayer(short layerNumber, BOOL RemoveLayerFromLayerList);
   afx_msg short DeleteByInserttype(short geometryNumber, short inserttype);
   afx_msg short DeleteByGraphicClass(short geometryNumber, short graphicClass);
   afx_msg short SplashMin();
   afx_msg short SetFileLocation(long filePosition, double x, double y);
   afx_msg short AutoBoardOrigin(long filePosition, BOOL Left, BOOL Bottom);
   afx_msg short ClearAllDRCs();
   afx_msg short OEMLicense(LPCTSTR OEMName, short licenseNumber, long accessCode);
   afx_msg short SaveCompressedDataFileAs(LPCTSTR filename);
   //}}AFX_DISPATCH
   DECLARE_DISPATCH_MAP()
   DECLARE_INTERFACE_MAP()
   short SetCentroid(short geometryNumber, float x, float y, float rotation, short overwrite);
   short AutoGenerateCentroid(short geometryNumber, short method, short overwrite);
   short GetCentroid(short geometryNumber, float FAR* x, float FAR* y, float FAR* rotation);
   short ExportGENCAD(LPCTSTR filename);
   afx_msg short GetSchematicViewArray(VARIANT FAR* fileArray);
   afx_msg short GetSchematicSheetArray(long filePosition, VARIANT FAR* sheetArray);
   afx_msg short GetLogicSymbolPlacementData(short geometryNumber, VARIANT FAR* array);
   afx_msg short GetSchematicNetData(long filePosition, VARIANT FAR* array);
   afx_msg short SetDataPolyFilled(short geometryNumber, long dataPosition, long polyPosition, short filled);
   afx_msg short SetDataPolyClosed(short geometryNumber, long dataPosition, long polyPosition, short closed);
   afx_msg short SetDataPolyVoid(short geometryNumber, long dataPosition, long polyPosition, short voidPoly);
   afx_msg short SetDataPolyWidthIndex(short geometryNumber, long dataPosition, long polyPosition, short widthIndex);
   afx_msg short SetDataGraphicClass(short geometryNumber, long dataPosition, short graphicClass);
   afx_msg short EditGeometry(short geometryNumber);
   afx_msg short OpenCadFile(LPCTSTR filename);
   afx_msg short SetSchematicNetCrossProbe(BOOL On);
   afx_msg short SetSchematicComponentCrossProbe(BOOL On);
   afx_msg short SetSchematicSheet(long filePosition, short geometryNumber);
   afx_msg short LoadSchematicNetCrossReference(LPCTSTR filename);
   afx_msg short LoadSchematicComponentCrossReference(LPCTSTR filename);
   afx_msg short CrossProbeSchematicComponent(LPCTSTR compName);
   afx_msg short CrossProbeSchematicNet(LPCTSTR netName);
   afx_msg short GetCAMCADVersion(BSTR FAR* ccVersion);
   afx_msg short DeleteGenericGraphic(BOOL components, BOOL graphics);
   afx_msg short DeleteTracesVias(short traces, short vias);
   afx_msg short OpenNewDocument();
   afx_msg short LoadSTDLicenses();
   afx_msg short SetWindowName(LPCTSTR name);
   afx_msg short SendLayerToBack(short layerNumber);
   afx_msg short BringLayerToFront(short layerNumber);
   afx_msg short SetWindowIcon(LPCTSTR iconFilename, short icon);
   afx_msg short GetRealPartData(BSTR FAR* data);
   afx_msg short SetRealPartData(LPCTSTR data);
   afx_msg short GenerateCompCentroid(long filePosition, short method);
   afx_msg short GetNetCompPinData3(long filePosition, VARIANT FAR* array);
   afx_msg short ExportFormat(short fileType, LPCTSTR filename, LPCTSTR formatString);
   afx_msg short GetProbeData(long filePosition, VARIANT FAR* array);
   afx_msg short SortPanelBoards(long filePosition, short startPoint, short sortStyle, short tolerancePercentage);
   afx_msg short LockDocument(long documentPosition, short lockDoc, LPCTSTR userName);
	afx_msg short GetDataExtents(short geometryNumber, long dataPosition, double FAR* xMin, double FAR* xMax, double FAR* yMin, double FAR* yMax);
   afx_msg short AddRealPartDrcMarker(long filePosition, LPCTSTR refDes, LPCTSTR packageName, LPCTSTR algorithmName, float x, float y, short failureRange, short priority, LPCTSTR comment, long FAR* entityNumber);
   afx_msg short RemoveRealPartDrcMarker(long filePosition, LPCTSTR refDes);
	afx_msg short DeleteOccuranceOfKeyword(short keyword);
	afx_msg short GetProjectPathAPI(BSTR FAR* ProjectPath);

   afx_msg short Validate(long nLicenseToken, long FAR* pKey);
};
 

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_API_H__C3DB3A30_0B71_11D2_BA40_0080ADB36DBB__INCLUDED_)
