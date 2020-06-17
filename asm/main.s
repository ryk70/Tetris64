;
; File generated by cc65 v 2.17 - Git N/A
;
	.fopt		compiler,"cc65 v 2.17 - Git N/A"
	.setcpu		"6502"
	.smart		on
	.autoimport	on
	.case		on
	.debuginfo	off
	.importzp	sp, sreg, regsave, regbank
	.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
	.macpack	longbranch
	.forceimport	__STARTUP__
	.import		_rand
	.import		__randomize
	.import		_clrscr
	.import		_kbhit
	.import		_gotoxy
	.import		_cputcxy
	.import		_cprintf
	.import		_cgetc
	.import		_cpeekc
	.import		_cursor
	.import		_textcolor
	.import		_bgcolor
	.import		_bordercolor
	.import		_cclearxy
	.import		_screensize
	.import		_strlen
	.export		_main

.segment	"DATA"

_i:
	.byte	$00
_j:
	.byte	$00
_isPlaced:
	.byte	$00
_blockTile:
	.byte	$A9

.segment	"RODATA"

_Title:
	.byte	$D4,$C5,$D4,$D2,$C9,$D3,$36,$34,$00
_Inst:
	.byte	$D0,$D2,$C5,$D3,$D3,$20,$D8,$20,$D4,$CF,$20,$D0,$CC,$C1,$D9,$00
L0026:
	.byte	$25,$53,$00
L001D	:=	L0026+0

.segment	"BSS"

_xdim:
	.res	1,$00
_ydim:
	.res	1,$00
_isGameOver:
	.res	1,$00
_curTet:
	.res	1,$00
_curPos:
	.res	8,$00
_linesCleared:
	.res	2,$00
_xTet:
	.res	1,$00
_yTet:
	.res	1,$00
_curColor:
	.res	1,$00
_initPlacement:
	.res	1,$00
_rotState:
	.res	1,$00
_dirKey:
	.res	1,$00

; ---------------------------------------------------------------
; void __near__ draw_title (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_draw_title: near

.segment	"CODE"

	jsr     _clrscr
	lda     #$00
	jsr     _bgcolor
	lda     #$00
	jsr     _bordercolor
	lda     #$01
	jsr     _textcolor
	lda     _xdim
	jsr     pusha0
	lda     #<(_Title)
	ldx     #>(_Title)
	jsr     _strlen
	jsr     tossubax
	jsr     shrax1
	jsr     pusha
	lda     _ydim
	lsr     a
	sec
	sbc     #$01
	jsr     _gotoxy
	lda     #<(L001D)
	ldx     #>(L001D)
	jsr     pushax
	lda     #<(_Title)
	ldx     #>(_Title)
	jsr     pushax
	ldy     #$04
	jsr     _cprintf
	lda     _xdim
	jsr     pusha0
	lda     #<(_Inst)
	ldx     #>(_Inst)
	jsr     _strlen
	jsr     tossubax
	jsr     shrax1
	jsr     pusha
	lda     _ydim
	lsr     a
	jsr     _gotoxy
	lda     #<(L0026)
	ldx     #>(L0026)
	jsr     pushax
	lda     #<(_Inst)
	ldx     #>(_Inst)
	jsr     pushax
	ldy     #$04
	jmp     _cprintf

.endproc

; ---------------------------------------------------------------
; void __near__ draw_game (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_draw_game: near

.segment	"CODE"

	jsr     _clrscr
	lda     #$01
	sta     _i
	jmp     L0310
L030F:	lda     _i
	clc
	adc     #$05
	jsr     pusha
	lda     #$02
	jsr     pusha
	lda     #$EF
	jsr     _cputcxy
	inc     _i
L0310:	lda     _i
	cmp     #$0B
	bcc     L030F
	lda     #$01
	sta     _i
	jmp     L0312
L0311:	lda     _i
	clc
	adc     #$05
	jsr     pusha
	lda     #$17
	jsr     pusha
	lda     #$F7
	jsr     _cputcxy
	inc     _i
L0312:	lda     _i
	cmp     #$0B
	bcc     L0311
	lda     #$01
	sta     _i
	jmp     L0313
L0043:	lda     #$05
	jsr     pusha
	lda     _i
	clc
	adc     #$02
	jsr     pusha
	lda     #$EA
	jsr     _cputcxy
	inc     _i
L0313:	lda     _i
	cmp     #$15
	bcc     L0043
	lda     #$01
	sta     _i
	jmp     L0314
L004E:	lda     #$10
	jsr     pusha
	lda     _i
	clc
	adc     #$02
	jsr     pusha
	lda     #$F4
	jsr     _cputcxy
	inc     _i
L0314:	lda     _i
	cmp     #$15
	bcc     L004E
	lda     #$03
	sta     _j
	lda     #$07
	sta     _i
	jmp     L0316
L005F:	lda     _i
	jsr     pusha
	lda     _j
	jsr     pusha
	lda     #$20
	jsr     _cputcxy
	inc     _j
L0315:	lda     _j
	cmp     #$17
	bcc     L005F
	inc     _i
L0316:	lda     _i
	cmp     #$0F
	bcc     L0315
	rts

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ checkIfEmpty (unsigned char, unsigned char)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_checkIfEmpty: near

.segment	"CODE"

	jsr     pusha
	ldy     #$01
	lda     (sp),y
	jsr     pusha
	ldy     #$01
	lda     (sp),y
	jsr     _gotoxy
	jsr     _cpeekc
	cmp     #$20
	beq     L006D
	lda     #$01
	sta     _isGameOver
	ldx     #$00
	jmp     incsp2
L006D:	ldx     #$00
	txa
	jmp     incsp2

.endproc

; ---------------------------------------------------------------
; void __near__ drawTet (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_drawTet: near

.segment	"CODE"

	lda     _curTet
	bne     L031D
	lda     #$0E
	sta     _curColor
	lda     #$09
	sta     _curPos
	lda     #$03
	sta     _curPos+1
	lda     #$0A
	sta     _curPos+2
	lda     #$03
	sta     _curPos+3
	lda     #$0B
	sta     _curPos+4
	lda     #$03
	sta     _curPos+5
	lda     #$0C
	sta     _curPos+6
	lda     #$03
	sta     _curPos+7
	lda     #$09
	sta     _xTet
	lda     #$04
	sta     _yTet
L031D:	lda     _curTet
	cmp     #$01
	bne     L031E
	lda     #$07
	sta     _curColor
	lda     #$0A
	sta     _curPos
	lda     #$03
	sta     _curPos+1
	lda     #$0B
	sta     _curPos+2
	lda     #$03
	sta     _curPos+3
	lda     #$0A
	sta     _curPos+4
	lda     #$04
	sta     _curPos+5
	lda     #$0B
	sta     _curPos+6
	lda     #$04
	sta     _curPos+7
	lda     #$0A
	sta     _xTet
	lda     #$04
	sta     _yTet
L031E:	lda     _curTet
	cmp     #$02
	bne     L031F
	lda     #$04
	sta     _curColor
	lda     _initPlacement
	sta     _curPos
	lda     #$03
	sta     _curPos+1
	lda     _initPlacement
	clc
	adc     #$01
	sta     _curPos+2
	lda     #$03
	sta     _curPos+3
	lda     _initPlacement
	clc
	adc     #$02
	sta     _curPos+4
	lda     #$03
	sta     _curPos+5
	lda     _initPlacement
	clc
	adc     #$01
	sta     _curPos+6
	lda     #$04
	sta     _curPos+7
	lda     _initPlacement
	sta     _xTet
	lda     #$04
	sta     _yTet
L031F:	lda     _curTet
	cmp     #$03
	bne     L0320
	lda     #$05
	sta     _curColor
	lda     _initPlacement
	sta     _curPos
	lda     #$04
	sta     _curPos+1
	lda     _initPlacement
	clc
	adc     #$01
	sta     _curPos+2
	lda     #$04
	sta     _curPos+3
	lda     _initPlacement
	clc
	adc     #$01
	sta     _curPos+4
	lda     #$03
	sta     _curPos+5
	lda     _initPlacement
	clc
	adc     #$02
	sta     _curPos+6
	lda     #$03
	sta     _curPos+7
	lda     _initPlacement
	sta     _xTet
	lda     #$04
	sta     _yTet
L0320:	lda     _curTet
	cmp     #$04
	bne     L0321
	lda     #$02
	sta     _curColor
	lda     _initPlacement
	sta     _curPos
	lda     #$03
	sta     _curPos+1
	lda     _initPlacement
	clc
	adc     #$01
	sta     _curPos+2
	lda     #$03
	sta     _curPos+3
	lda     _initPlacement
	clc
	adc     #$01
	sta     _curPos+4
	lda     #$04
	sta     _curPos+5
	lda     _initPlacement
	clc
	adc     #$02
	sta     _curPos+6
	lda     #$04
	sta     _curPos+7
	lda     _initPlacement
	sta     _xTet
	lda     #$04
	sta     _yTet
L0321:	lda     _curTet
	cmp     #$05
	bne     L0322
	lda     #$08
	sta     _curColor
	lda     _initPlacement
	sta     _curPos
	lda     #$03
	sta     _curPos+1
	lda     _initPlacement
	sta     _curPos+2
	lda     #$04
	sta     _curPos+3
	lda     _initPlacement
	clc
	adc     #$01
	sta     _curPos+4
	lda     #$04
	sta     _curPos+5
	lda     _initPlacement
	clc
	adc     #$02
	sta     _curPos+6
	lda     #$04
	sta     _curPos+7
	lda     _initPlacement
	sta     _xTet
	lda     #$04
	sta     _yTet
L0322:	lda     _curTet
	cmp     #$06
	bne     L0323
	sta     _curColor
	lda     _initPlacement
	sta     _curPos
	lda     #$03
	sta     _curPos+1
	lda     _initPlacement
	sta     _curPos+2
	lda     #$04
	sta     _curPos+3
	lda     _initPlacement
	clc
	adc     #$01
	sta     _curPos+4
	lda     #$04
	sta     _curPos+5
	lda     _initPlacement
	clc
	adc     #$02
	sta     _curPos+6
	lda     #$04
	sta     _curPos+7
	lda     _initPlacement
	sta     _xTet
	lda     #$04
	sta     _yTet
L0323:	lda     #$00
	sta     _rotState
	tax
	sta     _i
	jmp     L0325
L0324:	lda     _i
	asl     a
	bcc     L0317
	ldx     #$01
L0317:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L031B
	inx
	clc
L031B:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     _checkIfEmpty
	cmp     #$01
	beq     L01A2
	inc     _i
	ldx     #$00
L0325:	lda     _i
	cmp     #$04
	bcc     L0324
	stx     _i
	lda     _curColor
	jsr     _textcolor
	jmp     L01A3
L0326:	lda     _i
	asl     a
	bcc     L0319
	ldx     #$01
L0319:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L031C
	inx
	clc
L031C:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     pusha
	lda     _blockTile
	jsr     _cputcxy
	inc     _i
L01A3:	ldx     #$00
	lda     _i
	cmp     #$04
	bcc     L0326
L01A2:	rts

.endproc

; ---------------------------------------------------------------
; void __near__ pickTet (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_pickTet: near

.segment	"CODE"

	jsr     __randomize
	jsr     _rand
	jsr     pushax
	ldx     #$00
	lda     #$07
	jsr     tosmoda0
	sta     _curTet
	jsr     _rand
	jsr     pushax
	ldx     #$00
	lda     #$07
	jsr     tosmoda0
	clc
	adc     #$06
	sta     _initPlacement
	rts

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ checkBotCollision (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_checkBotCollision: near

.segment	"CODE"

	lda     #$00
	jsr     pusha
	tax
	sta     _i
	jmp     L0333
L0331:	lda     _i
	asl     a
	bcc     L032D
	ldx     #$01
	clc
L032D:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	ldx     #$00
	lda     (ptr1),y
	sec
	dey
	sbc     (sp),y
	bcc     L0332
	beq     L0332
	lda     _i
	asl     a
	bcc     L032E
	inx
	clc
L032E:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	iny
	lda     (ptr1),y
	dey
	sta     (sp),y
L0332:	inc     _i
	ldx     #$00
L0333:	lda     _i
	cmp     #$04
	bcc     L0331
	stx     _i
	jmp     L0336
L0334:	lda     _i
	asl     a
	bcc     L032F
	ldx     #$01
	clc
L032F:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	ldx     #$00
	lda     (ptr1),y
	dey
	cmp     (sp),y
	bcc     L01D8
	lda     _i
	asl     a
	bcc     L032A
	inx
L032A:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L0330
	inx
	clc
L0330:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	clc
	adc     #$01
	jsr     _gotoxy
	jsr     _cpeekc
	cmp     #$20
	beq     L01D8
	ldx     #$00
	lda     #$01
	jmp     incsp1
L01D8:	inc     _i
	ldx     #$00
L0336:	lda     _i
	cmp     #$04
	bcc     L0334
	txa
	jmp     incsp1

.endproc

; ---------------------------------------------------------------
; void __near__ moveTet (unsigned char)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_moveTet: near

.segment	"CODE"

	jsr     pusha
	ldy     #$00
	lda     (sp),y
	jne     L0358
	sta     _i
	jmp     L0353
L01E2:	lda     #$00
	jsr     _textcolor
	ldx     #$00
	lda     _i
	asl     a
	bcc     L0337
	inx
L0337:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L0349
	inx
	clc
L0349:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     pusha
	lda     #$01
	jsr     _cclearxy
	inc     _i
L0353:	lda     _i
	cmp     #$04
	bcc     L01E2
	ldx     #$00
	stx     _i
	jmp     L0355
L0354:	lda     _i
	asl     a
	bcc     L034A
	ldx     #$01
	clc
L034A:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L034B
	inx
	clc
L034B:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$00
	lda     (sp),y
	clc
	adc     #$01
	iny
	sta     (ptr1),y
	inc     _i
	jsr     incsp1
	ldx     #$00
L0355:	lda     _i
	cmp     #$04
	bcc     L0354
	stx     _i
	lda     _curColor
	jsr     _textcolor
	jmp     L0205
L0356:	lda     _i
	asl     a
	bcc     L033B
	ldx     #$01
L033B:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L034C
	inx
	clc
L034C:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     pusha
	lda     #$A9
	jsr     _cputcxy
	inc     _i
L0205:	ldx     #$00
	lda     _i
	cmp     #$04
	bcc     L0356
	ldy     #$00
L0358:	lda     (sp),y
	cmp     #$01
	jne     L035E
	sty     _i
	jmp     L0359
L0214:	lda     #$00
	jsr     _textcolor
	ldx     #$00
	lda     _i
	asl     a
	bcc     L033D
	inx
L033D:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L034D
	inx
	clc
L034D:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     pusha
	lda     #$01
	jsr     _cclearxy
	inc     _i
L0359:	lda     _i
	cmp     #$04
	bcc     L0214
	ldx     #$00
	stx     _i
	jmp     L035B
L035A:	lda     _i
	asl     a
	bcc     L033F
	ldx     #$01
L033F:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L034E
	inx
	clc
L034E:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$00
	lda     (sp),y
	sec
	sbc     #$01
	sta     (ptr1),y
	inc     _i
	jsr     incsp1
	ldx     #$00
L035B:	lda     _i
	cmp     #$04
	bcc     L035A
	stx     _i
	lda     _curColor
	jsr     _textcolor
	jmp     L0237
L035C:	lda     _i
	asl     a
	bcc     L0341
	ldx     #$01
L0341:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L034F
	inx
	clc
L034F:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     pusha
	lda     #$A9
	jsr     _cputcxy
	inc     _i
L0237:	ldx     #$00
	lda     _i
	cmp     #$04
	bcc     L035C
	ldy     #$00
L035E:	lda     (sp),y
	cmp     #$02
	beq     L0363
	jmp     incsp1
L0363:	sty     _i
	jmp     L035F
L0246:	lda     #$00
	jsr     _textcolor
	ldx     #$00
	lda     _i
	asl     a
	bcc     L0343
	inx
L0343:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L0350
	inx
	clc
L0350:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     pusha
	lda     #$01
	jsr     _cclearxy
	inc     _i
L035F:	lda     _i
	cmp     #$04
	bcc     L0246
	ldx     #$00
	stx     _i
	jmp     L0361
L0360:	lda     _i
	asl     a
	bcc     L0345
	ldx     #$01
L0345:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L0351
	inx
	clc
L0351:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$00
	lda     (sp),y
	clc
	adc     #$01
	sta     (ptr1),y
	inc     _i
	jsr     incsp1
	ldx     #$00
L0361:	lda     _i
	cmp     #$04
	bcc     L0360
	stx     _i
	lda     _curColor
	jsr     _textcolor
	jmp     L0269
L0362:	lda     _i
	asl     a
	bcc     L0347
	ldx     #$01
L0347:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L0352
	inx
	clc
L0352:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     pusha
	lda     #$A9
	jsr     _cputcxy
	inc     _i
L0269:	ldx     #$00
	lda     _i
	cmp     #$04
	bcc     L0362
	jmp     incsp1

.endproc

; ---------------------------------------------------------------
; void __near__ handleTet (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_handleTet: near

.segment	"CODE"

	jsr     _checkBotCollision
	cmp     #$01
	bne     L0275
	sta     _isPlaced
	rts
L0275:	lda     #$00
	jmp     _moveTet

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ checkLeftCollision (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_checkLeftCollision: near

.segment	"CODE"

	lda     #$FF
	jsr     pusha
	ldx     #$00
	stx     _i
	jmp     L036D
L036B:	lda     _i
	asl     a
	bcc     L0364
	ldx     #$01
L0364:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	ldx     #$00
	lda     (ptr1),y
	ldy     #$00
	cmp     (sp),y
	bcs     L036C
	lda     _i
	asl     a
	bcc     L0365
	inx
L0365:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	ldy     #$00
	sta     (sp),y
L036C:	inc     _i
	ldx     #$00
L036D:	lda     _i
	cmp     #$04
	bcc     L036B
	stx     _i
	jmp     L0370
L036E:	lda     _i
	asl     a
	bcc     L0366
	ldx     #$01
L0366:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	ldx     #$00
	lda     (ptr1),y
	sec
	ldy     #$00
	sbc     (sp),y
	bcc     L036F
	bne     L029F
L036F:	lda     _i
	asl     a
	bcc     L0367
	inx
L0367:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	sec
	sbc     #$01
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L036A
	inx
	clc
L036A:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     _gotoxy
	jsr     _cpeekc
	cmp     #$20
	beq     L029F
	ldx     #$00
	lda     #$01
	jmp     incsp1
L029F:	inc     _i
	ldx     #$00
L0370:	lda     _i
	cmp     #$04
	bcc     L036E
	txa
	jmp     incsp1

.endproc

; ---------------------------------------------------------------
; unsigned char __near__ checkRightCollision (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_checkRightCollision: near

.segment	"CODE"

	lda     #$00
	jsr     pusha
	tax
	sta     _i
	jmp     L037A
L0378:	lda     _i
	asl     a
	bcc     L0371
	ldx     #$01
L0371:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	ldx     #$00
	lda     (ptr1),y
	sec
	ldy     #$00
	sbc     (sp),y
	bcc     L0379
	beq     L0379
	lda     _i
	asl     a
	bcc     L0372
	inx
L0372:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	ldy     #$00
	sta     (sp),y
L0379:	inc     _i
	ldx     #$00
L037A:	lda     _i
	cmp     #$04
	bcc     L0378
	stx     _i
	jmp     L037D
L037B:	lda     _i
	asl     a
	bcc     L0373
	ldx     #$01
L0373:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	ldx     #$00
	lda     (ptr1),y
	ldy     #$00
	cmp     (sp),y
	bcc     L02C7
	lda     _i
	asl     a
	bcc     L0374
	inx
L0374:	sta     ptr1
	txa
	clc
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #<(_curPos)
	lda     (ptr1),y
	clc
	adc     #$01
	jsr     pusha
	ldx     #$00
	lda     _i
	asl     a
	bcc     L0377
	inx
	clc
L0377:	adc     #<(_curPos)
	sta     ptr1
	txa
	adc     #>(_curPos)
	sta     ptr1+1
	ldy     #$01
	lda     (ptr1),y
	jsr     _gotoxy
	jsr     _cpeekc
	cmp     #$20
	beq     L02C7
	ldx     #$00
	lda     #$01
	jmp     incsp1
L02C7:	inc     _i
	ldx     #$00
L037D:	lda     _i
	cmp     #$04
	bcc     L037B
	txa
	jmp     incsp1

.endproc

; ---------------------------------------------------------------
; void __near__ checkMove (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_checkMove: near

.segment	"CODE"

	jsr     _kbhit
	cmp     #$01
	bne     L02DF
	jsr     _cgetc
	sta     _dirKey
	cmp     #$41
	bne     L037E
	jsr     _checkLeftCollision
	cmp     #$00
	bne     L037E
	lda     #$01
	jsr     _moveTet
L037E:	lda     _dirKey
	cmp     #$44
	bne     L037F
	jsr     _checkRightCollision
	cmp     #$00
	bne     L037F
	lda     #$02
	jsr     _moveTet
L037F:	lda     _dirKey
	cmp     #$53
	bne     L02DF
	jsr     _checkBotCollision
	cmp     #$00
	jeq     _moveTet
L02DF:	rts

.endproc

; ---------------------------------------------------------------
; void __near__ checkRot (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_checkRot: near

.segment	"CODE"

	jmp     _kbhit

.endproc

; ---------------------------------------------------------------
; void __near__ game_loop (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_game_loop: near

.segment	"CODE"

	jsr     _pickTet
	jsr     _drawTet
	jmp     L02EB
L0380:	lda     _isPlaced
	cmp     #$01
	bne     L02ED
	jsr     _pickTet
	jsr     _drawTet
	lda     #$00
	sta     _isPlaced
L02ED:	jsr     _checkMove
	jsr     _checkRot
	jsr     _handleTet
	lda     #0
	sta     162
wait:	lda     162
	cmp     #$1E
	bne     wait
L02EB:	lda     _isGameOver
	cmp     #$01
	bne     L0380
	rts

.endproc

; ---------------------------------------------------------------
; int __near__ main (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_main: near

.segment	"CODE"

	lda     #$00
	jsr     _cursor
	lda     #<(_xdim)
	ldx     #>(_xdim)
	jsr     pushax
	lda     #<(_ydim)
	ldx     #>(_ydim)
	jsr     _screensize
	jsr     _draw_title
L0308:	jsr     _cgetc
	cmp     #$58
	bne     L0308
	jsr     _draw_game
	jsr     _game_loop
	ldx     #$00
	txa
	rts

.endproc

