; //// pseudo.asm
;


;  command line buffer
; EDT_BUFFER [256]

        A = 0;      ; index
        X = 0;      ; EDT_BUFFER[X]
        Y = 0       ; command_LUT[Y] 
        increment A
loop:
        load b from EDT_BUFFER[X]
        compare b with 0xFF
        branch if EQUAL to failure
        compare b with NULL-TERMINATION
        branch if EQUAL to success
        compare b with command_LUT[Y]
        branch if NOT equal to next_token
        increment X
        increment Y
        branch back to loop      
next_token:
        increment A
        X = 0;
        ;move Y to the beginning of the next label
nt_1:
        branch back to loop        
failure:
        A = 0
success:
        return A

command_LUT:
			fcn		"cls"
			fcn		"load"
			fcn		"exec"
			fcn		"exit"
			fcb		0xFF