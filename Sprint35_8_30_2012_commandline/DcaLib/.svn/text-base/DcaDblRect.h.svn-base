// $Header: /CAMCAD/DcaLib/DcaDblRect.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaDblRect_h__)
#define __DcaDblRect_h__

#pragma once

//_____________________________________________________________________________
class CDblRect : public CObject
{
public:
	CDblRect(BOOL initToGrow = TRUE);
	CDblRect(const CDblRect &rect);
	CDblRect(double xmin, double xmax, double ymin, double ymax);

	double xMin;
	double xMax;
	double yMin;
	double yMax;

	void Init();
	void ReverseInit();

	double Width() { return xMax - xMin; };
	double Height() { return yMax - yMin; };
};

#endif
