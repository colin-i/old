






	push	[SEEK_END]
	push	0
	push	0
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	push	4
	push	CuePoint
	push	[temp_file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.cues1
	mov	[storevalue],10
	jmp	.lastend

	.cues1:
	push	[cl_timecode_size]
	push	timecode_value
	push	[temp_file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.cues2
	mov	[storevalue],10
	jmp	.lastend

	.cues2:
	push	7
	push	CueTrackPositions
	push	[temp_file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.cues3
	mov	[storevalue],10
	jmp	.lastend

	.cues3:
	cmp	[cueshighssz],0
	jz	.lowspos

	push	[cueshighssz]
	push	cueclusterposition_value1
	push	[temp_file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.lowspos
	mov	[storevalue],10
	jmp	.lastend

	.lowspos:
	push	[cueslowssz]
	push	cueclusterposition_value2
	push	[temp_file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.cues4
	mov	[storevalue],10
	jmp	.lastend

	.cues4:
	mov	eax,[cue_size]
	add	[cues_size],eax
