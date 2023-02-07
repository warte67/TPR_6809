; ******************
; * ROM_E000_H.asm *
; ******************


; system call macro to support the Warte variant 
;		of the MC6809e, the 6809w. This replaces
;		the SWI3 instruction with the SYS #n OS9 
;		style system call instruction.
SYS		macro
		SWI3
		fcb		\1		
		endm
					
; reserved system variables

;		org	$0100	; for now, just place reserved kernal variables at $0100
;				; after the rom kernel vectors are clearly established
;				; move this origin to just after them.
;
;TCSR_ROW	fcb	0	; current text cursor row
;TCSR_COL	fcb	0	; current text cursor column
;TCSR_ATTRIB	fcb	$10	; current cursor attribute
;TEXT_ATTRIB	fcb	$a2	; current text attribute
;TCSR_ANC_ROW	fcb	0	; beginning row of line currently being edited
;TCSR_ANC_COL	fcb	0	; beginning column of line currently being edited
;TCSR_ANC_ADR	fdb	0	; anchor address
;TCSR_EDT_ADR	fdb	0	; address when enter was pressed during line edit
;DEF_GFX_FLAGS	fcb	$02	; default graphics flags	($02)
;;TCSR_DECAY	fdb	$0000	; counter delay for the cursor
;TCSR_DECAY	fcb	$00	; counter delay for the cursor

user_vars_page	equ	$0100	; user variables page = $0100 - $01ff