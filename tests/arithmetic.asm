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

  brk



.dsb $fffa-*,$ff
.word $00
.word $C000
.word $00