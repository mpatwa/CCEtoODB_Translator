// $Header: /CAMCAD/DcaLib/DcaStandardAperture.h 5     5/21/07 3:47p Kurt Van Ness $

#if !defined(__DcaStandardAperture_h__)
#define __DcaStandardAperture_h__

#pragma once

#include "DcaUnits.h"
#include "DcaLib.h"
#include "DcaBasesVector.h"
#include "DcaApertureShape.h"

#define QDimStencilPad         "DimStencilPad"
#define QBrightStencilPad      "BrightStencilPad"
#define QDimOppositeCopperPad  "DimOppositeCopperPad"
#define QDimOppositeStencilPad "DimOppositeStencilPad"
#define QDimCopperPad          "DimCopperPad"
#define QBrightCopperPad       "BrightCopperPad"
#define QDimToolHole           "DimToolHole"
#define QBrightToolHole        "BrightToolHole"
#define QPointAnnotation       "PointAnnotation"
#define QTextAnnotation        "TextAnnotation"
#define QDimensionAnnotation   "DimensionAnnotation"
#define QSilkScreen            "SilkScreen"
#define QOppositeSilkScreen    "OppositeSilkScreen"
#define QAxes                  "Axes"

// class derivation hierarchy                                         normalized
//                                                          makeValid dimensions
//  CStandardApertureShape                                  .           .
//  .  CStandardApertureRectangle                           x           -
//  .  .  CStandardApertureChamferedRectangle               x           012
//  .  .  .  CStandardApertureRoundedRectangle              x           i
//  .  .  .  CStandardApertureBeveledRectangle                           
//  .  .  CStandardApertureDiamond                          o           i
//  .  .  CStandardApertureRectangledPentagon               x           -
//  .  .  CStandardApertureEllipse                          _           i
//  .  .  CStandardApertureHalfOval                         x           i
//  .  .  CStandardApertureHorizontalHexagon                x           -
//  .  .  .  CStandardApertureOctagon                       o           i
//  .  .  .  CStandardApertureVerticalHexagon               x           i
//  .  .  CStandardApertureOval                             x           i
//  .  .  CStandardApertureRectangularThermal               .           01245
//  .  .  .  CStandardApertureRectangularThermalOpenCorners .           i
//  .  .  CStandardApertureNotchedRectangle                 x           0123
//  .  .  .  CStandardApertureVeeCutRectangle               x           i
//  .  .  .  CStandardApertureTabbedRectangle               x           i
//  .  .  .  .  CStandardApertureSpurredRectangle           _           i
//  .  CStandardApertureCircle                              x           -
//  .  .  CStandardApertureSplitCircle
//  .  .  CStandardApertureButterfly                        o           i
//  .  .  CStandardApertureHole                             .           -
//  .  CStandardApertureMoire                               .           0134
//  .  CStandardApertureNull                                .           -
//  .  CStandardApertureRoundDonut                          x           -
//  .  .  CStandardApertureSquareDonut                      .           i
//  .  CStandardApertureRoundThermalRounded                 .           0124
//  .  .  CStandardApertureRoundThermalSquare               .           i
//  .  .  CStandardApertureSquareRoundThermal               .           i
//  .  .  .  CStandardApertureSquareThermal                 .           i
//  .  .  CStandardApertureSquareThermalOpenCorners         .           i
//  .  CStandardApertureSquare                              x           -
//  .  .  CStandardApertureSquareButterfly                  o           i
//  .  CStandardApertureTriangle                            x           -
//  .  CStandardApertureInvalid                             .           -

class CCamCadDatabase;
class CCamCadData;
class DataStruct;
class CDataList;
class BlockStruct;

enum HorizontalPositionTag;
enum VerticalPositionTag;

//_____________________________________________________________________________
enum StandardApertureTypeTag
{
   standardApertureButterfly,
   standardApertureBeveledRectangle,
   standardApertureChamferedRectangle,
   standardApertureCircle,
   standardApertureDiamond,
   standardApertureEllipse,
   standardApertureHalfOval,
   standardApertureHole,
   standardApertureHorizontalHexagon,
   standardApertureMoire,
   standardApertureNotchedRectangle,
   standardApertureNull,
   standardApertureOctagon,
   standardApertureOval,
   standardApertureRectangle,
   standardApertureRectangularThermal,
   standardApertureRectangularThermalOpenCorners,
   standardApertureRoundDonut,
   standardApertureRoundedRectangle,
   standardApertureRoundThermalRounded,
   standardApertureRoundThermalSquare,
   standardApertureSplitCircle,
   standardApertureSpurredRectangle,
   standardApertureSquare,
   standardApertureSquareButterfly,
   standardApertureSquareDonut,
   standardApertureSquareRoundThermal,
   standardApertureSquareThermal,
   standardApertureSquareThermalOpenCorners,
   standardApertureRectangledPentagon,
   standardApertureTabbedRectangle,
   standardApertureTriangle,
   standardApertureVeeCutRectangle,
   standardApertureVerticalHexagon,
   standardApertureInvalid,
   standardApertureUndefined,
   standardApertureFirstValid=standardApertureButterfly,
   standardApertureLastValid=standardApertureInvalid-1,
};

CString standardApertureTypeTagToString(StandardApertureTypeTag tagValue);
CString standardApertureTypeTagToDescriptorPrefix(StandardApertureTypeTag tagValue);
CString standardApertureTypeTagToValorDescriptorPrefix(StandardApertureTypeTag tagValue);
CString standardApertureTypeTagToApertureNameDescriptor(StandardApertureTypeTag tagValue);
StandardApertureTypeTag intToStandardApertureTypeTag(int tagValue);
StandardApertureTypeTag descriptorPrefixToStandardApertureTag(CString descriptorPrefix);
StandardApertureTypeTag stringToStandardApertureTag(const CString& string);

//_____________________________________________________________________________
enum CenterTag
{
   centerOfUndefined = 0x0,
   centerOfExtents   = 0x1,
   centerOfRectangle = 0x2,
   centerOfMass      = 0x4,
   centerOfVertices  = 0x8,

   centerOfMaskEmpty = 0x0,
   centerOfMask      = 0xf,
   centerOfMaskSize  = 0x10,
};

CString centerTagToString(CenterTag tagValue);
CString centerTagToDescription(CenterTag tagValue);
CString centerTagToDescriptorInfix(CenterTag tagValue);
CenterTag intToCenterTag(int tagValue);

//_____________________________________________________________________________
class CCenterTag
{
private:
   int m_bitmap;

public:
   static CenterTag getNext(CenterTag center=centerOfUndefined);

public:
   CCenterTag() { m_bitmap = 0; }
   CCenterTag(CenterTag center) { m_bitmap = center; }
   CCenterTag(int mask) { m_bitmap = (mask & centerOfMask); }
   CCenterTag(const CCenterTag& other) { m_bitmap = other.m_bitmap; }
   CCenterTag& operator=(const CCenterTag& other) { m_bitmap = other.m_bitmap;  return *this;  }

   void empty()                    { m_bitmap = 0; }
   void add(CenterTag center)      { m_bitmap |= center; }
   void remove(CenterTag center)   { m_bitmap &= ~center; }

   bool contains(CenterTag center) const { return (m_bitmap & center) != 0; }
   int  getCount() const;
};

//_____________________________________________________________________________
class CStandardApertureDialogParameter
{
protected:
   CEdit& m_parameterEditBox;
   CStatic& m_parameterCaption;
   CStatic* m_parameterDescription;
   CStatic* m_genericParameterCaption;
   CString m_descriptorDelimiter;
   CString m_caption;
   CString m_description;

public:
   CStandardApertureDialogParameter(CStatic& parameterCaption,CEdit& parameterEditBox,
      CStatic& genericParameterDescription,CStatic& genericParameterCaption);
   CStandardApertureDialogParameter(CStatic& parameterCaption,CEdit& parameterEditBox,
      CStatic& genericParameterDescription);

   virtual CString getCaption() const;
   virtual void setCaption(const CString& caption);

   virtual CString getValue() const;
   virtual void setValue(const CString& value);

   virtual CString getDescription() const;
   virtual void setDescription(const CString& description);

   const CString& getDescriptorDelimiter() const { return m_descriptorDelimiter; }
   void setDescriptorDelimiter(const CString& descriptorDelimiter) 
      { m_descriptorDelimiter = descriptorDelimiter; }

   virtual void clear();
};

//_____________________________________________________________________________
class CStencilApertureDialogParameter : public CStandardApertureDialogParameter
{
private:
   static const int m_maxDescriptionLength = 20;

public:
   CStencilApertureDialogParameter(CStatic& parameterCaption,CEdit& parameterEditBox,
      CStatic& genericParameterDescription);

   virtual CString getCaption() const;
   virtual void setCaption(const CString& caption);

   //virtual CString getValue() const;
   //virtual void setValue(const CString& value);

   virtual CString getDescription() const;
   virtual void setDescription(const CString& description);

   virtual CString getDescriptiveCaption() const;

   //virtual void clear();
};

//_____________________________________________________________________________
class CStandardApertureDialogParameters
{
private:
   CTypedPtrArrayContainer<CStandardApertureDialogParameter*> m_parameters;
   CString m_descriptorPrefix;
   CStatic*   m_originGroupBox;
   CComboBox* m_originOptions;
   CButton*   m_displayCenters;

   bool m_displayUnitsInPageUnitsFlag;

public:
   CStandardApertureDialogParameters(bool displayUnitsInPageUnitsFlag=false);

   bool getDisplayUnitsInPageUnitsFlag() const { return m_displayUnitsInPageUnitsFlag; }

   CStandardApertureDialogParameter* getAt(int index) const { return m_parameters.GetAt(index); }
   int getCount() const { return m_parameters.GetCount(); }
   CStandardApertureDialogParameter* lookupCaption(const CString& caption);
   CStandardApertureDialogParameter* lookupCaptionPrefix(const CString& captionPrefix);
   void addParameter(CStatic& parameterCaption,CEdit& parameterEditBox,
      CStatic& genericParameterDescription,CStatic& genericParameterCaption);
   void addParameter(CStatic& parameterCaption,CEdit& parameterEditBox,
      CStatic& genericParameterDescription);
   void setParameters(int index,const CString& caption,const CString& description,
      const CString& descriptorDelimiter,const CString& value=CString());
   void setParameters(int index,const CString& caption,const CString& description,
      const CString& descriptorDelimiter,const CString& value,double rawValue);
   void addCenters(CStatic& originGroupBox,CComboBox& originOptions,CButton& displayCenters);
   void setCenters(const CCenterTag& centers,CenterTag center);
   void clear();
   void clearCenters();

   CString getDescriptorTemplate() const;
   void setDescriptorPrefix(const CString& descriptorPrefix) { m_descriptorPrefix = descriptorPrefix; }
   CenterTag getCenter() const;
};

//_____________________________________________________________________________

#define DIMENSION_ARRAY_SIZE 6

class CStandardApertureShape
{
private:
   static PageUnitsTag m_defaultPageUnits;
   static double m_defaultDescriptorDimensionFactor;
   static CString m_descriptorDelimiter;

   //CString m_name;
   bool m_isValid;
   int m_symmetry;
   //double m_descriptorDimensionFactor;
   int m_descriptorDimensionFactorExponent;
   PageUnitsTag m_pageUnits;
   CenterTag m_center;
   double m_dimension[DIMENSION_ARRAY_SIZE];
   double m_rotationDegrees;      // angle in degrees counterclockwise
   double m_exteriorCornerRadius;
   double m_maxExteriorCornerRadius;

public:
   static CStandardApertureShape* create(CString name,PageUnitsTag pageUnits=pageUnitsInches);
   static CStandardApertureShape* create(StandardApertureTypeTag type,PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);
   static CStandardApertureShape* create(const CStandardApertureShape& other);
   static int cornerSymmetry(int corners);
   static int getCornerCount(int corners);
   static int getFeatureCount(int corners);
   static bool hasCorner(int corners,int cornerNumber);
   static int getCornerFlags(int corners,bool& corner1,bool& corner2,bool& corner3,bool& corner4);
   static bool hasFeature(int features,int featureNumber);
   static int getFeatureFlags(int features,bool& feature1,bool& feature2,bool& feature3,bool& feature4);
   static void setDefaultPageUnits(PageUnitsTag pageUnits) { m_defaultPageUnits = pageUnits; }
   static PageUnitsTag getDefaultPageUnits() { return m_defaultPageUnits; }
   static double getDefaultDescriptorDimensionFactor() { return m_defaultDescriptorDimensionFactor; }
   static CString getDescriptorDelimiter() { return m_descriptorDelimiter; }
   static void buildSpokes(CCamCadData& camCadData,DataStruct* polyStruct,
      double outerRadius,double startAngleDegrees,int numSpokes,double spokeWidth);
   static void buildThermalOpenCorners(CCamCadData& camCadData,DataStruct* polyStruct,
      double width,double height,double airGap,
      double startAngleDegrees,int numSpokes,double spokeWidth);
   static PageUnitsTag getDisplayPageUnits(PageUnitsTag pageUnits);
   static int getDefaultDescriptorDimensionFactorExponent(PageUnitsTag pageUnits);

public:
   CStandardApertureShape(int symmetry,PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);
   CStandardApertureShape(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0,
      CenterTag centerTag=centerOfExtents,double rotation=0.,int symmetry=0);
   virtual ~CStandardApertureShape();

public:
   double getDefaultSize() const;
   double getDimensionArrowLength() const;

   PageUnitsTag getPageUnits() const { return m_pageUnits; }
   void setPageUnits(PageUnitsTag pageUnits) { m_pageUnits = pageUnits; }

   int getDescriptorDimensionFactorExponent() const { return m_descriptorDimensionFactorExponent; }
   void setDescriptorDimensionFactorExponent(int exponent) { m_descriptorDimensionFactorExponent = exponent; }

   double getDescriptorDimensionFactor() const;
   CString getDescriptorDimensionUnitsString() const;

   int getSymmetry() const { return m_symmetry; }

   CenterTag getCenter() const { return m_center; }
   void setCenter(CenterTag center) { m_center = center; }

   double getRotationDegrees() const { return m_rotationDegrees; }
   double getRotationRadians() const;
   void   setRotationDegrees(double degrees) { m_rotationDegrees = degrees; }
   void   setRotationRadians(double radians);

   double getDimension(int index) const { return (index >= 0 && index < DIMENSION_ARRAY_SIZE) ? m_dimension[index] : 0.0; }
   void setDimension(int index,double dimension) { if (index >= 0 && index < DIMENSION_ARRAY_SIZE) m_dimension[index] = dimension; }
   int getIntDimension(int index) const { return (index >= 0 && index < DIMENSION_ARRAY_SIZE) ? DcaRound(getDimension(index)) : 0; }
   double getApertureDimension(int index) const;
   int getIntApertureDimension(int index) const;
   double apertureUnitsToPageUnits(double apertureUnits) const;
   bool isEnglish() const;
   bool isMetric() const;

   CString getDescriptorDimension(int index) const;
   CString getDescriptorIntDimension(int index) const;
   CString getCornersString(int index) const;
   CString getDescriptorCorners(int index) const;
   CString getFeaturesString(int index) const;
   CString getDescriptorFeatures(int index) const;
   CString getDescriptorRotation() const;

protected:
   void setValid(bool isValid) { m_isValid = isValid; }

   void setSymmetry(int symmetry) { m_symmetry = symmetry; }

   bool parseParameters(CStringArray& params);
   void setMaxExternalCornerRadius(double maxExteriorCornerRadius) { m_maxExteriorCornerRadius = maxExteriorCornerRadius; }

public:
   virtual bool isValid() const;
   virtual StandardApertureTypeTag getType() const;
   virtual bool isThermal() const;
   virtual CString getDescriptor() const;
   virtual CString getDescriptorUnitsSuffix() const;
   virtual CString getDescriptorCenterInfix() const;
   virtual void changeUnits(PageUnitsTag newPageUnits);
   virtual CCenterTag getCenterOptions() const;
   virtual void scale(double scaleFactor);
   virtual double getArea() const;
   virtual CBasesVector getMajorMinorAxes() const;
   virtual CBasesVector getSymmetryAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct);
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool transformPolyToCenter(DataStruct& polyStruct) const;
   virtual CPoint2d getRelativeCenterOfRectangle() const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void loadFromApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void setDimensionApertureUnits(int index,double dimension);
   virtual void normalizeDimension(int index);
   virtual void normalizeDimensions();

   virtual double getExteriorCornerRadius() const;
   virtual void setExteriorCornerRadius(double exteriorCornerRadius);
   virtual bool integrateExteriorCornerRadius();

   virtual double getMinimumDimension() const;
   virtual void setFromExtent(const CExtent& extent);
   virtual void makeValid();

   virtual double getAnnotationDimensionOffset() const;
   virtual void addDimensionArrow(CCamCadData& camCadData,CPoly* poly,CTMatrix& matrix) const;
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
   virtual void addPointAnnotation(CCamCadData& camCadData,double x,double y) const;
   virtual void addCentroidAnnotations(CCamCadData& camCadData);
   virtual void addTextAnnotation(CCamCadData& camCadData,int layerIndex,double x,double y,const CString& text,
      HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition) const;
   virtual void addTextAnnotation(CCamCadData& camCadData,double x,double y,const CString& text,
      HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition) const;
   virtual void addDimensionAnnotation(CCamCadData& camCadData,
      double x0,double y0,double x1,double y1,const CString& text,
      HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition) const;
   virtual void addDimensionAnnotation(CCamCadData& camCadData,
      const CPoint2d& center,const CPoint2d& p0,const CPoint2d& p1,const CString& text,
      HorizontalPositionTag horizontalPosition,VerticalPositionTag verticalPosition) const;
};

//_____________________________________________________________________________
class CStandardApertureRectangle : public CStandardApertureShape
{
public:
   CStandardApertureRectangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureRectangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getWidth()  const;
   virtual void   setWidth(double width);

   virtual double getHeight() const;
   virtual void   setHeight(double height);

   // attributes
   virtual CString getDescriptor() const;
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void setFromExtent(const CExtent& extent);
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureCircle : public CStandardApertureShape
{
public:
   CStandardApertureCircle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureCircle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getDiameter() const;
   virtual void   setDiameter(double diameter);

   virtual double getRadius()   const;

   // attributes
   virtual CString getDescriptor() const;
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void makeValid();

   virtual void setExteriorCornerRadius(double exteriorCornerRadius);
   virtual bool integrateExteriorCornerRadius();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureSplitCircle : public CStandardApertureCircle
{
public:
   CStandardApertureSplitCircle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureSplitCircle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getSplitWidth() const;
   virtual void   setSplitWidth(double splitWidth);
   //virtual double getDiameter() const;
   //virtual void   setDiameter(double diameter);

   //virtual double getRadius()   const;

   // attributes
   virtual CString getDescriptor() const;
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void makeValid();

   //virtual void setExteriorCornerRadius(double exteriorCornerRadius);
   //virtual bool integrateExteriorCornerRadius();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureButterfly : public CStandardApertureCircle
{
public:
   CStandardApertureButterfly(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureButterfly(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual CBasesVector getMajorMinorAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual void makeValid();

   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureChamferedRectangle : public CStandardApertureRectangle
{
public:
   CStandardApertureChamferedRectangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureChamferedRectangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;
   virtual void scale(double scaleFactor);

   // dimensions
   virtual double getCornerRadius() const;
   virtual void   setCornerRadius(double radius);

   virtual int    getCorners()      const;
   virtual void   setCorners(int corners);

   // attributes
   virtual CString getDescriptor() const;
   virtual void normalizeDimensions();
   virtual double getArea() const;
   virtual CBasesVector getMajorMinorAxes() const;
   virtual CBasesVector getSymmetryAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
   virtual void setDimensionApertureUnits(int index,double dimension);
   //virtual CCenterTag getCenterOptions() const;
};

//_____________________________________________________________________________
class CStandardApertureBeveledRectangle : public CStandardApertureChamferedRectangle
{
public:
   CStandardApertureBeveledRectangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureBeveledRectangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;
   virtual void scale(double scaleFactor);

   // dimensions
   virtual double getBevelWidth() const;
   virtual void   setBevelWidth(double width);

   virtual double getBevelHeight() const;
   virtual void   setBevelHeight(double height);

   virtual int    getCorners()      const;
   virtual void   setCorners(int corners);

   // attributes
   virtual CString getDescriptor() const;
   virtual void normalizeDimensions();
   virtual double getArea() const;
   //virtual CBasesVector getMajorMinorAxes() const;
   //virtual CBasesVector getSymmetryAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
   virtual void setDimensionApertureUnits(int index,double dimension);
   //virtual CCenterTag getCenterOptions() const;
};

//_____________________________________________________________________________
class CStandardApertureDiamond : public CStandardApertureRectangle
{
public:
   CStandardApertureDiamond(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureDiamond(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual void makeValid();
};

//_____________________________________________________________________________
class CStandardApertureRectangledPentagon : public CStandardApertureRectangle
{
public:
   CStandardApertureRectangledPentagon(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureRectangledPentagon(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getAltitude() const;
   virtual void   setAltitude(double altitued);

   // attributes
   virtual CString getDescriptor() const;
   virtual double getArea() const;
   virtual CBasesVector getMajorMinorAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
   virtual void makeValid();
};

//_____________________________________________________________________________
class CStandardApertureEllipse : public CStandardApertureRectangle
{
public:
   CStandardApertureEllipse(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureEllipse(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;

   virtual void setExteriorCornerRadius(double exteriorCornerRadius);
   virtual bool integrateExteriorCornerRadius();
};

//_____________________________________________________________________________
class CStandardApertureHalfOval : public CStandardApertureRectangle
{
public:
   CStandardApertureHalfOval(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureHalfOval(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual void makeValid();

};

//_____________________________________________________________________________
// The ODB++ specification defines the hole with a plating, +tolerance, and -tolerance
class CStandardApertureHole : public CStandardApertureCircle
{
public:
   CStandardApertureHole(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureHole(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual CString getDescriptor() const;
};

//_____________________________________________________________________________
class CStandardApertureHorizontalHexagon : public CStandardApertureRectangle
{
public:
   CStandardApertureHorizontalHexagon(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureHorizontalHexagon(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getCornerSize() const;
   virtual void   setCornerSize(double size);

   // attributes
   virtual CString getDescriptor() const;
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureMoire : public CStandardApertureShape
{
public:
   CStandardApertureMoire(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureMoire(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   double getRingWidth()  const;
   void   setRingWidth(double ringWidth);

   double getRingGap()    const;
   void   setRingGap(double ringGap);

   int    getRingCount()  const;
   void   setRingCount(int ringCount);

   double getLineLength() const;
   void   setLineLength(double lineLength);

   double getLineWidth()  const;
   void   setLineWidth(double lineWidth);

   double getLineAngle()  const;
   void   setLineAngle(double lineAngle);

   double getDiameter()   const;

   // attributes
   virtual CString getDescriptor() const;
   virtual void normalizeDimensions();
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual CExtent getExtent() const;
   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
   virtual void setDimensionApertureUnits(int index,double dimension);
};

//_____________________________________________________________________________
class CStandardApertureNull : public CStandardApertureShape
{
public:
   CStandardApertureNull(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureNull(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   int getExtensionNumber() const;

   // attributes
   virtual CString getDescriptor() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void setDimensionApertureUnits(int index,double dimension);
};

//_____________________________________________________________________________
class CStandardApertureOctagon : public CStandardApertureHorizontalHexagon
{
public:
   CStandardApertureOctagon(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureOctagon(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
   virtual void makeValid();
};

//_____________________________________________________________________________
class CStandardApertureOval : public CStandardApertureRectangle
{
public:
   CStandardApertureOval(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureOval(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   //
   double getLength()   const { return max(getWidth(),getHeight()); }
   double getDiameter() const { return min(getWidth(),getHeight()); }

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;

   virtual void setExteriorCornerRadius(double exteriorCornerRadius);
   virtual bool integrateExteriorCornerRadius();
};

//_____________________________________________________________________________
class CStandardApertureRectangularThermal : public CStandardApertureRectangle
{
public:
   CStandardApertureRectangularThermal(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureRectangularThermal(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;
   virtual bool isThermal() const;
   virtual void scale(double scaleFactor);

   // dimensions
   virtual double getSpokeAngleRadians() const;
   virtual void   setSpokeAngleRadians(double radians);

   virtual double getSpokeAngleDegrees()    const;
   virtual void   setSpokeAngleDegrees(double degrees);

   virtual int    getSpokeCount() const;
   virtual void   setSpokeCount(int spokeCount);

   virtual double getSpokeGap()   const;
   virtual void   setSpokeGap(double spokeGap);

   virtual double getAirGap()     const;
   virtual void   setAirGap(double airGap);

   // attributes
   virtual CString getDescriptor() const;
   virtual void normalizeDimensions();
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
   virtual void setDimensionApertureUnits(int index,double dimension);
};

//_____________________________________________________________________________
class CStandardApertureRectangularThermalOpenCorners : public CStandardApertureRectangularThermal
{
public:
   CStandardApertureRectangularThermalOpenCorners(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureRectangularThermalOpenCorners(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
};

//_____________________________________________________________________________
class CStandardApertureRoundDonut : public CStandardApertureShape
{
public:
   CStandardApertureRoundDonut(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureRoundDonut(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getOuterDiameter() const;
   virtual void   setOuterDiameter(double diameter);

   virtual double getInnerDiameter() const;
   virtual void   setInnerDiameter(double diameter);

   // attributes
   virtual CString getDescriptor() const;
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;

   virtual void setExteriorCornerRadius(double exteriorCornerRadius);
   virtual bool integrateExteriorCornerRadius();
};

//_____________________________________________________________________________
class CStandardApertureRoundedRectangle : public CStandardApertureChamferedRectangle
{
public:
   CStandardApertureRoundedRectangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureRoundedRectangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void setDimensionApertureUnits(int index,double dimension);

   virtual bool integrateExteriorCornerRadius();
};

//_____________________________________________________________________________
class CStandardApertureRoundThermalRounded : public CStandardApertureShape
{
public:
   CStandardApertureRoundThermalRounded(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureRoundThermalRounded(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;
   virtual bool isThermal() const;
   virtual void scale(double scaleFactor);

   // dimensions
   virtual double getOuterDiameter() const;
   virtual void   setOuterDiameter(double diameter);

   virtual double getInnerDiameter() const;
   virtual void   setInnerDiameter(double diameter);

   double getMedialDiameter() const { return (getOuterDiameter() + getInnerDiameter())/2.; }

   virtual double getSpokeAngleRadians()    const;
   virtual void   setSpokeAngleRadians(double radians);

   virtual double getSpokeAngleDegrees()    const;
   virtual void   setSpokeAngleDegrees(double degrees);

   virtual int    getSpokeCount()    const;
   virtual void   setSpokeCount(int count);

   virtual double getSpokeGap()      const;
   virtual void   setSpokeGap(double gap);

   // attributes
   virtual CString getDescriptor() const;
   virtual void normalizeDimensions();
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
   virtual void setDimensionApertureUnits(int index,double dimension);

   virtual void makeValid();
   virtual void setExteriorCornerRadius(double exteriorCornerRadius);
   virtual bool integrateExteriorCornerRadius();
};

//_____________________________________________________________________________
class CStandardApertureRoundThermalSquare : public CStandardApertureRoundThermalRounded
{
public:
   CStandardApertureRoundThermalSquare(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureRoundThermalSquare(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
};

//_____________________________________________________________________________
class CStandardApertureSquare : public CStandardApertureShape
{
public:
   CStandardApertureSquare(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureSquare(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getSize() const;
   virtual void   setSize(double size);

   // attributes
   virtual CString getDescriptor() const;
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureSquareButterfly : public CStandardApertureSquare
{
public:
   CStandardApertureSquareButterfly(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureSquareButterfly(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual CBasesVector getMajorMinorAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual void makeValid();
};

//_____________________________________________________________________________
class CStandardApertureSquareDonut : public CStandardApertureRoundDonut
{
public:
   CStandardApertureSquareDonut(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureSquareDonut(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
};

//_____________________________________________________________________________
class CStandardApertureSquareRoundThermal : public CStandardApertureRoundThermalRounded
{
public:
   CStandardApertureSquareRoundThermal(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureSquareRoundThermal(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
};

//_____________________________________________________________________________
class CStandardApertureSquareThermal : public CStandardApertureSquareRoundThermal
{
public:
   CStandardApertureSquareThermal(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureSquareThermal(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
};

//_____________________________________________________________________________
class CStandardApertureSquareThermalOpenCorners : public CStandardApertureRoundThermalRounded
{
public:
   CStandardApertureSquareThermalOpenCorners(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureSquareThermalOpenCorners(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
};

//_____________________________________________________________________________
class CStandardApertureTriangle : public CStandardApertureShape
{
public:
   CStandardApertureTriangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureTriangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getBase()   const;
   virtual void   setBase(double base);

   virtual double getHeight() const;
   virtual void   setHeight(double height);

   // attributes
   virtual CString getDescriptor() const;
   virtual double getArea() const;
   virtual CBasesVector getMajorMinorAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void setFromExtent(const CExtent& extent);
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureVerticalHexagon : public CStandardApertureHorizontalHexagon
{
public:
   CStandardApertureVerticalHexagon(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureVerticalHexagon(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual void makeValid();

   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureNotchedRectangle : public CStandardApertureRectangle
{
public:
   CStandardApertureNotchedRectangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureNotchedRectangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // dimensions
   virtual double getFeatureBase()   const;
   virtual void   setFeatureBase(double width);

   virtual double getFeatureAltitude() const;
   virtual void   setFeatureAltitude(double height);

   virtual int  getFeatures() const;
   virtual void setFeatures(int features);

   // attributes
   virtual CString getDescriptor() const;
   virtual void normalizeDimensions();
   virtual void scale(double scaleFactor);
   virtual double getArea() const;
   virtual CBasesVector getMajorMinorAxes() const;
   virtual CBasesVector getSymmetryAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
   virtual void makeValid();

   virtual void storeToApertureParameters(CStandardApertureDialogParameters& apertureParameters);
   virtual void setDimensionApertureUnits(int index,double dimension);
   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
   virtual CCenterTag getCenterOptions() const;
};

//_____________________________________________________________________________
class CStandardApertureVeeCutRectangle : public CStandardApertureNotchedRectangle
{
public:
   CStandardApertureVeeCutRectangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureVeeCutRectangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
   virtual void makeValid();

};

//_____________________________________________________________________________
class CStandardApertureTabbedRectangle : public CStandardApertureNotchedRectangle
{
public:
   CStandardApertureTabbedRectangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureTabbedRectangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual CBasesVector getMajorMinorAxes() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool transformPolyToCenter(DataStruct& polyStruct) const;
   virtual CPoint2d getRelativeCenterOfRectangle() const;
   virtual CExtent getExtent() const;
   virtual bool inset(double inset);
   virtual void makeValid();

   virtual void addDiagramAnnotations(CCamCadData& camCadData) const;
};

//_____________________________________________________________________________
class CStandardApertureSpurredRectangle : public CStandardApertureTabbedRectangle
{
public:
   CStandardApertureSpurredRectangle(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureSpurredRectangle(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual double getArea() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
   virtual bool inset(double inset);
};

//_____________________________________________________________________________
class CStandardApertureInvalid : public CStandardApertureShape
{
public:
   CStandardApertureInvalid(PageUnitsTag pageUnits=pageUnitsUndefined,int descriptorDimensionFactorExponent=0);  
   CStandardApertureInvalid(CStringArray& params,CenterTag centerTag,double rotation,
      PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=0);  

   virtual StandardApertureTypeTag getType() const;

   // attributes
   virtual CString getDescriptor() const;
   virtual bool getAperturePoly(CCamCadData& camCadData,
      CDataList& parentDataList,DataStruct* polyStruct,int widthIndex) const;
};

//_____________________________________________________________________________
class CStandardAperture
{
private:
   CStandardApertureShape* m_standardApertureShape;

   BlockStruct* m_apertureBlock;

public:
   CStandardAperture(StandardApertureTypeTag type,PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=-1);
   CStandardAperture(ApertureShapeTag apertureShape,double sizeA,double sizeB,double sizeC,double sizeD,PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=-1);
   CStandardAperture(CString name,PageUnitsTag pageUnits=pageUnitsInches);
   CStandardAperture(BlockStruct* apertureBlock,PageUnitsTag pageUnits=pageUnitsInches,int descriptorDimensionFactorExponent=-1);
   CStandardAperture(const CStandardAperture& other);
   ~CStandardAperture();

   void detachApertureBlock() { m_apertureBlock = NULL; }

   CString getName() const   { return m_standardApertureShape->getDescriptor(); }
   int getSymmetry() const   { return m_standardApertureShape->getSymmetry(); }

   double getRotationDegrees() const { return m_standardApertureShape->getRotationDegrees(); }
   void   setRotationDegrees(double degrees) { m_standardApertureShape->setRotationDegrees(degrees); }
   double getRotationRadians() const { return m_standardApertureShape->getRotationRadians(); }
   void   setRotationRadians(double radians) { m_standardApertureShape->setRotationRadians(radians); }

   bool isStandardAperture() const;
   bool isStandardNormalAperture() const;
   StandardApertureTypeTag getType() const { return m_standardApertureShape->getType(); }
   CBasesVector getMajorMinorAxes()  const { return m_standardApertureShape->getMajorMinorAxes(); }
   CBasesVector getSymmetryAxes()    const { return m_standardApertureShape->getSymmetryAxes(); }
   PageUnitsTag getPageUnits()       const { return m_standardApertureShape->getPageUnits(); }
   int getDesdescriptorDimensionFactorExponent() const { return m_standardApertureShape->getDescriptorDimensionFactorExponent(); }

   CString getDescriptor() const;
   CString getApertureBlockName(CCamCadData& camCadData) const;

   bool isEquivalent(const CStandardAperture& other) const;
   double getDimension(int index) const { return m_standardApertureShape->getDimension(index); }
   double getDimensionInPageUnits(int index) const { return m_standardApertureShape->getDimension(index); }
   void setDimensions(double dimension0=0.,double dimension1=0.,double dimension2=0.,
                      double dimension3=0.,double dimension4=0.,double dimension5=0.);
   void normalizeDimensions() { m_standardApertureShape->normalizeDimensions(); }
   CExtent getExtent() const;
   ApertureShapeTag getApertureShape();
   double getArea();
   double getArea(CCamCadData& camCadData);

   double getExteriorCornerRadius() const;
   void setExteriorCornerRadius(double exteriorCornerRadius);
   bool integrateExteriorCornerRadius();

   bool getAperturePoly(CCamCadData& camCadData,CDataList& parentDataList,DataStruct* polyStruct);

   BlockStruct* createNewAperture(CCamCadData& camCadData,CString apertureName,int fileNumber= -1);
   BlockStruct* findExistingAperture(CCamCadData& camCadData,CString& apertureName,int fileNumber= -1);
   BlockStruct* getDefinedAperture(CCamCadData& camCadData,int fileNumber= -1);
   BlockStruct* getDefinedAperture(CCamCadData& camCadData,const CString& apertureName,int fileNumber= -1);

   int getWidthIndex(CCamCadData& camCadData);

   bool inset(double distance);
   void scale(double scaleFactor);

   void dump(CWriteFormat& writeFormat,int depth = -1);

private:
   void constructAperture(ApertureShapeTag apertureShape,double sizeA,double sizeB,double sizeC,double sizeD,PageUnitsTag pageUnits,int descriptorDimensionFactorExponent);
};


#endif
