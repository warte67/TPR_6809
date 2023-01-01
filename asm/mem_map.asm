        ; **** ********************************************** ****
        ; **** * Allocated 64k Memory Mapped System Symbols * ****
        ; **** ********************************************** ****


        ; **** SOFTWARE Interrupt Vectors: ****
        ; (redefined)

        ; **** Zero-Page Kernal Variables: ****
SYSTEM_VARS     equ     $0010   ; start kernal vectors and variables

        ; **** Stack Frames: ****
U_STK_BTM       equ     $0100   ; 256 bytes default user stack space
U_STK_TOP       equ     $0200   ; User Stack initial address
S_STK_BTM       equ     $0200   ; 512 bytes default system stack space
S_STK_TOP       equ     $0400   ; System Stack initial address

        ; **** Video Buffer Memory (target = $0400): ****
VIDEO_START     equ     $0400   ; Start of Video Buffer Memory
VIDEO_END       equ     $17ff   ; Last Byte of Video Buffer Memory

        ; **** Graphics Hardware Registers: ****
BASE_GFX_REG    equ     $1800   ; Base GFX Hardware Register
GFX_REG2        equ     $1802   ; GFX Register Number Two
GFX_REG3        equ     $1804   ; GFX Register Number Three
RESERVED_HDW    equ     $2000   ; Reserved ($2000-$1FFB)

        ; **** Memory Bank Selects (16MB): ****
RAMBANK_SEL_1   equ     $1ffc   ; (Word)Indexes 65536 x 8kb banks
RAMBANK_SEL_2   equ     $1ffe   ; (Word)Indexes 65536 x 8kb banks

        ; **** Standard Usable (from FAST static 32KB) RAM: ****
RAM_START       equ     $2000   ; Begin System RAM (32k)
RAM_END equ     $9fff   ; End System RAM

        ; **** Switchable RAM Banks (from SLOW external serial 16MB RAM chip): ****
RAM_BANK_1      equ     $a000   ; switched 8KB ram bank 1
RAM_BANK_2      equ     $c000   ; switched 8KB ram bank 2

        ; **** Bios Kernal ROM: ****
BIOS_ROM        equ     $e000   ; Begin BIOS Kernal ROM (8KB)

        ; **** Hardware Interrupt Vectors: ****
        ; (redefined)