/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/
// This is a part of the PowerSYS System C++ library.
// Copyright (C) 1996 PADS Software Inc.
// All rights reserved.
// test comment.

// Name:				GL_Geom.h
// Date:				03-15-96
// Author:			Valery Veschinsky
// Description:	System geometry types declaration.


#ifndef __GL_GEOM_H__
#define __GL_GEOM_H__

#include "GL_Geom_Defs.h"
class GL_Transform;

/////////////////////////////////////////////////////////////////////////////
// Classes declared in this file
//   in addition to standard primitive data types and various helper macros

class GL_Point;
class GL_DPoint;
class GL_Location;
class GL_Position;
class GL_Circle;
class GL_Segment;
class GL_Arc;
class GL_Box;
class GL_Transform;


/////////////////////////////////////////////////////////////////////////////
// Other includes from standard "C" runtimes


/////////////////////////////////////////////////////////////////////////////
// GL_Point

class SYSGL_API GL_Point
{
public:
// Constructors
	inline GL_Point();
	inline GL_Point(const GL_Point &a_pnt);
	inline GL_Point(const long a_x1, const long a_y1);
	inline void Initiate();

	inline bool IsValid() const;

// Attributes & Operations
	inline const GL_Point &operator=(const GL_Point &a_pnt);
	inline const GL_Point &operator=(const GL_DPoint &a_pnt);
	inline const GL_Point &operator+=(const GL_Point &a_pnt);
	inline const GL_Point &operator+=(const long a_offset);
	inline const GL_Point &operator-=(const GL_Point &a_pnt);
	inline const GL_Point &operator-=(const long a_offset);

	const GL_Point operator+(const GL_Point &a_pnt) const;
	const GL_Point operator+(const long a_offset) const;
	const GL_Point operator-(const GL_Point &a_pnt) const;
	const GL_Point operator-(const long a_offset) const;

	inline bool operator==(const GL_Point &a_pnt) const;
	inline bool operator!=(const GL_Point &a_pnt) const;
	inline bool operator<(const GL_Point &a_pnt) const;
	inline bool operator>(const GL_Point &a_pnt) const;

	inline long operator[] (int a_index) const;

	inline operator GL_DPoint() const;

	inline GL_Point operator * (const long mult) const;
	inline GL_Point operator * (const double mult) const;
	inline GL_Point operator / (const long divider) const;
	inline double Length() const;
	inline double Distance(const GL_Point& other) const;
	inline GL_Point Median (const GL_Point& other) const;
	inline long MLength() const;
	inline long MDistance(const GL_Point& other) const;

	inline long GetX() const;
	inline long GetY() const;
	inline void		 SetX(const long a_x1);
	inline void		 SetY(const long a_y1);
	inline void		 Set(const long a_x1, const long a_y1);

// Utilities
	double GetPolarRadius() const;
	double GetPolarAngle() const;
	double ScalarProduct(const GL_Point &a_pnt) const;
	double VectorProduct(const GL_Point &a_pnt) const;
	void Scale(double a_factor);
	void Scale(double a_factor_x, double a_factor_y);
	void Transform (const GL_Transform &a_trans);

	void	Draw()	const {}
	long x;
	long y;
protected:

};


/////////////////////////////////////////////////////////////////////////////
// GL_DPoint

class SYSGL_API GL_DPoint
{
// Constructors
public:
	inline GL_DPoint();
	inline GL_DPoint(const GL_DPoint &a_pnt);
	inline GL_DPoint(const double a_x1, const double a_y1);
	inline void Initiate();

	inline bool IsValid() const;

// Attributes & Operations
	inline const GL_DPoint &operator=(const GL_DPoint &a_pnt);
	inline const GL_DPoint &operator=(const GL_Point &a_pnt);
	inline const GL_DPoint &operator+=(const GL_DPoint &a_pnt);
	inline const GL_DPoint &operator+=(const double a_offset);
	inline const GL_DPoint &operator-=(const GL_DPoint &a_pnt);
	inline const GL_DPoint &operator-=(const double a_offset);

	const GL_DPoint operator+(const GL_DPoint &a_pnt) const;
	const GL_DPoint operator+(const double a_offset) const;
	const GL_DPoint operator-(const GL_DPoint &a_pnt) const;
	const GL_DPoint operator-(const double a_offset) const;

	inline bool operator==(const GL_DPoint &a_pnt) const;
	inline bool operator!=(const GL_DPoint &a_pnt) const;
	inline bool operator<(const GL_DPoint &a_pnt) const;
	inline bool operator>(const GL_DPoint &a_pnt) const;

	inline double operator[] (int a_index) const;

	inline operator GL_Point() const;

	inline double GetX() const;
	inline double GetY() const;
	inline void	SetX(const double a_x1);
	inline void	SetY(const double a_y1);
	inline void	Set(const double a_x1, const double a_y1);

// Utilities
	double GetPolarRadius() const;
	double GetPolarAngle() const;
	double ScalarProduct(const GL_DPoint &a_pnt) const;
	double VectorProduct(const GL_DPoint &a_pnt) const;
	void	Scale(double a_factor);
	void	Scale(double a_factor_x, double a_factor_y);
	void	Transform (const GL_Transform &a_trans);


	double x;
	double y;
protected:

};


/////////////////////////////////////////////////////////////////////////////
// GL_Location

class SYSGL_API GL_Location:public GL_Point
{
// Constructors
public:
	inline GL_Location();
	inline GL_Location(const GL_Location &a_loc);
	inline GL_Location(const GL_Point &a_pnt, const double a_ang);
	inline GL_Location(const long a_x1, const long a_y1, const double a_ang);
	inline void Initiate();

	inline bool IsValid() const;

// Attributes & Operations
	inline const GL_Location &operator=(const GL_Location &a_loc);
	const GL_Location &operator+=(const GL_Location &a_loc);

	const GL_Location operator+(const GL_Location &a_loc) const;

	inline bool operator==(const GL_Location &a_loc) const;
	inline bool operator!=(const GL_Location &a_loc) const;

	inline const GL_Point &GetOrigin() const;
	inline double GetOrientation() const;
	inline void SetOrigin(const GL_Point &a_pnt);
	inline void SetOrigin(const long a_x1, const long a_y1);
	inline void SetOrientation(const double a_ang);
	inline void Set(const GL_Point &a_pnt, const double a_ang);
	inline void Set(const long a_x1, const long a_y1, const double a_ang);

// Utilities
	void Transform (const GL_Transform &a_trans);

protected:
	double orientation;

};


/////////////////////////////////////////////////////////////////////////////
// GL_Position

class SYSGL_API GL_Position:public GL_Location
{
public:
// Constructors
	inline GL_Position();
	inline GL_Position(const GL_Position &a_pos);
	inline GL_Position(const GL_Location &a_loc, const bool a_mir);
	inline GL_Position(const GL_Point &a_pnt, const double a_ang, const bool a_mir);
	inline GL_Position(const long a_x1, const long a_y1, const double a_ang, const bool a_mir);
	inline void Initiate();

	inline bool IsValid() const;

// Attributes & Operations
	inline const GL_Position &operator=(const GL_Position &a_pos);
	inline const GL_Position &operator+=(const GL_Position &a_pos);

	const GL_Position operator+(const GL_Position &a_pos) const;

	inline bool operator==(const GL_Position &a_pos) const;
	inline bool operator!=(const GL_Position &a_pos) const;

	inline bool GetMirror() const;
	inline void	SetMirror(const bool a_mir);
	inline void	Set(const GL_Point &a_pnt, const double a_ang,  const bool a_mir);
	inline void	Set(const long a_x1, const long a_y1, const double a_ang,  const bool a_mir);

// Utilities
	void Transform (const GL_Transform &a_trans);

protected:
	bool mirror;

};


/////////////////////////////////////////////////////////////////////////////
// GL_Circle

class SYSGL_API GL_Circle:public GL_Point
{
public:
// Constructors
	inline GL_Circle();
	inline GL_Circle(const GL_Circle &a_cir);
	inline GL_Circle(const GL_Point &a_pnt, const long a_rad);
	inline GL_Circle(const long a_xc, const long a_yc, const long a_rad);
	inline void Initiate();

	bool IsValid() const;

// Attributes & Operations
	inline const GL_Circle &operator=(const GL_Circle &a_cir);

	inline bool operator==(const GL_Circle &a_cir) const;
	inline bool operator!=(const GL_Circle &a_cir) const;

	inline void	GetCircle(GL_Point &cent, long &rad);
	inline const	GL_Point &GetCenter() const;
	inline long	GetRadius() const;
	inline void	SetCenter(const GL_Point &a_pnt);
	inline void	SetCenter(const long a_x1, const long a_y1);
	inline void	SetRadius(const long a_rad);
	inline void	Set(const GL_Point &a_pnt, const long a_rad);
	inline void	Set(const long a_x1, const long a_y1, const long a_rad);
	inline void	GetLPoint(GL_Point &pnt);
	inline void	GetRPoint(GL_Point &pnt);

// Utilities
	GL_Box	GetBox();
	void	Transform (const GL_Transform &a_trans);

	long radius;
};

inline	void GL_Circle::GetLPoint(GL_Point &pnt)
{
	pnt.x = x - ABS(radius);
	pnt.y = y;
}

inline	void GL_Circle::GetRPoint(GL_Point &pnt)
{
	pnt.x = x + ABS(radius);
	pnt.y = y;
}
inline void	GL_Circle::GetCircle(GL_Point &pnt, long &rad)
{
	pnt.x = x;
	pnt.y = y;
	rad = radius;
}


/////////////////////////////////////////////////////////////////////////////
// GL_Segment

class SYSGL_API GL_Segment
{
public:
// Constructors
	inline GL_Segment();
	inline GL_Segment(const GL_Segment &a_seg);
	inline GL_Segment(const GL_Point &a_pnt1, const GL_Point &a_pnt2);
	inline GL_Segment(const long a_x1, const long a_y1, const long a_x2, const long a_y2);
	inline void Initiate();

	inline bool IsValid() const;

// Attributes & Operations
	inline const GL_Segment &operator=(const GL_Segment &a_seg);
	inline const GL_Segment &operator+=(const GL_Point &a_pnt);
	inline const GL_Segment &operator+=(const long a_offset);
	inline const GL_Segment &operator-=(const GL_Point &a_pnt);
	inline const GL_Segment &operator-=(const long a_offset);

	const GL_Segment operator+(const GL_Point &a_pnt) const;
	const GL_Segment operator+(const long a_offset) const;
	const GL_Segment operator-(const GL_Point &a_pnt) const;
	const GL_Segment operator-(const long a_offset) const;

	inline bool operator==(const GL_Segment &a_seg) const;
	inline bool operator!=(const GL_Segment &a_seg) const;

	inline const GL_Point &operator[] (int a_index) const;

	inline const GL_Point &GetStart() const;
	inline const GL_Point &GetEnd() const;
	inline void	SetStart(const GL_Point &a_pnt);
	inline void	SetStart(const long a_x1, const long a_y1);
	inline void	SetEnd(const GL_Point &a_pnt);
	inline void	SetEnd(const long a_x1, const long a_y1);
	inline void	Set(const GL_Point &a_pnt1, const GL_Point &a_pnt2);
	inline void	Set(const long a_x1, const long a_y1, const long a_x2, const long a_y2);

// Utilities
	inline long GetDX() const;
	inline long GetDY() const;
	double GetLength() const;
	GL_Box GetBox();
	void	Transform (const GL_Transform &a_trans);

	GL_Point start;
	GL_Point end;
protected:

};


/////////////////////////////////////////////////////////////////////////////
// GL_Arc

class SYSGL_API GL_Arc:public GL_Segment, public GL_Circle
{
public:
// Constructors
	inline GL_Arc();
	inline GL_Arc(const GL_Arc &a_arc);
	inline GL_Arc(const GL_Point &a_pnt1, const GL_Point &a_pnt2, const GL_Point &a_pntc, const long a_rad);
	inline void Initiate();

	inline bool IsValid() const;
	bool IsValidArc() const;

// Attributes & Operations
	inline const GL_Arc &operator=(const GL_Arc &a_arc);

	inline bool operator==(const GL_Arc &a_arc) const;
	inline bool operator!=(const GL_Arc &a_arc) const;

	inline void	Set(const GL_Point &a_pnt1, const GL_Point &a_pnt2, const GL_Point &a_pntc, const long a_rad);

// Utilities
	double GetStartAngle () const;
	double GetEndAngle () const;
	double GetSizeAngle () const;
	double GetLength () const;
	int GetXNearestExtrem(const GL_Point &a_pnt, GL_Point *min_max);
	int GetYNearestExtrem(const GL_Point &a_pnt, GL_Point *min_max);
	GL_Box GetBox();
	void Transform (const GL_Transform &a_trans);

protected:

};


/////////////////////////////////////////////////////////////////////////////
// GL_Box

class SYSGL_API GL_Box
{
public:
//	data
	GL_Point ll;
	GL_Point ur;

// Constructors
	inline GL_Box();
	inline GL_Box(const GL_Box &a_box);
	inline GL_Box(const GL_Point &a_pnt1, const GL_Point &a_pnt2);
	inline GL_Box(const GL_Point &a_pnt, const long a_offset);
	inline GL_Box(const long a_x1, const long a_y1, const long a_x2, const long a_y2);

	inline bool IsValid() const;
	void Invalidate() { Set(GL_MAX_COORD, GL_MAX_COORD, GL_MIN_COORD, GL_MIN_COORD); }

// Attributes & Operations
	inline const GL_Box &operator=(const GL_Box &a_box);
	inline bool operator==(const GL_Box &a_loc) const;
	inline bool operator!=(const GL_Box &a_loc) const;

	inline void	Set(const GL_Point &a_pnt1, const GL_Point &a_pnt2);
	void	Set(const GL_Point &a_pnt, const long a_offset);
	inline void	Set(const long a_x1, const long a_y1, const long a_x2, const long a_y2);

	inline long GetLeft() const;
	inline long GetRight() const;
	inline long GetLower() const;
	inline long GetUpper() const;

	inline void SetLeft(const long a_x);
	inline void SetRight(const long a_x);
	inline void SetLower(const long a_x);
	inline void SetUpper(const long a_x);

	inline long GetDX() const;
	inline long GetDY() const;
	inline const GL_Point &GetLowerLeft() const;
	inline const GL_Point &GetUpperRight() const;
	inline GL_Point GetUpperLeft() const;
	inline GL_Point GetLowerRight() const;
	inline void	SetLowerLeft(const GL_Point &a_pnt);
	inline void	SetLowerLeft(const long a_x, const long a_y);
	inline void	SetUpperRight(const GL_Point &a_pnt);
	inline void	SetUpperRight(const long a_x, const long a_y);
	long GetWidth() const { return GetRight() - GetLeft(); }
	long GetHeight() const { return GetUpper() - GetLower(); }
	void SetEmpty() { Set( 0, 0, 0, 0 ); }
	bool IsEmpty() const { return (GetWidth() == 0 || GetHeight() == 0); }
	GL_Point GetCenter() const;
	void SetCenter(GL_Point new_center);

// Utilities
	void	Combine (GL_Box &);
	void	Transform (const GL_Transform &a_trans);
	void	Increase	(long a_x, long a_y);
	void	Expand (long);

	bool	SetAsIntersection(GL_Box rect1, GL_Box rect2);
	int		Clip( GL_Box b, GL_Box * res );
	void	Move( GL_Point vector );
	bool	IsInclude(GL_Point point);
	bool	IsIntersect(GL_Box &a_box);
	bool	IsIntExpBox(GL_Box &a_box, long w);
	void Normalize();
};

/////////////////////////////////////////////////////////////////////////////
// GL_GeometryType

typedef enum {
	GLT_Undefined = 0,
	GLT_Point,
	GLT_Double_point,
	GLT_Location,
	GLT_Position,
	GLT_Circle,
	GLT_Segment,
	GLT_Arc,
	GLT_Rectangle
} GL_GeometryType;

/////////////////////////////////////////////////////////////////////////////
// GL_Geometry

class SYSGL_API GL_Geometry
{
public:

	virtual bool IsValid() const;
	virtual GL_GeometryType GetGeometry() const;
};

const long	GL_SOLID_FILL							= 0;
const long 	GL_ORTHOGONAL_HATCH_FILL			= 1;
const long 	GL_DIAGONAL_HATCH_FILL				= 2;
const long 	GL_ORTHOGONAL_CROSSHATCH_FILL	= 3;
const long 	GL_DIAGONAL_CROSSHATCH_FILL		= 4;
const long 	GL_TRANSPARENT_FILL					= 5;
const long 	GL_HOLLOW_FILL						= 6;
const long 	GL_SKETCH_FILL_1						= 7;
const long 	GL_SKETCH_FILL_2						= 8;
const long 	GL_SKETCH_FILL_3						= 9;

const long 	GL_NUM_FILL							= 10;

enum GL_FillStyle{
	// do not change the order
	GL_FillStyleUndefined = -1,
	GL_FillStyleSolid,
	GL_FillStyleHollow,
	GL_FillStyleHatch,
	GL_FillStyleCrossHatch,
	GL_FillStyleCustom
};

class SYSGL_API GL_FillStyleInfo{
public:
	GL_FillStyleInfo(){
		m_enStyle = GL_FillStyleUndefined;
	}
	
	GL_FillStyleInfo(GL_FillStyle enStyle, double dbAngle, const GL_Point &xyOrign, long lnStepX, long lnStepY, void *ptData)
		:m_enStyle(enStyle), 
		m_dbAngle(dbAngle), 
		m_xyOrign(xyOrign), 
		m_lnStepX(lnStepX), 
		m_lnStepY(lnStepY), 
		m_ptData(ptData){
	}

	GL_FillStyle m_enStyle; // line hatch, cross hatch, ... 
	
	double     m_dbAngle; // radians
	GL_Point  m_xyOrign;

	long	m_lnStepX; //horisontal (X) density
	long	m_lnStepY; //vertical (Y) density
	
	void *m_ptData; // custom defined hatch

	bool operator == (GL_FillStyleInfo &rfFSI){
		return (
			m_enStyle == rfFSI.m_enStyle && 
			m_dbAngle == rfFSI.m_dbAngle && 
			m_lnStepX == rfFSI.m_lnStepX &&
			m_lnStepY == rfFSI.m_lnStepY &&
			m_xyOrign == rfFSI.m_xyOrign && 
			m_ptData  == rfFSI.m_ptData);
	}

	bool operator != (GL_FillStyleInfo &rfFSI){
		return (
			m_enStyle != rfFSI.m_enStyle || 
			m_dbAngle != rfFSI.m_dbAngle || 
			m_lnStepX != rfFSI.m_lnStepX ||
			m_lnStepY != rfFSI.m_lnStepY ||
			m_xyOrign != rfFSI.m_xyOrign || 
			m_ptData  != rfFSI.m_ptData);
	}

};

inline long Round( double val ) { return long( val > 0 ? val + 0.5 : val - 0.5 ); }


/////////////////////////////////////////////////////////////////////////////
// GL_Transform

//#include "GL_LinConv.h"


///////////////////////////////////////////////////////////////////////////////
// Inline functions

//#include "GL_Geom_Inline.h"


#endif // __GL_GEOM_H__

//
// End of file
//

