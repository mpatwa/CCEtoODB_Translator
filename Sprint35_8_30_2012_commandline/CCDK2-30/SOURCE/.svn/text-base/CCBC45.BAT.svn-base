REM Batch file for building libraries for
REM Borland C/C++ 4.5 

bcc -c -ml -I..\include -D__BRLNDC__ *.c > bc45.err
tlib /p32  ..\lib\2nb45.lib @bc45o.dat
del *.obj

bcc -c -ml -I..\include -DSPLINE -D__BRLNDC__ *.c >> bc45.err
tlib /p32  ..\lib\2sb45.lib @bc45o.dat
del *.obj


