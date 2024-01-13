


	push	codec
	call	[printf]

	push	stringpointer
	call	[query]
	cmp	eax,0
	jnz	.codecdetected

	push	nocodec
	call	[printf]
	jmp	.end

	.codecdetected:
	mov	eax,[stringpointer]
	mov	ecx,[eax]
	mov	[querycodec],ecx
	push	querycodec
	call	[printf]
	jmp	.end