

;;;;;;;content
	call	[GetTickCount]
	add	eax,1000
	mov	[nextsecond],eax

include "firstframe.inc"   ;the first frame isnt't compared with the previous




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;phase 1 start, search for a unique frame
	.phase1:
	push	PM_NOREMOVE
	push	0
	push	0
	push	0
	push	msg.hwnd
	call	[PeekMessage]
	cmp	[switch],0
	jz	.part2end
	;;;;;;;;;;;;;;;;;;;;;;;take a blit
	xor	[selectedbuffer],1
	mov	al,[selectedbuffer]

	cmp	al,0
	jnz	.sel2
	mov	ecx,[pvBits1]
	push	[hbm1]
	jmp	.selobj

	.sel2:
	mov	ecx,[pvBits2]
	push	[hbm2]

	.selobj:
	mov	[lastbuffer],ecx
	push	[destDC]
	call	[SelectObject]

	push	SRCCOPY
	push	[top]
	push	[left]
	push	[srcDC]
	push	[bottom]
	push	[right]
	push	0
	push	0
	push	[destDC]
	call	[BitBlt]
	;;;;;;;;;;;;;;;;;

	;;;;;;;;;;;;;;;;;
	;;compare
	mov	ecx,[pvBits1]
	mov	edx,[pvBits2]
	.compare:
	mov	eax,[ecx]
	cmp	eax,[edx]
	jnz	.timecmp
	cmp	ecx,[max_address]
	jz	.phase1;;;;;;;duplicate frame. go to search again
	add	ecx,4
	add	edx,4
	jmp	.compare
	;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;phase 1 end





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;phase 2 start, put the unique frame in the right second
	.timecmp:
	call	[GetTickCount]
	cmp	eax,[nextsecond]
	jb	.samesec

	.secgap:
	inc	[seconds]
	add	[nextsecond],1000
	mov	[framesinsec],1
	cmp	eax,[nextsecond]
	jae	.secgap
	jmp	.writeframe

	.samesec:
	inc	[framesinsec]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;phase 2 end





;;;;;;;;;;;;;;;;;;;;;;;;phase 3 start, write the frame to the temporary file
	.writeframe:
	push	1
	push	seconds
	push	[temp_file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.writedata
	mov	[storevalue],10
	jmp	.end6

	.writedata:
	push	[bmih.biSizeImage]
	push	[lastbuffer]
	push	[temp_file_handler]
	call	[write]
	add	esp,12
	cmp	eax,-1
	jnz	.phase4
	mov	[storevalue],10
	jmp	.end6
;;;;;;;;;;;;;;;;;;;;;;;;;;phase 3 end




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;phase 4 start, if frame per second is completed, sleep until next second
	.phase4:
	mov	al,[framesinsec]
	cmp	al,[fps]
	jnz	.phase1

	.sleep:
	push	1
	call	[Sleep]

	call	[GetTickCount]
	cmp	eax,[nextsecond]
	jae	.phase1

	push	PM_NOREMOVE
	push	0
	push	0
	push	0
	push	msg.hwnd
	call	[PeekMessage]
	cmp	[switch],0
	jnz	.sleep
;;;;;;;;;;;;;;;;;;;;;;;;phase 4 end


	.part2end:

