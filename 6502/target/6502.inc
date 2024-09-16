; $$$$$$$$$$$$$$ MOS 6502 ASSEMBLER $$$$$$$$$$$$$$
; WRITTEN FOR FASMG: \EXAMPLES\MOS. BY CODESTAR

;     .6*"
;   ,M' 
;  ,Mbmmm.    M******  ,pP""Yq.   pd*"*b.
;  6M'  `Mb. .M       6W'    `Wb (O)   j8
;  MI     M8 |bMMAg.  8M      M8     ,;j9
;  WM.   ,M9      `Mb YA.    ,A9  ,-='
;   WMbmmd9        jM  `Ybmmd9'  Ammmmmmm
;           (O)  ,M9
;            6mmm9

; a               - accumulator
; x, y            - general
; status          - flags: SV_BDIZC
; program counter
; stack register

; $00-$FF     - zero page "memory registers"
; $0100-$01FF - stack, 1K

; in some emulators:

; $FE         - random byte
; $FF         - last ASCII key
; $0200-$05FF - screen, 32x32

;;;;;;;;;;;;;;;;;;;; VERIFY ;;;;;;;;;;;;;;;;;;;;;;

macro verify_n n, min, max
  if ~n eqtype 0
    err 'Number expected'
  end if
  if n<min | n>max
    err 'Number exceeds range: '
  end if
end macro

irp <name,min,max>,\
  u8,0,$FF, i8,-$80,$7F,\
  u16,0,$FFFF, i16,-$8000,$7FFF
  macro verify_#name n
    verify_n n, min, max
  end macro
end irp

macro expect a, b
  match =b, a
  else
    display `a, ' - ', `b, ' expected'
    err ''
  end match
end macro

;;;;;;;;;;;;;; ONE-BYTE INSTRUCTIONS ;;;;;;;;;;;;;

irp <name,o>, brk,$00,\
  clc,$18, cld,$D8, cli,$58, clv,$B8,\
  dex,$CA, dey,$88, inx,$E8, iny,$C8,\
  nop,$EA, pha,$48, php,$08, pla,$68,\
  plp,$28, rti,$40, rts,$60, sec,$38,\
  sed,$F8, sei,$78, tax,$AA, tay,$A8,\
  tsx,$BA, txa,$8A, txs,$9A, tya,$98
  macro name
    db o
  end macro
end irp

;;;;;;;;;;;;;;;;;;;;; BRANCH ;;;;;;;;;;;;;;;;;;;;;

macro jmp a
  match (x), a    ; indirect
    db $6C
    dw x
  else match x, a ; absolute
    db $4C
    dw x
  end match
end macro

irp <name,o>,\
  beq,$F0, bne,$D0, bcc,$90, bcs,$B0,\
  bvc,$50, bvs,$70, bmi,$30, bpl,$10
  macro name a
    db o, (255-($-a)) and $FF
  end macro
end irp

macro jsr a ; call subroutine
  db $20
  dw a      ; absolute
end macro

;;;;;;;;;;;;;;;;;;;; INC, DEC ;;;;;;;;;;;;;;;;;;;;

; opc $80      ; zero page
; opc $80, x   ; zero page, x
; opc $A000    ; absolute
; opc $8000, x ; absolute, x

macro mos_id name, aaa, p
  local i, mode, size
  i=0
  mode=0
  size=0
  match a=,b, p         ; n, x
    expect b, x
    i=a
    if i>=0 & i<=$FF
      size=1
      mode=101b
    else
      size=2
      mode=111b
    end if
  else match a, p       ; n
    i=a
    if i>=0 & i<=$FF
      size=1
      mode=001b
    else
      size=2
      mode=011b
    end if
  else
    err 'Error'
  end match
  verify_u16 i
  db (aaa shl 5) or \
   (mode shl 2) or 2
  if size=1             ; zero page
    db i
  else                  ; absolute
    dw i
  end if
end macro

macro inc p&
  mos_id inc, 111b, <p>
end macro

macro dec p&
  mos_id dec, 110b, <p>
end macro

;;;;;;;;;;;;;;; ASL, LSR, ROL, ROR ;;;;;;;;;;;;;;;

; shift, rotate

; opc          ; accumulator
; opc $80      ; zero page
; opc $80, x   ; zero page, x
; opc $A000    ; absolute
; opc $8000, x ; absolute, x

macro mos_s name, aaa, p
  local i, mode, size
  i=0
  mode=0
  size=0
  match a=,b, p        ; n, x
    expect b, x
    i=a
    if i>=0 & i<=$FF
      size=1
      mode=101b
    else
      size=2
      mode=111b
    end if
  else match _a, p     ; x
    match =a, _a       ; accumulator
      size=1
      mode=010b
    else
      i=_a
      if i>=0 & i<=$FF
        size=1
        mode=001b
      else
        size=2
        mode=011b
      end if
    end match
  else                 ; accumulator
    size=1
    mode=010b
  end match
  db (aaa shl 5) or \
   (mode shl 2) or 2
  if mode<>010b        ; not accumulator?
    verify_u16 i
    if size=1          ; zero page
      db i
    else               ; absolute
      dw i
    end if
  end if
end macro

irp <name,o>,\
  asl,000b, rol,001b, lsr,010b, ror,011b
  macro name p&
    mos_s name, o, <p>
  end macro
end irp

;;;;;;;; LDA, STA, ADC, SBC, ORA, CMP, ETC ;;;;;;;

; basic arithmetic

; opc #$44     ; immediate
; opc $AA      ; zero page
; opc $7F, x   ; zero page, x
; opc $4FFF    ; absolute
; opc $88BB, x ; absolute, x
; opc $24EE, y ; absolute, y
; opc ($AC, x) ; (indirect, x)
; opc ($DC), y ; (indirect), y

macro mos_a name, aaa, p
  local i, mode, size
  i=0
  mode=0
  size=0
  match #a, p             ; immediate
    i=a
    mode=010b
    size=1
    match =sta, name
      err 'Invalid'
    end match
  else match (a=,b), p    ; (indirect, x)
    expect b, x
    i=a
    mode=000b
    size=1
  else match (a)=,b, p    ; (indirect), y
    expect b, y
    i=a
    mode=100b
    size=1
  else match a=,b, p      ; ?, ?
    i=a
    verify_u16 i          ; i, ?
    match =x, b           ; i, x
      if i>=0 & i<=$FF    ; zero page, x
        mode=101b
        size=1
      else                ; absolute, x
        mode=111b
        size=2
      end if
    else match =y, b      ; absolute, y
      mode=110b
      size=2
    else
      err 'Error'
    end match
  else match a, p
    i=a
    verify_u16 i
    if i>=0 & i<=$FF      ; zero page
      mode=001b
      size=1
    else                  ; absolute
      mode=011b
      size=2
    end if
  else
    err 'Error'
  end match
  db (aaa shl 5) or \
   (mode shl 2) or 1
  if size=1
    db i
  else
    dw i
  end if
end macro

irp <name,o>,\
  ora,000b, and,001b, eor,010b, adc,011b,\
  sta,100b, lda,101b, cmp,110b, sbc,111b
  macro name p&
    mos_a name, o, <p>
  end macro
end irp

;;;;;;;;;;;;;;;;;;;; STX, LDX ;;;;;;;;;;;;;;;;;;;;

; load/store x

; opc #$44     ; immediate
; opc $80      ; zero page
; opc $80, y   ; zero page, y
; opc $AAAA    ; absolute
; opc $AAAA, y ; absolute, y

macro mos_lsx name, aaa, p
  local i, mode, size
  i=0
  mode=0
  size=0
  match #a, p          ; immediate
    i=a
    mode=000b
    size=1
    match =stx, name
      err 'Error'
    end match
  else match a=,b, p   ; ?, ?
    i=a
    verify_u16 i       ; i, ?
    match =y, b        ; i, y
      if i>=0 & i<=$FF ; zero page, y
        mode=101b
        size=1
      else             ; absolute, y
        mode=111b
        size=2
        match =stx, name
          err 'Error'
        end match
      end if
    else
      err 'Error'
    end match
  else match a, p
    i=a
    verify_u16 i
    if i>=0 & i<=$FF   ; zero page
      mode=001b
      size=1
    else
      mode=011b        ; absolute
      size=2
    end if
  else
    err 'Error'
  end match
  db (aaa shl 5) or \
   (mode shl 2) or 2
  if size=1
    db i
  else
    dw i
  end if
end macro

macro stx p&
  mos_lsx stx, 100b, <p>
end macro

macro ldx p&
  mos_lsx ldx, 101b, <p>
end macro

;;;;;;;;;;;;;;;;;;;; STY, LDY ;;;;;;;;;;;;;;;;;;;;

; opc #$44     ; immediate
; opc $AA      ; zero page
; opc $7F, x   ; zero page, x
; opc $4FFF    ; absolute
; opc $88BB, x ; absolute, x

macro mos_lsy name, aaa, p
  local i, mode, size
  i=0
  mode=0
  size=0
  match #a, p        ; immediate
    i=a
    mode=000b
    size=1
    match =sty, name
      err 'Error'
    end match
  else match a=,b, p ; ?, ?
    i=a
    verify_u16 i     ; i, ?
    match =x, b      ; i, x
      if i>=0 \
        & i<=$FF     ; zero page, x
        mode=101b
        size=1
      else           ; absolute, x
        mode=111b
        size=2
      end if
    else
      err 'Error'
    end match
  else match a, p
    i=a
    verify_u16 i
    if i>=0 & i<=$FF ; zero page
      mode=001b
      size=1
    else             ; absolute
      mode=011b
      size=2
    end if
  else
    err 'Error'
  end match
  db (aaa shl 5) or \
   (mode shl 2)
  if size=1
    db i
  else
    dw i
  end if
end macro

macro sty p&
  mos_lsy sty, 100b, <p>
end macro

macro ldy p&
  mos_lsy ldy, 101b, <p>
end macro

;;;;;;;;;;;;;;;;;;;; CPX, CPY ;;;;;;;;;;;;;;;;;;;;

; compare x/y

; opc #$44     ; immediate
; opc $AA      ; zero page
; opc $4FFF    ; absolute

macro mos_c name, aaa, p
  local i, mode, size
  i=0
  mode=0
  size=0
  match #a, p      ; immediate
    verify_u8 a
    i=a
    mode=000b
    size=1
  else match a, p  ; x
    i=a
    verify_u16 i
    if i>=0 \
      & i<=$FF     ; zero page
      mode=001b
      size=1
    else           ; absolute
      mode=011b
      size=2
    end if
  else
    err 'Error'
  end match
  db (aaa shl 5) \
   or (mode shl 2)
  if size=1
    db i
  else
    dw i
  end if
end macro

macro cpx p&
  mos_c cpx, 111b, <p>
end macro

macro cpy p&
  mos_c cpy, 110b, <p>
end macro

;;;;;;;;;;;;;;;;;;;;;;; BIT ;;;;;;;;;;;;;;;;;;;;;;

macro bit i
  verify_u16 i
  if i>=0 & i<=$FF ; zero page
    db $24, i
  else             ; absolute
    db $2C
    dw i
  end if
end macro