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

// Name:				GL_Geom_Inline.h
// Date:				10-14-97
// Author:			Valery Veschinsky
// Description:	System geometry types declaration, inline functions


#ifndef __GL_GEOM_INLINE_H__
#define __GL_GEOM_INLINE_H__

#include	<math.h>
#ifdef UNIX
#include	<stdlib.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// Inline functions

// GL_Point

//	Constructs a GL_Point.
//
//	Parameters
//		x1   Specifies the value of the x member of GL_Point.
//		y1   Specifies the value of the y member of GL_Point.
//	Constructor a GL_Point.
inline GL_Point::GL_Point 
(
)
{
	x = 0;
	y = 0;
}
//	Constructor a GL_Point.
inline GL_Point::GL_Point 
(
	const GL_Point &a_pnt	//		pnt   GL_Point that specifies the values used to initialize GL_Point.
) : x(a_pnt.x), y(a_pnt.y)
{
}
//	Constructor a GL_Point.
inline GL_Point::GL_Point 
(
	const long a_x1,	//		x1   Specifies the value of the x member of GL_Point.
	const long a_y1		//		y1   Specifies the value of the y member of GL_Point.
) :x(a_x1), y(a_y1)
{
}

// Initiate point's coord to zero.
inline void GL_Point::Initiate 
(
)
{
	x = 0; y = 0;
}

//	Checks for validity of the geometry object.
//
//	Return Value
//		True for a valid point.
//	Remarks
//		The point is valid if both coordinates belong to coordinate range between GL_MIN_COORD
//		and GL_MAX_COORD values.
inline bool GL_Point::IsValid 
(
) const
{
///	return GL_ISVALIDCOORD(x) && GL_ISVALIDCOORD(y);
	return true;
}

//	Return Value
//		A double-precision point converted from the GL_Point.
//	Remarks
//		This useful casting operator provides an efficient method to convert a GL_Point
//		object to a GL_DPoint ones.
inline GL_Point::operator GL_DPoint 
(
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return GL_DPoint(x, y);
}

//	Return Value
//		A GL_Point initialized by a coords of this point, mutiplied by mult.
inline GL_Point GL_Point::operator * 
(
	const long mult			
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return GL_Point( x * mult, y * mult );
}

//	Return Value
//		A GL_Point initialized by a coords of this point, mutiplied by mult.
inline GL_Point GL_Point::operator * 
(
	const double mult
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return GL_Point( long(x * mult), long(y * mult) );
}

//	Return Value
//		A GL_Point initialized by a coords of this point, divided by divider.
inline GL_Point GL_Point::operator / 
(
	const long divider
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return GL_Point( LROUND( double(x) / divider ), LROUND( double(y) / divider ) );
}

//	Return Value
//		A double value of Euclidian length of the GL_Point vector.
inline double GL_Point::Length() const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	if ( x == 0 ) {
		return fabs( double(y) );
	} else if ( y == 0 ) {
		return fabs( double(x) );
	} else if (ABS(int(x)) == ABS(int(y))) {
		return _SQRT2*(double)(ABS(int(x)));		
	} else {
		return sqrt(double(x) * double(x) + double(y) * double(y));
	}
}

//	Return Value
//		A double value of Euclidian distance between this GL_Point and the other.
inline double GL_Point::Distance
(
	const GL_Point& other
) const
{
	return (*this - other).Length();
}

//	Return Value
//		A GL_Point which represents the middle of the span [this, other].
inline GL_Point GL_Point::Median
(
	const GL_Point& other
) const
{
	return (*this + other) / 2 ;
}

//	Return Value
//		A double value of Manhattan length of the GL_Point vector.
inline long GL_Point::MLength
(
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return ABS(int(x)) + ABS(int(y));
}

//	Return Value
//		A double value of Manhattan distance between this GL_Point and the other.
inline long GL_Point::MDistance
(
	const GL_Point& other
) const
{
	return (*this - other).MLength();
}

//	Returns a x coordinate of the point.
//
//	Return Value
//		These functions return x coordinate of the point.
inline long GL_Point::GetX 
(
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return x;
}

//	Returns a y coordinate of the point.
//
//	Return Value
//		These functions return y coordinate of the point.
inline long GL_Point::GetY 
(
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return y;
}

// Set x coordinate of the point.
inline void GL_Point::SetX 
(
	const long a_x1				//		x1   Specifies the x coordinate of point.
)
{
	x = a_x1;
	//ASSERT_WRN(IsValid(),"Invalid point");
}

// Set y coordinates of the point.
inline void GL_Point::SetY 
(
	const long a_y1				//		y1   Specifies the y coordinate of point.
)
{
	y = a_y1;
	//ASSERT_WRN(IsValid(),"Invalid point");
}

// Set coordinates of the point.
inline void	GL_Point::Set 
(
	const long a_x1,			//		x1   Specifies the x coordinate of point.
	const long a_y1				//		y1   Specifies the y coordinate of point.

)
{
	x = a_x1; y = a_y1;
	//ASSERT_WRN(IsValid(),"Invalid point");
}

//	Assigns a new value to the GL_Point.
//
//	Return Value
//		The GL_Point that is initialized by a point.
//	Remarks
//		Assigns one point to another.
inline const GL_Point &GL_Point::operator= 
(
	const GL_Point &a_pnt			//		input point
)
{
	x = a_pnt.x; y = a_pnt.y; 
	//ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Assigns a new value to the GL_Point.
//
//	Return Value
//		The GL_Point that is initialized by a double precision point.
//	Remarks
//		Assigns one point to another.
inline const GL_Point &GL_Point::operator= 
(
	const GL_DPoint &a_pnt			//		input point
)
{
	x = LROUND(a_pnt.GetX()); y = LROUND(a_pnt.GetY()); 
	//ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Offsets GL_Point by adding a point.
//
//	Return Value
//		The GL_Point that is offset by a point.
//	Parameters
inline const GL_Point &GL_Point::operator+= 
(
	const GL_Point &a_pnt			//		Moving vector.
)
{
	x += a_pnt.x; y += a_pnt.y; 
	//ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Offsets GL_Point by adding a size.
//
//	Return Value
//		The GL_Point that is offset by a coordinate.
inline const GL_Point &GL_Point::operator+= 
(
	const long a_offset		//		offset value.
)
{
	x += a_offset; y += a_offset; 
	//ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Offsets GL_Point by subtracting a point.
//
//	Return Value
//		The GL_Point that is the difference between two points.
inline const GL_Point &GL_Point::operator-= 
(
	const GL_Point &a_pnt			//		pnt   Contains GL_Point object.
)
{
	x -= a_pnt.x; y -= a_pnt.y; 
	//ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Offsets GL_Point by subtracting a size.
//
//	Return Value
//		The GL_Point that is the difference GL_Point that is offset
//		by the negation of a  coordinate
inline const GL_Point &GL_Point::operator-= 
(
	const long a_offset			//		offset   Contains a long variable.
)
{
	x -= a_offset; y -= a_offset; 
	//ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}
// Compares two points for eq. 
inline bool GL_Point::operator== 
(
	const GL_Point &a_pnt			// Second point
) const
{
	return x == a_pnt.x && y == a_pnt.y;
}

// Compares two points for neq. 
inline bool GL_Point::operator!= 
(
	const GL_Point &a_pnt			// Second point
) const
{
	return !(*this == a_pnt);
}

// Compares two points for lt. See the body for accuracy.
inline bool GL_Point::operator< 
(
	const GL_Point &a_pnt			// Second point
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	//ASSERT_WRN(a_pnt.IsValid(),"Invalid point");
	return (x < a_pnt.x) || ((x == a_pnt.x) && (y < a_pnt.y));
}

// Compares two points for gt. See the body for accuracy.
inline bool GL_Point::operator> 
(
	const GL_Point &a_pnt			// Second point
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	//ASSERT_WRN(a_pnt.IsValid(),"Invalid point");
	return (x > a_pnt.x) || ((x == a_pnt.x) && (y > a_pnt.y));
}

//	Represents point as an array of coordinates
//
//	Parameters
//		a_index	Zero-based index of a coordinate in the point
//	Return Value
//		Returns x [0] and y [1] coordinates of a point
//	Remarks
//		You can think of a GL_Point object as an array of coordinates. The overloaded
//		subscript ([]) operator returns a single coordinate specified by the zero-based
//		index in a_index.
//		
inline long GL_Point::operator[] 
(
	int a_index					// 0 or 1.
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return (a_index == 0) ? x : y;
}

// GL_DPoint
//	Constructs a GL_DPoint.
inline GL_DPoint::GL_DPoint 
(
)
{
	x = 0.;
	y = 0.;
}

//	Constructs a GL_DPoint.
inline GL_DPoint::GL_DPoint 
(
	const GL_DPoint &a_pnt		//		pnt   GL_DPoint that specifies the values used to initialize GL_DPoint.
) : x(a_pnt.x), y(a_pnt.y)
{
	//ASSERT_WRN(IsValid(),"Invalid point");
}

//	Constructs a GL_DPoint.
inline GL_DPoint::GL_DPoint 
(
	const double a_x1,		//		x1   Specifies the value of the x member of GL_DPoint.
	const double a_y1			//		y1   Specifies the value of the y member of GL_DPoint.
) : x(a_x1), y(a_y1)
{
	//ASSERT_WRN(IsValid(),"Invalid point");
}

// Initiates GL_DPoint by zero coordinates.
inline void GL_DPoint::Initiate 
(
)
{
	x = 0; y = 0;
}

//	Checks for validity of the point.
//
//	Return Value
//		True for a valid point.
//	Remarks
//		The point is valid if both coordinates belong to coordinate range between GL_MIN_COORD and GL_MAX_COORD values.
//
inline bool GL_DPoint::IsValid 
(
) const
{
	return IS_VALIDCOORD(x) && IS_VALIDCOORD(y);
}

//	Converts GL_DPoint to GL_Point
//
//	Return Value
//		A point converted from the GL_DPoint.
//	Remarks
//		This useful casting operator provides an efficient method to convert a GL_DPoint
//		object to a GL_Point ones.
//
inline GL_DPoint::operator GL_Point 
(
) const
{
	return GL_Point(LROUND(x), LROUND(y));
}

//	Returns a x coordinate of the point.
//
//	Return Value
//		These functions return x coordinate of the point.
inline double GL_DPoint::GetX 
(
) const
{
	//ASSERT_WRN(IsValid(),"Invalid point");
	return x;
}

//	Returns a y coordinate of the point.
//
//	Return Value
//		These functions return y coordinate of the point.
inline double GL_DPoint::GetY () const
{
//	ASSERT_WRN(IsValid(),"Invalid point");
	return y;
}

//	Set x coordinates of the point.
//
inline void GL_DPoint::SetX 
(
	const double a_x1			//		Specifies the x coordinate of point.

)
{
	x = a_x1;
//	ASSERT_WRN(IsValid(),"Invalid point");
}

//	Set y coordinates of the point.
inline void GL_DPoint::SetY 
(
	const double a_y1			//		Specifies the y coordinate of point.

)
{
	y = a_y1; 
//	ASSERT_WRN(IsValid(),"Invalid point");
}

//	Set x and y coordinates of the point.
inline void GL_DPoint::Set 
(
	const double a_x1,		//		Specifies the x coordinate of point.
	const double a_y1			//		Specifies the y coordinate of point.
)
{
	x = a_x1; y = a_y1;
//	ASSERT_WRN(IsValid(),"Invalid point");
}

//	Assigns a new value to the GL_DPoint.
//
//	Return Value
//		The GL_DPoint that is initialized by a double precision point.
inline const GL_DPoint &GL_DPoint::operator= 
(
	const GL_DPoint &a_pnt			// Input double precision point
)
{
	x = a_pnt.x; y = a_pnt.y; 
//	ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}
//	Assigns a new value to the GL_DPoint.
//
//	Return Value
//		The GL_DPoint that is initialized by a point.
inline const GL_DPoint &GL_DPoint::operator= 
(
	const GL_Point &a_pnt							// Input point
)
{
	x = (double)(a_pnt.GetX()); y = (double)(a_pnt.GetY()); 
//	ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Offsets GL_DPoint by adding a point.
//
//	Return Value
//		The GL_DPoint that is offset by a point.
inline const GL_DPoint &GL_DPoint::operator+= 
(
	const GL_DPoint &a_pnt			//		Contains GL_DPoint object.
)
{
	x += a_pnt.x; y += a_pnt.y; 
//	ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Offsets GL_DPoint by adding a coordinate.
//
//	Return Value
//		The GL_DPoint that is offset by double coordinate.
inline const GL_DPoint &GL_DPoint::operator+= 
(
	const double a_offset			//		offset   Contains a double variable.

)
{
	x += a_offset; y += a_offset; 
//	ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Offsets GL_DPoint by subtracting a point.
//
//	Return Value
//		The GL_DPoint that is the difference between two points.
inline const GL_DPoint &GL_DPoint::operator-= 
(
	const GL_DPoint &a_pnt			//		pnt   Contains GL_DPoint object.
)
{
	x -= a_pnt.x; y -= a_pnt.y; 
//	ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

//	Offsets GL_DPoint by subtracting a coordinate.
//
//	Return Value
//		The GL_DPoint that is the difference  a double coordinate.
inline const GL_DPoint &GL_DPoint::operator-= 
(
	const double a_offset			//		offset   Contains a double variable.
)
{
//	ASSERT_WRN(IsValid(),"Invalid point");
	x -= a_offset; y -= a_offset; 
//	ASSERT_WRN(IsValid(),"Invalid point");
	return *this;
}

// Compare two DoublePoint to eq.
inline bool GL_DPoint::operator== 
(
	const GL_DPoint &a_pnt			// Second GL_DPoint
) const
{
//	ASSERT_WRN(IsValid(),"Invalid point");
//	ASSERT_WRN(a_pnt.IsValid(),"Invalid point");
	return x == a_pnt.x && y == a_pnt.y;
}

// Compare two DoublePoint to neq.
inline bool GL_DPoint::operator!= 
(
	const GL_DPoint &a_pnt			// Second GL_DPoint
) const
{
//	ASSERT_WRN(IsValid(),"Invalid point");
//	ASSERT_WRN(a_pnt.IsValid(),"Invalid point");
	return !(*this == a_pnt);
}

// Compare two DoublePoint to lt. Please, see body for accuracy.
inline bool GL_DPoint::operator< 
(
	const GL_DPoint &a_pnt			// Second GL_DPoint
) const
{
//	ASSERT_WRN(IsValid(),"Invalid point");
//	ASSERT_WRN(a_pnt.IsValid(),"Invalid point");
	return (x < a_pnt.x) || ((x == a_pnt.x) && (y < a_pnt.y));
}

// Compare two DoublePoint to gt. Please, see body for accuracy.
inline bool GL_DPoint::operator> 
(
	const GL_DPoint &a_pnt			// Second GL_DPoint
) const
{
//	ASSERT_WRN(IsValid(),"Invalid point");
//	ASSERT_WRN(a_pnt.IsValid(),"Invalid point");
	return (x > a_pnt.x) || ((x == a_pnt.x) && (y > a_pnt.y));
}

//	Represents point as an array of coordinates
//
//	Parameters
//		a_index	Zero-based index of a coordinate in the point
//	Return Value
//		Returns x [0] and y [1] coordinates of a point
inline double GL_DPoint::operator[] 
(
	int a_index							// Index of coordinate
) const
{
//	ASSERT_WRN(IsValid(),"Invalid point");
	return (a_index == 0) ? x : y;
}

// GL_Location

//	Constructs a GL_Location.
inline GL_Location::GL_Location 
(
)
{
	Initiate();
}

//	Constructs a GL_Location.
inline GL_Location::GL_Location 
(
const GL_Location &a_loc			//		Specifies the location value used to initialize GL_Location.
) :	GL_Point(a_loc), orientation(a_loc.orientation)
{
}

//	Constructs a GL_Location.
inline GL_Location::GL_Location 
(	
	const GL_Point &a_pnt,			//		GL_Point that specifies the origin value used to initialize GL_Location.
	const double a_ang				//		Angle that specifies the orientation value used to initialize GL_Location.
) :	GL_Point(a_pnt), orientation(GL_ANGLE_NORM(a_ang))
{
}

//	Constructs a GL_Location.
inline GL_Location::GL_Location 
(
	const long a_x1,			//		Specifies the value of the x member of origin.
	const long a_y1,			//		Specifies the value of the y member of origin.
	const double a_ang				//		Angle that specifies the orientation value used to initialize GL_Location.
) :	GL_Point(a_x1, a_y1), orientation(GL_ANGLE_NORM(a_ang))
{
}

// Initiate by zero default location.
inline void GL_Location::Initiate ()
{
	GL_Point::Initiate(); orientation = 0;
}

//	Checks for validity of the location.
//
//	Return Value
//		True for a valid location.
//	Remarks
//		The location is valid if both origin and orientation are valid values.
//
inline bool GL_Location::IsValid 
(
) const
{
	return GL_Point::IsValid() && GL_ISVALIDANGLE(orientation);
}

//	Returns the origin of the location.
//
//	Return Value
//		This function returns the origin point of the location.
//	Remarks
//		Get-interface is the best way to access GL_Location members x and y.
//
inline const GL_Point &GL_Location::GetOrigin 
(
) const
{
	return *this;
}

//	Returns the orientation of the location.
//
//	Return Value
//		This function returns the orientation angle of the location.
//	Remarks
//		Get-interface is the best way to access GL_Location member orientation.
//
inline double GL_Location::GetOrientation 
(
) const
{
	return orientation;
}

//	Set origin point of the location.
inline void	GL_Location::SetOrigin 
(
	const GL_Point &a_pnt				//		Specifies an origin point of the location.
)
{
	GL_Point::operator=(a_pnt); 
//	ASSERT_WRN (IsValid(),"Invalid Location");
}

//	Set origin point of the location.
inline void GL_Location::SetOrigin 
(
	const long a_x1,				//		Specifies x coordinate of the origin.
	const long a_y1					//		Specifies y coordinate of the origin.
)
{
	x = a_x1; y = a_y1; 
//	ASSERT_WRN (IsValid(),"Invalid Location");
}

//	Set orientation angle of the location.
inline void	GL_Location::SetOrientation 
(
	const double a_ang					//		Specifies an orientation angle of the location.
)
{
	orientation = GL_ANGLE_NORM(a_ang); 
//	ASSERT_WRN (IsValid(),"Invalid Location");
}

//	Set both origin and orientation of the location.
inline void GL_Location::Set 
(
	const GL_Point &a_pnt,				//		Specifies an origin point of the location.
	const double a_ang					//		Specifies an orientation angle of the location.
)
{
	GL_Point::operator=(a_pnt); orientation = GL_ANGLE_NORM(a_ang); 
//	ASSERT_WRN (IsValid(),"Invalid Location");
}

// See comments for GL_Location::Set(const GL_Point &a_pnt, const double a_ang)
inline void GL_Location::Set 
(
	const long a_x1,				//		Specifies x coordinate of the origin.
	const long a_y1,				//		Specifies y coordinate of the origin.
	const double a_ang					//		Specifies an orientation angle of the location.
)
{
	x = a_x1; y = a_y1; orientation = GL_ANGLE_NORM(a_ang); 
//	ASSERT_WRN (IsValid(),"Invalid Location");
}

//	Assigns a new value to the GL_Location.
//
//	Return Value
//		The GL_Location that is initialized by a location.
//	Remarks
//		Assigns one location to another.
//
inline const GL_Location &GL_Location::operator= 
(
	const GL_Location &a_loc				// Assigned location
)
{
	x = a_loc.x; y = a_loc.y; orientation = a_loc.orientation; return *this;
}

// Compare two location by eq.
inline bool GL_Location::operator== 
(
	const GL_Location &a_loc				// Second location
) const
{
	return GL_Point::operator==(a_loc) && orientation == a_loc.orientation;
}

// Compare two location by neq.
inline bool GL_Location::operator!= 
(
	const GL_Location &a_loc				// Second location
) const
{
	return !(*this == a_loc);
}

// GL_Position

//	Constructs a GL_Position.
inline GL_Position::GL_Position 
(
)
{
	Initiate();
}

//	Constructs a GL_Position.
inline GL_Position::GL_Position 
(
	const GL_Position &a_pos				//		Specifies the position value used to initialize GL_Position.
) :	GL_Location(a_pos), mirror(a_pos.mirror)
{
}

//	Constructs a GL_Position.
inline GL_Position::GL_Position 
(
	const GL_Location &a_loc,				//		Specifies the location value used to initialize GL_Position.
	const bool a_mir					//		Specifies the mirror flag used to initialize GL_Position.
) : GL_Location(a_loc), mirror(a_mir ? 1 : 0)
{
}

//	Constructs a GL_Position.
inline GL_Position::GL_Position 
(
	const GL_Point &a_pnt,					//		GL_Point that specifies the origin value used to initialize location of the GL_Position.
	const double a_ang,						//		Angle that specifies the orientation value used to initialize location of the GL_Position.
	const bool a_mir					//		Specifies the mirror flag used to initialize GL_Position.
) :	GL_Location(a_pnt, a_ang), mirror(a_mir ? 1 : 0)
{
}

//	Constructs a GL_Position.
inline GL_Position::GL_Position 
(
	const long a_x1,					//		Specifies the value of the x member of the location origin.
	const long a_y1,					//		Specifies the value of the y member of the location origin.
	const double a_ang,						//		Angle that specifies the orientation value used to initialize location of the GL_Position.
	const bool a_mir					//		Specifies the mirror flag used to initialize GL_Position.
) :	GL_Location(a_x1, a_y1, a_ang), mirror(a_mir ? 1 : 0)
{
}

//	Initiate a GL_Position to zero values.
inline void GL_Position::Initiate 
(
)
{
	GL_Location::Initiate(); mirror = 0;
}

//	Checks for validity of the position.
//
//	Return Value
//		True for a valid position.
//	Remarks
//		The position is valid if location is valid value.
//
inline bool GL_Position::IsValid 
(
) const
{
	return GL_Location::IsValid() && (mirror == 0 || mirror == 1);
}

//	Returns the mirror flag of the position.
//
//	Return Value
//		This function returns the mirror flag of the position.
//	Remarks
//		Get-interface is the best way to access GL_Position member mirror.
//
inline bool GL_Position::GetMirror 
(
) const
{
	return mirror;
}

//	Set mirror flag of the position.
//
inline void	GL_Position::SetMirror 
(
	const bool a_mir					//		Specifies a mirror flag of the position.
)
{
	mirror = a_mir ? 1 : 0; 
}

//	Set both location and mirror flag of the position.
inline void	GL_Position::Set 
(
	const GL_Point &a_pnt,					//		Specifies a location origin point of the position.
	const double a_ang,						//		Specifies a location orientation angle of the position.
	const bool a_mir					//		Specifies a mirror flag of the position.
)
{
	mirror = a_mir ? 1 : 0; GL_Location::Set(a_pnt, a_ang);
//	ASSERT_WRN (IsValid(),"Invalid a Position");
}

//	Set both location and mirror flag of the position.
inline void	GL_Position::Set 
(
	const long a_x1,					//		Specifies x coordinate of the location origin.
	const long a_y1,					//		Specifies y coordinate of the location origin.
	const double a_ang,						//		Specifies a location orientation angle of the position.
	const bool a_mir					//		Specifies a mirror flag of the position.
)
{
	mirror = a_mir ? 1 : 0; GL_Location::Set(a_x1, a_y1, a_ang);
//	ASSERT_WRN (IsValid(),"Invalid a Position");
}

//	Assigns a new value to the GL_Position.
//
//	Return Value
//		The GL_Position that is initialized by a position.
//	Remarks
//		Assigns one position to another.
//
inline const GL_Position &GL_Position::operator= 
(
	const GL_Position &a_pos
)
{
	GL_Location::operator=(a_pos); mirror = a_pos.mirror; return *this;
//	ASSERT_WRN (IsValid(),"Invalid a Position");
}

//	Offsets GL_Position by adding a position.
inline const GL_Position &GL_Position::operator+= 
(
	const GL_Position &a_pos					//		pos   Contains GL_Position object.
)
{
	GL_Location::operator+=(a_pos); if (a_pos.mirror) mirror = !mirror; return *this;
//	ASSERT_WRN (IsValid(),"Invalid a Position");
}

//	Compare two position by eq.
inline bool GL_Position::operator== 
(
	const GL_Position &a_pos					// Second position
) const
{
	return GL_Location::operator==(a_pos) && mirror == a_pos.mirror;
}

//	Compare two position by ne.
inline bool GL_Position::operator!= 
(
	const GL_Position &a_pos					// Second position
) const
{
	return !(*this == a_pos);
}

// GL_Circle

//	Constructs a GL_Circle.
inline GL_Circle::GL_Circle 
(
)
{
	Initiate();
}

//	Constructs a GL_Circle.
inline GL_Circle::GL_Circle 
(
	const GL_Circle &a_cir				//		cir   Specifies the circle value used to initialize GL_Circle.
) :	GL_Point(a_cir), radius(a_cir.radius)
{
}

//	Constructs a GL_Circle.
inline GL_Circle::GL_Circle 
(
	const GL_Point &a_pnt,				//		pnt   GL_Point that specifies the center point value used to initialize GL_Circle.
	const long a_rad				//		rad   long that specifies the radius value used to initialize GL_Circle.
) :	GL_Point(a_pnt), radius(a_rad)
{
}

//	Constructs a GL_Circle.
inline GL_Circle::GL_Circle 
(
	const long a_xc,				//		xc  Specifies the value of the x member of the center point
	const long a_yc,				//		yc  Specifies the value of the y member of the center point.
	const long a_rad				//		rad   long that specifies the radius value used to initialize GL_Circle.
) :	GL_Point(a_xc, a_yc), radius(a_rad)
{
}

// Initiate a circle to zero values
inline void GL_Circle::Initiate 
(
)
{
	GL_Point::Initiate(); radius = 0;
}

//	Checks for validity of the circle.
//
//	Return Value
//		True for a valid circle.
//	Remarks
//		The circle is valid if both center point and radius are valid value.
//
inline bool GL_Circle::IsValid 
(
) const
{
	return GL_Point::IsValid();
}

//	Assigns a new value to the GL_Circle.
//
//	Return Value
//		The GL_Circle that is initialized by a circle.
//	Remarks
//		Assigns one circle to another.
//
inline const GL_Circle &GL_Circle::operator= 
(
	const GL_Circle &a_cir					// Assigned circle
)
{
	GL_Point::operator=(a_cir); radius = a_cir.radius; return *this;
}
// Comparing two circle by eq.
inline bool GL_Circle::operator== 
(
	const GL_Circle &a_cir					// Second circle
) const
{
	return radius == a_cir.radius && GL_Point::operator==(a_cir);
}

// Comparing two circle by eq.
inline bool GL_Circle::operator!= 
(
	const GL_Circle &a_cir					// Second circle
) const
{
	return !(*this == a_cir);
}

//	Returns center point of the circle.
//
//	Return Value
//		This function returns the center point of the circle.
//	Remarks
//		Get-interface is the best way to access GL_Circle member center.
//
inline const GL_Point &GL_Circle::GetCenter 
(
) const
{
	return *this;
}

//	Returns radius of the circle.
//
//	Return Value
//		This function returns the radius of the circle.
//	Remarks
//		Get-interface is the best way to access GL_Circle member radius.
//
inline long GL_Circle::GetRadius 
(
) const
{
	return radius;
}

//	Sets center point of the circle.
inline void	GL_Circle::SetCenter 
(
	const GL_Point &a_pnt					//		GL_Point that specifies the center point value.
)
{
	GL_Point::operator=(a_pnt); 
}

//	Sets center point of the circle.
inline void	GL_Circle::SetCenter 
(
	const long a_x1,					//		Specifies the value of the x member of the center point
	const long a_y1						//		Specifies the value of the y member of the center point.
)
{
	GL_Point::Set(a_x1, a_y1);
}

//	Sets radius of the circle.
inline void	GL_Circle::SetRadius 
(
	const long a_rad					//		long that specifies the radius value.
)
{
	radius = a_rad; 
}

//	Sets both center point and radius of the circle.
inline void	GL_Circle::Set
(
	const GL_Point &a_pnt,					//		GL_Point that specifies the center point value used to initialize GL_Circle.
	const long a_rad					//		long that specifies the radius value used to initialize GL_Circle.
)
{
	GL_Point::operator=(a_pnt); radius = a_rad; 
}

//	Sets both center point and radius of the circle.
inline void	GL_Circle::Set 
(
	const long a_x1,					//		Specifies the value of the x member of the center point
	const long a_y1,					//		Specifies the value of the y member of the center point.
	const long a_rad					//		long that specifies the radius value used to initialize GL_Circle.
)
{
	radius = a_rad; GL_Point::Set(a_x1, a_y1);
}

// GL_Segment

//	Constructs a GL_Segment.
//
//	Parameters
//		seg   Specifies the segment value used to initialize GL_Segment.
//		pnt1, pnt2   GL_Points that specify the start and end points value used to initialize GL_Segment.
//		x1, x2  Specify the value of the x member of the start and end points correspondingly
//		y1, y2  Specifies the value of the y member of the start and end points correspondingly.
inline GL_Segment::GL_Segment 
(
)
{
	Initiate();
}

//	Constructs a GL_Segment.
inline GL_Segment::GL_Segment 
(
	const GL_Segment &a_seg				//		Specifies the segment value used to initialize GL_Segment.
) :	start(a_seg.start), end(a_seg.end)
{
}

//	Constructs a GL_Segment.
inline GL_Segment::GL_Segment 
(
	const GL_Point &a_pnt1,				//		GL_Point that specify the start point value used to initialize GL_Segment.
	const GL_Point &a_pnt2					//		GL_Point that specify the end point value used to initialize GL_Segment.
) :	start(a_pnt1), end(a_pnt2)
{
}

//	Constructs a GL_Segment.
inline GL_Segment::GL_Segment 
(
	const long a_x1,					//		Specify the value of the x member of the start point
	const long a_y1,					//		Specify the value of the y member of the start point
	const long a_x2,					//		Specify the value of the x member of the end point
	const long a_y2						//		Specify the value of the y member of the end point
) :	start(a_x1, a_y1), end(a_x2, a_y2)
{
}

// Initiate a Segment to zero values
inline void GL_Segment::Initiate 
(
)
{
	start.Initiate(); end.Initiate();
}

//	Checks for validity of the segment.
//
//	Return Value
//		True for a valid segment.
//	Remarks
//		The segment is valid if both start and end points are valid value.
//
inline bool GL_Segment::IsValid 
(
) const
{
	return start.IsValid() && end.IsValid();
}

//	Assigns a new value to the GL_Segment.
//
//	Return Value
//		The GL_Segment that is initialized by a segment.
//	Remarks
//		Assigns one segment to another.
//
inline const GL_Segment &GL_Segment::operator= 
(
	const GL_Segment &a_seg
)
{
	start = a_seg.start; end = a_seg.end; 
//	ASSERT_WRN (IsValid(),"Invalid segment");
	return *this;
}

//	Offsets GL_Segment by adding a coordinate to both ends
inline const GL_Segment &GL_Segment::operator+= 
(
	const GL_Point &a_offset					// Adding offset
)
{
	start += a_offset; end += a_offset; 
//	ASSERT_WRN (IsValid(),"Invalid segment");
	return *this;
}
//	Offsets GL_Segment by adding a  point to both ends
inline const GL_Segment &GL_Segment::operator+= 
(
	const long a_offset					// Adding offset
)
{
	start += a_offset; end += a_offset; 
//	ASSERT_WRN (IsValid(),"Invalid segment");
	return *this;
}

//	Offsets GL_Segment by subtracting a point from both ends
inline const GL_Segment &GL_Segment::operator-= 
(
	const GL_Point &a_offset				//	Offset point.
)
{
	start -= a_offset; end -= a_offset; 
//	ASSERT_WRN (IsValid(),"Invalid segment");
	return *this;
}
//	Offsets GL_Segment by subtracting a coordinate from both ends
inline const GL_Segment &GL_Segment::operator-= 
(
	const long a_offset				//	offset a long variable.

)
{
	start -= a_offset; end -= a_offset; 
//	ASSERT_WRN (IsValid(),"Invalid segment");
	return *this;
}
// Compare two segments for eq.
inline bool GL_Segment::operator== 
(
	const GL_Segment &a_seg				// Second segment
) const
{
	return start == a_seg.start && end == a_seg.end;
}

// Compare two segments for ne.
inline bool GL_Segment::operator!= 
(
	const GL_Segment &a_seg				// Second segment
) const
{
	return !(*this == a_seg);
}

//	Represents segment as an array of points
//
//	Parameters
//		a_index	Zero-based index of a point in the segment
//	Return Value
//		Returns start [0] and end [1] points of segment
//	Remarks
//		You can think of a GL_Segment object as an array of points. The overloaded
//		subscript ([]) operator returns a single point specified by the zero-based
//		index in a_index.
//		
inline const GL_Point &GL_Segment::operator[] 
(
	int a_index
) const
{
	return (a_index == 0) ? start : end;
}

//	Returns start point of the segment.
//
//	Return Value
//		This function returns the start point of the segment.
//	Remarks
//		Get-interface is the best way to access GL_Segment member start.
//
inline const GL_Point &GL_Segment::GetStart 
(
) const
{
	return start;
}

//	Returns end point of the segment.
//
//	Return Value
//		This function returns the end point of the segment.
//	Remarks
//		Get-interface is the best way to access GL_Segment member end.
//
inline const GL_Point &GL_Segment::GetEnd 
(
) const
{
	return end;
}

//	Sets start point of the segment.
inline void GL_Segment::SetStart 
(
	const GL_Point &a_pnt				//	GL_Point that specifies the start point value of the segment.
)
{
	start = a_pnt; 
}

// See comments for GL_Segment::SetStart(const GL_Point &a_pnt)
inline void	GL_Segment::SetStart 
(
	const long a_x1,				//		Specify the value of the x member of the start point.
	const long a_y1					//		Specifies the value of the y member of the start point.
)
{
	start.Set(a_x1, a_y1);
}

//	Sets end of the segment.
inline void	GL_Segment::SetEnd 
(
	const GL_Point &a_pnt				//		GL_Point that specifies the end point value of the segment.
)
{
	end = a_pnt;
}

//	Sets end of the segment.
inline void GL_Segment::SetEnd 
(
	const long a_x1,				//		Specify the value of the x member of the end point.
	const long a_y1					//		Specifies the value of the y member of the start point.
)
{
	end.Set(a_x1, a_y1);
}

//	Sets both start and end points of the segment.
//
//	Parameters
//		pnt1, pnt2   GL_Points that specify the start and end points value used to initialize GL_Segment.
//		x1, x2  Specify the value of the x member of the start and end points correspondingly
//		y1, y2  Specifies the value of the y member of the start and end points correspondingly.
//	Remarks
//		Initializes the GL_Segment by a new values of start and end.
//
inline void	GL_Segment::Set 
(
	const GL_Point &a_pnt1,			//		GL_Point that specify the start point value used to initialize GL_Segment.
	const GL_Point &a_pnt2				//		GL_Point that specify the end point value used to initialize GL_Segment.
)
{
	start = a_pnt1; end = a_pnt2; 
}

// See comments for GL_Segment::Set(const GL_Point &a_pnt1, const GL_Point &a_pnt2)
inline void GL_Segment::Set 
(
	const long a_x1,				//		Specify the value of the x member of the start
	const long a_y1,				//		Specifies the value of the y member of the startpoint.
	const long a_x2,				//		Specify the value of the x member of the end point
	const long a_y2					//		Specifies the value of the y member of the end point.
)
{
	start.Set(a_x1, a_y1); end.Set(a_x2, a_y2); 
}

//	Returns difference between end and start x coordinates.
//
//	Return Value
//		This function returns the difference between end and start x coordinates of the segment.
//
inline long GL_Segment::GetDX 
(
) const
{
	return end.GetX() - start.GetX();
}

//	Returns difference between end and start y coordinates.
//
//	Return Value
//		This function returns the difference between end and start y coordinates of the segment.
//
inline long GL_Segment::GetDY 
(
) const
{
	return end.GetY() - start.GetY();
}

// GL_Arc

//	Constructs a GL_Arc.
inline GL_Arc::GL_Arc 
(
)
{
	Initiate();
}

//	Constructs a GL_Arc.
inline GL_Arc::GL_Arc 
(
	const GL_Arc &a_arc				//		arc   Specifies the arc value used to initialize GL_Arc.
) :	GL_Segment(a_arc),GL_Circle(a_arc)
{
}

//	Constructs a GL_Arc.
inline GL_Arc::GL_Arc 
(
	const GL_Point &a_pnt1,		//	   GL_Point that specifies the start point used to initialize GL_Arc.
	const GL_Point &a_pnt2,		//	   GL_Point that specifies the end point used to initialize GL_Arc.
	const GL_Point &a_pntc,		//	   GL_Point that specifies the center point used to initialize GL_Arc.
	const long a_rad			//	   long that specifies the radius value used to initialize GL_Arc.
) :	GL_Segment(a_pnt1, a_pnt2), GL_Circle(a_pntc, a_rad)
{
}

// Initiate GL_Arc by zero data
inline void GL_Arc::Initiate 
(
)
{
	GL_Segment::Initiate(); GL_Circle::Initiate();
}

//	Checks for validity of the arc parameters.
//
//	Return Value
//		True for a valid arc parameters.
//	Remarks
//		The arc has valid parameters if both ends, center point and radius are valid values.
//
inline bool GL_Arc::IsValid 
(
) const
{
	return GL_Segment::IsValid() && GL_Circle::IsValid();
}

//	Assigns a new value to the GL_Arc.
//
//	Return Value
//		The GL_Arc that is initialized by a arc.
//	Remarks
//		Assigns one arc to another.
//
inline const GL_Arc &GL_Arc::operator= 
(
	const GL_Arc &a_arc				// Assigned arc
)
{
	GL_Segment::operator=(a_arc); GL_Circle::operator=(a_arc); return *this;
}
// Compare two arcs for eq.
inline bool GL_Arc::operator== 
(
	const GL_Arc &a_arc				//	Second arc							
) const
{
	return GL_Segment::operator==(a_arc) && GL_Circle::operator==(a_arc);
}
// Compare two arcs for ne.
inline bool GL_Arc::operator!= 
(
	const GL_Arc &a_arc				// Second arc
) const
{
	return !(*this == a_arc);
}

//	Sets both ends, center point and radius of the arc.
inline void	GL_Arc::Set 
(
	const GL_Point &a_pnt1,			//		GL_Point that specifies the start point GL_Arc.
	const GL_Point &a_pnt2,			//		GL_Point that specifies the end point GL_Arc.
	const GL_Point &a_pntc,			//		GL_Point that specifies the center point GL_Arc.
	const long a_rad				//		long that specifies the radius value GL_Arc.
)
{
	GL_Segment::Set(a_pnt1, a_pnt2); GL_Circle::Set(a_pntc, a_rad); 
//	ASSERT_WRN (IsValid()," Invalid arc data");
}

// GL_Box

//	Constructs a GL_Box.
//
//	Parameters
//		rect   Specifies the rectangle value used to initialize GL_Box.
//		pnt1, pnt2   GL_Points that specify the start and end points value used to initialize GL_Box.
//		x1, x2  Specify the value of the x member of the start and end points correspondingly
//		y1, y2  Specifies the value of the y member of the start and end points correspondingly.
//	Remarks
//		Constructs a GL_Box object. If no arguments are given, start and end
//		are initialized by zero.
//
inline GL_Box::GL_Box 
(
) 
{
	ll.Initiate();
	ur.Initiate();
}

//	Constructs a GL_Box.
inline GL_Box::GL_Box 
(
	const GL_Box &a_box					//		Specifies the rectangle value used to initialize GL_Box.
) 
{
	*this = a_box;
}

//	Constructs a GL_Box.
inline GL_Box::GL_Box 
(
	const GL_Point &a_pnt1,			//		GL_Point that specify the start (low left) point value used to initialize GL_Box.
	const GL_Point &a_pnt2				//		GL_Points that specify the end (Up right) points value used to initialize GL_Box.
)
{
	ll = a_pnt1;
	ur = a_pnt2;
}

//	Constructs a GL_Box.
inline GL_Box::GL_Box 
(
	const long a_x1,				//		Specify the value of the x member of the start point
	const long a_y1,				//		Specify the value of the y member of the start point
	const long a_x2,				//		Specify the value of the x member of the end point
	const long a_y2					//		Specify the value of the y member of the end point
) 
{
	ll.x = a_x1;
	ll.y = a_y1;
	ur.x = a_x2;
	ur.y = a_y2;
}

//	Constructs a GL_Box.
inline GL_Box::GL_Box
(
	const GL_Point &a_pnt,
	const long a_offset
)
{
	Set(a_pnt, a_offset);
}


//	Checks for validity of the rectangle.
//
//	Return Value
//		True for a valid rectangle.
//	Remarks
//		The rectangle is valid if both start and end points are valid value.
//
inline bool GL_Box::IsValid 
(
) const
{
	return ll.x <= ur.x && ll.y <= ur.y;
}

//	Assigns a new value to the GL_Box.
//
//	Return Value
//		The GL_Box that is initialized by a rectangle.
//	Remarks
//		Assigns one rectangle to another.
//
inline const GL_Box &GL_Box::operator= 
(
	const GL_Box &a_box					// Assigned box
)
{
	this -> ll = a_box.ll;
	this -> ur = a_box.ur;
	return *this;
}
// Compare two boxes for eq.
inline 	bool GL_Box::operator==
(
  const GL_Box &a_loc					// Second box
) const
{
	return a_loc.ll == ll && a_loc.ur == ur;
}
// Compare two boxes for ne.
inline 	bool GL_Box::operator!=
(
	const GL_Box &a_loc					// Second box
) const
{
	return !(*this == a_loc);
}
//	Returns left x-coordinate of the rectangle.
//
//	Return Value
//		This function returns the left x-coordinate of the rectangle.
//	Remarks
//		Get-interface is the best way to access GL_Box member start.x.
//
inline long GL_Box::GetLeft 
(
) const
{
	return ll.x;
}

//	Returns right x-coordinate of the rectangle.
//
//	Return Value
//		This function returns the right x-coordinate of the rectangle.
//	Remarks
//		Get-interface is the best way to access GL_Box member ur.x.
//
inline long GL_Box::GetRight 
(
) const
{
	return ur.x;
}

//	Returns lower y-coordinate of the rectangle.
//
//	Return Value
//		This function returns the lower y-coordinate of the rectangle.
//	Remarks
//		Get-interface is the best way to access GL_Box member ll.y.
//
inline long GL_Box::GetLower 
(
) const
{
	return ll.y;
}

//	Returns upper y-coordinate of the rectangle.
//
//	Return Value
//		This function returns the upper y-coordinate of the rectangle.
//	Remarks
//		Get-interface is the best way to access GL_Box member ur.y.
//
inline long GL_Box::GetUpper 
(
) const
{
	return ur.y;
}

//	Set left x-coordinate of the rectangle.
//
inline void GL_Box::SetLeft 
(
	const	long	a_c
)
{
	ll.x = a_c;
}

//	Set right x-coordinate of the rectangle.
inline void GL_Box::SetRight 
(
	const	long	a_c
)
{
	ur.x = a_c;
}

//	Set lower y-coordinate of the rectangle.
inline void GL_Box::SetLower 
(
	const	long	a_c
)
{
	ll.y = a_c;
}

//	Set upper y-coordinate of the rectangle.
inline void GL_Box::SetUpper 
(
	const	long	a_c
)
{
	ur.y = a_c;
}


//	Returns lower left corner of the rectangle.
//
//	Return Value
//		This function returns the lower left corner of the rectangle.
//	Remarks
//		Get-interface is the best way to access GL_Box member ll.
//
inline const GL_Point &GL_Box::GetLowerLeft 
(
) const
{
	return ll;
}

//	Returns upper right corner of the rectangle.
//
//	Return Value
//		This function returns the upper right corner of the rectangle.
//	Remarks
//		Get-interface is the best way to access GL_Box member ur.
//
inline const GL_Point &GL_Box::GetUpperRight 
(
) const
{
	return ur;
}

//	Returns upper left corner of the rectangle.
//
//	Return Value
//		This function returns the upper left corner of the rectangle.
//
inline GL_Point GL_Box::GetUpperLeft 
(
) const
{
   GL_Point pnt;
	pnt.x = ll.x; pnt.y = ur.y;
	return pnt;
}

//	Returns lower right corner of the rectangle.
//
//	Return Value
//		This function returns the lower right corner of the rectangle.
//
inline GL_Point GL_Box::GetLowerRight 
(
) const
{
   GL_Point pnt;
	pnt.x = ur.x; pnt.y = ll.y;
	return pnt;
}

//	Returns difference between ur and ll x coordinates.
inline long GL_Box::GetDX 
(
) const
{
	return ur.GetX() - ll.GetX();
}

//	Returns difference between ur and ll y coordinates.
inline long GL_Box::GetDY 
(
) const
{
	return ur.GetY() - ll.GetY();
}

//	Sets lower left corner of the rectangle.
inline void GL_Box::SetLowerLeft 
(
	const GL_Point &a_pnt					// Seting lower left corner
)
{
	ll = a_pnt; 
//	ASSERT_WRN (IsValid(), "Invalid box");
}

//	Sets lower left corner of the rectangle.
inline void GL_Box::SetLowerLeft 
(
	const long a_x,					//		x1  Specify the value of the x member of the lower left corner.
	const long a_y						//		y1  Specifies the value of the y member of the lower left corner.
)
{
	ll.Set(a_x, a_y);
}

//	Sets upper right corner of the rectangle.
inline void GL_Box::SetUpperRight 
(
	const GL_Point &a_pnt						//		pnt   GL_Point that specifies the upper right corner of the rectangle.
)
{
	ur = a_pnt; 
}

//	Sets upper right corner of the rectangle.
inline void GL_Box::SetUpperRight 
(
	const long a_x,						//		x1  Specify the value of the x member of the upper right corner.
	const long a_y							//		y1  Specifies the value of the y member of the upper right corner.
)
{
	ur.Set(a_x, a_y);
}

//	Sets both lower left and upper right corners of the rectangle.
inline void GL_Box::Set 
(
	const GL_Point &a_pnt1,					//		GL_Points that specify the ll point value used to initialize GL_Box.
	const GL_Point &a_pnt2						//		GL_Points that specify the ur point value used to initialize GL_Box.
)
{
	ll = a_pnt1; ur = a_pnt2; 
}

//	Sets both lower left and upper right corners of the rectangle.
inline void	GL_Box::Set 
(
	const long a_x1,						//		Specify the value of the x member of the ll point	
	const long a_y1,						//		Specifies the value of the y member of the ll point.
	const long a_x2,						//		Specify the value of the x member of the ur point
	const long a_y2						//		Specifies the value of the y member of the ur point.
)
{
	ll.Set(a_x1, a_y1); ur.Set(a_x2, a_y2);
}


///////////////////////////////////////////////////////////////////////////////
// Inline functions

// GL_Geometry
inline bool GL_Geometry::IsValid() const
{
	return false;
}
inline GL_GeometryType GL_Geometry::GetGeometry() const
{
	return GLT_Undefined;
}

//
// GL_Node implementation
//

#endif // __GL_GEOM_INLINE_H__

//
// End of file
//

