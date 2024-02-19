		  NAME    C0T
		  PAGE    60,132
		  LOCALS
;[]------------------------------------------------------------[]
;|      C0.ASM -- Start Up Code                                 |
;|                                                              |
;|      Turbo C++ Run Time Library                              |
;|                                                              |
;|      Copyright (c) 1987, 1990 by Borland International Inc.  |
;|      All Rights Reserved.                                    |
;[]------------------------------------------------------------[]

        INCLUDE RULES.ASI

;       Segment and Group declarations

_TEXT           SEGMENT BYTE PUBLIC 'CODE'
                ENDS
_FARDATA	SEGMENT PARA PUBLIC 'FAR_DATA'
                ENDS
_INIT_          SEGMENT WORD PUBLIC 'INITDATA'
InitStart       label byte
                ENDS
_INITEND_       SEGMENT BYTE PUBLIC 'INITDATA'
InitEnd         label byte
                ENDS
_EXIT_          SEGMENT WORD PUBLIC 'EXITDATA'
ExitStart       label byte
                ENDS
_EXITEND_       SEGMENT BYTE PUBLIC 'EXITDATA'
ExitEnd         label byte
                ENDS
_DATA           SEGMENT PARA PUBLIC 'DATA'
                ENDS
_CVTSEG         SEGMENT WORD PUBLIC 'DATA'
                ENDS
_SCNSEG         SEGMENT WORD PUBLIC 'DATA'
                ENDS
IFNDEF __HUGE__
  _BSS          SEGMENT WORD PUBLIC 'BSS'
                ENDS
  _BSSEND       SEGMENT BYTE PUBLIC 'ENDBSS'
                ENDS
ENDIF

IFNDEF __NOFLOAT__
  IF LDATA
    IFDEF  __HUGE__
      DGROUP GROUP _DATA,_CVTSEG,_SCNSEG
    ELSE
      DGROUP GROUP _DATA,_CVTSEG,_SCNSEG,_BSS,_BSSEND
    ENDIF
    IGROUP GROUP _INIT_,_INITEND_
    EGROUP GROUP _EXIT_,_EXITEND_
  ELSE
    IFDEF __TINY__
      DGROUP GROUP _TEXT,_INIT_,_INITEND_,_EXIT_,_EXITEND_,_DATA,_CVTSEG,_SCNSEG,_BSS,_BSSEND
    ELSE
      DGROUP GROUP _DATA,_CVTSEG,_SCNSEG,_BSS,_BSSEND
      IGROUP GROUP _INIT_,_INITEND_
      EGROUP GROUP _EXIT_,_EXITEND_
    ENDIF
  ENDIF
ELSE
  IF LDATA
    IFDEF __HUGE__
      DGROUP GROUP _DATA,_CVTSEG,_SCNSEG
    ELSE
      DGROUP GROUP _DATA,_CVTSEG,_SCNSEG,_BSS,_BSSEND
    ENDIF
    IGROUP GROUP _INIT_,_INITEND_
    EGROUP GROUP _EXIT_,_EXITEND_
  ELSE
    IFDEF __TINY__
      DGROUP GROUP _TEXT,_INIT_,_INITEND_,_EXIT_,_EXITEND_,_DATA,_CVTSEG,_SCNSEG,_BSS,_BSSEND
    ELSE
      DGROUP GROUP _DATA,_CVTSEG,_SCNSEG,_BSS,_BSSEND
      IGROUP GROUP _INIT_,_INITEND_
      EGROUP GROUP _EXIT_,_EXITEND_
    ENDIF
  ENDIF
ENDIF

        ASSUME  CS:_TEXT, DS:DGROUP

;       External References

extrn		principal:DIST
extrn		_exit:DIST
extrn		__exitbuf:DIST
extrn		__exitfopen:DIST
extrn		__exitopen:DIST
extrn		__stklen:word
extrn		__heaplen:word

        SUBTTL  Start Up Code
        PAGE
;/*                                                     */
;/*-----------------------------------------------------*/
;/*                                                     */
;/*     Start Up Code                                   */
;/*     -------------                                   */
;/*                                                     */
;/*-----------------------------------------------------*/
;/*                                                     */
PSPHigh         equ     00002h
PSPEnv          equ     0002ch
PSPCmd          equ     00080h

                public  __AHINCR
__AHINCR        equ     1000h
                public  __AHSHIFT
__AHSHIFT       equ     12

IFDEF	__NOFLOAT__
MINSTACK        equ     128     ; minimal stack size in words
ELSE
MINSTACK        equ     256     ; minimal stack size in words
ENDIF
;
;       At the start, DS and ES both point to the segment prefix.
;       SS points to the stack segment except in TINY model where
;       SS is equal to CS
;
_TEXT           SEGMENT
					 ORG     100h
STARTX          PROC    NEAR
;       Save general information, such as :
;               DGROUP segment address
;               DOS version number
;               Program Segment Prefix address
;               Environment address
;               Top of far heap

					 mov     dx, cs          ; DX = GROUP Segment address
					 mov     cs:DGROUP@@, dx
                mov     ah, 30h
                int     21h             ; get DOS version number
                mov     bp, ds:[PSPHigh]; BP = Highest Memory Segment Addr
                mov     bx, ds:[PSPEnv] ; BX = Environment Segment address
                mov     ds, dx
                mov     _version@, ax   ; Keep major and minor version number
                mov     _psp@, es       ; Keep Program Segment Prefix address
                mov     _envseg@, bx    ; Keep Environment Segment address
                mov     word ptr _heaptop@ + 2, bp
;
;       Save several vectors and install default divide by zero handler.
;
                call    SaveVectors

;       Count the number of environment variables and compute the size.
;       Each variable is ended by a 0 and a zero-length variable stops
;       the environment. The environment can NOT be greater than 32k.

                les     di, dword ptr _envLng@
                mov     ax, di
                mov     bx, ax
                mov     cx, 07FFFh      ; Environment cannot be > 32 Kbytes
                cld
@@EnvLoop:
                repnz   scasb
                jcxz    InitFailed      ; Bad environment !!!
                inc     bx              ; BX = Nb environment variables
                cmp     es:[di], al
                jne     @@EnvLoop       ; Next variable ...
                or      ch, 10000000b
                neg     cx
                mov     _envLng@, cx    ; Save Environment size
                mov     cx, dPtrSize / 2
                shl     bx, cl
                add     bx, dPtrSize * 4
                and     bx, not ((dPtrSize * 4) - 1)
                mov     _envSize@, bx   ; Save Environment Variables Nb.

;       Determine the amount of memory that we need to keep

IF      LDATA
                mov     dx, ss
                sub     bp, dx          ; BP = remaining size in paragraphs
					 mov     di, __stklen    ; DI = Requested stack size
;
; Make sure that the requested stack size is at least MINSTACK words.
;
                cmp     di, 2*MINSTACK  ; requested stack big enough ?
                jae     AskedStackOK
                mov     di, 2*MINSTACK  ; no --> use minimal value
					 mov        __stklen, di ; override requested stack size
AskedStackOK    label   near
                mov     cl, 4
                shr     di, cl          ; $$$ Do not destroy CL $$$
		inc	di
                cmp     bp, di		; DI = Stack size in paragraphs
                jnb     ExcessOfMemory  ; Much more available than needed
ELSE
                mov     dx, ds
                sub     bp, dx          ; BP = remaining size in paragraphs
                mov     di, __stklen    ; DI = Requested stack size
;
; Make sure that the requested stack size is at least MINSTACK words.
;
                cmp     di, 2*MINSTACK  ; requested stack big enough ?
                jae     AskedStackOK
                mov     di, 2*MINSTACK  ; no --> use minimal value
                mov     __stklen, di    ; override requested stack size
AskedStackOK    label   near
                add     di, offset DGROUP: edata@
                jb      InitFailed      ; DATA segment can NOT be > 64 Kbytes
                add     di, __heaplen
                jb      InitFailed      ; DATA segment can NOT be > 64 Kbytes
                mov     cl, 4
                shr     di, cl          ; $$$ Do not destroy CL $$$
                inc     di              ; DI = DS size in paragraphs
                cmp     bp, di
                jb      InitFailed      ; Not enough memory
                cmp     __stklen, 0
                je      ExpandDS        ; Expand DS up to 64 Kb
                cmp     __heaplen, 0
                jne     ExcessOfMemory  ; Much more available than needed
ExpandDS        label   near
                mov     di, 1000h
                cmp     bp, di
                ja      ExcessOfMemory  ; Enough to run the program
                mov     di, bp
                jmp     short ExcessOfMemory  ; Enough to run the program
ENDIF

;       All initialization errors arrive here

InitFailed      label   near
                jmp     near ptr _abort

;       Return to DOS the amount of memory in excess
;       Set far heap base and pointer

ExcessOfMemory  label   near
                mov     bx, di
                add     bx, dx
                mov     word ptr _heapbase@ + 2, bx
                mov     word ptr _brklvl@ + 2, bx
                mov     ax, _psp@
                sub     bx, ax          ; BX = Number of paragraphs to keep
                mov     es, ax          ; ES = Program Segment Prefix address
                mov     ah, 04Ah
                push    di              ; preserve DI
                int     021h            ; this call clobbers SI,DI,BP !!!!!!
                pop     di              ; restore  DI

                shl     di, cl          ; $$$ CX is still equal to 4 $$$

                cli                     ; req'd for pre-1983 88/86s
                mov     ss, dx          ; Set the program stack
                mov     sp, di
IF      LDATA
                mov     __stklen, di    ; save stack size
ENDIF
                sti


;       Reset uninitialized data area

                xor     ax, ax
                mov     es, cs:DGROUP@@
                mov     di, offset DGROUP: bdata@
                mov     cx, offset DGROUP: edata@
                sub     cx, di
                cld
	        rep     stosb

;       Prepare main arguments

                mov	ah, 0
                int	1ah			; get current BIOS time in ticks
                mov	word ptr _StartTime@,dx	; save it for clock() fn
					 mov	word ptr _StartTime@+2,cx

		xor	bp,bp			; set BP to 0 for overlay mgr
					 mov     si,offset DGROUP:InitStart	;si = start of table
                mov     di,offset DGROUP:InitEnd	;di = end of table
					 call    StartExit
					 mov     ds, cs:DGROUP@@

; OK, to save space we use self-modifying code.
; Replace the JA instruction in the startup/exit logic with
; a JB instruction.  This allows us to scan the table on exit 
; executing functions in reverse priority.  Also change the 
; immediate which is stored in ah at the first instruction to a 0.

                mov     byte ptr cs:JA_JB,72h
                mov     byte ptr cs:StartExit+1,0

;       ExitCode = principal(argc,argv,envp);

					 push    word ptr __C0environ
					 push    word ptr __C0argv
					 push    __C0argc
					 call    principal

;       Flush and close streams and files

                push    ax
                call    _exit

;---------------------------------------------------------------------------
;	_exitclean()
;
;       Runs EXIT functions.
;       Flushes and closes files.
;
;       Called by exit().
;
;Note : Falls through to _exit()
;---------------------------------------------------------------------------
__exitclean	PROC	NEAR
		PUBLIC	__exitclean
					 mov     si,offset DGROUP:ExitStart
					 mov     di,offset DGROUP:ExitEnd
					 call    StartExit

                mov     ds, cs:DGROUP@@
					 call	word ptr [__exitbuf]
					 call	word ptr [__exitfopen]
					 call	word ptr [__exitopen]
					 ENDP

;---------------------------------------------------------------------------
;	_exit()
;
;       Restore interrupt vectors taken during startup.  signal() functions
;	could have grabbed vectors 0, 4, 5 or 6.
;
;	Check for NULL pointer errors.
;
;	Exit to DOS.
;
;NOTE : _exit() doesn't close any files or run exit functions.  This is a
;	minimal 'cleanup & quit' program exit.
;---------------------------------------------------------------------------
__exit		PROC	DIST
		PUBLIC	__exit
                mov     ds, cs:DGROUP@@

                call    DIST ptr __restorezero	; restore captured INT vectors

;       Exit to DOS

ExitToDOS       label   near
                mov     bp,sp
                mov     ah,4Ch
                mov     al,[bp+cPtrSize]
                int     21h                     ; Exit to DOS
		ENDP
STARTX          ENDP

        SUBTTL  Vector save/restore & default Zero divide routines
        PAGE
;[]------------------------------------------------------------[]
;|                                                              |
;| Interrupt Save/Restore routines and default divide by zero   |
;| handler.                                                     |
;|                                                              |
;[]------------------------------------------------------------[]

ZeroDivision    PROC    FAR
                mov     cx, lgth_ZeroDivMSG
                mov     dx, offset DGROUP: ZeroDivMSG
                jmp     MsgExit3
ZeroDivision    ENDP

;--------------------------------------------------------------------------
;	savevectors()
;
;	Save vectors for 0, 4, 5 & 6 interrupts.  This is for extended
;	signal()/raise() support as the signal functions can steal these
;	vectors during runtime.
;--------------------------------------------------------------------------
SaveVectors	PROC    NEAR
                push    ds
; Save INT 0
                mov     ax, 3500h
                int     021h
                mov     word ptr _Int0Vector@, bx
                mov     word ptr _Int0Vector@+2, es
; Save INT 4
                mov     ax, 3504h
                int     021h
                mov     word ptr _Int4Vector@, bx
                mov     word ptr _Int4Vector@+2, es
; Save INT 5
                mov     ax, 3505h
                int     021h
                mov     word ptr _Int5Vector@, bx
                mov     word ptr _Int5Vector@+2, es
; Save INT 6
                mov     ax, 3506h
                int     021h
                mov     word ptr _Int6Vector@, bx
                mov     word ptr _Int6Vector@+2, es
;
;	Install default divide by zero handler.
;
                mov     ax, 2500h
                mov     dx, cs
                mov     ds, dx
                mov     dx, offset ZeroDivision
                int     21h

                pop     ds
                ret
SaveVectors	ENDP

;--------------------------------------------------------------------------
;	restorezero() puts back all the vectors that SaveVectors took.
;
;NOTE : TSRs must BE AWARE that signal() functions which take these 
;	vectors will be deactivated if the keep() function is executed.
;	If a TSR wants to use the signal functions when it is active it 
;	will have to save/restore these vectors itself when activated and
;	deactivated.
;--------------------------------------------------------------------------
__restorezero 	PROC	DIST
		PUBLIC	__restorezero
					 push    ds
					 mov     ax, 2500h
					 lds     dx, _Int0Vector@
					 int     21h
					 mov     ax, 2504h
					 lds     dx, _Int4Vector@
					 int     21h
					 mov     ax, 2505h
					 lds     dx, _Int5Vector@
					 int     21h
					 mov     ax, 2506h
					 lds     dx, _Int6Vector@
					 int     21h
					 pop     ds
					 ret
		ENDP

;------------------------------------------------------------------
;  Loop through a startup/exit (SE) table, 
;  calling functions in order of priority.
;  DS:SI is assumed to point to the beginning of the SE table
;  DS:DI is assumed to point to the end of the SE table
;  First 64 priorities are reserved by Borland
;------------------------------------------------------------------
PNEAR           EQU     0
PFAR            EQU     1
NOTUSED         EQU     0ffh

SE              STRUC
calltype        db      ?                       ; 0=near,1=far,ff=not used
priority        db      ?                       ; 0=highest,ff=lowest
addrlow         dw      ?
addrhigh        dw      ?
SE              ENDS

StartExit       PROC    NEAR
@@Start:        mov     ah,0ffh                 ; start with lowest priority
                mov     dx,di                   ; set sentinel to end of table
                mov     bx,si                   ; bx = start of table

@@TopOfTable:   cmp     bx,di                   ; and the end of the table?
                je      @@EndOfTable            ; yes, exit the loop
                cmp     [bx.calltype],NOTUSED   ; check the call type
                je      @@Next
                cmp     [bx.priority],ah        ; check the priority
JA_JB:          ja      @@Next                  ; too high?  skip
                mov     ah,[bx.priority]        ; keep priority
                mov     dx,bx                   ; keep index in dx
@@Next:         add     bx,SIZE SE              ; bx = next item in table
                jmp     @@TopOfTable

@@EndOfTable:   cmp     dx,di                   ; did we exhaust the table?
                je      @@Done                  ; yes, quit
                mov     bx,dx                   ; bx = highest priority item
                push    ds                      ; save ds
                pop     es
                push    es                      ; es = ds
                cmp     [bx.calltype],PNEAR     ; is it near or far?
                mov     [bx.calltype],NOTUSED   ; wipe the call type
                mov     ds, cs:DGROUP@@
                je      @@NearCall

@@FarCall:      call    DWORD PTR es:[bx.addrlow]
                pop     ds                      ; restore ds
                jmp     @@Start

@@NearCall:     call    WORD PTR es:[bx.addrlow]
                pop     ds                      ; restore ds
                jmp     @@Start

@@Done:         ret
StartExit       ENDP

ErrorDisplay    PROC    NEAR
                mov     ah, 040h
                mov     bx, 2
                int     021h
                ret
ErrorDisplay    ENDP

_abort		PROC	DIST
		PUBLIC	_abort
                mov     cx, lgth_abortMSG
					 mov     dx, offset DGROUP: abortMSG
MsgExit3        label   near
                mov     ds, cs: DGROUP@@
                call    ErrorDisplay
CallExit3       label   near
                mov     ax, 3
                push    ax
                call    __exit           ; _exit(3);
		ENDP

; The DGROUP@ variable is used to reload DS with DGROUP

PubSym@		DGROUP@, <dw    ?>, __PASCAL__

; __MMODEL is used to determine the memory model or the default
; pointer types at run time.

		public __MMODEL
__MMODEL	dw	MMODEL

		ENDS

        	SUBTTL  Start Up Data Area
        	PAGE
;[]------------------------------------------------------------[]
;|      Start Up Data Area                                      |
;|                                                              |
;|      WARNING         Do not move any variables in the data   |
;|                      segment unless you're absolutely sure   |
;|                      that it does not matter.                |
;[]------------------------------------------------------------[]

_DATA		SEGMENT

;	Magic symbol used by the debug info to locate the data segment
		public DATASEG@
DATASEG@	label	byte

;       The CopyRight string must NOT be moved or changed without
;       changing the null pointer check logic

CopyRight       db      4 dup(0)
					 db      '(c) BIT SOFT 1994',0
lgth_CopyRight  equ     $ - CopyRight


ZeroDivMSG      db      'Division por cero', 13, 10
lgth_ZeroDivMSG equ     $ - ZeroDivMSG

abortMSG        db      'Terminacion anomala', 13, 10
lgth_abortMSG   equ     $ - abortMSG

;
;			Interrupt vector save areas
;	
;	Interrupt vectors 0,4,5 & 6 are saved at startup and then restored
;	when the program terminates.  The signal/raise functions might
;	steal these vectors during execution.
;
;       Note: These vectors save area must not be altered 
;             without changing the save/restore logic.
;
PubSym@         _Int0Vector	<dd     0>,             __CDECL__
PubSym@         _Int4Vector	<dd     0>,             __CDECL__
PubSym@         _Int5Vector	<dd     0>,             __CDECL__
PubSym@         _Int6Vector	<dd     0>,             __CDECL__
;
;			Miscellaneous variables
;	
PubSym@         _C0argc,        <dw     0>,             __CDECL__
dPtrPub@        _C0argv,        0,                      __CDECL__
dPtrPub@        _C0environ,     0,                      __CDECL__
PubSym@         _envLng,        <dw     0>,             __CDECL__
PubSym@         _envseg,        <dw     0>,             __CDECL__
PubSym@         _envSize,       <dw     0>,             __CDECL__
PubSym@         _psp,           <dw     0>,             __CDECL__
PubSym@         _version,       <label word>,           __CDECL__
PubSym@         _osmajor,       <db     0>,             __CDECL__
PubSym@         _osminor,       <db     0>,             __CDECL__
PubSym@         errno,          <dw     0>,             __CDECL__
PubSym@         _StartTime,     <dw   0,0>,             __CDECL__


;       Memory management variables

IF      LDATA EQ false
PubSym@         __heapbase,     <dw   DGROUP:edata@>,   __CDECL__
PubSym@         __brklvl,       <dw   DGROUP:edata@>,   __CDECL__
ENDIF
PubSym@         _heapbase,      <dd   0>,       __CDECL__
PubSym@         _brklvl,        <dd   0>,       __CDECL__
PubSym@         _heaptop,       <dd   0>,       __CDECL__

        	ENDS

_CVTSEG 	SEGMENT
PubSym@         _RealCvtVector, <label  word>,  __CDECL__
        	ENDS

_SCNSEG 	SEGMENT
PubSym@         _ScanTodVector,  <label word>,  __CDECL__
        	ENDS

IFNDEF __HUGE__
_BSS		SEGMENT
bdata@          label   byte
		ENDS

_BSSEND		SEGMENT
edata@          label   byte
        	ENDS
ENDIF

			END     STARTX