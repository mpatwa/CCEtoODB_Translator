// $Header: /CAMCAD/4.6/RegularExpression.h 5     9/17/06 6:53p Kurt Van Ness $

/*
History: RegularExpression.h
 * 
 * *****************  Version 7  *****************
 * User: Kvanness     Date: 2/06/00    Time: 11:56a
 * Updated in $/LibKnvPP
 * Added metacharacter control and case sensitive comparison control
 * 
 * *****************  Version 6  *****************
 * User: Kurtv        Date: 11/19/98   Time: 10:24a
 * Updated in $/include
 * Change BOOL to bool, where appropriate.
 * 
 * *****************  Version 5  *****************
 * User: Kurtv        Date: 4/22/98    Time: 11:56a
 * Updated in $/include
 * Added "#pragma once" directive.
 * 
 * *****************  Version 4  *****************
 * User: Kurtv        Date: 5/14/97    Time: 2:21a
 * Updated in $/include
 * Reimplemented, LibRegex is no longer used, structures were converted to
 * classes.
 * 
 * *****************  Version 3  *****************
 * User: Kurtv        Date: 2/27/97    Time: 5:27p
 * Updated in $/include
 * Changes for longer file names
 * 
 * *****************  Version 2  *****************
 * User: Kurtv        Date: 1/25/96    Time: 9:11p
 * Updated in $/include
*/

#if !defined(__RegularExpression_H__)
#define __RegularExpression_H__

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "TypedContainer.h"

#define REstringSize   256
#define MaxReplacements 10
#define MaxMatches     256
#define MaxSubExpressions 10
#define Cundefined '\xff'

enum RegExpTypeTag
{
   literalString,
   literalChar,
   matchingList,
   nonMatchingList,
   wild,
   beginAnchor,
   endAnchor,
   replaceStart,
   replaceEnd,
   nulltype
};

typedef struct regmatch_t
{
   const char *rm_sp;   /* pointer to start of substring */
   const char *rm_ep;   /* pointer past end of substring */
} REGMATCH; 

//_____________________________________________________________________________
class CRange
{
private:
   char m_range[REstringSize];

public:
   void init();
   void mark(const char index1,const char index2);
   char* getRange() { return m_range; }
   char& operator[](int index) { return m_range[index]; }
};

//_____________________________________________________________________________
class CLiteralBuffer
{
public:
   int m_cnt;
   char m_buf[REstringSize];

public:
   int pushLiteralChar(const char litchar);
};

//_____________________________________________________________________________
class CSubExpression : public CObject
{
   DECLARE_DYNAMIC(CSubExpression);
   
public:
   RegExpTypeTag m_expType;  // type of regular expression
   CString m_expString;      // pattern to match
   int m_expLen;             // length of expString (if literal)
   int m_curCount;           // current match count (for multiple occurance)
   int m_minCountLimit;      // limit for minimum match count (for multiple occurance) 
   int m_maxCountLimit;      // limit for maximum match count (for multiple occurance)
   const char* m_firstChar;  // first character matched by RE 

public:
   int reCharMatch();
   int reCharMatchNoCase();
}; 

//_____________________________________________________________________________
class CSubExpressions : public CObject
{
private:
   CObArray m_subExpressions;

public:
   ~CSubExpressions();

   CSubExpression* bottom(){ return (CSubExpression*)m_subExpressions[0]; }
   CSubExpression* top() { return (CSubExpression*)m_subExpressions[m_subExpressions.GetUpperBound()]; }

   CSubExpression* operator[](int index) { return (CSubExpression*)m_subExpressions[index]; }
   int getCount() { return m_subExpressions.GetSize(); }

   CSubExpression* push();
   void deleteAll();
};

//_____________________________________________________________________________
class CRegexp : public CObject
{
   DECLARE_DYNAMIC(CRegexp);
   
private:
   int m_index;               // current RE being evaluated
   int m_anchored;            // TRUE if RE is anchored at beginning
   char* m_string;            // string being matched
   const char* m_stringPos;   // current position in string
   int m_numSub;              // number of parenthesized subexpressions
   int m_maxSubexp;           // maximum number of sub expressions 
   bool m_caseSensitive;
   CSubExpressions m_re; 
   bool m_hasMetacharacters;  // true if metacharacters are present in the regular expression
   bool m_escapeRequiredForGrouping;

protected:
   char m_metaAnchorAtBegin;  // default '^'
   char m_metaAnchorAtEnd;    // default '$'
   char m_metaRepeat0;        // default '*'
   char m_metaRepeat1;        // default '+'
   char m_metaAnyChar;        // default '.'
   char m_metaAnyString;      // default Cundefined
   char m_metaSetBegin;       // default '['
   char m_metaSetEnd;         // default ']'
   char m_metaSetNegate;      // default '^'
   char m_metaSetRange;       // default '-'
   char m_metaGroupBegin;     // default '('
   char m_metaGroupEnd;       // default ')'
   char m_metaEscape;         // default '\'
   char m_metaSubstitute;     // default '\'

public:
   CRegexp();
   ~CRegexp();

   bool hasMetacharacters() { return m_hasMetacharacters; }
   void setCaseSensitive(bool caseSensitive) { m_caseSensitive = caseSensitive; }

   virtual void setMetacharacters();
   void resetMetacharacter(char metaChar);
   void resetMetacharacterSet(char metaChar);
   void setMetaAnchorAtBegin(char metaChar);
   void setMetaAnchorAtEnd(char metaChar);
   void setMetaRepeat0(char metaChar);
   void setMetaRepeat1(char metaChar);
   void setMetaAnyChar(char metaChar);
   void setMetaAnyString(char metaChar);
   void setMetaSetBegin(char metaChar);
   void setMetaSetEnd(char metaChar);
   void setMetaSetNegate(char metaChar);
   void setMetaSetRange(char metaChar);
   void setMetaGroupBegin(char metaChar);
   void setMetaGroupEnd(char metaChar);
   void setMetaEscape(char metaChar);
   void setMetaSubstitute(char metaChar);
   void setEscapeRequiredForGrouping(bool flag);

protected:
   int regcomp(const char *pattern);
   int pushExp(RegExpTypeTag exptype,const char *data);
   int modifyExp(const int minLimit,const int maxLimit);
   int pushLit(CLiteralBuffer* litbuf);
   int regexec(const char *string,int nmatch,struct regmatch_t *pmatch,int eflags);
   int tryre();
   int tryreNoCase();
}; 

//_____________________________________________________________________________
class CRegularExpression : public CRegexp
{
   DECLARE_DYNAMIC(CRegularExpression);
   
private:
   CString m_regularExpression;
   CString m_substituteExpression;
   CString m_matchString;
   bool m_anchorEntire;  // force pattern to be anchored at beginning and end
   struct regmatch_t m_regMatch[MaxSubExpressions];
   
public:
   CRegularExpression();
   CRegularExpression(const char* regularExpression,bool anchorEntire=false);
   CRegularExpression(const char* regularExpression,const char* substituteExpression,bool anchorEntire=false);
   virtual ~CRegularExpression();

   CString getRegularExpression() const { return m_regularExpression; }

   virtual void setMetacharacters();
   
   virtual struct regmatch_t getSubMatch(int index) { return m_regMatch[index]; }
   
   virtual void setRegularExpression(const char* regularExpression,bool anchorEntire=false);
   virtual void setSubstituteExpression(const char* substituteExpression);
   virtual bool matches(const char* string);
   virtual CString getSubstitution();
   virtual CString getSubstitution(const char* string);
   virtual CString getSubstitution(const char* string,const char* substituteExpression);
   virtual CString evalSubstitution(const char* substituteExpression);

   static CString fixRegularExpression(const CString& regularExpression);
};

//_____________________________________________________________________________
class CRegularExpressionList : public CTypedPtrListContainer<CRegularExpression*>
{
public:
   CRegularExpressionList();

   const CRegularExpression* findFirstMatchingExpression(const CString& string) const;
   bool isStringMatched(const CString& string) const;
};

//_____________________________________________________________________________
#endif

