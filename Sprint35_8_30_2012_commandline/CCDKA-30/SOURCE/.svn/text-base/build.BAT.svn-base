REM Run VCVARS32.BAT in Visual C++ BIN Directory to set environment variables for Command Line Compiler

REM Batch file to make libraries for 
REM Microsoft C 9.x 32-bit for Windows (WIN 32, NT)

REM DEBUG
cl -I..\include -I..\..\CCDK2-30\include -MTd -Fd..\lib\anm91swd.pdb -Zi -DDEBUG -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -c *.c > ms91swnd.err
lib /OUT:..\lib\anm91swd.lib @ms91swo.dat
del *.obj

REM compile with spline option
cl -I..\include -I..\..\CCDK2-30\include -MTd -Fd..\lib\asm91swd.pdb -Zi -DDEBUG -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -DSPLINE -c *.c > ms91swsd.err
lib /OUT:..\lib\asm91swd.lib @ms91swo.dat
del *.obj


REM RELEASE
cl -I..\include -I..\..\CCDK2-30\include -MT -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -c *.c > ms91swn.err
lib /OUT:..\lib\anm91sw.lib @ms91swo.dat
del *.obj

REM compile with spline option
cl -I..\include -I..\..\CCDK2-30\include -MT -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -DSPLINE -c *.c > ms91sws.err
lib /OUT:..\lib\asm91sw.lib @ms91swo.dat
del *.obj

: end
