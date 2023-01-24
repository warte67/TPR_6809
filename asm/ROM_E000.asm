; ****************
; * ROM_E000.asm *
; ****************
        

; system call macro to support the Warte variant 
;		of the MC6809e, the 6809w. This replaces
;		the SWI3 instruction with the SYS #n OS9 
;		style system call instruction.
SYS		macro
		SWI3
		fcb		\1		
		endm
			

			

; **************
; * MEMORY MAP *
; **************
				org     $0000
				
SOFT_RESET      fdb 	do_RESET      	; Software RESET Vector
SOFT_NMI        fdb 	do_NMI      	; Software NMI Vector
SOFT_SWI        fdb 	do_SWI      	; Software SWI Vector
SOFT_IRQ        fdb 	do_IRQ      	; Software IRQ Vector
SOFT_FIRQ       fdb 	do_FIRQ      	; Software FIRQ Vector
SOFT_SWI2       fdb 	do_SWI2      	; Software SWI2 Vector
SOFT_SWI3       fdb 	do_SWI3      	; Software SWI3 Vector
SOFT_RSRVD      fdb 	do_RSRV      	; Software Motorola Reserved Vector

; reserved system variables
EXEC_VECTOR		fdb		reset			; execution vector 

TCSR_ROW		fcb		0				; current text cursor row
TCSR_COL		fcb		0				; current text cursor column
TCSR_ATTRIB		fcb		$10				; current cursor attribute
TEXT_ATTRIB		fcb		$a2				; current text attribute
TCSR_ANC_ROW	fcb		0				; beginning row of line currently being edited
TCSR_ANC_COL	fcb		0				; beginning column of line currently being edited
TCSR_ANC_ADR	fdb		0				; anchor address
TCSR_EDT_ADR	fdb		0				; address when enter was pressed during line edit
DEF_GFX_FLAGS	fcb		$02				; default graphics flags	($02)

;TCSR_DECAY		fdb		$0000			; counter delay for the cursor
TCSR_DECAY		fcb		$00				; counter delay for the cursor


			INCLUDE "mem_map.asm"

; ***************************************
; * Read Only Sytem KERNAL ROM          *
; ***************************************
                      
 
SECTION.CODE
			org     $E000  
            ;* Power On Initialization            
ROM_ENTRY   
			LDU		#U_STK_TOP		; top of user stack	
			LDS     #S_STK_TOP		; top of stack space   
			LDX		#reset
			STX		EXEC_VECTOR        
            JMP     [SOFT_RESET]      

            
            ;* NMI Vector Handler                     
do_NMI      JMP just_rti
            
            ;* SWI Vector Handler (do_SWI)   
do_SWI		JMP just_rti

            ;* IRQ Vector Handler         
do_IRQ      JMP just_rti

            ;* FIRQ Vector Handler         
do_FIRQ     JMP just_rti

            ;* SWI2 Vector Handler         
do_SWI2     JMP just_rti

            ;* SWI3 Vector Handler         
do_SWI3     JMP just_rti

            ;* Reserved Vector Handler
do_RSRV     JMP reset

            ;* Reset Vector Handler         
do_RESET    JMP reset
            
;********************
;* RESET
;**************************			
			
; NOTES:  
;
;			SYNC still needs to be implemented along with NMI, IRQ, and FIRQ handlers

just_rti	
			rti 

test_file	fcn		"./asm/test.hex"

prompt_msg	fcc		"Two-PI Retro 6809\n"
			fcc		"BIOS KERNEL v.0.02\n"
			fcc		"Copyright 2023 by Jay Faries\n\n"
prompt_ready fcn	"OK"

reset		
			; display the starting screen
			jsr		starting_screen
			lda		#$0a
			jsr		char_out
			; fresh anchor
			lda		TCSR_ROW
			sta		TCSR_ANC_ROW
			lda		TCSR_COL
			sta		TCSR_ANC_COL
			clr		TCSR_ANC_ADR
			clr		TCSR_ANC_ADR+1			


			clr		TCSR_DECAY

;			ldb		#$10
;			stb		TCSR_ATTRIB
;			ldd		#0
;			std		TCSR_DECAY

; **** main KERNEL loop **********************************************
main_kernel
			; rotate the cursor attributes

			lda		GFX_CLK_DIV		; load the clock divisions
			anda	#%00001000		; mask out the 0.5 hz bit
			cmpa	TCSR_DECAY		; compare with the previous clock state
			beq		1f				; skip when no change			
			sta		TCSR_DECAY		; store the new clock state
			ldb		TCSR_ATTRIB		; B: current cursor attribute
			lda		TCSR_ATTRIB		; A: current cursor attribute
			anda	#$F0			; mask out the background bits
			sta		TCSR_ATTRIB		; store masked attribute			
			incb					; increment the background color
			andb	#$0F			; mask out the foreground color bits
			orb		TCSR_ATTRIB		; update with the background color bits
			stb		TCSR_ATTRIB		; store the new cursor attribute

			; mark the anchor
			lda 	TCSR_ANC_ROW	; A: text cursor row
			ldb		TCSR_ANC_COL	; B: text cursor column
			jsr		tcsr_pos_reg	; X: calculated offset within the video buffer
			stx		TCSR_ANC_ADR

; color the anchor character
;			ldb		#$3B			; anchor color attribute
;			stb		1,X				; highlight the anchor character

1			; display the cursor

			jsr		tcsr_pos		; fetch x from row/col
			lda		#' '			; space character
			ldb		TCSR_ATTRIB		; load the current attribute
			std		0,x				; place the cursor onto the screen
			
			; check for a key in the queue
			lda		CHAR_Q_LEN		; load the length of the key queue
			beq		main_kernel		; loop if nothing is queued

			; delete the old cursor
			jsr		tcsr_pos		; calculate X from row/col
			lda		#' '			; load a blank space character
			ldb		TEXT_ATTRIB		; load the current text attribute
			std		0,x				; store the colored character at X (row/col)
			
			; display typed character
			lda		CHAR_POP		; pop the last typed character	

			; was [ENTER] pressed
			cmpa	#$0D			; check for [ENTER]
			bne 	2f				; nope, that wasn't it. Skip ahead to 2

			pshs	A				; save the typed key
			lda		TCSR_ROW		; A: current cursor row
			ldb		TCSR_COL		; B: current cursor column
			jsr		tcsr_pos_reg	; X: calculated offset 
			stx		TCSR_EDT_ADR	; save as the end of the current edit buffer

;;			; color highlight the pending string 
;			ldb		#$4c		
;			ldx		TCSR_ANC_ADR			
;			leax	1,x
;99			stb		,x++
;			cmpx	TCSR_EDT_ADR
;			blt		99b

			; copy the string to the hardware buffer
			ldy		#EDT_BUFFER		; Y: current hardware edit buffer
			ldx		TCSR_ANC_ADR	; X: anchor or start of the edit string
4			lda		,x++			; load the character from the screen
			sta		,y+				; store it into the hardware edit buffer
			cmpx	TCSR_EDT_ADR	; check for the end of the string
			blt		4b				; keep looping if not at the end
			clr		,y				; append a NULL character in the hardware buffer
			;lda		#$0a
			;jsr 	char_out
			jsr		execute_command	; parse and run the command			
			puls	A				; A: restored Key typed
			
			ldx		TCSR_ROW
			cmpx	TCSR_ANC_ROW
			beq		2f
			bra		3f
2
			jsr		char_out		; display the last typed character
3
			cmpa	#$0D			; Was [ENTER] pressed?
			bne		2f				; no, move on

			; [ENTER] pressed			
			lda		TCSR_ANC_ROW
			ldb		TCSR_ANC_COL
			jsr		tcsr_pos_reg
			ldb		TEXT_ATTRIB
			stb		1,x

			; move the anchor to the new cursor position
			lda		TCSR_ROW
			sta		TCSR_ANC_ROW
			lda		TCSR_COL
			sta		TCSR_ANC_COL
2
			; display the new cursor
			jsr		tcsr_pos		; fetch x from row/col
			lda		#' '			; space character
			ldb		TCSR_ATTRIB		; load the current attribute
			std		0,x				; place the cursor onto the screen		

			; end of main kernel loop
			jmp		main_kernel		; continue the main kernel loop


; **** SUBROUTINES ***************************************************


; FILE SYSTEM TESTS:

load_hex
;			; load "test.hex"
;			ldx		#test_file		; fetch the filename
;			ldy		#FIO_FILEPATH	; fetch the filename hardware register storage
;1
;			lda		,x+				; copy a character from the source filename
;			sta		,y+				; store it in the hardware register
;			bne		1b				; keep looping until null-termination

			lda		#$07			; command: LoadHex
			sta		FIO_COMMAND		; executre the command
			lda		FIO_ERR_FLAGS	; load the errors flag
			cmpa	#$80			; test for bit: file not found?
			beq		1f				; dont call the sub if it wasnt loaded
			cmpa	#$10			; test for bit: file not found?
			beq		2f				; dont call the sub if it wasnt loaded
			bra		9f

1			; display, "ERROR: File Not Found!"
			ldx		#strz_nofile_error
			jsr		text_out
			bra		9f
2			; display, "ERROR: Wrong File Type"
			ldx		#strz_wrongfile_error
			jsr		text_out
9			
			rts


; **** SYSTEM CALLS ******************************

clear_text_screen	; clear the text screen
			pshs	D,X
			ldx		#VIDEO_START
			lda		#' '
			ldb		TEXT_ATTRIB
1			std		,x++
			cmpx	#VIDEO_END
			bls		1b	
			puls	D,X		
			clr		TCSR_ROW
			clr		TCSR_COL			
			rts


char_out	; Display character in the A register to the screen
			; at the current cursor position and in the 
			; current color.
			pshs	D, X, Y

			; just return if A=null
			tsta
			beq		3f
			ldb		TEXT_ATTRIB
			bsr		tcsr_pos		; find X from Row and Col
			cmpa	#$0a
			beq		_cr
			cmpa	#$0D
			beq		_cr
			cmpa	#$08			; backspace
			beq		_backspace
			cmpa	#$20
			blt		3f
			; standard printable character
			std		,x
			inc		TCSR_COL
			lda		TCSR_COL
			cmpa	GFX_FG_WDTH
			bls		3f
_cr
			clr		TCSR_COL
			lda		TCSR_ROW
			cmpa	GFX_FG_HGHT
			bge		_scroll
			inc		TCSR_ROW

			bra		3f
_backspace
			lda		TCSR_COL
			beq		3f
			dec		TCSR_COL
			;dec	EDT_BFR_CSR
			bra		3f

_scroll			
			lda		GFX_FG_WDTH
			inca
			ldb		#2
			mul
			ldx		#VIDEO_START
			leax	d,X
			ldy		#VIDEO_START
1			ldd		,x++
			std		,y++
			cmpx	#VIDEO_END
			blt		1b
			lda		#' '
			ldb		TEXT_ATTRIB
2			std		,y++
			cmpy	#VIDEO_END
			blt		2b

3			puls	D, X, Y
			rts

text_out	; output the string pointed to by X using the current attribute
			pshs	D,X
1			lda		,X+		
			beq		2f				
			jsr		char_out
			bra		1b
2			puls	D,X
			rts


tcsr_pos	; load into X according to TCSR_ROW & TCSR_COL

			pshs	D				; save for later clean up
			lda 	TCSR_ROW		; A: cursor row
			ldb		TCSR_COL		; B: cursor column
			jsr 	tcsr_pos_reg	; X: position within the video buffer
			puls	D				; clean up the registers
			rts						; return


tcsr_pos_reg	; load into X according to text cursor position (A:ROW, B:COL)
			pshs	D						; save A and B 
				pshs	D					; again, save A and B
					ldx		#VIDEO_START	; point X to the start of the video buffer
					lsla					; account for the attribute byte
					ldb		GFX_FG_WDTH		; load the max horizontal character position
					incb					; correct for width
					mul						; offset the horizontial position
					leax	D,X				; within the video buffer
				puls	D					; restore A and B
				lslb						; correct vertical to account for height
				leax	b,x					; offset into the video buffer
				cmpx	#VIDEO_END			; was the video buffer exceeded
				bls		1f					; branch out if not
				ldx		#VIDEO_END-1		; point X to the very last cell as an error
1			puls	D						; final register clean up
			rts								; return

starting_screen ; clear and display the starting screen condition
		; load the default graphics mode
			lda		DEF_GFX_FLAGS
			sta		GFX_FLAGS
		; set the text attribute default
			lda		#$a2
			sta		TEXT_ATTRIB	
		; clear screen
			jsr		clear_text_screen
		; display the text prompt
			ldx		#prompt_msg
			jsr		text_out
		; start the first anchor
			lda		TCSR_ROW
			sta		TCSR_ANC_ROW
			lda		TCSR_COL
			sta		TCSR_ANC_COL
			clr		TCSR_ANC_ADR
			clr		TCSR_ANC_ADR+1
			rts

ok_prompt ; display the ready prompt
			ldx		#prompt_ready
			jsr		text_out
			lda		#$0A
			jsr		char_out
			rts

text_screen_reset
		; load the default graphics mode
			lda		DEF_GFX_FLAGS
			sta		GFX_FLAGS
		; set the text attribute default
			lda		#$a2
			sta		TEXT_ATTRIB	
		; clear screen
			jsr		clear_text_screen
		; start the first anchor
			lda		TCSR_ROW
			sta		TCSR_ANC_ROW
			lda		TCSR_COL
			sta		TCSR_ANC_COL
			clr		TCSR_ANC_ADR
			clr		TCSR_ANC_ADR+1
			rts


execute_command	; parse and run the string that is currently in the hardware EDT_BUFFER register
			lda		EDT_BUFFER
			cmpa	#$20
			lbeq	1000f

			lda		#$0a
			jsr		char_out

		; parse
			jsr		lookup_cmd

		; [L] = test load "test.hex"
			cmpa	#0
			lbeq	999f			; do syntax error
			cmpa	#1				
			lbeq	1f				; do "cls"
			cmpa	#2				
			lbeq	2f				; do "load"
			cmpa	#3
			lbeq	3f				; do "exec"
			cmpa	#4
			lbeq	4f				; do "reset"
			cmpa	#5
			lbeq	5f				; do "exit"
			cmpa	#6
			lbeq	6f				; screen
			cmpa	#7
			lbeq	7f				; dir
			cmpa	#8
			lbeq	8f				; chdir
			lbra		999f				; syntax error

1 ; cls
			tst		FIO_FILEPATH
			lbeq		11f

			lda		FIO_FILEPATH
			ora		#$20			; force lower case
			cmpa	#'a'
			blt		101f
			cmpa	#'f'
			bgt		101f
			suba	#'a'
			adda	#10
			bra		102f
101			cmpa	#'0'
			blt		11f				; invalid, just use defaults
			cmpa	#'9'
			bgt		11f				; also invalid
			suba	#'0'
102;
			ldb		FIO_FILEPATH+1
			orb		#$20			; force lower case
			cmpb	#'a'
			blt		103f
			cmpb	#'f'
			bgt		103f
			subb	#'a'
			addb	#10
			bra		104f
103			cmpb	#'0'
			blt		11f				; invalid, just use defaults
			cmpb	#'9'
			bgt		11f				; also invalid
			subb	#'0'
104
			lsla
			lsla
			lsla
			lsla
			sta		TEXT_ATTRIB
			orb		TEXT_ATTRIB
			stb		TEXT_ATTRIB

			bra 	12f
11		; default color
			lda		#$a2
			sta		TEXT_ATTRIB	
12		; clear screen
			jsr		clear_text_screen
			jsr		ok_prompt
			rts		

2 ; load <arg>
			jsr 	load_hex			
			lda		#$02			; load the default graphics mode
			jsr		ok_prompt
			rts

3 ; exec			
			lda		FIO_ERR_FLAGS	; load the errors flag
			anda	#$80			; test for bit: file not found?		
			tsta	
			bne		31f				; dont call the sub if it wasnt loaded
			ldx		EXEC_VECTOR		; whats in the exec vector?
			cmpx	#reset			; if its set to default
			beq		31f				; then skip to OK prompt
			jsr		[EXEC_VECTOR]	; call the loaded subroutine

			jsr		text_screen_reset
			jsr		ok_prompt		
			rts
31 ; skip exec
			jsr		ok_prompt
			rts

4 ; reset
			lda		#$00			; $00 = Reset/Null
			sta		FIO_COMMAND		; send the command
			rts

			;jmp		reset

5 ; exit
			lda		#$17			; $17 = SYSTEM: Shutdown
			sta		FIO_COMMAND		; send the command 
			rts

6 ; screen <arg>
			; $30 = 0	32x20
			; $31 = 1	64x40
			; $32 = 2	128x80 bitmap mode
			; check the argument
			lda		FIO_FILEPATH
			cmpa	#'0'			; mode 0 = 32x20 
			beq		60f
			cmpa	#'1'			; mode 0 = 32x20 
			beq		61f
			cmpa	#'2'			; mode 0 = 32x20 
			beq		62f
			; ERROR: Argument out of Range!
			ldx 	#strz_range_error
			jsr		text_out
			jsr		ok_prompt	
			rts
60 ; 32x20 text
			lda		GFX_FLAGS
			anda	#$FC			; mask out FG mode
			ora		#$01			; mask in 32 column text mode
			sta		GFX_FLAGS		; update the display mode
			sta		DEF_GFX_FLAGS	; replace the defaults
			jmp		4b				; system: reset
61 ; 64x40 text (default)
			lda		GFX_FLAGS
			anda	#$FC			; mask out FG mode
			ora		#$02			; mask in 64 column text mode
			sta		GFX_FLAGS		; update the display mode
			sta		DEF_GFX_FLAGS	; replace the defaults
			jmp		4b				; system: reset

62 ; 128x80 16-color bitmap
			lda		GFX_FLAGS
			anda	#$FC			; mask out FG mode
			ora		#$03			; mask in 64 column text mode
			sta		GFX_FLAGS		; update the display mode
			sta		DEF_GFX_FLAGS	; replace the defaults
			jmp		4b				; system: reset

7 ; dir
			lda		#$0c			; list files
			sta		FIO_COMMAND

			clra
			ldx		#FIO_RET_BUFFER
71 ; loop
			sta		FIO_RET_INDEX	; get the File[A]
			jsr		text_out		; send it to the screen
			inca	
			cmpa	FIO_RET_COUNT	; how many entries?
			bne		71b
		
			lda		#$0A
			jsr		char_out
			jsr		ok_prompt	
			rts

8 ; chdir
			lda		#$0e			; $0E = command "CHDIR"
			sta		FIO_COMMAND

			lda 	FIO_ERR_FLAGS
			anda	#$08			; directory not found
			cmpa	#$08
			bne		81f				; nope, just end
			ldx		#strz_dirnope_error
			jsr		text_out
81 ; done
			jsr		ok_prompt	

			rts


999		; Report a Syntax Error
;			lda		#$0a
;			jsr		char_out		
			ldx		#strz_syntax_error
			jsr		text_out
			jsr		ok_prompt	
			rts

			lda		#':'
			jsr		char_out
			lda		#' '
			jsr		char_out
			lda		#$22			; "
			jsr		char_out
			ldx		#EDT_BUFFER
			jsr		text_out
			lda		#$22			; "
			jsr		char_out
			lda		#$0a
			jsr		char_out
			ldx		#prompt_ready
			jsr		text_out
			lda		#$0a
			jsr		char_out
1000 ; return from subroutine
			rts

fetch_cl_argument	; copy everything past the first space character to FIO_FILEPATH
			pshs 	A, X, Y
			ldx		#EDT_BUFFER
			ldy		#FIO_FILEPATH
1 ; look for space
			lda		,X+
			cmpa	#' '
			beq		2f				; copy the argument
			cmpx	#EDT_BUFFER+255
			bne		1b
2 ; loop through the argument
			clr		-1,X
			lda		,X+
			sta		,Y+
			tsta
			beq		3f	
			cmpy	#FIO_FILEPATH+255
			bne		2b
3 ; done
			puls    A, X, Y
			rts


lookup_cmd	; return in A index of the command
			; or A = 0 if command not found
			jsr 	fetch_cl_argument
			pshs 	B, X, Y
			lda		#1				; RET = 1
			pshs	A				; push local RET onto the stack
			clra					; A = Working Accumilator			
			clrb					; B = EDT_BUFFER[B] index
			ldx		#EDT_BUFFER		; X = EDT_BUFFER[0]
			ldy		#command_LUT	; Y points to the position within the lookup table
1 ; loop:	
			lda		B, X			; load A from EDT_BUFFER[X]				
			cmpa	#$00			; compare A with NULL-TERMINATION
			beq		4f				; branch if EQUAL to success
			ora		#$20			; force lower case
			cmpa	,Y+				; compare A with command_LUT[Y]
			bne		2f				; branch if NOT equal to next_token
			incb					; increment index in EDT_BUFFER[B]
			bra		1b				; branch back to loop    	
			
2 ; next_token:
			inc		0,S				; increment RET
			clrb					; clear EDT_BUFFER[B] index
			; move Y to the beginning of the next label
5 ; lp_1:
			lda		,Y+				; load a with the current character in the LUT
			cmpa	#$ff			; if were at the end of the LUT
			beq		3f				; branch to failure
			cmpa	#$00			; end of a token
			bne		5b				; keep searching if not
			bra		1b				; branch back to loop    
3 ; failure:
			clr		0,S				; RET = 0
4 ; success:
			puls	A				; recover local RET
			puls    B, X, Y
			rts						; return RET			


strz_syntax_error		fcn		"ERROR: Syntax!\n"
strz_nofile_error		fcn		"ERROR: File Not Found!\n"
strz_range_error		fcn		"ERROR: Argument out of Range!\n"
strz_wrongfile_error	fcn		"ERROR: Wrong File Type!\n"
strz_dirnope_error		fcn		"ERROR: Directory Not Found!\n"

command_LUT 
			fcn		"cls"			; 1
			fcn		"load"			; 2
			fcn		"exec"			; 3
			fcn		"reset"			; 4
			fcn		"exit"			; 5
			fcn		"screen"		; 6	
			fcn		"dir"			; 7
			fcn		"cd"			; 8		change directories
			fcb		0xFF




; ***********************************************************


; interrupt vectors
				org  $fff0
HARD_RSRVD      fdb  do_RSRV    ; Motorola RESERVED Hardware Interrupt Vector
HARD_SWI3       fdb  do_SWI3    ; SWI3 Hardware Interrupt Vector
HARD_SWI2       fdb  do_SWI2    ; SWI2 Hardware Interrupt Vector
HARD_FIRQ       fdb  do_FIRQ    ; FIRQ Hardware Interrupt Vector
HARD_IRQ        fdb  do_IRQ     ; IRQ Hardware Interrupt Vector
HARD_SWI        fdb  do_SWI     ; SWI/SYS Hardware Interrupt Vector
HARD_NMI        fdb  do_NMI     ; NMI Hardware Interrupt Vector
HARD_RESET      fdb  ROM_ENTRY  ; RESET Hardware Interrupt Vector

			END

