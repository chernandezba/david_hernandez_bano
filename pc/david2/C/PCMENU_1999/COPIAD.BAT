echo off
md d:\david
md d:\david\pcmenu
md d:\david\pcmenu\tem
cd d:\david\pcmenu
xcopy pc*.c d: 
xcopy pc*.asm d:
xcopy incluido\*.h d:incluido\
xcopy pc*.ide d:
xcopy pc*.dsw d:
copy copia.com d:
copy copiaa.bat d:
copy pc*.txt d:
echo on
