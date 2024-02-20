echo off
md c:\david
cd c:\david
xcopy pc*.c c: 
xcopy pc*.asm c:
xcopy incluido\*.h c:incluido\
xcopy pc*.prj c:
xcopy pc*.dsk c:
xcopy pc*.obj c:
copy copia.exe c:
copy copiab.bat c:
echo on