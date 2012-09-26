// $Header: /CAMCAD/4.6/read_wrt/ZukenCr5000Reader.h 76    6/04/07 6:21p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2006. All Rights Reserved.
*/

#if ! defined (__ZukenCr5000Reader_h__)
#define __ZukenCr5000Reader_h__

#pragma once

#include "AlienDatabase.h"
#include "RwLib.h"
#include "QfeLib.h"
#include "Units.h"
#include "RegularExpression.h"
#include "Consolidator.h"


#define EnableNewZukenReader


#define LineStatusUpdateInterval 200
#define PrefixGet 1
#define PrefixUnget 2
//#define HtmlDebugOutput

//_____________________________________________________________________________
#define tok(a) tok##a , ptok##a ,
#define tok2(a,b) tok##b ,

enum Cr5kTokenTag
{
   #include "ZukenCr5000ReaderTokens.h"

   tok_Eof,
   tok_Cr5kTokenTagUndefined
};

#undef tok
#undef tok2

Cr5kTokenTag stringToCr5kToken(CString tokenString);
Cr5kTokenTag paramToCr5kToken(const CStringArray& params,int index);

class CZukenCr5000Reader;
class CCr5kElement;
class CCr5kParameters;
class CCr5kProperties;
class CCr5kReferPrimitive;

class CCr5kLinkedPads;
class CCr5kLinkedPadstacks;
class CCr5kLinkedFootprints;
class CCr5kLinkedComponents;
class CCr5kPointVertex;
class CCr5kArcVertex;

//_____________________________________________________________________________
enum PolyTypeTag
{
   polyTypeOpen,
   polyTypeClosed,
   polyTypeFilled,  // closed and filled
   polyTypeVoid,    // closed, filled, and void
   polyTypeUndefined,
};

//_____________________________________________________________________________
enum TokenMatchCountTypeTag
{
   matchCountOne,
   matchCountOneOrMore,
   matchCountZeroOrMore,
   matchCountZeroOrOne,
   matchCountUndefined
};

//_____________________________________________________________________________
enum Cr5kSourceFileTypeTag
{
   sourceFileTypeFtf,
   sourceFileTypePcf,
   sourceFileTypePnf,
   sourceFileTypeUndefined
};

CString cr5kSourceFileTypeTagToString(Cr5kSourceFileTypeTag tagValue);

class CBoolStack
{
private:
   CArray<bool,bool> m_stack;

public:
   CBoolStack(int growBy = 10);
   void empty();

   void push(bool flag);
   bool pop();
   bool getTop() const;
   bool getAt(int index) const;
   int getSize() const;
   bool isEmpty() const;

};

//_____________________________________________________________________________
class CCr5kReadElement
{
private:
   CCr5kElement& m_element;
   Cr5kTokenTag m_token;
   TokenMatchCountTypeTag m_matchCountType;

public:
   CCr5kReadElement(CCr5kElement& element,Cr5kTokenTag token,TokenMatchCountTypeTag matchCountType);

   CCr5kElement& getElement() { return m_element; }
   Cr5kTokenTag getToken() { return m_token; }
   TokenMatchCountTypeTag getMatchCountType() { return m_matchCountType; }
};

//_____________________________________________________________________________
class CCr5kReadElementList
{
private:
   CTypedListContainer<CPtrList,CCr5kReadElement*> m_list;

public:
   CCr5kReadElementList();

   void add(CCr5kElement& element,Cr5kTokenTag token,TokenMatchCountTypeTag matchCountType);
   void addOne(CCr5kElement& element,Cr5kTokenTag token);
   void addZeroOrOne(CCr5kElement& element,Cr5kTokenTag token);
   //void addZeroOrMore(CCr5kElement& element,Cr5kTokenTag token);

   POSITION getHeadPosition() const { return m_list.GetHeadPosition(); }
   CCr5kReadElement* getNext(POSITION& pos) { return m_list.GetNext(pos); }
   bool isEmpty() const { return m_list.GetCount() == 0; }
   int getCount() const { return m_list.GetCount(); }
   void takeElement(CCr5kReadElementList& otherList,POSITION otherListPos);

private:
};

//_____________________________________________________________________________
class CCr5kTokenReaderToken
{
private:
   CString m_token;
   CString m_line;
   int m_linePosition;
   int m_lineNumber;
   ULONGLONG m_filePosition;
   CString m_functionName;

public:
   CCr5kTokenReaderToken();
   CCr5kTokenReaderToken(const CString& token,const CString& line,int linePosition,int lineNumber,ULONGLONG filePosition);

   CCr5kTokenReaderToken& operator=(const CCr5kTokenReaderToken& other);

   CString getToken() const { return m_token; }
   CString getLine() const { return m_line; }
   int getLinePosition() const { return m_linePosition; }
   int getLineNumber() const { return m_lineNumber; }
   ULONGLONG getFilePosition() const { return m_filePosition; }

   void set(const CString& token,const CString& line,int linePosition,int lineNumber,ULONGLONG filePosition);

   CString getFunctionName() const { return m_functionName; }
   void setFunctionName(const CString& functionName) { m_functionName = functionName; }
};

//_____________________________________________________________________________
class CCr5kTokenReaderTokenStack
{
private:
   CTypedPtrListContainer<CCr5kTokenReaderToken*> m_tokenStack;
   //CCr5kTokenReaderToken m_previousTop;
   enum { opPush, opPop, opReplaceTop, opUndefined } m_lastOperation;

public:
   CCr5kTokenReaderTokenStack();

   int getSize() const { return m_tokenStack.GetSize(); }
   void push(const CCr5kTokenReaderToken& currentReaderToken);
   void replaceTop(const CCr5kTokenReaderToken& currentReaderToken);
   void pop();
   void restoreLast(const CCr5kTokenReaderToken& previousReaderToken);
   CCr5kTokenReaderToken* getTop();

   void setFunctionName(const CString& functionName);

   void write(CWriteFormat& writeFormat);
};

//_____________________________________________________________________________
class CCr5kTokenMap
{
private:
   CMapStringToWord m_map;

public:
   CCr5kTokenMap();

   void init();

   bool lookup(CString tokenString,Cr5kTokenTag& tokenTag);
};

//_____________________________________________________________________________
class CCr5kTokenReaderException
{
private:
   CString m_reason;

public:
   CCr5kTokenReaderException(const CString& reason);

   CString getReason() const { return m_reason; }
};

//_____________________________________________________________________________
class CCr5kTokenReader
{
private:
   static CCr5kTokenMap* s_tokenMap;

   CZukenCr5000Reader& m_zukenReader;
   CString m_filePath;
   Cr5kSourceFileTypeTag m_sourceFileType;
   CFileReadProgress* m_readProgress;

   int m_lineNumber;
   CString m_token;
   CString m_rawToken;
   bool m_unTokenFlag;
   CStdioFile m_file;
   CCr5kTokenReaderTokenStack m_tokenStack;
   CString m_lineBuffer;
   int m_linePosition;
   bool m_eofFlag;
   CCr5kTokenReaderToken m_previousReaderToken;
   CCr5kTokenReaderToken m_currentReaderToken;
   CString m_functionName;
   int m_nextFunctionNameSetCount;
   int m_functionNameSetCount;
   bool m_optionWriteDebugInfo;
   bool m_optionMultiByteCharactersSupported;

   CUnits m_units;

public:
   CCr5kTokenReader(CZukenCr5000Reader& zukenReader,const CString& filePath);
	~CCr5kTokenReader();

   // properties
   void setSourceFileType(Cr5kSourceFileTypeTag sourceFileType) { m_sourceFileType = sourceFileType; }
   CZukenCr5000Reader& getZukenReader() { return m_zukenReader; }

   // token
   CString getNextTokenString();
   CString getNextRawTokenString();
   Cr5kTokenTag getNextTokenTag();
   CString getCurrentTokenString();
   CString getCurrentRawTokenString();
   Cr5kTokenTag getCurrentTokenTag();
   Cr5kTokenTag getDefinedCurrentTokenTag();
   bool getNextRequiredToken(Cr5kTokenTag token);
   bool getNextRequiredBoolean(bool& validValue);
   bool getNextRequiredInteger(int& validValue);
   bool getNextRequiredFloatingPoint(double& validValue);
   bool getNextRequiredNonListToken(CString& tokenString);
   bool getNextRequiredNonListDefinedToken(Cr5kTokenTag& token);
   bool getNextOptionalNonListToken();
   void ungetToken();
   bool getEofFlag() const { return m_eofFlag; }

   // units
   const CUnits& getUnits() const { return m_units; }
   void setUnits(const CUnits& units) { m_units = units; }
   void setUnits(Cr5kTokenTag unitsToken);

   // query
   bool isNonTerminatingToken();
   bool isNonListToken();
   static bool isBoolean(const CString& tokenString,bool& validValue);
   static bool isInteger(const CString& tokenString,int& validValue);
   static bool isFloatingPoint(const CString& tokenString,double& validValue);
   bool verifyBoolean(const CString& tokenString);
   int verifyInteger(const CString& tokenString);
   double verifyFloatingPoint(const CString& tokenString);
   bool verifyBoolean();
   int verifyInteger();
   double verifyFloatingPoint();

   bool getOptionWriteDebugInfo() const;

   // state
   int getLineNumber() const { return m_lineNumber; }

   // skip
   bool skipList();
   bool skipToEndOfList();
   bool skipExpectedEmptyRemainderOfList();  // any remaining elements are logged to error file

   // error processing
   void unexpectedTokenError(const char* functionName);
   void expectedTokenError(Cr5kTokenTag token);
   void expectedTokenError(const CString& tokenString);
   void expectedNonListTokenError(const CString& tokenString);
   void expectedKnownTokenError(const CString& tokenString);

   //bool skipListIfDuplicate(void* element,const char* elementDescription);
   //bool isNextTokenEndOfList();
   //bool readExpectedElement(CCr5kElement& element,Cr5kTokenTag token);
   bool readZeroOrOneElements(CCr5kElement& element,Cr5kTokenTag exepectedToken = tok_Cr5kTokenTagUndefined);
   bool readOneElement(CCr5kElement& element,Cr5kTokenTag expectedToken = tok_Cr5kTokenTagUndefined);

   // lists
   bool readOneFromList(CCr5kReadElementList& elementList);
   //bool readZeroOrOneEachFromList(CCr5kReadElementList& elementList);
   bool readList(CCr5kReadElementList& elementList);
   bool readElementsAndParameters(CCr5kReadElementList& elementList,CCr5kParameters& parameters);
   bool readElementsAndProperties(CCr5kReadElementList& elementList,CCr5kProperties& properties);

   // diagnostic support
   CWriteFormat& getLog() const;
   void throwException(const CString& message);
   CString getFunctionName() const;
   void setFunctionName(const CString& functionName);
   void resetFunctionName(const CString& functionName,int functionNameSetCount);
   int getFunctionNameSetCount() const;
   void setSourceLineNumber(CCr5kElement& element) const;

   void writeDebugLine(int prefix,int indentCount);

   static void releaseStaticData();

private:
   void getLine();
   bool isSupportedMultiByteCharacter(const char* p);

private:
   static CCr5kTokenMap& getTokenMap();

public:
   static Cr5kTokenTag stringToCr5kToken(CString tokenString);
   static CString cr5kTokenToString(Cr5kTokenTag token);
   static bool isNonTerminatingToken(Cr5kTokenTag token);
   static bool isNonListToken(const CString& tokenString);
};

//_____________________________________________________________________________
class CCr5kTokenReaderFunction
{
private:
   CCr5kTokenReader& m_tokenReader;
   CString m_previousFunctionName;
   int m_previousFunctionNameSetCount;

public:
   CCr5kTokenReaderFunction(CCr5kTokenReader& tokenReader,const CString functionName);
   ~CCr5kTokenReaderFunction();
};

//_____________________________________________________________________________
class CCr5kProperty
{
private:
   CString m_name;
   CString m_value;

public:
   CCr5kProperty(const CString& name,const CString& value);

   CString getName() const { return m_name; }

   CString getValue() const { return m_value; }
   void setValue(const CString& value);
};

//_____________________________________________________________________________
class CCr5kProperties
{
private:
   CTypedPtrMap<CMapStringToPtr,CString,CCr5kProperty*> m_properties;

public:
   CCr5kProperties();
   virtual ~CCr5kProperties();

   void empty();

   void setAt(const CString& name,const CString& value);
   CCr5kProperty* getAt(const CString& name);
   bool hasProperty(const CString& name);
   int getCount() const;

   POSITION getStartPosition() const { return m_properties.GetStartPosition(); }
   void getNextAssoc(POSITION& pos,CString& key,CCr5kProperty*& property) const
      { return m_properties.GetNextAssoc(pos,key,property); }

   bool read(CCr5kTokenReader& tokenReader);
};

//_____________________________________________________________________________
class CCr5kParameter
{
private:
   Cr5kTokenTag m_parameterKey;
   CString m_value;

public:
   CCr5kParameter(Cr5kTokenTag parameterKey,const CString& value);

   Cr5kTokenTag getKey() const { return m_parameterKey; }
   CString getValue() const { return m_value; }
   void setValue(const CString& value);
};

//_____________________________________________________________________________
class CCr5kParameters
{
private:
   CTypedPtrMap<CMapWordToPtr,WORD,CCr5kParameter*> m_parameters;

public:
   CCr5kParameters();
   virtual ~CCr5kParameters();

   void empty();

   void registerKey(Cr5kTokenTag key);
   void setAt(Cr5kTokenTag key,const CString& value);
   CCr5kParameter* getAt(Cr5kTokenTag key);
   bool hasKey(Cr5kTokenTag key) const;
   bool hasValueForKey(Cr5kTokenTag key) const;

   int read(CCr5kTokenReader& tokenReader);

   double       getDoubleValue(Cr5kTokenTag key,double defaultValue) const;
   int             getIntValue(Cr5kTokenTag key,int defaultValue) const;;
   bool        getBooleanValue(Cr5kTokenTag key,bool defaultValue) const;;
   CString      getStringValue(Cr5kTokenTag key,const CString& defaultValue) const;;
   Cr5kTokenTag  getTokenValue(Cr5kTokenTag key,Cr5kTokenTag defaultValue) const;;

   CString getContentsString() const;
};

//_____________________________________________________________________________
class CCr5kElement
{
private:
   static int m_nextId;

   int m_id;
   bool m_loadedFlag;
   Cr5kSourceFileTypeTag m_sourceFileType;
   int m_sourceLineNumber;

public:
   CCr5kElement();
   virtual ~CCr5kElement();

   int getId() const { return m_id; }

   void setLoadedFlag();
   bool isLoaded() const { return m_loadedFlag; }

   int getSourceLineNumber() const;
   CString getSourceLineDescriptor() const;
   void setSourceLineNumber(Cr5kSourceFileTypeTag sourceFileType,int sourceLineNumber);

   virtual void empty();
   virtual bool read(CCr5kTokenReader& tokenReader) = 0;

   static void resetNextId();
};

//_____________________________________________________________________________
class CCr5kSkippedElement : public CCr5kElement
{
private:

public:
   CCr5kSkippedElement();

   virtual bool read(CCr5kTokenReader& tokenReader);
};

//_____________________________________________________________________________
class CCr5kUnitDescriptor : public CCr5kElement
{
private:
   Cr5kTokenTag m_unitToken;

public:
   CCr5kUnitDescriptor();

   bool read(CCr5kTokenReader& tokenReader);

   Cr5kTokenTag getUnits() const { return m_unitToken; }
};

//_____________________________________________________________________________
class CCr5kVersion : public CCr5kElement
{
private:
   CString m_versionString;

public:
   CCr5kVersion();

   bool read(CCr5kTokenReader& tokenReader);
};

//_____________________________________________________________________________
class CCr5kHeader : public CCr5kElement
{
private:
   CCr5kVersion m_version;
   CCr5kUnitDescriptor m_unitDescriptor;

public:
   CCr5kHeader();

   virtual void empty();
   virtual bool read(CCr5kTokenReader& tokenReader);

   Cr5kTokenTag getUnits() const { return m_unitDescriptor.getUnits(); }
};

//_____________________________________________________________________________
class CCr5kSystemLayerType : public CCr5kElement
{
private:
   Cr5kTokenTag m_systemLayerType;

public:
   CCr5kSystemLayerType();

   Cr5kTokenTag getSystemLayerType() const { return m_systemLayerType; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   //virtual void empty();
};

//_____________________________________________________________________________
class CCr5kLayerIdentifier : public CCr5kElement
{
private:
   Cr5kTokenTag m_identifierType;
   CCr5kSystemLayerType m_systemLayerType;
   CString m_layerName;
   CCr5kLayerIdentifier* m_referedBy;

public:
   CCr5kLayerIdentifier();
   ~CCr5kLayerIdentifier();

   Cr5kTokenTag getType() const { return m_identifierType; }
   const CCr5kSystemLayerType& getSystemLayerType() const { return m_systemLayerType; }
   const CCr5kLayerIdentifier* getReferedBy() const { return m_referedBy; }
   CString getLayerName() const;

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();

   CString getName() const { return m_layerName; }
};

//_____________________________________________________________________________
class CCr5kPoint : public CCr5kElement
{
private:
   double m_x;
   double m_y;

   CUnits m_units;

public:
   CCr5kPoint();

   void setUnits(const CUnits& units) { m_units = units; }

   double getX() const { return m_x; }
   double getY() const { return m_y; }
   CPoint2d getPoint() const { return CPoint2d(m_units.convertFromPortUnits(m_x),m_units.convertFromPortUnits(m_y)); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kPointParameter : public CCr5kElement
{
private:
   CCr5kPoint m_point;

public:
   CCr5kPointParameter();

   const CCr5kPoint& getPoint() const { return m_point; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kMeshFlags : public CCr5kElement
{
private:
   CArray<int,int> m_meshFlags;

public:
   CCr5kMeshFlags();

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kFillet : public CCr5kElement
{
private:
   CCr5kParameters m_parameters;

public:
   CCr5kFillet();

   double getPointWidth()       const { return m_parameters.getDoubleValue(ptokWidth,0.); }
   double getFilletWidth()      const { return m_parameters.getDoubleValue(ptokFilletWidth,0.); }
   double getFilletLength()     const { return m_parameters.getDoubleValue(ptokLength,0.); }
   bool   hasForwardDirection() const { return m_parameters.getTokenValue(ptokDir ,tokForward ) == tokForward; }
   bool   hasStraightType()     const { return m_parameters.getTokenValue(ptokType,tokStraight) == tokStraight; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kTanArc : public CCr5kElement
{
private:
   Cr5kTokenTag m_type;
   double m_arcRadius;

public:
   CCr5kTanArc();

   bool isOn() const { return (isLoaded() && m_type != tokOff); }
   double getRadius() const { return m_arcRadius; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
// This class cannot be abstract because then CTypedCr5kElementContainer could not instantiate it
class CCr5kVertex : public CCr5kElement
{
private:

public:
   CCr5kVertex();

   CCr5kPointVertex* getPointVertex() { return (CCr5kPointVertex*)this; }
   CCr5kArcVertex*   getArcVertex()   { return (CCr5kArcVertex*)this; }

   virtual bool isArc() const;
   virtual bool isLine() const;
   virtual bool isTangentArc() const;
   virtual CPoint2d getBeginPoint() const;
   virtual CPoint2d getEndPoint() const;

   virtual bool read(CCr5kTokenReader& tokenReader);
};

//_____________________________________________________________________________
class CCr5kPointVertex : public CCr5kVertex
{
private:
   double m_x;
   double m_y;
   CCr5kParameters m_parameters;
   CCr5kTanArc m_tanArc;
   CCr5kFillet m_fillet;

public:
   CCr5kPointVertex();

   double getWidth() const;
   bool hasWidth() const;
   bool hasFillet() const;
   CPoint2d getPoint() const { return getEndPoint(); }
   double getTangentArcRadius() const { return m_tanArc.getRadius(); }
   const CCr5kFillet& getFillet() const { return m_fillet; }

   virtual bool isArc() const;
   virtual bool isLine() const;
   virtual bool isTangentArc() const;
   virtual CPoint2d getBeginPoint() const;
   virtual CPoint2d getEndPoint() const;

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kArcPoint : public CCr5kElement
{
private:
   double m_x;
   double m_y;
   CCr5kParameters m_parameters;
   CCr5kFillet m_fillet;

public:
   CCr5kArcPoint();

   double getX() const { return m_x; }
   double getY() const { return m_y; }
   CPoint2d getPoint() const { return CPoint2d(m_x,m_y); }
   double getWidth() const { return m_parameters.getDoubleValue(ptokWidth,0.); }
   const CCr5kFillet& getFillet() const { return m_fillet; }
   bool hasFillet() const;
   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kArcVertex : public CCr5kVertex
{
private:
   Cr5kTokenTag m_arcDirection;
   double m_arcRadius;
   CCr5kArcPoint m_beginPoint;
   double m_deltaX;
   double m_deltaY;
   CCr5kArcPoint m_endPoint;

public:
   CCr5kArcVertex();

   const CCr5kArcPoint& getBeginArcPoint() const { return m_beginPoint; }
   const CCr5kArcPoint& getEndArcPoint()   const { return m_endPoint; }
   bool isCcw()           const { return m_arcDirection == tokCcw; }
   bool isCw()            const { return !isCcw(); }
   double getArcRadius()  const { return m_arcRadius; }
   double getDeltaX()     const { return m_deltaX; }
   double getDeltaY()     const { return m_deltaY; }
   CSize2d getDelta()     const { return CSize2d(m_deltaX,m_deltaY); }
   double getBeginWidth() const { return m_beginPoint.getWidth(); }
   double getEndWidth()   const { return m_endPoint.getWidth(); }

   double getBulge() const;
   double getBulge(CPoint2d& arcCenter) const;

   virtual bool isArc() const;
   virtual bool isLine() const;
   virtual bool isTangentArc() const;
   virtual CPoint2d getBeginPoint() const;
   virtual CPoint2d getEndPoint() const;

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();

private:
   //CPoint2d correctCenterCoordinate(CPoint2d center,const CPoint2d& begin,double radius);

};

//_____________________________________________________________________________
class CCr5kPolyListManager
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CPolyList& m_polyList;
   double m_width;
   bool m_squareWidthFlag;
   PolyTypeTag m_polyType;
   const CUnits& m_units;
   CPoly* m_poly;
   double m_polyWidth; // in cr5k units
   CPoint2d m_firstPoint;
   int m_pointCount;
   int m_polyCount;
   double m_pointTolerance;
   static const double m_cleanTolerance;
   bool m_applyDoubleShrink;

public:
   CCr5kPolyListManager(CCamCadDatabase& camCadDatabase, const CUnits& units, CPolyList& polyList,
      double width, bool squareWidthFlag, PolyTypeTag polyType, bool applyDoubleShrink);

   void addArcVertex(const CCr5kArcVertex& arcVertex);
   void addTangentArc(const CPoint2d& beginPoint,const CCr5kPointVertex& pointVertex,const CPoint2d& endPoint);
   void addPointVertex(const CCr5kPointVertex& pointVertex);
   void addFillet(const CPoint2d& headPoint,const CPoint2d& tailPoint,bool straightTypeFlag,double pointWidth,double filletWidth,double filletLength);
   //void close();
   //void clean();
   void complete();
   bool ShouldConsiderOutlineWidth();

private:
   void splitPolyIfWidthChange(double width);
   void splitPoly();
   void addPoint(double x,double y,double bulge,double width);
   int getDefinedWidthIndex(double width);
   //CPoly& getPoly(double width);
};

//_____________________________________________________________________________
// bnf: '(' 'elements' {element} ')'
template<class ELEMENT_TYPE,Cr5kTokenTag elementTokenTag>
class CTypedCr5kElementContainer : public CCr5kElement
{
protected:
   //Cr5kTokenTag m_elementTokenTag;
   CTypedPtrListContainer<ELEMENT_TYPE*> m_elementList;

public:
   CTypedCr5kElementContainer()
   {
   }

   virtual void empty()
   {
      m_elementList.empty();

      CCr5kElement::empty();
   }

   virtual bool read(CCr5kTokenReader& tokenReader)
   {
      CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

      bool retval = true;

      for (bool loopFlag = true;loopFlag;)
      {
         ELEMENT_TYPE* element = new ELEMENT_TYPE();

         if (tokenReader.readZeroOrOneElements(*element,elementTokenTag))
         {
            m_elementList.AddTail(element);
         }
         else
         {
            delete element;
            loopFlag = false;
         }
      }

      retval = tokenReader.skipExpectedEmptyRemainderOfList() && retval;

      setLoadedFlag();

      return retval;
   }

   POSITION getHeadPosition() const
   {
      return m_elementList.GetHeadPosition();
   }

   ELEMENT_TYPE* getNext(POSITION& pos) const
   {
      return m_elementList.GetNext(pos);
   }

   ELEMENT_TYPE* getAt(POSITION pos) const
   {
      return m_elementList.GetAt(pos);
   }

   ELEMENT_TYPE* getHead() const
   {
      return m_elementList.GetHead();
   }

   ELEMENT_TYPE* getTail() const
   {
      return m_elementList.GetTail();
   }

   int getCount() const
   {
      return m_elementList.GetCount();
   }
};

//_____________________________________________________________________________
// bnf: {element}
template<class ELEMENT_TYPE,Cr5kTokenTag elementTokenTag>
class CTypedCr5kElementListContainer : public CTypedCr5kElementContainer<ELEMENT_TYPE,elementTokenTag>
{
public:
   CTypedCr5kElementListContainer()
   {
   }

   virtual bool read(CCr5kTokenReader& tokenReader)
   {
      CCr5kTokenReaderFunction tokenReaderFunction(tokenReader,__FUNCTION__);

      bool retval = true;

      while (true)
      {
         if (tokenReader.getCurrentTokenTag() != elementTokenTag)
         {
            tokenReader.ungetToken();
            break;
         }

         ELEMENT_TYPE* element = new ELEMENT_TYPE();
         m_elementList.AddTail(element);

         retval = element->read(tokenReader) && retval;

         tokenReader.getNextTokenString();
      }

      setLoadedFlag();

      return retval;
   }
};

//_____________________________________________________________________________
class CCr5kVertices : public CTypedCr5kElementContainer<CCr5kVertex,tok_Cr5kTokenTagUndefined>
{
private:
   CUnits m_units;

public:
   CCr5kVertices();

   const CUnits& getUnits() const { return m_units; }
   void setUnits(const CUnits& units) { m_units = units; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   //virtual void empty();
};

//_____________________________________________________________________________
class CCr5kOpenShape : public CCr5kElement
{
private:
   CCr5kParameters m_parameters;
   CCr5kVertices m_vertices;

   bool m_useOutlineWidth;
   CUnits m_units;

public:
   CCr5kOpenShape();

   void setUnits(const CUnits& units) { m_units = units; }

   double getOutlineWidth()      const { return m_useOutlineWidth ? m_parameters.getDoubleValue(ptokOutlineWidth,0.) : 0.0; }
   const CCr5kVertices& getVertices() const { return m_vertices; }

   double getOutlineWidthInPageUnits() const { return m_units.convertFromPortUnits(getOutlineWidth()); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kOpenShapes : public CTypedCr5kElementListContainer<CCr5kOpenShape,ptokOpenShape>
{
// bnf: {openShape}
// bnf: openShape           ::= '(' 'openShape' [outlineWidth] vertices ')'
// bnf:    outlineWidth        ::= '(' 'outlineWidth' distance ')'

//private:
//   CTypedPtrListContainer<CCr5kOpenShape*> m_openShapes;
//
//public:
//   CCr5kOpenShapes();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kRawString : public CCr5kElement
{
private:
   CString m_string;

public:
   CCr5kRawString();

   CString getString() const { return m_string; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kGeometry : public CCr5kElement
{
private:
   Cr5kTokenTag m_geometryType;
   double m_floatValue;  // radius or width
   CCr5kPoint m_point;  
   CCr5kPoint m_point2;  
   CCr5kRawString m_rawString;  
   CCr5kParameters m_parameters;
   mutable CCr5kVertices* m_vertices;
   mutable CCr5kOpenShapes* m_openShapes;
   CCr5kPointParameter* m_meshBasePoint;
   CCr5kMeshFlags* m_meshFlags;
   bool m_useOutlineWidth;
   CUnits m_units;

public:
   CCr5kGeometry();

   void setUnits(const CUnits& units) { m_units = units; }

   Cr5kTokenTag getType() const { return m_geometryType; }

   const CCr5kVertices& getVertices() const;
   double getOutlineWidth()      const { return m_useOutlineWidth ? m_parameters.getDoubleValue(ptokOutlineWidth,0.) : 0.0; }
   const CCr5kPoint& getPoint()  const { return m_point;  }
   const CCr5kPoint& getPoint2() const { return m_point2; }

   double getOutlineWidthInPageUnits() const { return m_units.convertFromPortUnits(getOutlineWidth()); }

   // circle
   double getRadius()            const { return m_floatValue; }
   double getRadiusInPageUnits() const { return m_units.convertFromPortUnits(getRadius()); }

   // donut, thermal
   double getInnerRadius()             const { return m_parameters.getDoubleValue(ptokIn ,0.); }
   double getOuterRadius()             const { return m_parameters.getDoubleValue(ptokOut,0.); }

   double getInnerRadiusInPageUnits()  const { return m_units.convertFromPortUnits(getInnerRadius()); }
   double getOuterRadiusInPageUnits()  const { return m_units.convertFromPortUnits(getOuterRadius()); }

   // thermal
   int getBridgeCount()                const { return m_parameters.getIntValue(ptokNBridge, 4); }
   double getBridgeAngle()             const { return m_parameters.getDoubleValue(ptokBridgeAngle,45.); } // default 45 degrees
   double getBridgeWidth()             const { return m_parameters.getDoubleValue(ptokBridgeWidth,0.); }
   double getBridgeWidthInPageUnits()  const { return m_units.convertFromPortUnits(getBridgeWidth()); }

   // rectangle
   double getWidth()                   const { return m_parameters.getDoubleValue(ptokWidth ,0.); }
   double getHeight()                  const { return m_parameters.getDoubleValue(ptokHeight,0.); }
   double getRectangleAngleDegrees()   const { return m_parameters.getDoubleValue(ptokRAngle,0.); }

   double getWidthInPageUnits()        const { return m_units.convertFromPortUnits(getWidth()); }
   double getHeightInPageUnits()       const { return m_units.convertFromPortUnits(getHeight()); }

   // oval
   double getOvalAngleDegrees()        const { return m_parameters.getDoubleValue(ptokOvalAngle,0.); }

   // oblong - getWidth(), getPoint(), getPoint2()

   // text
   CString getTextString()             const { return m_rawString.getString(); }
   double getTextCharWidth()           const { return m_parameters.getDoubleValue(ptokWidth  ,0.); }
   double getTextCharHeight()          const { return m_parameters.getDoubleValue(ptokHeight ,0.); }
   double getTextStrokeWidth()         const { return m_parameters.getDoubleValue(ptokStrokeWidth,0.); }
   double getTextAngleDegrees()        const { return (m_parameters.hasValueForKey(ptokTextAngle) ? m_parameters.getDoubleValue(ptokTextAngle,0.) : m_parameters.getDoubleValue(ptokAngle  ,0.)); }
   Cr5kTokenTag getTextJustification() const { return m_parameters.getTokenValue(ptokJustify ,tokCe_C); }
   Cr5kTokenTag getTextFlip()          const { return m_parameters.getTokenValue(ptokFlip    ,tokNone); }

   double getTextCharWidthInPageUnits()   const { return m_units.convertFromPortUnits(getTextCharWidth()); }
   double getTextCharHeightInPageUnits()  const { return m_units.convertFromPortUnits(getTextCharHeight()); }
   double getTextStrokeWidthInPageUnits() const { return m_units.convertFromPortUnits(getTextStrokeWidth()); }

   // surface
   bool hasOpenShapes() const;
   const CCr5kOpenShapes& getOpenShapes() const;

   // line
   bool hasSquarePenShape()            const { return (m_parameters.getTokenValue(ptokPenShape ,tokCircle) == tokSquare); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();

private:
   CCr5kVertices& getVertices();
   CCr5kOpenShapes& getOpenShapes();
   CCr5kPointParameter& getMeshBasePoint();
   CCr5kMeshFlags& getMeshFlags();

};

//_____________________________________________________________________________
class CCr5kGeometries : public CTypedCr5kElementContainer<CCr5kGeometry,tok_Cr5kTokenTagUndefined>
{
//private:
//   CTypedPtrListContainer<CCr5kGeometry*> m_geometries;
//
//public:
//   CCr5kGeometries();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kSingleGeometry : public CCr5kGeometries
{
private:
   mutable CCr5kGeometry* m_allocatedGeometry;

public:
   CCr5kSingleGeometry();

   const CCr5kGeometry& getGeometry() const;
   //virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kPad : public CCr5kElement
{
private:
   CString m_padName;
   CCr5kGeometries m_geometries;
   CCr5kProperties m_properties;

public:
   CCr5kPad();

   CString getName() const { return m_padName; }
   const CCr5kGeometries& getGeometries() const { return m_geometries; }
   const CCr5kProperties& getProperties() const { return m_properties; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kPads : public CCr5kElement
{
private:
   CTypedPtrListContainer<CCr5kPad*> m_pads;

public:
   CCr5kPads();

   void linkPads(CCr5kLinkedPads& linkedPads,COperationProgress* progress);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kPadstackHole : public CCr5kElement
{
private:
   CString m_layerName;
   CCr5kSingleGeometry m_singleGeometry;
   CCr5kParameters m_parameters;

public:
   CCr5kPadstackHole();

   CString getLayerName()                         const { return m_layerName; }
   const CCr5kSingleGeometry& getSingleGeometry() const { return m_singleGeometry; }
   int getMaterialNumber()                        const { return m_parameters.getIntValue(ptokMaterialNumber,-1); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kPadstackHoles : public CTypedPtrListContainer<CCr5kPadstackHole*>
{
};

//_____________________________________________________________________________
class CCr5kPadset : public CCr5kElement
{
private:
   int     m_materialNumber;
   CString m_layerName;
   CString m_connectPadName;
   CString m_noconnectPadName;
   CString m_thermalPadName;
   CString m_clearancePadName;

public:
   CCr5kPadset();

   int     getMaterialNumber()   const { return m_materialNumber; }
   CString getLayerName()        const { return m_layerName; }
   CString getConnectPadName()   const { return m_connectPadName; }
   CString getNoconnectPadName() const { return m_noconnectPadName; }
   CString getThermalPadName()   const { return m_thermalPadName; }
   CString getClearancePadName() const { return m_clearancePadName; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kPadsets : public CTypedPtrListContainer<CCr5kPadset*>
{
};

//_____________________________________________________________________________
class CCr5kPadstack : public CCr5kElement
{
private:
   CString m_padstackName;
   CCr5kParameters m_parameters;
   CCr5kPadstackHoles m_padstackHoles;
   CCr5kPadsets m_padsets;
   CCr5kProperties m_properties;

public:
   CCr5kPadstack();

   CString getName()       const { return m_padstackName; }
   bool    isVia()         const { return m_parameters.getBooleanValue(ptokIsBuildupVia,false); }
   bool    isThruHole()    const { return m_parameters.getTokenValue(ptokThroughMode,tokNonThrough) == tokThrough; }
   const CCr5kPadstackHoles& getPadstackHoles() const { return m_padstackHoles; }
   const CCr5kPadsets&       getPadsets()       const { return m_padsets; }
   const CCr5kProperties&    getProperties()    const { return m_properties; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kPadstacks : public CCr5kElement
{
private:
   CTypedPtrListContainer<CCr5kPadstack*> m_padstacks;

public:
   CCr5kPadstacks();

   void linkPadstacks(CCr5kLinkedPadstacks& linkedPadstacks,COperationProgress* progress);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kFromTo : public CCr5kElement
{
private:
   int m_layerNumber1;
   int m_layerNumber2;

public:
   CCr5kFromTo();

   int getFromLayerNumber() const { return m_layerNumber1; }
   int getToLayerNumber()   const { return m_layerNumber2; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kTestPad : public CCr5kElement
{
private:
   bool m_isTestPad;
   CCr5kParameters m_parameters;  // side, id
   CCr5kPoint m_point;

public:
   CCr5kTestPad();

   bool    isTestPad()             const { return m_isTestPad; }
   CString getTestPadReference()   const { return m_parameters.getStringValue(ptokId,""); }
   CString getTestProbeReference() const { return m_parameters.getStringValue(ptokTpProbeId,""); }
   bool isTestPadSideTop()         const { return m_parameters.getTokenValue(ptokSide,tokA) == tokA; }
   const CCr5kPoint& getPoint()    const { return m_point; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kConductivePadStackPadLayer : public CCr5kElement
{
private:
   int m_layerNumber;
   CCr5kParameters m_parameters;  // status
   CCr5kGeometries m_padstackGeometries;

public:
   CCr5kConductivePadStackPadLayer();

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();

   int getLayerNumber() const { return m_layerNumber; }
   Cr5kTokenTag getStatus() const;

   const CCr5kGeometries &getGeometries() const { return m_padstackGeometries; };
};

//_____________________________________________________________________________
class CCr5kConductivePadStackPad : public CTypedCr5kElementContainer<CCr5kConductivePadStackPadLayer,ptokLayerNumber>
{
public:
   bool containsPadTypeOnLayer(int layerNumber,Cr5kTokenTag padType) const;

   CString getDescriptor() const;
};

//_____________________________________________________________________________
class CCr5kNonConductivePadStackPadLayer : public CCr5kElement
{
private:
   CString m_layerName;
   CCr5kParameters m_parameters;  // visible
   CCr5kGeometries m_padstackGeometries;

public:
   CCr5kNonConductivePadStackPadLayer();

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();

   CString getLayerName()  { return m_layerName; }

   const CCr5kGeometries &getGeometries() const { return m_padstackGeometries; };
};

//_____________________________________________________________________________
class CCr5kNonConductivePadStackPad : public CTypedCr5kElementContainer<CCr5kNonConductivePadStackPadLayer,ptokLayer>
{
};

//_____________________________________________________________________________
class CCr5kPcbPadstackHoleGeometry : public CCr5kElement
{
private:
   CCr5kSingleGeometry m_singleGeometry;

public:
   CCr5kPcbPadstackHoleGeometry();

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kFootPadstackGroupSet : public CCr5kElement
{
private:
   CString m_padstackGroupName;
   CCr5kParameters m_parameters;

public:
   CCr5kFootPadstackGroupSet();

   CString getName()         const { return m_padstackGroupName; }
   CString getPadstackName() const { return m_parameters.getStringValue(ptokPadstack,""); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kLayoutPrimitive : public CCr5kElement
{
private:
   Cr5kTokenTag m_primitiveType;
   CString m_name;
   CCr5kParameters m_parameters;
   CCr5kSingleGeometry m_singleGeometry;
   CCr5kProperties m_properties;
   CCr5kPoint m_point;
   CCr5kFromTo m_fromTo;  // layer range
   CCr5kTestPad m_testPad;
   CCr5kConductivePadStackPad m_conductivePadStackPad;
   CCr5kNonConductivePadStackPad m_nonConductivePadStackPad;
   CCr5kPcbPadstackHoleGeometry m_padstackHoleGeometry;
   CCr5kFootPadstackGroupSet m_footPadstackGroupSet;
   CCr5kReferPrimitive* m_referPrimitive;

   CUnits m_units;

public:
   CCr5kLayoutPrimitive();

   void setUnits(const CUnits& units) { m_units = units; }

   Cr5kTokenTag getPrimitiveType() const { return m_primitiveType; }

   const CCr5kSingleGeometry&           getSingleGeometry()           const { return m_singleGeometry; }
   const CCr5kFootPadstackGroupSet&     getFootPadstackGroupSet()     const { return m_footPadstackGroupSet; }
   const CCr5kConductivePadStackPad&    getConductivePadStackPad()    const { return m_conductivePadStackPad; }
   const CCr5kNonConductivePadStackPad& getNonConductivePadStackPad() const { return m_nonConductivePadStackPad; }
   const CCr5kReferPrimitive*           getReferPrimitive()           const { return m_referPrimitive; }
   const CCr5kTestPad&                  getTestPad()                  const { return m_testPad; }

   CString getName()        const { return m_name; }
   CString getNetName()     const { return m_parameters.getStringValue(ptokNet,""); }
   Cr5kTokenTag getType()   const { return m_parameters.getTokenValue(ptokType,tokNone); }
   int    getSubBoard()     const { return m_parameters.getIntValue(ptokSubBoard,0); }
   double getAngleDegrees() const { return m_parameters.getDoubleValue(ptokAngle,0.); }
   double getUpperHeight()  const { return m_parameters.getDoubleValue(ptokUpperHeight,0.); }
   bool   getFlipX()        const { return m_parameters.getTokenValue(ptokFlip,tok_Cr5kTokenTagUndefined) == tokX; }
   bool   getFlipY()        const { return m_parameters.getTokenValue(ptokFlip,tok_Cr5kTokenTagUndefined) == tokY; }
   bool         isVia()     const { return (m_fromTo.isLoaded() && (m_fromTo.getFromLayerNumber() != m_fromTo.getToLayerNumber())); }
   bool     isTestPad()     const;
   bool    isThruHole()     const { return m_parameters.getTokenValue(ptokThroughMode,tokNonThrough) == tokThrough; }
   bool isBareChipPad()     const { return m_parameters.getBooleanValue(ptokBareChipPad,false); }
   const CCr5kPoint& getPoint() const { return m_point; }
   CBasesVector getPadstackBasesVector() const;
   bool getFromToLayers(int& fromLayerNumber,int& toLayerNumber) const;
   CString getTestPadReference() const;
   //CString getViaReference() const;
   const CCr5kProperties& getProperties() const { return m_properties; }
   bool getPlacedTop() const;
   bool hasSide() const;
   bool hasPadGeometry() const;
   bool hasConductivePadGeometry() const;
   bool hasNonConductivePadGeometry() const;

   bool hasKey(Cr5kTokenTag key)       const { return m_parameters.hasKey(key); }

   int getMaterialNumber()             const { return m_parameters.getIntValue(ptokMaterialNumber,-1); }
   bool isNet()                        const { return m_parameters.hasValueForKey(ptokNet); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kReferPrimitive : public CCr5kElement
{
private:
   CCr5kParameters m_parameters;
   CCr5kLayoutPrimitive m_layoutPrimitive;
   CCr5kProperties m_properties;

public:
   CCr5kReferPrimitive();

   CCr5kLayoutPrimitive& getLayoutPrimitive() { return m_layoutPrimitive; }
   const CCr5kLayoutPrimitive& getLayoutPrimitive() const { return m_layoutPrimitive; }
   const CCr5kProperties& getProperties() const { return m_properties; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kLayoutLayer : public CTypedCr5kElementContainer<CCr5kLayoutPrimitive,tok_Cr5kTokenTagUndefined>
{
private:
   CCr5kLayerIdentifier m_layerIdentifier;
//   CTypedPtrListContainer<CCr5kLayoutPrimitive*> m_primitiveList;
//
public:
//   CCr5kLayoutLayer();
//
   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
//
   CString getName() const { return m_layerIdentifier.getName(); }
   const CCr5kLayerIdentifier& getLayerIdentifier() const { return m_layerIdentifier; }
//private:
};

//_____________________________________________________________________________
class CCr5kLayout : public CTypedCr5kElementContainer<CCr5kLayoutLayer,ptokLayer>
{
//private:
//   CTypedPtrListContainer<CCr5kLayoutLayer*> m_layoutLayerList;
//
//public:
//   CCr5kLayout();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
//
//private:
//   void add(CCr5kLayoutLayer& layoutLayer);
};

//_____________________________________________________________________________
class CCr5kHeelprint : public CCr5kElement
{
private:
   CCr5kLayout m_layout;

public:
   CCr5kHeelprint();

   const CCr5kLayout& getLayout() const { return m_layout; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kBondwire : public CCr5kElement
{
private:
   int        m_bondwireNumber;
   CCr5kPoint m_startPoint;
   CCr5kPoint m_endPoint;
   CCr5kParameters m_parameters;

public:
   CCr5kBondwire();

   int getWireNumber() const { return m_bondwireNumber; }
   const CCr5kPoint& getStartPoint() const { return m_startPoint; }
   const CCr5kPoint&   getEndPoint() const { return   m_endPoint; }
   bool isGenerated() const { return m_parameters.getBooleanValue(ptokBwIsGenerated,true); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kBondwires : public CTypedCr5kElementContainer<CCr5kBondwire,ptokBondwire>
{
private:
   CCr5kParameters m_parameters;

   CUnits m_units;

public:
   CCr5kBondwires();

   void setUnits(const CUnits& units) { m_units = units; }

   int getMaterialNumber()  const { return m_parameters.getIntValue(ptokMaterialNumber,-1); }
   double getWireDiameter() const { return m_parameters.getDoubleValue(ptokBwDiameter,0.); }

   double getWireDiameterInPageUnits() const { return m_units.convertFromPortUnits(getWireDiameter()); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kExitDirection : public CCr5kSkippedElement
{
};

//_____________________________________________________________________________
class CCr5kToeprintPin : public CCr5kElement
{
private:
   CString              m_pinNumber;
   CCr5kPoint           m_point;
   CCr5kBondwires       m_bondwires;
   CCr5kLayout          m_layout;
   CCr5kExitDirection   m_exitDirection;

public:
   CCr5kToeprintPin();

   CString getPinNumber()               const { return m_pinNumber; }
   const CCr5kPoint&     getPoint()     const { return m_point; }
   const CCr5kBondwires& getBondwires() const { return m_bondwires; }
   const CCr5kLayout&    getLayout()    const { return m_layout; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kToeprintPins : public CTypedPtrListContainer<CCr5kToeprintPin*>
{
};

//_____________________________________________________________________________
class CCr5kToeprint : public CCr5kElement
{
private:
   CCr5kToeprintPins m_pinList;

public:
   CCr5kToeprint();

   const CCr5kToeprintPins& getToeprintPins() const { return m_pinList; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kMountBasePoint : public CCr5kElement
{
private:
   CCr5kPoint m_mountBasePoint;

public:
   CCr5kMountBasePoint();

   const CCr5kPoint& getMountBasePoint() const { return m_mountBasePoint; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kMountBasePoints : public CTypedCr5kElementContainer<CCr5kMountBasePoint,ptokMountBasePoint>
{
};

//_____________________________________________________________________________
class CCr5kMachineName : public CCr5kElement
{
private:
   CString              m_machineName;
   CCr5kMountBasePoints m_mountBasePoints;

public:
   CCr5kMachineName();

   CString getMachineName()                         const { return m_machineName; }
   const CCr5kMountBasePoints& getMountBasePoints() const { return m_mountBasePoints; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kMachineNames : public CTypedCr5kElementContainer<CCr5kMachineName,ptokMachineName>
{
};

//_____________________________________________________________________________
class CCr5kMounterInformation : public CCr5kElement
{
private:
   CString m_name;

   CCr5kMachineNames m_machineNames;

public:
   CCr5kMounterInformation();

   const CCr5kMachineNames& getMachineNames() const { return m_machineNames; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kFootprint : public CCr5kElement
{
private:
   CString m_name;

   CCr5kMounterInformation m_mounterInformation;
   CCr5kHeelprint m_heelprint;
   CCr5kToeprint  m_toeprint;
   CCr5kProperties m_properties;

public:
   CCr5kFootprint();

   CString getName() const { return m_name; }
   const CCr5kHeelprint&          getHeelprint()          const { return m_heelprint; }
   const CCr5kToeprint&           getToeprint()           const { return m_toeprint; }
   const CCr5kProperties&         getProperties()         const { return m_properties; }
   const CCr5kMounterInformation& getMounterInformation() const { return m_mounterInformation; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kFootprints : public CCr5kElement
{
private:
   CTypedPtrListContainer<CCr5kFootprint*> m_footPrintList;

public:
   CCr5kFootprints();

   void linkFootprints(CCr5kLinkedFootprints& linkedFootprints,COperationProgress* progress);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();

private:
   void add(CCr5kFootprint& footPrint);
};

//_____________________________________________________________________________
class CCr5kFootprintContainer : public CCr5kElement
{
private:
   CCr5kSkippedElement m_footContainerCommonTable;
   CCr5kSkippedElement m_padstackGroups;
   CCr5kPads           m_pads;
   CCr5kPadstacks      m_padstacks;
   CCr5kFootprints     m_footprints;

public:
   CCr5kFootprintContainer();

   void linkPads(CCr5kLinkedPads& linkedPads,COperationProgress* progress);
   void linkPadstacks(CCr5kLinkedPadstacks& linkedPadstacks,COperationProgress* progress);
   void linkFootprints(CCr5kLinkedFootprints& linkedFootprints,COperationProgress* progress);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kFootprintFile : public CCr5kElement
{
private:
   CCr5kHeader             m_header;
   CCr5kFootprintContainer m_footprintContainer;

public:
   CCr5kFootprintFile();

   void linkPads(CCr5kLinkedPads& linkedPads,COperationProgress* progress);
   void linkPadstacks(CCr5kLinkedPadstacks& linkedPadstacks,COperationProgress* progress);
   void linkFootprints(CCr5kLinkedFootprints& linkedFootprints,COperationProgress* progress);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();

   Cr5kTokenTag getUnits() const { return m_header.getUnits(); }
};

//_____________________________________________________________________________
class CCr5kFootprintTechnology : public CCr5kElement
{
private:
   CString m_footprintId;
   CString m_technologyId;

public:
   CCr5kFootprintTechnology();

   CString getFootprintName() const { return m_footprintId; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kCompGate : public CCr5kSkippedElement
{
};

//_____________________________________________________________________________
class CCr5kCompGates : public CTypedCr5kElementListContainer<CCr5kCompGate,ptokGate>
{
};

//_____________________________________________________________________________
class CCr5kGatePinRef : public CCr5kSkippedElement
{
};

//_____________________________________________________________________________
class CCr5kGatePinRefs : public CTypedCr5kElementListContainer<CCr5kGatePinRef,ptokGate>
{
};

//_____________________________________________________________________________
class CCr5kCompPin : public CCr5kElement
{
private:
   CString                  m_pinNumber;
   CCr5kPoint               m_point;
   CCr5kParameters          m_parameters;
   CCr5kLayout              m_layout;
   CCr5kBondwires           m_bondwires;
   CCr5kGatePinRefs         m_gatePinRefs;
   CCr5kExitDirection       m_exitDirection;

   mutable Bool3Tag         m_isThruHole;
   mutable CString          m_netName;

public:
   CCr5kCompPin();

   CString getPinNumber()  const { return m_pinNumber; }
   CCr5kPoint getPoint() const { return m_point;     }
   CBasesVector getBasesVector();
   CCr5kLayout& getLayout() { return m_layout; }
   const CCr5kLayout&       getLayout() const { return m_layout; }
   const CCr5kBondwires& getBondwires() const { return m_bondwires; }
   bool isThruHole() const;
   CString getNetName() const;

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kCompPins : public CTypedCr5kElementListContainer<CCr5kCompPin,ptokPin>
{
};

//_____________________________________________________________________________
class CCr5kCompPinReferenceMap
{
private:
   CTypedPtrMap<CMapStringToPtr,CString,CCr5kCompPin*> m_pinMap;

public:
   CCr5kCompPinReferenceMap(const CCr5kCompPins& compPins);

   const CCr5kCompPin* getAt(const CString& pinName);   
};

//_____________________________________________________________________________
class CCr5kComponent : public CCr5kElement
{
private:
   CCr5kParameters          m_parameters;
   CCr5kFootprintTechnology m_footprint;
   CCr5kFootprintTechnology m_reverseFootprint;
   CCr5kPointParameter      m_location;
   CCr5kLayout              m_layout;
   CCr5kCompPins            m_compPins;
   CCr5kCompGates           m_compGates;
   CCr5kProperties          m_properties;

   mutable Bool3Tag         m_isThruHole;
   mutable double           m_componentHeight;

   CUnits m_units;

public:
   CCr5kComponent();

   void setUnits(const CUnits& units) { m_units = units; }

   CString getFootprintName()        const { return        m_footprint.getFootprintName(); }
   CString getReverseFootprintName() const { return m_reverseFootprint.getFootprintName(); }
   CCr5kPoint getLocation()   const { return m_location.getPoint();          }
   void getBasesComponents(double& x,double& y,double& rotationDegrees,bool& mirror) const;
   CBasesVector getBasesVector() const;
   CBasesVector getBasesVector(bool mirrorRotationFlag) const;
   CBasesVector getBasesVector(bool mirrorRotationFlag,bool mirrorFlag) const;
   bool getPlacedTop() const;
   bool getPlacedBottom() const;
   CString getRefDes() const;
   CString getPartNumber() const { return m_parameters.getStringValue(ptokPart,""); }
   CString getPackage()    const { return m_parameters.getStringValue(ptokPackage,""); }
   CString getStockId()    const { return m_parameters.getStringValue(ptokStockId,""); }
   bool    getOutOfBoard() const { return m_parameters.getBooleanValue(ptokOutOfBoard,false); }
   bool    getDrawRefDes() const { return m_parameters.getBooleanValue(ptokDrawRefDes,true); }
   bool    getPlaced()     const { return m_parameters.getBooleanValue(ptokPlaced,true); }
   CCr5kLayout& getLayout() { return m_layout; }
   const CCr5kLayout& getLayout() const { return m_layout; }
   const CCr5kCompPins& getCompPins() const { return m_compPins; }
   bool isThruHole() const;
   CString getDescriptor() const;
   const CCr5kProperties& getProperties() const { return m_properties; }
   double getComponentHeight() const;
   double getComponentHeightInPageUnits() const { return m_units.convertFromPortUnits(getComponentHeight()); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kComponents : public CCr5kElement
{
private:
   CTypedPtrListContainer<CCr5kComponent*> m_components;

public:
   CCr5kComponents();

   void linkComponents(CCr5kLinkedComponents& linkedComponents,COperationProgress* progress);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kCompPinRef : public CCr5kElement
{
private:
   CString m_compReference;
   CString m_pinNumber;

public:
   CCr5kCompPinRef();

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kCompPinRefs : public CCr5kElement
{
private:
   CTypedPtrListContainer<CCr5kCompPinRef*> m_compPinRefs;

public:
   CCr5kCompPinRefs();

   int getCount() const { return m_compPinRefs.GetCount(); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kNet : public CCr5kElement
{
private:
   CString          m_netName;
   CCr5kParameters  m_parameters;
   CCr5kCompPinRefs m_compPinRefs;
   CCr5kProperties  m_properties;

public:
   CCr5kNet();

   const CCr5kProperties& getProperties() const { return m_properties; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kNets : public CCr5kElement
{
private:
   CTypedPtrListContainer<CCr5kNet*> m_nets;

public:
   CCr5kNets();

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kBoardLayout : public CCr5kElement
{
private:
   CCr5kLayout m_layout;

public:
   CCr5kBoardLayout();

   const CCr5kLayout& getLayout() const { return m_layout; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kSubBoard : public CCr5kElement
{
private:   

public:
   CCr5kSubBoard();
   CString GetAbsoluteBoardPathFromPath(CString path);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kSubBoards : public CCr5kElement
{
private:
   CTypedPtrListContainer<CCr5kSubBoard*> m_subBoards;

public:
   CCr5kSubBoards();

   int getCount() const { return m_subBoards.GetCount(); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kBoardContainer : public CCr5kElement
{
private:
   CCr5kComponents  m_components;
   CCr5kNets        m_nets;
   CCr5kBoardLayout m_boardLayout;
   CCr5kSubBoards   m_subBoards;
   CCr5kProperties  m_properties;

public:
   CCr5kBoardContainer();

   const CCr5kBoardLayout& getBoardLayout() const { return m_boardLayout; }
   const CCr5kProperties& getProperties()   const { return m_properties; }

   void linkComponents(CCr5kLinkedComponents& linkedComponents,COperationProgress* progress);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

////_____________________________________________________________________________
//class CCr5kBoardLayerRef : public CCr5kElement
//{
//private:
//   CCr5kLayerIdentifier m_layerIdentifier;
//
//public:
//   CCr5kBoardLayerRef();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
//};

//_____________________________________________________________________________
class CCr5kBoardLayerRefs : public CTypedCr5kElementContainer<CCr5kLayerIdentifier,tok_Cr5kTokenTagUndefined>
{
//private:
//   CTypedPtrListContainer<CCr5kLayerIdentifier*> m_boardLayerRefs;
//
//public:
//   CCr5kBoardLayerRefs();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kLayerCorrespondence : public CCr5kElement
{
private:
   CCr5kParameters     m_parameters;
   CCr5kBoardLayerRefs m_boardLayerRefs;

public:
   CCr5kLayerCorrespondence();

   CString getFootLayerName() const { return m_parameters.getStringValue(ptokFootLayer,""); }
   const CCr5kBoardLayerRefs& getBoardLayerRefs() const { return m_boardLayerRefs; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kLayerCorrespondences : public CTypedCr5kElementContainer<CCr5kLayerCorrespondence,ptokCorrespondence>
{
public:
   virtual bool read(CCr5kTokenReader& tokenReader);
};

//_____________________________________________________________________________
class CCr5kLayerMap : public CCr5kElement
{
private:
   Cr5kTokenTag              m_typeToken;
   CCr5kLayerCorrespondences m_correspondences;

public:
   CCr5kLayerMap();

   Cr5kTokenTag getType() const { return m_typeToken; }
   const CCr5kLayerCorrespondences& getLayerCorrespondences() const { return m_correspondences; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kLayerMaps : public CCr5kElement
{
private:
   CTypedPtrListContainer<CCr5kLayerMap*> m_layerMaps;

public:
   CCr5kLayerMaps();

   CCr5kLayerMap* getAt(Cr5kTokenTag mapType) const;

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kNonConductiveLayerRelation : public CCr5kElement
{
private:
   CString         m_layerName;
   CCr5kParameters m_parameters;

public:
   CCr5kNonConductiveLayerRelation();

   CString getLayerName() const { return m_layerName; }
   Cr5kTokenTag getType() const { return m_parameters.getTokenValue(ptokType,tok_Cr5kTokenTagUndefined); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kNonConductiveLayerRelations : public CTypedCr5kElementContainer<CCr5kNonConductiveLayerRelation,ptokRefer>
{
// bnf:                nonCondLayerRelation   ::= '(' 'refer' layerName ( nonCondLayerRelationType | nonCondLayerRelationUserDefType ) ')'
// bnf:                   nonCondLayerRelationType        ::= '(' 'type' ( 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'MOUNTER' | 'WIRE_PROHIBIT' | 
// bnf:                                                                    'PLACEMENT_PROHIBIT' | 'VIA_PROHIBIT' | 'THERMAL_SHAPE' | 'ONLYWIRE_PROHIBIT' | 'SUBCONDUCTIVE' | 'UNDEFINED' ) ')'
// bnf:                   nonCondLayerRelationUserDefType ::= '(' 'UserDefType' integerValue ')'
//private:
//   CTypedPtrListContainer<CCr5kNonConductiveLayerRelation*> m_nonConductiveLayerRelations;
//
public:
   CCr5kNonConductiveLayerRelations();

   //int getCount() const { return m_nonConductiveLayerRelations.GetCount(); }

   virtual bool read(CCr5kTokenReader& tokenReader);
   //virtual void empty();
};

//_____________________________________________________________________________
class CCr5kConductiveLayer : public CCr5kElement
{
private:
   int                              m_layerNumber;
   CCr5kParameters                  m_parameters;
   CCr5kNonConductiveLayerRelations m_nonConductiveLayerRelations;
   CCr5kProperties                  m_properties;

public:
   CCr5kConductiveLayer();

   CString getName()    const { CString layerName;  layerName.Format("%d",m_layerNumber);  return layerName; }
   int getLayerNumber() const { return m_layerNumber; }
   const CCr5kNonConductiveLayerRelations& getNonConductiveLayerRelations() const { return m_nonConductiveLayerRelations; }
   const CCr5kProperties& getProperties() const { return m_properties; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kConductiveLayers : public CTypedCr5kElementContainer<CCr5kConductiveLayer,ptokLayerNumber>
{
// bnf:          condLayers             ::= '(' 'conductiveLayer' {condLayer} ')' 
// bnf:             condLayer              ::= '(' 'layerNumber' integerValue { <condLayerType> | <signalName> | <solderingMethod> | nonCondLayerRelation | property } ')' 
// bnf:                condLayerType          ::= '(' 'type' ( 'POSI' | 'POSINEGA' | 'FULLSURF' ) ')'
// bnf:                signalName             ::= '(' 'signalName' string ')'
// bnf:                solderingMethod        ::= '(' 'soldering' ( 'FLOW' | 'REFLOW' | 'REFLOW_2' | 'NONE' ) ')'
// bnf:                nonCondLayerRelation   ::= '(' 'refer' layerName ( nonCondLayerRelationType | nonCondLayerRelationUserDefType ) ')'
// bnf:                   nonCondLayerRelationType        ::= '(' 'type' ( 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'MOUNTER' | 'WIRE_PROHIBIT' | 
// bnf:                                                                    'PLACEMENT_PROHIBIT' | 'VIA_PROHIBIT' | 'THERMAL_SHAPE' | 'ONLYWIRE_PROHIBIT' | 'SUBCONDUCTIVE' | 'UNDEFINED' ) ')'
// bnf:                   nonCondLayerRelationUserDefType ::= '(' 'UserDefType' integerValue ')'

//private:
//   CTypedPtrListContainer<CCr5kConductiveLayer*> m_conductiveLayers;
//
//public:
//   CCr5kConductiveLayers();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kNonConductiveLayer : public CTypedCr5kElementContainer<CCr5kNonConductiveLayer,ptokLayer>
{
private:
   CString         m_layerName;
   CCr5kParameters m_parameters;
   CCr5kProperties m_properties;

public:
   CCr5kNonConductiveLayer();

   CString getName() const { return m_layerName; }
   const CCr5kProperties& getProperties() const { return m_properties; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kNonConductiveLayers : public CTypedCr5kElementContainer<CCr5kNonConductiveLayer,ptokLayer>
{
};

//class CCr5kNonConductiveLayers : public CCr5kElement
//{
//private:
//   CTypedPtrListContainer<CCr5kNonConductiveLayer*> m_nonConductiveLayers;
//
//public:
//   CCr5kNonConductiveLayers();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
//};

//_____________________________________________________________________________
class CCr5kFootprintLayer : public CCr5kElement
{
private:
   CString         m_layerName;
   CCr5kParameters m_parameters;
   CCr5kProperties m_properties;

public:
   CCr5kFootprintLayer();

   CString getName() const { return m_layerName; }
   const CCr5kProperties& getProperties() const { return m_properties; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kFootprintLayers : public CTypedCr5kElementContainer<CCr5kFootprintLayer,ptokLayer>
{
// bnf:          footprintLayers        ::= '(' 'footprintLayer' {footprintLayer} ')' 
// bnf:             footprintLayer         ::= '(' 'layer' layerName { <footprintLayerType> | property } ')' 
// bnf:                footprintLayerType     ::= '(' 'type' ( 'CONDUCTIVE' | 'SYMBOLMARK' | 'SOLDERRESIST' | 'METALMASK' | 'HEIGHTLIMIT' | 'COMPAREA' | 'HOLE' | 'PROHIBIT' | 'UNDEFINED' ) ')' 


//private:
//   CTypedPtrListContainer<CCr5kFootprintLayer*> m_footprintLayers;
//
//public:
//   CCr5kFootprintLayers();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kTechnology : public CCr5kElement
{
private:
   CString                  m_technologyName;
   CCr5kParameters          m_parameters;
   CCr5kFootprintLayers     m_footprintLayers;
   CCr5kNonConductiveLayers m_nonConductiveLayers;
   CCr5kConductiveLayers    m_conductiveLayers;
   CCr5kLayerMaps           m_layerMaps;
   CCr5kProperties          m_properties;

public:
   CCr5kTechnology();

   const CCr5kLayerMaps&           getLayerMaps()           const { return m_layerMaps; }
   const CCr5kFootprintLayers&     getFootPrintLayers()     const { return m_footprintLayers; }
   const CCr5kNonConductiveLayers& getNonConductiveLayers() const { return m_nonConductiveLayers; }
   const CCr5kConductiveLayers&    getConductiveLayers()    const { return m_conductiveLayers; }
   const CCr5kProperties&          getProperties()          const { return m_properties; }

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
class CCr5kTechnologyContainer : public CTypedCr5kElementContainer<CCr5kTechnology,ptokTechnology>
{
// bnf:    technologyContainer    ::= '(' 'technologyContainer' {technology} ')'
// bnf:       technology             ::= '(' 'technology' techName [numberOfConductorLayer] [padstackGroupNameRef] [footprintLayers] 
// bnf:                                                            [nonCondLayers] [condLayers] [subLayers] [layerMaps] {property} ')'

//private:
//   CTypedPtrListContainer<CCr5kTechnology*> m_technologies;
//
//public:
//   CCr5kTechnologyContainer();
//
//   virtual bool read(CCr5kTokenReader& tokenReader);
//   virtual void empty();
public:
   const CCr5kTechnology* getTechnology() const  { return ((getCount() > 0) ? getHead() : NULL); }
};

//_____________________________________________________________________________
class CCr5kLayerDirectoryEntry
{
private:
   LayerStruct& m_layer;
   CString m_baseLayerName;
   CString m_materialName;
   int m_materialNumber;

public:
   CCr5kLayerDirectoryEntry(LayerStruct& layer,const CString& baseLayerName,const CString& materialName,int materialNumber);

   LayerStruct& getLayer()          { return m_layer; }
   CString getLayerName()     const { return m_layer.getName(); }
   CString getBaseLayerName() const { return m_baseLayerName; }
   CString getMaterialName()  const { return m_materialName; }
   int getMaterialNumber()    const { return m_materialNumber; }
};

//_____________________________________________________________________________
class CCr5kLayerDirectoryEntryList
{
private:
   CTypedPtrListContainer<CCr5kLayerDirectoryEntry*> m_list;

public:
   CCr5kLayerDirectoryEntryList();

   void addReference(CCr5kLayerDirectoryEntry* entry) { m_list.AddTail(entry); }
   POSITION getHeadPosition() const { return m_list.GetHeadPosition(); }
   CCr5kLayerDirectoryEntry* getNext(POSITION& pos) const { return m_list.GetNext(pos); }
};

//_____________________________________________________________________________
class CCr5kLayerDirectory
{
private:
   CTypedPtrArrayContainer<CCr5kLayerDirectoryEntry*> m_layerArray;

public:
   void addLayer(LayerStruct& layer,const CString& baseLayerName,const CString& materialName,int materialNumber);
   void getBaseLayerNameList(CCr5kLayerDirectoryEntryList& entryList,const CString& baseLayerName);
   void getMatchingBaseLayerNameList(CCr5kLayerDirectoryEntryList& entryList,const CString& baseLayerName);
   void getMatchingLayerNameList(CCr5kLayerDirectoryEntryList& entryList,const CString& layerName);

};

//_____________________________________________________________________________
class CCr5kRegularExpression : public CRegularExpression
{
public:
   CCr5kRegularExpression();
   CCr5kRegularExpression(const char* regularExpression,bool anchorEntire=false);
   CCr5kRegularExpression(const char* regularExpression,const char* substituteExpression,bool anchorEntire=false);
};

//_____________________________________________________________________________
class CCr5kLayerLink
{
private:
   CString m_layerName;
   const CCr5kFootprintLayer* m_footprintLayer;
   const CCr5kConductiveLayer* m_conductiveLayer;
   const CCr5kNonConductiveLayer* m_nonConductiveLayer;
   LayerStruct* m_ccLayer;
   Cr5kTokenTag m_layerType;
   bool m_topFlag;
   bool m_bottomFlag;
   CMaskBool m_referenceMask;

public:
   //CCr5kLayerLink(const CString& layerName);
   CCr5kLayerLink(const CCr5kFootprintLayer& footprintLayer);
   CCr5kLayerLink(const CCr5kConductiveLayer& conductiveLayer);
   CCr5kLayerLink(const CCr5kNonConductiveLayer& nonConductiveLayer);

   CString getLayerName()    const { return m_layerName; }
   const CCr5kFootprintLayer*     getFootprintLayer()     const { return m_footprintLayer;     }
   const CCr5kConductiveLayer*    getConductiveLayer()    const { return m_conductiveLayer;    }
   const CCr5kNonConductiveLayer* getNonConductiveLayer() const { return m_nonConductiveLayer; }

   LayerStruct* getCcLayer() const { return m_ccLayer; }
   void setCcLayer(LayerStruct* layer) { m_ccLayer = layer; }

   Cr5kTokenTag getLayerType() const { return m_layerType; }
   void setLayerType(Cr5kTokenTag type) { m_layerType = type; }

   bool isTop() const { return m_topFlag; }
   void setTop(bool flag) { m_topFlag = flag; }

   bool isBottom() const { return m_bottomFlag; }
   void setBottom(bool flag) { m_bottomFlag = flag; }

   bool isReferencedBy(int layerIndex) const;
   void addReference(int layerIndex);

   void dump(CWriteFormat& writeFormat) const;
};

//_____________________________________________________________________________
class CCr5kLayerLinkMap
{
private:
   CTypedPtrArrayWithMap<CCr5kLayerLink> m_map;

public:
   CCr5kLayerLinkMap(bool isContainer);

   //CCr5kLayerLink* addLayer(const CString& layerName);
   //CCr5kLayerLink* addLayer(int layerNumber);
   CCr5kLayerLink* addLayer(CCr5kLayerLink* layerLink);

   CCr5kLayerLink* addLayer(const CCr5kFootprintLayer& footprintLayer);
   CCr5kLayerLink* addLayer(const CCr5kConductiveLayer& conductiveLayer);
   CCr5kLayerLink* addLayer(const CCr5kNonConductiveLayer& nonConductiveLayer);

   CCr5kLayerLink* lookup(const CString& layerName) const;
   CCr5kLayerLink* getAt(int layerNumber) const;
   int getSize() const { return m_map.getSize(); }
   bool containsLayer(const CString& layerName) const;

   void dump(CWriteFormat& writeFormat) const;
};

//_____________________________________________________________________________
class CCr5kLayerLinkMaps
{
private:
   CCr5kLayerLinkMap m_layerMap;
   CCr5kLayerLinkMap m_conductiveLayerMap;
   CCr5kLayerLinkMap m_nonConductiveLayerMap;
   CCr5kLayerLinkMap m_footPrintLayerMap;
   CCr5kLayerDirectory m_layerDirectory;

public:
   CCr5kLayerLinkMaps();

   CCr5kLayerDirectory& getLayerDirectory();

   void addLayer(const CCr5kFootprintLayer& footprintLayer);
   void addLayer(const CCr5kConductiveLayer& conductiveLayer);
   void addLayer(const CCr5kNonConductiveLayer& nonConductiveLayer);

   LayerStruct& getDefinedLayer(CZukenCr5000Reader& zukenReader,const CString& layerName,int materialNumber,bool mirroredLayerFlag);
   void calculateNonConductiveLayerTypes();
   bool isLayerInConductiveRange(const CString& layerName,int fromLayerIndex,int toLayerIndex) const;
   bool isConductiveLayer(const CString& layerName) const;
   bool isConductiveLayerAssociate(const CString& layerName) const;
   int getMaximumConductiveLayerNumber() const;

   void dump(CWriteFormat& writeFormat) const;
};

//_____________________________________________________________________________
class CCr5kBoardFile : public CCr5kElement
{
private:
   CCr5kHeader              m_header;
   CCr5kTechnologyContainer m_technologyContainer;
   CCr5kBoardContainer      m_boardContainer;

public:
   CCr5kBoardFile();

   const CCr5kBoardContainer&      getBoardContainer()      const { return m_boardContainer; }
   const CCr5kTechnologyContainer& getTechnologyContainer() const { return m_technologyContainer; }

   void linkComponents(CCr5kLinkedComponents& linkedComponents,COperationProgress* progress);
   void generateLayerLinkMaps(CCr5kLayerLinkMaps& layerLinkMaps);

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();

   Cr5kTokenTag getUnits() const { return m_header.getUnits(); }
};

//_____________________________________________________________________________
class CCr5kPanelFile : public CCr5kBoardFile
{
public:
   CCr5kPanelFile();

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

//_____________________________________________________________________________
template<class ELEMENT_TYPE>
class CTypedCr5kLinkedElementToGeometry
{
private:
   ELEMENT_TYPE& m_element;
   BlockStruct* m_topGeometry;
   BlockStruct* m_bottomGeometry;
   CBasesVector m_topOffsetAndRotation;
   CBasesVector m_bottomOffsetAndRotation;

public:
   CTypedCr5kLinkedElementToGeometry(ELEMENT_TYPE& element)
   : m_element(element)
   , m_topGeometry(NULL)
   , m_bottomGeometry(NULL)
   {
   }

   ELEMENT_TYPE& getElement() { return m_element; }

   BlockStruct* getGeometry(bool topFlag) const { return (topFlag ? m_topGeometry : m_bottomGeometry); }
   void setGeometry(bool topFlag,BlockStruct* geometry) { if (topFlag) { m_topGeometry = geometry; } else { m_bottomGeometry = geometry; } }

   const CBasesVector& getOffsetAndRotation(bool topFlag) const { return (topFlag ? m_topOffsetAndRotation : m_bottomOffsetAndRotation); }
   void setOffsetAndRotation(bool topFlag,const CBasesVector& offsetAndRotation) { if (topFlag) { m_topOffsetAndRotation = offsetAndRotation; } else { m_bottomOffsetAndRotation = offsetAndRotation; } }
};

//_____________________________________________________________________________
template<class ELEMENT_TYPE>
class CTypedCr5kLinkedElementToGeometryMapContainer
{
private:
   CTypedMapStringToPtrContainer<CTypedCr5kLinkedElementToGeometry<ELEMENT_TYPE>*> m_elements;

public:
   CTypedCr5kLinkedElementToGeometryMapContainer(int hashTableSize)
   { 
      m_elements.InitHashTable(nextPrime2n(hashTableSize));
   }

   void empty()
   {
      m_elements.empty();
   }

   void add(ELEMENT_TYPE& element)
   {
      CTypedCr5kLinkedElementToGeometry<ELEMENT_TYPE>* linkedElement = new CTypedCr5kLinkedElementToGeometry<ELEMENT_TYPE>(element);
      m_elements.setAt(element.getName(),linkedElement);
   }

   CTypedCr5kLinkedElementToGeometry<ELEMENT_TYPE>* getElement(const CString& elementName)
   {
      CTypedCr5kLinkedElementToGeometry<ELEMENT_TYPE>* element;

      if (! m_elements.Lookup(elementName,element))
      {
         element = NULL;
      }

      return element;
   }

   int getCount() const
   {
      return m_elements.GetCount();
   }
};

//_____________________________________________________________________________
//class CCr5kLinkedPad
//{
//private:
//   CCr5kPad& m_pad;
//   BlockStruct* m_geometry;
//
//public:
//   CCr5kLinkedPad(CCr5kPad& m_pad);
//
//};
//
////_____________________________________________________________________________
//class CCr5kLinkedPads
//{
//private:
//   CTypedMapStringToPtrContainer<CCr5kLinkedPad*> m_pads;
//
//public:
//   CCr5kLinkedPads();
//
//   void add(CCr5kPad& pad);
//
//   void empty();
//};

//_____________________________________________________________________________
class CCr5kLinkedPads : public CTypedCr5kLinkedElementToGeometryMapContainer<CCr5kPad>
{
public:
   CCr5kLinkedPads(int hashTableSize) : CTypedCr5kLinkedElementToGeometryMapContainer<CCr5kPad>(hashTableSize)
   {
   }

};

////_____________________________________________________________________________
//class CCr5kLinkedPadstack
//{
//private:
//   CCr5kPadstack& m_padstack;
//   BlockStruct* m_geometry;
//
//public:
//   CCr5kLinkedPadstack(CCr5kPadstack& padstack);
//};
//
////_____________________________________________________________________________
//class CCr5kLinkedPadstacks
//{
//private:
//   CTypedMapStringToPtrContainer<CCr5kLinkedPadstack*> m_padstacks;
//
//public:
//   CCr5kLinkedPadstacks();
//
//   void add(CCr5kPadstack& padstack);
//
//   void empty();
//};

//_____________________________________________________________________________
class CCr5kLinkedPadstacks : public CTypedCr5kLinkedElementToGeometryMapContainer<CCr5kPadstack>
{
public:
   CCr5kLinkedPadstacks(int hashTableSize) : CTypedCr5kLinkedElementToGeometryMapContainer<CCr5kPadstack>(hashTableSize)
   {
   }

};

////_____________________________________________________________________________
//class CCr5kLinkedFootprint
//{
//private:
//   CCr5kFootprint& m_footprint;
//   BlockStruct* m_geometry;
//
//public:
//   CCr5kLinkedFootprint(CCr5kFootprint& footprint);
//};
//
////_____________________________________________________________________________
//class CCr5kLinkedFootprints
//{
//private:
//   CTypedMapStringToPtrContainer<CCr5kLinkedFootprint*> m_footprints;
//
//public:
//   CCr5kLinkedFootprints();
//
//   void add(CCr5kFootprint& footprint);
//
//   void empty();
//};

//_____________________________________________________________________________
class CCr5kLinkedFootprints : public CTypedCr5kLinkedElementToGeometryMapContainer<CCr5kFootprint>
{
public:
   CCr5kLinkedFootprints(int hashTableSize) : CTypedCr5kLinkedElementToGeometryMapContainer<CCr5kFootprint>(hashTableSize)
   {
   }

};

//_____________________________________________________________________________
template<class ELEMENT_TYPE>
class CTypedCr5kLinkedElementToData
{
private:
   ELEMENT_TYPE& m_element;
   DataStruct* m_data;

public:
   CTypedCr5kLinkedElementToData(ELEMENT_TYPE& element)
   : m_element(element)
   , m_data(NULL)
   {
   }

   ELEMENT_TYPE& getElement() { return m_element; }
   DataStruct* getData() { return m_data; }
   void setData(DataStruct* data) { m_data = data; }
};

//_____________________________________________________________________________
template<class ELEMENT_TYPE>
class CTypedCr5kLinkedElementToDataListContainer
{
private:
   CTypedPtrListContainer<CTypedCr5kLinkedElementToData<ELEMENT_TYPE>*> m_elements;

public:
   CTypedCr5kLinkedElementToDataListContainer()
   {
   }

   void empty()
   {
      m_elements.empty();
   }

   void add(ELEMENT_TYPE& element)
   {
      CTypedCr5kLinkedElementToData<ELEMENT_TYPE>* linkedElement = new CTypedCr5kLinkedElementToData<ELEMENT_TYPE>(element);
      m_elements.AddTail(linkedElement);
   }

   POSITION getHeadPosition() const
   {
      return m_elements.GetHeadPosition();
   }

   CTypedCr5kLinkedElementToData<ELEMENT_TYPE>* getNext(POSITION& pos)
   {
      return m_elements.GetNext(pos);
   }

   int getCount() const
   {
      return m_elements.GetCount();
   }
};

////_____________________________________________________________________________
//class CCr5kLinkedComponent
//{
//private:
//   CCr5kComponent& m_component;
//   DataStruct* m_componentData;
//
//public:
//   CCr5kLinkedComponent(CCr5kComponent& component);
//};
//
////_____________________________________________________________________________
//class CCr5kLinkedComponents
//{
//private:
//   CTypedPtrListContainer<CCr5kLinkedComponent*> m_components;
//
//public:
//   CCr5kLinkedComponents();
//
//   POSITION getHeadPosition() const;
//   CCr5kLinkedComponent* getNext(POSITION& pos);
//   void add(CCr5kComponent& component);
//
//   void empty();
//};

//_____________________________________________________________________________
class CCr5kLinkedComponents : public CTypedCr5kLinkedElementToDataListContainer<CCr5kComponent>
{
};

//_____________________________________________________________________________
class CCr5kMaterials
{
private:
   CMapStringToString m_materialsMap;

public:
   CCr5kMaterials();

   void setAt(int materialNumber,const CString& materialName);
   CString getMaterialName(int materialNumber) const;

private:
   CString getKey(int materialNumber) const;
};

//_____________________________________________________________________________
class CZukenCr5000LayerMapCorrespondence
{
private:
   CString m_footLayerName;
   CStringList m_boardLayers;

public:
   CZukenCr5000LayerMapCorrespondence(const CString& footLayerName);

   void addBoardLayer(const CString& boardLayerName);

   const CStringList& getBoardLayers() const { return m_boardLayers; }
};

//_____________________________________________________________________________
class CZukenCr5000LayerMap
{
private:
   CTypedMapStringToPtrContainer<CZukenCr5000LayerMapCorrespondence*> m_correspondences;
   CStringList m_emptyList;

public:
   CZukenCr5000LayerMap();
   void empty();

   void set(const CCr5kLayerMap& layerMap);

   const CStringList& getMappedBoardLayers(const CString& footLayerName) const;
};

//_____________________________________________________________________________
class CZukenCr5000LayerMapping
{
private:
   CZukenCr5000LayerMap m_mapA;
   CZukenCr5000LayerMap m_mapB;
   CZukenCr5000LayerMap m_mapAThru;
   CZukenCr5000LayerMap m_mapBThru;

public:
   CZukenCr5000LayerMapping();
   ~CZukenCr5000LayerMapping();
   void empty();

   void set(const CCr5kTechnologyContainer& technologyContainer,CWriteFormat& writeFormat);

   const CZukenCr5000LayerMap& getLayerMap(bool thruFlag,bool topFlag) const;
   const CStringList& getMappedBoardLayers(bool thruFlag,bool topFlag,const CString& footLayerName) const;
   //virtual int mapDataToLayers(CDataList& dataList,DataStruct* data,bool thruFlag,bool topFlag,const CString& footLayerName) const;
};

////_____________________________________________________________________________
//class CZukenCr5000IdentityLayerMapping : public CZukenCr5000LayerMapping
//{
//public:
//   virtual int mapDataToLayers(CDataList& dataList,DataStruct* data,bool thruFlag,bool topFlag,const CString& footLayerName) const;
//};

//_____________________________________________________________________________
class CPadstackBuilder
{
private:
   CZukenCr5000Reader& m_zukenReader;
   //int m_fromLayerIndex;
   //int m_toLayerIndex;
   CDataList m_dataList;

public:
   CPadstackBuilder(CZukenCr5000Reader& zukenReader/*,int fromLayerIndex,int toLayerIndex*/);

   CCamCadData& getCamCadData();
   const CDataList& getDataList() const;

   void segregateApertures(CDataList& apertureDataList,CDataList& dataList);
   void takeData(CDataList& dataList,int fromLayerIndex,int toLayerIndex);
   void copyData(const CDataList& dataList);
   void buildPadstack(CDataList& dataList,const CString& padstackName,int fileNumber,const CTMatrix& pinToCompMatrix);

private:
   void filterDataList(CDataList& dataList,int fromLayerIndex,int toLayerIndex) const;
};

//_____________________________________________________________________________
class CNamedDataList : public CObject
{
private:
   CString m_name;
   CBasesVector m_basesVector;
   int m_fromLayerNumber;
   int m_toLayerNumber;
   CDataList m_dataList;

public:
   CNamedDataList(const CString& name,const CBasesVector& basesVector,
      int fromLayerNumber,int toLayerNumber,bool isContainer=true);

   CString getName()        const { return m_name; }
   const CBasesVector& getBasesVector() const { return m_basesVector; }
   int getFromLayerNumber() const { return m_fromLayerNumber; }
   int getToLayerNumber()   const { return m_toLayerNumber; }

   CDataList& getDataList() { return m_dataList; }
};

//_____________________________________________________________________________
class CNamedDataListMap
{
private:
   CTypedMapSortStringToObContainer<CNamedDataList> m_dataListMap;

public:
   void empty();
   CNamedDataList& add(const CString& name,const CBasesVector& basesVector);
   CNamedDataList& add(CString name,const CBasesVector& basesVector,int fromLayerNumber,int toLayerNumber);
   void rewind(int& index);
   bool next(CNamedDataList*& namedDataList,CString& key,int& index);
   int getCount() const { return m_dataListMap.GetCount(); }
};

//_____________________________________________________________________________
class CCr5kMappedLayerDataList
{
private:
   CZukenCr5000Reader& m_zukenReader;
   CDataList& m_managedDataList;
   const CZukenCr5000LayerMapping* m_layerMapping;
   CBoolStack m_enableLayerMappingStack;
   CDataList m_bufferDataList;
   CDataList* m_dataList;
   CTypedPtrArray<CPtrArray,CDataList*> m_dataListStack;
   bool m_thruFlag;
   bool m_topFlag;
   bool m_padstackCachedModeFlag;
   CBasesVector m_pinBasesVector;
   int m_cacheStackCount;
   CNamedDataListMap m_padstackCacheMap;

public:
   CCr5kMappedLayerDataList(CZukenCr5000Reader& zukenReader,CDataList& managedDataList);
   CCr5kMappedLayerDataList(CZukenCr5000Reader& zukenReader,CDataList& managedDataList,const CZukenCr5000LayerMapping& layerMapping,bool enableLayerMapping=true);
   ~CCr5kMappedLayerDataList();

   CCamCadDatabase& getCamCadDatabase();
   CCamCadData&     getCamCadData();
   const CDataList& getDataList() const { return *m_dataList; }

   void addNetName(const CString& netName);
   void addRefName(const CString& refName);
   void unfillPolys();
   void transform(const CTMatrix& matrix);

   bool getThruFlag() const { return m_thruFlag; }
   void setThruFlag(bool thruFlag) { m_thruFlag = thruFlag; }

   bool getTopFlag() const { return m_topFlag; }
   void setTopFlag(bool topFlag) { m_topFlag = topFlag; }

   bool isLayerMappingEnabled() const;
   void pushEnableLayerMapping(bool enableFlag);
   bool popEnableLayerMapping();

   void pushBuffer();
   void popBuffer();
   void pushPadBuffer(const CString& padName,const CBasesVector& basesVector);
   void pushPadstackBuffer(const CString& padstackName,const CBasesVector& basesVector,int fromLayerNumber,int toLayerNumber);
   void startPadstackCache(const CBasesVector& pinBasesVector,bool enableFlag);

   bool getPadstackCacheModeFlag() const { return m_padstackCachedModeFlag; }
   void setPadstackCacheModeFlag(bool flag) { m_padstackCachedModeFlag = flag; }

   const CBasesVector& getPinBasesVector() const { return m_pinBasesVector; }

   void instantiatePadstackCache(const CTMatrix& pinMatrix,const CTMatrix& componentToBoardMatrix,bool bottomPlacedComponentFlag,const CString& refDes,const CString& pinName);
   BlockStruct* getConglomeratedPadstack(CBasesVector& pinBasesVector,const CTMatrix& pinMatrix,const CTMatrix& componentToBoardMatrix,const CString& refDes,const CString& pinName);
   BlockStruct* getRestructuredPadstack(CBasesVector& pinBasesVector,const CTMatrix& pinMatrix,const CTMatrix& componentToBoardMatrix,
      bool bottomPlacedComponentFlag,const CString& refDes,const CString& pinName);

   int addTail(DataStruct*& data,const CString& unmappedLayerName,int materialNumber=0,int fromLayerNumber=0,int toLayerNumber=0,
      const CCr5kConductivePadStackPad* conductivePadStackPad=NULL,Cr5kTokenTag padStatus=tokUndefined);
   int addTailWithoutLayerMapping(DataStruct*& data,int layerIndex);
};

//_____________________________________________________________________________
typedef CTypedCr5kLinkedElementToGeometry<CCr5kPad> CCr5kLinkedPad;
typedef CTypedCr5kLinkedElementToGeometry<CCr5kPadstack> CCr5kLinkedPadstack;
typedef CTypedCr5kLinkedElementToGeometry<CCr5kFootprint> CCr5kLinkedFootprint;

//_____________________________________________________________________________
class CCr5kLinkedGeometries
{
private:
   CCr5kLinkedPads       m_linkedPads;
   CCr5kLinkedPadstacks  m_linkedPadstacks;
   CCr5kLinkedFootprints m_linkedFootprints;
   //CCr5kLinkedComponents m_linkedComponents;

public:
   CCr5kLinkedGeometries();

   void initialize(CCr5kBoardFile& boardFile,CCr5kFootprintFile& footprintFile);

   CCr5kLinkedPad*       getLinkedPad(const CString& padName);
   CCr5kLinkedPadstack*  getLinkedPadstack(const CString& padstackName);
   CCr5kLinkedFootprint* getLinkedFootprint(const CString& geometryName);

   //CCr5kLinkedComponents& getLinkedComponents() { return m_linkedComponents; }
};

//_____________________________________________________________________________
class CCr5kMirroredLayerEntry
{
private:
   CString m_commandString;
   CString m_layerExpression1;
   CString m_layerExpression2;
   bool    m_baseLayerFlag;

public:
   CCr5kMirroredLayerEntry(const CString& commandString,const CString& layerExpression1,const CString& layerExpression2,bool baseLayerFlag);

   CString getCommandString()    const { return m_commandString; }
   CString getLayerExpression1() const { return m_layerExpression1; }
   CString getLayerExpression2() const { return m_layerExpression2; }
   bool    getBaseLayerFlag()    const { return m_baseLayerFlag; }
};

//_____________________________________________________________________________
class CCr5kMirroredLayers
{
private:
   CTypedPtrListContainer<CCr5kMirroredLayerEntry*> m_mirroredLayersList;

public:
   CCr5kMirroredLayers();

   void addLayerEntry(const CString& commandString,const CString& layerExpression1,const CString& layerExpression2);
   void addBaseLayerEntry(const CString& commandString,const CString& layerExpression1,const CString& layerExpression2);

   POSITION getHeadPosition()                      const { return m_mirroredLayersList.GetHeadPosition(); }
   CCr5kMirroredLayerEntry* getNext(POSITION& pos) const { return m_mirroredLayersList.GetNext(pos); }

   int getCount()    { return m_mirroredLayersList.GetCount(); }
};

//_____________________________________________________________________________
class CZukenCr5000Reader
{
private:
   CCamCadDatabase& m_camCadDatabase;
   CCEtoODBDoc& m_camCadDoc;
   CString m_projectName;
   CCr5kFootprintFile m_boardFootprintFile;
   CCr5kFootprintFile m_panelFootprintFile;
   CCr5kBoardFile m_boardFile;
   CCr5kBoardFile m_panelFile;

   mutable CWriteFormat* m_logFile;
   CString m_logFileDirectory;
   CString m_logFileName;  // name and extension only, no path

   // Suffix to append to geometry name for bottom side geometries.
   CString m_cr5kBottomSuffix;

   int m_unnamedPadCount;

   CWriteFormat* m_debugFile;
   CString m_debugFilePath;

   // Instantiation
   FileStruct*           m_cadFile;
   FileStruct*           m_pcbFile;
   FileStruct*           m_panFile;
   BlockStruct*          m_subBoardGeometry;
   int                   m_startingBlockIndex;
   BlockStruct*          m_testAccessGeometry;
   BlockStruct*          m_testProbeGeometry;

   CCr5kLinkedGeometries m_linkedBoardGeometries;
   CCr5kLinkedGeometries m_linkedPanelGeometries;


   CCr5kLinkedComponents m_linkedBoardComponents;


   CCr5kLinkedComponents m_linkedPanelComponents;

   CCr5kLayerLinkMaps    m_layerLinkMaps;
   CCr5kMaterials        m_materials;
   CZukenCr5000LayerMapping m_layerMapping;
   //CZukenCr5000IdentityLayerMapping m_identityLayerMapping;
   mutable int m_sourceLineNumberKeywordIndex;
   mutable int m_parentReferenceKeywordIndex;
   mutable int m_partKeywordIndex;
   mutable int m_packageKeywordIndex;
   mutable int m_stockIdKeywordIndex;
   CMessageFilter m_messageFilter;
   CString m_padstackGeometryPrefix;
   bool m_panelDataFlag;

   // Options
   bool m_optionLibraryMode;  // when true, component geometries are created from the ftf file data,
                              // when false, a separate geometry is created for each component instance from the pcf file data.
   bool m_optionAdjustLayerNames;
   bool m_optionAddSourceLineNumbers;
   bool m_optionAddPadstackDescriptor;
   bool m_optionUseOldReader;
   bool m_optionTestFtfRead;
   bool m_optionTestPcfRead;
   bool m_optionTestPnfRead;
   bool m_optionInstantiateConnectPads;
   bool m_optionInstantiateNoconnectPads;
   bool m_optionInstantiateThermalPads;
   bool m_optionInstantiateClearancePads;
   bool m_optionInstantiateFillets;
   bool m_optionPurgeUnusedGeometries;
   bool m_optionShowPurgeReport;
   bool m_optionIgnoreOutOfBoard;
   bool m_optionGenerateLoadedAttribute;
   bool m_optionUsePnfAsPcf;
   bool m_optionShowHiddenRefDes;
   bool m_optionRestructurePadstacks;
   bool m_optionConglomeratePadstacks;
   bool m_optionSegregateGeneratedBondwires;
   bool m_optionSegregateBondwiresByDiameter;
   bool m_optionDefaultVariantLoadedFlag;
   bool m_optionGenerateMirroredLayers;
   bool m_optionEnableLayerMirroring;
   bool m_optionMirrorElectricalLayers;
   bool m_optionEnablePanelFtfFile;
   bool m_optionGenerateDebugFile;
   bool m_optionGenerateLogFile;
   bool m_optionDisplayErrorMessages;
   bool m_optionSupportMultiByteCharacters;
   bool m_optionConvertComplexApertures;
   bool m_optionEnableReverseFootprints;
   double m_optionDefaultViaPadDiameter;
   double m_optionDefaultViaDrillDiameter;
   double m_optionProbeDiameter;
   double m_optionProbeDrillDiameter;
   CString m_optionProbeDescriptor;
   double m_optionTestAccessDiameter;
   CString m_optionMounterMachineName;
   double m_optionTextHeightFactor;
   double m_optionTextWidthFactor;
   double m_optionAttributeHeightFactor;
   double m_optionAttributeWidthFactor;
   CMapStringToString m_optionMirroredLayers;
   CCr5kMirroredLayers m_mirroredLayers;
   CMapStringToString m_optionLayerTypes;
   CRegularExpressionList m_optionFiducialExpressionList;
   CRegularExpressionList m_optionTestPointExpressionList;
   CRegularExpressionList m_optionToolingExpressionList;
   CRegularExpressionList m_optionCompOutlineExpressionList;
   bool m_optionUseOutlineWidth;
   CGeometryConsolidatorControl m_consolidatorControl;

public:
   CZukenCr5000Reader(CCamCadDatabase& camCadDatabase,const CString& projectName);
   ~CZukenCr5000Reader();
   void empty();

   CString getNewPadName()    { CString n; n.Format("PAD_%05d", ++m_unnamedPadCount); return n; }

   // accessors
   const CCr5kMaterials& getMaterials() const { return m_materials; }
   bool getPanelDataFlag() const { return m_panelDataFlag; }
   CCr5kLayerLinkMaps& getLayerLinkMaps() { return m_layerLinkMaps; }
   CCr5kMirroredLayers& getMirroredLayers() { return m_mirroredLayers; }

   bool read(const CString& filePath,Cr5kTokenTag startToken);
   bool read(CCr5kTokenReader& tokenReader,Cr5kTokenTag startToken);
   bool readDstFile(const CString& filePath);
   void error(const char* format, ...);

   // options
   bool loadSettings(const CString& filePath);
   PageUnitsTag parseUnitsParameter(CString unitsString,const CString& commandLineString);

   void addMirroredLayer(const CString& commandString,const CString& layerName1,const CString& layerName2);
   void addMirroredBaseLayer(const CString& commandString,const CString& layerName1,const CString& layerName2);
   CString getMirroredLayer(CString layerName) const;
   void    addFiducialExpression(const CString& regularExpressionString);
   void   addTestPointExpression(const CString& regularExpressionString);
   void     addToolingExpression(const CString& regularExpressionString);
   void addCompOutlineExpression(const CString& regularExpressionString);
   bool getOptionAdjustLayerNames()            const { return m_optionAdjustLayerNames; }
   bool getOptionAddSourceLineNumbers()        const { return m_optionAddSourceLineNumbers; }
   bool getOptionAddPadstackDescriptor()       const { return m_optionAddPadstackDescriptor; }
   bool getOptionUseOldReader()                const { return m_optionUseOldReader; }
   bool getOptionTestFtfRead()                 const { return m_optionTestFtfRead; }
   bool getOptionTestPcfRead()                 const { return m_optionTestPcfRead; }
   bool getOptionTestPnfRead()                 const { return m_optionTestPnfRead; }
   bool getOptionInstantiateConnectPads()      const { return m_optionInstantiateConnectPads; }
   bool getOptionInstantiateNoconnectPads()    const { return m_optionInstantiateNoconnectPads; }
   bool getOptionInstantiateThermalPads()      const { return m_optionInstantiateThermalPads; }
   bool getOptionInstantiateClearancePads()    const { return m_optionInstantiateClearancePads; }
   bool getOptionInstantiateFillets()          const { return m_optionInstantiateFillets; }
   bool getOptionPurgeUnusedGeometries()       const { return m_optionPurgeUnusedGeometries; }
   bool getOptionShowPurgeReport()             const { return m_optionShowPurgeReport; }
   bool getOptionIgnoreOutOfBoard()            const { return m_optionIgnoreOutOfBoard; }
   bool getOptionGenerateLoadedAttribute()     const { return m_optionGenerateLoadedAttribute; }
   bool getOptionUsePnfAsPcf()                 const { return m_optionUsePnfAsPcf; }
   bool getOptionShowHiddenRefDes()            const { return m_optionShowHiddenRefDes; }
   bool getOptionRestructurePadstacks()        const { return m_optionRestructurePadstacks; }
   bool getOptionConglomeratePadstacks()       const { return m_optionConglomeratePadstacks; }
   bool getOptionLibraryMode()                 const { return  m_optionLibraryMode; }
   bool getOptionInstanceSpecificMode()        const { return !m_optionLibraryMode; }
   bool getOptionDefaultVariantLoadedFlag()    const { return m_optionDefaultVariantLoadedFlag; }
   bool getOptionGenerateMirroredLayers()      const { return m_optionGenerateMirroredLayers; }   
   bool getOptionEnableLayerMirroring()        const { return m_optionEnableLayerMirroring; }   
   bool getOptionMirrorElectricalLayers()      const { return m_optionMirrorElectricalLayers; }   
   bool getOptionEnablePanelFtfFile()          const { return m_optionEnablePanelFtfFile; }   
   bool getOptionGenerateDebugFile()           const { return m_optionGenerateDebugFile; }   
   bool getOptionGenerateLogFile()             const { return m_optionGenerateLogFile; }   
   bool getOptionDisplayErrorMessages()        const { return m_optionDisplayErrorMessages; }   
   bool getOptionSupportMultiByteCharacters()  const { return m_optionSupportMultiByteCharacters; }   
   bool getOptionConvertComplexApertures()     const { return m_optionConvertComplexApertures; }
   bool getOptionEnableReverseFootprints()     const { return m_optionEnableReverseFootprints; }   
   double getOptionDefaultViaPadDiameter()     const { return m_optionDefaultViaPadDiameter; }
   double getOptionProbeDiameter()             const { return m_optionProbeDiameter; }   
   double getOptionProbeDrillDiameter()        const { return m_optionProbeDrillDiameter; }   
   CString getOptionProbeDescriptor()          const { return m_optionProbeDescriptor; }   
   double getOptionTestAccessDiameter()        const { return m_optionTestAccessDiameter; }   
   double getOptionDefaultViaDrillDiameter()   const { return m_optionDefaultViaDrillDiameter; }
   CString getOptionMounterMachineName()       const { return m_optionMounterMachineName; }   
   double getOptionTextHeightFactor()          const { return m_optionTextHeightFactor; }
   double getOptionTextWidthFactor()           const { return m_optionTextWidthFactor; }
   double getOptionAttributeHeightFactor()     const { return m_optionAttributeHeightFactor; }
   double getOptionAttributeWidthFactor()      const { return m_optionAttributeWidthFactor; }
   bool getOptionUseOutlineWidth()           const { return m_optionUseOutlineWidth; }

   CGeometryConsolidatorControl& GetConsolidatorControl()  { return m_consolidatorControl; }

   // logging
   CWriteFormat& getLog();
   void closeLog();
   CWriteFormat& getDebug();
   CMessageFilter& getMessageFilter() { return m_messageFilter; }
   void unimplementedWarning(const char* message) { getMessageFilter().formatMessage(message); }
   CString getLogFilePath(const CString& filePathString);
   void setLogFileDirectoryPath(const CString& directoryPathString);
   void formatErrorMessage(const char* format,...);

   // data instantiation
   CCamCadDatabase& getCamCadDatabase() { return m_camCadDatabase; }
   CCEtoODBDoc&      getCamCadDoc()      { return m_camCadDoc;      }
   CCamCadData&     getCamCadData()     { return m_camCadDoc.getCamCadData(); }

   const CUnits& getUnits() const { return m_camCadDoc.getUnits(); }
   const CString& getPadstackGeometryPrefix() const { return m_padstackGeometryPrefix; }
   //double convertCr5kUnitsToPageUnits(double cr5kUnits) const;
   //void updateCr5kUnits(Cr5kTokenTag token);
   FileStruct* getCadFile() { return m_cadFile; }
   FileStruct* getPcbFile() { return m_pcbFile; }
   FileStruct* getPanFile() { return m_panFile; }
   int getCadFileNumber() const { return m_cadFile->getFileNumber(); }
   CString adjustCase(const CString& string) const;
   //LayerStruct& getDefinedLayer(CString layerName);
   LayerStruct& getDefinedLayer(const CString& layerName,int materialNumber = -1,bool mirroredLayerFlag = false);
   const CZukenCr5000LayerMapping& getLayerMapping()                 const { return m_layerMapping; }
   //const CZukenCr5000IdentityLayerMapping& getIdentityLayerMapping() const { return m_identityLayerMapping; }
   void performLayerMirroring();

   //LayerStruct& getMaterialLayer(LayerStruct& layer,int materialNumber);
   //LayerStruct& getMaterialLayer(CString layerName,int materialNumber);

   bool isLayerConductiveOrInConductiveRange(const CString& layerName,int fromLayerIndex,int toLayerIndex) const;
   bool isLayerNonConductiveOrInConductiveRange(const CString& layerName,int fromLayerIndex,int toLayerIndex) const;
   bool isLayerInConductiveRange(const CString& layerName,int fromLayerIndex,int toLayerIndex) const;
   bool isConductiveLayer(const CString& layerName) const;
   bool isConductiveLayerAssociate(const CString& layerName) const;
   void normalizeFromToLayerNumbers(int& fromLayerNumber,int& toLayerNumber) const;
   void normalizeLayerNumberSpan(int& fromLayerNumber,int& toLayerNumber) const;

private:
   BlockStruct* getInstantiatedDefaultViaGeometry(int fromLayerNumber,int toLayerNumber);
   BlockStruct* getInstantiatedStandardPadGeometry(const CCr5kGeometry& padGeometry,CBasesVector& apertureOffsetAndRotation);
   BlockStruct* getInstantiatedSimplePadGeometry(const CCr5kGeometry& padGeometry,CBasesVector& apertureOffsetAndRotation);

   void PostProcessComponents();
   void PostProcessComponent(BlockStruct *compBlk);
   DataStruct *GetPinPolyData(BlockStruct *compBlk, CString &outParentReference);

public:
   BlockStruct* getInstantiatedPadGeometry(const CCr5kGeometries& cr5kPadGeometries, const CString& padName, CBasesVector& apertureOffsetAndRotation);
   BlockStruct* getInstantiatedPadGeometry(CCr5kLinkedGeometries& linkedGeometries,const CString& padName,CBasesVector& apertureOffsetAndRotation);

   BlockStruct* getInstantiatedPadstackGeometry(CCr5kLinkedGeometries& linkedGeometries,
      const CString& padstackName,bool placedTopFlag,int fromLayerNumber,int toLayerNumber,
      const CCr5kConductivePadStackPad* conductivePadStackPad);

   BlockStruct* getInstantiatedPadstackGeometry(const CCr5kLayoutPrimitive& primitive, CCr5kLinkedGeometries& linkedGeometries,
      const CString& pinRef, const CString& padstackName, bool placedTopFlag, int fromLayerNumber,int toLayerNumber,
      const CCr5kConductivePadStackPad* conductivePadStackPad);

   BlockStruct* getBoardComponentGeometry(CCr5kLinkedGeometries& linkedGeometries,const CCr5kComponent& component,const CString& geometryName, bool isReverseFootprintProcessed = true);
   BlockStruct* getLibraryComponentGeometry(CCr5kLinkedGeometries& linkedGeometries,const CCr5kComponent& component,const CString& geometryName);
   BlockStruct* getInstantiatedComponentGeometry(CCr5kLinkedGeometries& linkedGeometries,const CCr5kComponent& component);
   BlockStruct* getTestAccessGeometry();
   BlockStruct* getTestProbeGeometry();

   double compareGeometryPins(BlockStruct& geometry1,BlockStruct& geometry2);

   //CCr5kLinkedGeometries& getLinkedPanelGeometries();

   void initializeInstantiator();
   bool instantiateData(CCr5kBoardFile& boardFile,CCr5kLinkedComponents& linkedComponents,CCr5kLinkedGeometries& linkedGeometries,bool panelDataFlag);  
   bool instantiateBoardData();  
   bool instantiatePanelData();  
   //void instantiateComponentGeometryFromLayerData(BlockStruct& geometry,const CCr5kComponent& component,const CCr5kLayoutLayer& layer);
   void instantiateTestPad(const CCr5kLayoutPrimitive& layoutPrimitive,DataStruct& data,const CString& pinRef);
   void instantiateComponentPinData(CCr5kMappedLayerDataList& mappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,const CCr5kCompPins& compPins,const CCr5kComponent& component);
   void instantiateComponentCompPinData(const CCr5kComponent& component);
   void instantiateToeprintCompPinData(const CCr5kToeprintPins& toeprintPins,const CCr5kComponent& component);
   void instantiateToeprintPinData(CCr5kMappedLayerDataList& mappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,const CCr5kToeprintPins& toeprintPins,const CCr5kComponent& component);
   void instantiateLayoutData(CCr5kMappedLayerDataList& mappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,const CCr5kLayout& layout,bool boardLayerFlag,bool boardLevelFlag,bool pinFlag,
      const CString& parentReference,const CString& pinRef,bool filterByConductivePadstackPadFlag,bool componentPlacedBottomFlag,const CTMatrix* matrix=NULL,const CBasesVector* componentBasesVector=NULL,COperationProgress* progress=NULL);
   void instantiateLayerData(CCr5kMappedLayerDataList& mappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,const CCr5kLayoutLayer& layer,bool boardLayerFlag,bool boardLevelFlag,bool pinFlag,const CString& parentReference,
      const CString& pinRef,bool filterByConductivePadstackPadFlag,bool componentPlacedBottomFlag,const CTMatrix* matrix=NULL,const CBasesVector* componentBasesVector=NULL);
   void instantiatePrimitiveData(CCr5kMappedLayerDataList& mappedLayerDataList,CCr5kLinkedGeometries& linkedGeometries,const CString& layerName,bool boardLayerFlag,bool boardLevelFlag,bool pinFlag,
      const CCr5kLayoutPrimitive& primitive,const CString& parentReference,const CString& pinRef,bool filterByConductivePadstackPadFlag,bool componentPlacedBottomFlag,
      const CTMatrix* matrix=NULL,const CBasesVector* componentBasesVector=NULL);

   DataStruct *instantiatePadInsertData(BlockStruct *padGeometry, CBasesVector &apertureOffsetAndRotation,
      CCr5kMappedLayerDataList& mappedLayerDataList,
      CCr5kLinkedGeometries& linkedGeometries,
      const CString& layerName,
      bool boardLayerFlag,
      bool boardLevelFlag,
      bool pinFlag,
      const CCr5kLayoutPrimitive& primitive,
      const CString& parentReference,
      const CString& pinRef,
      bool filterByConductivePadstackPadFlag,
      bool componentPlacedBottomFlag,
      const CTMatrix* matrix,
      const CBasesVector* componentBasesVector);

   DataStruct *CZukenCr5000Reader::getSurfaceGeometryData(CCr5kMappedLayerDataList& mappedLayerDataList,
      const CString& layerName,int materialNumber,const CCr5kGeometry& cr5kGeometry,
      const CString& parentReference, GraphicClassTag graphicClass,
      const CTMatrix* matrix,const CCr5kProperties* properties, bool componentPlacedBottomFlag);

   void instantiateGeometryData(CCr5kMappedLayerDataList& mappedLayerDataList,const CString& layerName,int materialNumber,
      const CCr5kGeometry& cr5kGeometry,const CString& parentReference,GraphicClassTag graphicClass,
      const CTMatrix* matrix=NULL,const CCr5kProperties* primitiveProperties=NULL, bool componentPlacedBottomFlag = false);
   void instantiateMounterInformation(CCr5kMappedLayerDataList& mappedLayerDataList,const CCr5kFootprint& footprint);

   void mirrorLayers();
   void typeLayers();
   CString getBondwireLayerName(bool topFlag,bool isGeneratedFlag,double diameter) const;

   void instantiateComponentParameters(CAttributes& attributes,const CCr5kComponent& cr5kComponent);
   void instantiateComponentAttributes(CAttributes& attributes,const CCr5kComponent& cr5kComponent);
   void instantiateLayoutAttributes(CAttributes& attributes,bool visibleFlag,bool mirroredLayerFlag,const CCr5kLayout& layout,const CTMatrix& matrix);
   void instantiateLayerAttributes(CAttributes& attributes,bool visibleFlag,bool mirroredLayerFlag,const CCr5kLayoutLayer& layer,const CTMatrix& matrix);
   void instantiatePrimitiveAttributes(CAttributes& attributes,bool visibleFlag,bool mirroredLayerFlag,const CString& layerName,
      const CCr5kLayoutPrimitive& primitive,const CTMatrix* matrix=NULL);
   void instantiateGeometryAttributes(CAttributes& attributes,Cr5kTokenTag attributeType,bool visibleFlag,bool mirroredLayerFlag,
      const CString& layerName,int materialNumber,const CCr5kGeometry& geometry,const CTMatrix* matrix=NULL);

   //void instantiateComponentGeometryFromPrimitiveData(BlockStruct& ccGeometry,LayerStruct& ccLayer,const CCr5kLayoutPrimitive& primitive,bool placedTopFlag);
   void instantiateText(CCr5kMappedLayerDataList& mappedLayerDataList,const CString& layerName,int materialNumber,const CCr5kGeometry& geometry,
      const CString& parentReference,const CTMatrix* matrix=NULL,const CCr5kProperties* properties=NULL);
   void generatePolyListFromVertices(CPolyList& polyList,const CCr5kVertices& vertices,double width,bool squareWidthFlag,PolyTypeTag polyType);
   void setProperties(CAttributes& attributes,const CCr5kProperties& properties);

   void setStartingBlockIndex(int blockIndex) { m_startingBlockIndex = blockIndex; }
   void makeBoardOutline(FileStruct& fileStruct,bool panelDataFlag);
   void makeIntoCorrectTypes();

   void setParentReferenceAttribute(DataStruct& data,const CString& parentReference);
   void setSourceLineNumberAttribute(DataStruct& data,const CCr5kElement& element);
   void setSourceLineNumberAttribute(BlockStruct& geometry,const CCr5kElement& element);
   int getSourceLineNumberKeywordIndex();
   int getParentReferenceKeywordIndex();
   void setDeviceTypeAttribute(BlockStruct& geometry,const CString& deviceType);

   static CString getMaterialLayerName(const CString& layerName,const CString& materialName);

   void dts0100713526_CleanUpGeomNames();

private:
};

//_____________________________________________________________________________
class CZukenMapBlockNumberToBlock
{
private:
   CTypedMapIntToPtrContainer<BlockStruct*> m_blockMap;

public:
   CZukenMapBlockNumberToBlock(int hashSize);

   void addBlock(BlockStruct* block);
   bool containsBlock(BlockStruct* block) const;
   POSITION getStartPos() const;
   BlockStruct* getNext(POSITION& pos) const;

};

//_____________________________________________________________________________
class CZukenLayerMirrorer
{
private:
   CZukenCr5000Reader& m_zukenReader;
   int m_hashSize;

   CZukenMapBlockNumberToBlock* m_mirroredGeometries;
   CZukenMapBlockNumberToBlock* m_unmirroredGeometries;
   CZukenMapBlockNumberToBlock* m_tier1MirroredGeometries;
   CZukenMapBlockNumberToBlock* m_tier1UnmirroredGeometries;
   CZukenMapBlockNumberToBlock* m_tier2MirroredGeometries;
   CZukenMapBlockNumberToBlock* m_tier2UnmirroredGeometries;

public:
   CZukenLayerMirrorer(CZukenCr5000Reader& zukenReader);
   ~CZukenLayerMirrorer();

   CZukenCr5000Reader& getZukenReader() { return m_zukenReader; }
   CCamCadDatabase&    getCamCadDatabase() { return m_zukenReader.getCamCadDatabase(); }

   void performLayerMirroring();
   void mirrorLayers();
   void scanTier1Geometries(FileStruct& fileStruct);
   void scanTier2Geometries();
   //void scanGeometries(FileStruct& fileStruct);
   void scanGeometry(BlockStruct& geometry,CZukenMapBlockNumberToBlock& mirroredGeometries,CZukenMapBlockNumberToBlock& unmirroredGeometries,bool mirrorFlag);
   void checkGeometryUsage();
   void mirrorGeometryData();
   void mirrorInsertAttributes(BlockStruct *insertedBlock);
   void mirrorInsertAttributes(BlockStruct *insertedBlock, BlockStruct *datalistBlock);
};

//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
//_____________________________________________________________________________
class CCr5kPrototype : public CCr5kElement
{
public:
   CCr5kPrototype();

   virtual bool read(CCr5kTokenReader& tokenReader);
   virtual void empty();
};

#endif
