procedure prepformatat;near;assembler;
asm
  mov ah,$17
  mov al,valorsubfunc17
  mov dl,unidadbios
  int $13
  mov ah,$18
  mov cl,byte ptr paramdisco.sectorespista
  mov ch,byte ptr paramdisco.maxcilindros
  cmp ch,80
  jz @@99
  add ch,2
  jmp @@100
@@99:
  add ch,4
@@100:
  mov dl,unidadbios
  int $13
  end;

