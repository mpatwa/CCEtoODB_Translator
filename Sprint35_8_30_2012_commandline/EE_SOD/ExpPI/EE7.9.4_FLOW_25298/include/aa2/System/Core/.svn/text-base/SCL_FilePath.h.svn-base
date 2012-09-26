/* Copyright Mentor Graphics Corporation 2004

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
*/

#ifndef __SCL_FILE_PATH_H__
#define __SCL_FILE_PATH_H__

#include "SCL_String.h"

namespace SCL
{

/*!
<tt><b>FilePath</b></tt> is a standalone class that wraps the input full path name 
on the following components:
<ul>
	<li><b>base path</b> - disk or server name or root directory.<br>
	In the case of disk the first slash after disk name is a part of
	<tt>base path</tt>, so in <tt>"a:dir"</tt> and in <tt>"a:/dir"</tt>
	the <tt>base path</tt> is <tt>"a:"</tt> and <tt>"a:/"</tt> correspondingly.
	\note Disk name is searched by symbol <tt>":"</tt>, so the first part
	of path till <tt>":"</tt> is assumed as disk name even it begins with
	slashes, for example in <tt>"/abcd:/dir"</tt> <tt>base path</tt> is
	<tt>"/abcd:"</tt>. Be careful because it's not correct system disk name!
	<p>
	In the case of server name (path begins from two slashes) the first
	word after two leading slashes is a part of <tt>base path</tt>.
	So in <tt>"//serv.gr"</tt> and in <tt>"//serv.gr/dir"</tt> the
	<tt>base path</tt> is <tt>"//serv.gr"</tt>
	and <tt>"//serv.gr/"</tt> correspondingly.
	\note If path contains only from <tt>"//"</tt> or <tt>"\\"</tt> 
	then <tt>base path</tt> is equal to the path.
	<p>
	In the case of path beginning with a slash a <tt>base path</tt> is
	the beginning slash only.
	<p>
	If there are no beginning slashes or disk name then <tt>base path</tt>
	is empty.
	
	<li><b>relative path</b> - relative path from <tt>base path</tt>
	that ends with a slash. If there are no slashes after <tt>base path</tt>
	then <tt>relative path</tt> is empty.
	<p>	
	For example, in path <tt>"d:/win/sys/file.txt"</tt> <tt>relative path</tt>
	is <tt>"win/sys/"</tt>. But if path is <tt>"d:/win/sys"</tt> then
	<tt>relative path</tt> is <tt>win/</tt>.
	\note If there are only dots <tt>"."</tt> after <tt>base path</tt> of
	specified path then they are treated as <tt>full file name</tt>! In this
	case <tt>relative path</tt> is empty.
	
	<li><b>full file name</b> - is a part of path that begins from the
	last slash if any or otherwise from the vary beginning if there are
	no slashes.<br>
	<tt>full file name</tt> is subdivided into two logical parts:
	<ol>
		<li><b>file name</b> - is a part of <tt>full file name</tt>
		that begins from the beginning of <tt>full file name</tt>
		till the last dot <tt>"."</tt> but the leading dots
		are a part of <tt>file name</tt>. So, in full file name
		like <tt>"aa.ext"</tt> <tt>file name</tt> is <tt>"aa"</tt>
		but in <tt>".aa.ext"</tt> it's <tt>"<b>.</b>aa"</tt>.
		<li><b>extension</b> - is the rest part of <tt>full file name</tt>
		that is not <tt>file name</tt>.
		\note If extension is present then it begins from dot.
	</ol>
</ul>

*/
class SYSCORE_API FilePath
{

	string2 FullPath;
	string2 BasePath;	// Ex: "c:" (for relative path on c:),
						//		 "" ( for relative path on current drive), "\\Server\share" (UNC)
	string2 RelPath;	// Ex: "Winnt\System32\" (relative), "\Winnt\System32\" (from root)
	string2 FileName;	// Ex: "user32.dll"
	string2 Name;		// Ex: "user32"
	string2 Ext;		// Ex: ".dll"

	static int DefaultSeparator;

public:

	/*!
	Default constructor. 
	*/
	FilePath()
	{
	};
	/*!
	Constructs a FilePath object. If <tt>path</tt> is defined, the path will be split.
	*/
	FilePath(const char * path);

	/*!
	The member operator inits controlled path name by <tt>path</tt>.
	*/
	string operator = (const char * path);

	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then return <tt>base path name</tt>.
	*/
	string GetBasePath(const char * filename = NULL);
	/*!
	The member function replaces <tt>base path name</tt> with one from path name specified by <tt>bpath</tt>
	and returns <tt>full path name</tt>.
	*/
	string SetBasePathFromPath(const char * bpath);
	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then replaces <tt>base path name</tt> with one from path name specified by <tt>bpath</tt> and returns
	<tt>full path name</tt>.
	*/
	string SetBasePathFromPath(const char * filename, const char * bpath);

	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then returns <tt>relative path</tt>.
	*/
	string GetRelPath(const char * filename = NULL);
	/*!
	The member function replaces <tt>relative path</tt> with one from path name specified by <tt>rpath</tt>
	and returns <tt>full path name</tt>.
	*/
	string SetRelPathFromPath(const char * rpath);
	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then replaces <tt>relative path</tt> with one from path name specified by <tt>rpath</tt> and
	returns <tt>full path name</tt>.
	*/
	string SetRelPathFromPath(const char * filename, const char * rpath);

	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then returns <tt>name of the file</tt>.
	*/
	string GetFileName(const char * filename = NULL);
	/*!
	The member function replaces <tt>name of the file</tt> with one from path name specified by <tt>name</tt>.
	If <tt>name_only</tt> is <tt>true</tt> then <tt>name</tt> is used without
	extension. The function returns <tt>full path name</tt>.
	*/
	string SetFileNameFromPath(const char * name, bool name_only = true);
	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then replaces <tt>name of the file</tt> with one from path name specified by <tt>name</tt>.
	If <tt>name_only</tt> is <tt>true</tt> then <tt>name</tt> is used without
	extension. The function returns <tt>full path name</tt>.
	*/
	string SetFileNameFromPath(const char * filename, const char * name, bool name_only = true);

	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then returns <tt>extension of the file</tt>.
	*/
	string GetFileExtension(const char * filename = NULL);
	/*!
	The member function replaces <tt>extension of the file</tt> with one from path name specified by <tt>ext</tt> and
	returns <tt>full path name</tt>.
	*/
	string SetFileExtensionFromPath(const char * ext);
	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then replaces <tt>extension of the file</tt> with one from path name specified by <tt>ext</tt> and
	returns <tt>full path name</tt>.
	*/
	string SetFileExtensionFromPath(const char * filename, const char * ext);

	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then returns <tt>base path + relative path</tt>.
	*/
	string GetFileDirectory(const char * filename = NULL);
	/*!
	The member function replaces <tt>base path</tt> and <tt>relative path</tt> 
	with ones from path name specified by <tt>dir</tt> and returns <tt>full path name</tt>.
	*/
	string SetFileDirectoryFromPath(const char * dir);
	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then replaces <tt>base path</tt> and <tt>relative path</tt> with ones from path name specified by <tt>dir</tt> and
	returns <tt>full path name</tt>.
	*/
	string SetFileDirectoryFromPath(const char * filename, const char * dir);

	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then returns <tt>file name with extension</tt>.
	*/
	string GetFileNameExt(const char * filename = NULL);
	/*!
	The member function replaces <tt>file name</tt> and <tt>extension</tt>
	with ones from path name specified by <tt>name</tt> and returns <tt>full path name</tt>.
	*/
	string SetFileNameExtFromPath(const char * name);
	/*!
	The member function assigns <tt>filename</tt> (if any) to <tt>*this</tt>,
	then replaces <tt>file name</tt> and <tt>extension</tt> with ones from path name specified by <tt>name</tt> and
	returns <tt>full path name</tt>.
	*/
	string SetFileNameExtFromPath(const char * filename, const char * name);

	/*!
	The member function fills empty fields from corresponding <tt>def_path</tt>'s ones,
	then returns <tt>full path name</tt>.
	*/
	string SetDefault(const char * def_path);
	/*!
	The member function inserts <b>WHOLE</b> <tt>def_path</tt> logically before
	<tt>relative path</tt> if <tt>base path</tt> is empty.<br>
	Then returns <tt>full path name</tt>.
	<b>Example:</b><br>
	Before: <tt>MyDir\\MyFile;</tt><br>
	def_path: <tt>\\Dir\\Dir1.Ext;</tt><br>
	result: <tt>\\Dir\\Dir1.Ext\\MyDir\\MyFile</tt>
	\note The function adds a slash if it's not present at the end of <tt>file name</tt>
	of <tt>def_path</tt> and at the end of new <tt>relative path</tt> if it's not
	present yet. See example above.
	*/
	string SetDefPath(const char * def_path);
	/*!
	The member function replaces <tt>base path</tt> and <tt>relative path</tt> with ones from
	path name specified by <tt>directory</tt>, then replaces <tt>file name</tt> with one
	from path name specified by <tt>file name</tt>, then replaces 
	<tt>extansion</tt> with one from path name specified by <tt>extansion</tt>,
	then returns <tt>full path name</tt>.
	*/
	string SetFileDirNameExt(const char * dir, const char * name, const char * ext);

	/*!
	The member function appends OS-specific directory separator.
	*/
	void AddSeparator();

	/*!
	The member function returns <tt>true</tt> if full path is empty, otherwise - <tt>false</tt>.
	*/
	bool Empty()
	{
		return FullPath.empty(); 
	}

	/*!
	The member operator returns <tt>full path name</tt>.
	*/
	operator const string ()
	{
		return FullPath;
	}

private:
	virtual void Process(const string2& path_name);
	void Regenerate();
	void Clear();
};

} // namespace SCL

#endif // __SCL_FILE_PATH_H__

