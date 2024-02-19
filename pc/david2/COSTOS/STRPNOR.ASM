  .MODEL        LARGE,C

  .CODE

  LOCAL @@

  PUBLIC strpnor

  strpnor       PROC    C

          ARG cad:word,idx:word,cnt:word,car:word
          USES SI
          
          MOV  CX,cnt
          JCXZ @@fin
          MOV  SI,cad
          MOV  SI,[si+2]
          MOV  AX,car
          ADD  SI,idx
          DEC  SI
     @@buc1:
          OR  BYTE PTR [SI],AL
          INC  SI
          LOOP @@buc1
     @@fin:
          RET

  strpnor endp

  end