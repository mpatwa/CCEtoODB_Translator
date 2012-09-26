REM Batch file to make libraries for 
REM Microsoft C 9.x 32-bit for Windows (WIN 32, NT)

cl -I..\include -I%CCDK2%\include -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -c *.c > ms91sw.err
lib /OUT:..\lib\anm91sw.lib @ms91swo.dat
del *.obj

goto out

REM compile with spline option
cl -I..\include -I%CCDK2%\include -D__MCRSFTC__ -D__WIN32__ -D__WINDOWS__ -DSPLINE -c *.c >> ms91sw.err
lib /OUT:..\lib\asm91sw.lib @ms91swo.dat
del *.obj

:out
