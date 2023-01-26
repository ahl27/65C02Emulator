* = $C000

; Testing load/store operations

main:
  ;; IMMEDIATE LOADS
  lda #$06      ; lda immediate (value AA)
  ldx #$01      ; ldx immediate (value $02)
  ldy #$02      ; ldy immediate (value $04)

  ; Now we should have A=$AA, X=$02, Y=$04

  ;; STORE A COMMANDS
  sta $00       ; sta zp        (at $0000)
  sta $0001     ; sta abs       (at $0001)
  sta $01,X     ; sta zp,X      (at $0002)
  sta $0002,X   ; sta abs,X     (at $0003)
  sta $0002,Y   ; sta abs,Y     (at $0004)
  sta ($03,X)   ; sta (zp,X)    (memory at $03+$01 is 06 00, so will store at $0006)
  sta ($04),Y   ; sta (zp),Y    (memory at $0004 is 06 00, Y=02, will store at $0008)

  ; Memory from $0000-0004, 0006, 0008 should now be $06

  ;; STORE X COMMANDS
  stx $10       ; stx zp        (at $0010)
  stx $0F,Y     ; stx zp,Y      (at $0011)
  stx $0012     ; stx abs       (at $0012)   

  ; indexes $0010-0012 should be $01

  ;; STORE Y COMMANDS
  sty $20       ; sty zp        (at $0020)
  sty $20,X     ; sty zp,X      (at $0021)
  sty $0022     ; sty abs       (at $0022)

  ; indexes $0020-22 should be $02

  ;; LDA COMMANDS
  lda $10       ; lda zp        (load $01)
  sta $30
  lda $1F,X     ; lda zp,X      (load $02)
  sta $31
  lda $0006     ; lda abs       (load $06)
  sta $32
  lda $000F,X   ; lda abs,X     (load $01)
  sta $33
  lda $001E,Y   ; lda abs,Y     (load $02)
  sta $34
  ; Set up some indirects
  lda #$10
  sta $0E
  lda ($0D,X)   ; lda (zp,X)    (load from ($0D+X) = ($0E) = $1000 => $01) 
  sta $35
  lda #$0E
  sta $0C
  lda ($0C),Y   ; lda (zp),Y    (load from ($0C)+Y = $00F0+$02 = $0100 => $01) 
  sta $36

  ;; LDX Commands
  ldx #$FF
  ldx $10       ; ldx zp        (load $01)
  stx $40
  ldx $1F,Y     ; ldx zp,Y      (load $02)
  stx $41
  ldx $0004     ; ldx abs       (load $06) 
  stx $42
  ldx #$FF
  ldx $000F,Y   ; ldx abs,Y     (load $01)
  stx $43
  ldx #$01

  ;; LDY Commands
  ldy $12       ; ldy zp        (load $01)
  sty $50
  ldy $1F,X     ; ldy zp,X      (load $02)
  sty $51
  ldy $0004     ; ldy abs       (load $06) 
  sty $52
  ldy $000F,X   ; ldy abs,X     (load $01)
  sty $53
  ldy #$02

  ;; TRANSFER COMMANDS
  lda #$00
  ldy #$00
  ldx #$00

  lda #$AA
  tax
  tay
  sta $60
  stx $61
  sty $62

  lda #$00
  txa
  sta $63
  lda #$00
  tya
  sta $64
  tsx
  stx $65
  ldx #$BB
  txs
  ldx #$00
  tsx
  stx $66
  brk


;;; Expected Ending Memory Map:
;;  0000    06 06 06 06 06 00 06 00   06 00 00 00 0e 00 10 00   |................|
;;  0010    01 01 01 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0020    02 02 02 00 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0030    01 02 06 01 02 01 01 00   00 00 00 00 00 00 00 00   |................|
;;  0040    01 02 06 01 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0050    01 02 06 01 00 00 00 00   00 00 00 00 00 00 00 00   |................|
;;  0060    AA AA AA AA AA FF BB 00   00 00 00 00 00 00 00 00   |................|

.dsb $fffa-*,$ff
.word $00
.word $C000
.word $00