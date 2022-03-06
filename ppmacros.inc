;
; PPMACROS, written by E.J. van Veldhuizen
; version 1.00, 2004 - 2006, published on www.c4gen.com.
;
; The GNU General Public License (GPL) applies. 
; See www.gnu.org.
;
; Please report bugs, improvements or wishes, so the macros 
; stay up-to-date. Visit www.c4gen.com to see contacting
; details.
;
;	NOLIST
;
;
;**************************************************************
;***                         CHANGES                        ***
;**************************************************************
;
; v1.1
;   - "subwfb" is used as Mnemonic in newer PIC controllers. 
;     This name was used in PPMacros. Changed in "subwf_b"
;   - added loadl_i_f, loadl_i16_f, 
;   - bug in load_i_i fixed
;
; v1.2
;   - sub_l_f, sub_l16_f16 and sub_l32_f32 bug fixed
;
; v1.3
;   - added mul10_f_f, mul10_f16_f16, mul10_f32_f32
;   - added andwfb, iorwfb, xorwfb, swapfb, lslfb, lsrfb
;   - added add_f32_f16_f32
;   - gotop and callp always two bytes
;   - fixed gotopsz bugs in gotoif_..., added jcnttmp
;   - fixed callwp
;   - fixed gotoif_f16_ne_f16 (and if_f16_eq_f16)
;   - fixed bankset bug in load_f_b_f_b
; v1.4
;   - added PPM_BANKLARGE switch
;   - added PPM_BANKBLOCK, bankblockstart, bankblockend
;   - TODO added PPM_EXTENDED switch
;   - PPM_PAGESMALL switch
;   - added masks: gotoif_f_msk_etc
;   - added inc_f, inc_f16, dec_f, dec_f16, clr_f, clr_f16 and used in for-next etc.
;   - added bsfb (set_f_b), bcfb (clr_f_b)
;
;**************************************************************
;***                       INTRODUCTION                     ***
;**************************************************************
;
; This file contains macro definitions for MPASM. With these
; macros it is easier to program Microchip PIC processors,
; without having to buy a C-compiler. Just use these macros
; with the free MPLAB (including MPASM) program provided
; by MicroChip, the producer of the PIC processors. Download
; at www.microchip.com.
;
; The macros make it possible to have if-else-endif blocks,
; for-next and do-until loops. This will speed-up programming
; a lot and will decrease debugging time, since one does not
; have to use the twisted BTFSC and BTFSS instructions and
; difficult compare codes anymore.
;
; Also is taken care of the bank-bits (for data memory) and the
; page-bits (for program memory).
;
;**************************************************************
;***                     TIPS AND TRICKS                    ***
;**************************************************************
;
; * One is allowed to jump outside a loop or block. However,
;   one should be carefull when jumping INTO a block or loop.
; * Set the tab-size at 16 characters (options/
;   environment setup in MPLAB).
; * For debugging, open the .lst file which is created during
;   compiling or building. Start debugging (by using F7 or F8)
;   while in the window containing the .lst file. Debugging
;   in the original .asm file will be confusing, since MPLAB
;   will change all the time to the window with the macros.
;   This DID work up to version 6 of MPLAB. Hopefully, Microchip
;   will restore this function.
; * The chosen naming for the macros is based on the intention
;   to expand the macros with 16- and 32-bit words, 24- and
;   32-bit floating point variables and indirect addressing.
;   So, then macros will look like "if_fp24_gt_fp24".
;   However, everyone can change the naming of the macros
;   to whatever naming they please if (e.g.) another naming is
;   more close to their 'feeling' or knowledge of some
;   programming languages.
;   - Since the macros use eachother, use the 'replace' option
;     in the editor, in order to conserve the dependencies.
;   - You cannot use the words used by MPLAB itself, as:
;     "IF", "ELSE", "ENDIF", "END", "WHILE" etc. See the
;     help for MPASM (accessable in MPLAB) for all words.
;     This also explains the strange name "else_"...
; * Since all text (including this one :) ) will end up in the
;   .lst file and the .lst file is best used for debugging, one
;   can create a new include file in which all lines starting
;   with the semicolumn (;) can be deleted. 
;
;**************************************************************
;***                       HOW IT WORKS                     ***
;**************************************************************
;
; If you just want to use the macros, you can safely skip this
; part and continue to the SYNTAX section.
;
; In 1999, Karl Lunt already wrote a library and published a
; description in 'Nuts & Volts Magazine' (See
; http://www.nutsvolts.com/PDF_Files/picmacro.pdf and
; http://www.seanet.com/~karllunt/picmacro.htm).
; However, these for-next macros could not handle the
; two subquential loops inside another loops (written as
; BASIC statements):
;   for x=1 to 10
;     for y=1 to 3
;       statement
;     next y
;     for y=1 to 5
;       statement
;     next y
;   next x
;
; This problem was solved in these macros by using arrays in
; the MPASM compiler directives and expressions. Note: these
; are just arrays which are used by MPASM, it is not visable
; in the generated code or programmed in the chip!
; 
; The example above has two nesting levels. For every level
; (depth) an own bookkeeping is performed. If we just look at
; the outer loop (over x), in some simple code, it would look
; as follow (with after remark the original code).
;   remark for x=1 to 10
;   x=1
;   lab_1_1:
;   if x>10 goto lab_1_2
;     subloop
;   remark next x
;   x=x+1
;   goto lab_1_1
;   lab_1_2:
;
; If in nesting-level 1, always a label "lab_1_x" is used, one
; only has to increase the last number to get everything right.
;
; Now the subloop is nesting-level 2. The code for the first
; for-loop over y will roughly look the same as the code above.
; Now only 'lab_2_x' is used.
; The total code then will be:
;
;   remark for x=1 to 10
;   x=1
;   lab_1_1:
;   if x>10 goto lab_1_2
;
;     remark for y=1 to 3
;     y=1
;     lab_2_1:
;     if y>5 goto lab_2_2
;       statements
;     remark next y
;     y=y+1
;     goto lab_2_1
;     lab_2_2:
;
;     remark for y=1 to 5
;     y=1
;     lab_2_3:
;     if y>5 goto lab_2_4
;       statements
;     remark next y
;     y=y+1
;     goto lab_2_3
;     lab_2_4:
;
;   remark next x
;   x=x+1
;   goto lab_1_1
;   lab_1_2:
;
; As can be seen, this will always work. The only thing needed
; is that for every nesting-level a variable has to be defined
; which counts the current label number for the specific
; depth. This will be the array.
;
; How to make an array in MPASM. The "#v()" operator will 
; convert a number inside the brackets to text. So the 
; statement "lab#v(a)" in a macro, while "a" has the value 4,
; will end up as "lab4" before the final compilation starts.
; Now one step deeper, lab4 itself could be a variable, if
; used in the right contect. The following code shows an array:
;
;   arr0 = .18
;   arr1 = .26
;   arr2 = .87
;   i = 0
;   while i<3
;   lab#v(arr#v(i)):
;   i+=1
;   endw
;
; This will produce the following text before the final
; compilation:
;   lab18:
;   lab26:
;   lab87:
;
; This, just to indicate how to make arrays in MPASM.
;
; Since we now know how to make a for-next loop and use an array,
; let's look at the actual macro-code. The for_f_l_l macro is:
; 
;  1:  for_f_l_l   macro           var1,lit1,lit2
;  2:  cdep++
;  3:              if cdep>mdep
;  4:  mdep++
;  5:  labcnt#v(cdep) = 0
;  6:              endif
;  7:  labcnt#v(cdep)++
;  8:              load_f_l        var1,lit1	
;  9:  lab_#v(cdep)_#v(labcnt#v(cdep)):
; 10:  labcnt#v(cdep)++
; 11:              gotoif_f_gt_l   var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep))
; 12:              endm
;
; cdep (current depth) contains the current nesting-level. mdep
; (maximum reached depth) is just used to initialise (put zero) if
; the array element if a nesting-level has not been reached before.
; The array itself is labcntx, in which x is the arrayindexnumber. 
;
; Imagin that this is the first loop in the code. cdep and mdep equal
; zero. Stepping through the code: cdep is increased, the
; nesting-level is now 1. cdep is larger than mdep, meaning this is
; the first time this nesting level is reached. So this array-element
; has to initialised by setting it to zero. Line 5 will be
; translated to "labcnt1 = 0". labcnt1 is increased in line 7
; everytime a for-loop is entered, in order to produce a unique
; label. The label in line 9 is translated lab_1_1 (as in the example).
; labcnt1 is increased in line 10, since a for-loop needs two labels
; (an if-block only need one label). In line 11, the label
; lab_1_2 is generated (as in the example).
;
; Now the next-code:
; 13:  next_f   macro           var1
; 14:           incf            var1,f
; 15:           goto            lab_#v(cdep)_#v(labcnt#v(cdep)-1)
; 16:  lab_#v(cdep)_#v(labcnt#v(cdep)):
; 17:  cdep--	
; 18:           endm
; Line 15 generates the label lab_1_1, since cdep equals 2 and is
; decresed by 1. This label jumps back to the 'for'-code. Line 16
; generates the label lab_1_2, which was needed in the 'for' code
; when the loop was completed. Line 17 decreases cdep, since 
; nesting-level 1 is left and the program now enters nesting-
; level 0.
;
;**************************************************************
;***                          SYNTAX                        ***
;**************************************************************
;
; *** SYNTAX (load-statements):
;
;   load_x_y x,y
;     Loads x with y
;
;   x: f, f8, f16, f32  - file (variable) (alias: f8)
;      i, i8            - indirect file  (alias: if)
;    
;   y: l, l8, l16, l32  - literal
;      f, f8, f16, f32  - file (variable) (alias: f8)
;      i, i8            - indirect file   (alias: if)
;
;   load_f_b_l file, bit, literal
;      Loads bit1 of file1 with literal (lowest bit)
;   load_f_b_f_b file1, bit1, file2, bit2
;      Loads bit1 of file1 with bit2 of file2
;
;
; *** SYNTAX (if-statements):
;
;   gotoif_x_ex_y file, $2, label
;     $2: file, literal
;     jumps to label if the expression is met
;
;   if_x_ex_y file, $2
;     statements
;   {else
;     statements}
;   endif 
;     $2: file, literal
;     Executes blocks depending on if expression is met
;
;   ex:eq - equal  
;      ne - not equal
;      ge - greater equal
;      gt - greater than
;      lt - less than
;      le - less equal 
;
;   x: l, l8, l16, l32 - literal
;      f, f8, f16, f32 - file (variable)
;        
;   y: f, f8, f16, f32 - file (variable)
;        
;
;   gotoif_f_b_s file, literal, label 
;   gotoif_f_b_c file, literal, label 
;     jumps to label if bit (literal) of file is set (s) or clear (c)
;
;   if_f_b_s file, literal, label 
;     statements
;   {else_
;     statements} 
;   end_if
;   if_f_b_c file, literal, label 
;     statements
;   {else_
;     statements} 
;   end_if
;     Executes blocks depending on if bit (literal) of file is set (s) or clear (c)
;
;
; *** SYNTAX (for-statements):
;
;   for_f_x_y register, start,  end
;     statements
;   next register
;     x,y: l, l8, l16, l32, f, f8, f16, f32 
;     Executes for-next loop, in which register is increased from start to (and including) end
;
;   ford_f_x_y register, start,  end 
;     counts down
;
; *** SYNTAX (repeat and while-statements):
;
;   repeat
;     statements
;   until_x_ex_y
;   until_f_b_s
;   until_f_b_c 
;     Executes blocks until expression is met
;
;   
;   while_x_ex_y
;   while_f_b_s
;   while_f_b_c 
;     statements
;   end_while
;     Executes blocks if expression is met
;
;   ex:eq - equal  
;      ne - not equal
;      ge - greater equal
;      gt - greater than
;      lt - less than
;      le - less equal 
;
;   x: f, f8, f16, f32 - file (variable)
;
;   y: l, l8, l16, l32 - literal
;      f, f8, f16, f32 - file (variable)
;        
;
; *** SYNTAX (Math):
;
;
;  add_x_y_z    x,y,z       ; z = x + y
;  sub_x_y_z    x,y,z       ; z = x - y
;  mul_x_y_z    x,y,z       ; z = x * y
;  div_x_y_z    x,y,z       ; z = x / y
;  rem_x_y_z    x,y,z       ; z = x % y (the remainder of the division)
;  divr_x_y_z_z x,y,z1,z2   ; z1 = x / y, z2 = x % y
;
;   x: l, l8, l16, l32 - literal
;      f, f8, f16, f32 - file (variable)
;   y: l, l8, l16, l32 - literal (except l_l etc.)
;      f, f8, f16, f32 - file (variable)
;   z: f, f8, f16, f32 - file (variable)
; 


;**************************************************************
;***                        DEFINITIONS                     ***
;**************************************************************

#define 	_PPMVERSION	0x120	; version 1.20

#define 	_BANKSIZE	0x80
#define 	_PAGESIZE	0x0800

;**************************************************************
;***               Initialising block variables             ***
;**************************************************************

cdep = 0
mdep = 0

jcnt = 0
kcnt = 0
mcnt = 0

; page_option
;  0: no page switch (use for small programs)
;  1: always use pageselect
;  2: 
	ifndef	page_option
page_option=1
	endif
pgcnt = 0


; bank_option
;  0: bank switch only if out first bank; switch back to bank 0 afterwards (use if mainly variables in bank 0)
;  1: switch always to bank (and leave it)
;  2: switch only if bankblock is differnt; use banklabel at every label!
	ifndef	bank_option
bank_option=1
	endif
bankblock = -1

;**************************************************************
;***                      Internal macros                   ***
;**************************************************************

bankblockstart	macro
;	#define	PPM_BANKBLOCK
;	ifndef	PPM_BANKLARGE
;	 error "PPM_BANKLARGE should be defined for bankblock"
;	endif
;bankblock=-1
	endm

bankblockend	macro
;	#undefine	PPM_BANKBLOCK
;bankblock=-1
	endm

banklabel	macro
bankblock=-1
	endm


bankset	macro	f
	;messg	"bank_option= "  #v(bank_option)
	if	bank_option==0
	 if (f)>=_BANKSIZE
	  banksel	f
	 endif
	endif
	if	bank_option==1
	 if ((f)<0x70) || ((f)>=0x80)
	  banksel	f
	 endif
	endif
	if	bank_option==2
	 if ((f)/_BANKSIZE)!=bankblock && (((f)<0x70) || ((f)>=0x80))
bankblock=((f)/_BANKSIZE)
	  banksel	f
	 endif
	endif
	errorlevel 	-302	; supress bank bit warning
	endm

bankres	macro	f
	;messg	"bank_option= "  #v(bank_option)
	if	bank_option==0
	 if (f)>=_BANKSIZE
	  banksel	0
	 endif
	endif
	errorlevel 	+302
	endm

pageset	macro	f
	pagesel	f
	errorlevel 	-306	; supress page boundary warnin
	endm

pageres	macro
	if (page_option==1)
	 pagesel	$
	endif
	errorlevel 	+306
	endm

clrfb	macro	v
	bankset	v
	clrf	v
	bankres	v
	endm

addwfb	macro	v,d
	bankset	v
	addwf	v,d
	bankres	v
	endm

incfb	macro	v,d
	bankset	v
	incf	v,d
	bankres	v
	endm

subwf_b	macro	v,d
	bankset	v
	subwf	v,d
	bankres	v
	endm

decfb	macro	v,d
	bankset	v
	decf	v,d
	bankres	v
	endm

rlfb	macro	v,d
	bankset	v
	rlf	v,d
	bankres	v
	endm

rrfb	macro	v,d
	bankset	v
	rrf	v,d
	bankres	v
	endm

andwfb	macro	v,d
	bankset	v
	andwf	v,d
	bankres	v
	endm

iorwfb	macro	v,d
	bankset	v
	iorwf	v,d
	bankres	v
	endm

xorwfb	macro	v,d
	bankset	v
	xorwf	v,d
	bankres	v
	endm

swapfb	macro	v,d
	bankset	v
	swapf	v,d
	bankres	v
	endm

lslfb	macro	v,d
	bankset	v
	lslf	v,d
	bankres	v
	endm

lsrfb	macro	v,d
	bankset	v
	lsrf	v,d
	bankres	v
	endm

;**************************************************************
;***                        PAGE CODES                      ***
;**************************************************************

callc	macro	lab
	errorlevel 	+306
	call	lab
	endm


;
; callp lab
;
; Call across page boundary
callp	macro	lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 callc	lab
	else
	 pageset	lab
	 call	lab
	 pageres
	 banklabel
	endif
pgcnt++
	endm

callpp	macro	lab
	pageset	lab
	call	lab
	 pageres
	 banklabel
	endm

callwp	macro
;	if ($ / _PAGESIZE == lab / _PAGESIZE)
;	  call	lab
;	else
	  pagesel	$
	  errorlevel 	-306	; supress page boundary warning
	  callw
	  errorlevel 	+306
	  pagesel	$
;	endif
	 banklabel
	endm


gotoc	macro	lab
	errorlevel 	+306
	goto	lab
	endm
;
; gotop lab
;
; Jump across page boundary
gotop	macro	lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 gotoc	lab
	else
	 pageset	lab
	 goto	lab
	 pageres
	endif
pgcnt++
	endm

gotopp	macro	lab
	pageset	lab
	goto	lab
	pageres
	endm

;
; gotopsz lab
;
; Jump across page boundary; use after an skip-z instruction
; For new code, the if-macros are preferred
;gotopsz	macro	lab
;	if ($ / _PAGESIZE == lab / _PAGESIZE)
;	  goto	lab
;	else
;jcnt++
;	  goto	j_0_#v(jcnt)
;	  goto	j_0_#v(jcnt+1)
;j_0_#v(jcnt):
;	  pageset	lab
;	  goto	lab
;	  pageres
;j_0_#v(jcnt+1):
;jcnt++
;	endif
;	endm


;**************************************************************
;***                8-BITS BASIC CODES                      ***
;**************************************************************
; The following macros are defined in this section:
;   load_w_l
;   load_w_f
;   load_w_i
;   load_f_w
;   load_i_w
;   loadl_w_i
;   loadl_i_w

;
; load_w_l lit
;   Loads W with literal (lit)
;   Destroys W
load_w_l	macro	l
	movlw	l
	endm
move_l_w	macro	l
	load_w_l	l
	endm

;
; load_w_f var
;   Loads W with register (var)
;   Destroys W
load_w_f	macro	v
    bankset	v
	movf	v,W
	bankres	v
	endm
move_f_w	macro	v
	load_w_f	v
	endm

;
; load_w_i var
;   Loads W with indirect register (var)
;   Destroys W
load_w_i	macro	v
	load_f_w	v
#ifdef FSR0L
	clrf	FSR0H
	movwf	FSR0L
	movf	INDF0,W
#else
	movwf	FSR
	movf	INDF,W
#endif
	endm
move_i_w	macro	v
	load_w_i	v
	endm


;
; load_f_w var
;   Loads register (var) with W
;   Destroys W
load_f_w	macro	v
	bankset	v
	movwf	v
	bankres	v
	endm
move_w_f	macro	v
	load_f_w	v
	endm

;
; load_i_w var
;   Loads indirect register (var) with W
;   Destroys W; Needs _PPMCR1_
load_i_w	macro	v
	load_f_w	_PPMCR1_
#ifdef FSR0L
	movlw	v
	movwf	FSR0L
	clrf	FSR0H
	movf	_PPMCR1_,W
	movwf	INDF0
#else
	movlw	v
	movwf	FSR
	movf	_PPMCR1_,W
	movwf	INDF
#endif
	endm
move_w_i	macro	v
	load_i_w	v
	endm

;
; loadl_w_i var
;   Loads W with indirect register (var) in linear mode
;   Destroys W
loadl_w_i	macro	v
	movwf	FSR0H
	movlw	v
	load_f_w	v
	movwf	FSR0L
	movf	INDF0,W
	endm
movel_i_w	macro	v
	loadl_w_i	v
	endm


;
; loadl_i_w var
;   Loads indirect register (var) with W in linear mode
;   Destroys W; Needs _PPMCR1_
loadl_i_w	macro	v
	load_f_w	_PPMCR1_
	movlw	0x20
	movwf	FSR0H
	movlw	v
	movwf	FSR0L
	movf	_PPMCR1_,W
	movwf	INDF0
	endm
movel_w_i	macro	v
	loadl_i_w	v
	endm


;**************************************************************
;***              8-BITS LOAD / MOVE CODES                  ***
;**************************************************************
; The following macros are defined in this section:
;   load_f_l
;   load_f_f
;   load_f_i
;   load_f_x_z
;   load_i_l
;   load_x_z_l
;   load_i_f
;   load_x_z_f
;   load_i_i
;   loadl_f_i
;   loadl_i_f
;   loadl_f_i16
;   loadl_i16_f

; f = f8
; i = i8


;
; load_f_l var,lit
;   Loads register (var) with literal (lit).
;   Destroys W
load_f_l	macro	v,l
	movlw	l
	load_f_w	v
	endm
load_f8_l8	macro	v,l
	load_f_l	v,l
	endm
move_l_f	macro	l,v
	load_f_l	v,l
	endm
move_l8_f8	macro	l,v
	load_f_l	v,l
	endm

;
; load_f_f var1,var2
;   Loads register (var1) with register (var2).
;   Destroys W
load_f_f	macro	v1,v2
	load_w_f	v2
	load_f_w	v1
	endm
load_f8_f8	macro	v1,v2
	load_f_f	v1,v2
	endm
move_f_f	macro	v2,v1
	load_f_f	v1,v2
	endm
move_f8_f8	macro	v2,v1
	load_f_f	v1,v2
	endm


;
; load_f_i var1, var2
;   Loads register (var1) with indirect register (var2).
;   Destroys W
load_f_i	macro	f,i
#ifdef FSR0L
	load_f_f	FSR0L,i
	clrf	FSR0H
	load_f_f	f,INDF0
#else
	load_f_f	FSR,i
	load_f_f	f,INDF
#endif
	endm
load_f8_i8	macro	f,i
	load_f_i	f,i
	endm
move_i_f	macro	i,f
	load_f_i	f,i
	endm
move_i8_f8	macro	i,f
	load_f_i	f,i
	endm

;
; load_f_x_z var1, var2, off
;   Loads register (var1) with indirect register (var2, index) plus offset (off, zero-element).
; Do NOT mix 8-bits load-macros with 16 and 32 bits to obtain lower bytes (see also the f16 and f32 macros).
;   Destroys W
load_f_x_z	macro	f,i,o
	if o>=0x100
	  messg "load_f_x_z: offset outside bank 0/1."
	endif
	load_f_f	FSR,i
	movlw	o
	addwf	FSR,F
	load_f_f	f,INDF
	endm
load_f8_x8_z8	macro	f,i,o
	load_f_x_z	f,i,o
	endm
move_x_z_f	macro	i,o,f
	load_f_x_z	f,i,o
	endm
move_x8_z8_f8	macro	i,o,f
	load_f_x_z	f,i,o
	endm

;
; load_i_l var,lit
;   Loads indirect register (var) with literal (lit).
;   Destroys W
load_i_l	macro	i,l
#ifdef FSR0L
	load_f_f	FSR0L,i
	clrf	FSR0H
	load_f_l	INDF0,l
#else
	load_f_f	FSR,i
	load_f_l	INDF,l
#endif
	endm
load_i8_l8	macro	i,l
	load_i_l	i,l
	endm
move_l_i	macro	l,l
	load_i_l	i,l
	endm
move_l8_i8	macro	l,l
	load_i_l	i,l
	endm

;
; loadl_f_i var1, var2
;   Loads register (var1) with indirect register (var2) in linear mode.
;   Destroys W
loadl_f_i	macro	f,i
	load_f_l	FSR0H,0x20
	load_f_f	FSR0L,i
	load_f_f	f,INDF0
	endm
loadl_f8_i8	macro	f,i
	loadl_f_i	f,i
	endm
movel_i_f	macro	i,f
	loadl_f_i	f,i
	endm
movel_i8_f8	macro	i,f
	loadl_f_i	f,i
	endm

;
; loadl_f_i16 var1, var2
;   Loads register (var1) with indirect 16-bit register (var2) in linear mode.
;   Destroys W
loadl_f_i16	macro	f,i
	load_f_f	FSR0H,i+1
	load_w_l	0x3F
	andwf	FSR0H, F
	load_w_l	0x20
	iorwf	FSR0H, F
	load_f_f	FSR0L,i
	load_f_f	f,INDF0
	endm
loadl_f8_i16	macro	f,i
	loadl_f_i16	f,i
	endm
movel_i16_f	macro	i,f
	loadl_f_i16	f,i
	endm
movel_i16_f8	macro	i,f
	loadl_f_i16	f,i
	endm

;
; loadl_i_l var,lit
;   Loads indirect register (var) with literal (lit) in linear mode.
;   Destroys W
loadl_i_l	macro	i,l
	load_f_l	FSR0H,0x20
	load_f_f	FSR0L,i
	load_f_l	INDF0,l
	endm
loadl_i8_l8	macro	i,l
	loadl_i_l	i,l
	endm
movel_l_i	macro	l,l
	loadl_i_l	i,l
	endm
movel_l8_i8	macro	l,l
	loadl_i_l	i,l
	endm


;
; loadl_i16_l var,lit
;   Loads indirect 16-bit register (var) with literal (lit) in linear mode.
;   Destroys W
loadl_i16_l	macro	i,l
	load_f_f	FSR0H,i+1
	load_w_l	0x3F
	andwf	FSR0H, F
	load_w_l	0x20
	iorwf	FSR0H, F
	load_f_f	FSR0L,i
	load_f_l	INDF0,l
	endm
loadl_i16_l8	macro	i,l
	loadl_i16_l	i,l
	endm
movel_l_i16	macro	l,l
	loadl_i16_l	i,l
	endm
movel_l8_i16	macro	l,l
	loadl_i16_l	i,l
	endm


;
; load_x_z_l var,off,lit
;   Loads indirect register (var, index) plus offset (off, zero-element) with literal (lit).
; Do NOT mix 8-bits load-macros with 16 and 32 bits to obtain lower bytes (see also the f16 and f32 macros).
;   Destroys W
load_x_z_l	macro	i,o,l
	if o>=0x100
	  messg "load_x_z_l: offset outside bank 0/1."
	endif
	load_f_f	FSR,i
	movlw	o
	addwf	FSR,F
	load_f_l	INDF,l
	endm
load_x8_z8_l8	macro	i,o,l
	load_x_z_l	i,o,l
	endm
move_l_x_z	macro	l,i,o
	load_x_z_l	i,o,l
	endm
move_l8_x8_z8	macro	l,i,o
	load_x_z_l	i,o,l
	endm

;
; load_i_f var1,var2
;   Loads indirect register (var1) with register (var2).
;   Destroys W
load_i_f	macro	i,f
#ifdef FSR0L
	load_f_f	FSR0L,i
	clrf	FSR0H
	load_f_f	INDF0,f
#else
	load_f_f	FSR,i
	load_f_f	INDF,f
#endif
	endm
load_i8_f8	macro	i,f
	load_i_f	i,f
	endm
move_f_i	macro	f,i
	load_i_f	i,f
	endm
move_f8_i8	macro	f,i
	load_i_f	i,f
	endm

;
; loadl_i_f var1,var2
;   Loads indirect register (var1) with register (var2) in linear mode.
;   Destroys W
loadl_i_f	macro	i,f
	load_f_l	FSR0H,0x20
	load_f_f	FSR0L,i
	load_f_f	INDF0,f
	endm
loadl_i8_f8	macro	i,f
	loadl_i_f	i,f
	endm
movel_f_i	macro	f,i
	loadl_i_f	i,f
	endm
movel_f8_i8	macro	f,i
	loadl_i_f	i,f
	endm

;
; loadl_i16_f var1,var2
;   Loads indirect 16 bits register (var1) with register (var2) in linear mode.
;   Destroys W
loadl_i16_f	macro	i,f
	load_f_f	FSR0H,i+1
	load_w_l	0x3F
	andwf	FSR0H, F
	load_w_l	0x20
	iorwf	FSR0H, F
	load_f_f	FSR0L,i
	load_f_f	INDF0,f
	endm
loadl_i16_f8	macro	i,f
	loadl_i16_f	i,f
	endm
movel_f_i16	macro	f,i
	loadl_i16_f	i,f
	endm
movel_f8_i16	macro	f,i
	loadl_i16_f	i,f
	endm

;
; load_x_z_f var1, off, var2
;   Loads indirect register (var1, index) plus offset (off, zero-element) with register (var2).
; Do NOT mix 8-bits load-macros with 16 and 32 bits to obtain lower bytes (see also the f16 and f32 macros).
;   Destroys W
load_x_z_f	macro	i,o,f
	if o>=0x100
	  messg "load_x_z_f: offset outside bank 0/1."
	endif
	load_f_f	FSR,i
	movlw	o
	addwf	FSR,F
	load_f_f	INDF,f
	endm
load_x8_z8_f8	macro	i,o,f
	load_x_z_f	i,o,f
	endm
move_f_x_z	macro	f,i,o
	load_x_z_f	i,o,f
	endm
move_f8_x8_z8	macro	f,i,o
	load_x_z_f	i,o,f
	endm

;
; load_i_i var1,var2
;   Loads indirect register (var1) with indirect register (var2).
;   Destroys W; Needs _PPMCR1_
load_i_i	macro	i,i2
#ifdef FSR0L
	clrf	FSR0H
	load_f_f	FSR0L,i2
	load_f_f	_PPMCR1_,INDF0
	clrf	FSR0H
	load_f_f	FSR0L,i
	load_f_f	INDF0,_PPMCR1_
#else
	load_f_f	FSR,i2
	load_f_f	_PPMCR1_,INDF
	load_f_f	FSR,i
	load_f_f	INDF,_PPMCR1_
#endif
	endm
load_i8_i8	macro	i1,i2
	load_i_i	i1,i2
	endm
move_i_i	macro	i2,i1
	load_i_i	i1,i2
	endm
move_i8_i8	macro	i2,i1
	load_i_i	i1,i2
	endm

;**************************************************************
;***                  BIT SET CODES                         ***
;**************************************************************
; The following macros are defined in this section:
;   load_f_b_l

;
; load_f_b_l v1,b1,l
;   set bit b of file v with value l (lowest bit)
load_f_b_l	macro	f1,b1,l
	bankset	f1
	if (l & 1)==1
	  bsf	f1,b1
	else
	  bcf	f1,b1
	endif
	bankres	f1
	endm


;**************************************************************
;***                  BIT COPY CODES                        ***
;**************************************************************
; The following macros are defined in this section:
;   load_f_b_f_b
;   load_f_b_i_b
;   load_i_b_f_b
;   load_i_b_i_b

;
; load_f_b_f_b v1,b1,v2,b2
;   This is a bitcopy macro
load_f_b_f_b	macro	f1,b1,f2,b2
	if (f1<_BANKSIZE) && (f2<_BANKSIZE)
	  btfss	f2,b2
	  bcf	f1,b1
 	  btfsc	f2,b2
	  bsf	f1,b1
	else
	  errorlevel 	-302	; supress bank bit warning
jcnt++
	  banksel	f2
	  btfsc	f2,b2
	  goto	j_0_#v(jcnt)
	  banksel	f1
	  bcf	f1,b1
	  goto	j_0_#v(jcnt+1)
j_0_#v(jcnt):	  
	banklabel
	  banksel	f1
	  bsf	f1,b1
j_0_#v(jcnt+1):	  
	banklabel
	  banksel	0
jcnt++
	  errorlevel 	+302
	endif
	endm
load_f8_b_f8_b	macro	f1,b1,f2,b2
	load_f_b_f_b	f1,b1,f2,b2
	endm
move_f_b_f_b	macro	f2,b2,f1,b1
	load_f_b_f_b	f1,b1,f2,b2
	endm
move_f8_b_f8_b	macro	f2,b2,f1,b1
	load_f_b_f_b	f1,b1,f2,b2
	endm


;
; load_f_b_i_b v1,b1,v2,b2
;   This is a bitcopy macro
;   Destroys W
load_f_b_i_b	macro	f1,b1,f2,b2
	load_f_f	FSR,f2
	load_f_b_f_b	f1,b1,INDF,b2
	endm
load_f8_b_i8_b	macro	f1,b1,f2,b2
	load_f_b_i_b	f1,b1,f2,b2
	endm
move_i_b_f_b	macro	f2,b2,f1,b1
	load_f_b_i_b	f1,b1,f2,b2
	endm
move_i8_b_f8_b	macro	f2,b2,f1,b1
	load_f_b_i_b	f1,b1,f2,b2
	endm

;
; load_i_b_f_b v1,b1,v2,b2
;   This is a bitcopy macro
;   Destroys W
load_i_b_f_b	macro	f1,b1,f2,b2
	load_f_f	FSR,f1
	load_f_b_f_b	INDF,b1,f2,b2
	endm
load_i8_b_f8_b	macro	f1,b1,f2,b2
	load_i_b_f_b	f1,b1,f2,b2
	endm
move_f_b_i_b	macro	f2,b2,f1,b1
	load_i_b_f_b	f1,b1,f2,b2
	endm
move_f8_b_i8_b	macro	f2,b2,f1,b1
	load_i_b_f_b	f1,b1,f2,b2
	endm

;
; load_i_b_i_b v1,b1,v2,b2
;   This is a bitcopy macro
;   Destroys W. Needs _PPMCR1_
load_i_b_i_b	macro	f1,b1,f2,b2
	load_f_i	_PPMCR1_,f2
	load_f_f	FSR,f1
	load_f_b_f_b	INDF,b1,_PPMCR1_,b2
	endm
load_i8_b_i8_b	macro	f1,b1,f2,b2
	load_i_b_i_b	f1,b1,f2,b2
	endm
move_i_b_i_b	macro	f2,b2,f1,b1
	load_i_b_i_b	f1,b1,f2,b2
	endm
move_i8_b_i8_b	macro	f2,b2,f1,b1
	load_i_b_i_b	f1,b1,f2,b2
	endm


;**************************************************************
;***          16 and 32-BITS LOAD / MOVE CODES              ***
;**************************************************************
; The following macros are defined in this section:
;
;   load_f16_l16
;   load_f16_f16
;   load_f16_i8
;   load_f16_x8_z8
;   load_i8_l16
;   load_x8_z8_l16
;   load_i8_f16
;   load_x8_z8_f16
;
;   load_f32_l32
;   load_f32_f32
;   load_f32_i8
;   load_f32_x8_z8
;   load_i8_l32
;   load_x8_z8_l32
;   load_i8_f32
;   load_x8_z8_f32
;
; Conversion macros between f8, f16 and f32
;   load_f8_f16
;   load_f8_f32
;   load_f16_f8
;   load_f16_f32
;   load_f32_f8
;   load_f32_f16


;
; load_f16_l16 var,lit
;
; Loads 16 bits variable (var1) with literal (lit).
; destroys W; 
load_f16_l16	macro	f,l
	load_f_l	f,(l % 0x100)
	load_f_l	f+1,(l >> .8)
	endm
move_f16_l16	macro	l,f
	load_f16_l16	f,l
	endm

;
; load_f16_f16 var1,var2
;
; Loads 16 bits variable (var1) with 16 bits variable (var2).
; destroys W; 
load_f16_f16	macro	f1,f2
	load_f_f	f1,f2
	load_f_f	f1+1,f2+1
	endm
move_f16_f16	macro	f2,f1
	load_f16_f16	f1,f2
	endm

;
; load_f16_i8 var1,var2
;
; Loads 16 bits variable (var1) with indirect 16 bits variable (given by the 8 bit variable var2).
; destroys W; ; Needs _PPMCR1_
load_f16_i8	macro	f1,f2
	load_f_f	FSR,f2
	load_f_f	f1,INDF
	incf	FSR,F
	load_f_f	f1+1,INDF
	endm
move_i8_f16	macro	f2,f1
	load_f16_i8	f1,f2
	endm

;
; load_f16_x8_z8 var1,var2,off
;
; Loads 16 bits variable (var1) with indirect 16 bits variable (given by the 8 bit 
;   variable var2, index) plus offset (off, zero-element).
; Since x8 is multplied with 2 to get the address, one can NOT use the 
;   load_f8_x8_z8 to only get the lower byte.
; destroys W
load_f16_x8_z8	macro	f1,f2,o
	if o>=0x100
	  messg "load_f16_x8_z8: offset outside bank 0/1."
	endif
	load_f_f	FSR,f2
	bcf	STATUS,C
	rlf	FSR,F
	load_w_l	o
	addwf	FSR,F
	load_f_f	f1,INDF
	incf	FSR,F
	load_f_f	f1+1,INDF
	endm
move_x8_z8_f16	macro	f2,o,f1
	load_f16_x8_z8	f1,f2,o
	endm


; load_i8_l16 var,lit
;
; Loads indirect 16 bits variable (given by the 8 bit variable var) with literal (lit).
; destroys W
load_i8_l16	macro	f,l
	load_f_f	FSR,f
	load_f_l	INDF,(l % 0x100)
	incf	FSR,F
	load_f_l	INDF,(l >> .8)
	endm
move_l16_i8	macro	l,f
	load_i8_l16	f,l
	endm

; load_x8_z8_l16 var1,lit
;
; Loads indirect 16 bits variable (given by the 8 bit variable var, index) plus 
;   offset (off, zero-element) with with literal (lit).
; Since x8 is multplied with 2 to get the address, one can NOT use the 
;   load_x8_z8_l8 to only write the lower byte.
; destroys W
load_x8_z8_l16	macro	f,o,l
	if o>=0x100
	  messg "load_x8_z8_l16: offset outside bank 0/1."
	endif
	load_f_f	FSR,f
	bcf	STATUS,C
	rlf	FSR,F
	load_w_l	o
	addwf	FSR,F
	load_f_l	INDF,(l % 0x100)
	incf	FSR,F
	load_f_l	INDF,(l >> .8)
	endm
move_l16_x8_z8	macro	l,f,o
	load_x8_z8_f16	f,o,l
	endm

; load_i8_f16 var1,var2
;
; Loads indirect 16 bits variable (given by the 8 bit variable var1) with 16 bits variable (var2).
; destroys W
load_i8_f16	macro	f1,f2
	load_f_f	FSR,f1
	load_f_f	INDF,f2
	incf	FSR,F
	load_f_f	INDF,f2+1
	endm
move_f16_i8	macro	f2,f1
	load_i8_f16	f1,f2
	endm

;
; load_x8_z8_f16 var1,var2
;
; Loads indirect 16 bits variable (given by the 8 bit variable var1, index) plus 
;   offset (off, zero-element) with 16 bits variable (var2).
; Since x8 is multplied with 2 to get the address, one can NOT use the 
;   load_x8_z8_f8 to only write the lower byte.
; destroys W
load_x8_z8_f16	macro	f1,o,f2
	if o>=0x100
	  messg "load_x8_z8_f16: offset outside bank 0/1."
	endif
	load_f_f	FSR,f1
	bcf	STATUS,C
	rlf	FSR,F
	load_w_l	o
	addwf	FSR,F
	load_f_f	INDF,f2
	incf	FSR,F
	load_f_f	INDF,f2+1
	endm
move_f16_x8_z8	macro	f2,f1,o
	load_x8_z8_f16	f1,o,f2
	endm

;
; load_f32_l32 var,lit
;
; Loads 32 bits variable (var1) with literal (lit).
; destroys W; 
load_f32_l32	macro	f,l
	load_f_l	f,(l % 0x100)
	load_f_l	f+1,(l >> .8) % 0x100
	load_f_l	f+2,(l >> .16) % 0x100
	load_f_l	f+3,(l >> .24)
	endm
move_l32_f32	macro	l,f
	load_f32_l32	f,l
	endm

;
; load_f32_f32 var1,var2
;
; Loads 32 bits variable (var1) with 32 bits variable (var2).
; destroys W; 
load_f32_f32	macro	f1,f2
	load_f_f	f1,f2
	load_f_f	f1+1,f2+1
	load_f_f	f1+2,f2+2
	load_f_f	f1+3,f2+3
	endm
move_f32_f32	macro	f2,f1
	load_f32_f32	f1,f2
	endm

;
; load_f32_i8 var1,var2
;
; Loads 32 bits variable (var1) with indirect 32 bits variable (given by the 8 bit variable var2).
; destroys W
load_f32_i8	macro	f1,f2
	load_f_f	FSR,f2
	load_f_f	f1,INDF
	incf	FSR,F
	load_f_f	f1+1,INDF
	incf	FSR,F
	load_f_f	f1+2,INDF
	incf	FSR,F
	load_f_f	f1+3,INDF
	endm
move_i8_f32	macro	f2,f1
	load_f32_i8	f1,f2
	endm

;
; load_f32_x8_z8 var1,var2,off
;
; Loads 32 bits variable (var1) with indirect 32 bits variable 
;   (given by the 8 bit variable var2, index) plus offset (off, zero-element).
; Since x8 is multplied with 4 to get the address, one can NOT use the 
;   load_f8/f16_x8_z8 to only get the lower byte(s).
; destroys W
load_f32_x8_z8	macro	f1,f2,o
	if o>=0x100
	  messg "load_f32_x8_z8: offset outside bank 0/1."
	endif
	load_f_f	FSR,f2
	bcf	STATUS,C
	rlf	FSR,F
	bcf	STATUS,C
	rlf	FSR,F
	load_w_l	o
	addwf	FSR,F
	load_f_f	f1,INDF
	incf	FSR,F
	load_f_f	f1+1,INDF
	incf	FSR,F
	load_f_f	f1+2,INDF
	incf	FSR,F
	load_f_f	f1+3,INDF
	endm
move_x8_z8_f32	macro	f2,o,f1
	load_f32_x8_z8	f1,f2,o
	endm

; load_i8_l32 var,lit
;
; Loads indirect 32 bits variable (given by the 8 bit variable var) with literal (lit).
; destroys W
load_i8_l32	macro	f,l
	load_f_f	FSR,f
	load_f_l	INDF,(l % 0x100)
	incf	FSR,F
	load_f_l	INDF,(l >> .8) % 0x100
	incf	FSR,F
	load_f_l	INDF,(l >> .16) % 0x100
	incf	FSR,F
	load_f_l	INDF,(l >> .24)
	endm
move_l32_i8	macro	l,f
	load_i8_l32	f,l
	endm

;
; load_x8_z8_l32 var1,lit
;
; Loads indirect 32 bits variable (given by the 8 bit variable var1, index) plus 
;   offset (off, zero-element) with with literal (lit).
; Since x8 is multplied with 4 to get the address, one can NOT use the 
;   load_x8_z8_l8/l16 to only write the lower byte(s).
; destroys W
load_x8_z8_l32	macro	f,o,l
	if o>=0x100
	  messg "load_x8_z8_l32: offset outside bank 0/1."
	endif
	load_f_f	FSR,f
	bcf	STATUS,C
	rlf	FSR,F
	bcf	STATUS,C
	rlf	FSR,F
	load_w_l	o
	addwf	FSR,F
	load_f_l	INDF,(l % 0x100)
	incf	FSR,F
	load_f_l	INDF,(l >> .8) % 0x100
	incf	FSR,F
	load_f_l	INDF,(l >> .16) % 0x100
	incf	FSR,F
	load_f_l	INDF,(l >> .24)
	endm
move_l32_x8_z8	macro	l,f,o
	load_x8_z8_f32	f,o,l
	endm


;
; load_i8_f32 var1,var2
;
; Loads indirect 32 bits variable (given by the 8 bit variable var1) with 32 bits variable (var2).
; destroys W
load_i8_f32	macro	f1,f2
	load_f_f	FSR,f1
	load_f_f	INDF,f2
	incf	FSR,F
	load_f_f	INDF,f2+1
	incf	FSR,F
	load_f_f	INDF,f2+2
	incf	FSR,F
	load_f_f	INDF,f2+3
	endm
move_f32_i8	macro	f2,f1
	load_i8_f32	f1,f2
	endm

;
; load_x8_z8_f32 var1,var2
;
; Loads indirect 32 bits variable (given by the 8 bit variable var1, index) plus 
;   offset (off, zero-element) with 16 bits variable (var2).
; Since x8 is multplied with 4 to get the address, one can NOT use the 
;   load_x8_z8_f8/f16 to only write the lower byte(s).
; destroys W
load_x8_z8_f32	macro	f1,o,f2
	if o>=0x100
	  messg "load_x8_z8_f32: offset outside bank 0/1."
	endif
	load_f_f	FSR,f1
	bcf	STATUS,C
	rlf	FSR,F
	bcf	STATUS,C
	rlf	FSR,F
	load_w_l	o
	addwf	FSR,F
	load_f_f	INDF,f2
	incf	FSR,F
	load_f_f	INDF,f2+1
	incf	FSR,F
	load_f_f	INDF,f2+2
	incf	FSR,F
	load_f_f	INDF,f2+3
	endm
move_f32_x8_z8	macro	f2,f1,o
	load_x8_z8_f32	f1,o,f2
	endm

;
; load_f8_f16 var1,var2
;
; Loads 8 bits variable (var1) with 16 bits variable (var2) and truncates.
; destroys W; 
load_f8_f16	macro	f1,f2
	load_f_f	f1,f2
	endm
move_f16_f8	macro	f2,f1
	load_f8_f16	f1,f2
	endm

;
; load_f8_f32 var1,var2
;
; Loads 8 bits variable (var1) with 32 bits variable (var2) and truncates.
; destroys W; 
load_f8_f32	macro	f1,f2
	load_f_f	f1,f2
	endm
move_f32_f8	macro	f2,f1
	load_f8_f32	f1,f2
	endm

;
; load_f16_f8 var1,var2
;
; Loads 16 bits variable (var1) with 8 bits variable (var2), clear higher bytes.
; destroys W; 
load_f16_f8	macro	f1,f2
	load_f_f	f1,f2
	load_f_l	f1+1,0
	endm
move_f8_f16	macro	f2,f1
	load_f16_f8	f1,f2
	endm

;
; load_f16_f32 var1,var2
;
; Loads 16 bits variable (var1) with 32 bits variable (var2) and truncates.
; destroys W; 
load_f16_f32	macro	f1,f2
	load_f_f	f1,f2
	load_f_f	f1+1,f2+1
	endm
move_f32_f16	macro	f2,f1
	load_f16_f32	f1,f2
	endm

;
; load_f32_f8 var1,var2
;
; Loads 32 bits variable (var1) with 8 bits variable (var2), clear higher bytes.
; destroys W; 
load_f32_f8	macro	f1,f2
	load_f_f	f1,f2
	load_f_l	f1+1,0
	load_f_l	f1+2,0
	load_f_l	f1+3,0
	endm
move_f8_f32	macro	f2,f1
	load_f32_f8	f1,f2
	endm

;
; load_f32_f16 var1,var2
;
; Loads 32 bits variable (var1) with 16 bits variable (var2), clear higher bytes.
; destroys W; 
load_f32_f16	macro	f1,f2
	load_f_f	f1,f2
	load_f_f	f1+1,f2+1
	load_f_l	f1+2,0
	load_f_l	f1+3,0
	endm
move_f16_f32	macro	f2,f1
	load_f32_f16	f1,f2
	endm



;**************************************************************
;***                    8 BITS GOTOIF CODES                 ***
;**************************************************************
; The following macros are defined in this section:
;
; gotoif_f_eq_l
; gotoif_f_ne_l
; gotoif_f_gt_l
; gotoif_f_ge_l
; gotoif_f_lt_l
; gotoif_f_le_l
; gotoif_f_eq_f
; gotoif_f_ne_f
; gotoif_f_gt_f
; gotoif_f_ge_f
; gotoif_f_lt_f
; gotoif_f_le_f
; gotoif_f_b_c
; gotoif_f_b_s


;
; gotoif_f_eq_l var,lit,lab
;
; Jumps to label if register (var) is equal to literal (lit)
; destroys W
gotoif_f_eq_l	macro	v,l,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 btfsc	STATUS,Z
	 gotoc	lab
	else
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 pageset	lab
	 btfsc	STATUS,Z
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_eq_l8	macro	v,l,lab
	gotoif_f_eq_l	v,l,lab
	endm


;
; gotoif_f_ne_l var,lit,label
;
; Jumps to label if register (var) is not equal to literal (lit)
; destroys W
gotoif_f_ne_l	macro	v,l,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 btfss	STATUS,Z
	 gotoc	lab
	else
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 pageset	lab
	 btfss	STATUS,Z
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_ne_l8	macro	v,l,lab
	gotoif_f_ne_l	v,l,lab
	endm

;
; gotoif_f_gt_l var,lit,label
;
; Jumps to label if register (var) is larger (greater) than literal (lit)
; destroys W
gotoif_f_gt_l	macro	v,l,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
jcnt++
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 btfss	STATUS,C
	 gotoc	j_0_#v(jcnt)
	 btfss	STATUS,Z
	 gotoc	lab
j_0_#v(jcnt):	
	banklabel
	else
jcnt++
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 pageset	j_0_#v(jcnt)
	 btfss	STATUS,C
	 goto	j_0_#v(jcnt)
	 pageset	lab
	 btfss	STATUS,Z
	 goto	lab
	 pageres
j_0_#v(jcnt):	
	banklabel
	endif
pgcnt++
	endm
gotoif_f8_gt_l8	macro	v,l,lab
	gotoif_f_gt_l	v,l,lab
	endm

;
; gotoif_f_ge_l var,lit,label
;
; Jumps to label if register (var) is larger (greater) or equal to literal (lit)
; destroys W
gotoif_f_ge_l	macro	v,l,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 btfsc	STATUS,C
	 gotoc	lab
	else
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 pageset	lab
	 btfsc	STATUS,C
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_ge_l8	macro	v,l,lab
	gotoif_f_ge_l	v,l,lab
	endm

;
; gotoif_f_lt_l var,lit,label
;
; Jumps to label if register (var) is smaller (less) than literal (lit)
; destroys W
gotoif_f_lt_l	macro	v,l,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 btfss	STATUS,C
	 gotoc	lab
	else
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 pageset	lab
	 btfss	STATUS,C
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_lt_l8	macro	v,l,lab
	gotoif_f_lt_l	v,l,lab
	endm

;
; gotoif_f_le_l var,lit,label
;
; Jumps to label if register (var) is smaller (less) or equal to literal (lit)
; destroys W
gotoif_f_le_l	macro	v,l,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 btfss	STATUS,C
	 gotoc	lab
	 btfsc	STATUS,Z
	 goto	lab
	else
	 movlw	l
	 bankset	v
	 subwf	v,W
	 bankres	v
	 pageset	lab
	 btfss	STATUS,C
	 goto	lab
	 btfsc	STATUS,Z
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_le_l8	macro	v,l,lab
	gotoif_f_le_l	v,l,lab
	endm

;
; gotoif_f_eq_f var1,var2,label
;
; Jumps to label if register (var1) is equal to register (var2)
; destroys W
gotoif_f_eq_f	macro	v1,v2,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 btfsc	STATUS,Z
	 gotoc	lab
	else
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	lab
	 btfsc	STATUS,Z
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_eq_f8	macro	v1,v2,lab
	gotoif_f_eq_f	v1,v2,lab
	endm

;
; gotoif_f_ne_f var1,var2,label
;
; Jumps to label if register (var1) is not equal to register (var2)
; destroys W
gotoif_f_ne_f	macro	v1,v2,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 btfss	STATUS,Z
	 gotoc	lab
	else
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	lab
	 btfss	STATUS,Z
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_ne_f8	macro	v1,v2,lab
	gotoif_f_ne_f	v1,v2,lab
	endm

;
; gotoif_f_gt_f var1,var2,label
;
; Jumps to label if register (var1) is larger (greater) than register (var2)
; destroys W
gotoif_f_gt_f	macro	v1,v2,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
jcnt++
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	j_0_#v(jcnt)
	 btfss	STATUS,C
	 gotoc	j_0_#v(jcnt)
	 btfss	STATUS,Z
	 gotoc	lab
	 pageres
j_0_#v(jcnt):	
	banklabel
	else
jcnt++
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	j_0_#v(jcnt)
	 btfss	STATUS,C
	 goto	j_0_#v(jcnt)
	 pageset	lab
	 btfss	STATUS,Z
	 goto	lab
	 pageres
j_0_#v(jcnt):	
	banklabel
	endif
pgcnt++
	endm
gotoif_f8_gt_f8	macro	v1,v2,lab
	gotoif_f_gt_f	v1,v2,lab
	endm

;
; gotoif_f_ge_f var1,var2,label
;
; Jumps to label if register (var1) is larger (greater) or equal to register (var2)
; destroys W
gotoif_f_ge_f	macro	v1,v2,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	lab
	 btfsc	STATUS,C
	 gotoc	lab
	 pageres
	else
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	lab
	 btfsc	STATUS,C
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_ge_f8	macro	v1,v2,lab
	gotoif_f_ge_f	v1,v2,lab
	endm

;
; gotoif_f_lt_f var1,var2,label
;
; Jumps to label if register (var1) is smaller (less) than register (var2)
; destroys W
gotoif_f_lt_f	macro	v1,v2,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	lab
	 btfss	STATUS,C
	 gotoc	lab
	 pageres
	else
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	lab
	 btfss	STATUS,C
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_lt_f8	macro	v1,v2,lab
	gotoif_f_lt_f	v1,v2,lab
	endm

;
; gotoif_f_le_f var1,var2,label
;
; Jumps to label if register (var1) is smaller (less) or equal to register (var2)
; destroys W
gotoif_f_le_f	macro	v1,v2,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 btfss	STATUS,C
	 gotoc	lab
	 btfsc	STATUS,Z
	 gotoc	lab
	else
	 load_w_f	v2
	 bankset	v1
	 subwf	v1,W
	 bankres	v1
	 pageset	lab
	 btfss	STATUS,C
	 goto	lab
	 btfsc	STATUS,Z
	 goto	lab
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_le_f8	macro	v1,v2,lab
	gotoif_f_le_f	v1,v2,lab
	endm

;
; gotoif_f_b_s var,lit,label
;
; Jumps to label if bit (lit) of register (var) is set
;
gotoif_f_b_s	macro	v,l,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 bankset	v
	 btfsc	v,l
	 gotoc	lab
	 bankres	v
	else
jcnt++
	 bankset	v
	 pageset	j_0_#v(jcnt)
	 btfss	v,l
	 goto	j_0_#v(jcnt)
	 bankres	v
	 pageset	lab
	 goto	lab
j_0_#v(jcnt):
	banklabel
	 bankres	v
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_b_s	macro	v,l,lab
	gotoif_f_b_s	v,l,lab
	endm


;
; gotoif_f_b_c var,lit,label
;
; Jumps to label if bit (lit) of register (var) is clear
;
gotoif_f_b_c	macro	v,l,lab
	;if	(page_option==0) || ((page_option==2) && (PAGESELSW_#v(pgcnt) == 0))
	if	(page_option==0)
	 bankset	v
	 btfss	v,l
	 gotoc	lab
	 bankres	v
	else
jcnt++
	 bankset	v
	 pageset	j_0_#v(jcnt)
	 btfsc	v,l
	 goto	j_0_#v(jcnt)
	 bankres	v
	 pageset	lab
	 goto	lab
j_0_#v(jcnt):
	banklabel
	 bankres	v
	 pageres
	endif
pgcnt++
	endm
gotoif_f8_b_c	macro	v,l,lab
	gotoif_f_b_c	v,l,lab
	endm

;**************************************************************
;***               8 BITS GOTOIF CODES WITH MASK            ***
;**************************************************************
; The following macros are defined in this section:
;
; gotoif_f_msk_eq_l
; gotoif_f_msk_ne_l
; gotoif_f_msk_eq_f
; gotoif_f_msk_ne_f


;
; gotoif_f_msk_eq_l var,lit,lab
;
; Jumps to label if register (var) is equal to literal (lit)
; destroys W
gotoif_f_msk_eq_l	macro	v,m,l,lab
	movlw	l
	bankset	v
	subwf	v,W
	bankres	v
	pageset	lab
	andlw	m
	btfsc	STATUS,Z
	goto	lab
	pageres
	endm
gotoif_f8_msk_msk_eq_l8	macro	v,l,lab
	gotoif_f_msk_eq_l	v,l,lab
	endm


;
; gotoif_f_msk_ne_l var,lit,label
;
; Jumps to label if register (var) is not equal to literal (lit)
; destroys W
gotoif_f_msk_ne_l	macro	v,m,l,lab
	movlw	l
	bankset	v
	subwf	v,W
	bankres	v
	pageset	lab
	andlw	m
	btfss	STATUS,Z
	goto	lab
	pageres
	endm
gotoif_f8_msk_ne_l8	macro	v,l,lab
	gotoif_f_msk_ne_l	v,l,lab
	endm

;
; gotoif_f_msk_eq_f var1,var2,label
;
; Jumps to label if register (var1) is equal to register (var2)
; destroys W
gotoif_f_msk_eq_f	macro	v1,m,v2,lab
	load_w_f	v2
	bankset	v1
	subwf	v1,W
	bankres	v1
	pageset	lab
	andlw	m
	btfsc	STATUS,Z
	goto	lab
	pageres
	endm
gotoif_f8_msk_eq_f8	macro	v1,v2,lab
	gotoif_f_msk_eq_f	v1,v2,lab
	endm

;
; gotoif_f_msk_ne_f var1,var2,label
;
; Jumps to label if register (var1) is not equal to register (var2)
; destroys W
gotoif_f_msk_ne_f	macro	v1,m,v2,lab
	load_w_f	v2
	bankset	v1
	subwf	v1,W
	bankres	v1
	pageset	lab
	andlw	m
	btfss	STATUS,Z
	goto	lab
	pageres
	endm
gotoif_f8_msk_ne_f8	macro	v1,v2,lab
	gotoif_f_msk_ne_f	v1,v2,lab
	endm


;**************************************************************
;***               16/32 BITS GOTOIF CODES                  ***
;**************************************************************
; The following macros are defined in this section:
;
; gotoif_f16_eq_l16
; gotoif_f16_ne_l16
; gotoif_f16_gt_l16
; gotoif_f16_ge_l16
; gotoif_f16_lt_l16
; gotoif_f16_le_l16
; gotoif_f16_eq_f16
; gotoif_f16_ne_f16
; gotoif_f16_gt_f16
; gotoif_f16_ge_f16
; gotoif_f16_lt_f16
; gotoif_f16_le_f16

; gotoif_f32_eq_l32
; gotoif_f32_ne_l32
; gotoif_f32_gt_l32
; gotoif_f32_ge_l32
; gotoif_f32_lt_l32
; gotoif_f32_le_l32
; gotoif_f32_eq_f32
; gotoif_f32_ne_f32
; gotoif_f32_gt_f32
; gotoif_f32_ge_f32
; gotoif_f32_lt_f32
; gotoif_f32_le_f32


;
; gotoif_f16_eq_l16 var,lit,lab
;
; Jumps to label if 16 bits register (var) is equal to literal (lit)
; destroys W
gotoif_f16_eq_l16	macro	f,l,lab
kcnt++
	gotoif_f_ne_l	f+1,(l >> .8),k_0_#v(kcnt)
	gotoif_f_eq_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f16_ne_l16 var,lit,lab
;
; Jumps to label if 16 bits register (var) is not equal to literal (lit)
; destroys W
gotoif_f16_ne_l16	macro	f,l,lab
	gotoif_f_ne_l	f+1,(l >> .8),lab
	gotoif_f_ne_l	f,(l % 0x100),lab
	endm

;
; gotoif_f16_lt_l16 var1,var2,label
;
; Jumps to label if 16 bits register (var1) smaller (less) than literal (lit)
; destroys W
gotoif_f16_lt_l16	macro	f,l,lab
kcnt++
	gotoif_f_lt_l	f+1,(l >> .8),lab
	gotoif_f_gt_l	f+1,(l >> .8),k_0_#v(kcnt)
	gotoif_f_lt_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f16_le_l16 var1,var2,label
;
; Jumps to label if 16 bits register (var1) is smaller (less) or equal to literal (lit)
; destroys W
gotoif_f16_le_l16	macro	f,l,lab
kcnt++
	gotoif_f_lt_l	f+1,(l >> .8),lab
	gotoif_f_gt_l	f+1,(l >> .8),k_0_#v(kcnt)
	gotoif_f_le_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f16_gt_l16 var1,var2,label
;
; Jumps to label if 16 bits register (var1) is larger than literal (lit)
; destroys W
gotoif_f16_gt_l16	macro	f,l,lab
kcnt++
	gotoif_f_gt_l	f+1,(l >> .8),lab
	gotoif_f_lt_l	f+1,(l >> .8),k_0_#v(kcnt)
	gotoif_f_gt_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f16_ge_l16 var1,var2,label
;
; Jumps to label if 16 bits register (var1) is larger (greater) or equal to literal (lit)
; destroys W
gotoif_f16_ge_l16	macro	f,l,lab
kcnt++
	gotoif_f_gt_l	f+1,(l >> .8),lab
	gotoif_f_lt_l	f+1,(l >> .8),k_0_#v(kcnt)
	gotoif_f_ge_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm



;
; gotoif_f16_eq_f16 var1,var2,lab
;
; Jumps to label if 16 bits register (var1) is equal to 16 bits register (var2)
; destroys W
gotoif_f16_eq_f16	macro	f1,f2,lab
kcnt++
	gotoif_f_ne_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_eq_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f16_ne_f16 var1,var2,lab
;
; Jumps to label if 16 bits register (var1) is not equal to 16 bits register (var2)
; destroys W
gotoif_f16_ne_f16	macro	f1,f2,lab
	gotoif_f_ne_f	f1+1,f2+1,lab
	gotoif_f_ne_f	f1,f2,lab
	endm


;
; gotoif_f16_lt_f16 var1,var2,label
;
; Jumps to label if 16 bits register (var1) is smaller (less) than 16 bits register (var2)
; destroys W
gotoif_f16_lt_f16	macro	f1,f2,lab
kcnt++
	gotoif_f_lt_f	f1+1,f2+1,lab
	gotoif_f_gt_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_lt_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f16_le_f16 var1,var2,label
;
; Jumps to label if 16 bits register (var1) is smaller (less) or equal to 16 bits register (var2)
; destroys W
gotoif_f16_le_f16	macro	f1,f2,lab
kcnt++
	gotoif_f_lt_f	f1+1,f2+1,lab
	gotoif_f_gt_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_le_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f16_gt_f16 var1,var2,label
;
; Jumps to label if 16 bits register (var1) is larger (greater) than 16 bits register (var2)
; destroys W
gotoif_f16_gt_f16	macro	f1,f2,lab
kcnt++
	gotoif_f_gt_f	f1+1,f2+1,lab
	gotoif_f_lt_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_gt_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f16_ge_f16 var1,var2,label
;
; Jumps to label if 16 bits register (var1) is larger (greater) or equal to 16 bits register (var2)
; destroys W
gotoif_f16_ge_f16	macro	f1,f2,lab
kcnt++
	gotoif_f_gt_f	f1+1,f2+1,lab
	gotoif_f_lt_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_ge_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm


;
; gotoif_f32_eq_l32 var,lit,lab
;
; Jumps to label if 32 bits register (var) is equal to literal (lit)
; destroys W
gotoif_f32_eq_l32	macro	f,l,lab
kcnt++
	gotoif_f_ne_l	f+3,(l >> .24),k_0_#v(kcnt)
	gotoif_f_ne_l	f+2,(l >> .16) % 0x100,k_0_#v(kcnt)
	gotoif_f_ne_l	f+1,(l >> .8) % 0x100,k_0_#v(kcnt)
	gotoif_f_eq_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_ne_l32 var,lit,lab
;
; Jumps to label if 32 bits register (var) is not equal to literal (lit)
; destroys W
gotoif_f32_ne_l32	macro	f,l,lab
	gotoif_f_ne_l	f+3,(l >> .24),lab
	gotoif_f_ne_l	f+2,(l >> .16) % 0x100,lab
	gotoif_f_ne_l	f+1,(l >> .8) % 0x100,lab
	gotoif_f_ne_l	f,(l % 0x100),lab
	endm

;
; gotoif_f32_lt_l32 var1,var2,label
;
; Jumps to label if 32 bits register (var1) is smaller (less) than literal (lit)
; destroys W
gotoif_f32_lt_l32	macro	f,l,lab
kcnt++
	gotoif_f_lt_l	f+3,(l >> .24),lab
	gotoif_f_gt_l	f+3,(l >> .24),k_0_#v(kcnt)
	gotoif_f_lt_l	f+2,(l >> .16) % 0x100,lab
	gotoif_f_gt_l	f+2,(l >> .16) % 0x100,k_0_#v(kcnt)
	gotoif_f_lt_l	f+1,(l >> .8) % 0x100,lab
	gotoif_f_gt_l	f+1,(l >> .8) % 0x100,k_0_#v(kcnt)
	gotoif_f_lt_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_le_l32 var1,var2,label
;
; Jumps to label if 32 bits register (var1) is smaller (less) or equal to literal (lit)
; destroys W
gotoif_f32_le_l32	macro	f,l,lab
kcnt++
	gotoif_f_lt_l	f+3,(l >> .24),lab
	gotoif_f_gt_l	f+3,(l >> .24),k_0_#v(kcnt)
	gotoif_f_lt_l	f+2,(l >> .16) % 0x100,lab
	gotoif_f_gt_l	f+2,(l >> .16) % 0x100,k_0_#v(kcnt)
	gotoif_f_lt_l	f+1,(l >> .8) % 0x100,lab
	gotoif_f_gt_l	f+1,(l >> .8) % 0x100,k_0_#v(kcnt)
	gotoif_f_le_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_gt_l32 var1,var2,label
;
; Jumps to label if 32 bits register (var1) is larger than literal (lit)
; destroys W
gotoif_f32_gt_l32	macro	f,l,lab
kcnt++
	gotoif_f_gt_l	f+3,(l >> .24),lab
	gotoif_f_lt_l	f+3,(l >> .24),k_0_#v(kcnt)
	gotoif_f_gt_l	f+2,(l >> .16) % 0x100,lab
	gotoif_f_lt_l	f+2,(l >> .16) % 0x100,k_0_#v(kcnt)
	gotoif_f_gt_l	f+1,(l >> .8) % 0x100,lab
	gotoif_f_lt_l	f+1,(l >> .8) % 0x100,k_0_#v(kcnt)
	gotoif_f_gt_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_ge_l32 var1,var2,label
;
; Jumps to label if 32 bits register (var1) is larger (greater) or equal to literal (lit)
; destroys W
gotoif_f32_ge_l32	macro	f,l,lab
kcnt++
	gotoif_f_gt_l	f+3,(l >> .24),lab
	gotoif_f_lt_l	f+3,(l >> .24),k_0_#v(kcnt)
	gotoif_f_gt_l	f+2,(l >> .16) % 0x100,lab
	gotoif_f_lt_l	f+2,(l >> .16) % 0x100,k_0_#v(kcnt)
	gotoif_f_gt_l	f+1,(l >> .8) % 0x100,lab
	gotoif_f_lt_l	f+1,(l >> .8) % 0x100,k_0_#v(kcnt)
	gotoif_f_ge_l	f,(l % 0x100),lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_eq_f32 var1,var2,lab
;
; Jumps to label if 32 bits register (var1) is equal to 32 bits register (var2)
; destroys W
gotoif_f32_eq_f32	macro	f1,f2,lab
kcnt++
	gotoif_f_ne_f	f1+3,f2+3,k_0_#v(kcnt)
	gotoif_f_ne_f	f1+2,f2+2,k_0_#v(kcnt)
	gotoif_f_ne_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_eq_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_ne_f32 var1,var2,lab
;
; Jumps to label if 32 bits register (var1) is not equal to 32 bits register (var2)
; destroys W
gotoif_f32_ne_f32	macro	f1,f2,lab
	gotoif_f_ne_f	f1+3,f2+3,lab
	gotoif_f_ne_f	f1+2,f2+2,lab
	gotoif_f_ne_f	f1+1,f2+1,lab
	gotoif_f_ne_f	f1,f2,lab
	endm


;
; gotoif_f32_lt_f32 var1,var2,label
;
; Jumps to label if 32 bits register (var1) is smaller (less) than 32 bits register (var2)
; destroys W
gotoif_f32_lt_f32	macro	f1,f2,lab
kcnt++
	gotoif_f_lt_f	f1+3,f2+3,lab
	gotoif_f_gt_f	f1+3,f2+3,k_0_#v(kcnt)
	gotoif_f_lt_f	f1+2,f2+2,lab
	gotoif_f_gt_f	f1+2,f2+2,k_0_#v(kcnt)
	gotoif_f_lt_f	f1+1,f2+1,lab
	gotoif_f_gt_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_lt_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_le_f32 var1,var2,label
;
; Jumps to label if 32 bits register (var1) is smaller (less) or equal to 32 bits register (var2)
; destroys W
gotoif_f32_le_f32	macro	f1,f2,lab
kcnt++
	gotoif_f_lt_f	f1+3,f2+3,lab
	gotoif_f_gt_f	f1+3,f2+3,k_0_#v(kcnt)
	gotoif_f_lt_f	f1+2,f2+2,lab
	gotoif_f_gt_f	f1+2,f2+2,k_0_#v(kcnt)
	gotoif_f_lt_f	f1+1,f2+1,lab
	gotoif_f_gt_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_le_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_gt_f32 var1,var2,label
;
; Jumps to label if 32 bits register (var1) is larger (greater) than 32 bits register (var2)
; destroys W
gotoif_f32_gt_f32	macro	f1,f2,lab
kcnt++
	gotoif_f_gt_f	f1+3,f2+3,lab
	gotoif_f_lt_f	f1+3,f2+3,k_0_#v(kcnt)
	gotoif_f_gt_f	f1+2,f2+2,lab
	gotoif_f_lt_f	f1+2,f2+2,k_0_#v(kcnt)
	gotoif_f_gt_f	f1+1,f2+1,lab
	gotoif_f_lt_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_gt_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm

;
; gotoif_f32_ge_f32 var1,var2,label
;
; Jumps to label if 32 bits register (var1) is larger (greater) or equal to 32 bits register (var2)
; destroys W
gotoif_f32_ge_f32	macro	f1,f2,lab
kcnt++
	gotoif_f_gt_f	f1+3,f2+3,lab
	gotoif_f_lt_f	f1+3,f2+3,k_0_#v(kcnt)
	gotoif_f_gt_f	f1+2,f2+2,lab
	gotoif_f_lt_f	f1+2,f2+2,k_0_#v(kcnt)
	gotoif_f_gt_f	f1+1,f2+1,lab
	gotoif_f_lt_f	f1+1,f2+1,k_0_#v(kcnt)
	gotoif_f_ge_f	f1,f2,lab
k_0_#v(kcnt):
	banklabel
	endm


;**************************************************************
;***                    IF ELSE END CODES                   ***
;**************************************************************
; The following macros are defined in this section:
;
; end_if
; else_if
;
; if_f_eq_l
; if_f_ne_l
; if_f_gt_l
; if_f_ge_l
; if_f_lt_l
; if_f_le_l
; if_f_eq_f
; if_f_ne_f
; if_f_gt_f
; if_f_ge_f
; if_f_lt_f
; if_f_le_f
; if_f_b_c
; if_f_b_s
;
; if_f_msk_eq_l
; if_f_msk_ne_l
; if_f_msk_eq_f
; if_f_msk_ne_f
;
; if_f16_eq_l16
; if_f16_ne_l16
; if_f16_gt_l16
; if_f16_ge_l16
; if_f16_lt_l16
; if_f16_le_l16
; if_f16_eq_f16
; if_f16_ne_f16
; if_f16_gt_f16
; if_f16_ge_f16
; if_f16_lt_f16
; if_f16_le_f16

; if_f32_eq_l32
; if_f32_ne_l32
; if_f32_gt_l32
; if_f32_ge_l32
; if_f32_lt_l32
; if_f32_le_l32
; if_f32_eq_f32
; if_f32_ne_f32
; if_f32_gt_f32
; if_f32_ge_f32
; if_f32_lt_f32
; if_f32_le_f32

;
; end_if
;
; End of an if-block
; 
end_if	macro
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
cdep--
	endm

;
; else_ or else_if
;
; End of the first if-block, continues with else-block.
; 
else_	macro
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

else_if	macro
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm


;
; if_f_eq_l var,lit
;
; Executes following block if register (var) is equal to literal (lit)
; destroys W
if_f_eq_l	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_ne_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_ne_l var,lit
;
; Executes following block if register (var) is not equal to literal (lit)
; destroys W
if_f_ne_l	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_eq_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_gt_l var,lit
;
; Executes following block if register (var) is larger (greater) than literal (lit)
; destroys W
if_f_gt_l	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_le_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_ge_l var,lit
;
; Executes following block if register (var) larger (greater) or equal to literal (lit)
; destroys W
if_f_ge_l	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_lt_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_lt_l var,lit
;
; Executes following block if register (var) is smaller (less) than literal (lit)
; destroys W
if_f_lt_l	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_ge_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_le_l var,lit
;
; Executes following block if register (var) is smaller (less) or equal to literal (lit)
; destroys W
if_f_le_l	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_gt_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_eq_f var1,var2
;
; Executes following block if register (var1) is equal to register (var2)
; destroys W
if_f_eq_f	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_ne_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_ne_f var1,var2
;
; Executes following block if register (var1) is not equal to register (var2)
; destroys W
if_f_ne_f	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_eq_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_gt_f var1,var2
;
; Executes following block if register (var1) larger (greater) than register (var2)
; destroys W
if_f_gt_f	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_le_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_ge_f var1,var2
;
; Executes following block if register (var1) larger (greater) or equal to register (var2)
; destroys W
if_f_ge_f	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_lt_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_lt_f var1,var2
;
; Executes following block if register (var1) is smaller (less) than register (var2)
; destroys W
if_f_lt_f	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_ge_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_le_f var1,var2
;
; Executes following block if register (var1) is smaller (less) or equal to register (var2)
; destroys W
if_f_le_f	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_gt_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_b_s var,lit
;
; Executes following block if bit (lit) of register (var) is set
; destroys W
if_f_b_s	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_b_c	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_b_c var,lit
;
; Executes following block if bit (lit) of register (var) is clear
; destroys W
if_f_b_c	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_b_s	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_msk_eq_l var,mask,lit
;
; Executes following block if register (var) is equal to literal (lit)
; destroys W
if_f_msk_eq_l	macro	v,m,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_msk_ne_l	v,m,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_msk_ne_l var,mask,lit
;
; Executes following block if register (var) is not equal to literal (lit)
; destroys W
if_f_msk_ne_l	macro	v,m,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_msk_eq_l	v,m,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_msk_eq_f var1,mask,var2
;
; Executes following block if register (var1) is equal to register (var2)
; destroys W
if_f_msk_eq_f	macro	v1,m,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_msk_ne_f	v1,m,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f_msk_ne_f var1,mask,var2
;
; Executes following block if register (var1) is not equal to register (var2)
; destroys W
if_f_msk_ne_f	macro	v1,m,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f_msk_eq_f	v1,m,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm


;
; if_f16_eq_l16 var,lit
;
; Executes following block if 16 bits register (var) is equal to literal (lit)
; destroys W
if_f16_eq_l16	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_ne_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_ne_l16 var,lit
;
; Executes following block if 16 bits register (var) is not equal to literal (lit)
; destroys W
if_f16_ne_l16	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_eq_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_gt_l16 var,lit
;
; Executes following block if 16 bits register (var) is larger (greater) than literal (lit)
; destroys W
if_f16_gt_l16	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_le_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_ge_l16 var,lit
;
; Executes following block if 16 bits register (var) is larger (greater) or equal to literal (lit)
; destroys W
if_f16_ge_l16	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_lt_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_lt_l16 var,lit
;
; Executes following block if 16 bits register (var) is smaller (less) than literal (lit)
; destroys W
if_f16_lt_l16	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_ge_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_le_l16 var,lit
;
; Executes following block if 16 bits register (var) is less or equal to literal (lit)
; destroys W
if_f16_le_l16	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_gt_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_eq_f16 var1,var2
;
; Executes following block if 16 bits register (var1) is equal to 16 bits register (var2)
; destroys W
if_f16_eq_f16	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_ne_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_ne_f16 var1,var2
;
; Executes following block if 16 bits register (var1) is not equal to 16 bits register (var2)
; destroys W
if_f16_ne_f16	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_eq_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_gt_f16 var1,var2
;
; Executes following block if 16 bits register (var1) is larger (greater) than 16 bits register (var2)
; destroys W
if_f16_gt_f16	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_le_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_ge_f16 var1,var2
;
; Executes following block if 16 bits register (var1) is larger (greater) or equal to 16 bits register (var2)
; destroys W
if_f16_ge_f16	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_lt_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_lt_f16 var1,var2
;
; Executes following block if 16 bits register (var1) is smaller (less) than 16 bits register (var2)
; destroys W
if_f16_lt_f16	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_ge_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f16_le_f16 var1,var2
;
; Executes following block if 16 bits register (var1) is smaller (less) or equal to 16 bits register (var2)
; destroys W
if_f16_le_f16	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f16_gt_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_eq_l32 var,lit
;
; Executes following block if 32 bits register (var) is equal to literal (lit)
; destroys W
if_f32_eq_l32	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_ne_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_ne_l32 var,lit
;
; Executes following block if 32 bits register (var) is not equal to literal (lit)
; destroys W
if_f32_ne_l32	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_eq_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_gt_l32 var,lit
;
; Executes following block if 32 bits register (var) is larger (greater) than literal (lit)
; destroys W
if_f32_gt_l32	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_le_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_ge_l32 var,lit
;
; Executes following block if 32 bits register (var) is larger (greater) or equal to literal (lit)
; destroys W
if_f32_ge_l32	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_lt_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_lt_l32 var,lit
;
; Executes following block if 32 bits register (var) is smaller (less) than literal (lit)
; destroys W
if_f32_lt_l32	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_ge_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_le_l32 var,lit
;
; Executes following block if 32 bits register (var) is smaller (less) or equal to literal (lit)
; destroys W
if_f32_le_l32	macro	v,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_gt_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_eq_f32 var1,var2
;
; Executes following block if 32 bits register (var1) is equal to 32 bits register (var2)
; destroys W
if_f32_eq_f32	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_ne_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_ne_f32 var1,var2
;
; Executes following block if 32 bits register (var1) is not equal to 32 bits register (var2)
; destroys W
if_f32_ne_f32	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_eq_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_gt_f32 var1,var2
;
; Executes following block if 32 bits register (var1) is larger (greater) than 32 bits register (var2)
; destroys W
if_f32_gt_f32	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_le_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_ge_f32 var1,var2
;
; Executes following block if 32 bits register (var1) is larger (greater) or equal to 32 bits register (var2)
; destroys W
if_f32_ge_f32	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_lt_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_lt_f32 var1,var2
;
; Executes following block if 32 bits register (var1) is smaller (less) than 32 bits register (var2)
; destroys W
if_f32_lt_f32	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_ge_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; if_f32_le_f32 var1,var2
;
; Executes following block if 32 bits register (var1) is smaller (less) or equal to 32 bits register (var2)
; destroys W
if_f32_le_f32	macro	v1,v2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	gotoif_f32_gt_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm


;**************************************************************
;***                      FOR NEXT CODES                    ***
;**************************************************************
; The following macros are defined in this section:
;
; for_f_l_l
; for_f_l_f
; for_f_f_l
; for_f_f_f
;
; for_f16_l16_l16
; for_f16_l16_f16
; for_f16_f16_l16
; for_f16_f16_f16
;
; for_f32_l32_l32
; for_f32_l32_f32
; for_f32_f32_l32
; for_f32_f32_f32
;
; next

;
; for_f_l_l var1,lit1,lit2
;
; Executes following block in which var1 is increased from lit1 to (and including) lit2 
; destroys W
for_f_l_l	macro	var1,lit1,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f_l	var1,lit1	
	gotoif_f_gt_l	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
labcnt#v(cdep)++
	gotoif_f_ge_l	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	bankset	var1
	incf	var1,f
	bankres	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm
for_f8_l8_l8	macro	var1,lit1,lit2
	for_f_l_l	var1,lit1,lit2
	endm

;
; for_f_f_l var1,var2,lit2
;
; Executes following block in which var1 is increased from var2 to (and including) lit2 
; destroys W
for_f_f_l	macro	var1,var2,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f_f	var1,var2	
	gotoif_f_gt_l	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_ge_l	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	bankset	var1
	incf	var1,f
	bankres	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm
for_f8_f8_l8	macro	var1,var2,lit2
	for_f_f_l	var1,var2,lit2
	endm

;
; for_f_l_f var1,lit1,var3
;
; Executes following block in which var1 is increased from lit1 to (and including) var3 
; destroys W
for_f_l_f	macro	var1,lit1,var2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f_l	var1,lit1	
	gotoif_f_gt_f	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_ge_f	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	bankset	var1
	incf	var1,f
	bankres	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm
for_f8_l8_f8	macro	var1,lit1,var3
	for_f_l_f	var1,lit1,var3
	endm

;
; for_f_f_f var1,var2,var3
;
; Executes following block in which var1 is increased from var2 to (and including) var3 
; destroys W
for_f_f_f	macro	var1,var2,var3
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f_f	var1,var2
	gotoif_f_gt_f	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_ge_f	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	bankset	var1
	incf	var1,f
	bankres	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm
for_f8_f8_f8	macro	var1,var2,var3
	for_f_f_f	var1,var2,var3
	endm

;
; for_f16_l16_l16 var1,lit1,lit2
;
; Executes following block in which 16 bits var1 is increased from lit1 to (and including) lit2 
; destroys W
for_f16_l16_l16	macro	var1,lit1,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f16_l16	var1,lit1	
	gotoif_f16_gt_l16	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_ge_l16	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	inc_f16	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; for_f16_f16_l16 var1,var2,lit2
;
; Executes following block in which 16 bits var1 is increased from 16 bits var2 to (and including) lit2 
; destroys W
for_f16_f16_l16	macro	var1,var2,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f16_f16	var1,var2	
	gotoif_f16_gt_l16	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_ge_l16	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	incf_16	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; for_f16_l16_f16 var1,lit1,var3
;
; Executes following block in which 16 bits var1 is increased from lit1 to (and including) 16 bits var3 
; destroys W
for_f16_l16_f16	macro	var1,lit1,var2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f16_l16	var1,lit1	
	gotoif_f16_gt_f16	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_ge_f16	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	inc_f16	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; for_f16_f16_f16 var1,var2,var3
;
; Executes following block in which 16 bits var1 is increased from 16 bits var2 to (and including) 16 bits var3 
; destroys W
for_f16_f16_f16	macro	var1,var2,var3
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f16_f16	var1,var2
	gotoif_f16_gt_f16	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_ge_f16	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	inc_f16	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; for_f32_l32_l32 var1,lit1,lit2
;
; Executes following block in which 32 bits var1 is increased from lit1 to (and including) lit2 
; destroys W
for_f32_l32_l32	macro	var1,lit1,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f32_l32	var1,lit1	
	gotoif_f32_gt_l32	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_ge_l32	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	inc_f32	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; for_f32_f32_l32 var1,var2,lit2
;
; Executes following block in which 32 bits var1 is increased from 32 bits var2 to (and including) lit2 
; destroys W
for_f32_f32_l32	macro	var1,var2,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f32_f32	var1,var2	
	gotoif_f32_gt_l32	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_ge_l32	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	inc_f32	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; for_f32_l32_f32 var1,lit1,var3
;
; Executes following block in which 32 bits var1 is increased from lit1 to (and including) 32 bits var3 
; destroys W
for_f32_l32_f32	macro	var1,lit1,var2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f32_l32	var1,lit1	
	gotoif_f32_gt_f32	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_ge_f32	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	inc_f32	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; for_f32_f32_f32 var1,var2,var3
;
; Executes following block in which 32 bits var1 is increased from 32 bits var2 to (and including) 32 bits var3 
; destroys W
for_f32_f32_f32	macro	var1,var2,var3
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f32_f32	var1,var2
	gotoif_f32_gt_f32	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_ge_f32	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	inc_f32	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; ford_f_l_l var1,lit1,lit2
;
; Executes following block in which var1 is decreased from lit1 to (and including) lit2 
; destroys W
ford_f_l_l	macro	var1,lit1,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f_l	var1,lit1	
	gotoif_f_lt_l	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_le_l	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	bankset	var1
	decf	var1,f
	bankres	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm
ford_f8_l8_l8	macro	var1,lit1,lit2
	ford_f_l_l	var1,lit1,lit2
	endm

;
; ford_f_f_l var1,var2,lit2
;
; Executes following block in which var1 is decreased from var2 to (and including) lit2 
; destroys W
ford_f_f_l	macro	var1,var2,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f_f	var1,var2	
	gotoif_f_lt_l	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_le_l	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	bankset	var1
	decf	var1,f
	bankres	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm
ford_f8_f8_l8	macro	var1,var2,lit2
	ford_f_f_l	var1,var2,lit2
	endm

;
; ford_f_l_f var1,lit1,var3
;
; Executes following block in which var1 is decreased from lit1 to (and including) var3 
; destroys W
ford_f_l_f	macro	var1,lit1,var2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f_l	var1,lit1	
	gotoif_f_lt_f	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_le_f	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	bankset	var1
	decf	var1,f
	bankres	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm
ford_f8_l8_f8	macro	var1,lit1,var3
	ford_f_l_f	var1,lit1,var3
	endm

;
; ford_f_f_f var1,var2,var3
;
; Executes following block in which var1 is decreased from var2 to (and including) var3 
; destroys W
ford_f_f_f	macro	var1,var2,var3
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f_f	var1,var2
	gotoif_f_lt_f	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_le_f	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	bankset	var1
	decf	var1,f
	bankres	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm
ford_f8_f8_f8	macro	var1,var2,var3
	ford_f_f_f	var1,var2,var3
	endm

;
; ford_f16_l16_l16 var1,lit1,lit2
;
; Executes following block in which 16 bits var1 is decreased from lit1 to (and including) lit2 
; destroys W
ford_f16_l16_l16	macro	var1,lit1,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f16_l16	var1,lit1	
	gotoif_f16_lt_l16	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_le_l16	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	dec_f16	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; ford_f16_f16_l16 var1,var2,lit2
;
; Executes following block in which 16 bits var1 is decreased from 16 bits var2 to (and including) lit2 
; destroys W
ford_f16_f16_l16	macro	var1,var2,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f16_f16	var1,var2	
	gotoif_f16_lt_l16	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_le_l16	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	dec_f16	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; ford_f16_l16_f16 var1,lit1,var3
;
; Executes following block in which 16 bits var1 is decreased from lit1 to (and including) 16 bits var3 
; destroys W
ford_f16_l16_f16	macro	var1,lit1,var2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f16_l16	var1,lit1	
	gotoif_f16_lt_f16	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_le_f16	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	dec_f16	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; ford_f16_f16_f16 var1,var2,var3
;
; Executes following block in which 16 bits var1 is decreased from 16 bits var2 to (and including) 16 bits var3 
; destroys W
ford_f16_f16_f16	macro	var1,var2,var3
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f16_f16	var1,var2
	gotoif_f16_lt_f16	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_le_f16	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	dec_f16	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; ford_f32_l32_l32 var1,lit1,lit2
;
; Executes following block in which 32 bits var1 is decreased from lit1 to (and including) lit2 
; destroys W
ford_f32_l32_l32	macro	var1,lit1,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f32_l32	var1,lit1	
	gotoif_f32_lt_l32	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_le_l32	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	dec_f32	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; ford_f32_f32_l32 var1,var2,lit2
;
; Executes following block in which 32 bits var1 is decreased from 32 bits var2 to (and including) lit2 
; destroys W
ford_f32_f32_l32	macro	var1,var2,lit2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f32_f32	var1,var2	
	gotoif_f32_lt_l32	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_le_l32	var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	dec_f32	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; ford_f32_l32_f32 var1,lit1,var3
;
; Executes following block in which 32 bits var1 is decreased from lit1 to (and including) 32 bits var3 
; destroys W
ford_f32_l32_f32	macro	var1,lit1,var2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f32_l32	var1,lit1	
	gotoif_f32_lt_f32	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_le_f32	var1,var2,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	dec_f32	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm

;
; ford_f32_f32_f32 var1,var2,var3
;
; Executes following block in which 32 bits var1 is decreased from 32 bits var2 to (and including) 32 bits var3 
; destroys W
ford_f32_f32_f32	macro	var1,var2,var3
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	load_f32_f32	var1,var2
	gotoif_f32_lt_f32	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+2)
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)+1)
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_le_f32	var1,var3,lab_#v(cdep)_#v(labcnt#v(cdep)+1)	
	dec_f32	var1
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	endm


;
; next
; closes for-next loop
;
next	macro
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)-2) ; go directly to the for part
lab_#v(cdep)_#v(labcnt#v(cdep)):	; this is the exit of the loop (from the for part)
	banklabel
cdep--	
	endm
next_f	macro	var1	; for backward compatibility
	next
	endm
next_f8	macro	var1	; for backward compatibility
	next
	endm
next_f16	macro	var1	; for backward compatibility
	next
	endm
next_f32	macro	var1	; for backward compatibility
	next
	endm



;**************************************************************
;***                    REPEAT UNTIL CODES                  ***
;**************************************************************
; The following macros are defined in this section:
;
; repeat
;
; until_f_eq_l
; until_f_ne_l
; until_f_gt_l
; until_f_ge_l
; until_f_lt_l
; until_f_le_l
; until_f_eq_f
; until_f_ne_f
; until_f_gt_f
; until_f_ge_f
; until_f_lt_f
; until_f_le_f
;
; until_f16_eq_l16
; until_f16_ne_l16
; until_f16_gt_l16
; until_f16_ge_l16
; until_f16_lt_l16
; until_f16_le_l16
; until_f16_eq_f16
; until_f16_ne_f16
; until_f16_gt_f16
; until_f16_ge_f16
; until_f16_lt_f16
; until_f16_le_f16
;
; until_f32_eq_l32
; until_f32_ne_l32
; until_f32_gt_l32
; until_f32_ge_l32
; until_f32_lt_l32
; until_f32_le_l32
; until_f32_eq_f32
; until_f32_ne_f32
; until_f32_gt_f32
; until_f32_ge_f32
; until_f32_lt_f32
; until_f32_le_f32

;
;   repeat
;
repeat	macro
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
	banklabel
lab_#v(cdep)_#v(labcnt#v(cdep)):
	endm

;
;   until_f_eq_l var, lit
;
; Executes previous block until (var) is equal to (lit) 
; destroys W
until_f_eq_l	macro	v,l
	gotoif_f_ne_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_eq_l8	macro	v,l
	until_f_eq_l	v,l
	endm

;
;   until_f_ne_l var, lit
;
; Executes previous block until (var) is not equal to (lit) 
; destroys W
until_f_ne_l	macro	v,l
	gotoif_f_eq_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_ne_l8	macro	v,l
	until_f_ne_l	v,l
	endm

;
;   until_f_gt_l var, lit
;
; Executes previous block until (var) is greater than (lit) 
; destroys W
until_f_gt_l	macro	v,l
	gotoif_f_le_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_gt_l8	macro	v,l
	until_f_gt_l	v,l
	endm

;
;   until_f_ge_l var, lit
;
; Executes previous block until (var) is greater or equal than (lit) 
; destroys W
until_f_ge_l	macro	v,l
	gotoif_f_lt_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_ge_l8	macro	v,l
	until_f_ge_l	v,l
	endm

;
;   until_f_lt_l var, lit
;
; Executes previous block until (var) is less than (lit) 
; destroys W
until_f_lt_l	macro	v,l
	gotoif_f_ge_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_lt_l8	macro	v,l
	until_f_lt_l	v,l
	endm

;
;   until_f_le_l var, lit
;
; Executes previous block until (var) is less or equal than (lit) 
; destroys W
until_f_le_l	macro	v,l
	gotoif_f_gt_l	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_le_l8	macro	v,l
	until_f_le_l	v,l
	endm

;
;   until_f_eq_f var1, var2
;
; Executes previous block until (var1) is equal to (var2) 
; destroys W
until_f_eq_f	macro	v1,v2
	gotoif_f_ne_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_eq_f8	macro	v1,v2
	until_f_eq_f	v1,v2
	endm

;
;   until_f_ne_f var1, var2
;
; Executes previous block until (var1) is not equal to (var2) 
; destroys W
until_f_ne_f	macro	v1,v2
	gotoif_f_eq_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_ne_f8	macro	v1,v2
	until_f_ne_f	v1,v2
	endm

;
;   until_f_gt_f var1, var2
;
; Executes previous block until (var1) is greater than (var2) 
; destroys W
until_f_gt_f	macro	v1,v2
	gotoif_f_le_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_gt_f8	macro	v1,v2
	until_f_gt_f	v1,v2
	endm

;
;   until_f_ge_f var1, var2
;
; Executes previous block until (var1) is greater or equal than (var2) 
; destroys W
until_f_ge_f	macro	v1,v2
	gotoif_f_lt_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_ge_f8	macro	v1,v2
	until_f_ge_f	v1,v2
	endm

;
;   until_f_lt_f var1, var2
;
; Executes previous block until (var1) is less than (var2) 
; destroys W
until_f_lt_f	macro	v1,v2
	gotoif_f_ge_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_lt_f8	macro	v1,v2
	until_f_lt_f	v1,v2
	endm

;
;   until_f_le_f var1, var2
;
; Executes previous block until (var1) is less or equal than (var2) 
; destroys W
until_f_le_f	macro	v1,v2
	gotoif_f_gt_f	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_le_f8	macro	v1,v2
	until_f_le_f	v1,v2
	endm

;
;   until_f_b_s var, lit
;
; Executes previous block until bit (lit) of (var1) is set 
; destroys W
until_f_b_s	macro	v,l
	gotoif_f_b_c	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_b_s	macro	v,l
	until_f_b_s	v,l
	endm

;
;   until_f_b_c var, lit
;
; Executes previous block until bit (lit) of (var1) is clear 
; destroys W
until_f_b_c	macro	v,l
	gotoif_f_b_s	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm
until_f8_b_c	macro	v,l
	until_f_b_c	v,l
	endm

;
;   until_f16_eq_l16 var, lit
;
; Executes previous block until 16 bits (var) is equal to (lit) 
; destroys W
until_f16_eq_l16	macro	v,l
	gotoif_f16_ne_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_ne_l16 var, lit
;
; Executes previous block until 16 bits (var) is not equal to (lit) 
; destroys W
until_f16_ne_l16	macro	v,l
	gotoif_f16_eq_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_gt_l16 var, lit
;
; Executes previous block until 16 bits (var) is greater than (lit) 
; destroys W
until_f16_gt_l16	macro	v,l
	gotoif_f16_le_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_ge_l16 var, lit
;
; Executes previous block until 16 bits (var) is greater or equal than (lit) 
; destroys W
until_f16_ge_l16	macro	v,l
	gotoif_f16_lt_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_lt_l16 var, lit
;
; Executes previous block until 16 bits (var) is less than (lit) 
; destroys W
until_f16_lt_l16	macro	v,l
	gotoif_f16_ge_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_le_l16 var, lit
;
; Executes previous block until 16 bits (var) is less or equal than (lit) 
; destroys W
until_f16_le_l16	macro	v,l
	gotoif_f16_gt_l16	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_eq_f16 var1, var2
;
; Executes previous block until 16 bits (var1) is equal to 16 bits (var2) 
; destroys W
until_f16_eq_f16	macro	v1,v2
	gotoif_f16_ne_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_ne_f16 var1, var2
;
; Executes previous block until 16 bits (var1) is not equal to 16 bits (var2) 
; destroys W
until_f16_ne_f16	macro	v1,v2
	gotoif_f16_eq_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_gt_f16 var1, var2
;
; Executes previous block until 16 bits (var1) is greater than 16 bits (var2) 
; destroys W
until_f16_gt_f16	macro	v1,v2
	gotoif_f16_le_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_ge_f16 var1, var2
;
; Executes previous block until 16 bits (var1) is greater or equal than 16 bits (var2) 
; destroys W
until_f16_ge_f16	macro	v1,v2
	gotoif_f16_lt_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_lt_f16 var1, var2
;
; Executes previous block until 16 bits (var1) is less than 16 bits (var2) 
; destroys W
until_f16_lt_f16	macro	v1,v2
	gotoif_f16_ge_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f16_le_f16 var1, var2
;
; Executes previous block until 16 bits (var1) is less or equal than 16 bits (var2) 
; destroys W
until_f16_le_f16	macro	v1,v2
	gotoif_f16_gt_f16	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm



;
;   until_f32_eq_l32 var, lit
;
; Executes previous block until 32 bits (var) is equal to (lit) 
; destroys W
until_f32_eq_l32	macro	v,l
	gotoif_f32_ne_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_ne_l32 var, lit
;
; Executes previous block until 32 bits (var) is not equal to (lit) 
; destroys W
until_f32_ne_l32	macro	v,l
	gotoif_f32_eq_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_gt_l32 var, lit
;
; Executes previous block until 32 bits (var) is greater than (lit) 
; destroys W
until_f32_gt_l32	macro	v,l
	gotoif_f32_le_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_ge_l32 var, lit
;
; Executes previous block until 32 bits (var) is greater or equal than (lit) 
; destroys W
until_f32_ge_l32	macro	v,l
	gotoif_f32_lt_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_lt_l32 var, lit
;
; Executes previous block until 32 bits (var) is less than (lit) 
; destroys W
until_f32_lt_l32	macro	v,l
	gotoif_f32_ge_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_le_l32 var, lit
;
; Executes previous block until 32 bits (var) is less or equal than (lit) 
; destroys W
until_f32_le_l32	macro	v,l
	gotoif_f32_gt_l32	v,l,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_eq_f32 var1, var2
;
; Executes previous block until 32 bits (var1) is equal to 32 bits (var2) 
; destroys W
until_f32_eq_f32	macro	v1,v2
	gotoif_f32_ne_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_ne_f32 var1, var2
;
; Executes previous block until 32 bits (var1) is not equal to 32 bits (var2) 
; destroys W
until_f32_ne_f32	macro	v1,v2
	gotoif_f32_eq_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_gt_f32 var1, var2
;
; Executes previous block until 32 bits (var1) is greater than 32 bits (var2) 
; destroys W
until_f32_gt_f32	macro	v1,v2
	gotoif_f32_le_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_ge_f32 var1, var2
;
; Executes previous block until 32 bits (var1) is greater or equal than 32 bits (var2) 
; destroys W
until_f32_ge_f32	macro	v1,v2
	gotoif_f32_lt_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_lt_f32 var1, var2
;
; Executes previous block until 32 bits (var1) is less than 32 bits (var2) 
; destroys W
until_f32_lt_f32	macro	v1,v2
	gotoif_f32_ge_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;
;   until_f32_le_f32 var1, var2
;
; Executes previous block until 32 bits (var1) is less or equal than 32 bits (var2) 
; destroys W
until_f32_le_f32	macro	v1,v2
	gotoif_f32_gt_f32	v1,v2,lab_#v(cdep)_#v(labcnt#v(cdep))
cdep--
	endm

;**************************************************************
;***                       WHILE CODES                      ***
;**************************************************************
; The following macros are defined in this section:
;
; while_f_eq_l
; while_f_ne_l
; while_f_gt_l
; while_f_ge_l
; while_f_lt_l
; while_f_le_l
; while_f_eq_f
; while_f_ne_f
; while_f_gt_f
; while_f_ge_f
; while_f_lt_f
; while_f_le_f
;
; while_f16_eq_l16
; while_f16_ne_l16
; while_f16_gt_l16
; while_f16_ge_l16
; while_f16_lt_l16
; while_f16_le_l16
; while_f16_eq_f16
; while_f16_ne_f16
; while_f16_gt_f16
; while_f16_ge_f16
; while_f16_lt_f16
; while_f16_le_f16
;
; while_f32_eq_l32
; while_f32_ne_l32
; while_f32_gt_l32
; while_f32_ge_l32
; while_f32_lt_l32
; while_f32_le_l32
; while_f32_eq_f32
; while_f32_ne_f32
; while_f32_gt_f32
; while_f32_ge_f32
; while_f32_lt_f32
; while_f32_le_f32
;
; end_while

;
; while_f_eq_l var1, lit
;
; Executes next block (up to end_while) while (var) is equal to (lit) 
; destroys W
while_f_eq_l	macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_ne_l	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_eq_l8	macro	f1,l
	while_f_eq_l	f1,l
	endm

;
; while_f_ne_l var1, lit
;
; Executes next block (up to end_while) while (var) is not equal to (lit) 
; destroys W
while_f_ne_l	macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_eq_l	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_ne_l8	macro	f1,l
	while_f_ne_l	f1,l
	endm

;
; while_f_gt_l var1, lit
;
; Executes next block (up to end_while) while (var) is greater than (lit) 
; destroys W
while_f_gt_l	macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_le_l	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_gt_l8	macro	f1,l
	while_f_gt_l	f1,l
	endm

;
; while_f_ge_l var1, lit
;
; Executes next block (up to end_while) while (var) is greater or equal to (lit) 
; destroys W
while_f_ge_l	macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_lt_l	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_ge_l8	macro	f1,l
	while_f_ge_l	f1,l
	endm

;
; while_f_lt_l var1, lit
;
; Executes next block (up to end_while) while (var) is less than (lit) 
; destroys W
while_f_lt_l	macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_ge_l	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_lt_l8	macro	f1,l
	while_f_lt_l	f1,l
	endm

;
; while_f_le_l var1, lit
;
; Executes next block (up to end_while) while (var) is less or equal to (lit) 
; destroys W
while_f_le_l	macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_gt_l	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_le_l8	macro	f1,l
	while_f_le_l	f1,l
	endm

;
; while_f_eq_f var1, var2
;
; Executes next block (up to end_while) while (var1) is equal to (var2)
; destroys W
while_f_eq_f	macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_ne_f	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_eq_f8	macro	f1,f2
	while_f_eq_f	f1,f2
	endm

;
; while_f_ne_f var1, var2
;
; Executes next block (up to end_while) while (var1) is not equal to (var2) 
; destroys W
while_f_ne_f	macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_eq_f	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_ne_f8	macro	f1,f2
	while_f_ne_f	f1,f2
	endm

;
; while_f_gt_f var1, var2
;
; Executes next block (up to end_while) while (var1) is greater than (var2) 
; destroys W
while_f_gt_f	macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_le_f	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_gt_f8	macro	f1,f2
	while_f_gt_f	f1,f2
	endm

;
; while_f_ge_f var1, var2
;
; Executes next block (up to end_while) while (var1) is greater or equal to (var2) 
; destroys W
while_f_ge_f	macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_lt_f	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_ge_f8	macro	f1,f2
	while_f_ge_f	f1,f2
	endm

;
; while_f_lt_f var1, var2
;
; Executes next block (up to end_while) while (var1) is less than (var2)
; destroys W
while_f_lt_f	macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_ge_f	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_lt_f8	macro	f1,f2
	while_f_lt_f	f1,f2
	endm

;
; while_f_le_f var1, var2
;
; Executes next block (up to end_while) while (var1) is less or equal to (var2)
; destroys W
while_f_le_f	macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f_gt_f	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm
while_f8_le_f8	macro	f1,f2
	while_f_le_f	f1,f2
	endm



;
; while_f16_eq_l16 var1, lit
;
; Executes next block (up to end_while) while 16 bits (var) is equal to (lit) 
; destroys W
while_f16_eq_l16 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_ne_l16	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_ne_l16 var1, lit
;
; Executes next block (up to end_while) while 16 bits (var) is not equal to (lit) 
; destroys W
while_f16_ne_l16 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_eq_l16	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_gt_l16 var1, lit
;
; Executes next block (up to end_while) while 16 bits (var) is greater than (lit) 
; destroys W
while_f16_gt_l16 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_le_l16	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_ge_l16 var1, lit
;
; Executes next block (up to end_while) while 16 bits (var) is greater or equal to (lit) 
; destroys W
while_f16_ge_l16 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_lt_l16	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_lt_l16 var1, lit
;
; Executes next block (up to end_while) while 16 bits (var) is less than (lit) 
; destroys W
while_f16_lt_l16 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_ge_l16	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_le_l16 var1, lit
;
; Executes next block (up to end_while) while 16 bits (var) is less or equal to (lit) 
; destroys W
while_f16_le_l16 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_gt_l16	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_eq_f16 var1, var2
;
; Executes next block (up to end_while) while 16 bits (var1) is equal to 16 bits (var2)
; destroys W
while_f16_eq_f16 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_ne_f16	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_ne_f16 var1, var2
;
; Executes next block (up to end_while) while 16 bits (var1) is not equal to 16 bits (var2) 
; destroys W
while_f16_ne_f16 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_eq_f16	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_gt_f16 var1, var2
;
; Executes next block (up to end_while) while 16 bits (var1) is greater than 16 bits (var2) 
; destroys W
while_f16_gt_f16 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_le_f16	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_ge_f16 var1, var2
;
; Executes next block (up to end_while) while 16 bits (var1) is greater or equal to 16 bits (var2) 
; destroys W
while_f16_ge_f16 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_lt_f16	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_lt_f16 var1, var2
;
; Executes next block (up to end_while) while 16 bits (var1) is less than 16 bits (var2)
; destroys W
while_f16_lt_f16 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_ge_f16	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f16_le_f16 var1, var2
;
; Executes next block (up to end_while) while 16 bits (var1) is less or equal to 16 bits (var2)
; destroys W
while_f16_le_f16 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f16_gt_f16	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_eq_l32 var1, lit
;
; Executes next block (up to end_while) while 32 bits (var) is equal to (lit) 
; destroys W
while_f32_eq_l32 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_ne_l32	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_ne_l32 var1, lit
;
; Executes next block (up to end_while) while 32 bits (var) is not equal to (lit) 
; destroys W
while_f32_ne_l32 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_eq_l32	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_gt_l32 var1, lit
;
; Executes next block (up to end_while) while 32 bits (var) is greater than (lit) 
; destroys W
while_f32_gt_l32 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_le_l32	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_ge_l32 var1, lit
;
; Executes next block (up to end_while) while 32 bits (var) is greater or equal to (lit) 
; destroys W
while_f32_ge_l32 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_lt_l32	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_lt_l32 var1, lit
;
; Executes next block (up to end_while) while 32 bits (var) is less than (lit) 
; destroys W
while_f32_lt_l32 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_ge_l32	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_le_l32 var1, lit
;
; Executes next block (up to end_while) while 32 bits (var) is less or equal to (lit) 
; destroys W
while_f32_le_l32 macro	f1,l
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_gt_l32	f1,l,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_eq_f32 var1, var2
;
; Executes next block (up to end_while) while 32 bits (var1) is equal to 32 bits (var2)
; destroys W
while_f32_eq_f32 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_ne_f32	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_ne_f32 var1, var2
;
; Executes next block (up to end_while) while 32 bits (var1) is not equal to 32 bits (var2) 
; destroys W
while_f32_ne_f32 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_eq_f32	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_gt_f32 var1, var2
;
; Executes next block (up to end_while) while 32 bits (var1) is greater than 32 bits (var2) 
; destroys W
while_f32_gt_f32 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_le_f32	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_ge_f32 var1, var2
;
; Executes next block (up to end_while) while 32 bits (var1) is greater or equal to 32 bits (var2) 
; destroys W
while_f32_ge_f32 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_lt_f32	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_lt_f32 var1, var2
;
; Executes next block (up to end_while) while 32 bits (var1) is less than 32 bits (var2)
; destroys W
while_f32_lt_f32 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_ge_f32	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
; while_f32_le_f32 var1, var2
;
; Executes next block (up to end_while) while 32 bits (var1) is less or equal to 32 bits (var2)
; destroys W
while_f32_le_f32 macro	f1,f2
cdep++
	if cdep>mdep
mdep++
labcnt#v(cdep) = 0
	endif
labcnt#v(cdep)++
lab_#v(cdep)_#v(labcnt#v(cdep)):
	banklabel
labcnt#v(cdep)++
	gotoif_f32_gt_f32	f1,f2,lab_#v(cdep)_#v(labcnt#v(cdep))
	endm

;
;   end_while
;
end_while	macro
	gotop	lab_#v(cdep)_#v(labcnt#v(cdep)-1) ; jump directly to the while-part
lab_#v(cdep)_#v(labcnt#v(cdep)):	; this is the exit of the loop (from the while part)
	banklabel
cdep--	
	endm


;**************************************************************
;***            Bank selected PIC like commands             ***
;**************************************************************
;

bcfb	macro	flag,bit
	load_f_b_l	flag, bit, 0
	endm

clr_f_b	macro	flag,bit
	bcfb	flag, bit
	endm

bsfb	macro	flag,bit
	load_f_b_l	flag, bit, 1
	endm

set_f_b	macro	flag,bit
	bsfb	flag, bit
	endm

clr_f	macro	f1
	clrfb	f1
	endm

inc_f	macro	f1
	incfb	f1,f
	endm

dec_f	macro	f1
	decfb	f1,f
	endm

clr_f16	macro	f
	bankset	f
	clrf	f
	clrf	(f+1)
	bankres	f
	endm

inc_f16	macro	f1
	bankset	f1
	movlw	.1
	addwf	f1,f
	movlw	.0
	addwfc	(f1+1),f
	bankres	f1
	endm

dec_f16	macro	f1
	bankset	f1
	movlw	.1
	subwf	f1,f
	movlw	.0
	subwfb	(f1+1),f
	bankres	f1
	endm

clr_f32	macro	f
	bankset	f
	clrf	f
	clrf	(f+1)
	clrf	(f+2)
	clrf	(f+3)
	bankres	f
	endm

inc_f32	macro	f1
	bankset	f1
	movlw	.1
	addwf	f1,f
	movlw	.0
	addwfc	(f1+1),f
	addwfc	(f1+2),f
	addwfc	(f1+3),f
	bankres	f1
	endm

dec_f32	macro	f1
	bankset	f1
	movlw	.1
	subwf	f1,f
	movlw	.0
	subwfb	(f1+1),f
	subwfb	(f1+2),f
	subwfb	(f1+3),f
	bankres	f1
	endm



;**************************************************************
;***                     8 BITS MATH CODE                   ***
;**************************************************************
; The following macros are defined in this section:
;
; add_f_f_f
; add_l_f_f
; add_f_l_f
;
; sub_f_f_f
; sub_l_f_f
; sub_f_l_f
;
; mul_f_f_f
; mul_l_f_f
; mul_f_l_f
;
; div_f_f_f
; div_l_f_f
; div_f_l_f
;
; rem_f_f_f
; rem_l_f_f
; rem_f_l_f
;
; divr_f_f_f_f
; divr_l_f_f_f
; divr_f_l_f_f


;
; f_f_add_f var1,var2,var3
;
; add_f_f_f var2,var3,var1
; add_f8_f8_f8 var2,var3,var1
;
; Register (var1) = register (var2) + register (var3)
; var1 may be equal to var2 or var3
; Destroys W
add_f_f_f	macro	f1,f2,f3
	load_w_f	f1
	addwfb	f2,W
	load_f_w	f3
	endm
add_f8_f8_f8	macro	f1,f2,f3
	add_f_f_f	f1,f2,f3
	endm
load_f_f_add_f	macro	f3,f1,f2
	add_f_f_f	f1,f2,f3
	endm
load_f8_f8_add_f8 macro	f3,f1,f2
	add_f_f_f	f1,f2,f3
	endm
f_f_add_f	macro	f3,f1,f2
	add_f_f_f	f1,f2,f3
	endm
f8_f8_add_f8	macro	f3,f1,f2
	add_f_f_f	f1,f2,f3
	endm

;
; f_l_add_f var1,lit,var2
;
; add_l_f_f lit,var2,var1
; add_l8_f8_f8 lit,var2,var1
;
; Register (var1) = literal (lit) + register (var2)
; var1 may be equal to var2
; Destroys W
add_l_f_f	macro	l,f2,f3
	movlw	l
	addwfb	f2,W
	load_f_w	f3
	endm
add_l8_f8_f8	macro	l,f2,f3
	add_l_f_f	l,f2,f3
	endm
load_f_l_add_f	macro	f3,l,f2
	add_l_f_f	l,f2,f3
	endm
load_f8_l8_add_f8 macro	f3,l,f2
	add_l_f_f	l,f2,f3
	endm
f_l_add_f	macro	f3,l,f2
	add_l_f_f	l,f2,f3
	endm
f8_l8_add_f8	macro	f3,l,f2
	add_l_f_f	l,f2,f3
	endm


;
; f_f_add_l var1,var2,lit
;
; add_f_l_f var2,lit,var1
; add_f8_l8_f8 var2,lit,var1
;
; Register (var1) = register (var2) + literal (lit)
; var1 may be equal to var2
; Destroys W
add_f_l_f	macro	f1,l,f3
	add_l_f_f	l,f1,f3
	endm
add_f8_l8_f8	macro	f1,l,f3
	add_f_l_f	f1,l,f3
	endm
load_f_f_add_l	macro	f3,f1,l
	add_f_l_f	f1,l,f3
	endm
load_f8_f8_add_l8 macro	f3,f1,l
	add_f_l_f	f1,l,f3
	endm
f_f_add_l	macro	f3,f1,l
	add_f_l_f	f1,l,f3
	endm
f8_f8_add_l8	macro	f3,f1,l
	add_f_l_f	f1,l,f3
	endm

;
; f_f_sub_f var1,var2,var3
;
; sub_f_f_f var2,var3,var1
; sub_f8_f8_f8 var2,var3,var1
;
; Register (var1) = register (var2) - register (var3)
; var1 may be equal to var2 or var3
; Destroys W
sub_f_f_f	macro	f1,f2,f3
	load_w_f	f2
	subwf_b	f1,W
	load_f_w	f3
	endm
sub_f8_f8_f8	macro	f1,f2,f3
	sub_f_f_f	f1,f2,f3
	endm
load_f_f_sub_f	macro	f3,f1,f2
	sub_f_f_f	f1,f2,f3
	endm
load_f8_f8_sub_f8 macro	f3,f1,f2
	sub_f_f_f	f1,f2,f3
	endm
f_f_sub_f	macro	f3,f1,f2
	sub_f_f_f	f1,f2,f3
	endm
f8_f8_sub_f8	macro	f3,f1,f2
	sub_f_f_f	f1,f2,f3
	endm

;
; f_l_sub_f var1,lit,var2
;
; sub_l_f_f lit,var2,var1
; sub_l8_f8_f8 lit,var2,var1
;
; Register (var1) = literal (lit) - register (var2)
; var1 may be equal to var2
; Destroys W
sub_l_f_f	macro	l,f2,f3
	load_w_f	f2
	sublw	l
	load_f_w	f3
	endm
sub_l8_f8_f8	macro	l,f2,f3
	sub_l_f_f	l,f2,f3
	endm
load_f_l_sub_f	macro	f3,l,f2
	sub_l_f_f	l,f2,f3
	endm
load_f8_l8_sub_f8 macro	f3,l,f2
	sub_l_f_f	l,f2,f3
	endm
f_l_sub_f	macro	f3,l,f2
	sub_l_f_f	l,f2,f3
	endm
f8_l8_sub_f8	macro	f3,l,f2
	sub_l_f_f	l,f2,f3
	endm


;
; f_f_sub_l var1,var2,lit
;
; sub_f_l_f var2,lit,var1
; sub_f8_l8_f8 var2,lit,var1
;
; Register (var1) = register (var2) - literal (lit)
; var1 may be equal to var2
; Destroys W
sub_f_l_f	macro	f1,l,f3
	movlw	l
	subwf_b	f1,W
	load_f_w	f3
	endm
sub_f8_l8_f8	macro	f1,l,f3
	sub_f_l_f	f1,l,f3
	endm
load_f_f_sub_l	macro	f3,f1,l
	sub_f_l_f	f1,l,f3
	endm
load_f8_f8_sub_l8 macro	f3,f1,l
	sub_f_l_f	f1,l,f3
	endm
f_f_sub_l	macro	f3,f1,l
	sub_f_l_f	f1,l,f3
	endm
f8_f8_sub_l8	macro	f3,f1,l
	sub_f_l_f	f1,l,f3
	endm

;
; f_f_mul_f var1,var2,var3
;
; mul_f_f_f var2,var3,var1
; mul_f8_f8_f8 var2,var3,var1
;
; Register (var1) = register (var2) * register (var3)
; var1 may be equal to var2 or var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ and _PPMCR3_
mul_f_f_f	macro	f1,f2,f3
	load_f_f	_PPMCR1_,f1
	load_f_f	_PPMCR2_,f2
	clrfb	f3
	for_f_l_l	_PPMCR3_,1,8
	  if_f_b_s	_PPMCR2_,0
	    add_f_f_f	_PPMCR1_,f3,f3
	  end_if
	  rrfb	_PPMCR2_,F
	  bcf	STATUS,C
	  rlfb	_PPMCR1_,F
	next_f	_PPMCR3_
	endm
mul_f8_f8_f8	macro	f1,f2,f3
	mul_f_f_f	f1,f2,f3
	endm
load_f_f_mul_f	macro	f3,f1,f2
	mul_f_f_f	f1,f2,f3
	endm
load_f8_f8_mul_f8 macro	f3,f1,f2
	mul_f_f_f	f1,f2,f3
	endm
f_f_mul_f	macro	f3,f1,f2
	mul_f_f_f	f1,f2,f3
	endm
f8_f8_mul_f8	macro	f3,f1,f2
	mul_f_f_f	f1,f2,f3
	endm

;
; f_l_mul_f var1,lit,var2
;
; mul_l_f_f lit,var2,var1
; mul_l8_f8_f8 lit,var2,var1
;
; Register (var1) = literal (lit) * register (var2)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ and _PPMCR3_
mul_l_f_f	macro	l,f2,f3
	load_f_l	_PPMCR1_,l
	load_f_f	_PPMCR2_,f2
	clrfb	f3
	for_f_l_l	_PPMCR3_,1,8
	  if_f_b_s	_PPMCR2_,0
	    add_f_f_f	_PPMCR1_,f3,f3
	  end_if
	  rrfb	_PPMCR2_,F
	  bcf	STATUS,C
	  rlfb	_PPMCR1_,F
	next_f	_PPMCR3_
	endm
mul_l8_f8_f8	macro	l,f2,f3
	mul_l_f_f	l,f2,f3
	endm
load_f_l_mul_f	macro	f3,l,f2
	mul_l_f_f	l,f2,f3
	endm
load_f8_l8_mul_f8 macro	f3,l,f2
	mul_l_f_f	l,f2,f3
	endm
f_l_mul_f	macro	f3,l,f2
	mul_l_f_f	l,f2,f3
	endm
f8_l8_mul_f8	macro	f3,l,f2
	mul_l_f_f	l,f2,f3
	endm

;
; f_f_mul_l var1,var2,lit
;
; mul_f_l_f var2,lit,var1
; mul_f8_l8_f8 var2,lit,var1
;
; Register (var1) = register (var2) * literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ and _PPMCR3_
mul_f_l_f	macro	f1,l,f3
	load_f_f	_PPMCR1_,f1
	load_f_l	_PPMCR2_,l
	clrfb	f3
	for_f_l_l	_PPMCR3_,1,8
	  if_f_b_s	_PPMCR2_,0
	    add_f_f_f	_PPMCR1_,f3,f3
	  end_if
	  rrfb	_PPMCR2_,F
	  bcf	STATUS,C
	  rlfb	_PPMCR1_,F
	next_f	_PPMCR3_
	endm
mul_f8_l8_f8	macro	f1,l,f3
	mul_f_l_f	f1,l,f3
	endm
load_f_f_mul_l	macro	f3,f1,l
	mul_f_l_f	f1,l,f3
	endm
load_f8_f8_mul_l8 macro	f3,f1,l
	mul_f_l_f	f1,l,f3
	endm
f_f_mul_l	macro	f3,f1,l
	mul_f_l_f	f1,l,f3
	endm
f8_f8_mul_l8	macro	f3,f1,l
	mul_f_l_f	f1,l,f3
	endm


;
; f_f_mul10 var1,var2
;
; mul10_f_f var2,var1
; mul10_f8_f8 var2,var1
;
; Register (var1) = register (var2) * .10
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_
mul10_f_f	macro	f1,f3
	load_f_f	_PPMCR1_,f1
	clrfb	f3
	bcf	STATUS,C
	rlfb	_PPMCR1_,F
	add_f_f_f	_PPMCR1_,f3,f3
	bcf	STATUS,C
	rlfb	_PPMCR1_,F
	bcf	STATUS,C
	rlfb	_PPMCR1_,F
	add_f_f_f	_PPMCR1_,f3,f3
	endm
mul10_f8_f8	macro	f1,f3
	mul10_f_f	f1,f3
	endm
load_f_f_mul10	macro	f3,f1
	mul10_f_f	f1,f3
	endm
load_f8_f8_mul10 macro	f3,f1
	mul10_f_f	f1,f3
	endm
f_f_mul10	macro	f3,f1
	mul10_f_f	f1,f3
	endm
f8_f8_mul10	macro	f3,f1
	mul10_f_f	f1,f3
	endm


;
; f_f_div_f var1,var2,var3
;
; div_f_f_f var2,var3,var1
; div_f8_f8_f8 var2,var3,var1
;
; Register (var1) = register (var2) / register (var3)
; var1 may be equal to var2 or var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_, _PPMCR3_ and _PPMCR4_
; Algorithm inspired by microchip (AN617)
div_f_f_f	macro	f1,f2,f3
mcnt++
	load_f_f	_PPMCR1_,f1
	load_f_f	_PPMCR2_,f2
	clrfb	_PPMCR4_
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	_PPMCR4_,F
	  load_w_f	_PPMCR2_
	  subwf_b	_PPMCR4_,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	_PPMCR4_,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	load_f_f	f3,_PPMCR1_
	endm
div_f8_f8_f8	macro	f1,f2,f3
	div_f_f_f	f1,f2,f3
	endm
load_f_f_div_f	macro	f3,f1,f2
	div_f_f_f	f1,f2,f3
	endm
load_f8_f8_div_f8 macro	f3,f1,f2
	div_f_f_f	f1,f2,f3
	endm
f_f_div_f	macro	f3,f1,f2
	div_f_f_f	f1,f2,f3
	endm
f8_f8_div_f8	macro	f3,f1,f2
	div_f_f_f	f1,f2,f3
	endm

;
; f_l_div_f var1,lit,var2
;
; div_l_f_f lit,var2,var1
; div_l8_f8_f8 lit,var2,var1
;
; Register (var1) = literal (lit) / register (var2)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_, _PPMCR3_ and _PPMCR4_
; Algorithm inspired by microchip (AN617)
div_l_f_f	macro	l,f2,f3
mcnt++
	load_f_l	_PPMCR1_,l
	load_f_f	_PPMCR2_,f2
	clrfb	_PPMCR4_
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	_PPMCR4_,F
	  load_w_f	_PPMCR2_
	  subwf_b	_PPMCR4_,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	_PPMCR4_,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	load_f_f	f3,_PPMCR1_
	endm
div_l8_f8_f8	macro	l,f2,f3
	div_l_f_f	l,f2,f3
	endm
load_f_l_div_f	macro	f3,l,f2
	div_l_f_f	l,f2,f3
	endm
load_f8_l8_div_f8 macro	f3,l,f2
	div_l_f_f	l,f2,f3
	endm
f_l_div_f	macro	f3,l,f2
	div_l_f_f	l,f2,f3
	endm
f8_l8_div_f8	macro	f3,l,f2
	div_l_f_f	l,f2,f3
	endm

;
; f_f_div_l var1,var2,lit
;
; div_f_l_f var2,lit,var1
; div_f8_l8_f8 var2,lit,var1
;
; Register (var1) = register (var2) / literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_, _PPMCR3_ and _PPMCR4_
; Algorithm inspired by microchip (AN617)
div_f_l_f	macro	f1,l,f3
mcnt++
	load_f_f	_PPMCR1_,f1
	load_f_l	_PPMCR2_,l
	clrfb	_PPMCR4_
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	_PPMCR4_,F
	  load_w_f	_PPMCR2_
	  subwf_b	_PPMCR4_,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	_PPMCR4_,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	load_f_f	f3,_PPMCR1_
	endm
div_f8_l8_f8	macro	f1,l,f3
	div_f_l_f	f1,l,f3
	endm
load_f_f_div_l	macro	f3,f1,l
	div_f_l_f	f1,l,f3
	endm
load_f8_f8_div_l8 macro	f3,f1,l
	div_f_l_f	f1,l,f3
	endm
f_f_div_l	macro	f3,f1,l
	div_f_l_f	f1,l,f3
	endm
f8_f8_div_l8	macro	f3,f1,l
	div_f_l_f	f1,l,f3
	endm

;
; f_f_rem_f var1,var2,var3
;
; rem_f_f_f var2,var3,var1
; rem_f8_f8_f8 var2,var3,var1
;
; Register (var1) = register (var2) % register (var3), the remainder of a division
; var1 may be equal to var2 or var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ and _PPMCR3_ 
; Algorithm inspired by microchip (AN617)
rem_f_f_f	macro	f1,f2,f3
mcnt++
	load_f_f	_PPMCR1_,f1
	load_f_f	_PPMCR2_,f2
	clrfb	f3
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	f3,F
	  load_w_f	_PPMCR2_
	  subwf_b	f3,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	f3,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	endm
rem_f8_f8_f8	macro	f1,f2,f3
	rem_f_f_f	f1,f2,f3
	endm
load_f_f_rem_f	macro	f3,f1,f2
	rem_f_f_f	f1,f2,f3
	endm
load_f8_f8_rem_f8 macro	f3,f1,f2
	rem_f_f_f	f1,f2,f3
	endm
f_f_rem_f	macro	f3,f1,f2
	rem_f_f_f	f1,f2,f3
	endm
f8_f8_rem_f8	macro	f3,f1,f2
	rem_f_f_f	f1,f2,f3
	endm

;
; f_l_rem_f var1,lit,var2
;
; rem_l_f_f lit,var2,var1
; rem_l8_f8_f8 lit,var2,var1
;
; Register (var1) = literal (lit) % register (var2), the remainder of a division
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ and _PPMCR3_
; Algorithm inspired by microchip (AN617)
rem_l_f_f	macro	l,f2,f3
mcnt++
	load_f_l	_PPMCR1_,l
	load_f_f	_PPMCR2_,f2
	clrfb	f3
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	f3,F
	  load_w_f	_PPMCR2_
	  subwf_b	f3,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	f3,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	endm
rem_l8_f8_f8	macro	l,f2,f3
	rem_l_f_f	l,f2,f3
	endm
load_f_l_rem_f	macro	f3,l,f2
	rem_l_f_f	l,f2,f3
	endm
load_f8_l8_rem_f8 macro	f3,l,f2
	rem_l_f_f	l,f2,f3
	endm
f_l_rem_f	macro	f3,l,f2
	rem_l_f_f	l,f2,f3
	endm
f8_l8_rem_f8	macro	f3,l,f2
	rem_l_f_f	l,f2,f3
	endm

;
; f_f_rem_l var1,var2,lit
;
; rem_f_l_f var2,lit,var1
; rem_f8_l8_f8 var2,lit,var1
;
; Register (var1) = register (var2) % literal (lit), the remainder of a division
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ and _PPMCR3_
; Algorithm inspired by microchip (AN617)
rem_f_l_f	macro	f1,l,f3
mcnt++
	load_f_f	_PPMCR1_,f1
	load_f_l	_PPMCR2_,l
	clrfb	f3
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	f3,F
	  movfb	_PPMCR2_,W
	  subwf_b	f3,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	f3,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	endm
rem_f8_l8_f8	macro	f1,l,f3
	rem_f_l_f	f1,l,f3
	endm
load_f_f_rem_l	macro	f3,f1,l
	rem_f_l_f	f1,l,f3
	endm
load_f8_f8_rem_l8 macro	f3,f1,l
	rem_f_l_f	f1,l,f3
	endm
f_f_rem_l	macro	f3,f1,l
	rem_f_l_f	f1,l,f3
	endm
f8_f8_rem_l8	macro	f3,f1,l
	rem_f_l_f	f1,l,f3
	endm

;
; f_f_f_divr_f var1,var2,var3,var4
;
; divr_f_f_f_f var3,var4,var1,var2
; divr_f8_f8_f8_f8 var3,var4,var1,var2
;
; Register (var1) = register (var2) / register (var3)
; Register (var2) = register (var2) % register (var3), the remainder of a division
; var1, var2 may be equal to var3 or var4
; Destroys W; Needs _PPMCR1_, _PPMCR2_ _PPMCR3_
; Algorithm inspired by microchip (AN617)
divr_f_f_f_f	macro	f1,f2,f3,f4
mcnt++
	load_f_f	_PPMCR1_,f1
	load_f_f	_PPMCR2_,f2
	clrfb	f4
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	f4,F
	  load_w_f	_PPMCR2_
	  subwf_b	f4,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	f4,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	load_f_f	f3,_PPMCR1_
	endm
divr_f8_f8_f8_f8	macro	f1,f2,f3,f4
	divr_f_f_f_f	f1,f2,f3,f4
	endm
load_f_f_f_divr_f macro	f3,f4,f1,f2
	divr_f_f_f_f	f1,f2,f3,f4
	endm
load_f8_f8_f8_divr_f8 macro	f3,f4,f1,f2
	divr_f_f_f_f	f1,f2,f3,f4
	endm
f_f_f_divr_f	macro	f3,f4,f1,f2
	divr_f_f_f_f	f1,f2,f3,f4
	endm
f8_f8_f8_divr_f8 macro	f3,f4,f1,f2
	divr_f_f_f_f	f1,f2,f3,f4
	endm

;
; f_f_l_divr_f var1,var2,lit,var3
;
; divr_l_f_f_f lit,var3,var1,var2
; divr_l8_f8_f8_f8 lit,var3,var1,var2
;
; Register (var1) = literal (lit) / register (var3)
; Register (var2) = literal (lit) % register (var3), the remainder of a division
; var1 or var2 may be equal to var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ and _PPMCR3_
; Algorithm inspired by microchip (AN617)
divr_l_f_f_f	macro	l,f2,f3,f4
mcnt++
	load_f_l	_PPMCR1_,l
	load_f_f	_PPMCR2_,f2
	clrfb	f4
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	f4,F
	  load_w_f	_PPMCR2_
	  subwf_b	f4,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	f4,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	load_f_f	f3,_PPMCR1_
	endm
divr_l8_f8_f8_f8	macro	l,f2,f3,f4
	divr_l_f_f_f	l,f2,f3,f4
	endm
load_f_f_l_divr_f macro	f3,f4,l,f2
	divr_l_f_f_f	l,f2,f3,f4
	endm
load_f8_f8_l8_divr_f8 macro	f3,f4,l,f2
	divr_l_f_f_f	l,f2,f3,f4
	endm
f_f_l_divr_f	macro	f3,f4,l,f2
	divr_l_f_f_f	l,f2,f3,f4
	endm
f8_f8_l8_divr_f8 macro	f3,f4,l,f2
	divr_l_f_f_f	l,f2,f3,f4
	endm

;
; f_f_f_divr_l var1,var2,var3,lit
;
; divr_f_l_f_f var3,lit,var1,var2
; divr_f8_l8_f8_f8 var3,lit,var1,var2
;
; Register (var1) = register (var3) / literal (lit)
; Register (var2) = register (var3) % literal (lit), the remainder of a division
; var1 or var2 may be equal to var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ and _PPMCR3_
; Algorithm inspired by microchip (AN617)
divr_f_l_f_f	macro	f1,l,f3,f4
mcnt++
	load_f_f	_PPMCR1_,f1
	load_f_l	_PPMCR2_,l
	clrfb	f4
	for_f_l_l	_PPMCR3_,1,8
	  rlfb	_PPMCR1_,W
	  rlfb	f4,F
	  load_w_f	_PPMCR2_
	  subwf_b	f4,F
	  gotoif_f_b_s	STATUS,C,j_1_#v(mcnt)
	  addwfb	f4,F
	  bcf	STATUS,C
j_1_#v(mcnt):	banklabel
	  rlfb	_PPMCR1_, F
	next_f	_PPMCR3_
	load_f_f	f3,_PPMCR1_
	endm
divr_f8_l8_f8_f8	macro	f1,l,f3,f4
	divr_f_l_f_f	f1,l,f3,f4
	endm
load_f_f_f_divr_l macro	f3,f4,f1,l
	divr_f_l_f_f	f1,l,f3,f4
	endm
load_f8_f8_f8_divr_l8 macro	f3,f4,f1,l
	divr_f_l_f_f	f1,l,f3,f4
	endm
f_f_f_divr_l	macro	f3,f4,f1,l
	divr_f_l_f_f	f1,l,f3,f4
	endm
f8_f8_f8_divr_l8 macro	f3,f4,f1,l
	divr_f_l_f_f	f1,l,f3,f4
	endm


;**************************************************************
;***                  16/32 BITS MATH CODE                  ***
;**************************************************************
; The following macros are defined in this section:
;
; add_f16_f16_f16 var1,var2,var3
; add_l16_f16_f16 var1,var2,var3
; add_f16_l16_f16 var1,lit,var3
;
; add_f32_f32_f32 var1,var2,var3
; add_l32_f32_f32 var1,var2,var3
; add_f32_l32_f32 var1,lit,var3
;
; sub_f16_l16_f16 var1,lit,var3
; sub_f16_f16_f16 var1,var2,var3
; sub_f32_f32_f32 var1,var2,var3
;
; mul_f16_l16_f16 var1,lit,var3
; mul_f16_f16_f16 var1,var2,var3
;
; mul_f32_f32_f32 var1,var2,var3
;
; divr_f16_l16_f16_f16 var1,var2,var3,var4
;
; divr_f32_l32_f32_f32 var1,var2,var3,var4
;

;
; f16_l16_add_f16 var1,lit,var2
;
; add_l16_f16_f16 lit,var2,var1
;
; Register (var1) = literal (lit) + register (var2)
; var1 may be equal to var2
; Destroys W
add_l16_f16_f16	macro	l,f1,f3
	if (f1 != f3) 
	  load_f16_f16	f3,f1
	endif
	if ((l % 0x100) != 0)
	  load_w_l	(l % 0x100)
	addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	endif
	if ((l >> .8) != 0)
	  load_w_l	(l >> .8)
	  addwfb	f3+1,F
	endif
	endm
f16_l16_add_f16	macro	f3,l,f1
	add_l16_f16_f16	l,f1,f3
	endm
load_f16_l16_add_f16 macro	f3,l,f1
	add_l16_f16_f16	l,f1,f3
	endm

;
; f16_f16_add_l16 var1,var2,lit
;
; add_f16_l16_f16 var2,lit,var1
;
; Register (var1) = register (var2) + literal (lit) 
; var1 may be equal to var2
; Destroys W
add_f16_l16_f16	macro	f1,l,f3
	add_l16_f16_f16	l,f1,f3
	endm
f16_f16_add_l16	macro	f3,f1,l
	add_l16_f16_f16	l,f1,f3
	endm
load_f16_f16_add_l16 macro	f3,f1,l
	add_l16_f16_f16	l,f1,f3
	endm

; add_f16_f_f16 var2,var3,var1
;
; Register (var1) = register (var2) + register (var3)
; var1 may be equal to var23
; Destroys W
add_f16_f_f16	macro	f1,f2,f3
	if (f1 != f3)
	  if (f2 != f3)
	   load_f_f	f3,f2
	   clr_f	(f3+1)
	  endif
	  load_w_f	f1
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	  load_w_f	f1+1
	  addwfb	f3+1,F
	else
	  load_w_f	f2
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	endif
	endm
f16_f16_add_f	macro	f3,f1,f2
	add_f16_f_f16	f1,f2,f3
	endm
load_f16_f16_add_f macro	f3,f1,f2
	add_f16_f_f16	f1,f2,f3
	endm


;
; f16_f16_add_f16 var1,var2,var3
;
; add_f16_f16_f16 var2,var3,var1
;
; Register (var1) = register (var2) + register (var3)
; var1 may be equal to var2 or var3
; Destroys W
add_f16_f16_f16	macro	f1,f2,f3
	if (f1 != f3)
	  if (f2 != f3)
	   load_f16_f16	f3,f2
	  endif
	  load_w_f	f1
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	  load_w_f	f1+1
	  addwfb	f3+1,F
	else
	  load_w_f	f2
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	  load_w_f	f2+1
	  addwfb	f3+1,F
	endif
	endm
f16_f16_add_f16	macro	f3,f1,f2
	add_f16_f16_f16	f1,f2,f3
	endm
load_f16_f16_add_f16 macro	f3,f1,f2
	add_f16_f16_f16	f1,f2,f3
	endm

;
; f32_f32_add_f32 var1,var2,var3
;
; add_f32_f32_f32 var2,var3,var1
;
; Register (var1) = register (var2) + register (var3)
; var1 may be equal to var2 or var3
; Destroys W
add_f32_f32_f32	macro	f1,f2,f3
	if (f1 != f3)
	  if (f2 != f3)
	   load_f32_f32	f3,f2
	  endif
	  load_w_f	f1
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+2,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	  load_w_f	f1+1
	    addwfb	f3+1,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+2,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	  load_w_f	f1+2
	  addwfb	f3+2,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	  load_w_f	f1+3
	  addwfb	f3+3,F
	else
	  load_w_f	f2
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+2,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	  load_w_f	f2+1
	    addwfb	f3+1,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+2,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	  load_w_f	f2+2
	  addwfb	f3+2,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	  load_w_f	f2+3
	  addwfb	f3+3,F
	endif
	endm
f32_f32_add_f32	macro	f3,f1,f2
	add_f32_f32_f32	f1,f2,f3
	endm
load_f32_f32_add_f32 macro	f3,f1,f2
	add_f32_f32_f32	f1,f2,f3
	endm

;
; f32_f32_add_f316 var1,var2,var3
;
; add_f32_f32_f16 var2,var3,var1
;
; Register (var1) = register (var2) + register (var3)
; var1 may be equal to var2 or var3
; Destroys W
add_f32_f16_f32	macro	f1,f2,f3
	if (f2 != f3)
	  if (f1 != f3)
	   load_f16_f16	f3,f1
	  endif
	  load_w_f	f2
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+2,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	  load_w_f	f2+1
	    addwfb	f3+1,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+2,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	else
	 error "16 bit second variable is also used as 32 bit third variable"
	endif
	endm
f32_f32_add_f16	macro	f3,f1,f2
	add_f32_f16_f32	f1,f2,f3
	endm
load_f32_f32_add_f16 macro	f3,f1,f2
	add_f32_f16_f32	f1,f2,f3
	endm

;
; f32_l32_add_f32 var1,lit,var2
;
; add_l32_f32_f32 lit,var2,var1
;
; Register (var1) = literal (lit) + register (var2)
; var1 may be equal to var2
; Destroys W
add_l32_f32_f32	macro	l,f1,f3
	add_f32_l32_f32	f1,l,f3
	endm
f32_l32_add_f32	macro	f3,l,f1
	add_f32_l32_f32	f1,l,f3
	endm
load_f32_l32_add_f32 macro	f3,l,f1
	add_f32_l32_f32	f1,l,f3
	endm


;
; f32_f32_add_l32 var1,var2,lit
;
; add_f32_l32_f32 var2,lit,var1
;
; Register (var1) = register (var2) + literal (lit) 
; var1 may be equal to var2
; Destroys W
add_f32_l32_f32	macro	f1,l,f3
	if (f1 != f3) 
	  load_f32_f32	f3,f1
	endif
	if ((l % 0x100) != 0)
	  load_w_l	(l % 0x100)
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+1,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+2,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	endif
	if (((l >> .8) % 0x100) != 0)
	  load_w_l	((l >> .8) % 0x100)
	  addwfb	f3+1,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+2,F
	  end_if
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	endif
	if (((l >> .16) % 0x100) != 0)
	  load_w_l	((l >> .16) % 0x100)
	  addwfb	f3,F
	  if_f_b_s	STATUS,C
	    movlw	.1
	    addwfb	f3+3,F
	  end_if
	endif
	if ((l >> .24) != 0)
	  load_w_l	(l >> .24)
	    addwfb	f3+3,F
	endif
	endm
f32_f32_add_l32	macro	f3,f1,l
	add_f32_l32_f32	f1,l,f3
	endm
load_f32_f32_add_l32 macro	f3,f1,l
	add_f32_l32_f32	f1,l,f3
	endm

;
; f16_f16_sub_l16 var1,var2,lit
;
; sub_f16_l16_f16 var2,lit,var1
;
; Register (var1) = register (var2) - literal (lit)
; var1 may be equal to var2
; Destroys W
sub_f16_l16_f16	macro	f1,l,f3
	if (f1 != f3) 
	  load_f16_f16	f3,f1
	endif
	if ((l % 0x100) != 0)
	  movlw	(l % 0x100)
	  subwf_b	f3,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	f3+1,F
	  end_if
	endif
	if ((l >> .8) != 0)
	  movlw	(l >> .8)
	  subwf_b	f3+1,F
	endif
	endm
f16_f16_sub_l16	macro	f3,f1,l
	sub_f16_l16_f16	f1,l,f3
	endm
load_f16_f16_sub_l16 macro	f3,f1,l
	sub_f16_l16_f16	f1,l,f3
	endm

;
; f16_l16_sub_f16 var1,lit,var2
;
; sub_l16_f16_f16 lit,var2,var1
;
; Register (var1) = literal (lit)- register (var2)
; var1 may be equal to var2
; Destroys W
sub_l16_f16_f16	macro	l,f2,f3
	if (f2 != f3) 
	  load_f16_f16	f3,f2
	endif
	load_w_f	f3
	sublw	(l % 0x100)
	load_f_w	f3
	if_f_b_c	STATUS,C
	  ;load_w_f	f3+1
	  ;sublw	.1
	  ;load_f_w	f3+1
	  movlw	.1
	  addwfb	f3+1,F
	end_if

	load_w_f	f3+1
	sublw	(l >> .8)
	load_f_w	f3+1
	endm
f16_l16_sub_f16	macro	f3,l,f2
	sub_l16_f16_f16	l,f2,f3
	endm
load_f16_l16_sub_f16 macro	f3,l,f2
	sub_l16_f16_f16	l,f2,f3
	endm

; 
; f16_f16_sub_f16 var1,var2,var3
;
; sub_f16_f16_f16 var2,var3,var1
;
; Register (var1) = register (var2) - register (var3)
; var1 may be equal to var2, var1 may NOT be equal to var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ 
sub_f16_f16_f16	macro	f1,f2,f3
	if (f2 != f3)
	  if (f1 != f3)
	   load_f16_f16	f3,f1
	  endif
	  load_w_f	f2
	  subwf_b	f3,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	f3+1,F
	  end_if
	  load_w_f	f2+1
	  subwf_b	f3+1,F
	else
	  load_f16_f16	_PPMCR1_,f1
	  load_w_f	f2
	  subwf_b	_PPMCR1_,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	_PPMCR2_,F
	  end_if
	  load_w_f	f2+1
	  subwf_b	_PPMCR1_,F
	  load_f16_f16	f3,_PPMCR1_
	endif
	endm
f16_f16_sub_f16	macro	f3,f1,f2
	sub_f16_f16_f16	f1,f2,f3
	endm
load_f16_f16_sub_f16 macro	f3,f1,f2
	sub_f16_f16_f16	f1,f2,f3
	endm

; 
; f32_l32_sub_f32 var1,lit,var2
;
; sub_l32_f32_f32 lit,var2,var1
;
; Register (var1) = literal (lit) - register (var2)
; var1 may be equal to var2
; Destroys W
sub_l32_f32_f32	macro	l,f2,f3
	if (f2 != f3) 
	  load_f32_f32	f3,f2
	endif

	load_w_f	f3
	sublw	(l % 0x100)
	load_f_w	f3
	if_f_b_c	STATUS,C
	  ;load_w_f	f3+1
	  ;sublw	.1
	  ;load_f_w	f3+1
	  movlw	.1
	  addwfb	f3+1,F
	end_if
	if_f_b_c	STATUS,C
	  ;load_w_f	f3+2
	  ;sublw	.1
	  ;load_f_w	f3+2
	  movlw	.1
	  addwfb	f3+2,F
	end_if
	if_f_b_c	STATUS,C
	  ;load_w_f	f3+3
	  ;sublw	.1
	  ;load_f_w	f3+3
	  movlw	.1
	  addwfb	f3+3,F
	end_if

	load_w_f	f3+1
	sublw	((l >> .8) % 0x100)
	load_f_w	f3+1
	if_f_b_c	STATUS,C
	  ;load_w_f	f3+2
	  ;sublw	.1
	  ;load_f_w	f3+2
	  movlw	.1
	  addwfb	f3+2,F
	end_if
	if_f_b_c	STATUS,C
	  ;load_w_f	f3+3
	  ;sublw	.1
	  ;load_f_w	f3+3
	  movlw	.1
	  addwfb	f3+3,F
	end_if

	load_w_f	f3+2
	sublw	((l >> .16) % 0x100)
	load_f_w	f3+2
	if_f_b_c	STATUS,C
	  ;load_w_f	f3+3
	  ;sublw	.1
	  ;load_f_w	f3+3
	  movlw	.1
	  addwfb	f3+3,F
	end_if

	load_w_f	f3+3
	sublw	(l >> .24)
	load_f_w	f3+3

	endm
f32_l32_sub_f32	macro	f3,l,f2
	sub_l32_f32_f32	l,f2,f3
	endm
load_f32_l32_sub_f32 macro	f3,l,f2
	sub_l32_f32_f32	l,f2,f3
	endm

;
; f32_f32_sub_l32 var1,var2,lit
;
; sub_f32_l32_f32 var2,lit,var1
;
; Register (var1) = register (var2) - literal (lit)
; var1 may be equal to var2
; Destroys W
sub_f32_l32_f32	macro	f1,l,f3
	if (f1 != f3)
	  load_f32_f32	f3,f1
	endif
	movlw	(l % 0x100)
	subwf_b	f3,F
	if_f_b_c	STATUS,C
	  movlw	.1
	  subwf_b	f3+1,F
	end_if
	if_f_b_c	STATUS,C
	  movlw	.1
	  subwf_b	f3+2,F
	end_if
	if_f_b_c	STATUS,C
	  movlw	.1
	  subwf_b	f3+3,F
	end_if
	movlw	((l >> .8) % 0x100)
	subwf_b	f3+1,F
	if_f_b_c	STATUS,C
	  movlw	.1
	  subwf_b	f3+2,F
	end_if
	if_f_b_c	STATUS,C
	  movlw	.1
	  subwf_b	f3+3,F
	end_if
	movlw	((l >> .16) % 0x100)
	subwf_b	f3+2,F
	if_f_b_c	STATUS,C
	  movlw	.1
	  subwf_b	f3+3,F
	end_if
	movlw	(l >> .24)
	subwf_b	f3+3,F
	endm
f32_f32_sub_l32	macro	f3,f1,l
	sub_f32_l32_f32	f1,l,f3
	endm
load_f32_f32_sub_l32 macro	f3,f1,l
	sub_f32_l32_f32	f1,l,f3
	endm

;
; f32_f32_sub_f32 var1,var2,var3
;
; sub_f32_f32_f32 var2,var3,var1
;
; Register (var1) = register (var2) - register (var3)
; var1 may be equal to var2, var1 may NOT be equal to var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR4_
sub_f32_f32_f32	macro	f1,f2,f3
	if (f2 != f3)
	  if (f1 != f3)
	   load_f32_f32	f3,f1
	  endif
	  load_w_f	f2
	  subwf_b	f3,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	f3+1,F
	  end_if
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	f3+2,F
	  end_if
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	f3+3,F
	  end_if
	  load_w_f	f2+1
	    subwf_b	f3+1,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	f3+2,F
	  end_if
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	f3+3,F
	  end_if
	  load_w_f	f2+2
	  subwf_b	f3+2,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	f3+3,F
	  end_if
	  load_w_f	f2+3
	  subwf_b	f3+3,F
	else
	  load_f32_f32	_PPMCR1_,f1
	  load_w_f	f2
	  subwf_b	_PPMCR1_,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	_PPMCR2_,F
	  end_if
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	_PPMCR3_,F
	  end_if
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	_PPMCR4_,F
	  end_if
	  load_w_f	f2+2
	  subwf_b	_PPMCR2_,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	_PPMCR3_,F
	  end_if
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	_PPMCR4_,F
	  end_if
	  load_w_f	f2+3
	  subwf_b	_PPMCR3_,F
	  if_f_b_c	STATUS,C
	    movlw	.1
	    subwf_b	_PPMCR4_,F
	  end_if
	  load_w_f	f2+4
	  subwf_b	_PPMCR4_,F
	  load_f16_f16	f3,_PPMCR1_
	endm
f32_f32_sub_f32	macro	f3,f1,f2
	sub_f32_f32_f32	f1,f2,f3
	endm
load_f32_f32_sub_f32 macro	f3,f1,f2
	sub_f32_f32_f32	f1,f2,f3
	endm

;
; f16_f8_mul_l8 var1,var2,lit
;
; mul_f8_l8_f16 var2,lit,var1
;
; Register (var1) = register (var2) * literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR6_
mul_f8_l8_f16	macro	f1,l,f3
	if (f1 != f3)
	  load_f16_f8	_PPMCR1_,f1
	  load_f_l	_PPMCR3_,l
	  clrfb	f3
	  clrfb	f3+1
	  for_f_l_l	_PPMCR4_,1,.8
	    if_f_b_s	_PPMCR3_,0
	      add_f16_f16_f16	_PPMCR1_, f3, f3
	    end_if
	    rrf	_PPMCR3_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	  next
	else
	  load_f16_f8	_PPMCR1_,f1
	  load_f_l	_PPMCR3_,l
	  clrf	_PPMCR4_
	  clrf	_PPMCR5_
	  for_f_l_l	_PPMCR6_,1,.8
	    if_f_b_s	_PPMCR3_,0
	      add_f16_f16_f16	_PPMCR1_, _PPMCR4_, _PPMCR4_
	    end_if
	    rrf	_PPMCR3_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	  next
	  load_f16_f16	f3, _PPMCR4_
	endif
	endm
f16_f8_mul_l8	macro	f3,f1,l
	mul_f8_l8_f16	f1,l,f3
	endm
load_f16_f8_mul_l8 macro	f3,f1,l
	mul_f8_l8_f16	f1,l,f3
	endm

;
; f16_l8_mul_f8 var1,var2,lit
;
; mul_l8_f8_f16 var2,lit,var1
;
; Register (var1) = register (var2) * literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
mul_l8_f8_f16	macro	l,f1,f3
	mul_f8_l8_f16	f1,l,f3
	endm
f16_l8_mul_f8	macro	f3,l,f1
	mul_f8_l8_f16	f1,l,f3
	endm
load_f16_l8_mul_f8 macro	f3,l,f1
	mul_f8_l8_f16	f1,l,f3
	endm

;
; f16_f8_mul_f8 var1,var2,var3
;
; mul_f8_f8_f16 var2,var3,var1
;
; Register (var1) = register (var2) * register (var3)
; var3 may be equal to var1 or var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
mul_f8_f8_f16	macro	f1,f2,f3
	if ((f1 != f3) && (f2 != f3))
	  load_f16_f8	_PPMCR1_,f1
	  load_f_f	_PPMCR3_,f2
	  clrfb	f3
	  clrfb	f3+1
	  for_f_l_l	_PPMCR4_,1,.8
	    if_f_b_s	_PPMCR3_,0
	      add_f16_f16_f16	_PPMCR1_, f3, f3
	    end_if
	    rrf	_PPMCR3_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	  next
	else
	  load_f16_f8	_PPMCR1_,f1
	  load_f_f	_PPMCR3_,f2
	  clrf	_PPMCR4_
	  clrf	_PPMCR5_
	  for_f_l_l	_PPMCR6_,1,.8
	    if_f_b_s	_PPMCR3_,0
	      add_f16_f16_f16	_PPMCR1_, _PPMCR4_, _PPMCR4_
	    end_if
	    rrf	_PPMCR3_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	  next
	  load_f16_f16	f3, _PPMCR4_
	endif
	endm
f16_f8_mul_f8	macro	f3,f1,f2
	mul_f8_f8_f16	f1,f2,f3
	endm
load_f16_f8_mul_f8 macro	f3,f1,f2
	mul_f8_f8_f16	f1,f2,f3
	endm

;
; f16_f16_mul_l16 var1,var2,lit
;
; mul_f16_l16_f16 var2,lit,var1
;
; Register (var1) = register (var2) * literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
mul_f16_l16_f16	macro	f1,l,f3
	if (f1 != f3)
	  load_f16_f16	_PPMCR1_,f1
	  load_f16_l16	_PPMCR3_,l
	  clrfb	f3
	  clrfb	f3+1
	  for_f_l_l	_PPMCR5_,1,.16
	    if_f_b_s	_PPMCR3_,0
	      add_f16_f16_f16	_PPMCR1_, f3, f3
	    end_if
	    rrf	_PPMCR4_,F
	    rrf	_PPMCR3_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	  next
	else
	  load_f16_f16	_PPMCR1_,f1
	  load_f16_l16	_PPMCR3_,l
	  clrf	_PPMCR5_
	  clrf	_PPMCR6_
	  for_f_l_l	_PPMCR7_,1,.16
	    if_f_b_s	_PPMCR3_,0
	      add_f16_f16_f16	_PPMCR1_, _PPMCR5_, _PPMCR5_
	    end_if
	    rrf	_PPMCR4_,F
	    rrf	_PPMCR3_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	  next
	  load_f16_f16	f3, _PPMCR5_
	endif
	endm
f16_f16_mul_l16	macro	f3,f1,l
	mul_f16_l16_f16	f1,l,f3
	endm
load_f16_f16_mul_l16 macro	f3,f1,l
	mul_f16_l16_f16	f1,l,f3
	endm

;
; f16_f16_mul10 var1,var2
;
; mul10_f16_f16 var2,var1
;
; Register (var1) = register (var2) * .10
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_
mul10_f16_f16	macro	f1,f3
	load_f16_f16	_PPMCR1_,f1
	clrfb	f3
	clrfb	f3+1
	bcf	STATUS,C
	rlfb	_PPMCR1_,F
	rlfb	_PPMCR2_,F
	add_f16_f16_f16	_PPMCR1_, f3, f3
	bcf	STATUS,C
	rlfb	_PPMCR1_,F
	rlfb	_PPMCR2_,F
	bcf	STATUS,C
	rlfb	_PPMCR1_,F
	rlfb	_PPMCR2_,F
	add_f16_f16_f16	_PPMCR1_, f3, f3
	endm
f16_f16_mul10	macro	f3,f1
	mul10_f16_l16_f16	f1,f3
	endm
load_f16_f16_mul10 macro	f3,f1
	mul10_f16_l16_f16	f1,f3
	endm

;
; f16_l16_mul_f16 var1,var2,lit
;
; mul_l16_f16_f16 var2,lit,var1
;
; Register (var1) = register (var2) * literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
mul_l16_f16_f16	macro	l,f1,f3
	mul_f16_l16_f16	f1,l,f3
	endm
f16_l16_mul_f16	macro	f3,l,f1
	mul_f16_l16_f16	f1,l,f3
	endm
load_f16_l16_mul_f16 macro	f3,l,f1
	mul_f16_l16_f16	f1,l,f3
	endm

;
; f16_f16_mul_f16 var1,var2,var3
;
; mul_f16_f16_f16 var2,var3,var1
;
; Register (var1) = register (var2) * register (var3)
; var1 may be equal to var2 or var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
mul_f16_f16_f16	macro	f1,f2,f3
	if ((f1 != f3) && (f2 != f3))
	  load_f16_f16	_PPMCR1_,f1
	  load_f16_f16	_PPMCR3_,f2
	  clrfb	f3
	  clrfb	f3+1
	  for_f_l_l	_PPMCR5_,1,.16
	    if_f_b_s	_PPMCR3_,0
	      add_f16_f16_f16	_PPMCR1_, f3, f3
	    end_if
	    rrf	_PPMCR4_,F
	    rrf	_PPMCR3_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	  next
	else
	  load_f16_f16	_PPMCR1_,f1
	  load_f16_f16	_PPMCR3_,f2
	  clrf	_PPMCR5_
	  clrf	_PPMCR6_
	  for_f_l_l	_PPMCR7_,1,.16
	    if_f_b_s	_PPMCR3_,0
	      add_f16_f16_f16	_PPMCR1_, _PPMCR5_, _PPMCR5_
	    end_if
	    rrf	_PPMCR4_,F
	    rrf	_PPMCR3_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	  next
	  load_f16_f16	f3, _PPMCR5_
	endif
	endm
f16_f16_mul_f16	macro	f3,f1,f2
	mul_f16_f16_f16	f1,f2,f3
	endm
load_f16_f16_mul_f16 macro	f3,f1,f2
	mul_f16_f16_f16	f1,f2,f3
	endm

;
; f32_f16_mul_l16 var1,var2,lit
;
; mul_f16_l16_f32 var2,lit,var1
;
; Register (var1) = register (var2) * literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
mul_f16_l16_f32	macro	f1,l,f3
	ifndef _PPMCR2_
	  messg "_PPMCR2_ not declared"
	endif
	load_f32_f16	_PPMCR1_,f1
	load_f16_l16	_PPMCR5_,l
	clrfb	f3
	clrfb	f3+1
	clrfb	f3+2
	clrfb	f3+3
	for_f_l_l	_PPMCR7_,1,.16
	  if_f_b_s	_PPMCR5_,0
	    add_f32_f32_f32	_PPMCR1_, f3, f3
	  end_if
	  rrf	_PPMCR6_,F
	  rrf	_PPMCR5_,F
	  bcf	STATUS,C
	  rlf	_PPMCR1_,F
	  rlf	_PPMCR2_,F
	  rlf	_PPMCR3_,F
	  rlf	_PPMCR4_,F
	next
	endm
f32_f16_mul_l16	macro	f3,f1,l
	mul_f16_l16_f32	f1,l,f3
	endm
load_f32_f16_mul_l16 macro	f3,f1,l
	mul_f16_l16_f32	f1,l,f3
	endm

;
; f32_l16_mul_f16 var1,lit,var2
;
; mul_l16_f16_f32 lit,var2,var1
;
; Register (var1) = literal (lit) * register (var2)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
mul_l16_f16_f32	macro	l,f1,f3
	mul_f16_l16_f32	f1,l,f3
	endm
f32_l16_mul_f16	macro	f3,l,f1
	mul_f16_l16_f32	f1,l,f3
	endm
load_f32_l16_mul_f16 macro	f3,l,f1
	mul_f16_l16_f32	f1,l,f3
	endm

;
; f32_f16_mul_f16 var1,var2,var3
;
; mul_f16_f16_f32 var2,var3,var1
;
; Register (var1) = register (var2) * register (var3)
; var1 may be equal to var2 or var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
mul_f16_f16_f32	macro	f1,f2,f3
	ifndef _PPMCR2_
	  messg "_PPMCR2_ not declared"
	endif
	load_f32_f16	_PPMCR1_,f1
	load_f16_f16	_PPMCR5_,f2
	clrfb	f3
	clrfb	f3+1
	clrfb	f3+2
	clrfb	f3+3
	for_f_l_l	_PPMCR7_,1,.16
	  if_f_b_s	_PPMCR5_,0
	    add_f32_f32_f32	_PPMCR1_, f3, f3
	  end_if
	  rrf	_PPMCR6_,F
	  rrf	_PPMCR5_,F
	  bcf	STATUS,C
	  rlf	_PPMCR1_,F
	  rlf	_PPMCR2_,F
	  rlf	_PPMCR3_,F
	  rlf	_PPMCR4_,F
	next
	endm
f32_f16_mul_f16	macro	f3,f1,f2
	mul_f16_f16_f32	f1,f2,f3
	endm
load_f32_f16_mul_f16 macro	f3,f1,f2
	mul_f16_f16_f32	f1,f2,f3
	endm

;
; f32_f32_mul_l32 var1,var2,lit
;
; mul_f32_l32_f32 var2,lit,var1
;
; Register (var1) = register (var2) * literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
mul_f32_l32_f32	macro	f1,l,f3
	if (f1 != f3)
	  load_f32_f32	_PPMCR1_,f1
	  load_f32_l32	_PPMCR5_,l
	  clrfb	f3
	  clrfb	f3+1
	  clrfb	f3+2
	  clrfb	f3+3
	  for_f_l_l	_PPMCR9_,1,.32
	    if_f_b_s	_PPMCR5_,0
	      add_f32_f32_f32	_PPMCR1_, f3, f3
	    end_if
	    rrf	_PPMCR8_,F
	    rrf	_PPMCR7_,F
	    rrf	_PPMCR6_,F
	    rrf	_PPMCR5_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	    rlf	_PPMCR3_,F
	    rlf	_PPMCR4_,F
	  next
	else
	  load_f32_f32	_PPMCR1_,f1
	  load_f32_l32	_PPMCR5_,l
	  clrf	_PPMCR9_
	  clrf	_PPMCR10_
	  clrf	_PPMCR11_
	  clrf	_PPMCR12_
	  for_f_l_l	_PPMCR13_,1,.32
	    if_f_b_s	_PPMCR5_,0
	      add_f32_f32_f32	_PPMCR1_, _PPMCR9_, _PPMCR9_
	    end_if
	    rrf	_PPMCR8_,F
	    rrf	_PPMCR7_,F
	    rrf	_PPMCR6_,F
	    rrf	_PPMCR5_,F
	    bcf	STATUS,C
	    rlf	_PPMCR1_,F
	    rlf	_PPMCR2_,F
	    rlf	_PPMCR3_,F
	    rlf	_PPMCR4_,F
	  next
	  load_f32_f32	f3, _PPMCR9_
	endif
	endm
f32_f32_mul_l32	macro	f3,f1,l
	mul_f32_l32_f32	f1,l,f3
	endm
load_f32_f32_mul_l32 macro	f3,f1,l
	mul_f32_l32_f32	f1,l,f3
	endm

;
; f32_f32_mul10 var1,var2
;
; mul10_f32_f32 var2,var1
;
; Register (var1) = register (var2) * .10
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR4_
mul10_f32_f32	macro	f1,f3
	load_f32_f32	_PPMCR1_,f1
	clrfb	f3
	clrfb	f3+1
	clrfb	f3+2
	clrfb	f3+3
	bcf	STATUS,C
	rlf	_PPMCR1_,F
	rlf	_PPMCR2_,F
	rlf	_PPMCR3_,F
	rlf	_PPMCR4_,F
	add_f32_f32_f32	_PPMCR1_, f3, f3
	bcf	STATUS,C
	rlf	_PPMCR1_,F
	rlf	_PPMCR2_,F
	rlf	_PPMCR3_,F
	rlf	_PPMCR4_,F
	bcf	STATUS,C
	rlf	_PPMCR1_,F
	rlf	_PPMCR2_,F
	rlf	_PPMCR3_,F
	rlf	_PPMCR4_,F
	add_f32_f32_f32	_PPMCR1_, f3, f3
	endm
f32_f32_mul10	macro	f3,f1
	mul10_f32_f32	f1,f3
	endm
load_f32_f32_mul10 macro	f3,f1
	mul10_f32_f32	f1,f3
	endm

;
; f32_l32_mul_f32 var1,lit,var2
;
; mul_l32_f32_f32 lit,var2,var1
;
; Register (var1) = register (var2) * literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
mul_l32_f32_f32	macro	l,f1,f3
	mul_f32_l32_f32	f1,l,f3
	endm
f32_l32_mul_f32	macro	f3,l,f1
	mul_f32_l32_f32	f1,l,f3
	endm
load_f32_l32_mul_f32 macro	f3,l,f1
	mul_f32_l32_f32	f1,l,f3
	endm

;
; f32_f32_mul_f32 var1,var2,var3
;
; mul_f32_f32_f32 var2,var3,var1
;
; Register (var1) = register (var2) * register (var3)
; var1 may be equal to var1 or var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
mul_f32_f32_f32	macro	f1,f2,f3
	if ((f1 != f3) && (f2 != f3))
	  load_f32_f32	_PPMCR1_,f1
	  load_f32_f32	_PPMCR5_,f2
	  clrfb	f3
	  clrfb	f3+1
	  clrfb	f3+2
	  clrfb	f3+3
	  for_f_l_l	_PPMCR9_,1,.32
	    if_f_b_s	_PPMCR5_,0
	      add_f32_f32_f32	_PPMCR1_, f3, f3
	    end_if
	    rrfb	_PPMCR8_,F
	    rrfb	_PPMCR7_,F
	    rrfb	_PPMCR6_,F
	    rrfb	_PPMCR5_,F
	    bcf	STATUS,C
	    rlfb	_PPMCR1_,F
	    rlfb	_PPMCR2_,F
	    rlfb	_PPMCR3_,F
	    rlfb	_PPMCR4_,F
	  next
	else
	  load_f32_f32	_PPMCR1_,f1
	  load_f32_f32	_PPMCR5_,f2
	  clrfb	_PPMCR9_
	  clrfb	_PPMCR10_
	  clrfb	_PPMCR11_
	  clrfb	_PPMCR12_
	  for_f_l_l	_PPMCR13_,1,.32
	    if_f_b_s	_PPMCR5_,0
	      add_f32_f32_f32	_PPMCR1_, _PPMCR9_, _PPMCR9_
	    end_if
	    rrfb	_PPMCR8_,F
	    rrfb	_PPMCR7_,F
	    rrfb	_PPMCR6_,F
	    rrfb	_PPMCR5_,F
	    bcf	STATUS,C
	    rlfb	_PPMCR1_,F
	    rlfb	_PPMCR2_,F
	    rlfb	_PPMCR3_,F
	    rlfb	_PPMCR4_,F
	  next
	  load_f32_f32	f3, _PPMCR9_
	endif
	endm
f32_f32_mul_f32	macro	f3,f1,f2
	mul_f32_f32_f32	f1,f2,f3
	endm
load_f32_f32_mul_f32	macro	f3,f1,f2
	mul_f32_f32_f32	f1,f2,f3
	endm

;
; f16_l16_div_f16 var1,lit,var2
;
; div_l16_f16_f16 lit,var2,var1
;
; Register (var1) = literal (lit) / register (var2)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
; Algorithm inspired by microchip (AN617)
div_l16_f16_f16	macro	l,f2,f3
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_l16	_PPMCR1_,l
	load_f16_f16	_PPMCR3_,f2
	clrfb	_PPMCR5_
	clrfb	_PPMCR6_
	for_f_l_l	_PPMCR7_,.1,.16
	  rlf	_PPMCR2_,W
	  rlf	_PPMCR5_,F
	  rlf	_PPMCR6_,F
	  if_f16_ge_f16	_PPMCR5_,_PPMCR3_
	    sub_f16_f16_f16	_PPMCR5_,_PPMCR3_,_PPMCR5_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	next
	load_f16_f16	f3,_PPMCR1_
	endm
f16_l16_div_f16	macro	f3,l,f2
	div_l16_f16_f16	l,f2,f3
	endm
load_f16_l16_div_f16 macro	f3,l,f2
	div_l16_f16_f16	l,f2,f3
	endm

;
; f16_f16_div_l16 var1,var2,lit
;
; div_f16_l16_f16 var2,lit,var1
;
; Register (var1) = register (var2) / literal (lit)
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
; Algorithm inspired by microchip (AN617)
div_f16_l16_f16	macro	f1,l,f3
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_f16	_PPMCR1_,f1
	load_f16_l16	_PPMCR3_,l
	clrfb	_PPMCR5_
	clrfb	_PPMCR6_
	for_f_l_l	_PPMCR7_,.1,.16
	  rlf	_PPMCR2_,W
	  rlf	_PPMCR5_,F
	  rlf	_PPMCR6_,F
	  if_f16_ge_f16	_PPMCR5_,_PPMCR3_
	    sub_f16_f16_f16	_PPMCR5_,_PPMCR3_,_PPMCR5_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	next
	load_f16_f16	f3,_PPMCR1_
	endm
f16_f16_div_l16	macro	f3,f1,l
	div_f16_l16_f16	f1,l,f3
	endm
load_f16_f16_div_l16 macro	f3,f1,l
	div_f16_l16_f16	f1,l,f3
	endm

;
; f16_f16_div_f16 var1,var2,var3
;
; div_f16_f16_f16 var2,var3,var1
;
; Register (var1) = register (var2) / register (var3)
; var1 may be equal to var2 or var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR7_
; Algorithm inspired by microchip (AN617)
div_f16_f16_f16	macro	f1,f2,f3
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_f16	_PPMCR1_,f1
	load_f16_f16	_PPMCR3_,f2
	clrfb	_PPMCR5_
	clrfb	_PPMCR6_
	for_f_l_l	_PPMCR7_,.1,.16
	  rlf	_PPMCR2_,W
	  rlf	_PPMCR5_,F
	  rlf	_PPMCR6_,F
	  if_f16_ge_f16	_PPMCR5_,_PPMCR3_
	    sub_f16_f16_f16	_PPMCR5_,_PPMCR3_,_PPMCR5_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	next
	load_f16_f16	f3,_PPMCR1_
	endm
f16_f16_div_f16	macro	f3,f1,f2
	div_f16_f16_f16	f1,f2,f3
	endm
load_f16_f16_div_f16 macro	f3,f1,f2
	div_f16_f16_f16	f1,f2,f3
	endm

;
; f32_l32_div_f32 var1,lit,var2
;
; div_l32_f32_f32 lit,var2,var1
;
; Register (var1) = literal (lit) / register (var2)
; var1 may be equal to var2
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
; Algorithm inspired by microchip (AN617)
div_l32_f32_f32	macro	l,f2,f3
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_l32	_PPMCR1_,l
	load_f32_f32	_PPMCR5_,f2
	clrf	_PPMCR9_
	clrf	_PPMCR10_
	clrf	_PPMCR11_
	clrf	_PPMCR12_
	for_f_l_l	_PPMCR13_,.1,.32
	  rlf	_PPMCR4_,W
	  rlf	_PPMCR9_,F
	  rlf	_PPMCR10_,F
	  rlf	_PPMCR11_,F
	  rlf	_PPMCR12_,F
	  if_f32_ge_f32	_PPMCR9_,_PPMCR5_
	    sub_f32_f32_f32	_PPMCR9_,_PPMCR5_,_PPMCR9_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	  rlf	_PPMCR3_, F
	  rlf	_PPMCR4_, F
	next
	load_f32_f32	f3,_PPMCR1_
	endm
f32_l32_div_f32	macro	f3,l,f2
	div_l32_f32_f32	l,f2,f3
	endm
load_f32_l32_div_f32 macro	f3,l,f2
	div_l32_f32_f32	l,f2,f3
	endm

;
; f32_f32_div_l32 var1,var2,lit
;
; div_f32_l32_f32 var2,lit,var1
;
; Register (var1) = register (var2) / literal (lit)
; var1 may be equal to var2
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
; Algorithm inspired by microchip (AN617)
div_f32_l32_f32	macro	f1,l,f3
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_f32	_PPMCR1_,f1
	load_f32_l32	_PPMCR5_,l
	clrf	_PPMCR9_
	clrf	_PPMCR10_
	clrf	_PPMCR11_
	clrf	_PPMCR12_
	for_f_l_l	_PPMCR13_,.1,.32
	  rlf	_PPMCR4_,W
	  rlf	_PPMCR9_,F
	  rlf	_PPMCR10_,F
	  rlf	_PPMCR11_,F
	  rlf	_PPMCR12_,F
	  if_f32_ge_f32	_PPMCR9_,_PPMCR5_
	    sub_f32_f32_f32	_PPMCR9_,_PPMCR5_,_PPMCR9_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	  rlf	_PPMCR3_, F
	  rlf	_PPMCR4_, F
	next
	load_f32_f32	f3,_PPMCR1_
	endm
f32_f32_div_l32	macro	f3,f1,l
	div_f32_l32_f32	f1,l,f3
	endm
load_f32_f32_div_l32 macro	f3,f1,l
	div_f32_l32_f32	f1,l,f3
	endm

;
; f32_f32_div_f32 var1,var2,var3
;
; div_f32_f32_f32 var2,var3,var1
;
; Register (var1) = register (var2) / register (var3)
; var1 may be equal to var2 or var3
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
; Algorithm inspired by microchip (AN617)
div_f32_f32_f32	macro	f1,f2,f3
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_f32	_PPMCR1_,f1
	load_f32_f32	_PPMCR5_,f2
	clrfb	_PPMCR9_
	clrfb	_PPMCR10_
	clrfb	_PPMCR11_
	clrfb	_PPMCR12_
	for_f_l_l	_PPMCR13_,.1,.32
	  rlfb	_PPMCR4_,W
	  rlfb	_PPMCR9_,F
	  rlfb	_PPMCR10_,F
	  rlfb	_PPMCR11_,F
	  rlfb	_PPMCR12_,F
	  if_f32_ge_f32	_PPMCR9_,_PPMCR5_
	    sub_f32_f32_f32	_PPMCR9_,_PPMCR5_,_PPMCR9_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlfb	_PPMCR1_, F
	  rlfb	_PPMCR2_, F
	  rlfb	_PPMCR3_, F
	  rlfb	_PPMCR4_, F
	next
	load_f32_f32	f3,_PPMCR1_
	endm
f32_f32_div_f32	macro	f3,f1,f2
	div_f32_f32_f32	f1,f2,f3
	endm
load_f32_f32_div_f32 macro	f3,f1,f2
	div_f32_f32_f32	f1,f2,f3
	endm

;
; f16_l16_rem_f16 var1,lit,var2
;
; rem_l16_f16_f16 lit,var2,var1
;
; Register (var1) = literal (lit) % register (var2), the remainder of a division
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
rem_l16_f16_f16	macro	l,f2,f3
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_l	_PPMCR1_,l
	load_f16_f16	_PPMCR3_,f2
	clrfb	_PPMCR5_
	clrfb	_PPMCR6_
	for_f_l_l	_PPMCR7_,.1,.16
	  rlf	_PPMCR2_,W
	  rlf	_PPMCR5_,F
	  rlf	_PPMCR6_,F
	  if_f16_ge_f16	_PPMCR5_,_PPMCR3_
	    sub_f16_f16_f16	_PPMCR5_,_PPMCR3_,_PPMCR5_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	next
	load_f16_f16	f3,_PPMCR5_
	endm
f16_l16_rem_f16	macro	f3,l,f2
	rem_l16_f16_f16	l,f2,f3
	endm
load_f16_l16_rem_f16 macro	f3,l,f2
	rem_l16_f16_f16	l,f2,f3
	endm

;
; f16_f16_rem_l16 var1,var2,lit
;
; rem_f16_l16_f16 var2,lit,var1
;
; Register (var1) = literal (lit) % register (var2), the remainder of a division
; var1 may be equal to var2
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
rem_f16_l16_f16	macro	f1,l,f3
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_f16	_PPMCR1_,f1
	load_f16_l16	_PPMCR3_,l
	clrfb	_PPMCR5_
	clrfb	_PPMCR6_
	for_f_l_l	_PPMCR7_,.1,.16
	  rlf	_PPMCR2_,W
	  rlf	_PPMCR5_,F
	  rlf	_PPMCR6_,F
	  if_f16_ge_f16	_PPMCR5_,_PPMCR3_
	    sub_f16_f16_f16	_PPMCR5_,_PPMCR3_,_PPMCR5_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	next
	load_f16_f16	f3,_PPMCR5_
	endm
f16_f16_rem_l16	macro	f3,f1,l
	rem_f16_l16_f16	f1,l,f3
	endm
load_f16_f16_rem_l16 macro	f3,f1,l
	rem_f16_l16_f16	f1,l,f3
	endm

;
; f16_f16_rem_f16 var1,var2,var3
;
; rem_f16_f16_f16 var2,var3,var1
;
; Register (var1) = register (var2) % register (var3), the remainder of a division
; var1 may be equal to var2 or var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
rem_f16_f16_f16	macro	f1,f2,f3
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_f16	_PPMCR1_,f1
	load_f16_f16	_PPMCR3_,f2
	clrfb	_PPMCR5_
	clrfb	_PPMCR6_
	for_f_l_l	_PPMCR7_,.1,.16
	  rlf	_PPMCR2_,W
	  rlf	_PPMCR5_,F
	  rlf	_PPMCR6_,F
	  if_f16_ge_f16	_PPMCR5_,_PPMCR3_
	    sub_f16_f16_f16	_PPMCR5_,_PPMCR3_,_PPMCR5_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	next
	load_f16_f16	f3,_PPMCR5_
	endm
f16_f16_rem_f16	macro	f3,f1,f2
	rem_f16_f16_f16	f1,f2,f3
	endm
load_f16_f16_rem_f16 macro	f3,f1,f2
	rem_f16_f16_f16	f1,f2,f3
	endm

;
; f32_l32_rem_f32 var1,lit,var2
;
; rem_l32_f32_f32 lit,var2,var1
;
; Register (var1) = literal (lit) % register (var2), the remainder of a division
; var1 may be equal to var2
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
; Algorithm inspired by microchip (AN617)
rem_l32_f32_f32	macro	l,f2,f3
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_l32	_PPMCR1_,l
	load_f32_f32	_PPMCR5_,f2
	clrf	_PPMCR9_
	clrf	_PPMCR10_
	clrf	_PPMCR11_
	clrf	_PPMCR12_
	for_f_l_l	_PPMCR13_,.1,.32
	  rlf	_PPMCR4_,W
	  rlf	_PPMCR9_,F
	  rlf	_PPMCR10_,F
	  rlf	_PPMCR11_,F
	  rlf	_PPMCR12_,F
	  if_f32_ge_f32	_PPMCR9_,_PPMCR5_
	    sub_f32_f32_f32	_PPMCR9_,_PPMCR5_,_PPMCR9_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	  rlf	_PPMCR3_, F
	  rlf	_PPMCR4_, F
	next
	load_f32_f32	f3,_PPMCR9_
	endm
f32_l32_rem_f32	macro	f3,l,f2
	rem_l32_f32_f32	l,f2,f3
	endm
load_f32_l32_rem_f32 macro	f3,l,f2
	rem_l32_f32_f32	l,f2,f3
	endm

;
; f32_f32_rem_l32 var1,var2,lit
;
; rem_f32_l32_f32 var2,lit,var1
;
; Register (var1) = register (var2) % literal (lit), the remainder of a division
; var1 may be equal to var2
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
; Algorithm inspired by microchip (AN617)
rem_f32_l32_f32	macro	f1,l,f3
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_f32	_PPMCR1_,f1
	load_f32_l32	_PPMCR5_,l
	clrf	_PPMCR9_
	clrf	_PPMCR10_
	clrf	_PPMCR11_
	clrf	_PPMCR12_
	for_f_l_l	_PPMCR13_,.1,.32
	  rlf	_PPMCR4_,W
	  rlf	_PPMCR9_,F
	  rlf	_PPMCR10_,F
	  rlf	_PPMCR11_,F
	  rlf	_PPMCR12_,F
	  if_f32_ge_f32	_PPMCR9_,_PPMCR5_
	    sub_f32_f32_f32	_PPMCR9_,_PPMCR5_,_PPMCR9_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	  rlf	_PPMCR3_, F
	  rlf	_PPMCR4_, F
	next
	load_f32_f32	f3,_PPMCR9_
	endm
f32_f32_rem_l32	macro	f3,f1,l
	rem_f32_l32_f32	f1,l,f3
	endm
load_f32_f32_rem_l32 macro	f3,f1,l
	rem_f32_l32_f32	f1,l,f3
	endm

;
; f32_f32_rem_f32 var1,var2,var3
;
; rem_f32_f32_f32 var2,var3,var1
;
; Register (var1) = register (var2) % register (var3), the remainder of a division
; var1 may be equal to var2 or var3
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR13_
; Algorithm inspired by microchip (AN617)
rem_f32_f32_f32	macro	f1,f2,f3
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_f32	_PPMCR1_,f1
	load_f32_f32	_PPMCR5_,f2
	clrf	_PPMCR9_
	clrf	_PPMCR10_
	clrf	_PPMCR11_
	clrf	_PPMCR12_
	for_f_l_l	_PPMCR13_,.1,.32
	  rlf	_PPMCR4_,W
	  rlf	_PPMCR9_,F
	  rlf	_PPMCR10_,F
	  rlf	_PPMCR11_,F
	  rlf	_PPMCR12_,F
	  if_f32_ge_f32	_PPMCR9_,_PPMCR5_
	    sub_f32_f32_f32	_PPMCR9_,_PPMCR5_,_PPMCR9_
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	  rlf	_PPMCR3_, F
	  rlf	_PPMCR4_, F
	next
	load_f32_f32	f3,_PPMCR9_
	endm
f32_f32_rem_f32	macro	f3,f1,f2
	rem_f32_f32_f32	f1,f2,f3
	endm
load_f32_f32_rem_f32 macro	f3,f1,f2
	rem_f32_f32_f32	f1,f2,f3
	endm

;
; f16_f16_l16_divr_f16 var1,var2,lit,var3
;
; divr_l16_f16_f16_f16 lit,var3,var1,var2
;
; Register (var1) = literal (lit) / register (var3)
; Register (var2) = literal (lit) % register (var3), the remainder of a division
; var1, var2 may be equal to var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
divr_l16_f16_f16_f16	macro	l1,f2,f3,f4
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_l16	_PPMCR1_,l1
	load_f16_f16	_PPMCR3_,f2
	clrfb	f4
	clrfb	f4+1
	for_f_l_l	_PPMCR5_,.1,.16
	  rlf	_PPMCR2_,W
	  rlfb	f4,F
	  rlfb	f4+1,F
	  if_f16_ge_f16	f4,_PPMCR3_
	    sub_f16_f16_f16	f4,_PPMCR3_,f4
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	next_f	_PPMCR5_
	load_f16_f16	f3,_PPMCR1_
	endm
f16_f16_l16_divr_f16	macro	f3,f4,l1,f2
	divr_l16_f16_f16_f16	l1,f2,f3,f4
	endm
load_f16_f16_l16_divr_f16	macro	f3,f4,l1,f2
	divr_l16_f16_f16_f16	l1,f2,f3,f4
	endm

;
; f16_f16_f16_divr_l16 var1,var2,var3,lit
;
; divr_f16_l16_f16_f16 var3,lit,var1,var2
;
; Register (var1) = register (var3) / literal (lit)
; Register (var2) = register (var3) % literal (lit), the remainder of a division
; var1, var2 may be equal to var3
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
divr_f16_l16_f16_f16	macro	f1,l2,f3,f4
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_f16	_PPMCR1_,f1
	load_f16_l16	_PPMCR3_,l2
	clrf	f4
	clrf	f4+1
	for_f_l_l	_PPMCR5_,.1,.16
	  rlf	_PPMCR2_,W
	  rlfb	f4,F
	  rlfb	f4+1,F
	  if_f16_ge_f16	f4,_PPMCR3_
	    sub_f16_f16_f16	f4,_PPMCR3_,f4
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	next_f	_PPMCR5_
	load_f16_f16	f3,_PPMCR1_
	endm
f16_f16_f16_divr_l16	macro	f3,f4,f1,l2
	divr_f16_l16_f16_f16	f1,l2,f3,f4
	endm
load_f16_f16_f16_divr_l16	macro	f3,f4,f1,l2
	divr_f16_l16_f16_f16	f1,l2,f3,f4
	endm

;
; f16_f16_f16_divr_f16 var1,var2,var3,var4
;
; divr_f16_f16_f16_f16 var3,var4,var1,var2
;
; Register (var1) = register (var3) / register (var4)
; Register (var2) = register (var3) % register (var4), the remainder of a division
; var1, var2 may be equal to var3 or var4
; Destroys W; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
divr_f16_f16_f16_f16	macro	f1,f2,f3,f4
	ifndef _PPMCR4_
	  messg "_PPMCR4_ not declared"
	endif
	load_f16_f16	_PPMCR1_,f1
	load_f16_f16	_PPMCR3_,f2
	clrfb	f4
	clrfb	f4+1
	for_f_l_l	_PPMCR5_,.1,.16
	  rlfb	_PPMCR2_,W
	  rlfb	f4,F
	  rlfb	f4+1,F
	  if_f16_ge_f16	f4,_PPMCR3_
	    sub_f16_f16_f16	f4,_PPMCR3_,f4
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlfb	_PPMCR1_, F
	  rlfb	_PPMCR2_, F
	next_f	_PPMCR5_
	load_f16_f16	f3,_PPMCR1_
	endm
f16_f16_f16_divr_f16	macro	f3,f4,f1,f2
	divr_f16_f16_f16_f16	f1,f2,f3,f4
	endm
load_f16_f16_f16_divr_f16	macro	f3,f4,f1,f2
	divr_f16_f16_f16_f16	f1,f2,f3,f4
	endm

;
; f32_f32_l32_divr_f32 var1,var2,lit,var3
;
; divr_l32_f32_f32_f32 lit,var3,var1,var2
;
; Register (var1) = literal (lit) / register (var3)
; Register (var2) = literal (lit) % register (var3), the remainder of a division
; var1, var 2 may be equal to var3
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
divr_l32_f32_f32_f32	macro	l1,f2,f3,f4
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_l32	_PPMCR1_,l1
	load_f32_f32	_PPMCR5_,f2
	clrfb	f4
	clrfb	f4+1
	clrfb	f4+2
	clrfb	f4+3
	for_f_l_l	_PPMCR9_,.1,.32
	  rlf	_PPMCR4_,W
	  rlfb	f4,F
	  rlfb	f4+1,F
	  rlfb	f4+2,F
	  rlfb	f4+3,F
	  if_f32_ge_f32	f4,_PPMCR5_
	    sub_f32_f32_f32	f4,_PPMCR5_,f4
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	  rlf	_PPMCR3_, F
	  rlf	_PPMCR4_, F
	next_f	_PPMCR9_
	load_f32_f32	f3,_PPMCR1_
	endm
f32_f32_l32_divr_f32	macro	f3,f4,l1,f2
	divr_l32_f32_f32_f32	l1,f2,f3,f4
	endm
load_f32_f32_l32_divr_f32	macro	f3,f4,l1,f2
	divr_l32_f32_f32_f32	l1,f2,f3,f4
	endm

;
; f32_f32_f32_divr_l32 var1,var2,var3,lit
;
; divr_f32_l32_f32_f32 var3,lit,var1,var2
;
; Register (var1) = register (var3) / literal (lit)
; Register (var2) = register (var3) % literal (lit), the remainder of a division
; var1, var2 may be equal to var3
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
divr_f32_l32_f32_f32	macro	f1,l2,f3,f4
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_f32	_PPMCR1_,f1
	load_f32_l32	_PPMCR5_,l2
	clrfb	f4
	clrfb	f4+1
	clrfb	f4+2
	clrfb	f4+3
	for_f_l_l	_PPMCR9_,.1,.32
	  rlf	_PPMCR4_,W
	  rlfb	f4,F
	  rlfb	f4+1,F
	  rlfb	f4+2,F
	  rlfb	f4+3,F
	  if_f32_ge_f32	f4,_PPMCR5_
	    sub_f32_f32_f32	f4,_PPMCR5_,f4
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	  rlf	_PPMCR3_, F
	  rlf	_PPMCR4_, F
	next_f	_PPMCR9_
	load_f32_f32	f3,_PPMCR1_
	endm
f32_f32_f32_divr_l32	macro	f3,f4,f1,l2
	divr_f32_l32_f32_f32	f1,l2,f3,f4
	endm
load_f32_f32_f32_divr_l32	macro	f3,f4,f1,l2
	divr_f32_l32_f32_f32	f1,l2,f3,f4
	endm

;
; f32_f32_f32_divr_f32 var1,var2,var3,var4
;
; divr_f32_f32_f32_f32 var3,var4,var1,var2
;
; Register (var1) = register (var3) / register (var4)
; Register (var2) = register (var3) % register (var4), the remainder of a division
; var1, var2 may be equal to var3 or var4
; Destroys W, STATUS; Needs _PPMCR1_, _PPMCR2_ ... _PPMCR9_
; Algorithm inspired by microchip (AN617)
divr_f32_f32_f32_f32	macro	f1,f2,f3,f4
	ifndef _PPMCR6_
	  messg "_PPMCR6_ not declared"
	endif
	ifndef _PPMCR7_
	  messg "_PPMCR7_ not declared"
	endif
	ifndef _PPMCR8_
	  messg "_PPMCR8_ not declared"
	endif
	load_f32_f32	_PPMCR1_,f1
	load_f32_f32	_PPMCR5_,f2
	clrfb	f4
	clrfb	f4+1
	clrfb	f4+2
	clrfb	f4+3
	for_f_l_l	_PPMCR9_,.1,.32
	  rlf	_PPMCR4_,W
	  rlfb	f4,F
	  rlfb	f4+1,F
	  rlfb	f4+2,F
	  rlfb	f4+3,F
	  if_f32_ge_f32	f4,_PPMCR5_
	    sub_f32_f32_f32	f4,_PPMCR5_,f4
	    bsf	STATUS,C
	  else_if
	    bcf	STATUS,C
	  end_if
	  rlf	_PPMCR1_, F
	  rlf	_PPMCR2_, F
	  rlf	_PPMCR3_, F
	  rlf	_PPMCR4_, F
	next_f	_PPMCR9_
	load_f32_f32	f3,_PPMCR1_
	endm
f32_f32_f32_divr_f32	macro	f3,f4,f1,f2
	divr_f32_f32_f32_f32	f1,f2,f3,f4
	endm
load_f32_f32_f32_divr_f32	macro	f3,f4,f1,f2
	divr_f32_f32_f32_f32	f1,f2,f3,f4
	endm



;**************************************************************
;***                       IL  CODES                        ***
;**************************************************************
; The following macros are defined in this section:
;   load_f_il
;   load_i_il
;   load_il_l
;   load_il_f
;   load_il_i
;   load_il_il
;   load_f_b_il_b
;   load_i_b_il_b
;   load_il_b_f_b
;   load_il_b_i_b
;   load_il_b_il_b
;
;   load_f_b_il_b
;   load_i_b_il_b
;   load_il_b_f_b
;   load_il_b_i_b
;   load_il_b_il_b

;
; load_f_il var1, var2
;   Loads register (var1) with indirect literal (var2).
;   Destroys W
load_f_il	macro	f,i1
	load_f_l	FSR,i1
	load_f_f	f,INDF
	endm
load_f8_il8	macro	f,il
	load_f_il	f,il
	endm
move_il8_f	macro	il,f
	load_f_il	f,il
	endm
move_il8_f8	macro	il,f
	load_f_il	f,il
	endm

;
; load_i_il var1,var2
;   Loads indirect register (var1) with indirect register (var2).
;   Destroys W; Needs _PPMCR1_
load_i_il	macro	i1,i2
	load_f_l	FSR,i2,
	load_f_f	_PPMCR1_,INDF
	load_f_l	FSR,i1
	load_f_f	INDF,_PPMCR1_
	endm
load_i8_il8	macro	i1,i2
	load_i_il	i1,i2
	endm
move_il_i	macro	i2,i1
	load_i_il	i1,i2
	endm
move_il8_i8	macro	i2,i1
	load_i_il	i1,i2
	endm

;
; load_il_l var,lit
;   Loads indirect register (var) with literal (lit).
;   Destroys W
load_il_l	macro	i,l
	load_f_l	FSR,i
	load_f_l	INDF,l
	endm
load_il8_l8	macro	i,l
	load_il_l	i,l
	endm
move_l_il	macro	l,i
	load_il_l	i,l
	endm
move_l8_il8	macro	l,i
	load_il_l	i,l
	endm

;
; load_il_f var1,var2
;   Loads indirect register (var1) with register (var2).
;   Destroys W
load_il_f	macro	i,f
	load_f_l	FSR,i
	load_f_f	INDF,f
	endm
load_il8_f8	macro	i,f
	load_il_f	i,f
	endm
move_f_il	macro	f,i
	load_il_f	i,f
	endm
move_f8_il8	macro	f,i
	load_il_f	i,f
	endm

;
; load_il_i var1,var2
;   Loads indirect register (var1) with indirect register (var2).
;   Destroys W; Needs _PPMCR1_
load_il_i	macro	i1,i2
	load_f_f	FSR,i2
	load_f_f	_PPMCR1_,INDF
	load_f_l	FSR,i1
	load_f_f	INDF,_PPMCR1_
	endm
load_il8_i8	macro	i1,i2
	load_il_i	i1,i2
	endm
move_i_il	macro	i2,i1
	load_il_i	i1,i2
	endm
move_i8_il8	macro	i2,i1
	load_il_i	i1,i2
	endm

;
; load_il_il var1,var2
;   Loads indirect register (var1) with indirect register (var2).
;   Destroys W; Needs _PPMCR1_
load_il_il	macro	i1,i2
	load_f_l	FSR,i2
	load_f_f	_PPMCR1_,INDF
	load_f_l	FSR,i1
	load_f_f	INDF,_PPMCR1_
	endm
load_il8_il8	macro	i1,i2
	load_il_il	i1,i2
	endm
move_il_il	macro	i2,i1
	load_il_il	i1,i2
	endm
move_il8_il8	macro	i2,i1
	load_il_il	i1,i2
	endm

;
; load_f_b_il_b v1,b1,v2,b2
;   This is a bitcopy macro
;   Destroys W
load_f_b_il_b	macro	f1,b1,f2,b2
	load_f_l	FSR,f2
	load_f_b_f_b	f1,b1,INDF,b2
	endm
load_f8_b_il8_b	macro	f1,b1,f2,b2
	load_f_b_il_b	f1,b1,f2,b2
	endm
move_il_b_f_b	macro	f2,b2,f1,b1
	load_f_b_il_b	f1,b1,f2,b2
	endm
move_il8_b_f8_b	macro	f2,b2,f1,b1
	load_f_b_il_b	f1,b1,f2,b2
	endm

;
; load_i_b_il_b v1,b1,v2,b2
;   This is a bitcopy macro
;   Destroys W. Needs _PPMCR1_
load_i_b_il_b	macro	f1,b1,f2,b2
	load_f_il	_PPMCR1_,f2
	load_f_f	FSR,f1
	load_f_b_f_b	INDF,b1,_PPMCR1_,b2
	endm
load_i8_b_il8_b	macro	f1,b1,f2,b2
	load_i_b_il_b	f1,b1,f2,b2
	endm
move_il_b_i_b	macro	f2,b2,f1,b1
	load_i_b_il_b	f1,b1,f2,b2
	endm
move_il8_b_i8_b	macro	f2,b2,f1,b1
	load_i_b_il_b	f1,b1,f2,b2
	endm

;
; load_il_b_f_b v1,b1,v2,b2
;   This is a bitcopy macro
;   Destroys W
load_il_b_f_b	macro	f1,b1,f2,b2
	load_f_l	FSR,f1
	load_f_b_f_b	INDF,b1,f2,b2
	endm
load_il8_b_f8_b	macro	f1,b1,f2,b2
	load_il_b_f_b	f1,b1,f2,b2
	endm
move_f_b_il_b	macro	f2,b2,f1,b1
	load_il_b_f_b	f1,b1,f2,b2
	endm
move_f8_b_il8_b	macro	f2,b2,f1,b1
	load_il_b_f_b	f1,b1,f2,b2
	endm

;
; load_il_b_i_b v1,b1,v2,b2
;   This is a bitcopy macro
;   Destroys W. Needs _PPMCR1_
load_il_b_i_b	macro	f1,b1,f2,b2
	load_f_i	_PPMCR1_,f2
	load_f_l	FSR,f1
	load_f_b_f_b	INDF,b1,_PPMCR1_,b2
	endm
load_il8_b_i8_b	macro	f1,b1,f2,b2
	load_il_b_i_b	f1,b1,f2,b2
	endm
move_i_b_il_b	macro	f2,b2,f1,b1
	load_il_b_i_b	f1,b1,f2,b2
	endm
move_i8_b_il8_b	macro	f2,b2,f1,b1
	load_il_b_i_b	f1,b1,f2,b2
	endm

;
; load_il_b_il_b v1,b1,v2,b2
;   This is a bitcopy macro
;   Destroys W. Needs _PPMCR1_
load_il_b_il_b	macro	f1,b1,f2,b2
	load_f_il	_PPMCR1_,f2
	load_f_l	FSR,f1
	load_f_b_f_b	INDF,b1,_PPMCR1_,b2
	endm
load_il8_b_il8_b	macro	f1,b1,f2,b2
	load_il_b_il_b	f1,b1,f2,b2
	endm
move_il_b_il_b	macro	f2,b2,f1,b1
	load_il_b_il_b	f1,b1,f2,b2
	endm
move_il8_b_il8_b	macro	f2,b2,f1,b1
	load_il_b_il_b	f1,b1,f2,b2
	endm


;	LIST
