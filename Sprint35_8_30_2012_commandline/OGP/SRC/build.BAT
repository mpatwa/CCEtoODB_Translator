REM Run VCVARS32.BAT in Visual C++ BIN Directory to set environment variables for Command Line Compiler


REM DEBUG
cl -I..\include -MTd -Fd..\lib\ogpd.pdb -Zi -DDEBUG -c *.cpp database\*.cpp > ogpd.err
lib /OUT:..\lib\ogpd.lib *.obj
del *.obj

REM RELEASE
cl -I..\include -MT -c *.cpp database\*.cpp > ogp.err
lib /OUT:..\lib\ogp.lib *.obj
del *.obj

: end
