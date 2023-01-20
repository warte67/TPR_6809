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

TCSR_ROW		fcb		0				; current cursor row
TCSR_COL		fcb		0				; current cursor column
TCSR_ATTRIB		fcb		$10				; current cursor attribute
TEXT_ATTRIB		fcb		$a2				; current text attribute
TCSR_DECAY		fdb		$0000			; counter delay for the cursor


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
prompt_ready fcn	"OK\n"

reset		
		; simply run the test code		
			;jsr		test_load_hex


		; load the default graphics mode
			lda		#$02
			sta		GFX_FLAGS

		; set the text attribute default
			lda		#$a2
			sta		TEXT_ATTRIB	

		; clear screen
			jsr		clear_text_screen


		; preset the starting text cursor position
			clr		TCSR_ROW
			clr		TCSR_COL

		; display the text prompt
			ldx		#prompt_msg
			jsr		text_out


		; main KERNEL loop
			ldb		#$10
			stb		TCSR_ATTRIB
			ldd		#0
			std		TCSR_DECAY

main_kernel
			; rotate the cursor attributes
			ldd		TCSR_DECAY		; load the cursor delay
			addd	#1				; increment it
			std		TCSR_DECAY		; store it	
			cmpd	#$200			; check if delay has expired
			blt		1f				; skip past the color update
			inc		TCSR_ATTRIB		; increment the color attribute
			ldd		#0				; reset the cursor delay
			std		TCSR_DECAY		; store the reset delay

1			; display the cursor
			jsr		_tcsr_pos		; fetch x from row/col
			lda		#' '			; space character
			ldb		TCSR_ATTRIB		; load the current attribute
			std		0,x				; place the cursor onto the screen
			
			; check for a key in the queue
			lda		CHAR_Q_LEN		; load the length of the key queue
			beq		main_kernel		; loop if nothing is queued

			; delete the old cursor
			jsr		_tcsr_pos		; calculate X from row/col
			lda		#' '			; load a blank space character
			ldb		TEXT_ATTRIB		; load the current text attribute
			std		0,x				; store the colored character at X (row/col)
			
			; display typed character
			lda		CHAR_POP		; pop the last typed character			
			jsr		char_out		; display it

			; display the new cursor
			jsr		_tcsr_pos		; fetch x from row/col
			lda		#' '			; space character
			ldb		TCSR_ATTRIB		; load the current attribute
			std		0,x				; place the cursor onto the screen		

			; end of main kernel loop
			bra		main_kernel		; continue the main kernel loop


; **** SUBROUTINES ***************************************************


; FILE SYSTEM TESTS:

test_load_hex
			; load "test.hex"
			ldx		#test_file		; fetch the filename
			ldy		#FIO_FILEPATH	; fetch the filename hardware register storage
1
			lda		,x+				; copy a character from the source filename
			sta		,y+				; store it in the hardware register
			bne		1b				; keep looping until null-termination

			lda		#$07			; command: LoadHex
			sta		FIO_COMMAND		; executre the command
			lda		FIO_ERR_FLAGS	; load the errors flag
			cmpa	#$80			; test for bit: file not found?
			beq		1f				; dont call the sub if it wasnt loaded
			jsr		[$0010]			; call the loaded subroutine
1
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
			rts


char_out	; Display character in the A register to the screen
			; at the current cursor position and in the 
			; current color.

			pshs	D, X, Y
			ldb		TEXT_ATTRIB
			bsr		_tcsr_pos		; find X from Row and Col

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

_tcsr_pos	; load into X according to TCSR_ROW & TCSR_COL
			pshs	D
			ldx		#VIDEO_START
			lda		TCSR_ROW
			lsla
			ldb		GFX_FG_WDTH
			incb
			mul
			leax	D,X
			lda		TCSR_COL
			lsla
			leax	a,x
			cmpx	#VIDEO_END
			bls		1f
			ldx		#VIDEO_END-1
1
			puls	D
			rts

text_out	; output the string pointed to by X using the current attribute
			pshs	D,X
1			lda		,X+		
			beq		2f				
			jsr		char_out
			bra		1b
2			puls	D,X
			rts

		







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

