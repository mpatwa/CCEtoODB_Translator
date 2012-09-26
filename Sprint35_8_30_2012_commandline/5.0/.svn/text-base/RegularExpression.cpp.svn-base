// $Header: /CAMCAD/4.6/RegularExpression.cpp 5     9/17/06 6:53p Kurt Van Ness $

/*
History: RegularExpression.cpp
 * 
 * *****************  Version 7  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:55a
 * Updated in $/LibKnvPP
 * Added metacharacter control and case sensitive comparison control
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 11/19/98   Time: 10:27a
 * Updated in $/LibKnvPP
 * Change BOOL to bool, where appropriate.
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 11/14/97   Time: 8:13a
 * Updated in $/LibKnvPP
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 5/14/97    Time: 2:15a
 * Updated in $/libknvpp
 * Reimplemented, LibRegex is no longer used, structures were converted to
 * classes.
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 3/05/97    Time: 3:38a
 * Updated in $/LibKnvPP
 * Renamed files, (lengthened and fixed case)
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 12/27/95   Time: 8:36p
 * Updated in $/libknvpp
*/


#include "StdAfx.h"
#include "RegularExpression.h"

IMPLEMENT_DYNAMIC(CRegularExpression,CRegexp);
IMPLEMENT_DYNAMIC(CRegexp,CObject);
IMPLEMENT_DYNAMIC(CSubExpression,CObject);

#ifdef USE_DEBUG_NEW
#define new DEBUG_NEW
#endif

//_____________________________________________________________________________
void CRange::init()
{ 
   int i;
   
   for (i = 0;i < REstringSize;i++)
   {
      m_range[i] = '\0';
   }
}

void CRange::mark(const char index1,const char index2)
{
   int i,start,stop;
   
   start = (int) index1;
   stop  = (int) index2;
   
   if (start > stop)
   {
      start = (int) index2;
      stop  = (int) index1;
   }
    
   for (i = start;i <= stop;i++)
   {
      m_range[i] = '\1';
   }
}

//_____________________________________________________________________________
int CLiteralBuffer::pushLiteralChar(const char litchar)
{
   if (m_cnt >= REstringSize - 1)
   {
      return(1);
   }
   
   m_buf[m_cnt] = litchar;
   m_buf[++m_cnt] = '\0';
   
   return(0);
}

//_____________________________________________________________________________
CSubExpressions::~CSubExpressions()
{
   deleteAll();
}

CSubExpression* CSubExpressions::push()
{
   m_subExpressions.Add((CObject*)(new CSubExpression()));

   return (CSubExpression*)m_subExpressions[m_subExpressions.GetUpperBound()];
}

void CSubExpressions::deleteAll()
{
   for (int i = 0;i <= m_subExpressions.GetUpperBound();i++)
   {
      delete (CSubExpression*)m_subExpressions.GetAt(i);
   }
}

//_____________________________________________________________________________
CRegexp::CRegexp()
{
   setMetacharacters();  // will not call derived class versions

   m_hasMetacharacters = false;
   m_caseSensitive     = true;
   m_escapeRequiredForGrouping = true;
}

CRegexp::~CRegexp()
{
}

void CRegexp::setMetacharacters()
{
   m_metaAnchorAtBegin = '^';
   m_metaAnchorAtEnd   = '$';
   m_metaRepeat0       = '*';
   m_metaRepeat1       = '+';
   m_metaAnyChar       = '.';
   m_metaAnyString     = Cundefined;
   m_metaSetBegin      = '[';
   m_metaSetEnd        = ']';
   m_metaSetNegate     = '^';
   m_metaSetRange      = '-';
   m_metaGroupBegin    = '(';
   m_metaGroupEnd      = ')';
   m_metaEscape        = '\\';
   m_metaSubstitute    = '\\';
}

void CRegexp::resetMetacharacter(char metaChar)
{
   if (m_metaAnchorAtBegin == metaChar) m_metaAnchorAtBegin = Cundefined;
   if (m_metaAnchorAtEnd   == metaChar) m_metaAnchorAtEnd   = Cundefined;
   if (m_metaRepeat0       == metaChar) m_metaRepeat0       = Cundefined;
   if (m_metaRepeat1       == metaChar) m_metaRepeat1       = Cundefined;
   if (m_metaAnyChar       == metaChar) m_metaAnyChar       = Cundefined;
   if (m_metaAnyString     == metaChar) m_metaAnyString     = Cundefined;
   if (m_metaSetBegin      == metaChar) m_metaSetBegin      = Cundefined;
   if (m_metaSetEnd        == metaChar) m_metaSetEnd        = Cundefined;
   if (m_metaGroupBegin    == metaChar) m_metaGroupBegin    = Cundefined;
   if (m_metaGroupEnd      == metaChar) m_metaGroupEnd      = Cundefined;
   if (m_metaEscape        == metaChar) m_metaEscape        = Cundefined;
   if (m_metaSubstitute    == metaChar) m_metaSubstitute    = Cundefined;
}

void CRegexp::resetMetacharacterSet(char metaChar)
{
   if (m_metaSetBegin      == metaChar) m_metaSetBegin      = Cundefined;
   if (m_metaSetEnd        == metaChar) m_metaSetEnd        = Cundefined;
   if (m_metaSetNegate     == metaChar) m_metaSetNegate     = Cundefined;
   if (m_metaSetRange      == metaChar) m_metaSetRange      = Cundefined;
}

void CRegexp::setMetaAnchorAtBegin(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaAnchorAtBegin = metaChar;
}

void CRegexp::setMetaAnchorAtEnd(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaAnchorAtEnd = metaChar;
}

void CRegexp::setMetaRepeat0(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaRepeat0 = metaChar;
}

void CRegexp::setMetaRepeat1(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaRepeat1 = metaChar;
}

void CRegexp::setMetaAnyChar(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaAnyChar = metaChar;
}

void CRegexp::setMetaAnyString(char metaChar) 
{
   resetMetacharacter(metaChar);

   m_metaAnyString = metaChar;
}

void CRegexp::setMetaSetBegin(char metaChar)
{
   resetMetacharacter(metaChar);
   resetMetacharacterSet(metaChar);

   m_metaSetBegin = metaChar;
}

void CRegexp::setMetaSetEnd(char metaChar)
{
   resetMetacharacter(metaChar);
   resetMetacharacterSet(metaChar);

   m_metaSetBegin = metaChar;
}

void CRegexp::setMetaSetNegate(char metaChar)
{
   resetMetacharacterSet(metaChar);

   m_metaSetNegate = metaChar;
}

void CRegexp::setMetaSetRange(char metaChar)
{
   resetMetacharacterSet(metaChar);

   m_metaSetRange = metaChar;
}

void CRegexp::setMetaGroupBegin(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaGroupBegin = metaChar;
}

void CRegexp::setMetaGroupEnd(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaGroupEnd = metaChar;
}

void CRegexp::setMetaEscape(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaEscape = metaChar;
}

void CRegexp::setMetaSubstitute(char metaChar)
{
   resetMetacharacter(metaChar);

   m_metaSubstitute = metaChar;
}

void CRegexp::setEscapeRequiredForGrouping(bool flag)
{
   m_escapeRequiredForGrouping = flag;
}

int CRegexp::regcomp(const char *pattern)
{ 
   const char Cnull='\0';
   
   enum states {sChkAnchor,sBeginRE,sParseRE,sBeginBracket,sBracket,
               sBracketHyphen,sEnd,sFail} state;
               
   CLiteralBuffer litbuf;
   RegExpTypeTag bracketType;
   int parenCnt = 0,expError = 0;
   char *p,range1;
   CRange range;

   m_numSub = 0;
   m_re.deleteAll();
   m_anchored = false;
   m_hasMetacharacters = false;
   
   p = (char*)pattern;
   
   for (state = sChkAnchor;state != sEnd && state != sFail;)
   {
      switch (state)
      {
         case sChkAnchor: /* check for anchor */
            if (*p == m_metaAnchorAtBegin)
            {
               expError |= pushExp(beginAnchor,NULL);
               m_anchored = true;
               p++;
            }
            
            state = sBeginRE;
            break;
         case sBeginRE:  /* begining of RE */
            litbuf.m_cnt = 0;
            state = sParseRE;
            break;
         case sParseRE:  /* parse the RE */
            if (*p == m_metaAnyChar)
            {
               m_hasMetacharacters = true;
               expError |= pushLit(&litbuf);
               expError |= pushExp(wild,NULL);
               p++;
               state = sBeginRE;
            }
            else if (*p == m_metaRepeat0)
            {
               m_hasMetacharacters = true;
               expError |= pushLit(&litbuf);
               expError |= modifyExp(0,MaxMatches);
               p++;
               state = sBeginRE;
            }
            else if (*p == m_metaRepeat1)
            {
               m_hasMetacharacters = true;
               expError |= pushLit(&litbuf);
               expError |= modifyExp(1,MaxMatches);
               p++;
               state = sBeginRE;
            }
            else if (*p == m_metaAnyString)
            {
               m_hasMetacharacters = true;
               expError |= pushLit(&litbuf);
               expError |= pushExp(wild,NULL);
               expError |= modifyExp(0,MaxMatches);
               p++;
               state = sBeginRE;
            }
            else if (*p == m_metaEscape)
            {
               p++;
               
               if (*p == Cnull)
               {
                  state = sFail;
               }
               
               if (m_escapeRequiredForGrouping && *p == m_metaGroupBegin)
               {
                  expError |= pushLit(&litbuf);
                  
                  if (++parenCnt > MaxReplacements)
                  {
                     state = sFail;
                  }
                  else
                  {
                     m_numSub++;
                     expError |= pushExp(replaceStart,NULL);
                     p++;
                  }
                  
                  state = sBeginRE;
               }
               else if (m_escapeRequiredForGrouping && *p == m_metaGroupEnd)
               {
                  expError |= pushLit(&litbuf);
                  
                  if (--parenCnt < 0)
                  {
                     state = sFail;
                  }
                  else
                  {
                     expError |= pushExp(replaceEnd,NULL);
                     p++;
                  }
                  
                  state = sBeginRE;
               }
               else
               {
                  litbuf.pushLiteralChar(*p);
                  p++;
               }
            }
            else if (!m_escapeRequiredForGrouping && *p == m_metaGroupBegin)
            {
               expError |= pushLit(&litbuf);
               
               if (++parenCnt > MaxReplacements)
               {
                  state = sFail;
               }
               else
               {
                  m_numSub++;
                  expError |= pushExp(replaceStart,NULL);
                  p++;
               }
               
               state = sBeginRE;
            }
            else if (!m_escapeRequiredForGrouping && *p == m_metaGroupEnd)
            {
               expError |= pushLit(&litbuf);
               
               if (--parenCnt < 0)
               {
                  state = sFail;
               }
               else
               {
                  expError |= pushExp(replaceEnd,NULL);
                  p++;
               }
               
               state = sBeginRE;
            }
            else if (*p == m_metaSetBegin)
            {
               m_hasMetacharacters = true;
               expError |= pushLit(&litbuf);
               p++;
               state = sBeginBracket;
            }
            else if (*p == Cnull)
            {
               expError |= pushLit(&litbuf);
               state = sEnd;
            }
            else
            {
               if (*p == m_metaAnchorAtEnd && p[1] == Cnull)
               {
                  expError |= pushLit(&litbuf);
                  expError |= pushExp(endAnchor,NULL);
                  state = sEnd;
                  p++;
               }
               else
               {
                  litbuf.pushLiteralChar(*p);
                  p++;
               }
            }
            
            break;
         case sBeginBracket:   /* bracket expression */
            if (*p == m_metaSetNegate)
            {
               bracketType = nonMatchingList;
               p++;
            }
            else
            {
               bracketType = matchingList;
            }
            
            state = sBracket;
            range.init();
            range1 = Cnull;
            
            if (*p == m_metaSetEnd)
            {
               range1 = *p;
               range[(int)range1] = TRUE;
               p++;
            }
            
            break;
         case sBracket:   /* in bracket expression */
            if (*p == m_metaSetEnd)
            {
               expError |= pushExp(bracketType,range.getRange());
               p++;
               state = sBeginRE;
            }
            else if (*p == Cnull)
            {
               state = sFail;
            }
            else if (*p == m_metaSetRange && range1 != Cnull)
            {
               state = sBracketHyphen;
               p++;
            }
            else
            {
               range1 = *p;
               range[(int)range1] = TRUE;
               p++;
            }
            
            break;
         case sBracketHyphen:       /* after hyphen in bracket expression */
            if (*p == m_metaSetEnd)
            {
               range[(int)m_metaSetRange] = TRUE;
               
               expError |= pushExp(bracketType,range.getRange());
               p++;
               state = sBeginRE;
            }
            else if (*p == Cnull)
            {
               state = sFail;
            }
            else
            {
               range.mark(range1,*p);
               p++;
               range1 = Cnull;
               state = sBracket;
            }
            
            break;
         case sEnd:       /* end of RE */
            break;
         case sFail:      /* RE failure */
            break;
            
      }
   }
   
   if (state == sFail || expError || parenCnt != 0)
   {
      //regfree(reg);
      return(1);
   }
   
   return(0);
}
  
int CRegexp::pushExp(RegExpTypeTag exptype,const char *data)
{
   CSubExpression* re;
   char *p;

   re = m_re.push();
   
   re->m_expType = exptype;
   re->m_minCountLimit = 1;
   re->m_maxCountLimit = 1;
   
   switch (exptype)
   {
      case literalString:
      case literalChar:
         re->m_expString = data;
         re->m_expLen = re->m_expString.GetLength();
             
         break;
      case matchingList:
      case nonMatchingList:
         p = re->m_expString.GetBufferSetLength(REstringSize);
         memcpy(p,data,REstringSize);
          
         break;
      case beginAnchor:
      case endAnchor:
         re->m_minCountLimit = 1;
         break;
      case replaceStart:
      case replaceEnd:
         re->m_minCountLimit = 0;
         break;
      case wild:
         break;
   }   

   return(0);
}

int CRegexp::modifyExp(const int minLimit,const int maxLimit)
{
   CSubExpression* re;
   int retval;
   char tempbuf[2];
   
   if (m_re.getCount() < 1)
   {
      return(1);
   }
   
   re = m_re.top();
   
   if (re->m_expType == literalString)
   {
      (re->m_expLen)--;
      tempbuf[0] = re->m_expString[re->m_expLen];
      tempbuf[1] = '\0';
      re->m_expString.SetAt(re->m_expLen,'\0');
      
      if (re->m_expLen == 1)
      {
         re->m_expType = literalChar;
      }
      
      if (retval = pushExp(literalChar,tempbuf))
      {
         return(retval);
      }
      
      re = m_re.top();
   }
   
   re->m_minCountLimit = minLimit;
   re->m_maxCountLimit = maxLimit;

   return(0);
}

int CRegexp::pushLit(CLiteralBuffer* litbuf)
{
   int retval = 0;
   
   if (litbuf->m_cnt > 0)
   {
      retval = pushExp((litbuf->m_cnt > 1) ? literalString : literalChar,
         litbuf->m_buf);
   }
   
/*   litbuf->cnt = 0;   */
   
   return(retval);
}

int CRegexp::regexec(const char *string,int nmatch,REGMATCH pmatch[],int eflags)
{
   CSubExpression* re;
   int i,ind,sp,retval,matchStack[MaxReplacements];
   char *p;
   
   if (nmatch > MaxReplacements)
   {
      nmatch = MaxReplacements;
   }
   
   m_maxSubexp = nmatch;
   
   for (i = 0;i < m_maxSubexp;i++)
   {
      pmatch[i].rm_sp = NULL;
      pmatch[i].rm_ep = NULL;
   }  
   
   for (p = (char*)string,retval = TRUE;retval && *p != '\0';p++)
   {
      m_index = 0;
      m_string = p;
      m_stringPos = string;
   
      retval = !tryre();
      
      if (m_anchored)
      {
         break;
      }   
   }
   
   if (!retval)  // pattern matched, generate pmatch
   {
      sp  = 0;
      ind = 0;
      
      for (i = 0;i < m_re.getCount();i++)
      {
         re = m_re[i];
         
         switch(re->m_expType)
         {
            case replaceStart:
               ind++;
               matchStack[sp++] = ind;
               
               if (ind <= nmatch)
               {
                  pmatch[ind].rm_sp = re->m_firstChar;
               }
               
               break;
            case replaceEnd:
               sp--;
               
               if (matchStack[sp] <= nmatch)
               {
                  pmatch[matchStack[sp]].rm_ep = re->m_firstChar;
               }
               
               break;
         }
      }
      
      if (nmatch > 0)
      {
         pmatch[0].rm_sp = m_re.bottom()->m_firstChar;
         pmatch[0].rm_ep = m_re.top()->m_firstChar +
                         ((m_re.top()->m_expType == endAnchor) ? 
                           0 : m_re.top()->m_curCount);
      }
   }
                                                         
   return(retval);
}

// returns TRUE if re matches
int CRegexp::tryre()
{
   CSubExpression* re;
   int val,tryind,success;
   
   // does the current RE index equal the number of REs ?
   if (m_index == m_re.getCount())
   {
      return(TRUE); /* success */
   }

   re = m_re[m_index];
   re->m_curCount = 0;
   re->m_firstChar = m_stringPos;
   
   for (tryind = 0,success = TRUE;tryind < re->m_maxCountLimit && success;tryind++)
   {
      success = FALSE;
      
      switch (re->m_expType)
      {
         case beginAnchor:
            if (re->m_firstChar == m_string)
            {
               success = TRUE;
               re->m_curCount = 1;
            }
            
            break;
         case endAnchor:
            if (*(m_stringPos) == '\0')
            {
               success = TRUE;
               re->m_curCount = 1;
            }   
               
            break;         
         case literalString:
         case literalChar:
            val = (m_caseSensitive ? re->reCharMatch() : re->reCharMatchNoCase());
            
            if (val)
            {
               success = TRUE;
               re->m_curCount += re->m_expLen;
            }   
           
            break;         
         case matchingList:
         case nonMatchingList:
         case wild:
            val = (m_caseSensitive ? re->reCharMatch() : re->reCharMatchNoCase());
            
            if (val)
            {
               success = TRUE;
               (re->m_curCount)++;
            }   
           
            break;         
         case replaceStart:
         case replaceEnd:
               success = TRUE;
               re->m_curCount = 0;
            break;         
      }
   }
   
   if (re->m_curCount < re->m_minCountLimit)
   {
      return(FALSE);
   }
   
   (m_index)++;

   /* success */
   for (;re->m_curCount >= re->m_minCountLimit;re->m_curCount--)
   {
      if (re->m_expType != beginAnchor)
      {
         m_stringPos = re->m_firstChar + re->m_curCount;
      }
      
      success = tryre();
      
      if (success)
      {
         return(TRUE); 
      }
      
      if (re->m_expType == literalString)
      {
         break;
      }   
   }
   
   (m_index)--;
   
   return(FALSE);   
}

//_____________________________________________________________________________
int CSubExpression::reCharMatch()
{
   int i,retval,index;
   
   switch (m_expType)
   {
      case literalString:
         retval = TRUE;
            
         for (i = 0;i < m_expLen && retval;i++)
         {
            retval = (m_firstChar[i] == m_expString[i]);
         }
         break;         
      case literalChar:
         retval = (m_firstChar[m_curCount] == m_expString[0]);
          
         break;         
      case matchingList:
         retval =  (m_expString[ (int)m_firstChar[m_curCount] ]);
         break;         
      case nonMatchingList:
         index = m_firstChar[m_curCount];
         
         if (index == 0)
         {
            retval = FALSE;
         }
         else
         {
            retval = !(m_expString[index]);
         }
         
         break;         
      case wild:
         retval = m_firstChar[m_curCount] != '\0';
         break;         
   }
   
   return(retval);
}  

int CSubExpression::reCharMatchNoCase()
{
   int i,retval,indexL,indexU;
   
   switch (m_expType)
   {
      case literalString:
         retval = TRUE;
            
         for (i = 0;i < m_expLen && retval;i++)
         {
            retval = (tolower(m_firstChar[i]) == tolower(m_expString[i]));
         }
         break;         
      case literalChar:
         retval = (tolower(m_firstChar[m_curCount]) == tolower(m_expString[0]));
          
         break;         
      case matchingList:
         indexL = tolower(m_firstChar[m_curCount]);
         indexU = toupper(m_firstChar[m_curCount]);
         retval = m_expString[indexL] || m_expString[indexU];
         break;         
      case nonMatchingList:
         indexL = tolower(m_firstChar[m_curCount]);
         indexU = toupper(m_firstChar[m_curCount]);
         
         if (indexL == 0)
         {
            retval = FALSE;
         }
         else
         {
            retval = !(m_expString[indexL] || m_expString[indexU]);
         }
         
         break;         
      case wild:
         retval = m_firstChar[m_curCount] != '\0';
         break;         
   }
   
   return(retval);
}  

//_____________________________________________________________________________
CRegularExpression::CRegularExpression()
{
   m_anchorEntire = false;
}

CRegularExpression::CRegularExpression(const char* regularExpression,bool anchorEntire)
{
   setRegularExpression(regularExpression,anchorEntire);
}

CRegularExpression::CRegularExpression(const char* regularExpression,const char* substituteExpression,
   bool anchorEntire)
{
   setRegularExpression(regularExpression,anchorEntire);
   setSubstituteExpression(substituteExpression);
}

CRegularExpression::~CRegularExpression()
{
}

void CRegularExpression::setMetacharacters()
{
   CRegexp::setMetacharacters();
}

void CRegularExpression::setRegularExpression(const char* regularExpression,bool anchorEntire)
{
   m_regularExpression = regularExpression;

   if (anchorEntire)
   {
      if (m_regularExpression.GetLength() < 1 || m_regularExpression.GetAt(0) != m_metaAnchorAtBegin)
      { 
         m_regularExpression = m_metaAnchorAtBegin + m_regularExpression;
      }

      if (m_regularExpression.GetAt(m_regularExpression.GetLength() - 1) != m_metaAnchorAtEnd)
      { 
         m_regularExpression = m_regularExpression + m_metaAnchorAtEnd;
      }
   }

   regcomp(m_regularExpression);
}

void CRegularExpression::setSubstituteExpression(const char* substituteExpression)
{
   m_substituteExpression = substituteExpression;
}

bool CRegularExpression::matches(const char* string)
{
   m_matchString = string;
   bool retval = (regexec(string,MaxSubExpressions,m_regMatch,0) == 0);
   
   return retval;
}

CString CRegularExpression::getSubstitution()
{
   return evalSubstitution(m_substituteExpression);
}

CString CRegularExpression::getSubstitution(const char* string)
{
   return getSubstitution(string,m_substituteExpression);
}

CString CRegularExpression::getSubstitution(const char* string,const char* substituteExpression)
{
   if (regexec(string,MaxSubExpressions,m_regMatch,0) != 0)
   {
      return CString("");
   }

   return evalSubstitution(substituteExpression);
}

CString CRegularExpression::evalSubstitution(const char* substituteExpression)
{
   CString dst;

   int i;
   const char* r;
   const char* p = substituteExpression;

   while (*p != '\0')
   {
      if (*p == m_metaSubstitute)
      {
         if (*(++p) == '\0')
         {
            dst += m_metaSubstitute;
            break;
         }

         if (isdigit(*p))
         {
            i = *p - '0';

            for (r = m_regMatch[i].rm_sp;r < m_regMatch[i].rm_ep;r++)
            {
               dst += *r;
            }
         }
         else
         {
            dst += *p;
         }
      }
      else
      {
         dst += *p;
      }

      p++;
   }
   
   return dst;
}

CString CRegularExpression::fixRegularExpression(const CString& regularExpression)
{
   CString retval;

   char nil = '\0';
   const char* q = &nil;

   for (const char* p = regularExpression;*p != '\0';p++)
   {
      if (*p == '*' && *q != '.')
      {
         retval += ".";
         q = p;
      }

      retval += *p;
   }

   return retval;
}

//_____________________________________________________________________________
CRegularExpressionList::CRegularExpressionList()
{
}

const CRegularExpression* CRegularExpressionList::findFirstMatchingExpression(const CString& string) const
{
   CRegularExpression* regularExpression = NULL;

   for (POSITION pos = GetHeadPosition();pos != NULL;)
   {
      regularExpression = GetNext(pos);

      if (regularExpression->matches(string))
      {
         break;
      }

      regularExpression = NULL;
   }

   return regularExpression;
}

bool CRegularExpressionList::isStringMatched(const CString& string) const
{
   bool retval = (findFirstMatchingExpression(string) != NULL);

   return retval;
}






