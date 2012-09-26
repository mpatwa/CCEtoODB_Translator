REM Batch file to make libraries for 
REM Microsoft C 8.0 32-bit for windows (WIN 32, NT)

cl -I..\include -I%CCDK2%\include -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -c *.c > ms80sw.err
lib /OUT:..\lib\anm80sw.lib @ms80swo.dat
del *.obj

goto out

REM compile with spline option
cl -I..\include -I%CCDK2%\include -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -DSPLINE -c *.c >> ms80sw.err
lib /OUT:..\lib\asm80sw.lib @ms80swo.dat
del *.obj

:out
