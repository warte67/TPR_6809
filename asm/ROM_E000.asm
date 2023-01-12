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
var_count		fdb		$00

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

;	Byte ch = 0;
;	Byte at = 0;
;	Byte count = 0;
;	for (int ofs = VIDEO_START; ofs <= VIDEO_END; ofs += 2)
;	{
;		bus->write(ofs, ch++);
;		bus->write(ofs + 1, at);
;		if (count++ > 8)
;		{
;			at++;
;			count = 0;
;		}
;	}

			clr		var_ch
			clr		var_at
			clr		var_count
			ldx		#VIDEO_START
1
			cmpx	#VIDEO_END
			bge		2f
			lda		var_ch
			ldb		var_at
			std		,x++
			inc		var_ch
			inc		var_count
			lda		var_count
			cmpa	#64
			blt		1b
			inc		var_at
			clr		var_count
			bra		1b
2
	
;	for (int t = VIDEO_START; t <= VIDEO_END; t++)
;		bus->write(t, bus->read(t) + 1);

3
			ldx		#VIDEO_START
4
			cmpx	#VIDEO_END
			bge		3b
			inc		,x+
			inc		,x+
			bra		4b
	

done		BRA 	done			; infinate loop



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

