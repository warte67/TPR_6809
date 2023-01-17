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

var_ch			fcb		$00
var_at			fcb		$00
var_count		fcb		$00
var_csr			fcb		$ff
num_cycles		equ		$40
var_cycle		fcb		$00
var_mode_index	fcb		$00

			INCLUDE "mem_map.asm"

; ***************************************
; * Read Only Sytem KERNAL ROM          *
; ***************************************
                      
 
SECTION.CODE
			org     $E000  
            ;* Power On Initialization            
ROM_ENTRY   
			LDU		#U_STK_TOP+1		; top of user stack	
			LDS     #S_STK_TOP+1		; top of stack space            
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

reset		


			; TESTING: fill the first 256 bytes of screen ram 
			;		with ascending values to display
			
			lda		#num_cycles		; initially clear the cycle variable
			sta		var_cycle

;			; enable backbuffer mode
;			lda		GFX_FLAGS
;			ora		#$40
;			sta		GFX_FLAGS

			; set up the initial graphics mode 
			clr		var_mode_index	; start with index 0
			ldx		#mode_data
			lda		GFX_FLAGS
			anda	#$f0
			ora		,x
			sta		GFX_FLAGS


; ***********************
; *  Pre-Fill and Cycle 
; *  the Display Buffer
; ***********************

			clr		var_ch			; character = 0
			clr		var_at			; attribute = 0
			clr		var_count		; count = 0
			ldx		#VIDEO_START	; start of display buffer
1
			cmpx	#VIDEO_END		; at the end of the buffer?
			bge		2f				; yes, skip to the screen updates
			lda		var_ch			; load the current character
			ldb		var_at			; load the current attribute
			std		,x++			; store both character and attribute
			inc		var_ch			; next character
			inc		var_count		; increment count
			lda		var_count		; load the count
			cmpa	#17				; compare the count with this amount
			blt		1b				; loop if count lower than
			inc		var_at			; next attribute
			clr		var_count		; clear the count
			bra		1b				; resume the loop

			; INCREMENT THE SCREEN BUFFER
2
			ldx		#VIDEO_START	; start beginning of video buffer
4
			cmpx	#VIDEO_END		; until the end of the video buffer
			bge		3f				; restart when past the end
			inc		,x+				; increment character
			inc		,x+				; increment the attribute
			bra		4b				; loop until done
3
			; TOGGLE THE BACKBUFFER
			lda		GFX_FLAGS	; load current backbuffer
			eora	#$20		; toggle it
			sta		GFX_FLAGS	; save the backbuffer

			; COLOR CYCLE THE MOUSE CURSOR
			lda		#4
			sta		CSR_PAL_INDX
			lda		CSR_PAL_DATA
			adda	#$04
			sta		CSR_PAL_DATA		

			; INCREMENT THE CYCLE COUNTER
			inc		var_cycle	; increment the cycle counter
			lda		var_cycle
			cmpa	#num_cycles		; max cycles yet?
			bls		continue	; nope, continue with the main loop
			clr		var_cycle	; reset the cycle count

			; MODE CHANGES
6
			ldb		var_mode_index
			inc		var_mode_index
			ldx		#mode_data
			lda		b,x
			cmpa	#$ff
			beq		5f
			lda		GFX_FLAGS
			anda	#$f0
			ora		b,x
			sta		GFX_FLAGS
			bra		continue
5	
			clr		var_mode_index
			bra		6b
continue

	

			bra		2b

; ***********************************************************


mode_data	fcb		$00, $01, $02, $03, $04, $05, $06, $07
			fcb		$08, $09, $0a, $0b, $0c, $0d, $0e, $0f

			fcb		$ff

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

