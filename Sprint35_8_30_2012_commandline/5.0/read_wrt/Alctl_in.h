// $Header: /CAMCAD/4.3/read_wrt/Alctl_in.h 15    11/19/03 9:17p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if ! defined (__AlcatelDocicaDatabase_h__)
#define __AlcatelDocicaDatabase_h__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AlienDatabase.h"
#include "RwLib.h"
#include "TMState.h"

enum AttributeIndexTag
{
   attributeIndexNetName             ,
   attributeIndexValue               ,
   attributeIndexSmdShape            ,
   attributeIndexTolerance           ,
   attributeIndexPlusTolerance       ,
   attributeIndexMinusTolerance      ,
   attributeIndexPhysicalRefDes      ,
   attributeIndexCompHeight          ,
   attributeIndexPlated              ,
   attributeIndexCompPinnr           ,

   attributeIndexPackagePinName      ,
   attributeIndexPhysicalPinNumber   ,
   attributeIndexTest                ,
   attributeIndexAccess              ,
   attributeIndexTerminal            ,
   attributeIndexTypeOfPin           ,
   attributeIndexDescription         ,

   attributeIndex
};

//_____________________________________________________________________________
class CAlcatelDocicaDatabase : public CAlienDatabase
{
private:
   CString m_recordString;
   CString m_entityCodeString;
   int m_entityCode;
   int m_blockNumber;
   BlockStruct* m_figureBlock;
   DataStruct* m_figurePolyStruct;
   CPoly* m_figurePoly;
   double m_figureUnitsConversionFactor;
   int m_figureLayerIndex;
   CUIntArray m_attributeIndexDirectory;

   //IDEN Record Parameters
   CString m_idenProductPartNumber;
   CString m_idenProductPartNumberExtension;
   CString m_idenPartsListEdition;
   CString m_idenReserved1;
   CString m_idenEditionIndexOfTechDocica;
   CString m_idenReserved2;
   CString m_idenItemChangeStatus;
   CString m_idenEditionIndexOfManufDocica;
   CString m_idenReserved3;
   CString m_idenProductMnemonic;
   CString m_idenIndexSheet;
   CString m_idenIndexSheetEdition;
   CString m_idenPlatformIndicator;
   CString m_idenLayoutToolIndicator;
   CString m_idenTranslatorRelease;

   // PDES Record Parameters
   CString m_pdesDescription1;
   CString m_pdesDescription2;
   CString m_pdesDescription3;

   // GENE Record Parameters
   CString m_geneFiller1;
   CString m_geneCreationDate;
   CString m_geneFiller2;
   CString m_geneClusterImplementationLevel;
   CString m_genePanellizationImplementationLevel;
   CString m_geneCutsAndStrapsImplementationLevel;
   CString m_genePowerSupplyImplementationLevel;
   CString m_geneFiller3;
   CString m_geneChangeDocumentNumber;
   CString m_geneDataOriginatorTechnicalCenter;
   CString m_geneFiller4;
   CString m_geneNameOfPersonInCharge;
   CString m_geneSpecificationEdition;

   // PROJ Record Parameters
   CString m_projProjectSubproject;

   // PBT1 Record Parameters
   int     m_pbt1DensityClass;
   int     m_pbt1ProtrusionLength;
   int     m_pbt1ThicknessOfBoard;
   CString m_pbt1PlatingSolderSideFinish;
   CString m_pbt1PlatingComponentSideFinish;
   CString m_pbt1PcbOutlineGraphicSymbol;
   CString m_pbt1Filler1;
   CString m_pbt1TechnologyFileName;
   int     m_pbt1NumberOfWiresToBeAdded;
   int     m_pbt1NumberOfTracksCutOff;
   int     m_pbt1NumberOfSolderPointsToBeAddedDueToCutsAndWires;
   CString m_pbt1DocicaValidityLevel;
   int     m_pbt1NumberOfSolderingPointsOnSolderSide;
   int     m_pbt1NumberOfSolderingPointsOnComponentSide;

   // PBT2 Record Parameters
   int     m_pbt2XminOfPbShape;
   int     m_pbt2YminOfPbShape;
   int     m_pbt2XmaxOfPbShape;
   int     m_pbt2YmaxOfPbShape;

   // PANO Record Parameters
   CString m_panoFiller1;
   CString m_panoPanelOutlineSymbolName;
   int     m_panoXminOfThePanelShape;
   int     m_panoYminOfThePanelShape;
   int     m_panoXmaxOfThePanelShape;
   int     m_panoYmaxOfThePanelShape;

   // MULT Record Parameters
   int     m_multOrientationOfCopy1OnPanel;
   int     m_multXcoordinateOfCopy1;
   int     m_multYcoordinateOfCopy1;
   int     m_multNumberOfXcopies1;
   int     m_multXstepBetweenXcopies1;
   int     m_multNumberOfYcopies1;
   int     m_multYstepBetweenYcopies1;
   int     m_multOrientationOfCopy2OnPanel;
   int     m_multXcoordinateOfCopy2;
   int     m_multYcoordinateOfCopy2;
   int     m_multNumberOfXcopies2;
   int     m_multXstepBetweenXcopies2;
   int     m_multNumberOfYcopies2;
   int     m_multYstepBetweenYcopies2;

   // PSUP Record Parameters
   CString m_psupType;
   double  m_psupVoltage;
   double  m_psupCurrent;
   CString m_psupSignalName;
   CString m_psupReferenceSignal;
   double  m_psupReferencePotential;
   CString m_psupExternalOrInternal;

   // NLMC Record Parameters
   CString m_nlmcPartNumber;
   CString m_nlmcPartNumberExtension;
   CString m_nlmcComponentName;
   int     m_nlmcItemReferenceNumber;
   CString m_nlmcMeasuringUnit;
   double  m_nlmcQuantity;
   CString m_nlmcDescription;
   CString m_nlmcClusterName;

   // CMP1 Record Parameters
   CString m_cmp1PartNumber;
   CString m_cmp1PartNumberExtension;
   CString m_cmp1ComponentName;
   int     m_cmp1CenterX;
   int     m_cmp1CenterY;
   int     m_cmp1Orientation;  // in degrees
   int     m_cmp1ItemReferenceNumber;
   CString m_cmp1ComponentMountingSide;
   CString m_cmp1MountingCode;
   CString m_cmp1ComponentType;
   //---
   BlockStruct* m_footPrintBlock;
   DataStruct* m_componentData;
   CTMatrix m_compTm;
   int m_compLayerIndex;
   int m_compPinCount; 
   bool m_compTopSideFlag;
   bool m_padTopSideFlag;

   // CMP2 Record Parameters
   CString m_cmp2Description;
   CString m_cmp2SilkScreenMarkingIndicator;
   int     m_cmp2CompMountingHeight;
   CString m_cmp2StandardModel;
   double  m_cmp2Value;
   CString m_cmp2Unit;
   double  m_cmp2MaxTolerance;
   double  m_cmp2MinTolerance;
   int     m_cmp2NumCpins;
   int     m_cmp2NumChol;
   int     m_cmp2NumFidc;

   // CMP3 Record Parameters
   CString m_cmp3GraphicSymbolName;
   CString m_cmp3GraphicSymbolNameAlternate;
   CString m_cmp3Description;
   CString m_cmp3ClusterName;

   // ETC1 Record Parameters
   int     m_etc1XcoordOfComponentCenter;
   int     m_etc1YcoordOfComponentCenter;
   int     m_etc1Orientation;
   CString m_etc1GraphicSymbolName;
   CString m_etc1GraphicSymbolNameAlternate;
   CString m_etc1ComponentMountingSide;
   CString m_etc1ComponentName;
   CString m_etc1PartNumber;
   CString m_etc1PartNumberExtension;
   CString m_etc1MountingCode;

   // ETC2 Record Parameters
   CString m_etc2StandardModel;
   double  m_etc2Value;
   CString m_etc2Unit;
   double  m_etc2MaxTolerance;
   double  m_etc2MinTolerance;
   int     m_etc2CpinLinesNumber;
   CString m_etc2ClusterName;
   CString m_etc2ComponentType;

   // CPIN Record Parameters
   int     m_cpinXcoordOfTheHoleArea;
   int     m_cpinYcoordOfTheHoleArea;
   int     m_cpinHoleDiameterOrPadOrientation;
   CString m_cpinLayoutLayer;
   CString m_cpinGraphicDrawingPadSymbol;
   CString m_cpinPhysicalPinNumber;
   CString m_cpinTransferTypeIndicator;
   CString m_cpinPlatingIndicator;
   CString m_cpinPackagePinName;
   CString m_cpinSignalName;
   CString m_cpinTypeOfPinPart1;
   CString m_cpinTypeOfPinPart2;
   CString m_cpinTypeOfPinPart3;
   CString m_cpinTypeOfTerminal;
   CString m_cpinAccessibilityCode;
   CString m_cpinTestAccessCode;

   // CHOL Record Parameters
   int     m_cholXcoordOfTheHoleCenter;
   int     m_cholYcoordOfTheHoleCenter;
   int     m_cholHoleDiameter;
   CString m_cholPlatingIndicator;
   CString m_cholPhysicalPinNumber;

   // HOLB or HOLP Record Parameters
   int     m_holXcoordOfTheHoleCenter;
   int     m_holYcoordOfTheHoleCenter;
   int     m_holHoleDiameter;
   CString m_holPlatingIndicator;
   CString m_holFiller1;
   CString m_holNameOfTheReferenceHole;
   CString m_holHoleIndicator;
   CString m_holIdentificationKey;

   // FIDB, FIDC, or FIDP Record Parameters
   int     m_fidXcoordinate;
   int     m_fidYcoordinate;
   int     m_fidDiameterTarget;
   CString m_fidComponentMountingSide;
   CString m_fidGraphicFiducialSymbol;
   CString m_fidGraphicFiducialSymbolAlternate;
   CString m_fidFiducialName;
   int     m_fidOrientation;
   CString m_fidIdentificationKey;

   // VIAH Record Parameters
   int     m_viahXcoordinateOfTheHoleCenter;
   int     m_viahYcoordinateOfTheHoleCenter;
   int     m_viahHoleDiameter;
   CString m_viahFirstLayoutLayer;
   CString m_viahLastLayoutLayer;
   CString m_viahAccessibilityCode;
   CString m_viahTestAccessCode;
   int     m_viahPadDiameter;
   CString m_viahFiller1;
   CString m_viahSignalName;

   // TESP Record Parameters
   int     m_tespCenterXcoordinate;
   int     m_tespCenterYcoordinate;
   int     m_tespOrientation;
   CString m_tespGraphicSymbolName;
   CString m_tespGraphicSymbolNameAlternate;
   CString m_tespMountingSideOfTheElement;
   CString m_tespTestPointName;
   CString m_tespSilkscreenMarkingIndicator;
   int     m_tespCpinLinesNumber;
   CString m_tespIdentificationKey;
   //---
   BlockStruct* m_tespGeometry;
   DataStruct* m_tespData;
   CTMatrix m_tespTm;
   //int m_compLayerIndex;
   int m_tespPinCount; 
   int m_tespCount;
   bool m_tespNewGeometryFlag;
   //bool m_compTopSideFlag;
   //bool m_padTopSideFlag;

   // CUTS Record Parameters
   CString m_cutsSignalName;
   CString m_cutsStartingLayerNumberOfCut;
   CString m_cutsStoppingLayerNumberOfCut;
   int     m_cutsCutPointXcoordinate;
   int     m_cutsCutPointYcoordinate;
   int     m_cutsCutDiameter;

   // AWIR Record Parameters
   CString m_awirWireName;
   CString m_awirFiller1;
   CString m_awirWirePartNumber;
   CString m_awirWirePartNumberExtension;
   int     m_awirWireLength;

   // WPAT Record Parameters
   CString m_wpatNumberOfPointsInTheEntity;
   int     m_wpatXcoordinate1;
   int     m_wpatYcoordinate1;
   int     m_wpatXcoordinate2;
   int     m_wpatYcoordinate2;
   int     m_wpatXcoordinate3;
   int     m_wpatYcoordinate3;
   int     m_wpatXcoordinate4;
   int     m_wpatYcoordinate4;
   int     m_wpatXcoordinate5;
   int     m_wpatYcoordinate5;
   CString m_wpatContinuationCode;
   CString m_wpatSideOfTheAddedWire;

   // SIGN Record Parameters
   CString m_signDocicaName;
   CString m_signDrawingName;

   // CLU1 Record Parameters
   CString m_clu1PartNumberIdentificationKey;
   CString m_clu1PartNumberExtension;
   CString m_clu1ClusterName;
   int     m_clu1XcoordinateOfClusterCenter;
   int     m_clu1YcoordinateOfClusterCenter;
   int     m_clu1Orientation;
   CString m_clu1ClusterMountingSide;
   CString m_clu1MountingCode;
   CString m_clu1SilkScreenMarkingIndicator;
   int     m_clu1ComponentMountingHeight;
   int     m_clu1NumberOfElectricalComponentsInCluster;
   int     m_clu1NumberOfComponenetsInCluster;
   int     m_clu1CpinLinesNumber;
   int     m_clu1CholLinesNumber;
   int     m_clu1FidcLinesNumber;
   int     m_clu1ClusterType;

   // CLU2 Record Parameters
   CString m_clu2GraphicSymbolName;
   CString m_clu2GraphicSymbolNameAlternate;

   // DEB Record Parameters
   CString m_debName;

   // UNI Record Parameters
   CString m_uniMmOrIn;

   // ECH Record Parameters
   int     m_echScaleUsed;

   // FIN Record Parameters
   CString m_finName;

   // PLG Record Parameters
   int m_plgFillingDescriptor;

   // FPG Record Parameters
   // no parameters

   // PIN Record Parameters
   // no parameters

   // FPN Record Parameters
   // no parameters

   // LIG Record Parameters
   int     m_ligNumberOfPoints;
   int     m_ligXcoordinate1;
   int     m_ligYcoordinate1;
   int     m_ligXcoordinate2;
   int     m_ligYcoordinate2;
   int     m_ligXcoordinate3;
   int     m_ligYcoordinate3;
   int     m_ligXcoordinate4;
   int     m_ligYcoordinate4;
   int     m_ligXcoordinate5;
   int     m_ligYcoordinate5;
   CString m_ligContinuationCode;

   // REC Record Parameters
   int     m_recXcoordinateOfExtremity1;
   int     m_recYcoordinateOfExtremity1;
   int     m_recXcoordinateOfExtremity2;
   int     m_recYcoordinateOfExtremity2;

   // CER Record Parameters
   int     m_cerXcoordinateOfCircleCenter;
   int     m_cerYcoordinateOfCircleCenter;
   int     m_cerRadius;

   // ARC Record Parameters
   int     m_arcXcoordinateOfCircleCenter;
   int     m_arcYcoordinateOfCircleCenter;
   int     m_arcRadiusOfTheCircle;
   int     m_arcAngleAtStartOfTheArc;
   int     m_arcAngleAtEndOfTheArc;
   int     m_arcDirectionOfRotation;
   int     m_arcXcoordinateForStartOfArc;
   int     m_arcYcoordinateForStartOfArc;
   int     m_arcXcoordinateForEndOfArc;
   int     m_arcYcoordinateForEndOfArc;

   // PNT Record Parameters
   int     m_pntXcoordinateOfPoint;
   int     m_pntYcoordinateOfPoint;

   // TXT Record Parameters
   int     m_txtXcoordOfTheAlignmentPoint;
   int     m_txtYcoordOfTheAlignmentPoint;
   int     m_txtNumberOfCharactersInTheText;
   CString m_txtTextCharacters;

   // EMP Record Parameters
   int     m_empXcoordOfTheAlignmentPoint;
   int     m_empYcoordOfTheAlignmentPoint;
   CString m_empVariableAssociatedWithLocation;

   // ATL Record Parameters
   int     m_atlThicknessOfLine;
   int     m_atlLineType;
   int     m_atlColor;
   int     m_atlLayer;

   // ATH Record Parameters
   int     m_athInclinationOfHatchingLines;
   int     m_athDistanceBetweenHatchingLines;
   int     m_athThicknessOfHatchingLines;

   // ATT Record Parameters
   int     m_attTextAlignmentCode;
   int     m_attTextDirectionHorizLine;
   int     m_attInclinationOfCharacters;
   int     m_attMirrorEffect;
   int     m_attCharacterFont;
   int     m_attHeightOfCharacters;
   int     m_attWidthOfCharacters;
   int     m_attDistanceBetweenCharacters;

public:
   CAlcatelDocicaDatabase(CCamCadDatabase& camCadDatabase);

   bool extractField(int& fieldValue    ,int fieldPosition,int fieldLength,CWriteFormat* errorLog);
   bool extractField(double& fieldValue ,int fieldPosition,int fieldLength,CWriteFormat* errorLog);
   bool extractField(CString& fieldValue,int fieldPosition,int fieldLength,CWriteFormat* errorLog);

   virtual bool read(const CString& filePath,
      PageUnitsTag pageUnits,CWriteFormat& errorLog);

private:
   bool parseRecord(CWriteFormat& errorLog);
   bool processRecord(CWriteFormat& errorLog);
   void setFigureUnitsMetric(bool metricFlag);
   double convertFigureUnitsToInches(double figureUnit);
   int getFigureLayerIndex();
   BlockStruct* getPadStack(const CString& padSymbolName,bool thruHoleFlag,bool topSideFlag,
      int holeDiameterOrPadOrientation,CWriteFormat& errorLog);
   void initializeCamCadLayers();
   BlockStruct* getViaPadStack(int holeDiameter,bool topPadFlag,bool bottomPadFlag,int padDiameter);
   int getAttributeIndex(AttributeIndexTag attributeIndexTag);
   void addAccessAttribute(CAttributes** attributeMap,
      const CString& accessCodeString,CWriteFormat& errorLog);
   void addTestAccessAttribute(CAttributes** attributeMap,
      const CString& testAccessCodeString,CWriteFormat& errorLog);

};

#endif


