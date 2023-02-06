* = $C000

;; Jump/Branching operations

main:
  lda #$0

  ; Testing jmp, should store $45 at $00
  ; Obviously jsr/rts is better, but trying to test jmp
  ;; TODO:  There's a bug here, I'm double decoding address--basically using indirect when it shouldn't be
  ;; Not sure if I can change it without affecting other stuff
  jmp setA45
  lda #$0
returnfromjmp:
  sta $00

  ; Testing jsr/rts/bne
  ;jsr testSubroutine
  ; Testing all other branching commands
  jsr testBranches
  jsr testSubroutine
  brk



setA45:
  lda #$45
  jmp returnfromjmp

testSubroutine:
  ; Should store $20 at positions $0010-0015
  ldx #$05
  lda #$20
  .(
    loop:
      dex
      sta $10,X
      bne loop
  .)
  ;brk
  rts

testBranches:
  ; BCC / BCS
  ; Should store $F0-FF in 20-2F
  ;brk
  ldx #$00
  lda #$F0
  clc
  .(
    loop:
      sta $20,X
      inx
      adc #$1
      bcc loop
      bcs end
      lda #$AA
      jmp loop
    end:
  .)
  ;brk
  rts



;;; Expected Ending Memory Map:
;;  0000    00 00 00 00 00 00 00 00   06 00 00 00 0e 00 10 00   |................|
;;  0010    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0020    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0030    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0040    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0050    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0060    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|

.dsb $fffa-*,$ff
.word $00
.word $C000
.word $00