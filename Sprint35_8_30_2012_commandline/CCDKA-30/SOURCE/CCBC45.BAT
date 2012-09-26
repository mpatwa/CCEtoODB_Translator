REM Batch file for building libraries for
REM Borland C/C++ 4.5 32-bit

bcc32 -c -w3 -I..\include -I%CCDK2%\include -D__BRLNDC__ *.c > bc45.err
tlib /p32  ..\lib\anb45.lib @bc45o.dat
del *.obj

goto out

bcc32 -c -w3 -I..\include -I%CCDK2%\include -DSPLINE -D__BRLNDC__ *.c >> bc45.err
tlib /p32  ..\lib\asb45.lib @bc45o.dat
del *.obj

:out

