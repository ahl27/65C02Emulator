* = $C000

;; Testing all the arithmetic and shifting operations

main:
  ; Addition w/o carry
  lda #$0B
  sta $00
  lda #$70
  sta $01
  clc
  adc $00
  sta $02     ; Should store 7B, no carry

  lda #$AB
  sta $10
  lda #$B7
  sta $11
  clc
  adc $10
  sta $12     ; Should store 62, set carry

  lda #$0A
  sta $21
  lda #$F0
  sta $20
  sec
  sbc $21
  sta $22     ; Should store E6

  ; increment/decrement
  lda #$05
  sta $30
  inc $30     ; inc, should store $06
  tax
  inx
  inx
  stx $31     ; inx, should store $07
  txa
  dex
  stx $32     ; dex, should store $06
  tay
  iny
  sty $33     ; iny, should store $08
  dey
  dey
  sty $34     ; dey, should store $06
  sty $35
  dec $35     ; dec, should store $05

  ; shifts
  clc
  lda #08     ; 0000 1000 = $08
  sta $40
  asl 
  sta $41     ; store $10
  lsr
  sta $42     ; store $08
  sec
  ror
  sta $43     ; 1000 0100 = $84
  sec
  asl
  rol
  sta $44     ; 0001 0001 = $11 (carry bit should shift correctly)

  ; Logical operations
  lda #$0F
  sta $50     ; 0000 1111, lower nibble bitmask
  lda #$19
  and $50
  sta $51     ; 0000 1111 & 0001 1001 = 0000 1001 = $09
  eor $50
  sta $52     ; 0000 1111 ^ 0000 1001 = 0000 0110 = $06
  lda #$E0
  ora $50
  sta $53     ; 0000 1111 | 1110 0000 = 1110 1111 = $EF
  brk


;;; Expected Ending Memory Map:
;;  0000    0B 70 7B 00 00 00 00 00   00 00 00 00 00 00 00 00
;;  0010    AB B7 62 00 00 00 00 00   00 00 00 00 00 00 00 00
;;  0020    F0 0A E6 00 00 00 00 00   00 00 00 00 00 00 00 00
;;  0030    06 07 06 08 06 05 00 00   00 00 00 00 00 00 00 00
;;  0040    08 10 08 84 11 00 00 00   00 00 00 00 00 00 00 00
;;  0050    0F 09 06 EF 00 00 00 00   00 00 00 00 00 00 00 00
;;  0060    00 00 00 00 00 00 00 00   00 00 00 00 00 00 00 00



.dsb $fffa-*,$ff
.word $00
.word $C000
.word $00