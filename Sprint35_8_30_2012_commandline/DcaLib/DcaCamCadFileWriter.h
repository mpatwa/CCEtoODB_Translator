
#if !defined(__DcaCamCadFileWriter_h__)
#define __DcaCamCadFileWriter_h__

#pragma once

#include "DcaCamCadData.h"

// CAMCAD Doc version /////////////////////////////////////////////////////////////////////////////////
//#define DOC_VERSION "6.1"   // Change Edif delimiter from char(4) to char(32) [CAMCAD version = 4.2.18]
//#define DOC_VERSION "6.2"   // Added new structure to components for Real Part (RestructureCompData) [CAMCAD version = 4.3.0bl]
//#define DOC_VERSION "6.3"   // Added new insert types, graphic classes, layer types, and block types (RealPart types)
//#define DOC_VERSION "6.4"   // Added new DFT section [CAMCAD version = 4.4.0br]
//#define DOC_VERSION "7.0"   // Changed Exposed metal diameter attribute from a double to a unit-double
// Needed to make a major version change b/c previous versions of CAMCAD on check the major (changed to now check both)
#define DOC_VERSION "7.1"  // Added section <PPSolution></PPSolution> and <MultipleMachines></MultipleMachines> to CC file

class CDcaOperationProgress;
class CCamCadFileWriteProgress;
class CPntList;
class CVariant;
class CVariantItem;
class CDcaTestPlan;
class CDcaAccessAnalysisSolution;
class CDcaProbePlacementSolution;

//_____________________________________________________________________________
class CDcaCamCadFileWriter
{
private:
   CCamCadData& m_camCadData;

public:
   CDcaCamCadFileWriter(CCamCadData& camCadData);
   virtual ~CDcaCamCadFileWriter();

   CCamCadData& getCamCadData() { return m_camCadData; }

   void write(CWriteFormat& writeFormat,const CString& applicationName,const CString& applicationTitle,CDcaOperationProgress* operationProgress=NULL);

   static CString convertHtmlMetaCharacters(const CString& string);
   static CString floatingPointNumberString(double number);
   static int getWholenumCnt(double number);
   static void writeBufferedString(CWriteFormat& writeFormat,const CString& string);

private:
   virtual void writeXml(CWriteFormat& writeFormat,const CKeyWordArray&   keyWordArray,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const KeyWordStruct&  keyWordStruct,CCamCadFileWriteProgress& progress);

   virtual void writeXml(CWriteFormat& writeFormat,const CLayerArray&       layerArray,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const LayerStruct&      layerStruct,CCamCadFileWriteProgress& progress);

   virtual void writeXml(CWriteFormat& writeFormat,const CBlockArray&       blockArray,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const BlockStruct&      blockStruct,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDataList&           dataList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const DataStruct&        dataStruct,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CPolyList&           polyList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CPoly&                   poly,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CPntList&             pntList,CCamCadFileWriteProgress& progress);

   virtual void writeXml(CWriteFormat& writeFormat,const CFileList&           fileList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const FileStruct&        fileStruct,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CNetList&             netList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CCompPinList&     compPinList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CompPinStruct&  compPinStruct,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const NetStruct&          netStruct,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CBusList&             busList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CBusStruct&         busStruct,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CVariantList&     variantList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CVariant&             variant,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CVariantItem&     variantItem,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDeviceTypeDirectory& typeList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const TypeStruct&        typeStruct,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDRCList&             drcList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const DRCStruct&          drcStruct,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDcaDftSolutionList& dftSolutionList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDcaDftSolution&  dftSolution,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDcaTestPlan&        testPlan,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDcaAccessAnalysisSolution& accessAnalysisSolution,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDcaProbePlacementSolution& probePlacementSolution,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CDcaMachineList&  machineList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CFontProperties&    fontProps,CCamCadFileWriteProgress& progress);

   virtual void writeXml(CWriteFormat& writeFormat,const CGTabTableList& tableList, CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CGTabTable& table, CCamCadFileWriteProgress& progress);

   virtual void writeXml(CWriteFormat& writeFormat,const CNamedViewList& namedViewList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CNamedView&         namedView,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CViewLayerDataArray& viewLayerDataArray,CCamCadFileWriteProgress& progress);

   virtual void writeXml(CWriteFormat& writeFormat,const CAttributes&       attributes,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CViaSpansSection* ViaSpansSection,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CTopicsList&       topiclist,CCamCadFileWriteProgress& progress);
   virtual void writeExtendedXmlAttributes(CWriteFormat& writeFormat,const FileStruct&        fileStruct,CCamCadFileWriteProgress& progress);
   virtual void writeExtendedXml          (CWriteFormat& writeFormat,const FileStruct&        fileStruct,CCamCadFileWriteProgress& progress);

   virtual void writeXml(CWriteFormat& writeFormat,const CPoly3DList&           polyList,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CPoly3D&               poly,CCamCadFileWriteProgress& progress);
   virtual void writeXml(CWriteFormat& writeFormat,const CPnt3DList&             pntList,CCamCadFileWriteProgress& progress);
};

//_____________________________________________________________________________
class CCamCadFileWriteProgress : public CObject
{
private:
   CCamCadData& m_camCadData;
   CDcaOperationProgress* m_allocatedProgress;
   CDcaOperationProgress* m_progress;

   CString m_status;

   int m_keyWordCount;
   int m_layerCount;
   int m_blockCount;
   int m_dataCount;
   int m_fileCount;
   int m_algorithmCount;
   int m_widthCount;

   int m_keyWordCapacity;
   int m_layerCapacity;
   int m_blockCapacity;
   int m_dataCapacity;
   int m_fileCapacity;
   int m_algorithmCapacity;
   int m_widthCapacity;

   int m_blockUpdateInterval;
   int m_fileUpdateInterval;

public:
   CCamCadFileWriteProgress(CCamCadData& camCadData,bool showProgressFlag=true);
   CCamCadFileWriteProgress(CCamCadData& camCadData,CDcaOperationProgress* operationProgress);
   ~CCamCadFileWriteProgress();

   void init();
   void setStatus(const CString& status);
   void incKeyWord();
   void incLayer();
   void incBlock();
   void incData();
   void incFile();
   void incNet();
   void incBus();
   void incVariant();
   void incType();
   void incDrc();
   void incAlgorithm();
   void incWidth();

   void updateProgressStatus(const CString& status);
   void setProgressLength(double length);
   double updateProgress(double position);
};

#endif
