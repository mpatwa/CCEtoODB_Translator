#ifndef _APPRARC_H_
#define _APPRARC_H_


class SYSGL_API DrawPolyline // abstract class
{
public:
	int *length_poly;
	int max_num_poly;
	int crn_poly; // current index of polyline (current number of polylines is crn_poly+1)
	long *points;
	int max_points;
	int num_points; // current number of points of polylines
	long *circles;
	int max_circles;
	int num_circles; // current number of circles

	GL_Point *pnt;
public:
	DrawPolyline();

// Set external buffers for numbers of points in poly and for points (buffers format conforms to ::PolyPolyline() arguments format)
	void SetArrays(int *a_num_pt_poly, int a_max_poly, long *a_points, int a_max_points);

// Set external buffer for circles (buffer format is: center_x_1, center_y_1, ...)
	void SetCirclesArray(long *a_circles, int a_max_circles);

// Add poly for drawing. If (poly == NULL) then draw rest of buffers content and clear arrays.
	void AddPoly(GeomHdr *poly);

// At the moment of call drawing to be a_num_poly polylines in the array (a_num_poly <= max_poly)
	virtual void DrawPolylines(int a_num_poly) = 0; // Pure virtual function

// At the moment of call drawing to be a_num_circles in the array (a_num_circles <= max_poly)
	virtual void DrawCircles(int a_num_circles) = 0; // Pure virtual function

	void AddPntInArray(const GL_Point &pt);
	void AddPolyInArray();
};


#endif // Of #ifndef _APPRARC_H_

