REM Batch file for building libraries for
REM Microsoft C/C++ 8.0 32-bit 
REM (Extended DOS; requires Phar Lap TNT Extender 7.x)

cl -I..\include -I%ccdk2%\include -D__MCRSFTC__ -D__WIN32__ -c *.c > ms80s.err
lib /OUT:..\lib\anm80s.lib @ms80so.dat
del *.obj

goto out

REM compile with spline option
cl -I..\include -I%ccdk2%\include -D__MCRSFTC__ -D__WIN32__ -DSPLINE -c *.c >> ms80s.err
lib /OUT:..\lib\asm80s.lib @ms80so.dat
del *.obj

:out


