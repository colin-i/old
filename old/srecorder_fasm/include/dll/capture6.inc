





	push	[SEEK_END]
	push	0
	push	0
	push	[file_handler]
	call	[seek]
	add	esp,16
	sub	eax,52
	jnc	.jnb
	dec	edx
	.jnb:

	mov	[flag],1





	mov	ecx,eax
	call	size
	mov	[cueclusterposition_value2],ecx
	mov	ecx,[cueslowssz]

	cmp	al,cl
	jz	.seehighs




	.increment:
	inc	[cueslowssz]
	.increment2:
	inc	[cue_size]
	inc	[cuepoint_size]
	inc	[cuetrackpositions_size]
	inc	[cueclusterposition_size]
	inc	cl
	cmp	al,cl
	jnz	.increment





	.seehighs:
	cmp	edx,0
	jz	.preparetime

	mov	ecx,edx
	call	size
	mov	[cueclusterposition_value1],ecx
	mov	ecx,[cueshighssz]
	cmp	al,cl
	jz	.preparetime
	inc	[cueshighssz]
	mov	edx,0
	jmp	.increment2



	.preparetime:
	mov	ecx,[cl_timecode]
	call	size
	mov	[timecode_value],ecx;for here and cues

	.timecomparisson:
	cmp	[cl_timecode_size],eax
	jz	.clstart

	inc	[cl_timecode_size]
	inc	[timecode_size];0x81 to 0x82..
	inc	[timecode_cues];;;need at cues
	inc	[cuepoint_size];;;
	inc	[cue_size]     ;;;
	jmp	.timecomparisson




	.clstart:
	push	[SEEK_SET]
	push	[tfile_pointer_highs2]
	push	[tfile_pointer_lows2]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	push	4
	push	Cluster
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.clcontinue1
	jmp	.tolastend

	.clcontinue1:		      ;!!!stop point
	cmp	[FOURCC_identifier],0
	jz	.tounc

	mov	[flag],0
include "capture10.inc"
	jmp	.datawrite

;;;;;;;;;;;;;;;;;;;;;;;uncompressed
	.tounc:
	mov	edx,[bmih.biSizeImage]
	mov	[datawritesize],edx
	mov	eax,[cl_block_size]
	mul	[framesperinterval]
;;;;;;;;;;;;;;;;;;;;;;;

	.datawrite:
	add	eax,[cl_timecode_size]
	add	eax,2
	mov	ecx,eax
	mov	[flag],0
	call	size
	mov	[buffer2],eax

	cmp	eax,5
	jnz	 .lowdirect

	dec	[buffer2]
	push	1
	push	cluster_size1
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.lowdirect
	jmp	.tolastend

	.lowdirect:
	push	[buffer2]
	push	buffer1
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.clcontinue2
	jmp	.tolastend

	.clcontinue2:
	push	1
	push	Timecode
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.clcontinue3
	jmp	.tolastend

	.clcontinue3:
	push	1
	push	timecode_size
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.clcontinue4
	jmp	.tolastend

	.clcontinue4:
	push	[cl_timecode_size]
	push	timecode_value
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.clcontinue5
	jmp	.tolastend

	.clcontinue5:
	mov	[ltcursor],localtimes

	.loopzone:
	cmp	[FOURCC_identifier],0
	jz	.readwriteblock

	mov	eax,[framesizespointer]
	mov	edx,[eax]
	mov	[datawritesize],edx
	mov	eax,[uframesizespointer]
	mov	edx,[eax]
	mov	[simpleblock_size],edx
	xor	edx,edx
	mov	eax,[frameszszpointer]
	mov	dl,[eax]
	mov	[simpleblock_sizesize],edx
	mov	eax,[keyframespointer]
	mov	dl,[eax]
	mov	[flags],dl
	.readwriteblock:

	push	1
	push	SimpleBlock
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.loopzone1
	jmp	.tolastend

	.loopzone1:
	push	[simpleblock_sizesize]
	push	simpleblock_size
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.loopzone2
	jmp	.tolastend

	.loopzone2:
	push	1
	push	Track_Number
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.loopzone3
	jmp	.tolastend

	.loopzone3:
	mov	eax,[ltcursor]
	mov	ecx,[eax]
	mov	[buffer1],ecx

	push	2
	push	buffer1
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.loopzone4
	jmp	.tolastend

	.loopzone4:
	push	1
	push	flags
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.loopzone5
	jmp	.tolastend

	.loopzone5:
	push	[datawritesize]
	push	[pvBits1]
	push	[temp_file_handler]
	call	[read]
	add	esp,12

	push	[datawritesize]
	push	[pvBits1]
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.loopzone6
	jmp	.tolastend

	.loopzone6:
	cmp	[totalframes],0
	jz	.noevents
	push	[exeevent]
	call	[SetEvent]
	push	[INFINITE]
	push	[dllevent]
	call	[WaitForSingleObject]
	push	[dllevent]
	call	[ResetEvent]
	.noevents:

	cmp	[framesperinterval],1
	jz	.loopzoneend

	cmp	[FOURCC_identifier],0
	jz	.gounc

	add	[framesizespointer],4
	add	[uframesizespointer],4
	inc	[frameszszpointer]
	inc	[keyframespointer]

	mov	eax,[storeraw]
	add	[pointerlows],eax
	jnc	.jumpnextcode
	inc	[pointerhighs]
	.jumpnextcode:
	push	[SEEK_SET]
	push	[pointerhighs]
	push	[pointerlows]
	push	[temp_file_handler]
	jmp	.corezone

	.gounc:
	push	[SEEK_CUR]
	push	0
	push	1
	push	[temp_file_handler]

	.corezone:
	call	[seek]
	add	esp,16
	dec	[framesperinterval]
	add	[ltcursor],2
	jmp	.loopzone

	.loopzoneend:

include 'capture7.inc'



