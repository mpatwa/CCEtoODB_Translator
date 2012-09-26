/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_FSTREAM_H__
#define __SCL_FSTREAM_H__

namespace SCL
{

class SYSCORE_API file
{
private:
	file(const file & x);
	file & operator = (const file & x);

public:
	typedef long pos_type;

	enum seekdir {
		beg,
		cur,
		end
	};

protected:
	struct Impl;
	Impl * m_pImpl;

public:
	file();
	file(PCTSTR a_pszFileName, PCTSTR a_pszMode);
	virtual ~file();

	file * open(PCTSTR a_pszFileName, PCTSTR a_pszMode);
	void close();

	file & flush();

	bool is_open() const;
	bool eof() const;

	file & seek(pos_type a_Pos, seekdir a_Dir = beg);
	pos_type tell() const;

	file & read(void * buf, size_t count);
	file & write(void * buf, size_t count);

	file & get(PTSTR a_Str, size_t a_Count, char a_Delim = '\n');
	file & getline(PTSTR a_Str, size_t a_Count, char a_Delim = '\n');

	file & operator << (file & (*pfn)(file &));

	file & operator << (char a_Val);
	file & operator << (unsigned char a_Val);
	file & operator << (bool a_Val);
	file & operator << (short a_Val);
	file & operator << (unsigned short a_Val);
	file & operator << (int a_Val);
	file & operator << (unsigned int a_Val);
	file & operator << (long a_Val);
	file & operator << (unsigned long a_Val);
	file & operator << (float a_Val);
	file & operator << (double a_Val);

	file & operator << (const char * a_Val);
	file & operator << (unsigned const char * a_Val);

	file & operator >> (file & (*pfn)(file &));

	file & operator >> (char & a_Val);
	file & operator >> (unsigned char & a_Val);
	file & operator >> (bool & a_Val);
	file & operator >> (short & a_Val);
	file & operator >> (unsigned short & a_Val);
	file & operator >> (int & a_Val);
	file & operator >> (unsigned int & a_Val);
	file & operator >> (long & a_Val);
	file & operator >> (unsigned long & a_Val);
	file & operator >> (float & a_Val);
	file & operator >> (double & a_Val);

	file & operator >> (string & a_Val);
};

extern SYSCORE_API file & endl(file &);
extern SYSCORE_API file & ends(file &);
extern SYSCORE_API file & flush(file &);
extern SYSCORE_API file & ws(file &);

}; // namespace SCL

#endif // __SCL_FSTREAM_H__

