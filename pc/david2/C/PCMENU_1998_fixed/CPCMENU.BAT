@echo off
c:
if not exist c:\turboc.bat goto cargatc
call \turboc a:c\pcmenu\PCMENU.PRJ
goto fin
:cargatc
cd\borlandc\bin
bc a:\c\pcmenu\pcmenu.prj
:fin
@echo on
