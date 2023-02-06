* = $C000

;; Jump/Branching operations

main:
  lda #$0

  ; Testing jmp, should store $45 at $00
  ; Obviously jsr/rts is better, but trying to test jmp
  jmp setA45
  lda #$0
returnfromjmp:
  sta $00

  ; Testing jsr/rts/bne
  jsr testSubroutine
  
  ; Testing all other branching commands
  jsr testBranches
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
  rts

testBranches:
  ; BCC / BCS
  ; Should store $F0-FF in 20-2F
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

  ; BMI/BPL/BEQ
  ; Should store F0-F5 in 30-35 and FF in 36
  clc
  lda #$F0
  ldx #$0
  .(
    loop:
      sta $30,X
      adc #$1
      inx
      cmp $25 ; F5
      beq specialcase
      bmi loop
      bpl end
    specialcase:
      lda #$FF
      jmp loop
    end:
  .)

  ; BVC / BVS
  ; Should fill in values from 40-46
  clv
  lda #77
  ldx #0
  .(
    loop:
      bvc add
      bvs end
    add:
      clc
      clv
      adc #10
      sta $40,X
      inx
      jmp loop
    end:
  .)

  rts

;;; Expected Ending Memory Map:
;;  0000    45 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |E...............|
;;  0010    20 20 20 20 20 00 00 00   00 00 00 00 00 00 00 00   |     ...........|
;;  0020    f0 f1 f2 f3 f4 f5 f6 f7   f8 f9 fa fb fc fd fe ff   |????????????????|
;;  0030    f0 f1 f2 f3 f4 f5 ff 00   00 00 00 00 00 00 00 00   |???????.........|
;;  0040    57 61 6b 75 7f 89 00 00   00 00 00 00 00 00 00 00   |Waku............|
;;  0050    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0060    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|

.dsb $fffa-*,$ff
.word $00
.word $C000
.word $00