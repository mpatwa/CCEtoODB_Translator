
/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ?1994-2002. All Rights Reserved.
*/

#include "StdAfx.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/******************************************************************************
*/
CString CCEtoODBApp::getCopyrightString() const
{
   return "Copyright (C) Mentor Graphics Corporation 1995-2012.";
}

/******************************************************************************
*/
CString CCEtoODBApp::getFlowIdString() const
{
   return "CCASM1.5";
}

/******************************************************************************
* AssignVersionString
*  - new version at top
*/
CString CCEtoODBApp::getVersionString() const
{
   CString Version;
   CSupString date("$JustDate:  06/28/12 $");

   // Version = "x.x.x";   // MM/DD/YY - Case # - Feature or File name - Brief Description - INITIALS

   Version = "4.13.23"; // 06/26/12 - DR none - vPlan DFT - Probe numbering algorithm adjusted per Mark's feedback re Offset = 1. Added some error feedback via log. -rcf 
   //Version = "4.13.22"; // 06/26/12 - DR none - Orcad In - Silkscreen text was being dropped, is now retained. -rcf
                          //            DR 412392 - Orcad In - Changed from fixed input buffer size to dynamic input buffer. Now reads files that have no newlines. - rcf
   //Version = "4.13.21"; // 06/26/12 - DR 670329 - Orcad In - Fixed memory damage that leads to crash. -rcf 
   //Version = "4.13.20"; // 06/25/12 - DR 822062 - Protel In - forvPlan - Adjustments for vPlan compatibilify made. -rcf 
   //Version = "4.13.19"; // 06/25/12 - DR none - vPlan DFT - Exporters - Updated six exporters for vPlan compatibility, plus converted a bunch of MessageBox calls to ErrorMessage in random places. -rcf 
   //Version = "4.13.18"; // 06/20/12 - DR none - ODB Out - Orcad_6, Silkscreen layers in geometry inserts in component. -rcf 
   //Version = "4.13.17"; // 06/18/12 - DR 896947 - ODB Out - Fixed astoundingly old and basic error in drill hole export, bug since 2009 at least. -rcf 
   //Version = "4.13.16"; // 06/15/12 - DR none - vPlan DFT - Added parsing of probeNumberOffset to DftSolutions line. -rcf 
   //Version = "4.13.15"; // 06/15/12 - DR none - PADS In - From Vadim: Test pads job: drill is completely wrong. It contains some drawings instead of holes. The drawings should go to drill drawing doc layer instead. -rcf 
   //Version = "4.13.14"; // 06/15/12 - DR 816563 - ODB Out - Layer naming changes, Oops. Fixed damage to panel and board outline that previous change for this DR introduced. -rcf 
   //Version = "4.13.13"; // 06/14/12 - DR none - vPlan DFT - Probe numbering WIP snapshot. Changes in ODB Reader and SuperBoard creator. -rcf 
   //Version = "4.13.12"; // 06/13/12 - DR 816563 - ODB Out - Do not create layers in ODB output that do not have a layer of given type in CCZ. No more generated "misc" layer. -rcf 
   //Version = "4.13.11"; // 06/12/12 - DR 816563 - ODB Out - Layer naming changes, if only one layer of given type use CCZ name, otherwise use traditional name. Output DFT layers a Doc layers. -rcf 
   //Version = "4.13.10"; // 06/05/12 - DR 889248 - MyData out - Change dispense_skip in F8 (PCBCmd_MountPosition) to constant Y, per email from Mark 6/5/2012. -rcf 
   //Version = "4.13.9"; // 05/30/12 - DR 892646 - MyData out - Rotation issues - Additional refinements for Diodes and Capacitors. -rcf 
   //Version = "4.13.8"; // 05/30/12 - DR 892636 - Zuken In - Mods to dr 810019, needed to make copy of padstack not modifiy original, different comps modified padstack in different ways, ended up shared with all. - rcf
   //Version = "4.13.7"; // 05/30/12 - DR 892646 - MyData out - Rotation issues. Take PINNR consideration all the way out, due to unreliable data in this field. Fix bottom side rot. - rcf
   //Version = "4.13.6"; // 05/25/12 - DR 890862 - Takaya Out - Changes to no-value tests, "*" versus "0". - rcf
   //Version = "4.13.5"; // 05/24/12 - DR none - API Licensing - Made PEDIT work same as CPPROBASE and CPRINT for enabling base product feature set. - rcf
   //Version = "4.13.4"; // 05/24/12 - DR 889594 - Stencil Gen - Fixed crash involving bad mem handing for FontProperties in TextStruct operator=. - rcf
   //Version = "4.13.3"; // 05/21/12 - DR 884304 - ODB Out - Process pin inserts for insertTypeFiducial, was just pads, text, and polys. - rcf
   //Version = "4.13.2"; // 05/21/12 - DR 887868 - ODB In - Accept forward slash as valid char in package name. - rcf
   //Version = "4.13.1"; // 05/18/12 - EAC to Jun 2012, 4.11 to 4.12 on CCASM1.4 branch, here we go to 4.13 for CCASM1.5. - rcf
   
   // End of the 4.11 line, 4.12 is CCASM1.4 on branch, jump to 4.13. -rcf
   //Version = "4.11.169"; // 05/18/12 - DR 816563 (related) - ODB Out - Layer naming, now supports keeping CAD layer names as .out file option. - rcf
   //Version = "4.11.168"; // 05/17/12 - DR 839628 - Orcad Read - Support .out file option .LAYER_NAME_FIELD FIRST (or SECOND) to select name for imported layer. - rcf
   //Version = "4.11.167"; // 05/16/12 - DR 881174 - Probe Placement - Same situation for test resources and power inj resources. - rcf
   //Version = "4.11.166"; // 05/16/12 - DR Orcad_8 - Orcad Read - Now applies "origin" command from MIN data to properly position PCB in overall doc space. - rcf
   //Version = "4.11.165"; // 05/15/12 - DR 885718 - CCE read now available as licensed feature in interactive CAMCAD. - rcf
   //Version = "4.11.164"; // 05/11/12 - DR Orcad_10 - ODB Out - Ensure pin names in eda/data do not have spaces in the name. - rcf
   //Version = "4.11.163"; // 05/11/12 - DR 885796 ODB In - Fixed swapped plus/minus tolerance in ODB read. - rcf
   //Version = "4.11.162"; // 05/11/12 - DR 819664 ODB Out - Write .comp_mount_type attribute in components file. Use ccz TECHNOLOGY attrib to find value. - rcf
   //Version = "4.11.161"; // 05/10/12 - Email from Eli - SuperBoard Flipped PCA support - Fixed assumption that panel is present in SuperBoard processing for flipped PCAs. - rcf
   //Version = "4.11.160"; // 05/10/12 - Email from Motti - Unicam Read - Fixed issue that made all pads into rectangles, sometimes with zero height. - rcf
   //Version = "4.11.159"; // 05/02/12 - DR 850650 - Hide task bar button in Silent Running mode. - rcf
                           //          - DR 846573 - ODB++ Out - Adjustments to netlist output. - rcf
   //Version = "4.11.158"; // 04/30/12 - DR 846573 - ODB++ Out - Write netlist file. - rcf
   //Version = "4.11.157"; // 04/27/12 - DR 881174 - Probe Placement - Net Conditions tab, fixed sort order for pin number column. - rcf
   //Version = "4.11.156"; // 04/27/12 - vPlan DFT - SuperBoard - Added Flipped PCA Support. - rcf
   //Version = "4.11.155"; // 04/26/12 - DR 885398 - Gerber Out - Crashing on CCZ that has no width defined at index 0. - rcf
   //Version = "4.11.154"; // 04/25/12 - vPlan DFT - Added support for Flip in SuperBoard creation. - rcf
   //Version = "4.11.153"; // 04/24/12 - DR 847586 - MyData Out - Second shot, apply special case orientation detection for SOT23. - rcf
                           //          - DR 884690 - Yestech AOI - Add support of .PACKAGE_NAME command in out file. - rcf
   //Version = "4.11.152"; // 04/20/12 - Create dirs in log file path upon open if dirs/path does not already exist. - rcf
                           //          - SuperBoard - Adjust outlines, GC BoardOutline becomes GC Normal, GC Panel Outline becomes GC Board Outline. - rcf
   //Version = "4.11.151"; // 04/19/12 - vPlan DFT - Make SuperBoard the only visible file after import, before export is attempted, so it is the file that is exported. - rcf
   //Version = "4.11.150"; // 04/18/12 - vPlan DFT - Export minor adjustments re validating export format. - rcf
   //Version = "4.11.149"; // 04/18/12 - vPlan DFT - Added support for export settings file command line param /EXSETTINGS. - rcf
   //Version = "4.11.148"; // 04/13/12 - DR 792705 - Takaya Out - Revisited old fix, put limiter to jumpers and resistors, was catching a diode. - rcf
   //Version = "4.11.147"; // 04/12/12 - DR 851845 - IPC Out - Rotation handling adjustments. - rcf
   //Version = "4.11.146"; // 04/11/12 - vPlan DFT - Takaya - Fixed crashes (old, GUI related) in Takaya writer. - rcf
   //Version = "4.11.145"; // 04/11/12 - vPlan DFT - Exporters being fixed up to work with vPlan DFT. - rcf
                           //          - vPlan DFT - Agilent 3070 - Fixed up log file, Fixed up settings file, Converted AfxMessageBox to ErrorMessage. - rcf
                           //          - vPlan DFT - Teradyne Z1800 IPL - Fixed up... log, settings, messageBox. - rcf
                           //          - vPlan DFT - Teradyne 228x CKT  - Fixed up...
                           //          - vPlan DFT - Teradyne 228x NAV  - Fixed up...
                           //          - vPlan DFT - Teradyne Spectrum  - Fixed up...
                           //          - vPlan DFT - Agilent i1000      - Fixed up...
                           //          - vPlan DFT - Hioki              - Fixed up...
                           //          - vPlan DFT - Scorpion           - Fixed up...
                           //          - vPlan DFT - Seica PAR NOD      - Fixed up...
                           //          - vPlan DFT - Takaya             - Fixed up...  This one does not work yet due to mandatory settings in popup.
   //Version = "4.11.144"; // 04/09/12 - vPlan DFT - Aerflex CB - Fixed up log file, converted AfxMessageBox to ErrorMessage. - rcf
   //Version = "4.11.143"; // 04/05/12 - vPlan DFT - Activating test exports. - rcf
   
   // Branched at 4.11.142 for CCASM1.3 Updates branch (and for vPlan v11.1.2).
   //Version = "4.11.142"; // 04/04/12 - DR 856583 - Zuken Read - Not getting some component geometry defined right, when not a pin but is on electric layer. - rcf
   //Version = "4.11.141"; // 03/30/12 - DR 847586 - MyData - Fixed rotation failure due to failure to find pin 1, now uses PINNR attrib as backup plan to refname for pin 1 ID. - rcf
   //Version = "4.11.140"; // 03/30/12 - DR 851845 - IPC Netlist Out + In - Now writing Rotation for comp pins in Out, Fixed reading of rotation in SI for In. Also fixed board outline export, did continuation lines wrong. - rcf
   //Version = "4.11.139"; // 03/29/12 - DR 810019 - Zuken Read - Handle polyshapes in pin in footprint, make into complex apertures and put in padstack. - rcf
   //Version = "4.11.138"; // 03/23/12 - DR 831931 - Zuken Read - Pin related polys in wrong place, mirror problems. - rcf
   //Version = "4.11.137"; // 03/22/12 - DR 844415 - ODB Out - Though DR was written against Import, really it was three Export problems involving pad rotations, side, mirroring. - rcf
   //Version = "4.11.136"; // 03/16/12 - DR 854099 - DIRS 10812 by Samrat - eCCFileType (DcaFileTypeTag) enum update to include HyperLynx products fileTypeBoardsim, fileTypeLinesim, fileTypeHLDRC. - rcf
   //Version = "4.11.135"; // 03/14/12 - DR 854463 ODB Out - CCZ Variants now exported to ODB BOMs. - rcf
   //Version = "4.11.134"; // 03/13/12 - DR 851504 - CCZ.IN - Limit MakeInto command processing to Panel and PCB files. - rcf
   //Version = "4.11.133"; // 03/12/12 - DR 851367 - ODB Out - Fixed handling of bad ccz data. Data has duplicate per-refdes component instances. Now output one, log and skip the others. - rcf
   //Version = "4.11.132"; // 03/10/12 - vPlan DFT - DftSolution.xml read for panels is being applied. SuperBoard creation is fundementally in place. - rcf
   //Version = "4.11.131"; // 03/08/12 - DR 872998 - Delete Selected - Got broke in 4.10.098, side effect of AA grid conversion to Ultimate Grid, symbol name collision led to deleting one too many. - rcf
   //Version = "4.11.130"; // 03/07/12 - DR 851244 - ODB Out - Fixed user symbol names, can't match format of ODB standard symbol names. Prefix name with "usersym_". - rcf
   //Version = "4.11.129"; // 03/07/12 - DR 851236 - ODB Out - Fixed invalid numbers in output, caused by PKG geometry that has absolutely no content, so size and outline went berserk. - rcf
   //Version = "4.11.128"; // 03/07/12 - DR 851203 - ODB Out - Poly with zero vertices was crashing poly writer. Polylist with such polys made invalid Surface. - rcf
   //Version = "4.11.127"; // 03/07/12 - DR 851234 - ODB Out - Empty pin name is invalid, ensure non-blank pin name in eda/data and comp+top/components. - rcf
   //Version = "4.11.126"; // 03/06/12 - DR 851207 - ODB Out - Validate net names. Length limit = 64 chars. Char content not entirely sure, is different than other entities. - rcf
                           //          - DR 851214 ODB Out - Text width factor (in text in layers/<layer>/feature file) must be non-zero, positive. - rcf
   
   // CCASM1.3 Released 4.11.125
   //Version = "4.11.125"; // 02/18/12 - DR 806744 - 3070 Out - Outlines - Fixed handling of poly with non-zero bulge on last point. Got rid of jillion popups, instead send messages to log. - rcf
   //Version = "4.11.124"; // 02/17/12 - DR 806744 - 3070 Out - Outlines - Added control to increase max pnts, fixed up usage of single poly outline to be more as-is. - rcf
   //Version = "4.11.123"; // 02/15/12 - DR 850095 - ODB Out - Now resets the list of bom_attribs between exports, was keeping stale settings. - rcf
   //Version = "4.11.122"; // 02/15/12 - DR 850095 - ODB Out - Extra: added support for new .BOM_ATTRIB odb.out settings command. - rcf

   // End of the line for 4.10 versions.
   //Version = "4.10.121"; // 02/14/12 - No DR - Mydata, Cyberoptics, Saki, all BaseAoiExporters - Fixed handling of .OUTPUT_ATTRIB and .OUTPUT_VALUE. - rcf
   //Version = "4.10.120"; // 02/14/12 - DR 850095 - ODB Out - Write BOM values from CCZ attribs, DeviceType, Value, Etc. - rcf
   //Version = "4.10.119"; // 02/13/12 - No DR - About window "FlowID" updated to CCASM1.3. Splash BMPs updated with copyright year 2012. - rcf
                           //          - Exact Access EAC updated to Mar 01, 2012.
   //Version = "4.10.118"; // 02/08/12 - No DR - vPlan DFT Support - Fixed up DFTSolution parsing so CAMCAD can read DFTSolution.cc files fro vPlan. - rcf
   //Version = "4.10.117"; // 02/06/12 - DR 851503 - Fixed crash in Navigator from failure to update pin-to-net map after AA, when AA changes nets due to CreateUnconnectedNet. - rcf
   //Version = "4.10.116"; // 02/06/12 - No DR - Updated copyright year range to 2012, Exact Access EAC to Feb 01, 2012. - rcf
   //Version = "4.10.115"; // 02/02/12 - DR 850639 - ODB Out - Fixed long symbol names, > 64 caused vPlan import crash. - rcf
   //Version = "4.10.114"; // 01/31/12 - DR 848935 - ODB In & Out - BOM DSC. - rcf
   //Version = "4.10.113"; // 01/30/12 - DR 809121 - Hp aka Agilent 3070 Out - Upgrade to use DeviceType ala DataDoctor instead of local set of 5 tables and hierachical local dev type assignment scheme. - rcf
   //Version = "4.10.112"; // 01/26/12 - DR 837025 - Unicam PDW In - Add parse and skip support for two new keywords, %partid (a section) and %nextpartid (apparant name/value pair). - rcf
   //Version = "4.10.111"; // 01/26/12 - DR 847843 - Takaya Out - IC Open - ICOP - Fixed bottom side X for comps that are too small for individual pin output. - rcf
   //Version = "4.10.110"; // 01/20/12 - DR 847786 - AA Crash fixed. Was failing in processing non-zero-width (pen) polys that still ultimately had zero area. Now skips these. - rcf
                           //          - DR 847786 - Accell PCAD In - Fixed use of SmallWidth in places that should be ZeroWidth. - rcf
                           //          - DR 847786 - Accell PCAD In - Fixed trapping of zero dimension apertures for RndRect shape, was not but should be (is now) mapped to nullPad. - rcf
   //Version = "4.10.109"; // 01/20/12 - DR 805368 - Agilent aka HP 5dx Out - Add log entry for component inserts whose blocks have no pins. - rcf
   //Version = "4.10.108"; // 01/13/12 - DR 821512 - ODB In - Fixed import of mirrored panel. Was mirroring insert location (X) when it should not have been. - rcf
                           //          - DR 842552 - Odb Out - Long geometry names made invalid EDA/DATA package names, now > 64 chars gets PkgBlock_%d where %d is geometry block number. - rcf
                           //          - DR 842556 - Odb Out - Fix layer name collision with our catch-all "document" layer, also changed that name to "misc". - rcf
   //Version = "4.10.107"; // 01/04/12 - DR 822367 - ODB Out - Fixed handling of bottom built geoms (layer and geom mirror); characterized by PlacedBottom=true and MirrorGraphic=false. - rcf
                           //          - No DR - ODB In - Fixed pad insert roundoff error, was reading file with atoi should have been atof, caused +/- 1 degree error of pad insert in padstack. -rcf
   //Version = "4.10.106"; // 12/30/11 - DR 830083 - Converted SonyAOI Exporter grid to UG. - rcf
   //Version = "4.10.105"; // 12/24/11 - DR 669263 - Viscomm out - Made outfile command PACKAGE_NAME support any string attribute name. - rcf
   //Version = "4.10.104"; // 12/24/11 - DR 830083 - Converted ProbeStyles grid to UG - Fixed shape and color handling. - rcf
   //Version = "4.10.103"; // 12/23/11 - DR 830083 - Converted ProbeStyles grid to UG. Still have to deal with shape name and color column. - rcf
   //Version = "4.10.102"; // 12/22/11 - DR 830083 - Converted PP net conditions grids to UG, Done With PP Grids !!!. - rcf
   //Version = "4.10.101"; // 12/21/11 - DR 830083 - Converted PP preconditions grids to UG, got net conditions to go. - rcf
   //Version = "4.10.100"; // 12/19/11 - DR 830083 - Converted PP probes grids to UG, one grid in PP to go. - rcf
   //Version = "4.10.099"; // 12/17/11 - DR 830083 - Converted PP results grid to UG, three grids in PP to go. - rcf
   //Version = "4.10.098"; // 12/16/11 - DR 830083 - Converted AA results grid and height analysis grids to UG. - rcf
   //Version = "4.10.097"; // 12/14/11 - DR 830083 - Converted Layer Type (not Layer List) grid to UG. Access is from toolbar button, toolbar is in DD too. - rcf
   //Version = "4.10.096"; // 12/14/11 - DR 830083 - Refactored CDDBaseGrid out of DD into UltimateGrid.cpp for more general access. - rcf
                           //          - ColumnDefMap handling changed, updated Comps page only so far. - rcf
                           //          - Final death for InsertTypesPage, has not been enabled for years, no point in converting this flexgrid. DELETED! - rcf
   //Version = "4.10.095"; // 12/12/11 - DR 834603 - PADS In - Fixed parse error that messed up alpha pin names, bug intro'd in DR 608691 fix, only effected 5.0 format PADS files. - rcf
   //Version = "4.10.094"; // 12/06/11 - DR 806308 - DataDoctor, but really setDoc in PCB Navigator, was losing current DFT Solutions, seen as losing recent settings in AA tabs. - rcf
   //Version = "4.10.093"; // 12/05/11 - DR 831191 - INGUN aka TRi MDA export crash fixed, related to lack of DEVICETYPE attrib on all comps in data. - rcf
   //Version = "4.10.092"; // 12/05/11 - DR 837535 - Change to LOADED flag handling, when no BOM and no PNs present at all, default all to loaded=true. - rcf
   //Version = "4.10.091"; // 11/09/11 - Removed internal ZLib build, now using infra_oss zlib. - rcf
   //Version = "4.10.090"; // 11/02/11 - CCE Decryption - API::Validate function was not properly exposed in API, fixed. Maybe. - rcf
   //Version = "4.10.089"; // 11/01/11 - CCE Decryption - Finally works. - rcf
   //Version = "4.10.088"; // 10/31/11 - CCE Decryption - Snapshot - All done except for minor detail: Decryption doesn't work. - rcf
   //Version = "4.10.087"; // 10/25/11 - CCE Decryption - Snapshot - Basic infrastructure is in place, no actual decrypt yet. Next step: make CFlexFile work for standard CC/CCZ read. - rcf
   //Version = "4.10.086"; // 10/24/11 - WI 20940 - Saki Writer - Fixed bottom side output, added Line Config usage enforcement. - rcf
   //Version = "4.10.085"; // 10/21/11 - WI 20939 - Saki Writer - Fixed panel processing algorithm selection, previous was doubling comp placements. - rcf
   //Version = "4.10.084"; // 10/20/11 - GenCad Reader - Fixed crash caused by name collision between SHAPE and ARTWORK, caused recursive block reference. - rcf
   //Version = "4.10.083"; // 10/17/11 - WI 20115 - Cyberoptics export, but really ODB++ import and fiducials fixed. - rcf
   //Version = "4.10.082"; // 10/17/11 - WI 21401 - Slightly altered, all net names that evaluate to blank are now considered the same net. - rcf
   //Version = "4.10.081"; // 10/17/11 - WI 21401 - Validate net names upon CCZ read, make sure all are non-blank. Make up names for any that are  blank. - rcf
   //Version = "4.10.080"; // 10/14/11 - WI 21083 - ODB++ Write - Apply AutoJoin to Board Outline to try to make closed poly from separate vectors. User control: .OUTLINE_AUTOJOIN Y/N def Y in settings. - rcf
   //Version = "4.10.079"; // 10/12/11 - WI 20598 - Keep license checkout/checkin log files out of project dir, put in temp dir only. - rcf
   //Version = "4.10.078"; // 10/10/11 - WI 20655 - Sample CCZ in WI had comps with duplicate refnames, didn't work with ODB export, now it does. - rcf
   //Version = "4.10.077"; // 10/10/11 - WI 20655 - Make ODB export all visible PCBs to separate jobs, for vPlan consumption. - rcf
                           //        - Protel Layout Read - Was setting wrong file type in created CCZ, was "schematic", supposed to be "layout". - rcf
   
   // CCASM 1.2 Original Release - 4.10.076 --- vPlan v11.0 has 4.10.075
   //Version = "4.10.076"; // 09/27/11 - No DR - Fix crash from reading ccz file with damaged layer data, in layer list, mirror layer reference to a layer that does not exist. - rcf
   //Version = "4.10.075"; // 09/09/11 - Viscom - Applied email from Jesper Lykke feedback changes, comment header line in CAD file, width offset default, rommel commenting. - rcf
   //Version = "4.10.074"; // 09/08/11 - No DR - Agilent 5DX - (1) Use .DEFAULT_SHAPE setting for complex apertures. (2) If size reduction fails to fit in original ap poly then use original extent instead of final reduction attempt size. - rcf
   //Version = "4.10.073"; // 09/07/11 - DR 790211 - OrCad In - Messing with master versus instance outline level, trying to determine if comp geom is defined top or bottom. - rcf
   //Version = "4.10.072"; // 09/07/11 - Takaya - In email from Mark, change "0Ohm" back to "0O". - rcf
   //Version = "4.10.071"; // 09/06/11 - DR 789417 - Variant Manager - Fixed crash in grid edit. Really, just masked crash, need to replace this with Ultimate Grid. - rcf
                           //          - Takaya - Adjustments to log error and Cap Opens output, use comp centroid when outline shrink fails. - rcf
   //Version = "4.10.070"; // 09/06/11 - Probe Placement - GUI, fixed not-uncheckable Target Priorities checkboxes by converting tab to use Ultimate Grid. - rcf
   //Version = "4.10.069"; // 09/03/11 - DR 779515 - Takaya - Added support for Kelvin test (alternative resistor test). - rcf
   //Version = "4.10.068"; // 09/03/11 - DR 792705 - Takaya - Change output for zero-value resistors and jumpers, for all resistors change "O" to "Ohm". - rcf
   //Version = "4.10.067"; // 09/01/11 - DR 794091 - Viscom out - Five line items in DR, mostly supplying fake output to fulfill Viscom reader limitations. - rcf
   //Version = "4.10.066"; // 08/31/11 - WI 20117 - Cyberoptics - Fixed handling of pad rotation in padstack. - rcf
                           //          - WI 20065 - Cyberoptics - Fixed components off pcb, Enforce use of Line Config. - rcf
   //Version = "4.10.065"; // 08/26/11 - AOIs - Fixed log file name. Cyberoptics, Yestech, Omron, and Saki were all writing MyData.log. - rcf
   //Version = "4.10.064"; // 08/26/11 - DR 787338 - 5DX Out - Add changed dup pin names to log file, force log display popup if any pins renamed.  - rcf
   //Version = "4.10.063"; // 08/25/11 - WI 20073 - Yestech exporter - Changed "No Description Attribute" to "----" for default Extension field output.  - rcf
                           //          - DR 793703 - Yestech exporter - Turn off mirror of bottom side X. Change to cyberstyle panel processing, to get rid of duplicate comps in output lists. - rcf
   //Version = "4.10.062"; // 08/25/11 - DR 777288 - Takaya FPT - Fixed rejection, use if(x OR y dim < 8mm) instead of if(x AND y dim < 8mm) to trigger comp centroid output for pins.  - rcf
   //Version = "4.10.061"; // 08/25/11 - DR 769758 - Consolidator - Fixed crash in prev fix, holes in block list. - rcf
   //Version = "4.10.060"; // 08/24/11 - DR 769758 - Consolidator - Fixed flaw in previous rect/oblong optimization, needed to update inserts in all blocks, not just padstack blocks. - rcf
   //Version = "4.10.059"; // 08/24/11 - DR 789450 - Panel Template - Fixed fiducial insert mirror flag, was dropping it so all fids came out on top. - rcf
   //Version = "4.10.058"; // 08/22/11 - DR 792436 - Cadence Allegro Read - Fixed crash, classic null ptr dereference. - rcf
   //Version = "4.10.057"; // 08/11/11 - Changed CCASM1.1 -> CCASM1.2 in AboutBox. - rcf
   //Version = "4.10.056"; // 08/10/11 - DR 791758 - Data Doctor - Fixed cloning of RealPart geometry, was losing non-zero insert (x,y,rot) of Package Body, manifested as bad outline rotation in Data Doc. - rcf
                           //          - ODB++ Read - Fixed crash due to uninitialized member var, manifested when eda/data.Z was open in 7Zip when job import was attempted. - rcf
   //Version = "4.10.055"; // 08/09/11 - Stencil Stepper - Deleted leftover CGridControl references in code, was long ago converted to Ultimate Grid. - rcf
   //Version = "4.10.054"; // 08/05/11 - Yestech AOI - Fixed output file extention, changed .txt to .ycd. - rcf
   //Version = "4.10.053"; // 08/04/11 - Converted more exporters to use getExportSettingsFilePath instead of getUserPath. - rcf
   //Version = "4.10.052"; // 08/04/11 - Viscom out and elsewhere - Fixed typos BAORD -> BOARD, BOARD_THICKNES -> BOARD_THICKNESS. - rcf
   //Version = "4.10.051"; // 08/04/11 - HP5DX Writer - Added .STAGE_SPEED setting in .out to act as default for dialog, extend setting range in dialog from 1..6 to 1..20. - rcf
   //Version = "4.10.050"; // 08/04/11 - Consolidator - Handle normalization of Oblongs in same fashion as Rectangles. - rcf
   //Version = "4.10.049"; // 08/03/11 - DR 788418 - HP 5DX Writer - Used LOADED attrib when 5DX_TEST attrib is not present or is blank. - rcf
   //Version = "4.10.048"; // 08/01/11 - DR 787343 - Juki Writer - Fixed units handling, was mixing ccz page units and Juki output units resulting in garbage X on bottom side. - rcf
   //Version = "4.10.047"; // 07/29/11 - DR 787343 - Juki Writer - Messing with bottom side X coords. - rcf
   //Version = "4.10.046"; // 07/29/11 - DR 776403 - ODB++ Read - BOM read tweaking, for when AFFECTING_BOM in stephdr is not set, to get Loaded flag set properly. - rcf
   //Version = "4.10.045"; // 07/28/11 - DR 776403 - ODB++ Read - More RP Outline tweaking. - rcf
   //Version = "4.10.044"; // 07/27/11 - DR 787338 - HP 5DX - Add suffix to dup comp pin refnames to make them unique in output, in alpha pins mode. - rcf
   //Version = "4.10.043"; // 07/27/11 - DR 776403 - ODB++ Read - Fix RealPart outline generation to be compatible with Data Doctor. - rcf
   //Version = "4.10.042"; // 07/27/11 - ODB++ Read - Fixed mem free for coretrilogy call. - rcf
   //Version = "4.10.041"; // 07/26/11 - ODB++ Read - Work around crash in vpl decryptor, can't free result string mem like instructions say, crashes on XP. - rcf
                           //          - ODB++ Write - Email from Mark, spaces in pcb name in panel stephdr need to be underscores. - rcf
   //Version = "4.10.040"; // 07/22/11 - Aperture Edit Dialog - Fixed messed up Spoke count in GUI. - rcf
   //Version = "4.10.039"; // 07/22/11 - DR ? - Gerber Educator - Set electrical stackup numbers 1 and 2 for Pad Layer Top and Pad Layer Bottom, respectively. - rcf
   //Version = "4.10.038"; // 07/22/11 - DR 782892 - Protel Read - Added .MAKE_INTO_xxx support (xxx = TOOLING, FIDUCIAL, TESTPOINT). - rcf
                           //          - DR 787038 - Fixed crash due to old sourcecadsystems string array that was not being maintained, converted to DCA FileTypeTag usage. - rcf
   //Version = "4.10.037"; // 07/21/11 - DR 778905 - Add support for ccz.in settings file. Support MakeInto Fid/Tooling/Testpoint and Consolidator. - rcf
   //Version = "4.10.036"; // 07/20/11 - DR 777969 - EDIF 200 Read - Fixed validation/acceptance of pin refname values, e.g. was not accepting "A11". - rcf
   //Version = "4.10.035"; // 07/19/11 - DR 777288 - Takaya FPT - Apply size threshold to Cap Opens aka IC Opens test, based on comp body size. - rcf
   //Version = "4.10.034"; // 07/19/11 - DR pending - HP 5DX - Change complex ap output to Rect, supply means for user to select Rect or Circ for default ap shape in .out file. - rcf
                           // 07/18/11 - Stencil Gen - Fixed crash from padstack not having any pads when stencil source layer (or whatever) rule does not find a pad. - rcf
   //Version = "4.10.033"; // 07/18/11 - DR 780579 - Agilent aka HP 3070 Export - Component Outline processing fixed. - rcf
   //Version = "4.10.032"; // 07/15/11 - DR 777229 - Stencil Gen - Put DB lib file interface in SG GUI. - rcf
   //Version = "4.10.031"; // 07/14/11 - DR 777872 - Reports - Test Probe - Enable panel report, same style as Components report. - rcf
   //Version = "4.10.030"; // 07/13/11 - WI 15768 - ODB++ Export - Write ODB Standard Symbols for CCZ Simple Apertures, instead of equivalent of complex apertures for all in ODB. - rcf
   //                                  - Supporting Round, Square, Rectangle, Oblong, Donut, and Octagon. Must have zero offset and rotation.
   //Version = "4.10.029"; // 07/12/11 - The 4.10 version series begins. Keeping the third field rolling along, easier in case we need another 4.9 update. - rcf
   //Version = "4.9.029"; // 07/12/11 - CyberOptics finally working fairly well. MyData, Saki, Omron, Yestech refactor sort of stable. - rcf
   //Version = "4.9.028"; // 07/12/11 - From Mark - Access Analysis - Fixed: AA. (Options) Fixtureless. Access Offset. Click in the offset direction box for change A, N, S, W, E. Invalid argument. - rcf
   //Version = "4.9.027"; // 07/08/11 - DR 780581 - Consolidator - Added GUI to control interactive consolidation, and command line support for OriginalName and pin location Tolerance. - rcf
   //Version = "4.9.026"; // 07/06/11 - DR 780581 - Consolidator - Add special processing of circle/square/rectangle apertures, to promote addional consolidation. - rcf
   //Version = "4.9.025"; // 07/06/11 - DR 777247 - Panelization - Display panel size using number of decimals from settings, instead of full double digits. - rcf
   //Version = "4.9.024"; // 07/06/11 - WI 15762 - ODB++ import/export roundtrip was losing items on Rout layer because of layer type mapping to output. Now retains rout as special case. - rcf
   //Version = "4.9.023"; // 07/05/11 - WI 15763 - ODB++ import/export roundtrip was losing items on Smb layer; Items were pin inserts in board, inserting an aperture. - rcf
   //Version = "4.9.022"; // 06/30/11 - DR 780581 - Consolidator - Better handling of OriginalName comparison, when name is sometimes set and sometimes blank in same CCZ. - rcf
   //Version = "4.9.021"; // 06/22/11 - DR 777828 - Fuji Flexa - Added .out command to set fiducial name, aka fiducial partnumber. - rcf
   //Version = "4.9.020"; // 06/20/11 - vPlan 3.0.1 - Prepared vPlan 3.0.1 release version, disabled CyberOptics and Yestech export access. - rcf
   //Version = "4.9.019"; // 06/15/11 - YESTECH AOI Exporter - First plausible delivery. - rcf
   //Version = "4.9.018"; // 06/15/11 - CyberOpticsAOI Out - Got "Teach" working, minor output adjustments to make it read. - rcf
   //Version = "4.9.017"; // 06/15/11 - YESTECH AOI Exporter begins. - rcf
   //Version = "4.9.016"; // 06/15/11 - CyberOpticsAOI Exporter - First viable candidate. - rcf
   //Version = "4.9.015"; // 06/14/11 - DR 777824 - Agilent i1000, IPL, Spectrum - Bad format was tolerated by vs2003, but not now by vs2008. - rcf
                          //          - CyberOpticsAOI - Units under control, complex apertures supported. - rcf
   //Version = "4.9.014"; // 06/10/11 - CyberOpticsAOI Exporter - Bascially working, but not yet finished.  - rcf
   //Version = "4.9.013"; // 06/06/11 - DR 767608 - Consolidator - by way of Zuken Read - Fixed crash in polylist compare for emtpy polylist.  - rcf
   //Version = "4.9.012"; // 06/02/11 - IPC356 In - Set <File> name to just filename instead of full path to input file. - rcf
   //Version = "4.9.011"; // 06/02/11 - ScicardsEncore In - aka CII - Set <File> name to just filename instead of full path to input file. - rcf
   //Version = "4.9.010"; // 05/31/11 - CyberOptics AOI Exporter - Implementation begins. - rcf
   

   //
   // CCASM1.1 Update 1 release is 4.9.009
   //
   //Version = "4.9.009"; // 05/26/11 - DR 769579 - Zuken Reader - Fixed error in dr707246 fix that caused simple aperture xy and rotation offset to be lost. - rcf
   //Version = "4.9.008"; // 05/26/11 - DR 772133 - Gerber Out - Fixed output layer separation by thickness for panel output, this can never have worked at all. - rcf
   //Version = "4.9.007"; // 05/25/11 - DR 772130 - QD Out - Fixed crash, bad mem management, container class used for temp list trashed real list. - rcf
                          //        - ODB++ Import Blunder - Removed echo of VPL to log file. - rcf
    
   
   //
   // CCASM 1.1 Original release is 4.9.006
   //
   //Version = "4.9.006"; // 04/29/11 - DR 682296 - IPC Out - Give settings control to output pin loc raw instead of offset to center of pad.  - rcf
   //Version = "4.9.005"; // 04/28/11 - DR 749289 - Juki Writer - Fixed bottom side rotation, was wrong to be mirroring centroid rotation in formula.  - rcf
                          // 04/25/11 - DR 741085 - Gerber Writer - Separation by thickness should still keep all holes on original layer too (copy to thickness layer, not move to thickess layer).  - rcf
                          // 04/25/11 - DR 757103 - Teradyne CKT Writer - Was using wrong rotation to get pin location relative to board. - rcf
   //Version = "4.9.004"; // 04/22/11 - DR 748268 - Zuken Reader - Fixed pad transform, was shifting pad wrong direction. - rcf
                          // 04/22/11 - DR 743879 - Zuken Reader - Same issue as 748268. - rcf
                          // 04/21/11 - (alpha7) DR 764390 - ODB++ Reader - Fixed crash when using .EnablePackagePinReport Y, null ptr deref involving layers. - rcf
                          // 04/20/11 - (alpha6) DR 742618 - Exit Process - Failure to close new (unnamed, not loaded from ccz, not imported) file caused exit failure. - rcf
                          // 04/20/11 - (alpha5) DR 762247 - SPEA Writer - Adjusted refnames: Change second field in PARTLIST data to be TP_CompPin_PinName, eg via_64-1 becomes TP_via_64_1. - rcf
                          // 04/20/11 - (alpha4) DR 762247 - SPEA Writer - Added option to output testpoints as parts - rcf
                          // 04/19/11 - (alpha3) No DR - Takaya Writer - From Mark: Can you tweak the COMPHEIGHT field in the Takaya repair file. Round it up and make it a whole number. It is always written in MM. [Okay, done.] - rcf
                          // 04/18/11 - (alpha2) WI 16520 - ODB++ Read - Was missing reversal (mirror) of angle for pads in padstsack from bottom side defined component - rcf
                          // 04/18/11 - (alpha1) DR 765049 - ODB++ Write - Fixed fix for handling of unnamed apertures, was double scaling some rectangles, came out wrong size - rcf
   //Version = "4.9.003"; // 04/16/11 - DR 765049 - ODB++ Write - Fixed handling of unnamed apertures - rcf
                          // 04/15/11 - No DR - Adjustments to lookup of system STP files for Device Typer (uses seven .stp files!) - rcf
                          // 04/14/11 - DR 725207 - Zuken Read - Parser need consider original token in some circumstances, not the one already stripped of quotes - rcf
                          // 04/13/11 - DR 698842 - Allegro Reader - Added support for Octagon pad shape - rcf
                          // 04/12/11 - VI Tech AOI Writer - From Mark: units output should be "inch" not "inches" - rcf
                          // 04/11/11 - Change widt table index from short to int, customer presented CCZ with more widths than fit in short index range, caused bad mem access and bizarre behavior - rcf
                          // 04/04/11 - Saki AOI Writer - Don't need to make refnames unique with suffixes, just disable the action - rcf
                          // 04/04/11 - WI 17626 - ODB++ Write - Upcase PNs on output, ODB++ does not recognize case as difference in PN, this fix combines upper/lower to uppper - rcf
   //Version = "4.9.002"; // 04/04/11 - ODB++ Read - Turned off pin graphics for RP outlines, set OutlineMethod attrib on RPs to Custom - rcf
                          // 04/01/11 - DR 608691 - PADS Read - Handle V5.0 format sample with odd space-delimited section - rcf
                          // 04/01/11 - DR 711841 - PADS Read - Add PADS 9.2 format support - rcf
                          // 04/01/11 - DR 682741 - PADS Read - Fixed header recognition for "250L" mode - rcf
                          // 04/01/11 - DR 672509 - PADS Read - Fixed handling of "PAD 0", previously caused reset and loss of data during parsing - rcf
                          // 03/31/11 - DR 738292 - ODB++ Write - Fixed PCB outline, handle closed single poly withot closed flag set - rcf
                          // 03/31/11 - DR 691215 - Takaya Write - Fixed bad combobox resetting, caused selection shifts - rcf
   //Version = "4.9.001"; // 03/30/11 - DR 738437 - Siemens QD Write - New dq.out file command to support EBENE field output - rcf
   //Version = "4.9.000"; // 03/30/11 - Changed to 4.9 version series - rcf
                          //          - Adjusted title bar for TXP mode, to "vPlan Test and Inspection Engineering" - rcf
                          //          - Takaya writer - fixed bug, not resetting probe output lists, mulitple exports came out wrong - rcf

   // End of the road for 4.8 versions

   //Version = "4.8.096"; // 03/29/11 - No DR, Fixed command line param "input=?", lic changes for vplan broke it - rcf
   //Version = "4.8.095"; // 03/28/11 - DR 758798 - Fixed net conditions csv read for lines with blank devname - rcf
   //Version = "4.8.094"; // 03/26/11 - DR 738656 - Takaya Out - Make 2nd reference point optional - rcf
   //Version = "4.8.093"; // 03/24/11 - DR 640105 - Takaya Out - Add support for .EXPORT_ALL_ACCESS_POINTS Y/N in .out settings - rcf
   //Version = "4.8.092"; // 03/23/11 - vPlan WI 15758 - ODB++ round trip, much betta PIN shapes in eda/data (in package defs) - rcf
   //Version = "4.8.091"; // 03/19/11 - vPlan WI 15666 - File | Save and SaveAs adjustments for vPlan accomodation - rcf
   //Version = "4.8.090"; // 03/19/11 - ODB++ In VPL read is now working pretty well - rcf
   //Version = "4.8.089"; // 03/17/11 - ODB++ In VPL read is basically working, need to fix contour handling and units - rcf
   //Version = "4.8.088"; // 03/14/11 - Omron AOI Exporter, first draft - rcf
   //Version = "4.8.087"; // 03/04/11 - License tweaking - rcf
                          //          - Unicam PDW Reader - Fixed to handled multiple terminals with same name - rcf
   //Version = "4.8.086"; // 03/03/11 - Hioki Writer - Adjustments - rcf
   //Version = "4.8.085"; // 02/17/11 - Hioki Writer - Evolving Hioki writer from clone of Takaya writer - rcf
   //Version = "4.8.084"; // 02/15/11 - Cyberoptics, Hioki, Omron Writers - Added sys overhead, filetype, product codes, etc. Not actual writers yet. - rcf
   //Version = "4.8.083"; // 02/14/11 - Saki AOI Exporter - New! First shot. - rcf
   //Version = "4.8.082"; // 02/03/11 - WI 15760 - ODB++ Import & Export - Documentation layer type handling fixed - rcf
   //Version = "4.8.081"; // 02/02/11 - WI 15765 - ODB++ Import - Fixed drill holes, was creating bogus aperture insert in parallel with drill hole insert - rcf
   //Version = "4.8.080"; // 02/01/11 - WI 15758 - ODB++ Export, fixed pin outlines (EDA file), fixed DCA extent calc func plus local ODB code (2 separate bugs involved) - rcf
   //Version = "4.8.079"; // 01/28/11 - Probe Placement - Fixed crash just opening and closing gui (dumped direct OLE vsflex interface, now using CExtendedFlexGrid wrapper) - rcf
                          //          - Made PCB_Translator product setting work with and without underscore - rcf
                          //          - Changed default product to Professional (was Graphic)
   //Version = "4.8.078"; // 01/27/11 - Probe Placement - Device Typer - Fixed two crashes from bad cstring format statements (worked okay in vs2003!) - rcf
   //Version = "4.8.077"; // 01/25/11 - EDIF_300_In - Fixed crash, produce error message instead and quit import - rcf
                          //          - Investigation of EDIF crash at Mark's request, fixed crash but did not fix import itself. - rcf
   //Version = "4.8.076"; // 01/18/11 - Basic subclass.xml support is in place, read, saves to ccz table, can be viewed in data doc - rcf
                          //          - Exporters fixed up to use tables from CCZ, which in turn is the data from subclass.xml - rcf
   //Version = "4.8.075"; // 01/12/11 - Adjustments to getSettingsFilePath, created getImportSettingsFilePath and getExportSettingsFilePath - rcf
   //Version = "4.8.074"; // 01/07/11 - vPlan/TXP - ODB++ BOM Read implemented - rcf
                          // 01/12/11 - 073F - Cmd line import/export, Fixed initialization of font list - rcf
                          // 01/12/11 - 073G - Fixup ODB++ reader to use new settings-file-path getter - rcf
   //Version = "4.8.073"; // 12/08/10 - vPlan/TXP - Fixed loading of files like default.fnt, re-activated license checking - rcf
   //Version = "4.8.072"; // 12/03/10 - vPlan/TXP - Fixed damage to Silent Running cmd line import/export caused by TXP Test Session changes - rcf
   //Version = "4.8.071"; // 12/02/10 - vPlan/TXP - Changed arrangement of command line params to convey CCZ we'll want to be writing - rcf
   //Version = "4.8.070"; // 11/30/10 - ODB++ Write - Fix up ODB++ font file copy to try two different places, one for classic CAMCAD install and one for vPlan - rcf
   //Version = "4.8.069"; // 11/30/10 - ODB++ Write - Expanded log report to help figure out font issue in automated test (vPlan) - rcf
   //Version = "4.8.068"; // 11/30/10 - ODB++ Write - More fixes for processing zero-extent packages - rcf
   //Version = "4.8.067"; // 11/29/10 - Arrange startup for vPlan usage (aka TXP) to import ODB++ on command line - rcf
   //Version = "4.8.066"; // 10/27/10 - No DR# - ODB++ Write - Increase text xsize output by TextSpacingRatio factor, ODB does not have separate values to text size and spacing like CCZ does - rcf
                          //          -        - ODB++ Read - Do (equal and) opposite of above, to extract "just the char" width from the overall ODB char xsize - rcf
                          //          -        - ODB++ Read - Fixed text data parsing, was keeping delimiter single quotes as part of text data - rcf
   //Version = "4.8.065"; // 10/26/10 - No DR# - ODB++ Write - Sort pins in EDA Data file by pin refname - rcf
   //Version = "4.8.064"; // 10/25/10 - No DR# - ODB++ Write - Fixed PCB outline in "profile", and sst/ssb (and other layers) when ccz layer is not mirrored or is MirrorOnly or NeverMirror - rcf
   //Version = "4.8.063"; // 10/25/10 - No DR# - ODB++ Write - Fixed spaces ingeom names, illegal feature coords in outlines, outlines on silkscreen layers, write BOM file. - rcf
                          //          - Do DR# - Takaya - Fixed Sensor Diamter usage, was failing to apply for Real Park Pkg outlines. -rcf
   //Version = "4.8.062"; // 10/21/10 - No DR# - Fixed TECHNOLOGY attrib propogation to CompPins - rcf
   //Version = "4.8.061"; // 10/20/10 - No DR# - ODB++ Reader - Added postprocess to set LOADED based on presence of PARTNUMBER - rcf
                          //          - ODB++ Reader - Fixed components2 reader, was not looking up component correctly - rcf
   //Version = "4.8.060"; // 10/18/10 - No DR# - Zuken Reader - Added support to parse (and ignore) exitDirection token in toeprint pin (ftf file) - rcf
                          //          - 630462 - HP Agilent 5DX - More processing on aperture size output, to keep it within original pad size - rcf
   //Version = "4.8.059"; // 10/14/10 - No DR# - Zuken Reader - Fixed bottom side insert visible refdes attrib layer - rcf
   //Version = "4.8.058"; // 10/12/10 - dts0100422901 - Stencil Gen - Fixed Stencil Source Layer rule and add Passthru rule - rcf
   //Version = "4.8.057"; // 10/08/10 - dts0100500065 - Gerber Writer - Fixed ragged lines on corners in shape boundaries, input is stencil gen split stencil holes - rcf
                          //          - dts0100630462 - Agilent 5DX - Eliminated dup pins in output for THT, was outputting top and bot at same location, now output only top - rcf
   //Version = "4.8.056"; // 10/06/10 - no specific DR# - vPlan - Accomodate lack of regular install's registry settings for system settings file lookups, default to camcad exe folder instead of cwd - rcf
   //Version = "4.8.055"; // 10/01/10 - no specific DR# - Zuken CR5K reader - Fixed .UseOutlineWidth N mode, was still applying shrink to void polys when it should not e applying shrink at all - rcf
   //Version = "4.8.054"; // 10/01/10 - dts0100718797 - HKP Write - Fixed Board Outline, voids in original outline caused problems in Outline processing, just skip voids, irrelevant to outline anyway - rcf
                          //          - dts0100718545 - Delimited BOM Import - Fixed assignment of Unloaded comp default PARTNUMBER attrib - rcf
                          //          - no DR# - Consolidator - Fixed PinsOnly mode flaws, should allow mismatched datacounts - rcf
   //Version = "4.8.053"; // 09/29/10 - dts0100713526 - Zuken related - Consolidator Improvements - Group consolidation based on geom Original Name, also required setting Original Name in importer - rcf
   //Version = "4.8.052"; // 09/29/10 - dts0100679133 - VBASCII HKP - Stop putting testpoints in %parts in keyin file too, and no more PARTNO attrib on testpoints in layout.hkp - rcf
   //Version = "4.8.051"; // 09/24/10 - dts0100679133 Stop putting testpoints in netlist keyin file - rcf
   //Version = "4.8.050"; // 09/23/10 - dts0100719598 - Geometry Edit - Fixed func key F3 action in geometry edit window - rcf
                          // 09/22/10 - dts0100692603 - Protel PFW Reader - Fixed blind/buried via import - rcf
   //Version = "4.8.049"; // 09/21/10 - dts0100668020 - Database - Change DateTime in all tables from text field to SQl Timestamp (DateTime in msAccess) - rcf
   //Version = "4.8.048"; // 09/20/10 - dts0100635414 - Teradyne CKT - Add .ADAPTOR_NAME command support in ckt.out - rcf
   //Version = "4.8.047"; // 09/20/10 - dts0100712571 - Takaya FPT - Option to use reference pt as origin for comp output - rcf
                          //          - dts0100698760 - Zuken CR5K - Fix hang, Altered processing of void boundaries for shapes with non-zero boundary line width - rcf
   //Version = "4.8.046"; // 09/18/10 - dts0100630462 - Agilent HP 5DX Export - Dissect complex apertures, output each shape as "sub" pin  -rcf
   //Version = "4.8.045"; // 09/13/10 - dts0100707246 - Zuken CR5K - Support instance specific pads from PCF file. -rcf
                          // 09/13/10 - dts0100713526 - Zuken CR5K - Reset extransous _m suffixed geometry names back to original name - rcf
   //Version = "4.8.044"; // 09/08/10 - dts0100713452 - ODB++ Reader - Added support for components3 file - rcf
                          //          - dts0100706765 - ODB++ Reader - Fixed oversize arcs problem in polyllines - rcf
                          //          - dts0100715586 - ODB++ Reader - Dup of 706765, fixed, additional test cases verified - rcf
                          //          - vPlan accomodations put in place re mimize license checkout, consolidate log files, support ODB++ out on cmd line - rcf
   //Version = "4.8.043"; // 08/27/10 - dts0100711306 Fixed crash in Probe Placement - rcf
   //Version = "4.8.042"; // 07/23/10 - Fixed IPC blind via problem Dave found during QA (of CCASM1.0 Update 2) - rcf
   //Version = "4.8.041"; // 07/02/10 - Multiple fixes for CCASM1.0 Update 2 - rcf
                          //          - dts0100688218 - Fixed crash reading CC file - rcf
                          //          - dts0100675993 - Stencil Gen - Fixed crash - rcf
                          //          - dts0100671525 - Stencil Gen - Fixed crash - rcf
                          //          - dts0100654564 - ODB++ Writer - Fixed "invalid argument" error during export - rcf
                          //          - dts0100621011 - Fixes crash in HAF writer, fixed actual root import error in FATF reader - rcf
                          //          - dts0100652764 - Accel/PCAD PCB Reader, fixed duplicate comppin/bad padstack read issue - rcf
                          //          - dts0100682292 - IPC356A Writer - Added correct support for blind/buried vias - rcf
                          //          - dts0100647197 - IPC356A Writer - Same as 682292 - rcf
                          //          - dts0100688254 - IPC356 Writer - Added support for UNplated holes - rcf
                          //          - dts0100651429 - Extend filetype enum for new types added in CCZ Data DLL - rcf
                          //          - No DR#        - Added ODB++ Import and Export to CPPROBASE license - rcf
   //Version = "4.8.040"; // 04/21/10 - dts0100669642 ODB++ Reader fixed comp gen/pin location/padstack creation issues , Takaya writer fixed excess probes gen'd during Rapair file write - rcf
   //Version = "4.8.039"; // 04/12/10 - dts0100669642 Consolidator control enhancement - rcf
                          //          - dts0100671061 - Takaya Write - Fixed formatting - rcf
                          //          - dts0100669596 - Takaya Write - Fixed pin order in some tests - rcf
                          //          - dts0100640431 - ODB++ Reader - Fixed comp geom definition in case when subnet info is bad/missing - rcf


   //--------------------------------------------------------------------------------------------------------------------------------------------

   // CCASM1.0 Update 3

   //Version = "4.8.038.030"; // 10/27/10 - No DR# - ODB++ Write - Increase text xsize output by TextSpacingRatio factor, ODB does not have separate values to text size and spacing like CCZ does - rcf
                          //          -        - ODB++ Read - Do (equal and) opposite of above, to extract "just the char" width from the overall ODB char xsize - rcf
                          //          -        - ODB++ Read - Fixed text data parsing, was keeping delimiter single quotes as part of text data - rcf
   //Version = "4.8.038.029"; // 10/26/10 - No DR# - ODB++ Write - Sort pins in EDA Data file by pin refname - rcf
   //Version = "4.8.038.028"; // 10/25/10 - No DR# - ODB++ Write - Fixed PCB outline in "profile", and sst/ssb (and other layers) when ccz layer is not mirrored or is MirrorOnly or NeverMirror - rcf
   //Version = "4.8.038.027"; // 10/25/10 - No DR# - ODB++ Write - Fixed spaces ingeom names, illegal feature coords in outlines, outlines on silkscreen layers, write BOM file. - rcf
                              //          - Do DR# - Takaya - Fixed Sensor Diamter usage, was failing to apply for Real Park Pkg outlines. -rcf
   //Version = "4.8.038.026"; // 10/21/10 - No DR# - Fixed TECHNOLOGY attrib propogation to CompPins - rcf
   //Version = "4.8.038.025"; // 10/20/10 - No DR# - ODB++ Reader - Added postprocess to set LOADED based on presence of PARTNUMBER - rcf
                              //          - ODB++ Reader - Fixed components2 reader, was not looking up component correctly - rcf
   //Version = "4.8.038.024"; // 10/18/10 - No DR# - Zuken Reader - Added support to parse (and ignore) exitDirection token in toeprint pin (ftf file) - rcf
                              //          - 630462 - HP Agilent 5DX - More processing on aperture size output, to keep it within original pad size - rcf
   //Version = "4.8.038.023"; // 10/14/10 - No DR# - Zuken Reader - Fixed bottom side insert visible refdes attrib layer - rcf
   //Version = "4.8.038.022"; // 10/12/10 - dts0100422901 - Stencil Gen - Fixed Stencil Source Layer rule and add Passthru rule - rcf
   //Version = "4.8.038.021"; // 10/08/10 - dts0100500065 - Gerber Writer - Fixed ragged lines on corners in shape boundaries, input is stencil gen split stencil holes - rcf
                              //          - dts0100630462 - Agilent 5DX - Eliminated dup pins in output for THT, was outputting top and bot at same location, now output only top - rcf
   //Version = "4.8.038.020"; // 10/07/10 - no specific DR# - vPlan - Return non0zero exit code when base license or import license fails, these are fatal errors. vPlan needs to change is we want to do Warning level error codes too.- rcf
   //Version = "4.8.038.019"; // 10/06/10 - no specific DR# - vPlan - Accomodate lack of regular install's registry settings for system settings file lookups, default to camcad exe folder instead of cwd - rcf
   //Version = "4.8.038.018"; // 10/01/10 - no specific DR# - Zuken CR5K reader - Fixed .UseOutlineWidth N mode, was still applying shrink to void polys when it should not e applying shrink at all - rcf
   //Version = "4.8.038.017"; // 10/01/10 - dts0100718797 - HKP Write - Fixed Board Outline, voids in original outline caused problems in Outline processing, just skip voids, irrelevant to outline anyway - rcf
                              //          - dts0100718545 - Delimited BOM Import - Fixed assignment of Unloaded comp default PARTNUMBER attrib - rcf
                              //          - no DR# - Consolidator - Fixed PinsOnly mode flaws, should allow mismatched datacounts - rcf
   //Version = "4.8.038.016"; // 09/29/10 - dts0100713526 - Zuken related - Consolidator Improvements - Group consolidation based on geom Original Name, also required setting Original Name in importer - rcf
   //Version = "4.8.038.015"; // 09/28/10 - dts0100679133 - VBASCII HKP - Stop putting testpoints in %parts in keyin file too, and no more PARTNO attrib on testpoints in layout.hkp - rcf
   //Version = "4.8.038.014"; // 09/24/10 - dts0100679133 - VBASCII HKP - Stop putting testpoints in netlist keyin file - rcf
   //Version = "4.8.038.013"; // 09/23/10 - dts0100719598 - Geometry Edit - Fixed func key F3 action in geometry edit window - rcf
                              // 09/22/10 - dts0100692603 - Protel PFW Reader - Fixed blind/buried via import - rcf
   //Version = "4.8.038.012"; // 09/21/10 - dts0100668020 - Database - Change DateTime in all tables from text field to SQl Timestamp (DateTime in msAccess) - rcf
   //Version = "4.8.038.011"; // 09/20/10 - dts0100635414 - Teradyne CKT - Add .ADAPTOR_NAME command support in ckt.out - rcf
   //Version = "4.8.038.010"; // 09/20/10 - dts0100712571 - Takaya FPT - Option to use reference pt as origin for comp output - rcf
                              //          - dts0100698760 - Zuken CR5K - Fix hang, Altered processing of void boundaries for shapes with non-zero boundary line width - rcf
   //Version = "4.8.038.009"; // 09/18/10 - dts0100630462 - Agilent HP 5DX Export - Dissect complex apertures, output each shape as "sub" pin  -rcf
   //Version = "4.8.038.008"; // 09/13/10 - dts0100707246 - Zuken CR5K - Support instance specific pads from PCF file. -rcf
                              // 09/13/10 - dts0100713526 - Zuken CR5K - Reset extransous _m suffixed geometry names back to original name - rcf
   //Version = "4.8.038.007"; // 09/08/10 - dts0100713452 - ODB++ Reader - Added support for components3 file - rcf
                              //          - dts0100706765 - ODB++ Reader - Fixed oversize arcs problem in polyllines - rcf
                              //          - dts0100715586 - ODB++ Reader - Dup of 706765, fixed, additional test cases verified - rcf
                              //          - vPlan accomodations put in place re mimize oicense checkout, consolidate log files, support ODB++ out on cmd line - rcf
   //Version = "4.8.038.006"; // 08/27/10 - dts0100711306 Fixed crash in Probe Placement - rcf

//--------------------------------------------------------------------------------------------------------------------------------------------

   // CCASM1.0 Update 2

   //Version = "4.8.038.005"; // 07/23/10 - Fixed IPC blind via problem Dave found during QA - rcf
   //Version = "4.8.038.004"; // 07/02/10 - Multiple fixes for CCASM1.0 Update 2 - rcf
                          //          - dts0100688218 - Fixed crash reading CC file - rcf
                          //          - dts0100675993 - Stencil Gen - Fixed crash - rcf
                          //          - dts0100671525 - Stencil Gen - Fixed crash - rcf
                          //          - dts0100654564 - ODB++ Writer - Fixed "invalid argument" error during export - rcf
                          //          - dts0100621011 - Fixes crash in HAF writer, fixed actual root import error in FATF reader - rcf
                          //          - dts0100652764 - Accel/PCAD PCB Reader, fixed duplicate comppin/bad padstack read issue - rcf
                          //          - dts0100682292 - IPC356A Writer - Added correct support for blind/buried vias - rcf
                          //          - dts0100647197 - IPC356A Writer - Same as 682292 - rcf
                          //          - dts0100688254 - IPC356 Writer - Added support for UNplated holes - rcf
                          //          - dts0100651429 - Extend filetype enum for new types added in CCZ Data DLL - rcf
                          //          - No DR#        - Added ODB++ Import and Export to CPPROBASE license - rcf

   //--------------------------------------------------------------------------------------------------------------------------------------------

   // CCASM1.0 Update 1

   //Version = "4.8.038.003"; // 04/21/10 - dts0100669642 ODB++ Reader fixed comp gen/pin location/padstack creation issues , Takaya writer fixed excess probes gen'd during Rapair file write - rcf
   //Version = "4.8.038.002"; // 04/13/10 - dts0100669642 ODB++ Reader, added new control support, part of Consolidator control enhancement, rest of supported readers already got this - rcf
   //Version = "4.8.038.001"; // 04/12/10 - dts0100669642 Consolidator control enhancement - rcf
                          //          - dts0100671061 - Takaya Write - Fixed formatting - rcf
                          //          - dts0100669596 - Takaya Write - Fixed pin order in some tests - rcf
                          //          - dts0100640431 - ODB++ Reader - Fixed comp geom definition in case when subnet info is bad/missing - rcf

   //--------------------------------------------------------------------------------------------------------------------------------------------

   //Version = "4.8.038"; // 12/11/09 - dts0100640949 (related) - ODB++ Reader - Fixed accuracy problem in distinguishing circles from arcs (fixed loss of holes in plane shape) - rcf
   //Version = "4.8.037"; // 12/11/09 - No DR#- ODBWrite - Fixed Recursive looking for Component outline - Sharry
   //Version = "4.8.036"; // 12/10/09 - dts0100638070- ODBWrite - Fixed Component Outlines Not Preserved by rubber band outline - Sharry
                          //        - dts0100640978 - Mentor Reader - Fixed PCB Components mapped as Tooling Holes disappear on import in to CAMCAD - Sharry
                          //        - dts0100638065 - ODBWrite - Fixed Mirrored Boards Incorrect rotation In Panel - Sharry
                          //        - dts0100639704 - Fuji Output - Fixed Fiducial name has to be starting with "P" - Sharry
                          //        - dts0100641000 - Spea4040 Writer - Fixed Missing vias in the metlist section of the SPEA4040 exporter from CAMCAD - Sharry
   //Version = "4.8.035"; // 12/09/09 - No DR# - Change BOM Explorer and DataExchange default location based on CCASM1.0 - Sharry
   //Version = "4.8.034"; // 12/09/09 - dts0100639756 - Takaya Writer -Fixed Pin location and Access Marker location XY same in supplied test case when exporting to Takaya in CAMCAD - Sharry
                          //          - dts0100584188 - DXF Writer - DXF Add Justfy information to Visible Attribute Text - Sharry
                          //          - dts0100638072 - ODB++ Reader - Fixed Attributes with spaces may be problematic - Sharry
                          //          - dts0100638077 - ODB++ Writer - Fixed Tooling holes are not preserved after exporting and reimporting - Sharry
                          //          - dts0100638075 - ODB++ Writer - Fixed Fiducials are not preserved after exporting and reimporting - Sharry
                          //          - dts0100638070 - ODB++ Writer - Fixed Component Outlines Not Preserved - Sharry
                          //          - dts0100638065 - ODB++ Reader - Mirrored Boards Incorrect In Panel - Sharry
   //Version = "4.8.033"; // 12/08/09 - No DR# - VB99 Writer, aka Expedition Writer - Change format selector GUI title bar from "Format VB99 Out" to "Format Expedition Out", per Mark - rcf
   //Version = "4.8.032"; // 12/07/09 - dts0100639714 - Added Missing licenses for these products in CAMCAD base license - Sharry
                          //          - dts0100639704 - Fixed issue with the Fuji Output in CAMCAD - Sharry
   //Version = "4.8.031"; // 12/07/09 - dts0100637573 - Fixed Cells defined with the incorrect Expedition Package Group - Should be Edge Connector - Sharry
                          //          - dts0100637349 - Fixed Cells with pads located on mount side opposite are not properly written to layout.hkp file - Sharry
   //Version = "4.8.030"; // 12/05/09 - CAMCAD About - Add CCASM1.0 to the about - Sharry
   //Version = "4.8.029"; // 12/05/09 - dts0100638057 - ODB++ Writer - Fixed Missing Pad Usage Information - Sharry
                          //          - dts0100638069 - ODB++ Writer - Fixed Error reading file into Valor Viewer - Sharry
   //Version = "4.8.028"; // 12/05/09 - dts0100638055 - ODB++ Writer - Fixed Via XY Location Rounded - Sharry
                          //          - dts0100638055 - ODB++ Writer - Fixed Via Pad Size Incorrect - Sharry
   //Version = "4.8.027"; // 12/01/09 - dts0100638011 - TMentor Neutral Reader - Fixed crash from out of memory, caused by inifinite loop caused by panel and pcb having same geometry name (so got mixed up) - rcf
   //Version = "4.8.026"; // 12/01/09 - dts0100637407 - Teradyne Spectrum Writer - Fixed Probe numbering isn't correct for Teradyne Nailreq imported probes - Sharry
   //Version = "4.8.025"; // 11/30/09 - Command Line Import/Export - Corrected typo in logfile message, intput -> input - rcf
   //Version = "4.8.024"; // 11/25/09 - dts0100637056 - CCZ file I/O - Fixed Backslash is not being converted in CCZ file I/O properly. - Sharry
   //Version = "4.8.023"; // 11/20/09 - dts0100617711 - Expedition Writer - Fixed Wrong statements are written to the key in netlist file Created by CCE to ODB++ PCB Translator - Sharry
                          //          - dts0100634284 - Expedition Writer - Fixed Text exported to Expedition has wrong coordinates - Sharry
                          //          - dts0100633206 - Accel Reader - Fixed TECHNOLOGY Attribute for a SMD part is reported as "THRU" in imported Accel PCAD file - Sharry
                          //          - dts0100633570 - AQuery window - Fixed With Query window open when a file is closed and another is imported, selecting an item crashes CAMCAD - Sharry
   //Version = "4.8.022"; // 11/20/09 - dts0100633558 - Takaya - Added messages to log file so user has hint why some comps end up not tested - rcf
   //Version = "4.8.021"; // 11/16/09 - dts0100633843 - Takaya - Fixed activation of current DFT solution (was crash) and content of two OPTO test modes - rcf
   //Version = "4.8.020"; // 11/16/09 - dts0100626545 - Fuji Flexa Write - Fixed Fuji Flexa Write causing CAMCAD to crash - Sharry
                          //          - dts0100628395 - Custom Assembly Output - Fixed Custom Assembly Output hangs when no machines are defined in CAMCAD - Sharry
   //Version = "4.8.019"; // 11/13/09 - dts0100626545 - Zuken Reader - Fixed CompArea-A (layer) outlines not in correct position for some geometries - Sharry
   //Version = "4.8.018"; // 11/12/09 - dts0100628654 - Zuken Reader - Fixed project space blank after reading this customer file - Sharry
   //Version = "4.8.017"; // 11/12/09 - dts0100628654 - Zuken Reader - Fixed loss of panel items represented as pcb components - rcf
                          //          - dts0100629471 - Zuken Reader - Same cause as dts0100600232 already fixed in 4.8.004 by Sharry - rcf
   //Version = "4.8.016"; // 11/10/09 - dts0100626856 - Agilent 3070 Writer - Fixed unconnected nets aren't written to 3070 BXY file (in NODE Alternates section) - Sharry
                          //          - dts0100624646 - Fixed API Licensing shows Gerber and other base importers as licensed but no import dialog pops up - Sharry
                          //          - dts0100624383 - GenCAD reader issue - Fixed mirrored outlines not on proper layer in CAMCAD - Sharry
                          //          - dts0100624289 - Mentor BoardStation - geoms_ascii contains polygon shapes in PIN definitions but after import into CAMCAD padstacks are empty - Sharry
                          //          - dts0100624600 - Huntron HAF output - Fixed issue with incorrect surface value written - Sharry
                          //          - dts0100624198 - Accel import - Fixed using the default pattern is not working - Sharry
                          //          - dts0100622262 - Zuken CR5000 Reader - Fixed bond wire issue no seperation between BwIsGenerated=YES vs. BwIsGenerated=NO - Sharry
                          //          - dts0100625860 - EDIF import - Fixed in rotated or mirrored symbols, the pin properties are not displayed correctly. - Sharry
   //Version = "4.8.015"; // 11/04/09 - ODB++ Writer - Fixed crash for apertures with shape = T_UNDEFINED - rcf
   //Version = "4.8.014"; // 11/03/09 - port_lib.cpp, change default for GetExportFormatByString from fail to success, to allows PADS to use ODB++ Writer through API - rcf
   //Version = "4.8.013"; // 10/30/09 - Command Line Import/Export - Minor adjustment to licensing check - rcf
   //Version = "4.8.012"; // 10/28/09 - dts0100602491 - Fixed On Inner 2 in the attached is a Sternpunkt (Starpoint) component. It's Rotation / Mirroring is wrong. - Sharry
   //Version = "4.8.011"; // 10/28/09 - dts0100601901 - Fixed Pinlables are on wrong position. - Sharry
                          //          - dts0100584188 - Fixed CAMCAD looses Text Justify information on some Load pathways - Sharry  
                          //          - dts0100581993 - Fixed CAMCAD DXF export of rotated Text Blocks error - Sharry  
                          //          - dts0100388737 - Fixed EDIF 200 import issue - Handling TEXT properly - Sharry  
                          //          - dts0100388766 - FixedEDIF 200 Import CAMCAD needs to handle Port Name - Sharry  
   //Version = "4.8.010"; // 10/27/09 - dts0100622329 - Fixed: DRC List dialog chops off data above the comment in CAMCAD - rcf
   //Version = "4.8.009"; // 10/27/09 - dts0100626598 - Command Line Import/Export - Restrict CAMCAD CMD option to only work for specified readers - rcf
                          //          - dts0100626609 - Command Line Import/Export - Fixed Schematic Netlist reader interfaces (all 5, same problem) - rcf
                          //          - dts0100626614 - Command Line Import/Export - CR3000/5000 PWS import, fixed NULL progress bar dereference  - rcf
                          //          - dts0100626607 - ODB++ CCZ command line translation fixed (needed dir from /input without expanding to all files) - rcf
                          //          - dts0100626601 - Expedition ASCII command line CCZ translation fixed (needed dir from /input without expanding to all files) - rcf
   //Version = "4.8.008"; // 10/21/09 - Command Line Import/Export - Fixed CPCMD license handling - rcf
   //Version = "4.8.007"; // 10/15/09 - Command Line Import/Export - Fixed crash in 3 readers (progress bar problems), Orcad, VeribestEIF, and Fabmaster FATF - rcf
   //Version = "4.8.006"; // 10/14/09 - Allegro Reader - Cleaned up two error messages, caption and message were out of order, camcad.cpp: Open log file earler to capture early error messages - rcf
   //Version = "4.8.005"; // 10/13/09 - Add support for Command Line Import/Export feature, CPCMD = 100594 - rcf
                          // 10/13/09 - Set Exact Access Date = Nov 01 2009 - rcf
   //Version = "4.8.004"; // 10/12/09 - Add Family to the Packages table of the database and Packages tab and add RealPart_PackageFamily attribute to the package insert - Sharry
                          // 10/12/09 - dts0100569334 - Add Comment and UserName to the Packages, Centroids and Parts table of the database - Sharry
                          // 10/12/09 - dts0100591118 - Add a checkbox option in the grid so Clear existing BOM attributes before importing - Sharry
                          // 10/12/09 - dts0100588558 - Fixed Panel Template - mirror=Y as default (should be N) - Sharry
                          // 10/12/09 - dts0100600232 - Fixed Zuken CR5000 files causing CAMCAD to crash near completion of import process - Sharry
                          // 10/12/09 - dts0100597751 - Fixed Lost Refname attributes when changing variants in CAMCAD - Sharry
                          // 10/12/09 - dts0100596916 - Add an option to Color by Attribute to include Geometry name in CAMCAD - Sharry
   //Version = "4.8.003"; // 10/09/09 - Many Importers - Convert MessageBox() calls to ErrorMessage(), part of silencing popups and redirecting messages to log file for Import/Export Command Line feature - rcf
   //Version = "4.8.002"; // 10/04/09 - Update Version in Version Source and Release folder to 4.8 - Sharry
   //Version = "4.8.001"; // 10/04/09 - Data Doctor - Centroids Tab - Always send update notification, attributes may have been loaded even if no centroids were loaded - rcf
                          // Command Line Import/Export - Adjusted log file content per demo review, added /logfile=file.ext parameter support - rcf 
   //Version = "4.8.000"; // 10/01/09 - Support CAMCAD build in VS2008, Fixed all errors and warning messages - Sharry
                          //          - Add new exporter for Valor ODB ++ - Sharry
                          //          - Add new exporter for MYDATA assembly machine - Sharry
                          //          - Add new exporter for Fuji Flexa assembly machine - Sharry
                          //          - Add new exporter for Custom Template assembly machine - Sharry
                          //          - Support for machine origin - Sharry
                          //          - Support for Advanced Packaging and other new CCZ features - Sharry
                          //          - Support Command Line Import/Export by Rick 
   ////////////////////////////////////////////////////////////////////////////////////////////////////
   //Version = "4.7.586"; // 10/01/09 - Command Line Import/Export - Added /logfile=name command line param support  - rcf
   //Version = "4.7.585"; // 09/28/09 - Incorporated part of feedback from Demo of Centroids tab and AutoImportExport (Lib tab label, usage and echo command in log file)  - rcf
   //Version = "4.7.584"; // 09/28/09 - Merged Update 4 and 3PCS branches together, then to this CAMCAD_4_8 branch - rcf
   //Version = "4.7.583.3PCS.4"; // 09/24/09 - AutoImportExport mode WIP - Found way to centrally silence most if not all Progress Bars - rcf
   //Version = "4.7.583.3PCS.3"; // 09/24/09 - AutoImportExport mode WIP - Support dirs for input, misc other stuff - rcf
   //Version = "4.7.583.3PCS.2"; // 09/23/09 - AutoImportExport mode WIP - rcf
   //Version = "4.7.583.3PCS.1"; // 09/22/09 - Merged 4.7.570, aka Update 4 - rcf
   //Version = "4.7.583"; // 09/05/09 - dts0100608367 - Zuken Reader - Fixed text rotation issue - rcf
                          //          - dts0100602077 - Stencil Generator - SMD/THT Tree list view issue, fixed by backing out fix for dts594978 - rcf
                          //          - dts0100608368 - Stencil Generator - Paste in hole option not working, fixed by backing out fix for dts594978 - rcf
                          //          - dts0100608994 - Stencil Generator - Copper source rule not working, fixed by backing out change for dts594978 - rcf
   //Version = "4.7.582"; // 08/25/09 - dts???? - PCB Translator - VB99 aka Expedition Exporter - Fixed crash caused by ridiculous memory (mis)management that caused data size explosion of more than 1000X during processing of sample - rcf
   //Version = "4.7.581"; // 08/18/09 - dts0100608406 - Zuken Reader - Fixed crash (Invalid Argument message then hang) in ShouldConsiderOutlineWidth() when poly has single vertex - rcf
   //Version = "4.7.580"; // 08/05/09 - dts0100517091 - GenCad In - (Original DR fix had flaw) Fixed UNITS parsing logic error that caused error report in log file when no actual error occurred - rcf
   //Version = "4.7.579"; // 08/04/09 - dts0100603044 - Added missing button to Attribute Visibility dialog (Tools | Attributes | Attribute Visibility) - rcf
   //Version = "4.7.578"; // 07/29/09 - Tools | FileList - Made FileList dialog resizable - rcf
   //Version = "4.7.577"; // 07/28/09 - Poly3D - Support to Read/Write/Retain/Scale poly3D data to/from CC/CCZ file - Sharry
   //Version = "4.7.576"; // 07/27/09 - Generic Tables - Optimized table-write-to-xml for speed - rcf
   //Version = "4.7.575"; // 07/17/09 - Advanced Packaging - PCB Navigator - Added Bond Pad support in Net tree view, treated essentially like vias - rcf
   //                                 - PCB Navigator - Fixed bug in manual probe placement, was mixing degrees and radians, result was odd angles in probe insert - rcf
   //Version = "4.7.574"; // 07/17/09 - Advanced Packaging - Added Die icon for use in PCB Navigator, to give visual separation in comp tree between regular comps and Die comps - rcf
   //Version = "4.7.573"; // 07/16/09 - Advanced Packaging - Fixed up PCB Navigator to fundementally work with AdvPkg stuff - rcf
   //Version = "4.7.572"; // 07/16/09 - Advanced Packaging - Added new blockTypeBondPad, as per adjustment to AdvPkg Spec - rcf
   //Version = "4.7.571"; // 07/08/09 - Advanced Packaging - Added new Insert, Block, Graphic, and Layer types (enums) as per AdvPkg Spec - rcf
   //    ---    4.7.570  ==  CAMCAD 4.7 Update 3    ---
   //Version = "4.7.570"; // 06/30/09 - DcaNet.cpp/H, CADIF Importer - Fixed problem involving renaming of nets after already in netlist, was not updating map, result was creation of multiple nets with same name - rcf
   //Version = "4.7.569"; // 06/25/09 - dts0100596922 :- Un-connected nets ignored in Takaya output, modified TakayaFPTWrite.cpp - bkalyan
   //Version = "4.7.568"; // 06/24/09 - Fixed crash caused by double use of DataStruct generated in Graph_ComplexTool - rcf
   //Version = "4.7.567"; // 06/24/09 - dts0100594663 - Need to allocate probe numbers in Takaya export from CAMCAD so that numbers are consistent across nets, Problem with earlier fix - bkalyan
   //Version = "4.7.566"; // 06/23/09 - dts0100594978 - Stencil Generator - Fixed bad handling of subpin layers for bottom side stencils, visible symptom was misaligned stencils for bottom subpins - rcf
   //Version = "4.7.565"; // 06/23/09 - dts0100588780 - EDIF 2.0 and 3.0 import:  In rotated or mirrored symbols, the pin numbers...., modified Edif200In.cpp - bkalyan
   //Version = "4.7.564"; // 06/23/09 - dts0100593812 - Schematic Netlist From Layout - Set PortName map from DEVICE_TO_PACKAGE_PINMAP value, move refname to top instead of center - rcf
   //Version = "4.7.563"; // 06/23/09 - dts0100563238 - Delimited BOM Import - Add window register entry for unloaded component partnumber string in delimited BOM Import, modified BomCSV.cpp/BomCSV.h - Sharry
   //Version = "4.7.562"; // 06/22/09 - dts0100594663 - Need to allocate probe numbers in Takaya export from CAMCAD so that numbers are consistent across nets, modified TakayaFPTWrite.cpp and TakayaFPTWrite.h - bkalyan
   //Version = "4.7.561"; // 06/22/09 - dts0100588572 - PADS ASCII Import - Fixed mutliple part labels(attributes) result in both visible attributes being defined wrong in CAMCAD, modified DcaAttributes.cpp - Sharry
   //Version = "4.7.560"; // 06/22/09 - dts0100588264 - Zuken CR5000 read... corner of poly not rounded but it should be (outline width command does this), modified ZukenCr5000Reader.cpp and ZukenCr5000Reader.h - bkalyan
   //Version = "4.7.559"; // 06/19/09 - dts0100594989 - CR5000 files don't import into CAMCAD successfully but it's not known why, modified ZukenCr5000Reader.cpp and ZukenCr5000Reader.h - bkalyan
   //Version = "4.7.558"; // 06/19/09 - dts0100594619 - Stencil Stepper - Change tooltip of the color set, tittle of layer and show pads in the selected layer - Sharry
                          // 06/19/09 - dts0100595268 - BOM Attribute import - Fixed component not honoring LOADED=FALSE attribute in ATR file - Sharry
   //Version = "4.7.557"; // 06/18/09 - dts0100558613 - Zuken CR5000 - need to do better with text definitions from *.PCF File,
                        // dts0100572810 - Zuken CR5000 Read - TEXT in Geometry not oriented correctly, modified DcaText.cpp, ZukenCr5000Reader.cpp and ZukenCr5000ReaderTokens.h - bkalyan 
   //Version = "4.7.556"; // 06/18/09 - dts0100594088 - Out of memory error reading this HKP design with USE_GENERATED_DATA set to Y (vbascii.in command), modified Vb99In.cpp - bkalyan
   //Version = "4.7.555"; // 06/18/09 - dts0100560196 - Add support for mixed board panels in the Zuken CR5000 reader - bkalyan
   //Version = "4.7.554"; // 06/17/09 - dts0100558605f - Stencil Generator - SLOW - Improved SG initial startup speed, not whole fix for this DR but atleast an improvement - rcf
   //Version = "4.7.553"; // 06/16/09 - dts0100588913 - Fixture Reuse - Fixed test surface error for Fabmaster format (double sided) fixture files - rcf
   //Version = "4.7.552"; // 06/16/09 - dts0100593891 - PADS ASCII Write - Fixed issue with a data line that has no coordinates in PADS ASCII file, modified Pads_Out.cpp - Sharry
                          // 06/16/09 - dts0100594912 - PADS Importer - Fixed CAMCAD can't read PADS 9.0 ASCII files, modified PadsIn.cpp/PadsIn.h - Sharry
                          // 06/16/09 - dts0100590729 - Boardstation read - Fixed out of memory error when importing this reasonable sized file. modified MentorIn.cpp - Sharry
   //Version = "4.7.551"; // 06/16/09 - dts0100589646 - Expedition ASCII import - padstack not properly named or created so cell/geometry has no pins, modified VB99In.cpp, Graph.cpp and Graph.h - bkalyan
   //Version = "4.7.550"; // 06/16/09 - dts0100585625 - Juki Write - Changes to Juki output file format in CAMCAD, modified Juki.cpp - Sharry
   //Version = "4.7.549"; // 06/16/09 - dts0100586263 - CR5000 read - missing trace segments that are in Zuken and in Gerber but not in CAMCAD, modified ZukenCr5000ReaderTokens.h and ZukenCr5000Reader.cpp - bkalyan
   //Version = "4.7.548"; // 06/15/09 - dts0100588913 - Fixture Reuse - Added support for double-wired probes from fixture files (two probes on same access marker) - rcf
   //Version = "4.7.547"; // 06/15/09 - dts0100587970 - Vision tests in Takaya output when electrical test is present from CAMCAD - bkalyan
   //Version = "4.7.546"; // 06/15/09 - dts0100594164 - Errors in Takaya .asc file creation from CAMCAD- bkalyan
   //Version = "4.7.545"; // 06/15/09 - dts0100594619 - Stencil Stepper -  Add Stencil Top/Bottom layer types to the Stencil Step dialog and Gerber Write supports Stencil Step option- Sharry
   //Version = "4.7.544"; // 06/12/09 - dts0100594084 - Access Analysis - Fixed special situation for vias and CAD Padstack rule - rcf
   //Version = "4.7.543"; // 06/12/09 - dts0100594073 - Create Pin Pitch attribute automatically on starting Stencil Generator - rcf
   //Version = "4.7.542"; // 06/12/09 - Data Doctor - Centroids Tab - Reworked DB interface to get control of units for XY - rcf
   //Version = "4.7.541"; // 06/10/09 - Schematic Netlist Importer - Merged from branch rfalters_schematicNetlist - rcf
   //Version = "4.7.540"; // 06/10/09 - dts0100592726 - Neutral Import - Fixed some via instances and testpoints coordinates are not an exact match - Sharry
                          // 06/10/09 - dts0100518884 - PADS Reader - Fixed CAMCAD cannot correctly display the data of PADS in Power Plane Layer - Sharry
   //Version = "4.7.539"; // 06/10/09 - dts0100582886 - Stencil Generator - Fixed final rotation for bottom/mirrored stencil holes when rotation symmetry rule is used - rcf
   //Version = "4.7.538"; // 06/09/09 - dts0100570229 - Access Analysis - Fixed height analysis, problem with COMPHEIGHT attrib propagation to Real Parts - rcf
   //Version = "4.7.537"; // 06/08/09 - No DR# - Fixed text scale/transforamtion func, was losing mirror flag setting (e.g. when Settings -> Page Units was changed) - rcf
   //Version = "4.7.536"; // 06/04/09 - dts0100592152 - netlist report - Fixed the NC netnames aren't exported to netlist report after generating NC_comp.pin netnames - Sharry
                          // 06/04/09 - dts0100592152 - UniCAM PDW read - Support PDW file version 1 2 0 - Sharry
                          // 06/04/09 - dts0100592152 - Gerber Write - Fixed geometry level text mirrored to Bottom is not written to proper position in Gerber output - Sharry
   //Version = "4.7.535"; // 05/28/09 - Fixed crash when reading CCZ with VariantItem that has blank name, such items are now discarded (not valid) - rcf
   //Version = "4.7.534"; // 05/26/09 - Fixed crash in machine selection and recalculate the location of Access Markers in the DFT solution for RestructureFiles() - Sharry
   //Version = "4.7.533"; // 05/26/09 - Improvements made in RestructureFiles(), round two, more fixes to mirror handling - rcf
   //Version = "4.7.532"; // 05/26/09 - Generic Tables - Fixed pair of bugs Samrat found, one in parser section one in GUI, manifested when some items had NO table list - rcf
   //Version = "4.7.531"; // 05/26/09 - Improvements made in RestructureFiles(), in section that updates inserts of changed files, still a bug handling mirror in all sitations - rcf
   //Version = "4.7.530"; // 05/19/09 - dts0100558677 - Added layerTypeComponentOutlineBottom = 57 to layer types enum - rcf
                          //          - Added support for row names to gTables <Row name="anyname"> -rcf
                          //          - Added support for gTable List to <File> in cc xml -rcf
                          //          - Updated Table GUI with table list selector - rcf
   //Version = "4.7.529"; // 05/18/09 - dts0100558767 - Added the new font params support for text and visible attribs - rcf
   //Version = "4.7.528"; // 05/15/09 - dts0100587968 - Data Doc, UGCell.cpp, Fixed locale issue seen w/ German language setting - rcf
   //Version = "4.7.527"; // 05/14/09 - Added "Tables" option to "Tools" menu, for read-only display of Table List - rcf
   //Version = "4.7.526"; // 05/14/09 - dts0100587647 - Added mirror-flip support for visible attributes - rcf
                          //          - Foundation for Generic Tables is in place, supported at global CCDoc level only for now - rcf  
   //Version = "4.7.525"; // 05/12/09 - No DR# - Added insertTypeRouteTarget, blockTypeRouteTarget, graphicClassBuspath, graphicClassRegion - rcf
   //Version = "4.7.524"; // 04/21/09 - No DR# - Added ComplexDrillHole and CompositeComponent to View | Show Insert Types menu - rcf
   //Version = "4.7.523"; // 04/21/09 - dts0100566423 - ODB importer - Fixed cannot select freepad feature when using Bottom View in CAMCAD by converting freepads to vias, modified OdbppIn.cpp/OdbppIn.h - Sharry
   //Version = "4.7.522"; // 04/21/09 - No DR# - Added missing insert and block types for ComplexDrillHole and CompositeComp to respective enum-tag-to-display-string functions - rcf
   //Version = "4.7.521"; // 04/20/09 - dts0100576238 - Juki Writer - Write Bad Board(XOut) Information and adjust the circuit layout offset, Juki.cpp/Juki.h - Sharry
   //Version = "4.7.520"; // 04/16/09 - dts0100576238 - Juki Writer - Insufficient information written to Juki output from CAMCAD Professional - Sharry
   //Version = "4.7.519"; // 04/16/09 - dts0100580823 - Board Station Import - Fixed CAMCAD hangs when importing this Board Station data set, modified LyrManip.cpp - Sharry
   //Version = "4.7.518"; // 04/15/09 - dts0100580857 - About Box - Updated copyright date to 2009, support web site name, made company and support text link to open web pages hyperlink style - rcf
   //Version = "4.7.517"; // 04/14/09 - Complex Aperture Analysis and Simplification - CADIF and ZUKEN readers, changed conversion action default from Y to N - rcf
   //Version = "4.7.516"; // 04/13/09 - Complex Aperture Analysis and Simplification - More adjustments to data conversion for scanner lib consumption, worked around problems using poly->convertToOutline - rcf
   //Version = "4.7.515"; // 04/10/09 - Complex Aperture Analysis and Simplification - Fixed conversion problem, conversion factor was too big, apparantly caused (silent) integer overflow, bad results for some pads in mm doc - rcf
   //Version = "4.7.514"; // 04/09/09 - Complex Aperture Analysis and Simplification - Added access from tools menu, at Tools | PCB | Convert Drawn Pads to Filled Outlines - rcf
   //Version = "4.7.513"; // 04/09/09 - dts0100547858 - PADSPCB_CAMCAD Link - API licensing scheme for IPC and Flat DXF output generates errors for XP and Vista and fails under Vista - bkalyan
                        // dts0100563281 - PADSPCB_DXF Flat - You cannot export to Flat DXF format under Windows Vista - bkalyan
                        // dts0100563283 - PADSPCB_IPC - Export to IPC356 format fails under Windows Vista, modified CAMCAD.cpp and CAMCAD's post install script - bkalyan
   //Version = "4.7.512"; // 04/08/09 - Complex Aperture Analysis and Simplification using Scanner Engine aka SysGl - rcf
   //Version = "4.7.511"; // 04/08/09 - dts0100565677 - Fixed qd.out file in CAMCAD to select betwwen Oberseite for PROGRAM BESTUECKEN section, modified QD_out.cpp - Sharry
                          // 04/08/09 - dts0100575734 - Remove duplicate vertex in Plane shape of GenCAD file in CAMCAD, modified GenCadIn.cpp - Sharry
   //Version = "4.7.510"; // 04/06/09 - No DR# - Fixed crash when loading cc file that has Width Table entries that reference non-existant geometry blocks (such entries are now dropped from width table) - rcf
   //Version = "4.7.509"; // 04/03/09 - dts0100576236 - Missing shorts tests with Takaya output from CAMCAD, modified TakayaFPTWrite.cpp - bkalyan
                          //        - dts0100568656 - We need a methode to verify weather a subclass is existing or not in the Takaya.log, modified TakayaFPTWrite.cpp - bkalyan
   //Version = "4.7.508"; // 04/02/09 - dts0100573111 - Stencil Generator - Made it more restrictive on poly gr class when converting polys on elect layers to mechanical pins - rcf
   //Version = "4.7.507"; // 04/02/09 - dts0100575734 - GenCAD Reader - Fix incorrect Plane shape handling with GenCAD file in CAMCAD, modified GenCadIn.cpp - Sharry
   //Version = "4.7.506"; // 03/31/09 - dts0100571706 - Zuken CR3000 Reader - Fix reading of MDF file, make presence of SYM_POSITION record optional, also ignore some unknown keywords in BSF and UDF files - rcf
   //Version = "4.7.505"; // 03/31/09 - dts0100574881 - Enable Excellon Drill Write (cpwrexcdrl) with CPPROBASE license - rcf
   //Version = "4.7.504"; // 03/31/09 - dts0100573714 - EDIF netlist file crashes CAMCAD EDIF reader, modified Edif200_in.cpp - bkalyan
                          //          - dts0100574275 - Padstack.HKP has PAD offset that CAMCAD seems to ignore, modified Vb99In.cpp - bkalyan        
   //Version = "4.7.503"; // 03/31/09 - dts0100574316 - Neutral read - Fixed pin rotation and merge all extra geometries for components, modified Mneutin.cpp - Sharry
   //Version = "4.7.502"; // 03/11/09 - dts0100570665 - DXF exporter - Fixed DXF do not export with the correct/expected  rotation angles, modified DxfLib.cpp - Sharry
                          // 03/11/09 - dts0100570613 - IPC netlist exporter - Fixed the top comment about unit information is not correct, modified IPCLib.cpp - Sharry
   //Version = "4.7.501"; // 03/04/09 - dts0100540100 :- Expedition HKP Import - need to read and use Padstack Technology from JobPrefs.hkp file, problem with the earlier fix, modified VB99In.cpp - bkalyan
   //Version = "4.7.500"; // 03/04/09 - dts0100559721 - Stencil Generator - Fixed mirror/rotation problem for complex apertures - rcf
   //Version = "4.7.499"; // 03/04/09 - dts0100559864 - Gerber Export - Fixed issue with draws defined within a poly void area , modified GerberWriter.cpp - Sharry
   //Version = "4.7.498"; // 03/03/09 - dts0100540100 :- Expedition HKP Import - need to read and use Padstack Technology from JobPrefs.hkp file, problem with the earlier fix, modified VB99In.cpp - bkalyan
   //Version = "4.7.497"; // 03/02/09 - dts0100535622 :- CAMCAD session killed while simultanous exporting from PADS Layout to IPC356 format
                        // dts0100544762 :- it's not possible to connect to the instance of CAMCAD Professional that was launched in automation mode
                        // dts0100548301 :- PADSPCB_CAMCAD Link -CAMCADPro.exe remains running after IPC356 or Flat DXF export, problem with earlier fix, modified CAMCAD.cpp, Api.cpp and CCEtoODB.h - bkalyan
   //Version = "4.7.496"; // 02/27/09 - Data Doctor - Subclasses Tab - Fixed glitch in pins 3 & 4 enable/disable cell edit, cut&paste code error, also fixed elements grid cell auto-resizing - rcf
   //Version = "4.7.495"; // 02/27/09 - DcaData.cpp - Backed out previous change, change itself was good but SG needs more updating to work with it, so nix for now - rcf
   //Version = "4.7.494"; // 02/27/09 - dts0100550414 - TRI MDA output needs to support Resistor_Array, Capacitor_Array, Diode_Array and Filter device types, Issue in the earlier fix, modified Ingun_o.cpp - bkalyan
   //Version = "4.7.493"; // 02/27/09 - No DR# - EDIF200 Importer - Changes to fix shcematic link does not work after portname changed Edif200_in.cpp/ Edif200_in.h - Sharry
   //Version = "4.7.492"; // 02/27/09 - dts0100568908 - Hang in CAMCAD HKP importer with this data set, modified vb99In.cpp - bkalyan
   //Version = "4.7.491"; // 02/26/09 - dts0100558626 - Stencil Generator - Show mechanical pins for reference, in fashion similar to silkdscreen - rcf
   //Version = "4.7.490"; // 02/26/09 - dts0100567507 - The attached Edif files are not reading in properly., modified Edif200_in.cpp - bkalyan
   //Version = "4.7.489"; // 02/25/09 - No DR# - Changes to fix missing logfile messages in 4.7 generated Takaya.log compared to 4.6 generated Takaya.log, TakayaFPTWrite.cpp/TakayaFPTWrite.h - bkalyan
   //Version = "4.7.488"; // 02/24/09 - dts0100546536 - Access Analysis - Fixed Bead Probes are not tested properly in AA when Bead Probes box is not set -Sharry
   //Version = "4.7.487"; // 02/24/09 - dts0100535622 :- CAMCAD session killed while simultanous exporting from PADS Layout to IPC356 format
                        // dts0100544762 :- it's not possible to connect to the instance of CAMCAD Professional that was launched in automation mode
                        // dts0100548301 :- PADSPCB_CAMCAD Link -CAMCADPro.exe remains running after IPC356 or Flat DXF export, modified CAMCAD.cpp and CCEtoODB.h - bkalyan
   //Version = "4.7.486"; // 02/23/09 - dts0100566367 -  HKP import - Fixed Comp pin and net are out of sync, modified VB99In.cpp - Sharry
   //Version = "4.7.485"; // 02/20/09 - dts0100546544 - Stencil Generator - Compensate for really really bad complex aperture layer usage generated by Zuken reader - rcf
                          //          - dts0100565675 - License Management - Enable Aperture, DXF, Gerber, and HPGL readers for OEM API usage via CPRINT license - rcf  
   //Version = "4.7.484"; // 02/19/09 - No DR# - AeroflexNailWir Importer - Fixed the refname of proble attribute is not visible when applying refname to probe - Sharry
                          // 02/19/09 - No DR# - SinglePins Net- Fixed Create/Merge SinglePins Net can not work properly - Sharry
   //Version = "4.7.483"; // 02/18/09 - dts0100565676 - Data Doctor - Add support to let user choose to operate without database connection and not get errors about it - rcf
   //Version = "4.7.482"; // 02/17/09 - dts0100559721- Stencil Generator - Fixed bad mirror handling, appearance was getting rotation wrong for inset rule on complex pad - rcf
   //Version = "4.7.481"; // 02/17/09 - dts0100545838 - AeroflexNail Importer - Support .Wir file for AeroflexNail reader - Sharry
                          // 02/17/09 - dts0100565677 - SiemenQD Exporter - Add a command in QD.out file to select betwwen Oberseite and Unterseite - Sharry
                          // 02/17/09 - dts0100548440 - EDIF 200 Importer - First sheet of any schematic is shown as blank sheet and name of the sheet is EDIFFILENAME.
   //Version = "4.7.480"; // 02/13/09 - dts0100564176 - Protel Importer - Fixed the attached Protel file does not read in - Sharry
                          // 02/13/09 - dts0100560238 - DXF Exporter - Add an option for removing Xscale to solve flow problems in Downstream CAD systems - Sharry
                          // 02/13/09 - dts0100560237 - DXF Exporter - Add $ACADVER tag and a command in dxf.out for $ACADVER value to help load into Adobe Illustrator - Sharry
   //Version = "4.7.479"; // 02/12/09 - dts0100563217 - Access Analysis - Fixed process related to Ignore Unloaded Components, involved Real Part disconnect with PCB Comp where the Loaded attrib is - rcf
   //Version = "4.7.478"; // 02/11/09 - dts0100563939 - Takaya Writer - Fixed parsing of takaya.tst "test library" file - rcf
   //Version = "4.7.477"; // 02/10/09 - dts0100564212 - Spea Write - Fixed crash, also fixed determination of Pad Code, was getting too many -1 "not found" in output, related to layer type PAD_ALL - rcf
   //Version = "4.7.476"; // 02/10/09 - dts0100500913 - MRU File Lis -  Add CC/CCZ files to MRU when Saved in addition to when Loaded - Sharry
                          // 02/10/09 - dts0100439542 - BOM Template - Support Newline for Refname in delimited BOM import function - Sharry
                          // 02/10/09 - dts0100531823 - Protel import - Fixed additional geometries in geometry list when compared to 4.6 - Sharry
   //Version = "4.7.475"; // 02/06/09 - No DR# - Fixed backward compatibility for recently consolidated graphic class display strings, to recognize older names that might be in existing default.set files - rcf
   //Version = "4.7.474"; // 02/06/09 - dts0100551357 - IPC Writer - Fixed IPC Netlist output units always in mils; add unit option in IPC Dialog, modified Ipc_Out.cpp - Sharry
   //Version = "4.7.473"; // 02/05/09 - dts0100550018 - DXF Writer - Fixed Flat dxf is wrong generated, modified DXFOut.cpp - Sharry
                          // 02/05/09 - dts0100559664 - ODB Reader - Fixed Pinloc errors after importing ODB++ from Expedition, modified OdbPpIn.cpp - Sharry
                          // 02/05/09 - dts0100561578 - ODB Reader - Fixed Attribute mapping not working for PART_NUMBER keyword, modified OdbPpIn.cpp - Sharry
   //Version = "4.7.472"; // 02/05/09 - No DR# - Teradyne CKT Writer - Changed pin ref in output to be camcad pin refname instead of assigned int pin counter - rcf
   //Version = "4.7.471"; // 02/04/09 - No DR# - Teradyne CKT Writer - Fixed problem in pin number output, problem introduced with DR 496592 fix circa May 2008 - rcf
   //Version = "4.7.470"; // 02/04/09 - No DR# - Data Doctor - Pin Mapping - Was not clearing pin map attrib on DB load where DB was blank and ccz was not, also fixed Pin Map dialog to set pinmap attrib only for non-blank mapping - rcf
   //Version = "4.7.469"; // 02/02/09 - No DR# - Fixed extent calc in DcaCamCadData.cpp, was not considering insert location for apertures (considered insert loc of the padstack but not of the pad within the padstack) - rcf
   //Version = "4.7.468"; // 01/30/09 - dts0100562152- CADIF Reader - Bumped fixed layer limit from 255 to 512, in response to sample data from Dave Arnold that had 324 layers - rcf
   //Version = "4.7.467"; // 01/30/09 - Complex Drill Hole - Fixed draw of complex tool block for item selection - rcf
   //Version = "4.7.466"; // 01/30/09 - Complex Drill Hole - Modified Tool List dialog to show icon "C" from Complex DrillHole blocks - rcf
   //Version = "4.7.465"; // 01/30/09 - dts0100560619 - Crash when cancelling out of TRI MDA Exporter, modified Ingun_o.cpp - bkalyan
                          // dts0100550414 - TRI MDA output needs to support Resistor_Array, Capacitor_Array, Diode_Array and Filter device types, modified Ingun_o.cpp - bkalyan
   //Version = "4.7.464"; // 01/29/09 - dts0100560490 - Panel Template - Was failing to save Units with template in DB, messed up next round Load of template - rcf
   //Version = "4.7.463"; // 01/28/09 - Exe Time Profiler - Moved CCamcadTimer class out of CamcadLicense.cpp/h into ProfileLib.cpp/h, added CPU ID to report - rcf
   //Version = "4.7.462"; // 01/28/09 - Exe Time Profiler - Added licensing timing report support, use command line param /LICTIME - rcf
   //Version = "4.7.461"; // 01/27/09 - dts0100559099 - Added two new graphics classes, EP Dielectric and EP Conductive - rcf
                          //          - dts0100558670 - Removed product specific license check for Real Part / Package, is now included with Base Product - rcf
   //Version = "4.7.460"; // 01/27/09 - dts0100552619 - DXF exporter - Fixed DXF Export combining probedrill top and probedrill bot layers to TOOL and TOOLCODE layers (merging surfaces) - Sharry
                          // 01/27/09 - dts0100560094 - DXF exporter - Fixed Issue with layer separation - top and bottom surfaces content merged in dxf file - Sharry 
   //Version = "4.7.459"; // 01/27/09 - dts0100552016 - System | Load Menu Language File - Added some error detection and reporting, previously it just silently failed - rcf
   //Version = "4.7.458"; // 01/26/09 - dts0100551518 - Takaya CA9 needs to support Elements from Data Doctor, modified TakayaFPTWrite.cpp and TakayaFPTWrite.h - bkalyan
   //Version = "4.7.457"; // 01/23/09 - dts0100559036 - Agilent 3070 exporter - Add the exported net name as an attribute to the net - Sharry
   //Version = "4.7.456"; // 01/23/09 - No DR# - Data Doctor - Subclass Elements - Added support for 4 pin elements, and element dev types Opto, Fuse, Inductor, and Jumper - rcf
   //Version = "4.7.455"; // 01/23/09 - dts0100552187 - IPL Z1800 - Spectrum - In NET2NAIL file output unplaced probes as 9999 instead of actual refname - rcf
   //Version = "4.7.454"; // 01/22/09 - dts0100552854 - Probe Placement - Disable probe density optimization for Fixtureless probes, because density does not really matter for Fixtureless and this opt is causing probes to move off of designated Preferred targets - rcf
   //Version = "4.7.453"; // 01/19/09 - dts0100552242 - CAMCAD Agilent AOI output not getting component X,Y and rotation values correct with all out file settings (Panel only), modified Ag_AOI_o.cpp and Ag_AOI_o.h - bkalyan
   //Version = "4.7.452"; // 01/19/09 - dts0100442218 - OrCAD MIN import - Fixed Orcad read not interpreting testpoints correctly, modified OrcltdIn.cpp - Sharry
                          // 01/19/09 - dts0100550604 - OrCAD MIN import - Fixed the customer file causes CAMCAD to CRASH, modified OrcltdIn.cpp - Sharry
   //Version = "4.7.451"; // 01/16/09 - dts0100552575 - New graphic classes, previous mod was not quite enough to make them visible, fixed that and added support in View settings to turn on/off new classes - rcf
   //Version = "4.7.450"; // 01/16/09 - dts0100445481 - HKP Reader - Fixed Expedition fiducial inserts not being mirrored to the bottom correctly, modified VB99In.cpp - Sharry
                          // 01/16/09 - dts0100443433 - Accel Reader - Fixed creating duplicate geometries on import, modifided AccelIn.cpp - Sharry
   //Version = "4.7.449"; // 01/15/09 - dts0100552575 - Added some new graphic classes as requested in DR, also made some progress on converting #define GR_CLASS_nnnn to GraphicClassTag enum symbol usage - rcf
   //Version = "4.7.448"; // 01/15/09 - dts0100517091 - GenCAD output - need to add support for other UNITS besides USER 1000, modified GenCadIn.cpp, GenCad_o.cpp, GenCad.h, GenCad.cpp, 
                          // Port_Li.cpp and Port.h - bkalyan
   //Version = "4.7.447"; // 01/14/09 - dts0100551228 - HKP Output - via layer pair not defined in layout.hkp file when via padstack spans to layer typed 'Split Plane', modified Vb99Out.cpp - bkalyan
   //Version = "4.7.446"; // 01/14/09 - dts0100546580 - Add header line to the .asc file created by Takaya flying probe exporter, modified TakayaFPTWrite.cpp - bkalyan
   //Version = "4.7.445"; // 01/13/09 - dts0100546536 - Support for Bead Probes in AA, modified AccessAnalysis.cpp/h, AccessAnalysisDlg.cpp/h, Attrib.cpp,
                          // CAMCAD.RC, DbUtil.h, DFT.cpp/h, Resource.h and DcaDftCommon.cpp/h - bkalyan
   //Version = "4.7.444"; // 01/13/09 - dts0100551534 - N numbers are not updated on to the Probe Names like 4.6 used to do in Takaya CAMCAD Export, modified TakayaFPTWrite.cpp and TakayaFPTWrite.h - bkalyan
   //Version = "4.7.443"; // 01/13/09 - dts0100545911 - HKP Import - CAMCAD is creating 'padlayerX' layers for all elec layers but no content gets added to these layers, modified VB99In.cpp - bkalyan
   //Version = "4.7.442"; // 01/05/09 - dts0100546512 - Teradyne Spectrum - Added optional resistance test support for Inductors - rcf
   //Version = "4.7.441"; // 01/05/09 - dts0100481367 - Boardstation import - Set unit TN as default unit for all Boardstation input files, modified MentorIn.cpp - Sharry
   //Version = "4.7.440"; // 01/02/09 - No DR:- clean up of obsolete code which is replaced during the DCA conversion, modified CAMCAD.vcproj, DataFile.cpp, Xml_Read.cpp
                          // Xml_Read.h, Xml_Wrt.cpp, XmlLayerTypeContent.h and XmlRealPartContent.h , removed files Xml_cont.cpp and Xml_cont.h - bkalyan
   //Version = "4.7.439"; // 01/02/09 - dts0100537967 - Exporting IPC netlist you can see CAMCAD flashing on the screen. Is it possible to hide it?,
                          // modified CcRepair.cpp, Xml_Read.cpp, RwLib.h and Ipc_out.cpp - bkalyan
   //Version = "4.7.438"; // 01/02/09 - dts0100544948 - CAMCAD HKP Import not setting visible attributes on proper layer, modified Vb99In.cpp - bkalyan
   //Version = "4.7.437"; // 01/02/09 - dts0100450298 - Add support of Package_Group geometry level attribute in Expedition HKP import, modified Vb99In.cpp - bkalyan
   //Version = "4.7.436"; // 01/02/09 - dts0100438931 - CAMCAD not defining and displaying it's own created thermals correctly, modified Apertur2.cpp,Apertur2.h,
                          // Draw.cpp, AccelIn.cpp, AccelIn.h, StencilGenerator.cpp, DcaBlock.cpp and DcaBlock.h - bkalyan 
   //Version = "4.7.435"; // 01/02/09 - dts0100449137 - Distributed Pads.In File No Longer Works Well With Distributed Sample File, modified PadsIn.cpp - bkalyan
   //Version = "4.7.434"; // 12/23/08 - dts0100545205 - Boardstation import - add support for PINS file as part of Boardstation data set, modified MentorIn.cpp - Sharry
   //Version = "4.7.433"; // 12/23/08 - dts0100519142 - PADS ASC Read - drill not being properly added to via padstacks, modified DcaCamCadData.cpp - bkalyan
   //Version = "4.7.432"; // 12/18/08 - dts0100546147 - Component Report - Add option in components.out file to set quotes as intelligent or all fields - Sharry
   //Version = "4.7.431"; // 12/17/08 - dts0100547137 - Teradyne Spectrum Writer - Fixed loss of accuracy for resister ohms value output in ipl.dat file, trim trailing zeros to make pretty values - rcf
   //Version = "4.7.430"; // 12/17/08 - dts0100547021 - Data Doctor - Added status error message for device pin name mapped to more than one component pin - rcf
   //Version = "4.7.429"; // 12/17/08 - Issue with the previous merge of the changes done in 4.6 as part of CCZ gap to 4.7, modified Draw.cpp, ToolList.cpp, DcaCamCadFileWriter.cpp  and DcaXMLContent.cpp - bkalyan
   //Version = "4.7.428"; // 12/17/08 - dts0100547021 - Data Doctor - Pin Mapping Dialog - Make grid based on component comppins instead of current mapping, current map may be empty which resulted in empty grid; also changed status validation to require exactly 1:1 mappings of device pin names - rcf
   //Version = "4.7.427"; // 12/17/08 - dts0100545799 - Soldermask analysis is taking the outline poly in to account even though it is not filled, modified AccessAnalysis.cpp - bkalyan
   //Version = "4.7.426"; // 12/17/08 - dts0100545803 - Invalid argument encountered in AA of CAMCAD DFT, modified DcaPoly.cpp, DcaPoly.h and AccessAnalysis.cpp - bkalyan
   //Version = "4.7.425"; // 12/16/08 - dts0100545197 - Boardstation import - Fixed in padshape in padstack not defined on proper layer - Sharry
                          // 12/16/08 - dts0100481367 - Boardstation import - Support for testpoint file Enhancement - Sharry
   //Version = "4.7.424"; // 12/12/08 - dts0100540121 - HKP Output - Layer Mapping Improvements needed, modified CAMCAD.RC, VB99Out.cpp and PcbLayer.cpp - bkalyan
   //Version = "4.7.423"; // 12/12/08 - dts0100544828 - Create Single Pin Nets option not working for PADS reader, modified DcaNet.cpp, DcaNet.h and DbUtil.h - bkalyan
   //Version = "4.7.422"; // 12/12/08 - dts0100518715 - Neutral Import - pin rotations not properly read and set during Neutral import, modified Mneutin.cpp and Mneutin.h - bkalyan
   //Version = "4.7.421"; // 12/12/08 - dts0100513786 - Incorrect component rotation with this Zuken BD file, modified ZukenCr5000Reader.cpp - bkalyan
   //Version = "4.7.420"; // 12/12/08 - dts0100394661 - Gencad pins defined with POLYGON shapes not imported as filled, modified join.cpp and GenCadIn.cpp - bkalyan
   //Version = "4.7.419"; // 12/11/08 - dts0100545792 - Spectrum and Z1800/IPL Writers - Fixed some missing output for test elements that share comppins with other test elements - rcf
   //Version = "4.7.418"; // 12/11/08 - dts0100439542 - Delimited BOM Import - Add Template File to be able to import a cpl file through the delimited BOM import function - Sharry
                          // 12/11/08 - dts0100416536 - Delimited BOM Import - Fixed the empty import dialog if the file you are trying to open is already opened by Word or Excel  - Sharry
   //Version = "4.7.417"; // 12/10/08 - Boundary Scanner - Changed func name Round to DcaRound to fix conflict with scanner header file - rcf
   //Version = "4.7.416"; // 12/09/08 - dts0100540100 - Expedition HKP Import - need to read and use Padstack Technology from JobPrefs.hkp file, modified Vb99In.cpp and Vb99In.h - bkalyan
   //Version = "4.7.415"; // 12/09/08 - dts0100526388 - CAMCAD crashed while opening new CCZ file if we close the previously opened file with component selected, modified CAMCADNavigator.cpp - bkalyan
   //Version = "4.7.414"; // 12/09/08 - dts0100543244 - Agilent AOI (SJ) output - Agilent requesting new out file command to set board on panel starting number, modified Ag_AOI.cpp - bkalyan
   //Version = "4.7.413"; // 12/09/08 - dts0100519074 - Add .eds files to EDIF erader open dialog, modified Port_lib.cpp - bkalyan
   //Version = "4.7.412"; // 12/09/08 - dts0100533409 - CAMCAD 4.7 - Not able to deleted the attribute for selected Geometry., modified DcaAttributes.cpp - bkalyan
   //Version = "4.7.411"; // 12/08/08 - dts0100479012 - Data Doctor - But really DcaBlock.cpp/h - Fix up component outline generation to handle complex apertures made with non-zero width polys - rcf
   //Version = "4.7.410"; // 11/14/08 - dts0100503350 - CAMCAD DFT  - Exposed metal calculation needs to be better in dealing with board level copper & soldermask, Problem with the earlier fix modified AccessAnalysis.cpp - bkalyan
   //Version = "4.7.409"; // 11/14/08 - dts0100538803 - Data Doctor - Packages tab - Fixed crash when setting outlines in partnumber mode and no partnumbers are present - rcf
   //Version = "4.7.408"; // 11/13/08 - dts0100538629 - Access Analysis dialog in CAMCAD not sized correctly, modified CAMCAD.RC - bkalyan
   //Version = "4.7.407"; // 11/11/08 - Set Exact Access Date to Nov 21 2008 for 4.7 - rcf
   //Version = "4.7.406"; // 11/10/08 - Fixed damage done to exePathName "system" var during help file update in 4.7.392 - rcf
   //Version = "4.7.405"; // 11/07/08 - No DTS, Updated the CAMCAD splash screens - bkalyan
   //Version = "4.7.404"; // 11/07/08 - dts0100514373 - Incorrect pin placement on X0601 with this Zuken BD import, modified ZukenCr5000Reader.cpp - bkalyan
   //Version = "4.7.403"; // 11/06/08 - dts0100535706 - OrCAD MIN import - Fixed Component X1 not correct in CAMCAD Project space - Sharry
   //Version = "4.7.402"; // 11/05/08 - CamcadLicense.cpp - Fixed glitch in prev fix that routed all base license failures to CPPROBASE - rcf
   //Version = "4.7.401"; // 11/05/08 - CamcadLicense.cpp - Fixed minor glitch/conflict between use of old and new camcad base license keys - rcf
   //Version = "4.7.400"; // 11/03/08 - No DR# - Stencil Generator - On Global Config tab show min pitch separately for top and bottom, on geometry edit page show pin pitch for current geometry, value is from PIN_PITCH attribute on pcb conmponent inserts - rcf
   //Version = "4.7.399"; // 11/03/08 - dts0100535279 - CAMCAD Comps report problem with bottom side comps reported as top side, modified Report.cpp - bkalyan
                          //          - dts0100535684 - Option to add extra columns in the Components Report, modified Report.cpp - bkalyan
                          //          - dts0100500071 - Missing Variant and Panel sections of SPEA 4040 output, modified Spea_Out.cpp - bkalyan
   //Version = "4.7.398"; // 11/3/08 - dts0100533753 - VB99In - Keepouts incorrectly coming through on etch layer in Expedition read - Michael
                          //         - No DR# - Remove the new graphic class - keepout other
   //Version = "4.7.397"; // 11/03/08 - dts0100506629 - Stencil Generator - Fixed preview graphics for bottom side for padstacks that have rotated pad aperture inserts - rcf
   //Version = "4.7.396"; // 11/03/08 - dts0100535322 - Panelization - Add a bottum to update all boards in a panel in Panelization of CAMCAD - Sharry
   //Version = "4.7.395"; // 11/03/08 - dts0100535127 - Camcad network Client: ODB++ import fails to load in client location, modified OdbPPIn.cpp - bkalyan
   //Version = "4.7.394"; // 11/03/08 - dts0100447618 - Access Analysis - Fixed Option in Access Analysis and Probe Placement to generate all reason codes not just one - Sharry
   //Version = "4.7.393"; // 11/01/08 - dts0100500941 - Stencil Generator - Promote panel fids to have Stencil Layer graphics during stencil generation - rcf
   //Version = "4.7.392"; // 11/01/08 - dts0100535126 - CAMCAD Help now connected to InfoHub - rcf
   //Version = "4.7.391"; // 10/31/08 - dts0100535171 - Teradyne Spectrum Writer - Fixed crash when panel selected for export - rcf
                          //          - No DR# - Data Doctor - Added RefDes list to Centroid tab grid (email request from Mark) - rcf
                          //          - Stencil Generator - Fixed lingering display state bug [aka "sometimes turns pads blue"] from 4.7.385 change - rcf
   //Version = "4.7.390"; // 10/31/08 - dts0100534096 - No progress bar in CAMCAD V4.7, modified Xml_Read.cpp and DcaCamCadFileReader.cpp - bkalyan
   //Version = "4.7.389"; // 10/31/08 - dts0100531973 - Accel Import module should use 'originalName' when possible for geometry name in CAMCAD, modified AccelIn.cpp - bkalyan
   //Version = "4.7.388"; // 10/31/08 - dts0100531833 - New license feature to be implemented, modified CamcadLicense.cpp and CCEtoODBlicense.h - bkalyan
   //Version = "4.7.387"; // 10/31/08 - dts0100503350 - CAMCAD DFT  - Exposed metal calculation needs to be better in dealing with board level copper & soldermask, modified AccessAnalysis.cpp - bkalyan
   //Version = "4.7.386"; // 10/31/08 - dts0100531833 - New license feature to be implemented, modified CamcadLicense.cpp and CCEtoODBlicense.h - bkalyan
   //Version = "4.7.385"; // 10/30/08 - Stencil Gen - Get rid of side-specific rule support, was misguided idea; Now rules shared to both sides; Make DB lookups backward compatible to side-specific rules - rcf
   //Version = "4.7.384"; // 10/30/08 - dts0100534337 - Z1800 IPL Writer - Incorrect test channel formatting on passive tests of Z1800 CAMCAD export - Sharry
   //Version = "4.7.383"; // 10/30/08 - dts0100533753 - VB99In - Keepouts incorrectly coming through on etch layer in Expedition read - Michael
   //Version = "4.7.382"; // 10/30/08 - dts0100501793 - CAMCAD DFT - Exposed metal calculation needs to improve it's complex union calculation, modified AccessAnalysis.cpp - bkalyan
   //Version = "4.7.381"; // 10/30/08 - dts0100530154 - Data Doctor - Pin mapping change and Fixed blank grill problem in the Pin Mapping Dialog - Sharry
   //Version = "4.7.380"; // 10/29/08 - dts0100523921 - Incorrect through hole locations with the Orbotech output, modified Orbotech_Out.cpp - bkalyan
   //Version = "4.7.379"; // 10/28/08 - dts0100525244 - In CAMACAD, In Edit Probe, size of the probe shows the wrong in GUI., modified CAMCADNavigator.cpp - bkalyan
   //Version = "4.7.378"; // 10/28/08 - dts0100533455 - Fixed 3070 out file option to disable component outlines from BXY file, mdified Hp3070_o.cpp  - Sharry
   //Version = "4.7.377"; // 10/28/08 - dts0100531514 - All CAMCAD log files should be written to File | Project Path location  - bkalyan
   //Version = "4.7.376"; // 10/27/08 - Updated copyright to 2008, for splash screen, VersionResource.rc, and DbUtil.h - rcf
   //Version = "4.7.375"; // 10/24/08 - dts0100531693 - Data doctor - Fixed Copy (Ctrl+C) commands is not working in Data doctor components tab -tolerance grid, modified DataDoctorDialog.cpp.  - Sharry
                          // 10/24/08 - dts0100452317 - PCB Navigator - Fixed PCB Navigator disabled after RealParts added to ECAD, modified CAMCADNavigator.cpp.  - Sharry
   //Version = "4.7.374"; // 10/24/08 - dts0100377134 - Local Fiducials no exported to Agilent AOI PLX outputs,modified Ag_AOI.cpp and Ag_AOI.h  - bkalyan
   //Version = "4.7.373"; // 10/23/08 - dts0100500071 - Missing Variant and Panel sections of SPEA 4040 output,modified Resource.h,CAMCAD.rc,Spea_Out.cpp and Spea_Out.h  - bkalyan
   //Version = "4.7.372"; // 10/23/08 - dts0100466684 - Generic function to normalize component and probe ref names.  - bkalyan
   //Version = "4.7.371"; // 10/23/08 - dts0100532158 - Teradyne 228X ICT Writer - Fixed Export Teradyne 228X ICT Write (NAV) ,file name info in .pnl file is wrong, modified ckt_nav_out.cpp.  - Sharry
                          // 10/23/08 - dts0100531459 - Generate Component report - Fixed CAMCAD 4.7 report file has change in syntax from 4.6, modified general.cpp.  - Sharry
                          // 10/23/08 - dts0100531314 - CCZ Importer - Fixed In Aperture writes duplication of the same aperture information, modified DcaXmlContent.cpp.  - Sharry
   //Version = "4.7.370"; // 10/22/08 - dts0100530328 - Data Doctor - Packages Tab - Fixed DB Load connection to Outline Fill checkbox - rcf
   //Version = "4.7.369"; // 10/22/08 - dts0100531890 - Added Agilent i1000 exporter support, also started cleaning up obsolete file type #defines in DbUtil.h, there are still more to do - rcf
   //Version = "4.7.368"; // 10/22/08 - dts0100530280 - Panelizer - Make panel name entry and overwrite validation case-insensitive - rcf
   //Version = "4.7.367"; // 10/22/08 - dts0100426618 - Access Analysis - Data doctor Part Tab does not allow multi-row selections for editing cell contents, modified DataDoctorPageParts.cpp - bkalyan
   //Version = "4.7.366"; // 10/21/08 - dts0100500371 - Access Analysis - Honor decimal places setting, was at constant 3 decimals places - rcf
                          //          - dts0100531466 - Data Doctor - Centroids Tab - Display realpart Package outline w/ component geometry - rcf
   //Version = "4.7.365"; // 10/21/08 - dts0100520422 - Should write encrypted HKP based on ENCRYPT_OUTPUT flag set in vbascii.out - sasharma
   //Version = "4.7.364"; // 10/21/08 - dts0100482789 - CAMCAD Export - Z1800 IPL should support device type OPTO, modified Ipl_out.cpp - bkalyan
   //Version = "4.7.363"; // 10/20/08 - no DR# - VB99In - Ref names are not shown - Michael
   //Version = "4.7.362"; // 10/20/08 - dts0100521265 - CCZ importer - Add log messages for dropped polys while generating Pin to pin length report . - Sharry
                          //          - dts0100525616 - DMISV3 export - Fixed CAMCAD GRAPHIC- Tool crashes while export DMIS V3 write. - Sharry
                          //          - dts0100524872 - EDIF 300 importer - Fixed Pin Name text with upper and lower case is duplicated in CAMCAD. - Sharry
   //Version = "4.7.361"; // 10/17/08 - dts0100523726 - ODB++ importer - Fixed the incorrect circuit short in imported ODB++ file. - Sharry
   //Version = "4.7.360"; // 10/17/08 - dts0100461877 - DcaGeomLib.cpp - Fixed left behind change, fix was put in 4.6 circa Dec 2007, never made it to 4.7/5.0 until now - rcf
   //Version = "4.7.359"; // 10/17/08 - no DR# - Data Doctor - Packages and Library tabs - Added custom attrib feature on Packages tab (like Parts tab), added config interface to Library tab - rcf
   //Version = "4.7.358"; // 10/17/08 - Showing Stencil Layer places it under copper layers,modified Draw.cpp and CcDoc.cpp - bkalyan
   //Version = "4.7.357"; // 10/17/08 - dts0100529073 - VB99In - Cannot obtain Start/End layer of non-plated through holes for vias placed on cells - Michael
   //Version = "4.7.356"; // 10/16/08 - dts0100529422 - VB99In - Cannot distinguish Drill drawing (through) - Michael
   //Version = "4.7.355"; // 10/16/08 - dts0100529400 - VB99In - When a text is placed on the Soldermask Bottom layer,  LayerType is returned as LAYTYPE_UNKNOWN - Michael
   //Version = "4.7.354"; // 10/16/08 - dts0100529394 - VB99In - When a cell has a text on the soldermask opposite and is placed on the board (PCB Geometry), Layter Type of the text is returned as LAYTYPE_UNKNOWN - Michael
   //Version = "4.7.353"; // 10/16/08 - dts0100529390 - VB99In - "Net 0" trace must be graphicClass="6" (GR_CLASS_ETCH) instead of graphicClass="0" - Michael
   //Version = "4.7.352"; // 10/16/08 - dts0100529368 - VB99In - Oval mounting holes need to be supported - Michael
   //Version = "4.7.351"; // 10/16/08 - dts0100491618 - Aeroflex Nail Reader - Add some more data from fixture file as attribs on probe (from Mark's QA feedback) - rcf
   //Version = "4.7.350"; // 10/15/08 - dts0100528540 - VB99In - Hatch planes are exported as solid - Michael
   //Version = "4.7.349"; // 10/15/08 - dts0100507448 - Gerber import - Fixed Gerber import not working properly with 2 supplied files, used to work with version 4.5, modified GerbIn.cpp - Sharry
   //Version = "4.7.348"; // 10/14/08 - dts0100526972 - Testability Report - Fixed top/bot height threshold handling to allow 0, previously blank or 0 turned off test - rcf
   //Version = "4.7.347"; // 10/14/08 - dts0100491618 - Aeroflex Nail Reader - Ready for testing - rcf
   //Version = "4.7.346"; // 10/13/08 - dts0100528543 - VB99In - Conductive shape associated with a cell are exported outside borad outline - Michael
   //Version = "4.7.345"; // 10/10/08 - dts0100434389 - Request for support of electrical test of devicetype Relay in Takaya output, modified TakayaFPTWrite.cpp - bkalyan
   //Version = "4.7.344"; // 10/10/08 - dts0100469314 - CC importer - Fixed CAMCAD not updating collaboration information with new units - Sharry
   //Version = "4.7.343"; // 10/10/08 - dts0100511182 - GenCad Importer - Fixed select all with same D-Code (draws) not working, modified GenCadIn.cpp - Sharry
   //Version = "4.7.342"; // 10/09/08 - dts0100528207 - CAMCAD crashes upon executing menu command "Tool s>> Geometry Library >> Arrange Local Library", modified DcaFile.cpp and DcaBlock.cpp - bkalyan
   //Version = "4.7.341"; // 10/09/08 - dts0100527086 - CC importer- Fixed Schematic link doesnot work in CAMCAD 4.7 build, modified DcaAttributes.cpp - Sharry
                          //          - dts0100524909 - HPGL export - Fixed HPGL export hangs and creates huge hgl file - Sharry
   //Version = "4.7.340"; // 10/08/08 - dts0100527575 - Missing columns in Repair file of Takaya exporter, modified TakayaFPTWrite.cpp - bkalyan
   //Version = "4.7.339"; // 10/08/08 - dts0100523704 - Missing conductive pads on L7 with this Board Station set of files, modified MentorIn.cpp - bkalyan
   //Version = "4.7.338"; // 10/08/08 - dts0100528661 - Data Doctor - Packages Tab - Better management of enable/disable Custom Outline edit controls after group Create Outlines button is clicked - rcf
                          //          - dts0100527657 - Access Analysis - Coalesce separate realpart and dft use-outline conrols into a single Use Outline checkbox, priority now fixed at realpart, user does not need to know difference anymore - rcf
                          //          - dts0100527005 - Testability Report - But not really, was using "standard internal" keywords that were not properly registered at startup (fix is to register them), resulted in types defaulting to vtString instead of vtInteger, later difference was detected and reported in Probe Placement - rcf
                          //          - dts0100523447 - Data Doctor - Subclass Tab - Right-click Delete option now deletes from database, was only deleting from grid, offers confirmation with sticky don't-ask-again checkbox - rcf
   //Version = "4.7.337"; // 10/08/08 - dts0100521265 - CCZ - Fixed tool is crashing while generating Pin to pin length report, modified PcbUtil.cpp. - Sharry
   //Version = "4.7.336"; // 10/07/08 - dts0100527422 - CAMCAD crashes while deleting circular poly segment, modified EditPoly.cpp and Search.cpp - bkalyan
   //Version = "4.7.335"; // 10/06/08 - dts0100463145 - Tie Dot creation function in EDIF read, modified Edif200_in.cpp - bkalyan
   //Version = "4.7.334"; // 10/03/08 - dts0100465314 - Fixture Reuse - Added tooling hole differences to report - rcf
   //Version = "4.7.333"; // 10/02/08 - dts0100495329 - ODB++ Import - support ODB++ V7 - Sharry
                          //          - dts0100517399 - ODB++ Import - Fixed duplicate components - Sharry
   //Version = "4.7.332"; // 10/02/08 - dts0100460144 - Add new menu command Add Board Outline, modified Add.cpp and AddOutline.cpp - bkalyan
   //Version = "4.7.331"; // 10/02/08 - dts0100463145 - Tie Dot creation function in EDIF read, modified Edif200_in.cpp - bkalyan
   //Version = "4.7.330"; // 10/02/08 - dts0100523583 - Expedition HKP Export - NETCLASS attribute should write to netprops.hkp file as NETCLASS, modified Vb99Out.cpp - bkalyan
   //Version = "4.7.329"; // 10/02/08 - dts0100523921 - Incorrect through hole locations with the Orbotech output, modified Orbotech_Out.cpp - bkalyan
   //Version = "4.7.328"; // 09/30/08 - No case# - VB99In - Unable to obtain SOLDERPASTE for board - Michael
   //Version = "4.7.327"; // 09/30/08 - No case# - VB99In - Identifying assembly outlines placed on the opposite side - Michael
   //Version = "4.7.326"; // 09/30/08 - dts0100524761 - VB99In - insertType of Mechanical cell and Drawing cell is "0" - Michael
   //Version = "4.7.325"; // 09/30/08 - dts0100524757 - VB99In - Package cells and Mechanical cells can be identified in geometry data, but Drawing cells cannot be distinguished - Michael
   //Version = "4.7.324"; // 09/30/08 - No case# - VB99In - Unable to obtain contour in cell Editor - Michael
   //Version = "4.7.323"; // 09/30/08 - dts0100526325 - VB99In - Unable to obtain placement obstructs - Michael
   //Version = "4.7.322"; // 09/30/08 - No case# - VB99In - Unable to obtain route obstructs in the board - Michael
   //Version = "4.7.321"; // 09/29/08 - dts0100526291 - VB99In - Layer name of graphic that was routed in Cell Editor is "MNT_LYR" - Michael
   //Version = "4.7.320"; // 09/29/08 - dts0100526321 - VB99In - The layer name of the mount side is just "SOLDER_MASK" without layer type - Michael
   //Version = "4.7.319"; // 09/29/08 - dts0100526287 - VB99In - The layer on the mounting side is "SILKSCREEN_BOTTOM" - Michael
   //Version = "4.7.318"; // 09/26/08 - No case# - VB99In - Unable to obtaining specified sides from part editor for placing cells - Michael
   //Version = "4.7.317"; // 09/26/08 - dts0100524760 - VB99In - Unable to obtain SOLDERPASTE for cell - Michael
   //Version = "4.7.316"; // 09/26/08 - Merging the changes done in 4.6 as part of CCZ gap to 4.7 - bkalyan
   //Version = "4.7.315"; // 10/01/08 - dts0100527287 - Data Doctor - Packages tab - Fixed crash - rcf
                          //          - dts0100523783 - Digital Test Write - Fixed bad format, was writing address of string instead of controlled value - rcf
                          //          - dts0100523750 - Testronics Write - Abort export is anything other than a single PCB file is visible - rcf
                          //          - dts0100523729 - Testronics Write - Include version string in header of output file - rcf
                          //          - dts0100523723 - Testronics Write - Add field description comments to section headers - rcf
   //Version = "4.7.314"; // 10/01/08 - dts0100525599 - CAMCAD : Gencam imports blank PCB with no information other than Board outline, modified File.cpp and DcaFile.cpp - bkalyan
   //Version = "4.7.313"; // 10/01/08 - dts0100523738 - Panelizer - Fixed display after Apply button click, was leaving created Panel visible (as wallpaper), which was confusing with template display - rcf
                          //          - no DR - Panelizer - Added auto-assign of physical PCB upon template load from DB, applies only if single physical PCB is present in ccz - rcf
   //Version = "4.7.312"; // 09/30/08 - dts0100526250 - Panelizer - Show currently loaded template name in dialog - rcf
                          //          - dts0100524168 - Panelizer - Validate layout upon Apply button click, report to user, allow to proceed or cancel - rcf
   //Version = "4.7.311"; // 09/29/08 - dts0100522867 - CAMCAD crashes when we remove all files from file list, modified DcaBlock.cpp - bkalyan
   //Version = "4.7.310"; // 09/26/08 - Data Doctor - Packages Tab - Added Hide Single Pin Components checkbox (no DR, Mark's request via email) - rcf
   //Version = "4.7.309"; // 09/26/08 - Panelizer - Implemented database interface for Save/Load - rcf
   //Version = "4.7.308"; // 09/26/08 - dts0100511275 - ODB++ Read - Fixed Component references a package that CAMCAD drops on import - Sharry
   //Version = "4.7.307"; // 09/26/08 - Merging the changes done in 4.6 as part of CCZ gap to 4.7 - bkalyan
   //Version = "4.7.306"; // 09/25/08 - dts0100522486 - Doesnot import traces , fills for Boardstation import format in CAMCAD, modified MentorIn.cpp - bkalyan
   //Version = "4.7.305"; // 09/25/08 - dts0100522376 - CAMCAD crashes while exporting Takaya FPT if DFT is not run on testcase, modified Takaya_o.cpp - bkalyan
   //Version = "4.7.304"; // 09/23/08 - dts0100522936 - CAMCAD COMPS Report lists all comps as THRU but in CAMCAD there are SMD comps, modified Report.cpp and General.cpp - bkalyan
   //Version = "4.7.303"; // 09/23/08 - dts0100522816 - In CAMCAD pro , tool is crashing while merging two files, modified MergeFiles.cpp and DcaType.cpp - bkalyan
   //Version = "4.7.302"; // 09/22/08 - dts0100522430 -  DXF Import - Fixed arcs are not properly imported, width of line change - Sharry
   //Version = "4.7.301"; // 09/22/08 - dts0100521952 - Allegro Extract Import is creating FOUR files in file list - should only be ONE, modified DcaFile.cpp and DcaFile.h - bkalyan
   //Version = "4.7.300"; // 09/19/08 - Data Doctor - Packages Tab - Fixed DB interaction for custom outlines, Clear template edit boxes when template changes, Fixed status update for custom outline editing. - rcf
   //Version = "4.7.299"; // 09/19/08 - dts0100522104 - Mentor Neutral Read - Fixed Incorrect TEST attribute assignment on reading this Neutral file -Sharry
   //Version = "4.7.298"; // 09/18/08 - dts0100522174 - Panelizer - Fixed Cut[&Paste] crash -rcf
                          //          - dts0100522182 - Panelizer - Fixed Cut[&Paste] crash -rcf
                          //          - dts0100522189 - Panelizer - Fixed Cut[&Paste] crash -rcf
   //Version = "4.7.297"; // 09/18/08 - Data Doctor - Package Outline - Added means for user to offset custom outline in XY - rcf
                          //          - dts0100520492 - Data Doctor - Packages Page - Make number entry edit boxes and grid cells accept only appropriate number entries - rcf
                          //          - dts0100520536 - Data Doctor - Packages Page - Refresh XY grid after Outline Rotate 90 is applied, old leftover coords in grid ended up being applied to outline and messed it up - rcf
   //Version = "4.7.296"; // 09/17/08 - Data Doctor - Package Outline - Backward compatibility problem in conversion of old outline to custom is fixed, I think - rcf
   //Version = "4.7.295"; // 09/17/08 - Data Doctor - Generic Centroid Read - Adjustments to outline creation and handling, mainly involving origin management - rcf
   //Version = "4.7.294"; // 09/16/08 - dts0100523113 - CCZ importer - Fixed In CAMCAD Pro, Tool is crashing while importing ODB++ after removing layers from layerlist - Sharry
   //Version = "4.7.293"; // 09/16/08 - dts0100514377 - IPL/TRI-MDA Writer - Fixed Nails.asc file should reflect the different probe sizes used and not just default to 1 and add blockname to probenumber map in log - Sharry
   //Version = "4.7.292"; // 09/15/08 - dts0100520451 - Data Doctor - Fixed group outline creation fill mode, was using constant true instead of accessing checkbox setting - rcf
   //Version = "4.7.291"; // 09/15/08 - dts0100521226 - Data Doctor - Restrict input to valid chars and syntax for percent cells, on Subclass, Comps, and Parts tabs (plus tolerance and minus tolerance) - rcf
   //Version = "4.7.290"; // 09/15/08 - dts0100388759 :- EDIF 200 import issue - Need to add support for NetBundle constructs (BUSS Support), modified Edif200_in.cpp and Edif200_in.h - bkalyan
   //Version = "4.7.289"; // 09/15/08 - dts0100405620 :- Adjustment to Agilent AOI SJ writer, bottom side files, modified Ag_AOI_o.cpp - bkalyan
   //Version = "4.7.288"; // 09/12/08 - Generic Centroid Import - Added capability to create PCB Component inserts instead of Centroid inserts, option is enabled by new radio button to choose type of item to create - rcf
                          //          - dts0100521543 - Panelizer - Enhanced panel name getter dialog to not allow blank names and manage user approval of replacing existing panels (stop making duplicates) -rcf
                          //          - dts0100522370 - Panelizer - Different detail of same issue, same fix as for 521543 - rcf
   //Version = "4.7.287"; // 09/11/08 - dts0100520670 - Access Analysis - Turn on use-realpart-outline for default constraints setting, since that is the sort of outline Data Doctor makes now - rcf
   //Version = "4.7.286"; // 09/11/08 - dts0100521834 - TestAttrib2DFT - Fixed crash (processing tool holes) - rcf
   //Version = "4.7.285"; // 09/10/08 - dts0100521526 - Panelizer - FIxed problems instantiating PCB arrays, were going to wrong FileStruct - rcf
   //Version = "4.7.284"; // 09/10/08 - dts0100520504 - Data Doctor - Fixed crash, was using obsolete scheme (due to DCA) to add layers, potentially accessing layer list from closed/deleted doc - rcf
   //Version = "4.7.283"; // 09/10/08 - dts0100511787 - request for new Agilent AOI OUT file commands..., modified Ag_AOI_o.cpp and Ag_AOI_o.h - bkalyan
   //Version = "4.7.282"; // 09/10/08 - dts0100521231 - IPL aka Z1800 Export - Fixed crash due to attempt to export panel, exporter does not support panel export - rcf
   //Version = "4.7.281"; // 09/09/08 - dts0100422925 - Mentor Neutral Read - Add new functions for Importer to read testpoints section and place test probe and access marker - sharry
   //Version = "4.7.280"; // 09/09/08 - dts0100387067 :- VBASCII Import - CAMCAD needs to support board level geometry changes, modified VB99In.cpp - bkalyan
   //Version = "4.7.279"; // 09/09/08 - dts0100440717 - Customer requests ability to set default tolerance for IC Diode Test in Takaya output, modified TakayaFTPWrite.cpp - bkalyan
   //Version = "4.7.278"; // 09/09/08 - dts0100506304 - Fixed CAMCAD Neutral Import - fails with this file but file appears to be good - sharry
                          // 09/09/08 - dts0100511803 - TRI MDA Write - Change duplicate Nail name to $10000 - sharry 
   //Version = "4.7.277"; // 09/08/08 - dts0100491603 - CADStar Visula read - There are 'cutout' features that are not filled and therefore do not display properly, modified CadifIn.cpp - bkalyan
   //Version = "4.7.276"; // 09/08/08 - dts0100512899 - The RSI Exchange toolbar button needs to be updated to Data Exchange - Sharry
   //Version = "4.7.275"; // 08/27/08 - dts0100394659 - CAMCAD not refreshing all controls on PCB Navigator after importing Gencad file, modified CAMCADNavigator.cpp - bkalyan
   //Version = "4.7.274"; // 09/05/08 - dts0100460144 - Add new menu command Add Board Outline, modified VB99In.cpp,Add.cpp,CAMCAD.rc,CCView.cpp,CCView.h,Resource.h,AddOutline.cpp (Newly added file) andAddOutline.h (Newly added file) - bkalyan
   //Version = "4.7.273"; // 09/05/08 - dts0100506378 - In EE2007.3/FablinkXE-pcb/pnl, Data is missing in the generated .cc/.ccz files. Related case dts0100520422 - sasharma
   //Version = "4.7.272"; // 09/05/08 - No case# - Data Doctor - Centroids Tab - Change outline algorithm names to match standard set by Packages tab (and used by AA as well), special issues for DB backward compatibility handled,since these settings appear directly in DB - rcf
   //Version = "4.7.271"; // 09/05/08 - dts0100460144 - Add new menu command Add Board Outline, modified VB99In.cpp,Add.cpp,CAMCAD.rc,CCView.cpp,CCView.h,Resource.h,AddOutline.cpp (Newly added file) andAddOutline.h (Newly added file) - bkalyan
   //Version = "4.7.270"; // 09/04/08 - Panelizer - Some adjustments per Mark; column labels, improved Create action - rcf
   //Version = "4.7.269"; // 09/03/08 - dts0100466072 - Customer reports problems in zooming to probes in Placed Probe list in PCB Navigator, modified CAMCADNavigator.cpp, prior fix was not covering all scenarios - bkalyan
   //Version = "4.7.268"; // 09/03/08 - dts0100520581 - Data Doctor - Packages Tab - Enable add-custom-outline button only when all required params are non-zero - rcf
                          //          - dts0100520460 - Data Docor - Packages Tab - Removed stray static label - rcf
                          //          - dts0100520902 - Data Doctor - Elements Tab - Fixed validation that prevents duplicate subclass names, was not allowing self to retain same name - rcf
                          //          - dts0100520659 - Data Doctor - Elements Tab - Clear elements grid when subclass is deleted, was leaving leftovers in display (updated only when another subclass was selected) - rcf
                          //          - dts0100491618 - Aeroflex Nail File Reader - Implementation started, about 90% complete, awaiting test data set - rcf
   //Version = "4.7.267"; // 08/30/08 - dts0100421463 - Access Analysis - Promoted single pin SMD to first class citizen, is selectable target type like the rest - rcf
   //Version = "4.7.266"; // 08/29/08 - dts0100460119 - Allegro Write - Handle blank and duplicate block names, map them to unique name GEOM<blocknumber> - rcf
   //Version = "4.7.265"; // 08/29/08 - dts0100460120 - Allegro Write - Fixed problems in polyline write state management that caused both multiple and missing "done" records - rcf
   //Version = "4.7.264"; // 08/29/08 - dts0100466072 - Customer reports problems in zooming to probes in Placed Probe list in PCB Navigator, modified CAMCADNavigator.cpp - bkalyan
   //Version = "4.7.263"; // 08/28/08 - dts0100516098 - IPL Z1800 Write - Change form of Zener test in ipl.dat file, as requested in case notes - rcf
   //Version = "4.7.262"; // 08/28/08 - dts0100482778 - Stencil Generator - CHange default decimal places from 2 to 3 for metric stencil rules - rcf
   //Version = "4.7.261"; // 08/27/08 - Fixed copying of Version.cpp for release build so it is not dependent on build tree folder names matching version being built - rcf
   //Version = "4.7.260"; // 08/27/08 - dts0100453323 - Data Doctor - Fixed DB load so error in one part does not terminate load, added error message when PinMap field is found to be bad, change PinMap write to DB so no longer writes unmapped pins - rcf
   //Version = "4.7.259"; // 08/27/08 - dts0100394659 - CAMCAD not refreshing all controls on PCB Navigator after importing Gencad file, modified CAMCADNavigator.cpp - bkalyan
   //Version = "4.7.258"; // 08/27/08 - dts0100494785 - No vision tests generated with this design on Takaya export, modified TakayaFPTWrite.cpp - bkalyan
   //Version = "4.7.257"; // 08/26/08 - dts0100511439 - TRI MDA/ICT Write & - Fixed the defect that the output files are always in Inches (Format.asc and Parts.asc are incorrect) - Sharry
                          // 08/26/08 - dts0100511803 - TRI MDA Write & - Fixed the defect that a few changes in nails.asc we can make to provide a much better file to the users - Sharry
   //Version = "4.7.256"; // 08/26/08 - HKP Output - CAMCAD is writing a negative 'thermal_clearance' value which is illegal in Expedition - zpiecuch
   //Version = "4.7.255"; // 08/25/08 - Change to version number as reported by app, for release 4.7 - rcf
   //Version = "5.0.254"; // 08/25/08 - Data Doctor - Subclass Elements - Teradyne IPL aka Z1800 and Spectrum exporters updated to support Test Elements - rcf
   //Version = "5.0.253"; // 08/25/08 - dts0100512019  :- Conductive Shape not being read from Expedition data, modified Vb99In.cpp and Vb99In.h - bkalyan
   //Version = "5.0.252"; // 08/25/08 - dts0100510641  :- Plane outlines need to be removed on Expedition read when Use Generated data set to Y, modified Vb99In.cpp - bkalyan
   //Version = "5.0.251"; // 08/22/08 - No case# - DcaAttributes.cpp/h - Fixed failure to scale visible text attrib height and width during transform, e.g. when changing drawing units from mm to inch - rcf
   //Version = "5.0.250"; // 08/21/08 - dts0100388301 - CAMCAD Protel PCB import enhancement request (adding TEST attribute during import), modified PfwIn.cpp and PfwIn.h - bkalyan
   //Version = "5.0.249"; // 08/21/08 - dts0100511439 - TRI MDA/ICT Write - Fixed the defect that the output files are always in Inches regardless of CAMCAD setting - Sharry
   //Version = "5.0.248"; // 08/20/08 - Data Doctor - Konrad WR - Subclass Elements - Refactored Subclass and Elements support to new Element.cpp/h files, to centralize and so exporters can include succint file instead of two massive DataDoc h files to get the Element access - rcf
   //Version = "5.0.247"; // 08/20/08 - Data Doctor - Subclass Elements - Added element name to element data and grid, Konrad WR uses it in generated refnames - rcf
   //Version = "5.0.246"; // 08/19/08 - dts0100515897 - BoardStation Write - Fixed crash, code reported error to user but did not protect itself from consequence - rcf
   //Version = "5.0.245"; // 08/19/08 - Data Doctor - Subclasses tab - Added handling of Value and Tol fields similar to Comps and Parts tabs - rcf
                          //          - Konrad Writer - Implement usage of Subclass Elements - rcf
                          //          - dts0100513256 - Fixture Reuse - Add ability for user to proceed with import when alignment fails - rcf
   //Version = "5.0.244"; // 08/15/08 - Data Doctor - Subclasses tab - Added support for full list of elemental device types - rcf
   //Version = "5.0.243"; // 08/14/08 - Data Doctor - Subclasses tab - Database connection added - rcf
   //Version = "5.0.242"; // 08/13/08 - Data Doctor - Subclasses tab - Snapshot - rcf
   //Version = "5.0.241"; // 08/08/08 - Data Doctor - Added files and basic interface for new Subclasses tab - rcf
   //Version = "5.0.240"; // 08/07/08 - dts0100513485 - Zuken CR5K Reader - Add support for round thermal pads - rcf
   //Version = "5.0.239"; // 08/05/08 - dts0100508832 - CAMCAD HKP Output - Need to change the way we write testpoint inserts to the Layout.hkp - zpiecuch
                          //          - dts0100512909 - Expedition HKP Output - need to enhance padstack output for SMD padstacks to contain Top and Bottom Pastemask and Soldermask - zpiecuch
                          //          - dts0100499297 - Expedition HKP Output - '_MH' padstacks created... need to understand code logic - zpiecuch
   //Version = "5.0.238"; // 08/04/08 - dts0100505255 - Aeroflex writer, Fixed the incorrect test point numbers with this CCZ file with Aeroflex output  - Michael
   //Version = "5.0.237"; // 07/29/08 - dts0100481723 - Add option to export through hole pins in Orbotech output, modified Orbotech_Out.cpp - bkalyan
   //Version = "5.0.236"; // 07/29/08 - dts0100405620 - Adjustment to Agilent AOI SJ writer, bottom side files, modified Ag_AOI_o.cpp - bkalyan
   //Version = "5.0.235"; // 07/29/08 - dts0100469013 - Gencad writer putting illegal chars in gencad file, modified GenCad_o.cpp - bkalyan
   //Version = "5.0.234"; // 07/18/08 - Port_lib.cpp - Disabled connection to "new Veribest Ascii Reader", it is a demo of an alternate implemention proposal and is not part of upcoming release - rcf
   //Version = "5.0.233"; // 07/15/08 - dts0100508832 - CAMCAD HKP Output - Correction to fix - zpiecuch
   //Version = "5.0.232"; // 07/14/08 - dts0100473765 - Vb99In.cpp - HPK import ".MOUNTING_HOLE" feature of a geometry is set to Mechanical Pin instead of Tooling - Siep.
   //Version = "5.0.231"; // 07/14/08 - dts0100508832 - CAMCAD HKP Output - Need to change the way we write testpoint inserts to the Layout.hkp - zpiecuch
                          //          - dts0100509156 - Accel/PCAD Import not handling Thermal Padshapes (not getting created so they're not in padstacks) - zpiecuch
   //Version = "5.0.230"; // 07/09/08 - dts0100505706 - Juki_out.cpp - Appropriate changes were made in the Juki.cpp file - Siep.
   //Version = "5.0.229"; // 07/09/08 - dts0100507625 - QD Out - Fixed Underside versus Upperside in Bottom side file output, was always Upperside - rcf
   //Version = "5.0.228"; // 07/08/08 - Testronics ICT Writer - Fixed PINS TABLE output for Loaded=False parts - rcf
   //Version = "5.0.227"; // 07/08/08 - Testronics ICT Writer - New exporter added to CAMCAD Pro - rcf
   //Version = "5.0.226"; // 07/08/08 - dts0100420626 - CAMCAD Boardstation reader and Neutral reader not parsing wires files with PRP statements properly, modified MentorIn.cpp - bkalyan
   //Version = "5.0.225"; // 07/02/08 - dts0100469299 - CAMCAD creating gencad files with multiline text entries, modified GenCad_o.cpp - sasharma
   //Version = "5.0.224"; // 07/02/08 - dts0100499849 - AllegOut.cpp - Inconsistent checks in two sections for "keeper" points led to memory mismanagement and crash - rcf
   //Version = "5.0.223"; // 07/02/08 - dts0100506301 - DigitalTest_o.cpp - CAMCAD Digital Test output - un-needed colum in "Crystal" data line is fixed - Siep
   //Version = "5.0.222"; // 07/01/08 - dts0100505706 - Juki_out.cpp - New opotions are added to export unloaded parts including two command that allows user to determine which parts are exported in output file - Siep
   //Version = "5.0.221"; // 07/01/08 - dts0100481723 - Orbotech_out.cpp - A new opotion is added to export through hole pins in Orbotech output file is fixed - Siep
   //Version = "5.0.220"; // 06/24/08 - dts0100497448 :- EDIF Import - propertyOverride not updating original property string - bkalyan
   //Version = "5.0.219"; // 06/24/08 - dts0100503912 - Protel PCB Read - Soldermask expansion not correct - zpiecuch
   //Version = "5.0.218"; // 06/23/08 - dts0100495827 - MentorIn.cpp - CAMCAD Boardstation Import - CAMCAD hangs when reading geoms.ascii file problem fixed - Siep
   //Version = "5.0.217"; // 06/23/08 - Edif200_in.cpp - dts0100466742 - EDIF200  file from Orcad not importable into CAMCAD - bkalyan
   //Version = "5.0.216"; // 06/20/08 - DcaPnt.cpp - Vectorize() functin was not working for negative delta angle, i.e. for clockwise arcs - rcf
   //Version = "5.0.215"; // 06/18/08 - TakayaFPTWrite ?Fixed error in backward compatible output of 2nd field for Resistor tests - Siep
   //Version = "5.0.214"; // 06/17/08 - New Takaya Writer ?Fixed two bugs in conversion from old to new takaya writer - Siep
   //Version = "5.0.213"; // 06/16/08 - dts0100502158 - Accel/PCAD Import - PlaneObj constructs contains Width which CAMCAD ignores - zpiecuch
   //Version = "5.0.212"; // 06/13/08 - dts0100502346 - Ag_AOI_o.cpp - A New option is added in .out file to supress creation of TPP files - Siep
   //Version = "5.0.211"; // 06/10/08 - dts0100495852 - Takaya Exporter - User programmable sensor diameter options added in takaya.out file - Siep
                        //          - dts0100495798 - Takaya Exporter - A New option is added to create IC Diode tests even if IC Opens test is created - Siep
   //Version = "5.0.210"; // 06/06/08 - dts0100475106 - Vb99 Out - Polys on Power Negative layer really shrink, all else continues to Expand as before - rcf
                          // Also includes this fix from 4.6: Version = "4.6.465"; // 06/06/08 - dts0100488930 - correction to fix - zpiecuch
   //Version = "5.0.209"; // 06/06/08 - dts0100501008 - QD Writer - Panel fid location - Next step improvement, now handles qd preparer rotation - rcf
   //Version = "5.0.208"; // 06/04/08 - dts0100502626 - Vb99 Out - Fixed text_type for output of RefDes and $$DEVICE$$ - rcf
   //Version = "5.0.207"; // 06/04/08 - dts0100493993 - Stencil Gen - Rebug: Fixed propagate for INHERIT FROM XXX button - rcf
   //Version = "5.0.206"; // 06/04/08 - dts0100454286 - CadifIn.cpp - Request to add .MakeInntoTooling command in Cadif.In file is fixed - Siep
   //Version = "5.0.205"; // 06/04/08 - dts0100492589 - Ag_AOI_o.cpp - An option to suppress unloaded components from being written to the SJ AOI output format by adding a new commmand is fixed - Siep
   //Version = "5.0.204"; // 06/04/08 - Klocwork - pdifOut.cpp - Fixed Klocwork issues - Michael
                        //          - Klocwork - pfw_Out.cpp - Fixed Klocwork issues - Michael
   //Version = "5.0.203"; // 06/04/08 - dts0100502349 - Accel/PCAD import - Global Swell not being applied to Via geometries - zpiecuch
                        //          - dts0100499256 - Accel/PCAD 200x import issue - PolyCutout not being imported - zpiecuch
                        //          - dts0100488930 - Expedition Output - Negative Plane cutout translating as copper pour - should be obstruct - corrections according to new notes in CQ - zpiecuch
   //Version = "5.0.202"; // 06/03/08 - dts0100467613 - AccelIn - Fixed handling of bulge in polylines, was losing value, turning curves into straight segments - rcf
   //Version = "5.0.201"; // 06/03/08 - dts0100500082 - Stencil Gen - Improve SG rule loading on visula databases, map old DB padstack name based rules to pin numbers for new DB convention - rcf
                          //          - dts0100500083 - Stencil Gen - Improve SG rule loading on CR5K databases, perform "guess work" mapping of old rules to new DB conventions based on currently loaded ccz - rcf 
   //Version = "5.0.200"; // 06/03/08 - dts0100499636 - CAMCAD Accel/PCAD 200x Read - Need accel.in file command for choice between pcbPoly and Island+CutOut - corrections according to new notes - zpiecuch
                          //          - dts0100488930 - Expedition Output - Negative Plane cutout translating as copper pour - should be obstruct - corrections according to new notes in CQ - zpiecuch
                          //          - dts0100499256 - Accel/PCAD 200x import issue - PolyCutout not being imported - zpiecuch
   //Version = "5.0.199"; // 05/29/08 - dts0100501008 - QD Writer - Panel fid location, ECKE "offset" is not applied to Panel fids by QD machine, so need to add ECKE value to Panel fid locations in output - rcf
   //Version = "5.0.198"; // 05/29/08 - dts0100501551 - QD Writer, but once again, not really; GenerateGeometryDesignSurfaceAttributes() fixed to handle wider range of surface layer types, to determine fid surface - rcf
   //Version = "5.0.197"; // 05/29/08 - dts0100499636 - CAMCAD Accel/PCAD 200x Read - corrections to fix according to notes in CQ - zpiecuch
   //Version = "5.0.196"; // 05/27/08 - dts0100490600 - ThedaIn.cpp - Complex Rectangle shapes to simple apertures is fixed - Siep
   //Version = "5.0.195"; // 05/26/08 - dts0100499636 - CAMCAD Accel/PCAD 200x Read - Need accel.in file command for choice between pcbPoly and Island+CutOut - zpiecuch
   //Version = "5.0.194"; // 05/22/08 - dts0100496117 - Vb99 In - Fixed rotation for mirrored pcb insert in panel, CAMCAD mirrors about Y, Expedition mirrors about X - rcf
   //Version = "5.0.193"; // 05/21/08 - dts0100496016 - Vb99 In - Support Rect and Oblong Thermal pads, "regular" and "45" versions, Fixed aspect ratio consideration, was only working for width (x) >= length (y), now also works for length > width - rcf
   //Version = "5.0.192"; // 05/20/08 - Klocwork - ThedaIn.cpp - Fixed Klocwork issues - Michael
                          //          - Klocwork - Trdn73_o.cpp - Fixed Klocwork issues - Michael
                          //          - Klocwork - Triaoi_o.cpp - Fixed Klocwork issues - Michael
   //Version = "5.0.191"; // 05/20/08 - Klocwork - Ag_AOI_o.cpp - Fixed Klocwork issues - Siep
                          //          - Klocwork - Aeroflex_Out.cpp - Fixed Klocwork issues - Siep
                          //          - Klocwork - Api.cpp - Fixed Klocwork issues - Siep
                          //          - Klocwork - Api_Find.cpp - Fixed Klocwork issues - Siep
                          //          - Klocwork - Api_Get.cpp - Fixed Klocwork issues - Siep
   //Version = "5.0.190"; // 05/16/08 - Klocwork - DcaPolyLib.cpp   - Fixed Klocwork issues - rcf
                          //          - Klocwork - DcaPolygon.cpp - Fixed Klocwork issues - rcf
                          //          - Klocwork - DcaPnt.cpp     - Fixed Klocwork issues - rcf
                          //          - Klocwork - DcaLayer.cpp   - Fixed Klocwork issues - rcf
                          //          - Klocwork - DcaStandardAPerture.cpp/.h   - Fixed Klocwork issues - rcf
   //Version = "5.0.189"; // 05/16/08 - Klocwork - Select.cpp - Fixed Klocwork issues - rcf
                          //          - Klocwork - Search.cpp - Fixed Klocwork issues (some, possibly not all) - rcf
   //Version = "5.0.188"; // 05/16/08 - dts0100497457 - Vb99 Out - Fiducial and Tooling inserts not written to Layout.hkp file, corrections according to new notes - zpiecuch
                          //          - dts0100496657 - Vb99 Out - .obstruct content isn't written in proper construct of layout.hkp file, corrections according to new notes - zpiecuch
   //Version = "5.0.187"; // 05/15/08 - Klocwork - Sony_AIO_o.cpp - Fixed Klocwork issues - Siep
                          //          - Klocwork - fixture_out.cpp - Fixed Klocwork issues - Siep
                          //          - Klocwork - Spea_Out.cpp - Fixed Klocwork issues - Siep
                          //          - Klocwork - AllegIn.cpp - Fixed Klocwork issues - Siep
                          //          - Klocwork - AccelIn.cpp - Fixed Klocwork issues - Michael
   //Version = "5.0.186"; // 05/15/08 - dts0100488930 - Vb99 Out - Expedition Output - Negative Plane cutout translating as copper pour - should be obstruct - zpiecuch
                          //          - dts0100496657 - Vb99 Out - .obstruct content isn't written in proper construct of layout.hkp file - zpiecuch
   //Version = "5.0.185"; // 05/13/08 - dts0100496016 - Vb99 In - Support Rect and Oblong Thermal pads, "regular" and "45" versions - rcf
                          //          - dts0100496117 - Vb99 In - Fixed rotation for mirrored pcb insert in panel - rcf
   //Version = "5.0.184"; // 05/13/08 - dts0100496657 - VB99 Out - .obstruct content isn't written in proper construct of layout.hkp file - zpiecuch
                          // 05/13/08 - dts0100497457 - VB99 Out - Fiducial and Tooling inserts not written to Layout.hkp file - zpiecuch
   //Version = "5.0.183"; // 05/09/08 - dts0100496592 - CKT Writer - Fixed the pin mapping being applied to all device types in CKT exporter when it should not be - Michael
   //Version = "5.0.182"; // 05/08/08 - dts0100496261 - Stencil Gen - Added acceptance of layers types PadAll and SignalAll as surface copper layers to new class CPadStackInsert added circa 18 Dec 07 - rcf
   //Version = "5.0.181"; // 05/07/08 - dts0100489475 - EDIF 300 In + Sch_Lib.cpp - Fixed two issue for import and crosslinking re multiple uses of same refname, identified two more issues that are now postponed in same case - rcf
   //Version = "5.0.180"; // 05/07/08 - dts0100468710 - Orcad In - Fixed the incorrect place surface for components - Michael
                          // 05/07/08 - dts0100493382 - Orcad In - Fixed the Bottom built geometries display issue - Michael    
   //Version = "5.0.179"; // 05/05/08 - dts0100488078 - VB99 In - Stop default THIEVING_DATA to FILL=TRUE, honor data as it's found in data file - rcf
   //Version = "5.0.178"; // 05/02/08 - dts0100489560 - VB99 In - Fixed import of Panel Outline and Border Outline, also automatically make PanelOutline layer have layer type PANEL_OUTLINE, prev fix for dts100461892 was causing these outline to import as Hidden rcf
   //Version = "5.0.177"; // 04/29/08 - Matsuno-san issue S-15 - Applied same 0 & 90 apparant rotation limit (as done for attirbute values) to a symbol insert's TEXT items - rcf
   //Version = "5.0.176"; // 04/29/08 - dts0100480778 - Pads Reader - Fixed error in POUR polylist collector, was making multiple polylists when all polys should have been in one list - Michael
   //Version = "5.0.175"; // 04/24/08 - dts0100481706 - Pads Reader - The variants are not being created correctly when there is a part substitution - Michael
   //Version = "5.0.174"; // 04/23/08 - dts0100466667 - Spectrum Writer - During net name polishing and mapping, favor net names that do not have to change over those that do, if a polished name collides with existing name, keep existing name and alter polished name - rcf
                          //          - EDIF 200 Read - Set nevermirror on ALL text, customer in Japan seems to want it so - rcf
   //Version = "5.0.173"; // 04/22/08 - Klocwork - Trdn73_o.cpp - Fixed Klocwork issues - Michael
                          //          - Klocwork - Teradn_o.cpp.cpp - Fixed Klocwork issues - Michael
                          //          - Klocwork - Clr_Attr.cpp - Fixed Klocwork issues - Michael
                          //          - Klocwork - CEdit.cpp - Fixed Klocwork issues - Michael
                          //          - Klocwork - Centroid.cpp - Fixed Klocwork issues - Michael
   //Version = "5.0.172"; // 04/22/08 - Vb99 Out - Synchronized 5.0 and 4.6, made source sharable between both - rcf
   //Version = "5.0.171"; // 04/21/08 - dts0100466667 - Spectrum Read - CAMCAD allows case sensitive net names but the Spectrum software does not. Spectrum exporter needs to rename lower case nets. Change the duplicated name if there is any. - Michael
   //Version = "5.0.170"; // 04/19/08 - Klocwork - Lyr_Pen.cpp - Fixed Klocwork issues - rcf
                          //          - Klocwork - Lyr_Manip.cpp - Fixed Klocwork issues - rcf
                          //          - Klocwork - Lyr_Stack.cpp - Fixed Klocwork issues - rcf
                          //          - Klocwork - Lyr_Stackup.cpp - Fixed Klocwork issues - rcf
   //Version = "5.0.169"; // 04/19/08 - Klocwork - Lyr_File.cpp - Fixed Klocwork issues - rcf
   //Version = "5.0.168"; // 04/19/08 - Klocwork - Layer.cpp - Fixed Klocwork issues - rcf
   //Version = "5.0.167"; // 04/19/08 - Klocwork - DftDialog.cpp - Fixed Klocwork issues - rcf
   //Version = "5.0.166"; // 04/19/08 - Klocwork - General.cpp - Fixed Klocwork issues - rcf
   //Version = "5.0.165"; // 04/19/08 - Klocwork - EditPoly.cpp - Fixed Klocwork issues - rcf
   //Version = "5.0.164"; // 04/18/08 - No case# - CcDoc.cpp - Fixed Klocwork issues - rcf
   //Version = "5.0.163"; // 04/18/08 - dts0100466667 - Spectrum NailReq Read - Add support for finding nets based on attribute SPECTRUM_NETNAME - rcf
   //Version = "5.0.162"; // 04/18/08 - dts0100482665 - EDIF 200 In - Fixed attribute value rotation, particularly related to mirroring, new IN file command .LIMIT_ATTRIB_ROTATION_0_90 - rcf
   //Version = "5.0.161"; // 04/17/08 - dts0100490111 - Accel/PCAD 200x read - default padstack instance gets P: prefix but padstack geometry name doesn't have prefix - zpiecuch
                          // 04/17/08 - dts0100490154 - Accel/PCAD 200x import - RndRect shape types not rounded rectangles after import - zpiecuch
                          // 04/17/08 - dts0100490116 - Accel/PCAD import - MtHole padShapeType import change and correct handling of localSwell - zpiecuch
   //Version = "5.0.160"; // 04/16/08 - dts0100489798 - Variant - Add Update Variant function to Variant Manager - Michael
   //Version = "5.0.159"; // 04/14/08 - dts0100466083 - GenCadIn - Add "MAKE_INTO" commands in the gencad.in file and PcbUtil2 files - Michael
   //Version = "5.0.158"; // 04/14/08 - dts0100482444 - Fixed newline in attrib value to be handled like newline in text insert, so newline no longer lost nor attrib split to mulitple attribs - rcf
   //Version = "5.0.157"; // 04/14/08 - dts0100489387 - Konrad ICT Writer - New option in konrad.out file to remove tests that do not have accessible pins - Michael
   //Version = "5.0.156"; // 04/14/08 - dts0100488936 - VB99 Out - Expedition Output - Negative Plane layer polyline not written as OBSTRUCT - zpiecuch
                          //          - dts0100488930 - VB99 Out - Expedition Output - Negative Plane cutout translating as copper pour - should be obstruct - zpiecuch
   //Version = "5.0.155"; // 04/11/08 - dts0100394971 - PfWin.cpp - For some components, CAMCAD shows this on the top while this same feature in Protel shows on the bottom - Michael
   //Version = "5.0.154"; // 04/11/08 - dts0100488978 - AccelIn.cpp - Global Swell not being applied per side but as a single value increment - zpiecuch
   //Version = "5.0.153"; // 04/08/08 - dts0100487384 = VB99 In - Separate Conductive Area import from USE_GENERATED_DATA, no more hidden, always honor filled flag - rcf
   //Version = "5.0.152"; // 04/08/08 - dts0100451354 - Fix the incorrection of calculating oblong finger pad dimension. - Michael
   //Version = "5.0.151"; // 04/08/08 - correction to dts0100475106 - Vb99Out - now polys that have closed or filled parameters set or lies on power planes are treated as routes and are not adjusted - zpiecuch
   //Version = "5.0.150"; // 04/04/08 - No case# - DcaNet.cpp ?Fixed 4.6 to 5.0 DCA migration error involving comp pin location status - Michael
   //Version = "5.0.149"; // 04/02/08 - dts0100482665 - EDIF 200 In - Fixed text rotation, had bad floating point math for mirrored and rotated text - rcf
                          //          - dts0100486526 - EDIF 200 In - Fill circles used as connect dots in nets - rcf
   //Version = "5.0.148"; // 04/02/08 - Add a new writer, FabmasterFATFWriter, into the version 5.0 and made it compatible with 4.6
                          // 04/02/08  - Add null pointer checking and uninitialized variable initialization in AccelIn.cpp, Accel_Out,cpp, ckt_nav_out.cpp   - Michael
   //Version = "5.0.147"; // 03/27/08 - Data Doctor - Custom package outline templates, SO and SOT23 added - rcf
   //Version = "5.0.146"; // 03/25/08 - dts0100472686 - Konrad ICT Writer - Fixed the incorporate parallel analysis results. Add two methods to get merged status and value.
                          // 03/25/08 - dts0100419370 - Konrad ICT Writer - Add a method to get the partnumber to the second field. 
                          // 03/25/08 - Konrad ICT Writer - Change to version5.0 compatible
                          // 03/26/08 - dts0100474994 Add a konrad.out file with a command to control the exporting of loaded or unloaded parts. - Michael
   //Version = "5.0.145"; // 03/25/08 - No case# - Boardstation Men_Out.cpp - Stop writing $$create_generic_part, use $$create_component - rcf
   //Version = "5.0.144"; // 03/25/08 - dts0100446813 - Vb99Out - Assign names to un-named apertures that are inserted (as pads), only "pure width" apertures should be unnamed - rcf
   //Version = "5.0.143"; // 03/21/08 - Data Doctor - Custom Package Outline Editing - rcf
   //Version = "5.0.142"; // 03/20/08 - dts0100477341 - Vb99In - Fixed negative pad handling, was only working once per padstack type - rcf
   //Version = "5.0.141"; // 03/19/08 - dts0100478796 - HP3070 Write - Fixed SHOWOUTLINE control, Made separate normal and pinlib PN controls, extend to use arbitrary attribs in PN - rcf
   //Version = "5.0.140"; // 03/17/08 - dts0100480642 - Report.cpp - Fixed comps.csv report, messed up string compare caused reversal of SMD and THRU - rcf
                          //          - dts0100479377 - Fixture File Export - Fixed crash - Bad assumption that probe template would always be found - rcf
   //Version = "5.0.139"; // 03/10/08 - dts0100461803 - VB99 HKP In - Fixed position of round/octogonal end on finger pads, position depends on aspect ratio - rcf
   //Version = "5.0.138"; // 03/10/08 - dts0100466362 - FATF In - Fixed net processing, graphic type Etch and Netname attrib - rcf
   //Version = "5.0.137"; // 03/10/08 - dts0100478773 - CAMCAD PCB Translator - Expedition Output - Adjust layout.hkp for TEXT_LYR on display attributes - zpiecuch
                          //          - no dts        - change in DcaAttributes.cpp file - CAttribute::getPenWidthIndex() doesn't return negative value anymore - zpiecuch
   //Version = "5.0.136"; // 03/07/08 - dts0100478999 - Aeroflex Out - Small adjustments to format and content - rcf
   //Version = "5.0.135"; // 03/07/08 - dtsXXX (multiple) - Updated to latest BoardStation Writer (MEN_Out.cpp) and HKP Reader (VB99In.cpp) from collection of fixes in 4.6 - rcf
   //Version = "5.0.134"; // 03/07/08 - dts0100476398 - CAMCAD PCB Translator - Expedition Output - undefined pads referenced within padstack definition - zpiecuch
   //Version = "5.0.134"; // 03/04/08 - dts0100465000 - CC Navigator - Better detail management when user modifies probe placement in Navigator (access marker side, test_access attrib on target, etc) - rcf
                          //          - CamCad.cpp - Updated release build copy target to new server, \\Cob\dfs\sms-build - rcf
   //Version = "5.0.133"; // 03/04/08 - dts0100476993 - Draw.cpp, Fixed crash, failure to abandon operation after NULL block check - rcf
   //Version = "5.0.132"; // 02/29/08 - dts0100467613 - Accel/PCAD 200x Read - complex pad shapes are now generated according to swell parameter - zpiecuch
                          // 02/29/08 - dts0100472320 - Accel/PCAD 200x import - fixed - zpiecuch
                          // 02/29/08 - dts0100467615 - Expedition VBASCII Write - duplicate names changed before export and changed back after - zpiecuch
   //Version = "5.0.131"; // 02/26/08 - dts0100449862 - Zuken PWS Read - aka Cr3000In - Fixed to recognize and skip 3 new keywords - rcf   
   //Version = "5.0.130"; // 02/26/08 - dts0100460108 - Allegro Write - Fixed output of testpoints, was in net list but missing from packages, also fixed output file path, was ignoring folder in file chooser and always writing to where input (ccz, cad) came from - rcf
   //Version = "5.0.129"; // 02/20/08 - dts0100472902 - Stencil Gen DB - Only update DB record if rule data actually changed - rcf
   //Version = "5.0.128"; // 02/20/08 - dts0100475106 - Vb99Out.cpp - CAMCAD PCB Translator - Expedition Output - non zero width plane areas need adjusting - zpiecuch
                          //          - dts0100470008 - Vb99Out.cpp - correction to fix, ..TEXT have only one parameter when the TEXT_TYPE is USER_TYPE - zpiecuch
   //Version = "5.0.127"; // 02/19/08 - dts0100469644 - Mentor Boardstation Write - Normalize degrees in comps.comp output - rcf
   //Version = "5.0.126"; // 02/19/08 - dts0100460112 - Allegro Write - Was not resetting a map, causing 2nd round of output to think it was already done - rcf
   //Version = "5.0.125"; // 02/19/08 - dts0100472908 - Stencil Gen - Split was not accounting for line width effect on Region Poly during split, line width is dictated by corner radius - rcf 
   //Version = "5.0.124"; // 02/19/08 - dts0100470008 - Vb99Out.cpp - Expedition Write: Display_Attr not allowed on text of type Property_Pair - zpiecuch
                          //          - dts0100467640 - Vb99Out.cpp - Expedition VBASCII Write, Drawing Cells cannot have certain TEXT_TYPE entries - zpiecuch
   //Version = "5.0.123"; // 02/15/08 - dts0100470032 - Accel/PCAD 200x Read - layer type changed from layerTypePlaneClearance to layerTypePowerNegative in AccelIn.cpp file - zpiecuch
   //Version = "5.0.122"; // 02/14/08 - dts0100472921 - Fixture file alignment improved, now makes multiple attemps using different comp pin pairs - rcf
   //Version = "5.0.121"; // 02/13/08 - Data Doctor - Package Outlines - Fixed alignment problem (error in pin centroid calc) - rcf
   //Version = "5.0.120"; // 02/12/08 - Data Doctor - Package Outlines - Implemented DB Save/Load and Fixed alignment of outline to pin set centroid - rcf
   //Version = "5.0.119"; // 02/08/08 - Access Analysis - Multiple Reason Codes - Fixed management of NO SOLDER MASK error, was getting it when not appropriate - rcf
   //Version = "5.0.118"; // 02/07/08 - Access Analysis - Multiple Reason Codes - Snapshot - Basic functionality in place, CCZ I/O and backward compatibility not ready - rcf
   //Version = "5.0.117"; // 01/28/08 - dts0100468646 - Geometry Consolidator - Conversion of complex aperture rectangles to simple apertures now restricted to those with origin at center (no offset) - rcf
   //Version = "5.0.116"; // 01/27/08 - dts0100469758 - PCB Translator - Expedition Writer - Purge unused geometries before export proceeds - rcf
   //Version = "5.0.115"; // 01/25/08 - dts0100465858 - Probe Placement - Fixed retention of manually placed probes, was loosing probes when access marker had blank refname - rcf
   //Version = "5.0.114"; // 01/24/08 - Data Doctor - Performance improvement in DD startup, related to DD Load and Parts tab - rcf
                          //          - DcaFile.cpp - Fixed bug in m_nextFileNumber management, failure mode was when there were skips in file numbers, e.g. first file start at 2, in turn causing CFileList::addNewFile to create a new file with an existing file number, then crash if one or the other is removed because block ownership got mixed up - rcf
   //Version = "5.0.113"; // 01/23/08 - Data Doctor - Added UserID column to Generic Attribs DB table - rcf
   //Version = "5.0.112"; // 01/23/08 - dts0100460137 (related/rebug) - Fixture Reuse - Fixed Teradyne D2B bottom (only) fixture file processing, got messed up when adding Fabmaster fixture support - rcf
   //Version = "5.0.111"; // 01/21/08 - Data Doctor - Custom Attribs - Snapshot - SPart grid column re-order handling fix - rcf
   //Version = "5.0.110"; // 01/21/08 - Data Doctor - Custom Attribs - Snapshot - Supported added to Parts tab, disabled in Packages tab - rcf
   //Version = "5.0.109"; // 01/18/08 - Panilizer - Progress checkin - knv
   //Version = "5.0.108"; // 01/17/08 - Data Doctor - Custom Attribs - Snapshot - Database interaction in place - rcf
   //Version = "5.0.107"; // 01/16/08 - Data Doctor - Custom Attribs - Snapshot - Custom attrib cells filled and saved - rcf
   //Version = "5.0.106"; // 01/16/08 - dts0100466055 - Stencil generator saved pin rules not being recalled properly in new CAMCAD 4.6 - knv
                          //          - dts0100466322 - Stencil Generator - customer requests enhancement to only create stencil pad for TH parts on mount surface - knv
                          //          - dts0100466334 - Stencil Generator - view in dialog is incorrect for bottom side geometry when both top and bottom surface have different rules for the same geometry - knv
   //Version = "5.0.105"; // 01/16/08 - Data Doctor - Custom Attribs - Snapshot - Fundemental column management - rcf
   //Version = "5.0.104"; // 01/15/08 - dts0100465295 - CADIF In - Fixed crash, yet another IN file control combination crashed - rcf
   //Version = "5.0.103"; // 01/14/08 - dts0100465295 - CADIF In - Fixed crash, arguably data is bad, referenced PACKALT does not exist - rcf
                          //          - dts0100466087 - Data Doctor - Fixed crash in Centroids tab, when clicking in white space below grid, wasn't handling row == -1 - rcf
   //Version = "5.0.102"; // 01/11/08 - Data Doctor - New Library tab roughed in, two new files added to project - rcf
   //Version = "5.0.101"; // 01/11/08 - dts0100466055 - Stencil generator saved pin rules not being recalled properly in new CAMCAD 4.6 - knv
   //Version = "5.0.100"; // 01/10/08 - dts0100462379 - GenCad_o.cpp - Added option to treat mechanical pins same as regular pins on component - rcf
   //Version = "5.0.99";  // 01/05/08 - dts0100460768 - Boardstation Read - MentorIn.cpp - ReBug - Needed better separation of electrical versus physical layer manipulation - rcf
                          //          - dts0100462084 - Boardstation Read - MentorIn.cpp - New IN file command to control PARTNUMBER attrib generation - rcf
   //Version = "5.0.98";  // 01/08/08 - dts0100460768 - Boardstation Read - MentorIn.cpp - Set layer phsy stack num to match layer elect stack num - rcf
   //Version = "5.0.97";  // 01/05/08 - Stencil Generator - Updated 5.0 files from modified 4.6 files - knv
   //Version = "5.0.96";  // 01/04/08 - HP 3070 Write - Hp3070_o.cpp - Added 4.6 Patch 1 feature to 5.0 - rcf
   //Version = "5.0.95";  // 01/04/08 - Teradyne Z1800 Write aka IPL - ipl_out.cpp - Added 4.6 Patch 1 feature to 5.0 - rcf
   //Version = "5.0.94";  // 01/04/08 - Teradyne NAR Read - NailAssignIn.cpp - Added 4.6 Patch 1 feature to 5.0 - rcf
   //Version = "5.0.93";  // 01/04/08 - Fixture Reuse - Fabmaster Fixture Files - Added 4.6 Patch 1 feature to 5.0 - rcf
   //Version = "5.0.92";  // 01/03/08 - Testability Report - Data Doctor - Database - Interface added to DD and DB for DPMO  - rcf
   //Version = "5.0.91";  // 01/03/08 - GetBoardSize() func - Report.cpp - Fixed determination of return value, needed to compare relative values, not relative positions in memory !  - rcf
   //Version = "5.0.90";  // 01/03/08 - Stencil Generator - Fixed problem with "orientation" modifier - added units to specific rows in "StencilSettings" table and "inset" rule in "StencilRules" table - knv
   //Version = "5.0.89";  // 01/01/08 - Stencil Generator - Fixed problem in migration code; surface was not being prepended to entity in rule string - knv
   //Version = "5.0.88";  // 12/29/07 - Stencil Generator - Updated stencil rule database interface and migration code - knv
   //Version = "5.0.87";  // 12/29/07 - Testability Report - Made Cancel button work - rcf
   //Version = "5.0.86";  // 12/28/07 - dts0100463922 - Add support of DXF Write for API::ExportFormat() - LP
   //Version = "5.0.85";  // 12/28/07 - Stencil Generator - Progress checkin - knv
   //Version = "5.0.84";  // 12/27/07 - dts0100462613 - Additional fixes to this case is for "Create Single Pin" function to update net of Test Access and Test Probe that was on "~Unused_Pin~" - LP
                          //          - dts0100462613 - Zuken CR5000 - Test access markers has invalid DataLink value after import and "~Ununsed_Pin~" net need to have the flag NETFLAG_UNUSEDNET set - LP
                          //          - dts0100437811 - Access Analysis - Added option for "Retain Existing Probes" - LP
   //Version = "5.0.83";  // 12/27/07 - Stencil Generator - Progress checkin - knv
   //Version = "5.0.84";  // 12/26/07 - Stencil Generator - Progress checkin - knv
   //Version = "5.0.82";  // 12/21/07 - Testability Report - Arguably complete, meets spec, polishing and embellishment shall no doubt continue awhile - rcf
   //Version = "5.0.81";  // 12/21/07 - Testability Report - GUI roughed in for OUT file overrides, only 1/2 connected so far - rcf
   //Version = "5.0.80";  // 12/18/07 - dts0100459163 - Expedition HKP vbascii Read - Was making bad comp pins, had blank pin name - rcf
   //Version = "5.0.79";  // 12/18/07 - Testability Report Writer - Update - rcf
   //Version = "5.0.78";  // 12/12/07 - dts0100459426 - Added ability for Components report to work with panel file - LP
   //Version = "5.0.77";  // 12/10/07 - 4.7 PRD Item 2.6.4 - Options to delete Access Markers and also Probes  -- Delete Access Point Top/Bottom/Both and DeleteTest Probe Top/Bottom/Both added to Tools DFT menu - rcf
   //Version = "5.0.76";  // 12/09/07 - New Takaya Write - Rewrote Takaya Writer and added High-Fly-Zone test and added generation of Repair File - LP
   //Version = "5.0.75";  // 12/07/07 - Testability Report Writer - New feature added to camcad - rcf 
   //Version = "5.0.74";  // 11/26/07 - dts0100451581 - QD Writer, but once again, not really; GenerateGeometryDesignSurfaceAttributes() used mirrorGraphic flag when should be using mirrorLayer flag, resulted in wrong side determination when mirrorGraphic is set but not mirrorLayer, as found in this case's fid data construction - rcf 
   //Version = "5.0.73";  // 11/21/07 - dts0100447220 - Stencil Generator - Fixed rule DB save problem when names involved have embedded dots (.) - rcf
   //Version = "5.0.72";  // 11/20/07 - dts0100452974 - Expedition HKP Reader - Fixed crash, was making recursive padstack inserts - rcf
                          //          - dts0100447290 - Expedition HKP Reader - Fixed handling of TEARDROP conductive area - rcf
   //Version = "5.0.71";  // 11/20/07 - dts0100455422 - GenCam Reader - Fixed crash, related to complex aperture creation using geom block that does not exist yet - rcf 
   //Version = "5.0.70";  // 11/19/07 - dts0100451971 - Fixture Writer & Excellon Writer - Fixed crash, same code for both (in fixture_out.cpp), bad assumption that access point would have a probe - rcf
   //Version = "5.0.69";  // 11/19/07 - dts0100453282 - Access Analysis - Exposed metal with non-zero width aperture is not properly exposed and therefore is not accessible - LP
   //Version = "5.0.68";  // 11/19/07 - dts0100452346 - Edif 200 & 300 Read - AutoDetectSchematicDesignator is not properly detecting files with logical symbol having the same names; therefore the ".in" command "DESIGNATOR_BACKANNOTATE" is used again.
                          //                             However, this time, users can set multiple "DESIGNATOR_BACKANNOTATE" command.  The first value, which is an attribute name, of the command found on a logical symbol will be use as the Designator, and 
                          //                             AutoDetectSchematicDesignator will be skip - LP
   //Version = "5.0.67";  // 11/15/07 - Data Doctor - Package Outlines - Pre-Alpha but largely functional - rcf
   //Version = "5.0.66";  // 10/30/07 - dts0100447684 - Orcad Reader - Fixed side determination for pcb components with no component outline - rcf
                          //          - dts0100447680 - Geometry Consolidator - Made components with no pins are exempt from consolidation - rcf
   //Version = "5.0.65";  // 10/23/07 - dts0100424872 - SPEA Out - Fixed vectorizing back-to-back arcs in polyline, was skipping every other bulge - rcf
   //Version = "5.0.64";  // 10/22/07 - dts0100440298 - DataDoctor & DeviceTyper - DeviceTyper left behind open file ptr to devices.stp, causing DataDoctor to silently refuse to read same and end up using defaults - rcf
   //Version = "5.0.63";  // 10/19/07 - dts0100440355 - Generic Centroid In - Delete window title code, use title from RC file, looked like leftover of clone or cut&paste of Gerber Educator source - rcf
   //Version = "5.0.62";  // 10/19/07 - dts0100443804 - TRI MDA Write - aka INGUN - Add test support for Diode_LED, Capacitor_Polarized, and Capacitor_Tantalum - rcf
   //Version = "5.0.61";  // 10/09/07 - dts0100437071 - Gerber Import - CAMCAD needs to support a change in Gerber format spec for polygon Aperture Macros - knv
                          //                           - rebug - fixed diameter calculation for rev D - knv
   //Version = "5.0.60";  // 10/09/07 - dts0100437071 - Gerber Import - CAMCAD needs to support a change in Gerber format spec for polygon Aperture Macros - knv
                          //                             - rebug - added .in file commands: 
                          //                                ".AM_Polygon_RevD <boolean value>"
                          //                                ".PolygonApertureMacroOriginType <'fromCircumCircle' | 'fromExtent'>"
                          //                                ".PolygonApertureMacroDiameterType <'fromCircumCircle' | 'fromInCircle' | 'fromExtentWidth'>"
                          //                                ".PolygonApertureOrientationZero <boolean value>"
                          //          - dts0100428894 - CAMCAD Pads Write: Need to add simple routing rules. - knv
                          //                             - rebug - via names in design rule via section are now updated using chk file .
                          //                                     - design rules section header changed from "RULE_UNIT" to "RULES_SECTION"
   //Version = "5.0.59";  // 10/05/07 - dts0100444740 - Fixed crash, deref of NULL attribute list - rcf
   //Version = "5.0.58";  // 10/04/07 - dts0100437071 - Gerber Import - CAMCAD needs to support a change in Gerber format spec for polygon Aperture Macros - knv
                          //                             - rebug - added .in file command: ".Primitive5UseCircumCircleDiameter <boolean value>"
                          //          - dts0100428894 - CAMCAD Pads Write: Need to add simple routing rules. - rebug - knv
                          //          - Fixed infinite loop bug in CDcaXmlReader::ENTBODY() when "&#10;" was encountered in a .cc file - knv
   //Version = "5.0.57";  // 10/02/07 - dts0100441046 - QD sort of, but really BlockStruct::GenerateGeometryDesignSurfaceAttributes, added acceptance of mechanical pins for test, along with "regular" pins it already did; was failing to classify fids in this data as top or bottom - rcf
                          //          - dts0100434388 - Cadence Allegro Read - Support complex padshapes in fabmaster extracts and in new 5th file in camcad extracts - rcf
                          //          - dts0100419984 - Geometry Consolidator - Sidestep mind boggling peculiarity involving normalizeDegrees() function - rcf
   //Version = "5.0.56";  // 09/26/07 - dts0100414160 - Rotation Angle in Status Bar does not go away - rebug - knv
                          //          - Gerber Reader - Changed default for .in file option, .GenerateComplexThermalApertures, from false to true - knv
   //Version = "5.0.55";  // 09/25/07 - dts0100424118 - CAMCAD Stencil Generator Pin Report not properly calculating and displaying vloume information - knv
                          // 09/25/07 - dts0100441532 - Zuken Board Designer Read, Layer mirroring and Geometry rectification broken when panel contains mirrored board. - knv
   //Version = "5.0.54";  // 09/21/07 - Mentor In - Fixed reader's violation of complex aperture and ap's sublock file number alignment - rcf
   //Version = "5.0.53";  // 09/21/07 - No Case - Exact Access Date set to Oct 05 2007, by directive from Moustafa - rcf
                          //          - dts0100434438 - Mentor In - Fixed processing of asymmetrical pads, was using name-based recognition of bottom layer which turned out unreliable, now using electrical stackup number - rcf      
   //Version = "5.0.52";  // 09/19/07 - dts0100376630 - Zuken Board Designer Read - Need to detect when the reverse footprint should be used. - knv
                          //          - dts0100437054 - Gerber Import - AM / AD definitions in Gerber call for aperture rotation that CAMCAD ignores
                          //          - dts0100437071 - Gerber Import - CAMCAD needs to support a change in Gerber format spec for polygon Aperture Macros
                          //          - dts0100432102 - Gerber import not correct with supplied file 
                          //          - dts0100424057 - Boardstation Write multi-line text string support
                          //          - dts0100414160 - Rotation Angle in Status Bar does not go away 
                          //          - dts0100419754 - Auto Cad Open function not working with zuken pcf data
                          //          - dts0100433524 - Zuken Board Designer Read, chip pad missing from pin IN4
                          //          - dts0100432140 - OpenCADFile API needs to better handle these formats (see description)
                          //          - dts0100428894 - CAMCAD Pads Write: Need to add simple routing rules.
                          //          - dts0100438608 - 3 new commands for fatf.in configuration file
   //Version = "5.0.51";  // 09/19/07 - dts0100429248 - Fixed failure to process ref des text, failure to create visible refname attrib - rcf 
   //Version = "5.0.50";  // 09/17/07 - dts0100415101 - Fixed scaling of DRC list when page units setting is changed - rcf
   //Version = "5.0.50";  // 09/14/07 - dts0100425301 - VB99 aka HKP Reader - Fixed text justification parsing and processing, adjusted text aspect ratio - rcf
   //Version = "5.0.49";  // 09/32/07 - dts0100422405 - PADS Reader - Fixed assignment of pin refnames, 2007 format has pin names in different place, was defaulting to in (losing alpha) causing comppin mismatch and failure to assign net to pin - rcf
   //Version = "5.0.48";  // 09/12/07 - dts0100434811 - Fixed API GetGeometryData for block type = blockTypeDrillHole, it was only returning bogus extents and no useful drill info - rcf
   //Version = "5.0.47";  // 09/11/07 - dts0100428976 - ODB++ Reader - Fixed aperture name parsing to better distinguish between ODB++ Standard Aps and complex (externally defined) aps - rcf
                          //          - dts0100417515 - ODB++ Reader - Fixed failure to init dimensions and rotation when aperture name has digit as first character - rcf
   //Version = "5.0.46";  // 09/07/07 - dts0100422922 - Mentor Neutral Reader - Fixed handling of TEST attrib for CompPins - rcf
   //Version = "5.0.45";  // 09/06/07 - dts0100419062 - Theda.in file - .mirrorlayer command not creating a layer and setting up mirroring. - knv
                          // 09/06/07 - Added reference to new library, DcaVeriBestAsciiReader.lib - knv
   //Version = "5.0.44";  // 09/04/07 - Added new Dca Reader, DcaVeriBestAsciiReader, with refactoring to support it - knv
   //Version = "5.0.43";  // 09/03/07 - dts0100418707 - Bitmap creation from CAMCAD issues - Background color not black - knv
                          //          - dts0100422054 - CR5000 not setting Primary Board Outline polyline classification on board outline; Primary Panel Outline on panel outline - knv
                          //          - dts0100422057 - Reading a CR5000 file locked by a text editor crashes CAMCAD - knv
                          //          - dts0100423630 - Zuken CR5000 file error during import - empty project window after import, no file loaded - knv
                          //          - dts0100422601 - Implement COMPHEIGHT on parts in CR5000 read - knv
   //Version = "5.0.42";  // 08/23/07 - dts0100431536 - Library.cpp - Fixed crash - There is a lingering issue here, VERY BAD usage of CString::GetBuffer() - rcf
   //Version = "5.0.41";  // 08/23/07 - dts0100431844 - Data Doctor - Centroids tab, Fixed centX and centY column titles and order, they were swapped - rcf 
   //Version = "5.0.40";  // 08/20/07 - Update to match: Version = "4.6.300"; 
                          // 08/20/07 - dts0100432780 - User data exceeded ccz output line length limit (again) 
                          //                          - Temp fix is to increase buffer, the case is staying active so 5.0 will get new code to selectively remove limit - rcf
   //Version = "5.0.39";  // 08/17/07 - dts0100387309 - Fixed automated attribute assignment, it missed handling of VT_UNIT_DOUBLE in one place, also made Attr_Ass.cpp 4.6/5.0 sharable - rcf
   //Version = "5.0.38";  // 08/16/07 - dts0100430577 - Allegro In - Fixed processing of .PANELSYMBOL -rcf
   //Version = "5.0.37";  // 08/16/07 - dts0100428899 - PADS Out - Fixed scaling, some output was being scaled twice - rcf
                          //          - dts0100393737 - CAD File Open - Fixed file-not-found error detection and reporting - rcf
   //Version = "5.0.36";  // 08/01/07 - dts0100425293 - CCM Writer - Aperture offset normalized for rotation - rcf
                          //          - dts0100425222 - CCM Writer - All rotations normalized to 0 .. 359.9 - rcf
   //Version = "5.0.35";  // 07/30/07 - dts0100423258 - Konrad Writer - Must use lower case k for Kilo (match 4.6 patch update) - rcf
   //Version = "5.0.34";  // 07/30/07 - Zuken Reader - Synchronized with 4.6 - added compiler directives to allow the same code to be used for both 4.6 and 5.0 - knv
   //Version = "5.0.33";  // 07/27/07 - Geometry Consolidator - Updated to match: Version = "4.6.291"; 
                          // 07/27/07 - dts0100419984 - (Adjustments to geometry name scrubbing) - rcf
   //Version = "5.0.32";  // 07/27/07 - Significant refactoring to width table related classes and usage - knv
   //Version = "5.0.31";  // 07/26/07 - Geometry Consolidator - Updated to match: Version = "4.6.290"; 
                          // 07/26/07 - dts0100419984 Part II (part I still pending) - Geometry Consolidator - FIxed overzealous simplificatino of rectangular complex apertures,
                          //                          was loosing Rounded Rectangle made by thick pens when converting to simple rectangle, can't convert such to simple apertures - rcf
   //Version = "5.0.30";  // 07/24/07 - dts0100422058 - Manufacturing Report crash fixed (match Version = "4.6.289) - rcf
   //Version = "5.0.29";  // 07/23/07 - FATF Reader - Updated to match:  Version = "4.6.288"; // 07/23/07 - Case dts0100417492 - FATF Reader - Fixed crash - rcf
   //Version = "5.0.28";  // 07/17/07 - Refactor - Added CAttributeNode and collections classes - knv
   //Version = "5.0.27";  // 07/09/07 - Refactor - Privatized DataStruct and CDataList copy constructors - knv
   //Version = "5.0.26";  // 06/30/07 - Refactor - FileStruct class members, entity numbers - knv
   //Version = "5.0.25";  // 06/21/07 - Refactor - CFileList and FileStruct - knv
                          //            Refactor - Added parameters to callers tree for vectorize() functions.
   //Version = "5.0.24";  // 06/20/07 - Re-merge pads.cpp for 4.6 case 413831 changes - rcf
   //Version = "5.0.23";  // 06/19/07 - version.cpp, version_resource.rc - Updated company name, version, copyright date - rcf
   //Version = "5.0.22";  // 06/17/07 - Refactor - Removed data members from CCEtoODBDoc class: ValueArray, maxValueArray, ValueDictionary - knv
                          //            Refactor - Renamed Attrib to CAttribute, typedefed Attrib for backward compatibility.
   //Version = "5.0.21";  // 06/15/07 - Refactor - Added m_camCadData member to Attrib class - knv
   //Version = "5.0.20";  // 06/14/07 - Significant changes to fix ccz read/write - Removed library subdirectories: dcaLib, UltimateGrid2005 - knv
   //Version = "5.0.19";  // 06/04/07 - Label after checkin for 4.6/5.0 sync #8 - LP
   //Version = "5.0.18";  // 05/29/07 - Label after checkin for 4.6/5.0 sync #7 - LP
   //Version = "5.0.17";  // 05/25/07 - Label after checkin for 4.6/5.0 sync #6 LP
   //Version = "5.0.16";  // 05/22/06 - Label after checkin for 4.6/5.0 sync #5 - LP
   //Version = "5.0.15";  // 05/22/06 - Label after checkin for 4.6/5.0 sync #4 - LP
   //Version = "5.0.14";  // 05/22/06 - IT LINKS! - Label after checkin for 4.6/5.0 sync #3 - rcf
   //Version = "5.0.13";  // 05/21/06 - Label after checkin for 4.6/5.0 sync #2 - knv
   //Version = "5.0.12";  // 05/21/06 - Label after checkin for 4.6/5.0 sync #1 - knv
   //Version = "5.0.11";  // 05/18/06 - Label before checkin for 4.6/5.0 sync - knv
   //Version = "5.0.10";  // 03/22/06 - Checkin after successfull build - knv
   //Version = "5.0.9";   // 03/19/06 - Checkin before major port of CCamCadDatabase functionality to CCamCadData - knv
   //Version = "5.0.8";   // 03/12/06 - No compile errors, 300+ link errors - knv
   //Version = "5.0.7";   // 03/12/06 - Only 34 errors - knv
   //Version = "5.0.6";   // 03/09/06 - Only 593 errors - knv
   //Version = "5.0.5";   // 03/09/06 - Label after checkin - knv
   //Version = "5.0.4";   // 03/09/06 - Label before checkin - knv
   //Version = "5.0.3";   // 03/09/06 - Label before checkin - knv
   //Version = "5.0.2";   // 12/11/06 - Most Dca code compiles, project will not build at this point - knv
   //Version = "5.0.1";    // 12/10/06 - Major split of Dca code, project will not build at this point - knv
   //Version = "5.0.0";    // 11/15/06 - Created 5.0 project. - knv
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //Version = "4.6.79";   // 11/14/06 - Case #2247 - new Zuken Reader - Files crashing CamCad on import - knv
   //Version = "4.6.78";   // 11/14/06 - new Zuken Reader - insertTypeTestPad was used instead of insertTypeTestPoint - knv
   //Version = "4.6.77";   // 11/10/06 - Case #2243 - new Zuken Reader - The .MirrorLayer infile command is not implemented correctly - knv
   //Version = "4.6.76";   // 11/10/06 - new Zuken Reader - Remove message boxes used for debugging .in file read problems - knv
   //Version = "4.6.75";   // 11/09/06 - new Zuken Reader - Fixed problem with outline width token being used for poly width of shapes, now uses zero - knv
   //Version = "4.6.74";   // 11/09/06 - 
   //Version = "4.6.73";   // 11/09/06 - 
   //Version = "4.6.72";   // 11/07/06 - Case 2242 - Allegro Out - Fixed "name check" usage - rcf
   //Version = "4.6.71";   // 11/01/06 - new Zuken CR5000 Reader - fixed bug where data is created on "Referedby:" layer - knv
   //Version = "4.6.70";   // 11/01/06 - Case #2225 - new Zuken CR5000 Reader - Surface specific geometry contruction - fixed bug with opposite layer being used for mirrored layering - knv
   //Version = "4.6.69";   // 10/31/06 - Case #2234 - new Zuken CR5000 Reader - Some lines incorrectly assigned a zero width - knv
                           // 10/31/06 - Case #2225 - new Zuken CR5000 Reader - Surface specific geometry contruction - knv
                           // 10/31/06 - CamCad.cpp - changed release directory path - knv
   //Version = "4.6.68";   // 10/16/06 - Case #2212 - new Zuken CR5000 Reader - Fixed ignoring of units in file headers (always assumed to be DbUnits) - knv
   //Version = "4.6.67";   // 10/16/06 - Completed implementation of both Component Compare and Net Compare dialog with the exception of "Net Compare By Content With Pcb As Master".  Also Component and Net cross probing is implemented - LP
   //Version = "4.6.66";   // 10/13/06 - Completed inital implementation of Compare Net dialog - LP
   //Version = "4.6.65";   // 10/13/06 - Case #2212 - new Zuken CR5000 Reader - PCB components on bottom side have wrong (negative) rotation - knv
                           //          - Case #2213 - new Zuken CR5000 Reader - Many pins are missing from the netlist (Library Mode) - knv
   //Version = "4.6.64";   // 10/09/06 - Added Konrad_ICT_Out.cpp&h and more implementation of Schematic Link - LP
   //Version = "4.6.63";   // 10/08/06 - Completely initial implementation of Compare Reference Designator dialog - LP
   //Version = "4.6.62";   // 10/03/06 - Database Interface Replacement Project - ODBC interface goes live, here, now. Only in 4.6 for QA - rcf
   //Version = "4.6.61";   // 10/01/06 - Added new Schematic Link initial implementation to 4.6 - LP
   //Version = "4.6.60";   // 09/28/06 - Beta #2 - new Zuken CR5000 Reader - fixed layer span bugs in padstack restructuring - knv
   //Version = "4.6.59";   // 09/27/06 - Beta #1 - new Zuken CR5000 Reader - implemented restructured padstacks - fixed device type handling - knv
   //Version = "4.6.58";   // 09/21/06 - Fixed bug in CCEtoODBView::DrawPolyStruct, where void memDC fill processing was being performed when showFills was false - knv
   //Version = "4.6.57";   // 09/21/06 - Alpha #9 - new Zuken CR5000 Reader - implemented blind/buried vias - knv
   //Version = "4.6.56";   // 09/20/06 - Alpha #8 - new Zuken CR5000 Reader - knv
   //Version = "4.6.55";   // 09/19/06 - Alpha #7 - new Zuken CR5000 Reader - knv
   //Version = "4.6.54";   // 09/18/06 - Alpha #6 - new Zuken CR5000 Reader - knv
   //Version = "4.6.53";   // 09/16/06 - Alpha #5 - new Zuken CR5000 Reader - knv
                           //            - Default vias implemented
   //Version = "4.6.52";   // 09/15/06 - Alpha #4 - new Zuken CR5000 Reader - knv
                           //            - Net attributes added to areas, lines, text, surfaces
                           //            - Creation of "Floating Layer (Material 0)" layer bug fixed.
   //Version = "4.6.51";   // 09/15/06 - Alpha #3 - new Zuken CR5000 Reader - knv
   //Version = "4.6.50";   // 09/14/06 - Alpha #2 - new Zuken CR5000 Reader - knv
   //Version = "4.6.49";   // 09/12/06 - Alpha #1 - new Zuken CR5000 Reader - knv
   //Version = "4.6.48";   // 09/07/06 - Case #2160 - Sch_Link.cpp - In the Schematic Link dialog when net comparision is done, there are nets in the PCB "No Match" listbox when there should be any - LP
                           //          - CcDoc.cpp - Schematic Link will crash, when files are load the second and run Schematic Link - LP
   //Version = "4.6.47";   // 08/10/06 - Case #2130 - AccessAnalysis.cpp - Added attribute "TEST_ACCESS_FAIL_TOP" and "TEST_ACCESS_FAIL_BOTTOM" to indicate access failure for Access Analysis - LP
   //Version = "4.6.46";   // 08/04/06 - Case #2117 - AccessAnalysis.cpp & Data.cpp - Boardoutline Distance Check is creating invalid exposed metal causing Access Analysis to have inaccurate result - LP
   //Version = "4.6.45";   // 07/26/06 - Case #2061 - AccessAnalysis.cpp&h, Net_Util.cpp&h - Added progress dialog during load of Access Analysis dialog - LP
   //Version = "4.6.44";   // 07/24/06 - Case #2061 - AccessAnalysis.cpp&h, AccessAnalysisDlg.cpp&h, Sm_Anal.cpp&h, DFT.cpp - Restruct and improved Access Anlayisis - LP
   //Version = "4.6.43";   // 07/17/06 - Refactor - Stencil Generator - Updated code to restructure fiducial geometries - knv
   //Version = "4.6.42";   // 07/12/06 - Refactor - Privatized and renamed CCEtoODBDoc members; showFills, showApFills, showAttribs - knv
                           //                     - Added default values to SettingStruct for showFills, showApFills, showAttribs - knv
   //Version = "4.6.41";   // 07/10/06 - Stencil Generator - Added code to restructure fiducial geometries - knv
   //Version = "4.6.40";   // 06/22/06 - Refactor - Select Stack phase - removed changes implemented for 4.5.1304 - knv
   //Version = "4.6.39";   // 06/22/06 - Refactor - Select Stack phase 2 - knv
                           //                       Update select code to use new CSelectStack class.
   //Version = "4.6.38";   // 06/14/06 - Refactor - Select Stack phase 1 - knv
                           //                       Replace direct member access with accessor calls.
                           //                       Replace common idiom code with new methods. 
   //Version = "4.6.37";   // 05/23/06 - Stencil Generator - Fixed some bugs with the generation of stencil hole apertures - knv
   //Version = "4.6.36";   // 05/23/06 - Access Analysis - Disable generation of debug data, modified options dialog to minimize size increase - knv
   //Version = "4.6.35";   // 05/22/06 - Access Analysis - Updated user interface to support distal/medial/proximal smd hole offsets - Beta #4 - knv
   //Version = "4.6.34";   // 05/08/06 - Added UltimateGrid2005 source library to project - knv
                           //          - Stencil Generator - Changed grid implementation from CGridControl to CUGCtrl - knv
   //Version = "4.6.33";   // 04/30/06 - Refactor - Update some 4.6 source files that were not shared with 4.5 for the CCEtoODBDoc data member refactor - knv
   //Version = "4.6.32";   // 04/26/06 - Case 1921 - DFT.cpp (in 4.5 at this time, this function is in AccessAnalysisDlg.cpp) - Updated to synchronize with 4.5.1677 (and 1678) fix -rcf
   //Version = "4.6.31";   // 04/25/06 - Shared InFile.cpp,h from 4.5 - knv
   //Version = "4.6.30";   // 04/20/06 - Added VI Technology AOI Writer - knv
   //Version = "4.6.29";   // 04/19/06 - Access Analysis - Updated user interface to support direction priority for thru hole offsets - Beta #3 - knv
   //Version = "4.6.28";   // 04/18/06 - Fixed spelling for standardAttributeDataType and standardAttributeDataSource - knv
                           //          - Incorporated addDebugPolygon(), etc., into the CDebug class - knv
   //Version = "4.6.27";   // 04/07/06 - Access Analysis - Updated user interface to support direction priority for thru hole offsets - Beta #2 - knv
   //Version = "4.6.26";   // 04/06/06 - Access Analysis - Updated user interface to support direction priority for thru hole offsets - Beta #1 - knv
   //Version = "4.6.25";   // 04/04/06 - Access Analysis - Updated user interface to support direction priority for thru hole offsets - knv
   //Version = "4.6.24";   // 04/02/06 - Case #1873 - Edif200_in.cpp&h - Added support of "ViewMap" section to do "instanceBackAnnotate" - LP
   //Version = "4.6.23";   // 03/27/06 - Case #1805 - Theda Reader - Fix problem with incorrect layer mirroring of bottom rectified component geometries - knv
   //Version = "4.6.22";   // 03/27/06 - Case #1805 - Theda Reader - Fix problem with pcb component insert attributes showing up on _Ps layers - knv
   //Version = "4.6.21";   // 03/24/06 - Case #1805 - Theda Reader - Fix problem with graphics showing up on _Ps layers - knv
   //Version = "4.6.20";   // 03/24/06 - Case #1805 - Theda Reader - Fix rectification of component geometries, fix padstack restructuring - knv
   //Version = "4.6.19";   // 03/21/06 - Edif200_in.cpp&h - CAMCAD 4.6 cannot build due to class names in Edif200 reader the same as class names in Edif300 reader - LP
   //Version = "4.6.18";   // 03/06/06 - Case #1805 - Theda Reader - Remove _CS from padstack and pad geometries - knv
   //Version = "4.6.17";   // 03/13/06 - Case #1885 - Edif300_in.cpp - CAMCAD crashed reading Edif300 file due to invalid reference - LP
   //Version = "4.6.16";   // 03/06/06 - Case #1805 - Theda Reader - knv
   //Version = "4.6.15";   // 02/22/06 - Sch_Link.cpp - Set compare option "Content Using PCB as Master" as the default option - LP
   //Version = "4.6.14";   // 02/20/06 - Edif300_in.cpp&h - Add support of ripperHotspots in ripper structure - LP
   //Version = "4.6.13";   // 02/16/06 - Shared EnumIterator.cpp,h from 4.5 - knv
   //Version = "4.6.12";   // 02/16/06 - Takaya_o.cpp - Modified value of Resistor to maintian lower case "m" as "m" and upper case "M" as "M" - LP
   //Version = "4.6.11";   // 02/15/06 - Schematic Link - Rename merged Schematic nets for compare option "Content Using PCB as Master" when Schematic Link is finished - LP
   //Version = "4.6.10";   // 02/14/06 - Scheamtic Link - Added option in Schematic Link to compare net by "Content Using PCB as Master" - LP
   //Version = "4.6.9";    // 02/10/06 - Shared CrcCalculator.cpp,h from 4.5 - knv
   //Version = "4.6.8";    // 02/07/06 - Shared Debug.cpp,h from 4.5 - knv
   //Version = "4.6.7";    // 02/02/06 - Case 1836 - Juki_Out.cpp - Fixed output file name construction, was not handling periods in folder names - rcf
   //Version = "4.6.6";    // 01/26/06 - Edif300_in.cpp&h - Fixed crashed due to invalid reference of variable - LP
   //Version = "4.6.5";    // 01/04/06 - To update 4.6, Add several files and code from 4.5 to 4.6 - LP
   //Version = "4.6.4";    // 12/01/05 - Edif300_in.cpp&h - Added support of singalAnnotate and also a ".in" command "NET_BACKANNOTATE", which allows user to choose a property(attribute) and use its value as the net annotated name - LP
   //Version = "4.6.3";    // 11/29/05 - Edif300_in.cpp&h - Addition of new structures are completed, but still need annotation - LP
   //Version = "4.6.2";    // 11/17/05 - Shared GerberWriter.cpp,h; ArcPoint.cpp,h; LibGraphics files - knv
   //Version = "4.6.1";    // 10/31/05 - First part of enhancement project of Edif reader - LP
   //Version = "4.6.0";    // 10/24/05 - Created 4.6 project. - knv
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //Version = "4.5.1278"; // 10/24/05 - TakayWrite - Modified write to consider test point with TAKAYA_ATTRIBUTE as highest priority - LP
   //Version = "4.5.1277"; // 10/21/05 - Case #1700 - CR5000 Read - Unfilled pins in import should be filled. - knv
   //Version = "4.5.1276"; // 10/21/05 - Takaya Write - LP
                           //             - Modified the output of device type Transistor_FET_NPN, Transistor_Mosfet_NPN, Transistor_FET_PNP, Transistor_Mosfet_PNP
                           //             - Added the output of new device type OPTO
   //Version = "4.5.1275"; // 10/21/05 - Case #1701 - Gerber Educator - Hangs during create geometry. - knv
                           //          - Stencil Generator - Pads with insert type of clearance pad on pad stacks erroneousely became part of the surface copper pad - knv
   //Version = "4.5.1274"; // 10/20/05 - Case #1655 - Cadence Allegro Script Writer - Multiple refdes and device type attributes should be output - knv
   //Version = "4.5.1273"; // 10/20/05 - Case #1698 - Orbotech writer, changed to single tab from double tab.  MAN
   //Version = "4.5.1272"; // 10/19/05 - Case #1685 - MentorIn.cpp - Add report to log file if design has blind vias and no TECH file, a condition that promotes incorrect layer assignments to blilnd/buried vias - rcf
   //Version = "4.5.1271"; // 10/19/05 - Case #1650 - GenCam Reader - Circle pad definition defined in CamCad as complex - knv
   //Version = "4.5.1270"; // 10/19/05 - No case, request from Mark - Sony_AOI_o.cpp - Put output DB in folder named after board, not in "regular" output folder. Create DB folder in "regular" output folder - rcf
   //Version = "4.5.1269"; // 10/19/05 - Case #1698 - Orbotech Writer changes -MAN
   //Version = "4.5.1268"; // 10/19/05 - Case #1529 - DFM placed arrow incorrectly due to cut off of round poly during region creation - LP
                           //          - Case #1691 - Gerber Read needs to set Dcode after import - LP
                           //          - Lic.cpp - Added licensing requirement for Generic Centroid Read - LP
   //Version = "4.5.1267"; // 10/19/05 - Case 1500 - CadifIn.cpp/h - Added support for defCompPosn record (default component position) - rcf
   //Version = "4.5.1266"; // 10/18/05 - Adding missing Header lines to source files. - knv
   //Version = "4.5.1265"; // 10/18/05 - Case #1695 - Stencil Generator - hangs when the "Geometry Rules" property page is activated. - knv
   //Version = "4.5.1264"; // 10/18/05 - Case #1662 - CompPin editor changes -MAN   
   //Version = "4.5.1263"; // 10/17/05 - Case #1649 - UniCAM PDW Write - Round pins end up being square in resulting UniCAM File - knv
                           //          - Case #1657 - Rebug - Draw to aperture conversion - bad results - knv
                           //            -  Changed wording of exception handler message.
   //Version = "4.5.1262"; // 10/17/05 - Case 1653 - GerOut.cpp - Fixed text coming out in wrong place, mis-handled rotation matrix caused text rotation to be used for pcb rotation - rcf
                           //          - Case 1660 - PadsIn.cpp, PadsLib.cpp - Fixed drill size, error in list search caused selection of wrong pad - rcf
                           //          - None      - CadifIn.cpp - Fixed device gemoetry name generation, was criss-crossing blocknames in generated names, fixed inheritance of attributes in generated devices - rcf
   //Version = "4.5.1261"; // 10/17/05 - Case 1692 - Crash in MVP out -MAN 
   //Version = "4.5.1260"; // 10/15/05 - Completed refactor of classes using FindEntity(). - knv
   //Version = "4.5.1259"; // 10/15/05 - Continued refactor of classes using FindEntity(). - knv
   //Version = "4.5.1258"; // 10/14/05 - Case #1684 - Sony_AOI_o.cpp/h - Use pin extents for PSIZE for Dummy parts - rcf
   //Version = "4.5.1257"; // 10/14/05 - Case #1690 - CAMCAD Crash while completing Probe Placement - knv
                           //             - Refactor classes using FindEntity().  Created a new typesafe class, CEntity .
   //Version = "4.5.1256"; // 10/14/05 - Case #1649 - UniCAM PDW Write - Round pins end up being square in resultin UniVAM file - knv
                           //             - Only part of this case has been fixed.
   //Version = "4.5.1255"; // 10/13/05 - Case 1682  Fiducial not receiving correct XY location with Orbotech Writer -MAN   
   //Version = "4.5.1254"; // 10/13/05 - GenericCentroidIn.cpp&h - Improve the reader to recognize and automatic assign certain column to certain field and also make sure every field is assigned - LP
   //Version = "4.5.1253"; // 10/13/05 - Case 1683 - GerOut.cpp - Fixed rotated non-complex apertures, collection of fixes to complex apertures damaged these - rcf 
   //Version = "4.5.1252"; // 10/12/05 - GenericCentroidIn.cpp&h - Implement the 1st stage of Generic Centroid Read, now able to read and create centroid that is already for Gerber Educator - LP 
   //Version = "4.5.1251"; // 10/12/05 - Fixed Date format in Siemens Board Out. - MAN   
   //Version = "4.5.1250"; // 10/12/05 - Case 1683 - GerOut.cpp - Replace verctored arcs with arc command in aperture draws, don't draw 2nd boundary if it is zero width - rcf
   //Version = "4.5.1249"; // 10/12/05 - Gerber Educator - knv
                           //             - Updated input layer detection to detect centroid and silkscreen layers correctly.
   //Version = "4.5.1248"; // 10/11/05 - Added a class and dialog for Generic Centroid Read - LP
                           //          - Remove Gerber Educator dialog bar - LP
   //Version = "4.5.1247"; // 10/11/05 - Case 1680 - QD_Out.cpp - Fixed damage from case 1646 to rotation in mirrored panel - rcf
   //Version = "4.5.1246"; // 10/11/05 - Case 1606 - Support for optional use of board thickness in header added, fixed layer thickness, was ignoring units - rcf
   //Version = "4.5.1245"; // 10/11/05 - Case #1641, #1495 - Dfm_Dist.cpp - DFM test is generating self violation - LP
   //Version = "4.5.1244"; // 10/10/05 - Case #1678 - GenCadIn.cpp/h - Unprotected array bounds error caused mem damage and crash. Changed to dynamic resizing array. - rcf
   //Version = "4.5.1243"; // 10/10/05 - Gerber Educator - knv
                           //             - Add "Allow Selections" checkbox.
   //Version = "4.5.1242"; // 10/10/05 - Added a new dialog for Generic Centroid Read - LP
   //Version = "4.5.1241"; // 10/10/05 - Case #1676 - Spea_Out.cpp - Board outline was not being closed, was missing some vectors - rcf
   //Version = "4.5.1240"; // 10/07/05 - Gerber Educator - knv
                           //             - Use selected centroids when performing pattern matching.
                           //             - When creating components, update the centroid summary fields correctly.
                           //             - The centroid attributes are not being transferred correctly.
   //Version = "4.5.1239"; // 10/07/05 - Case #1640 - CadifIn.cpp - Retain original CADIF part number as attribute CADIF_PART_NAME on component insert - rcf
   //Version = "4.5.1238"; // 10/07/05 - Gerber Educator - Review Geometries dialog - Allows user to change drill size - LP
   //Version = "4.5.1237"; // 10/07/05 - Case #1676 - Spea_Out.cpp - Added board outline output - rcf
   //Version = "4.5.1236"; // 10/06/05 - Fixed SiemensBoardOut, used PackageStyle instead of GeomName, and output in fixed width( according to email from K.D.)
   //Version = "4.5.1236"; // 10/06/05 - Gerber Educator - Review Geometries dialog - Added Repaint button to toolbar - LP
                           //          - Takaya_o.cpp - Modified the old short test output per Mark's instruction - LP
   //Version = "4.5.1235"; // 10/05/05 - Case #1620 - AccellIn.cpp/h - Add support for "triplePointArc" entity found in customer's Accel data file - rcf
   //Version = "4.5.1234"; // 10/05/05 - Gerber Educator - knv
                           //             - Changing "Technology Detected" should update the statistics correctly.
                           //             - Single selection of centroid doesn't work.
   //Version = "4.5.1233"; // 10/05/05 - Gerber Educator - Review Component dialog - LP
                           //             - Added Repaint button to toolbar
                           //             - Allow empty refdes for non PCB Component
                           //             - Check empty refdes for non PCB Component and any duplicate refdes and rename them during loading of Review Component dialog 
   //Version = "4.5.1232"; // 10/05/05 - Case #1668 - pads_in.cpp - Fixed handling of Font line in version 2005 pads files - rcf
                           //          - Case #1675 - TeradyneSpectrum_Out.cpp, Ipl_out.cpp/h - Several changes to output of ipl.dat file - rcf
   //Version = "4.5.1231"; // 10/05/05 - Case #1657 - Draw to aperture conversion - bad results - knv
                           //             - Log exception messages to a file instead of displaying them in message boxes.
   //Version = "4.5.1230"; // 10/04/05 - Case #1658 - CAMCADNavigator.cpp - Do not call Reset() in OnInit() if doing Schematic Crosslinking - LP
   //Version = "4.5.1229"; // 10/04/05 - Case #1657 - Draw to aperture conversion - bad results - knv
   //Version = "4.5.1228"; // 10/03/05 - Case #1651 - Complex pads in original cad file not exported to gerber properly - knv
   //Version = "4.5.1227"; // 10/03/05 - Case #1489 - ExcelOut.cpp - Had units wrong in 4.5.1220 fix, also made Tool section in old format if include-deciaml-point mode is off - rcf
   //Version = "4.5.1226"; // 10/03/05 - Case #1674 - Edif300_in.cpp/h, Edif200_In.cpp, Edif_In.cpp - Fixed buffer overrun in Edif300, improved error message in all 3 Edif readers - rcf
   //Version = "4.5.1225"; // 10/03/05 - Case #1669 - AllegroIn.cpp&h - Allegro crashed during reading due to invalid reference to BlockStruct - LP
   //Version = "4.5.1224"; // 10/03/05 - Gerber Educator - knv
                           //             - Create Geometries dialog must ignore inappropriate window size parameters when restoring window state.
                           //             - Add "Close" button.
                           //             - Make dialog as small as practical.
                           //             - After components are deleted, fix refresh so that bottom pads are not drawn over top pads.
                           //             - Replace "Drills" with "Derived Drills".
                           //             - "Gerber Pad" count must include th pins.
                           // 10/03/05 - Added control-Left button to select logic.
   //Version = "4.5.1223"; // 10/01/05 - Added a new API called "DeleteOccuranceOfKeyword" - LP
   //Version = "4.5.1222"; // 09/30/05 - Case 1625 - CadifIn.cpp - Case 1299 was an incorrect request and caused this case, removed case 1299 changes - rcf
   //Version = "4.5.1221"; // 09/30/05 - Related to Case #1489 - Format.cpp, Format_s.h, Settings.cpp, Port_Lib.cpp - Fixed Excellon Writer settings save/restore - rcf
   //Version = "4.5.1220"; // 09/30/05 - Case #1489 - ExcelOut.cpp/h, CAMCAD.rc, resource.h, Format.cpp/h, Format_s.h, Setting.cpp, PortLib.cpp - Put control back in for Zero Suppression, without losing features added when zero Suppression was removed - rcf
   //Version = "4.5.1219"; // 09/29/05 - Case #1671 - Iges_Out.cpp, IgesLib.cpp/h - Fixed units setting having no effect on output - rcf
   //Version = "4.5.1218"; // 09/29/05 - Case #1188 - DeviceTyper.cpp/h - Made device typer open in 50% less time, made Pin Map tab open in Part Number mode if partnumber attrib present - rcf
   //Version = "4.5.1217"; // 09/28/05 - Case #1510 - AccelIn.cpp - Accel Read does not set graphic on layer specificed by the command ".COMOUTLINE" to Primary Component Outline - LP
                           //          - Case #1557 - AllegIn.cpp - Allegro Read is not reading the rest of line that has a carriage return or line feed at the begin of the line, causing format not to be recognized - LP
   //Version = "4.5.1216"; // 09/28/05 - Case #1536 - Ag_AOI_o.cpp - Agilent AOI SJ writer needs to handle aperture offsets and rotations correctly. - knv
   //Version = "4.5.1215"; // 09/27/05 - Case #1596 - Ag_AOI_o.cpp - Agilent AOI SJ writer needs to put bottom components into bottom file. - knv
   //Version = "4.5.1214"; // 09/27/05 - Case #1661 - Edit.cpp - Bad tolerance calc caused 0.0 tolerance always, leading to changes being detected when there were none, only round off error - rcf
   //Version = "4.5.1213"; // 09/27/05 - Case #1621 - Pads_out.cpp - Pads Write, output max width of 0.25 inchese if width of trace that is greater than 0.25 inches - LP
                           //          - Case #1647 - Mneutin.cpp - Mentor Neutral Read, attribute of 'COMPONENT_LAYOUT_TYPE' is not added to geometry - LP
   //Version = "4.5.1212"; // 09/26/05 - Case 1663 - Pads_In.cpp - Fixed vias failing to get TEST attrib, roundoff for units conversion was greater than default tolerance value, default tolerance was too small - rcf
   //Version = "4.5.1211"; // 09/26/05 - Gerber Educator  - Fixed behavior of "Selection Mode" and "Component Technology" controls - knv
   //Version = "4.5.1210"; // 09/26/05 - Gerber Educator - knv
                           //          ?Add new layer controls.
   //Version = "4.5.1209"; // 09/26/05 - Case 1667 - Aeroflex_Out.cpp, Ck.cpp/h - Stop up-casing names in output, added option to "check_name" utility to not up-case the output, capabilty was already there, but not accessible - rcf
   //Version = "4.5.1208"; // 09/25/05 - Aeroflex Out, no case # - Aeroflex_Out.cpp - fixed testpoint/net handling, testpoints don't have netname attribute - rcf
   //Version = "4.5.1207"; // 09/24/05 - Aeroflex Out, no case # - Aeroflex_Out.cpp - change processing of cacmad testPADS to testPOINTs, per Mark - -rcf
   //Version = "4.5.1206"; // 09/24/05 - Gerber Educator - Continued Create Geometries dialog changes - knv
                           //          ?Add "Selection Mode" selection filter control.
                           //          ?Add summary table.
   //Version = "4.5.1205"; // 09/24/05 - RealPart.CPP - CAMCAD wsa writing bad XML when SilkScreen had multiple polys in a polystruct - dd
   //Version = "4.5.1204"; // 09/24/05 - Aeroflex issues, no case# - Aeroflex.cpp/h, Ck.cpp/h - Added illegal char filtering, fixed layer mapping, filter fids and tool from clusters - rcf
   //Version = "4.5.1203"; // 09/23/05 - Case #1499 - MentorIn.cpp - Setting ".EXPLODE_POWER_SIGNAL_LAYER_SHAPE" to "Y" does not copy all data from "SIGNAL" and "POWER" to "SIGNAL_" and POWER_" - LP
   //Version = "4.5.1202"; // 09/23/05 - Gerber Educator - LP
                           //          - Review Geometries
                           //             o Add Query toolbar button
                           //          - Review Component
                           //             o Add Query toolbar button
                           //             o Turn on silkscreen layers
   //Version = "4.5.1201"; // 09/23/05 - OdbPpIn.cpp - In COdbNet::buildCamCadData(), need to set NETFLAG_UNUSEDNET flag for NET_UNUSED_PINS nets - LP
   //Version = "4.5.1200"; // 09/23/05 - Takaya_o.cpp - Fixed a bug in doPowerRailShortTest(), the coordinate of y2 is incorrectly set - LP
   //Version = "4.5.1199"; // 09/22/05 - Gerber Educator - knv
                           //          ?Pad patterns matching one type of capacitor in the "1981-Clean.cc" file are not found. 
                           //          ?Crystal oscillator patterns in the "1981-Clean.cc" file are inserted with the wrong orientation. 
                           //          ?Some single pin patterns are not found and when inserted individually use the wrong geometry. 
                           //          ?If a component is created as a thru hole when it should have been smd - after deleting by geometry and re-adding as smd, 
                           //               the thru hole geometry is used. 
                           //          ?When creating thru hole geometries from the bottom view, the geometry is created and inserted as if it was a top side component. 
                           //          ?Mechanical, Fiducial, and Tooling component pins are not matched with Gerber pads at initialization.
   //Version = "4.5.1198"; // 09/22/05 - Review Componenet - Implemented summary table - LP
                           //          - Takaya_o.cpp - Append "*" to refdes for Resistor, Capacitor, CapacitorTantalum, CapacitorPolarized - LP
   //Version = "4.5.1197"; // 09/22/05 - Case 1665 - Aeroflex_Out.cpp - Changed layer numbering, was 0 = top, n = bottom; now 0 = bottom, n = top - rcf
   //Version = "4.5.1196"; // 09/22/05 - Takaya_o.cpp - Fixed a bug in doPinToPinShortTest(), the coordinate of y2 is incorrectly set - LP
                           //          - Review Geometries - Implemented update of summary table - LP
   //Version = "4.5.1195"; // 09/22/05 - Agilent Fixture File Reuse - fixture_reuse.cpp - Cleaned up log file - rcf
   //Version = "4.5.1194"; // 09/21/05 - Case #1664 - Ipl_Out.cpp/h - Fixed crash - rcf
   //Version = "4.5.1193"; // 09/21/05 - Case #1659 - TeradyneSpectrum_Out.cpp/h, Ipl_Out.cpp/h - Added name cleanser for nets, subclasses, and refdeses, fixed "Net Type" (power,ground,signal) detection - rcf
   //Version = "4.5.1192"; // 09/21/05 - Case #1658 - Sm_Anal.cpp - There is an algorithm flaw in SM_ExplodeBlock() which explose some metals and attach them to the same polylist creating a large polylist - LP
   //Version = "4.5.1191"; // 09/21/05 - Case #1632 - fixture_out.cpp/h - Changed ProbeSize to TemplateName for PROBEDRILLs, reactivated Panel mode, supplied GUI for probe number offset - rcf
   //Version = "4.5.1190"; // 09/20/05 - Gerber Educator - knv
                           //          - Fiducials, Mechanical components, and Tooling holes cannot be deleted. 
                           //          ?All HOT KEYS should be disable except Query. - complete
                           //          ?Delete data on matched and unmatched layers when exiting "Create Geometries".
                           //          ?Remove pinloc log.
                           //          ?Remove orientation markers from JP2. 
                           //          ?Orient JP2 like chips.
                           //          ?Query dialog should be set to edit only mode.
                           //          ?ESC key should not hide dialog.
   //Version = "4.5.1189"; // 09/20/05 - Gerber Educator - LP
                           //          - Review Geometries
                           //             o ESC key should not hide dialog
                           //             o After warning of duplicate or empty geometry name, the selection need to stay with the currently editing row
                           //             o Zoom toolbar is disabled
                           //          - Review Components           
                           //             o Pressing ESC key should not hide dialog
                           //             o Add checkbox to allow user to turn on/off pin labels
                           //             o After warning of duplicate or empty refdes, the selection need to stay with the currently editing row
                           //             o Zoom toolbar is disabled
   //Version = "4.5.1188"; // 09/20/05 - Gerber Educator - knv
                           //          - "Invalid argument" message pops up when launching Create Geometries dialog without typing any layer,
                           //               and all toolbars and menus stay disabled so CAMCAD is locked.
                           //          - "Duplicate bottom gerber layers:?" Message pops up when more than one layer is typed the same,
                           //               then "Invalid argument" message pops up, then "The 'Gerber Educator Create Geometries' tool 
                           //               failed to initialize?message pops up, and all toolbars and menus stay disabled so CAMCAD is locked.
                           //          - Do the following to duplicate issue:
                           //             - Load attached Gerber files from Dino 
                           //             - Do Setup Data Type 
                           //             - Launch Create Geometries and exit 
                           //             - Select a via aperture and do “Select all with same D-code (flash)?and delete them 
                           //             - Redraw 
                           //             - Via aperture still show and the are on Educator Unmatched Top/Bottom 
                           //             - Select a via aperture and do "Select all with same D-code (flash)", but this time ALL apertures are selected 
                           //          - Buttons for Delete Component(s) are disabled when the selected component is either a fiducial, mechanical component, or tooling hole.
   //Version = "4.5.1187"; // 09/20/05 - Agilent Fixture Reuse - Snapshot 2 - rcf
   //Version = "4.5.1186"; // 09/19/05 - Gerber Eudcator - LP
                           //          - Review Geometries
                           //             o Should not allow empty geometry name
                           //             o Pressing Spacebar does not uncheck Review field
                           //             o Dialog size will increase by the height of the toolbar each time it is closed and launch again
                           //          - Review Components
                           //             o Should not allow empty refdes
                           //             o Pressing Spacebar does not uncheck Review field
                           //             o Dialog size will increase by the height of the toolbar each time it is closed and launch again 
   //Version = "4.5.1185"; // 09/19/05 - Agilent Fixture Reuse - fixture_reuse.cpp/h - Snapshot. Almost done. All but auto-alignment - rcf
   //Version = "4.5.1184"; // 09/17/05 - Gerber Educator - LP
                           //             o Selection need to stay with the currently editing row even if name is duplicate
                           //             o Remember Zoom Margin and View option
                           //          - Case #1635 - Dde9_in.cpp&h - Add additional instance of REFNAME on the geometry and "$$DEVICE$$" on component - LP
                           //          - Takaya_o.cpp&h - Modified PIN_TO_PINT short test output - LP
   //Version = "4.5.1183"; // 09/15/05 - Case #1643 - DRAW_ENT.CPP - Complexity of 27,000 vertex poly when zoomed in with translucency on severly crashes display device drivers which chrashes entire system - DD
                           //             o WORKAROUND : Filled Polys with more than 10,000 vertices ignore translucency
   //Version = "4.5.1182"; // 09/14/05 - Gerber Educator - LP
                           //          - Review Geometries
                           //             o Selection need to stay with the currently editing row even if name is duplicate
                           //             o Rotate 90 Degree button always set orientation marker button when there is no orientation marker on the geometry
                           //             o Disable Set IPC Rotation when Review is checked
                           //             o Make grid of geometry small to make room to add ZOOM toolbar to dialog
                           //             o Filter selection by Pin, Fiducial, and Mechanical Pin
                           //             o Selection need to stay with the component not the row position
                           //          - Review Components
                           //             o Filter selection by PCB Component, Fiducial, Tooling, and Mechanical Component
                           //             o When Surface dropdown is not allow, should not display dropdown arrow
                           //             o When duplicate refdes is detected, popup a warning message and undo rename
                           //             o Selection need to stay with the component not  the position
   //Version = "4.5.1181"; // 09/14/05 - Gerber Educator - knv
                           //          - Disable DFM toolbar.
                           //          - Disable pop-up menu.
                           //          - Remove By Orthogonal Pattern Match button.
                           //          - Clear selection when switch between surface.
                           //          - Update PCB Navigator after closing dialog.
                           //          - When exiting dialog, set view to top.
                           //          - SOT23 should not have orientation marker.
                           //          - When entering dialog, zoom 1:1.
                           //          - During initialization, match mechanical and fiducial pins, not just electrical pins.
   //Version = "4.5.1180"; // 09/13/05 - Case 1646 - QD_out.cpp - Fixed rotation for bottom side - rcf
   //Version = "4.5.1179"; // 09/12/05 - Takaya_o.cpp&h - Implemented Target Type Priority and ".MAX_VISION_COUNT" command - LP
   //Version = "4.5.1178"; // 09/09/05 - Sony AOI Writer - Sony_AOI_o.cpp - Change extension of output from .mdb to .lib, as per Sony AOI spec - rcf
   //Version = "4.5.1177"; // 09/09/05 - Case #1627 - Crash during Draw to Aperture conversion - knv
   //Version = "4.5.1176"; // 09/09/05 - Takaya_o.cpp&h - Implemented output Power Rail Short test to Takaya Export - LP
   //Version = "4.5.1175"; // 09/08/05 - Digital Test Writer, related to Case 1631 - DigitalTest_o.cpp/h - Adjustments to spaces in "model name" and "value", ensure units always appear in value - rcf
   //Version = "4.5.1174"; // 09/08/05 - Digital Test Writer, related to Case 1631 - DigitalTest_o.cpp/h - Four issues via email from Mark: spaces in model name, visible refname attrib, correct probe drill size, unplaced probe number handling - rcf
   //Version = "4.5.1173"; // 09/08/05 - Gerber Educator - Replaced CToolBar with CResizingDialogToolBar in dialogs - knv
   //Version = "4.5.1172"; // 09/08/05 - Gerber Educator - Added Zooming toolbar to Review Component dialog - LP
   //Version = "4.5.1171"; // 09/08/05 - Case #1637 - Incorrect panel placement on ODB++ import - knv
   //Version = "4.5.1170"; // 09/08/05 - Gerber Educator - Changed implementation of "Set IPC Standard" to "Set IPC Rotation" - LP
   //Version = "4.5.1169"; // 09/08/05 - Case #1638 - Unicam_o.cpp - Crashed on sample data, was not handling BLOCKTYPE_GENERICCOMPONENT - rcf
   //Version = "4.5.1168"; // 09/08/05 - Case #1631 - DigitalTest_o.cpp/h, CAMCAD.rc, resource.h - Renumber probes, removed duplicated Device entries, changed default output values when no value present, changed "model name" construction - rcf
   //Version = "4.5.1167"; // 09/08/05 - Case #1624 -MVP_AOI_Comp_Out.cpp&h - Added outpuf fiducial geometry definition in the "[USER]" section - LP
   //Version = "4.5.1166"; // 09/08/05 - Case #1630 - aeroflex_out.cpp - Was using wrong value for drill size - rcf
   //Version = "4.5.1165"; // 09/08/05 - Gerber Educator - Fixed "Geometry name already used." message from poping up when Review Geometry is launched - LP
                           // 09/07/05 - Takaya_o.cpp&h - Added output of PIN_TO_PIN short test - LP                       
   //Version = "4.5.1164"; // 09/06/05 - Case #1634 - ProbeStyles.cpp - More problems with insert type TESTPOINT - rcf
   //Version = "4.5.1163"; // 09/02/05 - Case #1630 - Aeroflex_Out.cpp/h - Output tool hole definitions - rcf
   //Version = "4.5.1162"; // 09/02/05 - Case #1134 - GenCadIn.cpp/h - Change items of type TESTPAD to type TESTPOINT, change structure to be like component (add a pin) and put item reference in net compin list - rcf
   //Version = "4.5.1161"; // 09/02/05 - Gerber Educator - Disable toolbars and menues when a session is in progress - knv
   //Version = "4.5.1160"; // 09/02/05 - Refix case #1494 - AllegIn.cpp - Only insert testprobe when the field "VAL_TEST_POINT" is not empty - LP
   //Version = "4.5.1159"; // 09/01/05 - Case #1583 - Vb99In.cpp/h - Fixed offsets going wrong direction - rcf
                           // 09/01/05 - Case #1584 - Vb99In.cpp/h - Fixed fill (being ignored) for solder mask items - rcf
                           // 09/01/05 - Case #1585 - Vb99In.cpp/h - Fixed Layer type assignment to vbascii PLANE_LAYER of layer type POSITIVE - rcf
   //Version = "4.5.1158"; // 08/31/05 - Case #1619 addendum - Sony_AOI_o_GUI.cpp - Add pan-to-fid upon fid selection in option menu, like Takaya - rcf 
   //Version = "4.5.1157"; // 08/31/05 - Takaya_o.cpp, Export.h - Refix modified output of ICOpenTest, Vision test, Transistor, Transistor NPN, and Transistor PNP - LP 
   //Version = "4.5.1156"; // 08/31/05 - Stencil Generator - Complex aperture being inserted with a mirror flag - knv
   //Version = "4.5.1155"; // 08/30/05 - No case# - Ccdoc.cpp - Fixed crash, null ptr de-ref; Was crashing upon 2nd import of vbascii data from case 1585. Possibly is really an importer bug, making an insert of non-existent block. But this makes it work, an no other ill effect of "bad" insert has been noticed. - rcf
   //Version = "4.5.1154"; // 08/30/05 - Takaya_o.cpp - Refix modified output of Transistor, Transistor NPN, and Transistor PNP - LP
   //Version = "4.5.1153"; // 08/30/05 - Case #1619 - Sony_AOI_*.cpp/h, Attrib.cpp, keyword.cpp/hCAMCAD.rc, resource.h - Case issues plus GUI issues (per Mark) not mentioned in the case - rcf
   //Version = "4.5.1152"; // 08/30/05 - Takaya_o.cpp - Modified output of Transistor, Transistor NPN, and Transistor PNP - LP
   //Version = "4.5.1151"; // 08/29/05 - Case #1619 - Sony_AOI_*.cpp/h, CAMCAD.rc, resource.h - Snapshot, work in progress, checked-in in order to release CAMCAD.rc/resource.h for others access - rcf
   //Version = "4.5.1150"; // 08/29/05 - Case #1513 - Takaya_o.cpp&h - Components with IC Open test need to consider the placed side of the component being tested - LP
                           //          - Refix Case #1515 - Takaya_o.cpp&h - Component placed side should be consider for vision test - LP
   //Version = "4.5.1149"; // 08/29/05 - Modified Royonics 500 Writer  -MAN
   //Version = "4.5.1148"; // 08/28/05 - Added Royonics 500 Writer  -MAN
   //Version = "4.5.1147"; // 08/27/05 - Gerber Educator - knv
                           //          - Data Source for all layers created by Gerber Educator should be set to Gerber Educator and filtered out of the Setup Data Types dialog.
                           //          - Eliminate debug dialogs that sometime come up during Create Geometry.
                           //          - Upon entering Create Geometry clear any current selections.
                           //          - Chip components should not have an orientation marker.
                           //          - Thru Hole components with 2 rows of pads should be called “DIP?not “SOIC?
                           //          - Geometries that are not needed must be removed from the file.
                           //          - Change names of layer types, e.g. "Top Pad" to "Pad - Top".
   //Version = "4.5.1146"; // 08/26/05 - Gerber Educator - Fixed updateSelectedEntity() in both Review Geometries and Review Components.  Also fixed duplicate geometry name, IPC Status, and Set IPC button in Review Geometries - LP
   //Version = "4.5.1145"; // 08/25/05 - Case 1619 - Sony_AOI_*.cpp/h, CAMCAD.rc, resource.h - Snapshot, work in progress, checked-in in order to release CAMCAD.rc/resource.h for others access - rcf
   //Version = "4.5.1144"; // 08/25/05 - Case 1579 - CadaifIn.cpp - Netname assignment to pins not working when using USE_PACKALT_NAME Y, problem was mismanaging "real alt name" list during parsing resulting in wrong associations - rcf
   //Version = "4.5.1143"; // 08/25/05 - Added saving of Takaya dialog settings to current PCB file - LP
   //Version = "4.5.1142"; // 08/24/05 - Fixed fiducials problem in the CCM Writer -MAN   
   //Version = "4.5.1141"; // 08/24/05 - Fixed a bug in CCM Writer, added a function to handle fiducials correctly -MAN
   //Version = "4.5.1140"; // 08/24/05 - Gerber Educator - Fixed bugs - knv
                           //            CFileList refactor
   //Version = "4.5.1139"; // 08/24/05 - Fixed a bug in CCM Write where fiducials were not written -MAN  
   //Version = "4.5.1138"; // 08/24/05 - Fixed a bug in the Siemens Board Write -MAN
   //Version = "4.5.1137"; // 08/24/05 - Added Siemens Board Write -MAN
   //Version = "4.5.1136"; // 08/24/05 - Ability to Override CAMCAD Lock - dd
   //Version = "4.5.1135"; // 08/24/05 - Case #1592 - Ingun_o.cpp, Ipl_out.cpp/h - Stop mangling probe names (disable use of Ipl_18xx.out for TRI-MDA exporter) - rcf
   //Version = "4.5.1134"; // 08/24/05 - Case #1586 - attrib.c/h, Atrblist.cpp - Hide All was not hiding all - Changed implementation of fix - knv
   //Version = "4.5.1133"; // 08/24/05 - Refix Case #1494 - Need to fix probe placement for classic Allegro - LP
   //Version = "4.5.1132"; // 08/23/05 - Case #1586 - attrib.c/h, Atrblist.cpp - Hide All was not hiding all - rcf
   //Version = "4.5.1131"; // 08/23/05 - Case #1471 - Cadif reader - Refname text on wrong layer - knv
   //Version = "4.5.1130"; // 08/23/08 - Case #1494 - AllegIn.cpp&h - Probe was placed at the location of the text in ".sym" file instead of the location given in ".rte" file - LP
                           //          - Case #1613 - QD_out.cpp - Subsequence output of board definition is missing "PROGRAMM BESTUECKEN OBERSEITE" - LP
   //Version = "4.5.1129"; // 08/23/05 - Case #1521 - Board level drill location not selected using Mark by Window - knv
   //Version = "4.5.1128"; // 08/23/05 - Case #1539 - Explode Poly bug - knv
   //Version = "4.5.1127"; // 08/23/05 - Case #1549 - Board size incorrect in placement report - knv
   //Version = "4.5.1126"; // 08/22/05 - Case 1518 - DeviceTyper.cpp/h - Only show and act on components in "only visible file" - rcf
   //Version = "4.5.1125"; // 08/22/05 - Refix case #1514 - ViscomAOI_Out.cpp - ".FIDUCIAL BOARD" should still output "LP" lines - LP
                           //          - Refix case #1515 - Takaya_o.cpp Vision need to be output is ".SELECT VISION" is set and parallels test is commented out - LP
                           //          - Case #1602 - MVP_AOI_Comp_Out.cpp&h - Modification to MVP output of SOIC, QFT, and USER components - LP
   //Version = "4.5.1124"; // 08/22/05 - Case #1538 - HpegsIn.cpp - Improved handling of component refdes - rcf
   //Version = "4.5.1123"; // 08/22/05 - Case #1598 - Can't click and select Bottom side components with this design on BOTTOM VIEW - knv
                           //          - Case #1607 - Pads Import - CAMCAD Crashes while optimizing database - knv
                           //          - Case #1601 - Agilent AOI SP50 bottom side export details - knv
   //Version = "4.5.1122"; // 08/18/05 - No case # - ProbeStyles.cpp - Made checkbox cell handling more like other examples in camcad, attempt to fix non-working checkbox on Win2000 (was working fine on WinXP) - rcf
   //Version = "4.5.1121"; // 08/18/05 - Case 1526 - GerbOut.cpp - Stop trying to draw zero height / width attribute text - rcf
   //Version = "4.5.1120"; // 08/18/05 - Case 1577 - ProbeStyles.cpp - Three problems: probe template name not determined correctly causing assignment failures, not enough accuracy saved in probestyles.dat to retain mm value accuracy, TEST attrib not working with TESTPOINT probe style rules - rcf
   //Version = "4.5.1119"; // 08/18/05 - Gerber Educator - Switching from Review Components dialog to Review Geometries dialog and back is working correctly.  Component and geometry are selected according - LP
   //Version = "4.5.1118"; // 08/17/05 - Gerber Educator - Added CGerberEducatorUi class to implement toolbar functionality - knv
   //Version = "4.5.1117"; // 08/17/05 - Case 1550 - OrcltdIn.cpp - Fixed bottom/mirrored parts appearing on top - rcf
   //Version = "4.5.1116"; // 08/17/05 - Case 1580 Orbotech Writer - fixed problems with fiducials without refnames -MAN
   //Version = "4.5.1115"; // 08/17/05 - Gerber Educator - Update IPCStatus whenever user change Pin Numbering Pattern or Rotation - LP
   //Version = "4.5.1114"; // 08/17/05 - Case 1580 Orbotech Writer - support fiducials and package attributes - MAN
   //Version = "4.5.1113"; // 08/16/05 - Gerber Educator - Implemented IPCStatus in Review Geometries dialog - LP
   //Version = "4.5.1112"; // 08/15/05 - Gerber Educator - Added enum for IPCStatus and OriginMethod, also use enum for PinOrderOption instead of constants - LP
   //Version = "4.5.1111"; // 08/15/05 - Refix Case #1558 - UnicamIn.cpp - Unicam Read did not calculate component location correctly and did not include pin insert offset - LP
   //Version = "4.5.1110"; // 08/14/05 - Case #1558 - UnicamIn.cpp - Unicam Read incorrectly placed component by using center of pin extend instead of center of pin centriod like how Unicam Write does - LP
                           //          - Case #1561 - Mneutin.cpp - Mentor Neutral does remove net hierarchy when the command ".HIERACHICAL_NETNAMES" is set to "N" - LP
                           //          - Case #1562 - Sch_Link.cpp - Schematic Link does not include Test Point, and it should - LP
                           //          - Case #1573 - Accel_out.cpp - Allegro Write crashed durping export due to improper implementation of adding TypeStruct - LP
   //Version = "4.5.1109"; // 08/12/05 - Case #1532 - Draw to aperture conversion not handling some pads - knv
   //Version = "4.5.1108"; // 08/12/05 - Gerber Educator - Improved Create Geometry - knv
   //Version = "4.5.1107"; // 08/11/05 - Gerber Educator - Added toolbar for Gerber Educator, which later will replace the Gerber Educator dialog bar - LP
   //Version = "4.5.1106"; // 08/10/05 - Gerber Read - Gerber Reads will set Data Source of layer to "Gerber" so Setup Data Type dialog will automatically set the Gerber layers - LP
                           //          - Gerber Educator - Added "Closed" buttons to Setup Data Type, Review Geometries, Review Inserts dialogs - LP
                           //                            - Changed size and thickness of orientation markers
                           //                            - Added slider to control zoom margin in Review Insert dialog - LP
   //Version = "4.5.1105"; // 08/09/05 - Gerber Educator - Added "Clear Orientation Marker" button and set Top view and proper layers for Review Geometries dialog - LP
                           //          - Gerber Educator - Added view option buttons and turning on proper layers for Review Components dialog - LP
   //Version = "4.5.1104"; // 08/09/05 - Case #1580 - changes to Orbotech writer to read package name from .out file.  -MAN
   //Version = "4.5.1103"; // 08/09/05 - Gerber Educator - Compled the implementation of Orientation Marker buttons for Review Geometries dialog - LP
   //Version = "4.5.1102"; // 08/09/05 - Gerber Educator - Fixed search code, changed corner orientation mark code - knv
   //Version = "4.5.1101"; // 08/08/05 - Case #1576 - Access Analysis no long always analyze 1 pin SMD components, instead it give an error "Single Pin SMD Component not analyzed" - LP
                           // 08/08/05 - Gerber Educator - Fixed a big in "Grid" option for Pin Numbering - LP
   //Version = "4.5.1100"; // 08/08/05 - Gerber Educator - Added bitmaps and buttons for Orientation Markers, functionality of buttons not yet implemented - LP
   //Version = "4.5.1099"; // 08/05/05 - Gerber Educator - Updated geometry creation code - knv
   //Version = "4.5.1098"; // 08/05/05 - Gerber Educator - Finished implementing buttons for Reading Order in "Review Geometries" dialog - LP
   //Version = "4.5.1097"; // 08/05/05 - Case #1516 - ProbePlacement.cpp, DFT.h - Place some probes in post processing step that the main algorithms drops, Consider preferred side when moving probes in the "Both" case - rcf
   //Version = "4.5.1096"; // 08/05/05 - Case #1572 - Access Analysis not calculating access surface correctly due to DesignSurfaceValueStringToTag() returning wrong access surface tag - LP
   //Version = "4.5.1095"; // 08/04/05 - Gerber Educator - Added more bitmaps and some implementation pin numbering in "Review Geometries" dialog - LP
   //Version = "4.5.1094"; // 08/04/05 - Reverted change in Search.cpp to fix problem with selecting access points - knv
   //Version = "4.5.1093"; // 08/04/05 - Gerber Educator - Fixed some problems with the component geometry comparison code - knv
   //Version = "4.5.1092"; // 08/02/05 - Case 1555 - ProbeStyles.cpp/h fixture_out.h - Fixed connectors and vias not getting right style - rcf
   //Version = "4.5.1091"; // 08/02/05 - Gerber Educator - Added few more bitmaps for "Review Geometries" dialog - LP
   //Version = "4.5.1090"; // 08/02/05 - Gerber Educator - Added bitmaps for "Review Geometries" dialog - LP
   //Version = "4.5.1089"; // 08/02/05 - Gerber Educator - Completed the implementation of "Setup Data Type" dialog. It is read for testing - LP
   //Version = "4.5.1088"; // 08/01/05 - Case #1548 - GerbOut.cpp/h - Fixed aperture drawing - rcf
   //Version = "4.5.1087"; // 08/01/05 - Gerber Educator - Fixed "Pin Numbering" by "Reading Order" and added implementation for "Setup Data Type" dialog - LP
   //Version = "4.5.1086"; // 07/29/05 - Gerber Educator - Added new "Setup Data Type" dialog for Gerber Educator - LP
   //Version = "4.5.1085"; // 07/29/05 - Case #1501 - MVP_AOI_Comp_Out.cpp/h - Change SOIC/QFP pin quadrant to be relative to centroid instead of insert point, if centroids not present report such and quit export - rcf
   //Version = "4.5.1084"; // 07/29/05 - Case #1514 - ViscomAOI_Out.cpp - Add .out command ".FIDUCIAL", ".XY_ADJUST", and ".ROT_ADJUST" - LP
                           //          - Case #1515 - Takaya_o.cpp - When comment out parallel component, we need to generate vision test based on ".SELECT_VISION" - LP
                           //          - Case #1513 - Takaya_o.cpp - Issue #2. when outputing IC Opens Test, we need to find two opposite groud net instead of the same one - LP
   //Version = "4.5.1083"; // 07/29/05 - Gerber Educator - Modified Review Geometris dialog to sort by Geometry Name, and Review Insert dialog to pan to XY coordinate if there is no refdes - LP
   //Version = "4.5.1082"; // 07/28/05 - Fixed a crash in probe style matrix running twice -MAN
   //Version = "4.5.1081"; // 07/27/05 - Gerber Educator - Modification to Review Geometries, improved pin numbering and change insert type when geometry type is changed - LP
   //Version = "4.5.1080"; // 07/26/05 - Case #1497 display a message if FR is ran before Access Analysis.  MAN
   //Version = "4.5.1079"; // 07/26/05 - Case #1551 Crash with variant manager --MAN
   //Version = "4.5.1078"; // 07/26/05 - Case #1546, 1486 - Ag_AOI.cpp - Errors in SJ and SP output, finally resolved I think -rcf
   //Version = "4.5.1077"; // 07/26/05 - Case #1543 - Error in calling the function macroStringToLayerTypeTag() when it need to call the function stringToLayerTypeTag() - LP
   //Version = "4.5.1076"; // 07/26/05 - Gerber Educator - Implmented "Clockwise" and "CounterClockwise" pin numbering and fixed duplicate geometry name detection - LP
   //Version = "4.5.1075"; // 07/25/05 - Case #1546 - Ag_AOI_o.cpp - SP50 output was entirely hosed, problems in scaling and alignment - rcf
   //Version = "4.5.1074"; // 07/25/05 - Gerber Educator - Fixed bug in "Reading Order" for Review Geometries dialog and fixed allowed degree for Review Inserts dialog - LP
   //Version = "4.5.1073"; // 07/25/05 - Gerber Educator - Implemented "Reading Order" for Pin Numbering Pattern in Review Geometries dialog - LP
   //Version = "4.5.1072"; // 07/22/05 - Stencil Generator Improvement - Beta #23 - knv
   //Version = "4.5.1071"; // 07/22/05 - Gerber Educator - Initial implementation of class for analysize pin numbering pattern - LP
   //Version = "4.5.1070"; // 07/22/05 - Changes to Digital Test Writer -MAN
   //Version = "4.5.1069"; // 07/21/05 - Stencil Generator Improvement - Beta #22 - knv
                           //            Added more attribute handling methods to CCamCadDatabase and CCEtoODBDoc.
                           //            Added attributes() accessor to various classes.
                           //            Added SymmetryCriteriaTag to CDataList::getPinSymmetryCount(...)
   //Version = "4.5.1068"; // 07/21/05 - Minor modification to "Review Geometries" and "Review Insert" dialogs - LP
   //Version = "4.5.1067"; // 07/21/05 - Implemented alphanumeric sort of refdes in "Review Inserts" - LP
   //Version = "4.5.1066"; // 07/21/05 - Implemented "Orientation Marker" - LP
   //Version = "4.5.1065"; // 07/20/05 - Implemented "Snap To Pad" and modified "Review Geometries" and "Review Insert" dialogs and made them resizable - LP
   //Version = "4.5.1064"; // 07/20/05 - Added "Orientation Marker" and "Snap To Pad" controls - LP
   //Version = "4.5.1063"; // 07/20/05 - Case 1481 - Gerb_Mgr.cpp - Finished - rcf
   //Version = "4.5.1062"; // 07/19/05 - Case 1481 - Gerb_Mgr.cpp/h - Some flashes were missing attrib assignments. Better now, but not finished yet. - rcf
   //Version = "4.5.1061"; // 07/19/05 - Added modifiation to Review Geometries and Review Insert dialog - LP
   //Version = "4.5.1060"; // 07/18/05 - Case 1535 - Fixed output of Probe Drill in Panel View mode - rcf
   //Version = "4.5.1059"; // 07/18/05 - Draw.cpp - Pin labels need to be drawn last so Review Geometries dialog shows pin labels properly when selecting pins - LP
   //Version = "4.5.1058"; // 07/18/05 - GerberEducatorReviewGeometries.cpp&h, GerberEducatorReviewInserts.cpp&h - Enhanced both dialog according to list of modifications after meeting, see qa.txt in this version's folder for detail - LP
   //Version = "4.5.1057"; // 07/16/05 - Case 1508 - fixture_out.cpp - Fixed crash, then changed to terminate operation if no test plan since output contains no information without test plan - rcf
   //Version = "4.5.1056"; // 07/15/05 - Case 1528 - PadsIn.cpp - Fixed deref of null pointer after (failed) layer lookup by name - rcf
   //Version = "4.5.1055"; // 07/15/05 - Case 1525 - XML_wrt.cpp - Increased buf size to 4096, case data has 2700+ char text item, old buf size 2048 caused cc save failure - rcf
   //Version = "4.5.1054"; // 07/14/05 - Removed Gerber Educator layer types - knv
   //Version = "4.5.1053"; // 07/14/05 - Case 1520 - fixture_reuse.cpp - Fixed crash, deref null ptr - rcf
   //Version = "4.5.1052"; // 07/13/05 - Case 1501 - MVP_AOI_Comp_o.cpp&h - Fixed three issues: (1) pin count limitations for SOIC & QFP  (2) Rotation angle for SOIC & QFP based on pin 1 quadrant  (3) BGA/PGA were output as SOIC, should be type USER - rcf
   //Version = "4.5.1051"; // 07/12/05 - Remove layertypes array. - knv
   //Version = "4.5.1049"; // 07/12/05 - Add more modification to both Review Geometries or Review Inserts - LP
   //Version = "4.5.1048"; // 07/12/05 - SOny AOI Writer - Sony_AOI_o_.cpp&h, Sony_AOI_o_GUI.cpp&h - Further refinements to exporter. - rcf
   //Version = "4.5.1047"; // 07/12/05 - Move classes from GerberDialog.cpp&h to GerberEducatorToolbar.cpp&h, GerberEducatorReviewGeometries.cpp&h, GerberEducatorReviewInserts.cpp&h - LP
   //Version = "4.5.1046"; // 07/11/05 - Stencil Generator Improvement - Beta #21 - knv
   //Version = "4.5.1045"; // 07/11/05 - Case 1383 - Huntrn_o.cpp - Use test access points for output, if they exist - rcf
                           //          - Sony AOI - Sony_AOI_o.cpp&h - Added .EXPORT_LOADED setting support, added support for spaces in DB paths/names - rcf
   //Version = "4.5.1044"; // 07/11/05 - GerberDialog.cpp&h, resource.h, Camcad.rc - Added implementation to Review Inserts dialog to load PCB component into grids - LP
   //Version = "4.5.1043"; // 07/11/05 - Stencil Generator Improvement - Beta #20 - knv
   //Version = "4.5.1042"; // 07/11/05 - GerberDialog.cpp&h - Review Geometries dialog is ready to be test with the exception that only the "Manual" options of "Pin Numbering Pattern" is working - LP
   //Version = "4.5.1041"; // 07/11/05 - Stencil Generator Improvement - Beta #19 - knv
   //Version = "4.5.1040"; // 07/08/05 - GerberDialog.cpp&h, resource.h, CCEtoODB.h - Added a grid to display pin number and type in Review Geometry dialog - LP
   //Version = "4.5.1039"; // 07/07/05 - Case #1523 - Mneutin.cpp - Modified to place holes on panel when hole definitions follow immediately after a P_ADDP and there is a panel - LP
                           //          - GerberDialog.cpp&h, resource.h, Camcad.rc - Add more modification to Review Geometry dialog - LP
   //Version = "4.5.1038"; // 07/07/05 - Sony AOI Alpha1 - Sony_AOI_o.cpp&h, Sony_AOI_o_GUI.cpp&h, response.cpp&h - rcf
   //Version = "4.5.1037"; // 07/07/05 - Added NoSplash Command Line parameter to running Tools Bom Explorer and RSI Exchange
   //Version = "4.5.1036"; // 07/06/05 - Stencil Generator Improvement - Beta #18 - knv
   //Version = "4.5.1035"; // 07/06/05 - Add more mortification to Review Geometry dialog - LP
   //Version = "4.5.1034"; // 07/05/05 - GerberDialog.cpp&h, resource.h, Camcad.rc - Add more implementation for "Review Geometry" dialog - LP
   //Version = "4.5.1033"; // 07/05/05 - Case #1512 - AccelIn.cpp - Accel Read crashed CAMCAD due to invalid reference of ACCELCompdef - LP
                           //          - Accel_out.cpp - Accel Write, the function do_compdef() need to write out block information even if there is not device type - LP
   //Version = "4.5.1032"; // 06/30/05 - Added more modification to Gerber Eduator dialogs - LP
   //Version = "4.5.1031"; // 06/30/05 - Stencil Generator Improvement - Beta #17 - knv
   //Version = "4.5.1030"; // 06/30/05 - NailAssignIn.cpp - Update the REFNAME attribute of probe - LP
   //Version = "4.5.1029"; // 06/29/05 - Stencil Generator Improvement - Beta #16 - knv
   //Version = "4.5.1028"; // 06/29/05 - Added many dialog classes to GerberDialog.cpp&h for Gerber Educators - LP
                           //          - Refix Case #1483 - Unicam_o.cpp - Need to add comma after "f_class", "f_type", "f_sel" - LP
                           //          - ViscomAOI_Out.cpp - Use panel name as file name instead of board name - LP
   //Version = "4.5.1027"; // 06/29/05 - Sony AOI development snapshot - rcf
   //Version = "4.5.1026"; // 06/28/05 - Case #1053 - Takaya_o.cpp - Do not generate IC Open test when the N number associated with G1 or G2 N nubmer - LP
   //Version = "4.5.1025"; // 06/28/05 - Case #1494 - AllegIn.cpp - Add refname as visible attribute - LP
   //Version = "4.5.1024"; // 06/28/05 - Case #1483 - Unicam_o.cpp - Modified the output of "f_class", "f_type", "f_sel" to numeric instead of string - LP
                           //          - Case #1502 - ViscomAOI_Out.cpp - Modified the output of ".SIZE" to output width and length of panel instead of board, and also allow asterisk character as comment character - LP
   //Version = "4.5.1023"; // 06/28/05 - Case #1496 - fixture_out.cpp - Fixed violation counting; Probe-to-probe violations not generated by fixture_out can not be "fixed" in fixture_out, was looping indefinitely looking for violations to go to zero. - rcf
   //Version = "4.5.1022"; // 06/27/05 - Case #1504 - AccessAnalysisDlg.cpp - Access Analysis need to indicate to users that they need to generate pin location again if padstack of comppin is invalid - LP
                           //          - Case #1505 - Block.cpp - "TOP_PADSTACK_DESIGN_SURFACE" and "BOT_PADSTACK_DESIGN_SURFACE" was not generated correctly cause invalid DFT result - LP
   //Version = "4.5.1021"; // 06/27/05 - Case #1486 - Ag_AOI_o.cpp - Coord/scale fixes for SP, FX, Fixed "refdes" for FX - rcf
   //Version = "4.5.1020"; // 06/27/05 - Stencil Generator Improvement - Beta #15 - knv
   //Version = "4.5.1019"; // 06/24/05 - Case #1493 - Vb99In.cpp, Layer.cpp&h - Need to create layers specified vias - LP
   //Version = "4.5.1018"; // 06/24/05 - Case #1481 - Inherit ECAD Data not inheriting correctly - knv
                           //            Several problem with the inherit ECAD process have been fixed,
                           //            however the inherit ECAD still has major flaws:
                           //            1) All flashes, both top and bottom are placed into a single qfe tree.
                           //            2) The search extent for finding entities in the qfe tree is set to 1 mil square centered on the pin insert - 
                           //               offset flashes can be missed.
                           //            Proposed future changes:
                           //            1) The flashes should be segregated into two qfe trees: top and bottom.
                           //            2) The search extent for finding candidate flashes should be set to the size of the copper pad (top or bottom).
                           //            3) The found list of flashes should be checked for overlapping the copper pad.
   //Version = "4.5.1017"; // 06/24/05 - Synchronized with SourceSafe after vss repair - knv
   //Version = "4.5.1016"; // 06/23/05 - Case #1492 - CadifIn.cpp - Added the command ".COPPER_TO_MECHPIN" to "cadif.in" file.  When this command is set to "Y" then the read convert filled poly in PCB Component block to Mechanical Pin - LP
   //Version = "4.5.1015"; // 06/22/05 - Case #1476 - PfwIn.cpp - Protel Read need to make the "Comment" as a visible attribute when "Comment=True" - LP
                           //          - Case #1484 - PadsInc.pp - PADS Read need to move text in decal that is on layer with electrical stack number 1 or max to layer "Silkscreen Top" or "Silkcreen Bottom" respectively - LP
   //Version = "4.5.1014"; // 06/21/05 - Case #1487 - AllegIn.cpp - Allegro Read was is not inserting the break out of ETCH with netname on the correct layer surface - LP
   //Version = "4.5.1013"; // 06/21/05 - Case #1486 - Ag_AOI.cpp & h - Fixed coordinate and rotation output for SJ. Was applying units conversion factor twice for coords, was getting rotation from "wrong" BasesVector. - rcf
   //Version = "4.5.1012"; // 06/21/05 - Case #1485 - AllegIn.cpp - Allegro Read is not properly assigning Panel outline when it should - LP
   //Version = "4.5.1011"; // 06/20/05 - Case #1491 - DataDoctor.cpp, CamCadDatabase.cpp - Attempt to use unregistered keyword caused crash - rcf
   //Version = "4.5.1010"; // 06/20/05 - Case #1245 - Edif300_in.cpp&h, Sch_Lib.cpp&h - Added proper support for "leafOccurrenceAnnotate" and "occurrenceAnnotate" - LP
   //Version = "4.5.1009"; // 06/17/05 - Stencil Generator Improvement - Beta #14 - knv
   //Version = "4.5.1008"; // 06/16/05 - Case #1426 - Gerber Write - written apertures are smaller than original apertures - knv
   //Version = "4.5.1007"; // 06/16/05 - Case #1426 - Gerber Write - written apertures are smaller than original apertures - Partial fix - knv
   //Version = "4.5.1006"; // 06/15/05 - Sony AOI - Camcad.vcproj, DbUtil.cpp&h, Lic.cpp&h, MultipleMachine.cpp, Port.h, Port_Lib.cpp, New: Sony_AOI_o.cpp&h - Added infrastructure for new exporter - rcf
   //Version = "4.5.1005"; // 06/14/05 - Added fixture reuse license before the fixture reuse dialog is shown.  -MAN
   //Version = "4.5.1004"; // 06/13/05 - Put an explicit filter to not add UNICAM PDW Write option to the unlicensed dialog when it is not licensed.  -MAN
   //Version = "4.5.1003"; // 06/13/05 - Changed logos in the about dialog -MAN   
   //Version = "4.5.1002"; // 06/13/05 - QD_out.cpp - Siemen QD Write, whenever the command ".COMP_POSITION" is set to "CENTROID", it also need add component rotation to the centroid rotation - LP
   //Version = "4.5.1001"; // 06/10/05 - Case #1478 - Inherit ECAD Data issue - many deposits are not getting attributes - knv
                           //          - Case #1479 - Inherit ECAD Data removing prior inherited attributes - knv
   //Version = "4.5.1000"; // 06/10/05 - After Lynn declined this honorable change, I am happy to make this switch... Congratulations to such extraordinary effort that was put in making this happen --MAN.  
   //Version = "4.5.423";  // 06/10/05 - Case #1480 - Mneutin.cpp - Mentor Neutral Read, board level drill hole was being repeated as many time as the number of board insert - LP
   //Version = "4.5.422";  // 06/10/05 - CAMCADNavigator.cpp - PCB Navigator wasn't updating Multiple Machine and Variant name correctly when switching between document - LP
   //Version = "4.5.421";  // 06/09/05 - Case #1469 - Hp3070_o.cpp - Added option to use value of TEST attribute to determine probe side output in Bxy file - rcf
   //Version = "4.5.420";  // 06/09/05 - DFT.cpp, AccessAnalysisDlg.cpp - Uncheck Height Analysis by default and output the setting in the report - LP
   //Version = "4.5.419";  // 06/09/05 - Added comfirmation for Parts library load and save.  Made modifications to controls captions. -MAN
   //Version = "4.5.418";  // 06/09/05 - Case #1477 - ProbePlacement.cpp - Fixed component boundary checking for placement on top or bottom, was okay when placing both, other issues in case were side effects of same bug - rcf
   //Version = "4.5.417";  // 06/09/05 - Case #1446 - Dfm_Rgn.cpp - eSight DFM soldermask webbing should still do webbing when the poly surrounding other soldermask is not both closed and filled - LP
                           //          - Variant.cpp&h - Added "Delete" button for Variant dialog - LP
   //Version = "4.5.416";  // 06/08/05 - Case #1472 - GenCad_o.cpp - Gencad Write skip outputing the padstack when that are invalid layer type - LP
   //Version = "4.5.415";  // 06/08/05 - Case #1430 - Edif300_In.cpp - Show the PrimaryName instead of PortName on ports - rcf
   //Version = "4.5.414";  // 06/07/05 - Case #1463 - Fixed a bug in reading Alegro files through ecad parser.  -MAN
   //Version = "4.5.413";  // 06/07/05 - Case #1470 - Changed many files to move Project Path from class CCEtoODBApp to class CCEtoODBDoc - LP
   //Version = "4.5.412";  // 06/07/05 - Case #1412 - Pads Write - Traces wider than 250 mils must be written as lines. - knv
   //Version = "4.5.411";  // 06/07/05 - Made Device.asc module read without a license requirement
   //Version = "4.5.410";  // 06/07/05 - Case #1464 - Outline.cpp - Outline for Pin Extent was not generated correctly for pin that used complex aperture - LP
   //Version = "4.5.409";  // 06/06/05 - Case #1338 - MergeFiles.cpp - Update file numbers in merged blocks, else blocks will disappear - rcf
   //Version = "4.5.408";  // 06/06/05 - Agilent SOW for AOI SP50 writer - Beta #7 - knv
   //Version = "4.5.407";  // 06/06/05 - Refix Case #1439 - UnidatIn.cpp - Remove code added in the first attemp to fix this case, and implement code to take CP_RULE_ABSOLUTE into consider for rotation - LP
   //Version = "4.5.406";  // 06/06/05 - Added licensing to fixture reuse.  -MAN
   //Version = "4.5.405";  // 06/06/05 - Case #1465 - fixture_reuses.cpp - Fixed crash (two ways) when loading fixture file if no board is visible. 1. Fixed null ptr deref, 2. Fixed GUI to stop user from doing useless operration. - rcf
   //Version = "4.5.404";  // 06/03/05 - Agilent SOW for AOI SP50 writer - Beta #6 - knv
   //Version = "4.5.403";  // 06/02/05 - Case #1455 - OdbPpIn.cpp - Convert step&repeat to page units when creating cc data - rcf
   //Version = "4.5.402";  // 06/02/05 - Case #1459 - ProbePlacement.cpp - Set unplaced probes from cc data as "not existing". Having them set to "existing" was causing all sorts of havoc in "add additional probes" scenario. - rcf
   //Version = "4.5.401";  // 06/02/05 - Agilent SOW for AOI SP50 writer - Beta #5 - knv
   //Version = "4.5.400";  // 06/02/05 - Case #1439 - UnidatIn.cpp - Added the command ".TOP_BUILD" to create padstack using top pad - LP
                           //          - Case #1448 - Vb99Out.cpp - Need to include all geometry level date in CELL definition - LP
                           //          - Refix Case #1461 - Need to refresh PCB Navigator with the new components that are just converted - LP
   //Version = "4.5.399";  // 06/02/05 - Removed FabMaster Device Read License -MAN
   //Version = "4.5.398";  // 06/01/05 - Stencil Generator Improvement - Beta #13 - knv
   //Version = "4.5.397";  // 06/01/05 - Case #1461 - GeneralDialog.cpp - Camcad crashed after doing "Convert To PCB Component" - LP
                           //          - Case #1462 - PfwIn.cpp - Camcad crashed during import of Protel PCB Ascii - LP
   //Version = "4.5.396";  // 06/01/05 - Case #1453 - Centroid.cpp - Generate component centroids by pin extents now works - rcf
   //Version = "4.5.395";  // 06/01/05 - Variant.cpp - Last row is not editable - LP
                           //          - DataDoctor.cpp&h, DataDoctorDialog.cpp&h - Need to make call to load data from library the first time the "Part" tab is show - LP
   //Version = "4.5.394";  // 06/01/05 - Removed Warning for files coming from visECAD --MAN
   //Version = "4.5.393";  // 05/31/05 - Preserved <Topics> and <ColorSets> sections from visECAD --MAN  
   //Version = "4.5.392";  // 05/31/05 - Case #1432 - QD_out.cpp&h - Added two ".out" command, ".OUTPUT_ATTRIBUTE" and ".OUTPUT_VALUE" - LP
                           //          - DataDoctorDialog.cpp&h, DataDoctor.cpp&h - More modification to "Part" tab in Data Doctor - LP
   //Version = "4.5.391";  // 05/31/05 - Stencil Generator Improvement - Beta #12 - knv
   //Version = "4.5.390";  // 05/31/05 - Case #1414 - ProbePlacement.cpp&h, DFT.h - Speed improvements in probe placement - rcf
   //Version = "4.5.389";  // 05/27/05 - DataDoctorDialog.cpp&h, Response.cpp&h - Added Browse to allow users to browse to a different Part Library database - LP
                           //          - QD_out.cpp - Added option for users to choose whether to mirror X or Y when output the bottom file for Siemen QD - LP
   //Version = "4.5.388";  // 05/27/05 - Completed adding "Pin Mapping" and "Store Not in Library" - LP
   //Version = "4.5.387";  // 05/26/05 - Case #1429 - DdeIn.cpp - Some polygons get class Etch too, not mentioned in original case message - rcf
   //Version = "4.5.386";  // 05/26/05 - Completed the removal of global variable "userPathName" - LP
   //Version = "4.5.385";  // 05/26/05 - Case #1442 (related to) - Pcb_Util.cpp - Update gencad reader to recognize unplaced probes - rcf
   //Version = "4.5.384";  // 05/26/05 - Xml_cont.cpp - Case 1452 - Fixed handling of probe_placement and test_resource attributes in xml reader - rcf
   //Version = "4.5.383";  // 05/25/05 - Stencil Generator Improvement - Beta #11 - knv
   //Version = "4.5.382";  // 05/25/05 - Added Pin Mapping to "Part" tab, but not complete yet - LP
   //Version = "4.5.381";  // 05/25/05 - Added support for Parts Library new function "AddPartsNotInLibrary" - MAN   
   //Version = "4.5.380";  // 05/25/05 - Case #1442 - GenCad_o.cpp - Change coords for unplaced probes from 0,0 to -32767,-32767 - rcf
                           //          - Case #1429 - DdeIn.cpp&h - Change traces from class Normal to class Etch - rcf
   //Version = "4.5.379";  // 05/25/05 - Stencil Generator Improvement - Beta #10 - knv
   //Version = "4.5.378";  // 05/24/05 - Added "Library" check field and "Description" field to "Parts" tab in DataDoctor dialog - LP
   //Version = "4.5.377";  // 05/24/05 - Teradyne Spectrum - TeradyneSpectrum_Out.cpp&h, Ipl_out.cpp&h - Various format changes and information additions to output, per email spec from Mark 5/02/05 - rcf
   //Version = "4.5.376";  // 05/23/05 - Case #1444 - DFT.cpp - Probe name is too large or too small after Probe Name Sort for units other than INCH
                           //          - Case #1445 - ProbePlacement.cpp&h - Multiple message boxes information in correct data for current in Net Condition - LP
   //Version = "4.5.375";  // 05/23/05 - Stencil Generator Improvement - Beta #9 - knv
   //Version = "4.5.374";  // 05/23/05 - Changed more file to replace the global variable "userPathName" with "getApp().getUserPath()" - LP
   //Version = "4.5.373";  // 05/22/05 - Case #1441 - MVP_AOI_Comp_Out.cpp&h - Fixed pin centroid problem found by Mark during testing - rcf
   //Version = "4.5.372";  // 05/20/05 - Remove global varialbe "CAMCADtitle" and replaced with "getApp().getCamCadTitle()" and replace some use of global "userPathName" with "getApp().getUserPath()" - LP 
                           //          - ProbePlacement.cpp&h - Moved the checkbox for "Write Ignore Surface to Report" to the "Option" tab in Probe Placement dialog - LP
   //Version = "4.5.371";  // 05/20/05 - Completed implementation of options in "Project Path" dialog to allow user to set project path by 1)the first file loaded or 2)the current path user set in the dialog - LP
                           //          - Changed many file to replace the global variable "projectPathName" with "getApp().GetProjectPath()" - LP
   //Version = "4.5.370";  // 05/19/05 - Case #1441 - MVP_AOI_Comp_Out.cpp&h - Changed component location to be centroid location (was insert location) and made pin locations relative to centroid (not insert) location - rcf
   //Version = "4.5.369";  // 05/19/05 - Camcad.cpp&h, Prj_Path.cpp&h - Not complete yet, only added class and option in "Project Path" dialog to allow user to set project path by 1)the first file loaded or 2)the current path user set in the dialog - LP
   //Version = "4.5.368";  // 05/19/05 - Case #1441 - pads writer crash --MAN
   //Version = "4.5.367";  // 05/19/05 - Enable/Disable default PN in bom delimited import --MAN
   //Version = "4.5.366";  // 05/19/05 - Stencil Generator Improvement - Beta #8 - knv
   //Version = "4.5.365";  // 05/18/05 - Case #1299 - CadifIn.cpp - Pads to bottom side test points were on wrong side (top) - rcf
   //Version = "4.5.364";  // 05/18/05 - Case #1433 - Accel_out.cpp - Accel Write, geometry rename was not correctly applied to all locations - LP
                           //          - Case #1437 - GenCad_o.cpp - GenCad Write, some padstack definition is missing pads - LP
   //Version = "4.5.363";  // 05/18/05 - CAMCADNavigator.cpp - Net count label of "Net Without Access" is not updated correctly - LP
                           //          - AccessAnalysisDlg.cpp - Filter out non-pcb components - LP
                           //          - ProbePlacement.cpp&h, DFT.cpp&h, resource.h, Camcad.rc - Added checkbox option to write ignored surface to report and save the setting of that option and net condition case sensitive option to testplan- LP
   //Version = "4.5.362";  // 05/17/05 - MVP "Amendment #041205b" - MVP_AOI_Out.cpp/h, MVPAOI_Paste_Out.cpp - Multiple changes to record formats, thickness determination - rcf
   //Version = "4.5.361";  // 05/17/05 - ProbeStyles.cpp - Change Default color in Probe Matrix from black to red - LP
                           //          - ProbePlacement.cpp - Fixed "Add Additional Probes" to work correctly - LP
   //Version = "4.5.360";  // 05/15/05 - Case 1417 - MentorIn.cpp/h - handle assymetric pad rules - rcf
                           //          - Case 1425 - DeviceTyper.cpp/h - Part 1 of 2 part case, Fixed violation count error in report - rcf
   //Version = "4.5.359";  // 05/13/05 - Changes to BOM CSV Reader.  MAN
   //Version = "4.5.358";  // 05/13/05 - Case #999 - Nets not analyzed in Access Analysis should be set as "No Probe" and should not show in "Net Without Access" in PCB Navigator - LP
                           //          - ProbePlacement.cpp - Target Priority is not showing in Probe Placement - LP
                           //          - Xml_Cont.cpp - Need to type "TEST_ACCESS_TOP", "TEST_ACCESS_BOT", "PROBE_TOP", "PROBE_BOT", "PROBEDRILL_TOP", and "PROBEDRILL_BOT" are DFT_TOP or DFT_BOTTOM during import - LP
   //Version = "4.5.357";  // 05/12/05 - Case #1428 - GenCad_o.cpp, GenCadIn.cpp - Do not output the attribute "DATALINK" during output and ignore that attribute during import - LP
   //Version = "4.5.356";  // 05/11/05 - Case #1427 - PfwIn.cpp - Component inserts of instance specific geometries need to point to the correct device($$DEVICE$$) - LP
   //Version = "4.5.355";  // 05/11/05 - Case #1384 - PADS 2005 support, MAN
   //Version = "4.5.354";  // 05/11/05 - Takaya_o.cpp&h - The second XY location for Test for IC Opens Tests is calculated by offseting the pin location to an outline is that 5mm smaller than DFT outline - LP
   //Version = "4.5.353";  // 05/10/05 - Agilent SOW for AOI SP50 writer - Beta #4 - knv
   //Version = "4.5.352";  // 05/10/05 - Agilent SOW for AOI SP50 writer - Beta #3 - knv
   //Version = "4.5.351";  // 05/10/05 - Agilent SOW for AOI SP50 writer - Beta #2 - knv
   //Version = "4.5.350";  // 05/10/05 - Case #1080 - ProbePlacement.cpp, DFT.h - Organized Probe Placement report into Multiple Pins, Single Pin, and Unconnected Pin sections - LP
                           //          - Case #1406 - AccessAnalysisDlg.cpp - CAMCAD crashed when reading NetAccess section due to unexpect data tye for a certain entity number becasue the entity number was duplicate for a poly and comppin - LP
                           //          - Takaya_o.cpp - Added more fixes for IC Opens Tests - LP
   //Version = "4.5.349";  // 05/09/05 - Agilent SOW for AOI SP50 writer - Beta #1 - knv
   //Version = "4.5.348";  // 05/06/05 - Case #1180 - GenerateCentroidAndOutline.cpp - Was truncating angle instead of rounding, e.g. 89.9998201 beccame 89 degrees, user expected 90. Saving angle in cen file fixed in version 4.5.330 - rcf
   //Version = "4.5.347";  // 05/06/05 - Case #1413 - GenCadIn.cpp - Not all traces are read onto the correct layer - LP
                           //          - Case #1423 - PfwIn.cpp - Protel Read, geometry names should not be case sensitive so that it would create two duplicate name as such "0805_w" and "0805_W" - LP
                           //          - Case #1424 - PfwIn.cpp - Protel Read, instance specific geometry does not have device - LP
   //Version = "4.5.346";  // 05/06/05 - Case #1362 - Dfm_Flat.cpp - Related to case #1362, fixed a bug in the funciton FlattenBlockByInserttype(), the main fix in DFT Runner - LP
   //Version = "4.5.345";  // 05/05/05 - RealPart.CPP (and PolyLib) - Transforming pads for RealPart - DD
   //Version = "4.5.344";  // 05/04/05 - Takaya_o.cpp&h, Export.h - LP
                           //             - Added ".out" command ".SELECT_VISION" to allow output of vision test for all components that are not tested 
                           //             - Added support of IC Opens test for IC, ICLinear, ICDigital, CapacitorTantilum, CapacitorPolarize
   //Version = "4.5.343";  // 05/04/05 - Case #1412 - Pads Write - Traces wider than 250 mils must be written as lines. - knv
   //Version = "4.5.342";  // 05/04/05 - No case# - DFT.cpp/h, ProbePlacement.cpp/h, camcad.rc, resource.h - Added button to output Net Conditions to CSV file - rcf
   //Version = "4.5.341";  // 05/04/05 - Case #1422 - Spea_Out.cpp - default test value units fixed - rcf
   //Version = "4.5.340";  // 05/03/05 - Case #1419 - Incorrect PLX output - knv
   //Version = "4.5.339";  // 05/02/05 - Case #1420 - AccessAnalysisDlg.cpp, Block.cpp - Access Analysis has a bug due to padstack not analized correctly in funtion getDesignSurfaceFromLayer() - LP
   //Version = "4.5.338";  // 05/02/05 - QD_out.cpp - Refixed the ".out" command ".COMP_POSITION" - LP
   //Version = "4.5.337";  // 04/29/05 - Format.cpp, Format.h, format_s.h, PortLib.cpp, Resource.h, CAMCAD.rc, Ingun_o.cpp, Ipl_out.h - TRI MDA add ICT support, add Nets and Parts file output - rcf
                           // 05/29/05 - Case #1419 - Incorrect PLX output - knv
   //Version = "4.5.336";  // 04/29/05 - QD_out.cpp&h - Changed the ".out" command ".XY_LOCATION" to "COMP_POSITION" which include X&Y offset and rotation offset - LP
                           //          - Takaya_o.cpp&h - Takaya crash when using fiducial for reference point but no fiducial is select, and Takaya output double negative coordinates for bottom file when using fiducials that has negative x-coordinates - LP
   //Version = "4.5.335";  // 04/29/05 - Case #1410 - PfwIn.cpp&h - Protel Read, pin inserts in instance specific derived geometries are not typed correctly and also missing some circular graphic - LP
                           //          - ProbePlacement.cpp, DbUtil.cpp&h, pcbUtil.cpp - Added new layer types "DFT TOP" and "DFT BOTTOM" - LP
                           //          - QD_out.cpp - Siemen QD Read - for board insert of the bottom file do this "QD rotation = 360 - CAMCAD rotation" - LP
   //Version = "4.5.334";  // 04/28/05 - Case #1416 - Ipc_Out.cpp - Bottom side parts were absent from output due to flaw in determination of max pad size - rcf
                           //          - No Case# - MneutIn.cpp - Fixed asserts due to bad pointer handling in importer - rcf
   //Version = "4.5.333";  // 04/27/05 - Case #1415 - AllegIn.cpp - Allegro Read crashed due to invalid block reference and machanical components with pin edit that are only defined in "rounte" file are missing - LP
                           //          - Varian.cpp&h, DeviceType.cpp&h - Added "Compre to Board Data" funtion and populate device type in combo box in Variant Manager - LP
   //Version = "4.5.332";  // 04/26/05 - Case #1407 - PCB Navigator - Manually adding a probe crashes CAMCAD - knv
   //Version = "4.5.331";  // 04/26/05 - Case #1409 - AllegIn.cpp - Not all Board and Panel level fiducials are coming through becasue their refdes were created by refdes, sym_x, sym_y, sym_rot, sym_mir and might not be unique - LP
   //Version = "4.5.330";  // 04/26/05 - GenerateCentroidAndOutline.cpp - Save/restore rotation in cen file, refresh gui on cen file load - rcf
   //Version = "4.5.329";  // 04/26/05 - QD_out.cpp&h - Siemen QD Export - LP
                           //             - Added new command ".PANEL_FIDUCIAL_SYMBOL_NUMBER", ".BOARD_FIDUCIAL_SYMBOL_NUMBER, ".GLUE_TOP", and ".GLUE_BOTTOM" to "qd.out" file - LP
                           //             - Check for fiducial need take surface of top/bottom into consideration
   //Version = "4.5.328";  // 04/26/05 - Case #1370 - AllegIn.cpp - Machanical components are not coming through - LP
                           //          - Case #1408 - AllegIn.cpp - Need to explode Primary Board outline onto board level - LP
                           //          - Case #1227 - Report.cpp - Component report, change the head of last column to "LOADED" and value to either "TRUE" or "FALSE" - LP
   //Version = "4.5.327";  // 04/26/05 - No case# - SeicaParNodOut.cpp/h - Fixed problem with "F" and "Y" when probes are only on top side - rcf
   //Version = "4.5.326";  // 04/25/05 - No case# - SeicaParNodOut.cpp/h - Add offset (_SHIFT) records to test output - rcf
   //Version = "4.5.325";  // 04/25/05 - Added changes to Viscom output according to 4.5 requirements -MAN
   //Version = "4.5.324";  // 04/22/05 - Case #1404 - MainFrm.cpp, Camcad.cpp - The command line option of VIEWMODE=X, where X is an integer, does not work - LP
                           //          - Case #1405 - FatfIn.cpp - Some geometries are inserting padstack that has nothing in it - LP
   //Version = "4.5.323";  // 04/22/05 - BOM Attribute import changes -> set default pn -MAN
   //Version = "4.5.322";  // 04/22/05 - Case #1403 - AllegIn.cpp - Allegro Extract Read not take into consideration of PIN_ROTATION field - LP
   //Version = "4.5.321";  // 04/22/05 - Case #1284 - RealPart - Custom Apertures now handled as pads - DD
   //Version = "4.5.320";  // 04/20/05 - Changes to Attribut BOM Reader BomCSV.cpp, BomCSV.h, SelectDelimiterDlg.ccp/h  -MAN
   //Version = "4.5.319";  // 04/21/05 - No case# - Ipl_out.cpp/h - Teradyne 1800, sort tests (order specified by Mark), within test group sort by refdes - rcf
   //Version = "4.5.318";  // 04/21/05 - CcDoc.cpp - call the function GenerateBlockDesignSurfaceInfo() for any file generated before version 4.5.318 - LP
   //Version = "4.5.317";  // 04/20/05 - Attribute bom importer changes (interim update)
   //Version = "4.5.316";  // 04/20/05 - Xml_Read.cpp, CcDoc.h - Implement code to parse the CAMCAD product type and the CAMCAD version that generated the CC or CCZ file - LP 
                           //          - CcDoc.cpp - call the function GenerateBlockDesignSurfaceInfo() for any file generated before version 4.5.274 - LP
   //Version = "4.5.315";  // 04/20/05 - Case 1157 - FabMasterDeviceIn.cpp/h - Support multiple PART (refdes) lines in same record - rcf
   //Version = "4.5.314";  // 04/19/05 - Rename Attribut/BOM CSV to Attribute/BOM Delimited Import  -MAN
   //Version = "4.5.313";  // 04/19/05 - Data Doctor - Interface "Parts" property page with CCPartLibrary - knv
   //Version = "4.5.312";  // 04/19/05 - Case #1398 - ProbePlacement.cpp, DFT.h - Probe Placement does not save Target Priority settings - LP
   //Version = "4.5.311";  // 04/19/05 - Cases 1399.  changed default layer colors to white.  MAN
   //Version = "4.5.310";  // 04/19/05 - Case #1398 - ProbePlacement.cpp, DFT.h - Probe Placement does not take Target Priority into consideration - LP
   //Version = "4.5.309";  // 04/19/05 - Case 1157 - FabMasterDeviceIn.cpp/h - Support refdes as range list - rcf
   //Version = "4.5.308";  // 04/16/05 - No case - FabMasterDeviceIn.cpp - Removed 4 unused variable declarations - rcf
   //Version = "4.5.307";  // 04/16/05 - Case 1157 - FabMasterDeviceIn.cpp/h - Ability to correlate parts by refdes - rcf
                        //          - Case 1293 - CxfIn.cpp - make .FILL areas type Etch, stop making them vias - rcf
   //Version = "4.5.306";  // 04/15/05 - Data Doctor - Added "Parts" property page - knv
   //Version = "4.5.305";  // 04/15/05 - Case #1397 - Gerber Import - Some apertures are being overwritten when reading multiple gerber files - knv
   //Version = "4.5.304";  // 04/15/05 - Case #1399 - default layer types were wrong LyrType.cpp -MAN
   //Version = "4.5.303";  // 04/14/05 - Case #1188 - Device Typer - takes too long to launch with this board - knv
   //Version = "4.5.302";  // 04/13/05 - Case #1265 - Stencil Generator - Rule modifier alignment of apertures incorrect - knv
   //Version = "4.5.301";  // 04/13/05 - Case #1295 - GenCad_o.cpp, AccelIn.cpp - Added the command ".NORMALIZE_BOTTOM_BUILD_GEOM" to the ".IN" file of Accel Read and VBAscii and ".OUT" file of Gencad Write - LP
   //Version = "4.5.300";  // 04/13/05 - Case #925 - Add document coordinates to Edit Insert dialog - knv
   //Version = "4.5.299";  // 04/13/05 - Case #1401 - Draw.cpp.  commented out the code for drawing pin labels on highlighted pins.
   //Version = "4.5.298";  // 04/13/05 - Case #1262 - GerOut.cpp - Gerber Export did not consider text alignment when exporting text and attributes - LP
   //Version = "4.5.297";  // 04/12/05 - Case #924 - PCB Navigator - Add right click popup menu item to highlight the net of a selected pin - knv
   //Version = "4.5.296";  // 04/12/05 - Case #1400 - CcDoc.cpp&h - Implemented "Normalize Bottom Build Geometries" - LP
   //Version = "4.5.295";  // 04/12/05 - Case #1400 - Camcad.rc, resource.h, CcDoc.cpp&h - Only added menu option to do "Normalize Bottom Build Geometries", not yet implemented - LP
   //Version = "4.5.294";  // 04/12/05 - RealPart Bug - UntransformPoint did not handle mirrored & rotated correctly - XFORM.H - DD
   //Version = "4.5.293";  // 04/11/05 - Case #1399 - default layershow to off instead of on. MAN
   //Version = "4.5.292";  // 04/11/05 - Case #1367 - World view loses symbols when schematic link opened - knv
   //Version = "4.5.291";  // 04/11/05 - Fixes and cleanup for case #1390 - knv
   //Version = "4.5.290";  // 04/11/05 - Case #1395 - Vb99In.cpp&h - VB Ascii Read need to create derived geometry for any insert that has a PIN_OPTION that is difference from the original definition - LP
                           //          - Case #1394 - FatfIn.cpp - FATF Read did not correctly read the drill of geometry; therefore, incorrectly identify component as SMD instead of THRU - LP
                           //          - Case #1371 - Dde9_in.cpp - DDE V9 Read need to read ".txt" as attribute - LP
   //Version = "4.5.289";  // 04/07/05 - Case #1390 - Panning to nets in the PCB navigator generates an error message when it shouldn't - knv
   //Version = "4.5.288";  // 04/07/05 - Case #1354 - Format.cpp&h - Button handler for HP3070 "include vias" was entirely missing - rcf
   //Version = "4.5.287";  // 04/07/05 - Case #1395 - Vb99In.cpp&h - VB Ascii Read need to create derived geometry for any insert that has a PIN_OPTION that is difference from the original definition - LP
   //Version = "4.5.286";  // 04/07/05 - Case #1366 - Net_Hilt.cpp, Retain show-selected-nets-only setting between dialog uses - rcf
                           //                       - Also made Clear button respect current show-selected-only setting.
   //Version = "4.5.285";  // 04/06/05 - Case #1326 - Inherit ECAD data pin attribute incorrect for some rotated ICs - knv
                           //          - added support for generation of ATT_ECAD_GEOMETRY_NAME attribute.
   //Version = "4.5.284";  // 04/06/05 - Case #1368 - Sch_Link.cpp&h, Camcad.rc, resource.h - Modify the "Select Item" dialog display in Schematic Link to enable resizable - LP
   //Version = "4.5.283";  // 04/06/05 - Case #1385 - Create Access Pt and add datalink, net attribs for FabMaster NAIL (probe) import - rcf
   //Version = "4.5.282";  // 04/05/05 - Case #1310 - Problem with "Convert Draws to Apertures" - knv
   //Version = "4.5.281";  // 04/05/05 - Case #1078 - Report.cpp - Added the "List of via geometry" section in the Manufacturing report - LP
                           //          - Case #1373 - Attrib.cpp - Pre-register the attribute "Current" as value type double - LP
   //Version = "4.5.280";  // 04/05/05 - Case #844 - Make "Generate Centroid" dialog resizable. - knv
   //Version = "4.5.279";  // 04/04/05 - added warning to explode all geometries.  MAN
   //Version = "4.5.278";  // 04/04/05 - Case #1341 - Replace $CcRepairBlock$ smiley face with question mark - knv
   //Version = "4.5.277";  // 04/04/05 - Fixed About Dialog to allow selection on serial and host id. -MAN
   //Version = "4.5.276";  // 04/04/05 - Case #1391 - UnidatIn.cpp - Component that is placed bottom is incorrectly rotated and pins are swapped - LP
   //Version = "4.5.275";  // 04/01/05 - Stencil Generator Improvement - Beta #7 - knv
   //Version = "4.5.274";  // 04/01/05 - Case #1364 - AccessAnalysisDlg.cpp, Block.cpp&h - Re-implement the process of analysising padstack accessible surface and fixed the analysis of Access Analysis - LP
   //Version = "4.5.273";  // 03/30/05 - Case #1380, PadsIn.cpp&h, Tooling hole support - rcf
   //Version = "4.5.272";  // 03/30/05 - modified licensing dialog - MAN
   //Version = "4.5.271";  // 03/30/05 - Case #1387 - DRC_LIST.CPP - Panning to DRCs when File is Rotated - DD
   //Version = "4.5.270";  // 03/29/05 - Changed prerelease version scheme - knv
                           //          - Stencil Generator Improvement - Beta #6 - knv
   //Version = "4.5.0kj";  // 03/29/05 - Case #1386 - Unicam_o.cpp - Added initialization of clocation to 0, 0 to support cases where components have no pins. - DF
   //Version = "4.5.0ki";  // 03/29/05 - Updated About dialog - rcf
   //Version = "4.5.0kh";  // 03/29/05 - Added LAYTYPE_PKG_BODY_TOP & LAYTYPE_PKG_BODY_BOT - DD
   //Version = "4.5.0kg";  // 03/28/05 - Stencil Generator Improvement - Beta #5 - knv
   //Version = "4.5.0kf";  // 03/28/05 - Stencil Generator Improvement - Beta #4 - knv
   //Version = "4.5.0ke";  // 03/25/05 - Case #1328 - Cr5000In.cpp - Top side component is flagged mirror graphic and layer - LP
                           //          - Case #1320 - GeomLib.cpp&h - Issue #1 - arcs within a surface is not properly interpreted - LP
   //Version = "4.5.0kd";  // 03/23/05 - Stencil Generator Improvement - Beta #3 - knv
   //Version = "4.5.0kc";  // 03/23/05 - RealPart APIs & Bottom-Side Component Issue - DD
   //Version = "4.5.0kb";  // 03/23/05 - Case #1358 - Xml_Cont.cpp - When reading testprobe added the attributes "PROBE_PLACEMENT" and "TEST_RESOURCE" incase for those in the older version of CC file that don't have these attributes - LP
                           //          - Case #1374 - PadsIn.cpp&h - Fixed crashed and indicate new PADS version 2005 is detected and might not import properly - LP
                           //          - Case #1378 - CadifIn.cpp - Crashed importing CADIF file due to invalid reference - LP
   //Version = "4.5.0ka";  // 03/22/05 - Stencil Generator Improvement - Beta #2 - knv
   //Version = "4.5.0jz";  // 03/22/05 - Case #1361 - FatIn.cpp - When pinName of a package is not available or duplicate, use the pinID as pinName - LP
                           //          - Case #1369 - MentorIn.cpp - CAMCAD crashed when referencing an invalid block - LP
                           //          - Case #1372 - File.h - The function getHeadCompPin() didn't check if there is comppin before return so it sometime return invalid value - LP
   //Version = "4.5.0jy";  // 03/18/05 - Stencil Generator Improvement - Beta #1 - knv
   //Version = "4.5.0jx";  // 03/18/05 - Case #1280 - FatfIn.cpp, PolyLib.cpp - In the funciton CalcTransform(), the calculation of whether a poly is built clockwise or counter clockwise is incorrect, which result in error in X&Y offset - LP
   //Version = "4.5.0jw";  // 03/18/05 - Stencil Generator Improvement - Alpha #16 - knv
   //Version = "4.5.0jv";  // 03/17/05 - Case #1292 - Problem with Realpart violations not showing.  changed Dfm_Dist.cpp -MAN   
   //Version = "4.5.0ju";  // 03/17/05 - Case #1299 - CadifIn.cpp&h - Cadif through hole padstack still come in on the wrong side, need to create mirrored version if through hold padstack is inserte mirror - LP
                           //          - Case #1352 - pcbUtil.cpp - TECHNOLOGY attribute is not calculated correctly - LP
   //Version = "4.5.0jt";  // 03/15/05 - Case #1279 - Gencad_o.cpp&h - Need mirrored version of padstack if it is used in the "$ROUTES" section - LP
                           //          - Case #1351 - CadifIn.cpp&h - Camcad crashed due to number package exceeded packageName array, also X & Y coordinates were set to zero when they exceed 100 inches - LP
   //Version = "4.5.0jq";  // 03/14/05 - Added SoftwareExpiration.h/.cpp - MAN   
   //Version = "4.5.0jr";  // 03/11/05 - Stencil Generator Improvement - Alpha #15 - knv
   //Version = "4.5.0jq";  // 03/11/05 - Stencil Generator Improvement - Alpha #14 - knv
   //Version = "4.5.0jp";  // 03/10/05 - Case #1315 - Centroid.cpp - Generate centroid should type the layer as "Centroid Top" and "Centroid Bottom" - LP
                           //          - Case #1336 - CadifIn.cpp - Cadif import is missing netlist - LP
   //Version = "4.5.0jo";  // 03/10/05 - Case #1350 - Allegro Read, Panel does not step and repeat board instead - LP
                           //          - Case #1334 - Allegro Read, Component "V5" is missing due to testpoint having the same name - LP
                           //          - Case #1325 - Allegro Read, Panel geometry is incorrectly typed as PCB Design - LP
                           //          - Case #1311 - Allegro Read, Linear Dimension text is not rotated correctly - LP
   //Version = "4.5.0jn";  // 03/09/05 - Added GetDataExtents API - MAN   
   //Version = "4.5.0jm";  // 03/09/05 - Case #1087 - GenCad_o.cpp&h - Gencad Export - Outputing duplicate DEVICE in the $DEVICES section - LP
   //Version = "4.5.0jl";  // 03/09/05 - Stencil Generator Improvement - Alpha #13 - knv
   //Version = "4.5.0jk";  // 03/08/05 - Case #1329 - Fixed error in MENTORIN.CPP where a message box pops up indefinitely due to a buffer larger than 64000 -MAN
   //Version = "4.5.0jj";  // 03/08/05 - Case #1347 - Fixed error in fatf reader leaving file path in the file name -MAN
   //Version = "4.5.0jf";  // 03/07/05 - Stencil Generator Improvement - Alpha #12 - knv
   //Version = "4.5.0je";  // 03/06/05 - Stencil Generator Improvement - Alpha #11 - knv
   //Version = "4.5.0jd";  // 03/04/05 - Stencil Generator Improvement - Alpha #10 - knv
   //Version = "4.5.0jc";  // 03/04/05 - Case #1339 - Sm_Anal.cpp - Access Analysis does not expose metal correctly when there are mulitple pads in the padstack - LP
   //Version = "4.5.0jb";  // 03/04/05 - Stencil Generator Improvement - Alpha #9 - knv
   //Version = "4.5.0ja";  // 03/03/05 - Case #1312 - CcDoc.cpp&h - The function purgeUnusedBlocks() and purgeUnusedWidths() are using CMapWordToPtr which resulted in expected result due to data lost during conversion from interget to WORD - LP
   //Version = "4.5.0iz";  // 03/02/05 - Stencil Generator Improvement - Alpha #8 - knv
   //Version = "4.5.0iy";  // 03/02/05 - Case #1340 - LyrManip.cpp - Fixed crash when purge unused layers on files without unused layers --MAN
   //Version = "4.5.0ix";  // 03/01/05 - Stencil Generator Improvement - Alpha #7 - knv
   //Version = "4.5.0iw";  // 03/01/05 - Case #1289 & #1330 - AllegIn.cpp&h - Duplicate data being written into geometries -  LP
   //Version = "4.5.0iv";  // 02/28/05 - Stencil Generator Improvement - Alpha #6 - knv
   //Version = "4.5.0iu";  // 02/28/05 - Case #1296 - CAMCAD.RC, resource.h - Removed the "More Info" button from DRC List dialog - LP
                           //          - Case #1307 - AllegIn.cpp - Allegro Read, didn't interprete plated and non-plated tool hole correctly - LP
                           //          - Case #1327 - AllegIn.cpp - Allegro Read, Incorrect message was writen to the log regarding the type of Allegro file - LP
   //Version = "4.5.0it";  // 02/25/05 - Stencil Generator Improvement - Alpha #5 - knv
   //Version = "4.5.0is";  // 02/25/05 - Case #1227 - Report.cpp - Added LOADED/UNLOADED attribute to the Component report - LP
   //Version = "4.5.0ir";  // 02/25/05 - SelectDelimiterDlg.cpp&h - Added delimited dialog to allow users to choose delimiter for the imported file for BOM/Attribute CSV - LP
   //Version = "4.5.0iq";  // 02/24/05 - Stencil Generator Improvement - Alpha #4 - knv
   //Version = "4.5.0ip";  // 02/24/05 - Add "IDD_SELECT_DELIMITER" dialog to allow selection of delimiter used for imported delimited file - LP
   //Version = "4.5.0io";  // 02/25/05 - Case #1335 - Fixed CAMCAD crash when loading file with Multiple Machine - LP
   //Version = "4.5.0in";  // 02/24/05 - Stencil Generator Improvement - Alpha #3 - knv
   //Version = "4.5.0im";  // 02/22/05 - QD_out.cpp&h - Added check for fiducial before output "LP LAGE ERKENNUNG" and output "Board Part Number" as comment if it is given - LP
   //Version = "4.5.0il";  // 02/22/05 - Format.cpp&h, Format_s.h, CAMCAD.RC, resource.h - Siemen QD Export dialog, added textbox for "Board Part Number" and option to mirror in X or Y - LP
   //Version = "4.5.0ik";  // 02/22/05 - CAMCADNavigator.cpp - Fixed the select dialog for Variant to include "~Default Variant~" - LP
   //Version = "4.5.0ij";  // 02/18/05 - License.cpp - Added new message boxed for maintainence information - AC
                           //          - Fixture_reuse.cpp - updated the fixture reuse log file with new structure
   //Version = "4.5.0ii";  // 02/18/05 - Case #1305 - PdifIn.cpp - Vias are not imported from PCAD - LP
                           //          - Case #1275 - Hp3070_o.cpp - "3070_probeaccess" attribute get recreated even when "Recreate 3070 Attribute" is unchecked during export - LP
                           //          - Case #1322 - Hp3070_o.cpp - "3070_probeside" attribute get recreated even when "Recreate 3070 Attribute" is unchecked during export - LP
   //Version = "4.5.0ih";  // 02/18/05 - Case #1308 - Unicam_o.cpp & h - Unicam writer writes Electrical and Mechanical pins to Shape records, but, mechanical pins are not concidered in the block->getPinCentroid() resulting in incorrect clocations. - DF
   //Version = "4.5.0ig";  // 02/17/05 - Stencil Generator Improvement - Alpha #2 - knv
                           //          - Case #1318 - Inherit ECAD data results not correct for multiple pads over a single comp pin - knv
   //Version = "4.5.0if";  // 02/17/05 - Case #1291 - Edif300_in.cpp - CAMCAD crashed when reading Edif 300 file - LP
                           //          - CAMCADNavigator.cpp - Do not display message box when variant is applied - LP
                           //          - SeicaParNodOut.cpp - Make sure value doesn't end with "." - LP
   //Version = "4.5.0ie";  // 02/17/05 - Fixture_Reuse.cpp&h - Completed first cut of the fixture reuse functionality (on boards only) - AC
   //Version = "4.5.0id";  // 02/17/05 - SeicaParNodOut.cpp&h - The dialog asking "Probes found on both top and bottom sides.  Is the top side the primary access side?" doesn't work correctly - LP 
   //Version = "4.5.0ic";  // 02/17/05 - Case #1323 - DFT.cpp - CAMCAD crashed when reading due to invalid character in netname in "NetCondition" section during export - LP
   //Version = "4.5.0ib";  // 02/16/05 - Stencil Generator Improvement - Alpha #2 - knv
   //Version = "4.5.0ia";  // 02/16/05 - SeicaParNodOut.cpp&h - Fixed output of PIN column, CHANNEL column, UNIT column, and VALUE column - LP
   //Version = "4.5.0hz";  // 02/15/05 - Stencil Generator Improvement - Alpha #1 - knv
   //Version = "4.5.0hy";  // 02/15/05 - fixture_reuse.cpp/h  - Added fixture reuse functionality (Still only available only for debug) - AC
   //Version = "4.5.0hx";  // 02/15/05 - SeicaParNodOut.cpp - Fixed output of POS column of THRU via, PAD column for square pad, TECN column for SMD - LP
   //Version = "4.5.0hw";  // 02/15/05 - Rename the function FineDFTSolution() to FindDFTSolution() - LP
                           //          - Only launch Access Analysis and Probe Placement dialogs if the current machine use DFT or when there is no machine - LP
   //Version = "4.5.0hv";  // 02/15/05 - Case #1298 - Seica par nod export issues - knv
   //Version = "4.5.0hu";  // 02/15/05 - Changed Fiducial to Fid in MVP output.  Request from ML -- MAN
   //Version = "4.5.0ht";  // 02/14/05 - Case #1298 - Seica par nod export issues - knv
   //Version = "4.5.0hs";  // 02/10/05 - Case #1314 - Region.cpp - Fixed a bug in DiffRegions() due to lost of accuracy during convertion from long to WORD in saving Y coordinates to map - LP
   //Version = "4.5.0hr";  // 02/10/05 - Search.cpp  - Fixed selection of text the text is rotated
   //Version = "4.5.0hq";  // 02/09/05 - Maded changes to QD_OUT.cpp to fix a bug with X offsets.  MAN
   //Version = "4.5.0hp";  // 02/09/05 - Case #1301 - Purge Unused Geometries does not remove Device Types referencing purged geometries - knv
                           // 02/09/05 - Case #1303 - The CC Repair function adds geometries and a layer with confusing names - knv
                           // 02/09/05 - Case #1251 - Convert Draws to Apertures is not working for certain shapes - knv
   //Version = "4.5.0ho";  // 02/08/05 - Search.cpp - Fixed selection of text when on bottom or if the text was mirrorred - AC
   //Version = "4.5.0hn";  // 02/08/05 - MultipleMachine.cpp&h, CAMCADNavigator.cpp, File.cpp - Machine of type AOI, AXI and PNP do not use DFT - LP
   //Version = "4.5.0hm";  // 02/08/05 - Case #1248 - Spea_Out.cpp - Package name need to be limit to 30 character and has to be unique - LP
                           //          - Case #1281 - GenCadIn.cpp - GenCad Read need to attach attribute in the $DEVICES section to component - LP
                           //          - Case #1306 - Sm_Anal.cpp - Expose metal need to also consider SIGNAL_ALL layer type - LP
   //Version = "4.5.0hl";  // 02/07/05 - Case #1300 - Xml_wrt.cpp - Fixed writing to attibutes to only write visible members when any have been changed - AC
   //Version = "4.5.0hk";  // 02/07/05 - Case #1286 - MentorIn.cpp - Fixed reading of double quoted fields in the net file (namely the net names) - AC
   //Version = "4.5.0hj";  // 02/04/05 - Fixed issue found in the probe placement results - AC
   //Version = "4.5.0hi";  // 02/03/05 - Added reading and writing of Probe Placement result and Multiple Machine to CC File - LP
   //Version = "4.5.0hh";  // 02/03/05 - ProbePlacement.cpp - Added implementation in CProbePlacementSolution::WriteXML() and CProbePlacementSolution::LoadXML() to write to and read from CC file - LP
                           //          - CcDoc.h - Changed CC file version number to 7.0 - LP
   //Version = "4.5.0hg";  // 02/02/05 - Fixed pilot drill generation logic to correctly finish - AC
   //Version = "4.5.0hf";  // 02/02/05 - Added implementation to re-apply DRC for both Access Analysis and Probe Placement when switch between machines - LP
   //Version = "4.5.0he";  // 02/02/05 - Added implementation to re-apply access points and probes when switching between machines; however DRC is not reapply yet - LP
   //Version = "4.5.0hd";  // 02/02/05 - Disabled licensing for "Stencil Stepper" - knv
                           //          - Region outline algorithm update - Beta #2 - knv
   //Version = "4.5.0hc";  // 02/01/05 - Fixed bug in the Excellon and Fixture output of pilot drill holes to correctly manage drill sizes - AC
   //Version = "4.5.0hb";  // 02/01/05 - Region outline algorithm update - Beta #1 - knv
   //Version = "4.5.0ha";  // 02/01/05 - MVP_AOI_Comp_Out.cpp - For [QFP] section, swap the 2nd and 3rd field - LP
   //Version = "4.5.0gz";  // 02/01/05 - Case #1277& #1294 - AccelIn.cpp - Remove "_Primary" from geometry name for the "Primary" PatternGraphicNameDef and remove "_1" from graphic name instead of using original name - LP
   //Version = "4.5.0gy";  // 01/31/05 - Case #1257 - Takaya writer - Poor output performance - knv
   //Version = "4.5.0gx";  // 01/31/05 - Fixed Excellon and Fixture output of pilot drill holes to correctly manage drill sizes - AC
   //Version = "4.5.0gw";  // 01/27/05 - Removed dependency of StdAfx.h on General.h
                           //            StdAfx.h included General.h; General.h included Resource.h - 
                           //            therefore, anytime resource.h was modified, the entire project needed to be rebuilt.
                           //            1) Created new file GeneralDialog.h and moved class definitions from General.h into it.
                           //            2) Created new file GeneralDialog.cpp and moved class methods from General.cpp into it.
                           //            3) Removed reference of Resource.h from General.h
                           //            4) Removed reference of General.h from StdAfx.h
                           //            5) Changed all references to General.h to GeneralDialog.h or Resource.h in all other source files as appropriate.
                           //            6) Added references to either General.h, GeneralDialog.h, Resource.h to other source files as appropriate.
   //Version = "4.5.0gv";  // 01/27/05 - Fixed Excellon export to omit unplaced probes and to handle probe surface correctly - AC
   //Version = "4.5.0gu";  // 12/27/05 - Modified Select Dialog to make more generic and renamed CMulSelItem to CSelItem - LP
   //Version = "4.5.0gt";  // 01/27/05 - Added functionality to hide all datas on the Exposed Metal layers when they are not visible - AC
   //Version = "4.5.0gs";  // 01/27/05 - Modified Variant and Machine selection on PCB Navigator - LP
   //Version = "4.5.0gr";  // 01/26/05 - Region outline algorithm update - Alpha #3 - knv
   //Version = "4.5.0gq";  // 01/26/05 - Added .MaxZoom command to default.set file processing - knv
   //Version = "4.5.0gp";  // 01/26/05 - probeStyles.cpp - Fixed bug with addition of the Use flag in the probe styles dialog - AC
   //Version = "4.5.0go";  // 01/26/05 - Ipl_out.cpp - Fixed crash when trying to write to the log file when the variable is NULL - AC
                           //          - ckt_out.cpp - Fixed crash because of mishandling of pins with no ref name
   //Version = "4.5.0gn";  // 01/25/05 - Case #1285 - MVP_AOI_Comp_Out.cpp, Format.cpp&h, resource.h, Camcad.rc - Added the option to dialog to allow user to choose whether to use Package or Partnumber as the 6th field in the component section - LP
   //Version = "4.5.0gm";  // 01/24/05 - Case #1278 - AccessAnalysisDlg.cpp, DFT.h - Access Analysis goes into loops asking if user want to do Generate Pin Location - LP
                           //          - Case #1288 - Sm_Anal.cpp - Access Analysis is not calculating explode metal and soldermask correctly - LP
   //Version = "4.5.0gk";  // 01/24/05 - Case #1286 & #1287 - Sch_Link.cpp - fixed bug created by schlink not supporting duplicate instance names (change in previous specs) - AC
   //Version = "4.5.0gj";  // 01/24/05 - Region outline algorithm update - Alpha #2 - knv
   //Version = "4.5.0gi";  // 01/21/05 - Added more options for exporting probe drills in the excellon writer and removed leading zeros correctly - AC
                           //          - Fixed excellon reader to properly read new excellon output, using decimal points (.)
                           //          - Added Use check box to probe styles
   //Version = "4.5.0gh";  // 01/20/05 - Added a lot of implementation for Multiple Machine - LP
   //Version = "4.5.0gg";  // 01/20/05 - Refix Case #1250 - Ipl_out.cpp - Only output "9999" instead of "9999&9999" - LP
   //Version = "4.5.0gf";  // 01/19/05 - Region outline algorithm update - Alpha #1 - knv
   //Version = "4.5.0ge";  // 01/19/05 - Added option in the Excellon writer to write PCB drills or Probe Drills w/ or w/o Pilot holes - AC
   //Version = "4.5.0gd";  // 01/19/05 - Fixed handling of overriden and highlighted insert colors to use layer information correctly - AC
   //Version = "4.5.0gc";  // 01/19/05 - Fixed probe styles to use probe name as the name of the probe template, not the probe refname - AC
                           //          - Fixed probe styles to use drill diameter to use the drill of the feature, not the probe
   //Version = "4.5.0gb";  // 01/19/05 - Case #1222 - Mirroring TH component padstacks when reading a PAF file needs to change - Part 3 - knv
   //Version = "4.5.0ga";  // 01/19/05 - Refix Case #1274 - MVP_AOI_Comp_Out.cpp&h - Filter only passes components with an even number of pins into SOIC and QFP, odd numbered geometries should stay in USER - LP
   //Version = "4.5.0fz";  // 01/19/05 - Case #1222 - Mirroring TH component padstacks when reading a PAF file needs to change - Part 2 - knv
   //Version = "4.5.0fy";  // 01/18/05 - Refix Case #1250 - Ipl_out.cpp - Listing of probe for Resistor, Capacitor, and Inductor should be formated "n&n", also need to update REFNAME attribute with new probe name - LP
                           //          - Case #1274 - MVP_AOI_Comp_Out.cpp&h - Fixed the output of first section, SOIC section and QFP section - LP
   //Version = "4.5.0fx";  // 01/17/05 - Case #1222 - Mirroring TH component padstacks when reading a PAF file needs to change - knv
   //Version = "4.5.0fw";  // 01/17/05 - Case #1252 - AccelIn.cpp - Component attributes are not on the correct layer - LP
                           // 01/17/05 - Case #1267 - BarcoIn.cpp - Set complex shapes to fill - LP
   //Version = "4.5.0fv";  // 01/17/05 - Case #1270 - FatfIn.cpp/h - Added option in the in file to specify a default surface if none is specified in the Nail file - AC
                           // 01/17/05 - Case #1269 - FatfIn.cpp/h - Added visible attribute to nails read from the nail file
   //Version = "4.5.0fu";  // 01/14/05 - Added more rules to processing probe placement with probe locking (still incomplete) - AC
   //Version = "4.5.0ft";  // 01/14/05 - Added function to copy DFT solution and to load Machine list into Multiple Machine Dialog - LP
   //Version = "4.5.0fs";  // 01/13/05 - Added probe locking functionality, though not working correctly (disabled for now) - AC
   //Version = "4.5.0fr";  // 01/13/05 - Added Multiple Machine Management dialog but implement completely yet - LP
   //Version = "4.5.0fq";  // 01/10/05 - Case #1248 - Spea_out.cpp - Restrict package name in spea output to 30 chars. MAN
   //Version = "4.5.0fp";  // 01/10/05 - Case #1230 - AllegIn.cpp - Fixed "Breaking out" of etches to occur on all instances of packages (not just the first one) - AC
                           // 01/10/05 - Xml_wrt.cpp - Fixed incorrect write format due to change of EXPOSED_METAL_DIAMETER value type
   //Version = "4.5.0fo";  // 01/10/05 - Case #1257 - Takaya_0.cpp&h - Need to apply the mirror of X of tooling hole used for board and auxilliary for bottom file - LP
   //Version = "4.5.0fn";  // 01/10/05 - Case #1203 - ViscomAOI_Out.cpp - Added ".PACKAGE_NAME" to ".in" file to allow user to specify what to use for package name in "*,cad" output file - LP
   //Version = "4.5.0fm";  // 01/07/05 - Synchronization of 4.4 code with 4.5 - Part 1 - knv
   //Version = "4.5.0fl";  // 01/06/05 - Excellon.cpp - Modified export settings dialog for excellon writer and coordinate output to correct formats - AC
                           // 01/06/05 - Xml_cont.cpp, Attrib.cpp, ... - Changed the value type of EXPOSED_METAL_DIAMETER from a double to a unit-double
                           // 01/06/05 - CCDoc.h - Up rev'ed the CC doc version to 7.0 because of the attibute type change above
                           // 01/06/05 - Xml_cont.cpp - Updated message when loading in a later CC doc than currently handled
   //Version = "4.5.0fk";  // 01/06/05 - Case #1238 - Edif300_in.cpp - Added command ".MINIMUM_PAGE_SIZE" in ".in" file to scale file if the X or Y size largest schematic sheet is smaller than the minimum size specifed - LP
                           // 01/05/05 - Case #1266 - CxfIn.cpp - Fixed error in cxf_fill() when parsing an .ARC in a .FILL struct. Fixed error in cxf_pcbmaclib() when a .FILL is directly followed by another command. Fixed error in cxf_netlist_pcbsubtree() when a .PCBSUBTREE does not contain a .FLAGS struct. - LP                         
   //Version = "4.5.0fj";  // 01/05/05 - Fixed CamCad.rc Data Doctor Component Page resource - knv
   //Version = "4.5.0fi";  // 01/04/05 - Version.cpp - Fixed date parsing to check for valid date - knv
   //Version = "4.5.0fh";  // 01/04/05 - Ipl_out.cpp - Case #1250 - Teradyne Z1800, Channel spacing need to be applied to nets that have more than two probes - LP
                           //          - DFT.cpp - Value type for setting the "Exposed Metal Diameter" attribute should be Unit double - LP 
   //Version = "4.5.0fg";  // 01/03/05 - Case #1253 - AccelIn.cpp/h - (v4.4.514) Layers are now being assigned the appropriate attribute based on flags in the file. - TLS
   //Version = "4.5.0ff";  // 01/03/05 - Case #1226 - Inconsistant use of Ohm for resistors in Data Doctor - knv
   //Version = "4.5.0fe";  // 12/29/04 - LEDA test #2  - knv
   //Version = "4.5.0fd";  // 12/29/04 - Case #1255 - Spurred rectangle needs to be simpler - knv
                           //          - Added Rectangled Pentagon shape
                           // 12/29/04 - MVP AOI Writer - Fixed pin row spacing bug - knv
   //Version = "4.5.0fc";  // 12/28/04 - Case #1249 - Ipl_out.cpp/h - 
                           //            Jumper types now have probes delimited by & instead of comma
                           //            Zener type is now prefixed with just a Z
                           //            Now when multiple probes are on a net, only the lowest numbered one is exported
                           //            Unplaced probes are now represented by a single output of 9999 - TLS
   //Version = "4.5.0fb";  // 12/27/04 - MVP AOI Writer - Added Format dialog, updated writer - knv
   //Version = "4.5.0fa";  // 12/23/04 - Testway_in.cpp - Fixed Nail assignment reader - AC
                           //          - ProbeStyles.cpp&h - Changed target type names (component and testpoint)
                           //          - ExcelOut.cpp - Added new tools to the tool section for the pilot drills
                           //          - Xml_Cont.cpp - Exposed Metal Attribute should be a Unit double
   //Version = "4.5.0ez";  // 12/23/04 - DFT.cpp&h, AccessAnalysisDlg.cpp&h, Sm_Anal.cpp&h - Finished implement Height Analysis - LP
                           //          - Spea_Out.cpp - Change the checking of mirror flag to place bottom flag to determine if a component is bottom - LP
   //Version = "4.5.0ey";  // 12/22/04 - Added configuration to support LEDA library - knv
   //Version = "4.5.0ex";  // 12/22/04 - DFT.cpp&h, AccessAnaysisDlg.cpp&h - Added implementation of Height Analysis setting in dialog, CC file, and Testplan - LP
   //Version = "4.5.0ew";  // 12/20/04 - Case #1214 - Data Doctor - Value column should not be blanked out - knv
   //Version = "4.5.0ev";  // 12/20/04 - Port.h, Port_Lib.cpp, QD_Out.cpp/h - Case #1225 - (4.4.503) Now populating dialog with .out file thickness settings.  File/View verifications occur before any user input is requested.  QD out is now implemented within its own class. - TLS
   //Version = "4.5.0eu";  // 12/20/04 - Case #1165 - Recreate 3070 attributes during export option mot working - knv
   //Version = "4.5.0et";  // 12/16/04 - Camcad.rc, resource.h - Added option to Excellon writer unit optionsand added Probe Style Matrix menu item - AC
                           //          - Ccdoc.cpp/h - Added probe styles matrix support, adds "PROBE_STYLE" attribute to probes when applied
                           //          - fixture_out.cpp/h - Modified fixture writer to use attribute applied by probe styles
   //Version = "4.5.0es";  // 12/16/04 - Camcad.rc, resource.h, AccessAnalysisDlg.cpp&h - Modified the Height Analysis dialog and added code to do popup menu on right moust click - LP
   //Version = "4.5.0er";  // 12/15/04 - Case #1103 - Data Doctor grid issue (ShowCell) - knv
   //Version = "4.5.0eq";  // 12/15/04 - Case #1103 - Data Doctor grid issue - knv
   //Version = "4.5.0ep";  // 12/14/04 - Case #1236 - Agilent AOI SP50 output getting zero size apertures - knv
   //Version = "4.5.0eo";  // 12/14/04 - Case #1170 - Cadif.in command ".use_layername" is no longer working - knv
   //Version = "4.5.0en";  // 12/14/04 - Camcad.rc, resource.h, AccessAnalysisDlg.cpp&h - Added the new tab "Height Analysis" to Access Analysis, no implementation yet - LP
   //Version = "4.5.0em";  // 12/14/04 - Case #1241 - RealPart.cpp - (4.4.496) Now checking if polylist is null before querying against it. - TLS
   //Version = "4.5.0el";  // 12/14/04 - Case #1241 - unfixed - RealPart.cpp - (4.4.494) Fix broke results for RealPart.  Reverting changes until better solution is found. - TLS
   //Version = "4.5.0ek";  // 12/13/04 - Case #1241 - RealPart.cpp - (4.4.493) Now checking if polylist is null before querying against it. - TLS
   //Version = "4.5.0ej";  // 12/13/04 - Case #1231 - Takaya_o.cpp - Takaya Write need to not output duplicate test for top and bottom file - LP
                           //          - Case #1237 & #1238 - Sch_Lib.cpp - Reading of Edif files crash due to invalid reference of a net - LP
   //Version = "4.5.0ei";  // 12/13/04 - Case #1223 - Changed PCAD reader to create geometries that are specific to the new file - AC
   //Version = "4.5.0eh";  // 12/13/04 - Case #1232 - Exporting Agilent SJ AOI Files causes crash - knv
                           // 12/13/04 - Case #1240 - Select All Same DCode Draws small issue - knv
                           // 12/13/04 - Added the following functionality to CCView right click popup menu - knv
                           //            "Select all with same geometry"
                           //            "Select all with same D-Code (flashes)"
                           //            "Select all with same D-Code (draws)"
   //Version = "4.5.0eg";  // 12/10/04 - Refix case #1217 - Change the "Ohm" in value to "O" for component with non-supported device type - LP
                           //          - Ipl_out.cpp&h, TeradyneSpectrum_Out.cpp&h - Teradyne Write - Added output of "CMAP.txt" file - LP
   //Version = "4.5.0ef";  // 12/10/04 - ProbeStyles.cpp - First complete impmentation of probe styles with shape and colors (inclusion into UI still not determined so only availabel in debug version) - AC
   //Version = "4.5.0ee";  // 12/09/04 - ProbePlacement.cpp - Added hour glass to the supported shapes in CreateTestProbeGeometry - AC
                           //            CcDoc.cpp - Create preliminary implementation of probe shapes and colors
   //Version = "4.5.0ed";  // 12/09/04 - Ipl_out.cpp&h, TeradyneSpectrum_Out.cpp&h - Added output of via section to "nets.asc" and "parts.asc" files - LP
   //Version = "4.5.0ec";  // 12/09/04 - Refix Case #1217 - Takaya_o.cpp - Component with AptModel but non-supported device type still not outputing - LP
   //Version = "4.5.0eb";  // 12/08/04 - Refix Case #1155 - DFT.cpp - Added probe visible attribute so they show on all probes - AC
   //Version = "4.5.0ea";  // 12/08/04 - Added selection of new target types in the probe styles dialog - AC
                           //            Fixed the implementation of CMultiSelectDlg trying to use uninitialized variables
   //Version = "4.5.0dz";  // 12/08/04 - Case #1217 - Takaya_o.cpp - Need to be able to output component that has AptModel but do not have the supported device type - LP
                           //          - Ipl_out.cpp&h, TeradyneSpectrum_Out.cpp&h - Added output of via section to "tstparts.asc" and "pins.asc" files - LP
   //Version = "4.5.0dy";  // 12/08/04 - Ipl_out.cpp, TeradyneSpectrum_Out.cpp - Fixed the output of pins in IPL.DAT file - LP
   //Version = "4.5.0dx";  // 12/07/04 - Moved probe shape and coloring from probes section in Probe Placement to Probe Styles (only implemented in the dialog) - AC
                           //            Added hour glass shape to the probe shapes (only implemented in the dialog)
                           //            Modified highlighting of inserts to always show the insert even when the insert wasn't visible
   //Version = "4.5.0dw";  // 12/07/04 - Refix Case #1218 - Takaya_o.cpp - Need to check if the pin of components are tested by surface - LP
   //Version = "4.5.0dv";  // 12/07/04 - Code Review Case #1221 - EdidPoly.cpp - Make a few minor modification to the fix - LP
   //Version = "4.5.0du";  // 12/06/04 - Case #1221 - Search.cpp, Select.cpp, EditPoly.cpp - (4.4.479) Now setting the selected poly to Null if it gets deleted. Also testing width based on negative value instead of against only -1 - TLS
   //Version = "4.5.0dt";  // 12/06/04 - Case #1229 - Feature.cpp - Fixed featureData to get the correct serial number when using package files - AC
   //Version = "4.5.0ds";  // 12/06/04 - Case #1201 - Ingun_o.cpp - TRI MDA Write, remove "Ohm" from value field when output it in "tri.dat" file - LP
   //Version = "4.5.0dr";  // 12/06/04 - Case #1218 - Takaya_o.cpp - Takaya Writer wasn't writing to the log when a component is not tested in the file - LP
   //Version = "4.5.0dq";  // 12/06/04 - Case #1155 - Modified Probe sorting to work under new specifications - AC
   //Version = "4.5.0dp";  // 12/06/04 - Case #1184 - PADS ASCII Import not flagging Tools as Plated (for vias) - knv
   //Version = "4.5.0do";  // 12/03/04 - Case #1220 - PadsIn.cpp - Added the command ".PANELOUTLINE" to type poly as Primary Panel Outline if it is on layer name specified by the command - LP
   //Version = "4.5.0dn";  // 12/03/04 - Lic.cpp - Add licensing of Teradyne Spectrum Write - LP
   //Version = "4.5.0dm";  // 12/03/04 - DeviceType.cpp&h, TeradyneSpectrum_Out.cpp&h, Ipl_out.cpp&h - Added output of IPL.DAT file - LP
   //Version = "4.5.0dl";  // 11/30/04 - Case #1149 - Mneutin.cpp - Neutral Read, incorrectly apply TEST attribute to test via and pin - LP
   //Version = "4.5.0dk";  // 11/29/04 - Case #1184 - PADS ASCII Import not flagging Tools as Plated - knv
   //Version = "4.5.0dj";  // 11/29/04 - Case #1209 - AllegIn.cpp, Explode.cpp - Allegro Read - modified explode_geometries() to call ExplodeInsert() instead of doc->OnExplodeBlocks() to avoid checking for EDIT license - LP
   //Version = "4.5.0di";  // 11/29/04 - Case #1156 - Data.cpp/h,Draw.cpp,CamcadNavigator.cpp - Added data highlighting (sesstion based highlighting of Data's) - AC
   //Version = "4.5.0dh";  // 11/29/04 - Case #1207 - Xml_wrt.cpp - (v4.4.463) always write visibility info for string types - TLS
   //Version = "4.5.0dg";  // 11/29/04 - Case #1212 - Clr_Attr.cpp - (v4.4.462) Now taking care of default case and UNIT_DOUBLE in value type switch - TLS
   //Version = "4.5.0df";  // 11/29/04 - Case #1204 - ViscomAOI_Out.cpp - (v4.4.461) Added * to components that are not loaded - TLS
   //Version = "4.5.0de";  // 11/29/04 - Case #1202 - QD_Out.cpp - (v4.4.460) If no PartNumber, default is now 0 - TLS
   //Version = "4.5.0dd";  // 11/29/04 - Case #1197 - PolyLib.cpp&h - Modified the function Is_Pnt_Inside_PntList() to VectorizePntList the list of point before checking if the a given point is inside the list of point - LP
   //Version = "4.5.0dc";  // 11/29/04 - Case #1192 - CADIF comples pads look very ragged - knv
                           //            Fixed drawing code in Draw_Ent.cpp to perform a MoveTo after an ArcTo
   //Version = "4.5.0db";  // 11/29/04 - Case #1132 - Access Analysis - imcomplete soldermask analysis of poly on soldermask layer on board level - LP
   //Version = "4.5.0da";  // 11/29/04 - Fixed drawing bug of CAMCAD not drawing (in Release) because of the uninitialized Update variables on the CAMCADView - AC
   //Version = "4.5.0cz";  // 11/29/04 - Case #1189 - Data Doctor tolerance issue - knv
   //Version = "4.5.0cy";  // 11/29/04 - Case #1181 - Fixed board outline test to measure to the center of the board outline - AC
   //Version = "4.5.0cx";  // 11/29/04 - Case #1177 - FileList.cpp, File.cpp - Fixed updating of the UI when viewing board and/or panel(s) - AC
   //Version = "4.5.0cw";  // 11/24/04 - DFT files - Added functionality for new probe locking features (in options tab), only dialog reactions - AC
   //Version = "4.5.0cv";  // 11/24/04 - Modified many files to add Teradyne Spectrum Write, only initial implementation of the writer - LP
   //Version = "4.5.0cu";  // 11/24/04 - Case #1187 - (v4.4.451) CcView.cpp/h, Print.cpp, ToolbarM.cpp - In toolbarM.cpp I removed event-driven toolbar hiding/showing.  In print.cpp the extra redraw was removed.  CcView.cpp/h I added the ability to prevent redraws using a begin and end update function. - TLS
   //Version = "4.5.0ct";  // 11/23/04 - ProbePlacement.cpp&h - Added new controls for probe locking (new tab and in options tab) - AC
   //Version = "4.5.0cs";  // 11/23/04 - Case #1211 - QD_out.cpp - Added the command ".CADIF_ROTATION" to output Cadif specific rotation - LP
   //Version = "4.5.0cr";  // 11/23/04 - Case #1179 - ExcelOut.cpp - (v4.4.448) Width index was -1 on some polys; when this happens we now skip it - TLS
   //Version = "4.5.0cq";  // 11/23/04 - Camcad.cpp - Removed the build date from the version in the title bar - AC
   //Version = "4.5.0cp";  // 11/23/04 - Case #1195 - QD_Out.cpp - (v4.4.446) only multiply by 100 after all other operations have completed - TLS
   //Version = "4.5.0co";  // 11/23/04 - MVP_AOI_Comp_Out.cpp - Add three more columns to the component section of the writer - LP
   //Version = "4.5.0cn";  // 11/22/04 - Case #1175 - Gerber Import not handling a complex shape correctly - knv
                           //            Case #1176 - Gerber Import not reading properly - graphics are incorrect - knv
   //Version = "4.5.0cm";  // 11/22/04 - Case #1191 - ViscomAOI_Out.cpp - Output "LP" line for bottom file - LP
   //Version = "4.5.0cl";  // 11/22/04 - Case #1190 - ckt_out.cpp - Corrected the tolerance of CKT file and some output of NDB file - LP
                           //          - Case #1193 - Takaya_o.cpp - Output "*" for resistor's value if it is "0Ohm" and add "@M" at the top of both output file -LP
   //Version = "4.5.0ck";  // 11/22/04 - Case #1194 - ProbePlacement.cpp - Fixed the probe placement report to ensure that atleast a space is between each field - AC
   //Version = "4.5.0cj";  // 11/22/04 - Case #1195 (v4.4.440) - QD_Out.cpp, RwLib.cpp/.h - All rotations are now normalized and then multiplied by 100 prior to being written - TLS
   //Version = "4.5.0ci";  // 11/22/04 - Case #1161 - Multiple issues with CR5000 Reader - knv
   //Version = "4.5.0ch";  // 11/22/04 - Case #1170 - Cadif.in command ".layerattr" not working for typing layers - knv
   //Version = "4.5.0cg";  // 11/19/04 - MainFrm.cpp - Fixed management of visible tool bars when showing the print preview window - AC
   //Version = "4.5.0cf";  // 11/19/04 - Region.cpp - Fixed crash running Access Analysis related to the region clone function - AC
   //Version = "4.5.0ce";  // 11/19/04 - MVP_AOI_Comp_Out.cpp&h - Added modification to MVP AOI Component Writer to output the package into categories per specification - LP
   //Version = "4.5.0cd";  // 11/18/04 - fixture_out.cpp&h - Added support for exporting panels in the fixture output (first cut) - AC
   //Version = "4.5.0cc";  // 11/18/04 - Camcad.vcproj - Just added MVP_AOI_Comp_Out.h, TeradyneSpectrum_Out.cpp&h to the project, no implementation yet - LP 
   //Version = "4.5.0cb";  // 11/17/04 - CAMCAD.cpp - Fixed problem checking for maintenance when running via API's - AC
                           //          - Added initial support for write fixture file from the panel (incomplete)
   //Version = "4.5.0ca";  // 11/16/04 - Case #1149 - Incorrect value for "TEST" attribute when reading Neutral file - LP
   //Version = "4.5.0bz";  // 11/16/04 - Refix Case #696 - Incorrect implementation in checking if two strings are the same - LP
                           //          - Refix Case #1166 - Added comment to indicate that we comment out the code to fix case #1105, 1108, and 1166 - LP
   //Version = "4.5.0by";  // 11/16/04 - Case #1076 - Sch_link.cpp&h - Added some new structures and functions to compare and apply component matches faster (still needs more improvements) - AC
                           //          - CAMCAD.cpp - Fixed copying CAMCAD release to M
                           //          - Modified the About Box to show the version
   //Version = "4.5.0bx";  // 11/15/04 - Case #696 - CamCad.cpp, Settings.cpp/.h - Updated settings read to use CString and added flag to Settings structure to determine whether to write verification log or not - TLS
   //Version = "4.5.0bw";  // 11/15/04 - Case #1164 - Sm_Anal.cpp - Metal and soldermask are not exposed correctly when there are more than one file shown - LP
                           //          - Case #1168 - Mneutin.cpp - CAMCAD crashed due to invalid reference - LP
                           //          - Sch_Lib.cpp - There was a bug in ConvertLogicToPhysicalSchematic() when formating a string - LP
   //Version = "4.5.0bv";  // 11/15/04 - Case #1166 - CamCadNavigator.cpp - Removed the following to fix the crash:  doc -> Clear Sub Select Array(0) - TLS
   //Version = "4.5.0bu";  // 11/11/04 - License.cpp&h, FLEXlm.cpp&h - Added handling of the maintenance feature required to run - AC
   //Version = "4.5.0bt";  // 11/11/04 - Case #1131 - Edif300_in.cpp&h - Added support of multiple display of text - LP 
   //Version = "4.5.0bs";  // 11/11/04 - Men_Out.cpp - Made non visible refnames not visible within Mentor, made correction to refname justification from attrib justification. - DF
   //Version = "4.5.0br";  // 11/11/04 - CiiIn.cpp - Made refnames without labels not visible, made correction to refname offsets after placement is corrected. - DF
   //Version = "4.5.0bq";  // 11/10/04 - License.cpp&h, FLEXlm.cpp&h - Added functionality to get maintenance expiration date (not being used for anything) - AC
   //Version = "4.5.0bp";  // 11/08/04 - Case #1162 - AllegIn.cpp - Allegro Read, duplicate pin inserted in a instance specific geometry - LP
   //Version = "4.5.0bo";  // 11/08/04 - Case #1154 - Changed handling of Capacitor_Tantalum and Capacitor_Polarized similar to that of Capacitor - AC
   //Version = "4.5.0bn";  // 11/08/04 - Case #1136 - Agilent 3070 export - PortLib.cpp - "Include Vias" checkbox not working.  Also fixed Delete3070Attributes checkbox - TLS
   //Version = "4.5.0bm";  // 11/08/04 - Case #1144 - QD_Out.cpp - Added option to suppress unloaded components - TLS
   //Version = "4.5.0bl";  // 11/08/04 - Case #1160 - Accel Export - CamCad crashes when exporting - knv
   //Version = "4.5.0bk";  // 11/08/04 - Case #1158 - ViscomAOI_Out.cpp - Board width and length were output wrong - LP
   //Version = "4.5.0bj";  // 11/08/04 - Case #1153 - Removed non necessary headers from Aeroflex writer - MAN 
   //Version = "4.5.0bi";  // 11/08/04 - Case #1125 - Dfm_flat.cpp - Fixed flattening of fiducials, bug found due to optimiztion marking blocks - AC
   //Version = "4.5.0bh";  // 11/08/04 - Case #1151 - Protel Reader - SMD Attributes are not copied to derived geometries - knv
   //Version = "4.5.0bg";  // 11/08/04 - Case #1159 - QD_Out.cpp - Added option in QD.out to specify rotation on Centroid or Origin.  Also fixed centroid rotation to do offset before rotation. - TLS
   //Version = "4.5.0bf";  // 11/08/04 - Case #1139 - Agilent 3070 export - "Include Unconnected Pins" checkbox not working - knv
                           //            Case #1138 - Agilent 3070 export - "Unconnected Pins in One Net" checkbox not working - knv
   //Version = "4.5.0be";  // 11/05/04 - BomCSV.cpp - Now only modifying/adding LOADED flag if checkbox is checked. - TLS
   //Version = "4.5.0bd";  // 11/05/04 - ProbeStyles.cpp - Modified probe style grid columns to better display the data - AC
                           //          - fixture_out.cpp - Added code to determine the technology of a data being probed
   //Version = "4.5.0bc";  // 11/05/04 - Camcad.rc, resource.h - Changed some caption of controls Variant and AccessAnalysis dialog - LP
                           //          - PcbUtil.cpp&h, CAMCADNavigator.cpp, Variant.cpp - Modification to implementation of Variant - LP
                           //          - Sm_Anal.cpp&.h, RealPart.cpp&h, AccessAnalysisDlg.cpp&h, XmlRealPartContent.cpp - Modified AA to support use of RealPart outline - LP
   //Version = "4.5.0bb";  // 11/05/04 - Version skipped accidentally...
   //Version = "4.5.0ba";  // 11/05/04 - BomCSV.cpp/.h, Lic.cpp, License.cpp - BomCSV: updated licensing, fixed column/listbox edit syncronization - TLS
   //Version = "4.5.0az";  // 11/04/04 - BomCSV.cpp/.h, Resources - Added BomCSV import - TLS
   //Version = "4.5.0ay";  // 11/04/04 - ProbeStyles.cpp - Fixed logic when determining what probe style should be applied - AC
                           //            Lic.cpp - Made the Digital Test Writer visible in release builds
                           //            DigitalTest_o.cpp - Fixed formatting
                           //            File.h, RealPart.cpp&h - Added functionality and structures to handle hiding of realparts depending on the loaded attributes
                           //            CAMCADNavigator.cpp, DataDoctor.cpp - Added calls to hide/show unloaded/loaded realparts
   //Version = "4.5.0ax";  // 11/04/04 - Variant.cpp&h - Tidy up the dialog and modify some functionality - LP
   //Version = "4.5.0aw";  // 11/03/04 - Variant.cpp&h, Camcad.rc, resource.h, CAMCADNavigator.cpp - Modification to Variant Management dialog and Variant functionality - LP 
   //Version = "4.5.0av";  // 11/03/04 - Ccm_Out.CPP - Added a stacknum to pad inserts of a padstack to better define blind/buried padstacks. - DF
                           // 11/03/04 - Ccm_Out.CPP - No stacknum added to non-electrical pads.  - DF
                           // 11/03/04 - Ccm_Out.CPP - Theoretical electrical layers spanning multiple phisical layers are identified with the following stacknum values: - DF
                           // 11/03/04 - Ccm_Out.CPP - SIGNAL INNER, PAD INNER := -1  - DF
                           // 11/03/04 - Ccm_Out.CPP - SIGNAL OUTER, PAD OUTER := -2  - DF
                           // 11/03/04 - Ccm_Out.CPP - SIGNAL ALL, PAD ALL     := -3  - DF
   //Version = "4.5.0au";  // 11/03/04 - AllegOut.CPP - Added support for Allegro v 15.2 special cases in padstack scripts, no default via padstack. - DF
   //Version = "4.5.0at";  // 11/03/04 - AllegOut.CPP - Added support for Allegro versions 15.0 and 15.1 with changes to shape adds an DRC on/off for status dialog. Also fixed error with rotated Square and circle pad shapes where size a and b were swapped then b assigned to the value of a which was now 0. - DF
   //Version = "4.5.0as";  // 11/03/04 - MVP_AOI_Comp_Out.cpp - Updated the writers to use TEST_STRATEGY attribute - LP
   //Version = "4.5.0ar";  // 11/03/04 - Fixed probe styles to correctly save and read blank fields and included probe style logic in the Fixture output - AC
   //Version = "4.5.0aq";  // 11/02/04 - Added new Tools toolbar to run eSight Runner and Scripter, BOM, Exchange, and Realpart - AC
                           //          - Added probe styles to the fixture output
   //Version = "4.5.0ap";  // 11/02/04 - DbUtil.h, Export.h, ckt_out.cpp&h, Takaya_o.cpp&h, Hp3070_o.cpp, ViscomAOI_Out.cpp - Updated the writers to use TEST_STRATEGY attribute - LP
   //Version = "4.5.0ao";  // 11/02/04 - DbUtil.h, Attrib.cpp, keyword.cpp&h - Added an internal keyword ATT_TEST_STRATEGY - LP
   //Version = "4.5.0an";  // 11/01/04 - Case #1107 - HPGL Export - Data inserted on floating layers is not exported - knv
   //Version = "4.5.0am";  // 11/01/04 - Case #1050 - MVP_AOI_PASTE.cpp - Fiducials use GeomName instead of "REF" - DD
   //Version = "4.5.0al";  // 11/01/04 - Case #1082 - IPC import - Units are not correct and are not affected by import dialog scaling - knv
   //Version = "4.5.0ak";  // 11/01/04 - Case #1147 - Takaya_o.cpp - Modified IC Capacitance ouput and "@O" and "@H" - LP
                           //          - Case #1145 - Takaya_o.cpp - The command ".TOP_POPULATED" and ".BOTTOM_POPULATED" will export the top & bottom when set to "Y" and will not when set to "N" - LP    
   //Version = "4.5.0aj";  // 11/01/04 - Case #1143, 1148 - Errors in SPEA out, renaming package to packages, and replacing hyphen with underscore. MAN
   //Version = "4.5.0ai";  // 11/01/04 - Case #1077 - Unicam read - PCB Components inserted at incorrect location - knv
   //Version = "4.5.0ah";  // 11/01/04 - Case #1129 - Dfm_Comp.cpp - Added function to normalize rotations between 0 and 360 for component orientation tests - AC
   //Version = "4.5.0ag";  // 10/29/04 - Xml_Cont.cpp, Xml_Wrt.cpp - Write and Read the default Variant from "CC" file - LP
   //Version = "4.5.0af";  // 10/29/04 - Implemented variant selection and application in PCB Navigator - LP
   //Version = "4.5.0ae";  // 10/29/04 - Camcad.rc, resource.h - Added variant list and apply button to PCB Navigator - LP
   //Version = "4.5.0ad";  // 10/28/04 - ProbePlacment.cpp - Added functionality to add DRCs for Probe Placement - AC
   //Version = "4.5.0ac";  // 10/28/04 - Variant.cpp&h, Camcad.rc, resource.h - Added fix for grid in Variand dialog - LP
   //Version = "4.5.0ab";  // 10/28/04 - AccessAnalysisDlg.cpp, DFT.cpp, ProbePlacement.cpp - Added DRC for Access Analysis - LP
   //Version = "4.5.0aa";  // 10/27/04 - Attrib.cpp&h, Variant.cpp&h - Implemented comparison of variants - LP
   //Version = "4.5.00z";  // 10/26/04 - Camcad.rc, resource.h, Variant.cpp&h, Entity.cpp - Modification to Variant dialog and implementation - LP
   //Version = "4.5.00y";  // 10/26/04 - Camcad.rc, resoucre.h - Modified Cross Reference Editor dialog in Schematic Link - LP
   //Version = "4.5.00x";  // 10/25/04 - Camcad.rc, resoucre.h, Variant.cpp&h - Modified Variant Navigator dialog - LP
   //Version = "4.5.00w";  // 10/21/04 - Camcad.rc, resoucre.h - Modified Takaya dialog and Probe Placement dialog - LP
   //Version = "4.5.00v";  // 10/18/04 - Removed global variable Version - knv
   //Version = "4.5.00u";  // 10/08/04 - Camcad.rc & resource.h - Removed target type "Testpoint" - LP
   //Version = "4.5.00t";  // 10/07/04 - AccessAnalysisDlg.cpp&h, Camcad.rc, resource.h - LP
                           //             - Add button to "Clear Visible" and "Clear Selected" for Package Outline
                           //             - Chaned label of "Use DFT Outline" to "Use Package Outline" and "Comp. Outline" to "Package Outline"
                           //             - Add messge box to indicate test plan is done loading and enable "Process" button
                           //             - Fixed "Browse" button to point to the same path as stated in the label
                           //             - Add button to "Set As Default" and "Restore Default" for test plan
                           //             - Add new target type "Testpoint" but not yet implement
   //Version = "4.5.00s";  // 10/04/04 - Camcad.rc, resource.h - Change caption of "Feature Size" to "Minimum Feature Size" in Access Analysis dialog - LP
   //Version = "4.5.00r";  // 10/01/04 - ProbeStyles.cpp&h, Fixture_out.cpp&h - First cut of probe style implemented in the fixture file - AC
                           //          - Camcad.rc - Added missing field in Gerber Educator dialog
   //Version = "4.5.00q";  // 09/23/04 - Case #1028 - CAMCAD.rc - Added TestPoint insert type button to the 2nd custom toolbar - AC
   //Version = "4.5.00p";  // 09/23/04 - Camcad.rc, resource.h - Change the captions of combo boxes in the export dialog of Takaya from "fiduial" to "fiducial" - LP
   //Version = "4.5.00o";  // 09/22/04 - Camcad.vcproj - Moved the file EifIn.cpp/.h into the correct folder under the Reader folder in the Solution Explorer.  Also renamed ViscomAOI_In.cpp to ViscomAOI_Out.cpp and moved under the Writers folder - LP
   //Version = "4.5.00n";  // 09/20/04 - Rebug Case #585 - Settings.cpp&H & Draw.cpp - Added new settings to control drawing of overrides when things are not visible (b/c layer, IT, ... ) - AC
   //Version = "4.5.00m";  // 09/17/04 - Added 2nd fiducial listbox and cancel button to Takaya Export dialog - LP
   //Version = "4.5.00l";  // 09/16/04 - Added saving functionality to the CProbeStyleList - AC
   //Version = "4.5.00k";  // 09/15/04 - Modified Takaya output dialog - LP
   //Version = "4.5.00j";  // 09/10/04 - Additional probe style changes - AC
   //Version = "4.5.00i";  // 09/10/04 - Added Takaya inhancement, see Version 4.4.200 for detail - LP
   //Version = "4.5.00h";  // 09/09/04 - Added Probe Styles Dialog (only for debugging) - AC
   //Version = "4.5.00g";  // 09/06/04 - Fixed case (enhancement 979) changes to HP3070 dialog - MAN
   //Version = "4.5.00f";  // 09/03/04 - Added PanelLib.cpp & .h from 4.4 - AC
                           //          - Added missing resource information from 4.4 dealing with the HP3070 output
   //Version = "4.5.00e";  // 08/25/04 - Added a checkbox for the HP5DX dialog which was added in version 4.4.x - LP
                           //          - Added more implementation for Variant support - LP
   //Version = "4.5.00d";  // 08/19/04 - Implemented saving of variant - LP
   //Version = "4.5.00c";  // 08/17/04 - Implemented Variant Navigation dialog to view different variants from list - LP
   //Version = "4.5.00b";  // 08/16/04 - Added RTI_Out.cpp/.h into the project and keep them share between 4.4 and 4.5 - LP
   //Version = "4.5.00a";  // 08/03/04 - Branched 4.5 -  LP
   ////////////////////////////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////////////////////////////
   //Version = "4.4.130";  // 08/13/04 - FlexGridLib.cpp/.h and many files - Moved CDfmFlexGrid from GerberThermalDfm.ccp/.h and CFlexGridStatic from DataDoctorDialog.ccp/.h to FlexGridLib.ccp/.h                           
   //Version = "4.4.129";  // 08/12/04 - ScorpionOut.cpp/.h - Added Scorpion first cut (for debug only) - AC
                           //          - File.cpp/.h - Added LookUpAttrib and SetAttrib on CompPinStruct
   //Version = "4.4.128";  // 08/12/04 - Added Variant Navigator Dialog and menu to access the dialog under TOOL - LP
   //Version = "4.4.127";  // 08/12/04 - File.h, Xml_Wrt.cpp, Xml_Cont.cpp/.h - Only added Variant into file and the reading and writing of CC file - LP
                           //          - Variant.cpp/.h - Add CVariantItem, CVariant, CVariantList - LP
   //Version = "4.4.126";  // 08/11/04 - Comment out ignored merged status lines in takaya - SN
   //Version = "4.4.125";  // 08/10/04 - DbUtil.h - Added enum for page unit EPageUnit - LP
                           //          - Sch_Link.cpp - Fixed a schematic "cross probe net" to look for graphic class signal - LP
                           //          - Settings.cpp/.h - Added a const m_dDistanceTolerance and set to one micron, use to check as tolerance for check distance - LP
                           //          - Case #944 - PadsIn.cpp - Incorrect usage of distance tolerance of SMALL_NUMBER, changed to use getDistanceTolerance(), which cause some comppin and via not to have the TEST attribute - LP
                           //          - Case #948 - CamCadDatabase.cpp - Use of invalid variable reference - LP
   //Version = "4.4.124";  // 08/10/04 - Added Scorpion writer (only for debug) - AC
   //Version = "4.4.123";  // 08/09/04 - Case #929 - Parallel Analysis in Takaya - SN
   //Version = "4.4.122";  // 08/06/04 - Stencil Generator enhancement - beta #4 - knv
   //Version = "4.4.121";  // 08/05/04 - Case #942 - Vb99In.cpp - Derived device type is not inheriting attributes - LP
   //Version = "4.4.120";  // 08/05/04 - Refix #896 - PdifIn.cpp - Change error message to be nicer when we don't import non-supported file - LP
                           //          - Case #936 - Mneutin.cpp - CAMCAD crash trying to reference a panel that is not valid - LP
                           //          - Case #939 - AccessAnalysisDlg.cpp/.h, DFT.h - LP
                           //             - checkbox of "Include Single Pin Component" on Package Outline tab is implemented
                           //             - if comppin location is not generate ask if users want to, if they choose "NO" then exist AA
                           //          - MainFrm.cpp/.h - Mapped the event OnDropFiles() to our CMainFrame so we can handle it internally - LP
   //Version = "4.4.119";  // 08/04/04 - Stencil Generator enhancement - beta #3 - knv
   //Version = "4.4.118";  // 08/04/04 - Added support for void in font correctly, although bug was found in the handling of voids altogether - AC
   //Version = "4.4.117";  // 08/04/04 - Refix #896 - PdifIn.cpp - Do not create file and layer if PCAD file is not read due to non-supported version - LP
                           //          - Case #934 - Incorrect exposed metal after boardoutline check is done - Lp
   //Version = "4.4.116";  // 08/03/04 - Case #896 - PdifIn.cpp - Do not import PCAD file that is not generate from PDIF-OUT v 8.x or earlier - LP
   //Version = "4.4.115";  // 08/03/04 - Added .cpp/.h and everything for new writer Orbotech Write - LP
   //Version = "4.4.114";  // 08/03/04 - Edif200_in.cpp - Fixed the followings - LP
                           //             - Case #930 - CAMCAD crashed due to invalid reference
                           //             - Added recognition of bus through array name of "name[lowerbound:upperbound]" and "name(lowerbound:upperbound)"
   //Version = "4.4.113";  // 08/02/04 - Added support for drawing fonts taking into account pen widths - AC
   //Version = "4.4.112";  // 07/30/04 - Case #806 - ThedaIn.cpp - Fixed placing probes on the correct surface from the TEST_POINTS section - AC
                           //          - Case #832 - GerOut.cpp - Fixed writing circles in complex apertures when in any current unit, needed conversion factor
                           //          - Draw_Ent.cpp - Fixed drawing of fonts using filled and void properties
   //Version = "4.4.111";  // 07/30/04 - Case #923 - Sm_Anal.cpp - Allow expose for poly of type not ETCH if poly is inside padshape - LP
   //Version = "4.4.110";  // 07/29/04 - Case #891 - MentorIn.h - Need to re-assign the geometry in COMPS file when the same comp is already inserted from Geoms_Ascii file - LP
   //Version = "4.4.109";  // 07/29/03 - Case #908 - Mneutin.cpp - Via is being typed as testpad - LP
                           //          - Refix Case #778 - Remove parenthesis from aperture name - LP
   //Version = "4.4.108";  // 07/29/03 - Case #778 - AccelIn.cpp - Resolved duplicate aperture by create aperture from shape, hight, and width - LP
                           //          - Case #782 - AccelIn.cpp - Padstack name prefixed with "P:" - LP
                           //          - Case #928 - Vb99In.cpp - Incorrect netname when component is inserted mirror - LP
                           //          - Sm_Anal.cpp - modified the function SM_ExplodeBlock() to explode ETCh on padshape - Lp
                           //          - Block.cpp, CcDoc.h, DbUtil.cpp/.h, Graph.cpp/.h - Added parameter of BlockTypeTag to several functions - LP
   //Version = "4.4.107";  // 07/27/04 - Data.cpp - Fixed getTextBox function that caused a miscalculation of the text extents - AC
                           //          - Search.cpp - Fixed search to correctly handle line and text alignment
   //Version = "4.4.106";  // 07/27/04 - Added new toolbar icon to toggle on/off Signal and Bus graphic class - LP
   //Version = "4.4.105";  // 07/27/04 - Edif200_in.cpp, Edif300_in.cpp - Changed graphic class from ETCH to SIGNAL for schematic net - Lp
                           //          - Sch_List.cpp/.h, Select.cpp - Added Parent Bus of busses and net when clicking a net or bus in navigator or in the viewer  - LP
                           //          - Camcad.cpp, Camcad.rc, resource.h, Sch_Link.cpp/.h - Added setting for bus highlighting color - LP
   //Version = "4.4.104";  // 07/26/04 - Unicam_o.cpp & UnicamIn.cpp - Fixed writing and reading of angles on board instances in a panel - AC
   //Version = "4.4.103";  // 07/23/04 - Case #909 - GenCadIn.cpp - Fixed scaling when reading in circle - AC
                           //          - Case #912 & 916 - GenCadIn.cpp - Fixed reading test point, building the placement correctly w/ links
                           //          - Case #918 - GenCad_o.cpp - fixed sorting of holes and pins in components, putting holes at the end
   //Version = "4.4.102";  // 07/23/04 - Case #915 - Gerber Educator crashed due to invalid variable declaration - LP
   //Version = "4.4.101";  // 07/23/04 - Case #905 - MentorIn.cpp - The command ".EXPLODE_POWER_SIGNAL_LAYER_SHAPE" need to work even when there is no layer file - LP
   //Version = "4.4.100";  // 07/23/04 - Case #903 - Dfm_Thru.cpp - First ViaPadSize DFM test to also take into account pads on signal layers - AC
                           //          - UnicamIn.cpp/.h, Unicam_o.cpp - Add support for reading and writing of panels
                           //          - Data.cpp - Fixed mirrorring of inserts via "transform" to no affect layer mirrorring
   //Version = "4.4.099";  // 07/22/04 - Case #742 - CAMCAD is not able to load Veribest EIF file when drag and drop because current working directory is not set - LP
   //Version = "4.4.098";  // 07/22/04 - Bus.cpp/.h, Entity.cpp/.h - Implement CBusStruct, CBusList, and CEntityWithAttributes structure - LP
                           //          - Edif300_in.cpp/.h, Attrib.cpp/.h - Add support for busses - LP
                           //          - Xml_Cont.cpp/.h, Xml_Wrt.cpp - Write and read the new "Busses" section in CC file - LP
   //Version = "4.4.097";  // 07/21/04 - Stencil Generator enhancement - beta #2 - knv
   //Version = "4.4.096";  // 07/21/04 - Case #910 - Panelization.cpp - Fixed crash for when no board outline exists - AC
   //Version = "4.4.095";  // 07/20/04 - Stencil Generator enhancement - beta #1 - knv
   //Version = "4.4.094";  // 07/20/04 - AccessAnalysisDlg.cpp, DFT.h - Changed the format of AccessReport.txt - LP
   //Version = "4.4.093";  // 07/20/04 - Case #906 - AccessAnalysisDlg.cpp, Sm_Anal.h/.cpp - CAMCAD crashed due to reference to invalid DataStruct - LP 
   //Version = "4.4.092";  // 07/19/04 - Case #443 - Unidat.cpp & h - Added new settings switch .ComponentPinCoordinates to indicate if Comp/pin coordinates are absolute or relative - AC
   //Version = "4.4.091";  // 07/16/04 - Block.cpp - Fixed unit recalculation when the units setting is changed - AC
                           //          - UnidatIn.cpp - Fixed interpretation of the radius of a circle
   //Version = "4.4.090";  // 07/16/04 - Case #902 - Block.cpp - IsDesignedOnTop() function will look at attribute "ATT_TOP_DESIGNED_SURFACE" when geometry is a padstack - LP
                           //          - AccessAnalysisDlg.cpp - Access Analysis, testpad and testpoint is recognized in the way as via and set target type as via - LP
   //Version = "4.4.089";  // 07/16/04 - UnidatIn.cpp & h - Added support to read the TEST_POINT section - AC
   //Version = "4.4.088";  // 07/15/04 - AccessAnalysisDlg.cpp - Even if calculateLocationAndMaxSize() return okay still need to check exposed metal diameter against featureSize - LP
   //Version = "4.4.087";  // 07/15/04 - Case #619 - PfWin.cpp - Fixed processing of filled polies with rotations (needed to rotation them about it's center) - AC
   //Version = "4.4.086";  // 07/15/04 - Case #897 - GenCad_o.cpp - Added new gen_comp_to_shape command in the GenCad.out file to deal with Generic components in PCB components - AC
   //Version = "4.4.085";  // 07/15/04 - Case #892 - Settings.cpp - Fixed issue when units are changed.  Make sure the page size if modified correctly base on the choosen unit - AC
   //Version = "4.4.084";  // 07/15/04 - Case #404 - Sch-Lib.cpp - Do not clone hierarchy sheet if it is only used once -  LP
                           //          - Edif200_in.cpp, Edif300_in.cpp - call the function CheckMissingOrDuplicateDesignator() at the end of import - LP
                           //          - Sch_List.cpp - Fixed a bug in sheet tree - LP
   //Version = "4.4.083";  // 07/14/04 - Case #843 - AllegIn.cpp - need to fix the way Allegro reads test probes...create and link probe to test access and ta to feature - AC
                           //          - Case #895 - Print.cpp - fix bug hiding toolbars and menus when printing
                           //          - Case #892 - Search.cpp - fixed searching of visible attributes
   //Version = "4.4.082";  // 07/14/04 - Case #866 -  Protel not reading V5 files - SN
   //Version = "4.4.081";  // 07/14/04 - Case #875 -  Allegro In sets components and geometries as fiducials - SN
   //Version = "4.4.080";  // 07/13/04 - UnidatIn.cpp - Added support to interpret layer with _CS suffixes as top layers - AC
   //Version = "4.4.079";  // 07/13/04 - Port.cpp - Fixed import API to flag CAMCAD not to show dialogs (before trying to ID the files) - AC
   //Version = "4.4.078";  // 07/13/04 - MVP AOI Writers Released - DD
   //Version = "4.4.077";  // 07/13/04 - UnidatIn.cpp - Typed drill layer, fixed bottom side component rotations, and fixed reading of netlist info - AC
   //Version = "4.4.076";  // 07/13/04 - PcbUtil.cpp - OnGenerateSmdComponent() will also update comppin in netlist - LP
                           //          - Project.cpp, Port.cpp, Open_Cad.cpp - OnGenerateSmdComponent() will be called after the following functions:
                           //             1) CCEtoODBApp::OpenCadFile()
                           //             2) OpenCadFile_API()
                           //             3) CCEtoODBDoc::OnFileImport()
                           //             4) API::Import()
                           //             5) CCEtoODBDoc::LoadProjectFile()
   //Version = "4.4.075";  // 07/12/04 - Case #692 - UnidatIn.cpp - Fixed pin rotation calculate to properly take into account component rotations - AC
   //Version = "4.4.074";  // 07/12/04 - Gencad In not reading cutout circle - SN
   //Version = "4.4.073";  // 07/09/04 - Case #818 - Hp3070_o.cpp - Fixed 3070 writer to use the package outline first if it is available - AC
                           //          - Unidat.cpp - Added processing of layer for etchs, typing of signal layers
   //Version = "4.4.072";  // 07/09/04 - Case #805 - Edit.cpp - After Access Analysis, strange error dialog appears when querying an exposed metal - LP
   //Version = "4.4.071";  // 07/09/04 - Case #853 - CadifIn.cpp - Fixed run-time crash for the 4.4.069 enhancement. - DF
   //Version = "4.4.070";  // 07/09/04 - Case #734 - Camcad.rc, resourc.h - Rename "Generate SMD Component" to "Generate TECHNOLOGY Attribute" - LP
                           //          - Case #817 - PcbUtil.cpp - "Generate TECHNOLOGY Attribute" should apply the attribute to pin level also - LP
                           //          - Case #794 - Gencad Write is not writing "cutout" section - SN
   //Version = "4.4.069";  // 07/09/04 - Case #853 - CadifIn.cpp - Enhanced the .FIDUCIALATTR command to accept wildcard (*) and apply to component geometries as well as padstacks. - DF
   //Version = "4.4.068";  // 07/09/04 - Case #854 - Report.cpp - Added the outpu of the following in the Manufacturing report - LP
                           //             1) Number of Electrical Layers
                           //             2) Smallest Trace Width
                           //             3) Smallest Via Diameter
                           //             4) Smallest Pin Pitch
   //Version = "4.4.067";  // 07/09/04 - Case #890 - CR3000In.cpp - CR3000 hangs due to incorrect use of empty string - LP
   //Version = "4.4.066";  // 07/09/04 - Case #874 - AllegOut.cpp - Cadence Allegro write, not output padstack scripts (*.psr) when version is set to 15.0 - LP
   //Version = "4.4.065";  // 07/08/04 - Added the recalculation modification when the working unit is changed in the settings (first cut) - AC
                           //          - Unidat.cpp - Reading in vias correctly when only on one surface and set etch layers as generic signals
   //Version = "4.4.064";  // 07/08/04 - Case #723 - AccelIn.cpp/.h - Geometry is incorrectly created under PatternDef & PatternDefExtended - LP
   //Version = "4.4.063";  // 07/07/04 - Case #869 - REBUG:Extra net appears and disappears - SN
   //Version = "4.4.062";  // 07/07/04 - Case #519 - Modified the calculation of visible attribute rotations and locations on components - AC
   //Version = "4.4.061";  // 07/06/04 - Case #797 - Double Clicking on a CC or CCZ file in Windows Explorer launches CAMCAD but doesn't LOAD THE FILE! - knv
   //Version = "4.4.060";  // 07/02/04 - Case #868 - Fixed crash with print preview, we now hide all the menus as per other main stream applications - AC
                           //          - Case #872 - CCRepair.cpp - Fixed crash when repairing inserts without existing blocks on layer -1
   //Version = "4.4.059";  // 07/02/04 - Case #877 - Spea_Out.cpp - Spea Write, in NETLIST section, surface should be DFT access surface if DFT is done - LP
                           //          - Case #878 - Spea_Out.cpp - Spea Write, in PACKAGE section, added ".in" command ".PACKAGE_ATTRIBUTE" to allow rename of package - LP
   //Version = "4.4.058";  // 07/02/04 - Case #869 - Extra net appears and disappears - SN
   //Version = "4.4.057";  // 07/02/04 - Case #857 - QD_Out.cpp - Siemen QD Write, add ".in" command ".ROTATION_OFFSET" - LP
                           //          - Case #862 - PcbUtil.cpp - Add check for valid drill size to determine the TECHNOLOGY of padstack - LP
   //Version = "4.4.056";  // 07/01/04 - Case #861 - AC & LP
                           //             - DESIGN SURFACE attribute generation - AC
                           //             - Access Analysis generate incorrect result due to VectorizePoly and CPntList::updateExtent function - LP
   //Version = "4.4.055";  // 07/01/04 - Case #871 - Multiple documents cause problems in Data Doctor - knv
   //Version = "4.4.054";  // 07/01/04 - Aeroflex_Out.cpp - LP
                           //             1) Corrected checking of TEST attribute to determine accessible surface
                           //             2) Corrected VALUE for TRANSISTOR_NPN and TRANSISTOR_PNP
                           //             4) Corrected getting surface for TESTSIDE
                           //             5) Added and implemented the command ".EXPORT_UNLOADED"
   //Version = "4.4.053";  // 06/30/04 - Aeroflex_Out.cpp - Correctly output multiplier for VALUE attribute - LP
                           //          - CompValues.cpp/.h - Added the function GetSmallestWholeNumber() - LP
   //Version = "4.4.052";  // 06/30/04 - DeviceTyper.cpp - items in pin mapping tree stay after being mapped by file - SN
   //Version = "4.4.051";  // 06/30/04 - Aeroflex_Out.cpp - LP
                           //             1) Correctly output "TESTSIDE" and "ALLOC"
                           //             2) If comppin doesn't have TEST attribute, then check component for the attribute
   //Version = "4.4.050";  // 06/30/04 - Aeroflex_Out.cpp - LP
                           //             1) Fixed output of ARC value
                           //             2) Don't output Graphics when ".WRITE_GRAPHICS" is set to N
                           //             3) Check device pin map without case sensitive 
                           //          - DataDoctor.cpp - Fixed an error in check the comparison of ATT_VALUE_NO_CONNECT - LP
                           //          - Spea_Out.cpp - Modified to check device pin map without case sensitive - Lp
   //Version = "4.4.049";  // 06/29/04 - Aeroflex_Out,cpp - LP
                           //             1) Under the DEFINE section rename geometry name to PIC#, where # is a sequencial number
                           //             2) For each geometry, create a mirrored version of the component outpine under DEFINE and correct reference it in the COMPONENT section
                           //          - DbUtil.h, Spea_Out.cpp, ckt_out.cpp - use the constant ATT_VALUE_NO_CONNECT instead of "no connect"
   //Version = "4.4.048";  // 06/28/04 - Aeroflex_Out.cpp - LP
                           //             - When output "TYPE" for components, check for ATT_AEROFLEX_SUBCLASS first
                           //             - Implemented command ".WRITE_GRAPHICS"
   //Version = "4.4.047";  // 06/28/04 - Aeroflex_Out.cpp - Added loading of "aeroflex.out" and the following commands - LP
                           //             1) ".WRITE_GRAPHICS" - added and implemented
                           //             2) ".WRITE_TRACES" - added but not implemented
                           //             3) ".DFT_SOURCE" - added and implemented
                           //             4) ".OUTPUT_UNITS" - added and implemented 
                           //          - Units.cpp, DbUtil.h - added a new unit UNIT_MICRONS - LP
   //Version = "4.4.046";  // 06/28/04 - Case #828 - Data Doctor not checking pin names correctly - knv
   //Version = "4.4.045";  // 06/28/04 - Case #809 - Devie Typer "Add to setup file" fixed - SN
   //Version = "4.4.044";  // 06/28/04 - Fixed GetProbeData API crash when a net with no comp/pin (connections) exist - AC
   //Version = "4.4.043";  // 06/25/04 - Takaya Updates - SN
   //Version = "4.4.042";  // 06/25/04 - Crash fixed - SN
   //Version = "4.4.041";  // 06/24/04 - Case #839 - Not zooming to correct location - SN
   //Version = "4.4.040";  // 06/24/04 - Case #860 - Fixed crashing when writing CC file b/c of the OriginalBlock pointer - AC
   //Version = "4.4.039";  // 06/23/04 - Case #826 - Centroid rotations - SN
   //Version = "4.4.038";  // 06/23/04 - Case #856 - Default Navigator Colors - SN
   //Version = "4.4.037";  // 06/23/04 - Enhanced Takaya Output - SN
   //Version = "4.4.036";  // 06/21/04 - Added probe size field to GetProbeData - AC
   //Version = "4.4.035";  // 06/21/04 - Access Analysis modification - LP
                           //             - convert default to page units
                           //             - values in test plan are in page units
                           //             - page unit is written to external test plan and values of Access Analysis section are in page units
   //Version = "4.4.034";  // 06/21/04 - CAMCADNavigator.cpp - Added nets with & without access modes - SN
   //Version = "4.4.033";  // 06/21/04 - Api_get.cpp - Added new GetProbeData API - AC
   //Version = "4.4.032";  // 06/21/04 - Spea_Out.cpp - Modified the ".EXPORT_VIAS", when set to "Y" output every via, when set to "N" only output via with test access point - LP
   //Version = "4.4.031";  // 06/18/04 - Case #846 - Sm_Anal.cpp - Join unjoin board outline when creating board outline distance check - LP
   //Version = "4.4.030";  // 06/18/04 - Case #852 - CadifIn.cpp - Added IGNORE_COMPPINS option to suppress creation of instance specific geometries when set to Y - DF
   //Version = "4.4.029";  // 06/17/04 - Case #848 - GenerateCentroidAndOutline.cpp & h - Fixed closing the dialog via the "X" to reset back to the board - AC
                           //          - ProbePlacment.cpp, DFT.cpp, & DbUtil.h - Added saving of net conditions on the net (NoProbe, TR_Req, PIR_Req)
                           //          - DFT.cpp - Fixed "=" operator on the testplan to assign m_bCreateNCNet member correctly
   //Version = "4.4.028";  // 06/17/04 - Case #840 (Part2) - AccessAnalysisDlg.cpp - Use the Minimum Thru-hole Drill Size from settings to determine if some feature is throught hole or not - LP
   //                      //          - PolyLib.cpp - Fixed a bug in VectorizePoly() due to the variable "resolution" not initialized - LP
   //Version = "4.4.027";  // 06/16/04 - Case #840 (Part1) - Settings.cpp & h - Added support for reading and writing of Minimum Thru-hole Drill Size - AC
   //Version = "4.4.026";  // 06/16/04 - Case #834 - GenCad_o.cpp - Output "PARTNUMBER" as "PART" in $DEVICES section. If "PARTNUMBER" is not on device type, then get it from the first component that use the device - LP
   //Version = "4.4.025";  // 06/16/04 - Case #812 - Panalization.cpp & h - Removed sorting for the combo box - AC
   //Version = "4.4.024";  // 06/15/04 - Case #831 - Poly to flash not converting some shapes - knv
                           //          - Case #830 - Stencil Generator crashes - knv
   //Version = "4.4.023";  // 06/15/04 - Case #835 - AccessAnalysisDlg.cpp - Incorrect analysis due to tolerance is setted to the wrong unit - LP
                           //          - Case #836 - AccessAnalysisDlg.cpp - Testpoint is not considered in analysis - LP
   //Version = "4.4.022";  // 06/15/04 - Case #812 - Panalization.cpp & h - Added new option of creating single board panels - AC
                           //          - Case #824 - Pads_Out.cpp - Fixed crash exporting PADS using GetNextIndex in the CBooleanArray
                           //          - Case #829 - Dfm_Flat.cpp - Fixed flattening by insert type when only wanting to use the insert point
   //Version = "4.4.021";  // 06/15/04 - Case #764 - 3070 Write - Incorrect surface written for test point - knv
   //Version = "4.4.020";  // 06/14/04 - Case #743 - AllegIn.cpp - Allegro Import, command .TOOLPART" in "allegro.in" file " is not working - LP
                           //          - Aeroflex_Out.cpp - Removed "WIDTH" from ENIVORMENT section and after 0 infront of decimal for value - LP
   //Version = "4.4.019";  // 06/14/04 - Case #777 - Sm_Anal.cpp - DFT Analysis need to take into consideration of layers being LY_NEVERMIRROR or LY_MIRRORONLY when do exposed metal analysis - LP
   //Version = "4.4.018";  // 06/11/04 - Case #801 - Accuracy tolerance was too small for checking if a point is inside a polylist, so change it to .001 MIL tolerance - LP
   //Version = "4.4.017";  // 06/11/04 - Case #771 - Centroid.cpp & h - Fixed GenerateCentroid algorithm to dig into inserted block for all component outlines - AC
                           //          - Case #799 (Part1) - Spea_Out.cpp - Modified Spea Writer to calculate centroid locations of each component relative to the board
                           //          - Case #799 (Part2) - PcbUtil.cpp - Modified GenerateSMD function to place the TECHNOLOGY flag on a component block when it's a thru-hole
   //Version = "4.4.016";  // 06/11/04 - Case #823 - Normalize Component Values function not correctly changing "4k7" value - knv
   //Version = "4.4.015";  // 06/11/04 - Aeroflex.cpp/.h - Added support of device type and pin function mapping - LP
   //Version = "4.4.014";  // 06/10/04 - Vb99In.cpp - Case #519 - Fixed interpretation of text location for refnames - AC
                           //          - Case #819 - Hp3070_o.cpp - Modified 3070 file to export at a greater precision when in MM
   //Version = "4.4.013";  // 06/10/04 - Aeroflex.cpp/.h - Added mapping of CAMCAD device type to output device type - LP
   //Version = "4.4.012";  // 06/10/04 - Aeroflex_Out.cpp/.h - Added TRACK and checking of DFT for ALLOC - LP
   //Version = "4.4.011";  // 06/10/04 - Case# 827 - AllegIn.cpp - Fixed error thad disallowed a 1-up panel. - DF
   //Version = "4.4.010";  // 06/09/04 - Aeroflex_Out.cpp/,h - Added the COMPONENTS and CONNECTION section - LP
   //Version = "4.4.009";  // 06/09/04 - Case #803 - Mentor neutral import creates two $centroid geometries - knv
   //Version = "4.4.008";  // 06/08/04 - ProbePlacment.cpp & AccessAnalysis.cpp - Updated Access Analysis and Probe Placement to check for the DFT license - AC
                           //          - NailAssignIn.cpp - Added handling of '#' preceding channel numbers w/o spaces
   //Version = "4.4.007";  // 06/08/04 - Aeroflex_Out.cpp/.h - Added the new writer "Aeroflex .CB Write", only finished the "ENVIRONMENT" section - LP
   //Version = "4.4.006";  // 06/07/04 - Case #807 - DeviceTyped.cpp - Color not updated in Components tab - SN
   //Version = "4.4.005";  // 06/07/04 - Case #808 - CAMCADNavigator.cpp - Pan mode in Navigator causes double redraw - SN
   //Version = "4.4.004";  // 06/07/04 - Lic.cpp & h - Added new Aeroflex CB Writer license - AC
   //Version = "4.4.003";  // 06/04/04 - Zoom&Pan to pins in other units incorrect - SN
   //Version = "4.4.002";  // 06/04/04 - Highlight/Zoom/Pan probe when selected from the net list - SN
   //Version = "4.4.001";  // 06/01/04 - Offical Release Build of CAMCAD 4.4.001 - knv
   //Version = "4.4.0iw";  // 05/28/04 - Case #548 - PfWin.cpp & h - Fixed problem when a new block needed to be defined using the same shape (instead of just moving pins (CP) we track segments (CT) also) - AC
                           //          - Added back removed indecies for the lic array, caused indexing error on ListConstants
                           //          - Fixed crash writing the NDB file in the CKT writer
   //Version = "4.4.0iv";  // 05/28/04 - Case765- Spea4040_subclass attribute added to sea output - SN
   //Version = "4.4.0iu";  // 05/28/04 - Added support of chr(13) in parser of the special foramt command for Gerber Write - LP
   //Version = "4.4.0it";  // 05/27/04 - Port_Lib.cpp - Added Support for Special format command for the Gerber Writer - LP
                           //          - Fixed crash when an Aperture of no size exists in the file - AC
   //Version = "4.4.0is";  // 05/27/04 - CAMCADNavigator.cpp - Fixed crash when trying to sort net names - AC
                           //          - Added new API ExportFormat
   //Version = "4.4.0ir";  // 05/27/04 - Case #792 - Edif300_in.cpp - CAMCAD hang during import of Edif 300 because it connect find the sheet at "occurrenceHierarchyAnnotate" - LP
   //Version = "4.4.0iq";  // 05/27/04 - Case #641 - Edif200_in.cpp - If a hierarchy symbol is tied to a hierarchy sheet that has a logic symbol, and the logic symbol is the same as the hierarchy symbol,
                           //                        then change the hierarchy symbol to a regular logic symbol because the hierarchy sheet is a bad one - LP
                           //          - Sch_List.cpp/.h - Added hierachy symbol to Instance tree - LP
   //Version = "4.4.0ip";  // 05/26/04 - Case #619 - PfWin.cpp - Fixed protel reader to understand filled polies in components - AC
                           //          - Case #786 - Lic.cpp - Removed PCAD Schematic, Protel Schemetic, and Viewlogic readers
                           //          - Case #722 - GerOut.cpp - Fixed Gerber writer to handle aperture offsets and rotations correctly
                           //          - Apertur2.cpp - Reverted aperture offset and rotation handling correctly
   //Version = "4.4.0io";  // 05/25/04 - Case #657 - DeviceTyper.cpp - Validate the components in Pin tab before clicking on them - SN
   //Version = "4.4.0in";  // 05/25/04 - Case #789 - Edif200_In.cpp - Edif 200 Read, Not recognize hierarchy sheet as top level design sheet - LP
   //Version = "4.4.0im";  // 05/25/04 - Case #760 - Ta_Short.cpp/.h - Added menu option to access Probable Shorts Analysis and moving the function to Ta_Short.cpp/.h - LP
   //Version = "4.4.0il";  // 05/24/04 - Search.cpp - Found and fixed residual problem with the selection algorithm - AC
                           //          - Ckt_Out.cpp & h - Fixed the CKT write with various minor issues
   //Version = "4.4.0ik";  // 05/24/04 - Case #788 - Fixed problem with selecting components when pins are selected, and polies - AC
   //Version = "4.4.0ij";  // 05/24/04 - Case #657 - Device typer pin mapping does not flag components correctly - SN
   //Version = "4.4.0ii";  // 05/21/04 - Added menu for "Probable Short Analysis", but not complete, yet - LP
                           // 05/21/04 - Case #763 - Camcad.cpp & MainFrm.cpp - Fixed help to open the chm file - AC
                           //          - Apertur2.cpp - Fixed ApertureToPoly_Base to take into account offset and rotation correctly
   //Version = "4.4.0ih";  // 05/21/04 - Case #765 - Spea_out, Ipl_out, Hp3070: Support multiple subclass for different exporters - SN
   //Version = "4.4.0ig";  // 05/21/04 - Case #762 - Zooming to pins in bottom view in incorrect - SN
   //Version = "4.4.0if";  // 05/21/04 - Case #766 - fixture_out.h/.cpp, CAMCADNavigator.cpp, Add user defined probe message attrib to edit probe dlg and output it infixture - SN
   //Version = "4.4.0ie";  // 05/21/04 - Case #780 - Edif300_in.pp - Edif 300 Read, Not recognize hierarchy sheet as top level design sheet - LP
   //Version = "4.4.0id";  // 05/20/04 - Case #761 - Remove Entry from Arrange Toolbars dialog - SN
   //Version = "4.4.0ic";  // 05/20/04 - Case #580 - Data Doctor - Pins - knv
   //Version = "4.4.0ib";  // 05/20/04 - Case #662 - xml_cont.cpp & h - Fixed to repair CC files with duplicate keywords - AC
   //Version = "4.4.0ia";  // 05/20/04 - Case #558 - Edif300_in.cpp/.h - Edif 300 Read does not recognize "occurrenceAnnotate" - LP 
   //Version = "4.4.0hz";  // 05/20/04 - ckt_out.cpp - Fixed writing of Adaptor section to only write test resource probes - AC
   //Version = "4.4.0hy";  // 05/20/04 - Case #711 - NailAssignIn.cpp & h - Updated the NAR log file to distigush whether placed or unplaced probes were removed - AC
                           //            fixture_out.cpp & h - correctly output the tester interface number when present
   //Version = "4.4.0hx";  // 05/20/04 - Case #767 - Change the minimum drill size from 0.013 to 0.011 and created a define for it, DFT_MIN_DRILL_SIZE - LP
   //Version = "4.4.0hw";  // 05/19/04 - Case #580 - Data Doctor - Pins - knv
   //Version = "4.4.0hv";  // 05/19/04 - Case #711 - Updated gather of probeable nets, the CKT writer and support of the NAR file - AC
   //Version = "4.4.0hu";  // 05/19/04 - Case #580 - Data Doctor - Pins - knv
                           //            Case #620 - Stencil Generator - stencil holes have incorrect rotation
   //Version = "4.4.0ht";  // 05/19/04 - Case #753 - "Create Unconnected Nets" is created too late for analysis - LP 
   //Version = "4.4.0hs";  // 05/19/04 - Case #769 - Writers not working correctly with new DFT - LP
   //Version = "4.4.0hr";  // 05/18/04 - Case #700 - Stencil Generator not applying maximum feature webbing - knv
   //Version = "4.4.0hq";  // 05/18/04 - Case #711 - Fixed multiple CKT writer and NAR reader functionality - AC
   //Version = "4.4.0hp";  // 05/18/04 - Case #580 - Data Doctor - Pins - knv
                           //            Case #586 - Data Doctor - Parallel
                           //            Case #597 - Data Doctor - Status
                           //            Case #599 - Data Doctor - Combo box
   //Version = "4.4.0ho";  // 05/18/04 - Refix Case #549 - Pads Read not correctly typing layer type from PADS file and PADS.IN file - LP
   //Version = "4.4.0hn";  // 05/17/04 - Case #709 - DataFile.cpp - Implemented a try..catch for loading in a CC file so the exception will be cleaned up correctly - AC
                           //          - Case #651 - MneuIn.cpp - Made sure error message uses ErrorMessage so dialogs will be suppressed when necessary
                           //          - Case #757 - Added option to loading in net conditions without being case sensitive
   //Version = "4.4.0hm";  // 05/14/04 - Case #732 - Panelization.cpp & h - Fixed creation of 1 up panel to ask if user wants to Auto Board Origin the board being inserted - AC
                           //          - Case #752 - ProbePlacment.cpp - Changed message box icon to an info icon when reporting how many probes were placed
                           //          - Case #750 - ProbePlacment & DFT - Added saving of net condition information onto the net and remove the resetting of net conditions
                           //          - Vb00In.cpp - Fixed VBASCII to only get the net source from the layout.hkp
   //Version = "4.4.0hl";  // 05/14/04 - Layer.cpp, CcDoc.cpp - Fixed layer array - LP
   //Version = "4.4.0hk";  // 05/14/04 - Added implementation for Create Probe right-click menu command - SN
   //Version = "4.4.0hj";  // 05/14/04 - Replaced the global variable maxLayerArray with the function getMaxLayerIndex() and removed maxLayerArray completely - LP 
                           //          - Case #754 - CAMCAD crashed due to accessing invalid layer - LP
   //Version = "4.4.0hi";  // 05/13/04 - Case #520 - Vb99In.cpp - Fixed handling of duplicate pin names - AC
                           //          - Case #624 - Panelization.cpp & h - Changed "Cancel" button to "Close"
   //Version = "4.4.0hh";  // 05/13/04 - Case #549 - PadsIn.cpp - Pads Read, layer type from PADS.IN is not being applied to the specified layers - LP
   //Version = "4.4.0hg";  // 05/12/04 - Case #726 - GenCad.cpp - Show the Select Nets dialog when the Highlight Selected Items Net is invoked - AC
                           //          - Case #733 - PcbUtil.cpp - Fixed writing of text types to handle quotes in the text correctly
   //Version = "4.4.0hf";  // 05/12/04 - Refix Case #701 - Need to output padstack for all geometry even those that are not used - LP
   //Version = "4.4.0he";  // 05/12/04 - Case #745 - AA is not correctly ananlysing when file has an offset - LP
   //Version = "4.4.0hd";  // 05/12/04 - Case #725 - Camcad boggin down - SN
   //Version = "4.4.0hc";  // 05/12/04 - Case #748 - Camcad crash when selectingProbes tab in probe placement - SN
   //Version = "4.4.0hb";  // 05/12/04 - Case #737 - Additional menu commands removed - SN
   //Version = "4.4.0ha";  // 05/11/04 - Case #648 - Orcad import issue - knv
   //Version = "4.4.0gz";  // 05/11/04 - Case #747 - Gerber writer issue dealing with round pad location calculations on rotated components - AC
   //Version = "4.4.0gy";  // 05/11/04 - Case #648 - Orcad import issue - knv
   //Version = "4.4.0gx";  // 05/11/04 - Case #744 - Vb99Out.cpp - Outputing incorrect HOLE_NAME for padstack - LP
   //Version = "4.4.0gw";  // 05/11/04 - Case #739 - QD_Out.cpp - Allow panel level insert to have non-orthagonal rotation - LP
                           //          - Case #720 - MentorIn.cpp - Creating default generic layer mapping is not working correctly - LP
   //Version = "4.4.0gv";  // 05/11/04 - Case #648 - Orcad import issue - knv
   //Version = "4.4.0gu";  // 05/11/04 - Added Gerber and HPGL write to Vision, but only visible to the user if licensed - AC
   //Version = "4.4.0gt";  // 05/11/04 - Case #718 - Access Analysis, tidy up some options - LP
   //Version = "4.4.0gs";  // 05/10/04 - Case #721 - AA round soldermask is not being interpreted properly when it is every closed to the same size as the metal - LP
   //Version = "4.4.0gr";  // 05/10/04 - Case #495 - GerbIn.cpp - Fixed interpretation of Macro Aperture to correctly calculate size and rotation - AC
                           //          - Case #729 - Fixed hot keys functionality
   //Version = "4.4.0gq";  // 05/10/04 - Case #740 - Gencam Read Crash fixed - SN
   //Version = "4.4.0gp";  // 05/10/04 - Case #730 - Enhance Assign Graphic Class dialog - SN
   //Version = "4.4.0go";  // 05/10/04 - Case #737 - Clean menu cmmands - SN
   //Version = "4.4.0gn";  // 05/10/04 - Case #735 - Remove Check Value Attribute command - SN
   //Version = "4.4.0gm";  // 05/10/04 - Case #585 - Draw.cpp - Probes not showing up when zooming with layer turned off - SN
   //Version = "4.4.0gl";  // 05/10/04 - Case #712 - Sm_Anal.cpp - Board outline check gives incorrect analysis due to the board primary outline was not closed -  LP
   //Version = "4.4.0gk";  // 05/07/04 - Case #731 - Add.cpp - Add Arc Radius calculation changed to floating point - DD
   //Version = "4.4.0gj";  // 05/07/04 - Case #557 - Vb99Out.cpp/.h - Padshape is output as rectangle when it should be square -  LP
                           //          - Case #701 - Vb99Out.cpp/.h - Need to support output of different PADSTACK_TYPE for VIA, PIN, FIDUCIAL, and MOUNTING HOLE - LP
   //Version = "4.4.0gi";  // 05/05/04 - Case #680 - Pads_Out.cpp - Fixed issue of writing a pin number 0 and renaming of pins and assignement of pin numbers incorrectly (check_terminals) - AC
                           //          - PanRef.cpp - Fixed ComponentExtents to correctly get the extents of all specified components
                           //          - Case #708 - Settings.cpp - Modified the saving and reading of the new settings file format
                           //          - Case #719 - VB99In.cpp - Initialize all variables in the static structure Global
   //Version = "4.4.0gh";  // 05/05/04 - Case #508 - Pads_Out.cpp & h - Fixed writing of pads mirror to be built from the bottom - AC
   //Version = "4.4.0gg";  // 05/05/04 - Case #699 - Vb99Out.cpp - VBAscii Write, incorrectly writing THRU HOLE padstack - LP
   //Version = "4.4.0gf";  // 05/05/04 - Case #561- Cr3000In.cpp - Added command, .COPYDEVICE, to copy the value of the internal $$DEVICE$$ attrib to a user defined keyword. - DF
   //Version = "4.4.0ge";  // 05/04/04 - Case #714- Fixed the button icon and tool tip for the test attribute assignment function on the DFT Toolbar - AC
                           //          - Case #715 - ProbePlacement.cpp - Added surface column to placed probes on the probe placement report
                           //          - Fixed error when editing a geometry and pressing enter...closed the dialog, but didn't reset state properly
   //Version = "4.4.0gd";  // 05/04/04 - Case #716- MentorIn.cpp - Attrach an attribute "Mentor_Layer" onto "Layer_1" and "Layer_2" and its corresponding "Layer_Top" and "Layer_Bot" - LP
   //Version = "4.4.0gc";  // 05/04/04 - Case #631 - MentorIn.cpp - Explode "SIGNAL" layer does not explode all the insert to non-consecutive "SIGNAL_#" layer - LP
                           //          - Case #632 - MentorIn.cpp - Attributes are not moved to layers of "COMPONENT_SPECIFIC_LAYER_ON" - LP
   //Version = "4.4.0gb";  // 05/04/04 - Alpha release #6 of "Convert Draws to Apertures" - knv
   //Version = "4.4.0ga";  // 05/04/04 - Case #527 - GenCad_o.cpp - Corrected output of board and panel outline - LP
                           //             - If writing BoardOutline or PanelOutline then, then check to see if there is any poly of that type
                           //             - If there is, then only write out that poly of that graphic class
                           //             - If there is not, then write poly on the layer type specified
   //Version = "4.4.0fz";  // 05/03/04 - Case #600 - GenCad_o.cpp - Fixed minor unplaced probe coordinates - AC
   //Version = "4.4.0fy";  // 04/30/04 - MentorIn.cpp - Need to set insert type to fiducial if the block type is a fiducial - LP
   //Version = "4.4.0fx";  // 04/30/04 - Case #486 - ExcelIn.cpp - Excellon Import, removed a popup dialog during import - LP
                           //          - Case #504 - Draw_Msc.cpp, PinLabelSetting.cpp/.h, Settings.cpp - Added "TextAngle" to default.set file - LP
                           //          - Case #551 - Net_Util.cpp - "Comp/Pins loaded" count was incorrect after loading netlist - LP
                           //          - Case #614 - CompPin.cpp - Added the netname next to the comppin when query comppin attributes - LP
   //Version = "4.4.0fw";  // 04/30/04 - Case #572 - Removed redundant menu options - AC
                           //          - Case #603 - Panelization.cpp - Added single board panel option, change procedure to create a panel
                           //          - Data.cpp - Fixed poly extents calculation to take into account negative bulges
   //Version = "4.4.0fv";  // 04/30/04 - Case #702 - Generate Device Type: Style: Configue, "A parameter is missing ..." error corrected - SN
   //Version = "4.4.0fu";  // 04/30/04 - Case #703 - Generate Device Type: Style, Make components with valid style Green - SN
   //Version = "4.4.0ft";  // 04/29/04 - Case #465 - Attrib.cpp/.h, LyrManip.cpp, PadsIn.cpp - Need to add multiple instance of REFNAME - LP
                           //          - Case #623 - Ipc_in.cpp - Corrected problem of only create round pads - AC
                           //          - Case #616 - GenCad_o.cpp - Corrected FID line of those found in a component
                           //          - Case #600 - GenCad_o.cpp - Corrected output of unplaced probes and probes not assigned to a net
                           //          - Case #537 - CcView.cpp - Fixed scrolling leaving black lines when bg is not black
                           //          - Case #464 - Menu.cpp - Fixed loading of menu.txt file
                           //          - Case #488 - Ipl_out.cpp - Fixed ipl algorith to start with 0 so it is supported as a preassigned channel
                           //          - Case #497 - Settings.cpp - Added new option in the settings to show/hide point entities
   //Version = "4.4.0fs";  // 04/29/04 - Case #510 - 
   //Version = "4.4.0fr";  // 04/29/04 - Case #704 - Asymtek, not initialized hearder file directory - LP
                           //          - Case #705 - Asymtek, addded the unit "tenthmil" - LP
                           //          - Case #706 - Asymtek, copy the file "Asymtek.avw" to output file (*,avw) - LP
   //Version = "4.4.0fq";  // 04/29/04 - Case #707 - Access Analysis, not finding some unconnected pin - LP
                           //          - Access Analysis, Net Feature Lost is not correctly update for Board Outline Distance - LP
   //Version = "4.4.0fp";  // 04/28/04 - Refix Case #661 - Access Analysis, Board Outline Distance check crashed and did not chec if the distance is too large - LP
                           //          - Case #419 - Ta_Dlg.cpp - Fixed use of icon on message box when displaying result information
                           //          - Case #697 - DdeIn.cpp - Removed message boxed displaying padshape errors and put them in the log file
   //Version = "4.4.0fo";  // 04/27/04 - Implemented the use of a check file for the CKT writer (ckt.chk) - AC
   //Version = "4.4.0fn";  // 04/27/04 - Alpha release #5 of "Convert Draws to Apertures" - knv
   //Version = "4.4.0fm";  // 04/27/04 - Alpha release of the Nail Assignment Report reader - AC
   //Version = "4.4.0fl";  // 04/27/04 - Case #492 - Cadif read has incorrect pin rotation due to the function Exchange_SubBlock() incorrectly set the rotation - LP
   //Version = "4.4.0fk";  // 04/27/04 - Alpha release #4 of "Convert Draws to Apertures" - knv
   //Version = "4.4.0fj";  // 04/27/04 - Case #661 - Access Analysis, added Board Outline Distance check - LP
   //Version = "4.4.0fi";  // 04/26/04 - Case #635 - Access Analysis, added precentage to result grid - LP
   //Version = "4.4.0fh";  // 04/23/04 - Case #610 - Access Analysis, added modification to the AccessAnalysis.txt report - LP
   //Version = "4.4.0fg";  // 04/23/04 - Refix Case #658 - "Ignore Unloaded Comp Outline" was not working correctly - LP
   //Version = "4.4.0ff";  // 04/23/04 - Case #695 - Fixed: Placed & Unplaced probes list not updated in Navigator - SN
   //Version = "4.4.0fe";  // 04/23/04 - Case #698 - Crash 2 fixed - SN
   //Version = "4.4.0fd";  // 04/23/04 - Case #694 - Crash fixed - SN
   //Version = "4.4.0fc";  // 04/23/04 - Case #660 - CKT exporter outputs diodes wrong, corrected - SN
   //Version = "4.4.0fb";  // 04/23/04 - Modified the function to generate DFT outline not to take centriod into consideration - LP
   //Version = "4.4.0fa";  // 04/23/04 - Case #404 - Fixed Schematic Link Net comparision when run the first time with existing cross referenece data - AC
   //Version = "4.4.0ez";  // 04/23/04 - Access Analysis, completed the implementation of Auto Offset - LP
                           //          - PolyLib.cpp/.h - Corrected the Is_Pnt_Inside_PntList() function to consider bulge when check if a point is in a point list - LP
                           //          - CcDoc.cpp - Modified the function OnPcbConvertviatopcbcomponent() to call OnGeneratePinloc() at the end to correctly creat comp pin location - LP
                           //          - Case #567 - Access Analysis, implemented "Ignore One Pin Comp Outline" - LP
                           //          - Case #638 - Access Analysis, missing access point for some comppins that should have them - LP
                           //          - Case #658 - Access Analysis, implemented "Ignore Unloaded Comp Outline" - LP
                           //          - Case #664 - Access Analysis, incorrect error message - LP
   //Version = "4.4.0ey";  // 04/22/04 - Alpha release #3 of "Convert Draws to Apertures" - knv
   //Version = "4.4.0ex";  // 04/22/04 - Case #376 - Dfm_Flat.cpp - modified the flattening of RealParts to be handle in a special way - AC
   //Version = "4.4.0ew";  // 04/21/04 - Refactor - privatized all TypeStruct and TypePinStruct data members - knv
   //Version = "4.4.0ev";  // 04/21/04 - Alpha release #2 of "Convert Draws to Apertures" - knv
   //Version = "4.4.0eu";  // 04/20/04 - Access Analysis, more implementation to support Auto Offset, but not complete yet - LP
   //Version = "4.4.0et";  // 04/20/04 - Case #561 - Added function to the CR3000 PWS reader to copy attributes of things in a file and block (Dean to continue working on case to close it) - AC
   //Version = "4.4.0es";  // 04/20/04 - Access Analysis, using the CExposedData structure with the old algorithm is working good, need to work on Auto offset algorithm - LP
   //Version = "4.4.0er";  // 04/20/04 - Case #691 - Added functionality to support a mentor.clr color file for layer colors - AC
   //Version = "4.4.0eq";  // 04/20/04 - Case #648 - Orcad import issue - knv
   //Version = "4.4.0ep";  // 04/19/04 - Case #648 - Orcad import issue - knv
   //Version = "4.4.0eo";  // 04/18/04 - Case #682 - Unable to zoom to net after creating single pin nets fixed - SN
   //Version = "4.4.0en";  // 04/19/04 - Case #685 - Access Analysis & Probe Placement, use Project path for the directory to output the AA and PP report - LP
                           //          - Case #687 - Access Analysis, duplicate test plan name in list box - LP
   //Version = "4.4.0em";  // 04/18/04 - Case #683 - CKT exporter crash fixed - SN
   //Version = "4.4.0el";  // 04/18/04 - Case #686 - Shows VIA when via has no refname - SN
   //Version = "4.4.0ek";  // 04/16/04 - Case #612 - Added the ability to change highlight colors for navigator and saving them in the settings file - SN
   //Version = "4.4.0ej";  // 04/16/04 - Modified the function CopySoldermaskAnalysisLayers() and CopyExplodeMetalLayers() - LP
   //Version = "4.4.0ei";  // 04/16/04 - Case #681 - Fixed crash when trying to access attribues where there aren't any (needed to check for NULL) - AC
                           //          - Case #684 - Fixed crash on testprobe report, needed to refresh list because of changes caused by Single Pin Net generation
                           //          - Case #595 - Fixed unplaced probes to remove only some probes so we can try to maximize used probes
   //Version = "4.4.0eh";  // 04/16/04 - Added new class and implementation to support automatic offset but not yet complete - LP
   //Version = "4.4.0eg";  // 04/16/04 - Case #613 - Added the option to choose between Pan/Zoom in PCB Navigator - SN
   //Version = "4.4.0ef";  // 04/14/04 - Case #588 - Added option to not write out ignored surface result to report - LP
                           //          - Added test plan setting to Access Analysis report - LP
   //Version = "4.4.0ee";  // 04/14/04 - Case #656 - Added Test Attribute Assignment toolbar button to the DFT toolbar - AC
                           //          - Case #639 - Fixed PP result table to reflect the actual name of specified probes
                           //          - Case #615 - Added support of the PP result table to gray out unused fields
   //Version = "4.4.0ed";  // 04/14/04 - Refix Case #670 - The change should only for 0 and 180 degree - LP
                           //          - Added, but not yet implement, options for "Access Offset" in Option tab and update testplan in CC file and external file - LP
   //Version = "4.4.0ec";  // 04/14/04 - Case #672 - Showing pins multiple times fixed - SN
   //Version = "4.4.0eb";  // 04/14/04 - Case #653 - Fixed pin to net mapping - SN
   //Version = "4.4.0ea";  // 04/14/04 - Alpha release of "Convert Draws to Apertures" - knv
   //Version = "4.4.0dz";  // 04/13/04 - Added option for "Ignore One Pin Comp Outline" and "Ignore Unloaded Comp Outline" in Physical Constrain tab and in testplan - LP
   //Version = "4.4.0dy";  // 04/13/04 - Case #577 - ProbePlacement.cpp - Re-gather tartget types from access points in case they were changed - AC
                           //          - Case #591 - ProbePlacement.cpp - Added prefixes to power injection probes
                           //          - Case #659 - Testway_in.cpp - added functionality to read testway feedback file
                           //          - Case #671 - LyerType & XmlLayerTypeContent.cpp - Fixed reading of layer type files correctly
   //Version = "4.4.0dx";  // 04/13/04 - Case #575 - GenerateCentroidAndOutline.cpp - DFT outline for "Body Outline" should be generated regardless if the body outline layer is visible or not - LP
   //Version = "4.4.0dv";  // 04/13/04 - Case #647 - AccessAnalysisDlg.cpp/.h - Check for existence of soldermask layer before enabling the Soldermask Analysis checkbox - LP
                           //          - CcDoc.h, Layer.h, LyrManip.cpp - Moved the function MarkUnusedLayers() into CLayerArray and CAMCADDoc - LP
   //Version = "4.4.0du";  // 04/13/04 - Case #668 - MentorIn.cpp - CAMCAD crash due to reference to an invalid block - LP
   //Version = "4.4.0dt";  // 04/13/04 - Case #669 - Block.cpp - CAMCAD crash due to unchecked NULL layer in GenerateDesignSurfaceAttribute() - LP
   //Version = "4.4.0ds";  // 04/12/04 - Case #576 - Fixed component outline evaluation for probes - AC
                           //          - Moved PCB Navigator option in menu and tool bar dialog into a sorted position
                           //          - Removed offset controls from the Probe Placement Options page & added button for TestWay reading (currently disabled)
   //Version = "4.4.0dr";  // 04/12/04 - Case #589 - Showing multiple probes uncer net in net mode - SN
   //Version = "4.4.0dq";  // 04/12/04 - Case #598 - Added PCB Navigator to Arrange ToolBar Dialog and to toolbar menu - SN
   //Version = "4.4.0dp";  // 04/12/04 - Case #670 - QD_Out.cpp - Do not subtract 180 degree from comp rotation for mirror board - LP
   //Version = "4.4.0do";  // 04/12/04 - Case #621 - GerOut.cpp - New aperture needed if rotation does not equal original rotation (instead of rotation does not equal zero).  FindMirBlocks() fixed to include LayerInFile().  Revamped several functions. - DD
   //Version = "4.4.0dn";  // 04/09/04 - Case #594 - ProbePlacement.cpp - Fixed another probe placement over lapping, error when checking a probes closes neighbor when both sizes are already set - AC
                           //          - Case #630 - Fixture_out.cpp - Fixed fixture file tooling hole output to generate names if not present
                           //          - Case #629 - Fixture_out.cpp - Fixed fixture file Cap Opens section to write the centroid location relative to the board origin
                           //          - Case #640 - ProbePlacement.cpp - Put in a contraint where each probe must have all fields definedFixed fixture file Cap Opens section to write the centroid location relative to the board origin
   //Version = "4.4.0dm";  // 04/09/04 - Case #667 - AllegIn.cpp - Moved the calls to generate_PADSTACKACCESSFLAG() and generate_PINLOC() so that they execute against all Allegro file types imported. - DF
   //Version = "4.4.0dl";  // 04/08/04 - Case #594 - ProbePlacement.cpp - Fixed probe placement over lapping, error when probe already assigned a size not reassigning to a smaller one when necessary - AC
                           //          - Case #663 - ProbePlacement.cpp - Changed progress dialog title in from "Schematic Link" to "Probe Placement" when placing probes
                           //          - Case #630 - Fixture_out.cpp - Fixed error in trying to recognize and output tooling holes
                           //          - Case #634 - ProbePlacement.cpp - Fixed crash when power injection value is set to zero causing a divide by zero error
                           //          - DFT.cpp - Fixed reading the surface of target priorities from the cc file
   //Version = "4.4.0dk";  // 04/08/04 - Case #564 - AccessAnalysisDlg.cpp/.h - Via should not be consider as a THRU HOLE for target type - LP
                           //          - Case #636 - AccessAnalysisDlg.cpp/.h - On Package Outline tab, clicking the same geometry in grid the second time should change back to board view - LP
   //Version = "4.4.0dj";  // 04/08/04 - Case #652 - Cr3000in.cpp - Fixed offset of padstack apertures (write_padstack function) - AC
   //Version = "4.4.0di";  // 04/07/04 - Case #566 & #573 - Access surface of components were incorrectly identified - LP
                           //          - Case #574 - Should only expose poly of type ETCH at geometry level - LP
   //Version = "4.4.0dh";  // 04/06/04 - AccessAnalysisDlg.cpp/.h, Camcad.rc, DFT.cpp/.h, resource.h - Load external testplan from AccessAnalysis - LP
                           //          - Case #563 - PadsIn.cpp - Fixed limitation of the pads reader when reading the line section - AC
   //Version = "4.4.0dg";  // 04/06/04 - Case #650 - Vb99In.cpp - Added "layout.hkp" option to .NETLISTSOURCE command and added comp/pin net assignments from the pinToUniqueNetZeroArray() array. - DF
   //Version = "4.4.0df";  // 04/05/04 - Case #592, 605, 504, 593, 584 - These problems don't seem to emit the errors indicated...probably fixed when correcting other issues previously - AC
   //Version = "4.4.0de";  // 04/05/04 - Case #655 - CR3000In.cpp - Corrected error in Get_Line() where conversion of certain extended ASCII characters resulted in a EOF marker or a Zuken structure terminator (;). - DF
   //Version = "4.4.0dd";  // 04/05/04 - Case #648 - OrcltdIn.cpp - Check for bottom built geometry and build a top version and reverse all the insert placement of this geometry - LP
                           //          - CcDoc.cpp/.h - Added public function GenerateBlockDesignSurfaceInfo() - LP
                           //          - Block.cpp/.h, GenerateCentroidAndOutline.cpp - Added a perameter to the function GenerateDesignSurfaceInfo() - LP
   //Version = "4.4.0dc";  // 04/02/04 - Case #646 - Added new functionality to generate the design surface attribute on blocks - AC
                           //          - Generate outlines on layers based on the design surface attribute
   //Version = "4.4.0db";  // 04/02/04 - Case #617 - PCB Navigator, Making size list dynamic in Edit Probe  - SN
   //Version = "4.4.0da";  // 04/02/04 - Case #602 - PCB Navigator crash after create singlePin nets fixed - SN
   //Version = "4.4.0cz";  // 04/02/04 - Case #649 - OrcltIn.cpp - Orcad Read SYM derived 0 should not be replace with another SYM - LP
   //Version = "4.4.0cy";  // 04/02/04 - Case #609 - AccessAnalysisDlg.cpp - Rename "TestibilityReport" to "AccessReport" - LP
                           //          - Case #570 - AccessAnalysisDlg.cpp - Rename "Rectangle Outline" to "Rectangular Outline" and uncheck the checkbox by default - LP
   //Version = "4.4.0cx";  // 04/02/04 - Case #608 - PCB Navigator Gray Screen fixed - SN
   //Version = "4.4.0cw";  // 04/01/04 - Case #571 - PCB Naviator sorting added - SN
   //Version = "4.4.0cv";  // 04/01/04 - Case #583 - AccessAnalysisDlg.cpp - Fixture THRU HOLE component is incorrectly analysized - LP
   //Version = "4.4.0cu";  // 04/01/04 - ViscomAOI_In.cpp - Modified the output unit to MM and the header comment for all files - LP
   //Version = "4.4.0ct";  // 03/31/04 - Added modification to the Convert to PCB Component function - LP
   //Version = "4.4.0cs";  // 03/31/04 - Case #560 - CR3000In.cpp, CR3000In.h - Added .TOOLPART, .FIDUCIALPART, .FIDUCIALPART, and .FIDUCIALPART to In file and added handalling. - DF
   //Version = "4.4.0cr";  // 03/31/04 - DeviceTyper.cpp - Fixed Device Typer bugs - SN
   //Version = "4.4.0cq";  // 03/30/04 - Case #628 Refix - MentorIn.cpp - Incorrectly use a global variable counter cause invalid result - LP
   //Version = "4.4.0cp";  // 03/30/04 - Case #601 - VIAS: use via refname - SN
   //Version = "4.4.0co";  // 03/30/04 - CAMCAD.RC, resource.h, CcDoc.cpp/.h - Renamed the menu command "PCB->Convert Via to PCB Component" to "PCB-> Convert to PCB Component" and removed the command "PCB->Convert Test Via to PCB Component" - LP
   //Version = "4.4.0cn";  // 03/30/04 - Case #606 - Edit Probe: Refresh after OK - SN
   //Version = "4.4.0cm";  // 03/30/04 - Case #607 - VIA ICON - SN
   //Version = "4.4.0cl";  // 03/30/04 - CcDoc.cpp/.h - Allows user to select several feature types to convert to PCB component - LP
   //Version = "4.4.0ck";  // 03/30/04 - First cut of probe shapes when probes are placed - AC
                           //          - ccrepair.cpp - Put in conditional statement not to automatically show validation report in release builds
   //Version = "4.4.0cj";  // 03/29/04 - Case #562 - Edif300_In.cpp/.h, Sch_Lib.cpp - Do not change netname if the net int the heirarchy sheet is connected to an off/on page connector - LP
                           //          - Sch_Link.cpp - Fixed a bug in the function SchLink::ApplyFilters() - LP
   //Version = "4.4.0ci";  // 03/29/04 - Refactor - privatized all NetStruct data members - knv
   //Version = "4.4.0ch";  // 03/28/04 - Refactor - privatized all CompPinStruct data members - knv
   //Version = "4.4.0cg";  // 03/26/04 - Added shapes and colors to the UI for probe definitions (still need to create probe with the shapes) - AC
   //Version = "4.4.0cf";  // 03/26/04 - Case #587 - Spea_Out.cpp - Implemented the outpf of pin name base on pin function for netlist section - LP
                           //          - Block.ccp/.h - - Add function in block to look for insert by refdes, function FindData() - LP
   //Version = "4.4.0ce";  // 03/25/04 - Case #531 - Unicam reader stops parsing "$netlist" section when "nail" or "short" section encountered - knv
                           //          - Added progress bar to Unicam reader - knv
   //Version = "4.4.0cd";  // 03/25/04 - Case #530 - Unicam reader stops parsing "$netlist" section when "nail" section encountered - knv
   //Version = "4.4.0cc";  // 03/25/04 - Added new DFT toolbar - AC
   //Version = "4.4.0cb";  // 03/25/04 - Case #553 - Pads_Out.cpp - Corrected error in Blind/Buried via output. - DF
   //Version = "4.4.0ca";  // 03/25/04 - Case #643 - Ta_Optn.cpp - The old Testability Analysis put keepout on loaded "FALSE" instead of "TRUE" - LP
   //Version = "4.4.0bz";  // 03/25/04 - Case #628 - Mentor Write is incorrectly writing normal padshape after writing a complex padshape - LP
   //Version = "4.4.0by";  // 03/24/04 - Removed the PPSolution section from the CC file (not necessary until we support multiple solutions)- AC
   //Version = "4.4.0bx";  // 03/24/04 - Added support to read testplan information from the CC file - AC
   //Version = "4.4.0bw";  // 03/24/04 - Case #530 - Unicam reader only creates SMD components - knv
   //Version = "4.4.0bv";  // 03/23/04 - ViscomAOI_In.cpp - Corrected an error message - LP
                           //          - Asymtek_Out.cpp/.h - Completed the Asymtek Dispenser writter - LP
   //Version = "4.4.0bu";  // 03/23/04 - Completed writing of all DFT solution data into the CC file (except for access name on AccessPoint's) - AC
                           //          - Fixed bug with cleaning up DFT solutions
                           //          - Fixed problem with keeping correct pointers to ProbeTemplates when copying the probe placement dialog's solution back to the current solution
   //Version = "4.4.0bt";  // 03/22/04 - Added the Asymtek Dispenser writer, but not complete, yet - LP
   //Version = "4.4.0bs";  // 03/22/04 - Enabled Large Address Support - knv
                           //          - Fixed refactoring bugs
   //Version = "4.4.0br";  // 03/22/04 - Completed the saving of the test plan into the cc file (still need the Access Analysis and Probe Placement solutions) - AC
                           //          - Added ability to save .pln testplan file (buttons still need to be mapped)
   //Version = "4.4.0bq";  // 03/22/04 - Refactor - privatized all Attrib data members - knv
   //Version = "4.4.0bp";  // 03/21/04 - Refactor - privatized all LayerStruct data members - knv
   //Version = "4.4.0bo";  // 03/19/04 - Replaced calls of DataStruct::setGraphicClass(graphicClassUndefined) with DataStruct::setGraphicClass(graphicClassNormal) - knv
                           //          - Case #622 - Crash during ODB++ read
                           //               Changed allocation scheme for COdbNet to reduce memory fragmentation.
                           //               Added new handler, camCadNewHandler(), for detecting and reporting memory allocation failures.
   //Version = "4.4.0bn";  // 03/19/04 - Added writing of some of the DFT testplan data - AC
                           //          - Changed access to the DFT solutions through the FileStruct
   //Version = "4.4.0bm";  // 03/18/04 - Case #539 - Mneutin.cpp - Refix - Reassign device type to the correct geometry - LP                          
                           //          - AtrbList.cpp - Fixed a crash due to reference of invalid geometry - LP
                           //          - CcRepair.cpp/.h - Added verifyDeviceType() in verifyAndRepairData() - LP
   //Version = "4.4.0bl";  // 03/19/04 - fixture.cpp - Fixed bug in Fixture exporter gather data twice and therefore writing duplicates - AC
                           //          - added handling for double wired features in the drill data and receptacle size sections
   //Version = "4.4.0bk";  // 03/19/04 - Lic.cpp - Make the licencing of Viscom AOI and Asymtek Dispenser available in release - LP
                           //          - ViscomAOI_IN.cpp - Implemented loading of setting file - LP
   //Version = "4.4.0bj";  // 03/18/04 - Major changes the CC file writing to make all objects write their own section - AC
                           //          - Modified the Fixture file output to write separate file for the top and bottom and to take in a directory
   //Version = "4.4.0bi";  // 03/18/04 - DbUtil.h, Lic.pp/.h - Added file type of Viscom AOI and Asymtek Dispenser and licencing for Releast Test version - LP
                           //          - ViscomAOI_In.cpp, Port_Lib.cpp, Port.h - Added Viscom AOI writer - LP
   //Version = "4.4.0bh";  // 03/18/04 - Case #539 - Mneutin.cpp - Refix - LP
   //Version = "4.4.0bg";  // 03/17/04 - Case #539 - MentorIn.cpp, Mneutin.cpp - Added support of recognized that a Boardstation geom contents the library info for Neutral - LP
   //Version = "4.4.0bf";  // 03/17/04 - First cut of the Fixture File - AC
   //Version = "4.4.0be";  // 03/16/04 - Case #622 - Crash during ODB++ read - Rebug - knv
   //Version = "4.4.0bd";  // 03/16/04 - Case #622 - Crash during ODB++ read - knv
   //Version = "4.4.0bc";  // 03/16/04 - Bom.cpp - Added support of ".ATTACH2COMPATTRIBUTE" - LP 
                           //          - keyword.cpp - Corrected keyword lookup to not be case sensitive - Lp
                           //          - Apertur2.cpp - Corrected square and restangle apertures to be filled - LP
   //Version = "4.4.0bb";  // 03/15/04 - Added more functionality to Feature File exporter - AC
                           //          - changed file handling in DFT to pass CFormatStdioFile's by reference instead of a pointer
   //Version = "4.4.0ba";  // 03/15/04 - Case #620 - Crash during Stencil Generation - knv
   //Version = "4.4.0az";  // 03/15/04 - Refactor - privatized all DataStruct data members - knv
   //Version = "4.4.0ay";  // 03/14/04 - Refactor - privatized all InsertStruct data members - knv
   //Version = "4.4.0ax";  // 03/13/04 - Refactor - privatized all CPoly data members - knv
   //Version = "4.4.0aw";  // 03/12/04 - Beta version 2 of Data Doctor, added refdes zooming and capacitorOpens attribute - knv
   //Version = "4.4.0av";  // 03/12/04 - linked with the DFT module yet (only for testing) - AC
                           //          - Added more implementation of fixture file output
   //Version = "4.4.0au";  // 03/11/04 - Added new Fixture file lic, not currently linked with the DFT module yet (only for testing) - AC
                           //          - Added partial implementation of fixture file output
   //Version = "4.4.0at";  // 03/11/04 - Completed the followings:
                           //          - Optimized exposed metal - LP
                           //          - calculating largest exposed diameter for expose metal of type round, square, rectangle, and oblong - LP
   //Version = "4.4.0as";  // 03/10/04 - Added Edit Probe dialog functionality - SN
   //Version = "4.4.0ar";  // 03/10/04 - Added access point violations to the probe placement report - AC
                           //          - Moved all DFT class declarations to DFT.h
   //Version = "4.4.0aq";  // 03/09/04 - CAMCAD.RC, CcDoc.cpp/.h, resource.h - Added menu command Tool->PCB->Convert Test Via to PCB Component - LP
   //Version = "4.4.0ap";  // 03/09/04 - Case #386 - Pad rotations in Padstacks - DD
   //Version = "4.4.0ao";  // 03/09/04 - Repaired refactor bugs for RealPart - DD
   //Version = "4.4.0an";  // 03/09/04 - Refactor - privatized all TextStruct data members - knv
   //Version = "4.4.0am";  // 03/08/04 - Updated PCB Navigator - SN
   //Version = "4.4.0al";  // 03/08/04 - report.cpp - Fixed Test Probe report to only included placed probes and a header - AC
                           //          - Added partial test plan information to the probe placement report
                           //          - Removed loading and saving of project files from the menus
   //Version = "4.4.0ak";  // 03/08/04 - Refactor - privatized all FileStruct data members - knv
   //Version = "4.4.0aj";  // 03/07/04 - Refactor - privatized all BlockStruct data members - knv
   //Version = "4.4.0ai";  // 03/06/04 - Refactor - privatized BlockStruct::m_name - knv
   //Version = "4.4.0ah";  // 03/05/04 - Beta version 1 of Data Doctor Parallel Analysis - knv
   //Version = "4.4.0ag";  // 03/05/04 - Fixed Generate Centroid bug and updated PCB Navigator - SN
   //Version = "4.4.0af";  // 03/05/04 - Added TestWay Report - AV
   //Version = "4.4.0ae";  // 03/05/04 - Added Report for Access Analysis and Probe Placement - LP & AV
   //Version = "4.4.0ad";  // 03/04/04 - Generate Centroid bug fixed & cen rotation offset added - SN
   //Version = "4.4.0ac";  // 03/01/04 - Right Click Menu added to PCB Navigator - SN
   //Version = "4.4.0ab";  // 02/27/04 - Version 4.3.177 - knv
   //Version = "4.4.0aa";  // 02/26/04 - More changes to accomodate validity of maps in the Keyword array (now only map for cc keyword) - AC
                           //          - Removed old dongle code and dongle.cpp
   //Version = "4.4.0z";   // 02/25/04 - Many changes to accomodate validity of maps in the Keyword array - AC
   //Version = "4.4.0y";   // 02/23/04 - Updated Camcad Navigator & DataDoctor - SN & Knv
   //Version = "4.4.0x";   // 02/23/04 - Updated Camcad Navigator - SN
   //Version = "4.4.0w";   // 02/23/04 - Added progress bar to probe placement algorithm
                           //          - Fixed updating of outline generation grid in access analysis
                           //          - Fixed sorting of entities to make sure realparts are drawn after pcb components
                           //          - Made sure probe targets are drawn last, after the drill hole so colors are consistant
                           //          - Added regen extents on loading of cc file
                           //          - Added CKT license to the release build
   //Version = "4.4.0v";   // 02/21/04 - Added drag and drop for placed & unplaced probes - SN
   //Version = "4.4.0u";   // 02/20/04 - Alpha version 2 of Data Doctor + fixed Normalized - knv & SN
   //Version = "4.4.0t";   // 02/20/04 - Added placed and unplaced probes navigation - SN
   //Version = "4.4.0s";   // 02/20/04 - Fixed determining of optimal probe sizes base to proximity of other probes - AC
   //Version = "4.4.0r";   // 02/20/04 - Updated Probe Placement algorthim to create probes for net w/o access points as unplaced - AC
   //Version = "4.4.0q";   // 02/19/04 - Alpha version of Data Doctor - knv
   //Version = "4.4.0p";   // 02/18/04 - Added placed and Unplaced Probes - SN
   //Version = "4.4.0o";   // 02/18/04 - Cleaned CAMCAD NAVIGATOR highlighting and coloring issues - SN
   //Version = "4.4.0n";   // 02/18/04 - Alphs version of Test Access Analysis with Soldermask Analysis working - LP
   //Version = "4.4.0m";   // 02/11/04 - Version = 4.3.160 - LP
   //Version = "4.4.0l";   // 02/04/04 - Version = 4.3.152 - knv
   //Version = "4.4.0k";   // 02/03/04 - Added CAMCAD Navigator - SN
   //Version = "4.4.0j";   // 01/14/04 - CcDoc.cpp/.h - Added DFTSolutionList and CDFTSolution and a few access functions to CCEtoODBDoc class - LP
                           //          - DFT.cpp/.h - Added CPhysicalConst, CTargetType, CTestPlan, CDFTSolution classes to support the implementation DFT solution - LP
                           //          - AccessAnalysisDlg.cpp/.h - Add dialogs and classes to implementa Access Analysis - LP
   //Version = "4.4.0i";   // 12/03/03 - CAMCAD solution and project file was converted to VS2003 - AC
                           //             - multiple files were modified due to stricter data type conversion rules for compilation
   //Version = "4.4.0h";   // 12/03/03 - Version = 4.3.87 - LP
   //Version = "4.4.0g";   // 12/01/03 - Version = 4.3.85 - AC
   //Version = "4.4.0f";   // 12/01/03 - Version = 4.3.84 - DD
   //Version = "4.4.0e";   // 12/01/03 - Version = 4.3.83 - AC
   //Version = "4.4.0d";   // 11/26/03 - Version = 4.3.82 - AC
   //Version = "4.4.0c";   // 11/24/03 - Version = 4.3.79 - knv
   //Version = "4.4.0b";   // 11/24/03 - Version 4.3.78 - AC
   //Version = "4.4.0a";   // 11/24/03 - Branched 4.4 - AC
   ////////////////////////////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////////////////////////////
   //Version = "4.3.218";  // 05/05/04 - Case #699 - Vb99Out.cpp - VBAscii Write, incorrectly writing THRU HOLE padstack - LP
   //Version = "4.3.217";  // 04/30/04 - MentorIn.cpp - Need to set insert type to fiducial if the block type is a fiducial - LP
   //Version = "4.3.216";  // 04/20/04 - Case #691 - Added functionality to support a mentor.clr color file for layer colors - AC
   //Version = "4.3.215";  // 04/16/04 - Case #681 - Fixed crash when trying to access attribues where there aren't any (needed to check for NULL) - AC
   //Version = "4.3.214";  // 04/14/04 - Refix Case #670 - The change should only for 0 and 180 degree - LP
   //Version = "4.3.213";  // 04/12/04 - Case #671 - LyerType & XmlLayerTypeContent.cpp - Fixed reading of layer type files correctly - AC
   //Version = "4.3.212";  // 04/12/04 - Case #670 - QD_Out.cpp - Do not subtract 180 degree from comp rotation for mirror board - LP
   //Version = "4.3.211";  // 04/12/04 - Case #621 - GerOut.cpp - New aperture needed if rotation does not equal original rotation (instead of rotation does not equal zero).  FindMirBlocks() fixed to include LayerInFile().  Revamped several functions. - DD
   //Version = "4.3.210";  // 04/09/04 - Case #667 - AllegIn.cpp - Moved the calls to generate_PADSTACKACCESSFLAG() and generate_PINLOC() so that they execute against all Allegro file types imported. - DF
   //Version = "4.3.209";  // 04/08/04 - Case #652 - Cr3000in.cpp - Fixed offset of padstack apertures (write_padstack function) - AC
   //Version = "4.3.208";  // 04/06/04 - Case #563 - PadsIn.cpp - Fixed limitation of the pads reader when reading the line section - AC
   //Version = "4.3.207";  // 04/06/04 - Case #650 - Vb99In.cpp - Added "layout.hkp" option to .NETLISTSOURCE command and added comp/pin net assignments from the pinToUniqueNetZeroArray() array. - DF
   //Version = "4.3.206";  // 04/05/04 - Case #655 - CR3000In.cpp - Corrected error in Get_Line() where conversion of certain extended ASCII characters resulted in a EOF marker or a Zuken structure terminator (;). - DF
   //Version = "4.3.205";  // 04/05/04 - Case #648 - OrcltdIn.cpp - Check for bottom built geometry and build a top version and reverse all the insert placement of this geometry - LP
                           //          - CcDoc.cpp/.h - Added public function GenerateBlockDesignSurfaceInfo() - LP
                           //          - Block.cpp/.h, GenerateCentroidAndOutline.cpp - Added a perameter to the function GenerateDesignSurfaceInfo() - LP
   //Version = "4.3.204";  // 04/02/04 - Case #646 - Block.cpp - Added new functionality to generate the design surface attribute on blocks - AC
                           //          - GenerateCentroidAndOutline.cpp - Generate outlines on layers based on the design surface attribute
   //Version = "4.3.203";  // 04/02/04 - Case #649 - OrcltIn.cpp - Orcad Read SYM derived 0 should not be replace with another SYM - LP
                           //          - CcDoc.cpp - Replace the log file with a dialog to display result from "Convert to PCB Component" - LP
   //Version = "4.3.202";  // 04/02/04 - Removed the call to "GlobalMemoryStatusEx" to resolve the issue with window NT4.0 - LP
   //Version = "4.3.201";  // 03/31/04 - Added modification to the Convert to PCB Component function - LP
   //Version = "4.3.200";  // 03/31/04 - Case #560 - CR3000In.cpp, CR3000In.h - Added .TOOLPART, .FIDUCIALPART, .FIDUCIALPART, and .FIDUCIALPART to In file and added handalling. - DF
   //Version = "4.3.199";  // 03/31/04 - Case #562 - Edif300_In.cpp/.h, Sch_Lib.cpp - Do not change netname if the net int the heirarchy sheet is connected to an off/on page connector - LP
                           //          - Sch_Link.cpp - Fixed a bug in the function SchLink::ApplyFilters() - LP
   //Version = "4.3.198";  // 03/30/04 - Case #628 Refix - MentorIn.cpp - Incorrectly use a global variable counter cause invalid result - LP
   //Version = "4.3.197";  // 03/30/04 - CAMCAD.RC, resource.h, CcDoc.cpp/.h - Renamed the menu command "PCB->Convert Via to PCB Component" to "PCB-> Convert to PCB Component" and removed the command "PCB->Convert Test Via to PCB Component" - LP
   //Version = "4.3.196";  // 03/30/04 - CAMCAD.RC, resource.h, General.cpp/.h - Added a multiple select dialog - LP
                           //          - CcDoc.cpp/.h - Allows user to select several feature types to convert to PCB component - LP
   //Version = "4.3.195";  // 03/29/04 - ccrepair.cpp - Removed the showing of the CC validation report (still in 4.4 though, needs to be evaluated) - AC
   //Version = "4.3.194";  // 03/26/04 - Case #587 - Spea_Out.cpp - Implemented the outpf of pin name base on pin function for netlist section - LP
   //Version = "4.3.193";  // 03/25/04 - Case #531 - Unicam reader stops parsing "$netlist" section when "nail" or "short" section encountered - knv
                           //          - Added progress bar to Unicam reader - knv
   //Version = "4.3.192";  // 03/25/04 - Case #530 - Unicam reader only creates SMD components - knv
                           //                   - Unicam reader stops parsing "$netlist" section when "nail" section encountered - knv
   //Version = "4.3.191";  // 03/25/04 - Case #643 - Ta_Optn.cpp - The old Testability Analysis put keepout on loaded "FALSE" instead of "TRUE" - LP
   //Version = "4.3.190";  // 03/25/04 - Case #553 - Pads_Out.cpp - Corrected error in Blind/Buried via output. - DF
   //Version = "4.3.189";  // 03/25/04 - Case #628 - Mentor Write is incorrectly writing normal padshape after writing a complex padshape - LP
   //Version = "4.3.188";  // 03/18/04 - Case #539 - Mneutin.cpp - Refix - Reassign device type to the correct geometry - LP                          
                           //          - AtrbList.cpp - Fixed a crash due to reference of invalid geometry - LP
                           //          - CcRepair.cpp/.h - Added verifyDeviceType() in verifyAndRepairData() - LP
   //Version = "4.3.187";  // 03/18/04 - Case #539 - Mneutin.cpp - Refix - LP
   //Version = "4.3.186";  // 03/17/04 - Case #539 - MentorIn.cpp, Mneutin.cpp - Added support of recognized that a Boardstation geom contents the library info for Neutral - LP
   //Version = "4.3.185";  // 03/17/04 - Case #622 - Crash during ODB++ read - Rebug - knv
   //Version = "4.3.184";  // 03/16/04 - Case #622 - Crash during ODB++ read - knv
   //Version = "4.3.183";  // 03/16/03 - Bom.cpp - Added support of ".ATTACH2COMPATTRIBUTE" - LP
   //Version = "4.3.182";  // 03/15/04 - Case #620 - Crash during Stencil Generation - knv
   //Version = "4.3.181";  // 03/09/04 - CAMCAD.RC, CcDoc.cpp/.h, resource.h - Added menu command Tool->PCB->Convert Test Via to PCB Component - LP
   //Version = "4.3.180";  // 03/03/04 - Rebuilt using 4.3.174 code base plus older versions of the following files - knv
                           //   read_wrt/ThedaIn.cpp  Kurt Van Ness  2/27/04   4:32p   Checked in $/CAMCAD/4.3/read_wrt
                           //   read_wrt/GenCad_o.cpp Dean Admin     2/26/04   4:47p   Checked in $/CAMCAD/4.3/read_wrt 
                           //   PcbUtil.cpp           Dean Admin     2/26/04   4:37p   Checked in $/CAMCAD/4.3
   //Version = "4.3.179";  // 03/03/04 - Reimplmented text drawing - knv
   //Version = "4.3.178";  // 03/01/04 - Reimplmented text extents calculation - knv
                           //          - Refactored FontStruct and CFontList
   //Version = "4.3.177";  // 02/27/04 - Case #518 - Theda reader does not read files from Pilz correctly - knv
                           //          - Reshared the following files branched for keyword changes - knv
                           //            Api_Attr.cpp, AtrbList.cpp, Attr_Ass.cpp, DataFile.cpp, DeviceTyper.cpp
                           //            Edit.cpp, Sch_Lib.cpp, StencilDialog.cpp, StencilGenerator.cpp, Xml_Cont.cpp,
                           //            Xml_Wrt.cpp, CamCadDatabase.cpp, CamCadDatabase.h, RwLib.h
   //Version = "4.3.176";  // 02/26/04 - Case #559 - GenCad_o.cpp - GenCAD writer was not normalizing the names of padstacks referenced by Fiducial instances in the $MECH section - DF
   //Version = "4.3.175";  // 02/26/04 - Case #553 - PcbUtil.cpp - CAMCAD incorrectly identified vias as SMD vias for conversion to testpoints, corrected logic - DF
   //Version = "4.3.174";  // 02/26/04 - Case #538 - MentorIn.cpp - CAMCAD crash when loading a Boardstation file on top of a Neutral file - LP
   //Version = "4.3.173";  // 02/20/04 - ckt_out.cpp - Added filtering of probes without netnames and fixed the the Adaptor section header - AC
   //Version = "4.3.172";  // 02/20/04 - Spea_Out.cpp - Add the PACKAGE and PAD section to the output - LP
   //Version = "4.3.171";  // 02/20/04 - ckt_out.cpp & h - Added the new CKT writer (only for test and shouldn't show in the release build) - AC
   //Version = "4.3.170";  // 02/19/04 - Spea_Out.cpp - Modification and addition to the output file - LP
   //Version = "4.3.169";  // 02/19/04 - CASE #546 - GenCamIn.cpp - Fixed a runtime only crash in gcam_device() caused by a fprintf statement expecting 3 string parameters and being passed only 2 - DF
   //Version = "4.3.168";  // 02/18/04 - CASE #547 - PcbUtil.cpp - Fixed a crash in update_smdCOMPONENTData that referenced a NULL attribute map - DF
   //Version = "4.3.167";  // 02/18/04 - PcbUtil.cpp - Fixed a crash in several functions that reference an NULL attribute map - LP
   //Version = "4.3.166";  // 02/16/04 - Ipl_out.cpp - Added checking for probe names, full pin types, Polarized/tantalum ouput format- SN
   //Version = "4.3.165";  // 02/16/04 - Fixed problem with "Make Manufacturing Ref Des" - knv
   //Version = "4.3.164";  // 02/13/04 - Ipl_out.cpp - Fixed bugs in ipl_out and added subclassing - SN
   //Version = "4.3.163";  // 02/13/04 - DeviceTyper.cpp - Fixed bugs in Pin Mapping tab - SN
   //Version = "4.3.162";  // 02/11/04 - DeviceTyper.cpp - Fixed "Cancel" bug in Type tab & and added device types - SN
   //Version = "4.3.161";  // 02/11/04 - RealPart.cpp & Xml_wrt.cpp - Fixed get real part algorithm to swap special characters correctly - AC
   //Version = "4.3.160";  // 02/11/04 - Case #534 - GenerateCentroidAndOutline.cpp/.h - Body Outline and Pin Center is not working correctly - LP
   //Version = "4.3.159";  // 02/10/04 - Case #528 - Report.cpp - Added Number of Nets, SiglePinNets, and MultiPinNets to Manufacturing Report - DD
   //Version = "4.3.158";  // 02/10/04 - Case #526 - Edif200_In.cpp - Fixed text rotation, port rename, and overwrite inherited attributes - LP
   //Version = "4.3.157";  // 02/10/04 - Case #529 - Undo messages should be made more succinct - SN
   //Version = "4.3.156";  // 02/09/04 - Case #359, Case #364 - ODB++ Reader - apertures rotated incorrectly - knv
   //Version = "4.3.155";  // 02/06/04 - Case #524 - AllegIn.cpp - Added support for using the .BOARDOUTLINE command in the .IN file when reading fabmaster sections - AC
   //Version = "4.3.154";  // 02/06/04 - Case #522 - Huntrn_o.cpp - HAF Write is not outputing the pin surface correctly - LP
   //Version = "4.3.153";  // 02/04/04 - Case #379 - Bottom view shows pads with incorrect rotation - knv
   //Version = "4.3.152";  // 02/04/04 - Case #384 - Panelization Dialog Cancel button doesn't work - knv
   //Version = "4.3.151";  // 01/30/04 - Case #473 - Ipc_Out.cpp - Fixed net connections - SN
   //Version = "4.3.150";  // 01/30/04 - CAMCAD.cpp - Updated the registry update for camcad registry entries - AC
                           //          - LyrType.cpp - Fixed bug with setting a color set layer to black, need to to special case due to FlexGrid's (correct) behavior - AC
   //Version = "4.3.149";  // 01/27/04 - Case #514 - AllegOut.cpp - Allegro Writer was not writing lines to the autoshp.bat, added fflush statements at various locations to ensure buffers are written. - DF
                           //          - Case #515 - AllegOut.cpp - Added checks to the Allegro netlist writer to check a net object for the number of pins. if there are no pins in the net the netname is not written to the .net file. - DF
                           //          - Case #516 - AllegOut.cpp - Allegro Writer incorrectly identifying board instances of Mechanical Symbols as being inserts of Format symbols. Changed decision to be based on the block type. - DF
                           //          - Case #517 - AllegOut.cpp - Allegro Write added horizontal text justification to add_text and refdes and device labels. Includes normalization of vertical justification to bottom. - DF
   //Version = "4.3.148";  // 01/26/04 - Case #512 - MentorIn.cpp - Mentor Read did not correct net names of hierarchical net names in the POWER_NET_NAMES property. - DF
                           //          - Case #513 - MentorIn.cpp - Mentor Read added nets from the Traces file not found in the nets file and having no pins. Nets without pins cause problems. - DF
   //Version = "4.3.147";  // 01/26/04 - Case #511 - MentorIn.cpp - Mentor Boardstation reader creating layer as floating layers in do_arc() function - LP
   //Version = "4.3.146";  // 01/26/04 - Case #507 - AllegIn.cpp - CAMCAD crashed when reading Allegro file - LP
   //Version = "4.3.145";  // 01/23/04 - Case #505 - Graph.cpp - The function PurgeUnusedBlocks reference a non-existing block - LP
   //Version = "4.3.144";  // 01/22/04 - Case #503 - Added call to verifyAndRepairData() after importing files. - knv      
   //Version = "4.3.143";  // 01/21/04 - Case #503 - CAMCAD crashes when layers dialog "Used on" pressed - knv
                           //          - Case #502 - MentorIn.cpp - Arc was not inserted on an unitialized layer - LP
   //Version = "4.3.142";  // 01/21/04 - CAMCAD.cpp - Updated the registry setting to put the CWD under the generic CAMCAD/Settings key - AC
   //Version = "4.3.141";  // 01/20/04 - Case #351 - ODB Read Issues - Attributes - knv
   //Version = "4.3.140";  // 01/20/04 - sch_link.cpp - Fixed bug of assuming a schematic net being added has and etch on the current sheet shown - AC
   //Version = "4.3.139";  // 01/16/04 - Case #501 - MentorIn.cpp - Mentor Boardstation reader, removing layer without removing all insert on the layer - LP
   //Version = "4.3.138";  // 01/16/04 - Case #493 - MentorIn.cpp - Mentor Boardstation reader, complex should be created with insert on floating layer instead of layer specific - LP 
   //Version = "4.3.137";  // 01/16/04 - Case #444 - unicamin.cpp & unicam_o.cpp - Fixed proper support for cLocation on placement of parts as the centroid of the pins - AC
   //Version = "4.3.136";  // 01/16/04 - Case #413 - ThedaIn.cpp - Theda reader, put attribute REFNAME on the insert instead of on the geometry when it is defined in the PACKAGE of COMPONENT - LP
   //Version = "4.3.135";  // 01/16/04 - Case #498 - Pads_out.cpp - Pads writer to handle better the array of layers when a hole in the array (layer == NULL) is encountered - AC
   //Version = "4.3.134";  // 01/13/04 - Fixed net compare algorithm to take into account the 1 to many relationship from PCB to SCH - AC
   //Version = "4.3.133";  // 01/12/04 - Case #490 - fixed crash when looking for a component found in the netlist not inserteed into a design - AC
   //Version = "4.3.132";  // 01/12/04 - Added Machine Feeder parsing to Agilent SJ PLX reader and corresponding attributes to centroids and 
                           //            components when using the "Gerber/Centroid Merge Tool" - knv
   //Version = "4.3.131";  // 01/12/04 - Case #489 - Fixed bug in SchLink that didn't take into account etches on different sheets when cross probing - AC
   //Version = "4.3.130";  // 01/12/04 - Beta test version 7 of Gerber Educator feature - knv
   //Version = "4.3.129";  // 01/09/04 - Beta test version 6 of Gerber Educator feature - knv
   //Version = "4.3.128";  // 01/08/04 - Case #338 - dfm_dist.cpp - Updated distance check to not violate etches that are connected to a component via the netname - AC
   //Version = "4.3.127";  // 01/06/04 - DeviceTyper.cpp - Device Type Style Updated - SN
   //Version = "4.3.126";  // 01/07/04 - Case #487 - MneutIn.cpp - Mentor Neutral crash due to invalid reference - LP
   //Version = "4.3.125";  // 01/06/04 - For the following cases - Open_Cad.cpp, MentorIn.cpp - LP
   //                                  - Case #472 - If the selected file is a Mentor Nuetral file, then search the directory for the WIRE file with the latest date
   //                                  - Case #475 - If the selected file is a Mentor Boardstation file, then search the directory for all the other five files with the latest date
   //                                  - Case #476 - If the selected file is a WIRE file, then search the directory first for Mentor Boardstation file.  If no Mentor Boardstation file then search for Mentor Neutral file
   //Version = "4.3.124";  // 01/06/04 - DeviceTyper.cpp - Device Type Style Updated - SN
   //Version = "4.3.123";  // 01/05/04 - DeviceTyper.cpp - Device Type Style Updated, Ability to Store Configurations to a file added - SN
   //Version = "4.3.122";  // 01/05/04 - Case #482 - OrcltdIn.cpp/.h - Inserts need to reference correct device type and unused device type need to be removed - LP
   //Version = "4.3.121";  // 12/30/03 - Case #478 - PcbUtil.cpp - In the function OptimizeComplexApertures_Blocks() the optimized aperture didn't take into consideration the ration of the complex aperture - LP
   //Version = "4.3.120";  // 12/30/03 - Case #477 - AllegIn.cpp - Allegro Read, duplicate pins are inserted for PINEDITED geometries - LP
   //Version = "4.3.119";  // 12/30/03 - DeviceTyper.cpp/.h, Resources - Bugs fixed in Style tab in device typer - SN.
   //Version = "4.3.118";  // 12/30/03 - Case #474 - AllegIn.cpp - Allegro Read, delta angle of arc is miss calculated when system mirror is true - LP
   //Version = "4.3.117";  // 12/29/03 - DeviceTyper.cpp/.h, Resources - Style tab in device typer updated - SN.
   //Version = "4.3.116";  // 12/26/03 - DeviceTyper.cpp/.h, Resources - Added new Style tab to Device Typer - SN.
   //Version = "4.3.115";  // 12/23/03 - Case #421 - StencilDialog.cpp/.h, TreeListCtrl.cpp/.h - Fixed crash in stencil generator - SN.
   //Version = "4.3.114";  // 12/19/03 - Sch_Link.cpp & .h - Fixed schematic link building up of sorted array of components and comp/pins (not sorts correctly) - AC
                           //             - Undo.cpp - Fixed FreeUndoLevel to make sure there is a valid list to free
   //Version = "4.3.113";  // 12/19/03 - Case #485 - Ipl_out.cpp - IPL Z1800 crashed due to referencing invalid channel - LP
   //Version = "4.3.112";  // 12/17/03 - Case #452 - Ccdoc.cpp/.h, CAMCAD.RC/resource.h - Added a new menu item "Convert Via to PCB Component" under Tool->PCB - LP
   //Version = "4.3.111";  // 12/16/03 - Case #439 - CAMCAD.cpp - Fixed error when no proxy stub was registers in the registry (added updateInterfaceRegistry) - AC
                           //             - RealPart.cpp - Modified GetRealPartData to correctly swap out special characters in values
                           //             - CAMCAD.cpp - Fixed function to update Current User with the Directory and Product values from Local Machine correctly
   //Version = "4.3.110";  // 12/16/03 - Case #479 - Huntrn_o.cpp - Output of comppin surface didn't take into consideration of surface specific padstack - LP
   //Version = "4.3.109";  // 12/16/03 - Changed license string for "Agilent SJ PLX Centroid Read"  - knv
   //Version = "4.3.108";  // 12/16/03 - Case #453 - Hp3070.cpp - Added the command ".SUPPRESS_PARTNUMBERandMESSAGE" to suppress the output of partnumber and message - LP
   //Version = "4.3.107";  // 12/15/03 - Added "Gerber/Centroid Merge Tool" from "Gerber Educator" feature  - knv
   //Version = "4.3.106";  // 12/15/03 - Case #444 - UnicamIn.cpp - Added the UseCLocation switch - SN
   //Version = "4.3.105";  // 12/15/03 - Refix Case #414 - GenCadIn.cpp - Changed command ".REFRESH_TYPETOCOMP_ATTRIBUTES" to ".REFRESH_DEVICETOCOMP_ATTRIBUTES" - LP
   //Version = "4.3.104";  // 12/15/03 - Case #414 - GenCadIn.cpp - Added the command ".REFRESH_TYPETOCOMP_ATTRIBUTES" to refresh attribute from device type - LP
   //Version = "4.3.103";  // 12/12/03 - Beta test version 5 of Gerber Educator feature - knv
   //Version = "4.3.102";  // 12/12/03 - Case #413 - ThedaIn.cpp - Theda Read, added support of true text justification - LP
   //Version = "4.3.101";  // 12/12/03 - Case #460 - Vb99In.cpp - VBAscii Reader, corrected refdes rotation and the correction work for Case #391 also - LP 
   //Version = "4.3.100";  // 12/11/03 - Beta test version 4 of Gerber Educator feature - knv
   //Version = "4.3.99";   // 12/09/03 - Case #462 - DdeIn.cpp - Modified .txt entry parsing for DDE v8 and prior to add support for justified text. - DF
                           // 12/09/03 - Case #462 - Pads_Out.cpp, PadsLib.cpp, PadsLib.h - Added Pads Write support for Justified Text. - DF
                           // 12/09/03 - Case #462 - PadsIn.cpp - Added Pads Read support for Justified Text, deficiency found testing output from Pads Write. - DF
   //Version = "4.3.98";   // 12/08/03 - Case #470 - Layer radio buttons are being set spuriousely. - knv
   //Version = "4.3.97";   // 12/05/03 - Case #458 - dfm_flat.cpp - Fixed flatten routine to flatten stuff on floating layers (optimization using marked flag on blocks in FlattenBlockByInserttype) - AC
                           //             - dfm_panl.cpp - Fixed ItemInCorners API to correctly calculate corner based on parameters and correctly check if an item is contained with the corners
   //Version = "4.3.96";   // 12/05/03 - Case #468 - draw_ent.cpp - Fixed text location calculations when the text is mirrored draw routine - AC
                           //             - search.cpp - fixed text location calculates when text is mirror for the search routine
   //Version = "4.3.95";   // 12/04/03 - Case #401 - Added map structure to catalog violations in order to make sure there are never same violations (by entity number) - AC
   //Version = "4.3.94";   // 12/04/03 - Case #454 - Mark by window not marking - knv
   //Version = "4.3.93";   // 12/04/03 - Case #457 - Accel file not reading becasue the reading is checking for few specific PROGRAM value - LP
   //Version = "4.3.92";   // 12/04/03 - Case #466 - Mark by window not working - knv
   //Version = "4.3.91";   // 12/04/03 - Case #415 - ExceIn.cpp - Format_Inch .in command - SN
   //Version = "4.3.90";   // 12/03/03 - Case #427 - dfm_misc.cpp - Etch blockers were created with a width index of -1 (this is invalid so changed to 0) - AC
                           //             - fixed testpad density test to not assume the pcb is visible - knv
   //Version = "4.3.89";   // 12/03/03 - Case #455 part 3 - Deleting by shape does not correctly convert matched pads to unmatched pads - knv
                           // 12/03/03 - Case #459 - Mark by window from the Gerber Educator doesn't work correctly - knv
   //Version = "4.3.88";   // 12/03/03 - Case #463 - DDEIN.CPP - Added support for mirrored SMD pin instances in ipl_wlp(). - DF
   //Version = "4.3.87";   // 12/03/03 - Case #447 - Spea writer makes "qd.log" instead of "spea.log" - LP
   //Version = "4.3.86";   // 12/02/03 - CASE# 450 - Pads_Out.cpp - Took out Placement Keepout lines from DECALS section. - DF
   //Version = "4.3.85";   // 12/01/03 - Added Kelvin Analysis (CAMCAD 4.3 SP1 Release) - AC
   //Version = "4.3.84";   // 12/01/03 - Case # 166 - Suppressed ShrinkPoly Error Messages - DD
   //Version = "4.3.83";   // 12/01/03 - Removed XMLFailed flag completely - AC
   //Version = "4.3.82";   // 11/26/03 - FileType.cpp - Made new IPL writer module visible in release builds - AC
   //Version = "4.3.81";   // 11/26/03 - Zollner request ZR2 - AccelIn.cpp, AccelIn.h - Added FIDUCIAL_GEOM and TOOLING_GEOM commands to IN file and code to change named geoms and their inserts to type fiducial or tooling respectivly. - DF
   //Version = "4.3.80";   // 11/25/03 - Case #434 - Rebug - OrcltdIn.cpp - Mirrored padstack is not created correctly - knv
   //Version = "4.3.79";   // 11/24/03 - Case #448 - When exporting a file with the CCM exporter, CAMCAD crashes. - knv
   //Version = "4.3.78";   // 11/24/03 - Added new command line argument to copy itself into a testfix directory on M based on current version - AC
   //Version = "4.3.77";   // 11/21/03 - First cut of Kelvin Analysis - AC
                           //             - IPL updates (APPLY_TESTPIN_REMAPPING option and writing newly generated channel numbers back to the database)
   //Version = "4.3.76";   // 11/21/03 - Case #440 - Spea_Out.cpp - Fixed the following four issue - LP
                           //             - X & Y location is output incorrectly
                           //             - The field for place or not placed component is output incorrectly
                           //             - Get value for PNData section from VALUE_NORMALIZED attribute instead of VALUE - LP
                           //             - Board info header is missing Measure Unit value
                           //             - Added a command ".EXPORT_VIAS" in ".out" file to suppress the output of all vias if set to N
                           // 11/21/03 - Case #434 - OrcltdIn.cpp - Mirrored padstack is not created correctly - knv
                           // 11/21/03 - Case #420 - Stencil tool set to NONE for a certain component, still gets pads generated - knv
   //Version = "4.3.75";   // 11/20/03 - Case #390 - AllegIn.cpp - Some PCB component is not typed as properly - LP
   //Version = "4.3.74";   // 11/20/03 - Case #435 - Sch_Lib.cpp - CAMCAD Crashed when try to do Schematic->Select Attribute when no file is loaded - LP
   //Version = "4.3.73";   // 11/20/03 - Case #426 - DxfLib.cpp - DXF Writer crashed during export - LP
                           // 11/19/03 - Case #438 - Polys found on two different layers are being merged into the same complex flash - knv
                           // 11/19/03 - Case #424 - When reading a Alcatel Docica file into the latest CAMCAD, it crashes!! - knv
                           // 11/19/03 - Case #359 - ODB READ ISSUE - Specifically some poly lines are being defined as ETCH, when they should not be defined as ETCH. - knv
   //Version = "4.3.72";   // 11/19/03 - xml_read.cpp - Set the project global variable XMLFailed was FALSE in XMLReader::parse() and only set to TRUE when it is parsed is not successfully - LP
   //Version = "4.3.71";   // 11/18/03 - Beta test version 3 of Gerber Educator feature - knv
   //Version = "4.3.70";   // 11/17/03 - Case #408 - ThedaIn.cpp - Added support of FILLED_SMOOTH_POLYGON with a number radius for RADIUS_TYPE - LP
   //Version = "4.3.69";   // 11/17/03 - Beta test version 2 of Gerber Educator feature - knv
   //Version = "4.3.68";   // 11/17/03 - ipl_out.cpp - Use part number for dexcription field, if it exists - AC
                           //             - added support for ComponentValue class to recognize OHM
                           //             - added "(Release Test)" to application title for release tests to ensure its not mistaken for a release build
                           //             - added support for the visible name on the export list dialog
   //Version = "4.3.67";   // 11/17/03 - Refix Case #430 - Ag_AOI_o.cpp - Changed error message for Mix Mode to "No Unload Components found. No SP lines created" - LP
   //Version = "4.3.66";   // 11/14/03 - Refix Case #430 - Ag_AOI_o.cpp - When there is more than one pad on the same PASTE layer, only output one - LP
   //Version = "4.3.65";   // 11/14/03 - Added new GetNetCompPinData3 API - AC
                           //             - ipl_out.cpp & .h  - added support for new CAP_DISCHARGE_MIN option and fix diode lines
   //Version = "4.3.64";   // 11/14/03 - Beta test version 1 of Gerber Educator feature - knv
   //Version = "4.3.63";   // 11/13/03 - Added CompValues.h & .cpp to project - AC
                           //             - Added ComponentValues class to aid in normalization of values (ie Ohms, Farads, M, K, M, ...)
                           //             - Moved component value unit #define's from dbutil.h to an enum in CompValues.h
                           //             - Move unit_values array to CompValues.cpp (no longer global)
                           //             - fixed NormalizeValuesDialog to set doc as a member variable and to not create multiple dialog instances
                           //             - modifed multiple files to use new ComponentValues class
   //Version = "4.3.62";   // 11/13/03 - Case #430 - Ag_AOI_o.cpp - Pin data that should be written to the BOTTOM file is written to the TOP file - LP
   //Version = "4.3.61";   // 11/13/03 - Case #429 - GerbIn.cpp - Some apertures are inserted twice - LP
   //Version = "4.3.60";   // 11/07/03 - LyrType.cpp - Fixed problem with check boxes in the flex grid (returned value must be casted to a short) (WIN2000 or earlier) - AC
                           //             - Fixed DRC, FileType and OCB directory dialog when adding IDC_STATIC controls to the resize list (WIN98 - maybe earlier as well)
   //Version = "4.3.59";   // 11/07/03 - Refix Case #243 - VBAscii write, "PIN_OPTIONS" is not write correctly - LP
   //Version = "4.3.58";   // 11/06/03 - Changed the 18xx IPL writer to use ipl_18xx.out instead of ipl.out - AC
   //Version = "4.3.57";   // 11/06/03 - Case #411 - Theda Read, when there is not TEXTPEN specified, use the first TEXTPEN defined - LP
   //Version = "4.3.57";   // 11/06/03 - Case #411 - Theda Read, when there is not TEXTPEN specified, use the first TEXTPEN defined - LP
   //Version = "4.3.56";   // 11/06/03 - LyrType.cpp - The function OnTopview() and OnBottomview() should call OnFitPageToImage() instead of OnRegen() - LP
   //Version = "4.3.55";   // 11/06/03 - Case #409 - Theda Read, added two commands ".PROBE_SIZE" and ".DRILL_SIZE" - LP
   //Version = "4.3.54";   // 11/06/03 - Lyrtype.cpp, LyrGroup.cpp, LAYER.cpp, version.cpp, LyrStackup.cpp, Layerlst.cpp - Change call from OnFitPageToImage() to OnRegen() - LP
   //Version = "4.3.53";   // 11/06/03 - Case #417 - Ccm_Out.cpp - Missing the data for sectin "pcbgeometries" - LP
   //Version = "4.3.52";   // 11/06/03 - Refix Case #410 - Edif200_In.cpp - Port designator is missing - LP
   //Version = "4.3.51";   // 11/05/03 - DbUtil.cpp - Updated layertype[] in DbUtil.cpp to support groups of layer types - AC
   //Version = "4.3.50";   // 11/05/03 - Mapped new colorset dialog to toolbar button - AC
                           //             - replaced old color set dialog
   //Version = "4.3.49";   // 11/05/03 - Case #410 - Edif200_In.cpp/.h - Added multiple text and attribute displays - LP
   //Version = "4.3.48";   // 11/03/03 - Licensed 18xx IPL writer (not in Release build) for testing - AC
                           //             - moved logic to not include readers and writer license in FileType.cpp in the dialog
   //Version = "4.3.47";   // 11/03/03 - Case #393 - Theda Reader does not process vias correctly - knv
   //Version = "4.3.46";   // 10/31/03 - Case #405 - filter.cpp, add.cpp - Redraw leave previous highlight during dragging geometry to be added acrros the board with "Snap To Center" turned on - LP
   //Version = "4.3.45";   // 10/30/03 - removed top level selection on Geom for Device typer Pin Mapping, MN
   //Version = "4.3.44";   // 10/30/03 - Fixed crash in Device Typer pin mapping, MN
   //Version = "4.3.43";   // 10/15/03 - Lyrtype.cpp, LryGroup.cpp, LyrStackup.cpp, and Layerlst.cpp
                           //             - Added the call to OnFitPageToImage() to recalculate the page extent - LP
   //Version = "4.3.42";   // 10/29/03 - Edif200_In.cpp & Edif300_In.cpp - Create layers for attribute using "AttrLyr_" + attribute name - LP
   //Version = "4.3.41";   // 10/29/03 - Case #403 - Unicam_o.cpp/.h - Unicam Write, unit factor and insert pin rotatio were calculated incorrectly - LP
   //Version = "4.3.40";   // 10/28/03 - Case #390 - AllegIn.cpp - Allegro Read, multiple insert for the same refdes and missing silkscreen outline and via fan out - LP
   //Version = "4.3.39";   // 10/24/03 - Added licensing for Stencil Generator - knv
   //Version = "4.3.38";   // 10/24/03 - Case #383 - Pads_Out.cpp - Pads Writer, added the command ".COLOR_ALL_LAYERS" to output all layer color when set to 'Y' - LP
   //Version = "4.3.37";   // 10/23/03 - Refix Case #395 - AllegOut.cpp - Allegro Writer, should only write padshape to AutoShape file once - LP
   //Version = "4.3.36";   // 10/23/03 - Case #374 - CAMCAD Window not maximized - knv
   //Version = "4.3.35";   // 10/23/03 - Case #396 - AllegOut.cpp - Allegro Writer, modified to output multiple line text as individual line of text - LP
                           //          - Refix Case #391 - Vb99In.cpp - VBAscii Read, refdes rotation - LP
   //Version = "4.3.34";   // 10/23/03 - Case #371 - Panelization.cpp - fixed bug when verifying panel with the board where the origin isn't the bottom left - AC
                           //             Updated the project file to build CAMCAD.exe for the release build
                           //             Adding flags to include the Teradyne IPL writer in the Rel Test build
                           //             Added more fixes to the IPL writer
   //Version = "4.3.33";   // 10/23/03 - Case #395 - AllegOut.cpp - Allegro Writer, no grapic complex padshape should still be output with a default 2X2 square - LP
   //Version = "4.3.32";   // 10/23/03 - Case #391 - Vb99In.cpp - VBAscii Read, refdes rotation was wrong - LP
   //Version = "4.3.31";   // 10/23/03 - Refix Case #344 - Mneutin.cpp - Need to correctly type the layer - LP
   //Version = "4.3.30";   // 10/22/03 - Case #392 - Mneutin.cpp - Fixed rotation when board insert is mirrored - LP
   //Version = "4.3.29";   // 10/22/03 - Case #344 - Mneutin.cpp/.h - Mentor Neutral - Modified to correcly handle mirror layers - LP
   //Version = "4.3.28";   // 10/22/03 - Sch_link.cpp - Modified linking to include all nets and etches (filtering out "~Unused Pins~") - AC
                           //             Saving of cross reference attribute is now found on net, not etches
   //Version = "4.3.27";   // 10/22/03 - Crash reported by Kevin Decker, no case was assigned, when clicking on text with Pin Filter, a crash appeared - M.N.
   //Version = "4.3.26";   // 10/21/03 - Case #345 - Edif200_In.cpp - Edif 200 Read - LP
                           //             1) record to log file when instance name is not unique
                           //             2) rename the property "Designator" to "Property_Designtor" to avoid conflict with the really "DESIGNATOR" 
   //Version = "4.3.25";   // 10/20/03 - Beta test version 5 of Stencil Generator feature - knv
   //Version = "4.3.24";   // 10/17/03 - Case #399 - AccelIn.cpp - Accel Reader added ".CONVERT_FREEPAD" command to allow convertion of freepad to component or via - LP
   //Version = "4.3.23";   // 10/17/03 - Beta test version 4 of Stencil Generator feature - knv
   //Version = "4.3.22";   // 10/17/03 - Case #394 - Mem.cpp - FreeFile() function should also free all the blocks that are typed to this file number - LP
   //Version = "4.3.21";   // 10/16/03 - Case #398 - Panelization.cpp - Fixed crash and calculation errors in board array placement - AC
   //Version = "4.3.20";   // 10/16/03 - Case #381 & 397 - Hp3070_o.cpp - Some components are not being output to the "OTHER" section in *.bxy file - LP
   //Version = "4.3.19";   // 10/15/03 - Case #375 - GerbIn.cpp - Thermal macro is not correctly read - LP
   //Version = "4.3.18";   // 10/15/03 - Case #363 - GerOut.cpp - Modified to output arc as small segment of line - LP
   //Version = "4.3.17";   // 10/15/03 - Case #346 - Layer.cpp, Settings.cpp - "Fit Page to Image" in setting dialog and clicking "Orignal Color Set" is not working correctly - LP
   //Version = "4.3.16";   // 10/14/03 - Case #360 - AccelIn.cpp - Modified the function attr_pt() and tan_unit() to correct parse numeric value that has a unit follows - LP
   //Version = "4.3.15";   // 10/14/03 - Case #341 - Gerb_Mrg.cpp/ch - Added the function isInsertBottom() to inherite attribute from insert on the correct into Gerber flash - LP
   //Version = "4.3.14";   // 10/13/03 - Case #343 - ExceIn.cpp - Tool size waw being read incorrectly - LP
   //Version = "4.3.13";   // 10/13/03 - Case #387 - MentorIn.cpp - Added support of multiple ".FILTER_GRAPHIC_CLASS" and ".FILTER_INSERT_TYPE" command - LP
   //Version = "4.3.12";   // 10/13/03 - Case #382 - CAMCAD crash when clicking "Stencil Generator", but Kurt fixed it - LP
   //Version = "4.3.11";   // 10/10/03 - Case #377 - Edit.cpp/.h - CAMCAD crash when clicking "xxx more polys..." in Edit Poly Item dialog.  Modified to continue loading the next 100 poly when the use click "NEXT 100 Polys..." until there is not more poly - LP
   //Version = "4.3.10";   // 10/10/03 - Case #380 - Ag_AOI_o.cpp - CAMCAD crash during Agilent AOI Write - LP
   //Version = "4.3.9";    // 10/10/03 - Refix Case #132, 292, 333, 350 - Change the implementation not to use a SYM that is never use by any COMP because it is missing COMPOUTLINE and thing like that - LP
   //Version = "4.3.8";    // 10/10/03 - Refix Case #132, 292, 333, 350 - Change the implementation to build SYM as it is define in the Orcad file, and then do a post process to change X-coordinate if it is mirror - LP
   //Version = "4.3.7";    // 10/09/03 - Refix Case #132, 292, 333, 350 - Added a lot of code to build a correct geometry for SYM that is mirrored - LP
   //Version = "4.3.6";    // 10/08/03 - PfWin.cpp - Protel Reader, IsPinLocOrNameDiff() to take into consider of mirroring of the geometry for the pin - LP
   //Version = "4.3.5";    // 10/08/03 - Case #292 - OrcltdInd.cpp/.h - Orcad Reader, removed the command ".USE_DERIVEDSYM" and only create instance specific geometry if it's pin location or name is differenct from the original geometry - LP
                           //          - PfWin.cpp - Protel Reader, modified the function CheckPinLocOrNameDiff() and IsPinLocOrNameDiff() - LP
   //Version = "4.3.4";    // 10/07/03 - Beta test version 3 of Stencil Generator feature - knv
   //Version = "4.3.3";    // 10/03/03 - Case #369 - MentorIn.cpp - Need to support the default layer mirroring pair when no layer file is loaded - LP
   //Version = "4.3.2";    // 10/03/03 - Case #365 - QD_Out.cpp - CAMCAD halt when exporting Siemens QD output - LP
   //Version = "4.3.1";    // 10/02/03 - Case #132, #333, and #350 - Components were incorrectly being created as a mirror copies due to unintialized variable - LP
                           //             - MentorIn.cpp - Moved the function FilterGraphicClass() and FilterInsertType() so that it only get called when the last Mentor file is read - LP
   //Version = "4.3.0";    // 10/01/03 - Offical Release Build of CAMCAD 4.3.0 - LP
                           //          - Refix case #362 - Remove the dialog that warn the use of unsupport verion, instead output the message to log file - LP
   //Version = "4.3.0iy";  // 10/01/03 - Case #362 - CadifIn.h - Cadif Reader, MAX_LINE of 3000 was not enough, changed to 30000 - LP
   //Version = "4.3.0ix";  // 10/01/03 - Case #356 - Pads_Out.cpp/.h - PADS Writer, fixed how via padstack is being evaluated for layer span - LP
                           //          - Layer.cpp/.h - Added the function GetMaxElectricalStackNum to return the max eletrical stack number - LP
   //Version = "4.3.0iw";  // 09/30/03 - Changed the visible name of Edif 200..400 Read to Edif 200..300 Read - AC
                           //          - Changed Spea 4040 to need a license (removed default property)
                           //          - Remove new DFT menu options and set flag to remove new DFT implementations
   //Version = "4.3.0iv";  // 09/30/03 - Case 352 - BlockDlg.cpp - Needed to add the new edit file to the file list before setting the filenum (adding to array sets it) - AC
                           //          - Case 353 - Cedit.cpp - Resetting of extents when the editted geometry is global (filenum = -1)
   //Version = "4.3.0iu";  // 09/29/03 - Added copy constructor to Point2.  Replaced memcpy of CPnt and Point2 with copy constructors - knv
   //Version = "4.3.0it";  // 09/29/03 - Added copy constructor and operator= to SelectStruct.  Replaced memcpy of SelectStruct with new methods - knv
   //Version = "4.3.0is";  // 09/26/03 - RwLib.cpp & .h - Added GetFirstPosition to CMapStringToWord for ability to iterate through the map - AC
                           //          - Ipl_out.cpp & .h - Added more functionality
                           //          - Find.cpp & .h - Changed FindEntity to use an enum for the entityType parameter
                           //          - Spea_Out.cpp - Updated to work with new FindEntity
   //Version = "4.3.0ir";  // 09/25/03 - Version = "4.2.138"
   //Version = "4.3.0iq";  // 09/25/03 - Added new IPL writer (no body yet) - AC
   //Version = "4.3.0ip";  // 09/25/03 - Case #347 - CamCad.cpp - Updated Schematic Link call to preprocess and saved cross ref names first - AC
   //Version = "4.3.0io";  // 09/25/03 - Version = "4.2.137"
   //Version = "4.3.0in";  // 09/24/03 - Panelization.cpp - Updated the center and origin coordinates when the apply button is pressed in the board edit dialog - AC
                           //          - SchLink.cpp - added new function to allow for applying XRef attributes w/o having to go through the linking dialogs (not using it yet)
   //Version = "4.3.0im";  // 09/24/03 - Spea_Out.cpp - Modified the following in the writer - LP
                           //          - PARTLIST section will output component w/o partnumber as a commented data line
                           //          - TEST POINT section will drill down a test probe to the component level and also corrected CONTACT TYPE CODE
   //Version = "4.3.0il";  // 09/24/03 - Value Normalization added - SN
   //Version = "4.3.0ik";  // 09/23/03 - New build including Device Typer updates, Paneliation, and Stecil - SN
   //Version = "4.3.0ij";  // 09/23/03 - Version 2 of new panelization - AC
                           //          1) Delete one board out of the panel, then select a second board and CAMCAD crashes.
                           //          2) Placement, rotation or flip by center does not work correctly.
                           //          3) We need an apply button in the edit board dialog so you can see your changes without exiting the dialog.
                           //          4) When you exit Panelization with the OK button, the panel you were working on should stay the active file.
                           //          5) If you start Panelization with a panel as the active file, that panel should be loaded into the dialog.
                           //          6) If there is only one board in the CC file then that board should be automatically selected in the board drop down list.
                           //          7) Disable the Cancel.
                           //          8) Remember if a board was modified by origin or by center
                           //          Updated the extents contains function to use SMALLNUMBER as a fudge factor
   //Version = "4.3.0ii";  // 09/22/03 - Version 1 of new panelization - AC
   //Version = "4.3.0ih";  // 09/22/03 - Beta test version 2 of Stencil Generator feature - knv
   //Version = "4.3.0ig";  // 09/21/03 - Beta test version of Stencil Generator feature - knv
   //Version = "4.3.0if";  // 09/19/03 - Spea_Out.cpp/.h - Modified the function WritePartNumberData() to output each PartNumber only once - LP
   //Version = "4.3.0ie";  // 09/19/03 - DeviceTyper.cpp/.h - Added "Generate Device Type" module and dialog - SN
   //Version = "4.3.0id";  // 09/19/03 - Lic.cpp/.h - Added licensing for SPEA 4040 Writer - Lp
   //Version = "4.3.0ic";  // 09/19/03 - Sch_Lib.cpp - Modified the function RenameRefnameAndDesignatorAndNetList() so that it won't attach a number to make the "Designator" unique anymore - LP
                           //          - Spea_Out.cpp/.h - Added a new writer SPEA 4040 Write and ready for testing - LP                           
   //Version = "4.3.0ib";  // 09/17/03 - TestPadDensityDfm.cpp - Fixed data loop to check if the AttribMap pointer is valid - AC
   //Version = "4.3.0ia";  // 09/16/03 - Version = "4.2.136"
   //Version = "4.3.0hz";  // 09/15/03 - CCDoc.h, File.cpp - Added Add_File to CCEtoODBDoc class - AC
                           //          - File.h - added nextFileNum to CFileList (didn't remove from Graph.cpp because of extern declarations in other files [api.cpp, datafile.cpp, XmlRealPartContent.cpp, xml_cont.cpp])
                           //          - Graph.h - modified Graph_File_Start to use Add_File
   //Version = "4.3.0hy";  // 09/15/03 - Added RealPart support for unasigning parts - AC
   //Version = "4.3.0hx";  // 09/12/03 - Added operator=() to  DataStruct,TextStruct,InsertStruct,BlobStruct,CPolyList - knv
   //Version = "4.3.0hw";  // 09/12/03 - Report.cpp - Added new report THT Pins - AC
   //Version = "4.3.0hv";  // 09/12/03 - OrcdltdIn.cpp - Ordad in: pads misplplaced and not offsetted correctly - SN
   //Version = "4.3.0hu";  // 09/11/03 - Case #348 - dbutil.cpp - CAMCAD crashed due to missing layer name "STENCIL TOP" and "STENCIL BOTTOM in layertypes[] - LP
   //Version = "4.3.0ht";  // 09/11/03 - Case #334 - GerOut.cpp - Gerber Write is incorrectly outputing complex aperture - LP
   //Version = "4.3.0hs";  // 09/11/03 - Changed CDataList to CTypedPtrListContainer and updated constructor calls  - knv
   //Version = "4.3.0hr";  // 09/11/03 - Removed memcpy of DataStruct,TextStruct,InsertStruct,BlobStruct,CPolyList  - knv
                           //          - Added isValid() members to several structs
   //Version = "4.3.0hq";  // 09/10/03 - Fixed CAttribute deletions, etc. - knv
   //Version = "4.3.0hp";  // 09/09/03 - updated TextStruct text member and handling, added destructor - knv
                           //          - updated InsertStruct refName member and handling, added destructor
                           //          - updated BlobStruct bitmap member and handling, added destructor
                           //          - updated DataStruct, added destructor
   //Version = "4.3.0ho";  // 09/09/03 - Case #253 - fixed search routine to use line and text alignment w/ rotations for text and visible attributes - AC
   //Version = "4.3.0hn";  // 09/09/03 - fixed extents algorithm to take into account multiple files by adding on to the extents instance - AC
   //Version = "4.3.0hm";  // 09/09/03 - Version = "4.2.135" - LP
   //Version = "4.3.0hl";  // 09/09/03 - reverted updating of panning when at 1 to 1 (4.3.0go) - AC
   //Version = "4.3.0hk";  // 09/09/03 - Case #335 - CAMCAD crash when reading PADS - LP
   //Version = "4.3.0hj";  // 09/08/03 - Fixed bug where insert type buttons for Test Probe and Drill Symbol - AC
   //Version = "4.3.0hi";  // 09/05/03 - added Toolstack functions to insert DFT tool stacks into a design - AC
   //Version = "4.3.0hh";  // 09/04/03 - CAMCAD.cpp - CAMCAD now looks in Loca Machine to get the CWD - AC
   //Version = "4.3.0hg";  // 09/04/03 - Case #310 - Enhanced Generate Centroid Dialog, added *.cen load/save - SN
   //Version = "4.3.0hf";  // 08/28/03 - Created save functions that return boolean values - AC
                           // 08/28/03 - Create HasData funtion that check is blocks are loaded
   //Version = "4.3.0he";  // 08/28/03 - Implemented hidden keywords - knv
   //Version = "4.3.0hd";  // 08/28/03 - Case #142 - PcbUtil.cpp - Re-wrote algorithm to take into acount 2 rows and/or 2 columns of pins - AC
   //Version = "4.3.0hc";  // 08/28/03 - Case #324 - Cadif Read, added ".COMPHEIGHT" command in ".in" file - LP
   //Version = "4.3.0hb";  // 08/28/03 - Added hidden flag to KeyWordStruct - knv
   //Version = "4.3.0ha";  // 08/28/03 - Case #325 - Gerber Read is not creating polygon correctly - LP
   //Version = "4.3.0gz";  // 08/27/03 - Case #322 - Protel PFW Read, the rountine to check for modified pin location is wrong; therefore, component geometries are not created correctly - LP
   //Version = "4.3.0gy";  // 08/27/03 - Added RealPart package and device assignmnent licenses - AC
   //Version = "4.3.0gx";  // 08/26/03 - Version = "4.2.134" - AC
   //Version = "4.3.0gw";  // 08/25/03 - Version = "4.2.133" - AC
   //Version = "4.3.0gv";  // 08/25/03 - Changed references to attribute maps from type CMapWordToPtr to CAttributes - knv
   //Version = "4.3.0gu";  // 08/22/03 - Version = "4.2.132" - AC
   //Version = "4.3.0gt";  // 08/21/03 - Version = "4.2.131" - added on 08/22/03 - LP
   //Version = "4.3.0gs";  // 08/21/03 - Updated SetRealPartData to set the layer type of the newly created layers - AC
                           // 08/21/03 - Version = "4.2.130" - LP
   //Version = "4.3.0gr";  // 08/21/03 - Case #321 - MentorIn.cpp - When attribute keyword is "COMPONENT_PLACMENT_OUTLINE", then place on "PLACE_1" or PLACE_2" or both -  LP
   //Version = "4.3.0gq";  // 08/21/03 - Edif300_In.cpp - Fixed the miss use of the variable name "sDefaultSetting" in figureGroup() and figureGroupOverride() - LP
   //Version = "4.3.0gp";  // 08/20/03 - Case #289 - CAMCAD.RC, resource.h, gerberpcb.cpp/.h - Added implementation to convert all type of apertures except COMPLEX, UNDEFINED, and BLANK - LP
   //Version = "4.3.0go";  // 08/20/03 - Updated panning to not redraw when current view is 1 to 1 - AC
                           //          - fixed bug panning to refdes when design is rotated at bottom view
                           //          - Allow selection when panning to refdes w/o a full redraw
   //Version = "4.3.0gn";  // 08/20/03 - Changed FlattenByAttrib to FlattenInsertByAttrib - AC
   //Version = "4.3.0gm";  // 08/20/03 - Case #256 - Graphic Class & Inserttype Filter dialogs added - SN
   //Version = "4.3.0gl";  // 08/19/03 - Version = "4.2.129" - knv
   //Version = "4.3.0gk";  // 08/19/03 - Case #315 - VBASCII write - knv
   //Version = "4.3.0gj";  // 08/19/03 - Version = "4.2.128" - LP
   //Version = "4.3.0gi";  // 08/18/03 - SetRealPartData now supports reading of attributes, package pins, and inheriting attributes to the datas - AC
                           //          - Updated licensing error dialog with the correct contact information
   //Version = "4.3.0gh";  // 08/18/03 - Version = "4.2.127" - LP
   //Version = "4.3.0gg";  // 08/18/03 - Case #308 - VBAscii Read added the command ".USE_UNIQUE_NETZERO" to assign comp pin to unique net zero - LP
   //Version = "4.3.0gf";  // 08/15/03 - added conditional compilation directives to remove DFT from CamCad builds - knv
   //Version = "4.3.0ge";  // 08/15/03 - updated Attrib x and y members and handling - knv
   //Version = "4.3.0gd";  // 08/15/03 - Case #265 - Vb99Out.cpp - VBAscii Write should put non-net etch and vias to "(NET0)" - LP
   //Version = "4.3.0gc";  // 08/15/03 - Case #264 - AllegIn.cpp - Fabmaster Allegro does not insert pin when ther is no pin rotation - LP
   //Version = "4.3.0gb";  // 08/15/03 - Version = "4.2.126" - LP
   //Version = "4.3.0ga";  // 08/15/03 - updated BlockStruct num member and handling - knv
   //Version = "4.3.0fz";  // 08/14/03 - updated BlockStruct extent members and handling - knv
   //Version = "4.3.0fy";  // 08/14/03 - Version = "4.2.125" - LP
   //Version = "4.3.0fx";  // 08/14/03 - Version = "4.2.124" - LP
   //Version = "4.3.0fw";  // 08/14/03 - Version = "4.2.123" - LP
   //Version = "4.3.0fv";  // 08/12/03 - Added SourceSafe $Header keyword lines to source files - knv
   //Version = "4.3.0fu";  // 08/12/03 - Version = "4.2.121" - Lp
   //Version = "4.3.0ft";  // 08/12/03 - Version = "4.2.120" - LP
   //Version = "4.3.0fs";  // 08/12/03 - Case #301 - Neutral+geoms_ascii read issue - knv
   //Version = "4.3.0fr";  // 08/11/03 - Case #298 - DDE Read - ATR visibility - knv
   //Version = "4.3.0fq";  // 08/08/03 - Edif200_in.cpp & Edif300-in.cpp - Modified so that the command ".OVERWRITE_ATTRIBUTE_DISPLAY" is by default set to TRUE - LP
   //Version = "4.3.0fp";  // 08/08/03 - Case #269 - No Inch - IPC units drop list - knv
   //Version = "4.3.0fo";  // 08/07/03 - Case #117 - Wrong size on panel display - knv
   //Version = "4.3.0fn";  // 08/07/03 - Case #302 - Net_Drv.cpp & FloodFil.cpp - using Derive Netlist causes CAMCAD to CRASH - SN
   //Version = "4.3.0fm";  // 08/06/03 - Version = "4.2.118" - LP
   //Version = "4.3.0fl";  // 08/06/03 - Case #280 - ODB read in MM = BAD - knv
                           //          - Added CEntityNumber class and modified code referencing EntityNum
   //Version = "4.3.0fk";  // 08/06/03 - Version = "4.2.117" - LP
   //Version = "4.3.0fj";  // 08/06/03 - Case #256 - MentorIn.cpp - Added filtering commands in the .in file - SN
   //Version = "4.3.0fi";  // 08/06/03 - Case #270 - CadifIn.cpp - Unexpected results with .CHANGEALTNAME command set - SN
   //Version = "4.3.0fh";  // 08/06/03 - Version = "4.2.116" - LP
   //Version = "4.3.0fg";  // 08/05/03 - Case #286 - PcbUtil.cpp - After running the "Generate Pin 1 Marker" function once, all other attempts will do nothing (Should overwrite the previous marker) - SN  
   //Version = "4.3.0ff";  // 08/05/03 - Case #249 - FATFin.cpp - Pins are misplaced and misrotated in some geometries - SN
   //Version = "4.3.0fe";  // 08/05/03 - Case #252 - Make schematic navigation dialog resizable - knv
   //Version = "4.3.0fd";  // 08/05/03 - Case #279 - Cadence Allegro read did not correctly create geometry that has pin edited - LP
   //Version = "4.3.0fc";  // 08/04/03 - Case #262 - Mentor Board Station Reader - Incorrect placement of pinref attributes (Try 2) - knv
   //Version = "4.3.0fb";  // 08/04/03 - Version = "4.2.115" - Lp
   //Version = "4.3.0fa";  // 08/01/03 - Case #262 - Mentor Board Station Reader - Incorrect placement of pinref attributes - knv
   //Version = "4.3.0ez";  // 08/01/03 - Updated Get/Set RealPartData to return/get library w/in PACKAGE, DEVICE, and PART - AC
                           //          - Updated message of not reading in intelligent data when in CAMCAD Graphic "intellegent" -> "intelligent" - AC
                           // 08/01/03 - Case #283 - QD_Out.cpp - When outputing to bottom file ("*_B"), change all X location to -X location - LP
   //Version = "4.3.0ey";  // 08/01/03 - Edif200_In.cpp, Edif300_In.cpp - ".Exchange_XY_Scale" to allow some system to swap X & Y scale are, so if sExchangeXYScale TRUE then swap them - LP
                           //          - Case #243 - Vb99Out.cpp - Miss type the component in "Cell.hkp" file as through hole instead of sureface - LP
   //Version = "4.3.0ex";  // 07/31/03 - Updated GetRealPartData and SetRealPartData calculate part offset and rotations (defaulting package to 0,0, and 0 rotation) - AC
                           //             - Updated DTransform to support UntransformPoint
   //Version = "4.3.0ew";  // 07/31/03 - Edif200_In.cpp, Edif300_In.cpp, and Sch_Lib.cpp - Added the following changes - LP
                           //             - ".DESIGNATOR_LOCATION_BACKANNOTATE" to get the attribute which provide the display location of designator
                           //             - ".PINNO_LOCATION_BACKANNOTATE" to get the attribute which provide the display location of pin number
                           //             - ".FONT_SCALE" to get the scaling value use to scale the text size
                           //             - The function RenameCompAndPinDesignator no long makes sure the value (designator & pin number) is unique
   //Version = "4.3.0ev";  // 07/31/03 - Updated GetRealPartData to return packages correctly - AC
   //Version = "4.3.0eu";  // 07/30/03 - Updated GetRealPartData and SetRealPartData to support new structure - AC
   //Version = "4.3.0et";  // 07/29/03 - Updated decision of which product to run starting with LOCAL_MACHINE, then CURRENT_USER - AC
   //Version = "4.3.0es";  // 07/29/03 - Version = "4.2.112" - LP
   //Version = "4.3.0er";  // 07/29/03 - Version = "4.2.111" - LP
   //Version = "4.3.0eq";  // 07/29/03 - Updated GetRealPartData to return new XML structure with packages  (return centroid information) - AC
                           //          - Removed RestructureComponentData API (API Version 1.47)
                           //          - Cleaned up pin pitch calculations
                           //          - Added RealPart.cpp and RealPart.h for RealPart functions
   //Version = "4.3.0ep";  // 07/29/03 - Version = "4.2.110" - LP
   //Version = "4.3.0eo";  // 07/29/03 - Case #275 - CAMCAD crashes when querying a poly - LP
   //Version = "4.3.0en";  // 07/28/03 - Updated RSI company name and copyright years in about box - AC
   //Version = "4.3.0em";  // 07/28/03 - Version = "4.2.109" - LP
   //Version = "4.3.0el";  // 07/25/03 - Added block types Package, RealPart, and Package Pin - AC
                           //          - Added insert types Package Pin and changed insert type Land Pattern to RealPart
                           //          - Added layer type Package Pin Leg Top & Bottom and Package Pin Foot Top & Bottom
                           //          - Added graphic class Package Pin and Package Body
   //Version = "4.3.0ek";  // 07/25/03 - Version = "4.2.108" - LP
   //Version = "4.3.0ej";  // 07/25/03 - Version = "4.2.107" - LP
   //Version = "4.3.0ei";  // 07/25/03 - Version = "4.2.106" - LP
   //Version = "4.3.0eh";  // 07/25/03 - MentorIn.cpp - Refix case #172 - LP
                           //          - If no layer file is loaded, instead set ".EXPLODE_POWER_SIGNAL_LAYER_SHAPE" to FASLE instead of ignoring it
                           //          - If ".EXPLODE_POWER_SIGNAL_LAYER_SHAPE" is false, don't delete the generic POWER and SIGNAL layer
   //Version = "4.3.0eg";  // 07/24/03 - ThedaIn.cpp - Fixed case #229 - CAMCAD crashed reading Theda because a padstack is inserting a padshape that has the same name as the padstack - LP
   //Version = "4.3.0ef";  // 07/24/03 - MentorIn.cpp - Refix case #172 - LP
                           //          - If no layer file is loaded, ignore the command ".EXPLODE_POWER_SIGNAL_LAYER_SHAPE"
                           //          - Change the handling of blink/buried padstack and via in the function Create_LayerrangePADSTACKData
   //Version = "4.3.0ee";  // 07/23/03 - returns the actual pad shape in GetRealPartData (top, bottom, extents)
   //Version = "4.3.0ed";  // 07/23/03 - Added Api_Rp.cpp for real part functions
                           //          - Optimized GetRealPartData to generate the real part string faster
                           //          - GetRealPartData now returns the pin numbers and names of the land patterns
   //Version = "4.3.0ec";  // 07/22/03 - MentorIn.cpp - Refix case #171 - LP
                           //          - If layer file is loaded, ignore ".MIRRORLAYER" and ".LAYERRENAME" command from mentor.in file
                           //          - If ther is ".LAYERATTR" setting for generic _1 or _2 layers, apply the setting to _TOP and _BOT layer also
   //Version = "4.3.0eb";  // 07/22/03 - Version = "4.2.105" - knv
   //Version = "4.3.0ea";  // 07/22/03 - Version = "4.2.104" - LP
                           //          - Dde9_In.cpp - Fixed the way Graph_Polygon removed poly from polylist because of the new support Kurt added to clean up list.  This will fix the crash in CAMCAD - LP
   //Version = "4.3.0dz";  // 07/21/03 - Version = "4.2.103" - knv
   //Version = "4.3.0dy";  // 07/21/03 - Case #140 - Theda Reader blind vias not generated correctly - knv
   //Version = "4.3.0dx";  // 07/18/03 - Fixed Generate outline function for DFM using the new list classes - AC
   //Version = "4.3.0dw";  // 07/18/03 - Case #228 - Query dialog and pin pitch - knv
   //Version = "4.3.0dv";  // 07/18/03 - Version = "4.2.102" - LP
   //Version = "4.3.0du";  // 07/18/03 - Case 234 - Fixed GeneratePinPitch with new PCB Comp Structure - AC
   //Version = "4.3.0dt";  // 07/18/03 - Version = "4.2.101" - LP
   //Version = "4.3.0ds";  // 07/17/03 - Removed support for embedded applications (COleTemplateServer in CCEtoODBApp) - AC
   //Version = "4.3.0dr";  // 07/17/03 - Pads_Out.cpp - LP
                           //          - Removed the function ResetApertureOffset() 
                           //          - Added the function DuplicateDeviceType to duplicate device type for the mirrored block 
   //Version = "4.3.0dq";  // 07/16/03 - Version = "4.2.100" - LP
   //Version = "4.3.0dp";  // 07/16/03 - Pads_Out.cpp - Modified the function update_mirrorlayers to not set the mirror flag for layers - LP
   //Version = "4.3.0do";  // 07/16/03 - Case #242 - Pads writer does not output padstack correctly because apertures has offset - LP
   //Version = "4.3.0dn";  // 07/16/03 - Version = "4.2.99" - AC
   //Version = "4.3.0dm";  // 07/16/03 - ccm_out.cpp - Case 254 - Fixed crash error cleaning up memory when it shouldn't yet through the new list classes - AC
   //Version = "4.3.0dl";  // 07/15/03 - Pads_Out.cpp - Modified the function update_mirrorlayers to check layer name with no case sensitive - LP
   //Version = "4.3.0dk";  // 07/15/03 - Outline.cpp - Fixed old outline routine to clean up polylist with the new list classes - AC
   //Version = "4.3.0dj";  // 07/14/03 - Version = "4.2.98" - LP
   //Version = "4.3.0di";  // 07/11/03 - Updated SetRealPartData to close any polys left open - AC
   //Version = "4.3.0dh";  // 07/11/03 - Updated new toe layers to be called foot layers - AC
                           //          - Updated GetRealPartData to return package information
   //Version = "4.3.0dg";  // 07/10/03 - Updated SetRealPartData to overwrite existing packages and to lighten toes - AC
   //Version = "4.3.0df";  // 07/10/03 - Added menu option to start the Device Typer application made by Chris - AC
   //Version = "4.3.0de";  // 07/10/03 - Version = "4.2.97" - LP
   //Version = "4.3.0dd";  // 07/09/03 - Version = "4.2.95" - AC
   //Version = "4.3.0dc";  // 07/09/03 - Version = "4.2.94" - AC
   //Version = "4.3.0db";  // 07/09/03 - Version = "4.2.92" - LP
   //Version = "4.3.0da";  // 07/09/03 - Version = "4.2.91" - LP
   //Version = "4.3.0cz";  // 07/08/03 - Refix case #171 - mentorin.cpp - crash due to writing to memory not allocated - LP
   //Version = "4.3.0cy";  // 07/03/03 - Added DFT attribute calculation functionality - knv
   //Version = "4.3.0cx";  // 07/03/03 - Version = "4.2.90" - LP
   //Version = "4.3.0cw";  // 07/03/03 - Version = "4.2.89" - LP
   //Version = "4.3.0cv";  // 07/03/03 - Version = "4.2.87" - LP
                           //          - Refix case #171 - mentorin.cpp - crash due to a bug - LP                          
   //Version = "4.3.0cu";  // 07/03/03 - Version = "4.2.86" - knv
   //Version = "4.3.0ct";  // 07/02/03 - Updated RestructureCompData to hide old silkscreen and pin graphics - AC
                           //          - Implemented SetRealPartData's content handler (attributes not supported yet)
                           //          - Updated XMLReader to use CFile instead of CStdioFile (updated GetLine)
                           //          - Added new defines for Real Part types
                           //          - Hide Real Part API's and SetWindowIcon
   //Version = "4.3.0cs";  // 07/02/03 - Version = "4.2.85" - LP
   //Version = "4.3.0cr";  // 07/01/03 - Version = "4.2.84" - LP
   //Version = "4.3.0cq";  // 07/01/03 - Version = "4.2.83" - AC
   //Version = "4.3.0cp";  // 07/01/03 - Added RealPartXML content handler and RealPartXML reader (still in progress) - AC
                           //          - Added XmlrealPartContent.h and XmlrealPartContent.cpp
   //Version = "4.3.0co";  // 06/30/03 - Refix case #174 - MentorIn.cpp - LP
                           //          - Added support for the attribute command "COMPONENT_SPECIFIC_LAYER_ON"
                           //          - Modified so that visible on generic layer of Top & Bottom mapping layer set always goes to layer_Top
                           //          - For board level data, thruhole, blind, or buried component
                           //             - If data are on generic layer of Top & Bottom mapping layer set, then data should goes on to layer_Top and layer_Bot
                           //             - If data are on specific layer_1 or layer_2, then data should goes to layer_Top and layer_Bot respectively
   //Version = "4.3.0cn";  // 06/27/03 - Refix of case #171 - MentorIn.cpp - LP
                           //          - Added code to set the Top & Bottom mirror layers pairs of layer_1 to NEVERMIRROR and layer_2 to MIRRORONLY
                           //          - Added code to add layer_Top & layer_Bot for every generic layer in the Top & Bottom mirror layer set
   //Version = "4.3.0cm";  // 06/25/03 - Version = "4.2.81" - LP
   //Version = "4.3.0cl";  // 06/25/03 - Version = "4.2.80" - LP
   //Version = "4.3.0ck";  // 06/25/03 - Version = "4.2.79" - LP
   //Version = "4.3.0cj";  // 06/23/03 - Added SetRealPartData - AC
   //Version = "4.3.0ci";  // 06/23/03 - Version = "4.2.78" - AC
   //Version = "4.3.0ch";  // 06/23/03 - Re-indexed RestructureCompData, GetRealPartData, SendLayerToBack and BringLayerToFront - AC
   //Version = "4.3.0cg";  // 06/20/03 - Added context menu in layer list to support send layer to back, bring to layer to front, and edit layer - AC
                           // Added SendLayerToBack and BringLayerToFront API's (API Version 1.43)
                           // Updated GetRealPartData to provide appropriate poly types (silk screen and pad)  
   //Version = "4.3.0cf";  // 06/19/03 - Version = "4.2.76" - AC
   //Version = "4.3.0ce";  // 06/19/03 - Version = "4.2.75" - SN
   //Version = "4.3.0cd";  // 06/18/03 - Version = "4.2.74" - AC
                           // Added content to GetRealPartData API (still need POLYPOINT)
   //Version = "4.3.0cc";  // 06/18/03 - Version = "4.2.72" - SN
   //Version = "4.3.0cb";  // 06/18/03 - Version = "4.2.71" - LP
   //Version = "4.3.0ca";  // 06/16/03 - Version = "4.2.70" - SN
   //Version = "4.3.0bz";  // 06/13/03 - Version = "4.2.68" - LP
   //Version = "4.3.0bx";  // 06/13/03 - Version = "4.2.67" - SN
   //Version = "4.3.0bw";  // 06/13/03 - Fixed bug in RestructureCompData and EditGeometry in case a doc is not open - AC
   //Version = "4.3.0bv";  // 06/13/03 - Version = "4.2.65" - AC
                           // renamed GenerateRealPartData to RestructureCompData
   //Version = "4.3.0bu";  // 06/13/03 - Case #213 - SN
   //Version = "4.3.0bt";  // 06/13/03 - Version = "4.2.66" - SN
   //Version = "4.3.0bs";  // 06/12/03 - DFT and Theda reader changes - knv
   //Version = "4.3.0br";  // 06/12/03 - Version = "4.2.64" - SN
   //Version = "4.3.0bq";  // 06/12/03 - Added new API GetRealPartData - AC
   //Version = "4.3.0bp";  // 06/12/03 - Version = "4.2.63" - AC
   //Version = "4.3.0bo";  // 06/12/03 - Version = "4.2.62" - knv
   //Version = "4.3.0bm";  // 06/11/03 - Version = "4.2.61" - knv
   //Version = "4.3.0bl";  // 06/11/03 - Added new GenerateRealPartData - AC
                           // Added new preprocessor flag _RDEBUG so we could still use the debug pop-up menu option
   //Version = "4.3.0bk";  // 06/11/03 - Version = "4.2.60" - knv
   //Version = "4.3.0bj";  // 06/10/03 - Version = "4.2.59" - LP
   //Version = "4.3.0bi";  // 06/11/03 - Version = "4.2.58" - SN
   //Version = "4.3.0bh";  // 06/10/03 - Version = "4.2.57" - LP
   //Version = "4.3.0bg";  // 06/10/03 - Version = "4.2.56" - AC
   //Version = "4.3.0bf";  // 06/10/03 - Version = "4.2.55" - LP
   //Version = "4.3.0be";  // 06/10/03 - Version = "4.2.54" - knv
   //Version = "4.3.0bd";  // 06/09/03 - Version = "4.2.53" - LP
   //Version = "4.3.0bc";  // 06/09/03 - Version = "4.2.52" - LP
   //Version = "4.3.0bb";  // 06/09/03 - Version = "4.2.50" - LP
   //Version = "4.3.0ba";  // 06/06/03 - Added table parser to Theda reader - knv
                           // Converted SelectGeometry from Dialog to CResizingDialog
   //Version = "4.3.0az";  // 06/06/03 - Version = "4.2.49" - LP
   //Version = "4.3.0ay";  // 06/06/03 - Added new insert types Land Pattern, Pad, and Package - AC
   //Version = "4.3.0ax";  // 06/06/03 - Case #173 - MentorIn.cpp/.h - Added the support of multiple display for pin refname - LP
   //Version = "4.3.0aw";  // 06/06/03 - Replaced all calls to memcpy() of Attrib with Attrib::allocateCopy() or Attrib::operator=() - knv
   //Version = "4.3.0av";  // 06/06/03 - Version = "4.2.48" - SN
   //Version = "4.3.0au";  // 06/05/03 - Changed implementation of Attrib - knv
   //Version = "4.3.0at";  // 06/04/03 - Version = "4.2.47" - AC 
   //Version = "4.3.0as";  // 06/04/03 - Version = "4.2.46" - LP
   //Version = "4.3.0ar";  // 06/04/03 - Version = "4.2.45" - LP
   //Version = "4.3.0aq";  // 06/03/03 - Version = "4.2.44" - TLS
   //Version = "4.3.0ap";  // 06/03/03 - Version = "4.2.43" - knv
   //Version = "4.3.0ao";  // 06/03/03 - Version = "4.2.41" - LP
   //Version = "4.3.0an";  // 06/03/03 - Version = "4.2.41" - LP
   //Version = "4.3.0am";  // 06/03/03 - Version = "4.2.40" - LP
   //Version = "4.3.0al";  // 06/02/03 - Version = "4.2.39" - LP
   //Version = "4.3.0ak";  // 05/30/03 - Version = "4.2.38" - SN
   //Version = "4.3.0aj";  // 05/30/03 - Added new insert type "Aperture" - AC
   //Version = "4.3.0ai";  // 05/29/03 - Version = "4.2.37" - LP
   //Version = "4.3.0ah";  // 05/29/03 - Version = "4.2.36" - LP
   //Version = "4.3.0ag";  // 05/28/03 - Upgraded most major dialogs to derive from CResizingDialog - knv
                           //          - Added save and restore of CMainFrame window state.
                           //          - Added classic user interface mode flag.
   //Version = "4.3.0af";  // 05/28/03 - Version = "4.2.35" - LP
   //Version = "4.3.0ae";  // 05/28/03 - Version = "4.2.34" - LP
   //Version = "4.3.0ad";  // 05/28/03 - Version = "4.2.33" - SN
   //Version = "4.3.0ac";  // 05/28/03 - Version = "4.2.32" - LP
   //Version = "4.3.0ab";  // 05/27/03 - Version = "4.2.31" - LP
   //Version = "4.3.0aa";  // 05/23/03 - Case #28 Allow multiple Refdes attributes - knv 
                           //          - Added Resizing Persistant Dialog functionality
   //Version = "4.3.0z";   // 05/22/03 - Changed sorting for EditAttribs dialog - knv 
   //Version = "4.3.0y";   // 05/21/03 - Added multiple visible attribute support - knv 
   //Version = "4.3.0x";   // 05/20/03 - Version = "4.2.30" - DD
   //Version = "4.3.0w";   // 05/20/03 - Version = "4.2.29" - LP
   //Version = "4.3.0v";   // 05/20/03 - Version = "4.2.28" - LP
   //Version = "4.3.0u";   // 05/20/03 - Version = "4.2.27" - LP
   //Version = "4.3.0t";   // 05/19/03 - Right-Click has Move Selected to Front/Back - DD
   //Version = "4.3.0s";   // 05/19/03 - Version = "4.2.26" - DD
   //Version = "4.3.0r";   // 05/16/03 - Converted Attrib structure to class - knv
   //Version = "4.3.0q";   // 05/15/03 - Version = "4.2.24" - AC
   //Version = "4.3.0p";   // 05/15/03 - Version = "4.2.23" - LP
   //Version = "4.3.0o";   // 05/13/03 - Version = "4.2.19" - LP
   //Version = "4.3.0n";   // 05/12/03 - Version = "4.2.18" - LP
   //Version = "4.3.0m";   // 05/12/03 - Version = "4.2.17" - knv
   //Version = "4.3.0l";   // 05/09/03 - Version = "4.2.16" - LP
   //Version = "4.3.0k";   // 05/08/03 - Version = "4.2.11" - LP
   //Version = "4.3.0j";   // 05/08/03 - Version = "4.2.10" - AC
   //Version = "4.3.0i";   // 05/07/03 - Version = "4.2.9" - LP
   //Version = "4.3.0h";   // 05/06/03 - Version = "4.2.8" - knv 
   //Version = "4.3.0g";   // 05/06/03 - Version = "4.2.7" - knv 
   //Version = "4.3.0f";   // 05/06/03 - Version = "4.2.6" - LP
                           // 05/05/03 - Version = "4.2.4" - DF
   //Version = "4.3.0e";   // 05/05/03 - Version = "4.2.5" - knv
   //Version = "4.3.0d";   // 05/02/03 - Version = "4.2.3" - knv 
   //Version = "4.3.0c";   // 05/02/03 - Version = "4.2.2" - LP
   //Version = "4.3.0b";   // 05/01/03 - Version = "4.2.1" - LP
   //Version = "4.3.0a";   // 05/01/03 - Branched 4.3
   ////////////////////////////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////////////////////////////
   //Version = "4.2.138" - Case #342 - pads_out.cpp - Outputing wrong PINNR. After creating PINNR and refname for pin that is missing PINNR and refname, need to re-assign it back to the data - LP
   //Version = "4.2.137";  // 09/25/03 - Case #349 - Mneutin.cpp - Added the command ".FIX_BOARD_OUTLINE" to reverse the last two coordinate of board outline - LP
                           //          - Case #355 - pads_out.cpp - Correction made to output lines on ALL layers and to output the right restriction code - LP
   //Version = "4.2.136";  // 09/16/03 - Case #340 - GencamIn.cpp , Gencam_O.cpp - CAMCAD crashed when outputing GenCAM and crash when closing a document after the import of GenCAM file - LP
   //Version = "4.2.135";  // 09/09/03 - Case #336 - CAMCAD will just crash once you close the project window - LP
   //Version = "4.2.134";  // 08/26/03 - Case #291 - Fixed the DFM flatten by inserttype to inherit the surface through the recursion - AC
   //Version = "4.2.133";  // 08/25/03 - Fixed the DFM attribute comparisons to compare string w/o regard to case - AC
   //Version = "4.2.132";  // 08/22/03 - Case #291 - Added surface parameter to FlattenByInsertType API (API Version 1.48) - AC
   //Version = "4.2.131";  // 08/22/03 - Case #320 - Huntrn_o.cpp - When in Goepel style, need to output board info for every board - LP
   //Version = "4.2.130";  // 08/21/03 - Case #314 - Hp3070_o.cpp - When probe acces is "default" and NO DFT, then output should be blank instead of "NO_PROBE" - LP
   //Version = "4.2.129";  // 08/19/03 - Case #319 - ODB++ import, missing uncompress error message - knv
   //Version = "4.2.128";  // 08/19/03 - Refix Case #279 - Allegin.cpp - Pins were inserted again for each instance of a geometry in do_netfile function - LP
   //Version = "4.2.127";  // 08/18/03 - Case #313 - GencadIn.cpp - A bug due to creating the pad stack name unique for each pad shape on a specific layer before check to see if it use a pad shape - LP
   //Version = "4.2.126";  // 08/15/03 - Case #304 - huntrn_o.cpp - HAF Writer, added ".EXPORT_STYLE" to ".in" file - LP
   //Version = "4.2.125";  // 08/14/03 - Case #309 - QD_out.cpp - Change the X to -X for the length and corner of panel and board if it is bottom - LP
   //Version = "4.2.124";  // 08/14/03 - Refix Case #233 - ddein.cpp, allegout.cpp - Fixed the rename of non-unique pin refname in netlist - LP 
   //Version = "4.2.123";  // 08/14/03 - Case #294 - dde9_in.cpp/.h - DDE V9 reader, the ".psi" records did not inherited drill and property from ".pst" records - LP 
   //Version = "4.2.122";  // 08/14/03 - Case #279 - Allegin.cpp/.h - Cadence Allegro read did not correctly create geometry that has pin edited - LP
   //Version = "4.2.121";  // 08/12/03 - Case #290 - Edif300_in.cpp - port designator backannotate was not correctly done - LP
   //Version = "4.2.120";  // 08/12/03 - Refix Case #284 - GencadIn.cpp - PIN padstack is not build correctly if it is referencing a PAD instead of a realy padstack - LP
   //Version = "4.2.118";  // 08/06/03 - Case #300 - CAMCAD crashed reading in Cadence Allegro Extract - LP
   //Version = "4.2.117";  // 08/06/03 - Case #233 - AllegOut.cpp - Allegro Script Write will use PINNR as pin refname when pin refname is not unique - LP
   //Version = "4.2.116";  // 08/06/03 - Case #285 - QD_out.cpp - Eliminate /* from the comment line in Siemen QD, except on the first line of the file - LP
   //Version = "4.2.115";  // 08/04/03 - Case #284 - GencadIn.cpp - CAMCAD crash reading GENCAD due invalid reference in the variable planePoly - LP
   //Version = "4.2.112";  // 07/29/03 - Case #278 - Edif300_in.cpp - Modified the read to skip if fontDefinitions or physicalDefaults structure is empty - LP
   //Version = "4.2.111";  // 07/29/03 - Case #272 - gerb_mrg.cpp - Modified the function mergeGerberEcad to always check for PASTE TOP OR PASTE BOTTOM of a fresh regardless if ECAD pin is SMD- Lp
   //Version = "4.2.110";  // 07/29/03 - Case #261 - mentorin.cpp - Centroid markers are not being created correctly in mentor - LP
   //Version = "4.2.109";  // 07/28/03 - Case #267 - QD_out.cpp - Output the word "SPERREN" when components are not LOADED - LP
   //Version = "4.2.108";  // 07/25/03 - Graph.cpp/.h & Ccdoc.cpp - LP
                           //             Change the function RefreshInheritedAttributes to overwrites attributes on the insert with those on the inserted geometry 
                           //             if the inherited flag on the insert's attribute is set to:
                           //                1) TRUE
                           //             or 2) FALSE, but the parameter "overwriteNotInherited" is set to TRUE
   //Version = "4.2.107";  // 07/25/03 - Case #266 - Graph.cpp - The function RefreshInheritedAttributes() did not check if the same attribute on the insert was inherited or not, it just overwrite in anyway - LP
   //Version = "4.2.106";  // 07/25/03 - Case #245 - Edif300_In.cpp - The command ".OVERWRITE_ATTRIBUTE_DISPLAY" was misspelled as "".OVERRIDE_ATTRIBUTE_DISPLAY"; therefore the port display was never overwrited - LP
   //Version = "4.2.105";  // 07/22/03 - Case #247 - Manufacturing grid in mm - knv
   //Version = "4.2.104";  // 07/22/03 - Case #150 - ddein.cpp - pin padstack create from shape was not created on the correct layer if the component is mirrored - LP
   //Version = "4.2.103";  // 07/21/03 - Case #170 - VBASCII write, via not converted to testpoint - knv
   //Version = "4.2.102";  // 07/18/03 - Case #257 - GencadIn.cpp - Gencar reader crash due to reference of not existing device type - LP
   //Version = "4.2.101";  // 07/18/03 - Case #255 - PfWin.cpp - Protel reader crash reading due to reference of not existing pin data - LP
   //Version = "4.2.100";  // 07/16/03 - Case #248 - excelout.cpp - corrected the output of trailing zero when the value is negative - LP
   //Version = "4.2.99";   // 07/16/03 - Added command line option to register CAMCAD only - AC
   //Version = "4.2.98";   // 07/14/03 - Case #238 - Theda Read crash when ".BAREBOARD_PROBES" set to "Y" in .in file because reader reference unplaced probe - LP
   //Version = "4.2.97";   // 07/10/03 - Case #246 - Zuken CR5000 Read crash - LP
   //Version = "4.2.95";   // 07/10/03 - Case #237 - vb99in.cpp - Fixed Veribest HKP read to add needed '\' to the end of the supplied path if no present - AC
   //Version = "4.2.94";   // 07/09/03 - Fixed DistanceLineToLine to support width correctly - AC
   //Version = "4.2.92";   // 07/09/03 - Case #237 - Ag_AOI_o.cpp - Modified so that reference designator in both columns are identical in *.plx file - LP
   //Version = "4.2.91";   // 07/09/03 - Case #236 - QD_out.cpp - Modified Siemen file output when there is an XOUT - LP
   //Version = "4.2.90";   // 07/03/03 - Allegin.cpp - Fixed a crash that is cause by not check if recordArrayCount < commandArrayCount - LP
   //Version = "4.2.89";   // 07/03/03 - Case #227 - Edif200_in.cpp - Handle net name rename
   //Version = "4.2.87";   // 07/03/03 - Case #208 - Edif 200 & 300 REad - add command .in file to allow use to select attribute for designator and pin number - LP
                           //          - Edif200_in.cpp & Edif300_in.cpp
                           //             1) Added the command ".DESIGNATOR_BACKANNOTATE" to allow user to select an attribute as designator
                           //             2) Added the command ".PINNO_BACKANNOTATE" to allow user to select an attribute as pin number
                           //          - Sch_Lib.cpp/.h - Modified the function RenameCompAndPinDesignator() to reassign designator to component attribute and pin number to compin attribute correctly
                           //          - Sch_List.cpp - Modified the funciton FillNetsTree so that comppin names are correctly added
   //Version = "4.2.86";   // 07/02/03 - Implemented .z file reading - knv
                           // 07/02/03 - Case #167 - Long Path Results in Errors - knv
                           // 07/02/03 - Case #179 - ODB++ read, renaming error - knv
                           // 07/02/03 - Case #225 - ODB++ read issue - knv
   //Version = "4.2.85";   // 07/02/03 - Case #239 - excelout.cpp & CAMCAD.RC & resource.h - support the output of trailing zero suppression - LP
   //Version = "4.2.84";   // 07/01/03 - Ck.cpp - LP
                           //             - Refixed case #29 - rename of netname was not renamed correctly
                           //             - Fixed case #175 - PADS write crash due to rename not correctly done causing memory overrided
   //Version = "4.2.83";   // 07/01/03 - Added SetWindowIcon API (APIVersion 1.46) - AC
   //Version = "4.2.82";   // 06/27/03 - Undo Case #171 & 172 since they should only be in Version 4.3 
   //Version = "4.2.81";   // 06/26/03 - CAMCAD.RC, resource.h, and FILETYPE.CPP - Added the file type Package and Layer to the Mentor File Type dialog - LP
                           //          - mentorin.cpp - Make sure the function load_mentorsettings alwasy get call before reading each file - LP
   //Version = "4.2.80";   // 06/25/03 - Case #172 - mentorin.cpp - Added the command ".EXPLODE_POWER_SIGNAL_LAYER_SHAPE" in mentor.in to duplicate contents of the generic SIGNAL and POWER layers to the specific SIGNAL_# and POWER_# layers - LP
   //Version = "4.2.79";   // 06/25/03 - Case #171 - mentorin.cpp/.h, port_lib.cpp - CAMCAD not reading the layer file and not properly interpretying some layers as being part of a mirror layer pair - LP
   //Version = "4.2.78";   // 06/24/03 - Added new DFM API CombinePolies to support Soldermask Webbing - AC
   //Version = "4.2.77";   // 06/23/03 - Version = "4.3.0cg" 
   //Version = "4.2.76";   // 06/19/03 - dfm_dist.cpp - modified distance check to only report by netnames when SkipSameNets is set - AC
   //Version = "4.2.75";   // 06/19/03 - Case #226 - DXFIN.cpp & attrib.cpp - Camcad crashes when reading the dxf file exported from sample pads - SN
   //Version = "4.2.74";   // 06/18/03 - Case #218 - lic.cpp - Added DXF Write back into CAMCAD Vision - AC
   //Version = "4.2.72";   // 06/18/03 - Case #224 - FATFIN.cpp - Net list not populated correctly and missing pin definitions - SN
   //Version = "4.2.71";   // 06/18/03 - Case #220 - dde9_in.cpp & ddein.cpp - forget to create new CPnt at few place in the function Add_Polygon_Data_To_PolyList(), which cause CAMCAD to crash due to unhandled exception - LP
   //Version = "4.2.70";   // 06/16/03 -  Gerout.cpp - Complex apertures not being exported in Gerber - SN
   //Version = "4.2.68";   // 06/13/03 - Case #210 - Some flashes are not showing because the callout "G45D3" or "G45D03" is called after an XY location to be created as a flash.  This was not supported before - LP
   //Version = "4.2.67";   // 06/13/03 - update.cpp - Case #189 - Reference Placement menu command - SN
   //Version = "4.2.66";   // 06/13/03 - sequence.cpp - Case #212 - SN
   //Version = "4.2.65";   // 06/12/03 - dfm_dist.cpp - fixed bug with the distance check algorithm when skipping same nets - AC
   //Version = "4.2.64";   // 06/12/03 - Orcltdin.cpp - fixed pads in padstack on wrong layers and sides - SN
   //Version = "4.2.63";   // 06/12/03 - port.cpp - fixed importing files through the API to put in the PATH attribute - AC
   //Version = "4.2.62";   // 06/12/03 - Case #193 - FATFIN.cpp - Refix, padstack created useing TRACK and BLOCK are not offseted correctly - LP
   //Version = "4.2.61";   // 06/11/03 - Case #185 - Saving ccz files - knv
   //Version = "4.2.60";   // 06/11/03 - Theda Reader crash on Case #140 file - knv
   //Version = "4.2.59";   // 06/11/03 - QD_out.cpp - LP
                           //             1) Modified the write to output the length of a mirrored board as a negative length
                           //             2) Modified so that if the component specific for ".MACHINE_ORIGIN_BOT" in the ".out" file is place on top, go ahead that use it even though it is not place bottom
   //Version = "4.2.58";   // 06/11/03 - Case #190 - PFWIN.cpp - components rotation incorrect - SN
   //Version = "4.2.57";   // 06/11/03 - Case #204 - Allegin.cpp - component specific device type was created each component even though there is not PIN_EDITED, in the function do_netfile - LP
   //Version = "4.2.56";   // 06/10/03 - CAMCAD.CPP - Updated logic to check for licenses when running through automation - AC
   //Version = "4.2.55";   // 06/10/03 - Case #145 - ta_report.cpp - DFT Testability Report  is not reporting a net that should be in "Multi Point Net Needing Additional Probess" section because the net has less probe that it is required - LP
   //Version = "4.2.54";   // 06/10/03 - Case #8 - Theda netlist read version - knv
   //Version = "4.2.53";   // 06/09/03 - Case #198 - Ciiin.cpp - CAMCAD crash importing Scicard file due because in the function do_562() it didn't exit the function when then is an error in the file - LP
   //Version = "4.2.52";   // 06/09/03 - Case #193 - FATFIN.cpp - When creating padstack for pin using "POLYGON", it is need to be offseted to the pin insert x and y - LP
   //Version = "4.2.50";   // 06/09/03 - Case #197 - FATFIN.cpp - CAMCAD crash when loading second FATF file because the read did not initialize members and the FreeMembers function did not reset the count of some members - LP
   //Version = "4.2.49";   // 06/06/03 - Case #186 - Mneutin.cpp/,h - CAMCAD crash loading the neutral file due to tok[80] declared as 80 by then expanded to 127 causing other memories to be overwrite.  Added the constant MAX_TOKEN = 127 and MAX_ATT = 255 to prevent further bug
                           //          - Graph.cpp - In RefreshInheritedAttributes() if attribute map is empty then create it again - LP
   //Version = "4.2.48";   // 06/06/03 - Graph.cpp - Protel import crash - SN
   //Version = "4.2.47";   // 06/04/03 - dfm_dist.cpp - Update agorithm to skip same nets to not exclude things w/o nets - AC
   //Version = "4.2.46";   // 06/04/03 - Case #177 - Mneutin.cpp - ".UNNAMEDNET" command in neutral.in is not implemented - LP
   //Version = "4.2.45";   // 06/04/03 - case #180 - Report.cpp - centroid rotation in Report->Spreadsheets->Components should be between 0 to 359 - LP
   //Version = "4.2.44";   // 06/03/03 - Case #148 - Accelin.cpp - Fixed mirroring of FreePads - TLS
   //Version = "4.2.43";   // 06/03/03 - Case #16 - Refdes are not being displayed correctly - knv
   //Version = "4.2.42";   // 06/03/03 - Case #178 - port.cpp - Trying to insert ATT_PATH in the function OnFileImport while the keyword is not there - LP
   //Version = "4.2.41";   // 06/03/03 - Case #135 - Graph.cpp - RefreshInheritedAttributes function is not working correctly - LP
   //Version = "4.2.40";   // 06/03/03 - Case #181 - FATFIN.cpp - Package inserting padstack with the same name as package name, causing other function to go into infinite loop - LP
   //Version = "4.2.39";   // 06/02/03 - Edif200_in.cpp - Fixed DoIinstance() not to return -1 if there is no data struct because it is possible to no insert in DoIinstance() - LP
                           //          - open_cad.cpp - Fix a bug in OpenCadFile().  Check file->getBlock() because using its attribMap - LP
   //Version = "4.2.38";   // 05/30/03 - hp3070_o.cpp - case #143, some testpoints were missing when exporting hp3070 from allegro - SN
   //Version = "4.2.37";   // 05/29/03 - Edif300_in.cpp - Fix a case sensitive bug in the function figure() and figureGroup() - LP
   //Version = "4.2.36";   // 05/29/03 - Edif200_in.cpp & Edif300_in.cpp - Added code to make the string into upper case if it is not inside a pair of quotes - LP
                           //          - Sch_Lib.cpp - Make the name of the duplicate geometry all upper case - LP
   //Version = "4.2.35";   // 05/28/03 - Edif200_in.cpp/.h - Implemented the identification of tiedots, rippers, and offpage connectors - LP
   //Version = "4.2.34";   // 05/28/03 - Edif200_in.cpp/.h & Edif300_in.cpp - Make the creation of geometry name all upper case to prevent case sensitive - LP
   //Version = "4.2.33";   // 05/28/03 - Fixed case #131 - GENCADIN.cpp - Board OutLine on wrong side - SN
   //Version = "4.2.32";   // 05/28/03 - Fixed case #113 - Edif300_in.cpp - Text rotation - LP
   //Version = "4.2.31";   // 05/27/03 - LP
                           //          - Net_hilt.cpp - Fixed how NetExtents() return TRUE or FALSE
                           //          - Edif200_in.cpp & Edif300_in.cpp 
                           //             1) Undo adding "Netname" attribute to every port
                           //             2) Undo the removal of comppin of hierarchy symbol from netlist
                           //             3) Added master port on schematic sheet as comppin to netlist
                           //             4) Call the function ConvertLogicToPhysicalSchematic() whenever the schematic has hierarchy
                           //          - Sch_List.cpp 
                           //             1) Fix how OnTvnSelchangedNetTree() check if an item is a sheet or a comppin in Net tree
                           //             2) Fixed the display of sheet name as part of the node in  Instance Tree
                           //             3) Sorted every node in all the tree view in Schematic Navigator
                           //          - Sch_Lib.cpp/.h - FINALLY finished the implementation of the followings and ready for release:
                           //             1) Flattened the netlist hierarchy
                           //             2) clone a hierarchy sheets
                           //             3) make the designators and refname of each symbol insert unique
                           //             5) Changed function name from  "AddAttribToCompPin" to "RenameCompAndPinDesignator"
                           //             6) Make sure that the newly selected designator is unique
                           //             7) Add an attribute nameed "$Designator#2$" which ha
   //Version = "4.2.30";   // 05/20/03 - API.CPP & .H - SaveCompressedDataFileAs() API - DD
   //Version = "4.2.29";   // 05/20/03 - Edif200_in.cpp & Edif300_in.cpp - Fixed the way we classify INSERTTYPE_HIERARCHYSYMBOL - LP
                           //          - Sch_List.cpp - Fixed the way we filter out inserty of type INSERTTYPE_HIERARCHYSYMBOL - LP
   //Version = "4.2.28";   // 05/20/03 - Edif200.in.cpp/.h - LP
                           //          - Changed the way of adding the attribute "NetName" to every port
                           //          - Previous I forgot to call the function LoadSettingFile() to read the value of "OverrideAttributeDisplay"
   //Version = "4.2.27";   // 05/20/03 - Sch_List.cpp - Modified code to not display "Net has no extents" when click on the net node in the net tree - LP
                           //          - Edif300_in.cpp - Added code to get the setting "OverrideAttributeDisplay", which indicate wheather to overwrite the display location of an existing attribute
                           //          - Edif200_in.cpp/.h - LP
                           //             1) Added code to add the attribute "NetName" to every port
                           //             2) Added code to get the setting "OverrideAttributeDisplay", which indicate wheather to overwrite the display location of an existing attribute
   //Version = "4.2.26";   // 05/19/03 - Case#160 - api_attr.cpp - GetAttribValue() was missing a "break;" in the case statement - DD
   //Version = "4.2.24";   // 05/15/03 - extents.cpp, dfm_flat.cpp - Fixed flatten algorithm to include the compRefName attribute on tools - AC
   //Version = "4.2.23";   // 05/15/03 - Net_hilt.cpp & Net_hilt.h - Change the function NetExtents() to return BOOL - LP
                           //          - Edif300_in.cpp - Initialiize sCurrentLine[0] = '\0' - LP
                           //          - Case #149 - port.cpp - Importing a non-CC file ontop of the provided CC file will crash CAMCAD  due to using wrong data type for attribute keyword
                           //          - Case #156 - Sch_List.cpp -  Bring up Schematic Navigator will crash CAMCAD - LP
   //Version = "4.2.19";   // 05/13/03 - Change applied to many files - LP
                           //          1) Addded SCH_ATT_HIERARCHYGEOMNUM to Sch_Lib.h
                           //          2) Replaced "HierarchyGeomNum" with SCH_ATT_HIERARCHYGEOMNUM
                           //          3) Replaced ATT_REFDES_MAP to SCH_ATT_REFDES_MAP
                           //          4) Replaced ATT_PINNO_MAP to SCH_ATT_PINNO_MAP
                           //          5) Moved the following defines to Sch_Lib.h and rename them 
                           //                  LEFT_JUSTIFY            ==>  SCH_JUSTIFY_LEFT  
                           //                  RIGHT_JUSTIFY           ==>  SCH_JUSTIFY_RIGHT
                           //                  CENTER_JUSTIFY          ==>  SCH_JUSTIFY_CENTER 
                           //                  BOTTOM_JUSTIFY          ==>  SCH_JUSTIFY_BOTTOM 
                           //                  TOP_JUSTIFY             ==>  SCH_JUSTIFY_TOP 
                           //                  
                           //                  LEFT_PARENTHESIS        ==>  SCH_PARENTHESIS_LEFT
                           //                  RIGHT_PARENTHESIS       ==>  SCH_PARENTHESIS_RIGHT
                           //                  
                           //                  TEXT_LAYER              ==>  SCH_LAYER_TEXT
                           //                  PORT_DESIGNATOR_LAYER   ==>  SCH_LAYER_PORT_DESIGNATOR
                           //                  SYMBOL_DESIGNATOR_LAYER ==>  SCH_LAYER_SYMBOL_DESIGNATOR
                           //                  PORT_GRAPHIC_LAYER      ==>  SCH_LAYER_PORT_GRAPHIC
                           //                  SYMBOL_GRAPHIC_LAYER    ==>  SCH_LAYER_SYMBOL_GRAPHIC
                           //                  SIGNAL_GRAPHIC_LAYER    ==>  SCH_LAYER_SIGNAL_GRAPHIC
                           //                  BORDER_GRAPHIC_LAYER    ==>  SCH_LAYER_BORDER_GRAPHIC
                           //                  ATTRIBUTE_LAYER         ==>  SCH_LAYER_ATTRIBUTE
                           //                  
                           //                  CELL_KEYWORD            ==>  SCH_ATT_CELL
                           //                  DESIGNATOR_KEYWORD      ==>  SCH_ATT_DESIGNATOR
                           //                  DIRECTION_KEYWORD       ==>  SCH_ATT_DIRECTION
                           //                  INSTANCE_KEYWORD        ==>  SCH_ATT_INSTANCE
                           //                  PORTNAME_KEYWORD        ==>  SCH_ATT_PORTNAME
                           //                  DISPLAYNAME_KEYWORD     ==>  SCH_ATT_DISPLAYNAME         
                           //                  UNUNSED_KEYWORD         ==>  SCH_ATT_UNUNSED
                           //                  NETNAME_KEYWORD         ==>  SCH_ATT_NETNAME
                           //                  LIBRARYNAME_KEYWORD     ==>  SCH_ATT_LIBRARYNAME
                           //                  CELLNAME_KEYWORD        ==>  SCH_ATT_CELLNAME
                           //                  CLUSTERNAME_KEYWORD     ==>  SCH_ATT_CLUSTERNAME
                           //                  INSTANCENAME_KEYWORD    ==>  SCH_ATT_INSTANCENAME
   //Version = "4.2.18";   // 05/12/03 - Change applied to many files - LP
                           //             1) Change DELIMITER to SCH_DELIMITER
                           //             2) Change the delimiter from char(4) to char(32)
                           //             3) Added DOC_VERSION to keep track of new CAMCAD Doc version
                           //             4) Added statusUserCancel to indicuate that user canceled the reading of CC file
   //Version = "4.2.17";   // 05/09/03 - Added CPinLabelSettings member to SettingsStruct - knv
   //Version = "4.2.16";   // 05/09/03 - Edif200_in.cpp & Edif300_in.cpp  - Removed the usage of BLOCKTYPE_HIERARCHYSYMBOL because we don't need to - LP
                           //          - dbutil.cpp - Removed the "Hierarchy Symbol" from the array blocktypes[]  because there is no need for it - LP
                           //          - dbutil.h - Removed the BLOCKTYPE_HIERARCHYSYMBOL because there is no need for it - LP
                           //          - Sch_List.cpp - When a hierarchy symbol is clicked on the view, it will pan to the hierarchy symbol node in the sheet tree 
                           //                           Also net selection in both the view and the net tree works very nicely - LP
                           //          - Sch_List.h - Added the declaration  function SelectHierarchySymbol() - lP
   //Version = "4.2.11";   // 05/08/03 - Edif200_in.cpp - LP
                           //             1) Modified so that  the block is created at DoView() and the content of the block will be provide in DoContent() and DoSymbol()
                           //             2) Fixed a bug in GetNameDef()
                           //          - Sch_List.cpp - LP
                           //             1) Added a new "Hierarchy" icon to be used for "Hierarchy Symbol" in the sheet tree
                           //             2) Modified sheet tree to insert hierarchy nodes under the sheet if the sheet has hierarchy symbol,
                           //             and the hierarchy sheet will be that hierarchy symbol will be inserted under the hierarchy node
   //Version = "4.2.10";   // 05/08/03 - Added new buttons to the insert type toolbar - AC
                           //          - Added new icon 'H' (IDI_HIERARCHY) for schematic navigator
   //Version = "4.2.9";    // 05/07/03 - dbutil.cpp - Added "Hierarchy Symbol" to blocktypes array - LP
                           //          - dbutil.h - Added the BLOCKTYPE_HIERARCHYSYMBOL - LP
                           //          - Edif200_in.cpp & Edif300_in.cpp - LP
                           //             1) Modified AddAttribToCompPin() to remove comppin whose insert is INSERTTYPE_HIERARCHYSYMBOL
                           //             2) Correctly identify an insert as INSERTTYPE_HIERARCHYSYMBOL, INSERTTYPE_SYMBOL, or INSERTTYPE_UNKNOWN
                           //             3) Correctly identify a block as BLOCKTYPE_HIERARCHYSYMBOL
                           //          - Sch_List.cpp - LP
                           //             1) Removed the code that fill the net tree from OnTvnSelchangedNetTree() and put it into FillNetsTree()
                           //             2) Modified FillNetsTree() to only remove sheet node if there is not child under it.  Also to only call FillNetsTree() once at OnInitDialog
                           //             3) Modified so the sheet node in the net tree only shows the sheet name instead of the long geometry name
                           //             4) Modified IsCompPinOnThisSheet() to match the comppin->getRefDes() to insert->refname and also compRefdes to designator
   //Version = "4.2.8";    // 05/06/03 - Case #111 - VB Ascii out, multiline text - knv
   //Version = "4.2.7";    // 05/06/03 - Case #103 - DFT Clear access results issue - knv
   //Version = "4.2.6";    // 05/06/03 - Edif300_in.cpp - Modified the classification of insert type - LP
   //Version = "4.2.5";    // 05/05/03 - Case #15 - Poly to flash conversion needs to make the center of the complex apertures the origin. - knv
   //Version = "4.2.4";    // 05/05/03 - CASE #141 - Thedain.cpp & thedain.h - added support for probes and conductions at all levels - DF
   //Version = "4.2.3";    // 05/05/03 - Case #95 - Pin label dynamic sizing - knv   
   //Version = "4.2.2";    // 05/02/03 - make change to many file to change the item listed below - LP
                                       //Change the followings:
                                       //==============
                                       //INSERTTYPE_GATE ==> INSERTTYPE_SYMBOL
                                       //BLOCKTYPE_SCHEMATICPAGE ==> BLOCKTYPE_SHEET
                                       //BLOCKTYPE_GATE ==> BLOCKTYPE_SYMBOL
                                       //"Schematic Page" ==> "Schematic Sheet"
                                       //"LogicSymbol" ==> "Logic Symbol"
                                       //"PortInstance" ==> "Port Instance"

                                       //Added the followings insert type:
                                       //====================
                                       //INSERTTYPE_HIERARCHYSYMBOL
                                       //INSERTTYPE_PAGECONNECTOR
                                       //INSERTTYPE_TIEDOT
                                       //INSERTTYPE_RIPPER  
                                       //INSERTTYPE_GROUND
                                       //INSERTTYPE_TERMINATOR
   //Version = "4.2.1";    // 05/01/03 - Refix Case #128 - Edif200_in.cpp -  Fixed the horizontal justification when attribute is mirrored - LP
   //Version = "4.2.0";    // 04/30/03 - Edif200_in.cpp - Fixed the parsing of "rename" construct. Modified so that visible of figureGroup is by default TRUE - LP
   //Version = "4.2.0en";  // 04/30/03 - Case #120 - Rebug - ODB read removes file - knv 
                           //          - Case #126 - ODB++ read crash
                           //          - Fixed wrong file filter displayed when saving a .ccz file
   //Version = "4.2.0em";  // 04/30/03 - Edif300_in.cpp - Fixed port design location and comp/pin in netlist - LP
                           //          - Edif200_in.cpp & Edif300_in.cpp - Modified the error message when inserting a geometry that is undefined. - LP
   //Version = "4.2.0el";  // 04/30/03 - Sch_Link.cpp - Added support to use preset attributes choosen through the SelectAttribute dialog - AC
   //Version = "4.2.0ek";  // 04/29/03 - Edif300_in.cpp - Fixed a bug in the function InsertSchematicSymbol() which accidently set the block to UNKNOWN - LP
   //Version = "4.2.0ej";  // 04/29/03 - Case #39 - Added convertSmdViasToTestPoints() and convertViasToTestPoints() - knv 
   //Version = "4.2.0ei";  // 04/29/03 - Case #120 - ODB read removes file - knv 
   //Version = "4.2.0eh";  // 04/29/03 - Edif300_in.cpp - Fixed text rotation and size - LP
   //Version = "4.2.0eg";  // 04/28/03 - Case #33 - vb99out.cpp - Corrected Cell level RefDes Placeholder text issues - DF
   //Version = "4.2.0ef";  // 04/28/03 - Case #33 - vb99out.cpp - Corrected Drawing Cell outline handling and layout level Drawing Cell instances - DF
   //Version = "4.2.0ed";  // 04/28/03 - Case #33 - fix handling of cell outlines and text entries - knv
   //Version = "4.2.0ec";  // 04/28/03 - draw_ent.cpp - Fixed width calculation taking into account '%' CAMCAD escape character - AC
   //Version = "4.2.0eb";  // 04/28/03 - CASE #121 - accelin.cpp, accelin.h - Plated/Non-Plated drill hole support - DF
   //Version = "4.2.0ea";  // 04/28/03 - CASE #29 - hp3070.cpp - renaming nets - JS
   //Version = "4.2.0dz";  // 04/28/03 - Added Generate Centroid Dialog.
   //Version = "4.2.0dy";  // 04/28/03 - Updated TestPadDensityCheck API to take in a grid size - AC
   //Version = "4.2.0dx";  // 04/28/03 - CASE #114 - pads_out.cpp - poly lines don't get written(This bug was introduced fixing TSR #4048, so actually fixed TSR #4048) - JS 
   //Version = "4.2.0dw";  // 04/25/03 - dfm_flat.cpp - updated the flatten to support adding COMP_REFNAME attribute to tools and components - AC
                           //          - sch_link.cpp - added support to use REFDES_MAP attribute for schematic components
   //Version = "4.2.0dv";  // 04/24/03 - CASE #119 - ddein.cpp - changed ipl_txt handler for component text and added handler option in IN file, .ALT_COM_TXT - DF
   //Version = "4.2.0du";  // 04/24/03 - version = "4.1.105" - JS
   //Version = "4.2.0dt";  // 04/24/03 - Sch_List.cpp - Modified so that the instance tree will look for the attribute name specified by "REF_MAP" attribute of the file->getBlock()->AttributeMap, and use that attribute to popullate the reference designator value - LP
   //Version = "4.2.0ds";  // 04/24/03 - Case #33 - fix handling of DRAWING_CELL and add cell RefDes placeholder - knv
   //Version = "4.2.0dr";  // 04/24/03 - CAMCAD.RC, resource.h, Sche_Lib.cpp & Sch_Lib.h - Added a new dialog for Tools->Schematic->SelectAttribute - LP
                           //          - CASE #112 - Edif200_in.cpp - CAMCAD crash upon exit - LP
   //Version = "4.2.0dq";  // 04/22/03 - Case #27 - pcbutil.cpp - Implemented new ipc.in command, .unit_override, - knv
   //Version = "4.2.0dp";  // 04/22/03 - Added .ccz file recognition for double-click and drag-drop - knv
   //Version = "4.2.0do";  // 04/22/03 - version = "4.1.102" - JS
                           // 04/21/03 - CAMCAD.vsproj, CAMCAD.RC, resource.h - Modified the dialog IDD_SCHLINK_CHOOSEREF so that the title label and the message label can be change during run time. - LP
                           //          - Sch_Link.cpp & Sch_Link.h - Moved the class that like to the dialog IDD_SCHLINK_CHOOSEREF to here.  Used to be in Sch_Lib.cpp & Sch_Lib.h. - LP
                           //          - Sch_Lib.cpp & Sch_Lib.h - Newly added files; moved the class that like to the dialog IDD_SCHLINK_CHOOSEREF to here.  Used to be in Sch_Link.cpp & Sch_Link.h.
   //Version = "4.2.0dn";  // 04/21/03 - Case #60 Generate SMD Component is not working on ODB++ files. - knv
                           //          - Fixed problems with DFM Tests; TP Density, Pin 1 orientation, Compheight, Comps not allowed per surface.
   //Version = "4.2.0dm";  // 04/21/03 - version = "4.1.101" - JS
   //Version = "4.2.0dl";  // 04/18/03 - Added archive file reading functionality to ODB++ Reader - knv
   //Version = "4.2.0dk";  // 04/18/03 - Edif200_in.cpp - Mean to use INSTANCENAME_KEYWORD instance of INSTANCE_KEYWORD.  Also fixed case sensitive in construct callout command - LP
                                      // port.cpp - fixed a crash due to using a file->getBlock() that is invalid - LP
   //Version = "4.2.0dj";  // 04/18/03 - CASE #43 - pads_out.CPP - Added Pads Write support for Plated vs. Non-Plated drills. - DF
   //Version = "4.2.0di";  // 04/18/03 - version = "4.1.100" - LP
   //Version = "4.2.0dh";  // 04/18/03 - CASE #43 - Allegout.CPP - Added Allegro Write support for Plated vs. Non-Plated drills. - DF
   //Version = "4.2.0dg";  // 04/18/03 - Version = "4.1.99" - LP
   //Version = "4.2.0df";  // 04/17/03 - Version = "4.1.98" - DF
   //Version = "4.2.0de";  // 04/17/03 - Version = "4.1.97" - DF
   //Version = "4.2.0dd";  // 04/17/03 - Version = "4.1.96" - DF
   //Version = "4.2.0dc";  // 04/17/03 - Version = "4.1.95" - LP
   //Version = "4.2.0db";  // 04/17/03 - Lyrtype.cpp - Case #66 - Fixed bug viewing bottom and top while keeping the same view location and level - AC
   //Version = "4.2.0da";  // 04/17/03 - Version = "4.1.94" - JS
   //Version = "4.2.0cz";  // 04/17/03 - Case #88 - CompPin.cpp, CompPin.h - Updated the Component/Pin List to unhighlight the last net when a new net is highlighted - AC
                           // 04/17/03 - SAX.cpp - Added error message when the SAX Macro Editor fails during creation
                           // 04/17/03 - attrib.cpp - Updated HighlightByAttrib and UnhighlightByAttrib to highlight and unhightlight pins
   //Version = "4.2.0cy";  // 04/17/03 - Version = "4.1.93" - DF
   //Version = "4.2.0cx";  // 04/17/03 - Case #98 & #69 - Graph.cpp - CAMCAD crash when doing "Link Poly" with query dialog open - LP
   //Version = "4.2.0cw";  // 04/17/03 - Version = "4.1.92" - DF
   //Version = "4.2.0cv";  // 04/16/03 - Case #89 - panel outlines are now classed as GR_CLASS_PANELOUTLINE - knv
                           // 04/16/03 - Version = "4.1.91" - LP
   //Version = "4.2.0cu";  // 04/16/03 - Added functionality for generating pin inner outlines (2 pin components and components w/ pins in 2 rows) - AC
   //Version = "4.2.0ct";  // 04/16/03 - Version = "4.1.91" - LP
   //Version = "4.2.0cs";  // 04/16/03 - Changed .ccz file format to PKZIP .zip format - knv
   //Version = "4.2.0cr";  // 04/15/03 - CASE #19 - ddein.cpp & dde9_in.cpp - Fix the way DDE create circle in ".dfa" and ".pol" - LP 
                           // 04/15/03 - ddein.cpp & dde9_in.cpp - Also add support for plated/non-plated in ".pst" - LP
   //Version = "4.2.0cq";  // 04/16/03 - bom.cpp tolerancee refix - BI 
                           // 04/16/03 - Edif200_in.cpp - commented out LoadSettingFile() since we are currently not loading setting file - LP
   //Version = "4.2.0cp";  // 04/15/03 - Edif200_in.cpp !!! NEW Edif 200 Reader !!! - LP
   //Version = "4.2.0co";  // 04/15/03 - Case 7 bom.cpp allowing tolerance to read multiple formats - BI
   //Version = "4.2.0cn";  // 04/15/03 - Version = "4.1.90" - JS
   //Version = "4.2.0cm";  // 04/15/03 - Updated Outline generation dialog to create pin extents outline - AC
   //Version = "4.2.0cl";  // 04/14/03 - Added net comparison to use saved xref name attribute upon the first attempt - AC
   //Version = "4.2.0ck";  // 04/14/03 - Version = "4.1.89" - JS
   //Version = "4.2.0cj";  // 04/11/03 - Added saving of XREF_NAME to components and nets in the attributes - AC
                           //       - Removed duplicate values in the Schematic Link Refdes selection dialog
                           //       - Use the XREF_NAME attribute for the first comparison of components
   //Version = "4.2.0ci";  // 04/11/03 - Removed unconditional dialog display in ODB++ reader - knv
   //Version = "4.2.0ch";  // 04/09/03 - Added kill trailing suffix functionality - AC 
   //Version = "4.2.0cg";  // 04/07/03 - Version = "4.1.88" - JS
   //Version = "4.2.0cf";  // 04/08/03 - Added plated property to tools - AC
                           //       - Updated query and edit dialogs to support the plated property on tools
   //Version = "4.2.0ce";  // 04/07/03 - Version = "4.1.87" - JS
   //Version = "4.2.0cd";  // 04/07/03 - Updated Gerber Thermal Check property sheet - knv
                           //       - Updated Siemens writer to:
                           //         1) export only SMD components.
                           //         2) allow panel level parameters to be specified
                           //         3) generate the KLEBEN parameter
                           //       - Added .cczip file generation (still under development)
   //Version = "4.2.0cb";  // 04/07/03 - Version = "4.1.86" - JS
   //Version = "4.2.0ca";  // 04/03/03 - Updated SetWindowName API to set the window's title - AC
                           // Top and Bottom View retain their zoomed locations and levels
                           // PageUp now zooms into the center when at 1 to 1
   //Version = "4.2.0bz";  // 04/03/03 - Version = "4.1.85" - JS
   //Version = "4.2.0by";  // 04/03/03 - added board/panel fiducial support to Siemens writer - knv
   //Version = "4.2.0bx";  // 04/01/03 - Version = "4.1.84" - knv
   //Version = "4.2.0bw";  // 04/01/03 - Version = "4.1.82" - JS
   //Version = "4.2.0bv";  // 04/01/03 - Version = "4.1.81" - JS
   //Version = "4.2.0bu";  // 03/28/03 - Version = "4.1.80"
   //Version = "4.2.0bt";  // 03/28/03 - Added new API DistanceCheck2 (similar to DistanceCheck) that allow for ignoring elements on the same component (refdes) - AC
                           // Added new DFM API's ComponentNotAllowedPerSurface and ComponentHeight
   //Version = "4.2.0bs";  // 03/27/03 - Added SetWindowName API - AC
   //Version = "4.2.0br";  // 03/26/03 - Added Generate Pin 1 Marker - SN
   //Version = "4.2.0bq";  // 03/26/03 - Added Pin1Orientation and TestPadDensityCheck API - AC
   //Version = "4.2.0bp";  // 03/25/03 - Version = "4.1.77" - SN
   //Version = "4.2.0bo";  // 03/25/03 - Version = "4.1.76" - SN
   //Version = "4.2.0bn";  // 03/25/03 - Version = "4.1.75" - SN
   //Version = "4.2.0bm";  // 03/25/03 - Version = "4.1.74" - SN
   //Version = "4.2.0bl";  // 03/25/03 - Version = "4.1.73" - SN
   //Version = "4.2.0bk";  // 03/25/03 - Version = "4.1.72" - SN
   //Version = "4.2.0bj";  // 03/25/03 - Version = "4.1.71" - SN
   //Version = "4.2.0bi";  // 03/24/03 - Version = "4.1.70" - knv
   //Version = "4.2.0bh";  // 03/24/03 - Version = "4.1.69" - DF
   //Version = "4.2.0bg";  // 03/24/03 - Version = "4.1.68" - JS
   //Version = "4.2.0bf";  // 03/24/03 - Version = "4.1.67" - JS
   //Version = "4.2.0be";  // 03/21/03 - Version = "4.1.66" - AC
   //Version = "4.2.0bd";  // 03/21/03 - Version = "4.1.65" - knv
   //Version = "4.2.0bc";  // 03/21/03 - Version = "4.1.64" - DF
   //Version = "4.2.0bb";  // 03/21/03 - Version = "4.2.63" - LP
   //Version = "4.2.0ba";  // 03/21/03 - Version = 4.1.62" - DF
                           // 03/21/03 - allegout.cpp - PCB Translator: Allegro Script Write: Added ALLEGRO.OUT file command .WRITE_BOUNDARY and write boundary with net assign to autoPour.scr - DF
                           // 03/21/03 - allegout.cpp - PCB Translator: Allegro Script Write: Changed the target class/subclass for voids and fills with widths to a generated MANUFACTURING subclass - DF
                           // 03/21/03 - allegout.cpp - PCB Translator: Allegro Script Write: Write filled zero width ETCH class elements to new autoFill.scr - DF
   //Version = "4.2.0az";  // 03/21/03 - Version = "4.1.61" - AC
   //Version = "4.2.0ay";  // 03/20/03 - Version = "4.1.60" - 4055 - Ag_AOI_o.cpp - Agilent AOI Writer - rot increment command - SN
   //Version = "4.2.0ax";  // 03/19/03 - Added CC Zip functionality - knv
   //Version = "4.2.0aw";  // 03/19/03 - Version = "4.1.59" - Crash in Pan Insert Type fixed - SN
   //Version = "4.2.0av";  // 03/19/03 - Version = "4.1.58" - LP
   //Version = "4.2.0au";  // 03/19/03 - Version = "4.1.57" - JS
   //Version = "4.2.0at";  // 03/18/03 - won't load licenses if running through automation - AC
                           // added LoadSTDLicenses API
   //Version = "4.2.0as";  // 03/18/03 - Version = "4.1.56" - JS
   //Version = "4.2.0ar";  // 03/18/03 - Version = "4.1.55" - DF
   //Version = "4.2.0aq";  // 03/17/03 - added GerberThermalBarrelPlugCheck DFM API - AC
   //Version = "4.2.0ap";  // 03/17/03 - Version = "4.1.54" - AC
   //Version = "4.2.0ao";  // 03/17/03 - Version = "4.1.53" - LP
   //Version = "4.2.0an";  // 03/17/03 - Version = "4.1.51" - LP
   //Version = "4.2.0am";  // 03/17/03 - Version = "4.1.50" - LP
   //Version = "4.2.0al";  // 03/17/03 - Version = "4.1.49" - LP
   //Version = "4.2.0ak";  // 03/14/03 - Version = "4.1.48" - LP
   //Version = "4.2.0aj";  // 03/14/03 - Version = "4.1.47" - LP
   //Version = "4.2.0ai";  // 03/12/03 - Updated Gerber merge dialog to remove all highlights when closed - AC
   //Version = "4.2.0ah";  // 03/12/03 - Version = "4.1.46" - JS
   //Version = "4.2.0ag";  // 03/11/03 - Version = "4.1.44" - JS
   //Version = "4.2.0af";  // 03/11/03 - Version = "4.1.43" - JS
   //Version = "4.2.0ae";  // 03/10/03 - Fixed crash when marking of unused layer to watch out for holes in the layer list - AC
   //Version = "4.2.0ad";  // 03/10/03 - Version = "4.1.42"; - JS
   //Version = "4.2.0ac";  // 03/07/03 - Version = "4.1.41" - JS
   //Version = "4.2.0ab";  // 03/07/03 - api.cpp - Fixed problem with API licensing clearing out all license, including default ones - AC
   //Version = "4.2.0aa";  // 03/06/03 - Edif300_in.cpp - Fixed a bug when after teh first Edif file is loaded any subsequent file load will invisible - LP
   //Version = "4.2.0z";   // 03/05/03 - Edif300_in.cpp - Modify so that the first view that has sheet with component insert will be the showing view when there is more than one view - LP
   //Version = "4.2.0y";   // 03/05/03 - Edif300_in.cpp - Modified so that on schematic symbol geometries with port are typed as BLOCKTYPE_GATE, all port geometries are typed as BLOCKTYPE_GATEPORT, page and view geometries are typed as BLOCKTYPE_SCHEMATICPAGE, and everything else is typed as BLOCKTYPE_UNKNOWN - LP
   //Version = "4.2.0x";   // 03/05/03 - Version = "4.1.40" - LP
   //Version = "4.2.0w";   // 03/05/03 - Version = "4.1.39" - JS
   //Version = "4.2.0v";   // 03/04/03 - Version = "4.1.38" - LP
   //Version = "4.2.0t";   // 03/03/03 - Version = "4.1.36" - LP
   //Version = "4.2.0s";   // 03/03/03 - Version = "4.1.35" - JS
   //Version = "4.2.0r";   // 03/03/03 - ag_aoi_o.cpp - Fixed Agilent AOI writer to support layer types PASTE_TOP and PASTE_BOTTOM - AC
   //Version = "4.2.0q";   // 02/28/03 - Version = "4.1.34" - JS
   //Version = "4.2.0p";   // 02/27/03 - Version = "4.1.33" - AC
   //Version = "4.2.0o";   // 02/27/03 - Version = "4.1.32" - AC
   //Version = "4.2.0n";   // 02/26/03 - Version = "4.1.31" - AC
   //Version = "4.2.0m";   // 02/26/03 - Version = "4.1.30" - JS
   //Version = "4.2.0l";   // 02/25/03 - Version = "4.1.29" - LP
   //Version = "4.2.0k";   // 02/25/03 - Updated ODB++ Reader and supporting libraries, fixed compile warnings - knv
   //Version = "4.2.0j";   // 02/25/03 - Version = "4.1.28" - LP
   //Version = "4.2.0i";   // 02/25/03 - Version = "4.1.23" - AC
                           // Version = "4.1.27" - AC
   //Version = "4.2.0h";   // 02/25/03 - search.cpp - Fixed crash - SN
   //Version = "4.2.0g";   // 02/25/03 - extents.cpp, draw_ent.cpp - Fixed crash - SN
   //Version = "4.2.0f";   // 02/25/03 - lic.cpp - Fixed problem with array of license names - AC
   //Version = "4.2.0e";   // 02/21/03 - Version = "4.1.21" - SN
   //Version = "4.2.0d";   // 02/21/03 - Updated AOI writer to support new attributes created from inheriting ECAD data into gerber data - AC
   //Version = "4.2.0c";   // 02/21/03 - Version = "4.1.20" - SN
                           // Cadifin.CPP - ChangeAltName command fixed, creating correct geoms - SN  
   //Version = "4.2.0b";   // 02/21/03 - Version = "4.1.19" - LP
                           // ddein.cpp - fixed so that padshape on layer "GENERIC255" will be read in correctly - LP
   //Version = "4.2.0a";   // 02/21/03 - gerb_mrg.cpp, gerb_mrg.h - updated algorithm with progress bars - AC
                           // add sorting of entities to non-inserts, insert non-components, and then components - AC
   //Version = "4.2.0"     // 02/20/03 - enh #3835 - gencadin.cpp - plane to become not void if the plane is inside of other void plane - JS
   //Version = "4.2.0"     // 02/20/03 - Added new functionality to Inherit ECAD data into gerber data - AC
   //Version = "4.2.0"     // 02/20/03 - Cr5000.cpp - Fixed reading tokens with line feeds - SN
   //Version = "4.2.0"     // 02/18/03 - Ag_AOI_o.cpp - Fixed the SP50 output for Fiducials - SN
   //Version = "4.2.0"     // 02/14/03 - pads_out.cpp - Fixed the issue of making pin numbers consecutive and non-zeros and make sure the the highest pin number is the same as the number of pin on the component - LP
   //Version = "4.2.0"     // 02/14/03 - Ck.cpp - Need to initialize StrTab_RaiseCase = TRUE in check_init() so ".case_sensitive" command will work correctly - LP
   //Version = "4.2.0"     // 02/14/03 - TSR #3985 - QD_out.cpp - when machine origin is change, the panel level fiducial insert is not changed - LP
   //Version = "4.2.0"     // 02/13/03 - CAMCAD.cpp - when camcad is started hidden or minimized, the splash screen doesn't show - AC
                           // added api GetCAMCADVersion (API version 1.34)
   //Version = "4.2.0"     // 02/13/03 - TSR #3965, TSR #3982 - allegin.cpp - text does not show - JS 
   //Version = "4.2.0"     // 02/12/03 - lic.cpp, api.cpp - updated ListConstants to include licenses - AC
   //Version = "4.2.0"     // 02/12/03 - ChildFrm.cpp, ChldFrm.h - Added new derived child window frame class - AC
                           // Locking of camcad application will now not allow the user to resize the main window (it is movable)
                           // Locking of camcad document will not not allow the user to resize the docuement window (it is movable)
   //Version = "4.2.0"     // 02/11/03 - TSR #3951 - pfwin.cpp - Remove CREATELIBRARY command and derive geom when it is necessary. - JS
   //Version = "4.2.0"     // 02/10/03 - TSR #3828 - CAMCAD.cpp, open_cad.cpp - Allow send to and draggin and droping of files will use OpenCadFile - AC
   //Version = "4.2.0"     // 02/07/03 - TSR #3975 - hpglout.cpp - fixed bug with HPGL not writing arcs - AC
                           // TSR #3976 - hpgl.in - updated to reflect CHAR_SPACERATIO command to CAMCAD's default - AC
                           // graph.cpp - fixed Graph_Text to default line and text alignment to 0 - AC
   //Version = "4.2.0"     // 02/03/03 - TSR #3969 - allegin.cpp - added new command to allegro.out "WRTIE_THERMAL_APR" - AC
                           // ddein.cpp - fixed error in get_height when height value is zero with no units
   //Version = "4.2.0"     // 02/03/03 - Updated the version in the resources to 4.2.0.0
   //Version = "4.2.0"     // 02/03/03 - TSR #3904 - fixed bug with snap to entity not working on mirror designs or while in bottom view - AC
   //Put into 4.1.0 on 01/31/03
                           // "4.1.0"  // 01/31/03 - Bug fix - Edif300_in.cpp - after reading Edif300 file with syntax error, other Edif300 file read into CAMCAD as blank.  The cause is probably of unfreed memory when the read exist because of syntax error. - LP
   //Version = "4.2.0"     // 01/30/03 - ENH #3756 - CR3000IN.cpp - support reading of PMA file - LP
   //Version = "4.2.0"     // 01/28/03 - TSR #3939 - pads_out.h - show drill size and make pin numbers consecutive and non-zero. - JS
   //Put into 4.1.0 on 01/31/03
                           // "4.1.0"  // 01/27/03 - TSR #3958 - Edif300_in.cpp - CAMCAD reports a syntax error when importing Edif300 file.  Rewrite error message so it makes more sense.  Also added PropertyOverride command to SchematicInstanceImplementation - LP
   //Version = "4.2.0"     // 01/22/03 - TSR #3953 - QD_out.cpp - issue with refdes on the board in Siemen writer - LP
   //Version = "4.2.0"     // 01/22/03 - TSR #3939 - pad_out.cpp - If a pad in padstack of pin is complex, it's always made with copper and tied to a pin.  vb99in.cpp - if the line width is not defined, create zero width aperture to draw the line - JS
   //Version = "4.2.0"     // 01/21/02 - TSR #3949 - Allegin.cpp - Text rotation issue - LP  
   //Version = "4.2.0"     // 01/21/02 - TSR #3947 - Edid_in.cpp & edif_in.h - unable to load Edif 200 file into CAMCAD correctly - LP
   //Version = "4.2.0"     // 01/17/02 - TSR #3915 - moved the board & panel thickness from QD.OUT file onto output dialog - LP
   //Version = "4.2.0"     // 01/17/03 - TSR #3939 - pads_out.cpp - change piece type to copper and show pin numbers - JS
   //Version = "4.2.0"     // 01/16/03 - TSR #3945 - Report.cpp - remove not yet implemented reports from list - LP
   ////////////////////////////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////////////////////////////
   //Version = "4.1.105"; // 04/24/03 - Case #40 - gencad_o.cpp - Implement support for mechanical and unknown insert and geometry types in GenCAD output. - JS
   //Version = "4.1.104"; // 04/23/03 - Case #60 - ODB++, Generate SMD Component - knv
   //Version = "4.1.103"; // 04/22/03 - CASE #27 - pcbutil.cpp - Implemented new ipc.in command, .unit_override, - knv
   //Version = "4.1.102";  // 04/22/03 - CASE #101 - pcbutil.cpp - The pins on the bottom pad are showing on the top when viewing pin numbers - JS
   //Version = "4.1.101";  // 04/21/03 - CASE #67 - ccm_out.cpp - fiducials have features top and bot but outputted as TOP - JS
   //Version = "4.1.100";  // 04/18/03 - CASE #45 - vb99in.cpp - Case sensitive cause geometries not to match - LP
   //Version = "4.1.99";   // 04/18/03 - CASE #14 - mentorin.cpp - Replace the command ".CHECK_CREATE_COMPONENT" with ".IGNORE_MISSING_GEOM_ENTRIES".  
                                                             // If set to "N" and there are component that try to insert an undefine geometry, then the import will stop
                                                             // If set to "Y", then it will log the error, continue to import, and remove the component pin entry from netlist - LP
                                              // Allegout.cpp - when writing netlist scrip if an inserted geometry is type UNKNOWN, then remove comp pin from netlist.
                                                             // if any other type then set the geometry's type to PCBCOMPONENT - LP
   //Version = "4.1.98";   // 04/17/03 - CASE #108 - mentorin.cpp - Fixed unplaced comp issue where unplaced comps without attribs were not placed, controlled by .MAKE_PLACEMENT command in mentor.in - DF
   //Version = "4.1.97";   // 04/17/03 - CASE #107 - mentorin.cpp - Added support in Mentor Read for RefDes ending in '-' - DF
   //Version = "4.1.96";   // 04/17/03 - CASE #83 & 105 - Allegout.cpp - Commented out Fix for #83 (allegro error reading correct script) and modified location and method for Fix for #105, all good - DF
   //Version = "4.1.95";   // 04/17/03 - CASE #100 - mentorin.cpp - CAMCAD uses large ammount of memory loading Tech file - LP
   //Version = "4.1.94";   // 04/17/03 - CASE #6 - sm_anal.cpp - SOLDERMASK ANALYZER | PREPARE doesn't work - JS
   //Version = "4.1.93";   // 04/17/03 - CASE #102 - Ciiin.cpp - Added option to in file to load record entries 382 as PCB componets, not testpoints, for PCB translation - DF
   //Version = "4.1.92";   // 04/17/03 - Case #104 - Cadifin.cpp & Cadifin.h - Added DO_ISLAND command to cadif.in and handling for "island" in .h and .cpp - DF
                     // 04/17/03 - CASE #105 - Allegout.cpp - added special text string handling for ` as last character in string due to Allegro error - DF
   //Version = "4.1.91";   // 04/16/03 - Case #84 - Allegout.cpp - Export path changes when different location for ALLEGRO.OUT is selected - LP                      
                           // 04/16/03 - CASE #83 - Allegout.cpp - Export the character ', ", and \ incorrectly in Allege Write - LP
   //Version = "4.1.90";   // 04/15/03 - Case #10 - refix - change y location for multi line text - JS
   //Version = "4.1.89";   // 04/10/03 - Case #12 - refix - JS
   //Version = "4.1.88";   // 04/10/03 - Case #54 - refix
                  //       - Case #13 - vb99out.cpp - SMD bottom only padstacks and pin instances - JS
   //Version = "4.1.87";   // 04/07/03 - Case #54 - vb99out.cpp - empty placement outline
                        // - Case #10 - TSR #4024 had a bad file and changed cadifin.cpp according to the bad file, so went back to the previous version - JS
   //Version = "4.1.86";   // 04/07/03 - Case #10 - Cadifin.cpp - Fatal parsing error reading text record - JS
   //Version = "4.1.85";   // 04/01/03 - Case #12 - Case #12 - Orcltdin.cpp, Orcltdin.h - Orcad read Components not being mirrored correctly - JS
   //Version = "4.1.82";   // 04/01/03 - Case #56 - DDE read crash ddein.cpp - JS
   //Version = "4.1.81";   // 04/01/03 - Case #51 - Gencad read crash - JS
   //Version = "4.1.80";   // 03/27/03 - PadsLibIn.cpp, lic.cpp, lic.h, filetype.cpp - Added Pads Library (.d) Read for PCB and PRO - DF
   //Version = "4.1.79";   // 03/26/03 - Added Gerber Thermal DFM check - knv
   //Version = "4.1.78";   // 03/25/03 - Added new functionality to Inherit ECAD data into gerber data (02/20/03 - Version 4.2.0) - AC
   //Version = "4.1.77";   // 03/25/03 - TSR #4066 -GenCam write crash - SN
   //Version = "4.1.76";   // 03/25/03 - TSR #4013 -Remove the CCT Reader - SN
   //Version = "4.1.75";   // 03/25/03 - TSR #4015 -Resources - Remove Generate BOM Graphic from the menu - SN
   //Version = "4.1.74";   // 03/25/03 - TSR #4048 -pads_out.cpp - pads out closed polys with 2 vertices - SN
   //Version = "4.1.73";   // 03/25/03 - TSR #4063 -LIC.h - Pads caption in export list - SN
   //Version = "4.1.72";   // 03/25/03 - TSR #4063 -HP5DX.cpp - Panel Name max character size - SN
   //Version = "4.1.71";   // 03/25/03 - TSR #4052 -sm_anal.cpp - Solder Mast Analysis -> Prepare bug fixed - SN
    //Version = "4.1.70";   //03/24/03
   //Version = "4.1.69";   // 03/24/03 - TSR #4062 -allegout.cpp - Corrected script output for board level drills in versions after 14.0. - DF
   //Version = "4.1.68";   // 03/24/03 - TSR #4050 - mentorin.cpp - Geometries imported with pads missing. - JS
   //Version = "4.1.67";   // 03/24/03 - TSR #4041 - gerout.cpp - complex apertures are not translated correctly. - JS
   //Version = "4.1.66";   // 03/21/03 - added error message to inform the user that no intellegent data will be read (give user the choice to continue or quit) - AC
   //Version = "4.1.65";   // 03/21/03 - TSR #4057 - ODB++ Read: boards are offsetted away from the drawing - knv
   //Version = "4.1.64";   // 03/21/03 - TSR #4058 - vb99out.cpp - Added check_name to component pin netname assignment and to component pin padstack assignment - DF
   //Version = "4.1.63";   // 03/21/03 - Refix TSR #4016 - hp3070_o.cpp - 3070 export: Single pin-single sided features are being marked as "UNRELIABLE" after running DFT - LP
   //Version = "4.1.62";   // 03/21/03 - allegout.cpp - PCB Translator: Allegro Script Write: Removed Complex Padshape x and y offset reassignment based on the x and y min and max - DF
                           // 03/21/03 - allegout.cpp - PCB Translator: Allegro Script Write: Fixed Padstack Script generation for SMD padstacks with BOTTOM only pads - DF
                           // 03/21/03 - allegout.cpp - PCB Translator: Allegro Script Write: Changed Complex Padshape scripts to .scx extension, updated autoshp.bat to include extension - DF
                           // 03/21/03 - allegout.cpp - PCB Translator: Allegro Script Write: Changed Complex Padshape name checking from 's' (geometry) to 'f' (flash) to correct name collision - DF
   //Version = "4.1.61";   // 03/21/03 - lic.cpp - Added OrCAD, PFW write into Graphic and removed HP EGS read - AC
                           // port_lib.cpp - Defaulted IPC write to only write 350 in Graphic
                           // xml_cont.cpp - Remove reading of Netlist and Typelist section and NETNAME attributes
   //Version = "4.1.60"    //03/20/03 4055 - Ag_AOI_o.cpp - Agilent AOI Writer - rot increment command - SN
   //Version = "4.1.59"    // 03/219/03 - Crash in Pan Insert Type fixed - SN
   //Version = "4.1.58";   // 03/19/03 - TSR #3988 - ta_optn.cpp - DFT: When the defined feature size is smaller then the actual pad size, DFT gives no access - LP
   //Version = "4.1.57";   // 03/19/03 - TSR #4020 - Mneutin.cpp - VIAS at same X Y wrong via gets TEST attribute - JS
   //Version = "4.1.56";   // 03/18/03 - TSR #4027 - gencam_o.cp - fiducials not coming through - JS
   //Version = "4.1.55";   // 03/18/03 - TSR #3999 - Allegout.cpp - CC PCB Translator: Symbol extends must include origin for Allegro write - DF
                           // 03/18/03 - TSR #3995 - Allegout.cpp - PCB Translator; Allegro Script Write: REF DES and DEVICE subclass - DF
                           // 03/18/03 - TSR #3991 - Allegout.cpp - PCB Translator; Allegro Script Write: Complex pad shape names - DF
   //Version = "4.1.54";   // 03/17/03 - edit.cpp - fixed edit dialog to show the correct width of poly lines in the width combo box - AC
   //Version = "4.1.53";   // 03/17/03 - TSR #4016 - hp3070_o.cpp - 3070 export: Single pin-single sided features are being marked as "UNRELIABLE" after running DFT - LP
   //Version = "4.1.52";   // 03/17/03 - TSR #4040 - ODB++ read: Pads are misaligned in the geometry. - knv
   //Version = "4.1.51";   // 03/17/03 - TSR #4017 - Accel/PCAD 200x Read: Board level inserts of geometry name=""
   //Version = "4.1.50";   // 03/17/03 - TSR #4044 - Allegout.cpp - Allegro writer not writing used complex pad shape scripts and autoshp.bat entries - LP
   //Version = "4.1.49";   // 03/17/03 - TSR #4043 - dde9_in.cpp - DDE read: CAMCAD crashes upon import because of reference to undefined layer number - LP
   //Version = "4.1.48";   // 03/14/03 - editpoly.cpp & polylib.cpp - Fixed the issue of the shrink/expand graphic being drawn on the wrong datalist - LP
   //Version = "4.1.47";   // 03/14/03 - Edif300_in.cpp - Modified so the read will output more meaningfull error message to the user - LP
   //Version = "4.1.46";   // 03/12/03 - TSR #4023 - hpglin.cpp - refix - JS
   //Version = "4.1.44";   // 03/11/03 - TSR #4033 - gerbin.cpp - crash while reading - JS
   //Version = "4.1.43";   // 03/11/03 - TSR #4023 - hpglin.cpp - big filled polygon shows because EP command didn't have correct polygon list. - JS
   //Version = "4.1.42";   // 03/10/03 - TSR #4024 - cadifin.cpp - needs to handle backslashes in netnames - JS
   //Version = "4.1.41";   // 03/07/03 - TSR #4022 - pcbutil.cpp - reported as pads reader, but the problem exists when optimizing padstack - JS
   //Version = "4.1.40";   // 03/05/03 - TSR #4000 - Edif300_in.cpp - Not all global port names are display - LP
   //Version = "4.1.39";   // 03/05/03 - TSR #4014 - gerbin.cpp - extra aperture added by misplacing parenthesis in if statement - JS
   //Version = "4.1.38";   // 03/04/03 - Refix TSR #3986 - Mneutin.cpp - Buried via missing drill - LP
   //Version = "4.1.36";   // 03/03/03 - Allegout.cpp - Remove the the output of "FORM mini offsetx 0.00" for mechanic pins per Dean's instruction - LP
   //Version = "4.1.35";   // 03/03/03 - TSR #4010 - gerbin.cpp - SR(step and repeat) doesn't work correctly. Creating wrong geometry - JS   
   //Version = "4.1.34";   // 02/28/03 - TSR #4001 - gerbin.cpp - unable to read complex apertures - JS
   //Version = "4.1.33";   // 02/27/03 - camcad.rc - Removed Add DRC Marker menu option - AC
   //Version = "4.1.32";   // 02/27/03 - select.cpp - Added partnumber to status bar when a part number exists on the selected item - AC
   //Version = "4.1.31";   // 02/26/03 - TSR #3997 - bitmap.cpp - extended support to write bitmaps while running in graphics mode other than 256 colors - AC
   //Version = "4.1.30";   // 02/26/03 - TSR #3977 - mentorin.cpp, extents.cpp - right reading for components on bottom, manuf refdes places refdes wrong cause of wrong block extents - JS
   //Version = "4.1.29";   // 02/25/03 - TSR #4008 - Edif300_in.cpp - CAMCAD crashes when reading this EDIF 300 file - LP
                           // 02/25/03 - FLEXlm.cpp - update license path logic - LP
   //Version = "4.1.28";   // 02/25/03 - TSR #3994 & 4005 - allegout.cpp - layer priority for padstack & SMD padstack generation - LP
   //Version = "4.1.27";   // 02/25/03 - dfx_outl.cpp - Updated Pin centers outline to support 2 and 1 pin components - AC
   //Version = "4.1.26";   // 02/25/03 - search.cpp - Fixed crash - SN
   //Version = "4.1.25";   // 02/25/03 - TSR #4006 - extents.cpp, draw_ent.cpp - Crash Fixed - SN
   //Version = "4.1.23";   // 02/23/03 - TSR #3941 - dfx_outl.cpp - re-added generation of pin centers outline - AC
   //Version = "4.1.21";   // 02/21/03 - TSR #4004 - QD_out.CPP, port_lib.cpp, format_s.h - Fiducial number fixed - SN
   //Version = "4.1.20";   // 02/21/03 - TSR #4002 - Cadifin.CPP - ChangeAltName command fixed, creating correct geoms - SN
   //Version = "4.1.19";   // 02/21/03 - TSR #3993 - ALLEGOUT.CPP - Allegro Script Write: Pin name text locations for component pins - LP
   //Version = "4.1.18";   // 02/20/03 - TSR #3980 - Ipc_out.cpp - IPC D-352A is outputing incorrect pad size - LP
   //Version = "4.1.17";   // 02/19/03 - TSR #3986 - Mneutin.cpp - Blind/buried via definition is incorrectly read in Mentor Neutral Reader - LP
   //Version = "4.1.16";   // 02/19/03 - ENH #3996 - ddein.cpp - DDE v8 Read: complex pad shape is incorrect - LP
   //Version = "4.1.15";   // 02/19/03 - ENH #3992 - ddein.cpp - DDE V8 Read: pad shapes and padstacks with the same names - LP
   //Version = "4.1.14";   // 02/19/03 - Refix TSR #3970 - ddein.cpp - modified so that ".LAYERATTR" will also assign layer type to layers defined in lpm section - LP
   //Version = "4.1.13";   // 02/18/03 - TSR #3989 - mentorin.cpp - number of pins is truncated in Mentor Boardstation Read due to max number of pin set to 1000 - LP
   //Version = "4.1.12";   // 02/18/03 - Ag_AOI_o.cpp - Fixed SP50 output for fiducials - SN
   //Version = "4.1.9";    // 02/07/03 - mainfrm.cpp - Custom toolbars no longer show up be default - AC
   //Version = "4.1.8";    // 02/07/03 - TSR #3970 & #3971 - ddein.cpp - Through hole & SMD padstack is incorrect in DDE Read - LP
   //Version = "4.1.7";    // 02/05/03 - TSR #3974 - hp3070_o.cpp - fixed crash in handling via with no pad stack definition - AC
   //Version = "4.1.6";    // 02/05/03 - TSR #3967 - sm_anal.cpp - rotation issue with "Explode to Soldermask Layers" - LP
   //Version = "4.1.5";    // 02/05/03 - EHN #3887 - blockdlg.cpp, ccview.h, ccview.cpp - added functionality to return back to zoom and pan state after editing a geometry - AC
   //Version = "4.1.4";    // 02/04/03 - CAMCAD.RC - remove the textboxes for board & panel thickness that are added by mistake - LP
   //Version = "4.1.3";    // 02/04/03 - TSR #3966 - added references to F7 as shortcut key for redraw on toolbars and menus - AC
   //Version = "4.1.2";    // 02/04/03 - TSR #3949 - Allegin.cpp - Text rotation issue - LP
                           // 02/04/03 - TSR #3953 - QD_out.cpp - issue with refdes on the board in Siemen writer - LP
   //Version = "4.1.1";    // 02/04/03 - TSR #3939 - pads_out.cpp - change piece type to copper and show pin numbers
                                    // - vb99in.cpp - if the line width is not defined, create zero width aperture to draw the line- JS
                           // 02/04/03 - Enh #3924 - gencad_out.cpp implement support for generic_insert, mechanical_insert, and unknown_insert - JS

   //Version = "4.1.0"
   ////////////////////////////////////////////////////////////////////////////////////////////////////


   ////////////////////////////////////////////////////////////////////////////////////////////////////
   //Version = "4.1.0"     // 01/31/03 - TSR #3962 - APREAD.CPP - CAMCAD can't read aperture file because the value of GLOBAL_APMIN & GLOBAL_APMAX were not initialized - LP
   //Version = "4.1.0"     // 01/31/03 - Removed various modules for each CAMCAD application - AC
                           // Updated the Access code dialog with a message about the rescan button - AC
   //Version = "4.1.0"     // 01/31/03 - 1) Fixed TSR #3958 - CAMCAD reports a syntax error when importing Edif300 file.  Rewrite error message so it makes more sense.
                           //            2) Also added PropertyOverride command to SchematicInstanceImplementation
                           //            3) Fixed bug - after reading Edif300 file with syntax error, other Edif300 file read into CAMCAD as blank.  The cause is probably of unfreed memory when the read exist because of syntax error.
   //Version = "4.1.0"     // 01/30/03 - Bgrndbmp.cpp, xml_cont.cpp - Fixed error ignoring the show flag for background bitmaps in the CC file - AC
                           // xml_wrt.cpp - Fixed problem with saving background bitmap information - AC
   //Version = "4.1.0"     // 01/27/03 - TSR #3960 - Ccm_out.cpp - CAMCAD crash during CCM export - LP
   //Version = "4.1.0"     // 01/24/03 - TSR #3955 - Padsin.cpp & Padsin.h - PADS reader crash CAMCAD because memory was overwriten due to array out of bounce - LP
   //Version = "4.1.0"     // 01/22/03 - TSR #3867 - Ck.cpp, CK.h, & hp3070_o.cpp - writing to 3070 give error because SbTab_MAX is reached - LP
   //Version = "4.1.0"     // 01/21/03 - open_cad.cpp - Updated open cad function to check for schematic link before continuing - AC
                           // api.h, api_sch.cpp, sch_link.cpp - Added new api's - AC
                           // SetSchematicNetCrossProbe
                           // SetSchematicComponentCrossProbe
                           // SetSchematicSheet
                           // LoadSchematicNetCrossReference
                           // LoadSchematicComponentCrossReference
                           // CrossProbeSchematicComponent
                           // CrossProbeSchematicNet
                        // attrib.cpp - updated UnhighlightByNet to work with individual nets - AC
                        // added menu options for the schematic toolbar - AC
   // 01/15/03 - TSR #3933 - file.cpp - DRC location need to be update when "Restructure File Around Orignal" is called - LP
   // 01/15/03 - enh #3931 - pcbutil2.cpp - normalize the text rotation when making manufacturing refdes - JS
   // 01/15/03 - Fixed text rotation and alignment
   // 01/14/03 - TSR #3938 - vb99out.cpp - VBASCII Write: Mechanical Cell instances in "netlist.kyn" and "PDB.HKP" - SN
   // 01/14/03 - TSR #3942 - vb99out.cpp - VBASCII Write: New GLOBAL_CLEARANCES section added - SN
   // 01/14/03 - TSR #3937 - vb99out.cpp - VBASCII Write: ROUND_DONUT pad shapes written incorrectly - SN
   // 01/14/03 - TSR #3929 - fatfin.cpp - FATF IN layer mirroring fixed - SN
   // 01/13/03 - Removed ODB++ Write and FAB Master Write module - AC
   // 01/10/03 - TSR #3910 - draw_ent.cpp - Fixed calculation of text width - AC
                  // Fixed GetLicense macro (added parenthesis around conditional macro)
   // 01/09/02 - Added new API OpenCadFile (API Version 1.33)- AC
   // 01/09/02 - TSR #3935 - Accelin.cpp - "Fatal parsing error" in the ASCIIHEADER section fixed. - SN
   // 01/09/02 - TSR #3934 - License.cpp - Camcad crashes when clicking on "Access Codes" - SN
   // 01/09/02 - Modified the tile of a message from "Orcad Settings" to "Edif 300 Settings" in the function LoadSettingFile - LP
   // 01/08/03 - TSR #3894 - change the command to accept two arguments .MACHINE_ORIGIN_TOP   [arg1]  [arg2] where 
                  // arg1 = the string "GEOM" or "REF" - this tells CAMCAD what kind of value to expect in argument 2
                  // arg2 = the geometry name or reference designator name - JS
   // 01/08/03 - TSR #3920 - when there are two patternGraphicsNameRefs defined, camcad was always taking the last one.  
                  // Now takes what is defined for patternGraphicsNameRef. - JS
   // 01/07/03 - xml_wrt.cpp - Re-added width on visible attributes (accidentally removed) - AC
   // 01/07/03 - VB ASCII Writer- vb99out.cpp - Errors writing arcs where fixed. - SN
   // 01/07/03 - TSR #3927 - ta-dlg.cpp - Access Marker is placed wrong when component is mirrored - LP
   // 01/06/03 - TSR #3903 - xml_wrt.cpp - to print 6 digits after decimal point and cut any trailing zeros. - JS
   // 01/03/03 - pcbutil.cpp - TSR #3925 - padstack identified as BLIND when it should be SMD - LP
   // 01/02/03 - mentorin.cpp - Updated Mentor reader to filter out heirarchical netnames in the trace file correctly for NET, ARE, and FIL lines - AC
   // 01/02/03 - TSR #3894 - qd_out.cpp - machine_origin command inspects all geometries with refname - JS
   // 12/31/02 - trdn73_0.cpp - Enhancement - Teradyne 7300 AOI Rotation Enhancement - SN
   // 12/26/02 - GENCADIN.CPP - TSR # 3929 - Gencad Reader - modifying the gencad.in Command "forcetext2silk" - SN
   // 12/26/02 - qd_out.cpp - ENH 3913 - siemens writer - added SUPPRESS_BOARDINFO command in qd.out - JS
   // 12/20/02 - Orcltdin.cpp - TSR #3906 - Crash read file due to reference memory that is overwritten by something else - LP
   // 12/18/02 - Accelin.cpp - Fixed the function get_attrmap() so that attribute the appropriate key in order to bring attributes into CAMCAD correctly - LP
   // 12/18/02 - mentorin.cpp - Fixed the read to recognize string inside single quote as one string so that attribute value is read correctly - LP
   // 12/18/02 - Modified so that instance name and port name are assign to designator by default.  Also added the command ".Not_A_Logic_Symbol" to Edif300.in file - LP
   // 12/18/02 - TSR #3912 - Draws the dxf incorrectly.  y value in LINE and extrusion in arc was not working correctly.  The changes are not coming from dxf spec.  It's coming from assumption and tested with couple of files and it looks correct. - JS
   // 12/18/02 - TSR #3876 - implemented ".REVERSE_ORDER" command to reverse order or electrical stack number and layer type if the command is "Y" or "y" - LP
   // 12/17/02 - TSR #3900 - Atrblist.cpp - overwrite all button now works correctly - AC
   // 12/17/02 - TSR #3916 - port_lib.cpp - make the GLUE boxes checked as default - LP
   // 12/17/02 - TSR #3901 - pcbutil.cpp - Polys to Flashes is not making flashes correctly - LP
   // 12/17/02 - TSR #3902 - Settings.cpp - setting the decimal place textbox to readonly and the scroll between 0 to 6 only - LP
   // 12/17/02 - TSR #3908 - hpglin.cpp - Loading in HPGL file multiple times causes the Y scale to shrink. Also, text is not coming in correctly. - JS
   // 12/17/02 - TSR #3897 - implement the command ".ATTACH2NET" for Attribute/Bom Read in bom.cpp - LP
   // 12/16/02 - TSR #3871 regarding not visible text & TSR #3868 which added cell name display, instance name display, and port direction attributes, Edif300_in.cpp & Edif300_in.h - LP
   // 12/16/02 - portNameDisplay and textHeight, Edif300_in.cpp - LP
   // 12/16/02 - TSR #3911 - gencad_o.cpp - Component level attributes are only written to the DEVICE section - JS 
   // 12/16/02 - TSR #3909 - QD_out.cpp - Rotations in Siemens output have one extra zero attached. - JS
   // 12/13/02 - Added implementation of Master Port and fixed a bug in creating polys in Edif300_in.cpp - LP
   // 12/13/02 - Modified the call to InsertSchematicGeometry to pass in InstanceRef instead of ImplementationName for Refdes in Edif300_in.cpp - LP
   // 12/13/02 - TSR #3813 - vb99out.cpp reopen bug : Net names are not being written to the Component Pins in the Layout.HKP file - JS 
   // 12/12/02 - TSR #3895 - QD_out.cpp (siemens writer) - Siemens chk file commands not being interpreted - JS
   // 12/12/02 - Refix TSR #3830 - CAMCAD crash because padstack inserts padshape that has the same geometry name, ddein.cpp - LP
   // 12/11/02 - TSR #3878 - #3881 & #3884 - Fixed text rotation and alignment in Edif300_in.cpp - LP
   // 12/11/02 - TSR #3889 - mechanical cells have no contents
               // TSR #3890 - mechanical pins not supported by writer
               // TSR #3891 - fiducial not supported by writer - JS
   // 12/06/02 - TSR #3873 - mentor board write - through via incorrectly comes out as buried - JS
   // 12/03/02 - TSR #3830 - original padstack name seem to be lost in translation in ddein.cpp - LP
   // 12/03/02 - TSR #3831 - allegro extract read - some attributes for components are getting mixed up. - JS
   // 11/25/02 - TSR #3064 - ipclib.cpp, ipc_out.cpp, ipclib.h CC IPC-356A Writer - Output is not being sorted according to specs.
   //Version = "4.0.32";   // 11/21/02 - New mode added to Agilent AOI writer (SP 50). In Ag_AOI_o.cpp - SN
   //Version = "4.0.31";   // 11/21/02 - TSR #3862 - Gerber is using the wrong widthindex in Gerbin.cpp - LP
   //Version = "4.0.30";   // 11/21/02 - TSR #3864 - surface of XOUT insert is incorrect in mneutin.cpp - LP
   //Version = "4.0.29";   // 11/20/02 - TSR #3856 - Crash during Make Manufacturing Refdes in pdbutil2.cpp - LP
   //Version = "4.0.28";   // 11/20/02 - TSR #3850 - output single quote character in attribute value is not acceptable in Allegout.cpp - LP
   //Version = "4.0.27";   // 11/20/02 - TSR #3829 - single quoted text is not bring in correctly in DDE V8 Read - LP
   //Version = "4.0.26";   // 11/20/02 - TSR #3846 - rename .P_ADDED_PART_NAME to .PANEL_MIRRORED_BOARD_NAME in Mneutin.cpp - LP
   //Version = "4.0.25";   // 11/19/02 - TSR #3849 - via padstack is being overwrite by pin padstack in Padsin.cpp - LP
   //Version = "4.0.24";   // 11/19/02 - TSR #3815 refix - renamed device type is not define when export to PADS - LP
   //Version = "4.0.23";   // 11/18/02 - TSR #3845 - panal added part as fiducial was insert twice - Mneutin.cpp - LP
   //Version = "4.0.22";   // 11/18/02 - TSR #3843 - do not output KLEBEN keyword if attribute LOADED is false, QD_out.cpp - LP
   //Version = "4.0.21";   // 11/18/02 - TSR #3842 - PINNR attribute is output incorrectly in PADS Write - LP
   //Version = "4.0.20";   // 11/15/02 - TSR #3838 - Panelization, rectangle not comming out filled - SN.
   //Version = "4.0.19";   // 11/15/02 - TSR #3840 - Pads Write - graphic class is written out incorrectly - LP
   //Version = "4.0.18";   // 11/15/02 - TSR #3839 - Allegro read "ANTI ETCH" are being classed in CAMCAD as "NORMAL"
   //Version = "4.0.17";   // 11/14/02 - TSR #3837 - VB ASCII Read, incorrect comp/pin assignment - LP
   //Version = "4.0.16";   // 11/14/02 - TSR #3813 - Infinite recursive call of FindOpenEndPoint because via->used is always reset to 0 - LP
   //Version = "4.0.15";   // 11/14/02 - TSR #3813 refix
   //Version = "4.0.14";   // 11/13/02 - TSR #3832 - Gencad writer hangs (The problem is actually in pcbutil, so modified pcbutil.cpp)
   //Version = "4.0.13";   // 11/12/02 - Removed Orcad Layout Plus Write, Protel PFW V2.8 Layout Write, Redac CADIF (.paf) Layout Write, THEDA (.tl) Layout, Panel Write - DD
   //Version = "4.0.12";   // 11/11/02 - TSR #3802 - ta_probe.cpp - Do not do probe placement if no probe is turn on - LP
   //Version = "4.0.11";   // 11/11/02 - TSR #3781 - Tooltip on DRC Dlg - DD
   //Version = "4.0.10";   // 11/08/02 - TSR #3778 GERBERIN.cpp - Crash fixed - SN.
   //Version = "4.0.9"; // 11/07/02 - Edif300_in.cpp - Fixed a bug in the function display()
   //Version = "4.0.8"; // 11/07/02 - TSR #3820 - attrib.cpp - visible attributes not show
   //Version = "4.0.7"; // 11/07/02 - TSR #3813, outputing pin/net in VB ASCII
   //Version = "4.0.6"; // 11/07/02 - TSR #3822, not able to recognize famaster-allegro import due to a remark line started with "R\"
                        // 11/07/02 - TSR #3767, Siemen write
   //Version = "4.0.5"; // 11/06/02 - Changed Alcatel Read to Alcatel Docica Read
   //Version = "4.0.4"; // 11/06/02 - Added API ExportGENCAD and new parameter to ExportFile to indicate exporting w/o showing dialogs
   //Version = "4.0.3"; // 11/05/02 - TSR 3814 & 3818 - the crash is fixed on both of these TSRs - SN.
   //Version = "4.0.2"; // 11/05/02 - TSR 3819 Huntron Write - TEST attrib on Comps (especially single pin comps)
   //Version = "4.0.1"; // 11/04/02 - OpenCADFile improvements (Gerber, Allegro, and most of Mentor BS multiple file issues)
   //Version = "4.0.0"; // 11/01/02
                        // 10/31/02 - TRS 3974 & TSR 3808 - allegout.cpp support version 14.2 and support comp boundary graphic class
                        // 10/29/02 - enh 3795 - gencad_o.cpp - if output_graphic_data is set to y then writer board level text and
                        // generic component as board level artwork.
                        // 10/24/02 - TSR 3785 - Fabmaster FATF Read - padstack is not being created for pin using "POLYGON" to create padshape
                        // 10/24/02 - TSR 3797 dxf read - inverted arcs
                        // 10/24/02 - TSR 3771 mentor board read wrong refdes position
   //Version = "3.11.77";  // 10/18/02 - TSR - DXF Reader Crash - SN
   //Version = "3.11.75";  // 10/21/02 - TSR 3805 - fatfin.cpp - Reader gets lost after reporting unknown layer symbol
   //Version = "3.11.74";  // 10/21/02 - TSR 3782 - gencad_o.cpp - implemented plane, TSR 3787 - gencadin.cpp, displaying insert on the wrong surface, immplement void plane in read before enhancement
   //Version = "3.11.73";  // 10/17/02 - TSR 3791 - gencadin.cpp - splits up the plane boundaries polystructure when arc is called out
   //Version = "3.11.72";  // 10/16/02 - TSR 3786 - gencadin.cpp - component text entries being placed relative to the board origin not component insert point
   //Version = "3.11.71";  // 10/15/02 - TSR 3798 - gencadin.cpp - artwork text shows a way too big
   //Version = "3.11.70";  // 10/14/02 - CIS 3689 - thedain.cpp - adding .LAYERATTER command
   //Version = "3.11.69";  // 10/11/02 - Enhancements - Path Attrib on file->block->getAttributesRef(), Import Placement Dialog removed - DD
   //Version = "3.11.68";  // 10/11/02 - TSR 3689 - thedain.cpp - separate layers by material and assign the correct electrical stack number. 
   //Version = "3.11.67";  // 10/11/02 - UsedLayers() API - fixed CRASH - DD
   //Version = "3.11.66";  // 10/10/02 - LogReader & NotePad now react to "NONE" or blank by not popping up - DD
   //Version = "3.11.65";  // 10/10/02 - TSR 3536 - HPGL Read "pe" mem problem - DD
   //Version = "3.11.64";  // 10/10/02 - TSR 3790 - gencadin.cpp, doesn't show text on the graphic
   //Version = "3.11.63";  // 10/09/02 - TSR 3778 - hpgllib.cpp, hpglout.cpp, HPGL1 doesn't draw filled square.
   //Version = "3.11.62";  // 10/08/02 - sv167 vb99in.cpp - handles radius_corner_rectangle
   //Version = "3.11.61";  // 10/04/02 - Fixed Bottom View & File Rotation.  Fixed insert angles outside of 0..2PI on RestructureFilesAroundOrigin() - DD
   //Version = "3.11.60";  // 10/03/02 - TSR #3773 - PCAD Padstack Extents problem fixed (invisible padstack because of extents) - DD
   //Version = "3.11.59";  // 10/03/02 - TSR #3776 - Reports | Pin to Pin crash removed - DD
   //Version = "3.11.58";  // 10/01/02 - TSR #3637 - Cost Analysis and Fabmaster Reports removed from Testability Report Dialog - DD
   //Version = "3.11.57";  // 09/26/02 - TSR #3757 - converting units not working component_outline also.
   //Version = "3.11.56";  // 09/30/02 - CIS 3767 Put MACHINE_ORIGIN_TOP & BOT command in out file.
   //Version = "3.11.55";  // 09/26/02 - TSR #3757 - converting units not working for keepouts and pad circle
   //Version = "3.11.54";  // 09/25/02 - TSR #3768 - Mentor Neutral Reader enhancements. 
   //Version = "3.11.53";  // 09/25/02 - TSR #3719 - gencamin.cpp - layers not created
   //Version = "3.11.52";  // 09/23/02 - TSR #3764 - gencad_o.cpp - Geometry with the wrong origin
   //Version = "3.11.51";  // 09/23/02 - TSR #3755 - vb99in.cpp - not recognizing the DISPLAY_WIDTH of a poly
   //Version = "3.11.50";  // 09/20/02 - TSR #3765 - qd_out.cpp - PART_NUMBER_KEYWORD command added to the .out file.
   //Version = "3.11.49";  // 09/20/02 - TSR #3766 - qd_out.cpp - Siemens writer: LOADED attribute checking added before writing BESTUKEN keyword.
   //Version = "3.11.48";  // 09/19/02 - TSR #3754 - dxfin.cpp - Endcap option not drawing the line correctly
   //Version = "3.11.47";  // 09/19/02 - TSR #3738 - Panref.cpp  - Pan/Zoom to refdes case sensitivity fixed.
   //Version = "3.11.46";  // 09/19/02 - CCM_OUT.cpp - Join segments of board/panel outline if can for one closed poly
   //Version = "3.11.45";  // 09/18/02 - TSR #3487 - HPGLIN.CPP - Text size fixed.
   //Version = "3.11.44";  // 09/17/02 - gencadin.cpp - Refname visiblity values are not displayed correctly.
   //Version = "3.11.43";  // 09/16/02 - CCM_OUT.cpp - Do not RubberBand if there is one closed poly for board/panel outline.
   //Version = "3.11.42";  // 09/12/02 - Padsin.cpp - TSR 3742 - Pads net attributes import fixed.
   //Version = "3.11.41";  // 09/12/02 - dxfout.cpp - TRS 3753 - Arcs and Bulges are not being written correctly
   //Version = "3.11.40";  // 09/09/02 - mentorin.cpp - Fixed the issue of global geometries overwriting each other because of they have the same name
   //Version = "3.11.39";  // 09/06/02 - Allegin.cpp - TSR#3729 - Re-fixed to tie devices to the correct geometry and to remove devices that are not tie to any geometry
   //Version = "3.11.38";  // 09/06/02 - ag_aoi_o.cpp - if a component is through hole and unloaded then put the info in Top and bottom PLX
   //Version = "3.11.37";  // 09/05/02 - mentorin.cpp - Fixed a crash bug related to enchancement #3370
                     // 09/05/02 - TSR #3737 - Refix the functionality of convert polys to flashes
   //Version = "3.11.36";  // 09/05/02 - Ag_AOI_o.cpp - TSR 3722 Components on the bottom surface being written on the top PLX
   //Version = "3.11.35";  // 09/05/02 - Allegin.cpp & Eifin.cpp - Both readers are modified so that all the selected file will be created in one boards.  Subsequent selections will be created in other boards
   //Version = "3.11.34";  // 09/04/02 - accel_out.cpp - change zero width arc to line segment and checking layer name
   //Version = "3.11.33";  // 09/03/02 - GERBIN.cpp - Modified so that all closed or filled polys will always have the last point the same as the first point
   //Version = "3.11.32";  // 08/30/02 - mentorin.cpp - Modified to allow reading in several board and keep them seperate
   //Version = "3.11.31";  // 08/30/02 - pcbutil.cpp & gerberpcb.cpp - TSR #3737 Fixed the funcational of convert polys to flashes
   //Version = "3.11.30";  // 08/29/02 - accel_out.cpp - TSR 3720 if arc has zero radius, write a line with the arc center point
   //Version = "3.11.29";  // 08/28/02 - ta_optn.cpp - Fixed Testibility Analyze bottom board keepout
   //Version = "3.11.28";  // 08/28/02 - allegin.cpp - Refix TSR #3730, COMP_ATT should be added to the geometry instead of the insert
   //Version = "3.11.27";  // 08/28/02 - accel_out.cpp - TSR 3720, issues 1-5 check the TSR page
   //Version = "3.11.26";  // 08/27/02 - DXFIN.cpp - TSR #3735, font information is being display as part of text
   //Version = "3.11.25";  // 08/27/02 - mentorin.cpp - TSR #3725, component_default_padstack and board_default_padstack were not initialize at beginning of the reader causing it to retain data from last call
   //Version = "3.11.24";  // 08/06/02 - takaya.cpp - TSR #3731 - Capacitance & Diode IC tests are not written.  Because some comparison are done with case sensitive.  Fixed to compare with case non-sensitive.
   //Version = "3.11.23";  // 08/26/02 - allegin.cpp - TSR# 3730 - .COMPATTR attribute are not added to inserted component
                     // 08/26/02 - takaya.cpp - TSR #3610, added handling of QUAD-BRIDGE RECTIFIER subclass to create test correctly
   //Version = "3.11.22";  // 08/23/02 - allegin.cpp - TSR# 3729 - Fixed TSR #3729, two duplicate geometry using the same device.  Resolved by creating new device for the second geometry
   //Version = "3.11.21";  // 08/20/02 - mentorin.cpp - TSR #3725 - file not recognized because of the way the header file was defined
   //Version = "3.11.20";  // 08/20/02 - ag_aoi_out.cpp - TSR #3722 - component that is on the bottom surface is being written out on the top PLX 
   //Version = "3.11.19";  // 08/19/02 - montorin.cpp - TSR #3663 - Fixed refname X, Y location of an insert
   //Version = "3.11.18";  // 08/19/02 - padsin.cpp - TSR #3713 - Change the code so it does not need to change Pads.IN
   //Version = "3.11.17";  // 08/16/02 - pfwsch.cpp  - TSR #3654 - Net name not being assigned
   //Version = "3.11.16";  // 08/16/02 - DXFIN.cpp - TSR #3617 - Added ATTDEF entity
   //Version = "3.11.15";  // 08/12/02 - padsin.cpp - TSR #3713 - Not reading correct layer type. Needed to change Pads.IN file also.
   //Version = "3.11.14";  // 08/09/02 - test_acc.cpp - TSR #3684 - Place test access
   //Version = "3.11.13";  // 08/09/02 - allegin.cpp - TSR 3707 - fixed attribute visibility
   //Version = "3.11.12";  // 08/08/02 - hp5DXin.cpp -  TSR #3692 drop backslash used as part of a name 
                     // 08/08/02 - draw.cpp - TSR #3718, move insert in bottom view
   //Version = "3.11.11";  // 08/08/02 - Mentorin.cpp - Enhanced to include these new commands: .FIL_ONLY, .IGNORE_ISLANDS, AND .IGNORE_THERMAL_TIE from mentor.in file
   //Version = "3.11.10";  // 08/08/02 - Mentorin.cpp - TSR #3696, thermal need to be part of parent object FIL
   //Version = "3.11.9";   // 08/08/02 - allegin.cpp - TSR 3707 .TEXTCOMPATTR was not working (it was not implemented)
   //Version = "3.11.8";   // 08/07/02 - DDE v9 Read - Added layer type mapping from dde.in
   //Version = "3.11.7";   // 08/07/02 - DDE v9 Read - Added eletricalstack number to layers
   //Version = "3.11.6";   // 08/07/02 - pcbutil.cpp - TSR #3625 - Fixed Complex_2PointLine to return false if more than one oblong or rectangle is found
   //Version = "3.11.5";   // 08/06/02 - DDE v8 & v9 Read - fixed x & y insert of port attributes
   //Version = "3.11.4";   // 08/06/02 - DDE v8 Read - TSR 3701 fixed mirror text
                     // 08/06/02 - takaya_o.cpp - TSR 3700 omit spaces b/t numeric value and unit, always positive values for tol
   //Version = "3.11.3";   // 08/05/02 - library.cpp - TSR 3705 Camcad crashing by not checking null pointer
   //Version = "3.11.2";   // 08/05/02 - Hp3070_o.cpp - TSR 3704 Show Partnumber when there's no message
   //Version = "3.11.1";   // 08/01/02 - DFM Release
   //Version = "3.10.66";  // 07/31/02 - mentorin.cpp - TSR 3691 Use BOARD_DEFAULT_PADSTACK when there's no padstack defined for a pin 
   //Version = "3.10.65";  // 07/25/02 - HP5DX_O.cpp - TSR 3699 Fixed wrong x location of components
   //Version = "3.10.64";  // 07/23/02 - Edif In - Added port attributes
   //Version = "3.10.63";  // 07/23/02 - gerberPCB.cpp & pcbutil.cpp - Modified OnCovertPolyToFlashes & OptimizeDuplicateAperture
   //Version = "3.10.62";  // 07/22/02 - Port_lib.cpp - TAKAYA8 has been accidentally removed from the case statement of ExportFile function, so need to add it back in order for TAKAYA8 WRITER to work
   //Version = "3.10.61";  // 07/19/02 - Mentor Board Station In - Fixed TSR #3693 regarding when it should draw and not draw an island
   //Version = "3.10.60";  // 07/18/02 - Pads In - Fixed TSR #3694 - Fixed creation of polyline in the function of pads_signal & pad6_signal
   //Version = "3.10.59";  // 07/15/02 - Mentor Out: Via Rules fixed & TERMINAL_DRILL_SIZE added to buried vias.
   //Version = "3.10.58";  // 07/11/02 - Mentor Neutral In - Modified the mirror flag for GEOMTOFIDUCIAL
   //Version = "3.10.57";  // 07/11/02 - VB99 Out - Fixed the output of diamter and clearance for thermal
   //Version = "3.10.56";  // 07/11/02 - Mentor Neutral In - fixed bug regarding GEOMTOFIDUCIAL
   //Version = "3.10.55";  // 07/11/02 - Takaya Out - Fixed TSR #3688 for crash due to odd of pin in SIP ISOLATE and DIP ISOLATE resistors
                     // 07/11/02 - Viewdraw Read - Fixed TSR #3683 for crash
   //Version = "3.10.54";  // 07/10/02 - Mentor Out: Via Rules and Pad Rules sections added.
   //Version = "3.10.53";  // 07/10/02 - DDE V8 - Fixed SMD rule for components
   //Version = "3.10.52";  // 07/10/02 - Mentor Neutral In - Added the function GetGeomToFiducial to check if a geometry is to be use as fiducial
   //Version = "3.10.51";  // 07/09/02 - Report - Geometry Report implemented
   //Version = "3.10.50";  // 07/08/02 - DDE V8 - TSR #3658 - Oblong apertures should not be rotated, they should be created just as the way they are
   //Version = "3.10.49";  // 07/08/02 - GerberPCB - Fixed bug in the function CombineTouchingApertures
   //Version = "3.10.48";  // 07/05/02 - DDE V8 - Added BLIND attribute to vias that are BLIND
   //Version = "3.10.47";  // 07/03/02 - Refix of Mentor Neutral In - Fixed crash in LoadSettings for the command SKIP 
   //Version = "3.10.46";  // 07/03/02 - FATF IN : Fixed TSR# 3665,  Illegal Tokens error fixed.
   //Version = "3.10.45";  // 07/03/02 - Outline - Fixed TSR# 3677
   //Version = "3.10.44";  // 07/03/02 - ODB++ import crash fixed.
   //Version = "3.10.43";  // 07/02/02 - Generate SMD Components Fixed
   //Version = "3.10.42";  // 07/02/02 - Mentor Neutral In - Fixed crash in LoadSettings for the command SKIP
   //Version = "3.10.41";  // 07/01/02 - Protel In - Fixed the feature of CONVERT_FREEPAD_COMP for version 3, so that it will appropriated identify freepad as freepad, via, or component
   //Version = "3.10.40";  // 07/01/02 - GerberPCB - Added the function CombineTouchingApertures in OnConverPolysToFlashes as one of Agilent's enhancement
   //Version = "3.10.39";  // 07/01/02 - Gencad Writer: Fixed definition of pads; It used to reference the wrong padshape.
   //Version = "3.10.38";  // 06/28/02 - Mentor Neutral In - Added a function SeachForXOUT to identify inserts of geomname with XOUTGEOM_ATTR as type INSERT_XOUT
   //Version = "3.10.37";  // 06/28/02 - Siemens QD - Fixed the output of the length and width of panel and board
   //Version = "3.10.36";  // 06/28/02 - Agilent 5DX Writer fixed: X's & Y's of components were written incorrectly.
   //Version = "3.10.35";  // 06/28/02 - Siemens QD - Fixed crash in Format.cpp  and typo in QD_out.h
   //Version = "3.10.34";  // 06/25/02 - Agilent AOI Writer: New Mixed Mode added -> a combination of SJ & SP.
   //Version = "3.10.33";  // 06/20/02 - Refixed - Mentor Boardstation - TSR# 3644 - Remove the call to change_vias_2_testpads
   //Version = "3.10.32";  // 06/20/02 - rotation fixed in writing bottom files for AOI 
   //Version = "3.10.31";  // 06/02/02 - HP3070 out - fixed the output of DEVICE to write "NC" if group not connected pins to one net and pin is flaged as single pin net
   //Version = "3.10.30";  // 06/20/02 - DDE v9 in - Fixed a crash cause by used of uninitialized variable in Graph_Area()
   //Version = "3.10.29";  // 06/20/02 - DDE v9 in - Fixed via inside a geometry and text display
   //Version = "3.10.28";  // 06/19/02 - HP3070 Out - fixed the output of DEVICE to write "NC" if group not connected pins to one net is choosen
   //Version = "3.10.27";  // 06/18/02 - HP3070 Out - Added the option to group not connected pins to one net
   //Version = "3.10.26";  // 06/18/02 - MVT changed to AOI
   //Version = "3.10.25";  // 06/17/02 - Fixed SMD pin counts for Manufacturing report
   //Version = "3.10.24";  // 06/17/02 - CR5000 side and mirroring fixed.
   //Version = "3.10.23";  // 06/17/02 - Fixed DFT Analysis for Probable Shorts
   //Version = "3.10.22";  // 06/14/02 - Mentor Board Station rotation in Part File fixed. Islands in traces file fixed.
   //Version = "3.10.21";  // 06/14/02 - Test Atribute Assignment - fixed to allow "BOTH" as value
   //Version = "3.10.20";  // 06/14/02 - VB ASCII - Fixed to allow multiple REFNAME on different layers
   //Version = "3.10.19";  // 06/13/02 - Attribute Visibility
   //Version = "3.10.18";  // 06/13/02 - Mentor Boardstation - TSR# 3644 - Remove the call to change_vias_2_testpads
   //Version = "3.10.17";  // 06/13/02 - Edif_in - Added more commands
   //Version = "3.10.16";  // 06/11/02 - DDE V9 - Fixed thermals and some polygons
   //Version = "3.10.15";  // 06/06/02 - Rotation fixed in bottom side for FATF IN.
   //Version = "3.10.14";  // 06/06/02 - Siemens QD - Added an insert type INSERTTYPE_XOUT
   //Version = "3.10.13";  // 06/06/02 - DDE V9 - Fixed the issue of duplicate tracks with different layers being on the same layer
   //Version = "3.10.12";  // 06/05/02 - DDE V9 - Fixed on mirror components
                     // 06/05/02 - Siemens QD - Fixed more bug on writeing out fiducials
   //Version = "3.10.11";  // 06/04/02 - Fixed bug on writing out fiducials
   //Version = "3.10.10";  // 06/04/02 - Added handling for TTT & TTI sections in Mentor Board Station traces file.
                     // 06/04/02 - Added Siemens QD writer
   //Version = "3.10.9";   // 05/28/02 - Added fixes for piece type in piece definition for PAD_OUT.cpp
   //Version = "3.10.8";   // 05/28/02 - TSR 3639 - Testablity Report Fixed.
   //Version = "3.10.7";   // 05/22/02 - Allegout.cpp - Add support for placing format & mechanical symbol in V14 and higher
   //Version = "3.10.6";   // 05/22/02 - Edif_in.cpp - Add modification to read in data for OccurrenceHierarchyAnnotate
   //Version = "3.10.5";   // 05/20/02 - Pads 4.0 update
   //Version = "3.10.4";   // 05/14/02 - DDE v9 - fixed boardoutline component to be exploded and bug in define area and arc
   //Version = "3.10.3";   // 05/08/02 - DDE fix
   //Version = "3.10.2";   // 05/07/02 - New DDE v9 and DDE fix
   //Version = "3.10.1";   // 05/06/02 - New DFM
   //Version = "3.9.6b";   // 03/07/02 -  

   CStringArray params;
   date.Parse(params," /");

   if (params.GetCount() > 3)
   {
      COleDateTime checkInDate(2000 + atoi(params[3]),atoi(params[1]),atoi(params[2]),0,0,0);

      if (checkInDate.GetStatus() == COleDateTime::valid)
      {
         Version += "." + checkInDate.Format("%y%m%d");
      }
   }

   return Version;
}


/*
   // title += " 2.1.2c";     // changed 12-Jul-97 WS
   // title += " 2.1.2d";     // changed 17-Jul-97 WS
   // title += " 2.1.2f";     // changed 29-Jul-97 WS - this should be the final August release
   // title += " 2.1.3a";     // changed 18-Aug-97 WS - released 24-Aug-97
   // title += " 2.1.3b";     // changed 24-Aug-97 WS - 
   //title += " 2.2.0a";      // changed 29-Aug-97 DD - new access codes per product
   //title += " 2.2.0b";      // changed 29-Sep-97 WS - 2.2.0a did fix lots of little and big things
                              // and should be the final 2.2.0a release. 
   //title += " 2.2.0c";      // changed 15-Oct-97 WS had a big error not being able to switch of 
                              // display for layers on apertures
   //title += " 2.2.0d";      // changed 24-Oct-97 WS correct shipment and release
   //title += " 2.2.0e";      // changed 27-Oct-97 WS 
   //title += " 2.2.0f";      // changed 10-Nov-97 WS  // released 24-Nov-97
   //title += " 2.2.0g";      // changed 25-Nov-97 DD 
   //title += " 2.2.1a";      // released  8-Dec-97 WS 
   //title += " 2.2.1b";      // changed and released 3-Dec-97 WS 
   //title += " 2.2.1c";      // changed to fix a CC file error, which caused a crash 16-Dec-97 WS 
   //title += " 2.2.1d";      // Arc and Outline 6-Jan-98 WS 
   //title += " 2.2.1e";      // Release 26-Jan-98
   //title += " 2.2.1f";      // Updated 23-Jan-98
   //title += " 2.2.1g";      // Release 5-Mar-98 final release on 2.2.1...
   //title += " 2.2.2a";      // Final release for shipping 15-Mar-98
   //title += " 2.2.2b";      // release for Cadence 2.2.2b
   //title += " 2.2.2c";      // release April 98
   //title += " 2.2.2d";      // fix a problem with PADS POWER Gerber files.
   //title += " 2.2.3a";      // Final release 16-Apr-98
   //title += " 2.2.3b";      // Release 28-Apr-98
   //title += " 2.2.3c";      // start 5-May-98
   //title += " 2.2.3d";      // start 13-May-98
   //title += " 2.2.4a";      // OLE implementation started. Released 28-May-98
   //title += " 2.2.4b";      // multiple fixes. Released 19-Jun-98   
   //title += " 2.2.4c";      // interim release 1-Jul-98
   //title += " 2.2.4d";      // interim release 15-jul-98
   //title += " 2.2.4e";          
   //title += " 2.2.5a";      // new VIEW toolbar, top and bottom layer view release 20-Aug-98
   // title += " 2.2.5b";     // device attribute unlinked from partnumber. 7-Sep-98
   //title += " 2.2.5c";         
   //title += " 2.3.0a";      // new attribute concept
   //title += " 2.3.0b";      // 19-Sep-98
   //title += " 2.3.0c";      // 2-Oct-98

   // license change !!!
   //title += " 3.0.0a";      // graphic speed !
   //title += " 3.0.0b";      // bunch of fixes.
   //title += " 3.0.0c";      // changed 13-Nov-98 continued V3 release.
   //title += " 3.0.0d";      // changed 20-Nov-98 continued V3 release.
   //title += " 3.0.0e";      // changed 25-Nov-98 , improved gerber write, join, worldview
   //title += " 3.0.0f";      // changed 7-DEC-98 , mulitple errors fixed
   //title += " 3.0.0g";      // changed 17-DEC-98 , mulitple errors fixed

   //title += " 3.1.0a";      // released 7-Jan-99
   //title += " 3.1.0b";      // 16-Jan-99  
   //title += " 3.1.0c";      // 24-Jan-99  
   //title += " 3.1.0d";      // 31-Jan-99  
   //title += " 3.1.0e";      // 6-Feb-99  
   //title += " 3.1.0f";      // 14-Feb-99  
   //title += " 3.1.0g";      // 21-Feb-99  
   //title += " 3.1.1a";      // 27-Feb-99  
   //title += " 3.1.1b";      // 7-Mar-99  
   //title += " 3.1.1c";      // 14-Mar-99  
   //title += " 3.1.1d";      // 20-Mar-99  
   //title += " 3.1.1e";      // 28-Mar-99  
   //title += " 3.2.0";       // 29-Mar-99  
   //title += " 3.2.0a";      // 8-Apr-99  
   //title += " 3.2.0b";      // 11-Apr-99  - lots of API changes, back on weekly track
   //title += " 3.2.0c";      // 22-Apr-99  
   //title += " 3.2.0d";      // 28-Apr-99  
   //title += " 3.2.1a";      // 1-May-99  
   //title += " 3.2.1b";      // 12-May-99  
   //title += " 3.2.1c";      // 29-May-99  
   //title += " 3.2.1d";      // 4-Jun-99  
   //title += " 3.2.1e";      // 8-Jun-99  
   //title += " 3.2.1f";      // 16-Jun-99  
   //title += " 3.3.0a";      // 20-Jun-99  // all fixup toolbars from Devin.
   //title += " 3.3.0b";      // 26-Jun-99  
   //title += " 3.3.0c";      // 6-Jul-99  
   //title += " 3.3.0d";      // 12-Jul-99  
   //title += " 3.3.1a";      // 17-Jul-99  
   //title += " 3.3.1b";      // 23-Jul-99  
   //title += " 3.3.1c";      // 2-Aug-99  
   //title += " 3.3.2a";      // 14-Aug-99  
   //title += " 3.3.2b";      // 22-Aug-99  
   //title += " 3.3.2c";      // 28-Aug-99  
   //title += " 3.3.3a";      // 2-Sep-99 big rebuild and all new installs
   //title += " 3.4.0a";      // 17-Sep-99 
   //title += " 3.4.1a";      // 27-Sep-99 
   //title += " 3.4.1b";      // 3-Oct-99 
   //title += " 3.4.1c";      // 10-Oct-99 
   //title += " 3.4.1d";      // 17-Oct-99 
   //title += " 3.5.0a";      // 18-Oct-99 
   //title += " 3.5.0b";      // 25-Oct-99 
   //title += " 3.5.0c";      // 31-Oct-99 
   //title += " 3.5.0d";      // 7-Nov-99 
   //title += " 3.5.0e";      // 14-Nov-99 
   //title += " 3.5.0f";      // 21-Nov-99 
   //title += " 3.5.0g";      // 28-Nov-99 
   //title += " 3.5.1a";      // 6-Dec-99 
   //title += " 3.5.1b";      // 11-Dec-99 
   //title += " 3.5.1c";      // 19-Dec-99 
   //title += " 3.5.1d";      // 27-Dec-99 
   //title += " 3.5.1f";      // 31-Dec-99 
   //title += " 3.5.2a";      // 2-Jan-00
   //title += " 3.6.0a";      // 9-Jan-00
   //title += " 3.6.0b";      // 16-Jan-00
   //title += " 3.6.0d";      // 30-Jan-00
   //title += " 3.6.1a";      // 4-Feb-00
   //title += " 3.6.1b";      // 14-Feb-00
   //title += " 3.7.0a";      // 17-Feb-00   <=== new font file
   //title += " 3.7.0b";      // 27-Feb-00   
   //title += " 3.7.1a";      // 12-Mar-00   
   //title += " 3.7.1b";      // 18-Mar-00   
   //title += " 3.7.1c";      // 27-Mar-00   
   //title += " 3.7.2a";      // 1-Apr-00 
   //title += " 3.7.2b";      // 14-Apr-00   
   //title += " 3.7.2c";      // 25-Apr-00   
   //title += " 3.7.2d";      // 28-Apr-00   
   //title += " 3.7.3a";      // 5-May-00 
   //title += " 3.7.4a";      // 16-May-00   
   //title += " 3.8.0a";      // 28-May-00   
   //title += " 3.8.0b";      // 3-Jun-00 
   //title += " 3.8.0c";      // 10-Jun-00   
   //title += " 3.8.0d";      // 22-Jun-00   
   //title += " 3.8.0e";      // 22-Jun-00   
   //title += " 3.8.1a";      // 7-Jul-00 
   //title += " 3.8.1b";      // 15-Jul-00   
   //title += " 3.8.1c";      // 18-Jul-00   
   //title += " 3.8.1d";      // 31-Jul-00   
   //title += " 3.8.1e";      // 10-Aug-00   
   //title += " 3.8.1f";      // 26-Aug-00   
   //title += " 3.8.2a";      // 2-Sep-00 
   //title += " 3.8.2b";      // 10-Sep-00   
   //title += " 3.8.2c";      // 16-Sep-00   
   //title += " 3.8.3a";      // 19-Sep-00   xml dll
   //title += " 3.8.3b";      // 2-Oct-00 
   //title += " 3.8.3c";      // 21-Oct-00   
   //title += " 3.8.4a";      // 30-Oct-00   
   //title += " 3.8.4b";      // 5-Nov-00 
   //title += " 3.8.4c";      // 12-Nov-00
   //title += " 3.8.4d";      // 17-Nov-00
   //title += " 3.8.5a";      // 4-Dec-00
   //title += " 3.8.5b";      // 15-Dec-00
   //title += " 3.8.5c";      // 22-Dec-00
   //title += " 3.8.6a";      // 8-Jan-01
   //title += " 3.8.6b";      // 12-Jan-01
   //title += " 3.8.6c";      // 25-Jan-01
   //title += " 3.9.0a";      // 3-Feb-01
   //title += " 3.9.0b";      // 19-Feb-01
   //title += " 3.9.0c";      // 1-March-01  <-- CCM Version 2
   //title += " 3.9.0d";      // 8-March-01  
   //title += " 3.9.1a";      // 23-March-01 <--- CC load XML memory leak fixed
   //title += " 3.9.1b";      // 2-April-01  
   //title += " 3.9.1c";      // 4-April-01  <--- API mixup fixed !
   //title += " 3.9.2a";      // 24-April-01 <--- multiple fixes for QRS, IPC etc...
   //title += " 3.9.2b";      // 30-April-01 <--- xml dll removed !
   //title += " 3.9.2c";      // 11-May-01   
   //title += " 3.9.2d";      // 2-Jun-01 
   //title += " 3.9.3a";      // 2-Jul-01 
   //title += " 3.9.3b";      // 13-Jul-01   
   //title += " 3.9.3c";      // 17-Jul-01   
   //title += " 3.9.3d";      // 23-Jul-01   
   //title += " 3.9.4a";      // 2-Aug-01 <--- fixes on DFM, DRC, finishing release 4.
   //title += " 3.9.4b";      // 16-Aug-01   
   //title += " 3.9.4c";      // 24-Aug-01   
   //title += " 3.9.4d";      // 31-Aug-01   
   //title += " 3.9.5a           // 11-Sep-01 <--- Gerber Netlist compare updates   
   //Version = "3.9.5b";      // 1-Oct-01 
   //Version = "3.9.5c";      // 6-Nov-01 
   //Version = "3.9.6a";      // 4-Jan-02 
   Version = "3.9.6b";        // 7-Mar-02 
*/
