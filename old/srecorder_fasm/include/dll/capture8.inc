





;;the segment size. here is the sum of seek head, segment informations, tracks, clusters and cues
;;                this is the end of the file without EBML head(40), the segment code(4) and its unicode size(8) do not count       = 52

	mov	[flag],2

;;;;;;;;segment size
	push	[SEEK_END]
	push	0
	push	0
	push	[file_handler]
	call	[seek]
	add	esp,16

	sub	eax,52
	jnc	.jumpnotcarry
	dec	edx
	.jumpnotcarry:

	or	edx,0x1000000
	mov	[buffer1],edx
	mov	[buffer2],eax

	push	[SEEK_SET]
	push	0
	push	44
	push	[file_handler]
	call	[seek]
	add	esp,16

	push	4
	push	buffer1
	push	[file_handler]
	mov	ecx,[buffer1]

	call	size
	call	[write]
	cmp	eax,-1
	jnz	.values1
	jmp	.valuesend

	.values1:
	mov	ecx,[buffer2]

	call	size
	call	[write]
	cmp	eax,-1
	jnz	.values2
	jmp	.valuesend
;;;;;;;;
;;
	.values2:
	push	[SEEK_SET]
	push	0
	push	98
	push	[file_handler]
	call	[seek]
	add	esp,16

	mov	ecx,[seekcues_highs]

	call	size
	call	[write]
	cmp	eax,-1
	jnz	.values3
	jmp	.valuesend

	.values3:
	mov	ecx,[seekcues_lows]

	call	size
	call	[write]
	cmp	eax,-1
	jnz	.values4
	jmp	.valuesend
;;
;;;;;;;;
	.values4:

	push	[SEEK_SET]
	push	0
	push	140
	push	[file_handler]
	call	[seek]
	add	esp,16

	mov	ecx,[cl_timecode]

	call	size
	call	[write]
	cmp	eax,-1
	jz	.valuesend
;;;;;;;;
	add	esp,12


