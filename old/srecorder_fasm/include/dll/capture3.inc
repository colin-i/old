



;;the file out
;;;;playsound
	push	[SND_SYNC]
	push	NULL
	push	sound
	call	[PlaySound]
;;;;

	push	[SEEK_END]
	push	0
	push	0
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	mov	ecx,[bmih.biSizeImage]
	inc	ecx
	mov	[storeraw],ecx
	mov	[tfile_pointer_highs1],0
	mov	[tfile_pointer_lows1],ecx

	;;;for getting back and then write cues
	mov	[temp_pointer_lows],eax;low bits
	mov	[temp_pointer_highs],edx;high bits

	push	0
	push	ecx
	push	edx
	push	eax
	push	frames
	call	[lldiv]
	add	esp,20

	cmp	[totalframes],0
	jz	.noinfo
	mov	eax,[frames]
	mov	edx,[totalframes]     ;;for the main program
	mov	[edx],eax
	.noinfo:

;;the main counters and the temp file cursor with pointers
	mov	[framestotalcounter],1
	mov	[timemin],0
	mov	[timemax],31
	push	[SEEK_SET]
	push	[tfile_pointer_highs1]
	push	[tfile_pointer_lows1]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	mov	[flag],0;here at comp and uncomp

	cmp	[FOURCC_identifier],0
	jz	.unc
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;"frame" is required to be "on the frame", normally it is "totalizing the frames"
	dec	[frames]

include "capture9.inc"

	push	[ICMODE_COMPRESS]
	push	[FOURCC_identifier]
	push	[ic.fccType]
	call	[ICOpen]
	cmp	eax,0
	jnz	.notnull
	mov	[storevalue],5
	jmp	.end7
	.notnull:
	mov	[ic.hic],eax

	push	bmih.biSize
	push	ic.cbSize
	call	[ICSeqCompressFrameStart]
	cmp	eax,FALSE
	jnz	.compfurther
	mov	[storevalue],14
	jmp	.end8

	.compfurther:
	mov	eax,[ic.lpbiOut]
	mov	[stop_point4_default],eax

	push	0
	push	bmih.biSize
	push	[ICM_COMPRESS_GET_FORMAT]
	push	[ic.hic]
	call	[ICSendMessage]

	mov	[stop_point4_default_size],eax

	mov	edx,eax
	mov	ecx,eax
	call	size
	mov	[stop_point3_default_size],eax
	mov	[stop_point3_default],cx

	add	edx,51
	mov	ecx,edx
	call	size
	mov	[stop_point2_default_size],eax
	mov	[stop_point2_default],cx

	add	edx,2
	mov	ecx,edx
	call	size
	mov	[stop_point1_default_size],eax
	mov	[stop_point1_default],cx

;;restore the main counters and the temp file cursor
	inc	[frames]
	mov	[timemin],0
	mov	[timemax],31
	push	[SEEK_SET]
	push	[tfile_pointer_highs1]
	push	[tfile_pointer_lows1]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16
;;
	jmp	.capture4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	.unc:
	mov	ecx,[bmih.biSizeImage]
	add	ecx,4
	mov	edx,ecx
	call	size
	mov	[simpleblock_size],ecx
	mov	[simpleblock_sizesize],eax
	inc	edx
	add	edx,eax
	mov	[cl_block_size],edx

;;;;;;;;capture4
	mov	[stop_point1_default],(0x80 or 93)
	mov	[stop_point1_default_size],1
	mov	[stop_point2_default],(0x80 or 91)
	mov	[stop_point2_default_size],1
	mov	[stop_point3_default],(0x80 or 40)
	mov	[stop_point3_default_size],1
	mov	[stop_point4_default],bmih.biSize
	mov	[stop_point4_default_size],40
;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include "capture4.inc"





