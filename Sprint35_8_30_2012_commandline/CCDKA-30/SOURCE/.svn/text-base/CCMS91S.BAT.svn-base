REM Batch file for building libraries for
REM Microsoft C/C++ 9.x 32-bit 
REM (Extended DOS; requires Phar Lap TNT Extender 7.x)

cl -I..\include -I%CCDK2%\include -D__MCRSFTC__ -D__WIN32__ -c *.c > ms91s.err
lib /OUT:..\lib\anm91s.lib @ms91so.dat
del *.obj

goto out

REM compile with spline option
cl -I..\include -I%CCDK2%\include -D__MCRSFTC__ -D__WIN32__ -DSPLINE -c *.c >> ms91s.err
lib /OUT:..\lib\asm91s.lib @ms91so.dat
del *.obj

:out


