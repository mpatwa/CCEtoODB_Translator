REM Batch file to build libraries for
REM Microsoft C/C++ 8.0 16-bit (Windows 3.x 16-bit)

cl -AL -c -I..\include -D__MCRSFTC__ -D__WINDOWS__ *.c > ms80rw.err
lib /PAGESIZE:32 ..\lib\2nm80rw.lib @ms80rwo.dat
del *.obj

REM compile with spline option
cl -AL -c -I..\include -DSPLINE -D__MCRSFTC__ -D__WINDOWS__ *.c >> ms80rw.err
lib /PAGESIZE:32 ..\lib\2sm80rw.lib @ms80rwo.dat
del *.obj


