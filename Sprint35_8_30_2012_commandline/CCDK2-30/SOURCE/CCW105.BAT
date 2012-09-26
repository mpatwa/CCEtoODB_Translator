REM Batch file for building libraries for
REM WATCOM C/C++ 10.x 32-bit

wcl386 -c -fpi87 -D__WATCOM__ -I..\include *.c > w105.err
wlib -o=..\lib\2nw105.lib ..\lib\2nw105.lib @w105o.dat
del *.obj

REM compile with spline option
wcl386 -c -fpi87 -DSPLINE -D__WATCOM__ -I..\include *.c >> w105.err
wlib -o=..\lib\2sw105.lib ..\lib\2sw105.lib @w105o.dat
del *.obj

