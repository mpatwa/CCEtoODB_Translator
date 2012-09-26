REM Run VCVARS32.BAT in Visual C++ BIN Directory to set environment variables for Command Line Compiler

REM Batch file to make libraries for 
REM Microsoft C 8.0/9.x 32-bit for windows (WIN 32, NT)


REM DEBUG
cl -I..\include -MTd -Fd..\lib\2nm91swd.pdb -Zi -DDEBUG -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -c *.c > ms91sw.err
lib /OUT:..\lib\2nm91swd.lib @ms91swo.dat
del *.obj

REM compile with spline option
cl -I..\include -MTd -Fd..\lib\2sm91swd.pdb -Zi -DDEBUG -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -DSPLINE -c *.c >> ms91sw.err
lib /OUT:..\lib\2sm91swd.lib @ms91swo.dat
del *.obj


REM RELEASE
cl -I..\include -MT -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -c *.c > ms91sw.err
lib /OUT:..\lib\2nm91sw.lib @ms91swo.dat
del *.obj

REM compile with spline option
cl -I..\include -MT -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -DSPLINE -c *.c >> ms91sw.err
lib /OUT:..\lib\2sm91sw.lib @ms91swo.dat
del *.obj

: end
