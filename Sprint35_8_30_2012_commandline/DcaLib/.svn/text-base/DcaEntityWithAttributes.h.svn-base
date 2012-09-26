// $Header: /CAMCAD/DcaLib/DcaEntityWithAttributes.h 3     6/30/07 2:59a Kurt Van Ness $

#if !defined(__DcaEntityWithAttributes_h__)
#define __DcaEntityWithAttributes_h__

#pragma once

class CAttributes;
class CAttribute;
class CCamCadData;

enum ValueTypeTag;
enum AttributeUpdateMethodTag;

//_____________________________________________________________________________
class CEntityWithAttributes
{
public:
	CEntityWithAttributes(CCamCadData& camCadData,bool hasEntityNumber = true, long entityNumber = -1);
	CEntityWithAttributes(const CEntityWithAttributes &other, bool copyAttributes = true);
	~CEntityWithAttributes();
	CEntityWithAttributes& operator=(const CEntityWithAttributes &other);

protected:
   CCamCadData& m_camCadData;

	CString m_className;

private:
	long m_entityNumber;
	CString m_name;
	CAttributes *m_attributes;

public:
   CCamCadData& getCamCadData() const;

	long GetEntityNumber() const					{ return m_entityNumber;	}

	CString GetName() const							{ return m_name;				}
	void SetName(CString name)						{ m_name = name;				}

   CAttributes* getAttributes() const        { return m_attributes;		}
   CAttributes*& getAttributesRef()          { return m_attributes;		}
   CAttributes& attributes();

	POSITION GetStartPositionAttrib() const;
   bool GetNextAssocAttrib(POSITION& nextPosition,WORD& key,CAttribute*& value) const;
   bool setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void* value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr);
   bool LookUpAttrib(WORD keyword, CAttribute *&attribute);
   void RemoveAttrib(WORD keyword);	
	//void KeywordMerge(CCamCadData& camCadData, WORD from, WORD to, int method);
	//void WriteXML(CWriteFormat& writeFormat, CCEtoODBDoc *doc);
};

#endif
