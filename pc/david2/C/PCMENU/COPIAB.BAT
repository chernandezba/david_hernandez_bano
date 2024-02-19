echo off
cls
echo introduce disco para copia y
pause
copia pc*.c b:\c\pcmenu /nologo
copia pc*.asm b:\c\pcmenu /nologo
copia incluido\*.h b:\c\pcmenu\incluido /nologo
copia pc*.prj b:\c\pcmenu /nologo
copia pc*.dsk b:\c\pcmenu /nologo
copia pc*.obj b:\c\pcmenu /nologo
copia pc*.c b:\c\pcmenu\*.bak /nologo
copia incluido\pc*.h b:\c\pcmenu\incluido\*.bak /nologo
echo on
