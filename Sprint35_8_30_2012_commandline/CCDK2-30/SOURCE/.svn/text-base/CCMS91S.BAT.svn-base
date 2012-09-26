REM Batch file for building libraries for
REM Microsoft C/C++ 8.0/9.x 32-bit 
REM (Extended DOS; requires Phar Lap TNT Extender 7.x)

cl -I..\include -D__MCRSFTC__ -D__WIN32__ -c *.c > ms91s.err
lib /OUT:..\lib\2nm91s.lib @ms91so.dat
del *.obj

REM compile with spline option
cl -I..\include -D__MCRSFTC__ -D__WIN32__ -DSPLINE -c *.c >> ms91s.err
lib /OUT:..\lib\2sm91s.lib @ms91so.dat
del *.obj


