REM Batch file to build libraries for
REM Microsoft C/C++ 8.0 16-bit (DOS)

cl -AL -c -I..\include -D__MCRSFTC__ *.c > ms80r.err
LIB /PAGESIZE:32 ..\LIB\2NM80R.LIB @ms80ro.dat
del *.obj

REM compile with spline option
cl -AL -c -I..\include -D__MCRSFTC__ -DSPLINE *.c >> ms80r.err
LIB /PAGESIZE:32 ..\lib\2SM80R.LIB @ms80ro.dat
del *.obj


