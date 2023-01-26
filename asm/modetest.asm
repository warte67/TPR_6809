; BUILD:	asm6809 -H -9 test.asm -o test.hex		
		
		org		$0010
; EXEC_VECTOR
exe_vect	fdb		start




		INCLUDE "mem_map.asm"

		ORG		$2000

var_ch			fcb		$00
var_at			fcb		$00
var_count		fcb		$00
var_csr			fcb		$ff
num_cycles		equ		$20
var_cycle		fcb		$00
var_mode_index	fcb		$00
var_mouse_color	fdb		$0000

start

			; TESTING: fill the first 256 bytes of screen ram 
			;		with ascending values to display
			
			lda		#num_cycles		; initially clear the cycle variable
			sta		var_cycle

			; SAVE THE MOUSE CURSOR COLOR
			lda		#4
			sta		CSR_PAL_INDX
			ldd		CSR_PAL_DATA
			std		var_mouse_color

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

			; fill the background buffer with incrementing values
			ldx		#0
1
			stx		GFX_EXT_ADDR
			sta		GFX_EXT_DATA
			inca
			cmpa	#$ff
			bne		2f
			clra
2
			leax	1,x
			cmpx	#$2800
			bne		1b



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

			; SCROLL THE EXTENDED SCREEN BUFFER
			lda		#2				; delta value = scroll by this many pixels
			sta		GFX_BG_ARG1		; arg1 holds the delta value for the scroll
			lda		#$04			; command: scroll 
			sta		GFX_BG_CMD		; issue the command

			; INCREMENT THE EXTENDED SCREEN BUFFER
			lda		GFX_FLAGS
			anda	#$0F
			cmpa	#$0C
			bge		81f

			ldx		#0
8			stx		GFX_EXT_ADDR
			inc		GFX_EXT_DATA
			leax	1,x
			cmpx	#640		;#$0800
			bne		8b
81




			; TOGGLE THE BACKBUFFERS
			;lda		#$08		; cmd: copy buffer front to back
			;sta 		GFX_BG_CMD			
			;lda		#$03		; cmd: swap backbuffers
			;sta		GFX_BG_CMD	; send the command

			lda		GFX_FLAGS	; load current backbuffer
			eora	#$20		; toggle it
			sta		GFX_FLAGS	; save the backbuffer

			; COLOR CYCLE THE MOUSE CURSOR
			lda		#4
			sta		CSR_PAL_INDX
			ldd		CSR_PAL_DATA
			addd	#$00a0
			std		CSR_PAL_DATA		

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
			; WAS [ESCAPE] PRESSED
			lda		CHAR_Q_LEN
			beq		2b

			; check for ESCAPE
			lda		CHAR_POP
			cmpa	#$1b		; [ESCAPE]
			lbne	2b

			; RESTORE THE MOUSE CURSOR COLOR
			lda		#4
			sta		CSR_PAL_INDX
			ldd		var_mouse_color
			std		CSR_PAL_DATA

done		rts
		
; ////////////////////////////////////////////////////

;mode_data	fcb		$0c, $0c, $0c, $0c, $0c, $0c, $0c, $0c
;			fcb		$0c, $0c, $0c, $0c, $0c, $0c, $0c, $0c

mode_data	fcb		$00, $01, $02, $03, $04, $05, $06, $07
			fcb		$08, $09, $0a, $0b, $0c, $0d, $0e, $0f

			fcb		$ff