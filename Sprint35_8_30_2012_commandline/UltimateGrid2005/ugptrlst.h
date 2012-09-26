// $Header: /CAMCAD/4.6/UltimateGrid2005/ugptrlst.h 1     5/08/06 8:28p Kurt Van Ness $

/*************************************************************************
				Class Declaration : CUGPtrList
**************************************************************************
	Source file : ugptrlst.cpp
	Header file : ugptrlst.h
	Copyright © The Code Project 1994 - 2002, All Rights Reserved

	Purpose
		The CUGPrtList class is a specialized
		link list, which is used by the Ultimate
		Grid to store additional pointers that are
		assigned to the grid (i.e.: additional
		fonts, bitmaps, celltypes, etc.).

		This class also provides functions to add,
		remove, and access stored pointers or the
		additional information that is storred
		with each pointer.
*************************************************************************/
#ifndef _ugptrlst_H_
#define _ugptrlst_H_

class UG_CLASS_DECL CUGPtrList: public CObject
{
protected:
	
	typedef struct UGPtrListTag
	{
		BOOL    isUsed;
		LPVOID	pointer;
		long	param;
		UGID	id;
	}UGPtrList;

	UGPtrList * m_arrayPtr;

	int		m_maxElements;
	int		m_elementsUsed;

	int AddMoreElements();

public:
	CUGPtrList();
	~CUGPtrList();

	int AddPointer(void *ptr,long param = 0,UGID *id = NULL);
	
	LPVOID GetPointer(int index);
	long GetParam(int index);
	UGID* GetUGID(int index);

	int GetPointerIndex(void * ptr);

	int UpdateParam(int index,long param);

	int DeletePointer(int index);
	int EmptyList();

	int GetCount();
	int GetMaxCount();

	int InitEnum();
	void *EnumPointer();
};

#endif // _ugptrlst_H_