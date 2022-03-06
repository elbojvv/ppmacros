# ppmacros
 Macros in assembly for PIC 8 bit MCUs
 
 Includes macro's for bit, 8, 16 and 32 bit operations, for-next loops, and if-else loops.
 Optimizes the minimal use of bank and page statements.


**************************************************************
***                       INTRODUCTION                     ***
**************************************************************

This file contains macro definitions for MPASM. With these
macros it is easier to program Microchip PIC processors,
without having to buy a C-compiler. Just use these macros
with the free MPLAB (including MPASM) program provided
by MicroChip, the producer of the PIC processors. Download
at www.microchip.com.

The macros make it possible to have if-else-endif blocks,
for-next and do-until loops. This will speed-up programming
a lot and will decrease debugging time, since one does not
have to use the twisted BTFSC and BTFSS instructions and
difficult compare codes anymore.

Also is taken care of the bank-bits (for data memory) and the
page-bits (for program memory).

**************************************************************
***                     TIPS AND TRICKS                    ***
**************************************************************

* One is allowed to jump outside a loop or block. However,
  one should be carefull when jumping INTO a block or loop.
* Set the tab-size at 16 characters (options/
  environment setup in MPLAB).
* For debugging, open the .lst file which is created during
  compiling or building. Start debugging (by using F7 or F8)
  while in the window containing the .lst file. Debugging
  in the original .asm file will be confusing, since MPLAB
  will change all the time to the window with the macros.
  This DID work up to version 6 of MPLAB. Hopefully, Microchip
  will restore this function.
* The chosen naming for the macros is based on the intention
  to expand the macros with 16- and 32-bit words, 24- and
  32-bit floating point variables and indirect addressing.
  So, then macros will look like "if_fp24_gt_fp24".
  However, everyone can change the naming of the macros
  to whatever naming they please if (e.g.) another naming is
  more close to their 'feeling' or knowledge of some
  programming languages.
  - Since the macros use eachother, use the 'replace' option
    in the editor, in order to conserve the dependencies.
  - You cannot use the words used by MPLAB itself, as:
    "IF", "ELSE", "ENDIF", "END", "WHILE" etc. See the
    help for MPASM (accessable in MPLAB) for all words.
    This also explains the strange name "else_"...
* Since all text (including this one :) ) will end up in the
  .lst file and the .lst file is best used for debugging, one
  can create a new include file in which all lines starting
  with the semicolumn (;) can be deleted. 

**************************************************************
***                       HOW IT WORKS                     ***
**************************************************************

If you just want to use the macros, you can safely skip this
part and continue to the SYNTAX section.

In 1999, Karl Lunt already wrote a library and published a
description in 'Nuts & Volts Magazine' (See
http://www.nutsvolts.com/PDF_Files/picmacro.pdf and
http://www.seanet.com/~karllunt/picmacro.htm).
However, these for-next macros could not handle the
two subquential loops inside another loops (written as
BASIC statements):
  for x=1 to 10
    for y=1 to 3
      statement
    next y
    for y=1 to 5
      statement
    next y
  next x

This problem was solved in these macros by using arrays in
the MPASM compiler directives and expressions. Note: these
are just arrays which are used by MPASM, it is not visable
in the generated code or programmed in the chip!

The example above has two nesting levels. For every level
(depth) an own bookkeeping is performed. If we just look at
the outer loop (over x), in some simple code, it would look
as follow (with after remark the original code).
  remark for x=1 to 10
  x=1
  lab_1_1:
  if x>10 goto lab_1_2
    subloop
  remark next x
  x=x+1
  goto lab_1_1
  lab_1_2:

If in nesting-level 1, always a label "lab_1_x" is used, one
only has to increase the last number to get everything right.

Now the subloop is nesting-level 2. The code for the first
for-loop over y will roughly look the same as the code above.
Now only 'lab_2_x' is used.
The total code then will be:

  remark for x=1 to 10
  x=1
  lab_1_1:
  if x>10 goto lab_1_2

    remark for y=1 to 3
    y=1
    lab_2_1:
    if y>5 goto lab_2_2
      statements
    remark next y
    y=y+1
    goto lab_2_1
    lab_2_2:

    remark for y=1 to 5
    y=1
    lab_2_3:
    if y>5 goto lab_2_4
      statements
    remark next y
    y=y+1
    goto lab_2_3
    lab_2_4:

  remark next x
  x=x+1
  goto lab_1_1
  lab_1_2:

As can be seen, this will always work. The only thing needed
is that for every nesting-level a variable has to be defined
which counts the current label number for the specific
depth. This will be the array.

How to make an array in MPASM. The "#v()" operator will 
convert a number inside the brackets to text. So the 
statement "lab#v(a)" in a macro, while "a" has the value 4,
will end up as "lab4" before the final compilation starts.
Now one step deeper, lab4 itself could be a variable, if
used in the right contect. The following code shows an array:

  arr0 = .18
  arr1 = .26
  arr2 = .87
  i = 0
  while i<3
  lab#v(arr#v(i)):
  i+=1
  endw

This will produce the following text before the final
compilation:
  lab18:
  lab26:
  lab87:

This, just to indicate how to make arrays in MPASM.

Since we now know how to make a for-next loop and use an array,
let's look at the actual macro-code. The for_f_l_l macro is:

 1:  for_f_l_l   macro           var1,lit1,lit2
 2:  cdep++
 3:              if cdep>mdep
 4:  mdep++
 5:  labcnt#v(cdep) = 0
 6:              endif
 7:  labcnt#v(cdep)++
 8:              load_f_l        var1,lit1	
 9:  lab_#v(cdep)_#v(labcnt#v(cdep)):
10:  labcnt#v(cdep)++
11:              gotoif_f_gt_l   var1,lit2,lab_#v(cdep)_#v(labcnt#v(cdep))
12:              endm

cdep (current depth) contains the current nesting-level. mdep
(maximum reached depth) is just used to initialise (put zero) if
the array element if a nesting-level has not been reached before.
The array itself is labcntx, in which x is the arrayindexnumber. 

Imagin that this is the first loop in the code. cdep and mdep equal
zero. Stepping through the code: cdep is increased, the
nesting-level is now 1. cdep is larger than mdep, meaning this is
the first time this nesting level is reached. So this array-element
has to initialised by setting it to zero. Line 5 will be
translated to "labcnt1 = 0". labcnt1 is increased in line 7
everytime a for-loop is entered, in order to produce a unique
label. The label in line 9 is translated lab_1_1 (as in the example).
labcnt1 is increased in line 10, since a for-loop needs two labels
(an if-block only need one label). In line 11, the label
lab_1_2 is generated (as in the example).

Now the next-code:
13:  next_f   macro           var1
14:           incf            var1,f
15:           goto            lab_#v(cdep)_#v(labcnt#v(cdep)-1)
16:  lab_#v(cdep)_#v(labcnt#v(cdep)):
17:  cdep--	
18:           endm
Line 15 generates the label lab_1_1, since cdep equals 2 and is
decresed by 1. This label jumps back to the 'for'-code. Line 16
generates the label lab_1_2, which was needed in the 'for' code
when the loop was completed. Line 17 decreases cdep, since 
nesting-level 1 is left and the program now enters nesting-
level 0.

**************************************************************
***                          SYNTAX                        ***
**************************************************************

*** SYNTAX (load-statements):

  load_x_y x,y
    Loads x with y

  x: f, f8, f16, f32  - file (variable) (alias: f8)
     i, i8            - indirect file  (alias: if)
   
  y: l, l8, l16, l32  - literal
     f, f8, f16, f32  - file (variable) (alias: f8)
     i, i8            - indirect file   (alias: if)

  load_f_b_l file, bit, literal
     Loads bit1 of file1 with literal (lowest bit)
  load_f_b_f_b file1, bit1, file2, bit2
     Loads bit1 of file1 with bit2 of file2


*** SYNTAX (if-statements):

  gotoif_x_ex_y file, $2, label
    $2: file, literal
    jumps to label if the expression is met

  if_x_ex_y file, $2
    statements
  {else
    statements}
  endif 
    $2: file, literal
    Executes blocks depending on if expression is met

  ex:eq - equal  
     ne - not equal
     ge - greater equal
     gt - greater than
     lt - less than
     le - less equal 

  x: l, l8, l16, l32 - literal
     f, f8, f16, f32 - file (variable)
       
  y: f, f8, f16, f32 - file (variable)
       

  gotoif_f_b_s file, literal, label 
  gotoif_f_b_c file, literal, label 
    jumps to label if bit (literal) of file is set (s) or clear (c)

  if_f_b_s file, literal, label 
    statements
  {else_
    statements} 
  end_if
  if_f_b_c file, literal, label 
    statements
  {else_
    statements} 
  end_if
    Executes blocks depending on if bit (literal) of file is set (s) or clear (c)


*** SYNTAX (for-statements):

  for_f_x_y register, start,  end
    statements
  next register
    x,y: l, l8, l16, l32, f, f8, f16, f32 
    Executes for-next loop, in which register is increased from start to (and including) end

  ford_f_x_y register, start,  end 
    counts down

*** SYNTAX (repeat and while-statements):

  repeat
    statements
  until_x_ex_y
  until_f_b_s
  until_f_b_c 
    Executes blocks until expression is met

  
  while_x_ex_y
  while_f_b_s
  while_f_b_c 
    statements
  end_while
    Executes blocks if expression is met

  ex:eq - equal  
     ne - not equal
     ge - greater equal
     gt - greater than
     lt - less than
     le - less equal 

  x: f, f8, f16, f32 - file (variable)

  y: l, l8, l16, l32 - literal
     f, f8, f16, f32 - file (variable)
       

*** SYNTAX (Math):


 add_x_y_z    x,y,z       ; z = x + y
 sub_x_y_z    x,y,z       ; z = x - y
 mul_x_y_z    x,y,z       ; z = x * y
 div_x_y_z    x,y,z       ; z = x / y
 rem_x_y_z    x,y,z       ; z = x % y (the remainder of the division)
 divr_x_y_z_z x,y,z1,z2   ; z1 = x / y, z2 = x % y

  x: l, l8, l16, l32 - literal
     f, f8, f16, f32 - file (variable)
  y: l, l8, l16, l32 - literal (except l_l etc.)
     f, f8, f16, f32 - file (variable)
  z: f, f8, f16, f32 - file (variable)

