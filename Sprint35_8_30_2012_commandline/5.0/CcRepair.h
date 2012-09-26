// $Header: /CAMCAD/5.0/CcRepair.h 9     3/22/07 12:54a Kurt Van Ness $

//   Router Solutions Inc.
//   Copyright © 2002. All Rights Reserved.

#if ! defined (__CcRepair_h__)
#define __CcRepair_h__

#include <map>
#include "CcDoc.h"
#include "WriteFormat.h"

//_____________________________________________________________________________
class CBlockStructList : public CTypedPtrListContainer<BlockStruct*>
{
public:
   CBlockStructList(bool isContainer=true);
};

//_____________________________________________________________________________
class CCamCadDocRepairTool
{
private:
   CCEtoODBDoc& m_camCadDoc;
   int m_undefinedLayerIndex;
   BlockStruct* m_floatingUndefinedBlock;
   BlockStruct* m_undefinedBlock;
   CWriteFormat* m_verificationLogFile;
   CBlockStructList m_recursiveCheckStack;
   std::map<CString,CString> m_changedNameMap;
   int m_netNameKW;

public:
   CCamCadDocRepairTool(CCEtoODBDoc& camCadDoc);
   virtual ~CCamCadDocRepairTool();

   bool verifyAndRepairData();
   void setVerificationLogFilePath(const CString& logFilePath);

   BlockStruct& getUndefinedBlock();
   BlockStruct& getFloatingUndefinedBlock();
   LayerStruct& getUndefinedLayer();

   //changes duplicated geometry names to new ones (case insensitive compare used)
   void verifyGeometryNames();
   //changes duplicated geometry names back
   void undoVerifyGeometryNames();

private:
   bool checkRecursiveReference(BlockStruct& block);
   CWriteFormat& getVerificationLogFile();
	void verifyDeviceType();
   void verifyNetList();

   void updateTraceAndViaNetNames(BlockStruct *block, CString netName);
   void updateBlankNetNameAttrib(DataStruct *data, CString netName);
};

#endif
