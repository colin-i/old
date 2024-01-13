


proc size
	cmp	[flag],2 ;at capture8
	jz	.next4

	mov	[sw],0	; for example: 0 is for 01xx xxxx  xxxx xxxx and 1 is for 0100 0000  xxxx xxxx

	cmp	ecx,0x7F
	ja	.next1
	mov	eax,1
	mov	[marker],0x80	 ;the unicode marker
	jmp	.jmp4

	.next1:
	cmp	ecx,0x3FFF
	ja	.next2
	cmp	cx,0xFF
	ja	.jmp1
	mov	[sw],1
	.jmp1:
	mov	al,cl
	mov	cl,ch
	mov	ch,al
	mov	eax,2
	mov	[marker],0x4000
	ror	[marker],8;;the unicode notation will be swaped
	jmp	.jmp4

	.next2:
	cmp	ecx,0x1FFFFF
	ja	.next3
	cmp	ecx,0xFFFF
	ja	.jmp2
	mov	[sw],1
	.jmp2:
	mov	al,cl
	ror	ecx,8
	mov	ah,ch
	mov	ch,al
	rol	ecx,8
	mov	cl,ah
	mov	eax,3
	mov	[marker],0x200000
	ror	[marker],16;;the unicode notation will be swaped
	jmp	.jmp4

	.next3:
	cmp	ecx,0xFFFFFFF
	ja	.next4
	cmp	ecx,0xFFFFFF
	ja	.jmp3
	mov	[sw],1
	.jmp3:
	mov	eax,4
	bswap	ecx
	mov	[marker],0x10000000
	ror	[marker],24;;the unicode notation will be swaped
	jmp	.jmp4

	.next4:
	bswap	ecx
	mov	eax,5
	cmp	[flag],0 ;1 or 2 to .jmp5
	jnz	.jmp5
	mov	[marker],0

	.jmp4:
	cmp	[flag],0
	jnz	.jmpnext1;;jump to no marker
	or	ecx,[marker]
	jmp	.jmpnext2
	.jmpnext1:
	cmp	[sw],0
	jz	.jmpnext2
	ror	ecx,8;;the number is "low" (ex: 0100 0000|xxxx xxxx; excluding above 0000 xxxx xxxx xxxx)
	.jmp5:
	dec	eax
	.jmpnext2:
	mov	[buffer1],ecx
	ret
endp