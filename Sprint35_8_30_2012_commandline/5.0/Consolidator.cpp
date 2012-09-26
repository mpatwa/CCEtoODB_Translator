

#include "stdafx.h"
#include "ccdoc.h"
#include "graph.h" 
#include "PcbUtil.h"
#include "polylib.h"
#include "Consolidator.h"
#include "ConsolidatorControlDlg.h"


//========================================================================================================

// Geometry Consolidator access from Edit | Geometries menu

void CCEtoODBDoc::OnConsolidateGeomPinsOnly()
{
   CGeometryConsolidatorControl control;
   control.SetInsertsOnly(true);

   CGeometryConsolidator squisher(this, "" /*no report*/); 
   squisher.Go( &control );
}

void CCEtoODBDoc::OnConsolidateGeomStandard()
{
   // Standard all-out requires no control settings, it is the default

   CGeometryConsolidator squisher(this, "" /*no report*/); 
   squisher.Go(  );
}

void CCEtoODBDoc::OnConsolidateGeomCustom()
{
   ConsolidatorControlDlg controlDlg;

   controlDlg.ConsiderOriginalName(true);
   controlDlg.ConsiderPinsOnly(true);
   controlDlg.Tolerance(0.00001);

   if (controlDlg.DoModal() == IDOK)
   {
      CGeometryConsolidatorControl control;
      control.SetInsertsOnly( controlDlg.ConsiderPinsOnly() );
      control.SetUseOriginalName( controlDlg.ConsiderOriginalName() );
      control.SetTolerance( controlDlg.Tolerance() );

      CGeometryConsolidator squisher(this, "" /*no report*/); 
      squisher.Go( &control );
   }
}


//========================================================================================================

bool CGeometryConsolidatorControl::Parse(CString commandline)
{
   // Parses line of text, returns true if this line is a Consolidator command,
   // returns false if it is not. This makes it possible to use Parse() in the
   // typical if-elseif-elseif parsing code found in most importers.
   // Caller should pass the entire command line as found in settings file.
   // Saves/updates settings that are found.

   bool isConsolidatorCommand = false;

   if (!commandline.IsEmpty())
   {
      CSupString supCmd( commandline );
      CStringArray params;
      supCmd.ParseQuote(params, ", ");

      if (params.GetCount() > 0)
      {
         CString firstField( params.GetAt(0) );
         firstField.Replace("_", "");  // Ignore underscore separator in name, accept with and without
         if (firstField.CompareNoCase(".CONSOLIDATEGEOMETRY") == 0)
         {
            // .CONSOLIDATE_GEOMETRY  Y/N/P

            isConsolidatorCommand = true;

            if (params.GetCount() > 1)
            {
               CString commandVal( params.GetAt(1) );

               // Valid command values are Yes, No, and Pins. Upper/lower case does not matter.
               // Abbreviation to single char is acceptable.
               // Therefore we will simply look for Y, N, or P in first char.

               if (commandVal.Left(1).CompareNoCase("Y") == 0)
               {
                  this->SetIsEnabled(true);
               }
               else if (commandVal.Left(1).CompareNoCase("N") == 0)
               {
                  this->SetIsEnabled(false);
               }
               else if (commandVal.Left(1).CompareNoCase("P") == 0)
               {
                  this->SetIsEnabled(true);
                  this->SetInsertsOnly(true);  // In reality, includes more than just pin inserts
               }
               // else not recognized, ignore it.
            }

            // No other commands supported/expected as of this writing, so
            // ignore what ever might be on rest of line.
         }
         else if (firstField.CompareNoCase(".CONSOLIDATORTOLERANCE") == 0)
         {
            // .CONSOLIDATOR_TOLERANCE floatingPntNumber  0 to 0.00000001

            isConsolidatorCommand = true;

            if (params.GetCount() > 1)
            {
               CString commandVal( params.GetAt(1) );
               double val = atof(commandVal);
               if (val < 0.)
                  val = -val;
               if (val >= 0. && val <= 1.0E-8)
                  this->SetTolerance(val);
            }
         }
         else if (firstField.CompareNoCase(".CONSOLIDATORCONSIDERORIGINALNAME") == 0)
         {
            // .CONSOLIDATOR_CONSIDER_ORIGINAL_NAME  Y/N

            isConsolidatorCommand = true;

            if (params.GetCount() > 1)
            {
               CString commandVal( params.GetAt(1) );
               this->SetUseOriginalName( (commandVal.Left(1).CompareNoCase("Y") == 0) );
            }
         }
      }

   }

   return isConsolidatorCommand;
}

//========================================================================================================

bool CDSWrapper::isEquivalent(DataStruct *otherData, double tolerance)
{
   if (otherData != NULL)
   {
      if (this->m_data->getDataType() == otherData->getDataType())
      {
         switch (this->m_data->getDataType())
         {
         case dataTypeInsert:
            {
               // Sample data showed coordinate errors slightly greater than SMALLNUMBER (1E-6).
               // E.g. x1 = .10000000 and x2 = .10000512, so we are using one mangitude greater tolerance for coords.
               // Update: Now tolerance is configurable.

               if (this->m_data->getInsert()->getBlockNumber() == otherData->getInsert()->getBlockNumber())
               {
                  bool sameLayer = (this->m_data->getLayerIndex() == otherData->getLayerIndex());
                  bool sameName = (this->m_data->getInsert()->getRefname().CompareNoCase(otherData->getInsert()->getRefname())) == 0;
                  bool sameX = fabs( this->m_data->getInsert()->getOriginX() - otherData->getInsert()->getOriginX() ) < (tolerance);
                  bool sameY = fabs( this->m_data->getInsert()->getOriginY() - otherData->getInsert()->getOriginY() ) < (tolerance);
                  bool sameR = radiansApproximatelyEqual(this->m_data->getInsert()->getAngleRadians(), otherData->getInsert()->getAngleRadians(), 0.01); // .01 rad is roughly 0.57 degrees
                  return sameLayer && sameName && sameX && sameY && sameR;
               }
            }
            break;

         case dataTypePoly:
            {
               // Compare two head polys with PolysMatch
               // This is purposely comparing only the first poly from each polylist, 
               // and is an area for possible future improvement by comparing all polys in the polylists.
               CPolyList* thisPolyList = m_data->getPolyList();
               CPolyList* otherPolyList = otherData->getPolyList();
               bool samePoly = (thisPolyList == otherPolyList);
               if(!samePoly && thisPolyList && otherPolyList)
               {
                  CPoly *poly1 = thisPolyList->isEmpty() ? NULL : thisPolyList->GetHead();
                  CPoly *poly2 = otherPolyList->isEmpty() ? NULL : otherPolyList->GetHead();
                  samePoly = PolysMatch(m_doc, poly1, poly2, SMALLNUMBER); // Should this use tolerance param?
               }

               bool sameLayer = (m_data->getLayerIndex() == otherData->getLayerIndex());
               bool sameGraphicClass = (m_data->getGraphicClass() == otherData->getGraphicClass());                  
               return (sameLayer && sameGraphicClass && samePoly);
            }
            break;  // Never gonna get here, still it is good form to have the break.

         case dataTypeDraw:
         case dataTypeText:
         case dataTypeTool:
         case dataTypePoint:
         case dataTypeBlob:
         case dataTypeUndefined:
            // PUNT
            // This, obviously, could be improved. However, by Mark's directive and requirements
            // we are currently only concerned that the two items have the same pin inserts. This
            // function is already going beyond that by requiring that the two not only have the
            // same pin inserts but also the same of every kind of insert (above). Here, we are
            // requiring that that two have an instance of the same other random data. We are
            // not yet checking that this other data is in fact the same, only that the both
            // have one of the same variety. Good enough for now, according to the current
            // requirements. But certainly I expect we'll be back here for improvements.
            return true;
            break;
         }
      }
   }

   return false;
}


bool CDLWrapper::hasEquivalent(DataStruct *otherData, double tolerance)
{
   if (otherData != NULL)
   {
      for (POSITION pos = this->m_datalist->GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = this->m_datalist->GetNext(pos);

         if (data != NULL)
         {
            CDSWrapper thisdata(data, m_doc);
            if (thisdata.isEquivalent(otherData, tolerance))
            {
               return true;
            }
         }
      }
   }

   return false;
}

bool CDLWrapper::isEquivalent(CDataList *otherList, bool insertsOnly, double tolerance)
{
   if (otherList == NULL || this->m_datalist == NULL)
   {
      return false; // no datalist voids equivalency, even if both are NULL
   }
   else
   {
      // check that "this" list has all that is in "other" list
      POSITION otherPos = otherList->GetHeadPosition();
      while (otherPos != NULL)
      {
         DataStruct *otherData = otherList->GetNext(otherPos);

         // if insertsOnly mode then check only if otherData is an insert
         if (!insertsOnly || otherData->getDataType() == dataTypeInsert)
         {
            if (!this->hasEquivalent(otherData, tolerance))
            {
               return false; // a datum is missing
            }
         }
      }

      // check that "other" list has all that is in "this" list
      CDLWrapper otherDLW(otherList, m_doc);

      POSITION thisPos = this->m_datalist->GetHeadPosition();
      while (thisPos != NULL)
      {
         DataStruct *thisData = this->m_datalist->GetNext(thisPos);

         // if insertsOnly mode then check only if otherData is an insert
         if (!insertsOnly || thisData->getDataType() == dataTypeInsert)
         {
            if (!otherDLW.hasEquivalent(thisData, tolerance))
            {
               return false; // a datum is missing
            }
         }
      }
   }

   return true; // no missing data was detected
}

bool CDLWrapper::hasPins(CCEtoODBDoc *doc)
{
   // Check that there is a pin insert and that the padstack that the pin inserts has
   // some data itself. Case dts0100447680 gave us sample data that produced fake
   // PCB Components (used as fids or something) that had pins but the padstack inserted
   // by the pin had an empty data list. For our purposes that is just the same as
   // not having a pin. Note that in CamCad's geometry editor you can't even see such
   // pins, but you can see the reference to the padstack in the selection tree.

   if (m_datalist != NULL)
   {
      for (POSITION pos = m_datalist->GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_datalist->GetNext(pos);

         if (data->isInsertType(insertTypePin))
         {
            BlockStruct *insertedBlk = doc->getBlockAt( data->getInsert()->getBlockNumber() );
            if (insertedBlk != NULL)
            {
               if (insertedBlk->getDataCount() > 0)
               {
                  // This could go further and check that the data list contains something
                  // that is visible, e.g. a non-zero size aperture or some polys or something.
                  // But for the case at hand we are only needing to spot blocks with empty
                  // datalists, so we'll let it go at that.

                  // Datalist is not empty so we'll accept this as a pin.
                  return true;
               }
            }
         }
      }
   }

   return false;
}
//========================================================================================================

BlockStruct *CApertureMap::FindEquivalent(CCEtoODBDoc *doc, BlockStruct *otherApBlk)
{
   POSITION pos = this->GetStartPosition();
   while (pos != NULL)
   {
      WORD key;
      void *value = NULL;
      this->GetNextAssoc(pos, key, value);

      if (value != NULL)
      {
         BlockStruct *blk = (BlockStruct*)value;

         // The IsEqualAperture() function is used by PolyToFlash stuff, it is reasonably nice.
         // It handles complex apertures that are made up of one polyline, in addition
         // to simple apertures. It does not compare block file numbers, it is from an
         // era when apertures were considered global. To us here, file numbers count, so we'll
         // compare file numbers here and let the func handle the geometry details.
         if (blk->getFileNumber() == otherApBlk->getFileNumber() &&
            IsEqualAperture(doc, blk, otherApBlk, 1.0E-5))  // SMALLNUMBER is too big !
         {
            return blk;
         }
      }
   }

   return NULL;
}

BlockStruct *CPadstackMap::FindEquivalent(BlockStruct *otherPadstack, CCEtoODBDoc *doc, double tolerance)
{
   if (otherPadstack != NULL)
   {
      POSITION mappos = this->GetStartPosition();
      while (mappos != NULL)
      {
         WORD key;
         void *value;
         this->GetNextAssoc(mappos, key, value);
         if (value != NULL)
         {
            BlockStruct *thisPadstack = (BlockStruct*)value;

            if (thisPadstack->getDataCount() == otherPadstack->getDataCount() &&
               thisPadstack->getFileNumber() == otherPadstack->getFileNumber())
            {
               CDLWrapper thisPadstackDatalist(&thisPadstack->getDataList(), doc);
               if (thisPadstackDatalist.isEquivalent( &otherPadstack->getDataList(), false, tolerance ))
               {
                  return thisPadstack;
               }
            }
         }
      }
   }

   return NULL;
}

BlockStruct *CGeometryMap::FindEquivalent(CCEtoODBDoc *doc, BlockStruct *otherComponent, bool insertsOnly, bool considerOriginalName, double tolerance)
{
   // Case dts0100447680 - Components with no pins are exempt from consolidation.
   // Since our consolidator is based on land pattern comparison, if there is no land pattern
   // then the component geometry is outside the jurisdiction of this consolidator. Treat
   // all such as unique geometries, which is done by simply refusing to find an
   // equivalent here.

   // Feedback from the field is the Consolidator was consolidating too much. Sometimes the pins/pads
   // are all the same, in fact, all geom is all the same, but they still do not wanted them
   // consolidated. This issue is they actually came from separate original geometries, and the
   // name alone is important, so they want to keep them separate. So we are introducing a
   // new aspect. We are comparing "original name", and grouping geoms by "original name", and
   // hence consolidating only within families of "original name".
   // Not all importers set "original name", so sometimes it will be blank. To take advantage of
   // this capability, a given importer must set "original name" on geometry blocks (at least for
   // pcb components). This is being done to the Zuken CR5K, as the case leader.
   // When the "original name" is blank in all the geometries, that is in fact a match, so it
   // would continue to work as it did before this refinement.

   if (otherComponent != NULL && CDLWrapper(&otherComponent->getDataList(), doc).hasPins(doc))
   {
      POSITION mappos = this->GetStartPosition();
      while (mappos != NULL)
      {
         WORD key;
         void *value;
         this->GetNextAssoc(mappos, key, value);
         if (value != NULL)
         {
            BlockStruct *thisComponent = (BlockStruct*)value;

            // When in insertsOnly mode, the data counts don't matter. One geom might have
            // more or less data (e.g. Text) than the other, but that is not an insert, so it
            // does not matter. So in insertsOnly mode do not discriminate on data counts.
            bool dataCountAcceptable = insertsOnly ? true : (thisComponent->getDataCount() == otherComponent->getDataCount());

            // Geometries must belong to same file, i.e. have same file numbers.
            bool fileNumbersMatch = thisComponent->getFileNumber() == otherComponent->getFileNumber();

            // If original name is set on both, then it must match.
            // If not set on one or the other, then it is a 'wildcard' sort of situation, and ultimate matchup
            // depends solely on geometry content, so let that specific kind of name mismatch pass.
            bool originalNameMatchOk =
               (!considerOriginalName) ||  // we're disregarding original name match, or
               (thisComponent->getOriginalName() == otherComponent->getOriginalName()) || // names do match, or
               (thisComponent->getOriginalName().IsEmpty()) ||  // this one is blank, or
               (otherComponent->getOriginalName().IsEmpty());   // other one is blank

            if (dataCountAcceptable  && fileNumbersMatch && originalNameMatchOk)
            {
               CDLWrapper thisComponentDatalist(&thisComponent->getDataList(), doc);
               if (thisComponentDatalist.isEquivalent( &otherComponent->getDataList(), insertsOnly, tolerance ))
               {
                  return thisComponent;
               }
            }
         }
      }
   }

   return NULL;
}

//========================================================================================================

void CGeometryConsolidator::Go(CGeometryConsolidatorControl *controls)
{
   m_activeControl = controls;

   if (!this->m_logFilename.IsEmpty())
   {
      m_logFile.Open(m_logFilename, CFile::modeWrite|CFile::modeCreate);
      m_logFile.WriteString("\nGeometry Consolidation Report\n\n");
   }

   CollectAllApertures();
   CollectAllPadStacks();

   SimplifyComplexApertures();
   NormalizeCircleSquareRectangleOblongApertures();
   NormalizeApertureRotation();
   
   ClassifyApertures();   
   ConsolidateApertureUsageInPadstacks();

   ClassifyPadstacks();
   ConsolidatePadstackUsageInComponents();

   CollectAllComponentGeometries();
   ClassifyComponentGeometries();
   ConsolidateComponentGeometryUsage();

   UpdateCompPins();

   ReportConsolidation();
   
   //CleanWidthTable();
   m_doc->purgeUnusedWidthsAndBlocks(false /* no report */); 
   
   ScrubNames();

   m_logFile.Close();
}

void CGeometryConsolidator::ReportConsolidation()
{
   // Aperture replacements

   if (m_logFilename.IsEmpty())
      return; // Don't bother with loops if not really writing

   POSITION pos = m_duplicateApertures.GetStartPosition();
   while (pos != NULL)
   {
      WORD key;
      void *value;
      m_duplicateApertures.GetNextAssoc(pos, key, value);
      if (value != NULL)
      {
         BlockStruct *newblk = (BlockStruct*)value;
         BlockStruct *oldblk = m_doc->getBlockAt(key);
         CString newname = newblk != NULL ? newblk->getName() : "<block missing>";
         CString oldname = oldblk != NULL ? oldblk->getName() : "<block missing>";
         m_logFile.WriteString("Aperture %s replaced with %s\n", oldname, newname);
      }
   }

   // Padstack replacements
   pos = m_duplicatePadstacks.GetStartPosition();
   while (pos != NULL)
   {
      WORD key;
      void *value;
      m_duplicatePadstacks.GetNextAssoc(pos, key, value);
      if (value != NULL)
      {
         BlockStruct *newblk = (BlockStruct*)value;
         BlockStruct *oldblk = m_doc->getBlockAt(key);
         CString newname = newblk != NULL ? newblk->getName() : "<block missing>";
         CString oldname = oldblk != NULL ? oldblk->getName() : "<block missing>";
         m_logFile.WriteString("Padstack %s replaced with %s\n", oldname, newname);
      }
   }

   // Component geometry replacements
   pos = m_duplicateComponents.GetStartPosition();
   while (pos != NULL)
   {
      WORD key;
      void *value;
      m_duplicateComponents.GetNextAssoc(pos, key, value);
      if (value != NULL)
      {
         BlockStruct *newblk = (BlockStruct*)value;
         BlockStruct *oldblk = m_doc->getBlockAt(key);
         CString newname = newblk != NULL ? newblk->getName() : "<block missing>";
         CString oldname = oldblk != NULL ? oldblk->getName() : "<block missing>";
         m_logFile.WriteString("Component %s replaced with %s\n", oldname, newname);
      }
   }
}

void CGeometryConsolidator::CollectAllInsertRefnames(CMapStringToPtr &refnameMap)
{
   if (m_doc != NULL)
   {
      POSITION filepos = m_doc->getFileList().GetHeadPosition();
      while (filepos != NULL)
      {
         FileStruct *f = m_doc->getFileList().GetNext(filepos);

         if (f != NULL && f->getBlock() != NULL)
         {
            BlockStruct *fileBlk = f->getBlock();

            POSITION datapos = fileBlk->getHeadDataInsertPosition();
            while (datapos != NULL)
            {
               DataStruct *d = fileBlk->getNextDataInsert(datapos);

               if (d != NULL)
               {
                  CString refname = d->getInsert()->getRefname();
                  refnameMap.SetAt(refname, NULL);
               }
            }
         }
      }
   }
}

void CGeometryConsolidator::ScrubNames()
{
   if (m_doc != NULL)
   {
      m_logFile.WriteString("\nComponent Geometry Name Scrubbing\n\n");

      CollectAllComponentGeometries();  // refresh the list

      //
      // First pass, promote Original Name if it is available
      //

      POSITION pos = this->m_allComponentsInDoc.GetStartPosition();
      while (pos != NULL)
      {
         WORD key;     // block number
         void *value;  // block ptr
         this->m_allComponentsInDoc.GetNextAssoc(pos, key, value);
         if (value != NULL)
         {
            BlockStruct *b = (BlockStruct*)value;

            // If block name matches original name, that is as good as it gets,
            // no need to do anything further to that one.
            // If those two names do not match, then try to promote original name.
            if (b->getOriginalName().Compare(b->getName()) != 0)
            {
               // Can only promote original name if it is not empty.
               if (!b->getOriginalName().IsEmpty())
               {
                  if (Graph_Block_Exists(m_doc, b->getOriginalName(), b->getFileNumber()))
                  {
                     // can't change current name to original name, already got one with the original name
                  }
                  else
                  {
                     m_logFile.WriteString("Component geometry name changed from %s to %s\n", b->getName(), b->getOriginalName());
                     b->setName(b->getOriginalName());
                  }
               }
            }
         }
      }



      //
      // Second pass, scrub instance specific refname suffixes that may 
      // have been appended.
      //

      // Collect component insert refnames for easy lookup
      CMapStringToPtr refnameMap;
      CollectAllInsertRefnames(refnameMap);
      
      pos = this->m_allComponentsInDoc.GetStartPosition();
      while (pos != NULL)
      {
         WORD key;     // block number
         void *value;  // block ptr
         this->m_allComponentsInDoc.GetNextAssoc(pos, key, value);
         if (value != NULL)
         {
            BlockStruct *b = (BlockStruct*)value;

            // Block is exempt if current name matches original name.
            if (b->getOriginalName().Compare(b->getName()) != 0)
            {

               // We are looking to remove a suffix that was appended to a 
               // base name. Assume the suffix was separated with an
               // underscore. Make a new name by removing what ever is after
               // the last underscore. If the new name is unique among blocks
               // in the same file as the block (okay if another file has same
               // block) then go with it. Otherwise original name stays.

               int underscoreIndx = b->getName().ReverseFind('_');
               int hyphenIndx = b->getName().ReverseFind('-');
               int selectedIndx = max(underscoreIndx, hyphenIndx);

               if (selectedIndx > 0) // "_" or "-" found and is not 1st char
               {
                  CString existingSuffix = b->getName().Right( b->getName().GetLength() - selectedIndx - 1);

                  // Proceed with block name change only if suffix is known to be a refname
                  void *dontCare = NULL;
                  if (refnameMap.Lookup(existingSuffix, dontCare))
                  {
                     CString newblockname(b->getName());
                     newblockname.Truncate(selectedIndx);

                     if (Graph_Block_Exists(m_doc, newblockname, b->getFileNumber()))
                     {
                        // can't change current name, already got one like the new name
                     }
                     else
                     {
                        m_logFile.WriteString("Component geometry name changed from %s to %s\n", b->getName(), newblockname);
                        b->setName(newblockname);
                     }
                  }
               }
            }

         }
      }
   }
}

void CGeometryConsolidator::SimplifyComplexApertures()
{
   // Try to distill complex apertures to simple apertures

   if (m_doc != NULL)
   {
      m_logFile.WriteString("\nAperture Simplification\n\n");

      for (int i = 0; i < m_doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *apBlk = m_doc->getBlockAt(i);

         if (apBlk != NULL && apBlk->isComplexAperture())
         {
            int insertedBlkNum = apBlk->getComplexApertureSubBlockNumber();
            BlockStruct *insertedBlk = m_doc->getBlockAt(insertedBlkNum);
            if (insertedBlk != NULL)
            {
               // Simplistic for now.
               // Works on blocks that have a single CPoly that forms a rectangle.
               // This is one of the places that are ripe for enhancement, i.e. handle more complexity
               // in complex apertures.

               if (insertedBlk->getDataCount() == 1)
               {
                  DataStruct *d = insertedBlk->getDataList().GetHead();
                  if (d != NULL)
                  {
                     if (d->getDataType() == dataTypePoly)
                     {
                        CPolyList *polylist = d->getPolyList();
                        if (polylist->GetCount() == 1)
                        {
                           CPoly *poly = polylist->GetHead();
                           double lineWidth = m_doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();                        
                           double llx, lly, urx, ury;

                           // If line width is not zero then shape ends up with rounded corners, and
                           // we can't do that with a simple aperture, so skip 'em if not zero width.

                           if (lineWidth == 0.0 && PolyIsRectangle(poly, &llx, &lly, &urx, &ury))
                           {
                              // Convert to rectangular simple aperture
                              // BUT only if origin is at the center (Case dts0100468646).
                              // If origin is at center then sum of corner coord values will be zero, i.e.
                              // lower left X = minus upper right X, likewise for Y's.
                              // NOTE - We could create aperture offsets to deal with non-centered situation, but
                              // word is Stencil Gen won't work with them so we don't want to go there right now.

                              if ( fabs(llx + urx + lly + ury) < 1.0E-6 )
                              {
                                 double xdim = urx - llx;
                                 double ydim = ury - lly;

                                 apBlk->setSizeA((DbUnit)xdim);
                                 apBlk->setSizeB((DbUnit)ydim);
                                 apBlk->setShape(apertureRectangle);

                                 CString apName = apBlk->getName();
                                 m_logFile.WriteString("Complex aperture %s reduced to Simple aperture %s, %0.3f x %0.3f\n",
                                    apName.IsEmpty() ? "<unnamed>" : apName, apertureShapeToName(apertureRectangle), xdim, ydim);
                              }
                           }
                        }
                     }
                  }
               }


            }
         }
      }
      m_logFile.WriteString("\n");
   }

}

void CGeometryConsolidator::NormalizeApertureRotation()
{
   // Some data has been encountered that has a padstack construction such that
   // the pad insert (aka aperture insert) has a rotation and the aperture definition
   // itself has a rotation. ODB++ importer is the noted offender here.
   // The result is a large collection of padstacks and apertures that appear
   // different at a casual glance at their parameters, but in fact after rotations
   // are applied all the padstacks end up giving the same result.
   // Excess apertures and padstacks in the data is annoying customers.
   // So here we normalize the rotations.
   // The goal is to have the apertures defines with zero rotation, and move
   // the total of the rotation into the pad insert in the padstack.

   POSITION apPos = m_allAperturesInDoc.GetStartPosition();
   while (apPos != NULL)
   {
      WORD key;
      void *value = NULL;
      m_allAperturesInDoc.GetNextAssoc(apPos, key, value);
      if (value != NULL)
      {
         BlockStruct *apBlk = (BlockStruct*)value;
         if (apBlk->getRotationDegrees() != 0.0)
         {
            double apRotDeg = apBlk->getRotationDegrees();

            POSITION psPos = this->m_allPadstacksInDoc.GetStartPosition();
            while (psPos != NULL)
            {
               WORD pskey;
               void *psvalue = NULL;
               m_allPadstacksInDoc.GetNextAssoc(psPos, pskey, psvalue);
               if (psvalue != NULL)
               {
                  BlockStruct *psBlk = (BlockStruct*)psvalue;
                  POSITION dPos = psBlk->getHeadDataInsertPosition();
                  while (dPos != NULL)
                  {
                     DataStruct *psData = psBlk->getNextDataInsert(dPos);
                     if (psData != NULL)
                     {
                        int insertedBlkNum = psData->getInsert()->getBlockNumber();
                        if (apBlk->getBlockNumber() == insertedBlkNum)
                        {
                           // Move rotation out of aperture definition and into padstack's insert of aperture.
                           double insertRotDeg = psData->getInsert()->getAngleDegrees();
                           double newInsertRotDeg = normalizeDegrees(insertRotDeg + apRotDeg);

                           // Update the rotation in the insert of this aperture.
                           psData->getInsert()->setAngleDegrees(newInsertRotDeg);
                        }
                     }
                  }
               }
            }

            // The aperture rotation has been moved out of the aperture and into the 
            // inserts of the aperture for all padstacks.
            // Now clear the aperture rotation.
            apBlk->setRotationDegrees( 0.0 );

         }
      }
   }
}

double CGeometryConsolidator::MinimizedApInsertAngleDeg(double insertAngleDeg, ApertureShapeTag shapeTag, double sizeA, double sizeB)
{

   // Minimize the rotation value.
   // For circles, there is no need for rotation other than 0.0.
   // For squares, useful range is 0..<90.
   // For rectangles, useful range is 0..<180.
   // For rectangles we want to follow an aspect ratio convention, we choose sizeA >= sizeB.
   // For oblongs, treat same as rectangles.
   // This way we can consolidate a 5x10 and a 10x5, for example.

   // This is in support of NormalizeCircleSquareRectangleOblongApertures(), and is not general purpose.
   // It applies only to the supported shapes, and it assumes aperture offsets and rotation are already qualified.

   double newInsertRotDeg = insertAngleDeg;

   if (shapeTag == apertureRound)
   {
      newInsertRotDeg = 0.0;  // Rotating a circle is pointless.
   }
   else if (shapeTag == apertureSquare)
   {
      newInsertRotDeg = fmod(newInsertRotDeg, 90.0);
      if (fpeq(newInsertRotDeg, 90.0, 0.005))
         newInsertRotDeg = 0.0;
   }
   else if (shapeTag == apertureRectangle || shapeTag == apertureOblong)
   {
      // Normalize for A >= B. 
      // If we swap A and B then also add 90 to insert rotation.
      // Then normalize the rotation to be 0..<180.

      if (sizeA < sizeB)
      {
         newInsertRotDeg += 90.0;
         newInsertRotDeg = normalizeDegrees(newInsertRotDeg);
      }
      newInsertRotDeg = fmod(newInsertRotDeg, 180.0);
      if (fpeq(newInsertRotDeg, 180.0, 0.005))
         newInsertRotDeg = 0.0; 
   }

   return newInsertRotDeg;
}

void CGeometryConsolidator::NormalizeCircleSquareRectangleOblongApertures()
{
   // Special handling of Circle, Square, Rectangle, and Oblong apertures.
   // Due to predictable nature of these shapes, we can normalize/simplify the usage.
   // Data might be all over the place, for what ever reason.
   // This step applies only when the xy offset in the aperture is zero.
   // Doing this makes the data "more the same" for items that are specified differently
   // but actually end up being the same. This in turn delivers better consolidation.

   CPoint2d ZeroZero(0.,0.);

   POSITION apPos = m_allAperturesInDoc.GetStartPosition();
   while (apPos != NULL)
   {
      WORD key;
      void *value = NULL;
      m_allAperturesInDoc.GetNextAssoc(apPos, key, value);
      if (value != NULL)
      {
         BlockStruct *apBlk = (BlockStruct*)value;
         ApertureShapeTag shapeTag = apBlk->getShape();
         bool apHasZeroOffset = apBlk->getOffset().fpeq(ZeroZero);

         if (apHasZeroOffset && (shapeTag == apertureRound || shapeTag == apertureSquare || 
            shapeTag == apertureRectangle || shapeTag == apertureOblong))
         {
            double apRotDeg = apBlk->getRotationDegrees();
            double sizeA = apBlk->getSizeA();
            double sizeB = apBlk->getSizeB();

            // The original incarnation of this function just updated aperture inserts in padstacks.
            // We then found an bug where a complex aperture contained an insert of a simple aperture, that did not get updated.
            // And so the revelation, although we are overall intending to just consolidate component geometries,
            // changing an aperture is changing a fundemental datum that may be used outside of just
            // padstacks. E.g. a fiducial might insert one of these apertures. So now we just
            // process all the blocks in the ccz, updating all inserts of the given aperture.
            // It doesn't matter what the block is, just process what ever is in the data list.

            for (int i = 0; i < m_doc->getMaxBlockIndex(); i++)
            {
               BlockStruct *psBlk = m_doc->getBlockAt(i); // We'll think of it as padstack block, but it could be any kind of block.

               // The only one to skip is the aperture block we are processing.
               if (psBlk != NULL && psBlk->getBlockNumber() != apBlk->getBlockNumber())
               {
                  POSITION dPos = psBlk->getHeadDataInsertPosition();
                  while (dPos != NULL)
                  {
                     DataStruct *psData = psBlk->getNextDataInsert(dPos);
                     if (psData != NULL)
                     {
                        int insertedBlkNum = psData->getInsert()->getBlockNumber();
                        if (apBlk->getBlockNumber() == insertedBlkNum)
                        {
                           // Move rotation out of aperture definition and into padstack's insert of aperture.
                           double insertRotDeg = psData->getInsert()->getAngleDegrees();
                           double newInsertRotDeg = normalizeDegrees(insertRotDeg + apRotDeg);

                           newInsertRotDeg = MinimizedApInsertAngleDeg(newInsertRotDeg, shapeTag, sizeA, sizeB);

                           // Update the rotation in the insert of this aperture.
                           psData->getInsert()->setAngleDegrees(newInsertRotDeg);
                        }
                     }
                  }
               }
            }

            // The aperture rotation has been moved out of the aperture and into the 
            // inserts of the aperture for all padstacks, er, I mean all blocks of any sort.
            // Now clear the aperture rotation.
            apBlk->setRotationDegrees( 0.0 );

            // And if shape is rectangle or oblong then reset the sizes to ensure sizeA >= sizeB.
            if (apHasZeroOffset && sizeA < sizeB && (shapeTag == apertureRectangle || shapeTag == apertureOblong) )
            {
               apBlk->setSizeA(sizeB);
               apBlk->setSizeB(sizeA);
            }

         }
      }
   }
}

void CGeometryConsolidator::CollectAllApertures()
{
   // Classify apertures as unique or duplicate.
   // Collect unique and duplicate info into the aperture related maps.

   this->m_allAperturesInDoc.RemoveAll();

   if (m_doc != NULL)
   {
      for (int i = 0; i < m_doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *apBlk = m_doc->getBlockAt(i);

         if (apBlk != NULL && apBlk->isAperture())
         {
            m_allAperturesInDoc.SetAt(apBlk->getBlockNumber(), apBlk);
         }
      }
   }
}

void CGeometryConsolidator::ClassifyApertures()
{
   // Classify apertures as unique or duplicate.
   // Collect unique and duplicate info into the aperture related maps.

   this->m_uniqueApertures.RemoveAll();
   this->m_duplicateApertures.RemoveAll();

   // The "Zero Width" width/aperture is sacred. Put it in the unique list
   // up front so it does not end up being replaced, it shall replace any others
   // that are equivalent.
   BlockStruct *zeroWidthBlk = NULL;
   int zwidx = m_doc->getZeroWidthIndex();
   if (zwidx >= 0 && zwidx < m_doc->getWidthTable().GetCount())
   {
      zeroWidthBlk = m_doc->getWidthTable()[zwidx];
      if (zeroWidthBlk != NULL) 
         m_uniqueApertures.SetAt(zeroWidthBlk->getBlockNumber(), zeroWidthBlk);
   }

   // The "Small Width" seems to be in the same boat as "Zero Width". They are
   // equivalent, so we don't really need both, but various parts of camcad assumes one
   // exists, other parts assume the othere exists, so we best leave them be.
   BlockStruct *smallWidthBlk = NULL;
   int swidx = m_doc->getSmallWidthIndex();
   if (swidx >= 0 && swidx < m_doc->getWidthTable().GetCount())
   {
      smallWidthBlk = m_doc->getWidthTable()[swidx];
      if (smallWidthBlk != NULL) 
         m_uniqueApertures.SetAt(smallWidthBlk->getBlockNumber(), smallWidthBlk);
   }

   // Now process the rest
   POSITION pos = this->m_allAperturesInDoc.GetStartPosition();
   while (pos != NULL)
   {
      WORD key;     // block number
      void *value;  // block ptr
      this->m_allAperturesInDoc.GetNextAssoc(pos, key, value);

      if (value != NULL)
      {
         BlockStruct *apBlk = (BlockStruct*)value;

         if (apBlk != NULL && apBlk->isAperture() && apBlk != smallWidthBlk && apBlk != zeroWidthBlk)
         {
            BlockStruct *uniqueApBlk = m_uniqueApertures.FindEquivalent(m_doc, apBlk);
            if (uniqueApBlk == NULL)
            {
               // New unique aperture, add to unique ap map
               m_uniqueApertures.SetAt(apBlk->getBlockNumber(), apBlk);
            }
            else
            {
               // Already got one like that, map duplicate's block num to unique block
               m_duplicateApertures.SetAt(apBlk->getBlockNumber(), uniqueApBlk);
            }
         }
      }
   }
}

void CGeometryConsolidator::ConsolidateApertureUsageInPadstacks()
{
   // Go through all geometries used as pin inserts (which by definition we say
   // are padstacks) and change them to use only apertures from the unique aperture
   // list.

///#define FIRST_TRY_PADSTACK_APERTURE_UPDATER
#ifdef  FIRST_TRY_PADSTACK_APERTURE_UPDATER
   // This implementation uses the padstack list to limit the number of blocks
   // that have to be looked at. This worked fine for all testes thrown at it
   // during development, but it does have a flaw. See #else.

   POSITION pos = this->m_allPadstacksInDoc.GetStartPosition();
   while (pos != NULL)
   {
      WORD key;
      void *value = NULL;
      this->m_allPadstacksInDoc.GetNextAssoc(pos, key, value);
      if (value != NULL)
      {
         BlockStruct *padstack = (BlockStruct*)value;
         POSITION pspos = padstack->getHeadDataInsertPosition();
         while (pspos != NULL)
         {
            DataStruct *paddata = padstack->getNextDataInsert(pspos);

            if (paddata->isInsert())
            {
               int insertedBlkNum = paddata->getInsert()->getBlockNumber();
               BlockStruct *uniqueApBlk = NULL;
               if (this->m_duplicateApertures.Lookup(insertedBlkNum, (void*&)uniqueApBlk))
               {
                  // Inserts a duplicate aperture, change to unique aperture
                  paddata->getInsert()->setBlockNumber( uniqueApBlk->getBlockNumber() );
               }
            }
         }
      }
   } 
#else
   // This does what poly-to-flash does, to be thorough about changing apertures aka widths.
   // It has to look at more blocks, and so it takes longer. But it is safe this way.
   // I suspect some more inpection and twiddling that includes the knowledge that we
   // are converting pad apertures and padstacks, not general apertures or widths, can
   // possibly get us to a more optimal yet safe implementation. For now, we'll have
   // to settle for correct and safe. Or, it is a simple enough loop, maybe we should
   // just keep it.

   POSITION dupApPos = this->m_duplicateApertures.GetStartPosition();
   while (dupApPos != NULL)
   {
      WORD killBlockNum;
      void *value;
      m_duplicateApertures.GetNextAssoc(dupApPos, killBlockNum, value);
      if (value != NULL)
      {
         BlockStruct *keepApBlk = (BlockStruct*)value;
         ExchangeAperture(m_doc, keepApBlk->getBlockNumber(), killBlockNum, false /*do not delete old block*/);
      }
   }
#endif
}

void CGeometryConsolidator::CollectAllPadStacks()
{
   // Put all the padstacks used in the document into a map.
   // Spans all files in doc.
   // A "padstack" is anything that is inserted using insertTypePin by
   // a block of blockTypePcbComponent or blockTypeTestPoint.

   this->m_allPadstacksInDoc.RemoveAll();

   if (m_doc != NULL)
   {
      for (int i = 0; i < m_doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *b = m_doc->getBlockAt(i);

         if (b != NULL &&
            (b->getBlockType() == blockTypePcbComponent ||
            b->getBlockType() == blockTypeTestPoint))
         {
            POSITION pos = b->getHeadDataInsertPosition();
            while (pos != NULL)
            {
               DataStruct *d = b->getNextDataInsert(pos);

               // Although there is a blockTypePadstack it is rare that an importer actually
               // sets it. Usually padstacks end up as blockTypeUnknown. For our purposes
               // we'll presume anything inserted as a pin is a padstack.
               if (d->isInsertType(insertTypePin))
               {
                  int insertedBlkNum = d->getInsert()->getBlockNumber();
                  void *value;
                  if (!m_allPadstacksInDoc.Lookup(insertedBlkNum, value))
                  {
                     BlockStruct *insertedBlk = m_doc->getBlockAt(insertedBlkNum);
                     m_allPadstacksInDoc.SetAt(insertedBlkNum, insertedBlk);
                  }
               }
            }
         }
      }
   }
}

void CGeometryConsolidator::ClassifyPadstacks()
{
   this->m_uniquePadstacks.RemoveAll();
   this->m_duplicatePadstacks.RemoveAll();

   POSITION pos = this->m_allPadstacksInDoc.GetStartPosition();
   while (pos != NULL)
   {
      WORD key;     // block number
      void *value;  // block ptr
      this->m_allPadstacksInDoc.GetNextAssoc(pos, key, value);
      
      if (value != NULL)
      {
         BlockStruct *blk = (BlockStruct*)value;

         BlockStruct *uniqueBlk = m_uniquePadstacks.FindEquivalent(blk, m_doc, m_tolerance);
         if (uniqueBlk == NULL)
         {
            // New unique padstack, add to unique map
            m_uniquePadstacks.SetAt(blk->getBlockNumber(), blk);
         }
         else
         {
            // Already got one like that, map duplicate's block num to unique block.
            // But first... decide which is the better block name.
            // Padstacks may have been generated with various sorts of suffixes, like _m or _bot
            // for bottom side. So we'll punt here, and keep the shortest name.
            // Switching actual block kept is problematic, since there may be multiple entries
            // in the map already designating the chosen unique block, so just swap block names
            // since they are deemed equivalent blocks.
            if (blk->getName().GetLength() > 0 && blk->getName().GetLength() < uniqueBlk->getName().GetLength())
            {
               CString tmp(uniqueBlk->getName());
               uniqueBlk->setName(blk->getName());
               blk->setName(tmp);
            }
            m_duplicatePadstacks.SetAt(blk->getBlockNumber(), uniqueBlk);
         }
      }
   }    
}

void CGeometryConsolidator::ConsolidatePadstackUsageInComponents()
{
   if (m_doc != NULL)
   {
      for (int i = 0; i < m_doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *b = m_doc->getBlockAt(i);

         if (b != NULL &&
            (b->getBlockType() == blockTypePcbComponent ||
            b->getBlockType() == blockTypeTestPoint))
         {
            POSITION pos = b->getHeadDataInsertPosition();
            while (pos != NULL)
            {
               DataStruct *d = b->getNextDataInsert(pos);

               if (d != NULL && d->isInsertType(insertTypePin))
               {
                  int insertedBlkNum = d->getInsert()->getBlockNumber();

                  BlockStruct *uniqueBlk;
                  if (m_duplicatePadstacks.Lookup(insertedBlkNum, (void*&)uniqueBlk))
                  {
                     d->getInsert()->setBlockNumber( uniqueBlk->getBlockNumber() );
                  }
               }
            }
         }
      }
   }
}

void CGeometryConsolidator::CollectAllComponentGeometries()
{
   this->m_allComponentsInDoc.RemoveAll();

   if (m_doc != NULL)
   {
      for (int i = 0; i < m_doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *b = m_doc->getBlockAt(i);

         if (b != NULL &&
            (b->getBlockType() == blockTypePcbComponent ||
            b->getBlockType() == blockTypeTestPoint))
         {
            m_allComponentsInDoc.SetAt(b->getBlockNumber(), b);
         }
      }
   }
}

void CGeometryConsolidator::ClassifyComponentGeometries()
{
   this->m_uniqueComponents.RemoveAll();
   this->m_duplicateComponents.RemoveAll();

   bool insertsOnly = false; // Assume full comparison.
   bool considerOriginalName = true; // Assume we want family grouping by original geometry name.
   if (m_activeControl != NULL) // If controls present, override default with control setting.
   {
      insertsOnly = m_activeControl->GetInsertsOnly();
      considerOriginalName = m_activeControl->GetUseOriginalName();
      m_tolerance = m_activeControl->GetTolerance();
   }

   POSITION pos = this->m_allComponentsInDoc.GetStartPosition();
   while (pos != NULL)
   {
      WORD key;     // block number
      void *value;  // block ptr
      this->m_allComponentsInDoc.GetNextAssoc(pos, key, value);
      if (value != NULL)
      {
         BlockStruct *b = (BlockStruct*)value;

         // When some next control gets added and needs to be passed in here, change the propagation
         // to just sent the whole control set. Would require generating default control set when incoming
         // is NULL.
         BlockStruct *uniqueBlk = m_uniqueComponents.FindEquivalent( m_doc, b, insertsOnly, considerOriginalName, m_tolerance );

         if (uniqueBlk == NULL)
         {
            // First one of this type, save it in unique list
            m_uniqueComponents.SetAt(b->getBlockNumber(), b);
         }
         else
         {
            // Already have one, map duplicate's block num to unique block
            // Keep shortest block names, swap names if the dup block has a more attractive name.
            // Switching actual block kept is problematic, since there may be multiple entries
            // in the map already designating the chosen unique block, so just swap block names
            // since they are deemed equivalent blocks.
            if (b->getName().GetLength() > 0 && b->getName().GetLength() < uniqueBlk->getName().GetLength())
            {
               CString tmp(uniqueBlk->getName());
               uniqueBlk->setName(b->getName());
               b->setName(tmp);
            }
            m_duplicateComponents.SetAt(b->getBlockNumber(), uniqueBlk);
         }
      }
   }
}

void CGeometryConsolidator::ConsolidateComponentGeometryUsage()
{
   m_logFile.WriteString("\nComponent Insert Geometry Replacements\n\n");

   if (m_doc != NULL)
   {
      POSITION filepos = m_doc->getFileList().GetHeadPosition();
      while (filepos != NULL)
      {
         FileStruct *f = m_doc->getFileList().GetNext(filepos);

         if (f != NULL && f->getBlock() != NULL)
         {
            BlockStruct *fileBlk = f->getBlock();

            POSITION datapos = fileBlk->getHeadDataInsertPosition();
            while (datapos != NULL)
            {
               DataStruct *d = fileBlk->getNextDataInsert(datapos);

               if (d != NULL)
               {
                  int insertedBlkNum = d->getInsert()->getBlockNumber();
                  BlockStruct *uniqueBlk = NULL;
                  if (m_duplicateComponents.Lookup(insertedBlkNum, (void*&)uniqueBlk))
                  {
                     CString oldblkname("<Unknown>");
                     void *value;
                     if (m_allComponentsInDoc.Lookup(insertedBlkNum, value))
                        oldblkname = value != NULL ? ((BlockStruct*)value)->getName() : "<Unknown2>";
                     m_logFile.WriteString("Insert %s geometry changed from %s to %s\n", d->getInsert()->getRefname(), oldblkname, uniqueBlk->getName());

                     d->getInsert()->setBlockNumber( uniqueBlk->getBlockNumber() );
                  }
               }
            }
         }
      }

   }
}

void CGeometryConsolidator::UpdateCompPins()
{
   // Comppins have a data member that is the block of the pad (or aperture or padstack)
   // that comprises the comppin target. Consolidation may have replaced and eliminated 
   // that block, so need to update the block number.

   if (m_doc != NULL)
   {
      POSITION filepos = m_doc->getFileList().GetHeadPosition();
      while (filepos != NULL)
      {
         FileStruct *f = m_doc->getFileList().GetNext(filepos);
         if (f != NULL)
         {
            POSITION netpos = f->getHeadNetPosition();
            while (netpos != NULL)
            {
               NetStruct *n = f->getNextNet(netpos);
               if (n != NULL)
               {
                  POSITION cppos = n->getHeadCompPinPosition();
                  while (cppos != NULL)
                  {
                     CompPinStruct *cp = n->getNextCompPin(cppos);
                     if (cp != NULL)
                     {
                        int padblknum = cp->getPadstackBlockNumber();

                        // If that block number is a key in any of the duplicate maps then
                        // replace it with the associated unique block.

                        BlockStruct *uniqueBlk = NULL;

                        if (m_duplicatePadstacks.Lookup(padblknum, (void*&)uniqueBlk))         // most likely
                           cp->setPadstackBlockNumber( uniqueBlk->getBlockNumber() );
                        else if (m_duplicateApertures.Lookup(padblknum, (void*&)uniqueBlk))    // possible, not as likely
                           cp->setPadstackBlockNumber( uniqueBlk->getBlockNumber() );
                        else if (m_duplicateComponents.Lookup(padblknum, (void*&)uniqueBlk))   // not likely but we'll check anyway
                           cp->setPadstackBlockNumber( uniqueBlk->getBlockNumber() );
                     }
                  }
               }
            }
         }
      }
   }
}

void CGeometryConsolidator::CleanWidthTable()
{
   // Never purge width 0

   for (int i = 1; i < m_doc->getNextWidthIndex(); i++)
   {
      BlockStruct *widthAp = m_doc->getWidthTable()[i];

      if (widthAp != NULL)
      {
         void *voidptr;
         if (m_duplicateApertures.Lookup(widthAp->getBlockNumber(), voidptr))
         {
            m_doc->getWidthTable().SetAt(i,NULL);  // obsolete width
         }
      }
   }


}
