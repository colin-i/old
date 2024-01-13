



;;;;;write the headers
	.capture4:

;;values for SegmentUID_value
	rdtsc
	mov	ecx,SegmentUID_value

	mov	[ecx],eax

	add	ecx,4
	not	eax
	mov	[ecx],eax

	add	ecx,4
	ror	eax,12
	mov	[ecx],eax

	add	ecx,4
	not	eax
	mov	[ecx],eax
;;
;;;;;;time field
	push	_stat64.st_dev
	push	[file_out]
	call	[stat]
	add	esp,8

	mov	ecx,_stat64.st_ctime
	mov	eax,[ecx]
	sub	eax,978307200;this is the difference in seconds between 1 jan 2001 and 1 jan 1970
	mov	[dwordvalue],eax
	jnc	.jnc ;decrementing the high bits if substraction is negative
	mov	[substractionloss],1
	.jnc:

;;the value need to be multiplied by 1 000 000 000
;;here is ABxCD (where first letter is the high bits of the double word, second is the low bits),considering quad words: AC<<32+AD<<16+BC<<16+BD

;;prepare the multiplicators
	mov	edx,[multiplicator]
	mov	[mul1],edx
	mov	[mul2],edx
	and	[mul1],0xFFFF
	and	[mul2],0xFFFF0000
	ror	[mul2],16
;;

;;BD
	and	eax,0xFFFF

	mul	[mul1]

	add	[date_value_lows],eax

;;BC<<16
	mov	eax,[dwordvalue]
	and	eax,0xFFFF

	mul	[mul2]

	rol	eax,16

	cmp	ax,0xFFFF
	mov	edx,eax
	jz	.lowsadd
	and	eax,0xFFFF
	add	[date_value_highs],eax

	.lowsadd:
	and	edx,0xFFFF0000
	add	[date_value_lows],edx

;;AD<<16
	mov	eax,[dwordvalue]
	rol	eax,16
	and	eax,0xFFFF

	mul	[mul1]

	ror	eax,16

	cmp	ax,0xFFFF
	mov	edx,eax
	jz	.lowsadd2
	and	eax,0xFFFF
	add	[date_value_highs],eax

	.lowsadd2:
	and	edx,0xFFFF0000
	add	[date_value_lows],edx

;;AC<<32
	mov	eax,[dwordvalue]
	ror	eax,16
	and	eax,0xFFFF

	mul	[mul2]

	add	[date_value_highs],eax


;;high quad word
	add	ecx,4
	mov	eax,[ecx]
	sub	eax,[substractionloss]
	mul	[multiplicator]
	add	[date_value_highs],eax
;;;;
;;
	mov	eax,[date_value_highs]
	bswap	eax
	mov	[date_value_highs],eax
	mov	eax,[date_value_lows]
	bswap	eax
	mov	[date_value_lows],eax
;;
;;video track width and height
	mov	eax,[bmih.biWidth]
	mov	dl,ah
	mov	ah,al
	mov	al,dl
	mov	[pixelwidth_size],ax

	mov	eax,[bmih.biHeight]
	mov	dl,ah
	mov	ah,al
	mov	al,dl
	mov	[pixelheight_size],ax
;;
;;;write until stop point
	push	203
	push	EBML
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.continue1
	jmp	.tolastend
;!!!stop point reached

	.continue1:
	push	[stop_point1_default_size]
	push	stop_point1_default
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.continue2
	jmp	.tolastend


	.continue2:
	push	1
	push	TrackEntry
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.continue3
	jmp	.tolastend

	.continue3:
	push	[stop_point2_default_size]
	push	stop_point2_default
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.continue4
	jmp	.tolastend

	.continue4:
	push	40
	push	TrackNumber
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.continue5
	jmp	.tolastend

	.continue5:
	push	[stop_point3_default_size]
	push	stop_point3_default
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.continue6
	jmp	.tolastend

	.continue6:
	push	[stop_point4_default_size]
	push	[stop_point4_default]
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.continue7
	jmp	.tolastend

	.continue7:
	push	10
	push	VideoTrack
	push	[file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.capture5
	jmp	.tolastend

include "capture5.inc"


