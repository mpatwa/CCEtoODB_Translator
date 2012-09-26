REM Batch file for building libraries for
REM WATCOM C/C++ 10.x 32-bit

wcl386 -w3 -c -D__WATCOM__ -I..\include -I%CCDK2%\include *.c > w105.err
wlib -o=..\lib\anw105.lib ..\lib\anw105.lib @w105o.dat
del *.obj

goto out

REM compile with spline option
wcl386 -w3 -c -DSPLINE -D__WATCOM__ -I..\include -I%CCDK2%\include *.c >> w105.err
wlib -o=..\lib\asw105.lib ..\lib\asw105.lib @w105o.dat
del *.obj

:out
