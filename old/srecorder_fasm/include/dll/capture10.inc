









	mov	eax,[frames_per_interval]
	mov	[framesininterval],eax
	mov	eax,[tfile_pointer_highs2]
	mov	[pointerhighs],eax
	mov	eax,[tfile_pointer_lows2]
	mov	[pointerlows],eax
	mov	[framesizespointer],framesizes
	mov	[uframesizespointer],uframesizes
	mov	[frameszszpointer],frameszsz
	mov	[keyframespointer],keyframes
	mov	[intervalsize],0

	.framezone:
	push	[bmih.biSizeImage]
	push	[pvBits1]
	push	[temp_file_handler]
	call	[read]
	add	esp,12




	push	storesize
	push	storekey
	push	[pvBits1]
	push	NULL
	push	ic.cbSize
	call	[ICSeqCompressFrame]
	mov	[pointer],eax
;;
	mov	eax,[storekey]
	cmp	eax,0
	jz	.storekey
	mov	eax,0x80
	.storekey:
	mov	edx,[keyframespointer]
	mov	[edx],al
;;

	push	[SEEK_SET]
	push	[pointerhighs]
	push	[pointerlows]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	mov	ecx,[storesize]
	push	ecx

	mov	edx,[framesizespointer]
	mov	[edx],ecx
	add	ecx,4

	add	[intervalsize],ecx;;for cluster final size

	call	size
	add	[intervalsize],eax;;

	mov	edx,[uframesizespointer]
	mov	[edx],ecx

	mov	edx,[frameszszpointer]
	mov	[edx],al

	push	[pointer]
	push	[temp_file_handler]
	call	[write]
	add	esp,12

	cmp	[framesininterval],1
	jz	.calculations

	mov	eax,[storeraw]
	add	[pointerlows],eax
	jnc	.jumptonextcode
	inc	[pointerhighs]
	.jumptonextcode:

	push	[SEEK_SET]
	push	[pointerhighs]
	push	[pointerlows]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	dec	[framesininterval]
	add	[framesizespointer],4
	add	[uframesizespointer],4
	inc	[keyframespointer]
	inc	[frameszszpointer]
	jmp	.framezone

	.calculations:
	mov	[framesizespointer],framesizes
	mov	[uframesizespointer],uframesizes
	mov	[frameszszpointer],frameszsz
	mov	[keyframespointer],keyframes

	push	[SEEK_SET]
	push	[tfile_pointer_highs2]
	push	[tfile_pointer_lows2]
	push	[temp_file_handler]
	call	[seek]
	add	esp,16

	mov	eax,[tfile_pointer_highs2]
	mov	[pointerhighs],eax
	mov	eax,[tfile_pointer_lows2]
	mov	[pointerlows],eax

	mov	eax,[framesperinterval]
	add	eax,[intervalsize]

