// $Header: /CAMCAD/4.5/read_wrt/Orcltd_o.cpp 35    8/05/06 4:56p Kurt Van Ness $

/*****************************************************************************/
/*
  Project CAMCAD
  Router Solutions Inc.
  Copyright © 1994, 2000. All Rights Reserved.

/****************************************************************************/
/*
   error messages from ORCAD LAYOUT.

0001361 ASCXLEX Bad units
0001430 ASCXMAIN Data Heap
0001852 ASCXPACK Unable to allocate packNameTable
0001853 ASCXPACK Unable to allocate stringPtr
0005860 STRXMAIN Failed to allocate SymNameTab in StrX
0005861 STRXMAIN Failed to allocate CompNameTab in StrX
0005862 STRXMAIN Failed to allocate Comp HeapSlice in StrX
0005863 STRXMAIN Failed to allocate Sym HeapSlice in StrX
0005864 STRXLIB Failed to allocate Comp HeapSlice in ParseIFL
0005865 STRXLIB Failed to allocate Sym HeapSlice in ParseIFL
0005882 STRXMAIN Expected ComPushDir (TCH_LIB)
0006260 STRBFILE Failed to allocate SymNameTab in StrX
0006261 STRBFILE Failed to allocate CompNameTab in StrX
0006263 STRBFILE Failed to allocate Sym HeapSlice in StrX
0006264 STRBFILE Failed to allocate Comp HeapSlice in ParseIFL
0006265 STRBFILE Failed to allocate Sym HeapSlice in ParseIFL
0006267 STRBFILE Failed to allocate Comp HeapSlice in StrX
0007101 NPPSPOOL Obs color heap
0007104 NPPSPOOL "(short) colorView.kind" out of range (0, TEXT_OTHER-1)
0007104 NPPINIT "(short) colorView.kind" out of range (0, TEXT_OTHER-1)
0007105 NPPSPOOL Shape not supported in drawOnePad
0007106 NPPSPOOL Obs colors for one layer
0007107 NPPSPOOL "(short) colorView.kind" out of range (0, OBS_OTHER-1)
0007107 NPPINIT "(short) colorView.kind" out of range (0, OBS_OTHER-1)
0007108 NPPSPOOL Text colors for one level
0007109 NPPSPOOL Text color heap
0007112 NPPHATCH Failed to allocate hatchHeap
0007113 NPPHATCH Failed assumption obsPtr->hatchKind
0007116 NPPHATCH Failed to allocate poly data in HatchObs
0007117 NPPHATCH Failed to allocate poly data in HatchObs
0007118 NPPHATCH Failed assumption obsPtr->hatchKind
0007119 NPPHATCH Failed to increase polyList in HatchObs
0007120 NPPHAT2 Failed to increase polyList in HatchObs
0007121 NPPHAT2 Failed assumption polyPtr->hatchKind
0007122 NPPHAT2 Failed assumption polyPtr->hatchKind
0007123 NPPHAT2 Failed to allocate hatchHeap
0007124 NPPHAT2 Failed to allocate polyList in HatchObs
0007125 NPPHAT2 Failed to allocate poly data in HatchObs
0007131 NPPMAIN Failed assumption InBatch == NO
0007140 NPPRAST2 Failed assumption dy > 0
0007142 NPPAPP1 Unknown shape
0007144 NPPAPP1 Unknown kind
0007151 NPPGER Allocating appHashTable
0007153 NPPGER Allocating one DRAW RECORD
0007154 NPPGER Allocating one FLASH RECORD
0007154 NPPGER Allocating one DRAW RECORD
0007154 NPPSIMAP Allocating one DRAW RECORD
0007156 NPPGER Failed assumption DbView.boardXMin >= 0
0007157 NPPGER Failed assumption DbView.boardYMin >= 0
0007158 NPPGER Unknown shape
0007159 NPPGER Failed assumption prevPtr
0007170 NPPPLOT Failed assumption DbView.boardYMin >= 0
0007171 NPPPLOT Allocating appHashTable
0007173 NPPPLOT Allocating one DRAW RECORD
0007174 NPPPLOT Allocating one FLASH RECORD
0007175 NPPPLOT yScanList HeapAssign failure in nppplot
0007176 NPPPLOT yScan HeapAssign failure nppplot
0007178 NPPPLOT Failed to allocate YscanTab in nppplot
0007179 NPPPLOT Failed to allocate YscanTab in nppplot
0007180 NPPPLOT Failed to allocate Vec HeapSlice in nppplot
0007182 NPPPLOT Failed to allocate Vec HeapSlice in PltMain
0007184 NPPPLOT conTableHeap HeapAssign failure in nppplot
0007185 NPPPLOT Failed to allocate conTablePtr in nppplot
0007188 NPPPLOT Failed to allocate penObject
0007189 NPPPLOT Failed assumption DbView.boardXMin >= 0
0007191 NPPINIT Obs color heap
0007191 NPPINIT "MaxDevice" out of range (1, MAX_DEVICE)
0007196 NPPINIT Obs colors for one layer
0007198 NPPINIT Text colors for one layer
0007234 NPPDLG Failed assumption altIdx != 0
0007237 NPPDLG Failed assumption altIdx != 0
0007238 NPPDLG Failed assumption altIdx != 0
0007400 NPPRAST2 Failed to allocate file_buff HeapSlice in PltRast2
0007431 NPPRAST1 "(unsigned int)NewVector.y1" out of range (1, YscanAlloc * 2)
0007432 NPPRAST1 Failed to allocate YscanTab linklist HeapSlice in npprast1
0007433 NPPRAST1 "(unsigned int)y1" out of range (1, YscanAlloc * 2)
0007451 NPPHP Expected ! plotBuffHandle
0007452 NPPHP Allocating pltBuffHandle
0007453 NPPHP Locking pltBuffHandle
0007454 NPPHP Expected ! imageHandle
0007455 NPPHP Allocating imageHandle
0007456 NPPHP Locking imageHandle
0007480 NPPLEX Failed assumption sourceFile
0007510 NPPHPGL Allocating appHashTable
0007511 NPPHPGL Too many appertures required
0007512 NPPHPGL Allocating one DRAW RECORD
0007513 NPPHPGL Allocating one FLASH RECORD
0007515 NPPHPGL Failed to allocate penObject in PltMain
0007517 NPPHPGL Failed assumption dpb->format == HPGL_FORMAT
0007518 NPPHPGL Failed assumption DbView.boardXMin >= 0
0007519 NPPHPGL Failed assumption DbView.boardYMin >= 0
0007533 NPPCOMP local heap
0007535 NPPCOMP Allocating buffer
0007633 ORCBACK Failed assumption gatePin == NULL
0007718 NPPHATCH Failed to allocate polyList in HatchObs
0008758 MTBMAIN Failed to assign heap1 in mtbmain
0008759 MTBMAIN Failed to assign heap2 in mtbmain
0008760 MTBMAIN Failed to allocate LocTable in mtbmain
0008761 MTBMAIN Failed to allocate LocTable in mtbmain
0008762 MTBMAIN Failed to allocate Loc HeapSlice in mtbmain
0008763 MTBMAIN Failed to allocate Loc HeapSlice in mtbmain
0008788 MTBMAIN Failed to assign heap3 in mtbmain
0009112 NPPRPT Allocating Drill Sort Table
0010130 FNETMAIN Data Heap
0010200 FNETLEX Failed assumption sourceFile
0011111 PROTXCON "viaCode" out of range (1, DbView.viaMax)
0011111 PROTXCON "viaCode" out of range (1, DbView.viaMax)
0011111 PROTXCMP "thruId" out of range (1, DbView.viaMax)
0011111 PROTXCMP "thruId" out of range (1, DbView.viaMax)
0011111 PROTBNET "viaIndex" out of range (1, DbView.viaMax)
0011111 PROTBNET "viaIndex" out of range (1, DbView.viaMax)
0011111 PROTBNET "viaIndex" out of range (1, DbView.viaMax)
0011111 PROTBNET "viaIndex" out of range (1, DbView.viaMax)
0011111 PROTBNET "viaIndex" out of range (1, DbView.viaMax)
0011111 PROTBCON "viaIndex" out of range (1, DbView.viaMax)
0015200 ORCADLEX Failed assumption sourceFile
0015230 ORCADMAN Data Heap
0016201 WINHAT2 Failed to allocate hatchHeap
0016202 WINHAT2 Failed to allocate polyList in HatchObs
0016205 WINHAT2 Failed to increase polyList in HatchObs
0016221 WINHAT2 Failed assumption polyPtr->hatchKind
0016223 WINHAT2 Failed to allocate poly data in HatchObs
0016237 WINHAT2 Bad drawing mode in drawHatchLine
0016271 WINHAT2 Failed assumption polyPtr->hatchKind
0016422 WINMNL Failed to allocate PackMap
0016432 WINMNL Failed to allocate CompTable entry
0016442 WINMNL Failed to allocate NetTable entry
0016701 WINMENU2 Unknown command menu
0016702 WINMENU2 Failed assumption AppendMenu (popupMenu, MF_SEPARATOR, 0, (LPSTR) NULL)
0016704 WINMENU2 Failed assumption popupMenu
0016706 WINMENU2 Failed assumption item != NULL
0016708 WINMENU2 Failed assumption popupMenu
0016735 WINMENU2 "lstrLen (acc)" out of range (0, 7)
0016735 WINMENU2 "lstrLen (string)" out of range (1, 39)
0016789 WINMENU2 Failed assumption popupMenu
0016790 WINMENU2 Failed assumption popupItemCount < MAX_POPUP_ITEMS
0016791 WINMENU2 Unrecognized hotkey
0016792 WINMENU2 Failed assumption AppendMenu (popupMenu, MF_STRING, (UINT) id, (LPSTR) popupItem[popupItemCount].string)
0016793 WINMENU2 Failed assumption AppendMenu (popupMenu, MF_OWNERDRAW, (UINT) id, (LPTSTR)id)
0016795 WINMENU2 Failed assumption popupMenu
0016796 WINMENU2 Failed assumption popupItemCount < MAX_POPUP_ITEMS
0016797 WINMENU2 Failed assumption AppendMenu (popupMenu, MF_OWNERDRAW, (UINT)id, (LPTSTR)id)
0016798 WINMENU2 Failed assumption popupMenu
0016799 WINMENU2 Failed assumption popupItemCount < MAX_POPUP_ITEMS
0016906 MZMAZE Failed assumption (SfView.xMax + 1) * (long) SfView.routeLayers < EXP_SIZE
0017324 DBOBS Failed assumption obsPtr->crns < OBS_CRN_LIMIT
0019035 MZCHECK2 Failed assumption object > 0
0021105 DBIO Invalid filemode in DbOpenfile
0021110 DBIO Failed assumption !inputFile
0021111 DBIO Failed assumption !outputFile
0021131 DBIO Failed assumption inputFile
0021135 DBIO Failed assumption inputFile
0021136 DBIO Failed assumption fseek (inputFile, offset, SEEK_SET) == 0
0021140 DBIO Failed assumption inputFile
0021143 DBIO Failed assumption outputFile
0021148 DBIO mode must be 'r' or 'w'
0021181 DBIO Failed assumption inputFile
0021182 DBIO Failed assumption outputFile
0021190 DBIO "ptr->sym.header.pads" out of range (0, SYM_PAD_LIMIT)
0021190 DBIO "ptr->v70Sym.header.pads" out of range (0, SYM_PAD_LIMIT)
0021191 DBIO "ptr->pack.header.pads" out of range (0, SYM_PAD_LIMIT)
0021195 DBIO Failed assumption len <= NAME_STR_LIMIT + 1
0021196 DBIO "len" out of range (1, LSTR_LIMIT + 1)
0021197 DBIO Default kind in DbWrite
0021255 DBDEFALT Default union kind in DbDefault
0021272 DBDEFALT Unknown union kind in DbSizeOf
0021302 DBLEV Failed assumption theDb
0021303 DBLEV Failed assumption theDb
0021304 DBLEV Failed assumption theDb
0021306 DBLEV Failed assumption theDb
0021307 DBLEV Failed assumption theDb
0021308 DBLEV Failed assumption theDb
0021308 DBLEV Failed assumption theDb
0021309 DBLEV Failed assumption theDb
0021309 DBLEV Failed assumption theDb
0021312 DBLEV "theDb->header.levMax" out of range (1, LEV_ETCH_LIMIT)
0021340 DBLEV "layer" out of range (0, LEV_ETCH_LIMIT)
0021341 DBLEV "layer" out of range (0, LEV_ETCH_LIMIT)
0021342 DBLEV "layer" out of range (0, LEV_ETCH_LIMIT)
0021343 DBLEV "oldLayer" out of range (0, LEV_ETCH_LIMIT)
0021344 DBLEV "newLayer" out of range (0, LEV_ETCH_LIMIT)
0021345 DBLEV "oldLayer" out of range (0, LEV_ETCH_LIMIT)
0021346 DBLEV "newLayer" out of range (1, theDb->header.levMax+1)
0021375 DBLEV "layer" out of range (1, theDb->header.levMax)
0021378 DBLEV Failed assumption theDb
0021379 DBLEV "layer" out of range (0, theDb->header.levMax)
0021382 DBLEV Failed assumption theDb
0021386 DBLEV Failed assumption theDb
0021387 DBLEV Failed assumption theDb
0021391 DBLEV Expected lstrLen (defaultNames[layer*3 + 1]) <= 4
0021392 DBLEV Failed assumption theDb
0021400 DBCOMP Failed assumption theDb->comp.blockLimit == 0
0021410 DBCOMP Failed assumption BLOCK_SIZE * (long) sizeof(COMP_VIEW) <= EXP_SIZE
0021412 DBCOMP Too many components during CompCreate
0021413 DBCOMP Not enough blocks to meet COMP_LIMIT
0021414 DBCOMP Failed to allocate blockTable[blockLimit++] in CompCreate
0021420 DBCOMP "index" out of range (1, theDb->header.compMax)
0021422 DBCOMP Failed assumption blockPtr
0021424 DBCOMP "index" out of range (1, theDb->header.compMax)
0021426 DBCOMP Failed assumption blockPtr
0021427 DBCOMP "index" out of range (1, theDb->header.compMax)
0021434 DBCOMP "compPtr->symIndex" out of range (0, theDb->header.symMax)
0021435 DBCOMP "compPtr->cluster" out of range (0, COMP_LIMIT)
0021436 DBCOMP Failed assumption compRenumberHeap != 0
0021450 DBCOMP "compIndex" out of range (1, theDb->header.compMax)
0021467 DBCOMP "i" out of range (1, theDb->header.compMax)
0021469 DBCOMP "j" out of range (1, theDb->header.compMax)
0021482 DBCOMP Allocating renumbering heap
0021483 DBCOMP Allocating renumbering table
0021489 DBCOMP "compId" out of range (1, theDb->header.compMax)
0021493 DBCOMP Failed assumption compId >= 0
0021494 DBCOMP "symId" out of range (1, theDb->header.symMax)
0021501 DBOBS3 Failed assumption theDb
0021502 DBOBS3 Failed assumption theDb
0021503 DBOBS3 Failed assumption theDb
0021504 DBOBS3 Failed assumption theDb
0021537 DBOBS3 "src->crns" out of range (0, OBS_CRN_LIMIT)
0021581 DBOBS3 Failed assumption view->symId == 0
0021583 DBOBS3 "view->symId" out of range (1, theDb->header.symMax)
0021587 DBOBS3 Failed assumption compView.symIndex == view->symId
0021601 DBTEXT Failed assumption theDb
0021602 DBTEXT Failed assumption theDb
0021603 DBTEXT Failed assumption theDb
0021604 DBTEXT Failed assumption theDb
0021605 DBTEXT Failed assumption theDb
0021606 DBTEXT Failed assumption theDb
0021607 DBTEXT Failed assumption theDb
0021608 DBTEXT Failed assumption theDb
0021609 DBTEXT Failed assumption theDb
0021610 DBTEXT Failed to allocate theDb->getText().txtTable in TextInit
0021611 DBTEXT Failed to allocate theDb->getText().symFirst in TextInit
0021612 DBTEXT Failed to allocate theDb->getText().symFirst in TextInit
0021638 DBTEXT Failed assumption theDb->getText().txtTable
0021639 DBTEXT Failed assumption theDb->getText().txtTable
0021643 DBTEXT "symId" out of range (1, theDb->header.symMax)
0021644 DBTEXT Too many Text pieces
0021645 DBTEXT "compId" out of range (1, theDb->header.compMax)
0021646 DBTEXT "txtId" out of range (1, theDb->header.txtMax)
0021647 DBTEXT Failed to allocate theDb->getText().txtTable in TextCreate
0021649 DBTEXT "index" out of range (1, theDb->header.txtMax)
0021650 DBTEXT "index" out of range (1, theDb->header.txtMax)
0021651 DBTEXT "(short) view->type" out of range (0, TEXT_OTHER)
0021652 DBTEXT Failed assumption view->width >= 0
0021653 DBTEXT Failed assumption view->height >= 0
0021654 DBTEXT Failed assumption view->radius >= 0
0021655 DBTEXT "view->rotation" out of range (0, 4 * COMP_90_DEGREES)
0021656 DBTEXT "(short) view->lev" out of range (0, theDb->header.levMax)
0021657 DBTEXT "view->aspect" out of range (1, 1000)
0021658 DBTEXT "view->charRot" out of range (0, 4* COMP_90_DEGREES)
0021660 DBTEXT "index" out of range (1, theDb->header.txtMax)
0021661 DBTEXT "i" out of range (1, theDb->header.txtMax)
0021661 DBTEXT Can't allocate renumber table
0021662 DBTEXT "j" out of range (1, theDb->header.txtMax)
0021668 DBTEXT Failed assumption lstrLen(view->string)
0021671 DBTEXT "textPtr->symId" out of range (0, theDb->header.symMax)
0021672 DBTEXT Failed to allocate theDb->getText().symFirst in addToLink
0021675 DBTEXT "-textPtr->symId" out of range (0, theDb->header.compMax)
0021676 DBTEXT Failed to allocate theDb->getText().compFirst in addToLink
0021680 DBTEXT "index" out of range (1, theDb->getText().txtLimit)
0021681 DBTEXT Failed assumption --count > 0
0021682 DBTEXT Expected view->symId == 0
0021683 DBTEXT Text not found on correct list
0021684 DBTEXT Expected view->symId != 0
0021687 DBTEXT Failed assumption theDb
0021688 DBTEXT Failed assumption theDb
0021691 DBTEXT Failed assumption theDb
0021691 DBTEXT Failed to allocate theDb->getText().txtTable in TextPut for first time
0021692 DBTEXT Failed assumption theDb
0021693 DBTEXT Failed assumption theDb
0021694 DBTEXT Failed assumption theDb
0021701 DBFONT Cannot read font file
0021703 DBFONT Failed to read font file header
0021710 DBFONT Font header format error
0021711 DBFONT Font header format error
0021712 DBFONT Font header format error
0021715 DBFONT "(short) fontHeader.cellSize" out of range (0, MAX_CELL_SIZE)
0021717 DBFONT "(short) fontHeader.numChars" out of range (1, MAX_FONT_CHARS)
0021720 DBFONT Failed to allocate font charTable
0021723 DBFONT Failed to read font char
0021725 DBFONT "fontChar.index" out of range (1, MAX_FONT_CHARS)
0021726 DBFONT "fontChar.segs" out of range (0, MAX_CHAR_SEGS)
0021730 DBFONT Failed to allocate font segTable
0021733 DBFONT Failed to read font segments
0021741 DBFONT Failed assumption charTable
0021775 DBFONT "charIdx" out of range (1, MAX_FONT_CHARS)
0021776 DBFONT "charIdx" out of range (1, MAX_FONT_CHARS)
0021807 DBLOAD Failed assumption theDb
0021811 DBLOAD "size" out of range (0, LSTR_LIMIT)
0021814 DBLOAD Failed assumption !dbFound
0021814 DBLOAD Failed assumption !dbFound
0021817 DBLOAD "size" out of range (2, sizeof (ioBuff->conn))
0021817 DBLOAD "size" out of range (2, sizeof (ioBuff->v70Conn))
0021823 DBLOAD Expected CompSymNext (0, symIndex) == 0); // Shouldn't happen, but... ...
0021830 DBLOAD Expected nameCount == 1
0021830 DBLOAD Expected nameCount == 1
0021837 DBLOAD Failed assumption nameCount < NAME_COUNT_LIMIT
0021838 DBLOAD "size" out of range (0, NAME_STR_LIMIT+1)
0021839 DBLOAD "nameCount" out of range (1, 3)
0021843 DBLOAD Text attached to derived footprint
0021845 DBLOAD undefined binaryType in DbLoad
0021846 DBLOAD Expected nameCount == 1
0021849 DBLOAD Failed assumption namesProcessed == nameCount
0021851 DBLOAD Failed assumption nameCount == symPtr->header.pads + 1
0021851 DBLOAD Failed assumption nameCount == v70SymPtr->header.pads + 1
0021853 DBLOAD "nameCount" out of range (1, 3)
0021854 DBLOAD Failed assumption size == sizeof (LEV_VIEW)
0021855 DBLOAD Failed assumption nameCount == packPtr->header.pads * 2 + 1
0021856 DBLOAD Failed assumption size == sizeof (THRU_VIEW)
0021857 DBLOAD Expected nameCount == 1
0021858 DBLOAD Expected nameCount == 1
0021858 DBLOAD Expected nameCount == 1
0021861 DBLOAD Expected nameCount == 1
0021862 DBLOAD Failed assumption ioBuff->db.levMax > 0
0021863 DBLOAD Failed assumption ioBuff->db.levMax <= LEV_ETCH_LIMIT
0021865 DBLOAD Failed assumption ioBuff->db.gridDiv >= 0
0021866 DBLOAD Failed assumption ioBuff->db.viaGrid >= 0
0021873 DBLOAD Failed assumption theDb
0021880 DBLOAD "binaryType" out of range (DEFAULT_BINARY, BINARY_TYPE_LIMIT - 1)
0021887 DBLOAD "nameCount" out of range (1, 4)
0021889 DBLOAD Expected nameCount == 0
0021894 DBLOAD Expected nameCount == 3
0021898 DBLOAD Default kind in DbLoad
0021898 DBLOAD Default kind in DbLoad
0021901 DBSAVE Failed assumption theDb
0021945 DBSAVE undefined binaryType in DbSave
0021976 DBSAVE "drillSymbols" out of range (0, DRILL_LIMIT + SPECIAL_SYM_LIMIT)
0021980 DBSAVE "binaryType" out of range (DEFAULT_BINARY, BINARY_TYPE_LIMIT - 1)
0021999 DBSAVE Security device failure
0022010 DBERR Failed to allocate errTable in DbErrInit
0022011 DBERR Failed assumption theDb
0022012 DBERR Failed assumption theDb
0022013 DBERR Failed assumption theDb
0022014 DBERR Failed assumption theDb
0022017 DBERR Failed assumption theDb
0022023 DBERR Failed assumption theDb
0022031 DBERR Failed to allocate errTable in ErrorPut for first time
0022044 DBERR Too many Errors
0022047 DBERR Failed to allocate errTable in DbErrorCreate
0022049 DBERR "index" out of range (1, theDb->header.errMax)
0022050 DBERR "index" out of range (1, theDb->header.errMax)
0022051 DBERR "(short) ptr->kind" out of range (1, ERR_KIND_LIMIT)
0022056 DBERR "(short) ptr->lev" out of range (0, theDb->header.levMax)
0022060 DBERR "index" out of range (1, theDb->header.errMax)
0022087 DBERR Failed assumption theDb
0022091 DBERR Bad error type in ErrorTypeName
0022093 DBERR Bad error type in ErrorIsSpacing
0023002 ASCWDATA Failed assumption nameList
0023004 ASCWDATA Failed assumption nameList[index]
0023005 ASCWDATA Failed assumption nameCount < NAME_COUNT_LIMIT
0023006 ASCWDATA Failed assumption len <= LSTR_LIMIT + 1
0023007 ASCWDATA "nameIndex" out of range (0, nameCount)
0023010 ASCWDATA Failed assumption nameCount < NAME_COUNT_LIMIT
0023041 ASCWDATA Can't allocate compNameList
0023042 ASCWDATA Failed assumption compNameList
0023042 ASCWDATA Failed assumption symPinList
0023043 ASCWDATA Can't allocate compNameList[index]
0023050 ASCWDATA "compNo" out of range (1, MaxComp)
0023051 ASCWDATA Can't allocate nameList
0023052 ASCWDATA Failed assumption netNameList
0023053 ASCWDATA Can't allocate netNameList[index]
0023060 ASCWDATA "netNo" out of range (1, MaxNet)
0023091 ASCWDATA Can't allocate compFrViaList
0023092 ASCWDATA Failed assumption compFrViaList
0023627 ASCWDATA Failed assumption nameCount == packPtr->header.pads * 2 + 1
0023646 ASCWDATA Failed assumption netIndex <= NET_LIMIT
0023646 ASCWDATA Failed assumption netIndex <= NET_LIMIT
0023876 DBLOAD3 Failed assumption sourceFile == NULL
0023879 DBLOAD3 Failed assumption sourceFile != NULL
0023888 DBLOAD3 Failed assumption compPtr->partName != 0
0023897 DBLOAD3 LIBRARY not enabled
0023987 ASCWDATA Can't allocate symPinList
0026425 DBOBS3 "crn" out of range (0, obsPtr->crns - 1)
0028692 DBALT Failed assumption theDb->alt.depth == 0
0030511 WINPEN Allocate window's logical palette
0031106 WINCTL Failed assumption Dlg->ctl
0031107 WINCTL "just" out of range (0, CELL_LIMIT)
0031109 WINCTL Multiply defined control
0031181 WINCTL Too many controls
0031257 WINMAIN Failed assumption ActiveWindow
0031301 WINADD Control id not found in UDSetFocus
0031308 WINDLG Dialog Heaps
0031309 WINDLG Cannot allocate Dialog globals
0031310 WINDLG Allocate control records
0031313 WINDLG Failed assumption ourDlg->nestLevel == 0
0031344 WINDLG Failed assumption Dlg != NULL
0031345 WINDLG Failed assumption Dlg != NULL
0031346 WINDLG Failed assumption Dlg != NULL
0031346 WINDLG Failed assumption Dlg->dialogHandle != 0
0031347 WINDLG Failed assumption Dlg != NULL
0031352 WINDLG2 Failed assumption idx < Dlg->ctlMax
0031352 WINDLG Failed assumption idx < Dlg->ctlMax
0031356 WINDLG Failed assumption Dlg->dialogHandle != 0
0031403 WINDRAW Bad via shape
0031445 WINDRAW Failed assumption DATA->hDC
0031446 WINDRAW Failed assumption (connPtr->crn[crn + 1].flags & CRN_ARC)
0031453 WINDRAW Failed assumption DATA->hDC
0031455 WINDRAW Failed assumption DATA->hDC
0031615 WINDRAW2 "Ui->level" out of range (1, DbView.levMax)
0031633 WINPEN Bad color rule
0031637 WINDRAW2 Bad drawing mode in DrawRectangle
0031638 WINDRAW2 Bad drawing mode in DrawRectangle
0031666 WINDRAW2 "lev" out of range (0, DbView.levMax)
0031684 WINPEN "(short) colorView.kind" out of range (0, TEXT_OTHER-1)
0031685 WINPEN "(short) colorView.kind" out of range (0, OBS_OTHER-1)
0031724 WINIPC Failed assumption atom
0031752 WINIPC Failed assumption LayoutToOrCAPMessage != 0
0031756 WINIPC Failed assumption backAtom
0031772 WINIPC Failed assumption lstrLen (command) + lstrLen(parameters) < CMD_LINE_LIMIT
0031817 WINSYS Failed assumption Ui
0031818 WINSYS Failed assumption info
0031819 WINSYS Failed assumption info->hWnd
0031821 WINSYS Failed assumption idleCursorHandle = ...
0031823 WINSYS Failed assumption commandCursorHandle = ...
0031825 WINSYS Failed assumption helpCursorHandle = ...
0031827 WINSYS Failed assumption dragCursorHandle = ...
0031829 WINSYS Failed assumption noCursorHandle = ...
0031831 WINSYS Failed assumption zoomCursorHandle = ...
0031833 WINSYS Bad cursor shape
0031835 WINSYS Failed assumption cursorHandle
0031857 WINSYS Failed assumption ApplicWindow
0031858 WINSYS Failed assumption info
0031859 WINSYS Failed assumption info->hWnd
0031873 WINQUERY Bad kind in UProbe
0031890 WINSYS Cannot find layout.lic
0031899 WINSYS Cannot find layout.dat
0031935 WINFILE Expected DbView.valid
0031990 WINFILE Cannot find layout.lic
0031999 WINFILE Unexpected command
0032037 WINOBS Bad drawing mode in DrawObsInstance
0032046 WINDRAW3 Bad drawing mode in DrawTextSegs
0032071 WINOBS "(short) kind" out of range (0, OBS_OTHER)
0032072 WINOBS Expected DATA->hDC
0032201 WINMENU Failed assumption accelTable
0032203 WINMENU Too many hotkeys defined
0032205 WINMENU Hotkey already defined (and different)
0032207 WINMENU Failed assumption accelTable
0032209 WINMENU Bad special accel chord
0032216 WINMENU Failed assumption accelTable
0032217 WINMENU unknown hotkey type
0032219 WINMENU Bad pulldown
0032221 WINMENU Can't find own menu items
0032223 WINMENU Failed assumption subMenu
0032223 WINMENU Failed assumption subMenu
0032225 WINMENU Failed assumption AppendMenu ( parentMenu, MF_POPUP, (UINT) subMenu, ...
0032225 WINMENU Failed assumption AppendMenu ( mainMenu, MF_POPUP, (UINT) subMenu, ...
0032227 WINMENU Failed assumption subMenu
0032229 WINMENU Failed assumption addIsValid
0032231 WINMENU Failed assumption menuPosition
0032233 WINMENU Unrecognized hotkey
0032235 WINMENU Failed assumption AppendMenu ( menuPosition, MF_STRING, (UINT) id, string)
0032237 WINMENU Failed assumption subMenu
0032239 WINMENU Failed assumption InsertMenu ( mainMenu, ...
0032241 WINMENU Failed assumption subMenu
0032243 WINMENU Failed assumption InsertMenu ( mainMenu, ...
0032245 WINMENU Failed assumption subMenu
0032247 WINMENU Failed assumption InsertMenu ( mainMenu, ...
0032249 WINMENU Failed assumption mainMenu
0032255 WINMENU Failed assumption SetMenu (hWnd, mainMenu)
0032257 WINMENU Failed assumption addIsValid
0032257 WINMENU Failed assumption addIsValid
0032259 WINMENU "accel" out of range ('A', 'Z')
0032259 WINMENU "accel" out of range ('A', 'Z')
0032261 WINMENU Failed assumption addIsValid
0032261 WINMENU Failed assumption addIsValid
0032263 WINMENU Failed assumption menuPosition
0032265 WINMENU Failed assumption AppendMenu ( hMenuParent, MF_SEPARATOR, 0, NULL)
0032265 WINMENU Failed assumption AppendMenu ( menuPosition, ...
0032267 WINMENU Failed assumption addIsValid
0032269 WINMENU Failed assumption accelPtr
0032273 WINMENU Failed assumption mainMenu
0032275 WINMENU Bad menu state
0032279 WINMENU Failed assumption DeleteMenu ( mainMenu, ...
0032281 WINMENU Failed assumption DeleteMenu ( mainMenu, ...
0032283 WINMENU "newLevel" out of range (0, DbView.levMax)
0032285 WINMENU "fKey" out of range (2, 10)
0032287 WINMENU Failed assumption (lParam & 0x80000000) == 0
0032391 WINPAINT Failed assumption obsPtr->symId == 0
0032399 WINPAINT "id" out of range (IDM_L1, IDM_L1 + DbView.levMax - 1)
0032561 WINZOOM Failed assumption DbView.viewWidth > 0
0032562 WINZOOM Failed assumption DbView.viewHeight > 0
0032585 WINZOOM Default id in ZoomCommand
0032597 WINZOOM Failed assumption DbView.batchIndex == 0
0032617 WINADD No such item in this dialog
0032618 WINADD No such item in this dialog
0032619 WINADD No such item in this dialog
0032622 WINADD Failed assumption Dlg->nestLevel
0032625 WINADD Unknown style in UDAddCombo
0032628 WINADD Unknown style in UDAddButton
0032629 WINADD Unknown style in UDAddStatic
0032633 WINADD Failed assumption idx < Dlg->ctlMax
0032636 WINADD Failed assumption idx < Dlg->ctlMax
0032637 WINADD Failed assumption idx < Dlg->ctlMax
0032645 ASCWDATA Failed assumption compIndex <= COMP_LIMIT
0032647 WINADD Failed assumption idx < Dlg->ctlMax
0032647 WINADD Failed assumption idx < Dlg->ctlMax
0032660 WINADD "item" out of range (1, ctl[idx].count)
0032660 WINADD "item" out of range (1, ctl[idx].count)
0032661 WINADD Failed assumption idx < Dlg->ctlMax
0032661 WINADD Failed assumption idx < Dlg->ctlMax
0032662 WINADD Failed assumption ctl[idx].class != COMBO_CLASS); 
0032662 WINADD Failed assumption ctl[idx].class != COMBO_CLASS); 
0032663 WINADD Failed assumption len <= maxLen
0032664 WINADD "item" out of range (1, ctl[idx].count)
0032665 WINADD Failed assumption idx < Dlg->ctlMax
0032666 WINADD Failed assumption idx < Dlg->ctlMax
0032667 WINADD Failed assumption idx < Dlg->ctlMax
0032668 WINADD Failed assumption idx < Dlg->ctlMax
0032669 WINADD Failed assumption idx < Dlg->ctlMax
0032670 WINADD Failed assumption item <= ctl[idx].itemMax
0032671 WINADD Failed assumption index > 0
0032673 WINADD Could not find control id
0032675 WINADD Control id not found in UDSetButtonState
0032678 WINADD Unknown style
0032679 WINADD Control id not found in UDIsButtonChecked
0032685 WINADD Failed assumption ctl[idx].count <= ctl[idx].itemMax
0032812 WINASK AskInterrupt terminated (debug version only)
0032814 WINASK AskInterrupt Terminated (debug version only)
0032903 WININIT Incompatible library
0032908 WININIT Cannot find LAYOUT.CTL
0032909 WININIT Cannot read LAYOUT.CTL
0033130 WINPREF undefined preference
0033132 WINPREF undefined preference
0033133 WINPREF undefined preference
0033134 WINPREF undefined preference
0033135 WINPREF undefined preference
0033141 WINASK2 ShowAbout not supported in WIN3_VERSION
0033195 WINASK2 GetOpenFileName failed
0033195 WINASK2 GetOpenFileName failed
0033196 WINASK2 GetSaveFileName failed
0033196 WINASK2 GetSaveFileName failed
0033208 WINQUERY Failed assumption hWnd
0033210 WINQUERY Failed assumption data
0033215 WINLIBM Class not registered
0033215 WINQUERY Class not registered
0033221 DBATTR "tableIndex" out of range (0, queryNumberOfSymAtr)
0033222 DBATTR "tableIndex" out of range (0, queryNumberOfNetAtr)
0033223 DBATTR "tableIndex" out of range (0, queryNumberOfPinAtr)
0033224 DBATTR Bad kind in GetQueryAttrib
0033230 WINQUERY Failed assumption hWnd
0033236 WINQUERY Failed assumption data
0033237 WINQUERY Failed assumption data
0033241 WINQUERY Not enough RAM for query data
0033242 WINQUERY Failed assumption hWnd);           .
0033244 WINQUERY Failed assumption hWnd != NULL
0033245 WINQUERY Failed assumption data
0033245 WINQUERY Failed assumption data
0033246 WINQUERY Failed assumption data
0033247 WINQUERY Expected AltGetDepth () == 0, 0
0033248 WINQUERY Expected DragProc == NULL
0033249 WINQUERY Failed assumption data
0033251 WINMENU Failed assumption !accelTable
0033256 WINQUERY Expected Ui->graphicsMode
0033270 WINQUERY Failed assumption data
0033271 WINQUERY Failed assumption hWnd
0033272 WINQUERY Failed assumption Ui
0033278 WINQUERY Failed assumption index >= 1
0033279 WINQUERY Failed assumption offset >= 0
0033282 WINQUERY Failed assumption tmFontInfo.tmHeight > 0
0033283 WINQUERY Failed assumption data
0033284 WINQUERY Failed assumption tmFontInfo.tmAveCharWidth > 0
0033290 WINQUERY Expected offset == 0
0033291 WINQUERY Expected offset == 0
0033293 WINQUERY Expected offset == 0
0033294 WINQUERY Expected offset == 0
0033295 WINQUERY Expected offset == 0
0033296 WINQUERY Expected offset == 0
0033297 WINQUERY Expected offset == 0
0033335 WINALT Bad conn draw mode in DrawAlt
0033335 WINALT Bad conn draw mode in DrawAlt
0033373 WINALT ALT kind not supported
0033374 WINALT ALT kind not supported
0033375 WINALT ALT kind not supported
0033375 WINALT ALT kind not supported
0033376 WINALT ALT kind not supported
0033380 WINALT Expected AltGetDepth () > 0, 1
0033381 WINALT Expected AltGetDepth () > 0, 1
0033382 WINALT Expected depth > 0, 1
0033383 WINALT Expected AltGetDepth () > 0, 1
0033384 WINALT Expected AltGetDepth () > 0, 1
0033385 WINALT Expected AltGetMax () > 0
0033402 WINSHOW Failed assumption linenum >= 0
0033406 WINSHOW Can't allocate table
0033408 WINSHOW Can't allocate copy of string
0033475 WINEDITP Failed assumption EDIT->printDC
0033510 WINREC Failed assumption !RecorderIsActive
0033632 WINDRAW5 Bad shape in DrawPad
0033640 WINDRAW4 Bad drawing mode in DrawCluster
0033650 WINDRAW5 Bad drawing mode in DrawPad
0033660 WINDRAW4 Failed assumption DATA->hDC
0033661 WINDRAW4 Bad drawing mode in drawClusterLabel
0033671 WINDRAW4 "symIndex" out of range (1, DbView.symMax)
0033674 WINDRAW4 "pinNum" out of range (1, symPtr->header.pads)
0033683 WINDRAW4 Failed assumption DATA->hDC
0033698 WINDRAW4 Failed assumption DATA->hDC
0033701 WINHATCH Failed to allocate hatchHeap
0033702 WINHATCH Failed to allocate polyList in HatchObs
0033705 WINHATCH Failed to increase polyList in HatchObs
0033706 WINHATCH Failed to allocate poly data in HatchObs
0033723 WINHATCH Failed to allocate poly data in HatchObs
0033737 WINHATCH Bad drawing mode in drawHatchLine
0033771 WINHATCH Failed assumption hatchKind
0033782 WINHATCH Failed assumption obsPtr->lev
0033804 WINDEBUG Failed assumption logHeap != 0
0033870 WINDEBUG Failed to allocate new message record
0033954 WINSTAT Failed assumption hDC != 0
0033956 WINSTAT Failed assumption hDC
0033957 WINTOOL Unknown case in UGlobalStatus
0033972 WINSTAT Expected LangTest ((LANG_ID_TYPE) helpId+1, ...
0033972 WINSTAT Failed assumption gaugeMax == 0
0033973 WINSTAT Failed assumption maxVal > 0
0033974 WINSTAT Failed assumption gaugeMax > 0
0033975 WINSTAT Failed assumption gaugeMax > 0
0033976 WINSTAT "val" out of range (0, gaugeMax)
0033979 WINSTAT Expected info
0033982 WINSTAT Can't allocate info text area
0033985 WINSTAT Failed assumption tmFontInfo.tmHeight > 0
0033986 WINSTAT Failed assumption info
0034101 WINLIBM Expected localHeap == 0
0034101 WINBROWS Failed assumption localHeap == 0
0034102 WINLIBM Expected fpListHeap == 0
0034102 WINBROWS Failed assumption fpListHeap == 0
0034109 UIINIT Cannot find LAYOUT.FNT
0034130 WINLIBM Failed assumption lev != 0
0034131 WINFRAME Failed assumption dwp
0034131 WINLIBM Failed assumption lev != 0
0034133 URINIT Security device missing
0034134 WINLIBM Failed assumption DbLoad (fullName, DEFAULT_BINARY)
0034134 WINLIBM Failed assumption DbLoad (fullName, DEFAULT_BINARY)
0034134 WINBROWS Failed assumption DbLoad (fullName, DEFAULT_BINARY)
0034134 WINBROWS Failed assumption DbLoad (fullName, DEFAULT_BINARY)
0034135 URINIT Security device missing
0034136 WINLIBM Failed assumption dlgReturnDb != 0
0034150 WINFRAME Failed assumption dwp
0034151 WINFRAME Failed assumption dwp
0034156 WINLIBM Failed assumption localHeap == 0
0034157 WINLIBM Failed assumption fpListHeap == 0
0034167 WINFRAME Failed assumption RecordStart ("TEST.REC")
0034172 WINFRAME Failed assumption GlobalGetAtomName (atom, string, LSTR_LIMIT)
0034178 WINFRAME Failed assumption strlen (netListFilename) != 0
0034181 WINFRAME failed timer test
0034191 WINFILE Failed assumption DbView.backupInterval >= 0
0034199 WINFILE Security device failed at least one query
0034210 WINGRAPH Failed assumption data
0034215 WINGRAPH Class not registered
0034235 WINGRAPH Failed assumption BusyProc
0034241 WINGRAPH Not enough RAM for edit data
0034242 WINGRAPH Failed assumption graphicsWindow); 
0034294 WINGRAPH Unknown window kind
0034310 WINEDITW Failed assumption EditData
0034315 WINEDITW Class not registered
0034341 WINEDITW Not enough RAM for edit data
0034342 WINEDITW Failed assumption editWindow); 
0034358 WINEDITW Failed assumption EDIT
0034359 WINEDITW Failed assumption Ui
0034382 WINEDITW Failed assumption tmFontInfo.tmHeight > 0
0034384 WINEDITW Failed assumption tmFontInfo.tmAveCharWidth > 0
0034407 WINEDITP Failed assumption rows % rowsPerRecord == 0
0034422 WINEDITP "col" out of range (0, COLUMN_LIMIT)
0034423 WINEDITP header text
0034426 WINEDITP "col" out of range (0, COLUMN_LIMIT)
0034427 WINEDITP "row" out of range (0, EDIT->rowMax-2)
0034427 WINEDITP "row" out of range (0, EDIT->rowMax-2)
0034464 WINEDITP Failed assumption pen
0034465 WINEDITP Failed assumption EDIT->printDC
0034466 WINEDITP Failed assumption pen
0034476 WINEDITP "col" out of range (0, COLUMN_LIMIT)
0034484 WINEDITP "EDIT->headerMax" out of range (1, HEADER_LIMIT)
0034512 WINEDITC Failed assumption row < EDIT->rowMax
0034525 WINEDITC Failed assumption BusyProc
0034572 WINEDITC Failed assumption row < EDIT->rowMax
0034767 WINPREF Expected DbView.pragma != 0
0034768 WINPREF Expected DbView.pragma != 0
0034787 WINPREF No tool bar type
0034822 WINNEW Security device missing
0034824 WINNEW Security device missing
0034851 UIWND Security device missing
0034851 UIWND Security device missing
0034852 UIWND Security device missing
0034852 UIWND Security device missing
0034861 WINNEW Failed assumption info
0034862 WINNEW Failed assumption ui
0034904 WINTOOL "lev" out of range (0, DbView.levMax)
0034910 WINTOOL Expected data->kind == BITMAP_TOOL
0034911 WINTOOL Failed assumption data->hNormal
0034912 WINTOOL Failed assumption BitBlt (hDC, ...
0034913 WINTOOL Failed assumption hDC
0034914 WINTOOL Failed assumption hDC
0034915 WINTOOL Normal bitmap not found
0034916 WINTOOL Grayed bitmap not found
0034916 WINTOOL Unknown tool type
0034917 WINTOOL Highlighted bitmap not found
0034917 WINTOOL Unknown tool type
0034931 WINTOOL Failed assumption hDC
0034932 WINTOOL Failed assumption hSrcDC
0034943 WINTOOL Failed assumption totalTools < MAX_TOOLS
0034945 WINTOOL Failed assumption totalTools < MAX_TOOLS
0034961 WINTOOL Expected rect.left == 0
0034962 WINTOOL Expected rect.top == 0
0034971 WINTOOL Bad menu state
0034981 WINTOOL Can't allocate toolbar table
0034982 WINTOOL Failed assumption tmFontInfo.tmHeight > 0
0034983 WINTOOL "lev" out of range (0, DbView.levMax)
0034984 WINTOOL Failed assumption dragging == FALSE
0034984 WINTOOL Failed assumption tmFontInfo.tmAveCharWidth > 0
0034985 WINTOOL Failed assumption tmFontInfo.tmHeight > 0
0034987 WINTOOL Expected selectedTool >= 0
0034990 WINTOOL "button" out of range (NO_AREA + 1, (totalTools-1))
0035002 WINPEN Heap for caching drawing information
0035010 WINPEN Bad drawing mode in PenMode
0035012 WINPEN Not enough RAM for altLev Table
0035017 WINPEN Failed assumption localHeap
0035032 WINPEN Expected DeleteObject(currentBrush)
0035033 WINPEN Failed assumption (hPalette = CreatePalette (palettePtr))
0035033 WINPEN Expected (hPalette = CreatePalette (palettePtr))
0035034 WINPEN Failed assumption SelectPalette (DATA->hDC, hPalette, NO)
0035035 WINPEN Expected SelectPalette (DATA->hDC, ...
0035036 WINPEN Expected DeleteObject (hPalette)
0035042 WINPEN Failed assumption pen
0035044 WINPEN Expected DeleteObject(currentPen)
0035050 WINPEN unable to assign palette
0035052 WINPEN Failed assumption pen
0035052 WINPEN Failed assumption pen
0035053 WINPEN Expected DeleteObject(currentPen)
0035053 WINPEN Expected DeleteObject(currentPen)
0035060 WINPEN Pen table not initialized
0035063 WINPEN Color class not recognized
0035065 WINPEN Pen table not initialized
0035068 WINPEN Color class not recognized
0035072 WINPEN Bad color in initColors
0035074 WINPEN Expected DeleteObject (oldPalette)
0035074 WINPEN Expected DeleteObject (oldPalette)
0035077 WINPEN Expected SelectObject (DATA->hDC, GetStockObject (HOLLOW_BRUSH))
0035078 WINPEN Expected SelectObject (DATA->hDC, GetStockObject (BLACK_PEN))
0035081 WINPEN Expected DeleteObject(currentPen)
0035082 WINPEN Failed assumption brush
0035082 WINPEN Failed assumption brush
0035083 WINPEN Expected oldBrush == currentBrush
0035083 WINPEN Expected oldBrush == currentBrush
0035084 WINPEN Expected DeleteObject(currentBrush)
0035084 WINPEN Expected DeleteObject(currentBrush)
0035101 WINPAIR Allocate memory for finding duplicates
0035102 WINPAIR Failed to allocate duplicate list
0035121 WINPAIR Failed assumption x1 >= ptr->x1
0035122 WINPAIR Failed assumption x2 <= ptr->x2
0035123 WINPAIR Failed to allocate duplicate list entry
0035167 WINPAIR Expected 1 == 0
0037354 COMMEM "size" out of range (0, 0x7fff)
0037355 COMMEM "size" out of range (0, 0x7fff)
0037370 COMMEM Failed assumption header->magicNumber == MAGIC_NUMBER
0037371 COMMEM Failed assumption header->magicNumber == MAGIC_NUMBER
0037372 COMMEM Failed assumption header->magicNumber == MAGIC_NUMBER
0037373 COMMEM Failed assumption header->magicNumber == MAGIC_NUMBER
0037379 COMMEM "unionSize" out of range (0, unionLimit)
0037380 COMMEM "unionSize" out of range (0, unionLimit)
0037381 COMMEM Failed assumption unionLimit < UNION_DEPTH_LIMIT
0037382 COMMEM Failed assumption unionPtr
0037383 COMMEM Failed Union Alloc
0037384 COMMEM Failed assumption unionPtr
0037385 COMMEM Failed assumption unionLimit < UNION_DEPTH_LIMIT
0037386 COMMEM Failed assumption unionStack[unionSize].ptr == unionPtr
0037387 COMMEM Failed assumption handle
0037390 COMMEM Failed assumption handle
0037391 COMMEM Failed assumption handle
0037394 COMMEM Failed Union Realloc
0037401 COMHEAP Failed assumption ! heapInUse
0037405 COMHEAP "heap" out of range (1, MAX_HEAP_ID)
0037406 COMHEAP Failed assumption ptr
0037407 COMHEAP Failed assumption ptr
0037409 COMHEAP Failed assumption pagePtr[pagesAllocated]
0037413 COMHEAP Failed assumption heapInUse
0037414 COMHEAP Failed assumption heapInUse
0037415 COMHEAP Failed assumption heapInUse
0037416 COMHEAP Failed assumption heapInUse
0037417 COMHEAP Failed assumption heapInUse
0037418 COMHEAP Failed assumption heapInUse
0037425 COMHEAP "heap" out of range (1, MAX_HEAP_ID)
0037426 COMHEAP "heap" out of range (1, MAX_HEAP_ID)
0037430 COMHEAP "size" out of range (1, EXP_SIZE)
0037450 COMHEAP Failed assumption header->magicNumber == MAGIC_NUMBER + heap
0037451 COMHEAP Failed assumption oldHeader->magicNumber == MAGIC_NUMBER + heap
0037452 COMHEAP Failed assumption header->size >= header->next->size
0037455 COMHEAP Failed assumption header->next->prev == header
0037457 COMHEAP Failed assumption header->magicNumber == MAGIC_NUMBER2 + heap
0037458 COMHEAP Failed assumption header->next->prev == header
0037459 COMHEAP Failed assumption header->prev->next == header
0037460 COMHEAP Failed assumption header->prev->next == header
0037462 COMHEAP Failed assumption header->size > list->size
0037463 COMHEAP "heap" out of range (1, MAX_HEAP_ID)
0037464 COMHEAP Failed assumption heapOwner[heap]
0037465 COMHEAP Failed assumption header == &activeList[heap]
0037466 COMHEAP Failed assumption header == &freeList[heap]
0037470 COMHEAP "size" out of range (1, EXP_SIZE - sizeof (MEM_HEADER))
0037471 COMHEAP "size" out of range (1, EXP_SIZE - sizeof (MEM_HEADER))
0037475 COMHEAP "heap" out of range (HEAP_ANON+1, MAX_HEAP_ID)
0037480 COMHEAP Anon
0037481 COMHEAP DB
0037482 COMHEAP ALT
0037483 COMHEAP Failed assumption totalPages == pagesAllocated
0037484 COMHEAP SORT
0037486 COMHEAP SEG
0037487 COMHEAP SEG2
0037488 COMHEAP SEG3
0037490 COMHEAP PLACE
0037812 COMPARSE Not enough RAM for Flags Table
0037814 COMPARSE Failed assumption Flags
0037822 COMPARSE Not enough RAM for LoadFilename
0037824 COMPARSE Failed assumption LoadFilename
0037832 COMPARSE Not enough RAM for SaveFilename
0037834 COMPARSE Failed assumption SaveFilename
0037842 COMPARSE Not enough RAM for StratFilename
0037843 COMPARSE Not enough RAM for SixthFilename
0037843 COMPARSE Not enough RAM for FifthFilename
0037843 COMPARSE Not enough RAM for FourthFilename
0037844 COMPARSE Failed assumption StratFilename
0037846 COMPARSE Failed assumption SixthFilename
0037846 COMPARSE Failed assumption FifthFilename
0037846 COMPARSE Failed assumption FourthFilename
0038011 WINHELP Failed assumption searchString[0] != '\0'
0038401 COMFIND Error accessing Registry root or subkey
0038680 COMLEV Default case in AskLevClass
0038690 COMLEV Default case in LevClassInit
0038711 COMLANG Failed assumption blockBuffer[MAGIC_NUMBER_OFFSET] == MAGIC_NUMBER
0038712 COMLANG "sizeOfDirectory" out of range (1,  BLOCK_LIMIT - DATA_OFFSET)
0038713 COMLANG Failed assumption dirHandle
0038714 COMLANG Failed assumption directory
0038720 COMLANG LangLoad ID not found
0038721 COMLANG LangStr not found
0038725 COMLANG LangCache not found
0038730 COMLANG Language subsystem not yet initialized
0038735 COMLANG Failed assumption blockBuffer[MAGIC_NUMBER_OFFSET] == MAGIC_NUMBER
0039165 ECOMAIN Failed assumption oldDatumX == DbView.datumX
0039166 ECOMAIN Failed assumption oldDatumY == DbView.datumY
0039190 ECOMAIN Data Heap
0039211 ECOREAD Error allocating LevTable
0039212 ECOREAD Error allocating ThruTable
0039213 ECOREAD Error allocating SymTable
0039214 ECOREAD Error allocating PackTable
0039215 ECOREAD Error allocating CompTable
0039216 ECOREAD Error allocating NetTable
0039218 ECOREAD Error allocating ObsTable
0039219 ECOREAD Error allocating TextTable
0039220 ECOREAD Error allocating nameList
0039221 ECOREAD Error allocating SymNameTable
0039230 ECOREAD "nameCount" out of range (1, 3)
0039231 ECOREAD Error allocating lev record
0039232 ECOREAD Error allocating connection record
0039233 ECOREAD Failed assumption connRecordPtr->connId == connIndex
0039234 ECOREAD Failed assumption connIndex == 0
0039235 ECOREAD Failed assumption connRecordPtr->connId == connIndex
0039236 ECOREAD Failed assumption connIndex == 0
0039241 ECOREAD Failed assumption nameCount == 1
0039242 ECOREAD Error allocating thru record
0039243 ECOREAD Error reallocating thru table
0039244 ECOREAD Error reallocating pack table
0039245 ECOREAD Failed assumption nameCount == unionPtr->sym.header.pads + 1
0039245 ECOREAD Failed assumption nameCount == ptr->header.pads + 1
0039246 ECOREAD Error allocating symbol record
0039247 ECOREAD Error reallocating sym table
0039248 ECOREAD Failed assumption nameCount ==  2 * packPtr->header.pads + 1
0039249 ECOREAD Error allocating package record
0039250 ECOREAD Failed assumption nameCount >= 1
0039251 ECOREAD Error allocating comp record
0039252 ECOREAD Error allocating symname record
0039253 ECOREAD Error reallocating comp table
0039254 ECOREAD Error reallocating symName table
0039255 ECOREAD Failed assumption nameCount == 1
0039256 ECOREAD Error allocating net record
0039257 ECOREAD Error reallocating net table
0039260 ECOREAD "nameCount" out of range (0, 1)
0039261 ECOREAD Error allocating connection record
0039267 ECOREAD "packIndex" out of range (1, DbViewB.packMax)
0039270 ECOREAD Failed assumption nameCount == 1
0039271 ECOREAD Error allocating obs record
0039272 ECOREAD Error reallocating obs table
0039273 ECOREAD Failed assumption nameCount == 1
0039274 ECOREAD Error allocating text record
0039275 ECOREAD Error reallocating text table
0039276 ECOREAD Failed assumption nameCount == 1
0039277 ECOREAD Error allocating attribute record
0039278 ECOREAD Failed assumption nameCount == 3
0039279 ECOREAD Error allocating text record
0039283 ECOREAD Error allocating revised pack record
0039285 ECOREAD Failed assumption fakeSym->header.flags & SYM_HOLDER
0039291 ECOREAD Failed assumption nameCount < NAME_COUNT_LIMIT
0039292 ECOREAD Failed assumption nameList
0039305 ECOMAP Failed assumption connRecordPtr->connId == connIndexB
0039311 ECOMAP Error allocating LevAB
0039312 ECOMAP Error allocating LevBA
0039315 ECOMAP Error allocating ThruAB
0039316 ECOMAP Error allocating ThruBA
0039317 ECOMAP Error allocating SymAB
0039318 ECOMAP Error allocating SymBA
0039319 ECOMAP Error allocating PackAB
0039320 ECOMAP Error allocating PackBA
0039333 ECOMAP "connRecordPtr->compId" out of range (1, DbViewB.compMax)
0039334 ECOMAP "connRecordPtr->netId" out of range (1, DbViewB.netMax)
0039335 ECOMAP "connRecordPtr->compId" out of range (1, DbViewB.compMax)
0039336 ECOMAP "connRecordPtr->netId" out of range (1, DbViewB.netMax)
0039351 ECOMAP Error allocating CompAB
0039352 ECOMAP Error allocating compBA
0039353 ECOMAP Error allocating NetAB
0039354 ECOMAP Error allocating NetBA
0039373 ECOMAP Error allocating ObsAB
0039374 ECOMAP Error allocating ObsBA
0039375 ECOMAP Error allocating TxtAB
0039376 ECOMAP Error allocating TxtBA
0039393 ECOMAP Failed assumption connRecordPtr->connId == connIndexB
0039398 ECOMAP "derivedIndex" out of range (1, DbView.symMax)
0039431 ECOMERGE Failed assumption packAIndex <= maxPacks
0039441 ECOMERGE Failed assumption thruAIndex <= maxThrus
0039451 ECOMERGE Failed assumption symAIndex <= maxSyms
0039461 ECOMERGE Failed assumption compAIndex <= maxComps
0039471 ECOMERGE Failed assumption netAIndex <= maxNets
0039472 ECOMERGE Failed assumption obsAIndex <= maxObs
0039473 ECOMERGE Failed assumption txtAIndex <= maxTxt
0039481 ECOMERGE Error reallocating ThruAB
0039482 ECOMERGE Error reallocating SymAB
0039483 ECOMERGE Error reallocating CompAB
0039484 ECOMERGE Error reallocating NetAB
0039485 ECOMERGE Error reallocating PackAB
0039486 ECOMERGE Error reallocating ObsAB
0039487 ECOMERGE Error reallocating TxtAB
0039501 ECOCONN Failed assumption connRecordPtr->pinNameId
0039505 ECOCONN Error allocating netModified
0039510 ECOCONN Failed assumption connRecordPtr->connId == connIndex
0039511 ECOCONN Failed assumption connRecordPtr->connId == connIndex
0039512 ECOCONN Failed assumption connIndex == 0
0039513 ECOCONN Failed assumption connIndex == 0
0039514 ECOCONN Failed assumption connRecordPtr->connId == connBIndex
0039515 ECOCONN Failed assumption connBIndex == 0
0039557 ECOCONN Error allocating netPad entry
0039569 ECOCONN "netId" out of range (1, DbView.netMax)
0039576 ECOCONN Failed assumption connRecordPtr->connPtr == NULL
0039577 ECOCONN Failed assumption compId
0039585 ECOCONN Error allocating netPad
0039621 ECODLG Failed to alloc partNameTable
0039636 ECODLG Failed to alloc lib name buffer
0039644 ECODLG Failed to alloc libNameTable
0039646 ECODLG Failed to alloc savePartName buffer
0039671 ECODLG Failed to alloc successList
0039672 ECODLG Failed to alloc successPair buffer
0041082 PADXMAIN Expected ComPushDir (TCH_LIB)
0041086 PADXMAIN Error allocating CompTyp
0041531 PADXPAR Error reallocating CompTyp
0042567 PADBCONN Failed assumption connView.net == netIndex
0042587 PADBCONN Allocating appHashTable
0043193 MINXMAIN "obsPtr->symId" out of range (1, DbView.symMax)
0043194 MINXMAIN "-obsPtr->symId" out of range (1, DbView.compMax)
0043220 MINXLEX Failed assumption sourceFile
0043265 MINXLEX Failed assumption ptr->next == NULL
0043602 MINWMAIN Failed assumption nameList
0043604 MINWMAIN Failed assumption nameList[index]
0043610 MINWMAIN Failed assumption nameCount < NAME_COUNT_LIMIT
0043627 MINWMAIN Failed assumption nameCount == packPtr->header.pads * 2 + 1
0043645 MINWMAIN Failed assumption compIndex <= COMP_LIMIT
0043646 MINWMAIN Failed assumption netIndex <= NET_LIMIT
0043784 MINBMAIN Failed assumption symId
0043786 MINBMAIN Failed assumption compId >= 0
0043787 MINBMAIN Failed assumption packId >= 0
0043788 MINBMAIN Failed assumption netId >= 0
0043789 MINBMAIN Failed assumption compId >= 0
0043791 MINBMAIN "pinId" out of range (0, symPtr->header.pads)
0043940 MINWMAIN "nameIndex" out of range (0, nameCount)
0043943 MINWMAIN Failed assumption nameCount < NAME_COUNT_LIMIT
0043945 MINWMAIN Failed assumption len <= LSTR_LIMIT + 1
0045010 MPXFILE "rotation" out of range (0, 3)
0045031 MPXPTAB Failed assumption PCADTableHeadHeap > 0
0045031 MPXPTAB Failed assumption PCADTableHeadHeap > 0
0045031 MPXPTAB Failed assumption PCADTableHeadHeap > 0
0045033 MPXPTAB Expected PCADTableHead[type].table[count] == NULL
0045033 MPXPTAB Expected PCADTableHead[type].table[0] == NULL
0045034 MPXPTAB Expected PCADTableHead[type].drillSize[count] == 0
0045034 MPXPTAB Expected PCADTableHead[type].padStackNameId[count] == 0
0045034 MPXPTAB Expected PCADTableHead[type].drillSize[0] == 0
0045034 MPXPTAB Expected PCADTableHead[type].padStackNameId[0] == 0
0045035 MPXPTAB Failed assumption PCADTableHead[type].drillHeap > 0
0045035 MPXPTAB Failed assumption PCADTableHead[type].drillHeap == 0
0045035 MPXPTAB Failed assumption PCADTableHead[type].drillHeap > 0
0045036 MPXPTAB Failed assumption type < PCAD_TABLE_TYPE_LIMIT
0045036 MPXPTAB Failed assumption type < PCAD_TABLE_TYPE_LIMIT
0045036 MPXPTAB Failed assumption type < PCAD_TABLE_TYPE_LIMIT
0045037 MPXPTAB Failed assumption PCADTableHead[type].tableHeap == 0
0045038 MPXPTAB Failed assumption PCADTableHead[type].tableHeap > 0
0045038 MPXPTAB Failed assumption PCADTableHead[type].nameHeap == 0
0045038 MPXPTAB Failed assumption PCADTableHead[type].tableHeap > 0
0045039 MPXPTAB Failed assumption PCADTableHead[type].nameHeap > 0
0045039 MPXPTAB Failed assumption PCADTableHead[type].nameHeap > 0
0045051 MPXPTAB Failed assumption PCADTableHeadHeap > 0
0045159 MPBNET "viaIndex" out of range (2, DbView.viaMax)
0045223 PROTXWIN Bad layer type in updateStrat
0045231 MPXPTAB Failed assumption PCADTableHead[type].table[index] != NULL
0045233 MPXPTAB Failed assumption sizeof (RAW_LEVEL_DATA) < sizeof (UNION_VIEW)
0045234 MPXMAIN Failed assumption dbView.userDiv == 6000
0045234 MPXPTAB Failed assumption prevIndex == index
0045234 MPXPTAB Failed assumption prevIndex == index
0045234 MPXPTAB Failed assumption obsPtr->crns <= OBS_CRN_LIMIT
0045234 MPXPTAB Failed assumption obsPtr->crns <= OBS_CRN_LIMIT
0045234 MPXPTAB Failed assumption prevIndex == index
0045236 MPXPTAB Failed to allocate errTable in DbErrorCreate
0045236 MPXPTAB Failed to allocate errTable in DbErrorCreate
0045236 MPXPTAB Failed assumption PCADTableHead[type].max == 0
0045236 MPXPTAB Failed assumption prevIndex == outIndex
0045239 MPXPTAB Expected layerNameID > 0
0045239 MPXPTAB Expected rawLevelPtr->layerNameID > 0
0045239 MPXPTAB Failed assumption layerNameID > 0
0045239 MPXPTAB Expected ((foundFirst == YES) && (foundSecond == YES))
0045247 MPXPTAB Failed assumption rawLevelPtr->obsPtr != NULL
0045248 MPXPTAB Failed assumption rawLevelPtr->obsPtr->crns <= OBS_CRN_LIMIT
0045282 MPXMAIN Expected ComPushDir (TCH_LIB)
0045282 MPXMAIN Expected ComPushDir (TCH_LIB)
0045297 MPXMAIN Failed assumption ObsLinkTable[thruId] != NULL
0045298 MPXMAIN Failed assumption ObsLinkTable[thruId][lev] != NULL
0045298 MPXPTAB Expected padstackDrill >= 0
0045367 MPXFILE Expected shape == POLYGON_PAD
0045367 MPXFILE Expected shape == POLYGON_PAD
0045607 MPXCOMP Failed assumption PadIndex
0045671 MPXCOMP "symPtr->pad[pinIndex].thru" out of range (dbView.viaMax + 1, dbView.thruMax)
0045672 MPXCOMP "symPtr->pad[tmpCount].thru" out of range (dbView.viaMax + 1, dbView.thruMax)
0045672 MPXSYM "newThruIndex" out of range (dbView.viaMax + 1, dbView.thruMax)
0045823 MPXPTAB Expected lastLayerNameId == layerNameId
0045823 MPXPTAB Expected lastIndex == index
0045823 MPXPTAB Expected lastType == type
0045847 MPXPTAB Failed assumption rawLevelPtr->obsPtr != NULL
0045848 MPXPTAB Failed assumption rawLevelPtr->obsPtr->crns <= OBS_CRN_LIMIT
0045876 MPXPAD Failed assumption returnedVia == via
0045878 MPXPAD Failed assumption returnedVia == 1
0045883 MPXPTAB Failed assumption inIndex >= 0
0045886 MPXPTAB Failed assumption outIndex >= 0
0049025 TANGMAIN Heap Map
0049082 TANGMAIN Expected ComPushDir (TCH_LIB)
0049111 TANGWIN Bad layer class in updateStrat
0050098 DBATTR Unknown case in SchemAttribLimit
0050137 UIAUTO Failed assumption SfView.sweepPrimary == SF_SWEEP_NEXT
0050157 UIAUTO Security device missing
0050158 UIAUTO Security device missing
0050182 UIAUTO Security device missing
0050184 UIAUTO Security device missing
0050190 UIAUTO Failed assumption Ui->windowType == IDS_ROUTE
0050191 UIAUTO Failed assumption Ui->windowType == IDS_ROUTE
0050196 UIAUTO Failed assumption fatMap.usingFatMap == NO
0050197 UIAUTO Failed assumption fatMap.usingFatMap == NO
0050199 UIAUTO Default id in AutoCommand
0050299 UIDEBUG Crash forced by user
0050310 UIGEN Failed assumption dividend > 0
0050331 UCNET netCount Heap
0050334 UCNET Failed to allocate netCountTable
0050530 UIMAIN Expected AltGetDepth() == 0, 0
0050620 UIQUERY "compId" out of range (0, DbView.compMax)
0050621 UIQUERY Expected attr.majorId == symIndex
0050623 UIQUERY ATTR not yet supported
0050625 UIQUERY Failed assumption (unionPtr->obs.symId == symIndex)
0050640 UIQUERY Expected attr.majorId == thruIndex
0050642 UIQUERY ATTR not yet supported
0050645 UIQUERY Failed to allocate pinTreeNode
0050653 UIQUERY "compId" out of range (1, DbView.compMax)
0050655 UIQUERY "-obsPtr->symId" out of range (1, DbView.compMax)
0050657 UIQUERY "obsPtr->net" out of range (1, DbView.netMax)
0050670 UIQUERY "symIndex" out of range (1, DbView.symMax)
0050671 UIQUERY "symPtr->header.pads" out of range (0, SYM_PAD_LIMIT)
0050673 UIQUERY Failed to allocate pinTreeNode
0050684 UIQUERY Failed assumption (textView.symId == symIndex)
0050684 UIQUERY Failed assumption (unionPtr->obs.symId == symIndex)
0050690 UIQUERY "levIndex" out of range (0, DbView.levMax)
0050694 UIQUERY Expected attr.majorId == levIndex
0050695 UIQUERY This Lev ATTR not yet supported
0050893 COMUNITS Bad units
0050906 UIQUERY "unionPtr->pack.header.pads" out of range (0, SYM_PAD_LIMIT)
0050907 UIQUERY "gateGroup[padIndex]" out of range (-1, PACK_GATE_LIMIT)
0050908 UIQUERY "gates" out of range (1, PACK_GATE_LIMIT)
0050931 UIQUERY Expected attr.minorKind == THRU_KIND
0050932 UIQUERY Expected attr.minorKind == LEV_KIND
0050934 UIQUERY "levOffset" out of range (0, DbView.levMax)
0050935 UIQUERY ATTR not yet supported
0050936 UIQUERY Failed assumption unionPtr->conn.net == netIndex
0050937 UIQUERY Failed assumption unionPtr->conn.net == netIndex
0050940 UIQUERY ATTR not yet supported
0050941 UIQUERY Expected attr.majorId == packIndex
0050942 UIQUERY ATTR not yet supported
0050944 UIQUERY Expected attr.minorKind == THRU_KIND
0050948 UIQUERY Expected attr.majorId == symIndex
0050953 UIQUERY "-obsPtr->symId" out of range (1, DbView.compMax)
0050954 UIQUERY "-obsPtr->symId" out of range (1, DbView.compMax)
0050989 UIQUERY "padOffset" out of range (0, symPtr->header.pads)
0050995 UIQUERY Unexpected probe kind
0051023 UAMAIN Bad mode in startDrag
0051030 UAMAIN Expected AltGetDepth () == 1, 1
0051072 UAMAIN Unexpected data type in ALT
0051091 UAMAIN Failed assumption MzGoal->connIndex
0051093 UAMAIN Expected !visible
0051094 UAMAIN Bad drawing mode in UaHighlight
0051232 UAERR Expected AltGetDepth () > 0, 1
0051242 UAERR Expected AltGetDepth () > 0, 0
0051282 UEATTACH Unexpected KIND
0051292 UEATTACH Failed assumption altView.instance <= DbView.compMax
0051313 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051317 UAVERTEX Failed assumption obsPtr->crns == 2
0051318 UAVERTEX Failed assumption altView.offset == 1
0051320 UAVERTEX "crn" out of range (0, obsPtr->crns)
0051324 UAVERTEX Failed assumption obsPtr->crns + 1 < OBS_CRN_LIMIT
0051333 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051338 UAVERTEX Expected obsPtr->kind == OBS_POURED_COPPER
0051339 UAVERTEX Expected AltGetDepth () == 0, 0
0051343 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051345 UAMAIN Expected AltGetDepth () == 1, 1
0051345 UAVERTEX "altPtr->instance" out of range (1, DbView.compMax)
0051346 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051347 UAVERTEX "altView.instance" out of range (1, DbView.compMax)
0051349 UAVERTEX Expected AltGetDepth () == 1, 1
0051355 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051356 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051357 UAVERTEX Expected AltGetDepth() == 2, 2
0051371 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051375 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051376 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051378 UAVERTEX Expected AltGetDepth () == 1, 1
0051382 UAVERTEX "crn" out of range (0, obsPtr->crns-2)
0051384 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051385 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051386 UAVERTEX Failed assumption obsIndex == altView.id
0051387 UAVERTEX Expected AltGetDepth () == 1, 1
0051388 UAVERTEX Failed assumption altView.kind == OBS_KIND
0051389 UAVERTEX Unknown case in obsFinish
0051396 UAVERTEX "work->obs.symId" out of range (-DbView.compMax, -1)
0051439 UAOBS Expected AltGetDepth () == 0, 0
0051442 UAOBS Expected AltGetDepth() > 0, 1
0051451 UAOBS Failed assumption altView.kind == OBS_KIND
0051456 UAOBS Failed assumption altView.kind == OBS_KIND
0051461 UAOBS Expected AltGetDepth() == 1, 1
0051464 UAOBS Cannot determine whether to add or delete
0051467 UAOBS Expected AltGetDepth () == 0, 0
0051468 UAOBS Failed assumption defaultObsIndex >= 0
0051469 UAOBS Failed assumption defaultObstacleIndex >= 0
0051471 UAOBS Expected AltGetDepth() == 1, 1
0051472 UAOBS Expected AltGetMax() >= 1
0051475 UAOBS Expected AltGetDepth() == 1, 1
0051485 UAOBS Failed assumption AltGetFirst (OBS_KIND)
0051732 UATEXT Expected AltGetDepth () > 0, 1
0051742 UATEXT Expected AltGetDepth () > 0, 1
0051765 UATEXT Expected AltGetDepth () == 1, 1
0051767 UATEXT Expected AltGetDepth () == 1, 1
0051786 UATEXT "compId" out of range (0, DbView.compMax)
0051787 UATEXT Expected highlightedComp == 0
0051881 UETEXT Failed assumption altView.kind == TEXT_KIND
0051882 UETEXT Failed assumption altView.kind == TEXT_KIND
0051887 UETEXT Expected AltGetDepth () == 1, 1
0051892 UETEXT Failed assumption compId <= DbView.compMax
0051972 UADATUM Expected AltGetDepth () == 0, 0
0052025 UPMAIN Failed assumption Ui->windowType != IDS_PLACE
0052030 UPMAIN Expected AltGetDepth () == 1, 1
0052033 UPMAIN Expected connId == altView.id
0052045 UPMAIN Failed assumption (! (altView.flags & ALT_ADD))
0052046 UPMAIN Failed assumption altView.id > DbView.connMax
0052062 UPMATRIX Expected AltGetDepth () == 1, 1
0052072 UPMAIN Unexpected data type in ALT
0052098 UPMAIN Expected AltGetDepth () == 1, 1
0052111 UPMOVE unable to allocate ignoreList
0052121 UPMOVE Expected AltGetDepth () == 0, 0
0052122 UPMOVE Failed assumption comp1Index > 0
0052123 UPMOVE Failed assumption comp2Index > 0
0052130 UPMOVE Expected altView.kind == COMP_KIND
0052133 UPMOVE Expected AltGetDepth() == 1, 1
0052134 UPMOVE Expected AltGetDepth () == 0, 0
0052136 UPMOVE "requestedLayer" out of range (1, DbView.levMax)
0052138 UPMOVE Failed assumption Ui->windowType == IDS_ROUTE
0052139 UPMOVE Expected 1 == 0
0052168 UPMOVE Expected AltGetDepth() > 0, 1
0052176 UPMOVE "altView.id" out of range (1, DbView.compMax)
0052176 UPMAIN "altView.id" out of range (1, DbView.compMax)
0052182 UPMOVE Failed assumption selectedComp > 0
0052186 UPMOVE "altView.id" out of range (1, DbView.connMax)
0052186 UPMAIN "altView.id" out of range (1, DbView.connMax)
0052190 UPMOVE Expected AltGetDepth () == 2, 2
0052198 UPMOVE Expected AltGetDepth () == 0, 0
0052233 UPDRAW Failed assumption Ui->modeId == IDM_INVOKE_TOOL_COMPONENT
0052234 UPDRAW Expected AltGetDepth () > 0, 1
0052314 UPEDIT Bad command id in UpSetFlag
0052377 UPEDIT Failed assumption sizeof (altView.instance) >= sizeof (work->comp.rotation)
0052378 UPEDIT Expected altView.instance == 0
0052426 UPINIT Failed assumption Up.netList
0052427 UPINIT Failed assumption Up.selected
0052471 UPINIT Failed assumption selectedHandle
0052473 UPINIT Failed assumption Up.selected
0052510 UPPICK Failed assumption Up.dragCount >= 0
0052512 UPPICK Failed assumption masterId
0052513 UPPICK Failed assumption Up.dragCount >= 0
0052515 UPPICK Expected AltGetDepth () == 1, 1
0052517 UPPICK Expected AltGetDepth () == 1, 1
0052519 UPPICK Failed assumption masterId > 0
0052527 URROUTE Security device missing
0052537 UPPICK Expected AltGetDepth () == 0, 0
0052538 UPPICK Failed assumption (compView.flag & (COMP_FIXED | COMP_LOCKED)) == 0
0052539 UPPICK Expected 1 == 0
0052542 UPPICK Failed assumption masterId
0052546 UPPICK Expected Obj.valid
0052547 UPPICK Expected AltGetDepth () == 0, 0
0052555 UPPICK "compId" out of range (1, DbView.compMax)
0052556 UPPICK "compId" out of range (1, DbView.compMax)
0052557 UPPICK Failed assumption (compView.flag & (COMP_FIXED | COMP_LOCKED)) == 0
0052578 UPPICK "compLev" out of range (1, DbView.levMax)
0052579 UPPICK Failed assumption Obj.valid
0052587 UPPICK "compLev" out of range (1, DbView.levMax)
0052591 UPPICK "nextComp" out of range (0, DbView.compMax)
0052612 UPPIN Expected AltGetDepth () > 0, 1
0052622 UPPIN "pin" out of range (1, symPtr->header.pads)
0052622 UPPIN "pin" out of range (1, symPtr->header.pads)
0052631 UPPIN "selectedPin" out of range (1, lastPin)
0052632 UPPIN "selectedPin" out of range (1, lastPin)
0052633 UPPIN Failed assumption altView.kind == COMP_KIND
0052634 UPPIN Failed assumption selectedPin > 0
0052634 UPPIN "OBS_PIN (work->obs.net)" out of range (1, altSymPtr->header.pads)
0052637 UPPIN Bad DRAW_MODE in calcMode
0052638 UPPIN how could we have gotten here?
0052639 UPPIN Expected 1 == 0
0052642 UPPIN Expected AltGetDepth() > 0, 1
0052643 UPPIN unknown mode in pin/gate proc
0052646 UPPIN Failed assumption altView.kind == COMP_KIND
0052647 UPPIN Failed assumption selectedPin > 0
0052648 UPPIN Unknown drawing mode
0052649 UPPIN "selectedPin" out of range (1, lastPin)
0052651 UPPIN Failed assumption work->obs.symId == symId
0052651 UPPIN Failed assumption work->obs.symId == symId
0052652 UPPIN Failed assumption altView.kind == COMP_KIND
0052653 UPPIN Failed assumption altView.kind == COMP_KIND
0052654 UPPIN Failed assumption selectedPin > 0
0052655 UPPIN Failed assumption work->obs.symId == -compId
0052655 UPPIN Failed assumption work->obs.symId == -compId
0052656 UPPIN Failed assumption selectedPin > 0
0052657 UPPIN Expected AltGetDepth() > 0, 1
0052661 UPPIN Failed assumption symHeader.pads == altSymPtr->header.pads + 1
0052662 UPPIN Failed assumption obsPtr->symId == work->comp.symIndex
0052663 UPPIN Failed assumption obsPtr->symId == -altView.id
0052664 UPPIN Failed assumption Ui->modeId == IDM_INVOKE_TOOL_PIN
0052664 UPPIN Failed assumption Ui->modeId == IDM_INVOKE_TOOL_PIN
0052665 UPPIN Failed assumption Ui->moveMode
0052666 UPPIN Failed assumption work->obs.symId == symId
0052667 UPPIN Failed assumption work->obs.symId == -compId
0052668 UPPIN Failed assumption work->obs.symId == symId
0052669 UPPIN Failed assumption work->obs.symId == -compId
0052672 UPPIN Failed assumption Ui->modeId == IDM_INVOKE_TOOL_PIN
0052674 UPPIN Failed assumption InLibMan
0052678 UPPIN "altView.offset" out of range (1, symPtr->header.pads)
0052679 UPPIN Expected AltGetDepth () > 0, 1
0052682 UPPIN Failed assumption selectedPin > 0
0052683 UPPIN Failed assumption work->obs.symId == symId
0052684 UPPIN Failed assumption work->obs.symId == -compId
0052690 UPPIN "compIndex" out of range (1, DbView.compMax)
0052691 UPPIN Failed assumption work->obs.symId == symId
0052692 UPPIN Failed assumption work->obs.symId == -compId
0052788 UPAUTO Unknown place operation
0052840 UPQUERY "compIndex" out of range (1, DbView.compMax)
0052854 UPQUERY ATTR not yet supported
0052864 UPQUERY Failed assumption unionPtr->conn.comp[0] == compId
0052871 UPQUERY Expected attr.minorKind == PIN_ATTR
0052872 UPQUERY Failed assumption (unionPtr->obs.symId == compView.symIndex) ...
0052874 UPQUERY Failed to allocate compNetTable
0052878 UPQUERY Failed to allocate compNetPtr
0052880 UPQUERY Failed assumption Ui->probe.kind == COMP_KIND
0052881 UPQUERY "compId" out of range (1, DbView.compMax)
0052892 UPQUERY Failed assumption (unionPtr->getText().symId == compView.symIndex) ...
0052896 UPQUERY Failed assumption compId >= 0
0052899 UPQUERY Failed assumption (unionPtr->obs.symId == compView.symIndex) ...
0052951 UPMATRIX Expected AltGetDepth () == 2, 2
0052952 UPMATRIX Expected AltGetMax () == 1
0052953 UPMATRIX Failed assumption obsPtr->crns == 2
0052955 UPMATRIX Failed assumption altView.kind == OBS_KIND
0052956 UPMATRIX Failed assumption altView.kind == OBS_KIND
0052957 UPMATRIX Expected AltGetDepth () == 2, 2
0052958 UPMATRIX Expected AltGetMax () == 1
0052962 UPMATRIX Expected AltGetDepth () == 1, 1
0052966 UPMATRIX Failed assumption altView.kind == OBS_KIND
0052971 UPMATRIX Expected AltGetDepth () == 1, 1
0052972 UPMATRIX Expected AltGetMax () >= 1
0052977 UPMATRIX Failed assumption altView.kind == OBS_KIND
0052978 UPMATRIX Failed assumption altView.kind == OBS_KIND
0052979 UPMATRIX Failed assumption altView.kind == OBS_KIND
0052988 UPMATRIX Failed assumption AltLoadShadow (altIndex, &altView, ptr)
0052989 UPMATRIX Expected AltGetDepth () == 0, 0
0053023 URMAIN Bad mode in startDrag
0053030 URMAIN Expected AltGetDepth () == 1, 1
0053038 URMAIN "MzGoal->connIndex" out of range (1, DbView.connMax)
0053056 URMAIN Security device missing
0053058 URMAIN Security device missing
0053067 URMAIN "MzGoal->connIndex" out of range (1, DbView.connMax)
0053072 URMAIN Unexpected data type in ALT
0053093 URMAIN Unknown tool
0053094 URMAIN Unknown tool
0053193 URAUTO Failed assumption Obj.valid
0053194 URAUTO Failed assumption mz.mapValid
0053199 URAUTO Bad id in RouteAutoCommand
0053250 UIAUTO Failed assumption DbView.batchIndex > 0
0053257 URINIT Security device missing
0053257 URINIT Security device missing
0053258 URINIT Security device missing
0053258 URINIT Security device missing
0053380 URINSTAL Failed assumption mz.mapValid
0053381 URINSTAL Failed assumption mz.hopAllocated
0053383 URINSTAL Path had too many corners
0053391 URINSTAL Bad status in hopFail
0053392 URINSTAL Bad status in hopFail
0053398 URINSTAL Expected AltGetDepth () > 0, 1
0053401 URMOVE Expected MzGoal->objIndex != 0
0053402 URMOVE "MzGoal->connIndex" out of range (1, DbView.connMax)
0053444 URMOVE Failed assumption crn1Index > 0
0053445 URMOVE Failed assumption crn1Index < workPath->crns - 1
0053461 URMOVE "crn1Index" out of range (1, workPath->crns - 2)
0053467 URMOVE "MzGoal->connIndex" out of range (1, DbView.connMax)
0053469 URMOVE "crn1Index" out of range (0, workPath->crns-1)
0053474 URMOVE Failed to allocate lastPath
0053488 URMOVE Failed assumption workPath->crns > 2
0053490 URMOVE Expected AltGetDepth () == 0, 0
0053525 URROUTE Security device missing
0053557 URROUTE Security device missing
0053558 URROUTE Security device missing
0053566 URROUTE Failed assumption MzGoal->connIndex == 0
0053571 URROUTE Failed assumption SfView.options[MzGoal->pass] & PASS_ENABLED_OPTION
0053576 URROUTE Bad sweep direction in nextWindow
0053577 URROUTE Bad sweep direction in nextWindow
0053579 URROUTE Cannot allocate lastPath
0053581 URROUTE Failed assumption nextPass() == PASS
0053584 URROUTE Failed assumption lastPath
0053589 URROUTE Failed assumption MzGoal->connIndex != 0
0053590 URROUTE Failed assumption Ui->windowType == IDS_ROUTE
0053591 URROUTE Failed assumption MzGoal->connIndex
0053592 URROUTE Failed assumption MzGoal->objIndex != 0
0053595 URROUTE Status error in failOneCell2
0053595 URROUTE "MzGoal->connIndex" out of range (1, DbView.connMax)
0053597 URROUTE Failed assumption objRecord.kind == OBJ_CONN
0053598 URROUTE Failed assumption fatMap.usingFatMap == NO
0053607 URGEN Unexpected mode
0053654 URSTEER "newLevel" out of range (0, DbView.levMax)
0053690 URSTEER Bad dir in findOnePath
0053691 URSTEER Failed assumption MzGoal->objIndex
0053692 URSTEER "MzGoal->connIndex" out of range (1, DbView.connMax)
0053745 URTACK Failed to allocate saveConnOrig
0053749 URTACK Failed assumption saveConnOrig
0053767 URTACK Failed assumption connIndex != 0
0053792 URTACK Failed assumption MzGoal->connIndex
0053919 URUNRUT rectEdgeSplit vertical with no crossing
0053920 URUNRUT ConnEdgeSplit horizontal with no crossing
0053991 URUNRUT Failed assumption MzGoal->connIndex
0053992 URUNRUT Failed assumption MzGoal->objIndex
0053995 URUNRUT (crn != crn1) && (crn != crn2) in breakLineAtEdge
0054087 UVCOMP Expected AltGetDepth () == 0, 0
0054088 UVCOMP Failed assumption Ui->rowCount == 0
0054091 UVCOMP UNION stack not empty
0054092 UVCOMP UNION stack not empty
0054093 UVCOMP Expected AltGetDepth () == 0, 0
0054097 UVCOMP Failed assumption Ui->rowCount == 0
0054218 UCNET "firstSelected" out of range (1, DbView.netMax)
0054219 UCNET Failed assumption DbView.netMax < NET_LIMIT
0054287 UVNET Expected AltGetDepth () == 0, 0
0054288 UVNET Failed assumption Ui->rowCount == 0
0054293 UVNET Expected AltGetDepth () == 0, 0
0054297 UVNET Failed assumption Ui->rowCount == 0
0054387 UVOBS Expected AltGetDepth () == 0, 0
0054388 UVOBS Failed assumption Ui->rowCount == 0
0054393 UVOBS Expected AltGetDepth () == 0, 0
0054397 UVOBS Failed assumption Ui->rowCount == 0
0054510 UVSPACE Failed assumption levMap
0054567 UVSPACE Failed assumption result == PASS
0054714 UVSWEEP Failed assumption numSelected
0054814 UVSWLEV Failed assumption numSelected
0054857 UVSWLEV Security device missing
0054858 UVSWLEV Security device missing
0054872 UVSWLEV "levMax" out of range (1, DbView.levMax)
0054923 UVPP Failed assumption hRestore == 0
0054926 UVPP Failed to allocate save image
0054932 UVPP Failed assumption numSelected == 1
0054933 UVPP Expected AltGetDepth () == 0, 0
0054933 UVPP Expected AltGetDepth () == 0, 0
0054934 UVPP Expected lastReport == NO
0054934 UVPP Expected lastReport == NO
0054935 UVPP Expected AltGetDepth () == 0, 0
0054935 UVPP Expected AltGetDepth () == 0, 0
0054936 UVPP Expected AltGetDepth () == 0, 0
0054936 UVPP Expected AltGetDepth () == 0, 0
0054939 UVPP "firstSelected+1" out of range (1, DbView.ppMax)
0054943 UVPP Failed assumption hSaveRestore == 0
0054946 UVPP Failed to allocate save image
0054963 UVPP "firstSelected+1" out of range (1, DbView.ppMax)
0054966 UVPP Failed assumption DbView.colorMax == 0
0054987 UVPP Expected AltGetDepth () == 0, 0
0054988 UVPP Failed assumption Ui->rowCount == 0
0055038 UVTHRU Failed assumption Ui->rowCount == 0
0055039 UVTHRU Expected AltGetDepth () == 0, 0
0055087 UVTHRU Failed assumption probeSet
0055096 UVTHRU Failed assumption Ui->rowCount == 0
0055143 UVCOLOR Failed assumption altView.kind == COLOR_KIND
0055153 UVCOLOR Failed assumption altView.kind == COLOR_KIND
0055163 UVCOLOR bad colorView.kind
0055173 UVCOLOR "colorIndex" out of range (0, UD_COLOR_MAX)
0055174 UVCOLOR Expected AltGetDepth () == 0, 0
0055175 UVCOLOR bad colorView.rule
0055182 UVCOLOR Expected AltGetDepth () == 0, 0
0055317 UVPACK Failed assumption numSelected
0055318 UVPACK "pack" out of range (1, DbView.packMax)
0055396 UVPACK Failed assumption Ui->rowCount == 0
0055421 UVDRC Expected AltGetDepth () == 1, 1
0055456 UVDRC Expected AltGetDepth () == 0, 0
0055787 UVTEXT Expected AltGetDepth () == 0, 0
0055788 UVTEXT Failed assumption Ui->rowCount == 0
0055793 UVTEXT Expected AltGetDepth () == 0, 0
0055797 UVTEXT Failed assumption Ui->rowCount == 0
0055811 UESYM Failed to alloc symbol name record
0055812 UESYM Failed to alloc symbol name data
0055813 UESYM Failed to alloc symbol lib name data
0055821 UESYM Unknown case in variable dialog switch
0055822 UESYM Unknown case in dialog title switch
0055833 UESYM "compId" out of range (1, DbView.compMax)
0055852 UESYM how could we have gotten here?
0055859 UESYM "altView.id" out of range (1, DbView.symMax)
0055861 UESYM Failed assumption sample->sym.pad[sampleOffset].flags ...
0055862 UESYM "altView.id" out of range (1, DbView.symMax+1)
0055872 UESYM "altView.offset" out of range (0, sample->sym.header.pads)
0055883 UESYM Failed assumption pinCount == sample->sym.header.pads
0055891 UESYM Failed assumption altMax
0055898 UESYM Failed assumption lastAltId > 0
0055936 UVSYM Failed assumption Ui->rowCount == 0
0055938 UVSYM Failed assumption Ui->rowCount == 0
0055939 UVSYM Expected AltGetDepth () == 0, 0
0055952 UVSYM "lastPinOffset" out of range (0, tempPtr->header.pads)
0055987 UVSYM Failed assumption probeSet
0055990 UVSYM UNION stack not empty
0055991 UVSYM UNION stack not empty
0055992 UVSYM UNION stack not empty
0056019 UIGRAPH Failed assumption dispatchWindowCommand (msg)
0056026 UIGRAPH Unknown mode
0056061 UPSWAP AltStore Heap
0056064 UPSWAP Failed to allocate pairViewTable
0056065 UPSWAP Failed to allocate pairViewPtr
0056078 UIGRAPH Expected Ui->windowType == IDS_ROUTE
0056110 URSPLIT Expected MzGoal->objIndex != 0
0056120 URSPLIT Expected MzGoal->connIndex != 0
0056126 URSPLIT Failed assumption catFromPtr->crn[0].x == catToPtr->crn[catToPtr->crns-1].x
0056127 URSPLIT Failed assumption catFromPtr->crn[0].y == catToPtr->crn[catToPtr->crns-1].y
0056128 URSPLIT Failed assumption catFromPtr->crn[0].lev == catToPtr->crn[catToPtr->crns-1].lev
0056129 URSPLIT Failed assumption catFromPtr->crn[0].viaIndex == catToPtr->crn[catToPtr->crns-1].viaIndex
0056131 URSPLIT2 Failed assumption inPtr->crn[0].x == outPtr->crn[outPtr->crns-1].x
0056132 URSPLIT2 Failed assumption inPtr->crn[0].y == outPtr->crn[outPtr->crns-1].y
0056133 URSPLIT2 Failed assumption inPtr->crn[0].lev == outPtr->crn[outPtr->crns-1].lev
0056134 URSPLIT2 Failed assumption inPtr->crn[0].viaIndex == outPtr->crn[outPtr->crns-1].viaIndex
0056135 URSPLIT2 Failed assumption inPtr->net == outPtr->net
0056150 URSPLIT Failed assumption catFromPtr->net == catToPtr->net
0056178 URSPLIT Failed assumption connListMax > 0
0056179 URSPLIT Failed assumption connListMax > 0
0056183 URSPLIT Failed assumption foundSections == SEG_OTHER
0056190 URSPLIT Expected AltGetDepth () == 0, 0
0056212 URMANUAL Expected MzGoal->objIndex != 0
0056215 URMANUAL Failed assumption MzGoal->connIndex != 0
0056217 URSELECT Failed assumption MzGoal->connIndex != 0
0056217 URMANUAL Failed assumption MzGoal->crn2Index > MzGoal->crn1Index
0056230 URMANUAL "lastIndex" out of range (1, DbView.connMax)
0056234 URMANUAL tmpPtr->net != connPtr->net
0056234 URMANUAL Expected AltGetDepth () == 0, 0
0056269 URMANUAL Failed assumption altView.id
0056271 URMANUAL Failed to allocate undoListConn
0056272 URMANUAL Failed to allocate undoListIndexes
0056273 URMANUAL Failed to allocate connPtr
0056273 URMANUAL Failed to allocate connPtr
0056274 URMANUAL Failed to allocate lastPath
0056274 URMANUAL Failed assumption undoListIndexes[item] == 0
0056276 URMANUAL Failed to allocate lastPath
0056282 URMANUAL Expected AltGetDepth () == 1, 1
0056298 URMANUAL "connIndex" out of range (1, DbView.connMax)
0056312 URGEN Failed assumption MzGoal->connIndex
0056312 URGEN Failed assumption MzGoal->connIndex
0056317 URGEN "viaIndex" out of range (1, DbView.viaMax)
0056350 URGEN Failed assumption MzGoal->connIndex != 0
0056351 URGEN Failed assumption MzGoal->connIndex != 0
0056370 URGEN Failed assumption MzGoal->connIndex != 0
0056371 URGEN Failed assumption MzGoal->connIndex != 0
0056372 URGEN Failed assumption MzGoal->connIndex != 0
0056373 URGEN Failed assumption MzGoal->connIndex != 0
0056374 URGEN Failed assumption MzGoal->connIndex != 0
0056377 URGEN Failed assumption MzGoal->connIndex != 0
0056379 URGEN Failed assumption MzGoal->connIndex != 0
0056465 UCCONN Failed assumption connPtr->crn[connPtr->crns-1].x ...
0056466 UCCONN Failed assumption connPtr->crn[connPtr->crns-1].y ...
0056483 URQUERY Invalid kind in tellAboutObject
0056573 URCOMP Expected AltGetDepth () == 0, 0
0056789 UPDENSE Failed assumption value >= 0
0056790 UPDENSE Expected AltGetDepth () == 0, 0
0056904 UETHRU Failed assumption foundLegitLayer
0056908 UETHRU "newId" out of range (1, DbView.thruMax)
0056922 UETHRU Unknown case in dialog title switch
0056923 UETHRU "newId" out of range (1, DbView.thruMax + 1)
0056939 UETHRU "altView.id" out of range (1, DbView.thruMax)
0056953 UETHRU Failed assumption match[0] > 0
0056954 UETHRU Failed assumption match[1] == 0
0056961 UETHRU Failed assumption sample->thru.layer[sampleOffset].flags ...
0056962 UETHRU "altView.id" out of range (1, DbView.thruMax+1)
0056972 UETHRU "altView.offset" out of range (0, DbView.levMax)
0056978 UETHRU Failed assumption altView.offset > 0
0056987 UETHRU Unknown case in dialog choice switch
0056991 UETHRU Failed assumption altMax
0057118 UIPROBE Bad kind
0057145 UIPROBE Bad kind in validateprobe.index
0057219 UELEVEL Failed assumption numSelected
0057327 UPSWAP Failed assumption altIndex > 0
0057328 UPSWAP "selectedComp" out of range (1, DbView.compMax)
0057329 UPSWAP Failed assumption selectedPin > 0
0057358 UPSWAP Invalid swap mode
0057359 UPSWAP Invalid swap mode
0057387 UPSWAP Failed assumption newPin2[pin2] == pin
0057403 UESYM Failed assumption localHeap == 0
0057404 UESYM Failed assumption thruNameHead == NULL
0057411 UECOMP Failed assumption altMax
0057411 UEFRVIA Failed assumption altMax
0057422 UECOMP Failed assumption fetchedAction == ACTION_REPLACE_ALL
0057422 UEFRVIA Failed assumption fetchedAction == ACTION_REPLACE_ALL
0057434 UECOMP Failed assumption oldSymId != 0
0057436 UECOMP "compId" out of range (1, DbView.compMax)
0057436 UECOMP "symId" out of range (1, DbView.symMax)
0057457 UECOMP Failed assumption replaceAction != 0
0057463 UECOMP Failed assumption symId != 0
0057464 UECOMP Failed assumption symId != 0
0057471 UECOMP Failed assumption symHeader.derived != 0
0057482 UECOMP Failed assumption newPin > 0
0057483 UECOMP Failed assumption newPin > 0
0057519 UENET Failed assumption altMax
0057574 UENET Failed assumption netIndex
0057574 UENET Failed assumption netIndex
0057587 UENET Failed assumption netIndex
0057587 UENET Failed assumption netIndex
0057588 UENET Failed assumption netIndex
0057589 UENET Failed assumption netIndex
0057615 UEOBS Failed assumption numSelected > 0
0057623 UEOBS "typeVal" out of range (OBS_OUTLINE, OBS_OTHER)
0057623 UEOBS "typeVal" out of range (OBS_OUTLINE, OBS_OTHER)
0057627 UEOBS Bad shape in queryConnSeg
0057681 UEOBS Failed assumption altView.kind == OBS_KIND
0057682 UEOBS Failed assumption altView.kind == OBS_KIND
0057687 UEOBS Expected AltGetDepth () == 1, 1
0057691 UEOBS Failed assumption (obsPtr->kind == OBS_KEEPOUT) ...
0057692 UEOBS Failed assumption compId <= DbView.compMax
0057901 UEUNITS Failed assumption DbView.userDiv == newUserDiv
0057902 UEUNITS Failed assumption DBL_EQUAL (DbView.inchFactor, newInchFactor) == YES
0057903 UEUNITS Failed assumption DbView.userDiv == newUserDiv
0057904 UEUNITS Failed assumption DBL_EQUAL (DbView.inchFactor, newInchFactor) == YES
0057923 UEUNITS Failed assumption (workDb.flags & DB_METRIC) == (DbView.flags & DB_METRIC)
0057926 UEUNITS Failed assumption (workDb.flags & DB_METRIC) == (DbView.flags & DB_METRIC)
0057927 UEUNITS Failed assumption workDb.displayUnits == DbView.displayUnits
0057928 UEUNITS Failed assumption (workDb.flags & DB_METRIC) == (DbView.flags & DB_METRIC)
0057929 UEUNITS Failed assumption (workDb.flags & DB_METRIC) == (DbView.flags & DB_METRIC)
0057931 UEUNITS Failed assumption workDb.displayUnits == DbView.displayUnits
0058482 PROTXMAI Expected ComPushDir (TCH_LIB)
0058801 PROTXNUM Failed assumption theDialog
0059069 UIHILITE Expected pairPtr->count < PAIR_LIMIT
0059071 UIHILITE Expected pairPtr->count < PAIR_LIMIT
0059072 UIHILITE Expected pairPtr->count < PAIR_LIMIT
0059400 UECOMP Failed assumption compId > 0
0059401 UECOMP "newSym" out of range (1, DbView.symMax)
0059403 UECOMP unexpected case in replaceFrViaFootprint
0059411 UECOMP nonsense case in replaceFrViaFootprint
0059411 UECOMP nonsense case in replaceFrViaFootprint
0059413 UECOMP nonsense case in replaceFrViaFootprint
0059414 UECOMP nonsense case in replaceFrViaFootprint
0059415 UECOMP nonsense case in replaceFrViaFootprint
0059416 UECOMP nonsense case in replaceFrViaFootprint
0061003 URCOPY Failed assumption cloneHead == NULL
0061009 URCOPY Failed assumption ptr->connPtr != NULL
0061014 URCOPY Failed assumption ptr->connPtr != NULL
0061016 URCOPY Failed assumption ptr->connPtr != NULL
0061018 URCOPY Failed assumption ptr->connPtr != NULL
0061034 URCOPY Expected (! (connPtr->crn[tmpCrn].flags & CRN_FREE_VIA))
0061042 URCOPY Failed assumption newConn->crn[0].lev == 0
0061045 URCOPY Expected AltGetDepth () == 0, 0
0061064 URCOPY "comp1" out of range (1, DbView.compMax)
0061101 URCHECK Expected mz.qtreeActive == YES
0061102 URCHECK Expected mz.qtreeActive == YES
0061103 URCHECK Expected mz.qtreeActive == YES
0061130 URCHECK Failed assumption DbView.batchIndex > 0
0061141 URCHECK Failed assumption Obj.valid
0061150 URCHECK Failed assumption DbView.batchIndex > 0
0061171 URCHECK Failed assumption Obj.valid
0061172 URCHECK Route not enabled
0061176 URCHECK Bad sweep direction in nextWindow
0061177 URCHECK Bad sweep direction in nextWindow
0061182 URCHECK Route not enabled
0061186 URCHECK Route not enabled
0061189 URCHECK Failed assumption Obj.valid
0061199 URCHECK Bad command
0061245 URCHECK2 Expected AltGetDepth () == 0, 0
0061245 URCHECK2 Expected AltGetDepth () == 0, 0
0061247 URCHECK2 "compId" out of range (1, DbView.compMax)
0061297 URCHECK2 Expected AltGetDepth () == 0, 0
0061300 URDFM Failed assumption 0 == 1
0061306 URDFM Failed assumption filterFunc != NULL
0061307 URDFM "passLimit" out of range (1, 5)
0061308 URDFM "dfmLevel" out of range (DFM_OPTIMIZE_LEVEL, DFM_GLOSS_LEVEL)
0061322 URDFM "c" out of range (0, connPtr->crns - 1)
0061323 URDFM "crn1" out of range (0, connPtr->crns - 1)
0061324 URDFM "crn2" out of range (0, connPtr->crns - 1)
0061333 URDFM "crn" out of range (1, connPtr->crns-2)
0061354 URDFM Failed assumption connPtr->crn[viaCrn].viaIndex != 0
0061380 URFANOUT Heap for dispersion list
0061380 URFANOUT Heap for dispersion list
0061381 URFANOUT Cannot allocate temporary table to compute cluster areas
0061381 URFANOUT Cannot allocate temporary table to compute cluster areas
0061381 URFANOUT Allocating one component score record
0061384 URFANOUT "connPtr->net" out of range (1, DbView.netMax)
0061385 URFANOUT Failed assumption ++maxPad <= SYM_PAD_LIMIT
0061389 URDFM Failed assumption connPtr->crn[crn].viaIndex
0061391 URDFM3 Failed assumption lev
0061394 URDFM Expected AltGetDepth () == 0, 0
0061395 URDFM Expected AltGetDepth () == 0, 0
0061439 URRECON Expected AltGetDepth () == 1, 1
0061456 URRECON Failed assumption crn + diff - addCorners < tmpPtr->crns
0061470 URRECON Failed to allocate newNode
0061472 URRECON Failed to allocate reconnectTable
0061473 URRECON Failed assumption AltGetDepth () >= 1
0061477 URRECON Failed assumption prevPtr != NULL
0061481 URRECON Failed assumption oldPath->crn[0].x == newPath->crn[0].x
0061482 URRECON Failed assumption oldPath->crn[0].y == newPath->crn[0].y
0061490 URRECON Failed assumption reconnectTableMax > 0
0061491 URRECON Failed assumption reconnectTableMax < altMax
0061494 URRECON Failed assumption lastChangedNewCrn - lastChangedOldCrn == addCorners
0061528 URDFM2 Failed assumption ret == 1
0061531 URDFM2 Failed assumption ret != 0
0061543 URDFM2 Failed assumption (ang1 % ROT_90) == 0
0061561 URDFM2 Failed assumption conn2Ptr->comp[0] == conn1Ptr->comp[0]
0061562 URDFM2 Failed assumption conn2Ptr->pin[0] == conn1Ptr->pin[0]
0061563 URDFM2 Failed assumption conn2Ptr->crn[0].x == conn1Ptr->crn[0].x
0061564 URDFM2 Failed assumption conn2Ptr->crn[0].y == conn1Ptr->crn[0].y
0061586 URDFM2 Failed assumption crn > 0
0061601 URDFM3 Failed assumption CheckAlt (NO)
0061634 URDFM3 Failed assumption AltGetMax () != 0
0061635 URDFM3 Failed assumption AltGet (CONN_KIND, connId, connPtr)
0061636 URDFM3 Failed assumption AltGetDepth () != 0
0061637 URDFM3 Failed assumption AltGetDepth () == 0
0061642 URDFM3 "crn" out of range (1, connPtr->crns - 1)
0061678 URDFM3 "symIndex" out of range (1, DbView.symMax)
0061679 URDFM3 "pin" out of range (1, SymPads(symIndex))
0061697 URDFM3 Failed assumption found == TRUE
0061710 URDFM4 Failed assumption DFMCheckAlt (connId, objIndex, connPtr, install)
0061711 URDFM4 Failed assumption DFMCheckAlt (connId, objIndex, connPtr, install)
0061742 URDFM4 Failed assumption result
0061743 URDFM4 Failed assumption result == 1
0061746 URDFM4 Failed assumption result == 1
0061811 URDFM5 Failed assumption connPtr->comp[0] == compId
0061877 URDFM5 Failed assumption width == height
0061878 URDFM5 Failed assumption width == height
0061879 URDFM5 Failed assumption width == height
0061901 URMATCH Failed assumption matchHeap == 0
0061901 URMATCH Failed assumption matchHeap == 0
0061903 URMATCH "netPtr->matchGroup" out of range (1, MATCH_GROUP_LIMIT)
0061903 URMATCH "netPtr->matchGroup" out of range (1, MATCH_GROUP_LIMIT)
0061931 URMATCH Failed assumption delta >= 0
0061933 URMATCH Failed assumption netPtr->minLength >= 0
0061934 URMATCH Failed assumption netPtr->minLength >= 0
0061935 URMATCH Failed assumption netPtr->maxLength >= 0
0061980 URMATCH Failed assumption lengthHeap == 0
0061982 URMATCH Failed to allocate new share record
0062047 UADIM Expected AltGetDepth() == 1, 1
0062049 UADIM Expected AltGetDepth () == 1, 1
0062071 UADIM Expected AltGetDepth() == 1, 1
0062081 UADIM Expected AltGetDepth () == 0, 0
0062082 UADIM Expected AltGetDepth () == 1, 1
0062082 UADIM Expected AltGetDepth () == 1, 1
0062082 UADIM Expected AltGetDepth () == 1, 1
0062083 UADIM Expected AltGetDepth () == 1, 1
0062084 UADIM Expected AltGetDepth () == 1, 1
0062088 UADIM Expected AltGetDepth() == 2, 2
0062089 UADIM Expected AltGetDepth() == 2, 2
0062304 UVCHART "idx" out of range (1, DbView.drillChart.drills)
0063360 UMODNET Failed assumption nodeHeap == 0
0063365 UMODNET Failed assumption nodeCount < MAX_NODES
0063369 UMODNET Failed assumption AltGetDepth () == 0
0063371 UMODNET Failed assumption n1 != 0
0063372 UMODNET Failed assumption n2 != 0
0064012 UCLEVEL Failed assumption symIndex <= DbView.symMax
0064013 UCLEVEL Failed assumption symIndex == DbView.symMax
0064018 UCLEVEL "firstSelected" out of range (1, DbView.levMax)
0064019 UCLEVEL Failed assumption DbView.levMax < LEV_ETCH_LIMIT
0064034 UCLEVEL Expected AltGetDepth () == 1, 1
0064056 UCLEVEL Failed assumption attrPtr->value > 0
0064057 UCLEVEL Failed assumption attrPtr->stringId > 0
0064058 UCLEVEL Expected AltGetDepth () == 1, 1
0064062 UCLEVEL "altView.id" out of range (0, DbView.levMax)
0064116 UCTHRU Failed assumption refLevel != 0
0064117 UCTHRU Failed assumption applyLevel != 0
0064133 UCTHRU "altView.offset" out of range (0, DbView.levMax)
0064134 UCTHRU "altView.offset" out of range (0, DbView.levMax)
0064136 UCTHRU Failed assumption DbView.thruMax < THRU_LIMIT
0064137 UCTHRU Failed assumption Ui->probe.kind == THRU_KIND
0064138 UCTHRU Failed assumption altView.offset > 0
0064141 UCTHRU Failed assumption DbLoad (fullName, DEFAULT_BINARY)
0064142 UCTHRU Failed assumption GetActiveDatabase () == topDb
0064143 UCTHRU Failed assumption localHeap == 0
0064147 UCTHRU Failed assumption altView.offset == 0
0064156 UCTHRU Failed assumption collisionCount == collisionCount2
0064196 UCTHRU "collisionCount2" out of range (1, collisionCount)
0064197 UCTHRU "collisionCount" out of range (1, altMax)
0064211 UCSYM UNION stack not empty
0064212 UCSYM UNION stack not empty
0064222 UCSYM "altView.offset" out of range (0, sample->sym.header.pads)
0064223 UCSYM Failed assumption DbView.symMax < SYM_IDX_LIMIT
0064225 UCSYM Expected Ui->probe.kind == SYM_KIND
0064234 UCSYM "altView.offset" out of range (0, sample->sym.header.pads)
0064235 UCSYM Failed assumption (obsPtr->symId == altView.id)
0064238 UCSYM Failed assumption altView.offset > 0
0064240 UCSYM Failed assumption (obsPtr->symId == obsSymIndex)
0064247 UCSYM Failed assumption altView.offset == 0
0064249 UCSYM Failed assumption (textPtr->symId == textSymIndex)
0064261 UCSYM Failed assumption symHeader.pads == altSymPtr->header.pads + 1
0064262 UCSYM Failed assumption (obsPtr->symId == sampleSymIndex)
0064263 UCSYM Failed assumption pinNumber > 0
0064265 UCSYM Failed assumption (textPtr->symId == altView.id)
0064276 UCSYM Failed assumption work->obs.symId == symId
0064277 UCSYM Failed assumption work->obs.symId == symId
0064278 UCSYM "OBS_PIN (work->obs.net)" out of range (1, altSymPtr->header.pads)
0064405 UCCOMP Expected AltGetDepth () == 1, 1
0064417 UCCOMP "symIndex" out of range (1, DbView.symMax)
0064443 UCCOMP Failed assumption compId
0064446 UCCOMP Failed to allocate tmpHeap
0064456 UCCOMP Expected altDepth > 0
0064462 UCCOMP Failed assumption compId
0064475 UCCOMP Expected AltGetDepth () == 1, 1
0064491 UCCOMP Expected AltGetDepth () == 0, 0
0064492 UCCOMP Failed assumption symPtr->header.derived != 0
0064492 UCCOMP Failed assumption minorId
0064493 UCCOMP Failed assumption mirroredIndex
0064532 UCNET Failed assumption sizeof (UNION_VIEW) > sizeof (BASE_UNIT) * (LEV_ETCH_LIMIT + 1)
0064532 UCNET Failed assumption sizeof (UNION_VIEW) > sizeof (BASE_UNIT) * (LEV_ETCH_LIMIT + 1)
0064543 UCNET "colorIndex" out of range (0, UD_COLOR_MAX)
0064567 UCNET "altView.id" out of range (1, DbView.netMax)
0064574 UCNET Expected netIndex > 0
0064581 UCNET Expected AltGetDepth () == 1, 1
0064606 UCCONN Failed assumption unionPtr->conn.comp[0] != 0
0064607 UCCONN Failed assumption unionPtr->conn.pin[0] != 0
0064608 UCCONN Failed assumption unionPtr->conn.comp[1] == 0
0064609 UCCONN Failed assumption unionPtr->conn.pin[1] == 0
0064621 UCCONN Failed assumption connPtr->comp[0] == 0
0064622 UCCONN Failed assumption connPtr->pin[0] == 0
0064623 UCCONN Failed assumption connPtr2->comp[1] == 0
0064624 UCCONN Failed assumption connPtr2->pin[1] == 0
0064632 UCFRVIA Expected AltGetDepth () == 0, 0
0064633 UCCONN Failed assumption compIndex == Ui->probe.index
0064634 UCCONN Failed assumption compIndex <= DbView.compMax
0064635 UCCONN "pinIndex" out of range (1, symHeader.pads)
0064638 UCCONN Failed assumption nodesFoundNet == 1
0064639 UCCONN Failed assumption nodesFoundNewNet == 1
0064646 UCFRVIA Failed assumption obsPtr->net > 0
0064646 UEFRVIA Failed assumption Ui->compFlags & COMP_FREE_AND_TP
0064647 UEFRVIA Failed assumption Ui->compFlags & COMP_FREE_AND_TP
0064651 UEFRVIA "testIndex" out of range (1, DbView.frViaMax)
0064654 UCCONN Expected AltGetDepth () > 0, 1
0064657 UCFRVIA Expected AltGetDepth () == 0, 0
0064658 UCFRVIA Expected AltGetDepth () == 1, 1
0064662 UCCONN "compIndex" out of range (1, DbView.compMax)
0064663 UCCONN Failed assumption pinIndex > 0
0064667 UCCONN Failed assumption ++loopCheck < LSTR_LIMIT/3
0064672 UCCONN Net discontinuity in original net
0064673 UCCONN Net discontinuity in new net
0064674 UCCONN Failed assumption connPtr->net == netIndex
0064678 UCCONN Failed assumption connPtr2->net == netIndex
0064681 UCCONN connection has two nodes
0064685 UCCONN Failed assumption ourPinConns > 0
0064691 UCCONN Nothing in Alt
0064696 UCCONN Failed assumption unionPtr->conn.net == netIndex
0064718 UCOBS "firstSelected" out of range (1, DbView.obsMax)
0064719 UCOBS Failed assumption DbView.obsMax < OBS_LIMIT
0064818 UCTEXT "firstSelected" out of range (1, DbView.txtMax)
0064819 UCTEXT Failed assumption DbView.txtMax < TEXT_LIMIT
0064879 UCTEXT "-textPtr->symId" out of range (1, DbView.compMax)
0065034 UPSYM Expected AltGetDepth () == 1, 1
0065039 UPSYM Expected 1 == 0
0065076 UPSYM "symIndex" out of range (1, DbView.symMax)
0065077 UPSYM Failed assumption countComps
0065089 UPSYM Expected AltGetDepth () == 1, 1
0065090 UPSYM Expected altMax > 0
0065093 UPSYM Failed assumption !visible
0065094 UPSYM "symIndex" out of range (1, DbView.symMax)
0065098 UPSYM Expected AltGetDepth () == 0, 0
0065344 URINIT Failed assumption MzGoal
0065345 UROBJ Expected Ui->windowType != IDS_ROUTE
0065346 UROBJ "obsPtr->symId" out of range (1, DbView.symMax)
0065346 URINIT Failed assumption GoalPath
0065347 UROBJ Failed assumption compViewPtr->symIndex == obsPtr->symId
0065390 UROBJ Failed assumption obsViewPtr->symId == 0
0071344 NPPMAIN Expected AltGetDepth () == 0, 0
0071346 NPPMAIN Allocating finalReportTable
0071954 NPPINIT "reportIndex" out of range (1, DbView.ppMax)
0072202 NPPDLG "formatKind" out of range (GERBER_FORMAT, EXTENDED_GERBER_FORMAT)
0072232 NPPDLG "rotValue" out of range (ROT0, ROT270)
0072282 NPPDLG2 "typeVal" out of range (HPPEN, HP7580)
0072284 NPPDLG2 "typeVal" out of range (EPSON1, EPSON3)
0072287 NPPDLG2 "typeVal" out of range (PEN1, PEN8)
0075002 NPPGER Failed to allocate draw sort table
0075002 NPPGER Unknown kind
0075003 NPPGER "DbView.appMax" out of range (DeviceParameter.maxApp, DeviceParameter.maxApp)
0075003 NPPGER Allocating AppTable
0075003 NPPGER Unknown shape
0075003 NPPGER Unknown shape
0075003 NPPPLOT Allocating AppTable
0075003 NPPHPGL Allocating AppTable
0075004 NPPGER Failed to allocate sort table
0075005 NPPGER Failed assumption prevPtr
0075007 NPPGER Unknown kind
0075008 NPPGER Allocating StdAppTable
0075009 NPPGER Failed assumption size > 12
0075010 NPPGER "DbView.appMax" out of range (0, 0)
0075020 NPPSIMAP Allocating one DRAW RECORD
0075029 NPPGER Unknown shape
0075032 NPPGER Failed assumption altView.kind == APP_KIND
0075055 NPPGER Unable to complete spooling due to aperture overload
0075063 NPPGER Failed assumption rememberSimulateTable
0075083 NPPGER Unknown shape
0075087 NPPGER Expected AltGetDepth () == 0, 0
0075088 NPPGER Expected AltGetDepth () == 0, 0
0075212 NPPRPT Allocating Drill Tape Table
0075234 NPPRPT Failed assumption customReportXRef[idx] < iniPtr->count
0075402 NPPNET connTable heap
0075404 NPPNET Allocating connTable
0075408 NPPNET local heap
0075409 NPPNET Allocating buffer
0075701 NPPCONN local heap
0075703 NPPCONN Allocating buffer
0075901 NPPNET3 viaCountTable heap
0075904 NPPNET3 viaCountTable heap
0075905 NPPNET3 Allocating viaCountTable
0076089 NPPPART nppPartHeap heap
0076100 NPPDRI1 Can't reallocate drill sort table
0076103 NPPDRI1 drillTable heap
0076103 NPPDRI1 drillTable heap
0076104 NPPDRI1 Allocating DrillTable
0076104 NPPDRI1 Allocating DrillTable
0076234 NPPTSTPT Unable to allocate via duplicate table
0076301 NPPRENAM Failed assumption compIndex2
0076303 NPPRENAM "pin1" out of range (1, maxPin)
0076304 NPPRENAM Failed assumption compIndex2
0076305 NPPRENAM Failed assumption compIndex2
0076306 NPPRENAM Failed assumption compIndex2
0076307 NPPRENAM Failed assumption AttrGet (WAS_ATTR, COMP_KIND, ...
0076308 NPPRENAM Failed assumption ++chainLength <= DbView.compMax
0076309 NPPRENAM Failed assumption AttrGet (WAS_ATTR, COMP_KIND, ...
0076310 NPPRENAM Failed assumption compIndex2
0076311 NPPRENAM Infinite pin rename loop
0076313 NPPRENAM Failed assumption AttrGet (WAS_ATTR, COMP_KIND, ...
0076314 NPPRENAM Failed assumption compIndex2
0076502 NPPDTAPE Can't allocate flashPtr
0076504 NPPDTAPE Can't allocate flashPtr
0076506 NPPDTAPE Can't reallocate drill sort table
0076508 NPPDTAPE Can't reallocate drill sort table
0076510 NPPDTAPE Can't reallocate drill location table
0076512 NPPDTAPE Local heap for drill tape
0076514 NPPDTAPE Allocating DriTable
0076516 NPPDTAPE Can't allocate DriPadsTable
0214101 DBCOMP Failed assumption theDb
0214102 DBCOMP Failed assumption theDb
0214103 DBCOMP Failed assumption theDb
0214104 DBCOMP Failed assumption theDb
0214107 DBCOMP Failed assumption theDb
0214108 DBCOMP Failed assumption theDb
0214111 DBCOMP Failed assumption theDb
0214115 DBCOMP Failed assumption theDb
0214117 DBCOMP Failed assumption theDb
0214121 DBCOMP Failed assumption theDb
0214125 DBCOMP Failed assumption theDb
0214131 DBCOMP Failed assumption theDb
0436026 GEOTHERM Expected --infiniteLoopTest > 0
0436029 GEOTHERM Expected --infiniteLoopTest > 0
0436401 GEOTRACE Failed assumption MzGoal->connIndex != 0
0436726 GEOTHERM Expected --infiniteLoopTest > 0
0436729 GEOTHERM Expected --infiniteLoopTest > 0
0520508 DBDLL Cannot find LAYOUT.CTL
0520509 DBDLL Cannot find LAYOUT.FNT
0520550 DBDLL No expanded memory
0526703 DBPINMAP Failed assumption pinMap->ourUID > 0
0712621 NPPDXF Failed assumption dpb->format == DXF_FORMAT
0712623 NPPDXF Failed assumption DbView.boardXMin >= 0
0712625 NPPDXF Failed assumption DbView.boardYMin >= 0
0713183 NPPAPP3 Failed to allocate object_list HeapSlice in PltApp3
0713192 NPPAPP3 Failed to allocate conTablePtr HeapSlice in PltApp3
0713193 NPPAPP3 Failed to allocate conTablePtr HeapSlice in PltApp3
0752001 NPPRPT Failed assumption iniPtr != NULL
0752008 NPPRPT Drill tape heap
0752016 NPPRPT Allocating buffer
0752020 NPPRPT local heap
0752031 NPPRPT Failed assumption iniPtr == NULL
0752035 NPPRPT Failed assumption ReportHeap == 0
0752035 NPPRPT Failed assumption ReportHeap == 0
0752062 NPPRPT local heap
0752063 NPPRPT Allocating buffer
0759020 NPPNET3 Allocating viaCountTable
2150012 MZSHOVE Failed to allocate retry candidates in mzShovePath
2150291 MZSHOVE Bad status in failRectCells
2152018 MZCLEARS Failed assumption xC == pathPtr->crn[currentCrn+1].x
2152052 MZCLEARS Failed assumption (dx == dy) || (dx == -dy)
2152053 MZCLEARS Failed assumption rowInfo[rowLimit].x == x2
2152072 MZCLEARS Failed to allocate rowInfo in ClearVert
2152080 MZCLEARS Failed assumption rowInfo
2152081 MZCLEARS Failed assumption stitchCrn (xC, yC, 0, 0)
2152082 MZCLEARS "rowLimit" out of range (0, ROW_LIMIT*2)
2152083 MZCLEARS Failed assumption stitchCrn(oldX, yC, oldLev, 0)
2152084 MZCLEARS Failed assumption stitchCrn(xC, yC, oldLev, rowPtr->viaIndex)
2152085 MZCLEARS Failed assumption stitchCrn(xC, oldY, oldLev, 0)
2152086 MZCLEARS Failed assumption stitchCrn(xC, yC, oldLev, rowPtr->viaIndex)
2152087 MZCLEARS Failed assumption stitchCrn (xC, yC, 0, 0)
2152088 MZCLEARS Failed assumption stitchCrn (xC, yC, oldLev, rowPtr->viaIndex)
2152089 MZCLEARS Failed assumption stitchCrn (xC, yC, 0, 0)
2152090 MZCLEARS "rowLimit" out of range (0, ROW_LIMIT)
2152091 MZCLEARS Failed assumption stitchCrn (xC, yC, Shove.level, 0)
2152095 MZCLEARS Status error in failOneCell
2152172 MZCLEARS Failed to allocate rowInfo in ClearVert
2152346 MZCLEARS "(int) currentConn->crn[viaCrn].viaIndex" out of range (1, DbView.viaMax)
2152350 MZCLEARS Failed assumption rowInfo
2152351 MZCLEARS Failed assumption rowInfo
2152450 MZCLEARS Failed assumption shadowRowInfo
2152678 MZPUSH2 "ObjTable[obstacle].id" out of range (1, DbView.connMax)
2152678 MZCLEARS "objRecord.id" out of range (1, DbView.connMax)
2152988 MZCLEARS Failed assumption rowPtr == testPtr
2152989 MZCLEARS Failed assumption pos >= 0
2153002 MZCLEARV Failed assumption limit > 0
2153010 MZCLEARV Failed assumption newCrn < CONN_CRN_LIMIT
2153011 MZCLEARV Failed assumption newCrn < CONN_CRN_LIMIT
2153015 MZCLEARV Failed assumption newCrn + returnPath->crns < CONN_CRN_LIMIT
2153016 MZCLEARV Failed assumption newCrn + returnPath->crns < CONN_CRN_LIMIT
2153019 MZCLEARV "newCrn" out of range (0, CONN_CRN_LIMIT-2)
2153021 MZCLEARV "viaIndex" out of range (1, DbView.viaMax)
2153022 MZCLEARV Failed assumption crn+2 < CONN_CRN_LIMIT
2153023 MZCLEARV "recursionDepthRemaining" out of range (0, VIA_DEPTH_LIMIT*4)
2153091 MZCLEARV Status error in stitchFailure
2154235 MZPUSH Bad delta in PushSeg
2154245 MZPUSH Failed assumption pushGoal->view.crn[crn+1].lev == 0
2154245 MZPUSH Failed assumption pushGoal->view.crn[crn+1].lev == 0
2154246 MZPUSH Failed assumption  ...
2154255 MZPUSH Failed assumption pushGoal->view.crn[crn].x < pushGoal->view.crn[crn+1].x
2154270 MZPUSH Unexpected shovable object type
2154271 MZPUSH Failed assumption IntSegSeg (&pushGoal->xNew, &pushGoal->yBNew, ...
2154272 MZPUSH Failed assumption pushGoal->xOld == pushGoal->xNew
2154273 MZPUSH Failed to allocate stack in PushInit
2154274 MZPUSH Failed assumption pushGoal->yBNew >= pushGoal->view.crn[crn].y
2154275 MZPUSH Failed assumption pushGoal->yBNew <= pushGoal->view.crn[crn+1].y
2154276 MZPUSH Failed assumption pushGoal->yBNew >= pushGoal->view.crn[crn+1].y
2154277 MZPUSH Failed assumption pushGoal->yBNew <= pushGoal->view.crn[crn].y
2155112 MZPUSH2 Failed pushParallel test
2155234 MZPUSH2 Failed assumption pushGoal->view.crn[crn].x == pushGoal->view.crn[crn+1].x
2155235 MZPUSH2 Failed assumption pushGoal->view.crn[crn].y < pushGoal->view.crn[crn+1].y
2155254 MZPUSH2 Failed assumption pushGoal->view.crn[crn].y == pushGoal->view.crn[crn+1].y
2155270 MZPUSH2 Failed assumption thruLevView.shape != THRU_UNDEF_PAD
2155351 MZPUSH2 Failed assumption crn == 0
2155352 MZPUSH2 Failed assumption pushGoal->view.crn[crn].y == pushGoal->view.crn[crn+1].y
2155353 MZPUSH2 Failed assumption crn == 0
2155353 MZPUSH2 Failed assumption pushGoal->view.comp[0] == 0
2155452 MZPUSH2 Failed assumption pushGoal->view.crn[0].x == pushGoal->view.crn[1].x
2155453 MZPUSH2 Failed assumption pushGoal->view.comp[0] == 0
2156255 MZALT Failed assumption ObjTable[altView.objIndex].id == altView.id
2156257 MZALT Failed assumption ObjTable[altView.objIndex].id == altView.id
2156258 MZALT Failed assumption ObjTable[altView.objIndex].id == altView.id
2156259 MZALT Failed assumption ObjTable[altView.objIndex].id == altView.id
2156282 MZALT unexpected kind
2156283 MZALT unexpected kind
2156284 MZALT unexpected kind
2156285 MZALT unexpected kind
2157211 MZCLEARJ Failed assumption crn1 > 0
2157212 MZCLEARJ Failed assumption crn2 < connPtr->crns-1
2157213 MZCLEARJ Failed assumption crn1 > 0
2157214 MZCLEARJ Failed assumption crn2 < connPtr->crns-1
2157218 MZCLEARJ Failed assumption length > 0
2157239 MZCLEARJ Failed assumption crn1x % viaGrid == 0
2157255 MZCLEARJ Status error in failJumper
2157261 MZCLEARJ Failed assumption findJumper (newPath, count, &crn1, &crn2)
2157264 MZCLEARJ Jumper segment with no via
2157268 MZCLEARJ Failed assumption crn2 == crn1+1
2157296 MZCLEARJ Infinite loop detected
2157298 MZCLEARJ Infinite loop detected
2157890 MZCLEARJ Unable to route horizontal or vertical jumpers
2158220 MZPUSHP Failed assumption ShoveReserve
2158223 MZPUSHP "pinNumber" out of range (1, symPtr->header.pads)
2158224 MZPUSHP Failed assumption (ObjTable[objIndex].flags & OBJ_RECURSION) == 0
2158227 MZPUSHP Failed assumption pinPtr[pinNumber].objIndex
2160110 MZCONN "crn" out of range (0, connPtr->crns - 2)
2160120 MZCONN Failed assumption (connPtr->crn[crn + 1].flags & CRN_ARC)
2160180 MZCONN Bad flag during in connReduce
2160180 DBCONN2 Bad flag during in ConnReduce2
2160190 MZCONN ConnEdgeSplit vertical with no crossing
2160200 MZCONN ConnEdgeSplit horizontal with no crossing
2160230 MZCONN "firstCrn" out of range (0, lastCrn)
2160240 MZCONN "lastCrn" out of range (firstCrn, connPtr->crns-1)
2160310 MZCONN extra via in ConnReduce
2160320 MZCONN missing via in ConnReduce
2160330 MZCONN wrong via in ConnReduce
2160340 MZCONN Bad via found in ConnReduce
2160400 MZCONN "myIndex" out of range (Obj.connMin, Obj.connMax)
2160401 MZCONN Failed assumption ObjTable[myIndex].kind == OBJ_CONN
2160760 MZCONN Bad via shape found during viaMap
2161080 MZRANK Heap for mzRank
2161810 MZRANK Failed to allocate sort buffer in RankRoutes
2161910 MZRANK Too many!  Must ignore
2161970 MZRANK Bad sort in heapTest
2162300 MZPICK Expected MzGoal->crn1Index < connPtr->crns - 1
2162310 MZPICK Expected MzGoal->crn2Index == MzGoal->crn1Index + 1
2162345 MZPICK Failed assumption DbView.thruMax > 0
2163020 MZINIT Heap Map
2163030 MZINIT Heap Near
2163040 MZINIT Heap Seed
2163410 MZINIT Failed to allocate ViaCost in mzInit
2163420 MZINIT Failed to allocate LevTable in initLev
2163532 MZINIT Failed assumption !mz.drcEnabled
2163533 MZINIT Failed assumption !Obj.valid
2164020 MZPREP Failed assumption DbView.viaMax <= THRU_VIA_LIMIT
2164022 MZPREP Failed assumption crn >= 0
2164024 MZPREP "MzGoal->crn2Index" out of range (crn+1, connPtr->crns-1)
2164040 MZPREP "value" out of range (0, 100)
2164042 MZPREP Failed assumption nominal >= 0
2164072 MZPREP Failed to allocate rowInfo in ClearVert
2164073 MZPREP PrepRipBack Walked off edge
2164090 MZPREP Failed assumption ObjTable[MzGoal->objIndex].kind == OBJ_CONN
2164092 MZPREP "MzGoal->objIndex" out of range (Obj.connMin, Obj.connMax)
2164093 MZPREP Failed assumption crn1 >= 0
2164094 MZPREP "crn2" out of range (crn1+1, connPtr->crns-1)
2164095 MZPREP "MzGoal->connIndex" out of range (1, DbView.connMax)
2164298 MZPREP Failed assumption actualViaCost
2164299 MZPREP Failed assumption LevTable
2164333 MZPREP Expected mz.usedVias != 0
2164398 MZPREP Failed assumption actualViaCost
2164399 MZPREP Failed assumption LevTable
2165003 MZPATH Failed assumption !MzGoal->hopActive
2165011 MZPATH Kludge
2165015 MZPATH Impossible back track
2165070 MZPATH "pathPtr->crns" out of range (2, CONN_CRN_LIMIT-1)
2165076 MZPATH Couldn't find reconnect reason
2165077 MZPATH Default case in connectToTarget
2165079 MZPATH How did we get here?
2165083 MZPATH Failed assumption lastCrn > 0
2165092 MZPATH "crn" out of range (1, conn2Ptr->crns-1)
2166027 MZCHECK Bad shape in checkConnSeg
2166027 MZCHECK Bad shape in checkConnSeg
2166028 MZCHECK Bad mark in checkConnRect
2166029 MZCHECK Bad mark in checkConnRect
2166030 MZCHECK Bad mark in checkConnSeg
2166030 MZCHECK Bad mark in checkConnSeg
2166031 MZCHECK Bad mark in checkConnSeg
2166031 MZCHECK Bad mark in checkConnSeg
2166036 MZCHECK Failed assumption majorId > 0
2166037 MZCHECK Failed assumption object > 0
2166038 MZCHECK bad shape found during checkConnRect
2166078 MZCHECK Bad shape in checkConnConn
2166079 MZCHECK Bad shape in checkConnConn
2166080 MZCHECK Failed assumption crn1 >= 0
2166082 MZCHECK "crn2" out of range (crn1, connPtr->crns-1)
2166083 MZCHECK bad shape found during checkConnRotPad
2166084 MZCHECK Bad mark in checkConnRotPad
2166087 MZCHECK Bad mark in checkConn
2166088 MZCHECK bad object kind in checkConn
2166091 MZCHECK Bad fill type in checkConnObs
2166092 MZCHECK Bad mark in checkConnRotPad
2166093 MZCHECK Bad mark in checkJumpers
2166094 MZCHECK Failed assumption attr.minorKind == THRU_KIND
2166098 MZCHECK Bad shape in checkConnPad
2166387 MZCHECK Bad mark in checkConn
2167009 MZQTREE Bad object in MzQTreeGatherShovable
2167009 MZGATHER Bad object in GatherShovable
2167011 MZGATHER Failed assumption Obj.valid
2167012 MZGATHER "lev" out of range (0, DbView.levMax)
2167014 MZGATHER Bad entry in GatherAll
2167020 MZGATHER "myIndex" out of range (1, Obj.objMax)
2167021 MZGATHER Bad object in GatherAll
2167022 MZGATHER "lev" out of range (0, DbView.levMax)
2167023 MZGATHER Bad pushDir in GatherShovable
2167029 MZGATHER Failed assumption Obj.valid
2167030 MZGATHER Failed assumption Obj.valid
2167031 MZGATHER "myIndex" out of range (1, Obj.objMax)
2167032 MZQTREE "lev" out of range (0, DbView.levMax)
2167032 MZGATHER "lev" out of range (0, DbView.levMax)
2167033 MZGATHER "lev" out of range (0, DbView.levMax)
2167039 MZGATHER Bad object in GatherIsEmpty
2167080 MZGATHER Failed assumption crn1 >= 0
2167081 MZGATHER Failed assumption Obj.valid
2167082 MZGATHER "crn2" out of range (crn1, connPtr->crns-1)
2167091 MZGATHER Bad obstacle in queryObs
2167801 MZGATHER Failed assumption ObjTable[myIndex].kind == OBJ_CONN
2167802 MZGATHER Failed assumption ObjTable[myIndex].kind == OBJ_CONN
2167803 MZGATHER Failed assumption ObjTable[objIndex].kind == OBJ_PAD
2167805 MZGATHER Failed assumption ObjTable[myObjIndex].kind == OBJ_OBS
2167806 MZGATHER Failed assumption ObjTable[myObjIndex].kind == OBJ_TEXT
2169004 MZMAZE "SfView.routeLayers" out of range (0, LEV_ETCH_LIMIT)
2169010 MZMAZE "x" out of range (0, mz.statusWidth-1)
2169011 MZMAZE "SfView.xMax" out of range (2, ROW_LIMIT)
2169012 MZMAZE "y" out of range (0, mz.statusHeight-1)
2169013 MZMAZE "SfView.yMax" out of range (2, 2*ROW_LIMIT)
2169014 MZMAZE "level" out of range (1, DbView.levMax)
2169015 MZMAZE Failed assumption LevTable[level].enabled
2169016 MZMAZE "x" out of range (0, mz.statusWidth-1)
2169017 MZMAZE "y" out of range (0, mz.statusHeight-1)
2169018 MZMAZE "level" out of range (1, DbView.levMax)
2169019 MZMAZE Failed assumption LevTable[level].enabled
2169020 MZMAZE "x" out of range (0, mz.statusWidth-1)
2169021 MZMAZE "level" out of range (1, DbView.levMax)
2169022 MZMAZE "y" out of range (0, mz.statusHeight-1)
2169023 MZMAZE Failed assumption LevTable[level].enabled
2169024 MZMAZE Failed assumption mz.hopAllocated
2169025 MZMAZE Failed assumption mz.mazeAllocated
2169026 MZMAZE Failed assumption numberOfStatusLayers > 0
2169027 MZMAZE "y" out of range (0, mz.statusHeight-1)
2169028 MZMAZE "level" out of range (1, DbView.levMax)
2169029 MZMAZE Failed assumption LevTable[level].enabled
2169030 MZMAZE "x" out of range (0, mz.statusWidth-1)
2169081 MZMAZE Failed assumption LevTable[lev].enabled
2169082 MZMAZE "level" out of range (1, DbView.levMax)
2169087 MZMAZE Failed assumption mz.mazeAllocated
2169345 MZMAZE resetFull not properly initialized
2169421 MZMAZE "(short) level" out of range (1, DbView.levMax)
2169423 MZMAZE Failed assumption LevTable[level].enabled
2169425 MZMAZE "statusLev" out of range (1, 16)
2169555 MZMAZE "statusLev" out of range (1, 16)
2170012 MZWAVE Failed assumption bitCount == 1
2170014 MZWAVE "size" out of range (4, EXP_SIZE)
2170040 MZWAVE "ptr->x" out of range (2, mz.statusWidth-2)
2170042 MZWAVE "ptr->y" out of range (2, mz.statusHeight-2)
2170046 MZWAVE "ptr->lev" out of range (1, DbView.levMax)
2170048 MZWAVE Failed assumption LevTable[ptr->lev].enabled
2170056 MZWAVE Failed assumption waveValid
2170060 MZWAVE Failed assumption waveSize == 0);  // Give up ...
2170070 MZWAVE Expected waveSize == 0
2170080 MZWAVE Failed assumption mz.mapValid
2170081 MZWAVE Failed assumption waveValid
2170092 MZWAVE Bad cell class in WaveRun
2170093 MZWAVE Bad cell class in FastRun
2171004 MZMAP "lev" out of range (1, DbView.levMax)
2171008 MZMAP "lev" out of range (1, DbView.levMax)
2171020 MZMAP "myIndex" out of range (2, Obj.objMax)
2171022 MZMAP "lev" out of range (0, DbView.levMax)
2171030 MZMAP "myIndex" out of range (2, Obj.objMax)
2171032 MZMAP "lev" out of range (0, DbView.levMax)
2171040 MZMAP "myIndex" out of range (1, Obj.objMax)
2171042 MZMAP "lev" out of range (0, DbView.levMax)
2171081 MZMAP "lev" out of range (0, DbView.levMax)
2171091 MZMAP "lev" out of range (0, DbView.levMax)
2171098 MZMAP "lev" out of range (1, DbView.levMax)
2172093 MZCOMP Failed assumption unionPtr->conn.comp[0] == compId
2172094 MZCOMP Needs comment:  line 496, mzcomp.c
2172464 MZCOMP Failed assumption AltLoadShadow (altIndex, &altView, unionPtr)
2173023 MZOBS Undefined state in findNet
2173026 MZOBS Failed assumption currentObsPtr->crns < OBS_CRN_LIMIT
2173043 MZOBS Failed assumption myHeap == 0
2173076 MZOBS Failed assumption currentObsPtr->lev != 0
2174001 MZSEED Failed to allocate seed record in createSeedRecord
2174005 MZSEED Failed assumption LevTable[level].enabled
2174084 MZSEED "lev" out of range (0, DbView.levMax)
2176052 MZSTRAIT Failed assumption crn2 == crn1+2
2177012 MZNEAR Failed to allocate nearestPath in NearestReset
2177678 MZNEAR Expected nearestPath != NULL
2181021 MZMEM Failed assumption thruView.shape != THRU_UNDEF_PAD); 
2181040 MZMEM Failed assumption ObjTable[middleObj].kind == OBJ_PAD
2181042 MZMEM "middleLev" out of range (1, DbView.levMax)
2185016 MZVIA Failed assumption otherThruView.rotation == 0); 
2185024 MZVIA "viaIndex" out of range (1, DbView.viaMax)
2185041 MZVIA "lev1" out of range (0, DbView.levMax)
2185042 MZVIA "lev2" out of range (0, DbView.levMax)
2185052 MZVIA "viaIndex" out of range (1, DbView.viaMax)
2185062 MZVIA "viaIndex" out of range (1, DbView.viaMax)
2185073 MZVIA Failed assumption lastCrn < connPtr->crns
2185074 MZVIA "firstCrn" out of range (0, lastCrn-1)
2186017 MZOBJ Failed assumption Obj.valid
2186018 MZOBJ Failed assumption ObjTable
2186019 MZOBJ "index" out of range (0, Obj.objMax)
2186021 MZOBJ Failed assumption Obj.valid
2186022 MZOBJ Failed assumption ObjTable
2186023 MZOBJ "index" out of range (0, Obj.objMax)
2186026 MZOBJ Failed assumption Obj.valid
2186027 MZOBJ Failed assumption ObjTable
2186028 MZOBJ Failed assumption ObjTable[objIndex].kind == OBJ_CONN
2186034 MZOBJ Failed assumption ObjTable
2186043 MZOBJ "index" out of range (0, Obj.objMax)
2186044 MZOBJ Bad object in ObjOut
2186049 MZOBJ Bad object in ObjAdd
2186073 MZOBJ Failed to allocate object table in ObjInit
2186092 MZOBJ Failed assumption ObjTable
2186236 MZOBJ Failed assumption Obj.valid
2186237 MZOBJ Failed assumption ObjTable
2186336 MZOBJ Failed assumption Obj.valid
2186337 MZOBJ Failed assumption ObjTable
2186521 MZOBJ Failed assumption Obj.valid
2186522 MZOBJ Failed assumption ObjTable
2186934 MZOBJ Failed assumption ObjTable
2187014 MZPAD "level" out of range (0, DbView.levMax)
2187081 MZPAD Failed assumption connPtr->crns > 3
2187082 MZPAD Failed assumption PT_IN_MAP (xCrn, yCrn)
2187087 MZPAD This crn has a redundant via
2187091 MZPAD Failed assumption viaIndex > 0
2187100 MZPAD Failed assumption dirDist[0] <= dirDist[1]
2187101 MZPAD Failed assumption dirDist[1] <= dirDist[2]
2187102 MZPAD Failed assumption dirDist[2] <= dirDist[3]
2187103 MZPAD Failed assumption testNDist == dirDist[NDist]
2187104 MZPAD Failed assumption numberOfBlocks < 5
2187667 MZPAD Failed assumption (result & (DIR_NE | DIR_NW | DIR_SW | DIR_NE)) == 0
2189012 MZCHECK2 ALT kind not supported
2189012 MZCHECK2 ALT kind not supported
2189023 MZCHECK2 bad mark type found during checkPadExit
2189041 MZCHECK2 Failed assumption comp1
2189043 MZCHECK2 Failed assumption comp2
2189044 MZCHECK2 Failed assumption pin2
2189072 MZCHECK2 Unexpected object type
2189078 MZCHECK2 Bad shape in checkPadToVia
2189080 MZCHECK2 Failed assumption crn1 >= 0
2189082 MZCHECK2 "crn2" out of range (crn1, connPtr->crns-1)
2189091 MZCHECK2 EDGE or ERROR found during MarkError
2189095 MZCHECK2 Bad shape in checkOnePad
2189095 MZCHECK2 Bad shape in checkOnePad
2189223 MZCHECK2 bad mark type found during checkPadLevel
2189306 MZCHECK2 Bad kind in OBJ table
2189307 MZCHECK2 Bad kind in OBJ table
2189543 MZCHECK2 Failed assumption compPtr->flags2 & COMP_FREE_AND_TP
2190620 MZCLEAN Failed assumption conn1.beg <= conn1.end
2190630 MZCLEAN Failed assumption conn2.beg <= conn2.end
2190880 MZCLEAN Default object in processObject
2192310 MZCLEAN2 "(int) conn1->view->crn[crn1].viaIndex" out of range (1, DbView.viaMax)
2192320 MZCLEAN2 "(int) conn2->view->crn[crn2].viaIndex" out of range (1, DbView.viaMax)
2192470 MZCLEAN2 "viaIndex" out of range (1, DbView.viaMax)
2192620 MZCLEAN2 "viaIndex" out of range (1, DbView.viaMax)
2192710 MZCLEAN2 "viaIndex" out of range (1, DbView.viaMax)
2194230 MZCLEANX Default checkFlag in checkEndAngle
2201056 MZCHECKO Failed assumption ! (obs1Ptr->flags & (OBS_CIRCLE | OBS_ARC))
2201057 MZCHECKO Failed assumption ! (obs2Ptr->flags & (OBS_CIRCLE | OBS_ARC))
2201080 MZCHECKO Failed assumption idx != myObsId
2201086 MZCHECKO Failed assumption textId
2201306 MZCHECKO Bad kind in OBJ table
2202010 MZQTREE Tree Heap
2202080 MZQTREE Failed assumption crn1 >= 0
2202082 MZQTREE "crn2" out of range (crn1, connPtr->crns-1)
2202110 MZQTREE Couldn't allocate first quad entry
2202241 MZQTREE "lev" out of range (0, DbView.levMax)
2202330 MZQTREE Failed assumption mz.qtreeActive
2202339 MZQTREE Bad object in MzQTreeGatherIsEmpty
2202423 MZQTREE Bad pushDir in GatherShovable
2202440 MZQTREE Couldn't allocate first quad entry
2202443 MZQTREE Failed assumption 0 == 1
2202444 MZQTREE Failed assumption 0 == 1
2202501 MZQTREE "compId" out of range (1, DbView.compMax)
2202664 MZQTREE Failed assumption treeHeap
2202666 MZQTREE Failed assumption erase == NO
2202668 MZQTREE Failed assumption treeHeap
2202991 MZQTREE Bad obstacle in queryObs
2301456 URFANOUT "newIndex" out of range (1, DB_ERROR_LIMIT)
2301456 MZDISP "newIndex" out of range (1, DB_ERROR_LIMIT)
2301901 MZDISP Failed assumption ObjTable[objIndex].kind == OBJ_PAD
2301903 MZDISP "compIndex" out of range (1, DbView.compMax)
2301904 MZDISP "netIndex" out of range (0, DbView.netMax)
2301905 MZDISP "pinIndex" out of range (1, SymPads (compView.symIndex))
2302323 MZDISPR "ObjTable[objIndex].net" out of range (1, DbView.netMax)
2302405 MZDISPR Failed assumption ++maxPad <= SYM_PAD_LIMIT
2302411 MZDISPR Linked list of pads to disperse
2302710 MZDISPR Cannot allocate temporary sort table
2302808 MZDISPR Heap for scoring components and pins
2302810 MZDISPR Cannot allocate temporary table to compute cluster areas
2302815 MZDISPR Allocating one component score record
2303011 MZFANOUT "info->pinLev" out of range (1, DbView.levMax)
2303011 MZDISPP "info->pinLev" out of range (1, DbView.levMax)
2303234 MZFRVIA Expected AltGetDepth () > 0, 1
2303234 MZFRVIA Expected AltGetDepth () > 0, 1
2303272 MZFANOUT Bad search direction
2303272 MZDISPP Bad search direction
2303292 MZFANOUT Bad search direction
2303292 MZDISPP Bad search direction
2303456 MZFRVIA Allocating locationTable
2303457 MZFRVIA Allocating locationTable
2303458 MZFRVIA Allocating locationTable
2303587 MZFRVIA Failed assumption connPtr->pin[0] == 1
2303587 MZFRVIA Failed assumption connPtr->pin[0] == 1
2303716 MZDISPP Failed assumption PT_IN_MAP (info->xPad, info->yPad)
2303760 MZFANOUT Failed assumption info->shift > 0
2303760 MZDISPP Failed assumption info->shift > 0
3130701 WINDLG "ctl[idx].state" out of range (0, 2)
3130920 WINDLG Help id not defined for dialog
3602120 PLBUBBLE "plView.cooldown" out of range (0, 100)
3602220 PLBUBBLE Failed assumption ClustTablePtr[clustId].compPtr
3602380 PLBUBBLE "compId" out of range (1, DbView.compMax)
3602620 PLBUBBLE Failed assumption NetClustTable
3602730 PLBUBBLE Failed assumption clustRecord.compPtr
3602880 PLBUBBLE "compId" out of range (1, DbView.compMax)
3604070 PLCLUST "compId" out of range (1, DbView.compMax)
3604120 PLCLUST "plView.cooldown" out of range (0, 100)
3604140 PLCLUST "netId" out of range (1, DbView.netMax)
3604240 PLCLUST "compId" out of range (1, DbView.compMax)
3604310 PLCLUST "groupId" out of range (1, DbView.compMax)
3604311 PLCLUST "groupId" out of range (1, GROUP_LIMIT)
3604410 PLCLUST Failed assumption compPtr
3604440 PLCLUST Failed assumption netOut >= 0
3604470 PLCLUST "netId" out of range (1, DbView.netMax)
3604480 PLCLUST Failed assumption netPtr
3604610 PLCLUST Failed assumption NetClustTable
3604620 PLCLUST Failed assumption NetClustTable
3604630 PLCLUST Failed assumption NetClustTable
3604640 PLCLUST Failed assumption ClustTablePtr
3604650 PLCLUST Failed assumption NetClustTable
3604660 PLCLUST Failed assumption netCompTable
3604670 PLCLUST Failed assumption NetClustTable
3604680 PLCLUST Failed assumption NetClustTable
3604690 PLCLUST Failed assumption NetClustTable
3604710 PLCLUST Failed assumption netPtr
3604740 PLCLUST Failed assumption netCompTable
3604760 PLCLUST Failed to allocate group table
3606010 PLANNEAL Selected Heap
3606120 PLANNEAL "plView.cooldown" out of range (0, 100)
3606220 PLANNEAL Failed assumption placeXGrid > 0
3606230 PLANNEAL Failed assumption placeYGrid > 0
3606380 PLANNEAL "comp" out of range (1, DbView.compMax)
3606760 PLANNEAL Failed to allocate component selelection table
3606810 PLANNEAL Failed assumption compSelected
3607010 PLPAIR Failed to allocate net pairs
3607020 PLPAIR Failed to allocate drag pairs
3607030 PLPAIR Failed to allocate selected component table
3607050 PLPAIR Failed to allocate empty pairview
3607350 PLPAIR Failed to allocate net pairs
3607410 PLPAIR Pair Heap
3607420 PLPAIR Pair Selected Heap
3607510 PLPAIR Failed assumption compSelected
3607520 PLPAIR Failed assumption netPairs
3607550 PLPAIR Failed assumption pairPtr->count < PAIR_LIMIT
3607560 PLPAIR Failed to allocate drag pair
3607610 PLPAIR Failed assumption compSelected
3607620 PLPAIR Failed assumption netPairs
3607810 PLPAIR Failed assumption compSelected
3607820 PLPAIR Failed assumption dragPairs
3607830 PLPAIR Failed assumption resultPtr
3607910 PLPAIR Failed assumption compSelected
3608020 PLRECON Failed to allocate new pair
3608040 PLRECON "hashCount" out of range (1, PAIR_LIMIT)
3608120 PLRECON Can not allocate hash element
3608240 PLRECON "link" out of range (0, PAIR_LIMIT)
3608260 PLRECON "link" out of range (0, PAIR_LIMIT)
3608320 PLRECON Can not allocate pair hash
3608420 PLRECON Recon Heap
3608430 PLRECON Recon Edge Heap
3608510 PLRECON Failed to allocate new node
3608520 PLRECON Failed to allocate new pair
3608610 PLRECON Failed to allocate new node
3608710 PLRECON Failed to allocate new pair
3608720 PLRECON Failed to allocate new pair
3608770 PLRECON Failed to allocate new edge
3608820 PLRECON Failed assumption dragPairs
3609010 PLINIT Selected Heap
3609020 PLINIT Component Heap
3609040 PLINIT Cluster Heap
3609060 PLINIT Failed to allocate component table
3609090 PLINIT Failed assumption compHeap
3609140 PLINIT "netId" out of range (1, DbView.netMax)
3609160 PLINIT Failed to allocate net-component bit map
3609170 PLINIT Failed to allocate net-component row
3609210 PLINIT Failed assumption plView.levMax < PLACE_LAYER_LIMIT
3609220 PLINIT Failed assumption placeToXGrid > 0
3609230 PLINIT Failed assumption placeToYGrid > 0
3609240 PLINIT "compId" out of range (1, DbView.compMax)
3609250 PLINIT Failed to allocate cluster table
3609260 PLINIT Failed to allocate net-cluster bit map
3609270 PLINIT Failed to allocate net-cluster row
3609360 PLINIT Failed to allocate net weights table
3609370 PLINIT Failed to allocate net-node table
3609390 PLINIT Failed to allocate table of components to process
3609440 PLINIT Failed to allocate obstacle table
3609470 PLINIT "compLimit" out of range (1, COMP_LIMIT)
3609480 PLINIT "DbView.netMax" out of range (0, NET_LIMIT)
3609610 PLINIT Failed to allocate component net-list record
3609647 PLINIT "compId" out of range (1, DbView.compMax)
3609657 PLINIT "clustId" out of range (1, CLUSTER_LIMIT)
3609660 PLINIT Failed to allocate test table
3609710 PLINIT Failed assumption DbView.placeXGrid > 0
3609720 PLINIT Failed assumption DbView.placeYGrid > 0
3609750 PLINIT Failed to allocate component record
3609760 PLINIT Failed to allocate component selelection table
3609780 PLINIT "compLev" out of range (1, DbView.levMax)
3609830 PLINIT Failed to allocate net node record
3610010 PLNEXT Allocate a heap for scoreTable and netFlagTable
3610050 PLNEXT Failed to allocate net flag table
3610110 PLNEXT compNetHeap
3610150 PLNEXT Failed to allocate comp net table
3610170 PLNEXT Failed to allocate comp net table
3610710 PLNEXT Can't allocate left stack
3610730 PLNEXT Can't allocate right stack
3610760 PLNEXT Failed to allocate component selection table
3612010 PLPACK Spread Heap
3612120 PLPACK Expected AltGetDepth () > 0, 1
3612200 PLPACK Failed assumption (spreadTable[compId].enabled == YES)
3612210 PLPACK Failed assumption (spreadTable[compId].processed == NO)
3612250 PLPACK Bad direction in minimizeCompScore
3612660 PLPACK Failed to allocate component spread table
3612760 PLPACK Failed to allocate component selelection table
3613010 PLPIN Pin Selected Heap
3613060 PLPIN Bad segment in SwapPins
3613100 PLPIN Failed assumption netCount == 1
3613160 PLPIN Bad connection in swapPinsInstall
3613960 PLPIN Failed to allocate component selelection table
3614110 PLSCORE Failed assumption reconnectTable
3614130 PLSCORE Failed assumption reconnectTable
3614140 PLSCORE "netId" out of range (1, DbView.netMax)
3614150 PLSCORE "netId" out of range (1, DbView.netMax)
3614170 PLSCORE "netId" out of range (1, DbView.netMax)
3614370 PLSCORE Failed to allocate reconnect table
3614580 PLSCORE "compId" out of range (1, DbView.compMax)
3614780 PLSCORE "compId" out of range (1, DbView.compMax)
3614870 PLSCORE Failed assumption NetNodePtr
3615580 PLSWAP "compId" out of range (1, DbView.compMax)
3615760 PLSWAP Failed to allocate component selelection table
3616010 PLDENSE Density Map Heap
3616030 PLDENSE Failed to allocate map
3616040 PLDENSE Failed to allocate row of the density map
3616990 PLDENSE Expected result >= 0.0
3617010 PLMATRIX Matrix Heap
3617110 PLMATRIX Cannot allocate comp record
3617170 PLMATRIX Cannot allocate cell record
3618010 PLTREE Tree Heap
3618070 PLTREE Failed PlTreeClose test
3618110 PLTREE Couldn't allocate first quad entry
3618190 PLTREE obstacle missing from map
3618440 PLTREE Couldn't allocate first quad entry
3618710 PLTREE Failed assumption --count >= 0
3618720 PLTREE Failed assumption count == 0
3618810 PLTREE Failed assumption compPtr
3618910 PLTREE Failed assumption foundCount <= 1
3618920 PLTREE Failed assumption classify[obs1Ptr->kind] != CLASS_KEEPIN
3618940 PLTREE Failed assumption classify[obs1Ptr->kind] != CLASS_KEEPIN
3618980 PLTREE Failed assumption classify[obsPtr->kind] != CLASS_KEEPIN
3619010 PLGATE Gate Heap
3619140 PLGATE "netId" out of range (1, DbView.netMax)
3619240 PLGATE "netId" out of range (1, DbView.netMax)
3619340 PLGATE "netId" out of range (1, DbView.netMax)
3619370 PLGATE Failed to allocate reconnect table
3619440 PLGATE "netId" out of range (1, DbView.netMax)
3619790 PLGATE Failed assumption pin1 == newPin2[pin2]
3619820 PLGATE Failed assumption NetNodePtr
3619840 PLGATE Failed assumption reconnectTable
3620010 PLGROUP Group Heap
3620020 PLGROUP Group Heap2
3620070 PLGROUP Failed to allocate group table
3620220 PLGROUP Failed assumption compNumber < COMP_GROUP_LIMIT
3620420 PLGROUP Failed assumption compNumber < COMP_GROUP_LIMIT
3620470 PLGROUP Failed to allocate group record
3620480 PLGROUP "compId" out of range (1, DbView.compMax)
3620640 PLGROUP Failed assumption netCompTable
3620670 PLGROUP Failed to allocate group record
3621010 PLRENAME rename Heap
3621030 PLRENAME Bucket Heap
3621036 PLRENAME Failed assumption BUCKET_TABLE_LIMIT < 32000
3621070 PLRENAME Failed to allocate name table
3621120 PLRENAME Failed to allocate bucket record
3621170 PLRENAME Failed to allocate bucket table
3621170 PLRENAME Failed to allocate duplicate table
3621270 PLRENAME Failed to allocate currentName
3621580 PLRENAME Failed assumption len <= LSTR_LIMIT
3621590 PLRENAME Failed assumption renameLimit > 0
3621590 PLRENAME Failed assumption symPtr->header.derived != 0
3621680 PLRENAME Failed assumption nameTable[nameTableId].selected == YES
3622010 PLCAPS Caps Heap
3622090 PLCAPS Failed assumption CompTablePtr
3622110 PLCAPS Cannot allocate cap record
3622120 PLCAPS Cannot allocate component record
3622190 PLCAPS Failed assumption CompGoalTable
3622220 PLCAPS Failed assumption placeXGrid > 0
3622230 PLCAPS Failed assumption placeYGrid > 0
3622240 PLCAPS Failed assumption bestPtr->numberOfCaps < 2
3624100 PLCLEAR Expected AltGetDepth () > 0, 1
3624120 PLCLEAR Expected AltGetDepth () > 0, 1
3624124 PLCLEAR Failed to alloc obsInnerTable
3624125 PLCLEAR Failed to alloc innerPtr
3624345 PLCLEAR Failed assumption innerPtr != NULL
3624940 PLCLEAR Infinite loop detected
3625030 PLSHOVE Failed assumption (blockedDir & shoveDir) == 0
3625060 PLSHOVE Bad shoveDir
3625124 PLSHOVE Failed to alloc obsInnerTable
3625125 PLSHOVE Failed to alloc innerPtr
3625130 PLSHOVE "recursionLimit" out of range (0, RECURSION_LIMIT)
3625290 PLSHOVE Failed assumption !(compView.flag & COMP_NOT_PLACED)
3625345 PLSHOVE Failed assumption innerPtr != NULL
3626010 PLMEM Selected Heap
3626220 PLMEM Failed assumption placeXGrid > 0
3626230 PLMEM Failed assumption placeYGrid > 0
3626233 PLMEM Failed assumption compPtr->type == COMP_MEMORY
3626241 PLMEM Failed assumption bestRows > 0
3626242 PLMEM Failed assumption bestColumns > 0
3626337 PLMEM Expected compCount > 0
3626338 PLMEM Expected compPtr
3626657 PLMEM "compPtr->clustId" out of range (1, CLUSTER_LIMIT)
3626760 PLMEM Failed to allocate component selelection table
4351550 GEOSEG Failed assumption hatchPtr
4351557 GEOSEG Failed assumption iSect[0].seg == NULL
4351558 GEOSEG Failed assumption iSect[0].x <= iSect[1].x
4351559 GEOSEG Failed assumption iSect[iCount-1].x >= iSect[iCount-2].x
4352010 GEOSPLIT Failed to allocate seglist heap
4352010 GEOOBS Failed to allocate seglist heap
4352020 GEOOBS Failed to allocate seglist heap
4352082 GEOOFFST Failed assumption obsPtr->flags & OBS_SHRINKED
4352111 GEOOBS Failed assumption chainTableSize > 0
4352112 GEOOBS Failed assumption chainPtr
4352143 GEOSPLIT Failed to allocate new seg record
4352143 GEOOBS Failed to allocate new seg record
4352151 GEOSPLIT Failed assumption obsTail->next == NULL
4352151 GEOOBS Failed assumption obsTail->next == NULL
4352183 GEOOBS Failed to allocate new seg record
4352340 GEOSPLIT Failed to allocate new seglist record
4352340 GEOOBS Failed to allocate new seglist record
4352341 GEOSPLIT Failed to allocate new altlist record
4352341 GEOOBS Failed to allocate new altlist record
4352350 GEOSPLIT Failed to allocate new seglist record
4352350 GEOOBS Failed to allocate new seglist record
4352351 GEOSPLIT Failed to allocate new altlist record
4352351 GEOOBS Failed to allocate new altlist record
4352359 GEOOBS Failed to allocate new seglist record
4352371 GEOSPLIT Failed to allocate chainTable in GeoObs
4352371 GEOOBS Failed to allocate chainTable in GeoObs
4353022 GEOOFFST Failed assumption ObsIsClosed (obsPtr)
4353098 GEOOFFST Bad orientation in offsetPoly
4353122 GEOOFFST Failed assumption (obsPtr->kind == OBS_KEEPOUT || obsPtr->kind == OBS_HOLE)
4353220 GEOOFFST "crn" out of range (0, connPtr->crns - 1)
4353222 GEOOFFST Failed assumption obsPtr->kind == OBS_KEEPOUT
4353230 GEOOFFST "crn" out of range (0, obsPtr->crns - 1)
4353310 GEOOFFST "viaIndex" out of range (1, DbView.viaMax)
4353710 GEOOFFST "symIndex" out of range (1, DbView.symMax)
4353720 GEOOFFST "pin" out of range (1, SymPads(symIndex))
4353780 GEOOFFST Bad shape in offsetVia
4353980 GEOOFFST Bad shape in OffsetPad
4354100 GEOPOUR Failed assumption obsPtr->kind == OBS_POURED_COPPER
4354103 GEOPOUR Failed assumption parentHead
4354120 GEOPOUR "obsIndex" out of range (1, DbView.obsMax)
4354123 GEOPOUR "netIndex" out of range (1, DbView.netMax)
4354129 GEOPOUR "compId" out of range (1, DbView.compMax)
4354440 GEOPOUR Failed to allocate new poly record
4354441 GEOPOUR Failed to allocate new seg record
4354444 GEOPOUR Failed to allocate parent poly
4354445 GEOPOUR Failed to allocate new seg record
4354446 GEOPOUR Failed to allocate new share record
4354447 GEOPOUR Failed to allocate new share record
4354525 GEOPOUR Failed assumption connPtr->crn[crn+1].flags & CRN_ARC
4354723 GEOPOUR Undefined state in findNet
4354776 GEOPOUR Failed assumption obsPtr->lev != 0
4354923 GEOPOUR findNet: symIndex == 0 for assigned pin
4355627 GEOCHECK Bad shape in queryConnSeg
4355691 GEOCHECK Failed assumption obsPtr->kind == OBS_KEEPOUT
4357001 GEOFILE Failed assumption parentHead != NULL
4357013 GEOFILE "kind" out of range (PARENT_KIND, HEAT_KIND)
4357063 GEOFILE "rHeader.kind" out of range (PARENT_KIND, HEAT_KIND)
4357101 GEOFILE Failed assumption polyHeap != 0
4357321 GEOFILE Failed assumption currentPoly != NULL
4357340 GEOFILE Failed to allocate new seg record
4357440 GEOFILE Failed to allocate new poly record
4357445 GEOFILE Unknown poly kind
4361010 GEODRD Create a new drill record
4361090 GEODRD Expected DbView.drillChart.drill[idx].symbol
4361092 GEODRD Failed assumption DbView.drillChart.drill[idx].symbol
4361096 GEODRD Failed assumption DbView.drillChart.drill[idx].symbol
4361098 GEODRD Failed assumption found == TRUE
4361100 GEODRD Drill drawing heap
4361102 GEODRD Failed to allocate hash table head pointer
4361104 GEODRD Failed to default text record
4361202 GEODRD "drlIdx" out of range (1, DbView.drillChart.drills)
4361204 GEODRD "symIdx" out of range ('A', LAST_ASC_SYM)
4361305 GEODRD Failed assumption hash >= 0
4361330 GEODRD Failed to allocate via heap
4361331 GEODRD Failed to allocate via hash table
4361337 GEODRD Failed to allocate via table entry
4361356 GEODRD Failed to allocate drill symbol segment
4362001 GEOTHERM Failed to allocate heapThermal
4362010 GEOTHERM Failed to allocate padTable in GeoThermInit
4362029 GEOTHERM Expected --infiniteLoopTest > 0
4362157 GEOTHERM Failed assumption comp > 0
4362158 GEOTHERM Failed assumption pin > 0
4362228 GEOTHERM Failed assumption hash < HASH_LIMIT
4362229 GEOTHERM Failed to allocate padTable entry
4363440 GEOTRACE Failed to allocate new poly record
4363440 GEOSS Failed to allocate new poly record
4363441 GEOTRACE Failed to allocate new seg record
4363441 GEOSS Failed to allocate new seg record
4364031 GEOTRACE Failed assumption connPtr->crn[crn].viaIndex != 0
4364040 GEOTRACE Failed assumption holeHead != NULL
4364041 GEOTRACE Failed assumption holeHead->next == NULL
4364051 GEOTRACE Failed assumption segPtr->x1 == prevPtr->x2
4364052 GEOTRACE Failed assumption segPtr->y1 == prevPtr->y2
4364442 GEOTRACE Failed to allocate new poly record
4364443 GEOTRACE Failed to allocate new seg record
5200016 DBINIT Failed assumption theDb->header.valid
5200020 DBINIT Failed assumption theDb->header.levMax == ptr->levMax
5200021 DBINIT Failed assumption theDb != NULL
5200022 DBINIT Failed assumption theDb->header.viaMax == ptr->viaMax
5200022 DBINIT Failed assumption !theDb->header.valid
5200024 DBINIT Failed assumption ptr->batchIndex <= SF_BATCH_LIMIT
5200024 CONTAIN Failed assumption theDb != NULL
5200025 DBINIT Failed assumption ptr->viaGrid >= 0
5200027 DBINIT Failed assumption ptr->userDiv > 0
5200071 DBINIT Can't allocate DbSort left stack
5200071 DBINIT Can't allocate DbSort left stack
5200073 DBINIT Can't allocate DbSort right stack
5200073 DBINIT Can't allocate DbSort right stack
5200083 DBINIT Can't allocate DbSort auxilliary table
5200083 DBINIT Can't allocate DbSort auxilliary table
5200084 DBINIT Bad displayUnits
5200086 DBINIT Failed assumption oneUnit > 0.0
5200087 DBINIT Failed assumption oneUnit > 0.0
5200088 DBINIT Failed assumption oneUnit > 0.0
5200107 DBINIT Failed assumption theDb
5200110 DBINIT Failed assumption theDb
5200110 DBINIT Failed assumption theDb
5200112 DBINIT Failed assumption theDb
5200112 DBINIT Failed assumption theDb
5200112 DBINIT Failed assumption theDb
5200127 DBINIT Failed assumption theDb
5200303 CONTAIN Failed assumption theDb
5200304 CONTAIN Failed assumption theDb
5200305 CONTAIN Failed assumption theDb
5200505 DBINIT Failed assumption hashHeap == 0
5211005 DBTIMING Failed to allocate event in TimingEvent
5211012 DBTIMING stackDepth is >= STACK_LIMIT in TimingStart
5211015 DBTIMING "idLimit" out of range (0, TIMING_LIMIT-1)
5211020 DBTIMING Attempt to leave module that we didn't enter!
5211024 DBTIMING stackDepth-- < 0
5211025 DBTIMING Failed to allocate name[currentId] in TimingStart
5211080 DBTIMING Failed assumption eTime >= sTime
5211091 DBTIMING stackDepth != 0 in TimingReport
5214010 DBOUT Failed assumption outFile
5214012 DBOUT Failed assumption outFile
5214013 DBOUT Failed assumption outFile
5214014 DBOUT Failed assumption outFile
5214015 DBOUT Failed assumption outFile
5214017 DBOUT Failed assumption outFile
5214020 DBOUT default currentMode in OutStr
5214030 DBOUT default currentMode in OutReset
5220029 DBUNION Bad UNION kind in UnionCreate
5220039 DBUNION Bad UNION kind in UnionDelete
5220049 DBUNION Bad UNION kind in UnionGet
5220059 DBUNION Bad UNION kind in UnionPut
5220079 DBUNION Bad UNION kind in unionOut
5220101 DBUNION Failed assumption kind != NODE_KIND
5220102 DBUNION Failed assumption kind != NODE_KIND
5220103 DBUNION Failed assumption kind != NODE_KIND
5220104 DBUNION Failed assumption kind != NODE_KIND
5220201 DBUNION Failed assumption theDb
5220204 DBUNION Failed assumption theDb
5220445 DBUNION Lev renumbering not supported
5220493 DBUNION Lev renumbering not supported
5220533 DBUNION Bad UNION kind in UnionGet
5220534 DBUNION Bad UNION kind in UnionRangeCheck
5221010 DBNAME Too many names defined or names too long
5221012 DBNAME No RAM for names
5221022 DBNAME No RAM for master name table
5221045 DBNAME Failed assumption len <= LSTR_LIMIT
5221052 DBNAME "heapNumber" out of range (0, theDb->name.maxHeapNumber)
5221054 DBNAME Failed assumption lstrLen (namePtr) <= NAME_STR_LIMIT
5221055 DBNAME Failed assumption hashValue < NAME_HASH_LIMIT
5221056 DBNAME Failed assumption hashValue < NAME_HASH_LIMIT
5221058 DBNAME Failed assumption maxLen <= LSTR_LIMIT
5221078 DBNAME Failed assumption maxLen <= LSTR_LIMIT
5221301 DBNAME Failed assumption theDb
5221302 DBNAME Failed assumption theDb
5221303 DBNAME Failed assumption theDb
5221304 DBNAME Failed assumption theDb
5221305 DBNAME Failed assumption theDb
5221306 DBNAME Failed assumption theDb
5221307 DBNAME Failed assumption theDb
5221308 DBNAME Failed assumption theDb
5221309 DBNAME Failed assumption theDb
5222201 DBUNITS Failed assumption theDb
5223011 DBATTR No RAM for attribute hash table
5223012 DBATTR No RAM for attribute
5223105 DBATTR Failed assumption theDb
5223205 DBATTR Failed assumption theDb
5223212 DBATTR Failed assumption theDb
5223221 DBATTR Failed assumption theDb
5223231 DBATTR Failed assumption theDb
5223236 DBATTR Failed assumption theDb
5223444 DBATTR Failed assumption found == TRUE
5224044 DBPS "ptr->iterations[pass]" out of range (0, 100)
5224046 DBPS "ptr->attempts[pass]" out of range (0, 100)
5224048 DBPS Failed assumption ptr->clustNumber[pass] >= 0
5224201 DBPS Failed assumption theDb
5224204 DBPS Failed assumption theDb
5224205 DBPS Failed assumption theDb
5224206 DBPS Failed assumption theDb
5225012 DBTHRU "theDb->header.viaMax" out of range (1, THRU_VIA_LIMIT)
5225015 DBTHRU "theDb->header.thruMax" out of range (theDb->header.viaMax, THRU_LIMIT)
5225017 DBTHRU Failed to allocate theDb->thru.thruTable in ThruInit
5225018 DBTHRU Failed to assign default names to vias in ThruInit
5225029 DBTHRU Too many Padstacks
5225032 DBTHRU Failed to allocate theDb->thru.thruTable in ThruCreate
5225034 DBTHRU Failed to create padstack id in ThruCreate
5225039 DBTHRU Failed assumption theDb->thru.thruTable[index]
5225040 DBTHRU Failed assumption levPtr->radius[0] == levPtr->radius[1]
5225041 DBTHRU Failed assumption levPtr->radius[0] == levPtr->radius[2]
5225042 DBTHRU Failed assumption levPtr->radius[0] == levPtr->radius[3]
5225044 DBTHRU Failed assumption levPtr->radius[0] >= 0
5225045 DBTHRU Failed assumption levPtr->radius[1] >= 0
5225046 DBTHRU Failed assumption levPtr->radius[2] >= 0
5225047 DBTHRU "deletedIndex" out of range (theDb->header.viaMax+1, theDb->header.thruMax)
5225049 DBTHRU Invalid pad shape - levPtr->shape
5225051 DBTHRU Failed assumption levPtr->radius[3] >= 0
5225054 DBTHRU Failed assumption theDb->thru.thruTable[index]
5225055 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225056 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225057 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225057 DBTHRU Failed assumption theDb->thru.thruTable[index]
5225060 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225070 DBTHRU "level" out of range (0, theDb->header.levMax)
5225072 DBTHRU "level" out of range (1, theDb->header.levMax)
5225076 DBTHRU Failed assumption !(levPtr->flags & ALT_THRULEV_SELECTED)
5225080 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225111 DBTHRU Wrong index in ThruDelete
5225147 DBTHRU Failed assumption theDb->thru.thruTable
5225148 DBTHRU Allocating renumbering table
5225149 DBTHRU Allocating renumbering table
5225201 DBTHRU Failed assumption theDb
5225204 DBTHRU Failed assumption theDb
5225207 DBTHRU Failed assumption theDb
5225211 DBTHRU Failed assumption theDb
5225214 DBTHRU Failed assumption theDb
5225221 DBTHRU Failed assumption theDb
5225227 DBTHRU Failed assumption theDb
5225231 DBTHRU Failed assumption theDb
5225234 DBTHRU Failed assumption theDb
5225237 DBTHRU Failed assumption theDb
5225241 DBTHRU Failed assumption theDb
5225244 DBTHRU Failed assumption theDb
5225247 DBTHRU Failed assumption theDb
5225276 DBTHRU Failed assumption theDb
5225277 DBTHRU "thruId" out of range (1, theDb->header.thruMax)
5225279 DBTHRU Failed assumption theDb
5225301 DBTHRU Failed assumption theDb
5225302 DBTHRU Failed assumption theDb
5225345 DBTHRU Rectangular via
5225346 DBTHRU Illegal via
5225471 DBTHRU "layer" out of range (1, LEV_ETCH_LIMIT)
5225477 DBTHRU Failed assumption theDb
5225479 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225657 DBTHRU Failed assumption theDb->thru.thruTable[index]
5225658 DBTHRU Failed assumption theDb->thru.thruTable[index]
5225670 DBTHRU "level" out of range (0, theDb->header.levMax)
5225680 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225681 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225682 DBTHRU "level" out of range (0, theDb->header.levMax)
5225683 DBTHRU "index" out of range (1, theDb->header.thruMax)
5225684 DBTHRU "level" out of range (0, theDb->header.levMax)
5226010 DBSYM Failed to allocate theDb->sym.symTable in SymInit
5226012 DBSYM Failed to allocate theDb->sym.symTable in SymCreate!!!!
5226015 DBSYM Too many Symbols during SymCreate
5226020 DBSYM "index" out of range (1, theDb->header.symMax)
5226021 DBSYM "index" out of range (1, theDb->header.symMax)
5226022 DBSYM Failed assumption theDb->sym.symTable
5226023 DBSYM "index" out of range (1, theDb->header.symMax)
5226024 DBSYM "index" out of range (1, theDb->header.symMax)
5226026 DBSYM "symIndex" out of range (1, theDb->header.symMax)
5226026 DBSYM "index" out of range (1, theDb->header.symMax)
5226028 DBSYM "index" out of range (1, theDb->header.symMax)
5226033 DBSYM Failed assumption theDb->sym.symTable
5226034 DBSYM Failed assumption theDb->sym.symTable
5226035 DBSYM Failed assumption theDb->sym.symTable
5226035 DBSYM Failed assumption theDb->sym.symTable
5226036 DBSYM Failed assumption theDb->sym.symTable
5226036 DBSYM Failed assumption theDb->sym.symTable
5226036 DBSYM Failed assumption theDb->sym.symTable
5226037 DBSYM Failed assumption theDb->sym.symTable
5226037 DBSYM Failed assumption ptr
5226037 DBSYM Failed assumption ptr
5226038 DBSYM Failed assumption theDb->sym.symTable
5226038 DBSYM "pinIndex" out of range (1, symHeader->pads)
5226038 DBSYM "pin" out of range (1, symHeader->pads)
5226040 DBSYM Failed assumption theDb->sym.symTable
5226040 DBSYM "index" out of range (1, theDb->header.symMax)
5226040 DBSYM "index" out of range (1, theDb->header.symMax)
5226046 DBSYM "deletedIndex" out of range (1, theDb->header.symMax)
5226047 DBSYM Failed assumption theDb->sym.symTable
5226048 DBSYM Allocating renumbering table
5226049 DBSYM Allocating renumbering table
5226051 DBSYM "headerPtr->pads" out of range (0, SYM_PAD_LIMIT)
5226052 DBSYM "viewPtr->header.pads" out of range (0, SYM_PAD_LIMIT)
5226053 DBSYM "viewPtr->pad[pin].thru" out of range (1, theDb->header.thruMax)
5226054 DBSYM Failed assumption viewPtr->pad[pin].nameId != 0
5226055 DBSYM Failed assumption !(viewPtr->pad[pin].flags & ALT_SYMPAD_SELECTED)
5226064 DBSYM Failed assumption theDb->sym.symTable
5226065 DBSYM Failed assumption headerPtr->pads == theDb->sym.symTable[index].header.pads
5226070 DBSYM "symIndex" out of range (1, theDb->header.symMax)
5226071 DBSYM "symIndex" out of range (1, theDb->header.symMax)
5226072 DBSYM "symIndex" out of range (1, theDb->header.symMax)
5226074 DBSYM "pinNum" out of range (1, theDb->sym.symTable[symIndex].header.pads)
5226074 DBSYM "pinNum" out of range (1, theDb->sym.symTable[symIndex].header.pads)
5226075 DBSYM "pinNum" out of range (1, theDb->sym.symTable[symIndex].header.pads)
5226083 DBSYM Couldn't allocate new memory for pins
5226091 DBSYM "symIndex" out of range (1, theDb->header.symMax)
5226093 DBSYM Failed assumption theDb->sym.symTable
5226094 DBSYM "pinNum" out of range (1, theDb->sym.symTable[symIndex].header.pads)
5226095 DBSYM "ptr->thru" out of range (1, theDb->header.thruMax)
5226171 DBSYM "symIndex" out of range (1, theDb->header.symMax)
5226201 DBSYM Failed assumption theDb
5226204 DBSYM Failed assumption theDb
5226211 DBSYM Failed assumption theDb
5226221 DBSYM Failed assumption theDb
5226224 DBSYM Failed assumption theDb
5226224 DBSYM Failed assumption theDb
5226231 DBSYM Failed assumption theDb
5226241 DBSYM Failed assumption theDb
5226241 DBSYM Failed assumption theDb
5226241 DBSYM Failed assumption theDb
5226244 DBSYM Failed assumption theDb
5226251 DBSYM Failed assumption theDb
5226254 DBSYM Failed assumption theDb
5226257 DBSYM Failed assumption theDb
5226261 DBSYM Failed assumption theDb
5226271 DBSYM Failed assumption theDb
5226274 DBSYM Failed assumption theDb
5226277 DBSYM Failed assumption theDb
5226281 DBSYM Failed assumption theDb
5226301 DBSYM Failed assumption theDb
5226386 DBSYM "symIndex" out of range (1, theDb->header.symMax)
5226387 DBSYM Failed assumption theDb->sym.symTable
5226554 DBSYM Failed assumption theDb
5226571 DBSYM "symIndex" out of range (1, theDb->header.symMax)
5226574 DBSYM "pinNum" out of range (1, symPtr->header.pads)
5226724 DBSYM "index" out of range (1, theDb->header.symMax)
5226736 DBSYM Failed assumption theDb->sym.symTable
5226751 DBSYM Failed assumption theDb
5227010 DBPACK Failed to allocate theDb->pack.packTable in PackInit
5227012 DBPACK Failed to allocate theDb->pack.packTable in PackCreate
5227015 DBPACK Too many Packages during PackCreate
5227020 DBPACK "index" out of range (1, theDb->header.packMax)
5227021 DBPACK "index" out of range (1, theDb->header.packMax)
5227022 DBPACK Failed assumption theDb->pack.packTable
5227023 DBPACK "index" out of range (1, theDb->header.packMax)
5227024 DBPACK "index" out of range (1, theDb->header.packMax)
5227028 DBPACK "packId" out of range (1, theDb->header.packMax)
5227036 DBPACK Failed assumption theDb->pack.packTable
5227047 DBPACK "id" out of range (1, theDb->header.packMax)
5227048 DBPACK PackDelete is not implemented
5227052 DBPACK "viewPtr->header.pads" out of range (0, SYM_PAD_LIMIT)
5227054 DBPACK Failed assumption viewPtr->pad[pin].padId != 0
5227055 DBPACK Failed assumption !(viewPtr->pad[pin].flags & ALT_PACKPAD_SELECTED)
5227058 DBPACK Can't allocate storage for package's pins
5227061 DBPACK "index" out of range (1, theDb->header.packMax)
5227062 DBPACK "headerPtr->pads" out of range (0, SYM_PAD_LIMIT)
5227063 DBPACK Failed assumption headerPtr->pads == theDb->pack.packTable[index].header.pads
5227071 DBPACK "packId" out of range (1, theDb->header.packMax)
5227072 DBPACK "pinNum" out of range (1, theDb->pack.packTable[packId].header.pads)
5227081 DBPACK Comment needed: dbpack.c, line 509
5227082 DBPACK Comment needed: dbpack.c, line 511
5227088 DBPACK "compId" out of range (1, theDb->header.compMax)
5227201 DBPACK Failed assumption theDb
5227204 DBPACK Failed assumption theDb
5227211 DBPACK Failed assumption theDb
5227221 DBPACK Failed assumption theDb
5227224 DBPACK Failed assumption theDb
5227227 DBPACK Failed assumption theDb
5227231 DBPACK Failed assumption theDb
5227241 DBPACK Failed assumption theDb
5227244 DBPACK Failed assumption theDb
5227251 DBPACK Failed assumption theDb
5227254 DBPACK Failed assumption theDb
5227257 DBPACK Failed assumption theDb
5227261 DBPACK Failed assumption theDb
5227331 DBPACK Failed assumption theDb
5228017 DBMIRROR "id" out of range (1, theDb->header.symMax)
5228052 DBMIRROR "id" out of range (1, theDb->header.thruMax)
5228058 DBMIRROR "thruId" out of range (1, theDb->header.thruMax)
5230028 DBCOMP2 "compPtr->symIndex" out of range (1, theDb->header.symMax)
5230030 DBCOMP2 Cannot allocate a heap
5230031 DBCOMP2 Cannot allocate temporary table to compute cluster areas
5230032 DBCOMP2 Cannot allocate oldRadius table
5230033 DBCOMP2 Cannot allocate temporary table to save best master comp
5230034 DBCOMP2 Cannot allocate temporary table to save best scores
5230040 DBCOMP2 "compId" out of range (0, theDb->header.compMax)
5230042 DBCOMP2 "lev" out of range (0, theDb->header.levMax)
5230044 DBCOMP2 Cannot allocate a heap
5230045 DBCOMP2 Cannot allocate temporary table
5230054 DBCOMP2 "compIndex" out of range (1, theDb->header.compMax)
5230057 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230058 DBCOMP2 "compView.cluster" out of range (0, theDb->header.compMax)
5230073 DBCOMP2 Failed assumption newArea[compId] > 0.0
5230077 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230078 DBFRVIA "connId" out of range (1, theDb->header.connMax + 1)
5230079 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230097 DBCOMP2 Undefined shape in CompPadRot
5230098 DBCOMP2 Bad shape in CompPadRot
5230111 DBFRVIA2 "connIndex" out of range (1, theDb->header.connMax)
5230111 DBFRVIA2 "connIndex" out of range (1, theDb->header.connMax)
5230111 DBFRVIA2 "inConnPtr->crns" out of range (0, CONN_CRN_LIMIT)
5230112 DBFRVIA2 "viaCount" out of range (0, inConnPtr->crns)
5230112 DBFRVIA2 "viaCount" out of range (1, inConnPtr->crns)
5230113 DBFRVIA2 "in1ConnPtr->crns" out of range (0, CONN_CRN_LIMIT)
5230114 DBFRVIA2 "in2ConnPtr->crns" out of range (0, CONN_CRN_LIMIT)
5230115 DBFRVIA2 "netIndex" out of range (1, theDb->header.netMax)
5230115 DBFRVIA2 Failed assumption in1ConnPtr->comp[1] == frViaIndex
5230116 MZFRVIA "netIndex" out of range (1, DbView.netMax)
5230116 DBFRVIA2 "netIndex" out of range (1, theDb->header.netMax)
5230116 DBFRVIA2 Failed assumption in2ConnPtr->comp[0] == frViaIndex
5230117 DBFRVIA2 Failed assumption frViaPtr->flags2 & COMP_FREE_AND_TP
5230117 DBFRVIA2 "connViaIndex" out of range (1, theDb->header.viaMax)
5230118 DBFRVIA2 "frViaSymIndex" out of range (1, theDb->header.symMax)
5230120 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230120 DBFRVIA "viaIndex" out of range (1, theDb->header.viaMax)
5230120 DBFRVIA "viaIndex" out of range (1, theDb->header.viaMax)
5230126 DBFRVIA2 "frViaIndex" out of range (1, theDb->header.frViaMax)
5230130 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230130 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230130 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230130 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230131 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230132 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230133 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230134 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230135 DBFRVIA "symId" out of range (1, theDb->header.symMax)
5230140 DBFRVIA "frViaPtr->symIndex" out of range (1, theDb->header.symMax)
5230141 DBFRVIA Failed assumption symPtr->header.flags & SYM_TEST_POINT
5230142 DBFRVIA Failed assumption frViaPtr->flags2 & COMP_TEST_POINT
5230143 DBFRVIA Failed assumption frViaPtr->flags2 & COMP_FREE_VIA
5230144 DBFRVIA Failed assumption symPtr->header.flags & SYM_FREE_VIA
5230150 DBFRVIA "frViaIndex" out of range (1, theDb->header.frViaMax)
5230200 DBFRVIA2 Failed assumption in2ConnPtr->pin[0] == 1
5230200 DBFRVIA2 Failed assumption in1ConnPtr->pin[1] == 1
5230201 DBCOMP2 Failed assumption theDb
5230201 DBFRVIA2 Failed assumption in1ConnPtr->crn[in1ConnPtr->crns-1].x == in2ConnPtr->crn[0].x
5230202 DBFRVIA2 Failed assumption in1ConnPtr->crn[in1ConnPtr->crns-1].y == in2ConnPtr->crn[0].y
5230203 DBFRVIA2 Failed assumption in1ConnPtr->net == in2ConnPtr->net
5230205 DBCOMP2 Failed assumption theDb
5230211 DBCOMP2 Failed assumption theDb
5230211 DBCOMP2 Failed assumption theDb
5230220 DBFRVIA2 Failed assumption conn1Ptr->comp[1] == frViaIndex
5230221 DBFRVIA2 Failed assumption conn1Ptr->pin[1] == 1
5230222 DBCOMP2 Failed assumption theDb
5230222 DBFRVIA2 Failed assumption conn2Ptr->comp[0] == frViaIndex
5230223 DBFRVIA2 Failed assumption conn2Ptr->pin[0] == 1
5230230 DBFRVIA2 "in2ConnPtr->crns" out of range (0, CONN_CRN_LIMIT)
5230230 DBFRVIA2 "in1ConnPtr->crns" out of range (0, CONN_CRN_LIMIT)
5230231 DBCOMP2 Failed assumption theDb
5230234 DBCOMP2 Failed assumption theDb
5230234 DBFRVIA2 Failed assumption nextFrViaIndex > theDb->header.frViaMax
5230235 DBFRVIA2 Failed assumption nextConnIndex > theDb->header.connMax
5230237 DBCOMP2 Failed assumption theDb
5230239 DBFRVIA2 Failed assumption nextConnIndex > theDb->header.connMax
5230250 DBFRVIA2 Failed assumption conn1Ptr->comp[1] == frViaIndex
5230251 DBFRVIA2 Failed assumption conn1Ptr->pin[1] == 1
5230252 DBFRVIA2 Failed assumption conn2Ptr->comp[0] == frViaIndex
5230253 DBFRVIA2 Failed assumption conn2Ptr->pin[0] == 1
5230254 DBFRVIA Failed assumption theDb
5230274 DBFRVIA Failed assumption theDb
5230277 DBFRVIA Failed assumption theDb
5230320 DBFRVIA Failed assumption viaListInitialized == YES
5230320 DBFRVIA Failed assumption viaListInitialized == YES
5230320 DBFRVIA2 "frViaSymIndex" out of range (1, theDb->header.symMax)
5230329 DBFRVIA2 Failed assumption crn2 == out2ConnPtr->crns
5230329 DBFRVIA2 Failed assumption foundVia == YES
5230340 DBFRVIA Failed assumption theDb->header.frViaMax == theDb->header.compMax
5230374 DBFRVIA Failed assumption netTable != NULL
5230375 DBFRVIA Failed assumption netTableMax == theDb->header.frViaMax
5230654 DBFRVIA2 "netIndex" out of range (1, theDb->header.netMax)
5230654 DBFRVIA2 "netIndex" out of range (1, theDb->header.netMax)
5230678 DBFRVIA2 "connIndex" out of range (1, theDb->header.connMax)
5230876 MZFRVIA Connection endpoint [0] contains deleted free via
5230877 MZFRVIA Connection endpoint [1] contains deleted free via
5230948 DBFRVIA "netIndex" out of range (1, theDb->header.netMax)
5230987 DBCOMP2 Failed assumption currentFP <= theDb->header.symMax
5232010 DBCOLOR Failed to allocate table in DbColorInit
5232031 DBCOLOR Too many Color assignments
5232032 DBCOLOR Failed to reallocate colorTable in DbColorCreate
5232037 DBCOLOR "index" out of range (1, theDb->header.colorMax)
5232040 DBCOLOR "index" out of range (1, theDb->color.colorLimit)
5232042 DBCOLOR "index" out of range (1, theDb->color.colorLimit)
5232071 DBCOLOR Expected theDb->color.colorTable[index].lev ...
5232101 DBCOLOR Failed assumption theDb
5232108 DBCOLOR Failed assumption theDb
5232115 DBCOLOR Failed assumption theDb
5232118 DBCOLOR Failed assumption theDb
5232121 DBCOLOR Failed assumption theDb
5232123 DBCOLOR Failed assumption theDb
5232126 DBCOLOR Failed assumption theDb
5232204 DBCOLOR Failed assumption theDb
5232214 DBCOLOR Failed assumption theDb
5233020 DBNET Too many nets
5233023 DBNET Failed to allocate clusterTable in NetCreate
5233030 DBNET "netIndex" out of range (1, theDb->header.netMax)
5233040 DBNET "netIndex" out of range (1, theDb->header.netMax)
5233047 DBNET "netIndex" out of range (1, theDb->header.netMax)
5233050 DBNET "netIndex" out of range (1, theDb->header.netMax)
5233061 DBNET "i" out of range (1, theDb->header.netMax)
5233062 DBNET "j" out of range (1, theDb->header.netMax)
5233201 DBNET Failed assumption theDb
5233203 DBNET Failed assumption theDb
5233207 DBNET Failed assumption theDb
5233211 DBNET Failed assumption theDb
5233213 DBNET Failed assumption theDb
5233221 DBNET Failed assumption theDb
5233231 DBNET Failed assumption theDb
5233241 DBNET Failed assumption theDb
5233251 DBNET Failed assumption theDb
5233254 DBNET Failed assumption theDb
5233261 DBNET Failed assumption theDb
5233264 DBNET Failed assumption theDb
5233456 DBNET Failed assumption theDb->header.netMax < NET_LIMIT
5233463 DBNET Can't allocate renumber table
5234006 DBSEG Failed to reallocate net table
5234016 DBSEG Failed to allocate net table
5234017 DBSEG Failed assumption netTablePtr
5234019 DBSEG "netId" out of range (1, NET_LIMIT)
5234020 DBSEG Failed assumption checkNet (netId)
5234022 DBSEG Failed assumption segPtr);       
5234023 DBSEG Failed assumption segPtr == (SEG_RECORD MEM_PTR) activeTail->next); 
5234027 DBSEG Failed assumption segCount == segCount2
5234028 DBSEG Failed assumption nodeCount-1 <= connCount
5234030 DBSEG Failed to allocate heap for new segment
5234034 DBSEG Failed to allocate new segment record
5234035 DBSEG Failed to allocate new segment record
5234036 DBSEG Failed to allocate new segment record
5234037 DBSEG Failed to allocate heap for new segment
5234042 DBSEG Failed assumption (segPtr->kind == SEG_KIND) || (segPtr->kind == VIA_KIND)
5234049 DBSEG "net" out of range (0, NET_LIMIT)
5234052 DBSEG Failed to allocate heap for new segment
5234057 DBSEG Failed assumption netTablePtr
5234062 DBSEG Failed to allocate heap for new segment
5234069 DBSEGALT Failed assumption seg1Head->kind == PAD_KIND
5234069 DBSEG Failed assumption seg1Head->kind == PAD_KIND
5234071 DBSEG Failed to allocate heap for new segment
5234073 DBSEG Failed to allocate segment record
5234074 DBSEG Failed to allocate new segment record
5234075 DBSEG Failed to allocate new segment record
5234076 DBSEG Failed to allocate new segment record
5234077 DBSEG Failed assumption netTablePtr
5234080 DBSEG Failed to allocate new segment record
5234086 DBSEG "netId" out of range (1, theDb->header.netMax)
5234087 DBSEG Failed assumption netTablePtr
5234201 DBSEG Failed assumption theDb
5234204 DBSEG Failed assumption theDb
5234231 DBSEG Failed assumption theDb
5234261 DBSEG Failed assumption theDb
5235001 DBSEGALG Failed to allocate segment record
5235002 DBSEGALG Failed to allocate hash table
5235002 DBSEGALG Failed to allocate hash table
5235003 DBSEGALG Failed to allocate segment record
5235004 DBSEGALG Failed to allocate chain
5235004 DBSEGALG Failed to allocate chain
5235005 DBSEGALG Failed to allocate chain length table
5235005 DBSEGALG Failed to allocate chain length table
5235012 DBSEGALG Failed to allocate new hash record
5235024 DBSEGALG "chain" out of range (0, CHAIN_LIMIT)
5235025 DBSEGALG "chain" out of range (0, CHAIN_LIMIT)
5235026 DBSEGALG "chain" out of range (0, CHAIN_LIMIT)
5235042 DBSEGALG Failed to allocate segment record
5235043 DBSEGALG Failed to allocate segment record
5235072 DBSEGALG Failed to allocate segment record
5235073 DBSEGALG Failed to allocate segment record
5235077 DBSEGALG Failed to allocate new edge
5235077 DBSEGALG Failed to allocate new edge
5235077 DBSEGALG Failed to allocate new edge
5235201 DBSEGALG Failed assumption theDb
5236001 DBHASH Conn Heap
5236002 DBHASH Failed to allocate hash table
5236011 DBHASH Failed assumption hashTablePtr
5236012 DBHASH Failed to allocate hash record
5236038 DBHASH bad via shape found during buildChains
5236053 DBHASH Failed to allocate new segment record
5236083 DBHASH Failed assumption netTablePtr
5236092 DBHASH Failed to allocate new segment record
5237001 DBSEGECL Failed to allocate segment record
5237003 DBSEGECL Failed to allocate segment record
5237012 DBSEGECL Failed assumption outPtr->kind == PAD_KIND
5237013 DBSEGECL Failed assumption outPtr->kind == PAD_KIND
5237014 DBSEGECL Failed assumption segPtr1->kind == PAD_KIND
5237015 DBSEGECL Failed assumption segPtr2->kind == PAD_KIND
5237022 DBSEGECL Failed assumption netView.reconRule & NET_RECONN_ECL
5237042 DBSEGECL Failed to allocate segment record
5237043 DBSEGECL Failed to allocate segment record
5237072 DBSEGECL Failed to allocate segment record
5237073 DBSEGECL Failed to allocate segment record
5238001 URSMTNET net Heap
5238001 DBNET2 net Heap
5238007 URSMTNET Failed to allocate pin table
5238007 DBNET2 Failed to allocate pin table
5238008 URSMTNET Failed to allocate visit table
5238008 DBNET2 Failed to allocate visit table
5238012 URSMTNET Failed to allocate adjList
5238012 DBNET2 Failed to allocate adjList
5238013 URSMTNET Failed to allocate adjList
5238013 DBNET2 Failed to allocate adjList
5238017 URSMTNET Failed to allocate Adjacency List Table
5238017 DBNET2 Failed to allocate Adjacency List Table
5238030 DBNET2 "netIndex" out of range (1, theDb->header.netMax)
5238058 URSMTNET Failed assumption comp > 0
5238058 DBNET2 Failed assumption comp > 0
5238068 URSMTNET Failed assumption pinId1 > 0
5238068 DBNET2 Failed assumption pinId1 > 0
5238069 URSMTNET Failed assumption pinId2 > 0
5238069 DBNET2 Failed assumption pinId2 > 0
5238101 URSMTNET net Heap
5238101 DBNET2 net Heap
5238107 URSMTNET Failed to allocate pin SMT table
5238107 DBNET2 Failed to allocate pin SMT table
5238113 DBNET2 Failed assumption AttrNext (&attr)
5238116 DBNET2 Failed assumption lev
5238140 DBNET2 "netIndex" out of range (1, theDb->header.netMax)
5238142 DBNET2 "comp" out of range (1, theDb->header.compMax)
5238158 URSMTNET Failed assumption comp > 0
5238158 DBNET2 Failed assumption comp > 0
5238201 DBNET2 Failed assumption theDb
5238204 DBNET2 Failed assumption theDb
5238224 URSMTNET Failed assumption pinLimit < PIN_TABLE_LIMIT
5238224 DBNET2 Failed assumption pinLimit < PIN_TABLE_LIMIT
5238242 URSMTNET "pin" out of range (0, pinLimit)
5238242 DBNET2 "pin" out of range (0, pinLimit)
5238922 DBNODE2 Failed assumption theDb->node.nodeGlobals.Work->pin[0] != theDb->node.nodeGlobals.Work->pin[1]
5239010 DBCONN Failed to allocate connTable1 in ConnInit
5239010 DBNODE Failed to allocate connTable1 in ConnInit
5239011 DBCONN Failed to allocate connTable2 in ConnInit
5239012 DBCONN Failed to allocate connTable1 in ConnInit
5239013 DBCONN Failed to allocate connTable2 in ConnInit
5239014 DBCONN Failed to allocate freeList in ConnInit
5239021 DBCONN "connPtr->crns" out of range (2, CONN_CRN_LIMIT)
5239023 DBCONN "lev" out of range (0, theDb->header.levMax)
5239025 DBCONN "viaIndex" out of range (0, theDb->header.viaMax)
5239027 DBCONN Bad flag during in validateCrns
5239030 DBCONN Failed assumption theDb->header.connMax < CONN_LIMIT
5239032 DBCONN Failed to reallocate connTable in ConnCreate
5239032 DBNODE Failed to reallocate nodeTable in NodeCreate
5239032 DBNODE Failed to reallocate nodeTable in NodeCreate
5239033 DBCONN Failed to reallocate connTable1 in ConnCreate
5239034 DBNODE Failed assumption listPtr->count == 0
5239035 DBCONN Bad arc during in validateArcs
5239051 DBCONN "connPtr->net" out of range (1, theDb->header.netMax)
5239057 DBCONN "compId" out of range (0, theDb->header.compMax)
5239063 DBCONN "netIndex" out of range (1, theDb->header.netMax)
5239064 DBCONN "netIndex" out of range (1, theDb->header.netMax)
5239065 DBCONN Failed to allocate netHeadTable in linkNet
5239066 DBCONN Failed to reallocate netHeadTable in linkNet
5239070 DBCONN "netIndex" out of range (1, theDb->conn.netLimit)
5239072 DBCONN Failed assumption current > 0
5239076 DBCONN "connIndex" out of range (1, theDb->header.connMax)
5239078 DBCONN "connId" out of range (1, theDb->header.connMax+1)
5239079 DBCONN "connIndex" out of range (1, theDb->header.connMax+1)
5239080 DBCONN "index" out of range (1, theDb->header.connMax)
5239081 DBCONN Failed to allocate new space in ConnStore
5239082 DBCONN "index" out of range (1, theDb->header.connMax)
5239082 DBNODE "netId" out of range (1, theDb->header.netMax)
5239083 DBCONN "index" out of range (1, theDb->header.connMax)
5239083 DBCONN "index" out of range (1, theDb->header.connMax)
5239084 DBCONN "index" out of range (1, theDb->header.connMax)
5239085 DBCONN "index" out of range (1, theDb->header.connMax)
5239086 DBCONN "index" out of range (1, theDb->header.connMax)
5239091 DBCONN "oldPtr->crns" out of range (0, CONN_CRN_LIMIT)
5239095 DBCONN Expected netView.stdWidth > 0
5239201 DBCONN Failed assumption theDb
5239201 DBNODE Failed assumption theDb
5239211 DBCONN Failed assumption theDb
5239211 DBNODE Failed assumption theDb
5239221 DBCONN Failed assumption theDb
5239221 DBNODE Failed assumption theDb
5239221 DBNODE Failed assumption theDb
5239222 DBCONN Failed assumption theDb
5239222 DBNODE "netId" out of range (1, theDb->header.netMax)
5239223 DBCONN Failed assumption theDb->header.connMax >= listPtr->count
5239223 DBNODE Failed assumption theDb->node.tableLimit >= netId
5239224 DBNODE "nodeId" out of range (1, listPtr->count)
5239224 DBNODE "nodeId" out of range (1, theDb->node.nodeGlobals.WorkingNodeList.count)
5239234 DBCONN Failed assumption theDb
5239234 DBNODE Failed assumption theDb
5239235 DBNODE Failed assumption theDb->node.tableLimit >= netId
5239236 DBNODE "nodeId" out of range (1, theDb->node.nodeGlobals.WorkingNodeList.count)
5239236 DBNODE "nodeId" out of range (1, listPtr->count)
5239236 DBNODE "nodeId" out of range (1, theDb->node.nodeGlobals.WorkingNodeList.count)
5239237 DBCONN Failed assumption theDb
5239237 DBCONN Failed assumption theDb
5239241 DBCONN Failed assumption theDb
5239243 DBNODE Failed assumption theDb
5239244 DBNODE "netId" out of range (1, theDb->header.netMax)
5239245 DBNODE Failed assumption theDb->node.tableLimit >= netId
5239246 DBCONN Failed assumption theDb
5239246 DBNODE "nodeId" out of range (1, listPtr->count)
5239247 DBNODE "view->startLev" out of range (1, theDb->header.levMax)
5239248 DBNODE "view->endLev" out of range (1, theDb->header.levMax)
5239251 DBCONN Failed assumption theDb
5239251 DBNODE Failed assumption theDb
5239252 DBNODE "netId" out of range (1, theDb->header.netMax)
5239254 DBNODE "startLev" out of range (1, theDb->header.levMax)
5239255 DBCONN Failed assumption theDb
5239255 DBNODE "endLev" out of range (1, theDb->header.levMax)
5239261 DBCONN Failed assumption theDb
5239264 DBCONN Failed assumption theDb
5239271 DBCONN Failed assumption theDb
5239274 DBCONN Failed assumption theDb
5239277 DBCONN Failed assumption theDb
5239281 DBCONN Failed assumption theDb
5239284 DBCONN Failed assumption theDb
5239294 DBCONN Failed assumption theDb
5239295 DBCONN "connId" out of range (1, theDb->header.connMax)
5239301 DBCONN Failed assumption theDb
5239301 DBNODE Failed assumption theDb
5239303 DBCONN Failed assumption theDb
5239321 DBNODE Failed assumption theDb
5239321 DBNODE Failed assumption theDb
5239322 DBNODE "netId" out of range (1, theDb->header.netMax)
5239322 DBNODE "netId" out of range (1, theDb->header.netMax)
5239323 DBNODE Failed assumption theDb->node.tableLimit >= netId
5239323 DBNODE Failed assumption theDb->node.tableLimit >= netId
5239324 DBNODE "nodeId" out of range (1, listPtr->count)
5239324 DBNODE "nodeId" out of range (1, theDb->node.nodeGlobals.WorkingNodeList.count)
5239324 DBNODE "nodeId" out of range (1, listPtr->count)
5239324 DBNODE "nodeId" out of range (1, theDb->node.nodeGlobals.WorkingNodeList.count)
5239331 DBNODE Failed assumption theDb
5239332 DBNODE Failed assumption theDb->node.searchLink != NULL
5239337 DBCONN Failed assumption theDb
5239341 DBNODE Failed assumption theDb
5239342 DBNODE "netId" out of range (1, theDb->header.netMax)
5239343 DBNODE Failed assumption theDb->node.tableLimit >= netId
5239344 DBNODE "nodeId" out of range (1, listPtr->count)
5239344 DBNODE "nodeId" out of range (1, theDb->node.nodeGlobals.WorkingNodeList.count)
5239345 DBCONN "theDb->conn.connTable1[connIndex]->netNext" out of range (0, theDb->header.connMax)
5239346 DBCONN "theDb->conn.connTable2[connIndex-16000]->netNext" out of range (0, theDb->header.connMax)
5239348 DBNODE Failed assumption idx == (freePtr->linkCount - 1)
5239351 DBNODE Failed assumption theDb
5239352 DBNODE "netId" out of range (1, theDb->header.netMax)
5239353 DBNODE Failed assumption theDb->node.tableLimit >= netId
5239354 DBNODE "nodeId" out of range (1, listPtr->count)
5239354 DBNODE "nodeId" out of range (1, theDb->node.nodeGlobals.WorkingNodeList.count)
5239358 DBNODE Failed assumption cnt == freePtr->linkCount
5239361 DBNODE Failed assumption theDb
5239362 DBNODE Failed assumption netId != WORKING_NET
5239400 DBNODE Failed to allocate nodeTable in NodeRenumber
5239431 DBNODE Failed assumption theDb
5239432 DBNODE "netId" out of range (1, theDb->header.netMax)
5239463 DBCONN Can't allocate renumber table
5239463 DBCONN Can't allocate renumber table
5239470 DBNODE2 Failed assumption nextNew <= theDb->node.nodeGlobals.NewMax
5239471 DBNODE2 Failed assumption newPtr != NULL
5239472 DBNODE2 Failed assumption nodeRec->comp
5239473 DBNODE2 Failed assumption nodeRec->comp
5239564 DBCONN "netIndex" out of range (1, theDb->header.netMax)
5239603 DBCONN Failed assumption theDb->conn.netHeadTable
5239604 DBNODE2 Failed assumption nodeId != 0
5239608 DBNODE2 Failed assumption MakeCommonAtSource (theDb->node.nodeGlobals.Work, tmpPtr2)
5239645 DBNODE2 Failed assumption tmpPtr != NULL
5239653 DBNODE2 Failed assumption tmpPtr->pin[0] == connPtr->pin[end]
5239656 DBNODE2 Failed assumption nodeRec->connLinks
5239657 DBNODE2 Failed assumption tmpPtr->pin[0] == connPtr->pin[1]
5239673 DBNODE2 Failed assumption AltGetDepth ()
5239712 DBNODE2 Failed assumption nodePtr != NULL
5239713 DBNODE2 Failed assumption nodePtr != NULL
5239720 DBNODE Failed assumption theDb->node.nodeGlobals.SpanHeap == 0
5239741 DBSPAN Failed assumption theDb->node.nodeGlobals.NodeAdded[node->id] == child
5239773 DBNODE2 Failed assumption nodeRec->connLinks == NULL
5239776 DBNODE2 Failed assumption sLev != 0
5239776 DBNODE2 Failed assumption connId > 0
5239777 DBNODE2 Failed assumption eLev != 0
5239777 DBNODE2 Failed assumption connId2 > 0
5239778 DBNODE2 Failed assumption thruId != 0
5239781 DBNODE2 "connPtr->pin[end]" out of range (1, theDb->node.nodeGlobals.WorkingNodeList.count)
5239785 DBSPAN Failed assumption (chain1 != NULL) && (chain2 != NULL)
5239795 DBSPAN Failed assumption (tmpPtr->comp[0] == 0) && (tmpPtr->comp[1] == 0)
5240020 DBCONN2 "crn" out of range (0, connPtr->crns)
5240022 DBCONN2 "delta" out of range (1, CONN_CRN_LIMIT - connPtr->crns - 1)
5240025 DBCONN2 "crn" out of range (0, connPtr->crns - 1)
5240027 DBCONN2 "delta" out of range (0, connPtr->crns - crn)
5240201 DBCONN2 Failed assumption theDb
5240221 DBCONN2 Failed assumption theDb
5240224 DBCONN2 Failed assumption theDb
5240910 DBCONN2 missing via in ConnReduce2
5240987 DBCONN2 "crn" out of range (1, workPtr->crns - 2)
5241032 DBARC Failed assumption segNumber > 0
5242403 DBLOAD2 Failed assumption SymFind (name) == 1
5242403 DBLOAD2 Failed assumption theDb
5242403 DBLOAD2 Failed assumption theDb
5242406 DBLOAD2 Failed assumption thruId
5242406 DBLOAD2 Failed assumption thruId
5242406 DBLOAD2 Failed assumption thruId
5242406 DBLOAD2 Failed assumption theDb
5242503 DBLOAD2 Failed assumption theDb
5242513 DBLOAD2 Failed assumption theDb
5242964 DBCOPY Expected algorithm == USER_DIV
5243501 DBLOAD2 Failed assumption size == sizeof (LEV_VIEW)
5243501 DBLOAD2 Failed assumption size == sizeof (LEV_VIEW)
5243502 DBLOAD2 Failed assumption size == sizeof (DB_VIEW)
5243502 DBLOAD2 Failed assumption size == sizeof (DB_VIEW)
5243503 DBLOAD2 Failed assumption !dbFound
5243503 DBLOAD2 Failed assumption !dbFound
5243504 DBLOAD2 Failed assumption ioBuff->db.levMax > 0
5243504 DBLOAD2 Failed assumption ioBuff->db.levMax > 0
5243505 DBLOAD2 Failed assumption ioBuff->db.levMax <= LEV_ETCH_LIMIT
5243505 DBLOAD2 Failed assumption ioBuff->db.levMax <= LEV_ETCH_LIMIT
5243507 DBLOAD2 Failed assumption ioBuff->db.gridDiv >= 0
5243507 DBLOAD2 Failed assumption ioBuff->db.gridDiv >= 0
5243508 DBLOAD2 Failed assumption ioBuff->db.viaGrid >= 0
5243508 DBLOAD2 Failed assumption ioBuff->db.viaGrid >= 0
5243511 DBLOAD2 "size" out of range (0, NAME_STR_LIMIT+1)
5243511 DBLOAD2 "size" out of range (0, NAME_STR_LIMIT+1)
5243511 DBLOAD2 "size" out of range (0, NAME_STR_LIMIT+1)
5243512 DBLOAD2 Failed assumption nameCount < NAME_COUNT_LIMIT
5243512 DBLOAD2 Failed assumption nameCount < NAME_COUNT_LIMIT
5243512 DBLOAD2 Failed assumption nameCount < NAME_COUNT_LIMIT
5243521 DBLOAD2 Failed assumption size == sizeof (THRU_VIEW)
5243521 DBLOAD2 Failed assumption size == sizeof (THRU_VIEW)
5243522 DBLOAD2 Expected nameCount == 1
5243522 DBLOAD2 Expected nameCount == 1
5243524 DBLOAD2 Failed assumption nameCount == ioBuff->sym.header.pads + 1
5243525 DBLOAD2 "nameCount" out of range (1, 3)
5243527 DBLOAD2 Expected nameCount == 1
5243528 DBLOAD2 Expected nameCount == 1
5243532 DBLOAD2 Failed assumption size == sizeof (DB_VIEW)
5243533 DBLOAD2 Failed assumption !dbFound
5243534 DBLOAD2 Failed assumption ioBuff->db.levMax > 0
5243535 DBLOAD2 Failed assumption ioBuff->db.levMax <= LEV_ETCH_LIMIT
5243537 DBLOAD2 Failed assumption ioBuff->db.gridDiv >= 0
5243538 DBLOAD2 Failed assumption ioBuff->db.viaGrid >= 0
5244000 DBOBS2 Failed assumption netTablePtr
5244009 DBOBS2 Failed to allocate heap for new segment
5244028 DBOBS2 Failed to allocate heap for new segment
5244029 DBOBS2 Failed to allocate heap for new segment
5245012 DBOBS Failed to allocate theDb->obs.obsTable1 in ObsInit
5245013 DBOBS Obstacle not found on correct list
5245014 DBOBS Failed to allocate theDb->obs.symFirst in ObsInit
5245015 DBOBS Failed to allocate compFirst in ObsInit
5245020 DBOBS "crn" out of range (0, obsPtr->crns)
5245022 DBOBS Failed assumption obsPtr->crn[crn+1].flags != 0
5245024 DBOBS Failed assumption obsPtr->crns < OBS_CRN_LIMIT
5245030 DBOBS Failed assumption obsPtr->crns == 3
5245031 DBOBS Failed assumption obsPtr->crn[0].x == obsPtr->crn[2].x
5245032 DBOBS Failed assumption obsPtr->crn[0].y == obsPtr->crn[2].y
5245033 DBOBS Expected obsPtr->crn[1].flags != 0
5245035 DBOBS Expected delta <= ACCURACY
5245038 DBOBS Failed assumption theDb->obs.obsTable
5245039 DBOBS Failed assumption theDb->obs.obsTable
5245043 DBOBS "symId" out of range (0, theDb->header.symMax)
5245045 DBOBS "compId" out of range (1, theDb->header.compMax)
5245046 DBOBS "obsId" out of range (1, theDb->header.obsMax)
5245048 DBOBS Failed to enlarge theDb->obs.obsTable in ObsCreate
5245050 DBOBS "index" out of range (1, theDb->header.obsMax)
5245051 DBOBS "index" out of range (1, theDb->header.obsMax)
5245053 DBOBS "index" out of range (1, theDb->header.obsMax)
5245061 DBOBS "i" out of range (1, theDb->header.obsMax)
5245062 DBOBS "j" out of range (1, theDb->header.obsMax)
5245063 DBOBS "view->crns" out of range (1, OBS_CRN_LIMIT)
5245064 DBOBS "(short) view->lev" out of range (0, theDb->header.levMax)
5245065 DBOBS Failed assumption view->width >= 0
5245066 DBOBS "view->net" out of range (1, theDb->header.netMax)
5245071 DBOBS "obsPtr->symId" out of range (0, theDb->header.symMax)
5245072 DBOBS Failed to allocate theDb->obs.symFirst in addToLink
5245075 DBOBS "-obsPtr->symId" out of range (0, theDb->header.compMax)
5245076 DBOBS Failed to allocate theDb->obs.compFirst in addToLink
5245080 DBOBS "index" out of range (1, theDb->obs.obsLimit)
5245082 DBOBS Failed assumption --count > 0
5245091 DBOBS Failed to allocate theDb->obs.obsTable in ObsPut for first time
5245092 DBOBS Failed to allocate theDb->obs.obsTable in ObsPut, previous version exists
5245121 DBOBS Arc on corner zero
5245122 DBOBS Arc on corner zero
5245123 DBOBS Arc on last corner
5245124 DBOBS Arc on last corner
5245151 DBOBS "index" out of range (1, theDb->header.obsMax)
5245151 DBOBS "index" out of range (1, theDb->header.obsMax)
5245201 DBOBS Failed assumption theDb
5245204 DBOBS Failed assumption theDb
5245211 DBOBS Failed assumption theDb
5245221 DBOBS Failed assumption theDb
5245224 DBOBS Failed assumption theDb
5245224 DBOBS Failed assumption theDb
5245231 DBOBS Failed assumption theDb
5245241 DBOBS Failed assumption theDb
5245244 DBOBS Failed assumption theDb
5245247 DBOBS Failed assumption theDb
5245251 DBOBS Failed assumption theDb
5245254 DBOBS Failed assumption theDb
5245261 DBOBS Failed assumption theDb
5245271 DBOBS Failed assumption theDb
5245331 DBOBS Failed assumption theDb
5245332 DBOBS Failed assumption theDb
5245333 DBOBS Failed assumption theDb
5245334 DBOBS Failed assumption theDb
5245456 DBOBS Failed assumption newUid < theDb->header.lastUID
5245463 DBOBS Can't allocate renumber table
5245987 DBOBS Failed assumption theDb->header.obsMax < OBS_LIMIT
5246010 DBNOTE "length" out of range (0, LSTR_LIMIT)
5246040 DBNOTE "index" out of range (1, theDb->header.noteMax)
5246050 DBNOTE NotePut not yet implemented
5246073 DBNOTE Failed assumption theDb->note.currentNote
5246081 DBNOTE Cannot allocate new NOTE
5246201 DBNOTE Failed assumption theDb
5246204 DBNOTE Failed assumption theDb
5246211 DBNOTE Failed assumption theDb
5246252 DBNOTE Failed assumption theDb
5246253 DBNOTE Failed assumption theDb
5247010 DBREPORT Failed to allocate table in DbReportInit
5247037 DBREPORT "index" out of range (1, theDb->header.ppMax)
5247040 DBREPORT "index" out of range (1, theDb->header.ppMax)
5247042 DBREPORT "index" out of range (1, theDb->header.ppMax)
5247045 DBREPORT Failed to allocate reportTable in ReportPut for first time
5247182 DBREPORT Too many Reports
5247183 DBREPORT Failed to allocate errTable in DbErrorCreate
5247201 DBREPORT Failed assumption theDb
5247211 DBREPORT Failed assumption theDb
5247214 DBREPORT Failed assumption theDb
5247223 DBREPORT Failed assumption theDb
5247301 DBREPORT Failed assumption theDb
5247302 DBREPORT Failed assumption theDb
5247463 DBREPORT Can't allocate renumber table
5248001 DBAPP Failed assumption theDb
5248010 DBAPP Failed to allocate table in DbAppInit
5248031 DBAPP Too many Apperture assignments
5248032 DBAPP Failed to reallocate appTable in DbAppCreate
5248033 DBAPP Failed assumption theDb
5248037 DBAPP "index" out of range (1, theDb->header.appMax)
5248040 DBAPP "index" out of range (1, theDb->app.appLimit)
5248041 DBAPP Failed assumption theDb
5248042 DBAPP "index" out of range (1, theDb->header.appMax)
5248043 DBAPP "ptr->dCode" out of range (10, APP_LIMIT)
5248044 DBAPP Failed assumption theDb
5248061 DBAPP "i" out of range (1, theDb->header.appMax)
5248062 DBAPP "j" out of range (1, theDb->header.appMax)
5248067 DBAPP Failed assumption theDb
5248182 DBAPP Failed to allocate table in DbAppCreate
5248223 DBAPP Failed assumption theDb
5248224 DBAPP Failed assumption theDb
5248227 DBAPP Failed assumption theDb
5248229 DBAPP Failed assumption theDb
5248463 DBAPP Can't allocate renumber table
5250031 DBSF "(short) theDb->header.batchIndex" out of range (0, SF_BATCH_LIMIT)
5250032 DBSF "(short) theDb->header.batchIndex" out of range (0, SF_BATCH_LIMIT)
5250044 DBSF "(short) ptr->viaCost[pass]" out of range (0, 100)
5250046 DBSF "(short) ptr->routeLimit[pass]" out of range (0, 100)
5250048 DBSF "(short) ptr->retryCost[pass]" out of range (0, 100)
5250052 DBSF "(short) ptr->cellCost[lev]" out of range (0, 100)
5250054 DBSF "(short) ptr->primaryDirect[lev]" out of range (0, 100)
5250056 DBSF Failed assumption lstrLen (ptr->title) <= SF_TITLE_MAX
5250106 DBCONN2 Failed assumption connPtr->crn[connPtr->crns-1].x == symPadView.x
5250107 DBCONN2 Failed assumption connPtr->crn[connPtr->crns-1].y == symPadView.y
5250110 DBCONN2 Failed assumption rotation[end]
5250201 DBSF Failed assumption theDb
5250211 DBSF Failed assumption theDb
5255000 DBALT "theDb->alt.depth" out of range (0, DEPTH_LIMIT)
5255002 DBALT Failed assumption listPtr->magicNumber == MAGIC_NUMBER
5255004 DBALT Failed assumption shadowPtr->magicNumber == MAGIC_NUMBER
5255006 DBALT Failed assumption listPtr->alt.id == shadowPtr->alt.id
5255008 DBALT Failed assumption listPtr->alt.kind == shadowPtr->alt.kind
5255009 DBALT Failed to allocate free list table in AltPush
5255009 DBALT Failed assumption listPtr->alt.kind == kind
5255010 DBALT Failed assumption theDb->alt.altMax == count
5255011 DBALT Failed assumption theDb->alt.listSize[kind] == kindCount
5255012 DBALT Failed assumption listPtr->alt.depth == 1
5255013 DBALT Failed assumption unionPtr != NULL
5255014 DBALT "size" out of range (0, sizeof (UNION_VIEW))
5255017 DBALT Failed assumption theDb->alt.depth == 1
5255018 DBALT Failed assumption theDb->alt.listSize[kind] == 0
5255019 DBALT Failed assumption theDb->alt.listTail[kind]->next
5255020 DBALT "size" out of range (0, sizeof (UNION_VIEW))
5255021 DBALT Failed to allocate new memory in allocMem
5255022 DBALT Failed assumption theDb->alt.listTail[kind]->next == NULL
5255023 DBALT Failed assumption theDb->alt.listTail[kind]->magicNumber == MAGIC_NUMBER
5255024 DBALT Failed assumption listPtr->magicNumber == MAGIC_NUMBER
5255031 DBALT Failed assumption theDb->alt.lastAltPtr
5255032 DBALT Failed assumption theDb->alt.lastAltPtr->linearNext
5255033 DBALT Failed assumption theDb->alt.lastAltPtr
5255034 DBALT Failed assumption theDb->alt.lastAltPtr->linearNext
5255034 DBALT Failed assumption theDb->alt.altMax == 0
5255035 DBALT Failed assumption theDb->alt.lastAltPtr
5255036 DBALT Failed assumption theDb->alt.lastAltPtr->linearNext
5255036 DBALT Failed assumption theDb->alt.linearTail->magicNumber == MAGIC_NUMBER
5255037 DBALT Failed assumption theDb->alt.linearTail->linearNext == NULL
5255038 DBALT Failed assumption theDb->alt.altMax == count
5255042 DBALT Failed assumption theDb
5255043 DBALT Failed assumption listPtr->alt.id == shadowPtr->alt.id
5255045 DBALT Failed assumption theDb->alt.linearTail->linearNext
5255048 DBALT Failed assumption theDb->alt.depth > 0
5255053 DBALT Failed assumption listPtr->alt.kind == kind
5255054 DBALT Failed assumption listPtr->magicNumber == MAGIC_NUMBER
5255058 DBALT Expected listPtr->alt.objIndex == objIndex
5255061 DBALT Deleting duplicates in ALT
5255062 DBALT Duplicate in ALT
5255067 DBALT Failed assumption theDb
5255068 DBALT Expected AltGetDepth () == 0, 0
5255068 DBALT Failed assumption listPtr->magicNumber == MAGIC_NUMBER
5255070 DBALT Failed assumption theDb->alt.lastAltPtr
5255071 DBALT Failed assumption listPtr->alt.kind == altPtr->kind
5255072 DBALT Failed assumption listPtr->alt.id == altPtr->id
5255073 DBALT Failed assumption listPtr->alt.altIndex == altPtr->altIndex
5255074 DBALT Expected listPtr->alt.objIndex == altPtr->objIndex
5255075 DBALT Failed assumption listPtr->alt.kind == NODE_KIND
5255076 DBALT Failed to allocate oldRecordPtr in AltStore
5255093 DBALT Failed assumption theDb->alt.listSize[NODE_KIND] == 0
5255093 DBALT Failed assumption theDb->alt.listSize[kind] == 0
5255094 DBALT Failed assumption theDb->alt.altMax == 0
5255094 DBALT Failed assumption theDb->alt.altMax == 0
5255111 DBALT Failed assumption theDb->alt.altMax == count
5255222 DBALT Failed assumption theDb
5255241 DBALT Failed assumption theDb
5255242 DBALT Failed assumption theDb
5255244 DBALT Failed assumption theDb
5255246 DBALT Failed assumption theDb
5255246 DBALT Failed assumption theDb
5255248 DBALT Failed assumption theDb
5255254 DBALT Failed assumption theDb
5255255 DBALT Failed assumption kind != NODE_KIND
5255263 DBALT Failed assumption theDb
5255266 DBALT Failed assumption theDb
5255268 DBALT Failed assumption theDb
5255269 DBALT Failed assumption listPtr->alt.kind == kind
5255272 DBALT Failed assumption theDb
5255274 DBALT Failed assumption theDb
5255275 DBALT Failed assumption listPtr->alt.kind == kind
5255277 DBALT Failed assumption theDb
5255282 DBALT Failed assumption theDb
5255283 DBALT Failed assumption kind != NODE_KIND
5255284 DBALT Failed assumption theDb
5255284 DBALT Failed assumption listPtr->alt.kind == kind
5255285 DBALT Failed assumption theDb
5255286 DBALT Failed assumption theDb
5255286 DBALT Failed assumption theDb
5255286 DBALT Failed assumption theDb
5255287 DBALT Failed assumption kind != NODE_KIND
5255287 DBALT Failed assumption kind != NODE_KIND
5255287 DBALT Failed assumption kind != NODE_KIND
5255288 DBALT Failed assumption theDb
5255288 DBALT Failed assumption listPtr->alt.kind == kind
5255289 DBALT Failed assumption theDb
5255289 DBALT Failed assumption kind != NODE_KIND
5255291 DBALT Failed assumption theDb
5255293 DBALT Failed assumption theDb
5255297 DBALT Failed assumption theDb
5255299 DBALT Failed assumption theDb
5255301 DBALT Failed assumption theDb
5255349 DBALT Failed assumption listPtr->oldData == NULL);  
5255389 DBALT Failed assumption theDb
5255456 DBALT Expected theDb->alt.depth == 1, 1
5255457 DBALT Failed assumption listPtr->magicNumber == MAGIC_NUMBER
5255458 DBALT Failed assumption listPtr->alt.depth == 1
5255648 DBALT Failed assumption theDb->alt.depth > 0
5255654 DBALT Failed assumption theDb
5255756 DBALT Tried to AltPush while altRevert flag set
5255757 DBALT Failed assumption theDb->alt.altRevert == NO
5255757 DBALT Failed assumption theDb->alt.depth == 0
5259035 DBSHIFT Expected theDb->header.pragma == 0
5259036 DBSHIFT Expected theDb->header.pragma == 0
5259085 DBSHIFT Expected (theDb->header.boardXMin >= 0)
5259086 DBSHIFT Expected (theDb->header.boardYMin >= 0)
5259201 DBSHIFT Failed assumption theDb
5259204 DBSHIFT Failed assumption theDb
5259552 DBSHIFT Failed assumption theDb->header.valid
5261018 DBDISTP Wrong shape in DistPntPad
5261048 DBDISTP Wrong shape in DistSegPad
5261049 DBDISTP Wrong shape in DistSegPad
5261050 DBDISTP "compId" out of range (1, theDb->header.compMax)
5261051 DBDISTP "compId" out of range (1, theDb->header.compMax)
5261052 DBDISTP "compId" out of range (1, theDb->header.compMax)
5261098 DBDISTP Wrong shape in DistPntPad
5261201 DBDISTP Failed assumption theDb
5261211 DBDISTP Failed assumption theDb
5261231 DBDISTP Failed assumption theDb
5261248 DBDISTP Wrong shape in DistRectPad
5261249 DBDISTP Wrong shape in DistRectPad
5262071 DBINTSEC IntSegDiag with non-diag
5263014 DBDISTA Failed assumption order == 1
5263015 DBDISTA Failed assumption order == 1
5263016 DBDISTA Failed assumption intersectAngle2 == YES
5263017 DBDISTA Failed assumption order == 1
5263024 DBDISTA Failed assumption theDb->header.gridDiv > 0
5263201 DBDISTA Failed assumption theDb
5263211 DBDISTA Failed assumption theDb
5264190 DBARC2 Failed assumption crn >= 0
5264192 DBARC2 Failed assumption crn >= 0
5265019 DBSEGALT "netId" out of range (1, NET_LIMIT)
5265020 DBSEGALT Failed assumption checkNet (netId, altMax)
5265022 DBSEGALT Failed assumption segPtr);       
5265023 DBSEGALT Failed assumption segPtr == (SEG_RECORD MEM_PTR) activeTail->next);
5265027 DBSEGALT Failed assumption segCount == segCount2
5265028 DBSEGALT Expected (nodeCount - 1) <= connCount
5265034 DBSEGALT Failed to allocate new segment record
5265035 DBSEGALT Failed to allocate new segment record
5265036 DBSEGALT Failed to allocate new segment record
5265069 DBSEGALT Failed assumption linkPtr->kind == VIA_KIND
5265073 DBSEGALT Failed to allocate segment record
5265101 DBSEGALT Failed assumption (altView.flags & ALT_DELETE) == NO
5265201 DBSEGALT Failed assumption theDb
5265531 DBSEGALT altNet Heap
5265534 DBSEGALT Failed to allocate altNetTable
5265590 DBSEGALT Failed assumption altView.kind == CONN_KIND
5265591 DBSEGALT Failed assumption ! (altView.flags & ALT_DELETE)
5265592 DBSEGALT Failed assumption work->conn.net == netId
5265596 DBSEGALT Failed assumption ! (altView.flags & ALT_DELETE)
5265597 DBSEGALT Failed assumption (altView.flags & ALT_ADD)
5265598 DBSEGALT Failed assumption work->conn.net == netId
5265999 DBSEGALT Failed assumption newConnId > altView.id
5266201 DBTEAR Failed assumption theDb
5267011 DBPINMAP No RAM for pinmap hash table
5267012 DBPINMAP No RAM for pinmap
5267012 ECOMAIN No RAM for FRVIA_RECORD
5267201 DBPINMAP Failed assumption theDb
5267331 DBPINMAP Failed assumption theDb
5267332 DBPINMAP Failed assumption theDb
5269001 DBCOPY Failed assumption symId == 0
5270123 DBSCAN Scan Heap
5270150 DBSCAN Failed assumption libFileId > 0
5270151 DBSCAN Failed assumption index > 0
5270154 DBSCAN Failed assumption libFileId == libFileId
5270174 DBSCAN Allocate a block for it
5270198 DBSCAN Unexpected union kind
5270201 DBSCAN Failed assumption theDb
5270334 DBSCAN Allocate a record for it
5270346 DBSCAN Object not found
5270406 DBSCAN Bad kind in SCAN_RECORD
5270702 DBSCAN Failed assumption dictHeap == 0
5270704 DBSCAN Dictionary Heap
5270967 DBSCAN Failed assumption currentLibId == libFileId
5270995 DBSCAN Failed assumption currentLibId == libFileId
5270996 DBSCAN Failed assumption currentLibId == libFileId
5270997 DBSCAN Failed assumption currentLibId == libFileId
5282201 DBMIRROR Failed assumption theDb
5282203 DBMIRROR Failed assumption theDb
5282206 DBMIRROR Failed assumption theDb
5282303 DBMIRROR Failed assumption AltGetDepth ()
5282305 DBMIRROR Failed assumption symPtr->header.derived != 0
5352134 GEOOBS Failed assumption prevPtr->next == NULL
5352135 GEOOBS Failed assumption prevPrevPtr->next == prevPtr
5352136 GEOOBS outSegList: bad seg list
5352137 GEOOBS outSegList: bad seg list
5352138 GEOOBS Failed assumption count == listSize
5352139 GEOOBS Failed assumption count == listSize
5352177 GEOSPLIT Failed assumption segPtr != NULL
5352177 GEOOBS Failed assumption segPtr != NULL
6060126 COMINI Can't allocate line buffer
6060126 COMINI Can't allocate line buffer
6060128 COMINI "Error finding Layout.Ini file"
6060129 COMINI "Error opening Layout.Ini file"
6060241 COMINI Expected !bufferFull
6060243 COMINI Failed assumption sectionIsActive == FALSE
6060345 COMINI Expected iniFile == NULL
6060346 COMINI Expected iniFile == NULL
6070234 COMFILE stat function failed
6070253 COMFILE stat function failed
6070275 COMFILE Expected --saveDepth == 0
6070277 COMFILE Failed assumption *drive >= 0
6070278 COMFILE "drive" out of range (0, 26)
6070334 COMFILE stat function failed
6070534 COMFILE stat function failed
6070800 COMFILE Expected ++saveDepth == 1
6070802 COMFILE Expected ++saveDepth == 1
6070900 COMFILE Expected --saveDepth == 0

*/
                                   
#include "stdafx.h"
#include "ccdoc.h"                                           
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "dbutil.h"
#include <math.h>
#include <float.h>
#include "ck.h"
#include "pcblayer.h"
#include "graph.h"
#include "centroid.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

#define  LAY_TOP           -1
#define  LAY_BOT           -2
#define  LAY_INNER         -3
#define  LAY_ALL           -4
#define  ORCAD_SMALLNUMBER 0.1      // accurracy 

typedef struct
{
   int            block_num;                    //
   CString        name;
   int            used_as_via;                  // 1 is used as pads, 2 is used as vias
   int            padstack_offset_index;        // vias and pads are seperated.
   int            padstack_mirror_offset_index; // vias and pads are seperated.
   int            via_offset_index;             // vias and pads are seperated.
   int            smd;
   int            layer_app[31];                // layer aperture index. -1 is not used.
}ORCADPadstack;
typedef CTypedPtrArray<CPtrArray, ORCADPadstack*> CPadstackArray;

typedef struct
{
   int            block_num;              //
   CString        name;
   int            sym_index;              // SYM number in orcad
   int            sym_mirindex;
   int            mirror;                 // this is the mirror definition
   int            viadef;
}ORCADSym;
typedef CTypedPtrArray<CPtrArray, ORCADSym*> CSymArray;

typedef struct
{
   int            block_num;              //
   CString        name;
   int            comp_index;
}ORCADComp;
typedef CTypedPtrArray<CPtrArray, ORCADComp*> CCompArray;

typedef struct
{
   CString        name;
   int            net_index;
}ORCADNet;
typedef CTypedPtrArray<CPtrArray, ORCADNet*> CNetArray;

//
typedef struct
{
   int      index;      // pinnumber
   double   pinx, piny;
   int      rotation;   // degree
   CString  padstackname;
   int      padstackindex;
   CString  pinname;
   int      mechanical;
}ORCADShapePinInst;

typedef CTypedPtrArray<CPtrArray, ORCADShapePinInst*> ShapePinInstArray;
//
typedef struct
{
   int      geomnum;
   CString  padstackname;
   int      index;      // pinnumber
   CString  pinname;
   double   x,y,rot;
}ORCADCompPinInst;
typedef CTypedPtrArray<CPtrArray, ORCADCompPinInst*> CompPinInstArray;

typedef struct
{
   CString  padname;
   CString  newpadname;
   int      rotation;   // rotation in degree
   int      padstack_offset_index;        //  
   int      padstack_mirror_offset_index; //  
} ORCADPadRotStruct;

typedef CTypedPtrArray<CPtrArray, ORCADPadRotStruct*> PadRotArray;

//extern char             *layertypes[];

static CCEtoODBDoc       *doc;
static FormatStruct     *format;
static FILE             *wfp;
static FILE             *flog;
static int              display_error;

static   CString        ident;
static   int            identcnt = 0;

static   PadRotArray    padrotarray;
static   int            padrotcnt;

static   ShapePinInstArray shapepininstarray;   
static   int            shapepininstcnt;

static   CompPinInstArray  comppininstarray; 
static   int            comppininstcnt;

static   CPadstackArray padstackarray;    // this is the device - mapping
static   int            padstackcnt;

static   CSymArray      symarray;      // this is the device - mapping
static   int            symcnt;

static   CCompArray     comparray;     // this is the device - mapping
static   int            compcnt;

static   CNetArray      netarray;      // this is the device - mapping
static   int            netcnt;

static   int            max_layercnt;
static   int            max_signalcnt;
static   int            output_units_accuracy = 1; // 1/10 of a mil.
static   long           cur_uid, cur_obscnt, cur_txtcnt;
 
static   int            mirror_layer[31]; // lookup of ORCAD layerindex to mirror

static   CORCADLayerArray orcadArr;
static   int            maxArr = 0;

static int Get_PADSTACKData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int drilllayer);

static void ORCAD_WriteSymData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer, int symindex);

static void ORCAD_WriteNetData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer, int netindex);

static int  Orcad_WriteSYMInsertData(FILE *wfp, CDataList *DataList,
                   double insert_x, double insert_y,
                   double rotation, int mirror, double scale,
                   int embeddedLevel,int layer);

static int  load_ORCADsettings(const char *fname);
static int  get_symptr(int blocknum, int mirror);
static int  get_netptr(const char *n);
static int  get_compptr(const char *n);
static int  get_comppinptr(const char *c, const char *p);
static int  DegToMin(double r);
static int  Layer_ORCAD(int l);
static int  get_padrot(const char *p, int rot);

static int  CREATE_MIRRORED_COMPONENTS = TRUE;

static   double         ARC_ANGLE_DEGREE;
static   bool           isNetArc;

/****************************************************************************/
/*
*/
static int worcad_uid(FILE *fo, long id)
{
   fprintf(fo,"%s(Uid %ld)\n",ident,id);
   return 1;
}

/****************************************************************************/
/*
   this converts mil to mass. 
   Scale converts design units to mil.
*/
static long  cnv_unit(double x)
{
   long i = (long)floor (x * 60.0 + 0.5);
   return i;
}  

/****************************************************************************/
/*
   cx, cy is the centroid
*/
static int worcad_symuid(FILE *fo, long id, double cx, double cy)
{
   fprintf(fo,"%s(Uid %ld %ld %ld 0 0)\n",ident,id, cnv_unit(cx), cnv_unit(cy));
   return 1;
}

/****************************************************************************/
/*
*/
static int   worcad_plusident()
{
   identcnt++;
   ident += " ";
   return 1;
}

/****************************************************************************/
/*
*/
static int   worcad_minusident()
{
   if (identcnt > 0)
   {
      identcnt--;
      if (identcnt == 0)
         ident = "";
      else
         ident.Format("%*s",identcnt, " ");
   }
   else
   {
      ErrorMessage("Error in minus ident\n");
      return -1;
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int worcad_close_b(FILE *fo)
{
   worcad_minusident(); 
   fprintf(fo,"%s)\n",ident);
   return 1;
}

/*****************************************************************************/
/*
*/
static int ORCADPinNameCompareFunc( const void *arg1, const void *arg2 )
{
   ORCADShapePinInst **a1, **a2;
   a1 = (ORCADShapePinInst**)arg1;
   a2 = (ORCADShapePinInst**)arg2;

   // order so that mechanical holes come in last.
   if ((*a1)->mechanical && !(*a2)->mechanical)
      return -1;
   // order so that mechanical holes come in last.
   if (!(*a1)->mechanical && (*a2)->mechanical)
      return 1;

   return compare_name((*a1)->pinname, (*a2)->pinname);
}

/*****************************************************************************/
/*
   If actually calls sort twice, once without any pins, once with. That is 
   because of the 2 pass to get artwork info.
*/
static void ORCAD_SortPinData(const char *s)
{
   int   done = FALSE;
   int   sortcnt = 0;

   if (shapepininstcnt < 2)   return;

   qsort(shapepininstarray.GetData(), shapepininstcnt, sizeof(ORCADShapePinInst *), ORCADPinNameCompareFunc);
   return;
}

/*****************************************************************************/
/*
*/
static int get_dummy_pinnr()
{
   int   i, dummy = 0;
   int   found = TRUE;

   while (found)
   {
      found = FALSE;
      CString  d;
      d.Format("Dummy%d", ++dummy);
      for (i=0;i<shapepininstcnt;i++)
      {
         if (shapepininstarray[i]->pinname.Compare(d) == 0)
            found = TRUE;
      }
   }
   return dummy;
}

/*****************************************************************************/
/*
   mechanical pins can have same names, but this is not allowed in ACCEL
*/
static void ORCAD_MakePinNames_Unique(const char *s)
{
   int   i, ii;

   // check empty pinnames 
   for (i=0;i<shapepininstcnt;i++)
   {
      ORCADShapePinInst *p1 = shapepininstarray[i];
      if (strlen(p1->pinname) == 0)
         p1->pinname.Format("Dummy%d", get_dummy_pinnr());
   }

   for (i=0;i<shapepininstcnt;i++)
   {
      for (ii=0;ii<i;ii++)
      {
         ORCADShapePinInst *p1 = shapepininstarray[i];
         ORCADShapePinInst *p2 = shapepininstarray[ii];
         if(p1->pinname.Compare(p2->pinname) == 0)
         {
            if (p1->mechanical && p2->mechanical)
            {
               // both are mechanical
               p1->pinname.Format("Dummy%d", get_dummy_pinnr());
               p2->pinname.Format("Dummy%d", get_dummy_pinnr());
            }
            else
            if (!p1->mechanical && p2->mechanical)
            {
               // p1 is electrical, p2 is mechanical
               p2->pinname.Format("Dummy%d", get_dummy_pinnr());
            }
            else
            if (p1->mechanical && !p2->mechanical)
            {
               // p2 is electrical, p1 is mechanical
               p1->pinname.Format("Dummy%d", get_dummy_pinnr());
            }
            else
            {
               fprintf(flog,"Pattern [%s] has 2 same electrical pinnames!\n", s);
               display_error++;
               p1->pinname.Format("Dummy%d", get_dummy_pinnr());
            }
         }
      }
   
   }

   return;
}

//--------------------------------------------------------------
static int get_padstackindex(const char *p)
{
   int   i;
   
   for (i=0;i<padstackcnt;i++)
   {
      if (!padstackarray[i]->name.CompareNoCase(p))
         return i;
   }

   fprintf(flog,"Padstack [%s] not found in index\n",p);
   display_error++;
   return -1;
}

//--------------------------------------------------------------
static int get_rotatedpadstackindex(const char *p)
{
   int   i;
   
   for (i=0;i<padstackcnt;i++)
   {
      if (!padrotarray[i]->newpadname.CompareNoCase(p))
         return i;
   }

   fprintf(flog,"Rotated Padstack [%s] not found in index\n",p);
   display_error++;
   return -1;
}

//--------------------------------------------------------------
static void get_vianames(CDataList *DataList)
{
   DataStruct *np;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)  continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL ||
                np->getInsert()->getInsertType() == INSERTTYPE_PIN ||
                np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN ||
                np->getInsert()->getInsertType() == INSERTTYPE_TESTPAD)
            {
               int ptr = get_padstackindex(block->getName());

               if (ptr > -1) 
               {
                  padstackarray[ptr]->used_as_via |= 1;
               }
               break;
            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA ||
                np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
            {
               int ptr = get_padstackindex(block->getName());

               if (ptr > -1) 
               {
                  padstackarray[ptr]->used_as_via |= 2;
               }
               break;
            }

            if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               get_vianames(&(block->getDataList()));
            } // end else not aperture
         } // case INSERT
         break;
      } // end switch
   } // end get_vianames */
   return;
}

//--------------------------------------------------------------
static int get_layernum_from_stacknum(int signr)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (orcadArr[i]->signr == signr)
         return orcadArr[i]->stackNum;
   }
   return 0;
}

//--------------------------------------------------------------
static int get_layer_stacknum(int stacknum)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (orcadArr[i]->stackNum == stacknum)
         return i;
   }
   return -1;
}

//--------------------------------------------------------------
// 0 - connect
// 1 - top
// 2 - bottom
// 3..16 -inner1..inner12
//
static void do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0, layernum;
   char        typ = 'D';
   CString     orcadlay, nickname, libname;

   int   j;
   int   doc_cnt = 0;
   int   highest_stacknum = 0;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.
      if (layer->getElectricalStackNumber() > highest_stacknum)
         highest_stacknum = layer->getElectricalStackNumber();
   }

   max_signalcnt = highest_stacknum;
   maxArr = 0;

   for (j=0; j< doc->getMaxLayerIndex(); j++)  // getMaxLayerIndex() is the exact number of layers, not
                                             // the next index.
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)   continue; // could have been deleted.

      signr = 0;
      typ = 'D';
      orcadlay = "";
      nickname = "";
      libname = "";

      if (layer->isFloating())    continue;

      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();

         if (layer->getLayerType() == LAYTYPE_PAD_TOP)
         {
            layernum = 1;
            orcadlay = "TOP";
            nickname = "TOP";
            libname = "TOP";
         }
         else
         if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
         {
            layernum = 2;
            orcadlay = "BOTTOM";
            nickname = "BOT";
            libname = "BOT";
         }
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_TOP)
         {
            layernum = 1;
            orcadlay = "TOP";
            nickname = "TOP";
            libname = "TOP";
         }
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_BOT)
         {
            layernum = 2;
            orcadlay = "BOTTOM";
            nickname = "BOT";
            libname = "BOT";
         }
         else
         if (layer->getLayerType() == LAYTYPE_POWERPOS)
         {
            orcadlay.Format("INNER%d",signr);
            nickname.Format("IN%d",signr);
            layernum = get_layernum_from_stacknum(signr);
            if (layernum == 0)
               layernum = 1+signr;  // 2 if always bottom
            libname = "INNER";
         }
         else
         if (layer->getLayerType() == LAYTYPE_SIGNAL_INNER)
         {
            orcadlay.Format("INNER%d",signr);
            nickname.Format("IN%d",signr);
            layernum = get_layernum_from_stacknum(signr);
            if (layernum == 0)
               layernum = 1+signr;
            libname = "INNER";
         }
         else
         if (layer->getLayerType() == LAYTYPE_POWERNEG || layer->getLayerType() == LAYTYPE_PAD_THERMAL)
         {
            typ = 'P';
            orcadlay.Format("INNER%d",signr);
            nickname.Format("IN%d",signr);
            layernum = get_layernum_from_stacknum(signr);
            if (layernum == 0)
               layernum = 1+signr;
            libname = "INNER";
         }
         else
         if (layer->getLayerType() == LAYTYPE_SPLITPLANE)
         {
            typ = 'P';
            orcadlay.Format("INNER%d",signr);
            nickname.Format("IN%d",signr);
            layernum = get_layernum_from_stacknum(signr);
            if (layernum == 0)
               layernum = 1+signr;
            libname = "INNER";
         }
         else
         if (layer->getLayerType() == LAYTYPE_PAD_INNER)
         {
            orcadlay.Format("INNER%d",signr);
            nickname.Format("IN%d",signr);
            layernum = get_layernum_from_stacknum(signr);
            if (layernum == 0)
               layernum = 1+signr;
            libname = "INNER";
         }
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_INNER)
      {
         // this is not a layer to assign. 
         continue;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_OUTER)
      {
         // this is not a layer to assign. 
         continue;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_ALL)
      {
         // this is not a layer to assign. 
         continue;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_TOP)
      {
         layernum = 1;
         orcadlay = "TOP";
         nickname = "TOP";
         libname = "TOP";
      }
      else
      if (layer->getLayerType() == LAYTYPE_PAD_BOTTOM)
      {
         layernum = 2;
         orcadlay = "BOTTOM";
         nickname = "BOT";
         libname = "BOT";
      }
      else
      if (layer->getLayerType() == LAYTYPE_DRILL)
      {
         orcadlay = "DRILL";
         nickname = "DRL";
         libname = "DRILL";
         layernum = 26;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PASTE_TOP || !layer->getName().CompareNoCase("SPTOP"))
      {
         orcadlay = "SPTOP";
         nickname = "SPTOP";
         libname = "SPTOP";
         layernum = 19;
      }
      else
      if (layer->getLayerType() == LAYTYPE_PASTE_BOTTOM || !layer->getName().CompareNoCase("SPBOT"))
      {
         orcadlay = "SPBOT";
         nickname = "SPBOT";
         libname = "SPBOT";
         layernum = 20;
      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_TOP || !layer->getName().CompareNoCase("SMTOP"))
      {
         orcadlay = "SMTOP";
         nickname = "SMTOP";
         libname = "SMTOP";
         layernum = 17;
      }
      else
      if (layer->getLayerType() == LAYTYPE_MASK_BOTTOM || !layer->getName().CompareNoCase("SMBOT"))
      {
         orcadlay = "SMBOT";
         nickname = "SMT";
         libname = "SMBOT";
         layernum = 18;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SILK_TOP || !layer->getName().CompareNoCase("SSTOP"))
      {
         orcadlay = "SSTOP";
         nickname = "SST";
         libname = "SSTOP";
         layernum = 21;
      }
      else
      if (layer->getLayerType() == LAYTYPE_SILK_BOTTOM || !layer->getName().CompareNoCase("SSBOT"))
      {
         orcadlay = "SSBOT";
         nickname = "SSB";
         libname = "SSBOT";
         layernum = 22;
      }
      else
      if (layer->getLayerType() == LAYTYPE_BOARD_OUTLINE || !layer->getName().CompareNoCase("Conn"))
      {
         orcadlay = "Conn";
         nickname = "0";
         libname = "0";
         layernum = 0;
      }
      else
      if (!layer->getName().CompareNoCase("TOP")) // just in case it comes here
      {
         orcadlay = "TOP";
         nickname = "TOP";
         libname = "TOP";
         layernum = 1;
      }
      else
      if (!layer->getName().CompareNoCase("BOTTOM")) // just in case 
      {
         orcadlay = "BOTTOM";
         nickname = "BOT";
         libname = "BOT";
         layernum = 2;
      }
      else
      if (!layer->getName().CompareNoCase("DRILL")) // just in case 
      {
         orcadlay = "DRILL";
         nickname = "DRL";
         libname = "DRILL";
         layernum = 26;
      }
      else
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;
         orcadlay = layer->getName();
         doc_cnt++;
         layernum = highest_stacknum+doc_cnt;
      }

      if (strlen(nickname) == 0) nickname = orcadlay;
      if (strlen(libname) == 0)  libname = orcadlay;

      int on = TRUE;
      if (layernum > 30)   
      {
         layernum = 30;
         on = FALSE;
      }
      //orcadArr.SetSizes
      ORCADLayerStruct *orcad = new ORCADLayerStruct;
      orcad->stackNum = layernum;  // layernum
      orcad->signr = signr;
      orcad->layerindex = j;
      orcad->on = on;
      orcad->type = typ;
      orcad->oldName = layer->getName();
      orcad->newName = orcadlay;
      orcad->nickName = nickname;
      orcad->libName = libname;
      orcadArr.SetAtGrow(maxArr++, orcad);
   }

   return;
}

//--------------------------------------------------------------
static   int   get_layernr_from_index(int index)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (orcadArr[i]->layerindex == index)
         return orcadArr[i]->stackNum;
   }

   return -99;
}

//--------------------------------------------------------------
static   int   get_layerptr_from_index(int index)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (orcadArr[i]->layerindex == index)
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static int get_padstacks(double scale)
{
   int   drilllayerindex = -1;
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (!STRCMPI(orcadArr[i]->nickName,"DRL"))
         drilllayerindex = orcadArr[i]->stackNum;
   }

   if (drilllayerindex < 0)
   {
      fprintf(flog, "No DRILL layer found -> drills are not translated.\n");
      display_error++;
   }

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PADSTACK))
      {
         ORCADPadstack  *p = new ORCADPadstack;
         padstackarray.SetAtGrow(padstackcnt,p);  
         padstackarray[padstackcnt]->used_as_via = 0;       
         padstackarray[padstackcnt]->name = block->getName();
         padstackarray[padstackcnt]->block_num = block->getBlockNumber();
         padstackarray[padstackcnt]->padstack_offset_index = 0;
         padstackarray[padstackcnt]->padstack_mirror_offset_index = 0;
         padstackarray[padstackcnt]->via_offset_index = 0;

         int smd = FALSE;
         Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 2);
         if (a)   smd = TRUE;
         padstackarray[padstackcnt]->smd = smd;
         for (int l=0;l<31;l++)
            padstackarray[padstackcnt]->layer_app[l] = -1;

         int typ = Get_PADSTACKData( &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1, drilllayerindex);
/*
         double psize = drill*scale+10;

         // must have top and bottom. (otherwise Accel EDA crashes)
         if (!(typ & 1))
            fprintf(fp,"%s(viaShape (layerNumRef 1) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);
         if (!(typ & 2))
            fprintf(fp,"%s(viaShape (layerNumRef 2) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);

         if (!(typ & 4))
            fprintf(fp,"%s(viaShape (layerType Signal) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);

         if (!(typ & 8))
            fprintf(fp,"%s(viaShape (layerType Plane) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);
         if (!(typ & 16))
            fprintf(fp,"%s(viaShape (layerType NonSignal) (viaShapeType Ellipse) (shapeWidth %1.*lf) (shapeHeight %1.*lf) )\n",
               ident, output_units_accuracy, psize, output_units_accuracy, psize);
*/       

         padstackcnt++;
      }
   }

   return 1;
}

//--------------------------------------------------------------
static int edit_layerlist()
{
   // fill array
   ORCADLayer orcaddlg;

   orcaddlg.arr = &orcadArr;
   orcaddlg.maxArr = maxArr;

   if (orcaddlg.DoModal() != IDOK) 
      return FALSE;

   int   i;
   max_layercnt = 0;

   for (i=0;i<maxArr;i++)
   {
      if (orcadArr[i]->on == FALSE) continue;
      if (orcadArr[i]->stackNum > max_layercnt)
         max_layercnt = orcadArr[i]->stackNum;
   }

   if (max_layercnt > 30)
   {
      ErrorMessage("Maximum 30 Orcad Layers are allowed!", "Layer Edit Error");
      return FALSE;
   }

   return TRUE;
}

//--------------------------------------------------------------
static void free_layerlist()
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      delete orcadArr[i];
   }
   orcadArr.RemoveAll();

   return;
}

//--------------------------------------------------------------
static int worcad_net(FILE *fp, FileStruct *file)
{

   NetStruct *net;
   POSITION  netPos;
   int   unusedpincnt = 0;
   
   netPos = file->getNetList().GetHeadPosition();
   while (netPos != NULL)
   {
      net = file->getNetList().GetNext(netPos);
      if ((net->getFlags() & NETFLAG_UNUSEDNET)) continue;

      ORCADNet *n = new ORCADNet;
      netarray.SetAtGrow(netcnt, n);
      netcnt++;
      n->name = net->getNetName();
      n->net_index = netcnt;

      fprintf(fp, "%s(Net %d (N \"%s\") (Uid %ld)\n", ident, netcnt,
         check_name('n', net->getNetName()), ++cur_uid);

      worcad_plusident();
      worcad_close_b(fp);

      CompPinStruct *compPin;
      POSITION compPinPos;
   
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);
         int compptr = get_compptr(compPin->getRefDes());
         if (compptr < 0)  continue;
         int pinptr = get_comppinptr(compPin->getRefDes(), compPin->getPinName());
         if (pinptr < 0)   continue;
         fprintf(fp,"%s(SEG (Net %d)(P %d %d))\n", ident, netcnt, 
            comparray[compptr]->comp_index, 
            comppininstarray[pinptr]->index);
      }
   }
   return 1;
}

//--------------------------------------------------------------
/*
*/
static int ORCAD_patterndefpin(CDataList *DataList, double scale)
{
   DataStruct *np;
   int         pincnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               Point2 point2;
   
               point2.x = np->getInsert()->getOriginX() * scale;
               point2.y = np->getInsert()->getOriginY() * scale;

               //TransPoint2(&point2, 1, &m, insert_x, insert_y);

               int r = round(RadToDeg(np->getInsert()->getAngle()));
   
               while (r < 0)     r+=360;
               while (r >= 360)  r-=360;

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               ORCADShapePinInst *c = new ORCADShapePinInst;
               shapepininstarray.SetAtGrow(shapepininstcnt,c);  
               shapepininstcnt++;   
               c->padstackname = block->getName();
               c->pinname = np->getInsert()->getRefname();
               c->rotation = r;
               c->pinx = point2.x;
               c->piny = point2.y;
               c->mechanical = FALSE;
               if (r)
               {
                  CString  degreerot, padname;
                  degreerot.Format("%d", r);
                  padname = block->getName();
                  padname += "_";
                  padname += degreerot;

                  if (get_padrot(block->getName(), r) < 0)
                  {
                     ORCADPadRotStruct *p = new ORCADPadRotStruct;
                     padrotarray.SetAtGrow(padrotcnt,p);  
                     padrotcnt++;   
                     p->padname = block->getName();
                     p->newpadname = padname;
                     p->rotation = r;
                  }
                  c->padstackname = padname;
               }

            }
            else
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)
            {
               Point2 point2;
   
               point2.x = np->getInsert()->getOriginX() * scale;
               point2.y = np->getInsert()->getOriginY() * scale;

               //TransPoint2(&point2, 1, &m, insert_x, insert_y);

               int r = round(RadToDeg(np->getInsert()->getAngle()));
   
               while (r < 0)     r+=360;
               while (r >= 360)  r-=360;

               BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

               ORCADShapePinInst *c = new ORCADShapePinInst;
               shapepininstarray.SetAtGrow(shapepininstcnt,c);  
               shapepininstcnt++;   
               c->padstackname = block->getName();
               c->pinname = np->getInsert()->getRefname();
               c->rotation = r;
               c->pinx = point2.x;
               c->piny = point2.y;
               c->mechanical = TRUE;

               if (r)
               {
                  CString  degreerot, padname;
                  degreerot.Format("%d", r);
                  padname = block->getName();
                  padname += "_";
                  padname += degreerot;

                  if (get_padrot(block->getName(), r) < 0)
                  {
                     ORCADPadRotStruct *p = new ORCADPadRotStruct;
                     padrotarray.SetAtGrow(padrotcnt,p);  
                     padrotcnt++;   
                     p->padname = block->getName();
                     p->newpadname = padname;
                     p->rotation = r;
                  }
                  c->padstackname = padname;
               }
            }
         }
         break;                                                                
      } // end switch
   } // end ACCEL_patterndefpin 
   return pincnt;
}

/*****************************************************************************/
/*
*/
static int  get_pcbcomp_pins(double scale)
{
   int   i;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if ( (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT))
      {
         ORCAD_patterndefpin(&(block->getDataList()), scale);
         ORCAD_MakePinNames_Unique(block->getName());
         ORCAD_SortPinData(block->getName());
			int pcnt=0;
         for (pcnt=0;pcnt<shapepininstcnt;pcnt++)
         {
            shapepininstarray[pcnt]->index = pcnt+1;
/*
            fprintf(wfp,"%s(pad (padNum %d) (padStyleRef \"%s\")(pt %1.*lf %1.*lf) (rotation %1.1lf))\n",
               ident, pcnt+1, check_name('p', shapepininstarray[pcnt]->padstackname), 
               output_units_accuracy, shapepininstarray[pcnt]->pinx, 
               output_units_accuracy, shapepininstarray[pcnt]->piny, 
               shapepininstarray[pcnt]->rotation);
*/
            ORCADCompPinInst *c = new ORCADCompPinInst;
            comppininstarray.SetAtGrow(comppininstcnt,c);  
            comppininstcnt++; 
            c->geomnum = block->getBlockNumber();
            c->padstackname = shapepininstarray[pcnt]->padstackname;
            c->pinname = shapepininstarray[pcnt]->pinname;
            c->index =  shapepininstarray[pcnt]->index;
            c->x = shapepininstarray[pcnt]->pinx;
            c->y = shapepininstarray[pcnt]->piny;
            c->rot = shapepininstarray[pcnt]->rotation;

         }

         for (pcnt=0;pcnt<shapepininstcnt;pcnt++)
         {
            delete shapepininstarray[pcnt];
         }
         shapepininstcnt = 0;
      }
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int do_symbol(FILE *wfp, BlockStruct *block, double scale, int mirror, int scnt)
{
   ORCADSym *c = new ORCADSym;
   symarray.SetAtGrow(symcnt, c);
   symcnt++;
   c->name = block->getName();
   if (mirror) c->name += "_M";
   c->block_num = block->getBlockNumber();
   c->sym_index = scnt;
   c->sym_mirindex = scnt;
   c->mirror = (mirror)?MIRROR_ALL:0;
   c->viadef = 0;

   int pcnt = 0;
	int i=0;
   for (i=0;i<comppininstcnt;i++)
   {
      if (comppininstarray[i]->geomnum == block->getBlockNumber())
         pcnt++;
   }
      
   fprintf(wfp,"%s(SYM %d(N \"%s\")\n", ident, scnt, c->name);
   worcad_plusident();

   DataStruct *d;
   double centroidx = 0, centroidy = 0;
   if ((d = centroid_exist_in_block(block)) != NULL)
   {
      // here update position
      centroidx = d->getInsert()->getOriginX() * scale;
      centroidy = d->getInsert()->getOriginY() * scale;
   }

   worcad_symuid(wfp, ++cur_uid, centroidx, centroidy);
   fprintf(wfp,"%s(Derived 0)\n", ident);
 
   // physical / logical pin map
   for (i=0;i<comppininstcnt;i++)
   {
      if (comppininstarray[i]->geomnum == block->getBlockNumber())
      {
         double   x,y;

         if (mirror)
            x = -comppininstarray[i]->x;
         else
            x = comppininstarray[i]->x;
         y = comppininstarray[i]->y;

         if (comppininstarray[i]->rot)
         {
            int ptr = get_rotatedpadstackindex(comppininstarray[i]->padstackname);
#ifdef _DEBUG
   ORCADPadRotStruct *pp = padrotarray[ptr];
   ORCADCompPinInst *cc = comppininstarray[i];
#endif

            fprintf(wfp,"%s(P %d(N \"%s\") %ld %ld %d(ExitRule \"Std\"))\n", ident,
               comppininstarray[i]->index, comppininstarray[i]->pinname,cnv_unit(x), cnv_unit(y),
               (!mirror)?padrotarray[ptr]->padstack_offset_index:padrotarray[ptr]->padstack_mirror_offset_index);
         }
         else
         {
            int ptr = get_padstackindex(comppininstarray[i]->padstackname);
#ifdef _DEBUG
   ORCADPadstack *pp = padstackarray[ptr];
   ORCADCompPinInst *cc = comppininstarray[i];
#endif

            fprintf(wfp,"%s(P %d(N \"%s\") %ld %ld %d(ExitRule \"Std\"))\n", ident,
               comppininstarray[i]->index, comppininstarray[i]->pinname,cnv_unit(x), cnv_unit(y),
               (!mirror)?padstackarray[ptr]->padstack_offset_index:padstackarray[ptr]->padstack_mirror_offset_index);
         }
      }
   }
   worcad_close_b(wfp);
   return symcnt-1;
}

/*****************************************************************************/
/*
*/
static int  worcad_sym(FILE *wfp, double scale)
{
   int   i;

   // here write vias as symbols -- That is needed to connect to POWER and GND planes
   //(SYM 82(N "PD400-83")(Uid -2 0 0 0 0)(Via)(Derived 0)
   //  (P 1(N "FV") 0 0 2(ExitRule "Std")(ForcedThermalRelief))
   //  )
   for (i=0;i<padstackcnt;i++)
   {
      if (!(padstackarray[i]->used_as_via & 2)) continue;
      //(SYM 82(N "PD400-83")(Uid -2 0 0 0 0)(Via)(Derived 0)
      ORCADSym *c = new ORCADSym;
      symarray.SetAtGrow(symcnt, c);
      symcnt++;
      c->name = padstackarray[i]->name;
      c->block_num = padstackarray[i]->block_num;
      c->sym_index = symcnt;
      c->sym_mirindex = symcnt;
      c->mirror = 0;
      c->viadef = TRUE;

      fprintf(wfp,"%s(SYM %d(N \"%s\")\n", ident, c->sym_index, c->name);
      worcad_plusident();
      worcad_symuid(wfp, ++cur_uid, 0 , 0);
      fprintf(wfp,"%s(Via)(Derived 0)\n", ident);
      fprintf(wfp,"%s(P 1(N \"FV\") 0 0 2(ExitRule \"Std\")(ForcedThermalRelief))\n", ident);
      worcad_close_b(wfp);
   }

   int   scnt = symcnt;

   for (i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
      if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         continue;
      
      switch(block->getBlockType())
      {
         case BLOCKTYPE_DRAWING:
         case BLOCKTYPE_GENERICCOMPONENT:
         case BLOCKTYPE_MECHCOMPONENT: 
         case BLOCKTYPE_PCBCOMPONENT:  
         {
            int sptr1 = do_symbol(wfp, block, scale, 0, ++scnt);
            if (CREATE_MIRRORED_COMPONENTS)
            {
               int sptr2 = do_symbol(wfp, block, scale, 1, ++scnt);
               symarray[sptr1]->sym_mirindex = scnt;
               symarray[sptr2]->sym_mirindex = scnt-1;
#ifdef _DEBUG
   ORCADSym *s1 = symarray[sptr1];
   ORCADSym *s2 = symarray[sptr2];
   int r = 0;
#endif
            }

         }
         break;
         default:
         {
            // do nothing
            int sptr1 = do_symbol(wfp, block, scale, 0, ++scnt);
            if (CREATE_MIRRORED_COMPONENTS)
            {
               int sptr2 = do_symbol(wfp, block, scale, 1, ++scnt);
               symarray[sptr1]->sym_mirindex = scnt;
               symarray[sptr2]->sym_mirindex = scnt-1;
#ifdef _DEBUG
   ORCADSym *s1 = symarray[sptr1];
   ORCADSym *s2 = symarray[sptr2];
   int r = 0;
#endif
            }
         }
         break;
      }
   }

   return 1;
}

/*****************************************************************************/
/*
   returns if a mirrored component was found;
*/
static int  worcad_comp(FILE *wfp, FileStruct *file, double scale)
{
   int mir = Orcad_WriteSYMInsertData(wfp, &(file->getBlock()->getDataList()), file->getInsertX() * scale,
                  file->getInsertY() * scale,
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale() * scale, 0,-1);

   if (mir && !CREATE_MIRRORED_COMPONENTS)
   {
      CString tmp;
      tmp = "There are mirrored Components placed on this design\n.";
      tmp +="But in ORCAD.IN the .CREATE_MIRRORED_COMPONENTS is set to N, therefore no mirrored components are created!";
      ErrorMessage(tmp, "Mirrored Components found!");

      fprintf(flog,"%s\n", tmp);
      display_error++;
   }
   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_pcbdesignheader(FILE *wfp, FileStruct *file, double scale)
{
   fprintf(wfp,"%s(pcbDesignHeader\n",ident);
   worcad_plusident();

   doc->validateBlockExtents(file->getBlock());

   double xmax = (file->getBlock()->getXmax() * scale);
   double ymax = (file->getBlock()->getYmax() * scale);

   if (file->getBlock()->getXmin() < 0)   xmax -= (file->getBlock()->getXmin() * scale);
   if (file->getBlock()->getYmin() < 0)   ymax -= (file->getBlock()->getYmin() * scale);

   // if the origin is moved !
   if (file->getInsertX() > 0)          xmax += (file->getInsertX() * scale);
   if (file->getInsertY() > 0)          ymax += (file->getInsertY() * scale);

   fprintf(wfp,"%s(workspaceSize %1.*lf %1.*lf)\n", ident, output_units_accuracy, xmax, output_units_accuracy, ymax);
   fprintf(wfp,"%s(gridDfns\n", ident);
   worcad_plusident();
   fprintf(wfp,"%s(grid \"100.0mil\")\n", ident);
   worcad_minusident(); // fieldSet
   fprintf(wfp,"%s)\n\n", ident);

   fprintf(wfp,"%s(designInfo\n", ident);
   worcad_plusident();

   fprintf(wfp,"%s(fieldSet \"(Default)\"\n", ident);
   worcad_plusident();

   fprintf(wfp,"%s(fieldDef \"Date\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Time\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Author\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Revision\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Title\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Approved By\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Checked By\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Company Name\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Drawing Number\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Drawn By\" \"\")\n", ident);
   fprintf(wfp,"%s(fieldDef \"Engineer\" \"\")\n", ident);

   worcad_minusident(); // fieldSet
   fprintf(wfp,"%s)\n\n", ident);
   
   worcad_minusident(); // designInfo
   fprintf(wfp,"%s)\n\n", ident);

   fprintf(wfp,"%s(solderSwell 10.0)\n", ident);
   fprintf(wfp,"%s(pasteSwell 0.0)\n", ident);
   fprintf(wfp,"%s(planeSwell 10.0)\n", ident);

   worcad_minusident(); // pcbdesignheader
   fprintf(wfp,"%s)\n\n", ident);

   return 1;
}

/*****************************************************************************/
/*
*/
static int  do_layercontents(FILE *wfp, FileStruct *file, double scale)
{
   return 1;
}

/*****************************************************************************/
/*
   return minumum 16
*/
static int get_viacnt()
{
   int   i;
   int   cnt = 0;

   for (i=0;i<padstackcnt;i++)
   {
      if (padstackarray[i]->used_as_via & 2)
         cnt++;
   }

   if (cnt < 16)  cnt = 16;
   return cnt;
}


/****************************************************************************/
/*
*/
static int worcad_header(FILE *fo, FileStruct *file, double scale)
{
   fprintf(fo,"%s(Header\n",ident);
   worcad_plusident();
   fprintf(fo,"%s(UserDiv 60)\n",ident);
   fprintf(fo,"%s(DisplayPrecision 60)\n",ident);
   fprintf(fo,"%s(Origin 0 0)\n",ident);
   fprintf(fo,"%s(InchFactor 0.00001666666666666667)\n",ident);

   doc->validateBlockExtents(file->getBlock());

   double xmin = (file->getBlock()->getXmin() * scale);
   double ymin = (file->getBlock()->getYmin() * scale);

// Drill chart location on lower bottom on extents
   fprintf(fo,"%s(DrlChartLoc %ld %ld)\n",ident, cnv_unit(xmin), cnv_unit(ymin-100));  // less 100 mil

/*
  (Grid 1500)
  (ViaGrid 1500)
  (PlaceGrid 6000 6000)
  (DetailGrid 1500 1500)
  (DotGrid 0 0)
  (View 120750 90000 183000 253500)
  (OffGrid)(MultiVia Off)(Metric Off)(BackupSweep)
  (BackupInterval 10)(TimeUsed 218986)
  (RotateAngle 5400)
  (AngleSnap 1)
  (PinTestPoints Off)
  (DrlChartLoc 240750 195033)
  (DrlChartTextHeight 6000)
  (DrlChartLineWidth 600)
  (DrillSize (Diam 1200)(Symbol 129))
  (DrillSize (Diam 2040)(Symbol 128))
*/
   worcad_close_b(fo); // close from HEADER
   return 1;
}
/****************************************************************************/
/*
*/
static int worcad_graphic_header(FILE *fo, FileStruct *file)
{
   fprintf(fo,"%s(Header\n",ident);
   worcad_plusident();
   fprintf(fo,"%s(UserDiv 60)\n",ident);
   fprintf(fo,"%s(DisplayPrecision 60)\n",ident);
   fprintf(fo,"%s(Origin 0 0)\n",ident);
   fprintf(fo,"%s(InchFactor 0.00001666666666666667)\n",ident);

   worcad_close_b(fo); // close from HEADER
   return 1;
}

/*****************************************************************************/
/*
*/
static int write_padform(FILE *fp, int blocknum, double scale, int rotation)
{
   BlockStruct *block = doc->Find_Block_by_Num(blocknum);
   
   if (block == NULL)   
   {
      fprintf(fp,"(UNDEF)");
      return 1;
   }

   double padxdim, padydim, padxoff, padyoff;
   double block_rot = 0;

   if (block->getFlags() & BL_TOOL) 
   {
      padxdim = block->getToolSize()/2 * scale;
      fprintf(fp,"(Round %ld)", cnv_unit(padxdim));
      return 1;
   }

   switch (block->getShape())
   {
      case T_SQUARE:
         padxdim = padydim = block->getSizeA()/2 * scale;
         padxoff = padyoff = 0;
         fprintf(fp,"(Square %ld)", cnv_unit(padxdim));
      break;
      case T_OCTAGON:
      case T_DONUT:
      case T_ROUND:
         padxdim = padydim = block->getSizeA()/2 * scale;
         padxoff = padyoff = 0;
         fprintf(fp,"(Round %ld)", cnv_unit(padxdim));
      break;
      case T_RECTANGLE:
      {
         double padx1, pady1, padx2, pady2;
         padx1 = (block->getSizeA()/2 +  block->getXoffset()) * scale;
         pady1 = (block->getSizeB()/2 +  block->getYoffset()) * scale;
         padx2 = (block->getSizeA()/2 +  block->getXoffset()) * scale;
         pady2 = (block->getSizeB()/2 +  block->getYoffset()) * scale;
         fprintf(fp,"(Rect %ld %ld %ld %ld)(R %d)", 
            cnv_unit(padx1), cnv_unit(pady1), cnv_unit(padx2), cnv_unit(pady2),
            DegToMin(RadToDeg(block->getRotation()+block_rot)+rotation));
      }
      break;
      case T_OBLONG:
      {
         double padx1, pady1, padx2, pady2;
         padx1 = (block->getSizeA()/2 +  block->getXoffset()) * scale;
         pady1 = (block->getSizeB()/2 +  block->getYoffset()) * scale;
         padx2 = (block->getSizeA()/2 +  block->getXoffset()) * scale;
         pady2 = (block->getSizeB()/2 +  block->getYoffset()) * scale;
         fprintf(fp,"(Oblong %ld %ld %ld %ld)(R %d)", 
            cnv_unit(padx1), cnv_unit(pady1), cnv_unit(padx2), cnv_unit(pady2),
            DegToMin(RadToDeg(block->getRotation()+block_rot)+rotation));
      }
      break;
      default:
         // unknown or complex
         fprintf(flog,"Unevaluated Complex Padform [%s] -> set to 5MIL round\n", block->getName() /*shapes[block->getShape()]*/);
         display_error++;
         padxdim = padydim = 5; // 10 mil to units
         padxoff = padyoff = 0;
         fprintf(fp,"(Round %ld)", cnv_unit(padxdim));
      break;
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int write_padstack(FILE *fp, int pindex, int mirror, double scale, int rotation)
{
   for (int l=1;l<=max_layercnt;l++)
   {
      if (get_layer_stacknum(l) > -1)
      {
         if (mirror)
            fprintf(fp,"%s(L %d ", ident, mirror_layer[l]);
         else
            fprintf(fp,"%s(L %d ", ident, l);
         write_padform(fp, padstackarray[pindex]->layer_app[l], scale, rotation);
         fprintf(fp,")\n");
      }
   }

   return 1;
}

/*****************************************************************************/
/*
(THRU 1(N "JMPVIA_AAAAA")(Uid -964)(TestPt Off)(NonPlated Off)
  (LgThermal Off)(DirectHit Off)
  (L 1(Round 1650))
  (L 2(Round 1650))
  (L 3(Round 2100))
  (L 4(Round 2100))
  (L 5(UNDEF))
  (L 6(UNDEF))
  (L 7(UNDEF))
  (L 8(UNDEF))
  (L 9(UNDEF))
  (L 10(UNDEF))
  (L 11(UNDEF))
  (L 12(UNDEF))
  (L 13(UNDEF))
  (L 14(UNDEF))
  (L 15(UNDEF))
  (L 16(UNDEF))
  (L 17(Round 1950))
  (L 18(Round 1950))
  (L 19(UNDEF))
  (L 20(UNDEF))
  (L 21(UNDEF))
  (L 22(UNDEF))
  (L 23(UNDEF))
  (L 24(UNDEF))
  (L 25(Round 1110))
  (L 26(Round 1110))
  (L 27(UNDEF))
  (L 28(UNDEF))
  (L 29(UNDEF))
  (L 30(UNDEF))
  )
(THRU 2(N "STANDARDVIA")(Uid -965)(TestPt Off)(NonPlated Off)
  (LgThermal Off)(DirectHit Off)
  (L 1(Round 1650))
  (L 2(Round 1650))
  (L 3(Round 1650))
  (L 4(Round 1650))
  (L 5(UNDEF))
  (L 6(UNDEF))
  (L 7(UNDEF))
  (L 8(UNDEF))
  (L 9(UNDEF))
  (L 10(UNDEF))
  (L 11(UNDEF))
  (L 12(UNDEF))
  (L 13(UNDEF))
  (L 14(UNDEF))
  (L 15(UNDEF))
  (L 16(UNDEF))
  (L 17(Round 1950))
  (L 18(Round 1950))
  (L 19(UNDEF))
  (L 20(UNDEF))
  (L 21(UNDEF))
  (L 22(UNDEF))
  (L 23(UNDEF))
  (L 24(UNDEF))
  (L 25(Round 1110))
  (L 26(Round 1110))
  (L 27(UNDEF))
  (L 28(UNDEF))
  (L 29(UNDEF))
  (L 30(UNDEF))
  )
*/
static int worcad_thru(FILE *fp, double scale)
{
   int   i, cnt = 0;
   // write normal
   // write vias, but min 16
   for (i=0;i<padstackcnt;i++)
   {
      if (!(padstackarray[i]->used_as_via & 2)) continue;
      cnt++;
      padstackarray[i]->via_offset_index = cnt;

      fprintf(fp,"%s(THRU %d(N \"%s\")(Uid %ld)(TestPt Off)(NonPlated Off)(LgThermal Off)(DirectHit Off)\n",
         ident, cnt, padstackarray[i]->name, ++cur_uid);
      worcad_plusident();
      write_padstack(fp, i, FALSE, scale,0);
      worcad_close_b(fp);
   }

   // fill up vias to min. 16
   for (i=cnt+1;i<16;i++)
   {
      fprintf(fp,"%s(THRU %d(N \"VIA %d\")(Uid %ld)(TestPt Off)(NonPlated Off)(LgThermal Off)(DirectHit Off)\n",
         ident, i, i, ++cur_uid);
      cnt++;
      worcad_plusident();
      for (int l=1;l<=max_layercnt;l++)
      {
         if (get_layer_stacknum(l) > -1)
            fprintf(fp,"%s(L %d (UNDEF))\n", ident, l);
      }
      worcad_close_b(fp);
   }

   // write padstacks
   for (i=0;i<padstackcnt;i++)
   {
      // if only used as a via, no need to write
      if (padstackarray[i]->used_as_via == 2)   continue;

      // here write all pads !
      cnt++;
      padstackarray[i]->padstack_offset_index = cnt;

      fprintf(fp,"%s(THRU %d(N \"%s\")(Uid %ld)(TestPt Off)(NonPlated Off)(LgThermal Off)(DirectHit Off)\n",
         ident, cnt, padstackarray[i]->name, ++cur_uid);
      worcad_plusident();
      write_padstack(fp, i, FALSE, scale,0);
      worcad_close_b(fp);
   }

   if (CREATE_MIRRORED_COMPONENTS)
   {
      // write mirrored padstacks
      for (i=0;i<padstackcnt;i++)
      {
         // if only used as a via, no need to write
         if (padstackarray[i]->used_as_via == 2)   continue;

         // here write all pads !
         cnt++;
         CString  mname;
         mname.Format("%s_M", padstackarray[i]->name);
         padstackarray[i]->padstack_mirror_offset_index = cnt;
         fprintf(fp,"%s(THRU %d(N \"%s\")(Uid %ld)(TestPt Off)(NonPlated Off)(LgThermal Off)(DirectHit Off)\n",
            ident, cnt, mname, ++cur_uid);
         worcad_plusident();
         write_padstack(fp, i, TRUE, scale,0);
         worcad_close_b(fp);
      }
   }

   for (i=0;i<padrotcnt;i++)
   {
#ifdef _DEBUG
      ORCADPadRotStruct *pp = padrotarray[i];
#endif

      int ptr = get_padstackindex(padrotarray[i]->padname);

      // here write all pads !
      cnt++;
      padrotarray[i]->padstack_offset_index = cnt;

      fprintf(fp,"%s(THRU %d(N \"%s\")(Uid %ld)(TestPt Off)(NonPlated Off)(LgThermal Off)(DirectHit Off)\n",
         ident, cnt, padrotarray[i]->newpadname, ++cur_uid);
      worcad_plusident();


      write_padstack(fp, ptr, FALSE, scale,padrotarray[i]->rotation);
      worcad_close_b(fp);

      // here write all mirrored pads !
      cnt++;
      CString  mname;
      mname.Format("%s_M", padrotarray[i]->newpadname);
      padrotarray[i]->padstack_mirror_offset_index = cnt;
      fprintf(fp,"%s(THRU %d(N \"%s\")(Uid %ld)(TestPt Off)(NonPlated Off)(LgThermal Off)(DirectHit Off)\n",
         ident, cnt, mname, ++cur_uid);
      worcad_plusident();
      write_padstack(fp, ptr, TRUE, scale, padrotarray[i]->rotation);
      worcad_close_b(fp);

   }

   return 1;
}

/****************************************************************************/
/* 
   because multiple layers can be mapped to 1 stacknumber, loop thru all layers
   to find the mirror. (i.e. SIGNAL_TOP and PAD_TOP)
*/
static int find_mirror_layer_from_stacknum(int stacknum)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
#ifdef _DEBUG
      ORCADLayerStruct *ol = orcadArr[i];
#endif
      if (orcadArr[i]->stackNum != stacknum) continue;

      LayerStruct *ll = doc->FindLayer(orcadArr[i]->layerindex);
      if (ll)
      {
         int ptr = get_layerptr_from_index(ll->getMirroredLayerIndex());
         if (ptr > -1 && ptr != i)
         {
#ifdef _DEBUG
      ORCADLayerStruct *ol1 = orcadArr[ptr];
#endif
            int ml = orcadArr[ptr]->stackNum;
            if (ml != stacknum)  return ptr;
         }
      }
   }

   return -1;
}

/****************************************************************************/
/*
   Standard Orcad layer
(L 0(N "Conn")(N "0")(N "0")(Uid -1))
(L 1(N "TOP")(N "TOP")(N "TOP")(Uid -2)(Etch)(Mirror 2)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 2(N "BOTTOM")(N "BOT")(N "BOT")(Uid -3)(Etch)(Mirror 1)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 3(N "GND")(N "GND")(N "PLANE")(Uid -4)(Plane)(TT 720)(TV 720)
  (TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 4(N "POWER")(N "PWR")(N "PLANE")(Uid -5)(Plane)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 5(N "INNER1")(N "IN1")(N "INNER")(Uid -6)(Etch)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 6(N "INNER2")(N "IN2")(N "INNER")(Uid -7)(Etch)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 7(N "INNER3")(N "IN3")(N "INNER")(Uid -8)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 8(N "INNER4")(N "IN4")(N "INNER")(Uid -9)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 9(N "INNER5")(N "IN5")(N "INNER")(Uid -10)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 10(N "INNER6")(N "IN6")(N "INNER")(Uid -11)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 11(N "INNER7")(N "IN7")(N "INNER")(Uid -12)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 12(N "INNER8")(N "IN8")(N "INNER")(Uid -13)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 13(N "INNER9")(N "IN9")(N "INNER")(Uid -14)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 14(N "INNER10")(N "I10")(N "INNER")(Uid -15)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 15(N "INNER11")(N "I11")(N "INNER")(Uid -16)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 16(N "INNER12")(N "I12")(N "INNER")(Uid -17)(Unused)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 17(N "SMTOP")(N "SMT")(N "SMTOP")(Uid -18)(Doc)(Mirror 18)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 18(N "SMBOT")(N "SMB")(N "SMBOT")(Uid -19)(Doc)(Mirror 17)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 19(N "SPTOP")(N "SPT")(N "SPTOP")(Uid -20)(Doc)(Mirror 20)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 20(N "SPBOT")(N "SPB")(N "SPBOT")(Uid -21)(Doc)(Mirror 19)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 21(N "SSTOP")(N "SST")(N "SSTOP")(Uid -22)(Doc)(Mirror 22)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 22(N "SSBOT")(N "SSB")(N "SSBOT")(Uid -23)(Doc)(Mirror 21)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 23(N "ASYTOP")(N "AST")(N "ASYTOP")(Uid -24)(Doc)(Mirror 24)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 24(N "ASYBOT")(N "ASB")(N "ASYBOT")(Uid -25)(Doc)(Mirror 23)
  (TT 720)(TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 25(N "DRLDWG")(N "DRD")(N "DRLDWG")(Uid -26)(Doc)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 26(N "DRILL")(N "DRL")(N "DRILL")(Uid -27)(Drill)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 27(N "FABDWG")(N "FAB")(N "FAB")(Uid -24)(Doc)(TT 720)(TV 720)
  (TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))
(L 28(N "NOTES")(N "NOT")(N "NOTES")(Uid -24)(Doc)(TT 720)
  (TV 720)(TP 720)(VV 720)(VP 720)(PP 720)(Stair 600))

here now put user defined layers


*/
static int worcad_layer(FILE *fo)
{
   int   i, ii;

   for (i=0;i<31;i++)
      mirror_layer[i] = i;

   fprintf(fo,"%s(L 0(N \"Conn\")(N \"0\")(N \"0\")(Uid -1))\n", ident);

   for (i=1;i<=max_layercnt;i++) // maximum 30 orcad layers
   {
      int   written = FALSE;
      for (ii=0;ii<maxArr;ii++)
      {
#ifdef _DEBUG
      ORCADLayerStruct *ol = orcadArr[ii];
#endif
         if (orcadArr[ii]->stackNum != i) continue;
         written = TRUE;
         fprintf(fo,"%s(L %d\n",ident,i);
         worcad_plusident();
         fprintf(fo,"%s(N \"%s\")\n",ident,orcadArr[ii]->newName);   // OEM name
         fprintf(fo,"%s(N \"%s\")\n",ident,orcadArr[ii]->nickName);  // strat layer
         fprintf(fo,"%s(N \"%s\")\n",ident,orcadArr[ii]->libName);   // library layer
         worcad_uid(fo, ++cur_uid);

         // need clearance
         if (orcadArr[ii]->type == 'P')
            fprintf(fo,"%s(Plane)\n",ident);
         else
         if (orcadArr[ii]->type == 'S')
            fprintf(fo,"%s(Etch)\n",ident);
         else
         if (!STRCMPI(orcadArr[ii]->nickName,"DRL"))
         {
            fprintf(fo,"%s(Drill)\n",ident);
         }
         else
         {
            LayerStruct *ll = doc->FindLayer(orcadArr[ii]->layerindex);
            if (ll)
            {
               if (ll->getLayerType() == LAYTYPE_DRILL)
                  fprintf(fo,"%s(Drill)\n",ident);
               else
                  fprintf(fo,"%s(Doc)\n",ident);
            }
            else
               fprintf(fo,"%s(Doc)\n",ident);
         }

         // find a mirror layer for this stacknumber
         int ptr = find_mirror_layer_from_stacknum(i);
         if (ptr > -1 && ptr != ii)
         {  
#ifdef _DEBUG
      ORCADLayerStruct *ol1 = orcadArr[ptr];
#endif
            
            int ml = orcadArr[ptr]->stackNum;
            fprintf(fo,"%s(Mirror %d)\n",ident, ml);
            mirror_layer[orcadArr[ii]->stackNum] = ml;
            mirror_layer[ml] = orcadArr[ii]->stackNum;
         }
         fprintf(fo,"%s(TT 480)(TV 480)(TP 480)(VV 480)(VP 480)(PP 480)\n",ident);
         worcad_close_b(fo); // close from L
         break;
      }

      if (!written)
      {
         // write a dummy layer !
         fprintf(fo,"%s(L %d(N \"LAYER%d\")(N \"%d\")(N \"%d\")(Uid %ld) (Unused))\n", ident, i, i, i, i, ++cur_uid);
      }
   }
   return 1;
}

//--------------------------------------------------------------
static void write_symattributes(CAttributes* map, int mirror, double scale, int symindex)
{
   if (map == NULL)
      return;

   CString  gerber_line;
   WORD     keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      //if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))    continue;
      //if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0)) continue;

      int      flg = 0;
      double   height, width, angle;
      int      lindex;

      if (attrib->isVisible() == 0)
      {
         flg = 1;
         continue;      // do not show an invisble attribute.
      }

      height = attrib->getHeight() * scale;
      width  = attrib->getWidth() * scale;
      angle  = attrib->getRotationRadians();

      // ix, iy
      Point2   point2;
      point2.x = attrib->getX()*scale;
      if (mirror)
         point2.x = -attrib->getX()*scale;
      point2.y = attrib->getY()*scale;

      if (!doc->get_layer_visible(attrib->getLayerIndex(), mirror))
         continue;
      if ((lindex = Layer_ORCAD(doc->get_layer_mirror(attrib->getLayerIndex(), mirror))) < 0)   continue;

      // if an attribute is set to be never mirrored.
      if (attrib->getMirrorDisabled()) mirror = 0;

      double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
      char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));
            
      tok = strtok(temp,"\n");
      while (tok)
      {
         // here do text

         if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))  
         {
            fprintf(wfp,"%s(Text %d (N \"&Comp\")(N \"%d\")(Uid %ld)\n",ident, 
               ++cur_txtcnt, symindex, ++cur_uid);
   
            worcad_plusident();
            if (mirror) fprintf(wfp,"%s(Mirrored)\n",ident);
            fprintf(wfp,"%s(W %ld)(H %ld) (%ld %ld) (Aspect 100)(L %d)(R %d)(Sym %d)(RefDes)\n",ident,
                  cnv_unit(penWidth), cnv_unit(attrib->getHeight()*scale),
                  cnv_unit(point2.x), cnv_unit(point2.y), orcadArr[lindex]->stackNum, DegToMin(RadToDeg(angle)),
                  symindex);
            worcad_close_b(wfp); // end of text
         }
         tok = strtok(NULL,"\n");
         free(temp);
      }
   }

   return;
}

/*****************************************************************************/
/*
*/
static int worcad_shapeobs(FILE *wfp, double scale)
{
   int   i;

   for (i=0;i<symcnt;i++)
   {
      if (symarray[i]->viadef)   continue;

#ifdef _DEBUG
      ORCADSym *s = symarray[i];
#endif
      BlockStruct *block = doc->Find_Block_by_Num(symarray[i]->block_num);
      if (!block)    continue;
      ORCAD_WriteSymData(wfp, &(block->getDataList()), 0.0,  0.0, 0, symarray[i]->mirror, scale, 0,-1,
         symarray[i]->sym_index);

      // here write block attributes
      write_symattributes(block->getAttributesRef(), 0, scale, symarray[i]->sym_index);

   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int worcad_otherobs(FILE *wfp, FileStruct *file, double scale)
{
   ORCAD_WriteNetData(wfp, &(file->getBlock()->getDataList()), file->getInsertX() * scale,
                  file->getInsertY() * scale,
                  file->getRotation(), 
                  file->isMirrored(), 
                  file->getScale() * scale, 0,-1, -1);
   return 1;
}

/****************************************************************************/
/*
   Obsticals consists of :

*/
static int worcad_obs(FILE *fp, FileStruct *file, double scale)
{
   worcad_shapeobs(fp, scale);
   worcad_otherobs(fp, file, scale);
   return 1;
}

/****************************************************************************/
/*
*/
static int worcad_color(FILE *fo)
{
   int   i, ii; 

   fprintf(fo,"%s(Color (default (L 0)(Group 0)(Value 65535)))\n", ident);

   for (i=1;i<=max_layercnt;i++) // maximum 30 orcad layers
   {
      int   written = FALSE;
      for (ii=0;ii<maxArr;ii++)
      {
         if (orcadArr[ii]->stackNum != i) continue;
         LayerStruct *l = doc->getLayerArray()[orcadArr[ii]->layerindex];
         if (l == NULL) continue;

         written = TRUE;
         fprintf(fo,"%s(Color (default (L %d)(Group 0)(Value %ld)))\n", 
            ident, i ,l->getColor());

         break;
      }
      if (!written)
      {
         // write a dummy layer !
      }
   }

   return 1;
}

/****************************************************************************/
/*
*/
static int worcad_begin(FILE *fo, FileStruct *file, double scale)
{
   fprintf(fo,"%s(Begin\n",ident);
   worcad_plusident();
// header
   worcad_header(fo, file, scale);
// layer
   worcad_layer(fo);
// throu must be defined comp and mirrored
   worcad_thru(fo, scale);
// sym must be defined comp and solder side (like zuken)
   worcad_sym(fo, scale);
// comp
   int mir = worcad_comp(fo, file, scale);
// net
   worcad_net(fo, file);
// connection datas in seg format
//   worcad_seg();
// obs + text
   worcad_obs(fo, file, scale);
// color
   worcad_color(fo);
   fprintf(fo,"%s(End)\n",ident);
   worcad_close_b(fo); // close from BEGIN
   
   return 1;
}

/****************************************************************************/
/*
*/
static int worcad_graphic_begin(FILE *fo, FileStruct *file, double scale)
{
   fprintf(fo,"%s(Begin\n",ident);
   worcad_plusident();
// header
   worcad_graphic_header(fo, file);
// layer
   worcad_layer(fo);
// throu must be defined comp and mirrored
   worcad_thru(fo, scale);
// sym must be defined comp and solder side (like zuken)
   worcad_sym(fo, scale);
// comp
   worcad_comp(fo, file, scale);
// obs + text
   worcad_obs(fo, file, scale);
// color
   worcad_color(fo);
   fprintf(fo,"%s(End)\n",ident);
   worcad_close_b(fo); // close from BEGIN
   
   return 1;
}

//--------------------------------------------------------------
void ORCADLTD_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *Format,int pageUnits)
{
   FileStruct  *file;
   double      scale;

   doc = Doc;
   format = Format;
   display_error = FALSE;
   ident = ""; 
   identcnt = 0;

   shapepininstarray.SetSize(10,10);
   shapepininstcnt = 0;

   symarray.SetSize(10,10);
   symcnt = 0;

   comparray.SetSize(10,10);
   compcnt = 0;

   netarray.SetSize(10,10);
   netcnt = 0;

   comppininstarray.SetSize(10,10);
   comppininstcnt = 0;

   padrotarray.SetSize(10,10);
   padrotcnt = 0;

   // open file for writting
   if ((wfp = fopen(filename, "wt")) == NULL)
   {
      CString tmp;

      tmp.Format("Can not open file [%s]", filename);
      ErrorMessage(tmp);
      return;
   }
   
   CString logFile = GetLogfilePath("orcad.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage("Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   CString settingsFile( getApp().getExportSettingsFilePath("orcad.out") );
   load_ORCADsettings(settingsFile);

   CString checkFile( getApp().getSystemSettingsFilePath("orcad.chk") );
   check_init(checkFile);

   scale = Units_Factor(pageUnits, UNIT_MILS);
   output_units_accuracy = 3;
   
   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getBlock() == NULL)  continue;   // empty file

      do_layerlist();

      if (edit_layerlist())
      {
         // here need to get rotated padstacks, so that they can be written in the $PADS section

         get_padstacks(scale);
         get_vianames(&(file->getBlock()->getDataList()));
         get_pcbcomp_pins(scale);

         // maximum 30 layers are allowed
         cur_uid = 0;
         cur_obscnt = 0;
         cur_txtcnt = 0;
         fprintf(wfp,"(MIN\n");
         worcad_plusident();
         fprintf(wfp,"%s(Version 9000)\n",ident);
         fprintf(wfp,"%s(MajorRev 9)(MinorRev 0)\n",ident);
         // number of layers
         fprintf(wfp,"%s(L %d)\n",ident, max_layercnt);
         // number of viatypes
         fprintf(wfp,"%s(V %d)\n",ident, get_viacnt());

         CWaitCursor wait;

         if (file->getBlockType() == BLOCKTYPE_PCB)
         {
            worcad_begin(wfp, file, scale);
         }
         else
         {
            worcad_graphic_begin(wfp, file, scale);
         }
         worcad_close_b(wfp);  // close from MIN
      } // edit_layer
   }

   free_layerlist();

   // it is deleted in patterdef
   shapepininstarray.RemoveAll();

   int i;

   for (i=0;i<padrotcnt;i++)
   {
      delete padrotarray[i];
   }
   padrotarray.RemoveAll();

   for (i=0;i<comppininstcnt;i++)
   {
      delete comppininstarray[i];
   }
   comppininstarray.RemoveAll();
   comppininstcnt = 0;

   for (i=0;i<padstackcnt;i++)
   {
      delete padstackarray[i];
   }
   padstackarray.RemoveAll();
   padstackcnt = 0;

   for (i=0;i<symcnt;i++)
   {
      delete symarray[i];
   }
   symarray.RemoveAll();
   symcnt = 0;

   for (i=0;i<compcnt;i++)
   {
      delete comparray[i];
   }
   comparray.RemoveAll();
   compcnt = 0;

   for (i=0;i<netcnt;i++)
   {
      delete netarray[i];
   }
   netarray.RemoveAll();
   netcnt = 0;

   fclose(wfp);

   if (check_report(flog)) display_error++;
   check_deinit();

   fprintf(flog,"Please check the Clearance Settings after loading into ORCAD Layout\n");
   display_error++;

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

/*****************************************************************************/
/*
*/
static int get_padrot(const char *p, int rot)
{
   int   i;

   for (i=0;i<padrotcnt;i++)
   {
      if (!strcmp(padrotarray[i]->padname, p) && padrotarray[i]->rotation == rot)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
   return -1 if not visible
*/
static int Layer_ORCAD(int l)
{
   int   i;

   for (i=0;i<maxArr;i++)
   {
      if (orcadArr[i]->on == 0)  continue;   // not visible
      if (orcadArr[i]->layerindex == l)
         return i;
   }

   fprintf(flog,"Layer [%s] not translated.\n", doc->getLayerArray()[l]->getName());
   return -1;
}

/******************************************************************************
* clean_text
*/
static CString clean_text(const char *t)
{
   CString tt;

   for (int i=0; i<(int)strlen(t); i++)
   {
      if (t[i] == '"')
         tt += '"';
      tt += t[i];
   }

   return tt;
}

/*****************************************************************************/
/*
   Add polyline must be changed to include voids
*/
static int  add_polyline(FILE *fp, Point2 *points, int cnt, 
                         int polyFilled, int closed, int voidout, double lineWidth)
{
   int   i;
   int   ll = 0;
   double oldx = 0,oldy = 0;
   double cx,cy,r,sa;
   double x1,y1,x2,y2;   

   ll += identcnt;

    for(i=0;i<cnt;i++)  // do not write the last entity, because line is written from i to i+1
   {

      if (ll > 80)   
      {
         fprintf(fp,"\n");
         ll = 0;
         fprintf(fp, "%s", ident);
            ll += identcnt;

      }
      double da = atan(points[i].bulge) * 4;

      if (fabs(da) > BULGE_THRESHOLD)
      {
         x1 = points[i].x;
         y1 = points[i].y;
         x2 = points[i+1].x;
         y2 = points[i+1].y;
         ArcPoint2Angle(x1, y1, x2, y2, da, &cx, &cy, &r, &sa);

         if (!isNetArc)
         {
            ll += fprintf(fp,"(Arc) (%ld %ld) (%ld %ld) (%s)",
               cnv_unit(points[i].x), cnv_unit(points[i].y), 
               cnv_unit(cx), cnv_unit(cy), (da < 0)?"CW":"CCW"); 
         }
         else
         {
            ArcPoint2Angle(x1, y1, x2, y2, da, &cx, &cy, &r, &sa);
            // make positive start angle.
            if (sa < 0) sa += PI2;
            // here arc to poly
            int ii, ppolycnt = 255; // only for an arc 
            // start center
            Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));
            ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));

            for (ii=0;ii<ppolycnt;ii++)
            {
               if (ll > 80)   
               {
                  fprintf(fp,"\n");
                  ll = 0;
                        fprintf(fp, "%s", ident);
                        ll += identcnt;
               }
               
               if (fabs(ppoly[ii].x - oldx) > ORCAD_SMALLNUMBER || fabs(ppoly[ii].y - oldy) > ORCAD_SMALLNUMBER)
               {
                  //fprintf(fp,"%s(pt %1.*lf %1.*lf)\n", ident, 
                  // output_units_accuracy, ppoly[ii].x, output_units_accuracy, ppoly[ii].y);
                  ll += fprintf(fp," (%ld %ld)", cnv_unit(ppoly[ii].x), cnv_unit(ppoly[ii].y));
                  //written++;
                  oldx = ppoly[ii].x;
                  oldy = ppoly[ii].y;
               }
            }
            free(ppoly);
         }
      }
      else
      {
         ll += fprintf(fp," (%ld %ld)", cnv_unit(points[i].x), cnv_unit(points[i].y));
      }
   }
   fprintf(fp,"\n");
   return 1; 
}

//--------------------------------------------------------------
void ORCAD_WriteSymData(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, int symindex)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         lindex;
   int         layer;

   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         if ((lindex = Layer_ORCAD(doc->get_layer_mirror(layer, mirror))) < 0)   continue;

      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL voided     = poly->isVoid();

               Point2   p;
                  
               int vertexcnt = 0;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  vertexcnt++;
               }        
               Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
               double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
               vertexcnt = 0;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p.x = pnt->x * scale;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y * scale;
                  p.bulge = pnt->bulge;
                  if (mirror)
                     p.bulge = -p.bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  // need to check for bulge
                  points[vertexcnt].x = p.x;
                  points[vertexcnt].y = p.y;
                  points[vertexcnt].bulge = p.bulge;
                  vertexcnt++;
               }

               CString  obsname;
               cur_obscnt++;
               obsname.Format("Obs%ld", cur_obscnt);
               fprintf(wfp,"%s(Obs %ld (N \"%s\") (Uid %ld) (Sym %d) (NoNet) (L %d)\n", ident,
                  cur_obscnt, obsname, ++cur_uid, symindex, orcadArr[lindex]->stackNum);
               worcad_plusident();
               fprintf(wfp,"%s(W %ld)(Grid 0)", ident, cnv_unit(lineWidth));

               if (np->getGraphicClass() == GR_CLASS_COMPOUTLINE)
                  fprintf(wfp,"(CompOutline)");

               // Anti-Copper
               // Comp group keepin
               // comp group keepout
               // Comp height keepin
               // comp height keepout
               // copper area
               // copper pour
               // free track
               // insertion outline
               // place outline
               // route keepout
               // route-via keepout
               // via keepout

               if (!add_polyline(wfp, points, vertexcnt, polyFilled, closed, voided, lineWidth))
               {
                  fprintf(flog,"Polyline [%ld] write error!\n", np->getEntityNumber());
                  display_error++;
               }
               worcad_close_b(wfp); // obstical

               free(points);
            }
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            point2.x = (np->getText()->getPnt().x)*scale;
            if (mirror) point2.x = -point2.x;
            point2.y = (np->getText()->getPnt().y)*scale;
            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            double text_rot = rotation + np->getText()->getRotation();
            double penWidth = doc->getWidthTable()[np->getText()->getPenWidthIndex()]->getSizeA()*scale;

            int text_mirror;
            // text is mirrored if mirror is set or text.mirror but not if none or both
            text_mirror = np->getText()->isMirrored();

            cur_txtcnt++;

            fprintf(wfp,"%s(Text %d (N \"%s\") (Uid %ld)",ident, 
               cur_txtcnt, clean_text(np->getText()->getText()), ++cur_uid);
            worcad_plusident();

            if (text_mirror)
               fprintf(wfp,"%s(Mirrored)\n",ident);

            fprintf(wfp,"%s(W %ld)(H %ld) (%ld %ld)(Sym %d)(Aspect 100)(L %d) (R %d)\n",ident,
               cnv_unit(penWidth), cnv_unit(np->getText()->getHeight()*scale),
               cnv_unit(point2.x), cnv_unit(point2.y), symindex,
               orcadArr[lindex]->stackNum, DegToMin(RadToDeg(text_rot)));
            worcad_close_b(wfp); // end of text
         }
         break;
         
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_FIDUCIAL) break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_PIN)   break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_MECHANICALPIN)  break;
            if (np->getInsert()->getInsertType() == INSERTTYPE_TESTPAD)  break;

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

            int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            // if inserting an aperture
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if (doc->get_layer_visible(layer, mirror))
               {
/*
                  //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                  if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
                  {
                     wpfw_Graph_Level(lname);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle());
                  }
*/
               } // end if layer visible
            }
            else // not aperture
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               ORCAD_WriteSymData(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, symindex);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end ORCAD_WriteSymData */

//--------------------------------------------------------------
static void write_compattributes(CAttributes* map, int mirror, double rot, double scale, int compindex)
{
   if (map == NULL)
      return;

   CString  gerber_line;
   WORD     keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      //if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))    continue;
      //if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0)) continue;

      int      flg = 0;
      double   height, width, angle;
      int      lindex;

      if (attrib->isVisible() == 0)
      {
         flg = 1;
         continue;      // do not show an invisble attribute.
      }

      height = attrib->getHeight() * scale;
      width  = attrib->getWidth() * scale;
      angle  = attrib->getRotationRadians(); // rotation is relative

      // ix, iy
      Point2   point2;
      point2.x = attrib->getX()*scale;

      if (mirror)
      {
         point2.x = -attrib->getX()*scale;
      }
      point2.y = attrib->getY()*scale;

      if (!doc->get_layer_visible(attrib->getLayerIndex(), mirror))
         continue;

      if ((lindex = Layer_ORCAD(doc->get_layer_mirror(attrib->getLayerIndex(), mirror))) < 0)   continue;

      // if an attribute is set to be never mirrored.
      if (attrib->getMirrorDisabled()) mirror = 0;

      double penWidth = doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale;
      char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));

      if (mirror)
         angle = -angle;
      
      tok = strtok(temp,"\n");

      while (tok)
      {
         // here do text

         if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))  
         {
            fprintf(wfp,"%s(Text %d (N \"&Comp\")(N \"%d\")(Uid %ld)\n",ident, 
               ++cur_txtcnt, compindex, ++cur_uid);
   
            worcad_plusident();

            if (mirror) fprintf(wfp,"%s(Mirrored)\n",ident);

            fprintf(wfp,"%s(W %ld)(H %ld) (%ld %ld) (Aspect 100)(L %d)(R %d)(Comp %d)(RefDes)\n",ident,
                  cnv_unit(penWidth), cnv_unit(attrib->getHeight()*scale),
                  cnv_unit(point2.x), cnv_unit(point2.y), orcadArr[lindex]->stackNum, DegToMin(RadToDeg(angle)),
                  compindex);

            worcad_close_b(wfp); // end of text
         }

         tok = strtok(NULL,"\n");
         free(temp);
      }
   }

   return;
}

//--------------------------------------------------------------
void ORCAD_WriteNetData(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, int netindex)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;
   POSITION    pos;
   int         lindex, i;
   int         layer;

   isNetArc = FALSE;
   RotMat2(&m, rotation);
   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

        if (np->getDataType() != T_INSERT)
        {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

            if (!doc->get_layer_visible(layer, mirror))
            continue;
         if ((lindex = Layer_ORCAD(doc->get_layer_mirror(layer, mirror))) < 0)   continue;

         netindex = -1;
         Attrib *a =  is_attvalue(doc, np->getAttributesRef(), LAYATT_NETNAME, 2);
         if (a)
         {
            CString  netname;
            netname = get_attvalue_string(doc, a); // can have multiple args
            int ptr = get_netptr(netname);
            if (ptr > -1)
               netindex = netarray[ptr]->net_index;
            else
               netindex = -1;
         }
        }

      switch(np->getDataType())
      {
            case T_POLY:
            {
                CPoly   *poly;
                CPnt *pnt;
                POSITION polyPos, pntPos;

                // loop thru polys
                polyPos = np->getPolyList()->GetHeadPosition();
                while (polyPos != NULL)
                {
                    poly = np->getPolyList()->GetNext(polyPos);
                    if (poly->isHidden())  continue;
                    BOOL polyFilled = poly->isFilled();
                    BOOL closed     = poly->isClosed();
                    BOOL voided      = poly->isVoid();

                    Point2 p;

                    int vertexcnt = 0;

                    pntPos = poly->getPntList().GetHeadPosition();

                    while (pntPos != NULL)
                    {
                        pnt = poly->getPntList().GetNext(pntPos);
                        vertexcnt++;
                    }         

                    Point2 *points = (Point2 *)calloc(vertexcnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
                    double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
                    vertexcnt = 0;
                    pntPos = poly->getPntList().GetHeadPosition();
    
                    while (pntPos != NULL)
                    {
                        pnt = poly->getPntList().GetNext(pntPos);
                        p.x = pnt->x * scale;
                        if (mirror) p.x = -p.x;
                        p.y = pnt->y * scale;
                        p.bulge = pnt->bulge;
                        TransPoint2(&p, 1, &m, insert_x, insert_y);
                        // need to check for bulge
                        points[vertexcnt].x = p.x;
                        points[vertexcnt].y = p.y;
                        points[vertexcnt].bulge = p.bulge;
                        vertexcnt++;
                    }

                    if (netindex > -1 && !polyFilled && !closed && !voided)
                    {
                        //fprintf(wfp,"%s(Seg (Net %d) (L %d) (BW %ld)(Grid 0)\n", 
                        // ident, netindex,orcadArr[lindex]->stackNum, cnv_unit(lineWidth));

                        fprintf(wfp,"%s(Seg (Net %d) (L %d)\n", ident, netindex, orcadArr[lindex]->stackNum, cnv_unit(lineWidth));

                        worcad_plusident();

                        fprintf(wfp,"%s", ident);

                        // Seg structure does not support arcs
                        for(i=0;i<vertexcnt;i++)
                        {
                           //if (points[i].bulge) polyArc = true;
                           double da = atan(points[i].bulge) * 4;
                           if (fabs(da) > BULGE_THRESHOLD)
                           {
                              isNetArc = TRUE;
                           }
                        }
                                                   
                        if (!add_polyline(wfp, points, vertexcnt, polyFilled, closed, voided, lineWidth))
                        {
                           fprintf(flog,"Polyline [%ld] write error!\n", np->getEntityNumber());
                           display_error++;
                        }

                        isNetArc = FALSE;

                        fprintf(wfp,"%s(BW %ld)\n", ident, cnv_unit(lineWidth));

                        worcad_close_b(wfp); // obstical

                        free(points);

                    }
                    else
                    {
                        CString  obsname;
                        cur_obscnt++;
                        obsname.Format("Obs%ld", cur_obscnt);

                        //fprintf(wfp,"%s(Obs %ld (N \"%s\")(Uid %ld)(NoNet)(W %ld)(Grid 0)\n", ident,
                        // cur_obscnt, obsname, ++cur_uid, cnv_unit(lineWidth));

                        fprintf(wfp,"%s(Obs %ld (N \"%s\")(Uid %ld)(W %ld)", ident,
                          cur_obscnt, obsname, ++cur_uid, cnv_unit(lineWidth));

                        //fprintf(wfp,"%s", ident);

                        if (np->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
                        {
                            if (closed)
                                fprintf(wfp,"(L 0) (Keepin)\n");
                            else
                                fprintf(wfp,"(L 0) (Detail)\n");
                        }
                        else 
                        {
                            fprintf(wfp,"(L %d) (Detail)\n", orcadArr[lindex]->stackNum);
                        }
                        // Anti-Copper
                        // Comp group keepin
                        // comp group keepout
                        // Comp height keepin
                        // comp height keepout
                        // copper area
                        // copper pour
                        // free track
                        // insertion outline
                        // place outline
                        // route keepout
                        // route-via keepout
                        // via keepout

                        worcad_plusident();

                        fprintf(wfp,"%s", ident);

                        if (!add_polyline(wfp, points, vertexcnt, polyFilled, closed, voided, lineWidth))
                        {
                            fprintf(flog,"Polyline [%ld] write error!\n", np->getEntityNumber());
                            display_error++;
                        }

                        if (!polyFilled)  fprintf(wfp,"%s(NoFill)", ident);

                        fprintf(wfp,"(Grid 0)(NoNet)\n");

                        worcad_close_b(wfp); // obstical

                        free(points);                 

                    }   // If
                }   // While
            }
            break;  // POLYSTRUCT

         case T_TEXT:
            {
               point2.x = (np->getText()->getPnt().x)*scale;
               if (mirror) point2.x = -point2.x;
               point2.y = (np->getText()->getPnt().y)*scale;
               TransPoint2(&point2, 1, &m,insert_x,insert_y);

               double text_rot = rotation + np->getText()->getRotation();
               double penWidth = doc->getWidthTable()[np->getText()->getPenWidthIndex()]->getSizeA()*scale;

               int text_mirror;
               // text is mirrored if mirror is set or text.mirror but not if none or both
               text_mirror = np->getText()->isMirrored();

               cur_txtcnt++;

               fprintf(wfp,"%s(Text %d (N \"%s\") (Uid %ld)",ident, cur_txtcnt, clean_text(np->getText()->getText()), ++cur_uid);
               worcad_plusident();

               if (text_mirror)
                  fprintf(wfp,"%s(Mirrored)\n",ident);

               fprintf(wfp,"%s(W %ld)(H %ld) (%ld %ld)(Free)(Aspect 100)(L %d) (R %d)\n",ident,
               cnv_unit(penWidth), cnv_unit(np->getText()->getHeight()*scale),
               cnv_unit(point2.x), cnv_unit(point2.y), orcadArr[lindex]->stackNum, DegToMin(RadToDeg(text_rot)));
               worcad_close_b(wfp); // end of text
            }
         break;
         
         case T_INSERT:
            {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            // insert if mirror is either global mirror or block_mirror, but not if both.
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x,insert_y);

                int block_mirror = /*mirror ^*/ np->getInsert()->getMirrorFlags();
                double block_rot = rotation + np->getInsert()->getAngle();

            // this insert was defined in the SYM table

            int ptr = get_symptr(block->getBlockNumber(), 0);

            // has to be first !!!
            if (np->getInsert()->getInsertType() == INSERTTYPE_VIA ||
                np->getInsert()->getInsertType() == INSERTTYPE_FREEPAD)
            {
               Attrib *a =  is_attvalue(doc, np->getAttributesRef(), LAYATT_NETNAME, 2);
               if (a)
               {
                  CString  netname;
                  netname = get_attvalue_string(doc, a); // can have multiple args
                  int ptr = get_netptr(netname);
                  if (ptr > -1)
                     netindex = netarray[ptr]->net_index;
                  else
                     netindex = -1;
               }

               int ptr = get_padstackindex(block->getName());
               if (ptr > -1 && netindex > -1)
               {
                  fprintf(wfp,"%s(Seg (Net %d) (%ld %ld)(V %d))\n",ident, netindex,
                     cnv_unit(point2.x), cnv_unit(point2.y), padstackarray[ptr]->via_offset_index);
               }
            }
            else
            if (ptr > -1)
            {
               int compptr = get_compptr(np->getInsert()->getRefname());
               if (compptr > -1)
               {
                  write_compattributes(np->getAttributesRef(), block_mirror, block_rot,
                     scale * np->getInsert()->getScale(), comparray[compptr]->comp_index);
               }
            }
            else
               if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
               {
               if (ptr < 0)   break;

                    int layer = Get_ApertureLayer(doc, np, block, insertLayer);

                if (doc->get_layer_visible(layer, mirror))
               {
/*
                    //PenNum = Get_Layer_PenNum(np->getLayerIndex(), np->filenum);
                    if ((lname = Get_Layer_Layername(np->getLayerIndex())) != NULL)
                    {
                     wpfw_Graph_Level(lname);
                     wpfw_Graph_Aperture(
                        point2.x,
                        point2.y,
                        block->getSizeA() * scale,
                        block->getSizeB() * scale,
                        block->getShape(), np->getInsert()->getAngle());
                  }
*/
               } // end if layer visible
            }
            else // not aperture
            {
               if (ptr < 0)   break;

               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               ORCAD_WriteNetData(wfp,&(block->getDataList()),
                     point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, netindex);
            } // end else not aperture
         } // case INSERT
         break;

      } // end switch
   } // end for

   return;
} /* end ORCAD_WriteSymData */

//--------------------------------------------------------------
static int get_symptr(int blocknum, int mirror)
{
   int   i;

   for (i=0;i<symcnt;i++)
   {
      if (symarray[i]->block_num == blocknum && symarray[i]->mirror == mirror)
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static int get_netptr(const char *n)
{
   int   i;

   for (i=0;i<netcnt;i++)
   {
      if (!netarray[i]->name.CompareNoCase(n))
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static int get_compptr(const char *c)
{
   int   i;

   if (c == NULL) return -1;

   for (i=0;i<compcnt;i++)
   {
      if (!comparray[i]->name.CompareNoCase(c))
         return i;
   }

   return -1;
}

/*****************************************************************************/
/*
*/
static int get_comppinptr(const char *c, const char *p)
{
   int   i;

   // get the blocknum
   int cptr = get_compptr(c);
   if (cptr < 0)  return -1;

   for (i=0;i<comppininstcnt;i++)
   {
      if (comppininstarray[i]->geomnum == comparray[cptr]->block_num &&
          comppininstarray[i]->pinname.CompareNoCase(p) == 0)
         return i;
   }
   return -1;
}

/****************************************************************************/
/*
*/
int DegToMin(double r)
{
   while (r < 0)  r += 360;
   while (r >= 360)  r -= 360;

   return round (r *60) ;
}

//--------------------------------------------------------------
int Orcad_WriteSYMInsertData(FILE *wfp, CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation, int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer)
{
   Mat2x2   m;
   Point2   point2;
   DataStruct *np;
   POSITION pos;
   int      mirfound = 0;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // if in SYM list, write it
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            int ptr = get_symptr(block->getBlockNumber(), 0);

            if (ptr < 0)               break;
            if (symarray[ptr]->viadef) break;

#ifdef _DEBUG
   ORCADSym *s = symarray[ptr];
#endif

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            if (block_mirror) mirfound++;

            ORCADComp *c = new ORCADComp;
            comparray.SetAtGrow(compcnt, c);
            compcnt++;
            c->name = np->getInsert()->getRefname();
            c->block_num = block->getBlockNumber();
            c->comp_index = compcnt;

            fprintf(wfp,"%s(COMP %d\n",ident,compcnt);
            worcad_plusident();

            fprintf(wfp,"%s(N \"%s\")\n",ident,np->getInsert()->getRefname());
            worcad_uid(wfp, ++cur_uid);
            fprintf(wfp,"%s(%ld %ld)\n",ident,
               cnv_unit(point2.x),cnv_unit(point2.y));

            fprintf(wfp,"%s(SYM %d)\n",ident,
               (block_mirror)?symarray[ptr]->sym_mirindex:symarray[ptr]->sym_index);
            int crot = round(RadToDeg(block_rot));
/*
            if (block_mirror)
            {
               crot = 360 - crot;
               if (crot >= 360)  crot = crot - 360;
               if (crot < 0)     crot = crot + 360;
            }
*/
            fprintf(wfp,"%s(R %d)\n",ident,DegToMin(crot));

            if (CREATE_MIRRORED_COMPONENTS)
            {
               fprintf(wfp,"%s(Mirror %d)\n",ident,
                  (!block_mirror)?symarray[ptr]->sym_mirindex:symarray[ptr]->sym_index);
            }

            if (block_mirror)
               fprintf(wfp,"%s(Mirrored)\n",ident);

            worcad_close_b(wfp);  // COMP
         } // case INSERT
         break;

      } // end switch
   } // end for

   return mirfound;
} /* end Orcad_WriteCompInstData */

//--------------------------------------------------------------
int Get_PADSTACKData(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int drilllayerindex)
{
   Mat2x2 m;
   Point2 point2;
   DataStruct *np;
   int   typ = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();

            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            //if (!doc->get_layer_visible(block_layer, mirror, np->getDataType()))  break;

            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if (block->getFlags() & BL_TOOL) 
            {
               if (drilllayerindex > -1)
                  padstackarray[padstackcnt]->layer_app[drilllayerindex] = block->getBlockNumber();
               break;   // special layer
            }
            LayerStruct *lay = doc->FindLayer(block_layer);

            int l = get_layernr_from_index(block_layer);

            // get LAY_TOP, LAY_BOT, LAY_ALL, LAY_INNER
            if (lay->getLayerType() == LAYTYPE_PAD_ALL || lay->getLayerType() == LAYTYPE_SIGNAL_ALL)
            {
               int i;
               for (i=1;i<=max_signalcnt;i++)
               {
                  if (padstackarray[padstackcnt]->layer_app[i] < 0)
                     padstackarray[padstackcnt]->layer_app[i] = block->getBlockNumber();
               }
            }
            else
            if (lay->getLayerType() == LAYTYPE_PAD_INNER || lay->getLayerType() == LAYTYPE_PAD_INNER)
            {
               //l1 = LAY_INNER;
               int i;
               for (i=3;i<=max_signalcnt;i++)
               {
                  if (padstackarray[padstackcnt]->layer_app[i] < 0)
                     padstackarray[padstackcnt]->layer_app[i] = block->getBlockNumber();
               }
            }
            else
            if (lay->getLayerType() == LAYTYPE_PAD_OUTER || lay->getLayerType() == LAYTYPE_SIGNAL_OUTER)
            {
               if (padstackarray[padstackcnt]->layer_app[1] < 0)
                  padstackarray[padstackcnt]->layer_app[1] = block->getBlockNumber();
               if (padstackarray[padstackcnt]->layer_app[2] < 0)
                  padstackarray[padstackcnt]->layer_app[2] = block->getBlockNumber();
            }
            else
            {
               if (l == -99) // NOT IN SIGLAY LITS
                  break;            
               // here l is the layer stack number
               padstackarray[padstackcnt]->layer_app[l] = block->getBlockNumber();
            }
         } // case INSERT
         break;
      } // end switch
   } // end Get_PADSTACKData */
   return typ;
}

/****************************************************************************/
/*
*/
static int load_ORCADsettings(const char *fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   CREATE_MIRRORED_COMPONENTS = TRUE;

   ARC_ANGLE_DEGREE = 5;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage( tmp,"  ORCAD Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] == '.')
      {
         if (!STRCMPI(lp,".CREATE_MIRRORED_COMPONENTS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue;
            if (lp[0] == 'n' || lp[0] == 'N')
               CREATE_MIRRORED_COMPONENTS = FALSE;
         }

         if (!STRICMP(lp,".ARCSTEPANGLE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            ARC_ANGLE_DEGREE = atof(lp);
            if (ARC_ANGLE_DEGREE < 4)     // 360 / 120 not allowed
            {
               fprintf(flog,".ARCSTEPANGLE too small, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 4;
            }
            else
            if (ARC_ANGLE_DEGREE > 45)    // 360 / 120 not allowed
            {
               fprintf(flog,".ARCSTEPANGLE too large, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 45;
            }
         }

      }
   }

   fclose(fp);
   return 1;
}

// end ORCLTD_OUT.CPP