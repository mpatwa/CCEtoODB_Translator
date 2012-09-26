/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/
/****************************************************************

file name:      GL_LinConv.h

file type:      main line source

date:            12-28-96

author:         Alex Zabolotnikov

description:   GL_Transform class

THIS PROGRAM IS THE PROPERTY OF AND EMBODIES THE PROPRIETARY TRADE
SECRETS OF PADS SOFTWARE, INC.  UNAUTHORIZED USE AND COPYING ARE
PROHIBITED BY LAW INCLUDING UNITED STATES AND FOREIGN COPYRIGHT LAW

****************************************************************/


#ifndef _GL_CONV_H
#define _GL_CONV_H

#include <math.h>

class GL_Point;
class GL_Position;
class GL_DPoint;
class GL_Box;


//----------- points & angles linear convertions -----------------------
//      X  = ax * x + bx * y + cx
//      Y  = ay * x + by * y + cy
//      F0 = a0 * f0 + b0
//      FZ = az * fz + bz
//      R  = ad * r
//----------------------------------------------------------------------
class SYSGL_API GL_Transform
{
	public:
	double  ax;
	double  bx;
	double  cx;

	double  ay;
	double  by;
	double  cy;

	double  a0;
	double  b0;

	double  az;
	double  bz;

	double  ad;

	GL_Transform();
// added for compability with old GL_Transform
	GL_Transform (const GL_Transform &a_trans);
	GL_Transform (const GL_Position &a_pos,
						const bool a_alternative = false,
						const bool a_in_coord_locate = true,
						const bool a_out_coord_locate = false);
	GL_Transform (const long a_origin_x,
						const long a_origin_y,
						const double a_angle,
						const bool a_mirror = false,
						const bool a_alternative = false,
						const bool a_in_coord_locate = true,
						const bool a_out_coord_locate = false);
	GL_Transform (const GL_Point &a_origin,
						const double a_angle,
						const bool a_mirror = false,
						const bool a_alternative = false,
						const bool a_in_coord_locate = true,
						const bool a_out_coord_locate = false);
	void	Transform		(GL_Point *)	const;	
	void	Transform		(GL_DPoint *point)	const;	
	GL_Point	Transform		(const GL_Point &point)	const;	
	GL_DPoint	Transform		(const GL_DPoint &point)	const;	
	double		GetOrientation() const;
// end add

	void	SetBoxConv(GL_Point &ll, GL_Point &ur, GL_Box &rect);
	void    SetMove(double dx, double dy);
	inline void SetMove(long x, long y){ SetMove((double)x, (double)y);}
	void    SetRotate(const GL_Point &Ori, double fi);
	void    SetRotate(const GL_Point &Ori, double sn, double cs);
	void    SetScale(GL_Point &Ori, double scale);
	void    SetMirror(GL_Point &Ori, double fi);
	void    SetMirror(GL_Point &Ori, double sn, double cs);
	short   SetRConv(GL_Transform &RConv) const;
	void 	SetCoordSys(GL_Point &Ori, double fi);
	void 	SetCoordSys(GL_Point &Ori, double sn, double cs);
	void    RConv();
	void 	Convert(long &x)			const;
	void    Convert(GL_Point &nP)		const;
	void    Convert(double &start, double &size)		const;
	void	BoxToBox(GL_Box &from, GL_Box &to)	const;
	short   IsRotate()		const;
	short   GetMirror()		const;
	short   IsScale()			const;
	void    AddConv(const GL_Transform &next);
	void    SubConv(const GL_Transform &next);

	GL_Transform operator=(const GL_Transform &next);
	GL_Transform operator=(const GL_Transform *next);
	GL_Transform operator+(const GL_Transform &next);
	GL_Transform operator-(const GL_Transform &next);
	GL_Point operator+(GL_Point nP);
	GL_Point operator-(GL_Point nP);
	inline long operator+(long x) { return LROUND(ad * x);}
	inline long operator-(long x) { return LROUND(x / ad);}
	short   operator==(const GL_Transform &next);
	short   operator!=(const GL_Transform &next);
	~GL_Transform();
};

#endif

// end of file

