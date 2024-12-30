echo off
cls
echo introduce disco para copia y
pause
copia pc*.c A:\c\pcmenu /nologo
copia pc*.asm A:\c\pcmenu /nologo
copia incluido\*.h A:\c\pcmenu\incluido /nologo
copia pc*.IDE A:\c\pcmenu /nologo
copia pc*.DSW A:\c\pcmenu /nologo
copia pc*.c A:\c\pcmenu\*.bak /nologo
copia pc*.asm A:\c\pcmenu\*.bak /nologo
copia incluido\pc*.h A:\c\pcmenu\incluido\*.bak /nologo
copia pc*.txt a:\c\pcmenu /nologo
echo on
