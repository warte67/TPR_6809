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

EXEC_VECTOR		fdb		reset


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

prompt_msg1	fcn		"Two-PI Retro 6809"
prompt_msg2	fcn		"BIOS KERNEL v.0.01"
prompt_msg3 fcn		"Copyright 2023 by Jay Faries"
prompt_ready fcn	"OK"

reset		
			;jsr		test_load_hex


	; load the default graphics mode
			lda		#$02
			sta		GFX_FLAGS

	; clear screen
			ldx		#VIDEO_START
			lda		#' '
			ldb		#$a2
1		
			std		,x++
			cmpx	#VIDEO_END
			bls		1b	

	; first prompt line
			ldy		#prompt_msg1
			ldx		#VIDEO_START
			jsr		prt_line

			ldy		#prompt_msg2
			ldx		#VIDEO_START + 128
			jsr		prt_line

			ldy		#prompt_msg3
			ldx		#VIDEO_START + 256
			jsr		prt_line

			ldy		#prompt_ready
			ldx		#VIDEO_START + 512
			jsr		prt_line

	; simply flash a cursor
			ldx		#VIDEO_START + 640
1
			lda		#' '	;$8f
			; update the cursor
			sta		0,x

			; increment bg cursor color with black fg
			ldb		1,x
			addb	#$01		;$10
			andb	#$0F		;$f0
			stb		1,x

			; delay
			ldy		#$4000
2			leay	-1, y
			bne		2b

	; check for a key press
			
			lda		CHAR_Q_LEN	; if no keys waiting in queue
			beq		1b			; continue looping

	; handle key presses
			lda		CHAR_POP	; pop the next key press from the queue
			sta		$0020


			bra		1b



done		bra		done


; ** Subroutines ****************************

prt_line	
2
			lda		,y+
			beq		3f
			sta		,x++
			bra		2b
3		
			rts
		


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

